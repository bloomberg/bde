// btesc_timedchannel.t.cpp      -*-C++-*-

#include <btesc_timedchannel.h>

#include <btesc_flag.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class (1) can be invoked and (2) the method calls are forwarded
// appropriately when invoked through the protocol.  We create a sample
// derived class that provides a dummy implementation of the base class
// virtual methods.  We then verify that when a method is called through a
// base class instance pointer the appropriate method in the derived class
// instance is invoked.
//-----------------------------------------------------------------------------
// [ 1] virtual ~btesc_TimedChannel()
// [ 1] virtual int read(...)
// [ 1] virtual int timedRead(...)
// [ 1] virtual int readv(...)
// [ 1] virtual int timedReadv(...)
// [ 1] virtual int readRaw(...)
// [ 1] virtual int timedReadRaw(...)
// [ 1] virtual int readvRaw(...)
// [ 1] virtual int timedReadvRaw(...)
// [ 1] virtual int bufferedRead(...)
// [ 1] virtual int timedBufferedRead(...)
// [ 1] virtual int bufferedReadRaw(...)
// [ 1] virtual int timedBufferedReadRaw(...)
// [ 1] virtual int write(...)
// [ 1] virtual int timedWrite(...)
// [ 1] virtual int writeRaw(...)
// [ 1] virtual int timedWriteRaw(...)
// [ 1] virtual int writev(...)
// [ 1] virtual int timedWritev(...)
// [ 1] virtual int writevRaw(const btes_Ovec *buffers, ...)
// [ 1] virtual int writevRaw(const btes_Iovec *buffers, ...)
// [ 1] virtual int timedWritevRaw(const btes_Ovec *buffers, ...)
// [ 1] virtual int timedWritevRaw(const btes_Iovec *buffers, ...)
// [ 1] virtual void invalidate()
// [ 1] virtual int isInvalid()
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

//============================================================================
//                      CONCRETE DERIVED TYPE
//----------------------------------------------------------------------------
class MyTimedChannel : public btesc_TimedChannel {
    // Test class used to verify protocol.

    int d_fun;  // holds code describing (non-const) function:
                //    1 read(...);
                //    2 read(int *augStatus, ...);
                //    3 timedRead(...);
                //    4 timedRead(int *augStatus, ...);
                //    5 readv(...);
                //    6 readv(int *augStatus, ...);
                //    7 timedReadv(int *augStatus, ...);
                //    8 timedReadv(...);
                //    9 readRaw(...);
                //   10 readRaw(int *augStatus, ...);
                //   11 timedReadRaw(...);
                //   12 timedReadRaw(int *augStatus, ...);
                //   13 readvRaw(...);
                //   14 readvRaw(int *augStatus, ...);
                //   15 timedReadvRaw(...);
                //   16 timedReadvRaw(int *augStatus, ...);
                //   17 bufferedRead(...);
                //   18 bufferedRead(int *augStatus, ...);
                //   19 timedBufferedRead(...);
                //   20 timedBufferedRead(int *augStatus, ...);
                //   21 bufferedReadRaw(...);
                //   22 bufferedReadRaw(int *augStatus, ...);
                //   23 timedBufferedReadRaw(...);
                //   24 timedBufferedReadRaw(int *augStatus, ...);
                //   25 write(...);
                //   26 write(int *augStatus, ...);
                //   27 timedWrite(...);
                //   28 timedWrite(int *augStatus, ...);
                //   29 writeRaw(...);
                //   30 writeRaw(int *augStatus, ...);
                //   31 timedWriteRaw(...);
                //   32 timedWriteRaw(int *augStatus, ...);
                //   33 writev(const btes_Ovec*, ...);
                //   34 writev(const btes_Iovec*, ...);
                //   35 writev(int *augStatus, const btes_Ovec*, ...);
                //   36 writev(int *augStatus, const btes_Iovec*, ...);
                //   37 timedWritev(const btes_Ovec*, ...);
                //   38 timedWritev(const btes_Iovec*, ...);
                //   39 timedWritev(int *augStatus, const btes_Ovec*, ...);
                //   40 timedWritev(int *augStatus, const btes_Iovec*, ...);
                //   41 writevRaw(const btes_Ovec *, ...);
                //   42 writevRaw(const btes_Iovec *, ...);
                //   43 writevRaw(int *augStatus, const btes_Ovec *, ...);
                //   44 writevRaw(int *augStatus, const btes_Iovec *, ...);
                //   45 timedWritevRaw(const btes_Ovec *, ...);
                //   46 timedWritevRaw(const btes_Iovec *, ...);
                //   47 timedWritevRaw(int *augStatus, const btes_Ovec*, ..);
                //   48 timedWritevRaw(int *augStatus, const btes_Iovec*,..);
                //   49 invalidate();
                //   50 isInvalid();

