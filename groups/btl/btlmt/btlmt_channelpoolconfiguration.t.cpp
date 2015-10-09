// btlmt_channelpoolconfiguration.t.cpp                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_channelpoolconfiguration.h>
#include <bdlat_sequencefunctions.h>
#include <bsls_timeinterval.h>

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
// [ 1] btlmt::ChannelPoolConfiguration();
// [ 1] btlmt::ChannelPoolConfiguration(original);
// [ 1] ~btlmt::ChannelPoolConfiguration();
// [ 1] btlmt::ChannelPoolConfiguration&
// [ 1] operator=(const btlmt::ChannelPoolConfiguration& rhs);
// [ 2] int setIncomingMessageSizes(int min, int typical, int max);
// [ 2] int setOutgoingMessageSizes(int min, int typical, int max);
// [ 2] int setMaxConnections(int maxConnections);
// [ 2] int setMaxThreads(int maxThreads);
// [ 2] int setMetricsInterval(double metricsInterval);
// [ 2] int setReadTimeout(double readTimeout);
// [ 1] int minIncomingMessageSize() const;
// [ 1] int typicalIncomingMessageSize() const;
// [ 1] int maxIncomingMessageSize() const;
// [ 1] int minOutgoingMessageSize() const;
// [ 1] int typicalOutgoingMessageSize() const;
// [ 1] int maxOutgoingMessageSize() const;
// [ 1] int maxConnections() const;
// [ 1] int maxThreads() const;
// [ 1] double metricsInterval() const;
// [ 1] double readTimeout() const;
//
// [ 1] bool operator==(const btlmt::ChannelPoolConfiguration& lhs, ...
// [ 1] bool operator!=(const btlmt::ChannelPoolConfiguration& lhs, ...
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const btemt_ChannelPoolConf...
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
// [ 4] TESTING GENERATED TEMPLATE METHODS and DATA
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

typedef btlmt::ChannelPoolConfiguration Obj;
typedef double TI;

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

template <class LVALUE_TYPE>
class GetValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE *d_lValue_p;  // held, not owned

  public:
    // CREATORS
    explicit GetValue(LVALUE_TYPE *lValue);

    // ACCESSORS
    template <class INFO_TYPE>
    int operator()(const LVALUE_TYPE& object,
                   const INFO_TYPE&   info) const;
        // Assign 'object' to '*d_destination_p'.

    template <class RVALUE_TYPE, class INFO_TYPE>
    int operator()(const RVALUE_TYPE& object,
                   const INFO_TYPE&   info) const;
        // Do nothing.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <class RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    const RVALUE_TYPE& d_value;  // held, not owned

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    template <class INFO_TYPE>
    int operator()(RVALUE_TYPE      *object,
                   const INFO_TYPE&  info) const;
        // Assign 'd_value' to '*object'.

    template <class LVALUE_TYPE, class INFO_TYPE>
    int operator()(LVALUE_TYPE      *object,
                   const INFO_TYPE&  info) const;
        // Do nothing.
};

                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <class LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <class LVALUE_TYPE>
template <class INFO_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object,
                                      const INFO_TYPE&   info) const
{
    *d_lValue_p = object;
    return 0;
}

template <class LVALUE_TYPE>
template <class RVALUE_TYPE, class INFO_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object,
                                      const INFO_TYPE&   info) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <class RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <class RVALUE_TYPE>
template <class INFO_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object,
                                         const INFO_TYPE&) const
{
    *object = d_value;
    return 0;
}

