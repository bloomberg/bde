// bael_log.t.cpp         -*-C++-*-
#include <bael_log.h>

#include <bael_administration.h>
#include <bael_defaultattributecontainer.h>
#include <bael_attributecontainer.h>  // for testing
#include <bael_attributecontext.h>
#include <bael_attribute.h>           // for testing
#include <bael_defaultobserver.h>
#include <bael_categorymanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <bael_record.h>
#include <bael_testobserver.h>

#include <bcema_blob.h>
#include <bcema_testallocator.h>
#include <bcemt_threadutil.h>
#include <bces_atomictypes.h>

#include <bdem_list.h>

#include <bdef_bind.h>
#include <bdef_function.h>

#include <bdesb_memoutstreambuf.h>

#include <bdetu_datetime.h>

#include <bdet_datetime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>
#include <bsls_platformutil.h>
#include <bsls_timeutil.h>

#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>    // strlen(), strcmp(), memset(), memcpy(), memcmp()

#include <bsl_iostream.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM__OS_UNIX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

#include <bsl_c_stdio.h>

// Warning: the following 'using' declarations interfere with the testing of
// the macros defined in this component.  Please do not uncomment them.
// using namespace BloombergLP;
// using namespace bsl;
//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a large number of preprocessor macros
// and a small number of 'static' utility functions.
//
// The utility functions are merely facades for methods defined in the
// 'bael_loggermanager' component.  It is sufficient to test that each "facade"
// method correctly forwards its arguments to the corresponding method of
// 'bael_loggermanager', and that the correct value is returned.
//
// The preprocessor macros are largely implemented in terms of the utility
// functions.  Each macro is individually tested to ensure that the macro's
// arguments are correctly forwarded and that the side-effects of the macro
// match the expected behavior.
//-----------------------------------------------------------------------------
// [ 1] static char *s_cachedMessageBuffer;
// [ 1] static int s_cachedMessageBufferSize;
// [ 1] static char *messageBuffer();
// [ 1] static int messageBufferSize();
// [ 1] static void logMessage(*category, severity, *file, line, *msg);
// [ 1] static const bael_Category *setCategory(const char *categoryName);
//-----------------------------------------------------------------------------
// [ 2] BAEL_LOG_SET_CATEGORY
// [ 2] BAEL_LOG_CATEGORY
// [ 2] BAEL_LOG_THRESHOLD
// [ 3] PRINTF-STYLE MACROS
// [ 4] OSTRSTREAM MACROS
// [ 5] TESTING MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
// [ 6] TESTING THE C++ MACRO WHEN LOGGING RETURNED VALUE OF A FUNCTION
// [ 7] TESTING THE DEFAULT LOG ORDER (LIFO)
// [ 8] TESTING THE FIFO LOG ORDER
// [ 9] CONCURRENT LOGGING TEST
// [10] C++ MACRO PERFORMANCE TEST WITH MULTIPLE THREADS
// [11] C++ MACRO PERFORMANCE TEST WITH 1 THREAD
// [12] PRINTF MACRO PERFORMANCE TEST WITH MULTIPLE THREADS
// [13] PRINTF MACRO PERFORMANCE TEST WITH 1 THREAD
// [14] BAEL_IS_ENABLED(SEVERITY) UTILITY MACRO
// [15] STRESS TEST
// [16] TESTING OSTRSTREAM MACROS WITH CALLBACK
// [17] TESTING CALLBACK MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
// [18] BAEL_LOG_SET_DYNAMIC_CATEGORY
// [19] BAEL_LOG_STREAM
// [20] bool isCategoryEnabled(Holder *holder, int severity);
// [21] BAEL_LOG_SET_CATEGORY and BAEL_LOG_TRACE WITH MULTIPLE THREADS
// [22] BAEL_LOG_SET_DYNAMIC_CATEGORY and BAEL_LOG_TRACE WITH MULTIPLE THREADS
// [23] BAEL_LOG_SET_CATEGORY and BAEL_LOG2 WITH MULTIPLE THREADS
// [24] BAEL_LOG_SET_DYNAMIC_CATEGORY and BAEL_LOG2 WITH MULTIPLE THREADS
// [25] RULE BASED LOGGING: bool isCategoryEnabled(Holder *, int);
// [26] RULE BASED LOGGING: void logMessage(const bael_Category *,
//                                          int,
//                                          bael_Record *);
// [27] BAEL_LOG_IS_ENABLED(SEVERITY)
//-----------------------------------------------------------------------------
// [28] USAGE EXAMPLE
// [29] RULE-BASED LOGGING USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)"       << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); \
             } }

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\n"; \
              aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print identifier
                                                        // and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote identifier
                                                        // literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", "<<bsl::flush; // P(X) without
                                                                // '\n'
#define L_ __LINE__                           // current line number
#define T_() bsl::cout << "\t" << flush;      // Print tab w/o newline.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef BloombergLP::bael_Log                Obj;
typedef BloombergLP::bael_Severity           Sev;
typedef BloombergLP::bael_Category           Cat;
typedef BloombergLP::bael_CategoryHolder     Holder;
typedef BloombergLP::bael_Log_Stream         LogStream;
typedef BloombergLP::bslma_TestAllocator     TestAllocator;
typedef BloombergLP::bael_CategoryManager    CategoryManager;
typedef BloombergLP::bael_ThresholdAggregate Thresholds;

const int TRACE = Sev::BAEL_TRACE;
const int DEBUG = Sev::BAEL_DEBUG;
const int INFO  = Sev::BAEL_INFO;
const int WARN  = Sev::BAEL_WARN;
const int ERROR = Sev::BAEL_ERROR;
const int FATAL = Sev::BAEL_FATAL;
const int OFF   = Sev::BAEL_OFF;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace BloombergLP {

int verbose;
int veryVerbose;
int veryVeryVerbose;

///Example 5: Rule Based Logging
///- - - - - - - - - - - - - - -
// The following example demonstrates using rules and attributes to
// conditionally enable logging particular messages.
//
// We start by defining a function, 'processData', that is passed a data
// blob and information about the user who sent the data.  This example
// function performs no actual processing, but does log a single message at
// the 'bael_Severity::DEBUG' threshold level.  The 'processData' function also
// adds the user information passed to this function to the thread's attribute
// context.  We will use these attributes later, to create a logging rule that
// enables verbose logging only for a particular user.
//..
  void processData(int               uuid,
                   int               luw,
                   int               terminalNumber,
                   const bcema_Blob& data)
      // Process the specified 'data' associated with the specified bloomberg
      // 'uuid', 'luw', and 'terminalNumber'.
  {
//..
// We add our attributes to the generic "default" attribute container.  In
// practice we could create a more efficient attribute container
// implementation specifically for these three attributes (uuid, luw, and
// terminalNumber).  See the 'bael_attributeContainer' component documentation
// for an example.
//..
      bael_DefaultAttributeContainer attributes;
      attributes.addAttribute(bael_Attribute("uuid", uuid));
      attributes.addAttribute(bael_Attribute("luw", luw));
      attributes.addAttribute(bael_Attribute("terminalNumber",
                                             terminalNumber));
      bael_AttributeContext *context = bael_AttributeContext::getContext();
      bael_AttributeContext::iterator it =
                                         context->addAttributes(&attributes);
//..
// In this simplified example we perform no actual processing, and simply log
// a message at the 'bael_Severity::DEBUG' level.
//..
      BAEL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");

      BAEL_LOG_DEBUG << "An example message" << BAEL_LOG_END;
//..
// Because 'attributes' is defined on this thread's stack, it must be removed
// from this thread's attribute context before exiting the function.
//..
      context->removeAttributes(it);
  }
//..

} // close namespace BloombergLP

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void executeInParallel(
                      int                                           numThreads,
                      BloombergLP::bcemt_ThreadUtil::ThreadFunction func)
   // Create the specified 'numThreads', each executing the specified 'func'.
   // Number each thread (sequentially from 0 to 'numThreads-1') by passing 'i'
   // to i'th thread.  Finally join all the threads.
{
    using namespace BloombergLP;
    bcemt_ThreadUtil::Handle *threads =
                                      new bcemt_ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::create(&threads[i], func, (void*)i);
    }
    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

const int FILL = 0xbb;

inline
char *messageBuffer()
{
    BloombergLP::bcemt_Mutex *mutex = 0;
    int bufferSize = 0;
    char *buffer = Obj::obtainMessageBuffer(&mutex, &bufferSize);
    Obj::releaseMessageBuffer(mutex);
    return buffer;
}

inline
int messageBufferSize()
{
    BloombergLP::bcemt_Mutex *mutex = 0;
    int bufferSize = 0;
    Obj::obtainMessageBuffer(&mutex, &bufferSize);
    Obj::releaseMessageBuffer(mutex);
    return bufferSize;
}

inline
static void scribbleBuffer()
    // Assign 'FILL' to each of the 'messageBufferSize' bytes starting at the
    // address returned by 'messageBuffer'.
{
    bsl::memset(messageBuffer(), FILL, messageBufferSize());
}

static int isBufferScribbled()
    // Return 1 if no portion of the buffer returned by 'messageBuffer' has
    // been overwritten since 'scribbleBuffer' was last called, and 0
    // otherwise.
{
    const char *p   = messageBuffer();
    const char *end = p + messageBufferSize();

    while (p < end) {
        if (*p++ != (char)FILL) {
            return 0;
        }
    }
    return 1;
}

static int isRecordOkay(const BloombergLP::bael_TestObserver&  observer,
                        const BloombergLP::bael_Category      *category,
                        int                                    severity,
                        const char                            *fileName,
                        int                                    lineNumber,
                        const char                            *message)
    // Return 1 if the last record published to the specified 'observer'
    // includes the name of the specified 'category' and the specified
    // 'severity', 'fileName', 'lineNumber', and 'message', and 0 otherwise.
{
    const BloombergLP::bael_RecordAttributes& attributes =
                                  observer.lastPublishedRecord().fixedFields();

    int status = 0    == bsl::strcmp(category->categoryName(),
                                     attributes.category())
        && severity   == attributes.severity()
        && 0          == bsl::strcmp(fileName, attributes.fileName())
        && lineNumber == attributes.lineNumber()
        && 0          == bsl::strcmp(message, attributes.message().data());

    if (!status) {
        P_(category->categoryName());  P(attributes.category());
        P_(severity);                  P(attributes.severity());
        P_(fileName);                  P(attributes.fileName());
        P_(lineNumber);                P(attributes.lineNumber());
        P_(message);                   P(attributes.message());
    }

    return status;
}

static int numIncCallback = 0;
void incCallback(BloombergLP::bdem_List *list) {
    ASSERT(list);
    ++numIncCallback;
    return;
}

class Point {
    int d_x;
    int d_y;
    bsl::string d_name;
  public:
    Point() : d_x(0), d_y(0), d_name("") {}
    int x() const { return d_x; };
    int y() const { return d_y; };
    const bsl::string& name() const { return d_name; };
};

void populateUsingPoint(BloombergLP::bdem_List *list, const Point& point)
{
    list->appendString(point.name());
    list->appendInt(point.x());
    list->appendInt(point.y());
}

//=============================================================================
//                         CASE 24 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_24 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
double      arg1          = 0.0;
const char *arg2          = 0;
int         numIterations = 0;

