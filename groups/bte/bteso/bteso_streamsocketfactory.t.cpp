// bteso_streamsocketfactory.t.cpp        -*-C++-*-

#include <bteso_streamsocketfactory.h>

#include <bteso_streamsocket.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that a concrete derived class compiles
// and links.  Verify that the allocate and deallocate functions for the
// concrete derived class are called and return a pointer to a concrete
// stream socket.
//-----------------------------------------------------------------------------
// [ 1] virtual ~bteso_StreamSocketFactory();
// [ 1] virtual bteso_StreamSocket<ADDRESS> *allocate() = 0;
// [ 1] virtual void deallocate(bteso_StreamSocket<ADDRESS> * socket) = 0;
//-----------------------------------------------------------------------------
// [ 2] USAGE TEST - Make sure main usage compiles and works properly.
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    GLOBAL TESTING CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------

                            // ===================
                            // class TestIPAddress
                            // ===================

struct TestIPAddress {
    int d_address;
    int d_port;
};

                           // ======================
                           // class TestStreamSocket
                           // ======================

class TestStreamSocket : public bteso_StreamSocket<TestIPAddress>
{

    // CLASS DATA
    static int  d_allocatedCount;

    // INSTANCE DATA
    int        *d_fcode_p;

    // FRIENDS
    friend class TestStreamSocketFactory;

  public:
    // CLASS METHODS
    static int getCount();

    // CREATORS
    TestStreamSocket(int *opCode);

    ~TestStreamSocket();

    // MANIPULATORS
    int read(char *buffer, int length);
    int readv(const btes_Iovec * iovec, int size);
    int write(const char *buffer, int length);
    int writev(const btes_Ovec *ovec, int size);
    int writev(const btes_Iovec *iovec, int size);
    int shutdown(bteso_Flag::ShutdownType);
    int connect(const TestIPAddress& address);
    int bind(const TestIPAddress& address);
    int listen(int backlog);
    int accept(bteso_StreamSocket<TestIPAddress>** socket, TestIPAddress *);
    int accept(bteso_StreamSocket<TestIPAddress>** socket);
    bteso_StreamSocket<TestIPAddress> * accept(TestIPAddress *);
    int setBlockingMode(bteso_Flag::BlockingMode flag);
    int setLingerOption(const bteso_SocketOptUtil::LingerData& options);
    int setOption(int level, int option, int value);
    int waitForConnect(const bdet_TimeInterval&);
    int waitForAccept(const bdet_TimeInterval&);
    int waitForIO(bteso_Flag::IOWaitType type, const bdet_TimeInterval& t);
    int waitForIO(bteso_Flag::IOWaitType type);

    // ACCESSORS
    int blockingMode(bteso_Flag::BlockingMode *flag) const;
    int connectionStatus() const;
    bteso_SocketHandle::Handle handle() const;
    int lingerOption(bteso_SocketOptUtil::LingerData *result) const;
    int localAddress(TestIPAddress *result) const;
    int peerAddress(TestIPAddress *result) const;
    int socketOption(int *result, int level, int option) const;
};

// CLASS DATA
int TestStreamSocket::d_allocatedCount = 0;

// CLASS METHODS
int TestStreamSocket::getCount()
{
    return d_allocatedCount;
}

// CREATORS
TestStreamSocket::TestStreamSocket(int *opCode)
: d_fcode_p(opCode)
{
    ++d_allocatedCount;
}

TestStreamSocket::~TestStreamSocket()
{
    --d_allocatedCount;
}

// MANIPULATORS
int TestStreamSocket::read(char *buffer, int length)
    { *d_fcode_p = 1; return -1; }
int TestStreamSocket::readv(const btes_Iovec * iovec, int size)
    { *d_fcode_p = 2; return -1; }
int TestStreamSocket::write(const char *buffer, int length)
    { *d_fcode_p = 3; return -1; }
int TestStreamSocket::writev(const btes_Ovec *ovec, int size)
    { *d_fcode_p = 4; return -1; }
int TestStreamSocket::writev(const btes_Iovec *iovec, int size)
    { *d_fcode_p = 5; return -1; }
