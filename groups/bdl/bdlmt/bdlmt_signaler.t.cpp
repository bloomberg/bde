// bdlmt_signaler.t.cpp                                               -*-C++-*-
#include <bdlmt_signaler.h>

#include <bdlmt_threadpool.h>

#include <bdlcc_deque.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlt_datetime.h>
#include <bdlt_currenttime.h>

#include <bdlsb_memoutstreambuf.h>

#include <bdlma_sequentialallocator.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>

#include <bslmt_threadutil.h>

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_buildtarget.h>
#include <bsls_objectbuffer.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

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
// [ 1] Signaler::Signaler
// [ 2] Signaler::~Signaler
// [ 3] Signaler::operator()
// [ 4] Signaler::connect
// [ 5] Signaler::disconnectGroup
// [ 6] Signaler::disconnectGroupAndWait
// [ 7] Signaler::disconnectAllSlots
// [ 8] Signaler::disconnectAllSlotsAndWait
// [ 9] Signaler::slotCount
// [10] SignalerConnection::creators
// [11] SignalerConnection::assignment
// [12] SignalerConnection::disconnect
// [13] SignalerConnection::disconnectAndWait
// [14] SignalerConnection::reset
// [15] SignalerConnection::swap
// [16] SignalerConnection::isConnected
// [17] SignalerConnection bitwise moveability
// [18] SignalerConnectionGuard::creators
// [19] SignalerConnectionGuard::assignment
// [20] SignalerConnectionGuard::release
// [21] SignalerConnectionGuard::swap
// [22] SignalerConnectionGuard bitwise moveability
// [23] operator()(T1&, T2&, ..., T9&)
// [24] Usage example
// ----------------------------------------------------------------------------

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

static bslmt::ThreadAttributes attr;


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
    void operator()() const BSLS_KEYWORD_NOEXCEPT
    { }

    template <class A1>
    void
    operator()(const A1&) const BSLS_KEYWORD_NOEXCEPT
    { }

    template <class A1,
              class A2>
    void
    operator()(const A1&,
               const A2&) const BSLS_KEYWORD_NOEXCEPT
    { }

    template <class A1,
              class A2,
              class A3>
    void
    operator()(const A1&,
               const A2&,
               const A3&) const BSLS_KEYWORD_NOEXCEPT
    { }

    template <class A1,
              class A2,
              class A3,
              class A4>
    void
    operator()(const A1&,
               const A2&,
               const A3&,
               const A4&) const BSLS_KEYWORD_NOEXCEPT
    { }

    template <class A1,
              class A2,
              class A3,
              class A4,
              class A5>
    void
    operator()(const A1&,
               const A2&,
               const A3&,
               const A4&,
               const A5&) const BSLS_KEYWORD_NOEXCEPT
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
    // This functional object is used to test the waiting behavior of waiting
    // disconnect functions.
    //
    // It's call operator accepts an output synchronizable queue of
    // 'bsls::TimeInterval' and the number of seconds to sleep.  When invoked,
    // does the following:
    //: 1 push the current timestamp to the queue (start timestamp)
    //:
    //: 2 sleeps for the specified number of seconds
    //:
    //: 3 push the current timestamp to the queue (end timestamp)

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
//..
// struct CondCall {
//     // Conditionally call a specified signaler. Before calling, set the
//     // condition to 'false'.
//
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
//..

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
            connection->disconnect(true);
        }
    }
};

#if defined(BDE_BUILD_TARGET_EXC)

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
#endif   // defined(BDE_BUILD_TARGET_EXC)

                                // -----
                                // Usage
                                // -----

///Usage
///-----
// Suppose we want to implement a GUI button class that allows users to
// keep track of its 'press' events.
//
// First, we declare the 'class':
//..
    class Button {
        // A pretend GUI button.

        // DATA
        int d_numPresses;

      public:
        // TYPES
        typedef bsl::function<void(int)> OnPressSlotType;
            // Slot argument is the number of times the button has been
            // pressed.

      private:
        // PRIVATE DATA
        bdlmt::Signaler<void(int)> d_onPress;
            // Signaler argument is the number of times the button has been
            // pressed.

      public:
        // CREATORS
        Button();
            // Construct a 'Button' object.

        // MANIPULATORS
        bdlmt::SignalerConnection onPressConnect(const OnPressSlotType& slot);
            // Connect the specified 'slot' to this button.

        void press();
            // Simulate user pressing on GUI button.
    };
//..
// Then, we define its methods:
//..
    // CREATORS
    Button::Button()
    : d_numPresses(0)
    {
    }

    // MANIPULATORS
    bdlmt::SignalerConnection Button::onPressConnect(
                                                   const OnPressSlotType& slot)
    {
        return d_onPress.connect(slot);
    }

    void Button::press()
    {
        d_onPress(++d_numPresses);
    }
//..
// Next, we provide an event handler callback printing its argument, which the
// class will pass the number of times the button has been pressed:
//..
    void showPresses(int numPresses)
    {
        bsl::cout << "Button pressed " << numPresses << " times.\n";
    }
//..

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
    // SIGNALER DESTRUCTOR
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
    //   2. Create a signaler. Connect slots #1, #2, #3 and #4, given that slot
    //      #3, when invoked, throws an exception. Call the signaler. Check
    //      that:
    //      - The exception thrown by slot #3 is propagated to the signaler's
    //        caller;
    //      - The slot invocation sequence is interrupted after the call to
    //        slot #3, i.e. only slots #1, #2 and #3 are invoked.
    //
    //   3. Check that the call operator correctly forwards lvalue references.
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

#if defined(BDE_BUILD_TARGET_EXC)
    // 2. slot throws an exception
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
#endif   // defined(BDE_BUILD_TARGET_EXC)

    // 3. use lvalue references in signaler's call signature
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

    if (veryVerbose) cout << "1. general case\n";
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

    if (veryVerbose) cout << "2. connect one slot from another\n";
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

#if defined(BDE_BUILD_TARGET_EXC)
# if defined(BSLS_PLATFORM_CMP_IBM) && defined(BSLS_PLATFORM_CPU_32_BIT)
    const int major = (0xff00 & BSLS_PLATFORM_CMP_VERSION) >> 8;
    const int minor =  0x00ff & BSLS_PLATFORM_CMP_VERSION;

    const bdlt::Datetime now = bdlt::CurrentTime::local();

    if (major < 16 || (16 == major && minor <= 1)) {
        // There is a compiler bug on Aix {DRQS 166134166<GO>} that causes the
        // 'this' pointer to be corrupted when a base class d'tor is called if
        // the derived class c'tor throws.

        // We want to discontinue this workaround once we get to xlC_r 16.2 or
        // later.

        cout << "Quitting before '3' to avoid xlC_r known compiler bug.\n" <<
                                            "see {DRQS 166134166<GO>}" << endl;

        return;       // Quit test.
    }
    else {
        // We want this workaround to be removed once it has outlived its
        // usefulness, and if the following tests fails, we want attention
        // drawn to the fact that it's a known compiler bug.

        ASSERT(0 && "Clean me up after xlC_r 16.1 or sooner.");
    }
# endif

    if (veryVerbose) cout << "3. connect a slot that throws on copy\n";
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
#endif   // defined(BDE_BUILD_TARGET_EXC)
}

namespace test5_signaler {

struct GroupDisconnector {
    // DATA
    bdlmt::Signaler<void()> *d_signaler_p;
    int                      d_group;

    // CREATOR
    GroupDisconnector(bdlmt::Signaler<void()> *signaler_p,
                      int                      group)
    : d_signaler_p(signaler_p)
    , d_group(group)
    {}

