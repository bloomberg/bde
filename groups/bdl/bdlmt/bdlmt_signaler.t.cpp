// bdlmt_signaler.t.cpp                                               -*-C++-*-
#include <bdlmt_signaler.h>

#include <bdlmt_fixedthreadpool.h>

#include <bdlcc_deque.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_movableref.h>
#include <bslmt_threadutil.h>

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_cpp11.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

// CONVENIENCE
using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
// [17] Usage example


// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

bslma::TestAllocator ta("test");

// ============================================================================
//                      ADDITIONAL COMPARISON TEST MACROS
// ----------------------------------------------------------------------------
// Set of additional useful and more powerful test macros that check for
// equality (or other common comparison operation), and print the expression as
// well as their evaluated value in case of negative result.  The template
// method is used so that we don't double evaluate the test operands (once for
// checking whether the assertion was success and once for printing the
// values).

#define ASSERT_COMPARE_DECLARE(NAME, OP)                                      \
template<typename P, typename T, typename U>                                  \
void _assertCompare ## NAME(const P&    prefix,                               \
                            const T&    xResult,                              \
                            const U&    yResult,                              \
                            const char *xStr,                                 \
                            const char *yStr,                                 \
                            int         line)                                 \
{                                                                             \
    if (!(xResult OP yResult)) {                                              \
        bdlsb::MemOutStreamBuf buffer(&ta);                                   \
        bsl::ostream os(&buffer);                                             \
        os << prefix                                                          \
           << "'" << xStr << "' "                                             \
           << "(" << xResult << ")"                                           \
           << " " #OP " "                                                     \
           << "'" << yStr << "' "                                             \
           << "(" << yResult << ")"                                           \
           << bsl::ends;                                                      \
        bslstl::StringRef str(buffer.data(), buffer.length());                \
        aSsErT(false, str.data(), line);                                      \
    }                                                                         \
}

// Create a definition of the above template method for each of the 6 common
// comparison operators.

ASSERT_COMPARE_DECLARE(Equals,        ==)
ASSERT_COMPARE_DECLARE(NotEquals,     !=)

#define ASSERT_EQ(X, Y)                                                       \
    _assertCompareEquals("", X, Y, #X, #Y, __LINE__)

#define ASSERT_NE(X, Y)                                                       \
    _assertCompareNotEquals("", X, Y, #X, #Y, __LINE__)

// ============================================================================
//                              GLOBAL VARIABLES
// ----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;

static bool               ignoreCheckGblAlloc = false;

                          // Global flag which can be set to ignore checking
                          // the global allocator usage for a specific test
                          // case.

// ============================================================================
//                            TEST HELPERS UTILITY
// ----------------------------------------------------------------------------
namespace {
namespace u {

struct NoOp {
    // A no-op.

    // TYPES
    typedef void ResultType;

    // MANIPULATORS
    void operator()() const BSLS_CPP11_NOEXCEPT
    { }

    template <class A1>
    void
    operator()(BSLS_ANNOTATION_UNUSED const A1&) const BSLS_CPP11_NOEXCEPT
    { }

    template <class A1,
              class A2>
    void
    operator()(BSLS_ANNOTATION_UNUSED const A1&,
               BSLS_ANNOTATION_UNUSED const A2&) const BSLS_CPP11_NOEXCEPT
    { }

    template <class A1,
              class A2,
              class A3>
    void
    operator()(BSLS_ANNOTATION_UNUSED const A1&,
               BSLS_ANNOTATION_UNUSED const A2&,
               BSLS_ANNOTATION_UNUSED const A3&) const BSLS_CPP11_NOEXCEPT
    { }

    template <class A1,
              class A2,
              class A3,
              class A4>
    void
    operator()(BSLS_ANNOTATION_UNUSED const A1&,
               BSLS_ANNOTATION_UNUSED const A2&,
               BSLS_ANNOTATION_UNUSED const A3&,
               BSLS_ANNOTATION_UNUSED const A4&) const BSLS_CPP11_NOEXCEPT
    { }

    template <class A1,
              class A2,
              class A3,
              class A4,
              class A5>
    void
    operator()(BSLS_ANNOTATION_UNUSED const A1&,
               BSLS_ANNOTATION_UNUSED const A2&,
               BSLS_ANNOTATION_UNUSED const A3&,
               BSLS_ANNOTATION_UNUSED const A4&,
               BSLS_ANNOTATION_UNUSED const A5&) const BSLS_CPP11_NOEXCEPT
    { }
};

struct DoubleTI : public bsls::TimeInterval {
    // This 'class' is meant to behave just like a 'bsls::TimeInterval' except
    // that it will also implicitly cast to a 'double'.

    // CREATORS
    DoubleTI() : TimeInterval() {}
    DoubleTI(const DoubleTI&          original)
    : TimeInterval(original)
        // Copy construct from the specified 'original'.
    {}

    DoubleTI(const bsls::TimeInterval other)
    : TimeInterval(other)                                           // IMPLICIT
        // Construct from the specified 'other'.
    {}

    explicit
    DoubleTI(const double             other)
    : TimeInterval(other)
        // Construct from the specified 'other'.
    {}

    // MANIPULATORS
    DoubleTI& operator=(const DoubleTI rhs)
        // Copy-assign from the specified 'rhs' and return this object.
    {
        bsls::TimeInterval& base = *this;
        base = rhs;

        return *this;
    }

    DoubleTI& operator=(const TimeInterval rhs)
        // Assign from the specified 'rhs' and return this object.
    {
        bsls::TimeInterval& base = *this;
        base = rhs;

        return *this;
    }

    DoubleTI& operator=(const double rhs)
        // Assign from the specified 'rhs' and return this object.
    {
        bsls::TimeInterval& base = *this;
        base = rhs;

        return *this;
    }

    // ACCESSORS
    operator double() const
        // Return the value of this object as a 'double'.
    {
        return this->totalSecondsAsDouble();
    }
};

inline
DoubleTI operator-(const DoubleTI& lhs, const DoubleTI& rhs)
    // Return the difference between the specified 'lhs' and 'rhs' and as
    // 'DoubleTI'.
{
    const bsls::TimeInterval& lhsBase = lhs;
    const bsls::TimeInterval& rhsBase = rhs;

    return DoubleTI(lhsBase - rhsBase);
}

struct PrintStr1 {
    // Prints a string to a specified 'ostringstream'.

    // ACCESSORS
    void operator()(bsl::ostringstream& out,
                    const char*         str) const
    {
        out << str;
    }
};

struct PrintStr2 {
    // Prints two strings to a specified 'ostringstream'.

    // ACCESSORS
    void operator()(bsl::ostringstream& out,
                    const char*         str1,
                    const char*         str2) const
    {
        out << str1 << str2;
    }
};

template <class TYPE>
TYPE abs(TYPE x)
{
    return x < 0 ? -x : x;
}
                                // -------
                                // RandGen
                                // -------

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64 d_seed;

    // PRIVATE MANIPULATOR
    void munge();
        // Churn 'd_seed'.

  public:
    // CREATOR
    explicit
    RandGen(int startSeed);
        // Initialize the generator with the specified 'startSeed'.

    // MANIPULATOR
    unsigned operator()();
        // Return the next random number in the series;
};

// PRIVATE MANIPULATOR
inline
void RandGen::munge()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
}

// CREATOR
inline
RandGen::RandGen(int startSeed)
: d_seed(startSeed)
{
    munge();
    munge();
    munge();
}

// MANIPULATOR
inline
unsigned RandGen::operator()()
{
    munge();
    return static_cast<unsigned>(d_seed >> 32);
}

struct SleepAndPushTimestamp {
    // This functional object is used to test the waiting behavior of
    // 'disconnectAndWait()' functions.
    //
    // It's call operator accepts an output synchronizable queue of
    // 'bsls::TimeInterval' and the number of seconds to sleep. When
    // invoked, does the following:
    // 1. push the current timestamp to the queue (start timestamp)
    // 2. sleeps for the specified number of seconds
    // 3. push the current timestamp to the queue (end timestamp)

    // ACCESSORS
    void operator()(double                     sleepForSec,
                    bdlcc::Deque<u::DoubleTI> *queue) const
    {
        queue->pushBack(
                    bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC));

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(sleepForSec));

        queue->pushBack(
                    bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC));
    }
};

// Commented out due to the test requiring it (test3_signaler_callOperator) not
// being enabled.
// struct CondCall {
//     // Conditionally call a specified signaler. Before calling, set the
//     // condition to 'false'.

