// btesc_cbchannel.t.cpp             -*-C++-*-

#include <btesc_cbchannel.h>
#include <btesc_flag.h>

#include <bdef_function.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class compiles and links.  We create a sample derived class that
// provides a dummy implementation of the base class virtual methods.  We then
// verify that when a method is called through a base class instance pointer
// the appropriate method in the derived class instance is invoked.
//-----------------------------------------------------------------------------
// [ 1] virtual ~btesc_CbChannel()
// [ 1] virtual int read(...)
// [ 1] virtual int readRaw(...)
// [ 1] virtual int readvRaw(...)
// [ 1] virtual int bufferedRead(...)
// [ 1] virtual int bufferedReadRaw(...)
// [ 1] virtual int write(...)
// [ 1] virtual int writeRaw(...)
// [ 1] virtual int writevRaw(const btes_Ovec *buffers, ...)
// [ 1] virtual int writevRaw(const btes_Iovec *buffers, ...)
// [ 1] virtual int bufferedWrite(const btes_Iovec *buffers, ...)
// [ 1] virtual int bufferedWritev(const btes_Ovec *buffers, ...)
// [ 1] virtual int bufferedWritev(const btes_Iovec *buffers, ...)
// [ 1] virtual void cancelAll()
// [ 1] virtual void cancelRead()
// [ 1] virtual void cancelWrite()
// [ 1] virtual void invalidate()
// [ 1] virtual void invalidateRead()
// [ 1] virtual void invalidateWrite()
// [ 1] virtual int isInvalidRead()
// [ 1] virtual int isInvalidWrite()
// [ 1] virtual int numPendingReadOperations()
// [ 1] virtual int numPendingWriteOperations()
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                      CONCRETE DERIVED TYPE
//-----------------------------------------------------------------------------
class MyChannel : public btesc_CbChannel {
  // Test class used to verify protocol.

    int d_fun;  // holds code describing (non-const) function:
                //   + 1 read
                //   + 2
                //   + 3 readRaw
                //   + 4
                //   + 5 readvRaw
                //   + 6
                //   + 7 bufferedRead
                //   + 8
                //   + 9 bufferedReadRaw
                //   +10
                //   +11 write
                //   +12
                //   +13 writeRaw
                //   +14
                //   +15 writevRaw(const btes_Ovec *, ...)
                //   +16 writevRaw(const btes_Iovec *, ...)
                //   +17
                //   +18
                //   +19 bufferedWrite
                //   +20
                //   +21 bufferedWritev(const btes_Ovec *, ...)
                //   +22 bufferedWritev(const btes_Iovec *, ...)
                //   +23
                //   +24
                //   +25 cancelAll
                //   +26 cancelRead
                //   +27 cancelWrite
                //   +28 invalidate
                //   +29 invalidateRead
                //   +30 invalidateWrite

    int d_flags;      // last value of flags passed

    int d_valid;      // Is this object instance valid?
    int d_validRead;  // Is this object instance valid for read?
    int d_validWrite; // Is this object instance valid for write?

  public:
    MyChannel() : d_fun(0),   d_flags(0),
                  d_valid(1), d_validRead(1), d_validWrite(1)  { }
    ~MyChannel() { }

