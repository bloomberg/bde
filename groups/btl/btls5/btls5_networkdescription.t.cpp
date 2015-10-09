// btls5_networkdescription.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls5_networkdescription.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component implements a value-semantic description of a network of
// SOCKS5 proxies.  The component is tested by exercising its setters and using
// the getters to compare to expected values.
//
// This particular attribute class does not provide a public default/value
// constructor capable of creating an object in any state relevant for testing.
// The only way to create a transition object is by calling 'addProxy' with the
// attribute values required for the proxy and accessing the proxy via the
// 'beginLevel' or 'endLevel' methods.  We will implement a 'gg' function for
// creating a proxy description having any value.  We will then use this
// generator function to create a single proxy and test its various properties.
//
// We will then follow a subset of our standard 10-case approach to testing
// value-semantic types.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btls5::NetworkDescription(bslma::Allocator *allocator = 0);
// [ 7] btls5::NetworkDescription(original, bslma::Allocator *allocator = 0);
// [ 2] ~btls5::NetworkDescription();
//
// MANIPULATORS
// [ 9] btls5::NetworkDescription& operator=(rhs);
// [11] bsl::size_t addProxy(level, addr);
// [11] bsl::size_t addProxy(level, addr, credentials);
// [11] void setCredentials(level, order, credentials);
// [ 8] void swap(NetworkDescription& other);
//
// ACCESSORS
// [ 4] bsl::size_t numLevels() const;
// [ 4] bsl::size_t numProxies(bsl::size_t level) const;
// [ 4] ProxyIterator beginLevel(bsl::size_t level) const;
// [ 4] ProxyIterator endLevel(bsl::size_t level) const;
// [ 4] bslma::Allocator* allocator() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
// [ 5] bsl::ostream& operator<<(stream, object);
//
// FREE FUNCTIONS
// [ 8] void swap(NetworkDescription& a, NetworkDescription& b);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ 2] CONCERN: All memory allocation is from the object's allocator.

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
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef btls5::NetworkDescription Obj;
typedef btls5::Credentials        Credentials;
typedef btlso::Endpoint           Endpoint;
typedef btls5::ProxyDescription   ProxyDescription;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

// Attribute values for the local time descriptor

// 'D' values: These are the default constructed values.

const char HOST_D[]   = "";
const char NAME_D[]   = "";
const char PWD_D[]    = "";
const int  PORT_D     = 0;

// 'A' values: Should cause memory allocation if possible.

const char HOST_A[]   = "a_" SUFFICIENTLY_LONG_STRING;
const char NAME_A[]   = "b_" SUFFICIENTLY_LONG_STRING;
const char PWD_A[]    = "c_" SUFFICIENTLY_LONG_STRING;
const int  PORT_A     = 65535;

// 'B' values: Should NOT cause allocation (use alternate string type).

const char HOST_B[]   = "hostname";
const char NAME_B[]   = "username";
const char PWD_B[]    = "password";
const int  PORT_B     = 255;


static const Endpoint ADDRS[] = {
    Endpoint(HOST_D, PORT_D),
    Endpoint(HOST_A, PORT_A),
    Endpoint(HOST_B, PORT_B)
};

static const Credentials CREDS[] = {
    Credentials(NAME_D, PWD_D),
    Credentials(NAME_A, PWD_A),
    Credentials(NAME_B, PWD_B)
};

struct DefaultDataRow {
    int         d_line;
    const char *d_spec_p;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //line  spec
    //----  ----
    { L_,   ""        },

    { L_,   "0nD"      },
    { L_,   "0xA"      },
    { L_,   "0yB"      },

    { L_,   "1nD"      },
    { L_,   "1xA"      },
    { L_,   "1yB"      },

    { L_,   "2nD"      },
    { L_,   "2xA"      },
    { L_,   "2yB"      },

    { L_,   "0nD1xA"    },
    { L_,   "0nA1xD"    },

    { L_,   "0nD0xA"    },
    { L_,   "1nD1xA"    },
    { L_,   "2nD2xA"    },

    { L_,   "0nA1xD2yA"  },
    { L_,   "0yA1xD2nD"  },

    { L_,   "0nA0xD0yA"  },
    { L_,   "1yA1xD1nD"  },
    { L_,   "2yA2xD2nD"  },

    // duplicates

    { L_,   "0nD1nA"    },
    { L_,   "0xD1xD"    },

    { L_,   "0nD0nA"    },
    { L_,   "1xD1xD"    },

    { L_,   "0xD1xA2xB"  },
    { L_,   "0xD1nA2xB"  },
    { L_,   "0xD1nA2xD"  },