//     // ACCESSORS
//     template <class SIGNALER>
//     void operator()(const SIGNALER   *signaler,
//                     bsls::AtomicBool *condition) const
//     {
//         if (*condition) {
//             *condition = false;
//             (*signaler)();
//         }
//     }
// };

struct Connect {
    // Connects a specified slot to a specified signaler.

    // ACCESSORS
    template <class SIGNALER, class SLOT>
    void operator()(SIGNALER *signaler, const SLOT& slot) const
    {
        signaler->connect(slot);
    }
};

struct CondDisconnectAndWait {
    // Conditionally disconnects a specified connection waiting for its
    // associated slot completion.

    // ACCESSORS
    template <class CONNECTION>
    void operator()(CONNECTION             *connection,
                    const bsls::AtomicBool *condition) const
    {
        if (*condition) {
            connection->disconnectAndWait();
        }
    }
};

struct ThrowOnCall {
    // Throws an instance of 'ThrowOnCall::ExceptionType' on call.

    // TYPES
    struct ExceptionType { };

    // ACCESSORS
    void operator()() const
    {
        throw ExceptionType();
    }
};

struct ThrowOnCopy {
    // Throws an instance of 'ThrowOnCopy::ExceptionType' on copy.

    // TYPES
    struct ExceptionType { };

    // CREATORS
    ThrowOnCopy()
    {
    }

    ThrowOnCopy(const ThrowOnCopy& original)
    {
        (void) original;

        throw ExceptionType();
    }

    // MANIPULATORS
    ThrowOnCopy& operator=(const ThrowOnCopy&)
    {
        throw ExceptionType();
    }

    // ACCESSORS
    void operator()() const
    {
        // NOTHING
    }
};

class Button {
    // A pretend GUI button from the usage example.

  public:
    // TYPES
    typedef bsl::function<void(int, int)> OnClickSlotType;
        // Slot arguments are the 'x' and 'y' cursor coordinates.

  private:
    // PRIVATE DATA
    bdlmt::Signaler<void(int, int)> d_onClick;
        // Signaler arguments are the 'x' and 'y' cursor coordinates.

  public:
    // MANIPULATORS
    bdlmt::SignalerConnection onClickConnect(const OnClickSlotType& slot)
        // Connect the specified 'slot' to this button.
    {
        return d_onClick.connect(slot);
    }

    void click(int x, int y)
        // Simulate user clicking on GUI button at coordinates 'x', 'y'.
    {
        d_onClick(x, y);
    }
};

void printCoordinates(int x, int y)
{
    bsl::cout << "(" << x << ", " << y << ")" << bsl::endl;
}

}  // close namespace u
}  // close unnamed namespace


// ============================================================================
//                                    TESTS
// ----------------------------------------------------------------------------

static void test1_signaler_defaultConstructor()
    // ------------------------------------------------------------------------
    // SIGNALER DEFAULT CONSTRUCTOR
    //
    // Concerns:
    //   Ensure proper behavior of the default constructor.
    //
    // Plan:
    //   Default-construct an instance of 'bdlmt::Signaler'. Check
    //   postconditions.
    //
    // Testing:
    //   bdlmt::Signaler's default constructor.
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // no slots connected
    ASSERT_EQ(sig.slotCount(), 0u);
}

static void test2_signaler_destructor()
    // ------------------------------------------------------------------------
    // DESTRUCTOR
    //
    // Concerns:
    //   Ensure proper behavior of the destructor.
    //
    // Plan:
    //   Create a signaler, connect several slots, destroy the signaler. Check
    //   that all slots were disconnected.
    //
    // Testing:
    //   bdlmt::Signaler's destructor.
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator      alloc;
    bdlmt::SignalerConnection con1, con2;

    {
        bdlmt::Signaler<void()> sig(&alloc);

        ASSERTV(sig.slotCount(), 0 == sig.slotCount());

        // connect slots
        con1 = sig.connect(u::NoOp());

        ASSERTV(sig.slotCount(), 1 == sig.slotCount());

        con2 = sig.connect(u::NoOp());

        ASSERTV(sig.slotCount(), 2 == sig.slotCount());

        (void) sig.connect(u::NoOp());

        ASSERTV(sig.slotCount(), 3 == sig.slotCount());

        // slots are connected
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con2.isConnected(), true);

        // destroy the signaler ...
    }

    // slots are disconnected
    ASSERT_EQ(con1.isConnected(), false);
    ASSERT_EQ(con2.isConnected(), false);
}

namespace test3_signaler {

typedef bsl::deque<bsl::function<void (const char *)> > FDeque;

u::RandGen rand(43);

void connectThem(bdlmt::Signaler<void(const char*)> *sig,
                 const FDeque&                       deque,
                 const int                          *groups)
{
    bool taken[3];
    bsl::memset(taken, 0, sizeof(taken));
    for (int ii = 0; ii < 3; ++ii) {
        unsigned jj = rand() % 3;
        while (taken[jj]) {
            jj = (jj + 1) % 3;
        }
        sig->connect(deque[jj], groups[jj]);
        taken[jj] = true;
    }
}

struct Op {
    // DATA
    bsl::ostream *d_stream_p;
    const char   *d_string;

    // CREATOR
    Op(bsl::ostream *stream_p, const char *str)
    : d_stream_p(stream_p)
    , d_string(str)
    {}

    // ACCESSOR
    void operator()()
    {
        *d_stream_p << d_string;
    }
};

static void callOperator()
    // ------------------------------------------------------------------------
    // SIGNALER CALL OPERATOR
    //
    // Concerns:
    //   Ensure proper behavior of the call operator.
    //
    // Plan:
    //   1. Create a signaler. Connect several slots, specifying a call group
    //      for each slot. Call the signaler. Check that:
    //      - All connected slots were invoked;
    //      - Invoked slots were passed the same arguments as the signaler's
    //        call operator;
    //      - Slots were invoked in the order defined by their respective
    //        groups, and (within groups) by the connection order.
    //
    //   2. Create a signaler. Connect slots #1, #2 and #3, given that slot #2,
    //      when invoked, "disables" itself and recursively calls the signaler.
    //      Call the signaler. Check that the slots invocation order is the
    //      following: #1, #2, #1, #3, #3.
    //
    //      NOTE: THIS TEST IS DISABLED
    //
    //   3. Create a signaler. Connect slots #1, #2, #3 and #4, given that slot
    //      #3, when invoked, throws an exception. Call the signaler. Check
    //      that:
    //      - The exception thrown by slot #3 is propagated to the signaler's
    //        caller;
    //      - The slot invocation sequence is interrupted after the call to
    //        slot #3, i.e. only slots #1, #2 and #3 are invoked.
    //
    //   4. Check that the call operator correctly forwards lvalue references.
    //
    // Testing:
    //   bdlmt::Signaler::operator()()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    FDeque d(&alloc);
    int groups[3];

    // 1. general case

    for (int ti = 0; ti < 100; ++ti) {
        bsl::ostringstream                 out(&alloc);
        bdlmt::Signaler<void(const char*)> sig(&alloc);

        // connect slots with low order '#1'

        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "-1#1:",
                                                bdlf::PlaceHolders::_1));
        groups[0] = -1;
        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "0#1:",
                                                bdlf::PlaceHolders::_1));
        groups[1] = 0;
        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "1#1:",
                                                bdlf::PlaceHolders::_1));
        groups[2] = 1;
        connectThem(&sig, d, groups);
        d.clear();

        // connect slots with low order '#2'

        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "0#2:",
                                                bdlf::PlaceHolders::_1));
        groups[0] = 0;
        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "-1#2:",
                                                bdlf::PlaceHolders::_1));
        groups[1] = -1;
        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "1#2:",
                                                bdlf::PlaceHolders::_1));
        groups[2] = 1;
        connectThem(&sig, d, groups);
        d.clear();

        // connect slots with low order '#3'

        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "1#3:",
                                                bdlf::PlaceHolders::_1));
        groups[0] = 1;
        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "-1#3:",
                                                bdlf::PlaceHolders::_1));
        groups[1] = -1;
        d.push_back(bdlf::BindUtil::bindR<void>(u::PrintStr2(),
                                                bsl::ref(out),
                                                "0#3:",
                                                bdlf::PlaceHolders::_1));
        groups[2] = 0;
        connectThem(&sig, d, groups);
        d.clear();

        ASSERTV(sig.slotCount(), 9 == sig.slotCount());

        // invoke the signaler
        sig("X_");

        // all slots were invoked in the group / connection order, and with the
        // same arguments as passed to the signaler's call operator

        ASSERT_EQ(out.str(), "-1#1:X_-1#2:X_-1#3:X_"
                             "0#1:X_0#2:X_0#3:X_"
                             "1#1:X_1#2:X_1#3:X_");
    }

