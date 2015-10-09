// bdlcc_objectcatalog.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlcc_objectcatalog.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bdlf_bind.h>

#include <bdlt_currenttime.h>

#include <bslma_default.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_queue.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Testing is divided into the following parts (apart from the breathing test):
// An alternate implementation (named my_bcec_ObjectCatalog) for
// 'bdlcc::ObjectCatalog' is provided just for testing purpose.  It is tested
// in
// [ 2].  It is used in the later test cases to verify the
// 'bdlcc::ObjectCatalog'.
//
// We have chosen the primary manipulators for 'bdlcc::ObjectCatalog' as 'add'
// and 'remove'.  They are tested in [3].  Test cases [4], [5], [6] and [7]
// test the remaining manipulators of 'bdlcc::ObjectCatalog'.
//
// [9] tests all the accessors of 'bdlcc::ObjectCatalog' (including access
// through iterator).
//
// [8] tests the 'bdlcc::ObjectCatalogIter' class.
//
// All the test cases above are for a single thread.  [12] verifies that the
// catalog remain consistent in the presence of multiple threads accessing it
// (either directly or through iteration).
//
// [10] verifies that stale handles are rejected properly by catalog.
//
// [10] verifies that the objects are constructed and destroyed properly by
// the catalog.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bdlcc::ObjectCatalog(bslma::Allocator *allocator=0);
// [ 1] ~bdlcc::ObjectCatalog();
//
// MANIPULATORS
// [ 3] int add(TYPE const& object);
// [ 3] int remove(int handle, TYPE* valueBuffer=0);
// [ 5] int remove(int handle, TYPE* valueBuffer=0);
// [ 6] int remove(int handle, TYPE* valueBuffer=0);
// [ 7] void removeAll();
// [ 4] int replace(int handle, TYPE const &newObject);
//
// ACCESSORS
// [ 9] int find(int handle, TYPE *valueBuffer=0) const;
// [ 9] int length() const;
//-----------------------------------------------------------------------------
// CREATORS
// [ 8] bdlcc::ObjectCatalogIter(const bdlcc::ObjectCatalog& catalog);
// [ 8] ~bdlcc::ObjectCatalogIter();
//
// MANIPULATORS
// [ 8] void operator++();
//
// ACCESSORS
// [ 8] operator const void *() const;
// [ 8] pair<int, TYPE> operator()() const;
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
// [2 ] TESTING ALTERNATE IMPLEMENTATION
// [10] TESTING OBJECT CONSTRUCTION/DESTRUCTION
// [11] TESTING OBJECT CONSTRUCTION/DESTRUCTION WITH ALLOCATORS
// [12] TESTING STALE HANDLE REJECTION
// [13] CONCURRENCY TEST
// [14] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   THREAD-SAFE OUTPUT AND ASSERT MACROS
// ----------------------------------------------------------------------------
typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;
static bslmt::Mutex printMutex;  // mutex to protect output macros
#define PT(X) { LockGuard guard(&printMutex); P(X); }
#define PT_(X) { LockGuard guard(&printMutex); P_(X); }

static bslmt::Mutex &assertMutex = printMutex; // mutex to protect assert
                                               // macros

