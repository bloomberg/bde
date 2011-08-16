// btemt_channelpoolconfiguration.t.cpp                               -*-C++-*-

#include <btemt_channelpoolconfiguration.h>
#include <bdeat_sequencefunctions.h>
#include <bdet_timeinterval.h>
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_byteinstream.h>                  // for testing only

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing
//
//-----------------------------------------------------------------------------
// [ 3] static int maxSupportedBdexVersion();
// [ 1] btemt_ChannelPoolConfiguration();
// [ 1] btemt_ChannelPoolConfiguration(const btemt_ChannelPoolConfiguration&);
// [ 1] ~btemt_ChannelPoolConfiguration();
// [ 1] btemt_ChannelPoolConfiguration&
// [ 1] operator=(const btemt_ChannelPoolConfiguration& rhs);
// [ 2] int setIncomingMessageSizes(int min, int typical, int max);
// [ 2] int setOutgoingMessageSizes(int min, int typical, int max);
// [ 2] int setMaxConnections(int maxConnections);
// [ 2] int setMaxThreads(int maxThreads);
// [ 2] int setMaxWriteCache(int numBytes);
// [ 2] int setMetricsInterval(double metricsInterval);
// [ 2] int setReadTimeout(double readTimeout);
// [ 2] int setWorkloadThreshold(int threshold);
// [ 3] bdex_InStream& streamIn(bdex_InStream& stream);
// [ 3] bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [ 1] int minIncomingMessageSize() const;
// [ 1] int typicalIncomingMessageSize() const;
// [ 1] int maxIncomingMessageSize() const;
// [ 1] int minOutgoingMessageSize() const;
// [ 1] int typicalOutgoingMessageSize() const;
// [ 1] int maxOutgoingMessageSize() const;
// [ 1] int maxConnections() const;
// [ 1] int maxThreads() const;
// [ 1] int maxWriteCache() const;
// [ 1] double metricsInterval() const;
// [ 1] double readTimeout() const;
// [ 3] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
// [ 3] bdex_OutStream& streamOut(bdex_OutStream& stream, int version) const;
//
// [ 1] bool operator==(const btemt_ChannelPoolConfiguration& lhs, ...
// [ 1] bool operator!=(const btemt_ChannelPoolConfiguration& lhs, ...
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const btemt_ChannelPoolConf...
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btemt_ChannelPoolConfiguration Obj;
typedef double TI;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

const int NUM_VALUES = 7;

// The 0th element in each array contains the value set by the default
// constructor for the corresponding attribute.

const TI T00 = 30.0;
const TI T10 = 60.0;
const TI T11 = 61.1;
const TI T20 = 120.0;
const TI T21 = 121.0;
const TI T30 = 333.0;
const TI T31 = 333.3;
const TI T40 = 444.0;
const TI T41 = 444.4;
const TI T50 = 555.0;
const TI T51 = 555.5;
const TI T60 = 678.0;
const TI T61 = 689.6;

const int MAXCONNECTIONS[NUM_VALUES]   = { 1024, 10,  20,  300, 400, 500,
                                                                         600 };
const int MAXNUMTHREADS[NUM_VALUES]    = { 1,   11,  21,  301, 401, 501, 601 };
const int MAXWRITECACHE[NUM_VALUES]    = { 1048576, 512, 600, 700, 800, 900,
                                                                         999 };
const TI  READTIMEOUT[NUM_VALUES]      = { T00, T10, T20, T30, T40, T50, T60 };
const TI  METRICSINTERVAL[NUM_VALUES]  = { T00, T11, T21, T31, T41, T51, T61 };
const int MINMESSAGESIZEOUT[NUM_VALUES]= { 1,   12,  22,  302, 402, 502, 602 };
const int TYPMESSAGESIZEOUT[NUM_VALUES]= { 1,   13,  23,  303, 403, 503, 603 };
const int MAXMESSAGESIZEOUT[NUM_VALUES]= {1<<20,14,  24,  304, 404, 504, 604 };
const int MINMESSAGESIZEIN[NUM_VALUES] = { 1,   15,  25,  305, 405, 505, 605 };
const int TYPMESSAGESIZEIN[NUM_VALUES] = { 1,   16,  26,  306, 406, 506, 606 };
const int MAXMESSAGESIZEIN[NUM_VALUES] = { 1024,17,  27,  307, 407, 507, 607 };
const int WORKLOADTHRESHOLD[NUM_VALUES]= { 100, 18,  28,  308, 408, 508, 608 };
const int THREADSTACKSIZE[NUM_VALUES]  = { 1048576, 512, 600, 700, 800, 900,
                                                                         999 };
const bool COLLECTMETRICS[NUM_VALUES] =
                                     { true, false, true, false, true, false };

//=============================================================================
//                             HELPER CLASSES
//-----------------------------------------------------------------------------

                        // ===========================
                        // class GetValue<LVALUE_TYPE>
                        // ===========================