int TestStreamSocket::shutdown(bteso_Flag::ShutdownType)
    { *d_fcode_p = 6; return -1; }
int TestStreamSocket::connect(const TestIPAddress& address)
    { *d_fcode_p = 7; return -1; }
int TestStreamSocket::bind(const TestIPAddress& address)
    { *d_fcode_p = 8; return -1; }
int TestStreamSocket::listen(int backlog)
    { *d_fcode_p = 9; return -1; }
int TestStreamSocket::accept(bteso_StreamSocket<TestIPAddress>** socket,
                             TestIPAddress *)
    { *d_fcode_p = 10; return -1; }
int TestStreamSocket::accept(bteso_StreamSocket<TestIPAddress>** socket)
    { *d_fcode_p = 11; return -1; }
bteso_StreamSocket<TestIPAddress> * TestStreamSocket::accept(TestIPAddress *)
    { *d_fcode_p = 12; return 0; }
int TestStreamSocket::setBlockingMode(bteso_Flag::BlockingMode flag)
    { *d_fcode_p = 13; return 0;  }
int TestStreamSocket::setLingerOption(
                                const bteso_SocketOptUtil::LingerData& options)
    { *d_fcode_p = 14; return 0;  }
int TestStreamSocket::setOption(int level, int option, int value)
    { *d_fcode_p = 15; return 0;  }
int TestStreamSocket::waitForConnect(const bdet_TimeInterval&)
    { *d_fcode_p = 16; return -1; }
int TestStreamSocket::waitForAccept(const bdet_TimeInterval&)
    { *d_fcode_p = 17; return -1; }
int TestStreamSocket::waitForIO(bteso_Flag::IOWaitType   type,
                                const bdet_TimeInterval& t)
    { *d_fcode_p = 18; return -1; }
int TestStreamSocket::waitForIO(bteso_Flag::IOWaitType type)
    { *d_fcode_p = 19; return -1; }

// ACCESSORS
int TestStreamSocket::blockingMode(bteso_Flag::BlockingMode *flag) const
    { *d_fcode_p = 20; return -1; }
int TestStreamSocket::connectionStatus() const
    { *d_fcode_p = 21; return 0;  }
bteso_SocketHandle::Handle TestStreamSocket::handle() const
    { *d_fcode_p = 22; return 0;  }
int TestStreamSocket::lingerOption(
                                 bteso_SocketOptUtil::LingerData *result) const
    { *d_fcode_p = 23; return 0;  }
int TestStreamSocket::localAddress(TestIPAddress *result) const
    { *d_fcode_p = 24; return 0;  }
int TestStreamSocket::peerAddress(TestIPAddress *result) const
    { *d_fcode_p = 25; return 0;  }
int TestStreamSocket::socketOption(int *result, int level, int option) const
    { *d_fcode_p = 26; return 0;  }

                       // =============================
                       // class TestStreamSocketFactory
                       // =============================

class TestStreamSocketFactory : public bteso_StreamSocketFactory<TestIPAddress>
{
    // CLASS DATA
    static int d_allocatedCount;
    static int d_opCode;

  public:
    // CLASS METHODS
    static int getCount();

    static int getOpCode();

    // CREATORS
    TestStreamSocketFactory();

    ~TestStreamSocketFactory();

    // MANIPULATORS
    bteso_StreamSocket<TestIPAddress> *allocate();

    void deallocate(bteso_StreamSocket<TestIPAddress> *socket);
};

// CLASS DATA
int TestStreamSocketFactory::d_allocatedCount = 0;

int TestStreamSocketFactory::d_opCode = 0;

// CLASS METHODS
int TestStreamSocketFactory::getCount()
{
    return d_allocatedCount;
}

int TestStreamSocketFactory::getOpCode()
{
    return d_opCode;
}

// CREATORS
TestStreamSocketFactory::TestStreamSocketFactory()
{
}

TestStreamSocketFactory::~TestStreamSocketFactory()
{
}

// MANIPULATORS
bteso_StreamSocket<TestIPAddress> * TestStreamSocketFactory::allocate()
{
    ++d_allocatedCount;

    return new TestStreamSocket(&d_opCode);
}