#if 0
    NOTE: This test is disabled. Currently, recursively calling a signaler is
          undefined behavior.

    // 2. recursively call the signaler from a slot

    {
        bsl::ostringstream      out(&alloc);
        bdlmt::Signaler<void()> sig(&alloc);
        bsls::AtomicBool slot2Enabled(true);

        // slot #1, prints "1_"

        sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                bsl::ref(out),
                                                "1_"));

        // slot #2, "disables" itself and calls the signaler

        sig.connect(bdlf::BindUtil::bindR<void>(CondCall(),
                                                &sig,
                                                &slot2Enabled));

        // slot #3, prints "3_"

        sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                bsl::ref(out),
                                                "3_"));

        // invoke the signaler

        sig();

        // check slot invocation order

        ASSERT_EQ(out.str(), "1_1_3_3_");
    }
#endif

    // 3. slot throws an exception
    {
        bsl::ostringstream      out(&alloc);
        bdlmt::Signaler<void()> sig(&alloc);

        // slot #1, prints "1_"
        sig.connect(Op(&out, "1_"));
        sig.connect(Op(&out, "2_"));

        // slot #3, throws

        sig.connect(u::ThrowOnCall());

        sig.connect(Op(&out, "4_"));


        bool exceptionThrown = false;

        // invoke the signaler
        try {
            sig();
        } catch(const u::ThrowOnCall::ExceptionType&) {
            exceptionThrown = true;
        }

        // exception thrown
        ASSERT(exceptionThrown);

        // the invocation sequence was interrupted
        ASSERT_EQ(out.str(), "1_2_");
    }

    // 4. use lvalue references in signaler's call signature
    {
        bdlmt::Signaler<void(int&, int&)> sig(&alloc);

        int x = 1;
        int y = 2;

        // connect a slot that swaps two values
        sig.connect(static_cast<void(*)(int&, int&)>(&bsl::swap));

        // invoke the signaler
        sig(x, y);

        // values swapped
        ASSERT_EQ(x, 2);
        ASSERT_EQ(y, 1);
    }
}

}  // close namespace test3_signaler

static void test4_signaler_connect()
    // ------------------------------------------------------------------------
    // SIGNALER CONNECT
    //
    // Concerns:
    //   Ensure proper behavior of the 'connect' method.
    //
    // Plan:
    //   1. Connect several slots to a signaler, obtaining an
    //      'bdlmt::SignalerConnection' object for each connected slot, Check
    //      that each connection object corresponds to its respective slot.
    //
    //   2. Connect to a signaler a slot that, when invoked, connects another
    //      slot to the same signaler. Call the signaler. Check that the first
    //      slot has successfully connected the second one.
    //
    //   3. Create a signaler. Connect slots #1, #2 and #3, given that slot #3
    //      throws on copy. Call the signaler. Check that:
    //      - No memory is leaked;
    //      - The signaler state has no changed since slot #2 was connected;
    //      - The signaler stays usable, i.e. can be called and accepts new
    //        slots.
    //
    // Testing:
    //   bdlmt::Signaler::connect()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    // 1. general case
    {
        bsl::ostringstream      out(&alloc);
        bdlmt::Signaler<void()> sig(&alloc);

        // connect the first slot, obtain a connection object
        bdlmt::SignalerConnection con1 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "1_"));

        // connect the second slot, obtain a connection object
        bdlmt::SignalerConnection con2 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "2_"));

        // check that the first connection object represents the first slot
        {
            out.str("");
            out.clear();

            sig();
            ASSERT_EQ(out.str(), "1_2_");

            out.str("");
            out.clear();

            con1.disconnect();
            sig();
            ASSERT_EQ(out.str(), "2_");
        }

        // check that the second connection object represents the second slot
        {
            out.str("");
            out.clear();

            sig();
            ASSERT_EQ(out.str(), "2_");

            out.str("");
            out.clear();

            con2.disconnect();
            sig();
            ASSERT_EQ(out.str(), "");
        }
    }

    // 2. connect one slot from another
    {
        bsl::ostringstream      out(&alloc);
        bdlmt::Signaler<void()> sig(&alloc);

        // this function object will connect a new slot to the signaler
        bsl::function<void()> printStr = bdlf::BindUtil::bindR<void>(
                                                         u::PrintStr1(),
                                                         bsl::ref(out),
                                                         "_X_");

        // connect a slot that connects another slot
        bdlmt::SignalerConnection con = sig.connect(
                                           bdlf::BindUtil::bindR<void>(
                                                           u::Connect(),
                                                           &sig,
                                                           printStr));

        // call the signaler
        sig();
        ASSERT_EQ(out.str(), "");

        ASSERTV(sig.slotCount(), 2 == sig.slotCount());

        // disconnect the original slot
        con.disconnect();

        ASSERTV(sig.slotCount(), 1 == sig.slotCount());

        // call the signaler again to check that a new slot was connected
        out.str("");
        out.clear();

        sig();
        ASSERT_EQ(out.str(), "_X_");

        ASSERTV(sig.slotCount(), 1 == sig.slotCount());

        // call the signaler again to check that a new slot was connected
        out.str("");
        out.clear();

        sig();
        ASSERT_EQ(out.str(), "_X_");

        ASSERTV(sig.slotCount(), 1 == sig.slotCount());
    }

    // 3. connect a slot that thows on copy
    {
        bsl::ostringstream      out(&alloc);
        bdlmt::Signaler<void()> sig(&alloc);

        // connect slot #1, prints "1_"
        bdlmt::SignalerConnection con1 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "1_"));

        // connect slot #2, prints "2_"
        bdlmt::SignalerConnection con2 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "2_"));

        // connect slot #3, throws on copy
        bool exceptionThrown = false;
        u::ThrowOnCopy         throwOnCopy;
        try {
            sig.connect(throwOnCopy);
        } catch(const u::ThrowOnCopy::ExceptionType&) {
            exceptionThrown = true;
        }

        // exception thrown
        ASSERT_EQ(exceptionThrown,    true);
        ASSERT("" == out.str());

        // the signaler state has not changed
        ASSERTV(sig.slotCount(), 2 == sig.slotCount());
        ASSERT(con1.isConnected());
        ASSERT(con2.isConnected());

        // connect a new slot, prints "3_"
        sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                bsl::ref(out),
                                                "3_"));

        // call the signaler
        sig();

        // it works
        ASSERT_EQ(out.str(), "1_2_3_");
    }
}