    int d_flags;      // last value of flags passed

    int d_valid;      // Is this object instance valid?

  private: // not implemented
    MyTimedChannel(const MyTimedChannel&);
    MyTimedChannel& operator=(const MyTimedChannel& );

  public:
    MyTimedChannel() : d_fun(0),   d_flags(0), d_valid(1) { }
    ~MyTimedChannel() { }

    virtual int read(char *buffer,
                     int   numBytes,
                     int   flags = 0)
    { d_fun = 1; d_flags = flags; return -1; }

    virtual int read(int  *augStatus,
                     char *buffer,
                     int   numBytes,
                     int   flags = 0)
    { d_fun = 2; d_flags = flags; return -1; }

    virtual int timedRead(char                     *buffer,
                          int                       numBytes,
                          const bdet_TimeInterval&  timeout,
                          int                       flags = 0)
    { d_fun = 3; d_flags = flags; return -1; }

    virtual int timedRead(int                      *augStatus,
                          char                     *buffer,
                          int                       numBytes,
                          const bdet_TimeInterval&  timeout,
                          int                       flags = 0)
    { d_fun = 4; d_flags = flags; return -1; }

    virtual int readv(const btes_Iovec *buffer,
                      int               numBytes,
                      int               flags = 0)
    { d_fun = 5; d_flags = flags; return -1; }

    virtual int readv(int              *augStatus,
                      const btes_Iovec *buffer,
                      int               numBytes,
                      int               flags = 0)
    { d_fun = 6; d_flags = flags; return -1; }

    virtual int timedReadv(const btes_Iovec         *buffer,
                           int                       numBytes,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0)
    { d_fun = 7; d_flags = flags; return -1; }

    virtual int timedReadv(int                      *augStatus,
                           const btes_Iovec         *buffer,
                           int                       numBytes,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0)
    { d_fun = 8; d_flags = flags; return -1; }

    virtual int readRaw(char *buffer,
                        int   numBytes,
                        int   flags = 0)
    { d_fun = 9; d_flags = flags; return -1; }

    virtual int readRaw(int  *augStatus,
                        char *buffer,
                        int   numBytes,
                        int   flags = 0)
    { d_fun = 10; d_flags = flags; return -1; }

    virtual int timedReadRaw(char                     *buffer,
                             int                       numBytes,
                             const bdet_TimeInterval&  timeout,
                             int                       flags = 0)
    { d_fun = 11; d_flags = flags; return -1; }

    virtual int timedReadRaw(int                      *augStatus,
                             char                     *buffer,
                             int                       numBytes,
                             const bdet_TimeInterval&  timeout,
                             int                       flags = 0)
    { d_fun = 12; d_flags = flags; return -1; }

    virtual int readvRaw(const btes_Iovec *buffers,
                         int               numBuffers,
                         int               flags = 0)
    { d_fun = 13; d_flags = flags; return -1; }

    virtual int readvRaw(int              *augStatus,
                         const btes_Iovec *buffers,
                         int               numBuffers,
                         int               flags = 0)
    { d_fun = 14; d_flags = flags; return -1; }

