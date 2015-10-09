// btlso_resolveutil.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_resolveutil.h>
#include <btlso_sockethandle.h>
#include <btlso_socketimputil.h>
#include <btlso_ipv4address.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_c_sys_time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>  // MAXHOSTNAMELEN (ibm and others)
#include <netdb.h>      // MAXHOSTNAMELEN (sun)
#else
#include <windows.h>

#define POPEN
#define popen _popen
#define pclose _pclose

#define MAXHOSTNAMELEN 256

#endif

#include <bsl_c_stdio.h>

#include <bsl_algorithm.h>
#include <bsl_iterator.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
// This class provides a namespace for platform-independent thread-safe
// procedures to get the IPv4 address and/or port number based on the names,
// and the canonical hostname for a host based on its IPv4 address (or for the
// local host).  The basic plan for testing is to ensure that all methods in
// the component pass correct parameters, and honor the contract.  Test set
// data is generally selected to make sure this testing has gone through all
// code branches in the function being tested.
// ----------------------------------------------------------------------------
// ACCESSORS
// [ 1] int getAddress(btlso::IPv4Address *result, hostname, errorCode = 0);
// [ 1] int getAddressDefault(btlso::IPv4Address *result, hostname, errorCode);
// [ 2] int getAddresses(vector<btlso::IPv4Address> *, hostname, errorCode);
// [ 3] int getServicePort(btlso::IPv4Address *result, svc, proto, errorCode);
// [ 4] int getHostnameByAddress(hostname, address, errorCode);
// [ 5] int getLocalHostname(bsl::string *hostname);
// [ 6] ResolveByNameCallback setResolveByNameCallback(callback);
// [ 6] ResolveByNameCallback currentResolveByNameCallback();
// [ 6] ResolveByNameCallback defaultResolveByNameCallback();
// ----------------------------------------------------------------------------
// [ 7] USAGE example
// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

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

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << (I) << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " \
              << (J) << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) \
              << "\t"  << #K << ": " << (K) << "\n"; \
              aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) \
       << "\t" << #K << ": " << (K) << "\t" << #L << ": " << (L) << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) \
       << "\t" << #K << ": " << (K) << "\t" << #L << ": " << (L) << "\t" << \
       #M << ": " << (M) << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btlso::ResolveUtil                        Obj;
typedef btlso::ResolveUtil::ResolveByNameCallback Callback;
typedef btlso::SocketHandle::Handle               SocketHandle;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsl::map<bsl::string, bsl::vector<btlso::IPv4Address> >  MyCallbackMap;

MyCallbackMap myCallbackMap;