    // ACCESSOR
    void operator()() const
    {
        d_signaler_p->disconnectGroup(d_group);
    }
};

static void disconnectGroup()
    // ------------------------------------------------------------------------
    // SIGNALER DISCONNECT GROUP
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
        sig.disconnectGroup(0);

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
        sig.disconnectGroup(1);

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
        bdlmt::SignalerConnection con3 = sig.connect(GroupDisconnector(&sig,
                                                                       0),
                                                     1);

        bdlmt::SignalerConnection con4 = sig.connect(GroupDisconnector(&sig,
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

        bdlmt::SignalerConnection con4 = sig.connect(GroupDisconnector(&sig,
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

}  // close namespace test5_signaler

static void test6_signaler_disconnectGroupAndWait()
    // ------------------------------------------------------------------------
    // SIGNALER DISCONNECT GROUP AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnectGroupAndWait' method.
    //
    // Plan:
    //: 1 Create a signaler having no connected slots.  Call
    //:   'disconnectGroupAndWait()' specifying the group 0.  Check that
    //:   nothing happened.
    //:
    //: 2 Create a signaler.  Connect several slots.  Invoke the signaler from
    //:   thread #1, and then again, from thread #2.  At the same time call
    //:   'disconnectGroupAndWait()' from thread #0 (the main thread)
    //:   specifying a group containing several slots.  Check that:
    //:   o All slots in the specified group were disconnected;
    //:   o No other slots were disconnected;
    //
    // Testing:
    //   bdlmt::Signaler::disconnectGroup
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::ThreadPool      threadPool(attr, 2, 2, 1000, &alloc);

    // 1. nothing to disconnect
    {
        bdlmt::Signaler<void()> sig(&alloc);

        // disconnect
        sig.disconnectGroupAndWait(0);

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
            sig.disconnectGroupAndWait(2);

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

            // 'disconnectGroup(*)' has blocked the calling thread
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

namespace test7_signaler {

struct AllSlotsDisconnector {
    // DATA
    bdlmt::Signaler<void()> *d_signaler_p;

    // CREATOR
    explicit
    AllSlotsDisconnector(bdlmt::Signaler<void()> *signaler_p)
    : d_signaler_p(signaler_p)
    {}

    // ACCESSOR
    void operator()() const
    {
        d_signaler_p->disconnectAllSlots();
    }
};

static void disconnectAllSlots()
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

        ASSERT_EQ(sig.slotCount(),    2u);

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

        bdlmt::SignalerConnection con2 = sig.connect(AllSlotsDisconnector(
                                                                        &sig));

        bdlmt::SignalerConnection con3 = sig.connect(u::NoOp());

        ASSERT_EQ(sig.slotCount(),    3u);

        // invoke signaler
        sig();

        // all slots were disconnected
        ASSERT_EQ(sig.slotCount(),    0u);
        ASSERT_EQ(con1.isConnected(), false);
        ASSERT_EQ(con2.isConnected(), false);
        ASSERT_EQ(con3.isConnected(), false);
    }
}

}  // close namespace test7_signaler

static void test8_signaler_disconnectAllSlotsAndWait()
    // ------------------------------------------------------------------------
    // SIGNALER  DISCONNECT ALL SLOTS AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnectAllSlots(true)' method.
    //
    // Plan:
    //   1. Create a signaler having no connected slots. Call
    //      'disconnectAllSlots(true)' and check that nothing happened.
    //
    //   2. Create a signaler. Connect several slots. Invoke the signaler from
    //      thread #1, and then again, from thread #2. At the same time call
    //      'disconnectAllSlots(true)' from thread #0 (the main thread).
    //      Check that:
    //      - All slot were disconnected;
    //      - 'disconnectAllSlots(true)' have blocked the calling thread
    //        pending completion of the currently executing slots.
    //
    // Testing:
    //   bdlmt::Signaler::disconnectAllSlotsAndWait()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::ThreadPool      threadPool(attr, 2, 2, 1000, &alloc);

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
    // SIGNALER SLOT COUNT
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
    //: 1 Default-construct an instance of 'bdlmt::SignalerConnection'.  Check
    //:   postconditions.
    //:
    //: 2 Copy-construct an instance of 'bdlmt::SignalerConnection'.  Check
    //:   postconditions.
    //:
    //: 3 Move-construct an instance of 'bdlmt::SignalerConnection'.  Check
    //:   postconditions.
    //
    // Testing:
    //   - bdlmt::SignalerConnection's default constructor
    //   - bdlmt::SignalerConnection's copy constructor
    //   - bdlmt::SignalerConnection's move constructor
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;
    const bdlmt::SignalerConnection def;

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
}

static void test11_connection_assignment()
    // -----------------------------------------------------------------------
    // CONNECTION ASSIGNMENT
    //
    // Concerns:
    //   Ensure proper behavior of assignment operators.
    //
    // Plan:
    //: 1 Copy-assign an instance of 'bdlmt::SignalerConnection'.  Check
    //:   postconditions.
    //:
    //: 2 Move-assign an instance of 'bdlmt::SignalerConnection'.  Check
    //:   postconditions.
    //
    // Testing:
    //   - bdlmt::SignalerConnection's copy assignment operator
    //   - bdlmt::SignalerConnection's move assignment operator
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;
    const bdlmt::SignalerConnection def;

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
}

namespace test12 {

struct ConnectionDisconnector {
    // DATA
    bdlmt::SignalerConnection *d_connection;

    // CREATOR
    explicit
    ConnectionDisconnector(bdlmt::SignalerConnection *connection)
    : d_connection(connection)
    {}

    // ACCESSOR
    void operator()() const
    {
        d_connection->disconnect();
    }
};

static void connection_disconnect()
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
    bslma::TestAllocator            alloc;
    const bdlmt::SignalerConnection def;

    // 1. disconnect "empty" connection
    {
        // create "empty" connection
        bdlmt::SignalerConnection con;
        ASSERT_EQ(con.isConnected(),                  false);
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);

        // do disconnect
        con.disconnect();

        // nothing happened
        ASSERT_EQ(con.isConnected(),                  false);
        ASSERT_EQ(con == bdlmt::SignalerConnection(), true);
        ASSERT(def == con);
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
        ASSERT(def != con1);

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
        bsl::ostringstream             out(&alloc);
        bdlmt::Signaler<void()>        sig(&alloc);
        bdlmt::SignalerConnection      con1, con2, con3, con4;

        // slot #1, prints "1_"
        con1 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "1_"));

        // slot #2, disconnects slot #1
        con2 = sig.connect(ConnectionDisconnector(&con1));

        // slot #3, disconnects slot #4
        con3 = sig.connect(ConnectionDisconnector(&con4));

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
        ASSERT(def != con1);
        ASSERT(def != con4);

        // slots #2 and #3 still connected
        ASSERT_EQ(con2.isConnected(), true);
        ASSERT_EQ(con3.isConnected(), true);

        // slot #1 invoked
        ASSERT_EQ(out.str(), "1_");
    }

    // 4. disconnect a slot from itself
    {
        bsl::ostringstream             out(&alloc);
        bdlmt::Signaler<void()>        sig(&alloc);
        bdlmt::SignalerConnection      con1, con2, con3, con4;

        // slot #1, prints "1_"
        con1 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "1_"));

        // slot #2, disconnects itself
        con2 = sig.connect(ConnectionDisconnector(&con2));

        // slot #3, prints "3_"
        con3 = sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                       bsl::ref(out),
                                                       "3_"));

        // slot #2, disconnects itself
        con4 = sig.connect(ConnectionDisconnector(&con4));

        {
            // slot #5, prints "5_"
            bdlmt::SignalerConnectionGuard con5(
                        sig.connect(bdlf::BindUtil::bindR<void>(u::PrintStr1(),
                                                                bsl::ref(out),
                                                                "5_")));

            ASSERT(5 == sig.slotCount());

            // call the signaler
            sig();

            // slot #2 was disconnected
            ASSERT_EQ(sig.slotCount(),    3u);
            ASSERT_EQ(con2.isConnected(), false);
            ASSERT_EQ(con4.isConnected(), false);
            ASSERT(def != con2);
            ASSERT(def != con4);

            // slots #1, #3, and #5 are still connected
            ASSERT_EQ(con1.isConnected(), true);
            ASSERT_EQ(con3.isConnected(), true);
            ASSERT_EQ(con5.connection().isConnected(), true);
            ASSERT(!con5.waitOnDisconnect());

            ASSERTV(out.str(), "1_3_5_" == out.str());

            ASSERT(3 == sig.slotCount());

            out.str("");
        }

        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con3.isConnected(), true);
        ASSERT(2 == sig.slotCount());

        // call the signaler
        sig();
        ASSERTV(out.str(), "1_3_" == out.str());
    }
}

}  // close namespace test12