static void test5_signaler_disconnect()
    // ------------------------------------------------------------------------
    // SIGNALER DISCONNECT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnect' method.
    //
    // Plan:
    //   1. Create a signaler having no connected slots. Call 'disconnect()'
    //      specifying the group 0. Check that nothing happened.
    //
    //   2. Create a signaler. Connect several slots specifying different
    //      groups. Call 'disconnect()' specifying a group having slots in it.
    //      Check that:
    //      - All slots in the specified group were disconnected;
    //      - No other slots were disconnected.
    //
    //   3. Create a signaler. Connect several slots, given that one of them,
    //      when invoked, calls 'disconnect()' on the signaler instance
    //      specifying a group it (the slot) does not belong to. Invoke the
    //      signaler. Check that:
    //      - All slots in the specified group were disconnected;
    //      - No other slots were disconnected.
    //
    //   4. Create a signaler. Connect several slots, given that one of them,
    //      when invoked, calls 'disconnect()' on the signaler instance
    //      specifying the same group it (the slot) belongs to. Invoke the
    //      signaler. Check that:
    //      - All slots in the specified group were disconnected, including the
    //        slot that invoked 'disconnect()';
    //      - No other slots were disconnected.
    //
    // Testing:
    //   bdlmt::Signaler::disconnect()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    // 1. nothing to disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // disconnect all
        sig.disconnect(0);

        // nothing happened
        ASSERT_EQ(sig.slotCount(), 0u);
    }

    // 2. regular disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // connect slots with group '0'
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp(), 0);
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp(), 0);

        // connect slots with group '1'
        bdlmt::SignalerConnection con3 = sig.connect(u::NoOp(), 1);
        bdlmt::SignalerConnection con4 = sig.connect(u::NoOp(), 1);

        // connect slots with group '2'
        bdlmt::SignalerConnection con5 = sig.connect(u::NoOp(), 2);
        bdlmt::SignalerConnection con6 = sig.connect(u::NoOp(), 2);

        // disconnect slots in group '1'
        sig.disconnect(1);

        // slots in group '1' were disconnected
        ASSERT_EQ(sig.slotCount(),    4u);
        ASSERT_EQ(con3.isConnected(), false);
        ASSERT_EQ(con4.isConnected(), false);

        // slots in other groups were not disconnected
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con2.isConnected(), true);
        ASSERT_EQ(con5.isConnected(), true);
        ASSERT_EQ(con6.isConnected(), true);
    }

    // 3. disconnect a group from a slot in another group
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // connect slots with group '0'
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp(), 0);
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp(), 0);

        // connect slots with group '1'
        // the first slot in this group disconnects group '0'
        // the second slot in this group disconnects group '2'
        bdlmt::SignalerConnection con3 = sig.connect(
                                     bdlf::BindUtil::bind(
                                          &bdlmt::Signaler<void()>::disconnect,
                                          &sig,
                                          0),
                                     1);

        bdlmt::SignalerConnection con4 = sig.connect(
                                     bdlf::BindUtil::bind(
                                          &bdlmt::Signaler<void()>::disconnect,
                                          &sig,
                                          2),
                                     1);

        // connect slots with group '2'
        bdlmt::SignalerConnection con5 = sig.connect(u::NoOp(), 2);
        bdlmt::SignalerConnection con6 = sig.connect(u::NoOp(), 2);

        // call the signaler
        sig();

        // groups '0' and '2' were disconnected
        ASSERT_EQ(con1.isConnected(), false);
        ASSERT_EQ(con2.isConnected(), false);
        ASSERT_EQ(con5.isConnected(), false);
        ASSERT_EQ(con6.isConnected(), false);

        // group '1' was not disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con3.isConnected(), true);
        ASSERT_EQ(con4.isConnected(), true);
    }

    // 4. disconnect a group from a slot in the same group
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // connect slots with group '0'
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp(), 0);
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp(), 0);

        // connect slots with group '1'
        // the second slot disconnect group '1' (which it belongs to)
        bdlmt::SignalerConnection con3 = sig.connect(u::NoOp(), 1);

        bdlmt::SignalerConnection con4 = sig.connect(
                                     bdlf::BindUtil::bind(
                                          &bdlmt::Signaler<void()>::disconnect,
                                          &sig,
                                          1),
                                     1);

        // connect slots with group '2'
        bdlmt::SignalerConnection con5 = sig.connect(u::NoOp(), 2);
        bdlmt::SignalerConnection con6 = sig.connect(u::NoOp(), 2);

        // call the signaler
        sig();

        // group '1' was disconnected
        ASSERT_EQ(con3.isConnected(), false);
        ASSERT_EQ(con4.isConnected(), false);

        // groups '0' and '2' were not disconnected
        ASSERT_EQ(sig.slotCount(),    4u);
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con2.isConnected(), true);
        ASSERT_EQ(con5.isConnected(), true);
        ASSERT_EQ(con6.isConnected(), true);
    }
}

static void test6_signaler_disconnectAndWait()
    // ------------------------------------------------------------------------
    // SIGNALER DISCONNECT AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnectAndWait' method.
    //
    // Plan:
    //   1. Create a signaler having no connected slots. Call
    //      'disconnectAndWait()' specifying the group 0. Check that nothing
    //      happened.
    //
    //   2. Create a signaler. Connect several slots. Invoke the signaler from
    //      thread #1, and then again, from thread #2. At the same time call
    //      'disconnectAndWait()' from thread #0 (the main thread) specifying a
    //      group containing several slots. Check that:
    //      - All slot were in the specified group were disconnected;
    //      - No other slots were disconnected;
    //      - 'disconnectAllSlotsAndWait()' have blocked the calling thread
    //        pending completion of the currently executing slots.
    //
    // Testing:
    //   bdlmt::Signaler::disconnectAndWait()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::FixedThreadPool threadPool(2, 0x01FFFFFF, &alloc);

    // 1. nothing to disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // disconnect
        sig.disconnectAndWait(0);

        // nothing happened
        ASSERT_EQ(sig.slotCount(), 0u);
    }

    // 2. regular disconnect
    {
        typedef bdlcc::Deque<u::DoubleTI>              TimestampQueue;
        typedef bdlmt::Signaler<void(TimestampQueue*)> Sig;

        TimestampQueue tQueue1(&alloc); // modified from thread #1
        TimestampQueue tQueue2(&alloc); // mofified from thread #2
        Sig            sig(&alloc);     // invoked from threads #1 and #2

        // start the thread pool
        int rc = threadPool.start();
        BSLS_ASSERT_OPT(rc == 0);

        // connect a couple of no-op slots to group '1'
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp(), 1);
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp(), 1);

        for (int i = 0; i < 10; ++i) {
            // Repeat 10 times.

            // connect a couple of no-op slots to group '2',
            // the group we'll be disconnecting
            bdlmt::SignalerConnection con3 = sig.connect(u::NoOp(), 2);
            bdlmt::SignalerConnection con4 = sig.connect(u::NoOp(), 2);

            // Connect another slot to group '2'. Does the following:
            // 1. push starting timestamp
            // 2. sleep
            // 3. push completion timestamp
            bdlmt::SignalerConnection con5 = sig.connect(
                                               bdlf::BindUtil::bindR<void>(
                                                    u::SleepAndPushTimestamp(),
                                                    0.5, // sleep 500 ms
                                                    bdlf::PlaceHolders::_1),
                                               2);

            // invoke the signaler from thread #1
            threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                       &sig,
                                                       &tQueue1));

            // invoke the signaler from thread #2
            threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                       &sig,
                                                       &tQueue2));

            // wait till the target slot starts executing on both threads
            u::DoubleTI start1 = tQueue1.popFront();
            u::DoubleTI start2 = tQueue2.popFront();

            // disconnect group '2' from thread #0 (the main thread)
            sig.disconnectAndWait(2);

            // disconnection timestamp
            u::DoubleTI disconnectionTime = bsls::SystemTime::now(
                                           bsls::SystemClockType::e_MONOTONIC);

            // timestamp of target slot completion on thread #1
            u::DoubleTI completionTime1 = tQueue1.popFront();

            // timestamp of target slot completion on thread #2
            u::DoubleTI completionTime2 = tQueue2.popFront();

            ASSERT(0 == tQueue1.length());
            ASSERT(0 == tQueue2.length());

            const double elapsed1 = completionTime1 - start1;
            const double elapsed2 = completionTime2 - start2;
            ASSERTV(elapsed1, 0.4 < elapsed1);
            ASSERTV(elapsed2, 0.4 < elapsed2);

            // 'disconnectAndWait()' has blocked the calling thread
            const double diff1 = disconnectionTime - start1;
            ASSERTV(diff1, diff1 >= 0.4);
            const double diff2 = disconnectionTime - start2;
            ASSERTV(diff2, diff2 >= 0.4);

            double diffStart = start2 - start1;
            ASSERTV(diffStart, u::abs(diffStart) < 0.1);
            const double diffCompletion = completionTime2 - completionTime1;
            ASSERTV(diffCompletion, u::abs(diffCompletion) < 0.1);

            if (veryVerbose) {
                P_(elapsed1);    P(elapsed2);
                P_(diff1);       P(diff2);
                P_(diffStart);   P(diffCompletion);
            }

            // slots in group '2' were disconnected
            ASSERT_EQ(con3.isConnected(), false);
            ASSERT_EQ(con4.isConnected(), false);
            ASSERT_EQ(con5.isConnected(), false);
        }

        // slots in group '1' were not disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con2.isConnected(), true);

        // stop the thread pool
        threadPool.stop();
    }
}