    virtual int read(char                *buffer,
                     int                  numBytes,
                     const ReadCallback&  readCallback,
                     int                  flags = 0)
        { d_fun = 1; d_flags = flags; return -1; }
    virtual int readRaw(char                *buffer,
                        int                  numBytes,
                        const ReadCallback&  readCallback,
                        int                  flags = 0)
        { d_fun = 3; d_flags = flags; return -1; }
    virtual int readvRaw(const btes_Iovec *buffers,
                         int                      numBuffers,
                         const ReadCallback&      readCallback,
                         int                      flags = 0)
        { d_fun = 5; d_flags = flags; return -1; }
    virtual int bufferedRead(int                         numBytes,
                             const BufferedReadCallback& bufferedReadCallback,
                             int                         flags = 0)
        { d_fun = 7; d_flags = flags; return -1; }
    virtual int bufferedReadRaw(
                              int                         numBytes,
                              const BufferedReadCallback& bufferedReadCallback,
                              int                         flags = 0)
        { d_fun = 9; d_flags = flags; return -1; }
    virtual int write(const char           *buffer,
                      int                   numBytes,
                      const WriteCallback&  writeCallback,
                      int                   flags = 0)
        { d_fun = 11; d_flags = flags; return -1; }
    virtual int writeRaw(const char           *buffer,
                         int                   numBytes,
                         const WriteCallback&  writeCallback,
                         int                   flags = 0)
        { d_fun = 13; d_flags = flags; return -1; }
    virtual int writevRaw(const btes_Ovec  *buffers,
                          int                      numBuffers,
                          const WriteCallback&     writeCallback,
                          int                      flags = 0)
        { d_fun = 15; d_flags = flags; return -1; }
    virtual int writevRaw(const btes_Iovec *buffers,
                          int                      numBuffers,
                          const WriteCallback&     writeCallback,
                          int                      flags = 0)
        { d_fun = 16; d_flags = flags; return -1; }
    virtual int bufferedWrite(const char           *buffer,
                              int                   numBytes,
                              const WriteCallback&  writeCallback,
                              int                   flags = 0)
        { d_fun = 19; d_flags = flags; return -1; }
    virtual int bufferedWritev(const btes_Ovec  *buffers,
                               int                      numBuffers,
                               const WriteCallback&     writeCallback,
                               int                      flags = 0)
        { d_fun = 21; d_flags = flags; return -1; }
    virtual int bufferedWritev(const btes_Iovec *buffers,
                               int                      numBuffers,
                               const WriteCallback&     writeCallback,
                               int                      flags = 0)
        { d_fun = 22; d_flags = flags; return -1; }
    virtual void cancelAll()
        { d_fun = 25; }
    virtual void cancelRead()
        { d_fun = 26; }
    virtual void cancelWrite()
        { d_fun = 27; }
    virtual void invalidate()
        { d_fun = 28; d_valid = 0; }
    virtual void invalidateRead()
        { d_fun = 29; d_validRead = 0; }
    virtual void invalidateWrite()
        { d_fun = 30; d_validWrite = 0; }
    virtual int isInvalidRead() const
        { return !d_validRead; }
    virtual int isInvalidWrite() const
        { return !d_validWrite; }
    // Return some special value to indicate that this method is being called.
    virtual int numPendingReadOperations() const
        { return -1; }
    // This should be called numPendingWriteOperations() for consistency.
    // Return some special value to indicate that this method is being called.
    virtual int numPendingWriteOperations() const
        { return -2; }

    // non-virtual functions for testing
    int fun()   const { return d_fun; }
    int flags() const { return d_flags; }
    int valid() const { return d_valid; }
};