template <typename LVALUE_TYPE>
class GetValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE *d_lValue_p;  // held, not owned

  public:
    // CREATORS
    explicit GetValue(LVALUE_TYPE *lValue);

    // ACCESSORS
    template <typename INFO_TYPE>
    int operator()(const LVALUE_TYPE& object,
                   const INFO_TYPE&   info) const;
        // Assign 'object' to '*d_destination_p'.

    template <typename RVALUE_TYPE, typename INFO_TYPE>
    int operator()(const RVALUE_TYPE& object,
                   const INFO_TYPE&   info) const;
        // Do nothing.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <typename RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    const RVALUE_TYPE& d_value;  // held, not owned

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    template <typename INFO_TYPE>
    int operator()(RVALUE_TYPE      *object,
                   const INFO_TYPE&  info) const;
        // Assign 'd_value' to '*object'.

    template <typename LVALUE_TYPE, typename INFO_TYPE>
    int operator()(LVALUE_TYPE      *object,
                   const INFO_TYPE&  info) const;
        // Do nothing.
};

                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <typename LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <typename LVALUE_TYPE>
template <typename INFO_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object,
                                      const INFO_TYPE&   info) const
{
    *d_lValue_p = object;
    return 0;
}

template <typename LVALUE_TYPE>
template <typename RVALUE_TYPE, typename INFO_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object,
                                      const INFO_TYPE&   info) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <typename RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <typename RVALUE_TYPE>
template <typename INFO_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object,
                                         const INFO_TYPE&) const
{
    *object = d_value;
    return 0;
}

