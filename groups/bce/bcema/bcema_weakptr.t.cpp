// bcema_weakptr.t.cpp                                                -*-C++-*-

#include <bcema_weakptr.h>

#include <bcema_sharedptr.h>

#include <bsl_map.h>
#include <bsl_hash_map.h>
#include <bsl_list.h>
#include <bsl_iostream.h>

#include <bsl_string.h>
#include <bsl_cstdlib.h>     // atoi()

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bdet_date.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TEST PLAN
//
// This component provides a mechanism to create weak references to
// reference-counted shared objects (managed by 'bcema_SharedPtr'.  The
// functions supported by 'bcema_WeakPtr' include creating weak references
// (via multiple constructors), changing the weak pointer object being
// referenced (via the assignment operators), getting a shared pointer (via
// the 'acquireSharedPtr' and 'lock' functions), resetting the weak pointer
// (via 'reset'), and destroying the weak pointer.
//
// All the functions in this component are reasonably straight-forward and
// typically increment or decrement the number of strong or weak references as
// a side effect.  In addition the destructor and the reset functions may
// destroy the representation.  To test these functions we create a simple
// test representation that allows us to check the current strong and weak
// count and additionally stores the number of times the data value and the
// representation were attempted to be destroyed.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bcema_WeakPtr<TYPE>(const bcema_SharedPtr<TYPE>& original);
// [ 2] bcema_WeakPtr<TYPE>(const bcema_WeakPtr<TYPE>& original);
// [ 2] template <typename BCE_OTHER_TYPE>
//      bcema_WeakPtr(const bcema_SharedPtr<BCE_OTHER_TYPE>& original);
// [ 2] template <typename BCE_OTHER_TYPE>
//      bcema_WeakPtr(const bcema_WeakPtr<BCE_OTHER_TYPE>& original);
// [ 2] ~bcema_WeakPtr();
//
// MANIPULATORS
// [ 3] WeakPtr<TYPE>& operator=(const SharedPtr<TYPE>& original);
// [ 3] WeakPtr<TYPE>& operator=(const WeakPtr<TYPE>& original);
// [ 3] template <typename OTHER_TYPE>
//      WeakPtr<TYPE>& operator=(const SharedPtr<OTHER_TYPE>& original);
// [ 3] template <typename OTHER_TYPE>
//      WeakPtr<TYPE>& operator=(const WeakPtr<OTHER_TYPE>& original);
// [ 4] void reset();
// [ 5] bcema_SharedPtr<TYPE> acquireSharedPtr();
// [ 6] void swap(bcema_WeakPtr<TYPE>& src);
//
// ACCESSORS
// [ 2] operator bcema_SharedPtr_UnspecifiedBool() const;
// [ 2] int numReferences() const;
// [ 2] bool isValid() const;
// [ 2] bcema_SharedPtrRep *rep() const;
// [ 2] int use_count() const;
// [ 2] bool expired() const;
// [ 5] bcema_SharedPtr<TYPE> lock() const;
// [ 7] bool owner_before(const bcema_SharedPtr<OTHER_TYPE>& rhs);
// [ 7] bool owner_before(const bcema_WeakPtr<OTHER_TYPE>& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE TEST

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;
class MyTestBaseObject;
class MyTestDerivedObject;

                   // *** 'MyTestObject' CLASS HIERARCHY ***

                           // ======================
                           // class MyTestBaseObject
                           // ======================

class MyTestBaseObject {
    // A simple base class.

  public:
    // CREATORS
    MyTestBaseObject() {}
    virtual ~MyTestBaseObject() {}
};

                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject {
    // A typical test class.

  public:
    // CREATORS
    MyTestObject() {}
    ~MyTestObject() {}
};

                         // =========================
                         // class MyTestDerivedObject
                         // =========================

class MyTestDerivedObject : public MyTestBaseObject {
    // A typical derived class.

  public:
    // CREATORS
    MyTestDerivedObject() {}
};

                        // ======================
                        // class TestSharedPtrRep
                        // ======================

template <typename TYPE>
class TestSharedPtrRep : public bcema_SharedPtrRep {
    // Partially implemented shared pointer representation ("letter") protocol.
    // This class provides a reference counter and a concrete implementation of
    // the 'bcema_Deleter' protocol that decrements the number references and
    // destroys itself if the number of references reaches zero.

    // DATA
    TYPE            *d_dataPtr_p;          // data ptr

    int              d_disposeRepCount;    // counter storing number of time
                                           // release is called

    int              d_disposeObjectCount; // counter storing number of time
                                           // releaseValue is called

    bslma_Allocator *d_allocator_p;        // allocator

  public:
    // CREATORS
    TestSharedPtrRep(bslma_Allocator *basicAllocator);
        // Construct a test shared ptr rep object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~TestSharedPtrRep();
        // Destroy this test shared ptr rep object.

    // MANIPULATORS
    virtual void disposeRep();
        // Release this representation.

    virtual void disposeObject();
        // Release the value stored by this representation.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the original pointer stored by this representation.

    TYPE *ptr() const;
        // Return the data pointer stored by this representation.

    int disposeRepCount() const;
        // Return the number of time 'release' was called.

    int disposeObjectCount() const;
        // Return the number of time 'releaseValue' was called.
};

                        // ------------------------
                        // class bcema_SharedPtrRep
                        // ------------------------