int myResolveByNameCallback(bsl::vector<btlso::IPv4Address> *hostAddresses,
                            const char                      *hostName,
                            int                              numAddresses,
                            int                             *errorCode)
    // Resolves the IP address(es) of the specified 'hostname' and the
    // specified 'hostAddresses' with a maximum number equal to the specified
    // 'numAddresses' (or unlimited number if 'numAddresses' is -1) of the IPv4
    // addresses if resolution succeeds.  Resolution is performed by lookup in
    // the global 'myCallbackMap'.  Return 0 with no effect on 'errorCode' upon
    // success, and otherwise returns a negative value.  Upon failure, also
    // load a native error code into 'errorCode'.  In any case, any
    // 'hostAddresses' entries present upon return will contain a valid
    // IPv4Address corresponding to the 'hostName', and 'hostAddresses' will be
    // resized accordingly.
{
    enum { NOT_FOUND = -1 };

    // Note that we must clear 'hostAddresses' in any case, as per the
    // contract.

    hostAddresses->clear();

    // Map lookup, return -1, if not found.

    MyCallbackMap::iterator iter = myCallbackMap.find(bsl::string(hostName));

    if (iter == myCallbackMap.end()) {
        if (errorCode) {
            *errorCode = NOT_FOUND;
        }
        return -1;                                                    // RETURN
    }

    // Copy up to 'numAddresses' addresses (or all if 'numAddresses' is -1)
    // into the output 'hostAddresses'.

    if (numAddresses == -1) {
        *hostAddresses = iter->second;
    } else {
        ASSERT(hostAddresses != 0);

        int actualNumAdresses = bsl::min(numAddresses,
                                         (int)iter->second.size());
        hostAddresses->assign(iter->second.begin(),
                              iter->second.begin() + actualNumAdresses);
    }
    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    const int FAIL = -1;
    const int SUCCESS = 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    int errCode=0;
    btlso::SocketImpUtil::startup(&errCode);
    ASSERT(0 == errCode);
    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // always the leading case.
        case 7: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'assert' with
            //   'ASSERT'.  Also disable output unless 'verbose'.
            //
            // Testing:
            //   USAGE EXAMPLE
            // ----------------------------------------------------------------
            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Resolving an IP address
///- - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to get an IPv4address or
// a port number from a name.  First we create two 'btlso::IPv4Address' objects
// to store the return value for host 'p111', 'ibm1' respectively, and verify
// the return addresses:
//..
        int                retCode = 0, errorCode = 0;
        btlso::IPv4Address result1, result2;

        const char *hostname = "p111";
        retCode = btlso::ResolveUtil::getAddress(&result1,
                                                 hostname,
                                                 &errorCode);
        ASSERT(0 == retCode);
        ASSERT(0 == errorCode);
        if (verbose)
            bsl::cout << "IPv4 address for '" << hostname << "': " << result1
                      << bsl::endl;

        const char *hostname2 = "ibm1";
        retCode = btlso::ResolveUtil::getAddress(&result2,
                                                 hostname2,
                                                 &errorCode);
        ASSERT(0 == retCode);
        ASSERT(0 == errorCode);
        if (verbose)
            bsl::cout << "IPv4 address for '" << hostname2 << "': " << result2
                      << bsl::endl;
//..
// Next we try retrieving all IPv4 addresses for the given host name 'n024'
// and verify the return addresses:
//..
        bsl::vector<btlso::IPv4Address> addresses;
        btlso::IPv4Address result3;

        const char *hostname3 = "n024";
        retCode = btlso::ResolveUtil::getAddresses(&addresses,
                                                   hostname3,
                                                   &errorCode);
        ASSERT(0 == retCode);
        ASSERT(0 == errorCode);
        if (verbose) {
            bsl::cout << "IPv4 addresses for '" << hostname3 << "': ";
            bsl::copy(addresses.begin(),
                      addresses.end(),
                      bsl::ostream_iterator<btlso::IPv4Address>(bsl::cout,
                                                                " "));
            bsl::cout << bsl::endl;
        }
//..
// Finally, we try to get the service port number of the given service
// names 'ftp', 'systat', 'telnet' (in "tcp") respectively, and verify the
// results:
//..
        const char *servname = "ftp";
        retCode = btlso::ResolveUtil::getServicePort(&result1,
                                                     servname,
                                                     0,
                                                     &errorCode);
        ASSERT(0 == retCode);
        if (verbose)
            bsl::cout << "Port number for 'ftp': " << result1 << bsl::endl;

        const char *servname2 = "systat";
        retCode = btlso::ResolveUtil::getServicePort(&result2,
                                                     servname2,
                                                     0,
                                                     &errorCode);
        ASSERT(0 == retCode);
        if (verbose)
            bsl::cout << "Port number for 'systat': " << result1 << bsl::endl;

        const char *servname3 = "telnet";
        retCode = btlso::ResolveUtil::getServicePort(&result3,
                                                     servname3,
                                                     "tcp",
                                                     &errorCode);
        ASSERT(0 == retCode);
        if (verbose)
            bsl::cout << "Port number for 'telnet': " << result1 << bsl::endl;
//..

        } break;
        case 6: {
            // ----------------------------------------------------------------
            // TESTING USER-INSTALLED CALLBACKS
            //
            // Plan:
            //   Redo the test cases of case 1 and case 2, but for each data
            //   first set the callback to a user-defined callback which uses a
            //   global map (initially empty), and check that resolution fails,
            //   then add the test data to the map and check that it now
            //   succeeds, then set the callback to the initial callback and
            //   verify that it works as initially in the corresponding test
            //   case.
            //
            // Testing
            //   ResolveByNameCallback setResolveByNameCallback(callback);
            //   ResolveByNameCallback currentResolveByNameCallback();
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting setResolveByNameCallback"
                              << "\n================================" << endl;

            if (verbose) cout << "\n\twith getAddress." << endl;
            {
                int retCode = 0, errCode = 0;

                enum {
                    UNUSED_VALUE = (int)0xDEADBEEF
                };

                struct {
                    int         d_lineNum;    // line number
                    const char *d_hostName;   // hostname string
                    int        *d_errCode_p;  // NULL or valid address
                    const char *d_expAddr;    // expected IPv4 address string
                    int         d_expRet;     // the expected return value
                } DATA[] =
                //  line   hostName      errCode   expAddr           expRet
                //  ----   --------      -------   ---------------   --------
                {
                #if defined(BSLS_PLATFORM_OS_UNIX)
                    { L_,  "jlu1",       &errCode, 0,                FAIL    },
                    { L_,  "jlu1",              0, 0,                FAIL    },
                    { L_,  "sdv1",       &errCode, "172.17.1.20",    SUCCESS },
                    { L_,  "sdv1",              0, "172.17.1.20",    SUCCESS },
                    { L_,  "sdv3",       &errCode, "10.122.130.92",  SUCCESS },
                    { L_,  "sdv3",              0, "10.122.130.92",  SUCCESS },
                    { L_,  "jlu_wrong",  &errCode, 0,                FAIL    },
                    { L_,  "jlu_wrong",         0, 0,                FAIL    },
                    { L_,  "n299",       &errCode, "10.126.48.90",   SUCCESS },
                #elif defined(BSLS_PLATFORM_OS_WINDOWS)
                    { L_,  "bny14",      &errCode, 0,                FAIL    },
                    { L_,  "bny14",             0, 0,                FAIL    },
                    { L_,  "330west",    &errCode, 0,                FAIL    },
                    { L_,  "330west",           0, 0,                FAIL    },
                    { L_,  "Dgm3w1",     &errCode, "160.43.7.102",   SUCCESS },
                    { L_,  "Dgm3w1",            0, "160.43.7.102",   SUCCESS },
                    { L_,  "Dgm3w10",    &errCode, "160.43.7.13",    SUCCESS },
                    { L_,  "Dgm3w10",           0, "160.43.7.13",    SUCCESS },
                #endif
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE      = DATA[i].d_lineNum;
                    const char *HOST_NAME = DATA[i].d_hostName;
                          int  *errorPtr  = DATA[i].d_errCode_p;
                    const char *EXP_ADDR  = DATA[i].d_expAddr;
                    const int   EXP_RET   = DATA[i].d_expRet;

                    btlso::IPv4Address expResult;
                    if (EXP_ADDR) {
                        expResult.setIpAddress(EXP_ADDR);
                    }
                    const btlso::IPv4Address& EXP = expResult;

                    if (veryVerbose) {
                        P_(LINE); P_(HOST_NAME); P_(EXP); P(EXP_RET);
                    }

                    if (veryVerbose)
                        cout << "\t\tWith user-installed callback." << endl;

                    Callback expCallback = Obj::defaultResolveByNameCallback();
                    Callback callback    = Obj::currentResolveByNameCallback();
                    LOOP3_ASSERT(LINE, expCallback, callback,
                                 expCallback == callback);

                    callback = Obj::setResolveByNameCallback(
                                                     &myResolveByNameCallback);
                    LOOP3_ASSERT(LINE, expCallback, callback,
                                 expCallback == callback);

                    // Failed attempt.

                    btlso::IPv4Address result;
                    errCode = UNUSED_VALUE;

                    retCode = Obj::getAddress(&result, HOST_NAME, errorPtr);

                    LOOP2_ASSERT(LINE, retCode, FAIL == retCode)

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                    }

                    // Successful attempt.

                    expResult = btlso::IPv4Address("123.45.67.89", 0);
                    myCallbackMap[HOST_NAME].push_back(expResult);
                    result = btlso::IPv4Address();
                    errCode = UNUSED_VALUE;

                    retCode = Obj::getAddress(&result, HOST_NAME, errorPtr);

                    LOOP2_ASSERT(LINE, retCode, SUCCESS == retCode);
                    LOOP2_ASSERT(LINE, result, EXP == result);

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr,
                                     UNUSED_VALUE == *errorPtr);
                    }

                    myCallbackMap.erase(myCallbackMap.find(HOST_NAME));

                    if (veryVerbose)
                        cout << "\t\tWith default callback." << endl;

                    expCallback = Obj::defaultResolveByNameCallback();
                    callback    = Obj::currentResolveByNameCallback();
                    LOOP3_ASSERT(LINE, expCallback, callback,
                                 expCallback != callback);

                    callback = Obj::setResolveByNameCallback(expCallback);
                    LOOP3_ASSERT(LINE, (void *) &myResolveByNameCallback,
                                 callback,
                                 &myResolveByNameCallback == callback);

                    if (EXP_ADDR) {
                        expResult.setIpAddress(EXP_ADDR);
                    } else {
                        expResult = btlso::IPv4Address();
                    }
                    result = btlso::IPv4Address();
                    errCode = UNUSED_VALUE;

                    retCode = Obj::getAddress(&result, HOST_NAME, errorPtr);

                    LOOP2_ASSERT(LINE, retCode, EXP_RET == retCode);

                    if (FAIL == EXP_RET) {
                        // For entries w/ invalid name.

                        if (errorPtr) {
                            LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                        }
                    }
                    else {
                        // Retrieved successfully.

                        LOOP2_ASSERT(LINE, result, EXP == result);
                        if (errorPtr) {
                            LOOP2_ASSERT(LINE, *errorPtr,
                                         UNUSED_VALUE == *errorPtr);
                        }
                    }
                }
            }

            if (verbose) cout << "\n\twith getAddresses." << endl;
            {
                enum {
                    UNUSED_VALUE = (int)0xDEADBEEF
                };
                btlso::IPv4Address UNUSED_ADDRESS("1.2.3.4", 1000);

                int errCode = UNUSED_VALUE;

            #ifdef BSLS_PLATFORM_OS_UNIX
                const char *applix[]  = { "86.0.0.32",
                                      #if !defined(BSLS_PLATFORM_OS_LINUX) \
                                       && !defined(BSLS_PLATFORM_OS_FREEBSD)
                                          "86.0.0.43",
                                          "87.0.0.36",
                                          "87.0.0.131",
                                      #endif
                                          0
                                        };
                const char *soros[] = { "202.217.132.211",
                                    #if !defined(BSLS_PLATFORM_OS_LINUX) \
                                     && !defined(BSLS_PLATFORM_OS_FREEBSD)
                                        "202.217.132.212",
                                        "202.217.132.213",
                                    #endif
                                        0
                                      };
                const char *fft[]   = { "192.168.218.1",
                                    #if defined(BSLS_PLATFORM_OS_AIX)        \
                                     || (defined(BSLS_PLATFORM_OS_SOLARIS) &&\
                                         (BSLS_PLATFORM_OS_VER_MAJOR >= 10 ||\
                                          defined(BSLS_PLATFORM_CMP_GNU)   ||\
                                          defined(BSLS_PLATFORM_CMP_CLANG)))
                                        "192.168.219.1",
                                    #endif
                                        0
                                      };
                const char *yusen[] = { "192.168.79.34",
                                    #if defined(BSLS_PLATFORM_OS_AIX)        \
                                     || defined(BSLS_PLATFORM_OS_SOLARIS)   \
                                     || defined(BSLS_PLATFORM_OS_HPUX)
                                        "192.168.79.65",
                                    #endif
                                        0
                                      };
            #else // defined BSLS_PLATFORM_OS_WINDOWS
                // Cannot come up with any multi-homed resolvable hosts.

        const char* sundev1[] = { "172.17.1.20",    0 };
        const char* sundev5[] = { "10.126.159.229", 0 };
        const char* ibm1[]    = { "172.17.5.40",    0 };
        const char* p111[]    = { "10.126.161.151", 0 };
            #endif

                struct {
                    int          d_lineNum;   // line number
                    const char  *d_hostName;  // hostname string
                    int         *d_errCode_p; // NULL or valid address
                    const char **d_expAddr;   // expected IPv4 address array
                    int          d_expRet;    // expected return value
                } DATA[] =
                //  line  hostname              errorCode  expAddr   expRet
                //  ----  --------              ---------  -------   ------
                {
            #ifdef BSLS_PLATFORM_OS_UNIX
                    { L_, "jlu1",               &errCode,       0,     FAIL },
                    { L_, "jlu1",                      0,       0,     FAIL },
                    { L_, "applix",             &errCode,  applix,  SUCCESS },
                    { L_, "applix",                    0,  applix,  SUCCESS },
                    { L_, "soros.wk1",          &errCode,   soros,  SUCCESS },
                    { L_, "soros.wk1",                 0,   soros,  SUCCESS },
                    { L_, "jlu_wrong",          &errCode,       0,     FAIL },
                    { L_, "jlu_wrong",                 0,       0,     FAIL },
                    { L_, "fft-corp-rtr1",      &errCode,     fft,  SUCCESS },
                    { L_, "yusen-rtr2",         &errCode,   yusen,  SUCCESS },
            #elif defined BSLS_PLATFORM_OS_WINDOWS
                    { L_, "sundev0",            &errCode,        0,       FAIL
                                                                            },
                    { L_, "sundev1",            &errCode,  sundev1,    SUCCESS
                                                                            },
                    { L_, "sundev1",                   0,  sundev1,    SUCCESS
                                                                            },
                    { L_, "sundev5",            &errCode,  sundev5,    SUCCESS
                                                                            },
                    { L_, "sundev5",                   0,  sundev5,    SUCCESS
                                                                            },
                    { L_, "ibm1",               &errCode,     ibm1,    SUCCESS
                                                                            },
                    { L_, "p111",               &errCode,     p111,    SUCCESS
                                                                            }
            #else

            #error getHostnameByAddress does not handle current platform type!

            #endif
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int    LINE      = DATA[i].d_lineNum;
                    const char  *HOST_NAME = DATA[i].d_hostName;
                          int   *errorPtr  = DATA[i].d_errCode_p;
                    const char **EXP_ADDR  = DATA[i].d_expAddr;
                    const int    EXP_RET   = DATA[i].d_expRet;

                    bsl::vector<btlso::IPv4Address> resultArray(16,
                                                               UNUSED_ADDRESS,
                                                               &testAllocator);
                    ASSERT(16 == resultArray.size());

                    if (veryVerbose) {
                        P_(LINE); P(HOST_NAME);
                    }

                    int retCode;

                    if (verbose) {
                        P_(LINE);
                        copy(resultArray.begin(), resultArray.end(),
                             ostream_iterator<btlso::IPv4Address>(cout, " "));
                        cout << endl;
                    }

                    if (veryVerbose)
                        cout << "\t\tWith user-installed callback." << endl;

                    Callback expCallback = Obj::defaultResolveByNameCallback();
                    Callback callback    = Obj::currentResolveByNameCallback();
                    LOOP3_ASSERT(LINE, expCallback, callback,
                                 expCallback == callback);

                    callback = Obj::setResolveByNameCallback(
                                                     &myResolveByNameCallback);
                    LOOP3_ASSERT(LINE, expCallback, callback,
                                 expCallback == callback);

                    // Failed attempt.

                    resultArray.assign(16, UNUSED_ADDRESS);
                    errCode = UNUSED_VALUE;

                    retCode = Obj::getAddresses(&resultArray,
                                                HOST_NAME,
                                                errorPtr);

                    LOOP2_ASSERT(LINE, retCode, FAIL == retCode)
                    LOOP2_ASSERT(LINE, resultArray.size(),
                                 resultArray.empty());

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                    }

                    // Successful attempt.

                    btlso::IPv4Address expResult("123.45.67.89", 0);
                    const btlso::IPv4Address& EXP_RESULT = expResult;
                    myCallbackMap[HOST_NAME].push_back(expResult);
                    myCallbackMap[HOST_NAME].push_back(expResult);
                    myCallbackMap[HOST_NAME].push_back(expResult);
                    resultArray.assign(16, UNUSED_ADDRESS);
                    errCode = UNUSED_VALUE;

                    retCode = Obj::getAddresses(&resultArray,
                                                HOST_NAME,
                                                errorPtr);

                    LOOP2_ASSERT(LINE, retCode, SUCCESS == retCode);
                    LOOP2_ASSERT(LINE, resultArray.size(),
                                 3 == resultArray.size());
                    LOOP2_ASSERT(LINE, resultArray[0],
                                 EXP_RESULT == resultArray[0]);
                    LOOP2_ASSERT(LINE, resultArray[1],
                                 EXP_RESULT == resultArray[1]);
                    LOOP2_ASSERT(LINE, resultArray[2],
                                 EXP_RESULT == resultArray[2]);

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr,
                                     UNUSED_VALUE == *errorPtr);
                    }

                    myCallbackMap.erase(myCallbackMap.find(HOST_NAME));

                    if (veryVerbose)
                        cout << "\t\tWith default callback." << endl;

                    expCallback = Obj::defaultResolveByNameCallback();
                    callback    = Obj::currentResolveByNameCallback();
                    LOOP3_ASSERT(LINE, expCallback, callback,
                                 expCallback != callback);

                    callback = Obj::setResolveByNameCallback(expCallback);
                    LOOP3_ASSERT(LINE, (void *) &myResolveByNameCallback,
                                 callback,
                                 &myResolveByNameCallback == callback);

                    resultArray.assign(16, UNUSED_ADDRESS);
                    errCode = UNUSED_VALUE;

                    retCode = Obj::getAddresses(&resultArray,
                                                HOST_NAME,
                                                errorPtr);

                    LOOP_ASSERT(LINE, EXP_RET == retCode);
                    if (FAIL == EXP_RET) {
                        // Entries with invalid name.

                        typedef bsl::vector<btlso::IPv4Address> VectorIP;
                        VectorIP expResultArray;
                        const VectorIP& EXP_RESULT_ARRAY = expResultArray;

                        LOOP_ASSERT(LINE, EXP_RESULT_ARRAY == resultArray);
                        if (EXP_RESULT_ARRAY != resultArray) {
                            cout << "Expected: ";
                            copy(EXP_RESULT_ARRAY.begin(),
                                 EXP_RESULT_ARRAY.end(),
                                 ostream_iterator<btlso::IPv4Address>(cout,
                                                                     " "));
                            cout << endl;
                            cout << "Got instead: ";
                            copy(resultArray.begin(), resultArray.end(),
                                 ostream_iterator<btlso::IPv4Address>(cout,
                                                                     " "));
                            cout << endl;

                        }
                        if (errorPtr) {
                            LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                        }
                    }
                    else {
                        // Retrieved successfully.

                        int j, k, length = resultArray.size();
                        for (j = 0; j < length; ++j) {
                            for (k = 0; EXP_ADDR[k]; ++k) {
                                if (btlso::IPv4Address(EXP_ADDR[k], 0) ==
                                    resultArray[j]) {
                                    break;
                                }
                            }
                            LOOP4_ASSERT(LINE, j, length, resultArray[j],
                                         EXP_ADDR[k] &&
                                         "Too many IP addresses returned");
                        }
                        for (j = 0; EXP_ADDR[j]; ++j) {
                            for (k = 0; k < length; ++k) {
                                if (btlso::IPv4Address(EXP_ADDR[j], 0) ==
                                    resultArray[k]) {
                                    break;
                                }
                            }
                            LOOP4_ASSERT(LINE, j, length, EXP_ADDR[j],
                                         k < length &&
                                         "Too few IP addresses returned");
                        }
                    }
                }
            }

        } break;
        case 5: {
            // ----------------------------------------------------------------
            // TESTING 'getLocalHostname' FUNCTION
            //
            // Plan:
            //   Get the system hostname from the hostname command --
            //   hostname(1) on Unix, hostname.exe on Windows -- and
            //   verify that the return value of 'getLocalHostname'
            //   matches this.  Hopefully no one will call sethostname()
            //   as superuser and with a different name during this time.
            //
            // Testing
            //   int getLocalHostname(bsl::string *hostname);
            // ----------------------------------------------------------------

            if (verbose) cout << "\nTesting getAddress"
                              << "\n==================" << endl;

            FILE *pfp = popen("hostname", "r");
            ASSERT(pfp);            // bump testStatus if false
            if (!pfp) break;        // cannot proceed

            char buf[MAXHOSTNAMELEN + 2] = { 0 };   // "\n\0"
            ASSERT(0 != fgets(buf, MAXHOSTNAMELEN + 2, pfp));
            ASSERT(*buf);

            pclose(pfp);

            if (veryVerbose)
                cout << "External hostname command: " << buf << endl;

            bsl::string hnam;
            int rc = Obj::getLocalHostname(&hnam);
            ASSERT(0 == rc);

            if (*buf && !rc) {
                int len = hnam.size();
                ASSERT(0 == strncmp(buf, hnam.c_str(), len)
                       && (buf[len] == '\n' || buf[len] == '\0'));
            }

            if (veryVerbose)
                cout << "getLocalHostname: " << hnam << endl;

        } break;
        case 4: {
            // ----------------------------------------------------------------
            // TESTING 'getHostnameByAddress' FUNCTION:
            //   The main concerns are:
            //   1. That the IPv4 address maps to the same hostname as was used
            //      to get its value; in addition, that 'errorCode' is
            //      unchanged.
            //   2. That 'errorCode' is loaded with the correct value on error
            //      and 'hostname' is unchanged
            //
            // Plan:
            //   Create a test table with a list of valid host names and their
            //   IPv4 addresses.  Call 'getHostnameByAddress' to retrieve the
            //   corresponding canonical hostname.  After stripping off the
            //   domain portion (XXX.com, XXX.org, XXX.net, etc.), verify the
            //   canonical hostname matches the name supplied.  Next, supply an
            //   IP address without a host mapping, and verify that an error
            //   value is returned.
            //
            // Testing
            //   int getHostnameByAddress(hostname, address, errorCode);
            // ----------------------------------------------------------------
            if (verbose) cout << "\nTesting getHostnameByAddress"
                              << "\n============================" << endl;

            enum {
                UNUSED_VALUE = (int)0xDEADBEEF
            };
            bsl::string UNUSED_STRING_VALUE("Value not yet overwritten.");

            int errCode = UNUSED_VALUE;

            if (verbose) cout << "\tTest valid IP addresses." << endl;

        #ifdef BSLS_PLATFORM_OS_UNIX
            const char *applix[]  = { "86.0.0.43",
                                      "87.0.0.36",
                                      "87.0.0.131",
                                      "86.0.0.32",
                                      0
                                    };
            const char *soros[] = { "202.217.132.211",
                                    "202.217.132.212",
                                    "202.217.132.213",
                                    0
                                  };
            const char *fft[]   = { "192.168.218.1",
                                    "192.168.219.1",
                                    0 };
            const char *yusen[] = { "192.168.79.34",
                                    "192.168.79.65",
                                    0
                                  };
        #else // defined BSLS_PLATFORM_OS_WINDOWS
            // Cannot come up with any multi-homed resolvable hosts.

            const char* sundev31[] = { "172.17.5.196",   0 };
            const char* sundev5[]  = { "10.126.159.229", 0 };
            const char* ibm1[]     = { "172.17.5.40",    0 };
            const char* p111[]     = { "10.126.151.36",  0 };
        #endif

            struct {
                int          d_lineNum;    // line number
                const char  *d_hostName;   // expected hostname string
                int         *d_errCode_p;  // NULL or valid address
                const char **d_expAddr;    // IPv4 address string array
            } DATA[] =
            //  line  hostname              errorCode   d_expAddr
            //  ----  --------              ---------   ---------
            {
        #ifdef BSLS_PLATFORM_OS_UNIX
                { L_, "applix",             &errCode,      applix },
                { L_, "applix",                    0,      applix },
                { L_, "soros.wk1",          &errCode,       soros },
                { L_, "soros.wk1",                 0,       soros },
                { L_, "fft-corp-rtr1",      &errCode,         fft },
                { L_, "yusen-rtr2",         &errCode,       yusen },
        #elif defined BSLS_PLATFORM_OS_WINDOWS
// It is hard to rely on Reverse DNS configuration on Windows
//                { L_, "sundev31",           &errCode,    sundev31 },
//                { L_, "sundev31",                  0,    sundev31 },
                { L_, "sundev5",            &errCode,     sundev5 },
                { L_, "sundev5",                   0,     sundev5 },
//                { L_, "ibm1",               &errCode,        ibm1 },
//                { L_, "p111",               &errCode,        p111 },
        #else

        #error getHostnameByAddress does not handle current platform type!

        #endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const char *HOST_NAME = DATA[i].d_hostName;
                      int  *errorPtr  = DATA[i].d_errCode_p;
                const int   EXP_RET   = SUCCESS;

                for (int j = 0; DATA[i].d_expAddr[j]; ++j) {
                    const char *EXP_ADDR  = DATA[i].d_expAddr[j];
                    if (veryVerbose) {
                        P_(LINE); P_(HOST_NAME); P(EXP_ADDR);
                    }

                    bsl::string result(UNUSED_STRING_VALUE);

                    btlso::IPv4Address hostAddress(EXP_ADDR, 0);
                    const btlso::IPv4Address& HOST_ADDRESS = hostAddress;

                    if (errorPtr) {
                        *errorPtr = UNUSED_VALUE;
                    }

                    int retCode = Obj::getHostnameByAddress(&result,
                                                            HOST_ADDRESS,
                                                            errorPtr);

                    if (verbose) {
                        P_(i); P(result);
                    }
                    LOOP3_ASSERT(LINE, retCode, EXP_RET, EXP_RET == retCode);

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr,
                                     UNUSED_VALUE == *errorPtr);
                    }

                    // Depending on how systems are administered the same
                    // ip address my result in either a partially or fully
                    // qualified domain name.  So a substring match is
                    // used to verify the returned response.

                    int len = strlen(HOST_NAME);
                    LOOP3_ASSERT(LINE, HOST_NAME, result,
                                 HOST_NAME == result.substr(0, len));
                }
            }

            {
                if (verbose) cout << "\tTest IP address 1.2.3.4:1000." << endl;

                btlso::IPv4Address UNUSED_ADDRESS("1.2.3.4", 1000);
                bsl::string result(UNUSED_STRING_VALUE);

                btlso::IPv4Address hostAddress(UNUSED_ADDRESS);
                const btlso::IPv4Address& HOST_ADDRESS = hostAddress;

                int retCode;
                retCode = Obj::getHostnameByAddress(&result,
                                                    HOST_ADDRESS,
                                                    NULL);
                LOOP_ASSERT(retCode, 0 != retCode);
                LOOP_ASSERT(result,  UNUSED_STRING_VALUE == result);

                errCode = UNUSED_VALUE;
                retCode = Obj::getHostnameByAddress(&result,
                                                    HOST_ADDRESS,
                                                    &errCode);
                LOOP_ASSERT(retCode, 0 != retCode);
                LOOP_ASSERT(result,  UNUSED_STRING_VALUE == result);
                ASSERT(errCode);
            }
        } break;
        case 3: {
            // ----------------------------------------------------------------
            // TESTING 'getServicePort'
            //   The main concerns are:
            //   1.  When given a correct 'servicename', the right return code
            //   and service number is returned; also, the 'errorCode' is
            //   unchanged.
            //   2.  When given a wrong 'servicename', the right return code
            //   and optionally 'errorCode' is set correctly.
            //
            // Plan:
            //   Create a test table with a list of valid or invalid host
            //   names.  Call 'getServicePort' to retrieve the corresponding
            //   IPv4 address.  Test the return address is as the expected in
            //   the test table for a valid host name, or test the optionally
            //   error code for an invalid name.
            //
            // Testing
            //   int getServicePort(btlso::IPv4Address *result,
            //                      const char        *servicename,
            //                      const char        *protocol,
            //                      int               *errorCode = 0);
            // ----------------------------------------------------------------
            if (verbose) cout << "\nTesting getServicePort"
                              << "\n======================" << endl;

            enum {
                UNUSED_VALUE = (int)0xDEADBEEF
            };

            int errCode = UNUSED_VALUE;

            struct {
                int         d_lineNum;      // line number
                const char *d_serviceName;  // service name string
                const char *d_protocol;     // protocol level
                int        *d_errCode_p;    // NULL or valid address
                short       d_expPort;      // expected port value
                int         d_expRet;       // expected return value
            } DATA[] =
            //  line   serviceName   protocol   errorCode  expPort   expRet
            //  ----   -----------   --------   ---------  -------   ------
            {
            #ifdef BSLS_PLATFORM_OS_UNIX
                { L_,  "jlu123",     "tcp",     &errCode,       0,     FAIL },
                { L_,  "jlu123",     "tcp",            0,       0,     FAIL },
                { L_,  "echo",       "tcp",     &errCode,       7,  SUCCESS },
                { L_,  "echo",       "udp",            0,       7,  SUCCESS },
              #if defined(BSLS_PLATFORM_OS_FREEBSD)
                { L_,  "echo",           0,            0,       4,  SUCCESS },
              #else
                { L_,  "echo",           0,            0,       7,  SUCCESS },
              #endif
                { L_,  "discard",    "tcp",     &errCode,       9,  SUCCESS },
                { L_,  "discard",    "udp",     &errCode,       9,  SUCCESS },
                { L_,  "discard",        0,            0,       9,  SUCCESS },
                { L_,  "jlu_wrong",  "tcp",     &errCode,       0,     FAIL },
                { L_,  "jlu_wrong",  "udp",            0,       0,     FAIL },
                { L_,  "time",       "tcp",     &errCode,      37,  SUCCESS },
                { L_,  "biff",       "tcp",     &errCode,       0,     FAIL },
                { L_,  "biff",       "udp",     &errCode,     512,  SUCCESS },
                { L_,  "syslog",     "tcp",     &errCode,       0,     FAIL },
                { L_,  "syslog",     "udp",     &errCode,     514,  SUCCESS },
                { L_,  "ingreslock", "tcp",     &errCode,    1524,  SUCCESS },
              #if !defined(BSLS_PLATFORM_OS_LINUX) \
               && !defined(BSLS_PLATFORM_OS_FREEBSD)
                // These ports are not in the FreeBSD/Linux default
                // /etc/services.
                { L_,  "whois",      "tcp",     &errCode,      43,  SUCCESS },
                { L_,  "sapdp99",    "tcp",     &errCode,    3299,  SUCCESS },
                { L_,  "blp-ctrb",   "tcp",     &errCode,    3649,  SUCCESS },
              #endif
            #elif defined BSLS_PLATFORM_OS_WINDOWS
                { L_,  "jlu123",     "tcp",     &errCode,       0,     FAIL },
                { L_,  "jlu123",     "tcp",            0,       0,     FAIL },
                { L_,  "echo",       "tcp",     &errCode,    1792,  SUCCESS },
                { L_,  "echo",       "udp",            0,    1792,  SUCCESS },
                { L_,  "time",       "tcp",     &errCode,    9472,  SUCCESS },
                { L_,  "time",       "tcp",            0,    9472,  SUCCESS },
                { L_,  "whois",      "tcp",     &errCode,   11008,  SUCCESS },
                { L_,  "biff",       "udp",     &errCode,       2,  SUCCESS },
                { L_,  "syslog",     "udp",     &errCode,     514,  SUCCESS },
                { L_,  "ingreslock", "tcp",     &errCode,   62469,  SUCCESS },
//              { L_,  "sapdp99",    "tcp",     &errCode,   58124,  SUCCESS },
//              { L_,  "blp-ctrb",   "tcp",     &errCode,   16654,  SUCCESS },
            #endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *SERVICE_NAME = DATA[i].d_serviceName;
                const char *PROTOCOL     = DATA[i].d_protocol;
                      int  *errorPtr     = DATA[i].d_errCode_p;
                const int   EXP_PORT     = DATA[i].d_expPort;
                const int   EXP_RET      = DATA[i].d_expRet;

                if (errorPtr) {
                    *errorPtr = UNUSED_VALUE;
                }

                btlso::IPv4Address expResult;
                expResult.setPortNumber(EXP_PORT);
                const btlso::IPv4Address& EXP = expResult;

                btlso::IPv4Address result;
                int retCode = Obj::getServicePort(&result,
                                                  SERVICE_NAME,
                                                  PROTOCOL,
                                                  errorPtr);

                LOOP2_ASSERT(LINE, retCode, EXP_RET == retCode);
                if (FAIL == EXP_RET) {
                    // For entries w/ invalid name.
#ifdef BSLS_PLATFORM_OS_WINDOWS
                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                    }
#endif
                }
                else {
                    // Retrieved successfully.

                    LOOP2_ASSERT(LINE, result, EXP == result);
                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr,
                                     UNUSED_VALUE == *errorPtr);
                    }
                }
            }

        } break;
        case 2: {
            // ----------------------------------------------------------------
            // TESTING 'getAddresses' FUNCTION:
            //   The main concerns are:
            //   1.  When given a correct 'hostname', the right return code and
            //   IPv4 address is returned;  in addition, 'errorCode' is
            //   unchanged.
            //   2.  When given a wrong 'hostname', the right return code
            //   and optionally 'errorCode' is set correctly.
            //
            // Plan:
            //   Create a test table with a list of valid or invalid host
            //   names.  Call 'getAddresses' to retrieve the corresponding
            //   IPv4 addresses.  Test the return address is as the expected
            //   in the test table for a valid host name, or test the
            //   optionally error code for an invalid name.
            //
            // Testing
            //   int getAddresses(vector<btlso::IPv4Address> *, host, code);
            // ----------------------------------------------------------------
            if (verbose) cout << "\nTesting getAddress"
                              << "\n==================" << endl;

            enum {
                UNUSED_VALUE = (int)0xDEADBEEF
            };
            btlso::IPv4Address UNUSED_ADDRESS("1.2.3.4", 1000);

            int errCode = UNUSED_VALUE;

            const char *me[]      = { "127.0.0.1", 0 };
        #ifdef BSLS_PLATFORM_OS_UNIX
            // Cannot come up with any multi-homed resolvable hosts.

            const char *applix[]  = { "86.0.0.32",
                                  #if !defined(BSLS_PLATFORM_OS_LINUX) \
                                   && !defined(BSLS_PLATFORM_OS_FREEBSD)
                                      "86.0.0.43",
                                      "87.0.0.36",
                                      "87.0.0.131",
                                  #endif
                                      0
                                    };
            const char *soros[] = { "202.217.132.211",
                                #if !defined(BSLS_PLATFORM_OS_LINUX) \
                                 && !defined(BSLS_PLATFORM_OS_FREEBSD)
                                    "202.217.132.212",
                                    "202.217.132.213",
                                #endif
                                    0
                                  };
            const char *fft[]   = { "192.168.218.1",
                                #if defined(BSLS_PLATFORM_OS_AIX)            \
                                 || (defined(BSLS_PLATFORM_OS_SOLARIS) &&    \
                                       (BSLS_PLATFORM_OS_VER_MAJOR >= 10 ||  \
                                        defined(BSLS_PLATFORM_CMP_GNU)   ||  \
                                        defined(BSLS_PLATFORM_CMP_CLANG)))
                                    "192.168.219.1",
                                #endif
                                    0
                                  };
            const char *yusen[] = { "192.168.79.34",
                                #if defined(BSLS_PLATFORM_OS_AIX)        \
                                 || defined(BSLS_PLATFORM_OS_SOLARIS)    \
                                 || defined(BSLS_PLATFORM_OS_HPUX)
                                    "192.168.79.65",
                                #endif
                                    0
                                  };
        #else // defined BSLS_PLATFORM_OS_WINDOWS
            // Cannot come up with any multi-homed resolvable hosts.

            const char* sundev1[] = { "172.17.1.20",    0 };
            const char* sundev5[] = { "10.126.159.229", 0 };
            const char* ibm1[]    = { "172.17.5.40",    0 };
            const char* p111[]    = { "10.126.161.151", 0 };
        #endif

            struct {
                int          d_lineNum;   // line number
                const char  *d_hostName;  // hostname string
                int         *d_errCode_p; // NULL or valid address
                const char **d_expAddr;   // expected IPv4 address string array
                int          d_expRet;    // expected return value
            } DATA[] =
            //  line  hostname              errorCode   expAddr     expRet
            //  ----  --------              ---------   -------     ------
            {
                { L_, "localhost",          &errCode,      me,     SUCCESS },
                { L_, "localhost",                 0,      me,     SUCCESS },
        #ifdef BSLS_PLATFORM_OS_UNIX
                { L_, "jlu1",               &errCode,        0,       FAIL },
                { L_, "jlu1",                      0,        0,       FAIL },
                { L_, "applix",             &errCode,   applix,    SUCCESS },
                { L_, "applix",                    0,   applix,    SUCCESS },
                { L_, "soros.wk1",          &errCode,    soros,    SUCCESS },
                { L_, "soros.wk1",                 0,    soros,    SUCCESS },
                { L_, "jlu_wrong",          &errCode,        0,       FAIL },
                { L_, "jlu_wrong",                 0,        0,       FAIL },
                { L_, "fft-corp-rtr1",      &errCode,      fft,    SUCCESS },
                { L_, "yusen-rtr2",         &errCode,    yusen,    SUCCESS },
        #elif defined BSLS_PLATFORM_OS_WINDOWS
                { L_, "sundev0",            &errCode,        0,       FAIL },
                { L_, "sundev1",            &errCode,  sundev1,    SUCCESS },
                { L_, "sundev1",                   0,  sundev1,    SUCCESS },
                { L_, "sundev5",            &errCode,  sundev5,    SUCCESS },
                { L_, "sundev5",                   0,  sundev5,    SUCCESS },
                { L_, "ibm1",               &errCode,     ibm1,    SUCCESS },
                { L_, "p111",               &errCode,     p111,    SUCCESS }
        #else

        #error getHostnameByAddress does not handle current platform type!

        #endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE      = DATA[i].d_lineNum;
                const char  *HOST_NAME = DATA[i].d_hostName;
                      int   *errorPtr  = DATA[i].d_errCode_p;
                const char **EXP_ADDR  = DATA[i].d_expAddr;
                const int    EXP_RET   = DATA[i].d_expRet;

                bsl::vector<btlso::IPv4Address> resultArray(16,
                                                           UNUSED_ADDRESS,
                                                           &testAllocator);
                ASSERT(16 == resultArray.size());

                if (veryVerbose) {
                    P_(LINE); P(HOST_NAME);
                }

                int retCode = Obj::getAddresses(&resultArray,
                                                HOST_NAME,
                                                errorPtr);

                if (verbose) {
                    P_(LINE);
                    copy(resultArray.begin(), resultArray.end(),
                         ostream_iterator<btlso::IPv4Address>(cout, " "));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, EXP_RET == retCode);
                if (FAIL == EXP_RET) {
                    // Entries with invalid name.

                    bsl::vector<btlso::IPv4Address> expResultArray;
                    const bsl::vector<btlso::IPv4Address>& EXP_RESULT_ARRAY =
                                                                expResultArray;

                    LOOP_ASSERT(LINE, EXP_RESULT_ARRAY == resultArray);
                    if (EXP_RESULT_ARRAY != resultArray) {
                        cout << "Host: " << HOST_NAME << "    Expected: ";
                        copy(EXP_RESULT_ARRAY.begin(), EXP_RESULT_ARRAY.end(),
                              ostream_iterator<btlso::IPv4Address>(cout, " "));
                        cout << endl;
                        cout << "Got instead: ";
                        copy(resultArray.begin(), resultArray.end(),
                              ostream_iterator<btlso::IPv4Address>(cout, " "));
                        cout << endl;

                    }
                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                    }
                }
                else {
                    // Retrieved successfully.

                    int j, k, length = resultArray.size();
                    for (j = 0; j < length; ++j) {
                        for (k = 0; EXP_ADDR[k]; ++k) {
                            if (btlso::IPv4Address(EXP_ADDR[k], 0) ==
                                resultArray[j]) {
                                break;
                            }
                        }
                        LOOP4_ASSERT(LINE, j, length, resultArray[j],
                                     EXP_ADDR[k] &&
                                     "Too many IP addresses returned");
                    }
                    for (j = 0; EXP_ADDR[j]; ++j) {
                        for (k = 0; k < length; ++k) {
                            if (btlso::IPv4Address(EXP_ADDR[j], 0) ==
                                resultArray[k]) {
                                break;
                            }
                        }
                        LOOP4_ASSERT(LINE, j, length, EXP_ADDR[j],
                                     k < length &&
                                     "Too few IP addresses returned");
                    }
                }
            }
        } break;
        case 1: {
            // ----------------------------------------------------------------
            // TESTING 'getAddress'
            //   The main concerns are:
            //   1.  When given a correct 'hostname', the right return code and
            //   IPv4 address is returned; in addition, 'result's port number
            //   and 'errorCode' are unchanged.
            //   2.  When given a wrong 'hostname', the right return code
            //   and optionally 'errorCode' is set correctly, and 'result' is
            //   unchanged.
            //
            // Plan:
            //   Create a test table with a list of valid or invalid host
            //   names.  Call 'getAddress' to retrieve the corresponding
            //   IPv4 address.  Test the return address is as the expected in
            //   the test table for a valid host name and make sure that the
            //   'errorCode' is unchanged, or test the optionally specified
            //   error code for an invalid name.
            //
            // Note:
            //   Host ip addresses may change from time to time.  To determine
            //   the current ip address for a given machine, say
            //   'host <hostname>'.
            //
            // Testing
            //   int getAddress(btlso::IPv4Address *result,
            //                  const char        *hostname,
            //                  int               *errorCode = 0);
            // ----------------------------------------------------------------
            if (verbose)
                cout << "\nTesting getAddress and getAddressDefault"
                     << "\n========================================" << endl;

            enum {
                UNUSED_VALUE = (int)0xDEADBEEF,
                UNUSED_PORT  = (int)0xBAD
            };

            int errCode = UNUSED_VALUE;

            struct {
                int         d_lineNum;    // line number
                const char *d_hostName;   // hostname string
                int        *d_errCode_p;  // NULL or valid address
                const char *d_expAddr;    // expected IPv4 address string
                int         d_expRet;     // the expected return value
            } DATA[] =
            //  line   hostName      errorCode  expAddr           expRet
            //  ----   --------      ---------  --------------    ------
            {
            #if defined(BSLS_PLATFORM_OS_UNIX)
                { L_,  "jlu1",       &errCode,  0,                FAIL    },
                { L_,  "jlu1",              0,  0,                FAIL    },
                { L_,  "sdv1",       &errCode,  "172.17.1.20",    SUCCESS },
                { L_,  "sdv1",              0,  "172.17.1.20",    SUCCESS },
                { L_,  "sdv3",       &errCode,  "10.122.130.92",  SUCCESS },
                { L_,  "sdv3",              0,  "10.122.130.92",  SUCCESS },
                { L_,  "jlu_wrong",  &errCode,  0,                FAIL    },
                { L_,  "jlu_wrong",         0,  0,                FAIL    },
                { L_,  "n270",       &errCode,  "10.126.45.63",   SUCCESS },
                { L_,  "n299",       &errCode,  "10.126.48.90",   SUCCESS },
            #elif defined(BSLS_PLATFORM_OS_WINDOWS)
                { L_,  "bny14",      &errCode,  0,                FAIL    },
                { L_,  "bny14",             0,  0,                FAIL    },
                { L_,  "330west",    &errCode,  0,                FAIL    },
                { L_,  "330west",           0,  0,                FAIL    },
                { L_,  "Dgm3w1",     &errCode,  "160.43.7.102",   SUCCESS },
                { L_,  "Dgm3w1",            0,  "160.43.7.102",   SUCCESS },
                { L_,  "Dgm3w10",    &errCode,  "160.43.7.13",    SUCCESS },
                { L_,  "Dgm3w10",           0,  "160.43.7.13",    SUCCESS },
            #endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const char *HOST_NAME = DATA[i].d_hostName;
                      int  *errorPtr  = DATA[i].d_errCode_p;
                const char *EXP_ADDR  = DATA[i].d_expAddr;
                const int   EXP_RET   = DATA[i].d_expRet;

                btlso::IPv4Address expResult(UNUSED_VALUE, UNUSED_PORT);
                if (EXP_ADDR) {
                    expResult.setIpAddress(EXP_ADDR);
                }
                const btlso::IPv4Address& EXP = expResult;

                if (errorPtr) {
                    *errorPtr = UNUSED_VALUE;
                }

                btlso::IPv4Address result(UNUSED_VALUE, UNUSED_PORT);
                int retCode = Obj::getAddress(&result,
                                              HOST_NAME,
                                              errorPtr);

                if (veryVerbose) {
                    cout << "getAddress: ";
                    P_(LINE); P_(HOST_NAME); P_(EXP); P(EXP_RET);
                    P_(retCode); P_(result);
                    if (errorPtr) {
                        P(*errorPtr);
                    }
                    else {
                        cout << endl;
                    }
                }
                LOOP2_ASSERT(LINE, retCode, EXP_RET == retCode);
                LOOP3_ASSERT(LINE, result, EXP, EXP == result);

                if (FAIL == EXP_RET) {
                    // Entries with invalid name.

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                    }
                }
                else {
                    // Retrieved successfully.

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr,
                                     UNUSED_VALUE == *errorPtr);
                    }
                }

                // Reset result and *errorPtr.

                if (errorPtr) {
                    *errorPtr = UNUSED_VALUE;
                }

                result = btlso::IPv4Address(UNUSED_VALUE, UNUSED_PORT);
                retCode = Obj::getAddressDefault(&result,
                                                 HOST_NAME,
                                                 errorPtr);

                if (veryVerbose) {
                    cout << "getAddressDefault: ";
                    P_(LINE); P_(HOST_NAME); P_(EXP); P(EXP_RET);
                    P_(retCode); P_(result);
                    if (errorPtr) {
                        P(*errorPtr);
                    }
                    else {
                        cout << endl;
                    }
                }
                LOOP2_ASSERT(LINE, retCode, EXP_RET == retCode);
                LOOP3_ASSERT(LINE, result, EXP, EXP == result);

                if (FAIL == EXP_RET) {
                    // Entries with invalid name.

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr, *errorPtr);
                    }
                }
                else {
                    // Retrieved successfully.

                    if (errorPtr) {
                        LOOP2_ASSERT(LINE, *errorPtr,
                                     UNUSED_VALUE == *errorPtr);
                    }
                }

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