template <typename RVALUE_TYPE>
template <typename LVALUE_TYPE, typename INFO_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *object,
                                         const INFO_TYPE&) const
{
    return -1;
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        btemt_ChannelPoolConfiguration cpc;
        ASSERT(0 == cpc.setIncomingMessageSizes(1, 2, 3));
        ASSERT(1 == cpc.minIncomingMessageSize());
        ASSERT(2 == cpc.typicalIncomingMessageSize());
        ASSERT(3 == cpc.maxIncomingMessageSize());

        ASSERT(0 == cpc.setOutgoingMessageSizes(4, 5, 6));
        ASSERT(4 == cpc.minOutgoingMessageSize());
        ASSERT(5 == cpc.typicalOutgoingMessageSize());
        ASSERT(6 == cpc.maxOutgoingMessageSize());

        ASSERT(0 == cpc.setMaxConnections(100));
        ASSERT(100 == cpc.maxConnections());

        ASSERT(0 == cpc.setMaxThreads(200));
        ASSERT(200 == cpc.maxThreads());

        ASSERT(0 == cpc.setWriteCacheWatermarks(0, 1024));
        ASSERT(0 == cpc.writeCacheLowWatermark());
        ASSERT(1024 == cpc.writeCacheHiWatermark());

        ASSERT(0 == cpc.setReadTimeout(3.5));
        ASSERT(3.5 == cpc.readTimeout());

        ASSERT(0 == cpc.setMetricsInterval(5.25));
        ASSERT(5.25 == cpc.metricsInterval());

        ASSERT(0 == cpc.setThreadStackSize(1024));
        ASSERT(1024 == cpc.threadStackSize());

        ASSERT(0 != cpc.setIncomingMessageSizes(8, 4, 256));
        ASSERT(1 == cpc.minIncomingMessageSize());
        ASSERT(2 == cpc.typicalIncomingMessageSize());
        ASSERT(3 == cpc.maxIncomingMessageSize());

        {
            bsl::ostringstream os;
            os << cpc << bsl::flush;
            if (verbose) cout << "X1 buf:\n" << os.str() << endl;
            bsl::string s =
                "[" NL
                "\tmaxConnections         : 100" NL
                "\tmaxThreads             : 200" NL
                "\twriteCacheLowWat       : 0" NL
                "\twriteCacheHiWat        : 1024" NL
                "\treadTimeout            : 3.5" NL
                "\tmetricsInterval        : 5.25" NL
                "\tminOutgoingMessageSize : 4" NL
                "\ttypOutgoingMessageSize : 5" NL
                "\tmaxOutgoingMessageSize : 6" NL
                "\tminIncomingMessageSize : 1" NL
                "\ttypIncomingMessageSize : 2" NL
                "\tmaxIncomingMessageSize : 3" NL
                "\tthreadStackSize        : 1024" NL
                "\tcollectTimeMetrics     : 1" NL
                "]" NL
                ;
            ASSERT(os.str().c_str() == s);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING GENERATED TEMPLATE METHODS and DATA
        //
        // Plan:
        //   First we verify that the attribute info for this object are right.
        //   Then we verify that NUM_ATTRIBUTES is indeed the number of
        //   attributes in this object.  This will prove the thoroughness of
        //   the next test.  Use manipulateAttribute to set all supported
        //   attributes and verify that they affect the value of the object and
        //   the provided related manipulators.
        //   'manipulateAttribute(MANIPULATOR&)' is not tested directly but we
        //   verify that the number of attributes and verify we set all of
        //   them.  Since we use a tested generator, we do not need to test it.
        //   'accessAttribute(MANIPULATOR&)' will be tested similarly.  This
        //   method will be used to access a attribute on an object and
        //   we will use the manipulators to set the read value in a third
        //   object.  We will then compare all three created objects.  For
        //   the same reasons as 'manipulateAttribute' we do not need to
        //   test directly the others flavors of this method.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nGenerated template methods" << endl
                          << "\n==========================" << endl;

        enum {
            NUM_ATTRIBUTES = 14
        };

        ASSERT(NUM_ATTRIBUTES == Obj::NUM_ATTRIBUTES);

        const char* NAMES[] = {
        "MaxConnections", "MaxThreads", "ReadTimeout", "MetricsInterval",
        "MinMessageSizeOut", "TypMessageSizeOut", "MaxMessageSizeOut",
        "MinMessageSizeIn", "TypMessageSizeIn", "MaxMessageSizeIn",
        "WriteCacheLowWat", "WriteCacheHiWat", "ThreadStackSize",
        "CollectTimeMetrics"
        };

        const int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

        {
            btemt_ChannelPoolConfiguration cpc;
            for (int i = 0; i <  NUM_NAMES; ++i) {
                bdeat_AttributeInfo info
                     = btemt_ChannelPoolConfiguration::ATTRIBUTE_INFO_ARRAY[i];

                LOOP_ASSERT(i, NAMES[i] == bsl::string(info.name(),
                                                           info.nameLength()));
                LOOP_ASSERT(i, bsl::string("") == info.annotation());

                LOOP_ASSERT(i, i+1 == info.id());
            }
        }
        {
            // Check that NUM_ATTRIBUTES + 1 is not a valid id.

            btemt_ChannelPoolConfiguration cpc;
            AssignValue<int> visitor(0);
            ASSERT(-1 ==
                 bdeat_SequenceFunctions::manipulateAttribute(
                                                    &cpc,
                                                    visitor,
                                                    NUM_ATTRIBUTES + 1));
        }

        for (int i = 0; i < NUM_VALUES; ++i) {

            btemt_ChannelPoolConfiguration mA, mB, mC;

            // We need to go from the end since the order of attributes is
            // min,typ,max but we cannot set a min which is over max so
            // we need to make sure that max is set first.

            for (int j = NUM_ATTRIBUTES - 1; j >= 0; --j) {
                switch (j) {
                  case 0: {
                    ASSERT(0 == mA.setMaxConnections(MAXCONNECTIONS[i]));
                    AssignValue<int> visitor(MAXCONNECTIONS[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 1: {
                    ASSERT(0 == mA.setMaxThreads(MAXNUMTHREADS[i]));
                    AssignValue<int> visitor(MAXNUMTHREADS[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 2: {
                    ASSERT(0 == mA.setReadTimeout(READTIMEOUT[i]));
                    AssignValue<double> visitor(READTIMEOUT[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 3: {
                    ASSERT(0 == mA.setMetricsInterval(METRICSINTERVAL[i]));
                    AssignValue<double> visitor(METRICSINTERVAL[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 4: {
                    ASSERT(0 == mA.setOutgoingMessageSizes(
                                               MINMESSAGESIZEOUT[i],
                                               mA.typicalOutgoingMessageSize(),
                                               mA.maxOutgoingMessageSize()));
                    AssignValue<int> visitor(MINMESSAGESIZEOUT[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 5: {
                    ASSERT(0 == mA.setOutgoingMessageSizes(
                                                 mA.minOutgoingMessageSize(),
                                                 TYPMESSAGESIZEOUT[i],
                                                 mA.maxOutgoingMessageSize()));
                    AssignValue<int> visitor(TYPMESSAGESIZEOUT[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 6: {
                    ASSERT(0 == mA.setOutgoingMessageSizes(
                                               mA.minOutgoingMessageSize(),
                                               mA.typicalOutgoingMessageSize(),
                                               MAXMESSAGESIZEOUT[i]));
                    AssignValue<int> visitor(MAXMESSAGESIZEOUT[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 7: {
                    ASSERT(0 == mA.setIncomingMessageSizes(
                                               MINMESSAGESIZEIN[i],
                                               mA.typicalIncomingMessageSize(),
                                               mA.maxIncomingMessageSize()));
                    AssignValue<int> visitor(MINMESSAGESIZEIN[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 8: {
                    ASSERT(0 == mA.setIncomingMessageSizes(
                                                 mA.minIncomingMessageSize(),
                                                 TYPMESSAGESIZEIN[i],
                                                 mA.maxIncomingMessageSize()));
                    AssignValue<int> visitor(TYPMESSAGESIZEIN[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 9: {
                    ASSERT(0 == mA.setIncomingMessageSizes(
                                               mA.minIncomingMessageSize(),
                                               mA.typicalIncomingMessageSize(),
                                               MAXMESSAGESIZEIN[i]));
                    AssignValue<int> visitor(MAXMESSAGESIZEIN[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 10: {
                    ASSERT(0 == mA.setWriteCacheWatermarks(
                                                  i,
                                                  mA.writeCacheHiWatermark()));
                    AssignValue<int> visitor(i);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 11: {
                    ASSERT(0 == mA.setWriteCacheWatermarks(
                                                   mA.writeCacheLowWatermark(),
                                                   MAXWRITECACHE[i]));
                    AssignValue<int> visitor(MAXWRITECACHE[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 12: {
                    ASSERT(0 == mA.setThreadStackSize(THREADSTACKSIZE[i]));
                    AssignValue<int> visitor(THREADSTACKSIZE[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 13: {
                    ASSERT(0 == mA.setCollectTimeMetrics(COLLECTMETRICS[i]));
                    AssignValue<bool> visitor(COLLECTMETRICS[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdeat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;

                  default:
                    ASSERT(0);
                }
                LOOP2_ASSERT(i, j, mA == mB);

                if (j == 2 || j == 3) {
                    double value;
                    GetValue<double> gvisitor(&value);
                    ASSERT(0 ==
                     bdeat_SequenceFunctions::accessAttribute(mA, gvisitor,
                                                              j + 1));
                    AssignValue<double> avisitor(value);
                    ASSERT(0 ==
                     bdeat_SequenceFunctions::manipulateAttribute(&mC,
                                                                  avisitor,
                                                                  j + 1));
                }
                else if (j == 13) {
                    bool value;
                    GetValue<bool> gvisitor(&value);
                    ASSERT(0 ==
                     bdeat_SequenceFunctions::accessAttribute(mA, gvisitor,
                                                              j + 1));
                    AssignValue<bool> avisitor(value);
                    ASSERT(0 ==
                     bdeat_SequenceFunctions::manipulateAttribute(&mC,
                                                                  avisitor,
                                                                  j + 1));
                }
                else {
                    int value;
                    GetValue<int> gvisitor(&value);
                    ASSERT(0 ==
                     bdeat_SequenceFunctions::accessAttribute(mA, gvisitor,
                                                              j + 1));
                    AssignValue<int> avisitor(value);
                    ASSERT(0 ==
                     bdeat_SequenceFunctions::manipulateAttribute(&mC,
                                                                  avisitor,
                                                                  j + 1));
                }
                LOOP2_ASSERT(i, j, mA == mC);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the available bdex stream functions, which forward appropriate
        //   calls to the member functions.  We next step through the sequence
        //   of possible stream states (valid, empty, invalid, incomplete, and
        //   corrupted), appropriately selecting data sets as described below.
        //   In all cases, exception neutrality is confirmed using the
        //   specially instrumented 'bdex_TestInStream' and a pair of standard
        //   macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'outStream' and
        //     'inStream' methods (the rest of the testing will use the stream
        //     operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each data field in the
        //     stream (beginning with the version number), provide one or more
        //     similar tests with that data field corrupted.  After each test,
        //     verify that the object is in some valid state after streaming,
        //     and that the input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // object values for various stream tests

        Obj va, vb, vc, vd, ve, vf;
        ASSERT(0 == va.setIncomingMessageSizes(MINMESSAGESIZEIN[1],
                                                TYPMESSAGESIZEIN[1],
                                                MAXMESSAGESIZEIN[1]));
        ASSERT(0 == va.setOutgoingMessageSizes(MINMESSAGESIZEOUT[1],
                                                TYPMESSAGESIZEOUT[1],
                                                MAXMESSAGESIZEOUT[1]));
        ASSERT(0 == va.setMaxConnections(MAXCONNECTIONS[1]));
        ASSERT(0 == va.setMaxThreads(MAXNUMTHREADS[1]));
        ASSERT(0 == va.setMaxWriteCache(MAXWRITECACHE[1]));
        ASSERT(0 == va.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT(0 == va.setReadTimeout(READTIMEOUT[1]));
        ASSERT(0 == va.setThreadStackSize(THREADSTACKSIZE[1]));
        ASSERT(0 == va.setCollectTimeMetrics(COLLECTMETRICS[1]));

        ASSERT(0 == vb.setIncomingMessageSizes(MINMESSAGESIZEIN[2],
                                                TYPMESSAGESIZEIN[2],
                                                MAXMESSAGESIZEIN[2]));
        ASSERT(0 == vb.setOutgoingMessageSizes(MINMESSAGESIZEOUT[2],
                                                TYPMESSAGESIZEOUT[2],
                                                MAXMESSAGESIZEOUT[2]));
        ASSERT(0 == vb.setMaxConnections(MAXCONNECTIONS[2]));
        ASSERT(0 == vb.setMaxThreads(MAXNUMTHREADS[2]));
        ASSERT(0 == vb.setMaxWriteCache(MAXWRITECACHE[2]));
        ASSERT(0 == vb.setMetricsInterval(METRICSINTERVAL[2]));
        ASSERT(0 == vb.setReadTimeout(READTIMEOUT[2]));
        ASSERT(0 == vb.setThreadStackSize(THREADSTACKSIZE[2]));
        ASSERT(0 == vb.setCollectTimeMetrics(COLLECTMETRICS[2]));

        ASSERT(0 == vc.setIncomingMessageSizes(MINMESSAGESIZEIN[3],
                                                TYPMESSAGESIZEIN[3],
                                                MAXMESSAGESIZEIN[3]));
        ASSERT(0 == vc.setOutgoingMessageSizes(MINMESSAGESIZEOUT[3],
                                                TYPMESSAGESIZEOUT[3],
                                                MAXMESSAGESIZEOUT[3]));
        ASSERT(0 == vc.setMaxConnections(MAXCONNECTIONS[3]));
        ASSERT(0 == vc.setMaxThreads(MAXNUMTHREADS[3]));
        ASSERT(0 == vc.setMaxWriteCache(MAXWRITECACHE[3]));
        ASSERT(0 == vc.setMetricsInterval(METRICSINTERVAL[3]));
        ASSERT(0 == vc.setReadTimeout(READTIMEOUT[3]));
        ASSERT(0 == vc.setThreadStackSize(THREADSTACKSIZE[3]));
        ASSERT(0 == vc.setCollectTimeMetrics(COLLECTMETRICS[3]));

        ASSERT(0 == vd.setIncomingMessageSizes(MINMESSAGESIZEIN[4],
                                                TYPMESSAGESIZEIN[4],
                                                MAXMESSAGESIZEIN[4]));
        ASSERT(0 == vd.setOutgoingMessageSizes(MINMESSAGESIZEOUT[4],
                                                TYPMESSAGESIZEOUT[4],
                                                MAXMESSAGESIZEOUT[4]));
        ASSERT(0 == vd.setMaxConnections(MAXCONNECTIONS[4]));
        ASSERT(0 == vd.setMaxThreads(MAXNUMTHREADS[4]));
        ASSERT(0 == vd.setMaxWriteCache(MAXWRITECACHE[4]));
        ASSERT(0 == vd.setMetricsInterval(METRICSINTERVAL[4]));
        ASSERT(0 == vd.setReadTimeout(READTIMEOUT[4]));
        ASSERT(0 == vd.setThreadStackSize(THREADSTACKSIZE[4]));
        ASSERT(0 == vd.setCollectTimeMetrics(COLLECTMETRICS[4]));

        ASSERT(0 == ve.setIncomingMessageSizes(MINMESSAGESIZEIN[5],
                                                TYPMESSAGESIZEIN[5],
                                                MAXMESSAGESIZEIN[5]));
        ASSERT(0 == ve.setOutgoingMessageSizes(MINMESSAGESIZEOUT[5],
                                                TYPMESSAGESIZEOUT[5],
                                                MAXMESSAGESIZEOUT[5]));
        ASSERT(0 == ve.setMaxConnections(MAXCONNECTIONS[5]));
        ASSERT(0 == ve.setMaxThreads(MAXNUMTHREADS[5]));
        ASSERT(0 == ve.setMaxWriteCache(MAXWRITECACHE[5]));
        ASSERT(0 == ve.setMetricsInterval(METRICSINTERVAL[5]));
        ASSERT(0 == ve.setReadTimeout(READTIMEOUT[5]));
        ASSERT(0 == ve.setThreadStackSize(THREADSTACKSIZE[5]));
        ASSERT(0 == ve.setCollectTimeMetrics(COLLECTMETRICS[5]));

        ASSERT(0 == vf.setIncomingMessageSizes(MINMESSAGESIZEIN[6],
                                                TYPMESSAGESIZEIN[6],
                                                MAXMESSAGESIZEIN[6]));
        ASSERT(0 == vf.setOutgoingMessageSizes(MINMESSAGESIZEOUT[6],
                                                TYPMESSAGESIZEOUT[6],
                                                MAXMESSAGESIZEOUT[6]));
        ASSERT(0 == vf.setMaxConnections(MAXCONNECTIONS[6]));
        ASSERT(0 == vf.setMaxThreads(MAXNUMTHREADS[6]));
        ASSERT(0 == vf.setMaxWriteCache(MAXWRITECACHE[6]));
        ASSERT(0 == vf.setMetricsInterval(METRICSINTERVAL[6]));
        ASSERT(0 == vf.setReadTimeout(READTIMEOUT[6]));
        ASSERT(0 == vf.setThreadStackSize(THREADSTACKSIZE[6]));
        ASSERT(0 == vf.setCollectTimeMetrics(COLLECTMETRICS[6]));

        const Obj VA(va);
        const Obj VB(vb);
        const Obj VC(vc);
        const Obj VD(vd);
        const Obj VE(ve);
        const Obj VF(vf);

        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;               ASSERT(X != T);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(X == T);
            ASSERT(in);                                 ASSERT(in.isEmpty());

            if (veryVerbose) {
                P_(X); P(T);
            }
        }

        if (verbose) cout <<
            "\nThorough test of stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    bdex_InStreamFunctions::streamIn(in, t, VERSION);
                  } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;               // default value (0)
        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, 1);
            ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y != t);

            // Thread Stack Size is a new parameter that is unsupported in
            // version < 4.

            t.setThreadStackSize(THREADSTACKSIZE[2]);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        if (verbose) cout << "\t\t\tVersion too small." <<endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\t\tVersion too big." << endl;
        {
            const char version = 6; // too large (current versions all < 5)

            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value." << endl;
        if (verbose) cout << "\t\t\tmaxConnections neg." <<endl;
        {
            Out out;

            out.putInt32(-MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tmaxThreads neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(-MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tmaxWriteCache neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(-MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\treadTimeOut neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(-1.1);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tminOutgoingMessageSize neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(-MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "typOutgoingMessageSize < minOutgoingMessageSize." << endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEOUT[2] - 1);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "maxOutgoingMessageSize < typOutgoingMessageSize." << endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2] - 1);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tminIncomingMessageSize neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(-MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "typIncomingMessageSize < minIncomingMessageSize." << endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(MINMESSAGESIZEIN[2] - 1);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "maxIncomingMessageSize < typIncomingMessageSize." << endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2] - 1);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tworkloadThreshold neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(-WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tthreadStackSize neg." <<endl;
        {
            Out out;

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);
            out.putInt32(-THREADSTACKSIZE[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            const Obj X;
            ASSERT(5 == X.maxSupportedBdexVersion());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Constraints Test:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Constraints Test" << endl
                          << "================" << endl;

        Obj mX1; const Obj& X1 = mX1;

        if (verbose) cout << "\t Check maxConnections contraint. " << endl;
        {
            ASSERT(0 != mX1.setMaxConnections(-1));
            ASSERT(MAXCONNECTIONS[0] == X1.maxConnections());
            ASSERT(0 == mX1.setMaxConnections(0));
            ASSERT(0 == X1.maxConnections());
            ASSERT(0 == mX1.setMaxConnections(1));
            ASSERT(1 == X1.maxConnections());
            ASSERT(0 == mX1.setMaxConnections(2));
            ASSERT(2 == X1.maxConnections());
        }
        if (verbose) cout << "\t Check maxThreads contraint. " << endl;
        {
            ASSERT(0 != mX1.setMaxThreads(-1));
            ASSERT(MAXNUMTHREADS[0] == mX1.maxThreads());
            ASSERT(0 == mX1.setMaxThreads(0));
            ASSERT(0 == X1.maxThreads());
            ASSERT(0 == mX1.setMaxThreads(1));
            ASSERT(1 == X1.maxThreads());
        }
        if (verbose) cout << "\t Check maxWriteCache contraint. " << endl;
        {
            ASSERT(0 != mX1.setMaxWriteCache(-1));
            ASSERT(MAXWRITECACHE[0] == mX1.maxWriteCache());
            ASSERT(0 == mX1.setMaxWriteCache(0));
            ASSERT(0 == X1.maxWriteCache());
            ASSERT(0 == mX1.setMaxWriteCache(1));
            ASSERT(1 == X1.maxWriteCache());
        }
        if (verbose) cout << "\t Check readTimeOut contraint. " << endl;
        {
            ASSERT(0 != mX1.setReadTimeout(-1.1));
            ASSERT(READTIMEOUT[0] == X1.readTimeout());
            ASSERT(0 == mX1.setReadTimeout(0.0));
            ASSERT(0.0 == X1.readTimeout());
            ASSERT(0 == mX1.setReadTimeout(0.1));
            ASSERT(0.1 == X1.readTimeout());
        }
        if (verbose) cout << "\t Check metricsInterval contraint. " << endl;
        {
            ASSERT(0 != mX1.setMetricsInterval(-1.1));
            ASSERT(METRICSINTERVAL[0] == X1.metricsInterval());
            ASSERT(0 == mX1.setMetricsInterval(0.0));
            ASSERT(0.0 == X1.metricsInterval());
            ASSERT(0 == mX1.setMetricsInterval(0.1));
            ASSERT(0.1 == X1.metricsInterval());
        }
        if (verbose) cout << "\t Check workloadThreshold contraint. " << endl;
        {                                                         // DEPRECATED
            ASSERT(0 != mX1.setWorkloadThreshold(-1));
            ASSERT(0 == mX1.setWorkloadThreshold(0));
            ASSERT(0 == mX1.setWorkloadThreshold(1));
        }
        if (verbose) cout << "\t Check messageSizeIn contraint. " << endl;
        {
            ASSERT(0 != mX1.setIncomingMessageSizes(-1,  1,  1));
            ASSERT(0 != mX1.setIncomingMessageSizes( 1, -1,  1));
            ASSERT(0 != mX1.setIncomingMessageSizes( 1,  1, -1));
            ASSERT(0 != mX1.setIncomingMessageSizes(-1, -1,  0));
            ASSERT(0 != mX1.setIncomingMessageSizes(-1, -1, -1));

            ASSERT(0 != mX1.setIncomingMessageSizes(1, 0, 1));
            ASSERT(0 != mX1.setIncomingMessageSizes(1, 1, 0));
            ASSERT(0 != mX1.setIncomingMessageSizes(0, 2, 1));

            ASSERT(1 == X1.minIncomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(1024 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSizes(0, 0, 0));
            ASSERT(0 == X1.minIncomingMessageSize());
            ASSERT(0 == X1.typicalIncomingMessageSize());
            ASSERT(0 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSizes(1, 1, 1));
            ASSERT(1 == X1.minIncomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(1 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSizes(0, 1, 1));
            ASSERT(0 == X1.minIncomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(1 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSizes(0, 1, 2));
            ASSERT(0 == X1.minIncomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(2 == X1.maxIncomingMessageSize());
        }
        if (verbose) cout << "\t Check messageSizeOut contraint. " << endl;
        {
            ASSERT(0 != mX1.setOutgoingMessageSizes(-1,  1,  1));
            ASSERT(0 != mX1.setOutgoingMessageSizes( 1, -1,  1));
            ASSERT(0 != mX1.setOutgoingMessageSizes( 1,  1, -1));
            ASSERT(0 != mX1.setOutgoingMessageSizes(-1, -1,  0));
            ASSERT(0 != mX1.setOutgoingMessageSizes(-1, -1, -1));

            ASSERT(0 != mX1.setOutgoingMessageSizes(1, 0, 1));
            ASSERT(0 != mX1.setOutgoingMessageSizes(1, 1, 0));
            ASSERT(0 != mX1.setOutgoingMessageSizes(0, 2, 1));

            ASSERT(1 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(1 << 20 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSizes(0, 0, 0));
            ASSERT(0 == X1.minOutgoingMessageSize());
            ASSERT(0 == X1.typicalOutgoingMessageSize());
            ASSERT(0 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSizes(1, 1, 1));
            ASSERT(1 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(1 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSizes(0, 1, 1));
            ASSERT(0 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(1 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSizes(0, 1, 2));
            ASSERT(0 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(2 == X1.maxOutgoingMessageSize());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Basic Attribute Test" << endl
                          << "=====================" << endl;

        Obj mX1, mY1; const Obj& X1 = mX1; const Obj& Y1 = mY1;
        Obj mZ1; const Obj& Z1 = mZ1; // Z1 is the control
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\n Check default ctor. " << endl;

        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\t    Setting default values explicitly." <<endl;

        ASSERT(0 == mX1.setIncomingMessageSizes(MINMESSAGESIZEIN[0],
                                                TYPMESSAGESIZEIN[0],
                                                MAXMESSAGESIZEIN[0]));
        ASSERT(0 == mX1.setOutgoingMessageSizes(MINMESSAGESIZEOUT[0],
                                                TYPMESSAGESIZEOUT[0],
                                                MAXMESSAGESIZEOUT[0]));
        ASSERT(0 == mX1.setMaxConnections(MAXCONNECTIONS[0]));
        ASSERT(0 == mX1.setMaxThreads(MAXNUMTHREADS[0]));
        ASSERT(0 == mX1.setMaxWriteCache(MAXWRITECACHE[0]));
        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[0]));
        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[0]));
        ASSERT(0 == mX1.setThreadStackSize(THREADSTACKSIZE[0]));
        ASSERT(0 == mX1.setCollectTimeMetrics(COLLECTMETRICS[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTesting set/get methods."
                          << "\n\t  Change attribute 0." << endl;

        ASSERT(0 == mX1.setIncomingMessageSizes(MINMESSAGESIZEIN[1],
                                                TYPMESSAGESIZEIN[1],
                                                MAXMESSAGESIZEIN[1]));
        ASSERT( MINMESSAGESIZEIN[1] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[1] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[1] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setIncomingMessageSizes(MINMESSAGESIZEIN[0],
                                                TYPMESSAGESIZEIN[0],
                                                MAXMESSAGESIZEIN[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 1." << endl;

        ASSERT(0 == mX1.setOutgoingMessageSizes(MINMESSAGESIZEOUT[1],
                                                TYPMESSAGESIZEOUT[1],
                                                MAXMESSAGESIZEOUT[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[1] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[1] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[1] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setOutgoingMessageSizes(MINMESSAGESIZEOUT[0],
                                                TYPMESSAGESIZEOUT[0],
                                                MAXMESSAGESIZEOUT[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 2." << endl;

        ASSERT(0 == mX1.setMaxConnections(MAXCONNECTIONS[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[1] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMaxConnections(MAXCONNECTIONS[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 3." << endl;

        ASSERT(0 == mX1.setMaxThreads(MAXNUMTHREADS[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[1] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMaxThreads(MAXNUMTHREADS[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 4." << endl;

        ASSERT(0 == mX1.setMaxWriteCache(MAXWRITECACHE[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[1] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMaxWriteCache(MAXWRITECACHE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 5." << endl;

        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[1] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 6." << endl;

        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[1] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 7." << endl;

        ASSERT(0 == mX1.setThreadStackSize(THREADSTACKSIZE[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[1] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[0] == X1.collectTimeMetrics());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setThreadStackSize(THREADSTACKSIZE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 8." << endl;

        ASSERT(0 == mX1.setCollectTimeMetrics(COLLECTMETRICS[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(  THREADSTACKSIZE[0] == X1.threadStackSize());
        ASSERT(   COLLECTMETRICS[1] == X1.collectTimeMetrics());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setCollectTimeMetrics(COLLECTMETRICS[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "Testing output operator (<<)." << endl;

        ASSERT(0 == mY1.setIncomingMessageSizes(MINMESSAGESIZEIN[1],
                                                TYPMESSAGESIZEIN[1],
                                                MAXMESSAGESIZEIN[1]));
        ASSERT(0 == mY1.setOutgoingMessageSizes(MINMESSAGESIZEOUT[1],
                                                TYPMESSAGESIZEOUT[1],
                                                MAXMESSAGESIZEOUT[1]));
        ASSERT(0 == mY1.setMaxConnections(MAXCONNECTIONS[1]));
        ASSERT(0 == mY1.setMaxThreads(MAXNUMTHREADS[1]));
        ASSERT(0 == mY1.setMaxWriteCache(MAXWRITECACHE[1]));
        ASSERT(0 == mY1.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT(0 == mY1.setReadTimeout(READTIMEOUT[1]));
        ASSERT(0 == mY1.setThreadStackSize(THREADSTACKSIZE[1]));
        ASSERT(mX1 != mY1);

        char buf[10000];
        {
            bsl::ostrstream o(buf, sizeof buf);
            o << X1 << ends;
            if (verbose) cout << "X1 buf:\n" << buf << endl;
            bsl::string s =
                "[" NL
                "\tmaxConnections         : 1024" NL
                "\tmaxThreads             : 1" NL
                "\twriteCacheLowWat       : 0" NL
                "\twriteCacheHiWat        : 1048576" NL
                "\treadTimeout            : 30" NL
                "\tmetricsInterval        : 30" NL
                "\tminOutgoingMessageSize : 1" NL
                "\ttypOutgoingMessageSize : 1" NL
                "\tmaxOutgoingMessageSize : 1048576" NL
                "\tminIncomingMessageSize : 1" NL
                "\ttypIncomingMessageSize : 1" NL
                "\tmaxIncomingMessageSize : 1024" NL
                "\tthreadStackSize        : 1048576" NL
                "\tcollectTimeMetrics     : 1" NL
                "]" NL
                ;
            ASSERT(buf == s);
        }
        {
            bsl::ostrstream o(buf, sizeof buf);
            o << Y1 << ends;
            if (verbose) cout << "Y1 buf:\n" << buf << endl;
            bsl::string s =
                "[" NL
                "\tmaxConnections         : 10" NL
                "\tmaxThreads             : 11" NL
                "\twriteCacheLowWat       : 0" NL
                "\twriteCacheHiWat        : 512" NL
                "\treadTimeout            : 60" NL
                "\tmetricsInterval        : 61.1" NL
                "\tminOutgoingMessageSize : 12" NL
                "\ttypOutgoingMessageSize : 13" NL
                "\tmaxOutgoingMessageSize : 14" NL
                "\tminIncomingMessageSize : 15" NL
                "\ttypIncomingMessageSize : 16" NL
                "\tmaxIncomingMessageSize : 17" NL
                "\tthreadStackSize        : 512" NL
                "\tcollectTimeMetrics     : 1" NL
                "]" NL
                ;
            ASSERT(buf == s);
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