static void test7_signaler_disconnectAllSlots()
    // ------------------------------------------------------------------------
    // SIGNALER DISCONNECT ALL SLOTS
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnectAllSlots' method.
    //
    // Plan:
    //   1. Create a signaler having no connected slots. Call
    //      'disconnectAllSlots()' and check that nothing happened.
    //
    //   2. Create a signaler. Connect several slots. Call
    //      'disconnectAllSlots()' and check that all slots were disconnected.
    //
    //   3. Create a signaler. Connect several slots, given that one of them,
    //      when invoked, calls 'disconnectAllSlots()' on the signaler
    //      instance. Invoke the signaler. Check that all slots were
    //      disconnected.
    //
    // Testing:
    //   bdlmt::Signaler::disconnectAllSlots()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    // 1. nothing to disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // disconnect all
        sig.disconnectAllSlots();

        // nothing happened
        ASSERT_EQ(sig.slotCount(), 0u);
    }

    // 2. regular disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // connect slots
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp());

        // disconnect all
        sig.disconnectAllSlots();

        // check
        ASSERT_EQ(sig.slotCount(),    0u);
        ASSERT_EQ(con1.isConnected(), false);
        ASSERT_EQ(con2.isConnected(), false);
    }

    // 3. disconnect from a slot
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // connect 3 slots, the seconds one calls 'disconnectAllSlots()'
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

        bdlmt::SignalerConnection con2 = sig.connect(
                             bdlf::BindUtil::bind(
                                  &bdlmt::Signaler<void()>::disconnectAllSlots,
                                  &sig));

        bdlmt::SignalerConnection con3 = sig.connect(u::NoOp());

        // invoke signaler
        sig();

        // all slots were disconnected
        ASSERT_EQ(sig.slotCount(),    0u);
        ASSERT_EQ(con1.isConnected(), false);
        ASSERT_EQ(con2.isConnected(), false);
        ASSERT_EQ(con3.isConnected(), false);
    }
}

static void test8_signaler_disconnectAllSlotsAndWait()
    // ------------------------------------------------------------------------
    // SIGNALER  DISCONNECT ALL SLOTS AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnectAllSlotsAndWait' method.
    //
    // Plan:
    //   1. Create a signaler having no connected slots. Call
    //      'disconnectAllSlotsAndWait()' and check that nothing happened.
    //
    //   2. Create a signaler. Connect several slots. Invoke the signaler from
    //      thread #1, and then again, from thread #2. At the same time call
    //      'disconnectAllSlotsAndWait()' from thread #0 (the main thread).
    //      Check that:
    //      - All slot were disconnected;
    //      - 'disconnectAllSlotsAndWait()' have blocked the calling thread
    //        pending completion of the currently executing slots.
    //
    // Testing:
    //   bdlmt::Signaler::disconnectAllSlotsAndWait()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::FixedThreadPool threadPool(2, 0x01FFFFFF, &alloc);

    // 1. nothing to disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // disconnect all
        sig.disconnectAllSlotsAndWait();

        // nothing happened
        ASSERT_EQ(sig.slotCount(), 0u);
    }

    // 2. regular disconnect
    {
        typedef bdlcc::Deque<u::DoubleTI>              TimestampQueue;
        typedef bdlmt::Signaler<void(TimestampQueue*)> Sig;

        TimestampQueue tQueue1(&alloc); // modified from thread #1
        TimestampQueue tQueue2(&alloc); // mofified from thread #2
        Sig            sig(&alloc);     // invoked from threads #1 and #2

        // start the thread pool
        int rc = threadPool.start();
        BSLS_ASSERT_OPT(rc == 0);

        for (int i = 0; i < 10; ++i) {
            // Repeat 10 times.

            // connect a couple of no-op slots
            bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
            bdlmt::SignalerConnection con2 = sig.connect(u::NoOp());

            // Connect another slot. Does the following:
            // 1. push starting timestamp
            // 2. sleep
            // 3. push completion timestamp
            bdlmt::SignalerConnection con3 = sig.connect(
                                              bdlf::BindUtil::bindR<void>(
                                                    u::SleepAndPushTimestamp(),
                                                    0.5, // sleep 500 ms
                                                    bdlf::PlaceHolders::_1));

            // invoke the signaler from thread #1
            threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                       &sig,
                                                       &tQueue1));

            // invoke the signaler from thread #2
            threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                       &sig,
                                                       &tQueue2));

            // wait till the target slot starts executing on both threads
            tQueue1.popFront();
            tQueue2.popFront();

            // disconnect group '2' from thread #0 (the main thread)
            sig.disconnectAllSlotsAndWait();

            // disconnection timestamp
            bsls::TimeInterval disconnectionTime = bsls::SystemTime::now(
                                           bsls::SystemClockType::e_MONOTONIC);

            // timestamp of target slot completion on thread #1
            u::DoubleTI completionTime1 = tQueue1.popFront();

            // timestamp of target slot completion on thread #2
            u::DoubleTI completionTime2 = tQueue2.popFront();

            // 'disconnectAllSlotsAndWait()' has blocked the calling thread
            ASSERT_EQ(disconnectionTime >= completionTime1, true);
            ASSERT_EQ(disconnectionTime >= completionTime2, true);

            // all slots were disconnected
            ASSERT_EQ(sig.slotCount(),    0u);
            ASSERT_EQ(con1.isConnected(), false);
            ASSERT_EQ(con2.isConnected(), false);
            ASSERT_EQ(con3.isConnected(), false);
        }

        // stop the thread pool
        threadPool.stop();
    }
}

static void test9_signaler_slotCount()
    // ------------------------------------------------------------------------
    // SIGNALER SLOTCOUNT
    //
    // Concerns:
    //   Ensure proper behavior of the 'slotCount' method.
    //
    // Plan:
    //   Create a signaler. Connect, and then, disconnect slots one by one.
    //   Check that 'slotCount()' always returns the right number of slots.
    //
    // Testing:
    //   bdlmt::Signaler::slotCount()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    ASSERT_EQ(sig.slotCount(), 0u);
    bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
    ASSERT_EQ(sig.slotCount(), 1u);
    bdlmt::SignalerConnection con2 = sig.connect(u::NoOp());
    ASSERT_EQ(sig.slotCount(), 2u);
    bdlmt::SignalerConnection con3 = sig.connect(u::NoOp());
    ASSERT_EQ(sig.slotCount(), 3u);

    con1.disconnect();
    ASSERT_EQ(sig.slotCount(), 2u);
    con2.disconnect();
    ASSERT_EQ(sig.slotCount(), 1u);
    con3.disconnect();
    ASSERT_EQ(sig.slotCount(), 0u);
}