static void test13_connection_disconnectAndWait()
{
    // -----------------------------------------------------------------------
    // CONNECTION DISCONNECT AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'disconnect(true)' method.
    //
    // Plan:
    //   1. Default-construct an instance of 'bdlmt::SignalerConnection', 'c'.
    //      Call 'c.disconnect(true)'. Check that nothing happened to the
    //      connection object.
    //
    //   2. Create a connection object 'c' by connecting a slot to a signaler.
    //      Connect other slots to the same signaler. Invoke the signaler from
    //      thread #1 and thread #2. At the same time, call
    //      'c.disconnect(true)' from thread #0 (the main thread). Check that:
    //      - The slot associated with 'c' was disconnected;
    //      - 'c' is not reset to a default-constructed state;
    //      - No other slot was disconnected;
    //      - 'disconnect(true)' have blocked the calling thread (thread #0)
    //        pending completion of the disconnected slot.
    //
    // Testing:
    //   bdlmt::SignalerConnection::disconnectAndWait()
    // ------------------------------------------------------------------------

    bslma::TestAllocator   alloc;
    bdlmt::ThreadPool      threadPool(attr, 2, 2, 1000, &alloc);

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
            u::DoubleTI start1 = tQueue1.popFront();
            u::DoubleTI start2 = tQueue2.popFront();

            // disconnect the target slot from thread #0 (the main thread)
            con3.disconnectAndWait();

            // disconnection timestamp
            u::DoubleTI disconnectionTime = bsls::SystemTime::now(
                                           bsls::SystemClockType::e_MONOTONIC);

            // timestamp of target slot completion on thread #1
            u::DoubleTI completionTime1 = tQueue1.popFront();

            // timestamp of target slot completion on thread #2
            u::DoubleTI completionTime2 = tQueue2.popFront();

            // 'disconnect(true)' has blocked the calling thread
            ASSERT_EQ(disconnectionTime >= completionTime1, true);
            ASSERT_EQ(disconnectionTime >= completionTime2, true);

            double diff1 = disconnectionTime - start1;
            double diff2 = disconnectionTime - start2;

            ASSERTV(diff1, diff1 >= 0.5);
            ASSERTV(diff2, diff2 >= 0.5);

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
}

static void test14_connection_reset()
    // ------------------------------------------------------------------------
    // CONNECTION RESET
    //
    // Concerns:
    //   Ensure proper behavior of the 'reset' method.
    //
    // Plan:
    //   Obtain a connection object by connecting a slot to a signaler. Release
    //   that connection.  Check that the connection object is reset to a
    //   default-constructed state.  Then, destroy the connection object and
    //   check that the slot was not disconnected.
    //
    // Testing:
    //   bdlmt::SignalerConnectionGuard::release()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator            alloc;
    bdlmt::Signaler<void()>         sig(&alloc);
    const bdlmt::SignalerConnection def;

    // connect a slot
    bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());

    // create a connection
    bdlmt::SignalerConnection con2 = con1;
    ASSERT(con2 == con1);

    // reset it
    con2.reset();

    // the connection is now "empty"
    ASSERT(con2 == def);

    // the slot was not disconnected
    ASSERT(con1.isConnected());
}

static void test15_connection_swap()
    // ------------------------------------------------------------------------
    // CONNECTION SWAP
    //
    // Concerns:
    //   Ensure proper behavior of the 'swap' method.
    //
    // Plan:
    //   Create two instance of 'bdlmt::SignalerConnection', swap them. Check
    //   that they were swapped.
    //
    // Testing:
    //   - bdlmt::SignalerConnection::swap()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    bdlmt::SignalerConnection def;

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