    virtual int timedReadvRaw(const btes_Iovec         *buffers,
                              int                       numBuffers,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0)
    { d_fun = 15; d_flags = flags; return -1; }

    virtual int timedReadvRaw(int                      *augStatus,
                              const btes_Iovec         *buffers,
                              int                       numBuffers,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0)
    { d_fun = 16; d_flags = flags; return -1; }

    virtual int bufferedRead(const char **buffer,
                             int          numBytes,
                             int          flags = 0)
    { d_fun = 17; d_flags = flags; return -1; }

    virtual int bufferedRead(int         *augStatus,
                             const char **buffer,
                             int          numBytes,
                             int          flags = 0)
    { d_fun = 18; d_flags = flags; return -1; }

    virtual int timedBufferedRead(const char              **buffer,
                                  int                       numBytes,
                                  const bdet_TimeInterval&  timeout,
                                  int                       flags = 0)
    { d_fun = 19; d_flags = flags; return -1; }

    virtual int timedBufferedRead(int                      *augStatus,
                                  const char              **buffer,
                                  int                       numBytes,
                                  const bdet_TimeInterval&  timeout,
                                  int                       flags = 0)
    { d_fun = 20; d_flags = flags; return -1; }

    virtual int bufferedReadRaw(const char **buffer,
                                int          numBytes,
                                int          flags = 0)
    { d_fun = 21; d_flags = flags; return -1; }

    virtual int bufferedReadRaw(int                      *augStatus,
                                const char              **buffer,
                                int                       numBytes,
                                int                       flags = 0)
    { d_fun = 22; d_flags = flags; return -1; }

    virtual int timedBufferedReadRaw(const char              **buffer,
                                     int                       numBytes,
                                     const bdet_TimeInterval&  timeout,
                                     int                       flags = 0)
    { d_fun = 23; d_flags = flags; return -1; }

    virtual int timedBufferedReadRaw(int                      *augStatus,
                                     const char              **buffer,
                                     int                       numBytes,
                                     const bdet_TimeInterval&  timeout,
                                     int                       flags = 0)
    { d_fun = 24; d_flags = flags; return -1; }

    virtual int write(const char *buffer,
                      int         numBytes,
                      int         flags = 0)
    { d_fun = 25; d_flags = flags; return -1; }

    virtual int write(int        *augStatus,
                      const char *buffer,
                      int         numBytes,
                      int         flags = 0)
    { d_fun = 26; d_flags = flags; return -1; }

    virtual int timedWrite(const char               *buffer,
                           int                       numBytes,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0)
    { d_fun = 27; d_flags = flags; return -1; }

    virtual int timedWrite(int                      *augStatus,
                           const char               *buffer,
                           int                       numBytes,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0)
    { d_fun = 28; d_flags = flags; return -1; }

    virtual int writeRaw(const char *buffer,
                         int         numBytes,
                         int         flags = 0)
    { d_fun = 29; d_flags = flags; return -1; }

    virtual int writeRaw(int        *augStatus,
                         const char *buffer,
                         int         numBytes,
                         int         flags = 0)
    { d_fun = 30; d_flags = flags; return -1; }

    virtual int timedWriteRaw(const char               *buffer,
                              int                       numBytes,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0)
    { d_fun = 31; d_flags = flags; return -1; }

    virtual int timedWriteRaw(int                      *augStatus,
                             const char               *buffer,
                             int                       numBytes,
                             const bdet_TimeInterval&  timeout,
                             int                       flags = 0)
    { d_fun = 32; d_flags = flags; return -1; }

    virtual int writev(const btes_Ovec *buffers,
                       int              numBuffers,
                       int              flags = 0)
    { d_fun = 33; d_flags = flags; return -1; }

    virtual int writev(const btes_Iovec *buffers,
                       int               numBuffers,
                       int               flags = 0)
    { d_fun = 34; d_flags = flags; return -1; }

    virtual int writev(int             *augStatus,
                       const btes_Ovec *buffers,
                       int              numBuffers,
                       int              flags = 0)
    { d_fun = 35; d_flags = flags; return -1; }