void TestStreamSocketFactory::deallocate(
                                     bteso_StreamSocket<TestIPAddress> *socket)
{
    --d_allocatedCount;
    ASSERT(d_allocatedCount >= 0);

    delete (TestStreamSocket *) socket;
}

                              // ================
                              // local function f
                              // ================

void f(bteso_StreamSocketFactory<TestIPAddress> *factory)
{
    bteso_StreamSocket<TestIPAddress> *socket = factory->allocate();
    if (!socket) {
        // cout << "Socket could not be allocated" << endl;
        ASSERT(1);
    }
    else {

        if (socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE) != 0) {
            //cout << "Blocking Mode could not be set" << endl;
            ASSERT(1);
        }
        else {
            //cout << "further socket operations" << endl;
        }
    }

    factory->deallocate(socket);
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE TEST:
        //   This test is really just to make sure the syntax is correct.
        // Testing:
        //   USAGE TEST - Make sure main usage compiles and works properly.
        // --------------------------------------------------------------------

        if (verbose) cout << "Usage example." << endl
                          << "==============" << endl;

         TestStreamSocketFactory testFactory;
         f(&testFactory);

      } break;
      case 1: {
        // ----------------------------------------------------------------
        // TESTING ALLOCATE AND DEALLOCATE FUNCTIONS
        //
        // Plan:
        //
        // Testing
        //   virtual ~bteso_StreamSocketFactory();
        //   virtual bteso_StreamSocket<ADDRESS> *allocate() = 0;
        //   virtual void deallocate(bteso_StreamSocket<ADDRESS> * socket) = 0;
        // ----------------------------------------------------------------

        if (verbose) cout << "PROTOCOL TEST" << endl
                          << "=============" << endl;

        TestStreamSocketFactory factory;

        bteso_StreamSocketFactory<TestIPAddress> *factoryP = &factory;

        ASSERT(factory.getCount() == 0);
        ASSERT(factory.getOpCode() == 0);
        ASSERT(TestStreamSocket::getCount() == 0);

        bteso_StreamSocket<TestIPAddress> *socketP = factoryP->allocate();

        // Verify that correct factory performs the allocate.
        ASSERT(factory.getCount() == 1);
        ASSERT(TestStreamSocket::getCount() == 1);

        // Invoke a function on the socket returned and verify that
        // the function on the test concrete class was called.
        char buf[100];
        ASSERT(socketP->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH) == -1);
        ASSERT(factory.getOpCode() == 6);

        factoryP->deallocate(socketP);

        // Verify that correct factory performs the deallocate.
        ASSERT(factory.getCount() == 0);
        ASSERT(TestStreamSocket::getCount() == 0);

        socketP = factoryP->allocate();
        ASSERT(factory.getCount() == 1);
        ASSERT(TestStreamSocket::getCount() == 1);

        {
            bteso_StreamSocketFactoryAutoDeallocateGuard<TestIPAddress>
                                                      guard(socketP, factoryP);

            guard.release();
        }
        ASSERT(socketP != 0);
        ASSERT(factory.getCount() == 1);
        ASSERT(TestStreamSocket::getCount() == 1);

        {
            bteso_StreamSocketFactoryAutoDeallocateGuard<TestIPAddress>
                                                     guard(&socketP, factoryP);

            guard.release();
        }
        ASSERT(socketP != 0);
        ASSERT(factory.getCount() == 1);
        ASSERT(TestStreamSocket::getCount() == 1);

        {
            bteso_StreamSocketFactoryAutoDeallocateGuard<TestIPAddress>
                                                      guard(socketP, factoryP);
        }
        ASSERT(socketP != 0);
        ASSERT(factory.getCount() == 0);
        ASSERT(TestStreamSocket::getCount() == 0);

        socketP = factoryP->allocate();
        ASSERT(socketP != 0);
        ASSERT(factory.getCount() == 1);
        ASSERT(TestStreamSocket::getCount() == 1);

        {
            bteso_StreamSocketFactoryAutoDeallocateGuard<TestIPAddress>
                                                     guard(&socketP, factoryP);
        }
        ASSERT(socketP == 0);
        ASSERT(factory.getCount() == 0);
        ASSERT(TestStreamSocket::getCount() == 0);

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