extern "C" {
void *workerThread24(void *arg)
{
    BAEL_LOG_SET_DYNAMIC_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BAEL_LOG2(bael_Severity::BAEL_TRACE, msg, arg1, arg2);
        BAEL_LOG2(bael_Severity::BAEL_DEBUG, msg, arg1, arg2);
        BAEL_LOG2(bael_Severity::BAEL_INFO, msg, arg1, arg2);
//         BAEL_LOG_WARN   << msg << BAEL_LOG_END;
//         BAEL_LOG_ERROR  << msg << BAEL_LOG_END;
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_24

//=============================================================================
//                         CASE 23 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_23 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
double      arg1          = 0.0;
const char *arg2          = 0;
int         numIterations = 0;

extern "C" {
void *workerThread23(void *arg)
{
    BAEL_LOG_SET_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BAEL_LOG2(bael_Severity::BAEL_TRACE, msg, arg1, arg2);
        BAEL_LOG2(bael_Severity::BAEL_DEBUG, msg, arg1, arg2);
        BAEL_LOG2(bael_Severity::BAEL_INFO, msg, arg1, arg2);
//         BAEL_LOG_WARN   << msg << BAEL_LOG_END;
//         BAEL_LOG_ERROR  << msg << BAEL_LOG_END;
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_23

//=============================================================================
//                         CASE 22 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_22 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
int         numIterations = 0;

extern "C" {
void *workerThread22(void *arg)
{
    BAEL_LOG_SET_DYNAMIC_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BAEL_LOG_TRACE  << msg << BAEL_LOG_END;
        BAEL_LOG_DEBUG  << msg << BAEL_LOG_END;
        BAEL_LOG_INFO   << msg << BAEL_LOG_END;
//         BAEL_LOG_WARN   << msg << BAEL_LOG_END;
//         BAEL_LOG_ERROR  << msg << BAEL_LOG_END;
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_22

//=============================================================================
//                         CASE 21 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_21 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
int         numIterations = 0;

extern "C" {
void *workerThread21(void *arg)
{
    BAEL_LOG_SET_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BAEL_LOG_TRACE  << msg << BAEL_LOG_END;
        BAEL_LOG_DEBUG  << msg << BAEL_LOG_END;
        BAEL_LOG_INFO   << msg << BAEL_LOG_END;
//         BAEL_LOG_WARN   << msg << BAEL_LOG_END;
//         BAEL_LOG_ERROR  << msg << BAEL_LOG_END;
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_21

//=============================================================================
//                         CASE 18 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_18 {

enum {
    NUM_THREADS  = 4,
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bcemt_Mutex categoryMutex;

extern "C" {
void *workerThread18(void *arg)
{
    BAEL_LOG_SET_DYNAMIC_CATEGORY("main category");
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        if (i % 2) {
            BAEL_LOG_WARN << msg << BAEL_LOG_END;
        }
        else {
            BAEL_LOG_INFO << msg << BAEL_LOG_END;
        }
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_18
//=============================================================================
//                         CASE 15 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_15 {

class my_PublishCountingObserver : public BloombergLP::bael_Observer {
    // This concrete implementation of 'bael_Observer' maintains a count
    // of the number of messages published to it and gives access to that
    // count through 'publishCount'.
    int d_publishCount;
  public:
    // CONSTRUCTORS
    my_PublishCountingObserver() : d_publishCount(0)
    {
    }

    ~my_PublishCountingObserver()
    {
    }

    //MANIPULATORS
    void publish(const BloombergLP::bael_Record&  record,
                 const BloombergLP::bael_Context& context)
    {
        ++d_publishCount;
    }

    //ACCESSORS
    int publishCount() const
    {
        return d_publishCount;
    }
};

}  // close namespace BAEL_LOG_TEST_CASE_15
//=============================================================================
//                         CASE 13 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_13 {

enum {
    NUM_THREADS  = 1,      // don't change
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bcemt_Mutex categoryMutex;

extern "C" {
void *workerThread13(void *arg)
{
    categoryMutex.lock();
    BAEL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        BAEL_LOG1_INFO("%s", msg);
    }

    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_13
//=============================================================================
//                         CASE 12 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_12 {

enum {
    NUM_THREADS  = 4,
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bcemt_Mutex categoryMutex;
extern "C" {
void *workerThread12(void *arg)
{
    categoryMutex.lock();
    BAEL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        BAEL_LOG1_INFO("%s", msg);
    }

    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_12
//=============================================================================
//                         CASE 11 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_11 {

enum {
    NUM_THREADS  = 1,
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bcemt_Mutex categoryMutex;
extern "C" {
void *workerThread11(void *arg)
{
    categoryMutex.lock();
    BAEL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        BAEL_LOG_INFO << msg << BAEL_LOG_END;
    }

    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_11
//=============================================================================
//                         CASE 10 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_10 {

enum {
    NUM_THREADS  = 4,
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bcemt_Mutex categoryMutex;
extern "C" {
void *workerThread10(void *arg)
{
    categoryMutex.lock();
    BAEL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();

    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        BAEL_LOG_INFO << msg << BAEL_LOG_END;
    }

    return NULL;
}
}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_10
//=============================================================================
//                         CASE 9 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_9 {

enum {
    NUM_THREADS = 4,       // number of threads

    N_TOTAL     = 1000,    // total number of message logged by each thread

    N_PUBLISH   = 10,      // frequency of messages that will cause
                           // a *publish* event (see the documentation for
                           // 'workerThread9')

    N_TRIGGER   = 100,     // frequency of messages that will cause
                           // a *trigger* event (see the documentation for
                           // 'workerThread9')

    // number of messages published to the observer, that were logged with
    // a severity causing a *Record* event.  Note that we need to do
    // multiplication by 2, because such messages are published twice
    // to the observer.  Also the value is per thread.
    EXP_N_RECORD  = N_TOTAL - N_TOTAL/N_PUBLISH,

    // number of messages published to the observer, that were logged with
    // a severity causing a *Publish* event.  Note that we need to do
    // multiplication by 2, because such messages are published twice
    // to the observer.  Also the value is per thread.
    EXP_N_PUBLISH = 2 * (N_TOTAL/N_PUBLISH - N_TOTAL/N_TRIGGER),

    // number of messages published to the observer, that were logged with
    // a severity causing a *Trigger* event.  Note that we need to do
    // multiplication by 2, because such messages are published twice to
    // the observer.  Also the value is per thread.
    EXP_N_TRIGGER = 2 * N_TOTAL/N_TRIGGER,

    // total number of messages published to the observer by one thread
    EXP_N_TOTAL   = N_TOTAL + N_TOTAL/N_PUBLISH,
    // note that EXP_N_TOTAL = EXP_N_RECORD + EXP_N_PUBLISH + EXP_N_TRIGGER'

    // fixed record buffer limit.  The limit is big enough to accommodate
    // all the records logged to the buffer.  400 accounts *roughly* for
    // dynamic memory allocated for each record and memory required by
    // buffer to accommodate the record handles.
    REC_BUF_LIMIT = NUM_THREADS * EXP_N_TOTAL *
                                 (sizeof (BloombergLP::bael_Record) + 400)
};

enum { RECORD, PUBLISH, TRIGGER };
class my_Observer : public BloombergLP::bael_Observer {
    // This thread-safe, concrete implementation of 'bael_Observer' stores
    // all the messages published to it in a vector and provide access to
    // the vector by 'get' method.

    // DATA
    bsl::vector<bsl::string> d_publishedMessages;
    BloombergLP::bcemt_Mutex d_mutex;

  public:
    // CONSTRUCTORS
    my_Observer()  {}
    ~my_Observer() {}

    //MANIPULATORS
    void publish(const BloombergLP::bael_Record&  record,
                                     const BloombergLP::bael_Context& context)
    {
        d_mutex.lock();
        d_publishedMessages.push_back(record.fixedFields().message());
        d_mutex.unlock();
    }

    //ACCESSORS
    const bsl::vector<bsl::string>& get() const
    {
        return d_publishedMessages;
    }

    void print() const
    {
        for (int i = 0; i < (signed) d_publishedMessages.size(); ++i) {
            bsl::cout << d_publishedMessages[i] << bsl::endl;
        }
    }
};

BloombergLP::bcemt_Mutex categoryMutex;

extern "C" {
void *workerThread9(void *arg)
{
    // Log 'N_TOTAL' messages using a loop as following.
    // (a) Every 'N_TRIGGER'th message is logged with a severity that will
    //     cause a *Trigger* event.
    // (b) Every 'N_PUBLISH'th message except those described in (a), is
    //     logged with a severity that will cause a *Publish* event.
    // (c) rest of the message are logged with a severity that will cause
    //     a *Record* event.
    // Each logged message contains following information.
    // - the loop iteration number
    // - thread id
    // - the event it will cause

    using namespace BloombergLP;
    int id = (int)(bsls_PlatformUtil::IntPtr)arg;

    categoryMutex.lock();
    BAEL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();

    for (int i = 0; i < N_TOTAL; ++i) {
        if ((i + 1)%N_TRIGGER == 0) {
            BAEL_LOG_ERROR << id      << " "   // thread id
                           << TRIGGER << " "   // event type
                           << i       << " "   // iteration
                           << BAEL_LOG_END;
        }
        else if ((i + 1)%N_PUBLISH == 0) {
            BAEL_LOG_WARN  << id      << " "   // thread id
                           << PUBLISH << " "   // event type
                           << i       << " "   // iteration
                           << BAEL_LOG_END;
        }
        else {
            BAEL_LOG_TRACE << id      << " "   // thread id
                           << RECORD << " "    // event type
                           << i       << " "   // iteration
                           << BAEL_LOG_END;
        }
    }
    return NULL;
}

}  // extern "C"

}  // close namespace BAEL_LOG_TEST_CASE_9

//=============================================================================
//                         CASE 6 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_LOG_TEST_CASE_6 {

const char *message1 = "MESSAGE-1";
const char *message2 = "MESSAGE-2";

const char *f()
{
    BAEL_LOG_SET_CATEGORY("main category");
    BAEL_LOG_WARN << message1 << BAEL_LOG_END;
    return message2;
}

}  // close namespace BAEL_LOG_TEST_CASE_6

//=============================================================================
//                         CASE -1 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BAEL_LOG_TEST_CASE_MINUS_1 {

using namespace BloombergLP;

struct ThreadFunctor {
    void operator()() {
        BAEL_LOG_SET_CATEGORY( "CATEGORY_5" );

        bsls_PlatformUtil::Int64 id =
                         (bsls_PlatformUtil::Int64) bcemt_ThreadUtil::selfId();

        while( true )
        {
            BAEL_LOG_ERROR << "ERROR " << id << BAEL_LOG_END;
        }
    }
};

}  // close namespace BAEL_LOG_TEST_CASE_MINUS_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

using namespace BloombergLP;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;  // not used

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    BloombergLP::bael_TestObserver  testObserver(bsl::cout);
    BloombergLP::bael_TestObserver *TO = &testObserver;

    TestAllocator ta(veryVeryVerbose); const TestAllocator& TA = ta;

    switch (test) { case 0:  // Zero is always the leading case.
      case 29: {
        //---------------------------------------------------------------------
        // TESTING RULE BASED LOGGING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //---------------------------------------------------------------------
        using namespace BloombergLP;

        if (verbose) bsl::cout << bsl::endl
                               << "Testing Rule-Based Logging Usage Example\n"
                               << "========================================\n";

//..
// Next we demonstrate how to create a logging rule that sets the passthrough
// logging threshold to 'bael_Severity::TRACE' (i.e., enables more verbose
// logging) for a particular user when calling the 'processData' function
// above.
//
// We start by creating the singleton logger manager which we configure with
// the default observer and default configuration.  We then call the
// 'processData' function.  This first call to 'processData' will not result in
// any logged messages because the 'processData' function logs its message at
// the 'bael_Severity::DEBUG' level, which is below the default configured
// logging threshold.
//..
    bael_DefaultObserver observer(bsl::cout);
    bael_LoggerManagerConfiguration lmc;
    bael_LoggerManagerScopedGuard lmg(&observer, lmc);

    BAEL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");

    bcema_Blob message;

    BAEL_LOG_ERROR << "Processing the first message." << BAEL_LOG_END;
    processData(3938908, 2, 9001, message);
//..
// Now we add a logging rule to set the passthrough threshold to be
// 'bael_Severity::TRACE' (i.e., enable more verbose logging) if the thread's
// context contains a "uuid" of '3938908'.  Note that the category for the
// 'bael_Rule' is the wild-card value '*' so that the rule will apply to all
// categories.
//..
    bael_Rule rule("*", 0, bael_Severity::BAEL_TRACE, 0, 0);
    rule.addPredicate(bael_Predicate("uuid", 3938908));
    bael_LoggerManager::singleton().addRule(rule);

    BAEL_LOG_ERROR << "Processing the second message." << BAEL_LOG_END;
    processData(3938908, 2, 9001, message);

    BAEL_LOG_ERROR << "Processing the third message." << BAEL_LOG_END;
    processData(2171395, 2, 9001, message);
//..
// The final call to the 'processData' function above, passes a "uuid" of
// '2171395' (not '3938908') so the default logging threshold will apply and no
// message will be logged.
//
// The resulting logged output for this example looks like the following:
//..
// ERROR example.cpp:105 EXAMPLE.CATEGORY Processing the first message.
// ERROR example.cpp:117 EXAMPLE.CATEGORY Processing the second message.
// DEBUG example.cpp:35 EXAMPLE.CATEGORY An example message
// ERROR example.cpp:129 EXAMPLE.CATEGORY Processing the third message.
//..
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl << "Testing Usage Example"
                               << bsl::endl << "====================="
                               << bsl::endl;

        if (verbose) bsl::cout << "Initialize logger manager" << bsl::endl;

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc);

        BloombergLP::bael_Administration::addCategory(
                                      "EQUITY.NASD",
                                      BloombergLP::bael_Severity::BAEL_TRACE,
                                      BloombergLP::bael_Severity::BAEL_WARN,
                                      BloombergLP::bael_Severity::BAEL_ERROR,
                                      BloombergLP::bael_Severity::BAEL_FATAL);

        if (verbose) bsl::cout << "ostrstream-based macro usage" << bsl::endl;
        {
            int         lotSize = 400;
            const char *ticker  = "SUNW";
            double      price   = 5.65;

            BloombergLP::bdet_Datetime now;
            BloombergLP::bdetu_Datetime::convertFromTimeT(&now, time(0));

            BAEL_LOG_SET_CATEGORY("EQUITY.NASD")
            // Logging with category "EQUITY.NASD" from here on.
            BAEL_LOG_TRACE << "[1] " << now << ": " << lotSize << " shares of "
                           << ticker << " sold at " << price << BAEL_LOG_END
            if (veryVerbose) P(messageBuffer());
            {
                BAEL_LOG_SET_CATEGORY("EQUITY.NASD.SUNW")
                // Now logging with category "EQUITY.NASD.SUNW".
                BAEL_LOG_TRACE << "[2] " << now << ": " << lotSize
                               << " shares of " << ticker << " sold at "
                               << price << BAEL_LOG_END
                if (veryVerbose) P(messageBuffer());
            }
            // Now logging with category "EQUITY.NASD" again.
            BAEL_LOG_TRACE << "[3] " << now << ": " << lotSize << " shares of "
                           << ticker << " sold at " << price << BAEL_LOG_END
            if (veryVerbose) P(messageBuffer());
        }

        if (verbose) bsl::cout << "printf-style macro usage" << bsl::endl;
        {
            int         lotSize = 400;
            const char *ticker  = "SUNW";
            double      price   = 5.65;

            BAEL_LOG_SET_CATEGORY("EQUITY.NASD")
            // Logging with category "EQUITY.NASD" from here on.
            BAEL_LOG3_INFO("[4] %d shares of %s sold at %f\n",
                           lotSize, ticker, price);
            if (veryVerbose) P(messageBuffer());
            {
                BAEL_LOG_SET_CATEGORY("EQUITY.NASD.SUNW")
                // Now logging with category "EQUITY.NASD.SUNW".
                BAEL_LOG3_INFO("[5] %d shares of %s sold at %f\n",
                               lotSize, ticker, price);
                if (veryVerbose) P(messageBuffer());
            }
            // Now logging with category "EQUITY.NASD" again.
            BAEL_LOG3_INFO("[6] %d shares of %s sold at %f\n",
                           lotSize, ticker, price);
            if (veryVerbose) P(messageBuffer());
        }

        // Note that the following Usage example was moved from the .h file
        // to the .cpp file so as not to encourage direct use of the utility
        // functions.

        if (verbose) bsl::cout << "Utility usage" << bsl::endl;
        {
            static const BloombergLP::bael_Category *category =
                        BloombergLP::bael_Log::setCategory("EQUITY.NASD.SUNW");
            {
                using BloombergLP::bael_Log;
                using BloombergLP::bael_Severity;
                if (category->isEnabled(bael_Severity::BAEL_INFO)) {
                    const char *formatSpec =
                                         "[7] %d shares of %s sold at %f\n";
                    bael_Log::format(messageBuffer(),
                                     messageBufferSize(),
                                     formatSpec, 400, "SUNW", 5.65);
                    bael_Log::logMessage(category,
                                         BloombergLP::bael_Severity::BAEL_INFO,
                                         __FILE__, __LINE__,
                                         messageBuffer());
                    if (veryVerbose) P(messageBuffer());
                }
            }
        }

        if (verbose) bsl::cout << "callback macro usage" << bsl::endl;
        {
            using namespace BloombergLP;

            BAEL_LOG_SET_CATEGORY("callback test");
            const Point point;
            bdef_Function <void (*)(BloombergLP::bdem_List *)> cb;
            cb = bdef_BindUtil::bind(&populateUsingPoint,
                                     bdef_PlaceHolders::_1,
                                     point);

            int errors = 0;
            if (point.x() > 255) {
                BAEL_LOGCB_ERROR(cb) << "X > 255" << BAEL_LOGCB_END;
                ++errors;
            }
            if (point.x() < -255) {
                BAEL_LOGCB_ERROR(cb) << "X < -255" << BAEL_LOGCB_END;
                ++errors;
            }
            if (point.y() > 255) {
                BAEL_LOGCB_ERROR(cb) << "Y > 255" << BAEL_LOGCB_END;
                ++errors;
            }
            if (point.y() < -255) {
                BAEL_LOGCB_ERROR(bdef_BindUtil::bind(&populateUsingPoint,
                                                     bdef_PlaceHolders::_1,
                                                     point))
                    << "Y < -255" << BAEL_LOGCB_END;
                ++errors;
            }
        }

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // BAEL_LOG_IS_ENABLED(SEVERITY);
        //
        // Concerns:
        //:  1. If the logger manager is not initialized,
        //:     'BAEL_LOG_IS_ENABLED' returns 'true' for severities higher
        //:     than 'WARN', and 'false' otherwise.
        //:
        //:  2. If the logger manager is initialized, 'BAEL_LOG_IS_ENABLED'
        //:     returns 'true' if any of the thresholds configured for the
        //:     current category are higher than the severity.
        //:
        //:  3. 'BAEL_LOG_IS_ENABLED' tests the thresholds configured for the
        //:     current category by rule based logging.
        //
        // Plan:
        //   1. Do not initialize a logger manager, and test calling
        //      'BAEL_LOG_IS_ENABLED' with various severities.  (C-1)
        //
        //   2. Initialize a logger manager, and for a set of possible
        //      severities, exhaustively test each combination of threshold
        //      severity value with 'BAEL_LOG_IS_ENABLED' severity value, for
        //      each of the 4 thresholds (record, passthrough, trigger, and
        //      trigger-all). (C-2)
        //
        //   3. Exhaustively test each combination of a rule-enabled threshold
        //      severity value with 'BAEL_LOG_IS_ENABLED' severity value. (C-3)
        //
        // Testing:
        //   BAEL_IS_ENABLED(SEVERITY)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nBAEL_LOG_IS_ENABLED(SEVERITY)\n"
                      << "\n=============================\n";

        if (verbose) bsl::cout << "\tTest without a logger manager.\n";

        {
            BAEL_LOG_SET_CATEGORY("TEST.CATEGORY1");
            ASSERT(!BAEL_LOG_IS_ENABLED(TRACE));
            ASSERT(!BAEL_LOG_IS_ENABLED(DEBUG));
            ASSERT(!BAEL_LOG_IS_ENABLED(INFO));
            ASSERT( BAEL_LOG_IS_ENABLED(WARN));
            ASSERT( BAEL_LOG_IS_ENABLED(ERROR));
            ASSERT( BAEL_LOG_IS_ENABLED(FATAL));
        }

        const int DATA[] = { OFF, TRACE, DEBUG, INFO, WARN, ERROR, FATAL };
        const int NUM_DATA = sizeof (DATA) / sizeof (*DATA);

        BloombergLP::bael_DefaultObserver observer(bsl::cout);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(OFF, OFF, OFF, OFF);

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        bael_LoggerManager& manager = bael_LoggerManager::singleton();

        if (verbose) bsl::cout << "\tExhaustively test w/o logging rules.\n";
        {
            BAEL_LOG_SET_CATEGORY("TEST.CATEGORY2");
            for (int thresholdIdx = 0; thresholdIdx < 4; ++thresholdIdx) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    Thresholds threshold(OFF, OFF, OFF, OFF);
                    switch (thresholdIdx) {
                      case 0: threshold.setRecordLevel(DATA[i]); break;
                      case 1: threshold.setPassLevel(DATA[i]); break;
                      case 2: threshold.setTriggerLevel(DATA[i]); break;
                      case 3: threshold.setTriggerAllLevel(DATA[i]); break;
                      default: BSLS_ASSERT(false);
                    }
                    manager.setCategory("TEST.CATEGORY2",
                                        threshold.recordLevel(),
                                        threshold.passLevel(),
                                        threshold.triggerLevel(),
                                        threshold.triggerAllLevel());
                    for (int j = 1; j < NUM_DATA; ++j) {
                        bool EXP = DATA[j] <= DATA[i];
                        LOOP3_ASSERT(EXP, DATA[i], DATA[j],
                                     EXP == BAEL_LOG_IS_ENABLED(DATA[j]));
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\tTest w/ logging rules." << bsl::endl;
        {
            BAEL_LOG_SET_CATEGORY("TEST.CATEGORY3");
            for (int i = 0; i < NUM_DATA; ++i) {
                bael_Rule rule("TEST.CATEGORY3", OFF, DATA[i], OFF, OFF);
                manager.addRule(rule);
                for (int j = 1;  j < NUM_DATA; ++j) {
                    bool EXP = DATA[j] <= DATA[i];
                    LOOP3_ASSERT(EXP, DATA[i], DATA[j],
                                 EXP == BAEL_LOG_IS_ENABLED(DATA[j]));
                }
                manager.removeAllRules();
            }
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING RULE BASED LOGGING: 'logMessage'
        //
        // Concerns:
        //   That the 'logMessages' method uses the current installed rules
        //   when determining whether to log a message.
        //
        // Plan:
        //   Create a series of test threshold values and a series of
        //   threshold-aggregate values.  Maintain, over the test, the
        //   expected  number of published records and the number of records
        //   currently in the buffer, at any time (note that the records in
        //   the buffered are added to the published records when a trigger
        //   occurs).  For each threshold-aggregate value: create a category
        //   with that threshold-aggregate value; test the 'logMessage' method
        //   with a variety of severity levels without a logging rule, then for
        //   each test threshold-aggregate value, create a logging rule that
        //   applies to the category and test the 'logMessage' method.  Compare
        //   the expected number of published records against the actual number
        //   of published records.
        //
        // Testing:
        //   bool logMessage(const bael_Category *category,
        //                   int                  severity,
        //                   bael_Record         *record);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing Rule Based Logging: logMessage\n"
                      << "======================================\n";
        using namespace BloombergLP;

        int VALUES[] = { 1,
                         Sev::BAEL_FATAL - 1,
                         Sev::BAEL_FATAL,
                         Sev::BAEL_FATAL + 1,
                         Sev::BAEL_ERROR - 1,
                         Sev::BAEL_ERROR,
                         Sev::BAEL_ERROR + 1,
                         Sev::BAEL_WARN - 1,
                         Sev::BAEL_WARN,
                         Sev::BAEL_WARN + 1,
                         Sev::BAEL_INFO - 1,
                         Sev::BAEL_INFO,
                         Sev::BAEL_INFO + 1,
                         Sev::BAEL_DEBUG - 1,
                         Sev::BAEL_DEBUG,
                         Sev::BAEL_DEBUG + 1,
                         Sev::BAEL_TRACE - 1,
                         Sev::BAEL_TRACE,
                         Sev::BAEL_TRACE + 1
        };
        const int NUM_VALUES = sizeof (VALUES)/sizeof(*VALUES);
        bsl::vector<Thresholds> thresholds;
        for (int i = 0; i < NUM_VALUES; ++i) {
            thresholds.push_back(Thresholds(VALUES[i], 255, 255, 255));
            thresholds.push_back(Thresholds(255, VALUES[i], 255, 255));
            thresholds.push_back(Thresholds(255, 255, VALUES[i], 255));
            thresholds.push_back(Thresholds(255, 255, 255, VALUES[i]));
        }

        // We must keep track of the total number of published records, and
        // the number of buffered records.  The accumulated buffered records
        // will be published when logged message is at a higher severity than
        // the trigger level.
        int numPublished       = 0;
        int numBufferedRecords = 0;
        bael_LoggerManagerConfiguration lmc;
        lmc.setTriggerMarkers(
                             bael_LoggerManagerConfiguration::BAEL_NO_MARKERS);

        bael_LoggerManager::initSingleton(TO, lmc, &ta);
        bael_LoggerManager& manager = bael_LoggerManager::singleton();
        for (int i = 0; i < (int) thresholds.size(); ++i) {
            bael_Category *category =
                          manager.setCategory("TestCategory",
                                              thresholds[i].recordLevel(),
                                              thresholds[i].passLevel(),
                                              thresholds[i].triggerLevel(),
                                              thresholds[i].triggerAllLevel());

            // Test with no rules.
            for (int j = 0; j < NUM_VALUES; ++j) {
                bael_Record *record = manager.getLogger().getRecord("X", 1);

                // Figure out the various thresholds.  Note that we have only
                // one thread, so trigger is equivalent to triggerAll.
                bool buffer  = VALUES[j] <= thresholds[i].recordLevel();
                bool pass    = VALUES[j] <= thresholds[i].passLevel();
                bool trigger = VALUES[j] <= thresholds[i].triggerLevel()
                            || VALUES[j] <= thresholds[i].triggerAllLevel();

                Obj::logMessage(category, VALUES[j], record);
                if (buffer) { ++numBufferedRecords; }
                if (pass)   { ++numPublished; }
                if (trigger) {
                    numPublished += numBufferedRecords;
                    numBufferedRecords = 0;
                }
                LOOP2_ASSERT(i,j, numPublished == TO->numPublishedRecords());
            }

            // Test with rule that does not apply.
            manager.addRule(bael_Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                bael_Record *record = manager.getLogger().getRecord("X", 1);

                // Figure out the various thresholds.  Note that we have only
                // one thread, so trigger is equivalent to triggerAll.
                bool buffer  = VALUES[j] <= thresholds[i].recordLevel();
                bool pass    = VALUES[j] <= thresholds[i].passLevel();
                bool trigger = VALUES[j] <= thresholds[i].triggerLevel()
                            || VALUES[j] <= thresholds[i].triggerAllLevel();

                Obj::logMessage(category, VALUES[j], record);
                if (buffer) { ++numBufferedRecords; }
                if (pass)   { ++numPublished; }
                if (trigger) {
                    numPublished += numBufferedRecords;
                    numBufferedRecords = 0;
                }
                LOOP2_ASSERT(i,j, numPublished == TO->numPublishedRecords());
            }

            // Test with a rule that does apply.
            for (int j = 0; j < (int) thresholds.size(); ++j) {
                manager.removeAllRules();
                bael_Rule rule("Test*",
                               thresholds[j].recordLevel(),
                               thresholds[j].passLevel(),
                               thresholds[j].triggerLevel(),
                               thresholds[j].triggerAllLevel());
                manager.addRule(rule);
                for (int k = 0; k < NUM_VALUES; ++k) {
                    bael_Record *record = manager.getLogger().getRecord("X",1);

                    // Each level is the minimum severity (maximum value)
                    // between the rule's threshold and the categories
                    // threshold.
                    int recordLvl  = bsl::max(thresholds[i].recordLevel(),
                                              thresholds[j].recordLevel());
                    int passLvl    = bsl::max(thresholds[i].passLevel(),
                                              thresholds[j].passLevel());
                    int triggerLvl = bsl::max(thresholds[i].triggerLevel(),
                                              thresholds[j].triggerLevel());
                    int trigAllLvl = bsl::max(thresholds[i].triggerAllLevel(),
                                              thresholds[j].triggerAllLevel());

                    // Figure out the various thresholds.  Note that we have
                    // only one thread, so trigger is equivalent to triggerAll.
                    bool buffer  = VALUES[k] <= recordLvl;
                    bool pass    = VALUES[k] <= passLvl;
                    bool trigger = VALUES[k] <= triggerLvl
                                || VALUES[k] <= trigAllLvl;

                    Obj::logMessage(category, VALUES[k], record);
                    if (buffer) { ++numBufferedRecords; }
                    if (pass)   { ++numPublished; }
                    if (trigger) {
                        numPublished += numBufferedRecords;
                        numBufferedRecords = 0;
                    }
                    LOOP3_ASSERT(i, j, k,
                                 numPublished == TO->numPublishedRecords());
                }
            }
            manager.removeAllRules();
        }
        bael_LoggerManager::shutDownSingleton();

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING RULE BASED LOGGING: 'isCategoryEnabled'
        //
        // Concerns:
        //   That the 'isCategoryEnabled' method is using the current installed
        //   rules when determining whether a category is enabled.
        //
        // Plan:
        //   Create a series of test threshold values and a series of
        //   threshold-aggregate values.  Test the 'isCategoryEnabled' method
        //   for each test threshold value without a logger manager.  Then
        //   create a logger manager.  For each threshold-aggregate value:
        //   create a category with that threshold-aggregate value, test the
        //   'isCategoryEnabled' method with a variety of severity levels
        //   without a logging rule, then for each test threshold-aggregate
        //   value, create a logging rule that applies to the category and test
        //   the 'isCategoryEnabled' method.  Verify that the
        //   'isCategoryEnabled' method behaves correctly given the categories
        //   threshold-aggregate, the rule's threshold-aggregate, and the
        //   supplied severity.
        //
        // Testing:
        //   bool isCategoryEnabled(Holder *holder, int severity)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing Rule Based Logging: isCategoryEnabled\n"
                      << "=============================================\n";
        using namespace BloombergLP;

        int VALUES[] = { 1,
                         Sev::BAEL_FATAL - 1,
                         Sev::BAEL_FATAL,
                         Sev::BAEL_FATAL + 1,
                         Sev::BAEL_ERROR - 1,
                         Sev::BAEL_ERROR,
                         Sev::BAEL_ERROR + 1,
                         Sev::BAEL_WARN - 1,
                         Sev::BAEL_WARN,
                         Sev::BAEL_WARN + 1,
                         Sev::BAEL_INFO - 1,
                         Sev::BAEL_INFO,
                         Sev::BAEL_INFO + 1,
                         Sev::BAEL_DEBUG - 1,
                         Sev::BAEL_DEBUG,
                         Sev::BAEL_DEBUG + 1,
                         Sev::BAEL_TRACE - 1,
                         Sev::BAEL_TRACE,
                         Sev::BAEL_TRACE + 1
        };
        const int NUM_VALUES = sizeof (VALUES)/sizeof(*VALUES);

        bsl::vector<Thresholds> thresholds;
        for (int i = 0; i < NUM_VALUES; ++i) {
            thresholds.push_back(Thresholds(VALUES[i], 255, 255, 255));
            thresholds.push_back(Thresholds(255, VALUES[i], 255, 255));
            thresholds.push_back(Thresholds(255, 255, VALUES[i], 255));
            thresholds.push_back(Thresholds(255, 255, 255, VALUES[i]));
        }

        if (veryVerbose) {
            bsl::cout << "\tSanity test without a logger manager" << bsl::endl;
        }
        for (int i = 0; i < NUM_VALUES; ++i) {
            bool enabled = VALUES[i] <= Sev::BAEL_WARN;
            Holder holder = { 0, 0, 0};
            ASSERT(enabled == Obj::isCategoryEnabled(&holder, VALUES[i]));
        }

        if (veryVerbose) {
            bsl::cout << "\tTest with and without an applicable rule."
                      << bsl::endl;
        }

        bael_LoggerManagerConfiguration lmc;
        bael_LoggerManager::initSingleton(TO, lmc, &ta);
        bael_LoggerManager& manager = bael_LoggerManager::singleton();

        // Create a test category for each test threshold-aggregate
        for (int i = 0; i < (int) thresholds.size(); ++i) {
            bael_Category *category =
                          manager.setCategory("TestCategory",
                                              thresholds[i].recordLevel(),
                                              thresholds[i].passLevel(),
                                              thresholds[i].triggerLevel(),
                                              thresholds[i].triggerAllLevel());

            // Test 'isCategoryEnabled' with no rules.
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                        VALUES[j] <= Thresholds::maxLevel(thresholds[i]);
                Holder holder = { 0, category, 0};
                LOOP2_ASSERT(i, j,
                             enabled ==
                             Obj::isCategoryEnabled(&holder, VALUES[j]));
            }

            // Test with rule that does not apply.
            manager.addRule(bael_Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                        VALUES[j] <= Thresholds::maxLevel(thresholds[i]);
                Holder holder = { 0, category, 0};
                LOOP2_ASSERT(i, j,
                             enabled ==
                             Obj::isCategoryEnabled(&holder, VALUES[j]));
            }

            // Test with a rule that does apply.
            for (int j = 0; j < (int) thresholds.size(); ++j) {
                manager.removeAllRules();
                bael_Rule rule("Test*",
                               thresholds[j].recordLevel(),
                               thresholds[j].passLevel(),
                               thresholds[j].triggerLevel(),
                               thresholds[j].triggerAllLevel());
                manager.addRule(rule);
                for (int k = 0; k < NUM_VALUES; ++k) {
                    int maxLevel =
                        bsl::max(Thresholds::maxLevel(thresholds[i]),
                                 Thresholds::maxLevel(thresholds[j]));
                    bool enabled = VALUES[k] <= maxLevel;
                    Holder holder = { 0, category, 0};
                    LOOP3_ASSERT(i, j, k,
                                 enabled ==
                                 Obj::isCategoryEnabled(&holder, VALUES[k]));
                    if (enabled !=
                        Obj::isCategoryEnabled(&holder, VALUES[k])) {
                        P_(thresholds[i]); P_(thresholds[j]); P(enabled);
                    }

                }

            }
            manager.removeAllRules();
        }
        bael_LoggerManager::shutDownSingleton();

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING BAEL_LOG_SET_DYNAMIC_CATEGORY and BAEL_LOG2 in multiple
        // threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BAEL_LOG_SET_DYNAMIC_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BAEL_LOG_SET_DYNAMIC_CATEGORY\n"
                      << "=====================================\n";

        using namespace BAEL_LOG_TEST_CASE_24;

        numIterations = 10;
        arg1          = -99.244;
        arg2          = "Hello World";

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread24);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bcema_TestAllocator ta(veryVeryVerbose);
            BloombergLP::bael_LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level
            BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread24);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread24);
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING BAEL_LOG_SET_CATEGORY and BAEL_LOG2 in multiple threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BAEL_LOG_SET_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BAEL_LOG_SET_CATEGORY\n"
                      << "=============================\n";

        using namespace BAEL_LOG_TEST_CASE_23;

        numIterations = 10;
        arg1          = -99.234;
        arg2          = "Hello World";

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread23);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bcema_TestAllocator ta(veryVeryVerbose);
            BloombergLP::bael_LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level
            BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread23);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread23);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING BAEL_LOG_SET_DYNAMIC_CATEGORY and BAEL_LOG_TRACE in
        // multiple threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BAEL_LOG_SET_DYNAMIC_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BAEL_LOG_SET_DYNAMIC_CATEGORY\n"
                      << "=====================================\n";

        using namespace BAEL_LOG_TEST_CASE_22;

        numIterations = 10;
        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread22);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bcema_TestAllocator ta(veryVeryVerbose);
            BloombergLP::bael_LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level
            BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread22);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread22);
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING BAEL_LOG_SET_CATEGORY and BAEL_LOG_TRACE in multiple threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BAEL_LOG_SET_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BAEL_LOG_SET_CATEGORY\n"
                      << "=============================\n";

        using namespace BAEL_LOG_TEST_CASE_21;

        numIterations = 10;
        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread21);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bcema_TestAllocator ta(veryVeryVerbose);
            BloombergLP::bael_LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
               BloombergLP::bael_Severity::BAEL_TRACE,  // record level
               BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
               BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
               BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level
            BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread21);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread21);
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING STATIC FUNCTIONS
        //
        // Concerns:
        //   TBD
        //
        // Plan:
        //   TBD
        //
        // Testing:
        //   bool isCategoryEnabled(Holder *holder, int severity)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing static functions\n"
                      << "========================\n";

        const int UC = Holder::BAEL_UNINITIALIZED_CATEGORY;
        const int DC = Holder::BAEL_DYNAMIC_CATEGORY;

        // Warning: Do not change the order of the test cases.  This test case
        // relies on having all the test cases that require the logger manager
        // to be initialized to be after all the cases that don't require it.
        static const struct {
            int         d_line;            // line number
            bool        d_useLM;           // is logger manager initialized
            int         d_threshold;       // threshold
            bool        d_useCategory;     // use category
            const char *d_name_p;          // category name
            int         d_severity;        // severity
            bool        d_expResult;       // result
        } DATA[] = {
          // Line  lmInit Threshold  Category  name    severity   result
          // ----  ------ ---------  --------  ----    --------   ------
            { L_, false,       0,     false,    "",        1,       true   },
            { L_, false,       5,     true,     "AA",     32,       true   },
            { L_, false,       5,     false,    "AB",     64,       true   },
            { L_, false,       5,     false,    "AC",     96,       true   },
            { L_, false,       5,     false,    "AD",    128,       false  },
            { L_, false,      32,     true,     "ZA",     32,       true   },
            { L_, false,      64,     false,    "ZB",     64,       true   },
            { L_, false,     128,     false,    "ZC",     96,       true   },
            { L_, false,     128,     false,    "ZD",     97,       false  },
            { L_, false,     255,     false,    "ZE",    128,       false  },
            { L_, false,      UC,     true,     "AE",      1,       true   },
            { L_, false,      UC,     false,    "AF",      1,       true   },
            { L_, false,      UC,     true,     "AG",     32,       true   },
            { L_, false,      UC,     false,    "AH",     32,       true   },
            { L_, false,      UC,     true,     "BE",     64,       true   },
            { L_, false,      UC,     false,    "BF",     64,       true   },
            { L_, false,      UC,     true,     "CE",     95,       true   },
            { L_, false,      UC,     false,    "CF",     95,       true   },
            { L_, false,      UC,     true,     "DE",     96,       true   },
            { L_, false,      UC,     false,    "DF",     96,       true   },
            { L_, false,      UC,     true,     "EE",     97,       false  },
            { L_, false,      UC,     false,    "EF",     97,       false  },
            { L_, false,      DC,     false,    "GA",      1,       true   },
            { L_, false,      DC,     false,    "GB",     95,       true   },
            { L_, false,      DC,     false,    "GC",     96,       true   },
            { L_, false,      DC,     false,    "GD",     97,       false  },
            { L_, false,      DC,     true,     "GE",      1,       true   },
            { L_, false,      DC,     true,     "GF",     95,       true   },
            { L_, false,      DC,     true,     "GG",     96,       true   },
            { L_, false,      DC,     true,     "GH",     97,       false  },
            { L_, true,       DC,     false,    "GI",      1,       true   },
            { L_, true,       DC,     false,    "GJ",     95,       true   },
            { L_, true,       DC,     false,    "GK",     96,       true   },
            { L_, true,       DC,     false,    "GL",     97,       false  },
            { L_, true,       DC,     true,     "GM", DC - 3,       true  },
            { L_, true,       DC,     true,     "GN", DC - 2,       true  },
            { L_, true,       DC,     true,     "GO", DC - 1,       false  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        CategoryManager CM;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const bool    USE_LM      = DATA[i].d_useLM;
            const int     THRESHOLD   = DATA[i].d_threshold;
            const bool    USE_CAT     = DATA[i].d_useCategory;
            const char   *CAT_NAME    = DATA[i].d_name_p;
            const int     SEVERITY    = DATA[i].d_severity;
            const bool    RESULT      = DATA[i].d_expResult;

            Holder mH = { THRESHOLD, 0, 0 };
            if (USE_CAT) {
                const Cat *CATEGORY = CM.addCategory(CAT_NAME, THRESHOLD - 2,
                                                     1, 1, 1);
                LOOP2_ASSERT(LINE, CAT_NAME, CATEGORY);
                mH.setCategory(CATEGORY);
            }
            static bool LM_INITIALIZED = false;
            if (USE_LM && !LM_INITIALIZED) {
                LM_INITIALIZED = true;
                BloombergLP::bael_LoggerManagerConfiguration lmc;
                BloombergLP::bael_LoggerManager::initSingleton(TO, lmc, &ta);
            }
            bool result = bael_Log::isCategoryEnabled(&mH, SEVERITY);
            LOOP_ASSERT(LINE, RESULT == result);
        }

        ASSERT(bael_LoggerManager::isInitialized());

        // Test getRecord
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const bool    USE_CAT     = DATA[i].d_useCategory;
            const char   *FILENAME    = DATA[i].d_name_p;

            const Cat   *CATEGORY = USE_CAT ? (Cat *) 0 : (Cat *) 1;
            bael_Record *RECORD   = bael_Log::getRecord(CATEGORY,
                                                        FILENAME,
                                                        LINE);
            LOOP_ASSERT(LINE, LINE == RECORD->fixedFields().lineNumber());
            LOOP_ASSERT(LINE, 0 == bsl::strcmp(
                            FILENAME,
                            RECORD->fixedFields().fileName()));
        }

        BloombergLP::bael_LoggerManager::shutDownSingleton();

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING bael_Log_Stream
        //
        // Concerns:
        //   The basic concern is that the creators, manipulators and
        //   accessors operate as expected.
        //
        // Plan:
        //   Specify a large set of data values that can be used to construct
        //   a bael_Log_Stream object.  Initialize an object X by calling the
        //   constructor using one of the specified data values.  Using the
        //   accessor functions verify that X was correctly initialized.  Then
        //   use the manipulator functions to put X into a different state.
        //   Again use the accessor functions to confirm the state of X.
        //
        // Testing:
        //  TBD
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Test bael_Log_Stream" << bsl::endl
                               << "====================" << bsl::endl;

        static const struct {
            int         d_line;            // line number
            int         d_recordLevel;     // record level
            int         d_passLevel;       // pass level
            int         d_triggerLevel;    // trigger level
            int         d_triggerAllLevel; // trigger all level
            const char *d_name_p;          // category name
            const char *d_fileName_p;      // filename to use for logging
            int         d_severity;        // logging severity
        } DATA[] = {
 // line  record    pass     trigger triggerAll cat         file   severity
 // no.   level     level    level   level      name        name
 // ----  ------    ------   ------  -----      ------      ----   --------
  {  L_,  0,        0,        0,        0,       "",        "",           1 },
  {  L_,  1,        0,        0,        0,       "A1",      "a.c",        1 },
  {  L_,  0,        1,        0,        0,       "A2",      "h.f",        2 },
  {  L_,  0,        0,        1,        0,       "AA1",     "z.cpp",      3 },
  {  L_,  0,        0,        0,        1,       "AA2",     "hello.c",    4 },
  {  L_,  16,       32,       48,       64,      "AB",      __FILE__,     5 },
  {  L_,  64,       48,       32,       16,      "AC",      "bael",       6 },
  {  L_,  16,       32,       64,       48,      "AD",      "baelu",      7 },
  {  L_,  16,       48,       32,       64,      "AAA",     "aaa",        8 },
  {  L_,  32,       16,       48,       64,      "ABC",     "ABCD",       9 },
  {  L_,  255,      0,        0,        0,       "ABCD1",   "w.c",        10 },
  {  L_,  0,        255,      0,        0,       "ABCD2",   "a.h",        11 },
  {  L_,  0,        0,        255,      0,       "DEFG",    "b.c",        12 },
  {  L_,  0,        0,        0,        255,     "HIJK",    "c.d",        13 },
  {  L_,  255,      255,      255,      255,     "ALL",     "e.f",        14 },
  {  L_,  254,      0,        0,        0,       "ALL.FS",  __FILE__,     15 },
  {  L_,  0,        254,      0,        0,       "ALL.TCP", __FILE__,     16 },
  {  L_,  0,        0,        254,      0,       "ALL.1.2", __FILE__,     17 },
  {  L_,  0,        0,        0,        254,     "BAEL1",   __FILE__,     18 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        TestAllocator ta(veryVeryVerbose); const TestAllocator& TA = ta;
        TestAllocator da(veryVeryVerbose); const TestAllocator& DA = da;
        bslma_DefaultAllocatorGuard guard(&da);

        CategoryManager CM(&ta);
        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const int     RECORD      = DATA[i].d_recordLevel;
            const int     PASS        = DATA[i].d_passLevel;
            const int     TRIGGER     = DATA[i].d_triggerLevel;
            const int     TRIGGER_ALL = DATA[i].d_triggerAllLevel;
            const char   *CAT_NAME    = DATA[i].d_name_p;
            const char   *FILENAME    = DATA[i].d_fileName_p;
            const int     SEVERITY    = DATA[i].d_severity;
            const int     NUM_BYTES   = DA.numBytesInUse();

            const Cat *CATEGORY = CM.addCategory(CAT_NAME,
                                                 RECORD,
                                                 PASS,
                                                 TRIGGER,
                                                 TRIGGER_ALL);

            ASSERT(CATEGORY);

            LogStream mL(CATEGORY, FILENAME, LINE, SEVERITY);
            const LogStream& L = mL;

            LOOP2_ASSERT(CATEGORY, L.category(), CATEGORY == L.category());
            LOOP2_ASSERT(SEVERITY, L.severity(), SEVERITY == L.severity());
            LOOP_ASSERT(LINE, LINE == L.record()->fixedFields().lineNumber());
            LOOP_ASSERT(LINE, 0 == bsl::strcmp(FILENAME,
                                    L.record()->fixedFields().fileName()));
            LOOP_ASSERT(LINE, LINE == mL.record()->fixedFields().lineNumber());
            LOOP_ASSERT(LINE, 0 == bsl::strcmp(FILENAME,
                                    mL.record()->fixedFields().fileName()));
//             BAEL_LOG_SET_CATEGORY(CAT_NAME);
//             BAEL_LOG_TRACE << "Random Trace";
//             LOOP_ASSERT(LINE, BAEL_STREAM == mL.stream());
//             BAEL_LOG_END;
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING BAEL_LOG_SET_DYNAMIC_CATEGORY
        //
        // Concerns:
        //   This macro must be safe in absence of a logger manager.  This
        //   macro must be correct when 'bael_LoggerManager::setCategory' is
        //   called to change the threshold levels of the dynamic category
        //   before logging macros are encountered.
        //
        // Plan:
        //   Intentionally invoke the macro without first initializing the
        //   logger manager.  Verify that the number of records going to
        //   'bsl::cout' is expected.  Then invoke the macro in presence of a
        //   logger manager and call 'bael_LoggerManager::setCategory' to
        //   change the underlying category.  Verify that the number of
        //   published records is expected.
        //
        // Testing:
        //   BAEL_LOG_SET_DYNAMIC_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BAEL_LOG_SET_DYNAMIC_CATEGORY\n"
                      << "=====================================\n";

        using namespace BAEL_LOG_TEST_CASE_18;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {

#ifdef BSLS_PLATFORM__OS_UNIX
            fflush(stderr);
            bsl::string filename = tempnam(0, "bael_log");
            int fd = creat(filename.c_str(), 0777);
            ASSERT(fd != -1);
            int saved_stderr_fd = dup(2);
            dup2(fd, 2);
            if (veryVerbose)
                bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            BAEL_LOG_SET_DYNAMIC_CATEGORY("ThereIsNoLoggerManager");

            BAEL_LOG_TRACE << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_INFO  << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_WARN  << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "No Logger Manager!" << BAEL_LOG_END;

#ifdef BSLS_PLATFORM__OS_UNIX
            fflush(stderr);
            dup2(saved_stderr_fd, 2);

            bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
            int numLines = 0;
            bsl::string line;
            while (getline(fs, line)) {
                ++numLines;
                if (veryVerbose)
                    bsl::cout << "\t>>" << line << "<<" << bsl::endl;
            }
            fs.close();
            ASSERT(6 == numLines);
            unlink(filename.c_str());
#endif
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bael_LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level
            BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc);

            // Re "sieve" category: (1) if recorded, then also published;
            // (2) never triggered.

            BloombergLP::bael_Administration::addCategory(
                                         "sieve",
                                         BloombergLP::bael_Severity::BAEL_INFO,
                                         BloombergLP::bael_Severity::BAEL_INFO,
                                         0,
                                         0);
            BAEL_LOG_SET_DYNAMIC_CATEGORY("sieve")

            int numPublishedRecords = TO->numPublishedRecords();

            BAEL_LOG_TRACE << "message" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "message" << BAEL_LOG_END;
            BAEL_LOG_INFO  << "message" << BAEL_LOG_END;
            BAEL_LOG_WARN  << "message" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "message" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "message" << BAEL_LOG_END;

            ASSERT(numPublishedRecords + 4 == TO->numPublishedRecords());

            numPublishedRecords = TO->numPublishedRecords();

            bael_LoggerManager::singleton().setCategory(
                                         "sieve",
                                         0,
                                         BloombergLP::bael_Severity::BAEL_WARN,
                                         0,
                                         0);

            BAEL_LOG_TRACE << "message" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "message" << BAEL_LOG_END;
            BAEL_LOG_INFO  << "message" << BAEL_LOG_END;
            BAEL_LOG_WARN  << "message" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "message" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "message" << BAEL_LOG_END;

            ASSERT(numPublishedRecords + 3 == TO->numPublishedRecords());

            numPublishedRecords = TO->numPublishedRecords();

            executeInParallel(NUM_THREADS, workerThread18);

            ASSERT(numPublishedRecords + NUM_MSGS / 2 * NUM_THREADS
                                                == TO->numPublishedRecords());
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {

#ifdef BSLS_PLATFORM__OS_UNIX
            fflush(stderr);
            bsl::string filename = tempnam(0, "bael_log");
            int fd = creat(filename.c_str(), 0777);
            ASSERT(fd != -1);
            int saved_stderr_fd = dup(2);
            dup2(fd, 2);
            if (veryVerbose)
                bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

            bsl::stringstream os;
            bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
            bsl::cerr.rdbuf(os.rdbuf());

            BAEL_LOG_SET_DYNAMIC_CATEGORY("ThereIsNoLoggerManager");

            BAEL_LOG_TRACE << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_DEBUG << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_INFO  << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_WARN  << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_ERROR << "No Logger Manager!" << BAEL_LOG_END;
            BAEL_LOG_FATAL << "No Logger Manager!" << BAEL_LOG_END;

#ifdef BSLS_PLATFORM__OS_UNIX
            fflush(stderr);
            dup2(saved_stderr_fd, 2);

            bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
            int numLines = 0;
            bsl::string line;
            while (getline(fs, line)) {
                ++numLines;
                if (veryVerbose)
                    bsl::cout << "\t>>" << line << "<<" << bsl::endl;
            }
            fs.close();
            ASSERT(6 == numLines);
            unlink(filename.c_str());
#endif
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING CALLBACK MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
        //
        // Concerns:
        //   This macro must be safe against being invoked before the logger
        //   manager is instantiated.
        //
        // Plan:
        //   Invoke the macro and confirm that the process continues normally.
        //
        // Testing:
        //   BAEL_LOGCB_TRACE
        //   BAEL_LOGCB_DEBUG
        //   BAEL_LOGCB_INFO
        //   BAEL_LOGCB_WARN
        //   BAEL_LOGCB_ERROR
        //   BAEL_LOGCB_FATAL
        // --------------------------------------------------------------------

        class LogOnDestruction {
          public:
            LogOnDestruction()
            {
            }
            ~LogOnDestruction()
            {

#ifdef BSLS_PLATFORM__OS_UNIX
                fflush(stderr);
                bsl::string filename = tempnam(0, "bael_log");
                int fd = creat(filename.c_str(), 0777);
                ASSERT(fd != -1);
                int saved_stderr_fd = dup(2);
                dup2(fd, 2);
#endif

                bsl::stringstream os;
                bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
                bsl::cerr.rdbuf(os.rdbuf());

                ASSERT(false == bael_LoggerManager::isInitialized());
                BloombergLP::bdef_Function
                        <void (*)(BloombergLP::bdem_List *)> callback =
                                                                  &incCallback;
                numIncCallback = 0;

                BAEL_LOG_SET_CATEGORY("LoggerManagerDestroyed");

                BAEL_LOGCB_TRACE(callback) << "No Logger Manager!"
                                           << BAEL_LOGCB_END;
                BAEL_LOGCB_DEBUG(callback) << "No Logger Manager!"
                                           << BAEL_LOGCB_END;
                BAEL_LOGCB_INFO(callback)  << "No Logger Manager!"
                                           << BAEL_LOGCB_END;
                BAEL_LOGCB_WARN(callback)  << "No Logger Manager!"
                                           << BAEL_LOGCB_END;
                BAEL_LOGCB_ERROR(callback) << "No Logger Manager!"
                                           << BAEL_LOGCB_END;
                BAEL_LOGCB_FATAL(callback) << "No Logger Manager!"
                                           << BAEL_LOGCB_END;

                ASSERT(3 == numIncCallback);

#ifdef BSLS_PLATFORM__OS_UNIX
                fflush(stderr);
                dup2(saved_stderr_fd, 2);

                bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
                int numLines = 0;
                bsl::string line;
                while (getline(fs, line)) {
                    ++numLines;
                }
                fs.close();
                ASSERT(6 == numLines);
                unlink(filename.c_str());
#endif
                ASSERT("" == os.str());
                bsl::cerr.rdbuf(cerrBuf);
            }
        };

        static LogOnDestruction logOnDestruction;

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing callback macro safety w/o LoggerManager\n"
                      << "===============================================\n";

        BloombergLP::bdef_Function<void (*)(BloombergLP::bdem_List *)> callback
                                                                = &incCallback;

        numIncCallback = 0;

        if (verbose)
            bsl::cout << "Safely invoked 'BAEL_LOG_SET_CATEGORY' macro"
                      << bsl::endl;

#ifdef BSLS_PLATFORM__OS_UNIX
        fflush(stderr);
        bsl::string filename = tempnam(0, "bael_log");
        int fd = creat(filename.c_str(), 0777);
        ASSERT(fd != -1);
        int saved_stderr_fd = dup(2);
        dup2(fd, 2);
        if (verbose)
            bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

        bsl::stringstream os;
        bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
        bsl::cerr.rdbuf(os.rdbuf());

        BAEL_LOG_SET_CATEGORY("ThereIsNoLoggerManager");

        BAEL_LOGCB_TRACE(callback) << "No Logger Manager!" << BAEL_LOGCB_END;
        BAEL_LOGCB_DEBUG(callback) << "No Logger Manager!" << BAEL_LOGCB_END;
        BAEL_LOGCB_INFO(callback)  << "No Logger Manager!" << BAEL_LOGCB_END;
        BAEL_LOGCB_WARN(callback)  << "No Logger Manager!" << BAEL_LOGCB_END;
        BAEL_LOGCB_ERROR(callback) << "No Logger Manager!" << BAEL_LOGCB_END;
        BAEL_LOGCB_FATAL(callback) << "No Logger Manager!" << BAEL_LOGCB_END;

#ifdef BSLS_PLATFORM__OS_UNIX
        fflush(stderr);
        dup2(saved_stderr_fd, 2);

        bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
        int numLines = 0;
        bsl::string line;
        while (getline(fs, line)) {
            ++numLines;
            if (veryVerbose) bsl::cout << "\t>>" << line << "<<" << bsl::endl;
        }
        fs.close();
        ASSERT(6 == numLines);
        unlink(filename.c_str());
#endif

        ASSERT(3 == numIncCallback);

        if (verbose)
            bsl::cout << "Safely invoked stream-style macros" << bsl::endl;

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard guard(TO, lmc);

        ASSERT("" == os.str());
        bsl::cerr.rdbuf(cerrBuf);

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING OSTRSTREAM MACROS WITH CALLBACK
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   BAEL_LOGCB_TRACE
        //   BAEL_LOGCB_DEBUG
        //   BAEL_LOGCB_INFO
        //   BAEL_LOGCB_WARN
        //   BAEL_LOGCB_ERROR
        //   BAEL_LOGCB_FATAL
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing callback 'ostrstream' Macros"
                               << bsl::endl
                               << "===================================="
                               << bsl::endl;

        BloombergLP::bdef_Function
                        <void (*)(BloombergLP::bdem_List *)> callback =
                                                                  &incCallback;
        numIncCallback = 0;

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::bael_Administration::addCategory(
                                     "sieve",
                                     BloombergLP::bael_Severity::BAEL_TRACE,
                                     BloombergLP::bael_Severity::BAEL_TRACE,
                                     0,
                                     0);
        BAEL_LOG_SET_CATEGORY("sieve")

        BAEL_LOG_TRACE << "This will load the category" << BAEL_LOG_END

        const Cat  *CAT   = BAEL_LOG_CATEGORY;
        const char *FILE  = __FILE__;

        const int TRACE = BloombergLP::bael_Severity::BAEL_TRACE;
        const int DEBUG = BloombergLP::bael_Severity::BAEL_DEBUG;
        const int INFO  = BloombergLP::bael_Severity::BAEL_INFO;
        const int WARN  = BloombergLP::bael_Severity::BAEL_WARN;
        const int ERROR = BloombergLP::bael_Severity::BAEL_ERROR;
        const int FATAL = BloombergLP::bael_Severity::BAEL_FATAL;

        BloombergLP::bael_Administration::addCategory(
                                  "noTRACE",
                                  BloombergLP::bael_Severity::BAEL_TRACE - 1,
                                  BloombergLP::bael_Severity::BAEL_TRACE - 1,
                                  BloombergLP::bael_Severity::BAEL_TRACE - 1,
                                  BloombergLP::bael_Severity::BAEL_TRACE - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOGCB_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOGCB_TRACE(callback) << "message" << BAEL_LOGCB_END
                ASSERT(0 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOGCB_TRACE(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2] << BAEL_LOGCB_END
            ASSERT(1 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noDEBUG",
                                   BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                                   BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                                   BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                                   BloombergLP::bael_Severity::BAEL_DEBUG - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOGCB_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOGCB_DEBUG(callback) << "message" << BAEL_LOGCB_END
                ASSERT(1 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
           }

            const int LINE = L_ + 1;
            BAEL_LOGCB_DEBUG(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2] << BAEL_LOGCB_END
            ASSERT(2 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                    "noINFO",
                                    BloombergLP::bael_Severity::BAEL_INFO - 1,
                                    BloombergLP::bael_Severity::BAEL_INFO - 1,
                                    BloombergLP::bael_Severity::BAEL_INFO - 1,
                                    BloombergLP::bael_Severity::BAEL_INFO - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOGCB_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOGCB_INFO(callback) << "message" << BAEL_LOGCB_END
                ASSERT(2 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOGCB_INFO(callback) << "message" << SEP << ARGS[0]
                                      << SEP << ARGS[1]
                                      << SEP << ARGS[2] << BAEL_LOGCB_END
            ASSERT(3 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noWARN",
                                   BloombergLP::bael_Severity::BAEL_WARN - 1,
                                   BloombergLP::bael_Severity::BAEL_WARN - 1,
                                   BloombergLP::bael_Severity::BAEL_WARN - 1,
                                   BloombergLP::bael_Severity::BAEL_WARN - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOGCB_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOGCB_WARN(callback) << "message" << BAEL_LOGCB_END
                ASSERT(3 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOGCB_WARN(callback) << "message" << SEP << ARGS[0]
                                      << SEP << ARGS[1]
                                      << SEP << ARGS[2] << BAEL_LOGCB_END
            ASSERT(4 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noERROR",
                                   BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                   BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                   BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                   BloombergLP::bael_Severity::BAEL_ERROR - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOGCB_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                int numPublishedRecords = TO->numPublishedRecords();
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
                BAEL_LOGCB_ERROR(callback) << "message" << BAEL_LOGCB_END
                ASSERT(4 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOGCB_ERROR(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2] << BAEL_LOGCB_END
            ASSERT(5 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOGCB_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                int numPublishedRecords = TO->numPublishedRecords();
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
                BAEL_LOGCB_FATAL(callback) << "message" << BAEL_LOGCB_END
                ASSERT(5 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOGCB_FATAL(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2] << BAEL_LOGCB_END
            ASSERT(6 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE));
        }

        //callback.clear();

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'ostrstream' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longString = new char[N];
            char *p   = longString;
            char *end = longString + N;
            while (p < end) {
                *p++ = 'x';
            }
            *--p = '\0';

            {
                const int LINE = L_ + 1;
                BAEL_LOGCB_TRACE(callback) << longString << BAEL_LOGCB_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOGCB_DEBUG(callback) << longString << BAEL_LOGCB_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOGCB_INFO(callback) << longString << BAEL_LOGCB_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOGCB_WARN(callback) << longString << BAEL_LOGCB_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOGCB_ERROR(callback) << longString << BAEL_LOGCB_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOGCB_FATAL(callback) << longString << BAEL_LOGCB_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            delete [] longString;
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // STRESS TEST:
        //
        // Concerns:
        //   That logger is in good state after logging large number of
        //   messages.
        //
        // Plan:
        //   Initialize the logger manager with an observer that keeps a
        //   count of number of messages published to it.  Log a large
        //   number of messages with record level, log 1 message at
        //   trigger level and ensure that at least 1 message of record
        //   level is published.
        //
        // Testing:
        // --------------------------------------------------------------------

        using namespace BAEL_LOG_TEST_CASE_15;

        if (verbose)
            bsl::cout << bsl::endl << "STRESS TEST"
                      << bsl::endl << "===========" << bsl::endl;

        my_PublishCountingObserver observer;
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                 BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                 BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                 BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                 BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration);

        // int NUM_MESSAGES = bsl::atoi(argv[2]);
        enum { NUM_MESSAGES = 100000 };
        BAEL_LOG_SET_CATEGORY("TEST.CATEGORY");
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            BAEL_LOG_TRACE << "DUMMY MESSAGE" << BAEL_LOG_END;
        }
        BAEL_LOG_ERROR << "DUMMY MESSAGE" << BAEL_LOG_END;

        if (verbose) P(observer.publishCount());

        ASSERT(observer.publishCount() > 1); // because the triggering message
                                             // is alwayed published
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // BAEL_IS_ENABLED(SEVERITY) UTILITY MACRO TEST:
        //
        // Concerns:
        //   We are concerned that the macro can distinguish between severity
        //   levels that are and are not "enabled" for the category that is
        //   in effect in a given scope.
        //
        // Plan:
        //   Initialize the logger manager with a configuration such that
        //   'TRACE' is enabled (for "record", for definiteness) and verify
        //   that all of the six named severities are enabled.  Then
        //   reconfigure the threshold levels for the category in effect (using
        //   the 'setCategory' method) such that 'TRACE', 'DEBUG', and 'INFO'
        //   are not enabled, and confirm that only more severe levels are
        //   enabled.  Finally, reconfigure the threshold levels such that
        //   they are all 'OFF', and confirm that none of the severities is
        //   enabled.
        //
        // Testing:
        //   BAEL_IS_ENABLED(SEVERITY)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "BAEL_IS_ENABLED(SEVERITY) Utility MACRO TEST"
                      << bsl::endl
                      << "============================================"
                      << bsl::endl;

        BloombergLP::bael_DefaultObserver observer(bsl::cout);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        BloombergLP::bael_LoggerManager& manager =
            BloombergLP::bael_LoggerManager::singleton();

        {
            BAEL_LOG_SET_CATEGORY("TEST.CATEGORY");

            BAEL_LOG_TRACE << "This will load the category" << BAEL_LOG_END

            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_TRACE));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_DEBUG));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_INFO));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_WARN));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_ERROR));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_FATAL));

            manager.setCategory("TEST.CATEGORY",
                                BloombergLP::bael_Severity::BAEL_WARN,
                                BloombergLP::bael_Severity::BAEL_ERROR,
                                BloombergLP::bael_Severity::BAEL_FATAL,
                                BloombergLP::bael_Severity::BAEL_FATAL);

            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_TRACE));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_DEBUG));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_INFO));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_WARN));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_ERROR));
            ASSERT(BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_FATAL));

            manager.setCategory("TEST.CATEGORY",
                                BloombergLP::bael_Severity::BAEL_OFF,
                                BloombergLP::bael_Severity::BAEL_OFF,
                                BloombergLP::bael_Severity::BAEL_OFF,
                                BloombergLP::bael_Severity::BAEL_OFF);

            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_TRACE));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_DEBUG));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_INFO));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_WARN));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_ERROR));
            ASSERT(!BAEL_IS_ENABLED(BloombergLP::bael_Severity::BAEL_FATAL));

        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PRINTF MACRO PERFORMANCE TEST WITH 1 THREAD:
        //
        // Concerns:
        //   That when a single thread is logging messages using printf
        //   macro, the logging should be efficient when messages are
        //   not published to the observer.
        //
        // Plan:
        //   In a loop, log random size messages using printf macro.
        //   Measure the time to log all the messages.
        //
        // Testing:
        //   performance of printf macro with one thread
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "PRINTF MACRO PERFORMANCE TEST WITH 1 THREADS"
                      << bsl::endl
                      << "============================================"
                      << bsl::endl;

        using namespace BAEL_LOG_TEST_CASE_13;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        BloombergLP::bael_DefaultObserver observer(bsl::cout);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls_PlatformUtil::Int64 t =
                         BloombergLP::bsls_TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread13);
        t = BloombergLP::bsls_TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is chosen randomly from range 0 to "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with printf style macro = "
                 << t << " nanoseconds" << bsl::endl;
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PRINTF MACRO PERFORMANCE TEST WITH MULTIPLE THREADS:
        //
        // Concerns:
        //   That when multiple threads are logging messages (to the same
        //   logger) using printf macro, the logging should be efficient when
        //   messages are not published to the observer.
        //
        // Plan:
        //   create several threads, each, in a tight loop, logs random
        //   size messages using printf macro.  Finally join all the
        //   messages.  Measure the time to log all the messages.
        //
        // Testing:
        //   performance of printf macro with one thread
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "PRINTF MACRO PERFORMANCE TEST WITH MULTIPLE THREADS"
                      << bsl::endl
                      << "==================================================="
                      << bsl::endl;

        using namespace BAEL_LOG_TEST_CASE_12;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        BloombergLP::bael_DefaultObserver observer(bsl::cout);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
               BloombergLP::bael_Severity::BAEL_TRACE,  // record level
               BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
               BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
               BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls_PlatformUtil::Int64 t =
                          BloombergLP::bsls_TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread12);
        t = BloombergLP::bsls_TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is chosen randomly from range 0 to "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with printf style macro = "
                 << t << " nanoseconds" << bsl::endl;
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // C++ MACRO PERFORMANCE TEST WITH 1 THREAD:
        //
        // Concerns:
        //   That when a single thread is logging messages using c++
        //   macro, the logging should be efficient when messages are
        //   not published to the observer.
        //
        // Plan:
        //   In a loop, log random size messages using c++ macro.
        //   Measure the time to log all the messages.
        //
        // Testing:
        //   performance of c++ macro with one thread
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "C++ MACRO PERFORMANCE TEST WITH 1 THREADS"
                      << bsl::endl
                      << "========================================="
                      << bsl::endl;

        using namespace BAEL_LOG_TEST_CASE_11;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        BloombergLP::bael_DefaultObserver observer(bsl::cout);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls_PlatformUtil::Int64 t =
                      BloombergLP::bsls_TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread11);
        t = BloombergLP::bsls_TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is chosen randomly from range 0 to "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with c++ style macro = "
                 << t << " nanoseconds" << bsl::endl;
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // C++ MACRO PERFORMANCE TEST WITH MULTIPLE THREADS:
        //
        // Concerns:
        //   That when multiple threads are logging messages (to the same
        //   logger) using c++ macro, the logging should be efficient when
        //   messages are not published to the observer.
        //
        // Plan:
        //   create several threads, each, in a tight loop, logs random
        //   size messages using c++ macro.  Finally join all the
        //   messages.  Measure the time to log all the messages.
        //
        // Testing:
        //   performance of c++ macro with one thread
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "C++ MACRO PERFORMANCE TEST WITH MULTIPLE THREADS"
                      << bsl::endl
                      << "================================================"
                      << bsl::endl;

        using namespace BAEL_LOG_TEST_CASE_10;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        BloombergLP::bael_DefaultObserver observer(bsl::cout);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls_PlatformUtil::Int64 t =
                     BloombergLP::bsls_TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread10);
        t = BloombergLP::bsls_TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is chosen randomly from range 0 to "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with c++ style macro = "
                 << t << " nanoseconds" << bsl::endl;
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CONCURRENT LOGGING TEST:
        //   Verify the concurrent logging.
        //
        // Concerns:
        //   That multiple threads can concurrently log messages (with
        //   varying severities) to the same logger without introducing
        //   race conditions.
        //
        // Plan:
        //   Create several threads, each, in a tight loop, logs messages
        //   with varying severities.  Finally join all the threads and
        //   verify the state.
        //
        // Testing:
        //   Testing concurrent logging.
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "CONCURRENT LOGGING TEST:" << bsl::endl
                      << "========================" << bsl::endl;

        using namespace BAEL_LOG_TEST_CASE_9;

        ASSERT(EXP_N_TOTAL == EXP_N_RECORD + EXP_N_PUBLISH + EXP_N_TRIGGER);

        my_Observer observer;
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setLogOrder(
            BloombergLP::bael_LoggerManagerConfiguration::BAEL_FIFO);
        configuration.setDefaultThresholdLevelsIfValid(
               BloombergLP::bael_Severity::BAEL_TRACE,  // record level
               BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
               BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
               BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level
        configuration.setDefaultRecordBufferSizeIfValid(REC_BUF_LIMIT);
        configuration.setTriggerMarkers(
               BloombergLP::bael_LoggerManagerConfiguration::BAEL_NO_MARKERS);
        BloombergLP::bcema_TestAllocator ta(veryVeryVerbose);

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        executeInParallel(NUM_THREADS, workerThread9);
        if (veryVeryVerbose) observer.print();

        bsl::vector<bsl::pair<int, int> > vv[NUM_THREADS];
        // 'vv[i]' contains the message information for i'th thread

        const bsl::vector<bsl::string>& v = observer.get();
        ASSERT(v.size() == NUM_THREADS * EXP_N_TOTAL);
        // ignore any unconditional printing
        for (int i = 0; i < (int)v.size(); ++i) {
            bsl::istringstream is(v[i]);
            int id, type, iter;
            is >> id; is >> type; is >> iter;
            vv[id].push_back(bsl::pair<int, int> (type, iter));
        }

        for (int i = 0; i < NUM_THREADS; ++i) {
            bsl::vector<bsl::pair<int, int> >& v = vv[i];
            ASSERT(v.size() == EXP_N_TOTAL);
            int n_record = 0, n_publish = 0, n_trigger = 0;
            for(int j = 0; j < (int)v.size(); ++j) {
                if      (v[j].first == RECORD)  ++n_record;
                else if (v[j].first == PUBLISH) ++n_publish;
                else if (v[j].first == TRIGGER) ++n_trigger;
                else    ASSERT(0);
            }
            ASSERT(n_record  == EXP_N_RECORD);
            ASSERT(n_publish == EXP_N_PUBLISH);
            ASSERT(n_trigger == EXP_N_TRIGGER);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING THE FIFO LOG ORDER:
        //   Verify the FIFO log order.
        //
        // Concerns:
        //   That when the logger manager is configured with the FIFO
        //   (first in first out) log order, it publishes the logged
        //   messages in FIFO order.
        //
        // Plan:
        //   Create a logger manager configured with the default log
        //   order, log several messages to it and verify that they get
        //   published in the desired order.
        //
        // Testing:
        //   the FIFO log order
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING THE FIFO LOG ORDER" << bsl::endl
                      << "==========================" << bsl::endl;

        bsl::ostrstream os;
        BloombergLP::bael_DefaultObserver observer(os);
        BloombergLP::bael_LoggerManagerConfiguration configuration;

        // for simplicity we keep the passthrough level to be 'FATAL',
        // so that on trigger event, the message is published only once.
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_FATAL,  // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        BAEL_LOG_SET_CATEGORY("main category");
        char helloWorld1[] = "hello world 1";
        char helloWorld2[] = "hello world 2";
        char helloWorld3[] = "hello world 3";
        char helloWorld4[] = "hello world 4";

        BAEL_LOG_INFO <<  helloWorld1 << BAEL_LOG_END;
        BAEL_LOG_INFO <<  helloWorld2 << BAEL_LOG_END;
        BAEL_LOG_INFO <<  helloWorld3 << BAEL_LOG_END;
        BAEL_LOG_ERROR << helloWorld4 << BAEL_LOG_END;

        char *ptr4 = bsl::strstr(os.str(), helloWorld4); ASSERT(ptr4 != NULL);
        char *ptr3 = bsl::strstr(ptr4+1,   helloWorld3); ASSERT(ptr3 != NULL);
        char *ptr2 = bsl::strstr(ptr3+1,   helloWorld2); ASSERT(ptr2 != NULL);
        char *ptr1 = bsl::strstr(ptr2+1,   helloWorld1); ASSERT(ptr1 != NULL);

        ASSERT(ptr4  < ptr3);
        ASSERT(ptr3  < ptr2);
        ASSERT(ptr2  < ptr1);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING THE DEFAULT LOG ORDER (LIFO):
        //   Verify the default log order.
        //
        // Concerns:
        //   That when the logger manager is configured with the default
        //   log order (LIFO), it publishes the logged messages in LIFO
        //   (last in first out) order.
        //
        // Plan:
        //   Create a logger manager configured with the default log
        //   order, log several messages to it and verify that they get
        //   published in the desired order.
        //
        // Testing:
        //   the default log order
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING THE DEFAULT LOG ORDER (LIFO)" << bsl::endl
                      << "====================================" << bsl::endl;

        bsl::ostrstream os;
        BloombergLP::bael_DefaultObserver observer(os);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setLogOrder(
                  BloombergLP::bael_LoggerManagerConfiguration::BAEL_FIFO);

        // for simplicity we keep the passthrough level to be 'FATAL',
        // so that on trigger event, the message is published only once.
        configuration.setDefaultThresholdLevelsIfValid(
              BloombergLP::bael_Severity::BAEL_TRACE,  // record level
              BloombergLP::bael_Severity::BAEL_FATAL,  // passthrough level
              BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
              BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        BAEL_LOG_SET_CATEGORY("main category");
        char helloWorld1[] = "hello world 1";
        char helloWorld2[] = "hello world 2";
        char helloWorld3[] = "hello world 3";
        char helloWorld4[] = "hello world 4";

        BAEL_LOG_INFO  << helloWorld1 << BAEL_LOG_END;
        BAEL_LOG_INFO  << helloWorld2 << BAEL_LOG_END;
        BAEL_LOG_INFO  << helloWorld3 << BAEL_LOG_END;
        BAEL_LOG_ERROR << helloWorld4 << BAEL_LOG_END;

        char *ptr1 = bsl::strstr(os.str(), helloWorld1); ASSERT(ptr1  != NULL);
        char *ptr2 = bsl::strstr(ptr1+1,   helloWorld2); ASSERT(ptr2  != NULL);
        char *ptr3 = bsl::strstr(ptr2+1,   helloWorld3); ASSERT(ptr3  != NULL);
        char *ptr4 = bsl::strstr(ptr3+1,   helloWorld4); ASSERT(ptr4  != NULL);

        ASSERT(ptr1 < ptr2);
        ASSERT(ptr2 < ptr3);
        ASSERT(ptr3 < ptr4);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING THE C++ MACRO WHEN LOGGING RETURNED VALUE OF A FUNCTION:
        //   Verify the c++ macro when logging returned value of a function
        //   that internally log some other message using c++ macro.
        //
        // Concerns:
        //   Suppose a function 'f()' logs some data (say "message1")
        //   using c++ macro, and return some data (say "message2"), then
        //   'BAEL_LOG_SEVERITY << f() << BAEL_END' should result in
        //   logging "message1" followed by "message2".
        //
        // Plan:
        //   Invoke the function 'f()' described in Concern: section, and
        //   verify the order of logged messages.
        //
        // Testing:
        //   the c++ macro when logging returned value of a function.
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING THE C++ MACRO WHEN LOGGING "
                      << "RETURNED VALUE OF A FUNCTION"         << bsl::endl
                      << "====================================" << bsl::endl;

        using namespace BAEL_LOG_TEST_CASE_6;

        bsl::ostrstream os;
        BloombergLP::bael_DefaultObserver observer(os);
        BloombergLP::bael_LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::bael_Severity::BAEL_TRACE,  // record level
                  BloombergLP::bael_Severity::BAEL_WARN,   // passthrough level
                  BloombergLP::bael_Severity::BAEL_ERROR,  // trigger level
                  BloombergLP::bael_Severity::BAEL_FATAL); // triggerAll level

        BloombergLP::bael_LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        BAEL_LOG_SET_CATEGORY("main category");

        BAEL_LOG_WARN << f() << BAEL_LOG_END;
        if (verbose)  bsl::cout << os.str() << bsl::endl;
        char *msg1   = bsl::strstr(os.str(), message1); ASSERT(msg1 != NULL);
        char *msg2   = bsl::strstr(msg1+1,   message2); ASSERT(msg2 != NULL);

        ASSERT(msg1 < msg2);
      }break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
        //
        // Concerns:
        //   A logging macros must be safe against being invoked before the
        //   logger manager is instantiated, and it must be safe to use the
        //   bael macros after the singleton has been destroyed.
        //
        // Plan:
        //   Invoke the macro and confirm that the process continues normally.
        //   An object with BAEL macro calls in the d'tor is created before the
        //   bael singleton is created, so those macro calls happen after the
        //   singleton is destroyed, testing that the macros will work with a
        //   destroyed singleton.
        //
        // Testing:
        //   BAEL_LOG_TRACE
        //   BAEL_LOG_DEBUG
        //   BAEL_LOG_INFO
        //   BAEL_LOG_WARN
        //   BAEL_LOG_ERROR
        //   BAEL_LOG_FATAL
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS
        bsl::cout << "TEST 5 TEMPORARILY SUPPRESSED ON WINDOWS -- MUST FIX\n";
#else
        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing macro safety w/o LoggerManager" << bsl::endl
                      << "======================================" << bsl::endl;

        class LogOnDestruction {
          public:
            LogOnDestruction()
            {
            }
            ~LogOnDestruction()
            {
                if (verbose) bsl::cout << "Entered ~LogOnDestruction\n";

#ifdef BSLS_PLATFORM__OS_UNIX
                // temporarily reroute stderr to a temp file
                fflush(stderr);
                bsl::string filename = tempnam(0, "bael_log");
                int fd = creat(filename.c_str(), 0777);
                ASSERT(fd != -1);
                int saved_stderr_fd = dup(2);
                dup2(fd, 2);
                if (verbose) bsl::cout << "STDERR redirected to " << filename
                                       << bsl::endl;
#endif

                ASSERT(false == bael_LoggerManager::isInitialized());
                BAEL_LOG_SET_CATEGORY("LoggerManagerDestroyed");
                if (verbose)
                    bsl::cout << "Safely invoked 'BAEL_LOG_SET_CATEGORY' macro"
                              << bsl::endl;

                BAEL_LOG_TRACE << "No Logger Manager!" << BAEL_LOG_END;
                BAEL_LOG_DEBUG << "No Logger Manager!" << BAEL_LOG_END;
                BAEL_LOG_INFO  << "No Logger Manager!" << BAEL_LOG_END;
                BAEL_LOG_WARN  << "No Logger Manager!" << BAEL_LOG_END;
                BAEL_LOG_ERROR << "No Logger Manager!" << BAEL_LOG_END;
                BAEL_LOG_FATAL << "No Logger Manager!" << BAEL_LOG_END;

                if (verbose)
                    bsl::cout << "Safely invoked stream-style macros"
                              << bsl::endl;

                BloombergLP::bael_Severity::Level severity =
                                  BloombergLP::bael_Severity::BAEL_FATAL;

                BAEL_LOG0(severity, "Hello!");
                BAEL_LOG1(severity, "Hello!", 1);
                BAEL_LOG2(severity, "Hello!", 1, 2);
                BAEL_LOG3(severity, "Hello!", 1, 2, 3);
                BAEL_LOG4(severity, "Hello!", 1, 2, 3, 4);
                BAEL_LOG5(severity, "Hello!", 1, 2, 3, 4, 5);
                BAEL_LOG6(severity, "Hello!", 1, 2, 3, 4, 5, 6);
                BAEL_LOG7(severity, "Hello!", 1, 2, 3, 4, 5, 6, 7);
                BAEL_LOG8(severity, "Hello!", 1, 2, 3, 4, 5, 6, 7, 8);
                BAEL_LOG9(severity, "Hello!", 1, 2, 3, 4, 5, 6, 7, 8, 9);

                if (verbose) bsl::cout <<
                                        "Safely invoked printf-style macros\n";

#ifdef BSLS_PLATFORM__OS_UNIX
                // restore stderr to the state it was in before we rerouted it.
                fflush(stderr);
                dup2(saved_stderr_fd, 2);
                if (verbose) bsl::cout << "STDERR redirected back to normal\n";

                // verify 26 lines were written to the temp file
                bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
                int numLines = 0;
                bsl::string line;
                while (getline(fs, line)) {
                    ++numLines;
                    if (veryVerbose) bsl::cout << "\t>>" << line << "<<\n";
                }
                fs.close();
                ASSERT(26 == numLines);

                unlink(filename.c_str());
#endif
            }
        };

        // This static object is created before the bael singleton is created,
        // so it will be destroyed AFTER the singleton is destroyed, so the
        // macro calls in the d'tor will be called after the singleton is
        // destroyed.
        static LogOnDestruction logOnDestruction;

        BAEL_LOG_SET_CATEGORY("ThereIsNoLoggerManager");

        if (verbose)
            bsl::cout << "Safely invoked 'BAEL_LOG_SET_CATEGORY' macro"
                      << bsl::endl;

#ifdef BSLS_PLATFORM__OS_UNIX
        // temporarily reroute stderr to a temp file
        fflush(stderr);
        bsl::string filename = tempnam(0, "bael_log");
        int fd = creat(filename.c_str(), 0777);
        ASSERT(fd != -1);
        int saved_stderr_fd = dup(2);
        dup2(fd, 2);
        if (verbose)
                 bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

        BAEL_LOG_TRACE << "No Logger Manager!" << BAEL_LOG_END;
        BAEL_LOG_DEBUG << "No Logger Manager!" << BAEL_LOG_END;
        BAEL_LOG_INFO  << "No Logger Manager!" << BAEL_LOG_END;
        BAEL_LOG_WARN  << "No Logger Manager!" << BAEL_LOG_END;
        BAEL_LOG_ERROR << "No Logger Manager!" << BAEL_LOG_END;
        BAEL_LOG_FATAL << "No Logger Manager!" << BAEL_LOG_END;

        if (verbose)
            bsl::cout << "Safely invoked stream-style macros" << bsl::endl;

        BloombergLP::bael_Severity::Level severity =
            BloombergLP::bael_Severity::BAEL_FATAL;

        scribbleBuffer();
        BAEL_LOG0(severity, "Hello!");
        ASSERT(0 == isBufferScribbled());
        BAEL_LOG1(severity, "Hello!", 1);
        BAEL_LOG2(severity, "Hello!", 1, 2);
        BAEL_LOG3(severity, "Hello!", 1, 2, 3);
        BAEL_LOG4(severity, "Hello!", 1, 2, 3, 4);
        BAEL_LOG5(severity, "Hello!", 1, 2, 3, 4, 5);
        BAEL_LOG6(severity, "Hello!", 1, 2, 3, 4, 5, 6);
        BAEL_LOG7(severity, "Hello!", 1, 2, 3, 4, 5, 6, 7);
        BAEL_LOG8(severity, "Hello!", 1, 2, 3, 4, 5, 6, 7, 8);
        BAEL_LOG9(severity, "Hello!", 1, 2, 3, 4, 5, 6, 7, 8, 9);

        if (verbose)
            bsl::cout << "Safely invoked printf-style macros" << bsl::endl;

#ifdef BSLS_PLATFORM__OS_UNIX
        // restore stderr to the state it was in before we rerouted it.
        fflush(stderr);
        dup2(saved_stderr_fd, 2);
        if (verbose) bsl::cout << "STDERR redirected back to normal\n";

        // verify 26 lines were written to the temp file
        bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
        int numLines = 0;
        bsl::string line;
        while (getline(fs, line)) {
            ++numLines;
            if (veryVerbose) bsl::cout << "\t>>" << line << "<<" << bsl::endl;
        }
        fs.close();
        ASSERT(26 == numLines);

        unlink(filename.c_str());
#endif

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc);
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OSTRSTREAM MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   BAEL_LOG_TRACE
        //   BAEL_LOG_DEBUG
        //   BAEL_LOG_INFO
        //   BAEL_LOG_WARN
        //   BAEL_LOG_ERROR
        //   BAEL_LOG_FATAL
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing 'ostrstream' Macros" << bsl::endl
                               << "===========================" << bsl::endl;

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::bael_Administration::addCategory(
                                "sieve",
                                BloombergLP::bael_Severity::BAEL_TRACE,
                                BloombergLP::bael_Severity::BAEL_TRACE,
                                0,
                                0);
        BAEL_LOG_SET_CATEGORY("sieve")

        BAEL_LOG_TRACE << "This will load the category" << BAEL_LOG_END

        const Cat  *CAT  = BAEL_LOG_CATEGORY;
        const char *FILE = __FILE__;

        const int TRACE = BloombergLP::bael_Severity::BAEL_TRACE;
        const int DEBUG = BloombergLP::bael_Severity::BAEL_DEBUG;
        const int INFO  = BloombergLP::bael_Severity::BAEL_INFO;
        const int WARN  = BloombergLP::bael_Severity::BAEL_WARN;
        const int ERROR = BloombergLP::bael_Severity::BAEL_ERROR;
        const int FATAL = BloombergLP::bael_Severity::BAEL_FATAL;

        BloombergLP::bael_Administration::addCategory(
                           "noTRACE",
                           BloombergLP::bael_Severity::BAEL_TRACE - 1,
                           BloombergLP::bael_Severity::BAEL_TRACE - 1,
                           BloombergLP::bael_Severity::BAEL_TRACE - 1,
                           BloombergLP::bael_Severity::BAEL_TRACE - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOG_TRACE << "message" << BAEL_LOG_END
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOG_TRACE << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2] << BAEL_LOG_END
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                              "noDEBUG",
                              BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                              BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                              BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                              BloombergLP::bael_Severity::BAEL_DEBUG - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOG_DEBUG << "message" << BAEL_LOG_END
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOG_DEBUG << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2] << BAEL_LOG_END
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                               "noINFO",
                               BloombergLP::bael_Severity::BAEL_INFO - 1,
                               BloombergLP::bael_Severity::BAEL_INFO - 1,
                               BloombergLP::bael_Severity::BAEL_INFO - 1,
                               BloombergLP::bael_Severity::BAEL_INFO - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOG_INFO << "message" << BAEL_LOG_END
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOG_INFO << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2] << BAEL_LOG_END
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                  "noWARN",
                                  BloombergLP::bael_Severity::BAEL_WARN - 1,
                                  BloombergLP::bael_Severity::BAEL_WARN - 1,
                                  BloombergLP::bael_Severity::BAEL_WARN - 1,
                                  BloombergLP::bael_Severity::BAEL_WARN - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOG_WARN << "message" << BAEL_LOG_END
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOG_WARN << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2] << BAEL_LOG_END
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                "noERROR",
                                BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                BloombergLP::bael_Severity::BAEL_ERROR - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOG_ERROR << "message" << BAEL_LOG_END
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOG_ERROR << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2] << BAEL_LOG_END
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                int numPublishedRecords = TO->numPublishedRecords();
                BAEL_LOG_FATAL << "message" << BAEL_LOG_END
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BAEL_LOG_FATAL << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2] << BAEL_LOG_END
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'ostrstream' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longString = new char[N];
            char *p   = longString;
            char *end = longString + N;
            while (p < end) {
                *p++ = 'x';
            }
            *--p = '\0';

            {
                const int LINE = L_ + 1;
                BAEL_LOG_TRACE << longString << BAEL_LOG_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG_DEBUG << longString << BAEL_LOG_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG_INFO << longString << BAEL_LOG_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG_WARN << longString << BAEL_LOG_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG_ERROR << longString << BAEL_LOG_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG_FATAL << longString << BAEL_LOG_END
                // longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            delete [] longString;
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRINTF-STYLE MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   BAEL_LOG[0-9]
        //   BAEL_LOG[0-9]_TRACE
        //   BAEL_LOG[0-9]_DEBUG
        //   BAEL_LOG[0-9]_INFO
        //   BAEL_LOG[0-9]_WARN
        //   BAEL_LOG[0-9]_ERROR
        //   BAEL_LOG[0-9]_FATAL
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing 'printf-style' Macros" << bsl::endl
                               << "=============================" << bsl::endl;

        static bslma_TestAllocator testAllocator(veryVerbose);
        static bslma_DefaultAllocatorGuard taGuard(&testAllocator);

        const int MAX_ARGS = 9;

        const char *FORMAT[] = {
            "message",
            "message:%d",
            "message:%d:%d",
            "message:%d:%d:%d",
            "message:%d:%d:%d:%d",
            "message:%d:%d:%d:%d:%d",
            "message:%d:%d:%d:%d:%d:%d",
            "message:%d:%d:%d:%d:%d:%d:%d",
            "message:%d:%d:%d:%d:%d:%d:%d:%d",
            "message:%d:%d:%d:%d:%d:%d:%d:%d:%d"
        };
        ASSERT(MAX_ARGS + 1 == sizeof FORMAT / sizeof *FORMAT);

        const char *MESSAGE[] = {
            "message",
            "message:1",
            "message:1:2",
            "message:1:2:3",
            "message:1:2:3:4",
            "message:1:2:3:4:5",
            "message:1:2:3:4:5:6",
            "message:1:2:3:4:5:6:7",
            "message:1:2:3:4:5:6:7:8",
            "message:1:2:3:4:5:6:7:8:9"
        };
        ASSERT(MAX_ARGS + 1 == sizeof MESSAGE / sizeof *MESSAGE);

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

        const int TRACE = BloombergLP::bael_Severity::BAEL_TRACE;
        const int DEBUG = BloombergLP::bael_Severity::BAEL_DEBUG;
        const int INFO  = BloombergLP::bael_Severity::BAEL_INFO;
        const int WARN  = BloombergLP::bael_Severity::BAEL_WARN;
        const int ERROR = BloombergLP::bael_Severity::BAEL_ERROR;
        const int FATAL = BloombergLP::bael_Severity::BAEL_FATAL;

        if (veryVerbose) bsl::cout << "\tTesting exception safety"
                                   << bsl::endl;
        {
            BloombergLP::bael_Administration::addCategory(
                                             "pass",
                                             0,
                                             TRACE,
                                             0,
                                             0);

            BAEL_LOG_SET_CATEGORY("pass")

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG0"
                                           << bsl::endl;

            BAEL_LOG0(TRACE, FORMAT[0]);
            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG0(TRACE, FORMAT[0]);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG1"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG1(TRACE, FORMAT[1], 1);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG2"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG2(TRACE, FORMAT[2], 1, 2);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG3"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG3(TRACE, FORMAT[3], 1, 2, 3);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG4"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG4(TRACE, FORMAT[4], 1, 2, 3, 4);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG5"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG5(TRACE, FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG6"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG6(TRACE, FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG7"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG7(TRACE, FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG8"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG8(TRACE, FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BAEL_LOG9"
                                           << bsl::endl;

            BEGIN_BSLMA_EXCEPTION_TEST
            scribbleBuffer();
            BAEL_LOG9(TRACE, FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(0 == isBufferScribbled());
            END_BSLMA_EXCEPTION_TEST
        }

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::bael_Administration::addCategory(
                                             "sieve",
                                             TRACE,
                                             TRACE,
                                             0,
                                             0);
        BAEL_LOG_SET_CATEGORY("sieve")

        BAEL_LOG_TRACE << "This will load the category" << BAEL_LOG_END

        const Cat  *CAT  = BAEL_LOG_CATEGORY;
        const char *FILE = __FILE__;

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG0'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG0(255, FORMAT[0]);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG0(INFO, FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG1'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG1(255, FORMAT[1], 1);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG1(INFO, FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG2'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG2(255, FORMAT[2], 1, 2);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG2(INFO, FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG3'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG3(255, FORMAT[3], 1, 2, 3);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG3(INFO, FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG4'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG4(255, FORMAT[4], 1, 2, 3, 4);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG4(INFO, FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG5'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG5(255, FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG5(INFO, FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG6'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG6(255, FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG6(INFO, FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG7'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG7(255, FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG7(INFO, FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG8'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG8(255, FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG8(INFO, FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BAEL_LOG9'" << bsl::endl;
        {
            scribbleBuffer();
            BAEL_LOG9(255, FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(1 == isBufferScribbled());
            const int LINE = L_ + 1;
            BAEL_LOG9(INFO, FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[9]));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noTRACE",
                                   BloombergLP::bael_Severity::BAEL_TRACE - 1,
                                   BloombergLP::bael_Severity::BAEL_TRACE - 1,
                                   BloombergLP::bael_Severity::BAEL_TRACE - 1,
                                   BloombergLP::bael_Severity::BAEL_TRACE - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG0_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG0_TRACE(FORMAT[0]);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG0_TRACE(FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG1_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG1_TRACE(FORMAT[1], 1);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG1_TRACE(FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG2_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG2_TRACE(FORMAT[2], 1, 2);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG2_TRACE(FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG3_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG3_TRACE(FORMAT[3], 1, 2, 3);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG3_TRACE(FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG4_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG4_TRACE(FORMAT[4], 1, 2, 3, 4);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG4_TRACE(FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG5_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG5_TRACE(FORMAT[5], 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG5_TRACE(FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG6_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG6_TRACE(FORMAT[6], 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG6_TRACE(FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG7_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG7_TRACE(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG7_TRACE(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG8_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG8_TRACE(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG8_TRACE(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG9_TRACE'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BAEL_LOG9_TRACE(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG9_TRACE(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[9]));
        }

        BloombergLP::bael_Administration::addCategory(
                                  "noDEBUG",
                                  BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                                  BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                                  BloombergLP::bael_Severity::BAEL_DEBUG - 1,
                                  BloombergLP::bael_Severity::BAEL_DEBUG - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG0_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG0_DEBUG(FORMAT[0]);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG0_DEBUG(FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG1_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG1_DEBUG(FORMAT[1], 1);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG1_DEBUG(FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG2_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG2_DEBUG(FORMAT[2], 1, 2);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG2_DEBUG(FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG3_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG3_DEBUG(FORMAT[3], 1, 2, 3);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG3_DEBUG(FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG4_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG4_DEBUG(FORMAT[4], 1, 2, 3, 4);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG4_DEBUG(FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG5_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG5_DEBUG(FORMAT[5], 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG5_DEBUG(FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG6_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG6_DEBUG(FORMAT[6], 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG6_DEBUG(FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG7_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG7_DEBUG(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG7_DEBUG(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG8_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG8_DEBUG(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG8_DEBUG(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG9_DEBUG'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BAEL_LOG9_DEBUG(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG9_DEBUG(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[9]));
        }

        BloombergLP::bael_Administration::addCategory(
                                  "noINFO",
                                  BloombergLP::bael_Severity::BAEL_INFO - 1,
                                  BloombergLP::bael_Severity::BAEL_INFO - 1,
                                  BloombergLP::bael_Severity::BAEL_INFO - 1,
                                  BloombergLP::bael_Severity::BAEL_INFO - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG0_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG0_INFO(FORMAT[0]);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG0_INFO(FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG1_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG1_INFO(FORMAT[1], 1);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG1_INFO(FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG2_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG2_INFO(FORMAT[2], 1, 2);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG2_INFO(FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG3_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG3_INFO(FORMAT[3], 1, 2, 3);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG3_INFO(FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG4_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG4_INFO(FORMAT[4], 1, 2, 3, 4);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG4_INFO(FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG5_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG5_INFO(FORMAT[5], 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG5_INFO(FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG6_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG6_INFO(FORMAT[6], 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG6_INFO(FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG7_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG7_INFO(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG7_INFO(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG8_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG8_INFO(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG8_INFO(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG9_INFO'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BAEL_LOG9_INFO(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG9_INFO(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[9]));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noWARN",
                                   BloombergLP::bael_Severity::BAEL_WARN - 1,
                                   BloombergLP::bael_Severity::BAEL_WARN - 1,
                                   BloombergLP::bael_Severity::BAEL_WARN - 1,
                                   BloombergLP::bael_Severity::BAEL_WARN - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG0_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG0_WARN(FORMAT[0]);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG0_WARN(FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG1_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG1_WARN(FORMAT[1], 1);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG1_WARN(FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG2_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG2_WARN(FORMAT[2], 1, 2);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG2_WARN(FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG3_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG3_WARN(FORMAT[3], 1, 2, 3);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG3_WARN(FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG4_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG4_WARN(FORMAT[4], 1, 2, 3, 4);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG4_WARN(FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG5_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG5_WARN(FORMAT[5], 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG5_WARN(FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG6_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG6_WARN(FORMAT[6], 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG6_WARN(FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG7_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG7_WARN(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG7_WARN(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG8_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG8_WARN(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG8_WARN(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG9_WARN'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BAEL_LOG9_WARN(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG9_WARN(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[9]));
        }

        BloombergLP::bael_Administration::addCategory(
                                  "noERROR",
                                  BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                  BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                  BloombergLP::bael_Severity::BAEL_ERROR - 1,
                                  BloombergLP::bael_Severity::BAEL_ERROR - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG0_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG0_ERROR(FORMAT[0]);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG0_ERROR(FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG1_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG1_ERROR(FORMAT[1], 1);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG1_ERROR(FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG2_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG2_ERROR(FORMAT[2], 1, 2);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG2_ERROR(FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG3_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG3_ERROR(FORMAT[3], 1, 2, 3);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG3_ERROR(FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG4_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG4_ERROR(FORMAT[4], 1, 2, 3, 4);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG4_ERROR(FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG5_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG5_ERROR(FORMAT[5], 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG5_ERROR(FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG6_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG6_ERROR(FORMAT[6], 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG6_ERROR(FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG7_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG7_ERROR(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG7_ERROR(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG8_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG8_ERROR(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG8_ERROR(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG9_ERROR'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BAEL_LOG9_ERROR(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG9_ERROR(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[9]));
        }

        BloombergLP::bael_Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1,
                                   BloombergLP::bael_Severity::BAEL_FATAL - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG0_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG0_FATAL(FORMAT[0]);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG0_FATAL(FORMAT[0]);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG1_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG1_FATAL(FORMAT[1], 1);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG1_FATAL(FORMAT[1], 1);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG2_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG2_FATAL(FORMAT[2], 1, 2);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG2_FATAL(FORMAT[2], 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG3_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG3_FATAL(FORMAT[3], 1, 2, 3);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG3_FATAL(FORMAT[3], 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG4_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG4_FATAL(FORMAT[4], 1, 2, 3, 4);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG4_FATAL(FORMAT[4], 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG5_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG5_FATAL(FORMAT[5], 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG5_FATAL(FORMAT[5], 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG6_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG6_FATAL(FORMAT[6], 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG6_FATAL(FORMAT[6], 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG7_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG7_FATAL(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG7_FATAL(FORMAT[7], 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG8_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG8_FATAL(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG8_FATAL(FORMAT[8], 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BAEL_LOG9_FATAL'" << bsl::endl;
        {
            {
                BAEL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BAEL_LOG9_FATAL(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferScribbled());
            }

            const int LINE = L_ + 1;
            BAEL_LOG9_FATAL(FORMAT[9], 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'printf' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longString = new char[N];
            char *p   = longString;
            char *end = longString + N;
            while (p < end) {
                *p++ = 'x';
            }
            *--p = '\0';

            {
                longString[BUFLEN - 2] = 'a';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0(DEBUG, longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'b';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1(DEBUG, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'c';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2(DEBUG, "%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'd';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3(DEBUG, "%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'e';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4(DEBUG, "%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'f';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5(DEBUG, "%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'g';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6(DEBUG, "%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'h';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7(DEBUG, "%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'i';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8(DEBUG, "%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9(DEBUG, "%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0_TRACE(longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1_TRACE("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'm';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2_TRACE("%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'n';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3_TRACE("%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'o';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4_TRACE("%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'p';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5_TRACE("%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'q';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6_TRACE("%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'r';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7_TRACE("%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 's';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8_TRACE("%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 't';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9_TRACE("%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'u';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0_DEBUG(longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'v';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1_DEBUG("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'w';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2_DEBUG("%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'x';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3_DEBUG("%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'y';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4_DEBUG("%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'z';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5_DEBUG("%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'A';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6_DEBUG("%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'B';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7_DEBUG("%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'C';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8_DEBUG("%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'D';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9_DEBUG("%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'E';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0_INFO(longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'F';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1_INFO("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'G';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2_INFO("%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'H';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3_INFO("%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'I';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4_INFO("%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'J';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5_INFO("%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'K';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6_INFO("%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'L';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7_INFO("%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'M';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8_INFO("%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'N';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9_INFO("%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'O';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0_WARN(longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'P';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1_WARN("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'Q';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2_WARN("%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'R';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3_WARN("%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'S';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4_WARN("%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'T';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5_WARN("%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'U';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6_WARN("%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'V';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7_WARN("%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'W';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8_WARN("%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'X';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9_WARN("%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'Y';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0_ERROR(longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'Z';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1_ERROR("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '0';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2_ERROR("%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '1';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3_ERROR("%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '2';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4_ERROR("%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '3';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5_ERROR("%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '4';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6_ERROR("%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '5';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7_ERROR("%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '6';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8_ERROR("%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '7';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9_ERROR("%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '8';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG0_FATAL(longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '9';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG1_FATAL("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'a';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG2_FATAL("%s", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'b';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG3_FATAL("%s", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'c';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG4_FATAL("%s", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'd';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG5_FATAL("%s", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'e';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG6_FATAL("%s", longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'f';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG7_FATAL("%s", longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'g';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG8_FATAL("%s", longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'h';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BAEL_LOG9_FATAL("%s", longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            delete [] longString;
        }

        // Note that the following is expressly meant to test the modifications
        // made to the 'printf'-style macros to use the 'do { ... } while(0)'
        // idiom (DRQS 13261698).  In particular, the 'if' statements are
        // *INTENTIONALLY* *NOT* fully bracketed ('{}'-enclosed), contrary to
        // the BDE coding standard.

        if (veryVerbose)
            bsl::cout << "\tTesting macros in unbracketed context."
                      << bsl::endl;
        {
            int unbracketedLoggingFlag = 1;

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BAEL_LOG0_TRACE(FORMAT[0]);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[0]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BAEL_LOG1_DEBUG(FORMAT[1], 1);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[1]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BAEL_LOG2_INFO(FORMAT[2], 1, 2);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BAEL_LOG3_WARN(FORMAT[3], 1, 2, 3);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[3]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BAEL_LOG4_ERROR(FORMAT[4], 1, 2, 3, 4);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[4]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BAEL_LOG5_FATAL(FORMAT[5], 1, 2, 3, 4, 5);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[5]));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BAEL_LOG_* MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   BAEL_LOG_SET_CATEGORY
        //   BAEL_LOG_CATEGORY
        //   BAEL_LOG_THRESHOLD
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
            << "Testing BAEL_LOG_* Macros" << bsl::endl
            << "=========================" << bsl::endl;
        }

        ASSERT(0 == bsl::strcmp(__FILE__, __FILE__));

        // Sanity check BAEL_LOG_CATEGORY & BAEL_LOG_THRESHOLD
        {
            const int UC = Holder::BAEL_UNINITIALIZED_CATEGORY;
            const int DC = Holder::BAEL_DYNAMIC_CATEGORY;

            struct {
                const int  d_line;
                const int  d_threshold;
                const Cat *d_category;
            } DATA[] = {
                // Line           Threshold      Category
                // ----           ---------      --------
                { L_,                0,        (Cat *)  0    },
                { L_,                1,        (Cat *)  0    },
                { L_,                1,        (Cat *) 32    },
                { L_,               UC,        (Cat *) 32    },
                { L_,               DC,        (Cat *) 64    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int  LINE      = DATA[i].d_line;
                const int  THRESHOLD = DATA[i].d_threshold;
                const Cat *CATEGORY  = DATA[i].d_category;

                Holder BAEL_LOG_CATEGORYHOLDER = {
                    THRESHOLD, CATEGORY, 0
                };

                LOOP3_ASSERT(LINE, THRESHOLD, BAEL_LOG_THRESHOLD,
                             THRESHOLD == BAEL_LOG_THRESHOLD);
                LOOP3_ASSERT(LINE, CATEGORY, BAEL_LOG_CATEGORY,
                             CATEGORY == BAEL_LOG_CATEGORY);
            }
        }

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc, &ta);

        {
            const char *CATEGORY_NAME1 = "EQUITY.NASD";
            BAEL_LOG_SET_CATEGORY(CATEGORY_NAME1)  // creates new category

            const Cat  *CATEGORY1  = BAEL_LOG_CATEGORYHOLDER.category();
            const int   MAX_LEVEL1 = CATEGORY1->maxLevel();

            LOOP2_ASSERT(CATEGORY_NAME1, CATEGORY1->categoryName(),
                         0 == bsl::strcmp(CATEGORY_NAME1,
                                          CATEGORY1->categoryName()));
            LOOP2_ASSERT(MAX_LEVEL1, BAEL_LOG_CATEGORYHOLDER.threshold(),
                         MAX_LEVEL1 == BAEL_LOG_CATEGORYHOLDER.threshold());

            BAEL_LOG_TRACE << "This will load the category" << BAEL_LOG_END

            BloombergLP::bael_Administration::setMaxNumCategories(3);
            ASSERT(3 == BloombergLP::bael_Administration::maxNumCategories());
            {
                const char *CATEGORY_NAME2 = "EQUITY.NYSE";
                BAEL_LOG_SET_CATEGORY(CATEGORY_NAME2)

                const Cat  *CATEGORY2  = BAEL_LOG_CATEGORYHOLDER.category();
                const int   MAX_LEVEL2 = CATEGORY2->maxLevel();

                LOOP2_ASSERT(CATEGORY_NAME2, CATEGORY2->categoryName(),
                             0 == bsl::strcmp(CATEGORY_NAME2,
                                          CATEGORY2->categoryName()));
                LOOP2_ASSERT(MAX_LEVEL2, BAEL_LOG_CATEGORYHOLDER.threshold(),
                            MAX_LEVEL2 == BAEL_LOG_CATEGORYHOLDER.threshold());
                ASSERT(0 == bsl::strcmp(CATEGORY_NAME2,
                                        BAEL_LOG_CATEGORY->categoryName()));
                ASSERT(0 != bsl::strcmp(CATEGORY_NAME1,
                                        BAEL_LOG_CATEGORY->categoryName()));

                BAEL_LOG_TRACE << "This will load the category"
                               << BAEL_LOG_END

                {
                    const char *CATEGORY_NAME3 = "EQUITY.DOW";
                    BAEL_LOG_SET_CATEGORY(CATEGORY_NAME3)

                    const Cat *CATEGORY3  = BAEL_LOG_CATEGORYHOLDER.category();
                    const int  MAX_LEVEL3 = CATEGORY3->maxLevel();

                    LOOP2_ASSERT(CATEGORY_NAME3, CATEGORY3->categoryName(),
                                 0 != bsl::strcmp(CATEGORY_NAME3,
                                                  CATEGORY3->categoryName()));
                    LOOP2_ASSERT(MAX_LEVEL3,
                            BAEL_LOG_CATEGORYHOLDER.threshold(),
                            MAX_LEVEL3 == BAEL_LOG_CATEGORYHOLDER.threshold());
                    ASSERT(0 != bsl::strcmp(CATEGORY_NAME3,
                                           BAEL_LOG_CATEGORY->categoryName()));
                    ASSERT(0 != bsl::strcmp(CATEGORY_NAME2,
                                           BAEL_LOG_CATEGORY->categoryName()));
                    ASSERT(0 != bsl::strcmp(CATEGORY_NAME1,
                                           BAEL_LOG_CATEGORY->categoryName()));

                    BAEL_LOG_TRACE << "This will load the default category"
                                   << BAEL_LOG_END
                }

                ASSERT(0 == bsl::strcmp(CATEGORY_NAME2,
                                        BAEL_LOG_CATEGORY->categoryName()));

            }

            ASSERT(0 == bsl::strcmp(CATEGORY_NAME1,
                                    BAEL_LOG_CATEGORY->categoryName()));
        }
        for (int i = 0; i < 2; ++i) {
            BloombergLP::bael_Administration::setMaxNumCategories(4 + i);

            bsl::ostringstream os;
            os << "CAT" << i;

            bsl::string cat1 = os.str();
            os.str("");

            BAEL_LOG_SET_DYNAMIC_CATEGORY(cat1.c_str());// creates new category
            ASSERT(0 == bsl::strcmp(cat1.c_str(),
                                    BAEL_LOG_CATEGORY->categoryName()));

            {
                os << "CAT.NESTED" << i;
                bsl::string cat2 = os.str();
                os.str("");

                BAEL_LOG_SET_DYNAMIC_CATEGORY(cat2.c_str()); // get default
                ASSERT(0 != bsl::strcmp(cat1.c_str(),
                                        BAEL_LOG_CATEGORY->categoryName()));
                ASSERT(0 != bsl::strcmp(cat2.c_str(),
                                        BAEL_LOG_CATEGORY->categoryName()));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING UTILITY FUNCTIONS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //   (1) Verify that 'messageBuffer' returns non-null and that
        //       'messageBufferSize' bytes may be overwritten
        //
        // Testing:
        //   static void logMessage(*category, severity, *file, line, *msg);
        //   static char *messageBuffer();
        //   static int messageBufferSize();
        //   static const bael_Category *setCategory(Holder *, const char *);
        //   static const bael_Category *setCategory(const char *categoryName);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl << "Testing Utility Functions"
                               << bsl::endl << "========================="
                               << bsl::endl;

        using namespace BloombergLP;

        BloombergLP::bael_LoggerManagerConfiguration lmc;
        BloombergLP::bael_LoggerManagerScopedGuard lmg(TO, lmc);
        BloombergLP::bael_LoggerManager& lm =
            BloombergLP::bael_LoggerManager::singleton();

        if (veryVerbose) {
            bsl::cout << "\tTesting 'messageBuffer' and 'messageBufferSize'"
                      << bsl::endl;
        }

        if (veryVerbose) bsl::cout << "\tTesting 'setCategory'" << bsl::endl;
        {
            lm.setDefaultThresholdLevels(192, 96, 64, 32);
            const bael_Category *category;
            category = bael_Log::setCategory("EQUITY.NASD");  // creates new
                                                              // category
            ASSERT(0 == bsl::strcmp("EQUITY.NASD", category->categoryName()));

            bael_Administration::setMaxNumCategories(2);
            ASSERT(2 == bael_Administration::maxNumCategories());
            category = bael_Log::setCategory("EQUITY.NYSE");  // gets *Default*
                                                              // *Category*
            ASSERT(0 != bsl::strcmp("EQUITY.NYSE", category->categoryName()));
            ASSERT(0 != bsl::strcmp("EQUITY.NASD", category->categoryName()));
        }

#if 0 // TBD setCategory taking a holder
        if (veryVerbose) bsl::cout << "\tTesting 'setCategory' taking a holder"
                                   << bsl::endl;
        {
            lm.setDefaultThresholdLevels(192, 96, 64, 32);
            Holder mH;
            const bael_Category *category;
            category = bael_Log::setCategory(&mH, "EQUITY.NASD");
                                                        // creates new category
            ASSERT(0 == bsl::strcmp("EQUITY.NASD", category->categoryName()));

            bael_Administration::setMaxNumCategories(2);
            ASSERT(2 == bael_Administration::maxNumCategories());
            category = bael_Log::setCategory("EQUITY.NYSE");  // gets *Default*
                                                              // *Category*
            ASSERT(0 != bsl::strcmp("EQUITY.NYSE", category->categoryName()));
            ASSERT(0 != bsl::strcmp("EQUITY.NASD", category->categoryName()));
        }
#endif
         if (veryVerbose) bsl::cout << "\tTesting 'logMessage'" << bsl::endl;
         {
             const Cat  *CAT  = bael_Log::setCategory("EQUITY.NASD");
             const int   SEV  = BloombergLP::bael_Severity::BAEL_WARN;
             const char *FILE = __FILE__;
             const int   LINE = 1066;
             const char *MSG  = "logMessage test";

             const int NREC = TO->numPublishedRecords();
             bael_Log::logMessage(CAT, SEV, FILE, LINE, MSG);
             ASSERT(NREC + 1 == TO->numPublishedRecords());
             ASSERT(isRecordOkay(*TO, CAT, SEV, FILE, LINE, MSG));
         }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // IBM MULTITHREADED SPEWING TEST
        //
        // Concerns:
        //   That multiple threads writing with bael will not crash.  Streams
        //   on ibm have a problem where they are not thread-safe.  Ibm solved
        //   this by having ONE global mutex shaved by all streams, including
        //   cin, cout, cerr and even stringstreams, which undermined
        //   multithreading.  We set a compilation flag to not use this
        //   mutex, and we changed bael to use 'FILE *' i/o instead of streams
        //   wherever possible.  There are some who believe the thread-unsafe
        //   part was only to do with locales, which are going to be completely
        //   set up before any multithreading begins, so it shouldn't be a
        //   problem.
        //
        // Plan:
        //   Have 10 threads, each in an infinite loop spewing BAEL messages,
        //   make sure that the task doesn't crash.  Run this test with
        //   output redirected to /dev/null leave it for hours, and see if it
        //   crashes.  This test need only be performed on ibm.
        // --------------------------------------------------------------------

        using namespace BAEL_LOG_TEST_CASE_MINUS_1;

        bael_DefaultObserver observer(bsl::cout);
        bael_LoggerManager::initSingleton( &observer, 0 );

        bcemt_Attribute attributes;
        bcemt_ThreadUtil::Handle handles[10];
        for (int i = 0; i < 10; ++i) {
            bcemt_ThreadUtil::create(&handles[i], attributes, ThreadFunctor());
        }

        char buffer[256];
        bsl::string input;
        do
        {
            bsl::cout << "Enter something: ";
            bsl::cin.getline(buffer, 256);
            input = buffer;
        } while( input != "exit" );

        // just exit the program, which will kill the threads
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