static void test16_connection_isConnected()
    // ------------------------------------------------------------------------
    // CONNECTION IS CONNECTED, GUARDS
    //
    // Concerns:
    //   Ensure proper behavior of the 'isConnected' method.
    //
    // Plan:
    //: 1 Default-construct an instance of 'bdlmt::SignalerConnection'. Check
    //:    that 'isConnected()' returns false.
    //:
    //: 2 Obtain an instance of 'bdlmt::SignalerConnection' by connecting a
    //:   slot to a signaler. Check that 'isConnected()' returns true, then
    //:   disconnect the slot and check that 'isConnected()' now returns
    //:   false.
    //:
    //: 3 Construct guard from connection, single arg, observe disconnect on
    //:   destruction.
    //:
    //: 4 Construct guard from connection, double arg, observe disconnect on
    //:   destruction.
    //:
    //: 5 Construct guard from moved connection, single arg, observe disconnect
    //:   on destruction.
    //:
    //: 6 Construct guard from moved connection, double arg, observe disconnect
    //:   on destruction.
    //:
    //: 7 Construct guard from moved guard, single arg, observe disconnect on
    //:   destruction.
    //:
    //: 8 Construct guard from moved default-constructed guard, single arg,
    //:   observe disconnect on destruction.
    //:
    //: 9 Assign to guard, observe disconnect on assignment and destruction.
    //:
    //: 10 Assign to guard from default-constructed guard, observe disconnect
    //:    on assignment.
    //:
    //: 11 Assign to default constructed guard, observe disconnect on
    //:    destruction.
    //
    // Testing:
    //   bdlmt::SignalerConnection::isConnected()
    //   bdlmt::SignalerConnectionGuard::creators
    //   bdlmt::SignalerConnectionGuard::assignment
    //   bdlmt::SignalerConnectionGuard::isConnected()
    //   bdlmt::SignalerConnectionGuard::waitOnDisconnect()
    //   bdlmt::SignalerConnectionGuard::release()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // 1. default-constructed connection
    const bdlmt::SignalerConnection def;
    bdlmt::SignalerConnection       con;
    bdlmt::SignalerConnection       conB;
    bdlmt::SignalerConnection       conC;
    bdlmt::SignalerConnection       conD;
    ASSERT_EQ(con.isConnected(), false);

    // 2. connection obtained by connecting a slot
    con = sig.connect(u::NoOp());
    ASSERT_EQ(con.isConnected(), true);
    con.disconnect();
    ASSERT_EQ(con.isConnected(), false);

    ASSERT(0 == sig.slotCount());

    // 3. Construct guard from connection, single arg
    {
        con = sig.connect(u::NoOp());
        bdlmt::SignalerConnectionGuard guardA(con);
        ASSERT(con.isConnected());
        ASSERT(guardA.connection().isConnected());
        ASSERT(!guardA.waitOnDisconnect());
        ASSERT(guardA.connection() == con);
        ASSERT(1 == sig.slotCount());
    }
    ASSERT(!con.isConnected());
    ASSERT(def != con);
    ASSERT(0 == sig.slotCount());

    // 4. Construct guard from connection, double arg
    for (int ii = 0; ii < 2; ++ii) {
        const bool rhsB = ii;

        if (veryVerbose) cout << "Construct Guard: " << rhsB << endl;

        {
            con = sig.connect(u::NoOp());
            bdlmt::SignalerConnectionGuard guardA(con, rhsB);
            ASSERT(con.isConnected());
            ASSERT(guardA.connection().isConnected());
            ASSERT(rhsB == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(1 == sig.slotCount());
        }
        ASSERT(!con.isConnected());
        ASSERT(def != con);
        ASSERT(0 == sig.slotCount());
    }

    // 5. Construct guard from moved connection, single arg
    {
        con = sig.connect(u::NoOp());
        conB = con;
        bdlmt::SignalerConnectionGuard guardA(
                                            bslmf::MovableRefUtil::move(conB));
        ASSERT(con.isConnected());
        ASSERT(def == conB);
        ASSERT(guardA.connection().isConnected());
        ASSERT(!guardA.waitOnDisconnect());
        ASSERT(guardA.connection() == con);
        ASSERT(1 == sig.slotCount());
    }
    ASSERT(!con.isConnected());
    ASSERT(def != con);
    ASSERT(0 == sig.slotCount());

    // 6 Construct guard from moved connection, double arg
    for (int ii = 0; ii < 2; ++ii) {
        const bool rhsB = ii;

        if (veryVerbose) cout << "Construct Guard Moved Conn Double: " <<
                                                                  rhsB << endl;

        {
            con = sig.connect(u::NoOp());
            conB = con;
            bdlmt::SignalerConnectionGuard guardA(
                                      bslmf::MovableRefUtil::move(conB), rhsB);
            ASSERT(con.isConnected());
            ASSERT(def == conB);
            ASSERT(guardA.connection().isConnected());
            ASSERT(rhsB == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(1 == sig.slotCount());
        }
        ASSERT(!con.isConnected());
        ASSERT(def != con);
        ASSERT(0 == sig.slotCount());
    }

    // 7 Construct guard from moved guard, single arg
    for (int ii = 0; ii < 2; ++ii) {
        const bool rhsB = ii;

        if (veryVerbose) cout << "Construct Guard Moved Guard Single: " <<
                                                                  rhsB << endl;
        {
            con = sig.connect(u::NoOp());
            conB = con;
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(con == conB);
            bdlmt::SignalerConnectionGuard guardA(
                                      bslmf::MovableRefUtil::move(conB), rhsB);
            ASSERT(con.isConnected());
            ASSERT(def == conB);
            ASSERT(guardA.connection().isConnected());
            ASSERT(rhsB == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(1 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardB(
                                          bslmf::MovableRefUtil::move(guardA));
            ASSERT(con.isConnected());
            ASSERT(def == conB);
            ASSERT(def == guardA.connection());
            ASSERT(!guardA.connection().isConnected());
            ASSERT(guardB.connection().isConnected());
            ASSERT(false == guardA.waitOnDisconnect());
            ASSERT(rhsB  == guardB.waitOnDisconnect());
            ASSERT(guardB.connection() == con);
            ASSERT(1 == sig.slotCount());
        }
        ASSERT(!con.isConnected());
        ASSERT(def != con);
        ASSERT(def == conB);
        ASSERT(0 == sig.slotCount());
    }

    // 8 Construct guard from moved default-constructed guard
    {
        if (veryVerbose) cout << "Construct Guard Moved Guard\n";
        {
            bdlmt::SignalerConnectionGuard guardA;
            ASSERT(!guardA.connection().isConnected());
            ASSERT(false == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == def);
            ASSERT(0 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardB(
                                          bslmf::MovableRefUtil::move(guardA));
            ASSERT(def == guardA.connection());
            ASSERT(def == guardB.connection());
            ASSERT(!guardA.connection().isConnected());
            ASSERT(!guardB.connection().isConnected());
            ASSERT(false == guardA.waitOnDisconnect());
            ASSERT(false == guardB.waitOnDisconnect());
            ASSERT(0 == sig.slotCount());
        }
        ASSERT(0 == sig.slotCount());
    }

    // 9 Assign to guard
    for (int ii = 2 * 2; 0 < ii--; ) {
        const bool lhsB = ii & 1;
        const bool rhsB = ii & 2;
        const bool expWait = rhsB;

        if (veryVerbose) cout << "Move Assign guard to guard: " << lhsB <<
                                                          ", " << rhsB << endl;

        {
            con = sig.connect(u::NoOp());
            conB = con;
            conC = sig.connect(u::NoOp());
            conD = conC;
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(conC.isConnected());
            ASSERT(conD.isConnected());
            ASSERT(con  == conB);
            ASSERT(conC == conD);
            ASSERT(2 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardA(conB, rhsB);
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(guardA.connection().isConnected());
            ASSERT(rhsB == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(2 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardB(
                                      bslmf::MovableRefUtil::move(conD), lhsB);
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(conC.isConnected());
            ASSERT(def == conD);
            ASSERT(guardA.connection().isConnected());
            ASSERT(guardB.connection().isConnected());
            ASSERT(rhsB == guardA.waitOnDisconnect());
            ASSERT(lhsB == guardB.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(guardB.connection() == conC);
            ASSERT(2 == sig.slotCount());

            guardB = bslmf::MovableRefUtil::move(guardA);
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(!conC.isConnected());
            ASSERT(def == conD);
            ASSERT(def != conC);
            ASSERT(!guardA.connection().isConnected());
            ASSERT(guardB.connection().isConnected());
            ASSERT(false   == guardA.waitOnDisconnect());
            ASSERT(expWait == guardB.waitOnDisconnect());
            ASSERT(guardA.connection() == def);
            ASSERT(guardB.connection() == con);
            ASSERT(1 == sig.slotCount());
        }

        ASSERT(!con.isConnected());
        ASSERT(!conB.isConnected());
        ASSERT(!conC.isConnected());
        ASSERT(!conD.isConnected());
        ASSERT(def != con);
        ASSERT(def != conB);
        ASSERT(def != conC);
        ASSERT(def == conD);
        ASSERT(0 == sig.slotCount());
    }


    // 10 Assign to guard from default-constructed guard
    for (int ii = 2; 0 < ii--; ) {
        const bool lhsB = ii;

        if (veryVerbose) {
            cout << "Move Assign guard from default constructed guard: " <<
                                                                  lhsB << endl;
        }

        {
            conC = sig.connect(u::NoOp());
            conD = conC;
            ASSERT(conC.isConnected());
            ASSERT(conD.isConnected());
            ASSERT(conC == conD);
            ASSERT(1 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardA;
            ASSERT(!guardA.connection().isConnected());
            ASSERT(false == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == def);
            ASSERT(1 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardB(
                                      bslmf::MovableRefUtil::move(conD), lhsB);
            ASSERT(conC.isConnected());
            ASSERT(def == conD);
            ASSERT(!guardA.connection().isConnected());
            ASSERT(guardB.connection().isConnected());
            ASSERT(false == guardA.waitOnDisconnect());
            ASSERT(lhsB == guardB.waitOnDisconnect());
            ASSERT(guardA.connection() == def);
            ASSERT(guardB.connection() == conC);
            ASSERT(1 == sig.slotCount());

            guardB = bslmf::MovableRefUtil::move(guardA);
            ASSERT(!conC.isConnected());
            ASSERT(def == conD);
            ASSERT(def != conC);
            ASSERT(!guardA.connection().isConnected());
            ASSERT(!guardB.connection().isConnected());
            ASSERT(false == guardA.waitOnDisconnect());
            ASSERT(false == guardB.waitOnDisconnect());
            ASSERT(guardA.connection() == def);
            ASSERT(guardB.connection() == def);
            ASSERT(0 == sig.slotCount());
        }

        ASSERT(!conC.isConnected());
        ASSERT(!conD.isConnected());
        ASSERT(def == conD);
        ASSERT(0 == sig.slotCount());
    }

    // 11 Assign to default constructed guard
    for (int ii = 2; 0 < ii--; ) {
        const bool rhsB    = ii;
        const bool expWait = rhsB;

        if (veryVerbose) {
            cout << "Move Assign guard to default constructed guard" <<
                                                 " from rhs: " << rhsB << endl;
        }

        {
            con = sig.connect(u::NoOp());
            conB = con;
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(con  == conB);
            ASSERT(1 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardA(conB, rhsB);
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(guardA.connection().isConnected());
            ASSERT(rhsB == guardA.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(1 == sig.slotCount());

            bdlmt::SignalerConnectionGuard guardB;
            ASSERT(guardA.connection().isConnected());
            ASSERT(!guardB.connection().isConnected());
            ASSERT(rhsB  == guardA.waitOnDisconnect());
            ASSERT(false == guardB.waitOnDisconnect());
            ASSERT(guardA.connection() == con);
            ASSERT(guardB.connection() == def);
            ASSERT(1 == sig.slotCount());

            guardB = bslmf::MovableRefUtil::move(guardA);
            ASSERT(con.isConnected());
            ASSERT(conB.isConnected());
            ASSERT(!guardA.connection().isConnected());
            ASSERT(guardB.connection().isConnected());
            ASSERT(false   == guardA.waitOnDisconnect());
            ASSERT(expWait == guardB.waitOnDisconnect());
            ASSERT(guardA.connection() == def);
            ASSERT(guardB.connection() == con);
            ASSERT(1 == sig.slotCount());
        }

        ASSERT(!con.isConnected());
        ASSERT(!conB.isConnected());
        ASSERT(def != con);
        ASSERT(def != conB);
        ASSERT(0 == sig.slotCount());
    }
}

static void test17_connection_bitwiseMoveability()
    // ------------------------------------------------------------------------
    // CONNECTION BITWISE MOVEABILITY
    //
    // Concerns:
    //   Ensure that connection objects are bitwise-moveable.
    //
    // Plan:
    //   Check that 'bdlmt::SignalerConnection' is a bitwise-moveable type.
    //   NOTE: see 'bslmf_isbitwisemoveable' component documentation for the
    //   definition of a bitwise-moveable type.
    //
    // Testing:
    //   - bdlmt::SignalerConnection's bitwise-moveability
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // 'SignalerConnection' exports the 'IsBitwiseMoveable' trait
    ASSERT(bslmf::IsBitwiseMoveable<bdlmt::SignalerConnection>::value);

    // connect a slot
    bdlmt::SignalerConnection connection = sig.connect(u::NoOp());
    ASSERT(connection.isConnected() == true);

    // construct a connection
    bsls::ObjectBuffer<bdlmt::SignalerConnection> connection1Buffer;
    new (connection1Buffer.buffer()) bdlmt::SignalerConnection(connection);

    ASSERT(connection1Buffer.object() == connection);

    // move the connection using 'memcpy'
    bsls::ObjectBuffer<bdlmt::SignalerConnection> connection2Buffer;
    bsl::memcpy(connection2Buffer.buffer(), // dst
                connection1Buffer.buffer(), // src
                sizeof(bdlmt::SignalerConnection));

    ASSERT(connection2Buffer.object() == connection);

    // destroy the connection
    typedef bdlmt::SignalerConnection TYPE;
    connection2Buffer.object().~TYPE();
}

static void test18_guard_creators()
    // ------------------------------------------------------------------------
    // GUARD CREATORS
    //
    // Concerns:
    //   Ensure proper behavior of creators methods.
    //
    // Plan:
    //: 1 Default-construct an instance of 'bdlmt::SignalerConnectionGuard'.
    //:   Check postconditions.
    //:
    //: 2 Move-construct an instance of 'bdlmt::SignalerConnectionGuard'.
    //:   Check postconditions.
    //:
    //: 3 Construct an instance of 'bdlmt::SignalerConnectionGuard', from an
    //:   instance of bdlmt::SignalerConnection.  Check postconditions.
    //:
    //: 4 Construct an instance of 'bdlmt::SignalerConnectionGuard', from an
    //:   instance of 'bdlmt::SignalerConnection' by moving it.  Check
    //:   postconditions.
    //:
    //: 5 Construct an instance of 'bdlmt::SignalerConnectionGuard', from an
    //:   instance of 'bdlmt::SignalerConnection', destroy the instance of
    //:   'bdlmt::SignalerConnectionGuard' and check that 'disconnect()' was
    //:   invoked.
    //
    // Testing:
    //   - bdlmt::SignalerConnectionGuard's default constructor
    //   - bdlmt::SignalerConnectionGuard's move constructor
    //   - bdlmt::SignalerConnectionGuard's connection copying constructor
    //   - bdlmt::SignalerConnectionGuard's connection moving constructor
    //   - bdlmt::SignalerConnectionGuard's destructor
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;
    const bdlmt::SignalerConnection def;

    // 1. SignalerConnectionGuard default c-tor
    {
        bdlmt::SignalerConnectionGuard guard;

        // check postconditions
        ASSERT(guard.connection()       == def);
        ASSERT(guard.waitOnDisconnect() == false);
    }

    // 2. SignalerConnectionGuard move c-tor 1
    {
        bdlmt::Signaler<void()>        sig(&alloc);
        bdlmt::SignalerConnection      con = sig.connect(u::NoOp());
        bdlmt::SignalerConnectionGuard guard1(con, true);

        // move
        bdlmt::SignalerConnectionGuard guard2(
                                          bslmf::MovableRefUtil::move(guard1));

        // 'guard1' was reset to a default-constructed state
        ASSERT(guard1.connection()       == def);
        ASSERT(guard1.waitOnDisconnect() == false);

        // 'guard2' assumed the state of 'guard1'
        ASSERT(guard2.connection()       == con);
        ASSERT(guard2.waitOnDisconnect() == true);
    }

    // 3. SignalerConnectionGuard connection copy c-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con = sig.connect(u::NoOp());

        // copy
        bdlmt::SignalerConnectionGuard guard(con);

        // 'guard' reffers to the same slot as 'con'
        ASSERT(guard.connection() == con);
    }

    // 4. SignalerConnectionGuard connection move c-tor 1
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con1 = sig.connect(u::NoOp());
        bdlmt::SignalerConnection con2 = con1;

        // move
        bdlmt::SignalerConnectionGuard guard(
                                            bslmf::MovableRefUtil::move(con1));

        // 'con1' is now "empty"
        ASSERT(con1 == def);

        // 'guard' reffers to the same slot as 'con1' used to
        ASSERT(guard.connection() == con2);
    }

    // 5. SignalerConnectionGuard d-tor
    {
        bdlmt::Signaler<void()>   sig(&alloc);
        bdlmt::SignalerConnection con = sig.connect(u::NoOp());

        {
            bdlmt::SignalerConnectionGuard guard(con);
        }

        // 'guard's destructor invoked 'disconnect()'
        ASSERT_EQ(con.isConnected(), false);
    }
}

static void test19_guard_assignment()
    // -----------------------------------------------------------------------
    // GUARD ASSIGNMENT
    //
    // Concerns:
    //   Ensure proper behavior of assignment operators.
    //
    // Plan:
    //: 1 Move-assign an instance of 'bdlmt::SignalerConnectionGuard'.  Check
    //:   postconditions.
    //
    // Testing:
    //   - bdlmt::SignalerConnectionGuard's move assignment operator
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator alloc;
    const bdlmt::SignalerConnection def;

    // 1. SignalerConnectionGuard move assignment
    {
        bdlmt::Signaler<void()>        sig(&alloc);
        bdlmt::SignalerConnection      con = sig.connect(u::NoOp());
        bdlmt::SignalerConnectionGuard guard1(con, true);

        // move-assign
        bdlmt::SignalerConnectionGuard guard2;
                                       guard2 = bslmf::MovableRefUtil::move(
                                                                       guard1);

        // 'guard1' was reset to a defaut-constructed state
        ASSERT(guard1.connection()       == def);
        ASSERT(guard1.waitOnDisconnect() == false);

        // 'guard2' assumed the state of 'guard1'
        ASSERT(guard2.connection()       == con);
        ASSERT(guard2.waitOnDisconnect() == true);
    }
}

static void test20_guard_release()
    // ------------------------------------------------------------------------
    // GUARD RELEASE
    //
    // Concerns:
    //   Ensure proper behavior of the 'release' method.
    //
    // Plan:
    //  Obtain a connection object by connecting a slot to a signaler. Create
    //  a guard on top of the connection. Resease the guard. Check that the
    //  guard is reset to a default-constructed state. Then, destroy the guard
    //  and check that the slot was not disconnected.
    //
    // Testing:
    //   bdlmt::SignalerConnectionGuard::release()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator            alloc;
    bdlmt::Signaler<void()>         sig(&alloc);
    const bdlmt::SignalerConnection def;

    // connect a slot
    bdlmt::SignalerConnection con = sig.connect(u::NoOp());

    {
        // create a connection guard
        bdlmt::SignalerConnectionGuard guard(con, true);
        ASSERT(guard.connection()       == con);
        ASSERT(guard.waitOnDisconnect() == true);

        // release it
        ASSERT(con                      == guard.release());
        ASSERT(guard.waitOnDisconnect() == false);

        // the guard's connection is now "empty"
        ASSERT(guard.connection() == def);

        // destroy the guard ...
    }

    // the slot was not disconnected
    ASSERT(con.isConnected());
}

static void test21_guard_swap()
    // ------------------------------------------------------------------------
    // GUARD SWAP
    //
    // Concerns:
    //   Ensure proper behavior of the 'swap' method.
    //
    // Plan:
    //   Create two instance of 'bdlmt::SignalerConnectionGuard', swap them.
    //   Check that they were swapped.
    //
    // Testing:
    //   - bdlmt::SignalerConnectionGuard::swap()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    bdlmt::SignalerConnection def;

    bdlmt::SignalerConnectionGuard guard1(sig.connect(u::NoOp()), false);
    bdlmt::SignalerConnectionGuard guard2(sig.connect(u::NoOp()), true);
    bdlmt::SignalerConnection      con1(guard1.connection());
    bdlmt::SignalerConnection      con2(guard2.connection());

    ASSERT(guard1.connection() != def);
    ASSERT(guard2.connection() != def);
    ASSERT(con1 != def);
    ASSERT(con2 != def);

    ASSERT(guard1.connection() == con1);
    ASSERT(guard2.connection() == con2);
    ASSERT(con1 != guard2.connection());
    ASSERT(con2 != guard1.connection());

    ASSERT(!guard1.waitOnDisconnect());
    ASSERT( guard2.waitOnDisconnect());

    // swap
    guard1.swap(guard2);

    ASSERT(guard1.connection() != def);
    ASSERT(guard2.connection() != def);
    ASSERT(con1 != def);
    ASSERT(con2 != def);

    ASSERT(guard1.connection() != con1);
    ASSERT(guard2.connection() != con2);
    ASSERT(con1 == guard2.connection());
    ASSERT(con2 == guard1.connection());

    ASSERT( guard1.waitOnDisconnect());
    ASSERT(!guard2.waitOnDisconnect());

    // check
    ASSERT_EQ(guard1.connection().isConnected() &&
              guard2.connection().isConnected() &&
              con1.isConnected() &&
              con2.isConnected(),   true);
    con2.disconnect();

    ASSERT(guard1.connection() != def);
    ASSERT(guard2.connection() != def);
    ASSERT(con1 != def);
    ASSERT(con2 != def);

    ASSERT(guard1.connection() != con1);
    ASSERT(guard2.connection() != con2);
    ASSERT(con1 == guard2.connection());
    ASSERT(con2 == guard1.connection());

    ASSERT_EQ(!guard1.connection().isConnected() &&
               guard2.connection().isConnected() &&
               con1.isConnected() &&
              !con2.isConnected(),   true);

    con1.disconnect();

    ASSERT(guard1.connection() != def);
    ASSERT(guard2.connection() != def);
    ASSERT(con1 != def);
    ASSERT(con2 != def);

    ASSERT(guard1.connection() != con1);
    ASSERT(guard2.connection() != con2);
    ASSERT(con1 == guard2.connection());
    ASSERT(con2 == guard1.connection());

    ASSERT_EQ(!guard1.connection().isConnected() &&
              !guard2.connection().isConnected() &&
              !con1.isConnected() &&
              !con2.isConnected(),   true);
}

static void test22_guard_bitwiseMoveability()
    // ------------------------------------------------------------------------
    // GUARD BITWISE MOVEABILITY
    //
    // Concerns:
    //   Ensure that guard objects are bitwise-moveable.
    //
    // Plan:
    //   Check that 'bdlmt::SignalerConnectionGuard' is a bitwise-moveable
    //   type. NOTE: see 'bslmf_isbitwisemoveable' component documentation for
    //   the definition of a bitwise-moveable type.
    //
    // Testing:
    //   - bdlmt::SignalerConnectionGuard's bitwise-moveability
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator    alloc;
    bdlmt::Signaler<void()> sig(&alloc);

    // 'SignalerConnectionGuard' exports the 'IsBitwiseMoveable' trait
    ASSERT(bslmf::IsBitwiseMoveable<bdlmt::SignalerConnectionGuard>::value);

    // connect a slot
    bdlmt::SignalerConnection connection = sig.connect(u::NoOp());
    ASSERT(connection.isConnected() == true);

    // construct a guard
    bsls::ObjectBuffer<bdlmt::SignalerConnectionGuard> guard1Buffer;
    new (guard1Buffer.buffer()) bdlmt::SignalerConnectionGuard(connection,
                                                               true);

    ASSERT(guard1Buffer.object().connection()       == connection);
    ASSERT(guard1Buffer.object().waitOnDisconnect() == true);

    // move the guard using 'memcpy'
    bsls::ObjectBuffer<bdlmt::SignalerConnectionGuard> guard2Buffer;
    bsl::memcpy(guard2Buffer.buffer(), // dst
                guard1Buffer.buffer(), // src
                sizeof(bdlmt::SignalerConnectionGuard));

    ASSERT(guard2Buffer.object().connection()       == connection);
    ASSERT(guard2Buffer.object().waitOnDisconnect() == true);

    // destroy the guard and check that the slot was disconnected
    typedef bdlmt::SignalerConnectionGuard TYPE;
    guard2Buffer.object().~TYPE();
    ASSERT(connection.isConnected() == false);
}

namespace test23_signaler {

typedef bsls::Types::Int64 Int64;

template <class TYPE>
inline
TYPE sqr(TYPE x)
{
    return x * x;
}

template <class TYPE>
inline
TYPE cube(TYPE x)
{
    return x * x * x;
}

struct Functor {
    // DATA
    Int64 d_fib;

  private:
    // NOT IMPLEMENTED
    Functor& operator=(const Functor&);

  public:
    // CREATOR
    explicit
    Functor(Int64 fib) : d_fib(fib)
    {
        // NOTHING
    }

    Functor(const Functor& original)
    : d_fib(original.d_fib)
    {
        // NOTHING
    }

    // MANIPULATOR
    Int64 operator()(Int64& sum, Int64& sumSquares)                   // LVALUE
    {
        sum        += d_fib;
        sumSquares += d_fib * d_fib;

        return sum + sumSquares;
    }

    Int64 operator()(Int64        a1,
                     Int64        a2,
                     const Int64  a3,
                     const int&   a4,
                     const int    a5,
                     const Int64& a6,
                     Int64&       sum,
                     Int64&       sumSquared,
                     Int64&       sumCubed)                           // LVALUE
    {
        sum        += d_fib + a1 + a2 + a3 + a4 + a5 + a6;
        sumSquared += sqr(d_fib) + sqr(a1) + sqr(a2) + sqr(a3) + sqr(a4) +
                                                             sqr(a5) + sqr(a6);
        sumCubed   += cube(d_fib) + cube(a1) + cube(a2) + cube(a3) +
                                                cube(a4) + cube(a5) + cube(a6);

        return sum + sumSquared + sumCubed;
    }
};

void test_lvaluesComplex()
{
    bslma::TestAllocator                      alloc;
    const Int64 fibs[] = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233 };
    enum { k_NUM_FIBS = sizeof fibs / sizeof *fibs };

    if (verbose) cout << "Complex multi-args passed by lvalues cases\n";

    if (veryVerbose) cout << "Two arg case:\n";
    {
        bdlmt::Signaler<void(Int64&, Int64&)> sig(&alloc);

        for (int ii = 0; ii < k_NUM_FIBS; ++ii) {
            (void) sig.connect(Functor(fibs[ii]));
        }

        Int64 sumA = 0, sumSquaresA = 0;
        sig(sumA, sumSquaresA);

        ASSERT(0 != sumA);
        ASSERT(0 != sumSquaresA);

        Int64 sumB = 0, sumSquaresB = 0;
        for (int ii = 0; ii < k_NUM_FIBS; ++ii) {
            sumB        += fibs[ii];
            sumSquaresB += fibs[ii] * fibs[ii];
        }

        ASSERT(sumA == sumB);
        ASSERT(sumSquaresA == sumSquaresB);
    }

    if (veryVerbose) cout << "Nine arg case:\n";
    {
        bdlmt::Signaler<void(Int64,
                             Int64,
                             const Int64,
                             const int&,
                             const int,
                             const Int64&,
                             Int64&,
                             Int64&,
                             Int64&)>          sig(&alloc);

        for (int ii = 0; ii < k_NUM_FIBS; ++ii) {
            (void) sig.connect(Functor(fibs[ii]));
        }

        Int64 sumA = 0, sumSquaredA = 0, sumCubedA = 0;

        for (int ii = 5; ii < 100; ii += 5) {
            sig(ii+1, ii+2, ii+3, ii+4, ii+5, ii+6,
                                                 sumA, sumSquaredA, sumCubedA);
        }

        ASSERT(0 != sumA);
        ASSERT(0 != sumSquaredA);
        ASSERT(0 != sumCubedA);

        Int64 sumB = 0, sumSquaredB = 0, sumCubedB = 0;

        for (int ii = 5; ii < 100; ii += 5) {
            Int64 a1 = ii + 1;
            Int64 a2 = ii + 2;
            Int64 a3 = ii + 3;
            int   a4 = ii + 4;
            int   a5 = ii + 5;
            Int64 a6 = ii + 6;

            for (int jj = 0; jj < k_NUM_FIBS; ++jj) {
                Int64 f = fibs[jj];
                sumB        += f + a1 + a2 + a3 + a4 + a5 + a6;
                sumSquaredB += sqr(f) + sqr(a1) + sqr(a2) + sqr(a3) + sqr(a4) +
                                                             sqr(a5) + sqr(a6);
                sumCubedB   += cube(f) + cube(a1) + cube(a2) + cube(a3) +
                                                cube(a4) + cube(a5) + cube(a6);
            }
        }

        ASSERTV(sumB, sumA,               sumB        == sumA);
        ASSERTV(sumSquaredB, sumSquaredA, sumSquaredB == sumSquaredA);
        ASSERTV(sumCubedB, sumCubedA,     sumCubedB   == sumCubedA);
    }
}

struct MultiFunctor {
    int operator()(int& a1)                                           // LVALUE
    {
        ASSERT(1 == a1);

        return a1 *= 2;
    }

    int operator()(int& a1, int& a2)                                  // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        a1 *= 2;
        a2 *= 2;

        return a1;
    }

    int operator()(int& a1, int& a2, int& a3)                         // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;

        return a1;
    }

    void operator()(int& a1, int& a2, int& a3, int& a4)               // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;
        a4 *= 2;
    }

    Int64 operator()(int& a1, int& a2, int& a3, int& a4, int& a5)    // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        ASSERT(5 == a5);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;
        a4 *= 2;
        a5 *= 2;

        return a1;
    }

    Int64 operator()(int& a1, int& a2, int& a3, int& a4, int& a5,
                                                             int& a6) // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        ASSERT(5 == a5);
        ASSERT(6 == a6);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;
        a4 *= 2;
        a5 *= 2;
        a6 *= 2;

        return a1;
    }

    Int64 operator()(int& a1, int& a2, int& a3, int& a4, int& a5, int& a6,
                                                             int& a7) // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        ASSERT(5 == a5);
        ASSERT(6 == a6);
        ASSERT(7 == a7);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;
        a4 *= 2;
        a5 *= 2;
        a6 *= 2;
        a7 *= 2;

        return a1;
    }

    Int64 operator()(int& a1, int& a2, int& a3, int& a4, int& a5, int& a6,
                                                    int& a7, int& a8) // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        ASSERT(5 == a5);
        ASSERT(6 == a6);
        ASSERT(7 == a7);
        ASSERT(8 == a8);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;
        a4 *= 2;
        a5 *= 2;
        a6 *= 2;
        a7 *= 2;
        a8 *= 2;

        return a1;
    }

    Int64 operator()(int& a1, int& a2, int& a3, int& a4, int& a5, int& a6,
                                           int& a7, int& a8, int& a9) // LVALUE
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        ASSERT(5 == a5);
        ASSERT(6 == a6);
        ASSERT(7 == a7);
        ASSERT(8 == a8);
        ASSERT(9 == a9);
        a1 *= 2;
        a2 *= 2;
        a3 *= 2;
        a4 *= 2;
        a5 *= 2;
        a6 *= 2;
        a7 *= 2;
        a8 *= 2;
        a9 *= 2;

        return a1;
    }
};