static void test10_connection_creators()
    // ------------------------------------------------------------------------
    // CONNECTION CREATORS
    //
    // Concerns:
    //   Ensure proper behavior of creators methods.
    //
    // Plan:
    //   1. Default-construct an instance of 'bdlmt::SignalerConnection'. Check
    //      postconditions.
    //
    //   2. Copy-construct an instance of 'bdlmt::SignalerConnection'. Check
    //      postconditions.
    //
    //   3. Move-construct an instance of 'bdlmt::SignalerConnection'. Check
    //      postconditions.
    //
    //   4. Default-construct an instance of 'bdlmt::SignalerScopedConnection'.
    //      Check postconditions.
    //
    //   5. Move-construct an instance of 'bdlmt::SignalerScopedConnection'.
    //      Check postconditions.
    //
    //   6. Construct an instance of 'bdlmt::SignalerScopedConnection', from an
    //      instance of bdlmt::SignalerConnection. Check postconditions.
    //
    //   7. Construct an instance of 'bdlmt::SignalerScopedConnection', from an
    //      instance of 'bdlmt::SignalerConnection' by moving it. Check
    //      postconditions.
    //
    //   8. Construct an instance of 'bdlmt::SignalerScopedConnection', from an
    //      instance of 'bdlmt::SignalerConnection', destroy the instance of
    //      'bdlmt::SignalerScopedConnection' and check that 'disconnect()' was
    //      invoked.
    //
    // Testing:
    //   - bdlmt::SignalerConnection's default constructor
    //   - bdlmt::SignalerConnection's copy constructor
    //   - bdlmt::SignalerConnection's move constructor
    //   - bdlmt::SignalerScopedConnection's default constructor
    //   - bdlmt::SignalerScopedConnection's move constructor
    //   - bdlmt::SignalerScopedConnection's connection copying constructor
    //   - bdlmt::SignalerScopedConnection's connection moving constructor
    //   - bdlmt::SignalerScopedConnection's destructor
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    // 1. SignalerConnection default c-tor
    {
        bdlmt::SignalerConnection con;

        // the connection is "empty"
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);
    }

    // 2. SignalerConnection copy c-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

        // copy
        bdlmt::SignalerConnection con2(con1);

        // 'con2' reffers to the same slot as 'con1'
        ASSERT_EQ(con1.isConnected() && con2.isConnected(),   true);
        con1.disconnect();
        ASSERT_EQ(!con1.isConnected() && !con2.isConnected(), true);
    }

    // 3. SignalerConnection move c-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2(con1);

        // move
        bdlmt::SignalerConnection con3(bslmf::MovableRefUtil::move(con1));

        // 'con1' is now "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerConnection(), true);

        // 'con3' reffers to the same slot as 'con1' used to
        ASSERT_EQ(con2.isConnected() &&
                  con3.isConnected(),   true);
        con2.disconnect();
        ASSERT_EQ(!con2.isConnected() &&
                  !con3.isConnected(),   true);
    }

    // 4. SignalerScopedConnection default c-tor
    {
        bdlmt::SignalerScopedConnection con;

        // the connection is "empty"
        ASSERT_EQ(con == bdlmt::SignalerScopedConnection(), true);
    }

    // 5. SignalerScopedConnection move c-tor
    {
        bdlmt::Signaler<void()>         sig(&alloc);
        bdlmt::SignalerConnection       con0 = sig.connect(u::NoOp());
        bdlmt::SignalerScopedConnection con1(con0);
        bdlmt::SignalerScopedConnection con2(con0);

        // move
        bdlmt::SignalerScopedConnection con3(
                                       bslmf::MovableRefUtil::move(con1));

        // 'con1' is now "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerScopedConnection(), true);

        // 'con3' reffers to the same slot as 'con1' used to
        ASSERT_EQ(con2.isConnected() &&
                   con3.isConnected(),   true);
        con2.disconnect();
        ASSERT_EQ(!con2.isConnected() &&
                  !con3.isConnected(),   true);
    }

    // 6. SignalerScopedConnection connection copy c-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

        // copy
        bdlmt::SignalerScopedConnection con2(con1);

        // 'con2' reffers to the same slot as 'con1'
        ASSERT_EQ(con1.isConnected() && con2.isConnected(),   true);
        con1.disconnect();
        ASSERT_EQ(!con1.isConnected() && !con2.isConnected(), true);
    }

    // 7. SignalerScopedConnection connection move c-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2(con1);

        // move
        bdlmt::SignalerScopedConnection con3(
                                        bslmf::MovableRefUtil::move(con1));

        // 'con1' is now "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerConnection(), true);

        // 'con3' reffers to the same slot as 'con1' used to
        ASSERT_EQ(con2.isConnected() &&
                   con3.isConnected(),   true);
        con2.disconnect();
        ASSERT_EQ(!con2.isConnected() &&
                  !con3.isConnected(),   true);
    }

    // 8. SignalerScopedConnection d-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

        {
            bdlmt::SignalerScopedConnection con2(con1);
        }

        // 'con2's destructor invoked 'disconnect()'
        ASSERT_EQ(con1.isConnected(), false);
    }
}

static void test11_connection_assignment()
    // -----------------------------------------------------------------------
    // CONNECTION ASSIGNMENT
    //
    // Concerns:
    //   Ensure proper behavior of assignment operators.
    //
    // Plan:
    //   1. Copy-assign an instance of 'bdlmt::SignalerConnection'. Check
    //      postconditions.
    //
    //   2. Move-assign an instance of 'bdlmt::SignalerConnection'. Check
    //      postconditions.
    //
    //   3. Move-assign an instance of 'bdlmt::SignalerScopedConnection'. Check
    //      postconditions.
    //
    //   4. Copy-assign an instance of 'bdlmt::SignalerScopedConnection', from
    //      an instance of 'bdlmt::SignalerConnection'. Check postconditions.
    //
    //   5. Move-assign an instance of 'bdlmt::SignalerScopedConnection', from
    //      an instance of 'bdlmt::SignalerConnection'. Check postconditions.
    //
    // Testing:
    //   - bdlmt::SignalerConnection's copy assignment operator
    //   - bdlmt::SignalerConnection's move assignment operator
    //   - bdlmt::SignalerScopedConnection's move assignment operator
    //   - bdlmt::SignalerScopedConnection's connection copying assignment
    //     operator
    //   - bdlmt::SignalerScopedConnection's connection moving assignment
    //     operator
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    // 1. SignalerConnection copy assignment
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

        // copy-assign
        bdlmt::SignalerConnection con2;
                                 con2 = con1;

        // 'con2' reffers to the same slot as 'con1'
        ASSERT_EQ(con1.isConnected() && con2.isConnected(),   true);
        con1.disconnect();
        ASSERT_EQ(!con1.isConnected() && !con2.isConnected(), true);
    }

    // 2. SignalerConnection move assignment
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2(con1);

        // move-assign
        bdlmt::SignalerConnection con3;
                                 con3 = bslmf::MovableRefUtil::move(con1);

        // 'con1' is now "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerConnection(), true);

        // 'con3' reffers to the same slot as 'con1' used to
        ASSERT_EQ(con2.isConnected() &&
                  con3.isConnected(),   true);
        con2.disconnect();
        ASSERT_EQ(!con2.isConnected() &&
                  !con3.isConnected(),  true);
    }

    // 3. SignalerScopedConnection move assignment
    {
        bdlmt::Signaler<void()>         sig(&alloc);
        bdlmt::SignalerConnection       con0 = sig.connect(u::NoOp());
        bdlmt::SignalerScopedConnection con1(con0);
        bdlmt::SignalerScopedConnection con2(con0);

        // move-assign
        bdlmt::SignalerScopedConnection con3;
                                       con3 = bslmf::MovableRefUtil::move(
                                                                     con1);

        // 'con1' is now "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerScopedConnection(), true);

        // 'con3' reffer to the same slot as 'con1' used to
        ASSERT_EQ(con2.isConnected() &&
                   con3.isConnected(),   true);
        con2.disconnect();
        ASSERT_EQ(!con2.isConnected() &&
                  !con3.isConnected(),   true);
    }

    // 4. SignalerScopedConnection connection copy assignment
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

        // copy-assign
        bdlmt::SignalerScopedConnection con2;
                                       con2 = con1;

        // 'con2' reffer to the same slot as 'con1'
        ASSERT_EQ(con1.isConnected() && con2.isConnected(),   true);
        con1.disconnect();
        ASSERT_EQ(!con1.isConnected() && !con2.isConnected(), true);
    }

    // 5. SignalerScopedConnection connection move assignment
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2(con1);

        // move-assign
        bdlmt::SignalerScopedConnection con3;
                                       con3 = bslmf::MovableRefUtil::move(
                                                                     con1);

        // 'con1' is now "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerConnection(), true);

        // 'con3' reffer to the same slot as 'con1' used to
        ASSERT_EQ(con2.isConnected() &&
                   con3.isConnected(),   true);
        con2.disconnect();
        ASSERT_EQ(!con2.isConnected() &&
                  !con3.isConnected(),   true);
    }
}