    { L_,   "0xD1nA2yB3xA4nB5yD"  },
    { L_,   "1xD1nA1yB1xA2nB2yD"  },
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;


// ============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void loadProxies(vector<vector<ProxyDescription> > *proxies, const char *spec)
{
    const int NUM_ELEMS = ((int) strlen(spec)) / 3;
    for (int i = 0; i < NUM_ELEMS; ++i) {
        const char LS = spec[i * 3];
        const char ES = spec[i * 3 + 1];
        const char CS = spec[i * 3 + 2];

        const int  LI = LS - '0';

        const int  TI = 'n' == ES ? 0
            : 'x' == ES ? 1
            :             2;

        const int  DI = 'D' == CS ? 0
            : 'A' == CS ? 1
            :             2;

        const Endpoint&    E = ADDRS[TI];
        const Credentials& C = CREDS[DI];

        proxies->resize(LI + 1);
        (*proxies)[LI].push_back(ProxyDescription(E, C));
    }
}

//=============================================================================
//            GENERATOR FUNCTIONS 'g', 'gg' and 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the zone info object according to a custom language.  The
// given 'spec' contains the data that will be parsed and called on the primary
// manipulator ('addProxy') to bring the object into any state suitable for
// testing.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>        ::= <EMPTY> | <LIST>
//
// <EMPTY>       ::=
//
// <LIST>        ::= <PROXY> | <PROXY><LIST>
//
// <PROXY>       ::= <ENDPOINT><CREDENTIALS>
//
// <ENDPOINT>    ::= 'n' | 'x' | 'y'
//
// <CREDENTIALS> ::= 'D' | 'A' | 'B'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// ----------------------------------------------------------------------------

void loadEndpoint(Endpoint *time, char spec)
{
    LOOP_ASSERT(spec, strchr("nxy", spec));

    const int index = 'n' == spec ? 0
                    : 'x' == spec ? 1
                    :               2;

    *time = ADDRS[index];
}

void loadCredentials(Credentials *d, char spec)
{
    LOOP_ASSERT(spec, strchr("DAB", spec));

    const int index = 'D' == spec ? 0
                    : 'A' == spec ? 1
                    :               2;

    *d = CREDS[index];
}

int ggg(Obj *object, const char *spec, bslma::Allocator *a = 0)
    // Configure the specified 'object' according to the specified 'spec' using
    // the primary manipulator function.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };

    enum { MAX_SPEC_LEN = 200 };

    ASSERT(bsl::strlen(spec) < MAX_SPEC_LEN);

    char cleanSpec[MAX_SPEC_LEN];  // spec after removing all white spaces

    int i = 0;
    for (int j = 0; spec[j]; ++j) {
        if (!isspace(spec[j])) {
            cleanSpec[i++] = spec[j];
        }
    }
    cleanSpec[i] = '\0';

    const char *p = cleanSpec;

    while (*p) {
        int level = *p - '0';
        ++p;
        Endpoint e(a);
        loadEndpoint(&e, *p);
        ++p;
        Credentials c(a);
        loadCredentials(&c, *p);
        ++p;
        object->addProxy(level, e, c);
    }

    return SUCCESS;
}

Obj& gg(Obj *object, const char *spec, bslma::Allocator *a = 0)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec, a) < 0);
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object;
    return gg(&object, spec);
}


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Example 1: Describe a Two-level Proxy Network
///- - - - - - - - - - - - - - - - - - - - - - -
// Let's define a network of proxies necessary to reach the Internet from a
// corporate Intranet.  The Internet can be reached through 2 levels: a
// corporate proxy, which then has a connection to a regional proxy, which
// finally has direct access to the Internet.  Each proxy level has two proxies
// for redundancy.
//
// First, we declare an empty network:
//..
    btls5::NetworkDescription network;
    ASSERT(0 == network.numLevels());
//..
// Then, we add the addresses of corporate proxies as level 0 (directly
// reachable from our host):
//..
    int proxyNumber;  // ordinal number of proxy within a level

    btlso::Endpoint corp1("proxy1.corp.com", 1081);
    btlso::Endpoint corp2("proxy2.corp.com", 1082);
    proxyNumber = network.addProxy(0, corp1);
    ASSERT(0 == proxyNumber);

    proxyNumber = network.addProxy(0, corp2);
    ASSERT(1 == proxyNumber);
    ASSERT(1 == network.numLevels());
//..
// Now, we add the regional proxies that we can connect to only through the
// corporate proxies.  There are two regional proxies, for redundancy.
//..
    btlso::Endpoint region1("proxy1.region.com", 1091);
    btlso::Endpoint region2("proxy2.region.com", 1092);
    proxyNumber = network.addProxy(1, region1);
    ASSERT(0 == proxyNumber);

    proxyNumber = network.addProxy(1, region2);
    ASSERT(1 == proxyNumber);
    ASSERT(2 == network.numLevels());