int x[10];

void resetX()
{
    for (int ii = 0; ii < 10; ++ii) {
        x[ii] = ii;
    }
}

void checkX(int maxIndex)
{
    for (int ii = 0; ii < 10; ++ii) {
        if (ii <= maxIndex) {
            ASSERTV(ii, x[ii], maxIndex, 2 * ii == x[ii]);
        }
        else {
            ASSERTV(ii, x[ii], maxIndex,     ii == x[ii]);
        }
    }
}

void test_lvaluesSimple()
{
    bslma::TestAllocator                      alloc;

    if (verbose) cout << "Simple multi-arg lvalues test\n";

    if (veryVerbose) cout << "1 arg\n";
    {
        bdlmt::Signaler<void(int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1]);
        checkX(1);
    }

    if (veryVerbose) cout << "2 args\n";
    {
        bdlmt::Signaler<void(int&, int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2]);
        checkX(2);
    }

    if (veryVerbose) cout << "3 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3]);
        checkX(3);
    }

    if (veryVerbose) cout << "4 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&, int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3], x[4]);
        checkX(4);
    }

    if (veryVerbose) cout << "5 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&, int&, int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3], x[4], x[5]);
        checkX(5);
    }

    if (veryVerbose) cout << "6 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&, int&, int&, int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3], x[4], x[5], x[6]);
        checkX(6);
    }

    if (veryVerbose) cout << "7 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&, int&, int&, int&, int&)>
                                                                   sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
        checkX(7);
    }

    if (veryVerbose) cout << "8 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&, int&, int&, int&, int&, int&)>
                                                                   sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8]);
        checkX(8);
    }

    if (veryVerbose) cout << "9 args\n";
    {
        bdlmt::Signaler<void(int&, int&, int&, int&, int&, int&, int&, int&,
                                                            int&)> sig(&alloc);
        sig.connect(MultiFunctor());

        resetX();
        sig(x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8], x[9]);
        checkX(9);
    }
}