static void test12_connection_disconnect()
    // -----------------------------------------------------------------------
    // CONNECTION DISCONNECT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnect' method.
    //
    // Plan:
    //   1. Default-construct an instance of 'bdlmt::SignalerConnection', 'c'.
    //      Call 'c.disconnect()'. Check that nothing happened to the
    //      connection object.
    //
    //   2. Create a connection object 'c' by connecting a slot to a signaler.
    //      Connect other slots to the same signaler. Call 'c.disconnect()'.
    //      Check that:
    //      - The slot associated with 'c' was disconnected;
    //      - 'c' is not reset to a default-constructed state;
    //      - No other slot was disconnected.
    //
    //   3. Connect slots #1, #2, #3 and #4 obtaining connection objects 'c1',
    //      'c2', 'c3' and 'c4' for each respective slot, given that slot #2,
    //      when invoked, disconnects the slot #1 via a call to
    //      'c1.disconnect()' and slot #3, when invoked, disconnects the slot
    //      #4 via a call to 'c4.disconnect()'. Call the signaler.
    //      Check that:
    //      - Slots #1 and #4 were disconnected;
    //      - Slots #2 and #4 were not disconnected;
    //
    //   4. Connect slots #1, #2 and #3, obtaining connection objects 'c1',
    //      'c2' and 'c3' for each respective slot, given that slot #2,
    //      when invoked, disconnects itself via a call to 'c2.disconnect()'.
    //      Call the signaler.
    //      Check that:
    //      - Slot #2 was disconnected;
    //      - Slots #1 and #3 were not disconnected;
    //      - Slots #1 and #3 were invoked.
    //
    // Testing:
    //   bdlmt::SignalerConnection::disconnect()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;

    // 1. disconnect "empty" connection
    {
        // create "empty" connection
        bdlmt::SignalerConnection con;
        ASSERT_EQ(con.isConnected(),                 false);
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);

        // do disconnect
        con.disconnect();

        // nothing happened
        ASSERT_EQ(con.isConnected(),                 false);
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);
    }

    // 2. regular disconnect
    {
        bsl::ostringstream      out(&alloc);
        bdlmt::Signaler<void()> sig(&alloc);

        // connect first slot, prints "1_", is disconnected
        bdlmt::SignalerConnection con1 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "1_"));

        // connect second slot, prints "2_"
        bdlmt::SignalerConnection con2 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "2_"));

        // connect third slot, prints "3_"
        bdlmt::SignalerConnection con3 = sig.connect(
                                            bdlf::BindUtil::bindR<void>(
                                                            u::PrintStr1(),
                                                            bsl::ref(out),
                                                            "3_"));

        ASSERT_EQ(sig.slotCount(), 3u);

        //disconnect first slot
        con1.disconnect();

        // first slot disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con1.isConnected(), false);

        // disconnected connection is not "empty"
        ASSERT_EQ(con1 == bdlmt::SignalerConnection(), false);

        // other two slots were not disconnected
        ASSERT_EQ(con2.isConnected(), true);
        ASSERT_EQ(con3.isConnected(), true);
        sig();
        ASSERT_EQ(out.str(), "2_3_");
    }

    // 3. disconnect one slot from another
    {
        bsl::ostringstream       out(&alloc);
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1, con2, con3, con4;

        // slot #1, prints "1_"
        con1 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "1_"));

        // slot #2, disconnects slot #1
        con2 = sig.connect(bdlf::BindUtil::bind(
                                 &bdlmt::SignalerConnection::disconnect,
                                 &con1));

        // slot #3, disconnects slot #4
        con3 = sig.connect(bdlf::BindUtil::bind(
                                 &bdlmt::SignalerConnection::disconnect,
                                 &con4));

        // slot #4, prints "4_"
        con4 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "4_"));

        ASSERT_EQ(sig.slotCount(), 4u);

        // call the signaler
        sig();

        // slots #1 and #4 disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con1.isConnected(), false);
        ASSERT_EQ(con4.isConnected(), false);

        // slots #2 and #3 still connected
        ASSERT_EQ(con2.isConnected(), true);
        ASSERT_EQ(con3.isConnected(), true);

        // slot #1 invoked
        ASSERT_EQ(out.str(), "1_");
    }

    // 4. disconnect a slot from itself
    {
        bsl::ostringstream       out(&alloc);
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1, con2, con3;

        // slot #1, prints "1_"
        con1 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "1_"));

        // slot #2, disconnects itself
        con2 = sig.connect(bdlf::BindUtil::bind(
                                 &bdlmt::SignalerConnection::disconnect,
                                 &con2));

        // slot #3, prints "3_"
        con3 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "3_"));

        ASSERT_EQ(sig.slotCount(), 3u);

        // call the signaler
        sig();

        // slot #2 was disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con2.isConnected(), false);

        // slots #1 and #3 are still connected
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con3.isConnected(), true);
    }
}

static void test13_connection_disconnectAndWait()
    // -----------------------------------------------------------------------
    // CONNECTION DISCONNECT AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnectAndWait' method.
    //
    // Plan:
    //   1. Default-construct an instance of 'bdlmt::SignalerConnection', 'c'.
    //      Call 'c.disconnectAndWait()'. Check that nothing happened to the
    //      connection object.
    //
    //   2. Create a connection object 'c' by connecting a slot to a signaler.
    //      Connect other slots to the same signaler. Invoke the signaler from
    //      thread #1 and thread #2. At the same time, call
    //      'c.disconnectAndWait()' from thread #0 (the main thread). Check
    //      that:
    //      - The slot associated with 'c' was disconnected;
    //      - 'c' is not reset to a default-constructed state;
    //      - No other slot was disconnected;
    //      - 'disconnectAndWait()' have blocked the calling thread (thread #0)
    //        pending completion of the disconnected slot.
    //
    //   3. Connect slots #1 and #2 obtaining connection objects 'c1' and 'c2'
    //      for each respective slot, given that slot #1, when invoked,
    //      (conditionally) disconnects the slot #2 via a call to
    //      'c2.disconnectAndWait()'. "Disable" slot #1 and call the signaler
    //      from thread #1. Then, "enable" slot #1 and call the signaler from
    //      thread #0 (the main thread).
    //      Check that:
    //      - Slot #2 was disconnected;
    //      - 'disconnectAndWait()' have blocked the calling thread (thread #0)
    //        pending completion of the disconnected slot (slot #2).
    //
    // Testing:
    //   bdlmt::SignalerConnection::disconnectAndWait()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::FixedThreadPool threadPool(2, 0x01FFFFFF, &alloc);

    // 1. disconnect "empty" connection
    {
        // create "empty" connection
        bdlmt::SignalerConnection con;
        ASSERT_EQ(con.isConnected(),                  false);
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);

        // do disconnect
        con.disconnectAndWait();

        // nothing happened
        ASSERT_EQ(con.isConnected(),                  false);
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);
    }

    // 2. regular disconnect
    {
        typedef bdlcc::Deque<u::DoubleTI>              TimestampQueue;
        typedef bdlmt::Signaler<void(TimestampQueue*)> Sig;

        TimestampQueue tQueue1(&alloc); // modified from thread #1
        TimestampQueue tQueue2(&alloc); // mofified from thread #2
        Sig            sig(&alloc);     // invoked from threads #1 and #2

        // start the thread pool
        int rc = threadPool.start();
        BSLS_ASSERT_OPT(rc == 0);

        // connect a couple of no-op slots
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp());

        for (int i = 0; i < 10; ++i) {
            // Repeat 10 times.

            // Connect the slot we'll be disconnecting. Does the following:
            // 1. push starting timestamp
            // 2. sleep
            // 3. push completion timestamp
            bdlmt::SignalerConnection con3 = sig.connect(
                                              bdlf::BindUtil::bindR<void>(
                                                    u::SleepAndPushTimestamp(),
                                                    0.5, // sleep 500 ms
                                                    bdlf::PlaceHolders::_1));

            // invoke the signaler from thread #1
            threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                       &sig,
                                                       &tQueue1));

            // invoke the signaler from thread #2
            threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                       &sig,
                                                       &tQueue2));

            // wait till the target slot starts executing on both threads
            tQueue1.popFront();
            tQueue2.popFront();

            // disconnect the target slot from thread #0 (the main thread)
            con3.disconnectAndWait();

            // disconnection timestamp
            u::DoubleTI disconnectionTime = bsls::SystemTime::now(
                                           bsls::SystemClockType::e_MONOTONIC);

            // timestamp of target slot completion on thread #1
            u::DoubleTI completionTime1 = tQueue1.popFront();

            // timestamp of target slot completion on thread #2
            u::DoubleTI completionTime2 = tQueue2.popFront();

            // 'disconnectAndWait()' has blocked the calling thread
            ASSERT_EQ(disconnectionTime >= completionTime1, true);
            ASSERT_EQ(disconnectionTime >= completionTime2, true);

            // target slot was disconnected
            ASSERT_EQ(con3.isConnected(), false);

            // target slots connection is not "empty"
            ASSERT_EQ(con3 == bdlmt::SignalerConnection(), false);
        }

        // other slots were not disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con2.isConnected(), true);

        // stop the thread pool
        threadPool.stop();
    }

    // 3. disconnect one slot from another
    {
        typedef bdlcc::Deque<u::DoubleTI>  TimestampQueue;
        typedef bdlmt::Signaler<void()>    Sig;

        TimestampQueue            tQueue(&alloc); // modified from thread #1
        Sig                       sig(&alloc);    // invoked from threads #0
        bdlmt::SignalerConnection con1, con2;
        bsls::AtomicBool          slot1Enabled;

        // start the thread pool
        int rc = threadPool.start();
        BSLS_ASSERT_OPT(rc == 0);

        for (int i = 0; i < 10; ++i) {
            // Repeat 10 times.

            // slot #1, disconnects slot #2 if 'slot1Enabled' is 'true'
            con1 = sig.connect(bdlf::BindUtil::bindR<void>(
                                                    u::CondDisconnectAndWait(),
                                                    &con2,
                                                    &slot1Enabled));

            // Connect slot #2 (the slot we'll be disconnecting). Does the
            // following:
            // 1. push starting timestamp
            // 2. sleep
            // 3. push completion timestamp
            con2 = sig.connect(bdlf::BindUtil::bindR<void>(
                                                    u::SleepAndPushTimestamp(),
                                                    0.5, // sleep 500 ms
                                                    &tQueue));

            // disable slot #1 and call the signaler from thread #1
            slot1Enabled = false;
            threadPool.enqueueJob(bdlf::MemFnUtil::memFn(
                                          &bdlmt::Signaler<void()>::operator(),
                                          &sig));

            // wait till slot #2 starts executing
            tQueue.popFront();

            // enable slot #1 and call the signaler from thread #2 (this
            // thread)
            slot1Enabled = true;
            sig();

            // disconnection timestamp
            u::DoubleTI disconnectionTime = bsls::SystemTime::now(
                                           bsls::SystemClockType::e_MONOTONIC);

            // timestamp of target slot completion on thread #1
            u::DoubleTI completionTime = tQueue.popFront();

            // 'disconnectAndWait()' has blocked the calling thread
            ASSERT_EQ(disconnectionTime >= completionTime, true);

            // slot #2 disconnected
            ASSERT_EQ(con2.isConnected(), false);
        }

        // stop the thread pool
        threadPool.stop();
    }
}