template <class RVALUE_TYPE>
template <class LVALUE_TYPE, class INFO_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *,
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        btlmt::ChannelPoolConfiguration cpc;
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
      case 3: {
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

        ASSERT(NUM_ATTRIBUTES == Obj::k_NUM_ATTRIBUTES);

        const char* NAMES[] = {
        "MaxConnections", "MaxThreads", "ReadTimeout", "MetricsInterval",
        "MinMessageSizeOut", "TypMessageSizeOut", "MaxMessageSizeOut",
        "MinMessageSizeIn", "TypMessageSizeIn", "MaxMessageSizeIn",
        "WriteCacheLowWat", "WriteCacheHiWat", "ThreadStackSize",
        "CollectTimeMetrics"
        };

        const int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

        {
            btlmt::ChannelPoolConfiguration cpc;
            for (int i = 0; i <  NUM_NAMES; ++i) {
                bdlat_AttributeInfo info =
                      btlmt::ChannelPoolConfiguration::ATTRIBUTE_INFO_ARRAY[i];

                LOOP_ASSERT(i, NAMES[i] == bsl::string(info.name(),
                                                           info.nameLength()));
                LOOP_ASSERT(i, bsl::string("") == info.annotation());

                LOOP_ASSERT(i, i+1 == info.id());
            }
        }
        {
            // Check that NUM_ATTRIBUTES + 1 is not a valid id.

            btlmt::ChannelPoolConfiguration cpc;
            AssignValue<int> visitor(0);
            ASSERT(-1 ==
                 bdlat_SequenceFunctions::manipulateAttribute(
                                                    &cpc,
                                                    visitor,
                                                    NUM_ATTRIBUTES + 1));
        }

        for (int i = 0; i < NUM_VALUES; ++i) {

            btlmt::ChannelPoolConfiguration mA, mB, mC;

            // We need to go from the end since the order of attributes is
            // min,typ,max but we cannot set a min which is over max so
            // we need to make sure that max is set first.

            for (int j = NUM_ATTRIBUTES - 1; j >= 0; --j) {
                switch (j) {
                  case 0: {
                    ASSERT(0 == mA.setMaxConnections(MAXCONNECTIONS[i]));
                    AssignValue<int> visitor(MAXCONNECTIONS[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 1: {
                    ASSERT(0 == mA.setMaxThreads(MAXNUMTHREADS[i]));
                    AssignValue<int> visitor(MAXNUMTHREADS[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 2: {
                    ASSERT(0 == mA.setReadTimeout(READTIMEOUT[i]));
                    AssignValue<double> visitor(READTIMEOUT[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 3: {
                    ASSERT(0 == mA.setMetricsInterval(METRICSINTERVAL[i]));
                    AssignValue<double> visitor(METRICSINTERVAL[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 10: {
                    ASSERT(0 == mA.setWriteCacheWatermarks(
                                                  i,
                                                  mA.writeCacheHiWatermark()));
                    AssignValue<int> visitor(i);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 11: {
                    ASSERT(0 == mA.setWriteCacheWatermarks(
                                                   mA.writeCacheLowWatermark(),
                                                   MAXWRITECACHE[i]));
                    AssignValue<int> visitor(MAXWRITECACHE[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 12: {
                    ASSERT(0 == mA.setThreadStackSize(THREADSTACKSIZE[i]));
                    AssignValue<int> visitor(THREADSTACKSIZE[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
                                                                    visitor,
                                                                    j + 1));
                  } break;
                  case 13: {
                    ASSERT(0 == mA.setCollectTimeMetrics(COLLECTMETRICS[i]));
                    AssignValue<bool> visitor(COLLECTMETRICS[i]);
                    LOOP2_ASSERT(i, j, 0 ==
                       bdlat_SequenceFunctions::manipulateAttribute(&mB,
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
                     bdlat_SequenceFunctions::accessAttribute(mA, gvisitor,
                                                              j + 1));
                    AssignValue<double> avisitor(value);
                    ASSERT(0 ==
                     bdlat_SequenceFunctions::manipulateAttribute(&mC,
                                                                  avisitor,
                                                                  j + 1));
                }
                else if (j == 13) {
                    bool value;
                    GetValue<bool> gvisitor(&value);
                    ASSERT(0 ==
                     bdlat_SequenceFunctions::accessAttribute(mA, gvisitor,
                                                              j + 1));
                    AssignValue<bool> avisitor(value);
                    ASSERT(0 ==
                     bdlat_SequenceFunctions::manipulateAttribute(&mC,
                                                                  avisitor,
                                                                  j + 1));
                }
                else {
                    int value;
                    GetValue<int> gvisitor(&value);
                    ASSERT(0 ==
                     bdlat_SequenceFunctions::accessAttribute(mA, gvisitor,
                                                              j + 1));
                    AssignValue<int> avisitor(value);
                    ASSERT(0 ==
                     bdlat_SequenceFunctions::manipulateAttribute(&mC,
                                                                  avisitor,
                                                                  j + 1));
                }
                LOOP2_ASSERT(i, j, mA == mC);
            }
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
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

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

        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
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

        if (verbose) cout << "\t Change attribute 5." << endl;

        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
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

        if (verbose) cout << "\t Change attribute 6." << endl;

        ASSERT(0 == mX1.setThreadStackSize(THREADSTACKSIZE[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
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

        if (verbose) cout << "\t Change attribute 7." << endl;

        ASSERT(0 == mX1.setCollectTimeMetrics(COLLECTMETRICS[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.minIncomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(    MAXNUMTHREADS[0] == X1.maxThreads());
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
                "\twriteCacheHiWat        : 1048576" NL
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