void test_lvalues()
    // ------------------------------------------------------------------------
    // LVALUES IN ARG LIST
    //
    // Concerns:
    //   Ensure lvalues are communicated properly.
    //
    // Plan:
    //: 1 'test_lvaluesComplex': a fairly complex test passing 9 arguments of
    //:   a variety of types.
    //:
    //: 2 'test_lvaluesSimple': test emitting signals passing a range of 1-9
    //:   lvalue arguments, checking tha they were passed correctly to slots,
    //:   then having the slots modify them and having the caller check that
    //:   the args we correctly modified by the called slot.
    //
    // Testing:
    //   operator()(T1&);
    //   operator()(T1&, T2&);
    //   operator()(T1&, T2&, T3&);
    //   operator()(T1&, T2&, T3&, T4&);
    //   operator()(T1&, T2&, T3&, T4&, T5&);
    //   operator()(T1&, T2&, T3&, T4&, T5&, T6&);
    //   operator()(T1&, T2&, T3&, T4&, T5&, T6&, T7&);
    //   operator()(T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&);
    //   operator()(T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&);
    // ------------------------------------------------------------------------
{
    test_lvaluesComplex();
    test_lvaluesSimple();
}

}  // close namespace test23_signaler

static void test24_destroyGuardAndWait()
    // ------------------------------------------------------------------------
    // SIGNALER DISCONNECT GROUP AND WAIT
    //
    // Concerns:
    //   Ensure proper behavior of the 'SignalerConnectionGuard' destructore.
    //
    // Plan:
    //: 1 Create a signaler having no connected slots.  Call
    //:   'disconnectGroupAndWait()' specifying the group 0.  Check that
    //:   nothing happened.
    //:
    //: 2 Create a signaler.  Connect several slots.  Invoke the signaler from
    //:   thread #1, and then again, from thread #2.  At the same time call
    //:   'disconnectGroupAndWait()' from thread #0 (the main thread)
    //:   specifying a group containing several slots.  Check that:
    //:   o All slots in the specified group were disconnected;
    //:   o No other slots were disconnected;
    //:   o 'disconnectAllSlotsAndWait()' have blocked the calling thread
    //:     pending completion of the currently executing slots.
    //
    // Testing:
    //   bdlmt::Signaler::disconnectGroupAndWait()
    // ------------------------------------------------------------------------
{
    bslma::TestAllocator   alloc;
    bdlmt::ThreadPool      threadPool(attr, 2, 2, 1000, &alloc);

    // 1. nothing to disconnect
    {
        bdlmt::SignalerConnectionGuard g;
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

            // Connect another slot to group '2'. Does the following:
            // 1. push starting timestamp
            // 2. sleep
            // 3. push completion timestamp

            bdlmt::SignalerConnectionGuard guard(
                                        sig.connect(
                                               bdlf::BindUtil::bindR<void>(
                                                    u::SleepAndPushTimestamp(),
                                                    0.5, // sleep 500 ms
                                                    bdlf::PlaceHolders::_1),
                                               2),
                                        true);

            u::DoubleTI start1, start2;

            {
                bdlmt::SignalerConnectionGuard con6(sig.connect(u::NoOp(), 2),
                                                    true);

                // invoke the signaler from thread #1
                threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                           &sig,
                                                           &tQueue1));

                // invoke the signaler from thread #2
                threadPool.enqueueJob(bdlf::BindUtil::bind(&Sig::operator(),
                                                           &sig,
                                                           &tQueue2));

                // wait till the target slot starts executing on both threads
                start1 = tQueue1.popFront();
                start2 = tQueue2.popFront();
            }

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

            // 'disconnectGroupAndWait(*)' has blocked the calling thread
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
        }

        // slots in group '1' were not disconnected
        ASSERT_EQ(sig.slotCount(),    2u);
        ASSERT_EQ(con1.isConnected(), true);
        ASSERT_EQ(con2.isConnected(), true);

        // stop the thread pool
        threadPool.stop();
    }
}

