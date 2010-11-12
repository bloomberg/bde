 // bcema_threadenabledallocatoradapter.t.cpp  -*-C++-*-

#include <bcema_threadenabledallocatoradapter.h>

#include <bcemt_thread.h>
#include <bcemt_lockguard.h>

#include <bcema_testallocator.h>            // for testing only
#include <bcemt_barrier.h>                  // for testing only
#include <bslma_testallocator.h>            // for testing only
#include <bslma_testallocatorexception.h>   // for testing only

#include <bsls_alignment.h>
#include <bsls_stopwatch.h>

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>                         // atoi()
#include <bsl_cstring.h>                         // memcpy(), memset()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bcema_ThreadEnabledAllocatorAdapter' class consists of one
// constructor, a destructor, and four manipulators.  The manipulators are
// used to allocate, deallocate, and reserve memory.  Since this component is
// a memory manager, the 'bdema_testallocator' component is used extensively
// to verify expected behaviors.  Note that the copying of objects is
// explicitly disallowed since the copy constructor and assignment operator
// are declared 'private' and left unimplemented.  So we are primarily
// concerned that the internal memory management system functions as expected
// and that the manipulators operator correctly.  Note that memory allocation
// must be tested for exception neutrality (also via the 'bdema_testallocator'
// component).  Several small helper functions are also used to facilitate
// testing.
//-----------------------------------------------------------------------------
// [2] bcema_ThreadEnabledAllocatorAdapter(int              numPools,
//                                         bslma_Allocator *ba = 0);
// [2] ~bcema_ThreadEnabledAllocatorAdapter();
// [3] void *allocate(int size);
// [4] void deallocate(void *address);
// [5] void release();
// [6] void reserveCapacity(int size, int numObjects);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [7] USAGE EXAMPLE
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bcema_ThreadEnabledAllocatorAdapter Obj;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class NoopAllocator : public bslma_Allocator {
    // The NoopAllocator is an empty implementation of the 'bslma_Allocator'
    // protocol that tracks the methods called and verifies the
    // 'bslma_Allocator' protocol.

  private:
    char const **d_lastMethod;  // the last method called on this instance

  public:

    // CREATORS
    NoopAllocator(char const **lastMethod)
        // Create this 'NoopAllocator' with the specified
        // 'lastMethod' to store the last method called.
    : d_lastMethod(lastMethod)
    {
        *d_lastMethod = "NoopAllocator";
    }

    ~NoopAllocator();
        // Destroy this 'NoopAllocator'

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // This method has no effect for this allocator except that a
        // subsequent call to 'lastMethod()' will return 'allocate'.

    virtual void deallocate(void *address);
        // This method has no effect for this allocator except that a
        // subsequent call to 'lastMethod()' will return 'deallocate'.

    // ACCESSORS
    const char *lastMethod() { return *d_lastMethod; }
        // Return the last method called on this 'NoopAllocator'
        // instance.
};

NoopAllocator::~NoopAllocator()
{
    *d_lastMethod = "~NoopAllocator";
}

void *NoopAllocator::allocate(size_type size)
{
    *d_lastMethod = "allocate";
    return 0;
}

void NoopAllocator::deallocate(void *address)
{
    *d_lastMethod = "deallocate";
}

enum {
    NUM_THREADS = 10
};
struct WorkerArgs {
    Obj       *d_allocator; // allocator to perform allocations
    const int *d_sizes;     // array of allocations sizes
    int        d_numSizes;  // number of allocations

};