    virtual int writev(int              *augStatus,
                       const btes_Iovec *buffers,
                       int               numBuffers,
                       int               flags = 0)
    { d_fun = 36; d_flags = flags; return -1; }

    virtual int timedWritev(const btes_Ovec         *buffers,
                            int                      numBuffers,
                            const bdet_TimeInterval& timeout,
                            int                      flags = 0)
    { d_fun = 37; d_flags = flags; return -1; }

    virtual int timedWritev(const btes_Iovec         *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0)
    { d_fun = 38; d_flags = flags; return -1; }

    virtual int timedWritev(int                      *augStatus,
                            const btes_Ovec          *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0)
    { d_fun = 39; d_flags = flags; return -1; }

    virtual int timedWritev(int                      *augStatus,
                            const btes_Iovec         *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0)
    { d_fun = 40; d_flags = flags; return -1; }

    virtual int writevRaw(const btes_Ovec *buffers,
                          int              numBuffers,
                          int              flags = 0)
    { d_fun = 41; d_flags = flags; return -1; }

    virtual int writevRaw(const btes_Iovec *buffers,
                          int               numBuffers,
                          int               flags = 0)
    { d_fun = 42; d_flags = flags; return -1; }

    virtual int writevRaw(int             *augStatus,
                          const btes_Ovec *buffers,
                          int              numBuffers,
                          int              flags = 0)
    { d_fun = 43; d_flags = flags; return -1; }

    virtual int writevRaw(int              *augStatus,
                          const btes_Iovec *buffers,
                          int               numBuffers,
                          int               flags = 0)
    { d_fun = 44; d_flags = flags; return -1; }

    virtual int timedWritevRaw(const btes_Ovec          *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0)
    { d_fun = 45; d_flags = flags; return -1; }

    virtual int timedWritevRaw(const btes_Iovec         *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0)
    { d_fun = 46; d_flags = flags; return -1; }

    virtual int timedWritevRaw(int                      *augStatus,
                               const btes_Ovec          *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0)
    { d_fun = 47; d_flags = flags; return -1; }

    virtual int timedWritevRaw(int                      *augStatus,
                               const btes_Iovec         *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0)
    { d_fun = 48; d_flags = flags; return -1; }

    virtual void invalidate()
    { d_fun = 49; d_valid = 0; }

    virtual int isInvalid() const
    {
      MyTimedChannel *const tmp = const_cast<MyTimedChannel* > (this);
      tmp-> d_fun = 50;
      return !d_valid;
    }