static void test14_connection_release()
    // ------------------------------------------------------------------------
    // CONNECTION RELEASE
    //
    // Concerns:
    //   Ensure proper behavior of the 'release' method.
    //
    // Plan:
    //   Obtain a scoped connection object by connecting a slot to a signaler.
    //   Release that connection. Check that the connection object is reset to
    //   a default-constructed state. Then, destroy the connection object and
    //   check that the slot was not disconnected.
    //
    // Testing:
    //   bdlmt::SignalerScopedConnection::release()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // connect a slot
    bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

    {
        // create a scoped connection
        bdlmt::SignalerScopedConnection con2(con1);
        ASSERT_EQ(con2 == bdlmt::SignalerScopedConnection(), false);

        // release it
        con2.release();

        // the connection is now "empty"
        ASSERT_EQ(con2 == bdlmt::SignalerScopedConnection(), true);

        // destroy the connection ...
    }

    // the slot was not disconnected
    ASSERT_EQ(con1.isConnected(), true);
}

static void test15_connection_swap()
    // ------------------------------------------------------------------------
    // CONNECTION SWAP
    //
    // Concerns:
    //   Ensure proper behavior of the 'swap' method.
    //
    // Plan:
    //   1. Create two instance of 'bdlmt::SignalerConnection', swap them.
    //      Check that they were swapped.
    //
    //   2. Create two instance of 'bdlmt::SignalerScopedConnection', swap
    //      them. Check that they were swapped.
    //
    // Testing:
    //   - bdlmt::SignalerConnection::swap()
    //   - bdlmt::SignalerScopedConnection::swap()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // 1. swap two SignalerConnection's
    {
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con3 = con1;
        bdlmt::SignalerConnection con4 = con2;

        // swap
        con1.swap(con2);

        // check
        ASSERT_EQ(con1.isConnected() &&
                  con2.isConnected() &&
                  con3.isConnected() &&
                  con4.isConnected(),   true);
        con4.disconnect();
        ASSERT_EQ(!con1.isConnected() &&
                   con2.isConnected() &&
                   con3.isConnected() &&
                  !con4.isConnected(),   true);
        con3.disconnect();
        ASSERT_EQ(!con1.isConnected() &&
                  !con2.isConnected() &&
                  !con3.isConnected() &&
                  !con4.isConnected(),   true);
    }

    // 2. swap two SignalerScopedConnection's
    {
        bdlmt::SignalerScopedConnection con1(sig.connect(u::NoOp()));
        bdlmt::SignalerScopedConnection con2(sig.connect(u::NoOp()));
        bdlmt::SignalerConnection       con3(con1);
        bdlmt::SignalerConnection       con4(con2);

        // swap
        con1.swap(con2);

        // check
        ASSERT_EQ(con1.isConnected() &&
                  con2.isConnected() &&
                  con3.isConnected() &&
                  con4.isConnected(),   true);
        con4.disconnect();
        ASSERT_EQ(!con1.isConnected() &&
                   con2.isConnected() &&
                   con3.isConnected() &&
                  !con4.isConnected(),   true);
        con3.disconnect();
        ASSERT_EQ(!con1.isConnected() &&
                  !con2.isConnected() &&
                  !con3.isConnected() &&
                  !con4.isConnected(),   true);
    }
}

static void test16_connection_isConnected()
    // ------------------------------------------------------------------------
    // CONNECTION IS CONNECTED
    //
    // Concerns:
    //   Ensure proper behavior of the 'isConnected' method.
    //
    // Plan:
    //   1. Default-construct an instance of 'bdlmt::SignalerConnection'. Check
    //      that 'isConnected()' returns false.
    //
    //   2. Obtain an instance of 'bdlmt::SignalerConnection' by connecting a
    //      slot to a signaler. Check that 'isConnected()' returns true, then
    //      disconnect the slot and check that 'isConnected()' now returns
    //      false.
    //
    // Testing:
    //   bdlmt::SignalerConnection::isConnected()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // 1. default-constructed connection
    bdlmt::SignalerConnection con;
    ASSERT_EQ(con.isConnected(), false);

    // 2. connection obtained by connecting a slot
    con = sig.connect(u::NoOp());
    ASSERT_EQ(con.isConnected(), true);
    con.disconnect();
    ASSERT_EQ(con.isConnected(), false);
}

static void test17_usageExample()
    // ------------------------------------------------------------------------
    // USAGE EXAMPLE
    //
    // Concerns:
    //   Test that the usage example provided in the documentation of the
    //   component is correct.
    //
    // Plan:
    //   Implement the usage example.
    //
    // Testing:
    //   Usage example
    // ------------------------------------------------------------------------
{
    // create a button and subscribe to its events
    u::Button                 button;
    bdlmt::SignalerConnection connection = button.onClickConnect(
                                                         &u::printCoordinates);

    // "click" the button
    button.click(100, 200);

    // disconnect
    connection.disconnect();
}


// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test       = argc > 1 ? atoi(argv[1]) : 0;
    verbose         = argc > 2;    (void) verbose;
    veryVerbose     = argc > 3;    (void) veryVerbose;
    veryVeryVerbose = argc > 4;

    // Install an assert handler to 'gracefully' mark the test as failure
    // in case of assert.

    // Global Allocator
    bslma::TestAllocator        _ga("global", veryVeryVerbose);
    bslma::TestAllocatorMonitor _gam(&_ga);
    bslma::Default::setGlobalAllocator(&_ga);

    // Default allocator
    bslma::TestAllocator         _da("default", veryVeryVerbose);
    bslma::TestAllocatorMonitor  _dam(&_da);
    bslma::DefaultAllocatorGuard _defaultAllocatorGuard(&_da);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // Prevent against compiler warning:
    // error: unused function template 'operator()' [-Werror,-Wunused-template]

    u::NoOp()(1);
    u::NoOp()(1, 2);
    u::NoOp()(1, 2, 3);
    u::NoOp()(1, 2, 3, 4);
    u::NoOp()(1, 2, 3, 4, 5);

    switch (test) {
      case  0:
      case  1:  test1_signaler_defaultConstructor();        break;
      case  2:  test2_signaler_destructor();                break;
      case  3:  test3_signaler::callOperator();             break;
      case  4:  test4_signaler_connect();                   break;
      case  5:  test5_signaler_disconnect();                break;
      case  6:  test6_signaler_disconnectAndWait();         break;
      case  7:  test7_signaler_disconnectAllSlots();        break;
      case  8:  test8_signaler_disconnectAllSlotsAndWait(); break;
      case  9:  test9_signaler_slotCount();                 break;
      case  10: test10_connection_creators();               break;
      case  11: test11_connection_assignment();             break;
      case  12: test12_connection_disconnect();             break;
      case  13: test13_connection_disconnectAndWait();      break;
      case  14: test14_connection_release();                break;
      case  15: test15_connection_swap();                   break;
      case  16: test16_connection_isConnected();            break;
      case  17: test17_usageExample();                      break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;

        return -1;                                                    // RETURN
      } break;
    }

//  // Verify no default allocator usage
//
//  if (!s_ignoreCheckDefAlloc) {
//      ASSERT(_dam.isTotalSame());
//  }

    // Verify no global allocator usage

    if (ignoreCheckGblAlloc) {
        ASSERT(_gam.isTotalSame());
    }

    // Check test result

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2018
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