static void test25_usageExample()
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
// Then, in 'main', create a button and subscribe to its events.
//..
    u::Button                 button;
    bdlmt::SignalerConnection connection = button.onPressConnect(
                                                              &u::showPresses);
//..
// Next the button is "pressed", we will receive a notification.
//..
    button.press();
//..
// Now, we see the following message:
//..
//  Button pressed 1 times.
//..
// Finally, unsubscribe from button's events when we don't want to receive
// notifications anymore.  (If we didn't call 'disconnect', 'button' would
// clean up all the allocated resources when it went out of scope):
//..
    connection.disconnect();
//..
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

    // Install an assert handler to 'gracefully' mark the test as failure in
    // case of assert.

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
    //..
    // error: unused function template 'operator()' [-Werror,-Wunused-template]
    //..

    u::NoOp()(1);
    u::NoOp()(1, 2);
    u::NoOp()(1, 2, 3);
    u::NoOp()(1, 2, 3, 4);
    u::NoOp()(1, 2, 3, 4, 5);

    switch (test) {
      case  0:
      case  1:  { test1_signaler_defaultConstructor();        } break;
      case  2:  { test2_signaler_destructor();                } break;
      case  3:  { test3_signaler::callOperator();             } break;
      case  4:  { test4_signaler_connect();                   } break;
      case  5:  { test5_signaler::disconnectGroup();          } break;
      case  6:  { test6_signaler_disconnectGroupAndWait();    } break;
      case  7:  { test7_signaler::disconnectAllSlots();       } break;
      case  8:  { test8_signaler_disconnectAllSlotsAndWait(); } break;
      case  9:  { test9_signaler_slotCount();                 } break;
      case  10: { test10_connection_creators();               } break;
      case  11: { test11_connection_assignment();             } break;
      case  12: { test12::connection_disconnect();            } break;
      case  13: { test13_connection_disconnectAndWait();      } break;
      case  14: { test14_connection_reset();                  } break;
      case  15: { test15_connection_swap();                   } break;
      case  16: { test16_connection_isConnected();            } break;
      case  17: { test17_connection_bitwiseMoveability();     } break;
      case  18: { test18_guard_creators();                    } break;
      case  19: { test19_guard_assignment();                  } break;
      case  20: { test20_guard_release();                     } break;
      case  21: { test21_guard_swap();                        } break;
      case  22: { test22_guard_bitwiseMoveability();          } break;
      case  23: { test23_signaler::test_lvalues();            } break;
      case  24: { test24_destroyGuardAndWait();               } break;
      case  25: { test25_usageExample();                      } break;
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
//      Copyright (C) Bloomberg L.P., 2019
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