    // non-virtual functions for testing
    int fun()   const { return d_fun; }
    int flags() const { return d_flags; }
    int valid() const { return d_valid; }
};

   // The help function for the server is to calculate the factorial value.
   int factorial(int base)
   {
       if (1 >= base) {
           return 1;
       }
       else {
           return base * factorial(base - 1);
       }
    }

    int factorialClient(double *result, int input, btesc_TimedChannel *channel)
        // Load into the specified 'result' the factorial of the specified
        // input using the specified channel (which is assumed to be connected
        // to an appropriate factorial service).  Return 0 on success and -1,
        // with no effect on 'result', on error.  The behavior is undefined
        // unless 0 <= input.
    {
        ASSERT (0 <= input);
        enum {
            ERROR_STATUS = -1,
            SUCCESS_STATUS = 0,
            TIMEOUT_STATUS = +1
        };

        enum {
            WRITE_TIMEOUT = 1,
            READ_TIMEOUT  = 5
        };
        bdet_TimeInterval timeNow = bdetu_SystemTime::now();
        bdet_TimeInterval writeTimeout(timeNow + WRITE_TIMEOUT);
        bdet_TimeInterval readTimeout(timeNow + READ_TIMEOUT);

        int numBytes = sizeof input;
        int writeStatus = channel->timedWrite((const char *)&input,
                                              numBytes,
                                              writeTimeout);
        ASSERT(0 != writeStatus);

        if (writeStatus != numBytes) {
            return ERROR_STATUS;
        }

        int readStatus = channel->timedRead((char *)result,
                                            sizeof *result,
                                            readTimeout);
        if (readStatus != sizeof *result) {
            return ERROR_STATUS;
        }

        return SUCCESS_STATUS;
    }

    int factorialServer(btesc_TimedChannel *channel)
        // Repeatedly read integer sequences from the specified
        // channel.  Return -1 if the read operation fails.
        // When a read succeeds, interpret the byte sequence as an
        // integer value in host-byte order.  Return 0 if that value is
        // negative.  Otherwise, calculate the factorial of the (non-negative)
        // integer and write the result to 'channel' as a sequence of bytes
        // representing a 'double' in the host's native format.
    {
        enum {
            WRITE_ERROR   = -2,
            READ_ERROR    = -1,
            SUCCESS       =  0,
            WRITE_TIMEOUT =  1
        };

        enum { TIMEOUT = 2 };
        bdet_TimeInterval timeNow = bdetu_SystemTime::now();
        bdet_TimeInterval timeout(timeNow + TIMEOUT);

        while (1) {
            int input;
            int readStatus = channel->timedRead((char *) &input,
                                                sizeof input,
                                                timeout);
            if (readStatus != sizeof input) {
                return READ_ERROR;
            }

            if (input < 0) {
                return SUCCESS;
            }

            double result = factorial(input);
            int writeStatus = channel->timedWrite((const char *)&result,
                                                  sizeof input,
                                                  timeout);
            if (writeStatus != sizeof input) {
                return WRITE_ERROR;
            }
        }
    }

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Test the code provided in the component header file as a client.
        //   Create an object of 'MyTimedChannel', invoke the function under
        //   test by passing the 'MyTimedChannel' object to the function.
        // Testing:
        //   USAGE EXAMPLE: factorialClient();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;
        {
            if (veryVerbose) {
              cout << "Usage example as the factorialClient" << endl
                   << "====================================" << endl;
            }
            MyTimedChannel myTimedChannel;
            int input = 5;
            double result = 0;
            int ret = factorialClient(&result, input, &myTimedChannel);
            ASSERT(0 > ret);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Test the code provided in the component header file as a client.
        //   Create an object of 'MyTimedChannel', invoke the function under
        //   test by passing the 'MyTimedChannel' object to the function.
        // Testing:
        //   USAGE EXAMPLE: factorialServer();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;
        {
            if (veryVerbose) {
              cout << "Usage example as the factorialServer" << endl
                   << "====================================" << endl;
            }
            MyTimedChannel myTimedChannel;
            int ret = factorialServer(&myTimedChannel);

            ASSERT(0 > ret);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a concrete subclass of the
        //   'btesc_TimedChannel' class compiles and links when all virtual
        //   functions are defined.
        // Testing:
        //   virtual ~btesc_TimedChannel(...)
        //   virtual int read(...)
        //   virtual int timedRead(...)
        //   virtual int readv(...)
        //   virtual int timedReadv(...)
        //   virtual int readRaw(...)
        //   virtual int timedReadRaw(...)
        //   virtual int readvRaw(...)
        //   virtual int timedReadvRaw(...)
        //   virtual int bufferedRead(...)
        //   virtual int timedBufferedRead(...)
        //   virtual int bufferedReadRaw(...)
        //   virtual int timedBufferedReadRaw(...)
        //   virtual int write(...)
        //   virtual int timedWrite(...)
        //   virtual int writev(...)
        //   virtual int timedWritev(...)
        //   virtual int writeRaw(...)
        //   virtual int timedWriteRaw(...)
        //   virtual int writevRaw(const btes_Ovec *buffers, ...)
        //   virtual int writevRaw(const btes_Iovec *buffers, ...)
        //   virtual int timedWritevRaw(const btes_Ovec *buffers, ...)
        //   virtual int timedWritevRaw(const btes_Iovec *buffers, ...)
        //   virtual void invalidate(...)
        //   virtual int isInvalid(...)
        //   virtual int isInvalidWrite(...)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        // Ensure that an instance of the derived class can be created.
        MyTimedChannel myC;
        btesc_TimedChannel& c = myC;
        ASSERT(1 == myC.valid());

        if (verbose) cout << "\nTesting protocol interface"
                          << "\n==========================" << endl;
        {
            // Invoke different methods via the base class reference and check
            // if the derived class method is invoked.

            // Flags to indicate that async interrupts are allowed.
            int myFlags = btesc_Flag::BTESC_ASYNC_INTERRUPT;
            int augStatus = 0;
            bdet_TimeInterval timeout(bdetu_SystemTime::now());
            ASSERT(0 == myC.flags());

            c.read(0, 0);
            ASSERT(1 == myC.fun()); ASSERT(0 == myC.flags());
            c.read(0, 0, myFlags);
            ASSERT(1 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.read(&augStatus, 0, 0, myFlags);
            ASSERT(2 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedRead(0, 0, timeout);
            ASSERT(3 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedRead(0, 0, timeout, myFlags);
            ASSERT(3 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedRead(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(4 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.readv(0, 0);
            ASSERT(5 == myC.fun()); ASSERT(0 == myC.flags());
            c.readv(0, 0, myFlags);
            ASSERT(5 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.readv(&augStatus, 0, 0, myFlags);
            ASSERT(6 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedReadv(0, 0, timeout);
            ASSERT(7 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedReadv(0, 0, timeout, myFlags);
            ASSERT(7 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedReadv(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(8 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.readRaw(0, 0);
            ASSERT(9 == myC.fun()); ASSERT(0 == myC.flags());
            c.readRaw(0, 0, myFlags);
            ASSERT(9 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.readRaw(&augStatus, 0, 0, myFlags);
            ASSERT(10 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedReadRaw(0, 0, timeout);
            ASSERT(11 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedReadRaw(0, 0, timeout, myFlags);
            ASSERT(11 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedReadRaw(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(12 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.readvRaw(0, 0);
            ASSERT(13 == myC.fun()); ASSERT(0 == myC.flags());
            c.readvRaw(0, 0, myFlags);
            ASSERT(13 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.readvRaw(&augStatus, 0, 0, myFlags);
            ASSERT(14 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedReadvRaw(0, 0, timeout);
            ASSERT(15 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedReadvRaw(0, 0, timeout, myFlags);
            ASSERT(15 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedReadvRaw(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(16 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedRead(0, 0);
            ASSERT(17 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedRead(0, 0, myFlags);
            ASSERT(17 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.bufferedRead(&augStatus, 0, 0, myFlags);
            ASSERT(18 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedBufferedRead(0, 0, timeout);
            ASSERT(19 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedBufferedRead(0, 0, timeout, myFlags);
            ASSERT(19 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedBufferedRead(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(20 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.bufferedReadRaw(0, 0);
            ASSERT(21 == myC.fun()); ASSERT(0 == myC.flags());
            c.bufferedReadRaw(0, 0, myFlags);
            ASSERT(21 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.bufferedReadRaw(&augStatus, 0, 0, myFlags);
            ASSERT(22 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedBufferedReadRaw(0, 0, timeout);
            ASSERT(23 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedBufferedReadRaw(0, 0, timeout, myFlags);
            ASSERT(23 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedBufferedReadRaw(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(24 == myC.fun()); ASSERT(myFlags == myC.flags());

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            c.write(0, 0);
            ASSERT(25 == myC.fun()); ASSERT(0 == myC.flags());
            c.write(0, 0, myFlags);
            ASSERT(25 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.write(&augStatus, 0, 0, myFlags);
            ASSERT(26 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWrite(0, 0, timeout);
            ASSERT(27 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedWrite(0, 0, timeout, myFlags);
            ASSERT(27 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedWrite(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(28 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writeRaw(0, 0);
            ASSERT(29 == myC.fun()); ASSERT(0 == myC.flags());
            c.writeRaw(0, 0, myFlags);
            ASSERT(29 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.writeRaw(&augStatus, 0, 0, myFlags);
            ASSERT(30 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWriteRaw(0, 0, timeout);
            ASSERT(31 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedWriteRaw(0, 0, timeout, myFlags);
            ASSERT(31 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedWriteRaw(&augStatus, 0, 0, timeout, myFlags);
            ASSERT(32 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writev(static_cast<const btes_Ovec *> (0), 0);
            ASSERT(33 == myC.fun()); ASSERT(0 == myC.flags());
            c.writev(static_cast<const btes_Ovec *> (0), 0, myFlags);
            ASSERT(33 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writev(static_cast<const btes_Iovec *> (0), 0);
            ASSERT(34 == myC.fun()); ASSERT(0 == myC.flags());
            c.writev(static_cast<const btes_Iovec *> (0), 0, myFlags);
            ASSERT(34 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writev(&augStatus, static_cast<const btes_Ovec *> (0), 0,
                                                                 myFlags);
            ASSERT(35 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.writev(&augStatus, static_cast<const btes_Iovec *> (0), 0,
                                                                 myFlags);
            ASSERT(36 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWritev(static_cast<const btes_Ovec *> (0), 0, timeout);
            ASSERT(37 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedWritev(static_cast<const btes_Ovec *> (0), 0,
                                                           timeout, myFlags);
            ASSERT(37 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWritev(static_cast<const btes_Iovec *> (0), 0, timeout);
            ASSERT(38 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedWritev(static_cast<const btes_Iovec *> (0), 0,
                                                           timeout, myFlags);
            ASSERT(38 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWritev(&augStatus, static_cast<const btes_Ovec *> (0), 0,
                                                           timeout,  myFlags);
            ASSERT(39 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedWritev(&augStatus, static_cast<const btes_Iovec *> (0), 0,
                                                           timeout,  myFlags);
            ASSERT(40 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writevRaw(static_cast<const btes_Ovec *> (0), 0);
            ASSERT(41 == myC.fun()); ASSERT(0 == myC.flags());
            c.writevRaw(static_cast<const btes_Ovec *> (0), 0, myFlags);
            ASSERT(41 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writevRaw(static_cast<const btes_Iovec *> (0), 0);
            ASSERT(42 == myC.fun()); ASSERT(0 == myC.flags());
            c.writevRaw(static_cast<const btes_Iovec *> (0), 0, myFlags);
            ASSERT(42 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.writevRaw(&augStatus, static_cast<const btes_Ovec *> (0), 0,
                                                                 myFlags);
            ASSERT(43 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.writevRaw(&augStatus, static_cast<const btes_Iovec *> (0), 0,
                                                                 myFlags);
            ASSERT(44 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWritevRaw(static_cast<const btes_Ovec *> (0), 0, timeout);
            ASSERT(45 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedWritevRaw(static_cast<const btes_Ovec *> (0), 0,
                                                           timeout, myFlags);
            ASSERT(45 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWritevRaw(static_cast<const btes_Iovec *> (0), 0, timeout);
            ASSERT(46 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedWritevRaw(static_cast<const btes_Iovec *> (0), 0,
                                                           timeout, myFlags);
            ASSERT(46 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedWritevRaw(&augStatus, static_cast<const btes_Ovec *> (0),
                                                     0, timeout,  myFlags);
            ASSERT(47 == myC.fun()); ASSERT(myFlags == myC.flags());
            c.timedWritevRaw(&augStatus, static_cast<const btes_Iovec *> (0),
                                                     0, timeout,  myFlags);
            ASSERT(48 == myC.fun()); ASSERT(myFlags == myC.flags());

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            ASSERT(0 == myC.isInvalid()); // non-virtual test method
            c.invalidate();               ASSERT(49 == myC.fun());
            ASSERT(1 == c.isInvalid());   ASSERT(50 == myC.fun());
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