// CREATORS
template <typename TYPE>
inline
TestSharedPtrRep<TYPE>::TestSharedPtrRep(bslma_Allocator *basicAllocator)
: d_dataPtr_p(0)
, d_disposeRepCount(0)
, d_disposeObjectCount(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_dataPtr_p = new (*d_allocator_p) TYPE();
}

template <typename TYPE>
TestSharedPtrRep<TYPE>::~TestSharedPtrRep()
{
    LOOP_ASSERT(numReferences(),      0 == numReferences());
    LOOP_ASSERT(d_disposeRepCount,    1 == d_disposeRepCount);
    LOOP_ASSERT(d_disposeObjectCount, 1 == d_disposeObjectCount);
}

// MANIPULATORS
template <typename TYPE>
inline
void TestSharedPtrRep<TYPE>::disposeRep()
{
    ++d_disposeRepCount;
}

template <typename TYPE>
inline
void TestSharedPtrRep<TYPE>::disposeObject()
{
    ++d_disposeObjectCount;
    d_allocator_p->deleteObject(d_dataPtr_p);
}

// ACCESSORS
template <typename TYPE>
inline
void *TestSharedPtrRep<TYPE>::originalPtr() const
{
    return (void *) d_dataPtr_p;
}

template <typename TYPE>
inline
TYPE *TestSharedPtrRep<TYPE>::ptr() const
{
    return d_dataPtr_p;
}

template <typename TYPE>
inline
int TestSharedPtrRep<TYPE>::disposeRepCount() const
{
    return d_disposeRepCount;
}