// Free functions passed in as callbacks.
void myRdCbFn(int, int) { }
void myBufRdCbFn(const char *, int, int) { }
void myWrCbFn(int, int) { }

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE TEST:
        //   This test is really just to make sure the syntax is correct.
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        //   TBD.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // OPERATOR TEST:
        //   No operators are defined.
        // Testing:
        //   N/a.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR TEST" << endl
                                  << "=============" << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STATIC MEMBER TEMPLATE METHOD TEST:
        //
        // Plan:
        //   No static member is defined.  Not applicable.
        //
        // Testing:
        //   N/A.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "STATIC MEMBER TEMPLATE METHOD" << endl
                                  << "=============" << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a concrete subclass of the
        //   'btesc_CbChannel' class compiles and links when all
        //   virtual functions are defined.
        // Testing:
        //   virtual ~btesc_CbChannel(...)
        //   virtual int read(...)
        //   virtual int readRaw(...)
        //   virtual int readvRaw(...)
        //   virtual int bufferedRead(...)
        //   virtual int bufferedReadRaw(...)
        //   virtual int write(...)
        //   virtual int writeRaw(...)
        //   virtual int writevRaw(const btes_Ovec *buffers, ...)
        //   virtual int writevRaw(const btes_Iovec *buffers, ...)
        //   virtual int bufferedWrite(const btes_Iovec *buffers, ...)
        //   virtual int bufferedWritev(const btes_Ovec *buffers, ...)
        //   virtual int bufferedWritev(const btes_Iovec *buffers, ...)
        //   virtual void cancelAll(...)
        //   virtual void cancelRead(...)
        //   virtual void cancelWrite(...)
        //   virtual void invalidate(...)
        //   virtual void invalidateRead(...)
        //   virtual void invalidateWrite(...)
        //   virtual int isInvalidRead(...)
        //   virtual int isInvalidWrite(...)
        //   virtual int numPendingReadOperations(...)
        //   virtual int numPendingWriteOperations(...)
        //
        //   PROTOCOL TEST - Make sure derived class compiles and links.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        // Ensure that an instance of the derived class can be created.
        MyChannel myC;
        btesc_CbChannel& c = myC;
        ASSERT(1 == myC.valid()); ASSERT(0 == myC.isInvalidRead());
                                  ASSERT(0 == myC.isInvalidRead());

        if (verbose) cout << "\nTesting protocol interface" << endl;
        {
            // Invoke different methods via the base class reference and check
            // if the derived class method is invoked.

            // Create a dummy functor object.
            btesc_CbChannel::ReadCallback myRdCb(&myRdCbFn);

            btesc_CbChannel::BufferedReadCallback myBufCb(&myBufRdCbFn);

            btesc_CbChannel::WriteCallback myWrCb(&myWrCbFn);

            // Flags to indicate that async interrupts are allowed.
            const int myFlags = btesc_Flag::BTESC_ASYNC_INTERRUPT;

            ASSERT(0 == myC.flags());

            c.read(NULL, 0, myRdCb);
            ASSERT(1 == myC.fun()); ASSERT(0 == myC.flags());
            c.read(NULL, 0, myRdCb, myFlags);
            ASSERT(1 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.readRaw(NULL, 0, myRdCb);
            ASSERT(3 == myC.fun()); ASSERT(0 == myC.flags());
            c.readRaw(NULL, 0, myRdCb, myFlags);
            ASSERT(3 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.readvRaw(NULL, 0, myRdCb);
            ASSERT(5 == myC.fun()); ASSERT(0 == myC.flags());
            c.readvRaw(NULL, 0, myRdCb, myFlags);
            ASSERT(5 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedRead(0, myBufCb);
            ASSERT(7 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedRead(0, myBufCb, myFlags);
            ASSERT(7 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedReadRaw(0, myBufCb);
            ASSERT(9 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedReadRaw(0, myBufCb, myFlags);
            ASSERT(9 == myC.fun()); ASSERT(myFlags == myC.flags());

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            c.write(NULL, 0, myWrCb);
            ASSERT(11 == myC.fun()); ASSERT(0 == myC.flags());
            c.write(NULL, 0, myWrCb, myFlags);
            ASSERT(11 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writeRaw(NULL, 0, myWrCb);
            ASSERT(13 == myC.fun()); ASSERT(0 == myC.flags());
            c.writeRaw(NULL, 0, myWrCb, myFlags);
            ASSERT(13 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writevRaw(static_cast<const btes_Ovec *>(0), 0, myWrCb);
            ASSERT(15 == myC.fun()); ASSERT(0 == myC.flags());
            c.writevRaw(static_cast<const btes_Ovec *>(0), 0, myWrCb, myFlags);
            ASSERT(15 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writevRaw(static_cast<const btes_Iovec *>(0), 0, myWrCb);
            ASSERT(16 == myC.fun()); ASSERT(0 == myC.flags());
            c.writevRaw(static_cast<const btes_Iovec *>(0), 0, myWrCb,
                        myFlags);
            ASSERT(16 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedWrite(NULL, 0, myWrCb);
            ASSERT(19 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedWrite(NULL, 0, myWrCb, myFlags);
            ASSERT(19 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedWritev(static_cast<const btes_Ovec *>(0), 0, myWrCb);
            ASSERT(21 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedWritev(static_cast<const btes_Ovec *>(0), 0, myWrCb,
                             myFlags);
            ASSERT(21 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedWritev(static_cast<const btes_Iovec *>(0), 0, myWrCb);
            ASSERT(22 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedWritev(static_cast<const btes_Iovec *>(0), 0, myWrCb,
                             myFlags);
            ASSERT(22 == myC.fun()); ASSERT(myFlags == myC.flags());

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            c.cancelAll();
            ASSERT(25 == myC.fun());

            c.cancelRead();
            ASSERT(26 == myC.fun());

            c.cancelWrite();
            ASSERT(27 == myC.fun());

            ASSERT(1 == myC.valid()); // non-virtual test method
            c.invalidate();
            ASSERT(28 == myC.fun());  ASSERT(0 == myC.valid());

            ASSERT(0 == c.isInvalidRead());
            c.invalidateRead();
            ASSERT(29 == myC.fun());  ASSERT(1 == c.isInvalidRead());

            ASSERT(0 == c.isInvalidWrite());
            c.invalidateWrite();
            ASSERT(30 == myC.fun());  ASSERT(1 == c.isInvalidWrite());

            ASSERT(-1 == c.numPendingReadOperations());

            ASSERT(-2 == c.numPendingWriteOperations());
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