#define ASSERTT(X) {                                                          \
       LockGuard guard(&assertMutex);                                        \
       aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERTT(I,X) {                                                   \
   if (!(X)) {                                                                \
       LockGuard guard(&assertMutex);                                         \
       cout << #I << ": " << I << endl;                                       \
       aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERTT(I,J,X) {                                                \
   if (!(X)) {                                                                \
       LockGuard guard(&assertMutex);                                         \
       cout << #I << ": " << I << "\t"                                        \
                       << #J << ": " << J << endl;                            \
       aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERTT(I,J,K,X) {                                              \
   if (!(X)) {                                                                \
       LockGuard guard(&assertMutex);                                         \
       cout << #I << ": " << I << "\t"                                        \
                       << #J << ": " << J << "\t" << #K << ": " << K << endl; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERTT(I,J,K,L,X) {                                            \
   if (!(X)) {                                                                \
       LockGuard guard(&assertMutex);                                         \
       cout << #I << ": " << I << "\t"                                        \
                  << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
                             << ": " << L << endl;                            \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERTT(I,J,K,L,M,X) {                                          \
   if (!(X)) {                                                                \
       LockGuard guard(&assertMutex);                                         \
       cout << #I << ": " << I << "\t"                                        \
                  << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
                             << ": " << L << "\t" << #M << ": " << M << endl; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//         GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bdlcc::ObjectCatalog<int> Obj;

// From the header file
enum {
    k_INDEX_MASK      = 0x007fffff
  , k_BUSY_INDICATOR  = 0x00800000
  , k_GENERATION_INC  = 0x01000000
  , k_GENERATION_MASK = 0xff000000
  , k_GENERATION_SHIFT = 24 // static_log2(k_GENERATION_INC)
  , k_RECYCLE_COUNT    = 256
};

template<class TYPE>
class my_bcec_ObjectCatalog
    // This class provides an alternative implementation for
    // 'bdlcc::ObjectCatalog'.
{
    enum { k_MAX = 100 };
    struct {
        union {
            int                                 d_valid;
            bsls::AlignmentUtil::MaxAlignedType d_filler;
        };
        char d_obj_p[sizeof(TYPE)];
    } d_arr[k_MAX];

    int d_length;
    int d_topIndex;
  public:
    // CONSTRUCTORS
    my_bcec_ObjectCatalog() : d_length(0), d_topIndex(-1)
    {
        for (int i=0;i<k_MAX;i++) {
            d_arr[i].d_valid = 0;
        }
    }

    ~my_bcec_ObjectCatalog()
    {
        removeAll();
    }

    // MANIPULATORS
    int add(TYPE const& object)
    {
        BSLS_ASSERT(d_topIndex != k_MAX-1);
        new (d_arr[++d_topIndex].d_obj_p) TYPE(object);
        d_arr[d_topIndex].d_valid = 1;
        d_length++;
        return d_topIndex;
    }

    int remove(int h, TYPE* valueBuffer=0)
    {
        if (h<0 || h>d_topIndex || d_arr[h].d_valid==0) {
            return -1;                                                // RETURN
        }

        if (valueBuffer != 0) {
            *valueBuffer = *((TYPE *)d_arr[h].d_obj_p);
        }

        d_length--;
        d_arr[h].d_valid = 0;
        ((TYPE *)d_arr[h].d_obj_p)->~TYPE();

        return 0;
    }

    void removeAll()
    {
        for (int i=0; i<=d_topIndex; i++) {
            d_arr[i].d_valid = 0;
            ((TYPE *)d_arr[i].d_obj_p)->~TYPE();
        }
        d_length = 0;
        d_topIndex = -1;
    }

    int replace(int h, TYPE const &newObject)
    {
        if (h<0 || h>d_topIndex || d_arr[h].d_valid==0) {
            return -1;                                                // RETURN
        }

        ((TYPE *)d_arr[h].d_obj_p)->~TYPE();

        new (d_arr[h].d_obj_p) TYPE(newObject);
        return 0;
    }

    // ACCESSORS
    int find(int h, TYPE *p=0) const
    {
        if (h<0 || h>d_topIndex || d_arr[h].d_valid==0) {
            return -1; //non zero                                     // RETURN
        }
        if (p != 0) {
            *p = *((TYPE const *)d_arr[h].d_obj_p);
        }

        return 0;
    }

    int isMember(TYPE val) const
    {
        TYPE v;
        for (int i=0; i<=d_topIndex; i++) {
            if (find(i, &v) == 0 && v == val) return 1;               // RETURN
        }
        return 0;
    }

    int length() const
    {
        return d_length;
    }

    void verifyState() const
    {
      ASSERT(d_topIndex+1 >= d_length);
      ASSERT(d_length >= 0);

      int nBusy = 0;
      for (int i=0;i<=d_topIndex;i++) {
          if (d_arr[i].d_valid == 1) {
              nBusy++;
          }
      }
      ASSERT(d_length == nBusy);
    }
};

typedef my_bcec_ObjectCatalog<int> my_Obj;

// Each entry of this array specifies a state of 'bdlcc::ObjectCatalog' object
// (say 'catalog').  When the 'catalog' is in the state specified by 'spec'
// then the following are true:
//
// 'catalog.d_nodes.size() == strlen(spec)'
//
// 'catalog.d_nodes[i]->d_handle & k_BUSY_INDICATOR != 0'
//               FOR   0 <= i < 'catalog.d_nodes.size()'
//               AND   'spec[i]' = '1'
//
// 'catalog.d_nodes[i]->d_handle & k_BUSY_INDICATOR == 0'
//               FOR   0 <= i < 'catalog.d_nodes.size()'
//               AND   'spec[i]' = '0'
//
const char *SPECS[] = {
    "",
    "0",
    "1",
    "00",
    "01",
    "10",
    "11",
    "000",
    "001",
    "010",
    "011",
    "100",
    "101",
    "110",
    "111",
    "0000",
    "0001",
    "0010",
    "0011",
    "0100",
    "0101",
    "0110",
    "0111",
    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101",
    "1110",
    "1111"
};
const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

void printSpec(const char *spec)
{
    int len = static_cast<int>(strlen(spec));
    cout << "[" ;
    for (int i=0; i<len; ++i) {
        cout << ((spec[i] == '0') ? "free" : "busy")
             << ((i == len-1) ? "" : "|") ;
    }
    cout << "]" ;
}

void verify(Obj          *o1,
            vector<int>&  handles1,
            my_Obj       *o2,
            vector<int>&  handles2,
            int           maxHandles)
    // Verify that the specified 'o1' is correct by comparing it with the
    // specified 'o2'.
{
    int v1, v2;
    ASSERT(o1->length() == o2->length());

    ASSERT(o1->find(-1) != 0);
    ASSERT(o2->find(-1) != 0);
    ASSERT(o1->find(maxHandles) != 0);
    ASSERT(o2->find(maxHandles) != 0);

    for (int i = 0; i < maxHandles; ++i) {
        int r1 = o1->find(handles1[i], &v1);
        int r2 = o2->find(handles2[i], &v2);

        if (r2 !=0) {
            LOOP_ASSERT(i, r1 != 0);
        }
        else {
            LOOP_ASSERT(i, r1 == 0);
            LOOP3_ASSERT(i, v1, v2, v1 == v2);
        }
    }
}

void gg(Obj          *o1,
        vector<int>&  handles1,
        my_Obj       *o2,
        vector<int>&  handles2,
        const char   *spec,
        const int     gens = 0)
    // Bring the specified object 'o1' into the state specified by the
    // specified 'spec' by using primary manipulators 'add' and 'remove' only.
    // Same sequence of method invocation is applied to the specified 'o2'.
    // Handles returned by 'o1->add' are put into the specified 'handles1' and
    // handles returned by 'o2->add' are put into the specified 'handles2'.
{
    // First invoke 'add' 'strlen(spec)' times, this will cause first
    // 'strlen(spec)' entries of 'o1->d_nodes' to be busy.  Then invoke
    // 'remove' for all the entries corresponding to char '0' of the 'spec',
    // this will cause those entries to be freed.  Optionally, add and remove
    // the entry 'gens' times to bring the generation numbers to 'gens' for the
    // entries still present, and to 'gens + 1' for freed entries.

    int v1, v2;
    int len = static_cast<int>(strlen(spec));
    for (int i=0 ;i < len; ++i) {
        if (veryVerbose) {
            cout << "\thandles1[" << i << "] = o1->add(" << i << "); // "
                 << gens << " generations\n";
        }

        handles1[i] = o1->add(i);
        for (int j=0; j < gens; ++j) {
            int r = o1->remove(handles1[i], &v1);
            ASSERT(r  == 0);
            ASSERT(v1 == i);
            int h = o1->add(i);
            ASSERT((h & k_INDEX_MASK) == (handles1[i] & k_INDEX_MASK));
            handles1[i] = h;
        }

        ASSERT((handles1[i] & k_INDEX_MASK) == (unsigned)i);
        ASSERT((handles1[i] & k_BUSY_INDICATOR) == k_BUSY_INDICATOR);
        ASSERT((((unsigned)handles1[i]) >> k_GENERATION_SHIFT) ==
                                                     (gens % k_RECYCLE_COUNT));

        handles2[i] = o2->add(i);
        ASSERT(handles2[i] == i);
    }

    for (int i=0; i < len; ++i) {
        if (spec[i] == '0') {
            if (veryVerbose) {
                cout << "\tr = o1->remove(handles1[" << i << "], &v1);\n";
            }
            int r = o1->remove(handles1[i], &v1);
            ASSERT(r == 0);
            ASSERT(v1 == i);

            r = o2->remove(handles2[i], &v2);
            ASSERT(r == 0);
            ASSERT(v2 == i);
        }
    }
}
// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_USAGE_EXAMPLE

{

typedef bsl::queue<int> *RemoteAddress;
static bsl::queue<int>   server;
static bslmt::Mutex      serverMutex;
static bslmt::Condition  serverNotEmptyCondition;

const int NUM_QUERIES_TO_PROCESS   = 128; // for testing purposes
const int CALLBACK_PROCESSING_TIME = 10;  // in microseconds

class QueryResult;

void queryCallBack(const QueryResult& result)
    // For testing only, we simulate a callback that takes a given time to
    // process a query.
{
    bslmt::ThreadUtil::microSleep(CALLBACK_PROCESSING_TIME);
}

///Usage
///-----
//
/// Example 1: Catalog Usage
/// - - - - - - - - - - - -
// Consider a client sending queries to a server asynchronously.  When the
// response to a query arrives, the client needs to invoke the callback
// associated with that query.  For good performance, the callback should be
// invoked as quickly as possible.  One way to achieve this is as follows.  The
// client creates a catalog for the functors associated with queries.  It sends
// to the server the handle (obtained by passing the callback functor
// associated with the query to the 'add' method of catalog), along with the
// query.  The server does not interpret this handle in any way and sends it
// back to the client along with the computed query result.  The client, upon
// receiving the response, gets the functor (associated with the query) back by
// passing the handle (contained in the response message) to the 'find' method
// of catalog.
//
// Assume the following declarations (we leave the implementations as
// undefined, as the definitions are largely irrelevant to this example):
//..
    struct Query {
        // Class simulating the query.
    };

    class QueryResult {
        // Class simulating the result of a query.
    };

    class RequestMsg
        // Class encapsulating the request message.  It encapsulates the
        // actual query and the handle associated with the callback for the
        // query.
    {
        Query d_query;
        int   d_handle;

      public:
        RequestMsg(Query query, int handle)
            // Create a request message with the specified 'query' and
            // 'handle'.
        : d_query(query)
        , d_handle(handle)
        {
        }

        int handle() const
            // Return the handle contained in this response message.
        {
            return d_handle;
        }
    };

    class ResponseMsg
        // Class encapsulating the response message.  It encapsulates the query
        // result and the handle associated with the callback for the query.
    {
        int d_handle;

      public:
        void setHandle(int handle)
            // Set the "handle" contained in this response message to the
            // specified 'handle'.
        {
            d_handle = handle;
        }

        QueryResult queryResult() const
            // Return the query result contained in this response message.
        {
            return QueryResult();
        }

        int handle() const
            // Return the handle contained in this response message.
        {
            return d_handle;
        }
    };

    void sendMessage(RequestMsg msg, RemoteAddress peer)
        // Send the specified 'msg' to the specified 'peer'.
    {
        serverMutex.lock();
        peer->push(msg.handle());
        serverNotEmptyCondition.signal();
        serverMutex.unlock();
    }

    void recvMessage(ResponseMsg *msg, RemoteAddress peer)
        // Get the response from the specified 'peer' into the specified 'msg'.
    {
        serverMutex.lock();
        while (peer->empty()) {
            serverNotEmptyCondition.wait(&serverMutex);
        }
        msg->setHandle(peer->front());
        peer->pop();
        serverMutex.unlock();
    }

    void getQueryAndCallback(Query                            *query,
                             bsl::function<void(QueryResult)> *callBack)
        // Set the specified 'query' and 'callBack' to the next 'Query' and its
        // associated functor (the functor to be called when the response to
        // this 'Query' comes in).
    {
        (void)query;
        *callBack = &queryCallBack;
    }
//..
// Furthermore, let also the following variables be declared:
//..
    RemoteAddress serverAddress;  // address of remote server

    bdlcc::ObjectCatalog<bsl::function<void(QueryResult)> > catalog;
        // Catalog of query callbacks, used by the client internally to keep
        // track of callback functions across multiple queries.  The invariant
        // is that each element corresponds to a pending query (i.e., the
        // callback function has not yet been or is in the process of being
        // invoked).
//..
// Now we define functions that will be used in the thread entry functions:
//..
    void testClientProcessQueryCpp()
    {
        int queriesToBeProcessed = NUM_QUERIES_TO_PROCESS;
        while (queriesToBeProcessed--) {
            Query query;
            bsl::function<void(QueryResult)> callBack;

            // The following call blocks until a query becomes available.
            getQueryAndCallback(&query, &callBack);

            // Register 'callBack' in the object catalog.
            int handle = catalog.add(callBack);
            ASSERT(handle);

            // Send query to server in the form of a 'RequestMsg'.
            RequestMsg msg(query, handle);
            sendMessage(msg, serverAddress);
        }
    }

    void testClientProcessResponseCpp()
    {
        int queriesToBeProcessed = NUM_QUERIES_TO_PROCESS;
        while (queriesToBeProcessed--) {
            // The following call blocks until some response is available in
            // the form of a 'ResponseMsg'.

            ResponseMsg msg;
            recvMessage(&msg, serverAddress);
            int handle = msg.handle();
            QueryResult result = msg.queryResult();

            // Process query 'result' by applying registered 'callBack' to it.
            // The 'callBack' function is retrieved from the 'catalog' using
            // the given 'handle'.

            bsl::function<void(QueryResult)> callBack;
            ASSERT(0 == catalog.find(handle, &callBack));
            callBack(result);

            // Finally, remove the no-longer-needed 'callBack' from the
            // 'catalog'.  Assert so that 'catalog' may not grow unbounded if
            // remove fails.

            ASSERT(0 == catalog.remove(handle));
        }
    }
//..
//
///Example 2: Iterator Usage
///- - - - - - - - - - - - -
// The following code fragment shows how to use bdlcc::ObjectCatalogIter to
// iterate through all the objects of 'catalog' (a catalog of objects of type
// 'MyType').
//..
    void use(bsl::function<void(QueryResult)> object)
    {
        (void)object;
    }
//..

}  // close namespace OBJECTCATALOG_TEST_USAGE_EXAMPLE

// ============================================================================
//                         CASE 13 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_13

{

typedef bdlcc::ObjectCatalogIter<int> Iter;

enum {
    k_NUM_THREADS    = 10,
    k_NUM_ITERATIONS = 1000
};

bslma::TestAllocator ta(veryVeryVerbose);
bdlcc::ObjectCatalog<int> catalog(&ta);

bslmt::Barrier barrier(k_NUM_THREADS + 3);

int getObjectFromPair(Iter &it)
{
    return it().second;
}

void validateIter (int arr[], int len)
    // Verify the iteration.  This function is invoked from 'testIteration'
    // after it has iterated the 'catalog'.
{
    ASSERT(len <= k_NUM_THREADS);
    for (int i=0; i<len; i++) {
        // value must be valid
       int present = 0;
       for (int id=0; id<k_NUM_THREADS; id++) {
           if (id == arr[i] || -id-1 == arr[i]) {
               present=1; break;
           }
       }
       ASSERT(present == 1);

       // no duplicate should be there
       for (int j=i+1; j<len; j++) {
           ASSERT(arr[i] != arr[j]);
       }

    }
}

extern "C" {

void *testAddFindReplaceRemove(void *arg)
    // Invoke 'add', 'find', 'replace' and 'remove' in a loop.
{
    barrier.wait();
    int id = static_cast<int>(reinterpret_cast<bsls::Types::IntPtr>(arg));
    int v;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        int h = catalog.add(id);
        LOOP_ASSERTT(i, catalog.find(h) == 0);
        LOOP_ASSERTT(i, catalog.find(h, &v) == 0);
        LOOP_ASSERTT(i, v == id);
        LOOP_ASSERTT(i, catalog.replace(h, -id-1) == 0);
        LOOP_ASSERTT(i, catalog.find(h) == 0);
        LOOP_ASSERTT(i, catalog.find(h, &v) == 0);
        LOOP_ASSERTT(i, v == -id-1);
        v = -1; // reset
        LOOP_ASSERTT(i, catalog.remove(h, &v) == 0);
        LOOP_ASSERTT(i, v == -id-1);
        LOOP_ASSERTT(i, catalog.find(h) == -1);
    }
    return NULL;
}

void *testLength(void *arg)
    // Invoke 'length' in a loop.
{
    (void)arg;
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        int len = catalog.length();
        LOOP2_ASSERTT(i, len, len >= 0);
        LOOP2_ASSERTT(i, len, len <= k_NUM_THREADS);
    }
    return NULL;
}

void *testIteration(void *arg)
    // Iterate the 'catalog' in a loop.
{
    (void)arg;
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {

        enum { k_MAX = 100 };
        int arr[k_MAX]; int size=0;
        for (Iter it(catalog); it; ++it) {
            arr[size++] = getObjectFromPair(it);
        }
        validateIter(arr, size);
    }
    return NULL;
}

void *verifyStateThread(void *arg)
    // Verify the 'catalog' in a loop.
{
    (void)arg;
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        catalog.verifyState();
    }
    return NULL;
}

} // extern "C"

}  // close namespace OBJECTCATALOG_TEST_CASE_13
// ============================================================================
//                         CASE 12 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_12

{

}  // close namespace OBJECTCATALOG_TEST_CASE_12
// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_11

{

class AllocPattern {
    // This class encapsulates an integer pattern.  It also has a static
    // variable 'objCount', that holds the number of objects created for this
    // class.  It uses memory allocation to store the pattern.

    bslma::Allocator *d_alloc_p;
    int              *d_pattern_p;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(AllocPattern,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    static int objCount;

    // CONSTRUCTORS
    AllocPattern(bslma::Allocator *alloc = 0)
    : d_alloc_p(bslma::Default::allocator(alloc))
    , d_pattern_p((int*)d_alloc_p->allocate(sizeof *d_pattern_p))
    {
        ASSERT(0 == (bsls::Types::IntPtr)((char *)this) %
                                 bsls::AlignmentFromType<AllocPattern>::VALUE);
        *d_pattern_p = 0;
        objCount++;
    }

    AllocPattern(const AllocPattern& rhs, bslma::Allocator *alloc = 0)
    : d_alloc_p(bslma::Default::allocator(alloc))
    , d_pattern_p((int*)d_alloc_p->allocate(sizeof *d_pattern_p))
    {
        ASSERT(0 == (bsls::Types::IntPtr)((char *)this) %
                                 bsls::AlignmentFromType<AllocPattern>::VALUE);
        *d_pattern_p = *rhs.d_pattern_p;
        objCount++;
    }

    ~AllocPattern()
    {
        d_alloc_p->deallocate(d_pattern_p);
        objCount--;
    }

    // MANIPULATORS
    AllocPattern& operator=(const AllocPattern& rhs) {
        if (this != &rhs) {
            d_alloc_p->deallocate(d_pattern_p);
            d_pattern_p = new(*d_alloc_p) int(rhs.pattern());
        }
        return *this;
    }

    void setPattern(int pattern)
    {
        *d_pattern_p = pattern;
    }

    // ACCESSORS
    int pattern() const
    {
        return *d_pattern_p;
    }
};

int AllocPattern::objCount = 0;

}  // close namespace OBJECTCATALOG_TEST_CASE_11
// ============================================================================
//                         CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_10

{

class Pattern {
    // This class encapsulates an integer pattern.  It also has a static
    // variable 'objCount', that holds the number of objects created for this
    // class.

    int d_pattern;

  public:
    static int objCount;
    // CONSTRUCTORS
    Pattern()
    {
        ASSERT(0 == (bsls::Types::IntPtr)((char *)this)
                                    % bsls::AlignmentFromType<Pattern>::VALUE);
        d_pattern = 0;
        objCount++;
    }

    Pattern(const Pattern& a)
    {
        d_pattern = a.d_pattern;
        objCount++;
    }

    ~Pattern()
    {
        objCount--;
    }

    // MANIPULATORS
    void setPattern(int pattern)
    {
        d_pattern = pattern;
    }

    // ACCESSORS
    int pattern() const
    {
        return d_pattern;
    }
};

int Pattern::objCount = 0;

}  // close namespace OBJECTCATALOG_TEST_CASE_10
// ============================================================================
//                          CASE 9 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_9

{

typedef bdlcc::ObjectCatalogIter<int> Iter;

void verifyAccessors(Obj          *o1,
                     vector<int>&  handles1,
                     my_Obj       *o2,
                     vector<int>&  handles2,
                     int           maxHandles)
    // Verify the catalog accessors (including iterator) by comparing with
    // alternate implementation.
{
    int v1, v2, v;

    if (veryVerbose) { cout << "\tverifying 'length'\n"; }
    ASSERT(o1->length() == o2->length());

    if (veryVerbose) { cout << "\tverifying 'find(i, &v)'\n"; }
    ASSERT(o1->find(-1, &v1) != 0);
    ASSERT(o2->find(-1, &v2) != 0);
    ASSERT(o1->find(maxHandles, &v1) != 0);
    ASSERT(o2->find(maxHandles, &v2) != 0);
    for (int i = 0; i < maxHandles; ++i) {
        int r1 = o1->find(handles1[i], &v1);
        int r2 = o2->find(handles2[i], &v2);

        if (r2 !=0) {
            LOOP_ASSERT(i, r1 != 0);
        }
        else {
            LOOP_ASSERT(i, r1 == 0);
            LOOP3_ASSERT(i, v1, v2, v1 == v2);
        }
    }

    if (veryVerbose) { cout << "\tverifying 'find(i)'\n"; }
    ASSERT(o1->find(-1) != 0);
    ASSERT(o2->find(-1) != 0);
    ASSERT(o1->find(maxHandles) != 0);
    ASSERT(o2->find(maxHandles) != 0);
    for (int i = 0; i < maxHandles; ++i) {
        int r1 = o1->find(handles1[i]);
        int r2 = o2->find(handles2[i]);

        if (r2 !=0) {
            LOOP_ASSERT(i, r1 != 0);
        }
        else {
            LOOP_ASSERT(i, r1 == 0);
        }
    }

    if (veryVerbose) { cout << "\tverifying iteration\n"; }

    for (Iter it(*o1); it; ++it) {
        pair<int, int> p = it();
        o1->find(p.first, &v);
        ASSERT(v == p.second);
        ASSERT(o2->isMember(p.second) == 1);
    }
}

}  // close namespace OBJECTCATALOG_TEST_CASE_9
// ============================================================================
//                          CASE 8 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_8

{

}  // close namespace OBJECTCATALOG_TEST_CASE_8
// ============================================================================
//                          CASE 7 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_7

{

}  // close namespace OBJECTCATALOG_TEST_CASE_7
// ============================================================================
//                          CASE 6 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_6

{

}  // close namespace OBJECTCATALOG_TEST_CASE_6
// ============================================================================
//                          CASE 5 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_5

{

}  // close namespace OBJECTCATALOG_TEST_CASE_5
// ============================================================================
//                          CASE 4 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_4

{
}  // close namespace OBJECTCATALOG_TEST_CASE_4
// ============================================================================
//                          CASE 3 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_3

{
}  // close namespace OBJECTCATALOG_TEST_CASE_3
// ============================================================================
//                          CASE 2 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_2

{
}  // close namespace OBJECTCATALOG_TEST_CASE_2
// ============================================================================
//                          CASE 1 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_1

{
}  // close namespace OBJECTCATALOG_TEST_CASE_1
// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE:
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

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n-------------" << bsl::endl;

        using namespace OBJECTCATALOG_TEST_USAGE_EXAMPLE;

        serverAddress = &server;

        {
            if (verbose) bsl::cout << "\n\tCatalog usage"
                                   << "\n\t-------------" << bsl::endl;

// In some thread, the client executes the following code.
//..
//  extern "C" void *testClientProcessQuery(void *)
//  {
//      testClientProcessQueryCpp();
//      return 0;
//  }
//..
// In some other thread, the client executes the following code.
//..
//  extern "C" void *testClientProcessResponse(void *)
//  {
//      testClientProcessResponseCpp();
//      return 0;
//  }
//..
        }

        {
            if (verbose) bsl::cout << "\n\tIterator usage"
                                   << "\n\t--------------" << bsl::endl;

// Now iterate through the 'catalog':
//..
//  for (bdlcc::ObjectCatalogIter<MyType> it(catalog); it; ++it) {
//      bsl::pair<int, MyType> p = it(); // p.first contains the handle and
//                                       // p.second contains the object
//      use(p.second);                   // the function 'use' uses the
//                                       // object in some way
//  }
//  // 'it' is now destroyed out of the scope, releasing the lock.
//..
// Note that the associated catalog is (read)locked when the iterator is
// constructed and is unlocked only when the iterator is destroyed.  This means
// that until the iterator is destroyed, all the threads trying to modify the
// catalog will remain blocked (even though multiple threads can concurrently
// read the object catalog).  So clients must make sure to destroy their
// iterators after they are done using them.  One easy way is to use the
// 'for (bdlcc::ObjectCatalogIter<MyType> it(catalog); ...' as above.

        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST:
        //   Verify the concurrent access to catalog.
        //
        // Concerns:
        //   That the catalog remain consistent in presence of multiple
        //   threads accessing/modifying it (either directly or through
        //   iteration).
        //
        // Plan:
        //   Create a catalog.  Create 'k_NUM_THREADS' threads and let each
        //   thread invoke 'add', 'find', 'replace' and 'remove' in a loop.
        //   Create a thread and let it invoke 'length' in a loop.  Create a
        //   thread and let it iterate the catalog in a loop.
        //   Create a thread and let it invoke 'verifyState' in a loop.
        //   Let all above (k_NUM_THREADS + 3) threads run concurrently.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONCURRENCY TEST" << endl
                          << "================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_13;

        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS + 3];

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::create(&threads[i],
                                      testAddFindReplaceRemove,
                                      (void*)(bsls::Types::IntPtr)i);
        }

        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS + 0],
                                  testLength,
                                  NULL);
        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS + 1],
                                  testIteration,
                                  NULL);
        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS + 2],
                                  verifyStateThread,
                                  NULL);

        for (int i = 0; i < k_NUM_THREADS + 3; ++i) {
            bslmt::ThreadUtil::join(threads[i]);
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING STALE HANDLE REJECTION:
        //   Verify that catalog rejects the stale handles properly.
        //
        // Concerns:
        //   That stale handles must be rejected by catalog.
        //
        // Plan:
        //   Create a catalog, add an object and remove the added object thus
        //   making the handle (returned by 'add') stale.  Verify that the
        //   catalog rejects this handle correctly.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING STALE HANDLE REJECTION" << endl
                          << "==============================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_12;
        typedef bdlcc::ObjectCatalog<double> Obj;
        enum {
            k_NUM_ITERATIONS = 5
        };

        const double VA = 1.0;
        const double VB = 2.0;
        int HA = -1;
        int HB = -1;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(&ta);

        HA = x.add(VA);
        x.remove(HA);

        for (bsl::size_t j=0; j < k_NUM_ITERATIONS; ++j) {
            for (bsl::size_t i = 1; i < k_RECYCLE_COUNT; ++i) {
                ASSERT(0 != x.find(HA)); // stale handle should be rejected
                                         // until the corresponding 'd_nodes'
                                         // entry is reused 'k_RECYCLE_COUNT'
                                         // times.

                HB = x.add(VB);
                x.remove(HB);
            }
            HB = x.add(VB);

            LOOP_ASSERT(j, HA == HB);
            LOOP_ASSERT(j, 0 == x.find(HA));

            double vbuf = 0.0;
            LOOP_ASSERT(j, 0 == x.find(HA, &vbuf));
            LOOP2_ASSERT(j, vbuf, VB == vbuf);
            x.remove(HB);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OBJECT CONSTRUCTION/DESTRUCTION WITH ALLOCATION
        //   Verify that catalog properly constructs and destroys the objects
        //   with the catalog's allocator.
        //
        // Concerns:
        //   That 'bdlcc::ObjectCatalog<TYPE>' properly passes the allocator
        //   to its object.
        //
        // Plan:
        //   Create a catalog of 'AllocPattern' (a class that encapsulates an
        //   integer pattern with allocation) objects.  Create an allocated
        //   pattern object 'a', set its pattern to 'PATTERN1', add it to the
        //   catalog, invoke 'find' to get it back and verify that its pattern
        //   is 'PATTERN1'.
        //
        //   Create another pattern object 'b', set its pattern to be
        //   'PATTERN2', invoke 'replace' to replace 'a' with 'b', invoke
        //   'find' to get 'b' back and verify that its pattern is 'PATTERN2'.
        //   Invoke 'remove' to remove 'b' and verify that pattern of the
        //   removed object is 'PATTERN2'.
        //
        //   Finally invoke 'removeAll' and verify that the number of created
        //   objects (of class 'AllocPattern') is correct.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING OBJECT CONSTRUCTION/DESTRUCTION" << endl
                          << "=======================================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_11;

        typedef bdlcc::ObjectCatalog<AllocPattern> Obj;

        enum {
            k_PATTERN1 = 0x33333333,
            k_PATTERN2 = 0xaaaaaaaa,
            k_PATTERN3 = 0xbbbbbbbb
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(&ta);
        {
            AllocPattern a(&ta), b(&ta), vbuf(&ta);
            int HA;

            a.setPattern(k_PATTERN1);
            HA = x.add(a);

            x.find(HA, &vbuf);
            ASSERT(vbuf.pattern() == k_PATTERN1);

            b.setPattern(k_PATTERN2);
            x.replace(HA, b);
            x.find(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            vbuf.setPattern(k_PATTERN3);
            x.remove(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            x.removeAll();
        } // let 'a', 'b', and 'vbuf' be destroyed

        ASSERT(AllocPattern::objCount == 0);

      }break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING OBJECT CONSTRUCTION/DESTRUCTION:
        //   Verify that catalog properly constructs and destroys the objects.
        //
        // Concerns:
        //   That 'bdlcc::ObjectCatalog<TYPE>' properly constructs and destroys
        //   the objects (of type 'TYPE').
        //
        // Plan:
        //   Create a catalog of 'Pattern' (a class that encapsulates an
        //   integer pattern) objects.  Create a pattern object 'a', set its
        //   pattern to 'k_PATTERN1', add it to the catalog, invoke 'find' to
        //   get it back and verify that its pattern is 'k_PATTERN1'.
        //
        //   Create another pattern object 'b', set its pattern to be
        //   'k_PATTERN2', invoke 'replace' to replace 'a' with 'b', invoke
        //   'find' to get 'b' back and verify that its pattern is
        //   'k_PATTERN2'.  Invoke 'remove' to remove 'b' and verify that
        //   pattern of the removed object is 'k_PATTERN2'.
        //
        //   Finally invoke 'removeAll' and verify that the number of created
        //   objects (of class 'Pattern') is correct.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING OBJECT CONSTRUCTION/DESTRUCTION" << endl
                          << "=======================================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_10;

        typedef bdlcc::ObjectCatalog<Pattern> Obj;

        enum {
            k_PATTERN1 = 0x33333333,
            k_PATTERN2 = 0xaaaaaaaa,
            k_PATTERN3 = 0xbbbbbbbb
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        {
            Pattern a, b, vbuf;
            int HA;

            a.setPattern(k_PATTERN1);
            HA = x1.add(a);
            x1.find(HA, &vbuf);
            ASSERT(vbuf.pattern() == k_PATTERN1);

            b.setPattern(k_PATTERN2);
            x1.replace(HA, b);
            x1.find(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            vbuf.setPattern(k_PATTERN3);
            x1.remove(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            x1.removeAll();
        } // let 'a', 'b', and 'vbuf' be destroyed

        ASSERT(Pattern::objCount == 0);

      }break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS:
        //   Verify the accessors of catalog.
        //
        // Concerns:
        //   That accessors of 'bdlcc::ObjectCatalog' (including access through
        //   iterator) work correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke various accessors and then verify the
        //   result.
        //
        // Testing:
        //   int find(int handle, TYPE *valueBuffer=0) const;
        //   int length() const;
        //   bdlcc::ObjectCatalogIter(const bdlcc::ObjectCatalog<TYPE>&);
        //   ~bdlcc::ObjectCatalogIter();
        //   void bdlcc::ObjectCatalogIter::operator++();
        //   operator bdlcc::ObjectCatalogIter::operator const void *() const;
        //   pair<int, TYPE> bdlcc::ObjectCatalogIter::operator()() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_9;

        for (int i=0; i<NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'length', 'find' and"
                      << "'iteration' with catalog-state \"";
                printSpec(SPECS[i]);
                cout << "\""<< endl;
            }
            int len = static_cast<int>(strlen(SPECS[i]));
            bslma::TestAllocator ta(veryVeryVerbose);

            for (bsl::size_t g = 0; g < 2 * k_RECYCLE_COUNT; g = 2 * g + 1) {
                if (veryVerbose)
                    cout  << "\tUsing handles with " << g << " generations\n";

                if (veryVeryVerbose)
                    cout  << "\t\tbringing the catalog in the desired state\n";

                Obj o1(&ta);
                my_Obj o2;
                vector<int> handles1(len, -1);
                vector<int> handles2(len, -1);
                gg(&o1,
                   handles1,
                   &o2,
                   handles2,
                   SPECS[i],
                   static_cast<int>(g));

                if (veryVeryVerbose)
                    cout << "\t\tbrought the catalog into the desired state\n";

                if (veryVeryVerbose) { cout << "\t\tverifying o1\n"; }
                o1.verifyState();
                if (veryVeryVerbose) { cout << "\t\tverifying o2\n"; }
                o2.verifyState();

                if (veryVeryVerbose) { cout << "\t\tverifying accessors \n"; }
                verifyAccessors(&o1, handles1, &o2, handles2, len);
            }
        }
      }break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ITERATION:
        //   Verify the iteration.
        //
        // Concerns:
        //   That iteration works correctly in presence of one thread.
        //
        // Plan:
        //
        // Testing:
        //   bdlcc::ObjectCatalogIter(const bdlcc::ObjectCatalog<TYPE>&);
        //   ~bdlcc::ObjectCatalogIter();
        //   void operator++();
        //   operator const void *() const;
        //   pair<int, TYPE> operator()() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ITERATION" << endl
                          << "=================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_8;
        typedef bdlcc::ObjectCatalog<double> Obj;
        typedef bdlcc::ObjectCatalogIter<double> Iter;
        int HA, HB, HC, HD, HE;
        double vbuffer;

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        x1.add(1); if (veryVerbose) {cout << "\tadd(1)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.remove(HB); if (veryVerbose) { cout << "\tremove(2)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        x1.remove(HB); if (veryVerbose) { cout << "\tremove(2)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        HC = x1.add(3); if (veryVerbose) { cout << "\tadd(3)\n"; }
        HD = x1.add(4); if (veryVerbose) { cout << "\tadd(4)\n"; }
        HE = x1.add(5); if (veryVerbose) { cout << "\tadd(5)\n"; }

        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        x1.remove(HC); if (veryVerbose) { cout << "\tremove(3)\n";}
        x1.remove(HE); if (veryVerbose) { cout << "\tremove(5)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        HC = x1.add(3); if (veryVerbose) { cout << "\tadd(3)\n"; }
        HD = x1.add(4); if (veryVerbose) { cout << "\tadd(4)\n"; }
        HE = x1.add(5); if (veryVerbose) { cout << "\tadd(5)\n"; }

        x1.remove(HB); if (veryVerbose) { cout << "\tremove(2)\n";}
        x1.remove(HD); if (veryVerbose) { cout << "\tremove(4)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }
        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVEALL':
        //   Verify the 'removeAll'.
        //
        // Concerns:
        //   That 'removeAll' works correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states.  For each state, invoke
        //   'removeAll' and then verify the result.
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVEALL'" << endl
                          << "=================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_7;

        for (int i=0; i<NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\nTesting 'removeAll' with catalog-state \"";
                printSpec(SPECS[i]);
                cout << "\"" << endl;
            }

            int len = static_cast<int>(strlen(SPECS[i]));
            if (veryVerbose) {
                cout  << "\tbringing the catalog in the desired state\n";
            }

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj o1(&ta);
            my_Obj o2;
            vector<int> handles1(len, -1);
            vector<int> handles2(len, -1);
            gg(&o1, handles1, &o2, handles2, SPECS[i]);

            if (veryVerbose) {
                cout << "\tbrought the catalog into the desired state\n";
            }

            if (veryVerbose) { cout << "\t\tnow doing removeAll();\n"; }

            o1.removeAll ();
            o2.removeAll ();

            if (veryVerbose) { cout << "\tverifying o1\n"; }
            o1.verifyState();
            if (veryVerbose) { cout << "\tverifying o2\n"; }
            o2.verifyState();
            if (veryVerbose) { cout << "\tmatching o1 and o2\n\n"; }
            verify(&o1, handles1, &o2, handles2, len);

        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE(handle, &valueBuf)':
        //   Verify the 'remove(handle, &valueBuf)'.
        //
        // Concerns:
        //   That 'remove(handle, valueBuffer)' works correctly in presence of
        //   one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke 'remove' (with various values of handle
        //   argument) and then verify the result.
        //
        // Testing:
        //   int remove(int handle, TYPE* valueBuffer=0);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVE(handle, &valueBuf)'\n"
                          << "===================================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_6;

        for (int i=0; i<NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'remove' with catalog-state \"";
                printSpec(SPECS[i]);
                cout << "\""<< endl;
            }

            int len = static_cast<int>(strlen(SPECS[i]));
            for (int j=0; j<len; ++j) {
                if (veryVerbose) {
                    cout  << "\tbringing the catalog in the desired state\n";
                }
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj o1(&ta);

                my_Obj o2;
                vector<int> handles1(len);
                vector<int> handles2(len);
                for (int k=0; k<len; ++k) {
                    handles1[k] = -1;
                    handles2[k] = -1;
                }
                gg(&o1, handles1, &o2, handles2, SPECS[i]);
                if (veryVerbose) {
                    cout << "\tbrought the catalog into the desired state\n";
                }

                if (veryVerbose) {
                    cout << "\tdoing remove(handles1[" << j  <<"], &v1);\n";
                }
                int v1, v2;
                int r1 = o1.remove (handles1[j], &v1);
                int r2 = o2.remove (handles2[j], &v2);
                if (veryVerbose) {
                    cout << "\tverifying the above remove operation\n";
                }
                if (r2 != 0) {
                    ASSERT(r1 != 0);
                }
                else {
                    ASSERT(r1 == 0);
                    ASSERT(v1 == v2);
                    r1 = o1.find(handles1[j]);
                    r2 = o2.find(handles2[j]);
                    ASSERT(r1 != 0);
                    ASSERT(r2 != 0);
                }
                if (veryVerbose) { cout << "\tverifying o1\n"; }
                o1.verifyState();
                if (veryVerbose) { cout << "\tverifying o2\n"; }
                o2.verifyState();
                if (veryVerbose) { cout << "\tmatching o1 and o2\n\n"; }
                verify(&o1, handles1, &o2, handles2, len);
            }
        }
      }break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE(handle)':
        //   Verify the 'remove(handle)'.
        //
        // Concerns:
        //   That 'remove(handle)' works correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke 'remove' (with various values of handle
        //   argument) and then verify the result.
        //
        // Testing:
        //   int remove(int handle, TYPE* valueBuffer=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'REMOVE(handle)'" << endl
                          << "========================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_5;

        for (int i=0; i<NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'remove' with catalog-state \"";
                printSpec(SPECS[i]);
                cout << "\""<< endl;
            }

            int len = static_cast<int>(strlen(SPECS[i]));
            for (int j=0; j<len; ++j) {
                if (veryVerbose) {
                    cout  << "\tbringing the catalog in the desired state\n";
                }
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj o1(&ta);

                my_Obj o2;
                vector<int> handles1(len);
                vector<int> handles2(len);
                for (int k=0; k<len; ++k) {
                    handles1[k] = -1;
                    handles2[k] = -1;
                }
                gg(&o1, handles1, &o2, handles2, SPECS[i]);
                if (veryVerbose) {
                    cout << "\tbrought the catalog into the desired state\n";
                }

                if (veryVerbose) {
                    cout << "\tnow doing remove(handles1[" << j  <<"]);\n";
                }
                int r1 = o1.remove (handles1[j]);
                int r2 = o2.remove (handles2[j]);
                if (veryVerbose) {
                    cout << "\tverifying the above remove operation\n";
                }
                if (r2 != 0) {
                    ASSERT(r1 != 0);
                }
                else {
                    int v1, v2;
                    ASSERT(r1 == 0);
                    r1 = o1.find(handles1[j], &v1);
                    r2 = o2.find(handles2[j], &v2);
                    ASSERT(r1 != 0);
                    ASSERT(r2 != 0);
                }
                if (veryVerbose) { cout << "\tverifying o1\n"; }
                o1.verifyState();
                if (veryVerbose) { cout << "\tverifying o2\n"; }
                o2.verifyState();
                if (veryVerbose) { cout << "\tmatching o1 and o2\n"; }
                verify(&o1, handles1, &o2, handles2, len);
            }
        }

      }break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'REPLACE':
        //   Verify the 'replace'.
        //
        // Concerns:
        //   That 'replace' works correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke 'replace' (with various values of handle
        //   argument) and then verify the result.
        //
        // Testing:
        //   int replace(int handle, TYPE const &newObject);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'REPLACE'" << endl
                          << "=================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_4;

        for (int i=0; i<NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'replace' with catalog-state \"";
                printSpec(SPECS[i]);
                cout << "\""<< endl;
            }

            int len = static_cast<int>(strlen(SPECS[i]));
            for (int j=0; j<len; ++j) {
                if (veryVerbose) {
                  cout  << "\tbringing the catalog in the desired state\n";
                }
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj o1(&ta);

                my_Obj o2;
                vector<int> handles1(len);
                vector<int> handles2(len);
                for (int k=0; k<len; ++k) {
                    handles1[k] = -1;
                    handles2[k] = -1;
                }
                gg(&o1, handles1, &o2, handles2, SPECS[i]);
                if (veryVerbose) {
                    cout << "\tbrought the catalog into the desired state\n";
                }
                const int V = 444;
                if (veryVerbose) {
                    cout << "doing replace(handles1[" << j  <<"], "
                         << V << ");\n";
                }
                int r1 = o1.replace (handles1[j], V);
                int r2 = o2.replace (handles2[j], V);
                if (veryVerbose) {
                    cout << "\tverifying the above replace operation\n";
                }
                if (r2 != 0) {
                    ASSERT(r1 != 0);
                }
                else {
                    int v1, v2;
                    ASSERT(r1 == 0);
                    r1 = o1.find(handles1[j], &v1);
                    r2 = o2.find(handles2[j], &v2);
                    ASSERT(r1 == 0);
                    ASSERT(r2 == 0);
                    ASSERT(v1 == V);
                    ASSERT(v2 == V);
                }
                if (veryVerbose) { cout << "\tverifying o1\n"; }
                o1.verifyState();
                if (veryVerbose) { cout << "\tverifying o2\n"; }
                o2.verifyState();
                if (veryVerbose) { cout << "\tmatching o1 and o2\n\n"; }
                verify(&o1, handles1, &o2, handles2, len);
            }
        }

      }break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //   Verify the primary manipulators of catalog.
        //
        // Concerns:
        //   That the chosen primary manipulators 'add' and 'remove' work
        //   correctly in presence of one thread.
        //
        // Plan:
        //   Using generator function 'gg', bring the catalog into various
        //   states (enumerated in 'SPECS' array, see the documentation for
        //   'SPECS') by invoking only 'add' and 'remove'.  Verify the result
        //   after each invocation.  Verification is done by the combination
        //   of following three.
        //     (1) Sanity checks using accessors.
        //
        //     (2) Comparison between the behavior of 'bdlcc::ObjectCatalog'
        //     implementation and the alternate (my_bcec_ObjectCatalog)
        //     implementation.
        //
        //     (3) Invocation of 'catalog.verifyState'.
        //
        // Testing:
        //   int add(TYPE const& object);
        //   int remove(int handle, TYPE* valueBuffer=0);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_3;

        for (int i=0; i<NUM_SPECS; ++i) {
            bslma::TestAllocator ta(veryVeryVerbose);
            Obj o1(&ta);

            my_Obj o2;
            int len = static_cast<int>(strlen(SPECS[i]));
            vector<int> handles1(len, -1);
            vector<int> handles2(len, -1);

            if (veryVerbose) {
                cout << "\nbringing into state with spec = \""
                     << SPECS[i] << "\"\n";
                cout << "above spec corresponds to following state:\n" ;
                printSpec(SPECS[i]);
            }

            gg(&o1, handles1, &o2, handles2, SPECS[i]);
            if (veryVerbose) { cout << "brought into state\n"; }

            if (veryVerbose) { cout << "verifying o1\n"; }
            o1.verifyState();
            if (veryVerbose) { cout << "verifying o2\n"; }
            o2.verifyState();

            if (veryVerbose) {cout << "matching o1 and o2\n"; }
            verify(&o1, handles1, &o2, handles2, len);
        }
      }break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ALTERNATE IMPLEMENTATION:
        //   Verify the alternate implementation.
        //
        // Concerns:
        //   That the alternate implementation (named my_bcec_ObjectCatalog)
        //   for 'bdlcc::ObjectCatalog' is operational for one thread.
        //
        // Plan:
        //   Create a catalog.  Add 6 objects to it, verifying state after
        //   each addition.  Remove one of the objects and verify the state.
        //   Replace one of the object and verify the state.  Invoke
        //   'removeAll' and verify the state.
        //
        //   Create a catalog, add five elements and verify that 'isMember'
        //   works correctly.
        //
        // Testing:
        //   my_bcec_ObjectCatalog()
        //   ~my_bcec_ObjectCatalog()
        //   int my_bcec_ObjectCatalog::add(TYPE const& object)
        //   int my_bcec_ObjectCatalog::remove(int h, TYPE* valueBuffer=0)
        //   void my_bcec_ObjectCatalog::removeAll()
        //   int my_bcec_ObjectCatalog::replace(int h, TYPE const &newObject)
        //   int my_bcec_ObjectCatalog::find(int h, TYPE *p=0) const
        //   int my_bcec_ObjectCatalog::isMember(TYPE val) const
        //   int my_bcec_ObjectCatalog::length() const
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ALTERNATE IMPLEMENTATION" << endl
                          << "================================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_2;

        typedef my_bcec_ObjectCatalog<double> Obj;
        const double VA = 1.0;
        const double VB = 1.1;
        const double VC = 19.02;
        const double VD = 11.1902;
        const double VE = 10.20;
        const double VF = 111902.1020;
        const double VG = 121902.1020;

        Obj x1;
        const Obj &X1=x1;
        double vbuffer;

        int HA = x1.add(VA);
        ASSERT(1 == X1.length());
        ASSERT(0 == X1.find(HA));
        ASSERT(0 == X1.find(HA, &vbuffer));
        ASSERT(VA == vbuffer);

        int HB = x1.add(VB);
        ASSERT(0 != HB);
        ASSERT(2 == X1.length());
        ASSERT(0 == X1.find(HB, &vbuffer));
        ASSERT(VB == vbuffer);

        int HC = x1.add(VC);
        ASSERT(0 != HC);
        ASSERT(3 == X1.length());
        ASSERT(0 == X1.find(HC, &vbuffer));
        ASSERT(VC == vbuffer);

        int HD = x1.add(VD);
        ASSERT(0 != HD);
        ASSERT(4 == X1.length());
        ASSERT(0 == X1.find(HD, &vbuffer));
        ASSERT(VD == vbuffer);

        int HE = x1.add(VE);
        ASSERT(0 != HE);
        ASSERT(5 == X1.length());
        ASSERT(0 == X1.find(HE, &vbuffer));
        ASSERT(VE == vbuffer);

        int HF = x1.add(VF);
        ASSERT(0 != HF);
        ASSERT(6 == X1.length());
        ASSERT(0 == X1.find(HF, &vbuffer));
        ASSERT(VF == vbuffer);

        ASSERT(0 == x1.remove(HD, &vbuffer));
        ASSERT(VD == vbuffer);
        ASSERT(5 == X1.length());
        ASSERT(0 != x1.find(HD));

        ASSERT(0 == x1.replace(HE, VG));
        ASSERT(0 == x1.find(HE, &vbuffer));
        ASSERT(VG == vbuffer);

        x1.removeAll();
        ASSERT(0 == x1.length());

        // testing isMember
        {
            typedef my_bcec_ObjectCatalog<int> Obj;
            Obj x;
            x.add(1); x.add(2); x.add(3); x.add(4); x.add(5);

            ASSERT(x.isMember(1) == 1);
            ASSERT(x.isMember(2) == 1);
            ASSERT(x.isMember(3) == 1);
            ASSERT(x.isMember(4) == 1);
            ASSERT(x.isMember(5) == 1);

            ASSERT(x.isMember(0) == 0);
            ASSERT(x.isMember(6) == 0);
        }

      }break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create a catalog.  Add 6 objects to it, verifying state after
        //   each addition.  Remove one of the objects and verify the state.
        //   Replace one of the object and verify the state.  Invoke
        //   'removeAll' and verify the state.
        //
        //   Create a catalog.  Add 3 objects to it, invoke
        //   'removeAll(buffer)' and verify the state.
        //
        //   Create a catalog and an iterator for it.  Add 5 objects to the
        //   catalog and then iterate through it and finally verify the state.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_1;

        typedef bdlcc::ObjectCatalog<double> Obj;
        const double VA = 1.0;
        const double VB = 1.1;
        const double VC = 19.02;
        const double VD = 11.1902;
        const double VE = 10.20;
        const double VF = 111902.1020;
        const double VG = 121902.1020;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        const Obj &X1=x1;
        double vbuffer;

        if (verbose)
            cout << "testing add(value), length(), and find(h,buffer)" << endl;

        int HA = x1.add(VA);
        ASSERT(0 != HA);
        ASSERT(1 == X1.length());
        ASSERT(0 == X1.find(HA));
        ASSERT(0 == X1.find(HA, &vbuffer));
        ASSERT(VA == vbuffer);

        int HB = x1.add(VB);
        ASSERT(0 != HB);
        ASSERT(2 == X1.length());
        ASSERT(0 == X1.find(HB, &vbuffer));
        ASSERT(VB == vbuffer);

        int HC = x1.add(VC);
        ASSERT(0 != HC);
        ASSERT(3 == X1.length());
        ASSERT(0 == X1.find(HC, &vbuffer));
        ASSERT(VC == vbuffer);

        int HD = x1.add(VD);
        ASSERT(0 != HD);
        ASSERT(4 == X1.length());
        ASSERT(0 == X1.find(HD, &vbuffer));
        ASSERT(VD == vbuffer);

        int HE = x1.add(VE);
        ASSERT(0 != HE);
        ASSERT(5 == X1.length());
        ASSERT(0 == X1.find(HE, &vbuffer));
        ASSERT(VE == vbuffer);

        int HF = x1.add(VF);
        ASSERT(0 != HF);
        ASSERT(6 == X1.length());
        ASSERT(0 == X1.find(HF, &vbuffer));
        ASSERT(VF == vbuffer);

        ASSERT(0 == x1.remove(HD, &vbuffer));
        ASSERT(VD == vbuffer);
        ASSERT(5 == X1.length());
        ASSERT(0 != x1.find(HD));

        ASSERT(0 == x1.replace(HE, VG));
        ASSERT(0 == x1.find(HE, &vbuffer));
        ASSERT(VG == vbuffer);

        x1.removeAll();
        ASSERT(0 == x1.length());

        {
            if (verbose) cout << "testing removeAll(buffer)" << endl;

            Obj x1(&ta);
            bsl::vector<double> vec;
            x1.add(VA);
            x1.add(VB);
            x1.add(VC);
            x1.removeAll(&vec);
            ASSERT(vec.size() == 3);
            ASSERT(vec[0] == VA);
            ASSERT(vec[1] == VB);
            ASSERT(vec[2] == VC);
        }

        // testing iteration
        {
        typedef bdlcc::ObjectCatalog<int> Obj;
        typedef bdlcc::ObjectCatalogIter<int> Iter;
        int vbuffer;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        x1.add(1);  if (veryVerbose) { cout << "\tadd(1)\n"; }
        x1.add(2);  if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.add(4);  if (veryVerbose) { cout << "\tadd(4)\n"; }
        x1.add(8);  if (veryVerbose) { cout << "\tadd(8)\n"; }
        x1.add(16); if (veryVerbose) { cout << "\tadd(16)\n"; }

        if (veryVerbose) { cout << "\tnow iterate\n"; }

        int expectedSum = 1 + 2 + 4 + 8 + 16;
        int sum = 0;
        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            ASSERT(x1.find(p.first, &vbuffer) == 0);
            ASSERT(vbuffer == p.second);
            sum += (int)p.second;
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        LOOP2_ASSERT(sum, expectedSum, sum == expectedSum);
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