template <typename TYPE>
inline
int TestSharedPtrRep<TYPE>::disposeObjectCount() const
{
    return d_disposeObjectCount;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Example 2 - Breaking cyclical dependencies
// - - - - - - - - - - - - - - - - - - - - - -
// Weak pointers are frequently used to break cyclical dependencies between
// objects that store references to each other via a shared pointer.  Consider
// for example a simplified news alert system that sends news alerts to users
// based on keywords that they register for.  The user information is stored
// in the 'User' class and the details of the news alert are stored in the
// 'Alert' class.  The class definitions for 'User' and 'Alert' are provided
// below (with any code not relevant to this example elided):
//..
    class Alert;
//
    class User {
        // This class stores the user information required for listening to
        // alerts.
//
        bsl::vector<bcema_SharedPtr<Alert> > d_alerts;  // alerts user is
                                                        // registered for
//
        // ...
//
      public:
        // MANIPULATORS
        void addAlert(const bcema_SharedPtr<Alert>& alertPtr) {
            // Add the specified 'alertPtr' to the list of alerts being
            // monitored by this user.

            d_alerts.push_back(alertPtr);
        }

//
        // ...
    };
//..
// Now we define an alert class, 'Alert':
//..
    class Alert {
        // This class stores the alert information required for sending
        // alerts.
//
        bsl::vector<bcema_SharedPtr<User> > d_users;  // users registered
                                                      // for this alert
//
      public:
        // MANIPULATORS
        void addUser(const bcema_SharedPtr<User>& userPtr) {
            // Add the specified 'userPtr' to the list of users monitoring this
            // alert.

            d_users.push_back(userPtr);
        }
//
        // ...
    };

//..
// Even though we have released 'alertPtr' and 'userPtr' there still exists a
// cyclic reference between the two objects, so none of the objects are
// destroyed.
//
// We can break this cyclical dependency we define a modified alert class
// 'ModifiedAlert' that stores a weak pointer to a 'ModifiedUser' object.
// Below is the definition for the 'ModifiedUser' class which is identical to
// the 'User' class, the only difference being that it stores shared pointer to
// 'ModifiedAlert's instead of 'Alert's:
//..
    class ModifiedAlert;
//
    class ModifiedUser {
        // This class stores the user information required for listening to
        // alerts.
//
        bsl::vector<bcema_SharedPtr<ModifiedAlert> > d_alerts;// alerts user is
                                                              // registered for
//
        // ...
//
      public:
        // MANIPULATORS
        void addAlert(const bcema_SharedPtr<ModifiedAlert>& alertPtr) {
            // Add the specified 'alertPtr' to the list of alerts being
            // monitored by this user.

            d_alerts.push_back(alertPtr);
        }

//
        // ...
    };
//..
// Now we define the 'ModifiedAlert' class:
//..
    class ModifiedAlert {
        // This class stores the alert information required for sending
        // alerts.
//
//..
// Note that the user is stored by a weak pointer instead of by a shared
// pointer:
//..
        bsl::vector<bcema_WeakPtr<ModifiedUser> > d_users;  // users registered
                                                            // for this alert
//
      public:
        // MANIPULATORS
        void addUser(const bcema_WeakPtr<ModifiedUser>& userPtr) {
            // Add the specified 'userPtr' to the list of users monitoring this
            // alert.

            d_users.push_back(userPtr);
        }
//
        // ...
    };

// Usage example 3 - Caching example
// - - - - - - - - - - - - - - - - -
// Suppose we want to implement a peer to peer file sharing system that allows
// users to search for files that match specific keywords.  A simplistic
// version of such a system with code not relevant to the usage example
// elided would have the following parts: 
//
// a) A peer manager class that maintains a list of all connected peers and
// updates the list based on incoming peer requests and disconnecting peers.
// The following would be a simple interface for the Peer and PeerManager
// classes:
//..
    class Peer {
        // This class stores all the relevant information for a peer.

        // ...
    };

    class PeerManager {
        // This class acts as a manager of peers and adds and removes peers
        // based on peer requests and disconnections.

        // DATA
//..
// The peer objects are stored by shared pointer to allow peers to be passed
// to search results and still allow their asynchronous destruction when peers
// disconnect.
//..
        bsl::map<int, bcema_SharedPtr<Peer> > d_peers;

        // ...
    };
//..
// b) A peer cache class that stores a subset of the peers that are used for
// sending search requests.  The cache may select peers based on their
// connection bandwidth, relevancy of previous search results, etc.  For
// brevity the population and flushing of this cache is not shown:
//..
    class PeerCache {
        // This class caches a subset of all peers that match certain criteria
        // including connection bandwidth, relevancy of previous search
        // results, etc.

//..
// Note that the cached peers are stored as a weak pointer so as not to
// interfere with the cleanup of Peer objects by the PeerManager if a Peer
// goes down.
//..
        // DATA
        bsl::list<bcema_WeakPtr<Peer> > d_cachedPeers;

      public:
        // TYPES
        typedef bsl::list<bcema_WeakPtr<Peer> >::const_iterator PeerConstIter;

        // ...

        // ACCESSORS
        PeerConstIter begin() const { return d_cachedPeers.begin(); }
        PeerConstIter end() const   { return d_cachedPeers.end(); }
    };
//..
// c) A search result class that stores a search result and encapsulates a peer
// with the file name stored by the peer that best matches the specified
// keywords:
//..
    class SearchResult {
        // This class provides a search result and encapsulates a particular
        // peer and filename combination that matches a specified set of
        // keywords.

//..
// The peer is stored as a weak pointer because when the user decides to
// select a particular file to download from this peer, the peer might have
// disconnected.
//..
        // DATA
        bcema_WeakPtr<Peer> d_peer;
        bsl::string         d_filename;

      public:
        // CREATORS
        SearchResult(const bcema_WeakPtr<Peer>& peer,
                     const bsl::string&         filename)
        : d_peer(peer)
        , d_filename(filename)
        {
        }

        // ...

        // ACCESSORS
        const bcema_WeakPtr<Peer>& peer() const { return d_peer; }
        const bsl::string& filename() const { return d_filename; }
    };
//..
// d) A search function that takes a list of keywords and returns available
// results by searching the cached peers:
//..
    PeerCache peerCache;

    void search(bsl::vector<SearchResult>       *results,
                const bsl::vector<bsl::string>&  keywords)
    {
        for (PeerCache::PeerConstIter iter = peerCache.begin();
             iter != peerCache.end();
             ++iter) {
//..
// First we check if the peer is still connected by acquiring a shared pointer
// to the peer.  If the acquire operation succeeds then we can send the peer a
// request to send back the file best matching the specified keywords:
//..            
            bcema_SharedPtr<Peer> peerSharedPtr = iter->acquireSharedPtr();
            if (peerSharedPtr) {

                // Search the peer for file best matching the specified
                // keywords and if a file is found add the returned
                // SearchResult object to result.

                // ...
            }
        }
    }
//..
// e) A download function that downloads a file selected by the user:
//..
    void download(const SearchResult& result)
    {
        bcema_SharedPtr<Peer> peerSharedPtr = result.peer().acquireSharedPtr();
        if (peerSharedPtr) {
            // Download the result.filename() file from peer knowing that
            // the peer is still connected.
        }
    }
//..

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    typedef bcema_WeakPtr<MyTestObject>           ObjWP;
    typedef bcema_SharedPtr<MyTestObject>         ObjSP;
    typedef bcema_WeakPtr<MyTestBaseObject>       BaseWP;
    typedef bcema_WeakPtr<MyTestDerivedObject>    DerivedWP;
    typedef bcema_SharedPtr<MyTestDerivedObject>  DerivedSP;

    switch (test) { case 0:

      case 8: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.  This usage
        //   test also happens to exhaustively test the entire component
        //   and is thus the only test in the suite.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //   Test each enumeration type by assigning a variable the value
        //   of each enumeration constant and verifying that the integral
        //   value of the variable after assignment is as expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
///Example 1 - Basic usage
///- - - - - - - - - - - -
// This example illustrates the basic syntax to create and use a
// 'bcema_WeakPtr'.  Lets suppose that we want to construct a weak pointer that
// references an 'int' managed by a shared pointer.  First we define the shared
// pointer and assign a value to the shared 'int':
//..
    bcema_SharedPtr<int> intPtr;
    intPtr.createInplace(bslma_Default::allocator());
    *intPtr = 10;
    ASSERT(10 == *intPtr);
//..
// Now we construct a weak pointer to the 'int':
//..
    bcema_WeakPtr<int> intWeakPtr(intPtr);
    ASSERT(!intWeakPtr.expired());
//..
// 'bcema_WeakPtr' does not provide direct access to the shared object being
// referenced.  So to access and manipulate the 'int' from the weak pointer we
// have to get the shared pointer from it:
//..
    bcema_SharedPtr<int> intPtr2 = intWeakPtr.acquireSharedPtr();
    ASSERT(intPtr2);
    ASSERT(10 == *intPtr2);
//
    *intPtr2 = 20;
    ASSERT(20 == *intPtr);
    ASSERT(20 == *intPtr2);
//..
// We can remove the weak reference to the shared 'int' by calling the 'reset'
// function:
//..
    intWeakPtr.reset();
    ASSERT(intWeakPtr.expired());
//..
// Note that resetting the weak pointer does not affect the shared pointers
// referencing the 'int' object:
//..
    ASSERT(20 == *intPtr);
    ASSERT(20 == *intPtr2);
//..
// Finally, we construct another weak pointer referencing the shared 'int':
//..
    bcema_WeakPtr<int> intWeakPtr2(intPtr);
    ASSERT(!intWeakPtr2.expired());
//..
// We now 'release' all shared references to the 'int'.  This causes the weak
// pointer to be 'expired' and any attempt to get a shared pointer from it
// will return an empty shared pointer:
//..
    intPtr.reset();
    intPtr2.reset();
    ASSERT(intWeakPtr2.expired());
    ASSERT(!intWeakPtr2.acquireSharedPtr());
        }
//..
// Example 2 - Breaking cyclical dependencies
// - - - - - - - - - - - - - - - - - - - - - -
//..
// Note that the 'User' and 'Alert' classes could typically be used as
// follows:
//..
        bslma_TestAllocator ta;
        {
            ta.setQuiet(1);

            bcema_SharedPtr<User> userPtr;
            userPtr.createInplace(&ta);
//
            bcema_SharedPtr<Alert> alertPtr;
            alertPtr.createInplace(&ta);
//
            alertPtr->addUser(userPtr);
            userPtr->addAlert(alertPtr);
//
            alertPtr.reset();
            userPtr.reset();

        }
//
//      // MEMORY LEAK !!
//
        {
//
            bcema_SharedPtr<ModifiedAlert> alertPtr;
            alertPtr.createInplace(&ta);
//
            bcema_SharedPtr<ModifiedUser> userPtr;
            userPtr.createInplace(&ta);
//
            bcema_WeakPtr<ModifiedUser> userWeakPtr(userPtr);
//
            alertPtr->addUser(userWeakPtr);
            userPtr->addAlert(alertPtr);
//
            alertPtr.reset();
            userPtr.reset();
        }
//
//  // No memory leak now
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST 'owner_before' FUNCTION:
        //
        // Concerns:
        //   Test that the 'owner_before' function works as expected.
        //
        // Plan:
        //
        // Testing:
        //   bool owner_before(const bcema_SharedPtr<OTHER_TYPE>& rhs);
        //   bool owner_before(const bcema_WeakPtr<OTHER_TYPE>& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'owner_before'" << endl
                                  << "======================" << endl;

        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep1(&ta);
            const TestSharedPtrRep<MyTestObject>& REP1 = rep1;
            TestSharedPtrRep<MyTestObject> rep2(&ta);
            const TestSharedPtrRep<MyTestObject>& REP2 = rep2;
            MyTestObject *PTR1 = REP1.ptr();
            MyTestObject *PTR2 = REP2.ptr();
            {
                const ObjWP EWP1;
                const ObjWP EWP2;
                const ObjSP ESP;

                ObjSP mSA(PTR1, &rep1); const ObjSP& SA = mSA;
                ObjSP mSB(PTR2, &rep2); const ObjSP& SB = mSB;

                ObjWP mWA(SA); const ObjWP& WA = mWA;
                ObjWP mWB(SB); const ObjWP& WB = mWB;

                ASSERT(false == EWP1.owner_before(EWP1));
                ASSERT(false == EWP1.owner_before(EWP2));
                ASSERT(false == EWP1.owner_before(ESP));
                ASSERT(true  == EWP1.owner_before(SA));
                ASSERT(true  == EWP1.owner_before(WA));
                ASSERT(true  == EWP1.owner_before(SB));
                ASSERT(true  == EWP1.owner_before(WB));

                ASSERT(false == WA.owner_before(EWP1));
                ASSERT(false == WA.owner_before(ESP));
                ASSERT(false == WA.owner_before(SA));
                ASSERT(false == WA.owner_before(WA));
                ASSERT(WA.owner_before(SB) == (&REP1 < &REP2));
                ASSERT(WA.owner_before(WB) == (&REP1 < &REP2));
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST 'swap' FUNCTION:
        //
        // Concerns:
        //   Test that the 'swap' function works as expected.
        //
        // Plan:
        //
        // Testing:
        //   void swap(bcema_WeakPtr<TYPE>& src);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'swap'" << endl
                                  << "==============" << endl;

        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep(&ta);
            const TestSharedPtrRep<MyTestObject>& REP = rep;
            MyTestObject *PTR = REP.ptr();
            {
                ObjWP mX; const ObjWP& X = mX;
                ObjWP mY; const ObjWP& Y = mY;

                ASSERT(X.expired());
                ASSERT(0 == X.rep());

                ASSERT(Y.expired());
                ASSERT(0 == Y.rep());

                mX.swap(mY);

                ASSERT(X.expired());
                ASSERT(0 == X.rep());

                ASSERT(Y.expired());
                ASSERT(0 == Y.rep());

                ObjSP mS(PTR, &rep);
                const ObjSP& S = mS;

                mX = S;

                ASSERT(!X.expired());
                ASSERT(&REP == X.rep());
                ASSERT(1 == REP.numReferences());
                ASSERT(1 == REP.numWeakReferences());

                mX.swap(mY);
                ASSERT(X.expired());
                ASSERT(!Y.expired());
                ASSERT(0    == X.rep());
                ASSERT(&REP == Y.rep());
                ASSERT(1 == REP.numReferences());
            }
            LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),
                        1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }

        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep1(&ta);
            const TestSharedPtrRep<MyTestObject>& REP1 = rep1;
            TestSharedPtrRep<MyTestObject> rep2(&ta);
            const TestSharedPtrRep<MyTestObject>& REP2 = rep2;
            MyTestObject *PTR1 = REP1.ptr();
            MyTestObject *PTR2 = REP2.ptr();
            {
                ObjSP mS1(PTR1, &rep1);
                const ObjSP& S1 = mS1;

                ObjSP mS2(PTR2, &rep2);
                const ObjSP& S2 = mS2;

                ObjWP mX(S1); const ObjWP& X = mX;
                ObjWP mY(S2); const ObjWP& Y = mY;

                ASSERT(&REP1  == X.rep());
                ASSERT(&REP2  == Y.rep());

                LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());
                LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());

                mX.swap(mY);

                ASSERT(&REP2  == X.rep());
                ASSERT(&REP1  == Y.rep());

                LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());
                LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());
            }

            LOOP_ASSERT(REP1.numReferences(), 0 == REP1.numReferences());
            LOOP_ASSERT(REP1.disposeRepCount(), 1 == REP1.disposeRepCount());
            LOOP_ASSERT(REP1.disposeObjectCount(),
                                               1 == REP1.disposeObjectCount());

            LOOP_ASSERT(REP2.numReferences(), 0 == REP2.numReferences());
            LOOP_ASSERT(REP2.disposeRepCount(), 1 == REP2.disposeRepCount());
            LOOP_ASSERT(REP2.disposeObjectCount(),
                                               1 == REP2.disposeObjectCount());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST 'acquireSharedPtr' and 'lock' FUNCTIONS:
        //
        // Concerns:
        //   Test that the 'acquireSharedPtr' and 'lock' work as expected.
        //
        // Plan:
        //
        // Testing:
        //   bcema_SharedPtr<TYPE> acquireSharedPtr();
        //   bcema_SharedPtr<TYPE> lock() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'acquireSharedPtr' and 'lock'" << endl
                          << "=================================" << endl;

        {
            ObjWP mX; const ObjWP& X = mX;
            ObjWP mY; const ObjWP& Y = mY;

            ASSERT(X.expired());
            ASSERT(0 == X.rep());

            ASSERT(Y.expired());
            ASSERT(0 == Y.rep());

            ObjSP S1 = mX.acquireSharedPtr();
            ASSERT(X.expired());
            ASSERT(0 == X.rep());
            ASSERT(!S1);
            ASSERT(0 == S1.ptr());
            ASSERT(0 == S1.rep());

            ObjSP S2 = Y.lock();
            ASSERT(Y.expired());
            ASSERT(0 == Y.rep());
            ASSERT(!S2);
            ASSERT(0 == S2.ptr());
            ASSERT(0 == S2.rep());
        }

        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep(&ta);
            const TestSharedPtrRep<MyTestObject>& REP = rep;
            MyTestObject *PTR = REP.ptr();

            ObjSP SC;
            {
                ObjSP mS(PTR, &rep);
                const ObjSP& S = mS;

                ObjWP mX(S); const ObjWP& X = mX;
                ObjWP mY(S); const ObjWP& Y = mY;
                LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());

                ObjSP SA = X.acquireSharedPtr();
                ASSERT(SA);
                ASSERT(PTR  == SA.ptr());
                ASSERT(&REP == SA.rep());
                LOOP_ASSERT(REP.numReferences(), 2 == REP.numReferences());

                ObjSP SB = Y.lock();
                ASSERT(SB);
                ASSERT(PTR  == SB.ptr());
                ASSERT(&REP == SB.rep());

                LOOP_ASSERT(REP.numReferences(),   3 == REP.numReferences());

                SC = X.acquireSharedPtr();
                LOOP_ASSERT(REP.numReferences(),   4 == REP.numReferences());
                ASSERT(PTR  == SC.ptr());
                ASSERT(&REP == SC.rep());
            }

            LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        0 == REP.disposeObjectCount());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST 'reset' FUNCTIONS:
        //
        // Concerns:
        //   Test that the 'reset' works as expected.
        //
        // Plan:
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'reset'" << endl
                                  << "===============" << endl;

        {
            ObjWP mX; const ObjWP& X = mX;
            ObjWP mY; const ObjWP& Y = mY;

            ASSERT(X.expired());
            ASSERT(0 == X.rep());

            ASSERT(Y.expired());
            ASSERT(0 == Y.rep());

            mY.reset();
            ASSERT(Y.expired());
            ASSERT(0 == Y.rep());
        }

        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep(&ta);
            const TestSharedPtrRep<MyTestObject>& REP = rep;
            LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        0 == REP.disposeObjectCount());
            MyTestObject *PTR = REP.ptr();
            {
                ObjWP mZ; const ObjWP& Z = mZ;
                ASSERT(0 == Z.rep());
                {
                    ObjSP mS(PTR, &rep); const ObjSP& S = mS;
                    {
                        ObjWP mX(S); const ObjWP& X = mX;
                        ObjWP mY(S); const ObjWP& Y = mY;
                        LOOP_ASSERT(REP.numReferences(),
                                                     1 == REP.numReferences());
                        LOOP_ASSERT(REP.disposeRepCount(),
                                                   0 == REP.disposeRepCount());
                        LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
                        ASSERT(&REP == X.rep());
                        ASSERT(&REP == Y.rep());

                        mX.reset();
                        LOOP_ASSERT(REP.numReferences(),
                                                     1 == REP.numReferences());
                        LOOP_ASSERT(REP.disposeRepCount(),
                                                   0 == REP.disposeRepCount());
                        LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
                        ASSERT(0    == X.rep());
                        ASSERT(&REP == Y.rep());

                        mY.reset();
                        LOOP_ASSERT(REP.numReferences(),
                                                     1 == REP.numReferences());
                        LOOP_ASSERT(REP.disposeRepCount(),
                                                   0 == REP.disposeRepCount());
                        LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
                        ASSERT(0 == X.rep());
                        ASSERT(0 == Y.rep());
                    }

                    LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                    LOOP_ASSERT(REP.disposeRepCount(),
                                0 == REP.disposeRepCount());
                    LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                    mZ = S;
                    LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                    LOOP_ASSERT(REP.disposeRepCount(),
                                0 == REP.disposeRepCount());
                    LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
                    ASSERT(&REP == Z.rep());
                }

                LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                1 == REP.disposeObjectCount());

                mZ.reset();
                LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 1 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                1 == REP.disposeObjectCount());
                ASSERT(0 == Z.rep());
            }

            LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),  1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST ASSIGNMENT OPERATORS:
        //
        // Concerns:
        //   Test that the assignment operators work as expected.
        //
        // Plan:
        //
        // Testing:
        //   WeakPtr<TYPE>& operator=(const SharedPtr<TYPE>& original);
        //   WeakPtr<TYPE>& operator=(const WeakPtr<TYPE>& original);
        //   template <typename OTHER_TYPE>
        //   WeakPtr<TYPE>& operator=(const SharedPtr<OTHER_TYPE>& original);
        //   template <typename OTHER_TYPE>
        //   WeakPtr<TYPE>& operator=(const WeakPtr<OTHER_TYPE>& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATORS" << endl
                                  << "============================" << endl;

        if (verbose) cout << endl
                          << "Testing assignment from same TYPE" << endl
                          << "---------------------------------" << endl;
        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep(&ta);
            const TestSharedPtrRep<MyTestObject>& REP = rep;
            MyTestObject *PTR = REP.ptr();
            {
                ObjSP mS(PTR, &rep); const ObjSP& S = mS;

                ObjWP mX; const ObjWP& X = mX;
                ObjWP mY; const ObjWP& Y = mY;
                ObjWP mZ; const ObjWP& Z = mZ;

                ASSERT(1 == REP.numReferences());

                mX = S;
                ASSERT(!X.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(&REP == X.rep());

                mY = X;
                ASSERT(!Y.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(&REP == Y.rep());

                mY = Z;
                ASSERT(Y.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(0 == Y.rep());
            }

            LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }

        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep1(&ta);
            const TestSharedPtrRep<MyTestObject>& REP1 = rep1;
            TestSharedPtrRep<MyTestObject> rep2(&ta);
            const TestSharedPtrRep<MyTestObject>& REP2 = rep2;
            MyTestObject *PTR1 = REP1.ptr();
            MyTestObject *PTR2 = REP2.ptr();
            {
                ObjSP mS1(PTR1, &rep1); const ObjSP& S1 = mS1;
                ObjSP mS2(PTR2, &rep2); const ObjSP& S2 = mS2;

                ObjWP mX(S1); const ObjWP& X = mX;
                LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());
                ASSERT(&REP1 == X.rep());

                ObjWP mY(S2); const ObjWP& Y = mY;
                ObjWP mZ(S2); const ObjWP& Z = mZ;
                ASSERT(&REP2 == Y.rep());
                ASSERT(&REP2 == Z.rep());

                LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());

                mY = S1;
                LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());

                LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());
                ASSERT(&REP1 == Y.rep());

                mZ = X;
                LOOP_ASSERT(REP1.numReferences(), 1 == REP1.numReferences());

                LOOP_ASSERT(REP2.numReferences(), 1 == REP2.numReferences());
                ASSERT(&REP1 == Z.rep());
            }
            LOOP_ASSERT(REP1.numReferences(), 0 == REP1.numReferences());

            LOOP_ASSERT(REP2.numReferences(), 0 == REP2.numReferences());
        }

        if (verbose) cout << endl
                          << "Testing different TYPE operators" << endl
                          << "--------------------------------" << endl;
        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestDerivedObject> rep(&ta);
            const TestSharedPtrRep<MyTestDerivedObject>& REP = rep;
            MyTestDerivedObject *PTR = REP.ptr();
            {
                DerivedSP mS1(PTR, &rep);
                const DerivedSP& S1 = mS1;
                DerivedSP mS2; const DerivedSP& S2 = mS2;

                DerivedWP mC1(S1); const DerivedWP& C1 = mC1;
                ASSERT(&REP == C1.rep());

                DerivedWP mC2; const DerivedWP& C2 = mC2;
                ASSERT(0 == C2.rep());

                BaseWP mX; const BaseWP& X = mX;
                BaseWP mY; const BaseWP& Y = mY;
                ASSERT(0 == X.rep());
                ASSERT(0 == Y.rep());
                
                mX = S1;
                ASSERT(!X.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(&REP == X.rep());

                mY = C1;
                ASSERT(!Y.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(&REP == Y.rep());

                mX = S2;
                ASSERT( X.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(0 == X.rep());

                mY = C2;
                ASSERT( Y.expired());
                ASSERT(1 == REP.numReferences());
                ASSERT(0 == Y.rep());
            }

            LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }
        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestDerivedObject> rep1(&ta);
            const TestSharedPtrRep<MyTestDerivedObject>& REP1 = rep1;
            TestSharedPtrRep<MyTestDerivedObject> rep2(&ta);
            const TestSharedPtrRep<MyTestDerivedObject>& REP2 = rep2;
            MyTestDerivedObject *PTR1 = REP1.ptr();
            MyTestDerivedObject *PTR2 = REP2.ptr();
            {
                DerivedSP mS1(PTR1, &rep1);
                const DerivedSP& S1 = mS1;
                DerivedSP mS2(PTR2, &rep2);
                const DerivedSP& S2 = mS2;

                BaseWP mX(S1); const BaseWP& X = mX;
                LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());
                ASSERT(&REP1 == X.rep());

                mX = S2;
                LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());

                LOOP_ASSERT(REP2.numReferences(),   1 == REP2.numReferences());
                ASSERT(&REP2 == X.rep());

                BaseWP mY(S1); const BaseWP& Y = mY;
                LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());
                ASSERT(&REP1 == Y.rep());

                mY = X;
                LOOP_ASSERT(REP1.numReferences(),   1 == REP1.numReferences());

                LOOP_ASSERT(REP2.numReferences(),   1 == REP2.numReferences());
                ASSERT(&REP2 == Y.rep());
            }

            LOOP_ASSERT(REP1.numReferences(), 0 == REP1.numReferences());
            LOOP_ASSERT(REP1.disposeRepCount(), 1 == REP1.disposeRepCount());
            LOOP_ASSERT(REP1.disposeObjectCount(),
                                               1 == REP1.disposeObjectCount());

            LOOP_ASSERT(REP2.numReferences(), 0 == REP2.numReferences());
            LOOP_ASSERT(REP2.disposeRepCount(), 1 == REP2.disposeRepCount());
            LOOP_ASSERT(REP2.disposeObjectCount(),
                                               1 == REP2.disposeObjectCount());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST CREATORS AND ACCESSORS:
        //
        // Concerns:
        //   Test that the creators work as expected.
        //
        // Plan:
        //
        // Testing:
        //   bcema_WeakPtr<TYPE>(const bcema_SharedPtr<TYPE>& original);
        //   bcema_WeakPtr<TYPE>(const bcema_WeakPtr<TYPE>& original);
        //   template <typename BCE_OTHER_TYPE>
        //   bcema_WeakPtr(const bcema_SharedPtr<BCE_OTHER_TYPE>& original);
        //   template <typename BCE_OTHER_TYPE>
        //   bcema_WeakPtr(const bcema_WeakPtr<BCE_OTHER_TYPE>& original);
        //   ~bcema_WeakPtr();
        //   operator bcema_SharedPtr_UnspecifiedBool() const;
        //   int numReferences() const;
        //   bcema_SharedPtrRep *rep() const;
        //   int use_count() const;
        //   bool expired() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING CREATORS" << endl
                                  << "================" << endl;

        if (verbose) cout << endl
                          << "Testing default constructor" << endl
                          << "---------------------------" << endl;
        {
            ObjWP mX; const ObjWP& X = mX;
            ASSERT(X.expired());
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.use_count());
            ASSERT(0 == X.rep());
            ObjSP mS1 = mX.acquireSharedPtr(); const ObjSP& S1 = mS1;
            ObjSP mS2 = X.lock(); const ObjSP& S2 = mS2;

            ASSERT(!S1);
            ASSERT(0 == S1.ptr());
            ASSERT(!S2);
            ASSERT(0 == S2.ptr());
        }

        if (verbose) cout << endl
                          << "Testing same TYPE constructors" << endl
                          << "------------------------------" << endl;
        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep(&ta);
            const TestSharedPtrRep<MyTestObject>& REP = rep;
            LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        0 == REP.disposeObjectCount());
            MyTestObject *PTR = REP.ptr();
            {
                ObjSP mS(PTR, &rep);
                const ObjSP& S = mS;
                LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                ObjWP mX(S); const ObjWP& X = mX;
                ASSERT(!X.expired());
                ASSERT(&REP == X.rep());
                LOOP_ASSERT(X.numReferences(),     1 == X.numReferences());
                LOOP_ASSERT(X.use_count(),         1 == X.use_count());
                LOOP_ASSERT(REP.numReferences(),   1 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                ObjWP mY(X); const ObjWP& Y = mY;
                ASSERT(!Y.expired());
                ASSERT(&REP == Y.rep());
                LOOP_ASSERT(Y.numReferences(),     1 == Y.numReferences());
                LOOP_ASSERT(Y.use_count(),         1 == Y.use_count());

                LOOP_ASSERT(X.numReferences(),     1 == X.numReferences());
                LOOP_ASSERT(X.use_count(),         1 == X.use_count());

                LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
            }
            LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }

        if (verbose) cout << endl
                          << "Testing different TYPE constructors" << endl
                          << "-----------------------------------" << endl;
        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestDerivedObject> rep(&ta);
            const TestSharedPtrRep<MyTestDerivedObject>& REP = rep;
            LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        0 == REP.disposeObjectCount());
            MyTestDerivedObject *PTR = REP.ptr();
            {
                DerivedSP mS(PTR, &rep);
                const DerivedSP& S = mS;
                LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                BaseWP mX(S); const BaseWP& X = mX;
                ASSERT(!X.expired());
                ASSERT(&REP == X.rep());
                LOOP_ASSERT(X.numReferences(),     1 == X.numReferences());
                LOOP_ASSERT(X.use_count(),         1 == X.use_count());
                LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                DerivedWP mZ(S); const DerivedWP& Z = mZ;
                BaseWP mY(Z); const BaseWP& Y = mY;
                ASSERT(!Y.expired());
                ASSERT(&REP == Y.rep());
                LOOP_ASSERT(Y.numReferences(),     1 == Y.numReferences());
                LOOP_ASSERT(Y.use_count(),         1 == Y.use_count());

                LOOP_ASSERT(X.numReferences(),     1 == X.numReferences());
                LOOP_ASSERT(X.use_count(),         1 == X.use_count());

                LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(), 0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
            }
            LOOP_ASSERT(REP.numReferences(),     0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }

        if (verbose) cout << endl
                          << "Testing destructor" << endl
                          << "------------------" << endl;
        {
            bslma_TestAllocator ta;
            TestSharedPtrRep<MyTestObject> rep(&ta);
            const TestSharedPtrRep<MyTestObject>& REP = rep;
            LOOP_ASSERT(REP.numReferences(),     1 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),   0 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        0 == REP.disposeObjectCount());
            MyTestObject *PTR = REP.ptr();
            {
                ObjWP mY; const ObjWP& Y = mY;
                {
                    ObjSP mS(PTR, &rep); const ObjSP& S = mS;
                    LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                    LOOP_ASSERT(REP.disposeRepCount(), 
                                0 == REP.disposeRepCount());
                    LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                    {
                        ObjWP mX(S); const ObjWP& X = mX;
                        LOOP_ASSERT(REP.numReferences(),
                                                     1 == REP.numReferences());
                        LOOP_ASSERT(REP.disposeRepCount(),
                                                   0 == REP.disposeRepCount());
                        LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());
                    }

                    LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                    LOOP_ASSERT(REP.disposeRepCount(),
                                0 == REP.disposeRepCount());
                    LOOP_ASSERT(REP.disposeObjectCount(),
                                                0 == REP.disposeObjectCount());

                    mY = S;
                    LOOP_ASSERT(REP.numReferences(), 1 == REP.numReferences());
                    LOOP_ASSERT(REP.disposeRepCount(),
                                0 == REP.disposeRepCount());
                    LOOP_ASSERT(REP.disposeObjectCount(),
                                0 == REP.disposeObjectCount());
                }

                LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
                LOOP_ASSERT(REP.disposeRepCount(),
                            0 == REP.disposeRepCount());
                LOOP_ASSERT(REP.disposeObjectCount(),
                            1 == REP.disposeObjectCount());
            }

            LOOP_ASSERT(REP.numReferences(), 0 == REP.numReferences());
            LOOP_ASSERT(REP.disposeRepCount(),  1 == REP.disposeRepCount());
            LOOP_ASSERT(REP.disposeObjectCount(),
                        1 == REP.disposeObjectCount());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Exercise a broad cross-section of functionality before beginning
        //   testing in earnest.  Probe that functionality systematically and
        //   incrementally to discover basic errors in isolation.
        //
        // Plan:
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            MyTestObject *obj = new MyTestObject();

            ObjSP mS(obj); const ObjSP& S = mS;
            ASSERT(1 == S.numReferences());

            ObjWP mX(S); const ObjWP& X = mX;
            ASSERT(!X.expired());
            ASSERT(1 == X.numReferences());

            ObjWP mY(X); const ObjWP& Y = mY;
            ASSERT(!Y.expired());
            ASSERT(1 == Y.numReferences());

            ASSERT(1 == X.numReferences());

            ObjWP mA; const ObjWP& A = mA;

            mA = S;
            ASSERT(!A.expired());
            ASSERT(1 == A.numReferences());

            ObjWP mB; const ObjWP& B = mB;

            mB = X;
            ASSERT(!B.expired());
            ASSERT(1 == B.numReferences());

            mA = Y;
            ASSERT(!A.expired());
            ASSERT(1 == A.numReferences());

            ObjSP mT = mX.acquireSharedPtr(); const ObjSP& T = mT;
            ASSERT(mT);
            ASSERT(!X.expired());
            ASSERT(2 == A.numReferences());
            ASSERT(S == T);
        }

        {
            MyTestDerivedObject *obj = new MyTestDerivedObject();

            DerivedSP mS(obj); const DerivedSP& S = mS;
            ASSERT(1 == S.numReferences());

            BaseWP mX(S); const BaseWP& X = mX;
            ASSERT(!X.expired());
            ASSERT(1 == X.numReferences());

            DerivedWP mY(S); const DerivedWP& Y = mY;
            ASSERT(!Y.expired());
            ASSERT(1 == Y.numReferences());

            BaseWP mZ(Y); const BaseWP& Z = mZ;
            ASSERT(!Z.expired());
            ASSERT(1 == Z.numReferences());

            BaseWP mA; const BaseWP& A = mA;

            mA = S;
            ASSERT(!A.expired());
            ASSERT(1 == A.numReferences());

            BaseWP mB; const BaseWP& B = mB;

            mB = Y;
            ASSERT(!B.expired());
            ASSERT(1 == B.numReferences());

            mA = Y;
            ASSERT(!A.expired());
            ASSERT(1 == A.numReferences());
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