//..
// Finally, we have a fully defined network that we can use for connection
// using 'btls5::NetworkDescription'.

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'addProxy' and 'setCredentials'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bsl::size_t addProxy(level, addr);
        //   bsl::size_t addProxy(level, addr, credentials);
        //   void setCredentials(level, order, credentials);
        // --------------------------------------------------------------------

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        static const struct {
            int          d_line;
            const char  *d_spec_p;

            int          d_level;
            Endpoint     d_address;
            Credentials  d_credentials;
        }DATA[] = {
            //line  spec                level       address     creds
            //----  ----                -----       -------     -----
            { L_,   "",                    0,      ADDRS[0],   CREDS[0]    },

            { L_,   "0nD",                 0,      ADDRS[1],   CREDS[2]    },
            { L_,   "0xA",                 1,      ADDRS[2],   CREDS[1]    },
            { L_,   "0yB",                 2,      ADDRS[0],   CREDS[0]    },

            { L_,   "1nD",                 0,      ADDRS[2],   CREDS[0]    },
            { L_,   "1xA",                 1,      ADDRS[1],   CREDS[1]    },
            { L_,   "1yB",                 2,      ADDRS[0],   CREDS[2]    },

            { L_,   "2nD",                 0,      ADDRS[0],   CREDS[2]    },
            { L_,   "2xA",                 1,      ADDRS[1],   CREDS[1]    },
            { L_,   "2yB",                 2,      ADDRS[2],   CREDS[0]    },

            { L_,   "0nD1xA",              2,      ADDRS[0],   CREDS[2]    },
            { L_,   "0nA1xD",              3,      ADDRS[2],   CREDS[0]    },

            { L_,   "0nD0xA",              1,      ADDRS[1],   CREDS[1]    },
            { L_,   "1nD1xA",              2,      ADDRS[0],   CREDS[0]    },
            { L_,   "2nD2xA",              3,      ADDRS[2],   CREDS[2]    },

            { L_,   "0nA1xD2yA",           0,      ADDRS[2],   CREDS[1]    },
            { L_,   "0yA1xD2nD",           1,      ADDRS[1],   CREDS[2]    },

            { L_,   "0nA0xD0yA",           1,      ADDRS[0],   CREDS[2]    },
            { L_,   "1yA1xD1nD",           2,      ADDRS[1],   CREDS[1]    },
            { L_,   "2yA2xD2nD",           3,      ADDRS[2],   CREDS[0]    },

            // duplicates

            { L_,   "0nD1nA",              1,      ADDRS[1],   CREDS[1]    },
            { L_,   "0xD1xD",              2,      ADDRS[0],   CREDS[0]    },

            { L_,   "0nD0nA",              1,      ADDRS[2],   CREDS[2]    },
            { L_,   "1xD1xD",              2,      ADDRS[0],   CREDS[0]    },

            { L_,   "0xD1xA2xB",           0,      ADDRS[1],   CREDS[1]    },
            { L_,   "0xD1nA2xB",           1,      ADDRS[2],   CREDS[2]    },
            { L_,   "0xD1nA2xD",           2,      ADDRS[0],   CREDS[0]    },

            { L_,   "0xD1nA2yB3xA4nB5yD",  1,      ADDRS[1],   CREDS[2]    },
            { L_,   "1xD1nA1yB1xA2nB2yD",  3,      ADDRS[0],   CREDS[1]    },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const Credentials DEFAULT_CRED = CREDS[0];

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         LEVEL  = DATA[ti].d_level;
            const Endpoint    ADDR   = DATA[ti].d_address;
            const Credentials CRED   = DATA[ti].d_credentials;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);   const Obj& X = gg(&mX, SPEC);
            Obj mY(&oa);   const Obj& Y = gg(&mY, SPEC);

            Obj mZ(&oa);   const Obj& Z = gg(&mZ, SPEC);

            if (veryVerbose) { T_ P_(LINE) P_(X) P_(Y) P(Z) }

            LOOP2_ASSERT(X, Z, X == Z);
            LOOP2_ASSERT(Y, Z, Y == Z);

            int ord1 = mX.addProxy(LEVEL, ADDR);

            LOOP2_ASSERT(X, Z, X != Z);
            LOOP_ASSERT(ADDR, ADDR == (X.endLevel(LEVEL)  - 1)->address());

            mY.addProxy(LEVEL, ADDR, CRED);

            LOOP2_ASSERT(Y, Z, Y != Z);

            if (CRED != DEFAULT_CRED) {
                LOOP2_ASSERT(X, Y, X != Y);
            }

            mX.setCredentials(LEVEL, ord1, CRED);

            LOOP2_ASSERT(X, Y, X == Y);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSLX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator address held by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   operator=(const NetworkDescription& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj& Z = gg(&mZ, SPEC1);
            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);   const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    LOOP4_ASSERT(LINE1, LINE2, Z, X,
                                 (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                        LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);


                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);   const Obj& X = gg(&mX, SPEC1);  (void) X;

                Obj mZZ(&scratch);   const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                    LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                LOOP_ASSERT(LINE1, !oam.isInUseUp());
            }

            // Verify all object memory is released on destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address held by both objects is
        //:   unchanged.
        //:
        //: 3 Neither function allocates memory from any allocator.
        //:
        //: 4 Both functions have standard signatures and return types.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 5)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-5)
        //:
        //:     1 The value is unchanged.  (C-5)
        //:
        //:     2 The allocator address held by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', and having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator address held by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-6)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (having default attribute values); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg_SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-6)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-6)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
        //
        // Testing:
        //   void swap(NetworkDescription& other);
        //   void swap(NetworkDescription& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace btls5;

            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mW(&oa);   const Obj& W = gg(&mW, SPEC1);

            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), W, Obj() == W);
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[ti].d_spec_p;

                      Obj mX(XX, &oa);  const Obj& X = mX;

                      Obj mY(&oa);   const Obj& Y = gg(&mY, SPEC2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, YY, X, YY == X);
                    LOOP4_ASSERT(LINE1, LINE2, XX, Y, XX == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, XX, X, XX == X);
                    LOOP4_ASSERT(LINE1, LINE2, YY, Y, YY == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }
            }
        }

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            // 'A' values: Should cause memory allocation if possible.

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mX(&oa);  const Obj& X = gg(&mX, "0xA");
            const Obj XX(X, &scratch);

                  Obj mY(&oa);  const Obj& Y = gg(&mY, "1yB");
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            LOOP2_ASSERT(YY, X, YY == X);
            LOOP2_ASSERT(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVerbose) { T_ P_(X) P(Y) }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mA.swap(mZ));
            }

            if (veryVerbose) cout << "\t'swap' free function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(swap(mA, mB));
                ASSERT_SAFE_FAIL(swap(mA, mZ));
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address held by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a null allocator address explicitly, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator address that it holds is
        //:       unchanged.  (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   NetworkDescription(const NetworkDescription& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj mZ(&scratch);   const Obj& Z = gg(&mZ, SPEC);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE, CONFIG,  Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row
        }


        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj mZ(&scratch);   const Obj& Z = gg(&mZ, SPEC);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &sa);
                    LOOP3_ASSERT(LINE, Z, obj, Z == obj);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);

                LOOP3_ASSERT(LINE, &scratch, Z.allocator(),
                             &scratch == Z.allocator());
                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'NetworkDescription' EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compare
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., aliasing)
        //:
        //: 4 'false == (X != X)'  (i.e., aliasing)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects providing
        //:   only non-modifiable access).
        //:
        //:10 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:11 The return type of the equality comparison operators is 'bool'.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate structure for
        //:   the two homogeneous, free equality comparison operators defined
        //:   in this component.  (C-8..9, 11)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..8)
        //:   1 Construct a single object and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:     1 Record, in 'EXP', whether or not distinct objects created,
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value 'R1'.
        //:
        //:     3 Create an object 'Y' having the value 'R2'.
        //:
        //:     4 Verify the commutativity property and expected return value
        //:       for both '==' and '!=', while monitoring that no object
        //:       memory is ever allocated by either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const NetworkDescription& lhs, rhs);
        //   bool operator!=(const NetworkDescription& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                         << "'NetworkDescription' EQUALITY OPS" << endl
                         << "=================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace btls5;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signature and return type is standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            // Quash unused variable warning.

            operatorEq; operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   "0nD"         },
            { L_,   "0nA"         },
            { L_,   "1xA"         },
            { L_,   "1xB"         },
            { L_,   "2yB"         },

            { L_,   "0nD1xA"      },
            { L_,   "1nD2xA"      },

            { L_,   "0nD1xA2yB"   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE1 = DATA[ti].d_lineNum;
            const char *const SPEC1 = DATA[ti].d_spec_p;

            bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);

            Obj mX(&oa1);  const Obj& X = gg(&mX, SPEC1, &sa);

            // Ensure an object compares correctly with itself (alias test).

            LOOP2_ASSERT(LINE1, X,   X == X);
            LOOP2_ASSERT(LINE1, X, !(X != X));

            for (int tj = 0; tj < NUM_DATA ; ++tj) {
                const int         LINE2 = DATA[tj].d_lineNum;
                const char *const SPEC2 = DATA[tj].d_spec_p;

                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mY(&oa2);  const Obj& Y = gg(&mY, SPEC2, &sa);

                const bool EXP = ti == tj;  // expected for equality comparison

                if (veryVerbose) { T_ P_(LINE2) P_(EXP) P_(X) P(Y) }

                // Verify value, commutativity, and no memory allocation.

                bslma::TestAllocatorMonitor oam1(&oa1), oam2(&oa2);

                LOOP4_ASSERT(LINE1, LINE2, X, Y, EXP == (X == Y));
                LOOP4_ASSERT(LINE1, LINE2, X, Y, EXP == (Y == X));

                LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (Y != X));

                LOOP2_ASSERT(LINE1, LINE2, oam1.isTotalSame());
                LOOP2_ASSERT(LINE1, LINE2, oam2.isTotalSame());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return value is standard.
        //:
        //: 5 The output 'operator<<' signature and return value is standard.
        //:
        //: 6 The 'print' method returns the address of supplied stream.
        //:
        //: 7 The output 'operator<<' returns the address of supplied stream.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' method and 'operator<<' defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structure.  (C-4, 5)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 6..7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     formatting parameters ('level' and 'spacesPerLevel'), along
        //:     with the expected output:
        //:     1. { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        //:     2. { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        //:     3. { B } x { 2 }     x { 3 }         -->  1 expected output
        //:     4. { A B } x { -9 }  x { -9 }        -->  2 expected output
        //:
        //:   2 For each row in the table defined in P2-1:  (C-1..3, 6..7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are
        //:       (-9, -9) in which case 'operator<<' will be invoked instead.
        //:       (C-3, 7)
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Very the address of what is returned is that of the supplied
        //:       stream.  (C-6)
        //:
        //:     4 Compare the contents captured in P2-2 with what is expected.
        //:       (C-1..2)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, sPL = 4) const;
        //   operator<<(ostream& s, const NetworkDescription& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRINT AND OUTPUT OPERATOR" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nAssign the address of print "
                             "and output operator to a variable." << endl;
        {
            using namespace btls5;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signature and return type is standard.

            funcPtr     print      = &Obj::print;
            operatorPtr operatorOp = operator<<;

            // Quash unused variable warning.

            print; operatorOp;

        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const struct {
            int                d_line;           // source line number
            int                d_level;
            int                d_spacesPerLevel;

            const char        *d_spec_p;

            const char        *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        {
            L_,
            0,
            0,
            "0yB1nD",
            "["                             NL
            "level 0 = ["                   NL
            "["                             NL
            "address = ["                   NL
            "hostname = \"hostname\""       NL
            "port = 255"                    NL
            "]"                             NL
            "credentials = ["               NL
            "username = \"username\""       NL
            "password = \"password\""       NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
            "level 1 = ["                   NL
            "["                             NL
            "address = ["                   NL
            "hostname = \"\""               NL
            "port = 0"                      NL
            "]"                             NL
            "credentials = ["               NL
            "username = \"\""               NL
            "password = \"\""               NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
        },
        {
            L_,
            0,
            1,
            "0yB1nD",
            "["                                NL
            " level 0 = ["                     NL
            "  ["                              NL
            "   address = ["                   NL
            "    hostname = \"hostname\""      NL
            "    port = 255"                   NL
            "   ]"                             NL
            "   credentials = ["               NL
            "    username = \"username\""      NL
            "    password = \"password\""      NL
            "   ]"                             NL
            "  ]"                              NL
            " ]"                               NL
            " level 1 = ["                     NL
            "  ["                              NL
            "   address = ["                   NL
            "    hostname = \"\""              NL
            "    port = 0"                     NL
            "   ]"                             NL
            "   credentials = ["               NL
            "    username = \"\""              NL
            "    password = \"\""              NL
            "   ]"                             NL
            "  ]"                              NL
            " ]"                               NL
            "]"                                NL
        },
        {
            L_,
            0,
            -1,
            "0yB1nD",
            "["                             SP
            "level 0 = ["                   SP
            "["                             SP
            "address = ["                   SP
            "hostname = \"hostname\""       SP
            "port = 255"                    SP
            "]"                             SP
            "credentials = ["               SP
            "username = \"username\""       SP
            "password = \"password\""       SP
            "]"                             SP
            "]"                             SP
            "]"                             SP
            "level 1 = ["                   SP
            "["                             SP
            "address = ["                   SP
            "hostname = \"\""               SP
            "port = 0"                      SP
            "]"                             SP
            "credentials = ["               SP
            "username = \"\""               SP
            "password = \"\""               SP
            "]"                             SP
            "]"                             SP
            "]"                             SP
            "]"
        },
        {
            L_,
            0,
            -8,
            "0yB1nD",
            "["                                            NL
            "    level 0 = ["                              NL
            "        ["                                    NL
            "            address = ["                      NL
            "                hostname = \"hostname\""      NL
            "                port = 255"                   NL
            "            ]"                                NL
            "            credentials = ["                  NL
            "                username = \"username\""      NL
            "                password = \"password\""      NL
            "            ]"                                NL
            "        ]"                                    NL
            "    ]"                                        NL
            "    level 1 = ["                              NL
            "        ["                                    NL
            "            address = ["                      NL
            "                hostname = \"\""              NL
            "                port = 0"                     NL
            "            ]"                                NL
            "            credentials = ["                  NL
            "                username = \"\""              NL
            "                password = \"\""              NL
            "            ]"                                NL
            "        ]"                                    NL
            "    ]"                                        NL
            "]"                                            NL
        },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        {
            L_,
            3,
            0,
            "0yB1nD",
            "["                             NL
            "level 0 = ["                   NL
            "["                             NL
            "address = ["                   NL
            "hostname = \"hostname\""       NL
            "port = 255"                    NL
            "]"                             NL
            "credentials = ["               NL
            "username = \"username\""       NL
            "password = \"password\""       NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
            "level 1 = ["                   NL
            "["                             NL
            "address = ["                   NL
            "hostname = \"\""               NL
            "port = 0"                      NL
            "]"                             NL
            "credentials = ["               NL
            "username = \"\""               NL
            "password = \"\""               NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
        },
        {
            L_,
            3,
            2,
            "0yB1nD",
            "      ["                                    NL
            "        level 0 = ["                        NL
            "          ["                                NL
            "            address = ["                    NL
            "              hostname = \"hostname\""      NL
            "              port = 255"                   NL
            "            ]"                              NL
            "            credentials = ["                NL
            "              username = \"username\""      NL
            "              password = \"password\""      NL
            "            ]"                              NL
            "          ]"                                NL
            "        ]"                                  NL
            "        level 1 = ["                        NL
            "          ["                                NL
            "            address = ["                    NL
            "              hostname = \"\""              NL
            "              port = 0"                     NL
            "            ]"                              NL
            "            credentials = ["                NL
            "              username = \"\""              NL
            "              password = \"\""              NL
            "            ]"                              NL
            "          ]"                                NL
            "        ]"                                  NL
            "      ]"                                    NL
        },
        {
            L_,
            3,
            -2,
            "0yB1nD",
            "      ["                       SP
            "level 0 = ["                   SP
            "["                             SP
            "address = ["                   SP
            "hostname = \"hostname\""       SP
            "port = 255"                    SP
            "]"                             SP
            "credentials = ["               SP
            "username = \"username\""       SP
            "password = \"password\""       SP
            "]"                             SP
            "]"                             SP
            "]"                             SP
            "level 1 = ["                   SP
            "["                             SP
            "address = ["                   SP
            "hostname = \"\""               SP
            "port = 0"                      SP
            "]"                             SP
            "credentials = ["               SP
            "username = \"\""               SP
            "password = \"\""               SP
            "]"                             SP
            "]"                             SP
            "]"                             SP
            "]"
        },
        {
            L_,
            3,
            -8,
            "0yB1nD",
            "            ["                                            NL
            "                level 0 = ["                              NL
            "                    ["                                    NL
            "                        address = ["                      NL
            "                            hostname = \"hostname\""      NL
            "                            port = 255"                   NL
            "                        ]"                                NL
            "                        credentials = ["                  NL
            "                            username = \"username\""      NL
            "                            password = \"password\""      NL
            "                        ]"                                NL
            "                    ]"                                    NL
            "                ]"                                        NL
            "                level 1 = ["                              NL
            "                    ["                                    NL
            "                        address = ["                      NL
            "                            hostname = \"\""              NL
            "                            port = 0"                     NL
            "                        ]"                                NL
            "                        credentials = ["                  NL
            "                            username = \"\""              NL
            "                            password = \"\""              NL
            "                        ]"                                NL
            "                    ]"                                    NL
            "                ]"                                        NL
            "            ]"                                            NL
        },
        {
            L_,
            -3,
            0,
            "0yB1nD",
            "["                             NL
            "level 0 = ["                   NL
            "["                             NL
            "address = ["                   NL
            "hostname = \"hostname\""       NL
            "port = 255"                    NL
            "]"                             NL
            "credentials = ["               NL
            "username = \"username\""       NL
            "password = \"password\""       NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
            "level 1 = ["                   NL
            "["                             NL
            "address = ["                   NL
            "hostname = \"\""               NL
            "port = 0"                      NL
            "]"                             NL
            "credentials = ["               NL
            "username = \"\""               NL
            "password = \"\""               NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
            "]"                             NL
        },
        {
            L_,
            -3,
            2,
            "0yB1nD",
            "["                                          NL
            "        level 0 = ["                        NL
            "          ["                                NL
            "            address = ["                    NL
            "              hostname = \"hostname\""      NL
            "              port = 255"                   NL
            "            ]"                              NL
            "            credentials = ["                NL
            "              username = \"username\""      NL
            "              password = \"password\""      NL
            "            ]"                              NL
            "          ]"                                NL
            "        ]"                                  NL
            "        level 1 = ["                        NL
            "          ["                                NL
            "            address = ["                    NL
            "              hostname = \"\""              NL
            "              port = 0"                     NL
            "            ]"                              NL
            "            credentials = ["                NL
            "              username = \"\""              NL
            "              password = \"\""              NL
            "            ]"                              NL
            "          ]"                                NL
            "        ]"                                  NL
            "      ]"                                    NL
        },
        {
            L_,
            -3,
            -2,
            "0yB1nD",
            "["                             SP
            "level 0 = ["                   SP
            "["                             SP
            "address = ["                   SP
            "hostname = \"hostname\""       SP
            "port = 255"                    SP
            "]"                             SP
            "credentials = ["               SP
            "username = \"username\""       SP
            "password = \"password\""       SP
            "]"                             SP
            "]"                             SP
            "]"                             SP
            "level 1 = ["                   SP
            "["                             SP
            "address = ["                   SP
            "hostname = \"\""               SP
            "port = 0"                      SP
            "]"                             SP
            "credentials = ["               SP
            "username = \"\""               SP
            "password = \"\""               SP
            "]"                             SP
            "]"                             SP
            "]"                             SP
            "]"
        },
        {
            L_,
            -3,
            -8,
            "0yB1nD",
            "["                                                        NL
            "                level 0 = ["                              NL
            "                    ["                                    NL
            "                        address = ["                      NL
            "                            hostname = \"hostname\""      NL
            "                            port = 255"                   NL
            "                        ]"                                NL
            "                        credentials = ["                  NL
            "                            username = \"username\""      NL
            "                            password = \"password\""      NL
            "                        ]"                                NL
            "                    ]"                                    NL
            "                ]"                                        NL
            "                level 1 = ["                              NL
            "                    ["                                    NL
            "                        address = ["                      NL
            "                            hostname = \"\""              NL
            "                            port = 0"                     NL
            "                        ]"                                NL
            "                        credentials = ["                  NL
            "                            username = \"\""              NL
            "                            password = \"\""              NL
            "                        ]"                                NL
            "                    ]"                                    NL
            "                ]"                                        NL
            "            ]"                                            NL
        },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        {
            L_,
            2,
            3,
            "0nD0yB",
            "      ["                                     NL
            "         level 0 = ["                        NL
            "            ["                               NL
            "               address = ["                  NL
            "                  hostname = \"\""           NL
            "                  port = 0"                  NL
            "               ]"                            NL
            "               credentials = ["              NL
            "                  username = \"\""           NL
            "                  password = \"\""           NL
            "               ]"                            NL
            "            ]"                               NL
            "            ["                               NL
            "               address = ["                  NL
            "                  hostname = \"hostname\""   NL
            "                  port = 255"                NL
            "               ]"                            NL
            "               credentials = ["              NL
            "                  username = \"username\""   NL
            "                  password = \"password\""   NL
            "               ]"                            NL
            "            ]"                               NL
            "         ]"                                  NL
            "      ]"                                     NL
        },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }      -->  2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC   EXP
        //---- - ---  ----   ---

        {
            L_,
            -9,
            -9,
            "0yB1nD",
            "[ Proxy level 0: [ [ \"hostname\" 255 ] "
            "[ \"username\" \"password\" ] ] "
            "Proxy level 1: [ [ \"\" 0 ] [ \"\" \"\" ] ] ]"
        },

        {
            L_,
            -9,
            -9,
            "0nD0yB",
            "[ Proxy level 0: [ [ \"\" 0 ] [ \"\" \"\" ] ] "
            "[ [ \"hostname\" 255 ] [ \"username\" \"password\" ] ] ]"
        },


#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const char       *SPEC = DATA[ti].d_spec_p;
                const char       *EXP  = DATA[ti].d_expected_p;

                bslma::TestAllocator oa("supplied", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) { T_ P_(LINE) P_(X) }

                ostringstream os;

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'NetworkDescription' ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 Attributes that can allocate memory are returned by reference
        //:   providing only non-modifiable access.
        //
        // Plan:
        //
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor to create an object (having default
        //:   attribute values) holding the other test allocator created in
        //:   P-1.
        //:
        //: 3 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 4 For each salient attribute (contributing to value):  (C-1, 3..4)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   ProxyIterator beginLevel() const;
        //   ProxyIterator endLevel() const;
        //   bsl::size_t numLevels() const;
        //   bsl::size_t numProxies(level) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        Endpoint    D1 = ADDRS[0];
        Credentials D2 = CREDS[0];

        Endpoint    A1 = ADDRS[1];
        Credentials A2 = CREDS[1];

        Endpoint    B1 = ADDRS[2];
        Credentials B2 = CREDS[2];

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("specified", veryVeryVeryVerbose);

        bslma::Default::setDefaultAllocatorRaw(&da);

        {
            Obj mX(&oa);  const Obj& X = mX;   // original spec
            LOOP_ASSERT(X.numLevels(), 0 == X.numLevels());
            LOOP_ASSERT(&oa, &oa == X.allocator());

            mX.addProxy(0, A1, A2);
            LOOP_ASSERT(X.numLevels(),   1 == X.numLevels());
            LOOP_ASSERT(X.numProxies(0), 1 == X.numProxies(0));

            bslma::TestAllocatorMonitor oam1(&oa), dam1(&da);

            Obj::ProxyIterator FRONT = mX.beginLevel(0);
            LOOP2_ASSERT(A1, FRONT->address(),     A1 == FRONT->address());
            LOOP2_ASSERT(A2, FRONT->credentials(), A2 == FRONT->credentials());

            Obj::ProxyIterator begin = X.beginLevel(0);
            Obj::ProxyIterator end   = X.endLevel(0);

            ASSERT(1 == (end - begin));

            LOOP2_ASSERT(A1, begin->address(),     A1 == begin->address());
            LOOP2_ASSERT(A2, begin->credentials(), A2 == begin->credentials());

            ASSERT(oam1.isInUseSame());  ASSERT(dam1.isInUseSame());

            mX.addProxy(0, B1, B2);
            LOOP_ASSERT(X.numLevels(),   1 == X.numLevels());
            LOOP_ASSERT(X.numProxies(0), 2 == X.numProxies(0));

            bslma::TestAllocatorMonitor oam2(&oa), dam2(&da);

            FRONT = X.beginLevel(0);
            LOOP2_ASSERT(A1, FRONT->address(),     A1 == FRONT->address());
            LOOP2_ASSERT(A2, FRONT->credentials(), A2 == FRONT->credentials());

            ++FRONT;
            LOOP2_ASSERT(B1, FRONT->address(),     B1 == FRONT->address());
            LOOP2_ASSERT(B2, FRONT->credentials(), B2 == FRONT->credentials());

            begin = X.beginLevel(0);
            end   = X.endLevel(0);

            ASSERT(2 == (end - begin));

            LOOP2_ASSERT(A1, begin->address(),     A1 == begin->address());
            LOOP2_ASSERT(A2, begin->credentials(), A2 == begin->credentials());

            ++begin;

            LOOP2_ASSERT(B1, begin->address(),     B1 == begin->address());
            LOOP2_ASSERT(B2, begin->credentials(), B2 == begin->credentials());

            ASSERT(oam2.isInUseSame());  ASSERT(dam2.isInUseSame());
        }

        {
            Obj mX(&sa);  const Obj& X = mX;   // original spec
            LOOP_ASSERT(X.numLevels(), 0 == X.numLevels());
            LOOP_ASSERT(&sa, &sa == X.allocator());

            mX.addProxy(0, A1, A2);
            LOOP_ASSERT(X.numLevels(), 1 == X.numLevels());
            LOOP_ASSERT(X.numProxies(0), 1 == X.numProxies(0));

            mX.addProxy(1, D1, D2);
            LOOP_ASSERT(X.numLevels(), 2 == X.numLevels());
            LOOP_ASSERT(X.numProxies(1), 1 == X.numProxies(1));

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            Obj::ProxyIterator FRONT = mX.beginLevel(1);
            LOOP2_ASSERT(D1, FRONT->address(),     D1 == FRONT->address());
            LOOP2_ASSERT(D2, FRONT->credentials(), D2 == FRONT->credentials());

            Obj::ProxyIterator begin = X.beginLevel(1);
            Obj::ProxyIterator end   = X.endLevel(1);

            ASSERT(1 == (end - begin));

            LOOP2_ASSERT(D1, begin->address(),     D1 == begin->address());
            LOOP2_ASSERT(D2, begin->credentials(), D2 == begin->credentials());

            ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //   Ensure that the generator function is able to create an object in
        //   any state.
        //
        // Concerns:
        //   The 'gg' generator is implemented as a finite state machine (FSM).
        //   All symbols must be recognized successfully, and all possible
        //   state transitions must be verified.
        //
        // Plan:
        //   Evaluate a series of test strings of increasing complexity to
        //   validate every major state and transition in the FSM underlying
        //   gg.  Add extra tests to validate less likely transitions until
        //   they are all covered.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Generator Function 'gg'" << endl
                          << "=======================" << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout << "\nTesting generator" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const SPEC = DATA[ti].d_spec_p;

                vector<vector<ProxyDescription> > proxies;

                loadProxies(&proxies, SPEC);

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                const int NUM_LEVELS = proxies.size();
                LOOP_ASSERT(LINE,
                            NUM_LEVELS == static_cast<int>(X.numLevels()));

                for (int i = 0; i < NUM_LEVELS; ++i) {
                    const int NUM_PROXIES = proxies[i].size();
                    LOOP_ASSERT(LINE, NUM_PROXIES ==
                                              X.endLevel(i) - X.beginLevel(i));

                    Obj::ProxyIterator objIter = X.beginLevel(i);
                    Obj::ProxyIterator expIter = proxies[i].begin();

                    for (int j = 0; j < NUM_PROXIES; ++j,++objIter,++expIter) {
                        const ProxyDescription&     T = *objIter;
                        const ProxyDescription& EXP_T = *expIter;

                        LOOP3_ASSERT(LINE, EXP_T, T, EXP_T == T);
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any argument can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values.  Then
        //:     incrementally set each attribute to it's corresponding  'B'
        //:     value and verify after each manipulation that only that
        //:     attribute's value changed.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   explicit NetworkDescription(bslma::Allocator *basicAllocator = 0);
        //   ~NetworkDescription() = default;
        //   bsl::size_t addProxy(level, addr, credentials);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const Endpoint    D1("", 0);        // 'endpoint'
        const Credentials D2("", "");       // 'credentials'

        // 'A' values: Should cause memory allocation if possible.

        const Endpoint    A1("a_" SUFFICIENTLY_LONG_STRING, 1);
        const Credentials A2("b_" SUFFICIENTLY_LONG_STRING,
                             "c_" SUFFICIENTLY_LONG_STRING);

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const Endpoint    B1("hostname", 65535);
        const Credentials B2("username", "password");

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            // Also invoke the object's 'allocator' accessor.

            LOOP3_ASSERT(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         0 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP2_ASSERT(CONFIG, X.numLevels(), 0 == X.numLevels());

            int proxyNum;
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    proxyNum = mX.addProxy(0, A1, A2);
                    LOOP_ASSERT(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(CONFIG,  0 == proxyNum);
                LOOP_ASSERT(CONFIG,  1 == X.numLevels());
                LOOP_ASSERT(CONFIG,  1 == X.numProxies(0));
                LOOP_ASSERT(CONFIG, A1 == X.beginLevel(0)->address());
                LOOP_ASSERT(CONFIG, A2 == X.beginLevel(0)->credentials());
                LOOP_ASSERT(CONFIG, X.beginLevel(0)   != X.endLevel(0));

                Obj::ProxyIterator iter = X.beginLevel(0);
                ++iter;
                LOOP_ASSERT(CONFIG, iter == X.endLevel(0));

                bslma::TestAllocatorMonitor tam(&oa);

                proxyNum = mX.addProxy(1, B1, B2);
                LOOP_ASSERT(CONFIG,  0 == proxyNum);
                LOOP_ASSERT(CONFIG, tam.isInUseUp());
                LOOP_ASSERT(CONFIG,  2 == X.numLevels());
                LOOP_ASSERT(CONFIG,  1 == X.numProxies(0));
                LOOP_ASSERT(CONFIG,  1 == X.numProxies(1));
                LOOP_ASSERT(CONFIG, A1 == X.beginLevel(0)->address());
                LOOP_ASSERT(CONFIG, A2 == X.beginLevel(0)->credentials());
                LOOP_ASSERT(CONFIG, B1 == X.beginLevel(1)->address());
                LOOP_ASSERT(CONFIG, B2 == X.beginLevel(1)->credentials());
                LOOP_ASSERT(CONFIG, X.beginLevel(0)   != X.endLevel(0));
                LOOP_ASSERT(CONFIG, X.beginLevel(1)   != X.endLevel(1));

                iter = X.beginLevel(0);
                ++iter;
                LOOP_ASSERT(CONFIG, iter == X.endLevel(0));

                iter = X.beginLevel(1);
                ++iter;
                LOOP_ASSERT(CONFIG, iter == X.endLevel(1));
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            LOOP_ASSERT(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());

            // Double check that some object memory was allocated.

            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        veryVerbose && cout << "C-tor and numLevels()" << endl;
        btls5::NetworkDescription network;
        ASSERT(0 == network.numLevels());

        btlso::Endpoint corp1("proxy1.corp.com", 1081);
        btlso::Endpoint corp2("proxy2.corp.com", 1082);
        btlso::Endpoint region1("proxy2.region.com", 1091);
        btlso::Endpoint region2("proxy2.region.com", 1092);
        network.addProxy(0, corp1);
        network.addProxy(0, corp2);
        ASSERT(1 == network.numLevels());
        network.addProxy(1, region1);
        network.addProxy(1, region2);
        ASSERT(2 == network.numLevels());

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