bcemt_Barrier g_barrier(NUM_THREADS);
extern "C" void *workerThread(void *arg) {
    // Perform a series of allocate, and deallocate operations on the
    // 'bcema_ThreadEnabledAllocatorAdapter' and verify their results.  This
    // operation is intended to be a thread entry point.  Cast the specified
    // 'args' to a 'WorkerArgs', and perform a series of
    // '(WorkerArgs *)args->d_numSizes' allocations using the corresponding
    // allocations sizes specified by  '(WorkerARgs *)args->d_sizes'.  Use the
    // barrier 'g_barrier' to ensure tests are performed while the allocator
    // is in the correct state.

    WorkerArgs *args = (WorkerArgs *) arg;
    ASSERT(0 != args);
    ASSERT(0 != args->d_sizes);

    Obj       *allocator  = args->d_allocator;
    const int *allocSizes = args->d_sizes;
    const int  numAllocs  = args->d_numSizes;

    bsl::vector<char *> blocks(bslma_Default::allocator(0));
    blocks.resize(numAllocs);

    g_barrier.wait();

    // Perform allocations
    for (int i = 0; i < numAllocs; ++i) {
        blocks[i] = (char *)allocator->allocate(allocSizes[i]);
    }

    // deallocate all the blocks
    for (int i = 0; i < numAllocs; ++i) {
        allocator->deallocate(blocks[i]);
    }

    g_barrier.wait();

    return arg;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In the following usage example we develop a simple containing two lists of
// strings.  For the purpose of discussion we first define a simple thread
// enabled vector:
//..
    template <typename T>
    class ThreadEnabledVector {
        // This class defines a trivial thread enabled vector.

        // DATA
        mutable bcemt_Mutex  d_mutex;       // synchronize access
        bsl::vector<T>       d_elements;    // underlying list of strings
        bslma_Allocator     *d_allocator_p; // allocator (held, not owned)

        // NOT IMPLEMENTED
        ThreadEnabledVector(const ThreadEnabledVector&);
        ThreadEnabledVector& operator=(const ThreadEnabledVector&);

      public:

        // CREATORS
        ThreadEnabledVector(bslma_Allocator *basicAllocator = 0)
            // Create a thread enabled vector.  Optionally specify
            // 'basicAllocator', used to supply memory.  If 'basicAllocator'
            // is 0, the currently installed default allocator will be used.
        : d_elements(basicAllocator)
        , d_allocator_p(bslma_Default::allocator(basicAllocator))
        {
        }

        ~ThreadEnabledVector() {}
            // Destroy this vector object.

        // MANIPULATORS
        int pushBack(const T& value)
            // Append the specified 'value' to this vector of values and
            // return the index of the new element.
        {
            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
            d_elements.push_back(value);
            return d_elements.size() - 1;
        }

        void set(int index, const T& value)
            // Set the element at the specified 'index' to the specified
            // 'value'.  The behavior is undefined unless 'index < size()'.
        {
            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
            d_elements[index] = value;
        }

        // ACCESSORS
        T element(int index) const
            // Return the value of the element at the specified 'index'.  Note
            // that elements are returned *by* *value* because references to
            // elements managed by this container may be invalidated by
            // another thread of control.
        {
            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
            return d_elements[index];
        }

        int length() const
            // Return the number elements in this vector object.
        {
            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
            return d_elements.size();
        }
    };
//..
// We use this-thread enabled vector to create a Rolodex class.  However, we
// use the 'bcema_ThreadEnabledAllocatorAdapter' to prevent our two
// (thread-enabled) vectors from attempting synchronous memory allocations
// from our (potentially) non-thread safe 'bslma_Allocator'.  Note that we
// define a local class 'Rolodex_PrivateData' in order to guarantee that
// 'd_allocatorAdapter' and 'd_mutex' are initialized before the
// thread-enabled vectors that depend on them.
//..
    struct Rolodex_PrivateData {
        // Define a structure containing a mutex and an allocator adapter.  The
        // 'Rolodex' class will inherit from this structure, ensuring that the
        // mutex and adapter are initialized before the thread-enabled vectors
        // that depend on them.

        bcemt_Mutex           d_mutex;             // synchronize allocator

        bcema_ThreadEnabledAllocatorAdapter
                              d_allocatorAdapter;  // adapter for allocator

        Rolodex_PrivateData(bslma_Allocator *basicAllocator = 0)
            // Create an empty Rolodex private data object.  Optionally
            // specify 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default
            // allocator is used.
        : d_allocatorAdapter(&d_mutex, basicAllocator)
        {
        }
    };

    class Rolodex : private Rolodex_PrivateData{
        // Define a thread-enabled Rolodex class containing a vector of
        // names and addresses.  Note that this class uses private inheritance
        // to ensure that the allocator adapter and mutex are initialized
        // before the vectors of names and addresses.

        // DATA
        ThreadEnabledVector<bsl::string>
                             d_names;             // list of names (owned)

        ThreadEnabledVector<bsl::string>
                             d_addresses;         // list of addresses (owned)

      public:
        // CREATORS
        Rolodex(bslma_Allocator *basicAllocator = 0)
            // Create an empty rolodex for storing names and addresses.
            // Optionally specify 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default
            // allocator is used.
        : Rolodex_PrivateData(basicAllocator)
        , d_names(&d_allocatorAdapter)
        , d_addresses(&d_allocatorAdapter)
        {
        }

        ~Rolodex()
            // Destroy this Rolodex object.
        {
        }

        // MANIPULATORS
        int addName(const bsl::string& name)
            // Add the specified 'name' to this Rolodex object.  Return the
            // index of the element where 'name' is located.
        {
            return d_names.pushBack(name);
        }

        int addAddress(const bsl::string& address)
            // Add the specified 'address' to this Rolodex object.  Return the
            // index of the element where 'address' is located.
        {
            return d_addresses.pushBack(address);
        }

        // ACCESSORS
        bsl::string name(int index) const
            // Return the value of the name at the specified 'index'.
        {
            return d_names.element(index);
        }

        bsl::string address(int index) const
            // Return the value of the address at the specified 'index'.
        {
            return d_addresses.element(index);
        }

        int numNames() const
            // Return the number of names in this rolodex.
        {
            return d_names.length();
        }

        int numAddresses() const
            // Return the number of addresses in this rolodex.
        {
            return d_addresses.length();
        }

    };

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 3: {
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

        if (verbose) cout << endl << "Testing Usage Example"
                          << endl << "=====================" << endl;

        Rolodex rolodex(Z);
        int nameIdx1 = rolodex.addName("Name1");
        int addrIdx1 = rolodex.addAddress("Address1");
        int nameIdx2 = rolodex.addName("Name2");
        int addrIdx2 = rolodex.addAddress("Address2");

        ASSERT(2 == rolodex.numNames());
        ASSERT(2 == rolodex.numAddresses());
        ASSERT(0 == nameIdx1);
        ASSERT(0 == addrIdx1);
        ASSERT(1 == nameIdx2);
        ASSERT(1 == addrIdx2);

        ASSERT("Name1"    == rolodex.name(nameIdx1));
        ASSERT("Address1" == rolodex.address(addrIdx1));
        ASSERT("Name2"    == rolodex.name(nameIdx2));
        ASSERT("Address2" == rolodex.address(addrIdx2));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of allocate/deallocate methods.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;
        bcemt_ThreadUtil::Handle threads[NUM_THREADS];

        bslma_TestAllocator talloc(false);
        bcemt_Mutex         mutex;
        Obj                 mX(&mutex, &talloc);

        const int SIZES [] = { 1 , 2 , 4,  8, 16, 32, 64, 128, 256, 512,
                               1 , 2 , 4,  8, 16, 32, 64, 128, 256, 512};

        const int NUM_SIZES = sizeof (SIZES) / sizeof(*SIZES);

        WorkerArgs args;
        args.d_allocator = &mX;
        args.d_sizes     = (int *)&SIZES;
        args.d_numSizes  = NUM_SIZES;

        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::create(&threads[i], workerThread, &args);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::join(threads[i]);
            LOOP_ASSERT(i, 0 == rc);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We are concerned that the basic functionality of
        //   'bcema_ThreadEnabledAllocatorAdapter' works
        //   properly.
        //
        // Plan:
        //   Create a NoopAllocator and supply it to a
        //   'bcema_ThreadEnabledAllocatorAdapter' under test.  Verify that
        //   operations on the allocator are delegated to the noop allocator.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Breathing Test"
                          << endl << "==============" << endl;

        const char   *lastMethod = 0;
        bcemt_Mutex   mutex;
        NoopAllocator noopAllocator = NoopAllocator(&lastMethod);
        {
            bcema_ThreadEnabledAllocatorAdapter mX(&mutex, &noopAllocator);

            ASSERT( 0 == bsl::strcmp("NoopAllocator",
                                     lastMethod));

            void *memory = mX.allocate(1);
            ASSERT(0 == bsl::strcmp("allocate", lastMethod));

            mX.deallocate(memory);
            ASSERT(0 == bsl::strcmp("deallocate", lastMethod));
        }
        ASSERT(0 == bsl::strcmp("deallocate", lastMethod));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
