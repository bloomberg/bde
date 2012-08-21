// bsls_timeutil.t.cpp                                                -*-C++-*-

#include <bsls_timeutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#ifdef BSLS_PLATFORM__OS_UNIX
    #include <time.h>      // NOTE: <ctime> conflicts with <sys/time.h>
    #include <sys/time.h>  // 'gethrtime()'
    #include <unistd.h>    // 'sleep'
#ifdef BSLS_PLATFORM__OS_SOLARIS   // Solaris OR late SunOS!
    #include <limits.h>    // 'CLK_TCK', for Sun (on FreeBSD, in <sys/time.h>)
#endif
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
typedef unsigned long DWORD;
typedef struct _LARGE_INTEGER {
    long long QuadPart;
} LARGE_INTEGER;

extern "C" {
    __declspec(dllimport) void __stdcall Sleep(DWORD dwMilliseconds);
    __declspec(dllimport) int  __stdcall QueryPerformanceFrequency(
                                                   LARGE_INTEGER *lpFrequency);
};
#endif

#include <cstdio>                   // printf()
#include <cstdlib>                  // atoi()
#include <iostream>
#include <iomanip>
#include <string>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This utility component has at present only four static functions (one being
// just an alias to one of the other three), which are a timer functions based
// on a platform-dependent system clocks.
// It is not possible to completely test a system-dependent function, but we
// address basic concerns to probe both our own code for consistent behavior
// and the system results for plausible correct behavior.
//-----------------------------------------------------------------------------
// [11] bsls::Types::Int64 convertRawTime(OpaqueNativeTime rawTime);
// [ 1] bsls::Types::Int64 bsls::TimeUtil::getProcessSystemTimer();
// [ 1] void bsls::TimeUtil::getProcessTimers(bsls::Types::Int64);
// [ 1] bsls::Types::Int64 bsls::TimeUtil::getTimer();
// [ 1] bsls::Types::Int64 bsls::TimeUtil::getProcessUserTimer();
// [11] OpaqueNativeTime getTimerRaw();
//-----------------------------------------------------------------------------
// [XX] Breathing Test -- NOT IMPLEMENTED
// [ 2] USAGE
// [ 3] Performance Test
// [ 4] Test for unique, monotonically increasing return values (statistical)
// [ 5] Test correct hooking of methods to underlying OS APIs (approximately)
// [ 6] Test of 'gethrtime()' (Sun and HP only -- statistical)
// [ 7] Initialization test: getProcessSystemTimer (UNIX only)
// [ 8] Initialization test: getProcessUserTimer (UNIX only)
// [ 9] Initialization test: getProcessTimers (UNIX only)
// [10] Initialization test: getTimer (Windows only)
//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
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

//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);} }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

//=============================================================================

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                 GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::TimeUtil     TU;
typedef bsls::Types::Int64 Int64;

typedef Int64 (*TimerMethod) ();

//=============================================================================
//                              HELPER FUNCTIONS
//-----------------------------------------------------------------------------

static Int64 callGetProcessTimersRetSystem() {
    Int64 cTimer;
    Int64 uTimer;
    TU::getProcessTimers(&cTimer, &uTimer);
    return cTimer;
}

static Int64 callGetProcessTimersRetUser() {
    Int64 cTimer;
    Int64 uTimer;
    TU::getProcessTimers(&cTimer, &uTimer);
    return uTimer;
}

static void osSleep(unsigned seconds) {
#if defined(BSLS_PLATFORM__OS_UNIX)
    for(;;) {
        int t = sleep(seconds);
        if (t <= 0) {
            break;
        }
        seconds = t;
    }
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
    Sleep(seconds*1000);  // milliseconds
#else
    #error "Do not know how to sleep on this platform"
#endif
}

//=============================================================================
//                             USAGE EXAMPLE CODE
//-----------------------------------------------------------------------------

class my_Timer {
    // This class implements a simple interval timer that is created in
    // the "running" state, and may be queried for its cumulative
    // interval (as a 'double', in seconds) but never stopped or reset.

    bsls::Types::Int64 d_startWallTime;   // time at creation (nsec)
    bsls::Types::Int64 d_startUserTime;   // time at creation (nsec)
    bsls::Types::Int64 d_startSystemTime; // time at creation (nsec)

  public:
    // CREATORS
    my_Timer() {
        d_startWallTime = bsls::TimeUtil::getTimer();
        d_startUserTime = bsls::TimeUtil::getProcessUserTimer();
        d_startSystemTime = bsls::TimeUtil::getProcessSystemTimer();
    }
        // Create a timer object initialized with the times at creation.
        // All values returned by subsequent calls to 'elapsed<...>Time()'
        // are with respect to this creation time.

    ~my_Timer() {};

    // ACCESSORS
    double elapsedWallTime();
        // Return the total elapsed time in seconds since the creation of
        // this timer object.
    double elapsedUserTime();
        // Return the elapsed user time in seconds since the creation of
        // this timer object.
    double elapsedSystemTime();
        // Return the elapsed system time in seconds since the creation of
        // this timer object.
};

inline
double my_Timer::elapsedWallTime()
{
    return (double) (bsls::TimeUtil::getTimer() - d_startWallTime) * 1.0E-9;
}

inline
double my_Timer::elapsedUserTime()
{
    return (double) (bsls::TimeUtil::getProcessUserTimer() - d_startUserTime)
                                                                      * 1.0E-9;
}

inline
double my_Timer::elapsedSystemTime()
{
    return (double) (bsls::TimeUtil::getProcessSystemTimer()
                                                 - d_startSystemTime) * 1.0E-9;
}

//=============================================================================
//                         HELPER FUNCTIONS FOR CASE 11
//-----------------------------------------------------------------------------

// Data Generation Script for Case 11
// ----------------------------------
//
// The following python script is used to generate the data in Section 2 of the
// DATA array.  Python is used because it exists on all of our platforms and
// provides unbounded integer arithmetic.
//
// The script provides two classes:
//: o Transform: Provides a facility for conversions between clock ticks and
//:   nanoseconds, given a set clock frequency and initial time.
//:
//: o Generator: Generate a set of interesting sample conversions from clock
//:   ticks to nanoseconds for a variety of frequencies.
//:
//:   The frequencies
//:   sampled include hardcoded frequencies typically seen on Windows machines,
//:   the highest possible 32-bit frequency, and all of the powers of a given
//:   base that are valid 32-bit numbers.  By choosing a base that is
//:   relatively prime to 1 billion and (1 << 32), we can get representative
//:   frequencies that cover the 32-bit range well and that consistently leave
//:   a remainder when calculating the constant factors used in the Windows
//:   implementation of 'convertRawTimer'.
//:
//:   For each frequency, we calculate nanoseconds based on the following
//:   values:
//:     o Lowest valid clock tick value
//:     o Clock tick values around the 32-bit limit
//:     o Clock tick values near the value that generates the maximum number of
//:       nanoseconds that can be represented for this frequency.
//:
//:   Additionally, a full range of tick counts that are powers of a given base
//:   can be generated, but they are not included in the current test data.
//:
//:   Any generated values that include frequencies, tick counts or nanosecond
//:   values that cannot be represented by 'bsls::Types::Int64' are discarded.
//..
//  #!/usr/bin/env python
//
//  class Transform:
//      """Provide a converter to transform a number of clock ticks and a
//      frequency to a time expressed in nanoseconds"""
//
//      billion = long(1000 * 1000 * 1000)
//      frequency = long(0)
//      initialTime = long(0)
//
//      def __init__(self, frequency, initialTime):
//          self.frequency = frequency
//          self.initialTime = initialTime
//
//      def nanoseconds(self, ticks):
//          return ((ticks - self.initialTime) * self.billion) / self.frequency
//
//      def ticks(self, nanoseconds):
//          return (nanoseconds * self.frequency) / self.billion \
//              + self.initialTime
//
//  class Generator:
//      """Provide a driver to generate nanosecond conversions of a number of
//      frequencies and clock tick values"""
//      max64 = (1 << 63) - 1
//      billion = long(1000 * 1000 * 1000)
//      frequencies = [
//          (1 << 32) - 1
//          ,2992530000
//          ,3579545
//      ]
//      verbose = True
//
//      def __init__(self, base, verbose):
//          self.verbose = verbose
//
//          count = 0
//          limit = 1 << 32
//          n = base
//
//          while n < limit:
//              self.frequencies.append(n)
//              self.frequencies.append(n + 1)
//              self.frequencies.append(n - 1)
//              ++count
//              n *= base
//
//      def generateTicks(self, base, initialTime):
//          result = []
//          limit = self.max64
//
//          n = base
//          while n < limit:
//              result.append(n)
//              n *= base
//
//          return result
//
//      def maxTicks(self, frequency, initialTime):
//          maxNSeconds = self.max64
//
//          return min((maxNSeconds * frequency) / self.billion - initialTime,
//                     self.max64)
//
//      def report(self, frequency, ticks, initialTime):
//          if initialTime < ticks:
//              t = Transform(frequency, initialTime)
//
//              nanoseconds = t.nanoseconds(ticks)
//              if (nanoseconds != 0 and nanoseconds <= self.max64):
//                  if self.verbose:
//                      print 'Init: f=%d, t=%d, i=%d' \
//                          % (frequency, ticks, initialTime)
//
//                      print 'Ticks: %d' % (t.ticks(nanoseconds))
//                      print 'Nanoseconds: %d' % (nanoseconds)
//
//                  print ',{ L_, %d, %d, %d, %d }' \
//                      % (ticks, initialTime, frequency, nanoseconds)
//
//                  if self.verbose:
//                      print
//              elif self.verbose:
//                  if nanoseconds == 0:
//                      print 'SKIP: %s f=%d, t=%d, i=%d' \
//                          % ("zero", frequency, ticks, initialTime)
//                  else:
//                      print 'SKIP: %s f=%d, t=%d, i=%d' \
//                          % ("overflow", frequency, ticks, initialTime)
//          elif self.verbose:
//              print 'SKIP: bad init f=%d, t=%d, i=%d' \
//                  % (frequency, ticks, initialTime)
//
//      def generate(self):
//          for frequency in self.frequencies:
//              initialTime = ((frequency) * 7) / 5
//              print
//              print '// Frequency: %d, Initial Time: %d' \
//                  % (frequency, initialTime)
//              self.report(frequency, initialTime + 1, initialTime)
//              self.report(frequency, (1 << 32) - 2, initialTime)
//              self.report(frequency, (1 << 32) - 1, initialTime)
//              self.report(frequency, (1 << 32), initialTime)
//              self.report(frequency, (1 << 32) + 1, initialTime)
//              self.report(frequency,
//                          self.maxTicks(frequency, initialTime) - 1,
//                          initialTime)
//              self.report(frequency,
//                          self.maxTicks(frequency, initialTime),
//                          initialTime)
//
//              #for baseValue in [2, 5, 7]:
//              #    for ticks in self.generateTicks(baseValue, initialTime):
//              #        self.report(frequency, ticks, initialTime)
//
//  g = Generator(3, False)
//  g.generate()
//..

#if defined BSLS_PLATFORM__OS_WINDOWS

bsls::Types::Int64 fakeConvertRawTime(bsls::Types::Int64 rawTime,
                                      bsls::Types::Int64 initialTime,
                                      bsls::Types::Int64 timerFrequency)
{
    const bsls::Types::Int64 K = 1000;
    const bsls::Types::Int64 G = K * K * K;
    const bsls::Types::Int64 HIGH_DWORD_MULTIPLIER = G * (1LL << 32);
    const bsls::Types::Int64 highPartDivisionFactor
                                      = HIGH_DWORD_MULTIPLIER / timerFrequency;
    const bsls::Types::Int64 highPartRemainderFactor
                                      = HIGH_DWORD_MULTIPLIER % timerFrequency;

    const bsls::Types::Uint64 LOW_MASK = 0x00000000ffffffff;

        rawTime -= initialTime;

        return (
                // Divide high part by frequency
                static_cast<bsls::Types::Int64>(rawTime >> 32)
                    * highPartDivisionFactor
                +
                (
                 // Restore remainder of high part division
                 static_cast<bsls::Types::Int64>(rawTime >> 32)
                    * highPartRemainderFactor
                 +
                 // Calculate low part contribution
                 static_cast<bsls::Types::Uint64>(rawTime & LOW_MASK)
                    * G
                )
                    / timerFrequency
                );
}

bsls::Types::Int64 getFrequency()
{
    LARGE_INTEGER frequency;

    ::QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

bsls::Types::Uint64 getBitMask(int numBits)
{
    bsls::Types::Uint64 mask = 0;

    return ~((~mask) << numBits);
}

unsigned getRand32()
{
    return ((unsigned) rand()) * 1103515245 + 12345;
}

bsls::Types::Uint64 getRand64()
{
    const bsls::Types::Uint64 random = getRand32();
    return (random << 32) + (bsls::Types::Uint64) getRand32();
}

bsls::Types::Int64 getTestPeriod(const bsls::Types::Int64 frequency)
{
    // const bsls::Types::Int64 testPeriod
    //     = frequency * 60LL * 60LL * 24LL; // 24 hours
    // return testPeriod * 106653; // 292 years: close to limit of Int64 as
    //                             // nanoseconds

    const bsls::Types::Int64 K = 1000;
    const bsls::Types::Int64 G = K * K * K;
    const bsls::Types::Int64 maxInt64 = getBitMask(63);

    if (frequency < G) {
        return (maxInt64 / G) * frequency;                            // RETURN
    } else {
        return maxInt64;                                              // RETURN
    }
}

void compareRealToFakeConvertRawTime(const TU::OpaqueNativeTime& startTime,
                                     const bsls::Types::Int64& offset,
                                     const bsls::Types::Int64& frequency)
{
    TU::OpaqueNativeTime endTime;
    endTime.d_opaque = startTime.d_opaque + offset;
    const bsls::Types::Int64 realStart
        = TU::convertRawTime(startTime);
    const bsls::Types::Int64 fakeStart
        = fakeConvertRawTime(startTime.d_opaque,
                             startTime.d_opaque,
                             frequency);
    const bsls::Types::Int64 realEnd
        = TU::convertRawTime(endTime);
    const bsls::Types::Int64 fakeEnd
        = fakeConvertRawTime(endTime.d_opaque,
                             startTime.d_opaque,
                             frequency);

    // We expect 'realEnd - realStart' to be the same as
    // 'fakeEnd - fakeStart'.  However, we know that
    // 'realEnd != fakeEnd' and 'realStart != fakeStart', because
    // the internal initial time used by 'TU::convertRawTime' is
    // not the same as 'startTime.d_opaque'.  Because the
    // calculations of 'realEnd', 'realStart, 'fakeEnd', and
    // 'fakeStart' all discard fractions of nanoseconds, the
    // intervals we compare will include a small amount of rounding
    // error, and therefore 'realEnd - realStart' and
    // 'fakeEnd - fakeStart' may differ by at most 1.
    //
    // "I have an elegant demonstration of this fact, but it will
    // not fit in this comment" -- ARB

    LOOP5_ASSERT(offset,
                 realEnd,
                 fakeEnd,
                 (realEnd - realStart),
                 (fakeEnd - fakeStart),
                 (realEnd - realStart) - (fakeEnd - fakeStart)
                 <= 1
                 &&
                 (realEnd - realStart) - (fakeEnd - fakeStart)
                 >= -1);
}

#endif

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header must build and
        //   run with a normal exit status.
        //
        // Plan:
        //   Copy the implementation portion of the Usage Example to the
        //   reserved space above, and copy the executable portion below,
        //   adding any needed supporting code.
        //
        // Testing:
        //   USAGE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
            my_Timer tw;
            for (int i = 0; i < 1000000; ++i) {
                // ...
            }
            double dTw = tw.elapsedWallTime();
            my_Timer tu;
            for (int i = 0; i < 1000000; ++i) {
                // ...
            }
            double dTu = tu.elapsedUserTime();
            my_Timer ts;
            for (int i = 0; i < 1000000; ++i) {
                // ...
            }
            double dTs = ts.elapsedSystemTime();
            if (verbose)
                std::cout
                    << "elapsed wall time: " << dTw << std::endl
                    << "elapsed user time: " << dTu << std::endl
                    << "elapsed system time: " << dTs << std::endl;
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING convertRawTime() arithmetic *** Windows Only ***
        //
        // Concerns:
        //   When the 'QueryPerformanceCounter' interface is available on
        //   Windows platforms, the conversion arithmetic is non-obvious and
        //   cannot be validated simply by reading the code.  Verify that the
        //   conversion is accurate.
        //
        //   Subconcerns include:
        //    o Underflow due to a small time interval and high frequency does
        //      not result in inaccurate calculation.
        //
        //    o Overflow due to a large interval and low frequency does not
        //      result in overflow.
        //
        //    o The minimum possible interval (one clock tick) is accurately
        //      handled.
        //
        //    o The maximum possible interval (frequency dependent, up to the
        //      maximum number of nanoseconds that can be represented by
        //      'bsls::Types::Int64' is accurately handled.
        //
        //    o Because calculation of the contributions from the high part and
        //      low part of the ''bsls::Types::Int64' representing the number
        //      of clock ticks in an interval are done separately, intervals
        //      just above or below the value '1 << 32' might not be handled
        //      correctly.
        //
        // Plan:
        //   'convertRawTime' cannot be tested directly because it relies on
        //   two hidden values: 's_timerFrequency' and 's_initialTime'.  We
        //   note, however, that the value of 's_timerFrequency' can be
        //   retrieved directly from the machine, and that the value of
        //   's_initialTime' will cancel itself out on any comparison between
        //   two converted times.  Therefore, use a copy of the Windows
        //   implementation of 'convertRawTime' to test the correctness of the
        //   arithmetic.  This copy must be kept in sync with the actual
        //   component code.
        //
        //   Test the correctness of the arithmetic by using the fake
        //   'convertRawTime' and a table-based strategy to convert a number of
        //   raw values for which the output value is known.
        //
        //   Test that the fake 'convertRawTime' behaves the same as the real
        //   'convertRawTime' by using both to measure real time intervals and
        //   compare the nanosecond results.  Test using fixed data based on
        //   the concerns above, as well as random data distributed over the
        //   entire range of valid clock tick values.
        //
        // Testing:
        //   bsls::TimeUtil::convertRawTime(bsls::TimeUtil::OpaqueNativeTime)
        // --------------------------------------------------------------------

#if defined BSLS_PLATFORM__OS_WINDOWS

        if (verbose) cout << "\nTesting convertRawTime arithmetic"
                          << "\n=================================" << endl;

        const bsls::Types::Int64 K = 1000;
        const bsls::Types::Int64 G = K * K * K;
        struct TimerTestData {
            int d_line;
            bsls::Types::Int64 d_input;
            bsls::Types::Int64 d_initialTime;
            bsls::Types::Int64 d_frequency;
            bsls::Types::Int64 d_output;
        } DATA[] = {
            //  Line Input      InitialTime Frequency  Output
            //  ---- ---------- ----------- ---------  ---------
            // Section 1: Simple overflow test covering ability to calculate
            // times from 1 second to 2^11 seconds.
             {  L_,  123456789,         0,  123456789, G }
            ,{  L_,  123456789LL << 1, 0, 123456789, G << 1 }
            ,{  L_,  123456789LL << 2, 0, 123456789, G << 2 }
            ,{  L_,  123456789LL << 3, 0, 123456789, G << 3 }
            ,{  L_,  123456789LL << 4, 0, 123456789, G << 4 }
            ,{  L_,  123456789LL << 5, 0, 123456789, G << 5 }
            ,{  L_,  123456789LL << 6, 0, 123456789, G << 6 }
            ,{  L_,  123456789LL << 7, 0, 123456789, G << 7 }
            ,{  L_,  123456789LL << 8, 0, 123456789, G << 8 }
            ,{  L_,  123456789LL << 9, 0, 123456789, G << 9 }
            ,{  L_,  123456789LL << 10, 0, 123456789, G << 10 }
            ,{  L_,  123456789LL << 11, 0, 123456789, G << 11 }

            ,{  L_,  3579545,         0,  3579545, G }
            ,{  L_,  3579545LL << 1, 0, 3579545, G << 1 }
            ,{  L_,  3579545LL << 2, 0, 3579545, G << 2 }
            ,{  L_,  3579545LL << 3, 0, 3579545, G << 3 }
            ,{  L_,  3579545LL << 4, 0, 3579545, G << 4 }
            ,{  L_,  3579545LL << 5, 0, 3579545, G << 5 }
            ,{  L_,  3579545LL << 6, 0, 3579545, G << 6 }
            ,{  L_,  3579545LL << 7, 0, 3579545, G << 7 }
            ,{  L_,  3579545LL << 8, 0, 3579545, G << 8 }
            ,{  L_,  3579545LL << 9, 0, 3579545, G << 9 }
            ,{  L_,  3579545LL << 10, 0, 3579545, G << 10 }
            ,{  L_,  3579545LL << 11, 0, 3579545, G << 11 }

            ,{  L_,  2992530000,         0,  2992530000, G }
            ,{  L_,  2992530000LL << 1, 0, 2992530000, G << 1 }
            ,{  L_,  2992530000LL << 2, 0, 2992530000, G << 2 }
            ,{  L_,  2992530000LL << 3, 0, 2992530000, G << 3 }
            ,{  L_,  2992530000LL << 4, 0, 2992530000, G << 4 }
            ,{  L_,  2992530000LL << 5, 0, 2992530000, G << 5 }
            ,{  L_,  2992530000LL << 6, 0, 2992530000, G << 6 }
            ,{  L_,  2992530000LL << 7, 0, 2992530000, G << 7 }
            ,{  L_,  2992530000LL << 8, 0, 2992530000, G << 8 }
            ,{  L_,  2992530000LL << 9, 0, 2992530000, G << 9 }
            ,{  L_,  2992530000LL << 10, 0, 2992530000, G << 10 }
            ,{  L_,  2992530000LL << 11, 0, 2992530000, G << 11 }

            // Section 2: Generated data
            // Frequency: 4294967295, Initial Time: 6012954213
            ,{ L_, 9223372036854775806, 6012954213, 4294967295,
                                                          2147483647099999999 }
            ,{ L_, 9223372036854775807, 6012954213, 4294967295,
                                                          2147483647099999999 }
            // Frequency: 2992530000, Initial Time: 4189542000
            ,{ L_, 4294967294, 4189542000, 2992530000, 35229486 }
            ,{ L_, 4294967295, 4189542000, 2992530000, 35229486 }
            ,{ L_, 4294967296, 4189542000, 2992530000, 35229486 }
            ,{ L_, 4294967297, 4189542000, 2992530000, 35229487 }
            ,{ L_, 9223372036854775806, 4189542000, 2992530000,
                                                          3082131852534555645 }
            ,{ L_, 9223372036854775807, 4189542000, 2992530000,
                                                          3082131852534555645 }
            // Frequency: 3579545, Initial Time: 5011363
            ,{ L_, 5011364, 5011363, 3579545, 279 }
            ,{ L_, 4294967294, 5011363, 3579545, 1198464031322 }
            ,{ L_, 4294967295, 5011363, 3579545, 1198464031601 }
            ,{ L_, 4294967296, 5011363, 3579545, 1198464031881 }
            ,{ L_, 4294967297, 5011363, 3579545, 1198464032160 }
            ,{ L_, 33015475252651964, 5011363, 3579545, 9223372034054775397 }
            ,{ L_, 33015475252651965, 5011363, 3579545, 9223372034054775676 }
            // Frequency: 3, Initial Time: 4
            ,{ L_, 5, 4, 3, 333333333 }
            ,{ L_, 4294967294, 4, 3, 1431655763333333333 }
            ,{ L_, 4294967295, 4, 3, 1431655763666666666 }
            ,{ L_, 4294967296, 4, 3, 1431655764000000000 }
            ,{ L_, 4294967297, 4, 3, 1431655764333333333 }
            ,{ L_, 27670116105, 4, 3, 9223372033666666666 }
            ,{ L_, 27670116106, 4, 3, 9223372034000000000 }
            // Frequency: 4, Initial Time: 5
            ,{ L_, 6, 5, 4, 250000000 }
            ,{ L_, 4294967294, 5, 4, 1073741822250000000 }
            ,{ L_, 4294967295, 5, 4, 1073741822500000000 }
            ,{ L_, 4294967296, 5, 4, 1073741822750000000 }
            ,{ L_, 4294967297, 5, 4, 1073741823000000000 }
            ,{ L_, 36893488141, 5, 4, 9223372034000000000 }
            ,{ L_, 36893488142, 5, 4, 9223372034250000000 }
            // Frequency: 2, Initial Time: 2
            ,{ L_, 3, 2, 2, 500000000 }
            ,{ L_, 4294967294, 2, 2, 2147483646000000000 }
            ,{ L_, 4294967295, 2, 2, 2147483646500000000 }
            ,{ L_, 4294967296, 2, 2, 2147483647000000000 }
            ,{ L_, 4294967297, 2, 2, 2147483647500000000 }
            ,{ L_, 18446744070, 2, 2, 9223372034000000000 }
            ,{ L_, 18446744071, 2, 2, 9223372034500000000 }
            // Frequency: 9, Initial Time: 12
            ,{ L_, 13, 12, 9, 111111111 }
            ,{ L_, 4294967294, 12, 9, 477218586888888888 }
            ,{ L_, 4294967295, 12, 9, 477218587000000000 }
            ,{ L_, 4294967296, 12, 9, 477218587111111111 }
            ,{ L_, 4294967297, 12, 9, 477218587222222222 }
            ,{ L_, 83010348318, 12, 9, 9223372034000000000 }
            ,{ L_, 83010348319, 12, 9, 9223372034111111111 }
            // Frequency: 10, Initial Time: 14
            ,{ L_, 15, 14, 10, 100000000 }
            ,{ L_, 4294967294, 14, 10, 429496728000000000 }
            ,{ L_, 4294967295, 14, 10, 429496728100000000 }
            ,{ L_, 4294967296, 14, 10, 429496728200000000 }
            ,{ L_, 4294967297, 14, 10, 429496728300000000 }
            ,{ L_, 92233720353, 14, 10, 9223372033900000000 }
            ,{ L_, 92233720354, 14, 10, 9223372034000000000 }
            // Frequency: 8, Initial Time: 11
            ,{ L_, 12, 11, 8, 125000000 }
            ,{ L_, 4294967294, 11, 8, 536870910375000000 }
            ,{ L_, 4294967295, 11, 8, 536870910500000000 }
            ,{ L_, 4294967296, 11, 8, 536870910625000000 }
            ,{ L_, 4294967297, 11, 8, 536870910750000000 }
            ,{ L_, 73786976282, 11, 8, 9223372033875000000 }
            ,{ L_, 73786976283, 11, 8, 9223372034000000000 }
            // Frequency: 27, Initial Time: 37
            ,{ L_, 38, 37, 27, 37037037 }
            ,{ L_, 4294967294, 37, 27, 159072861370370370 }
            ,{ L_, 4294967295, 37, 27, 159072861407407407 }
            ,{ L_, 4294967296, 37, 27, 159072861444444444 }
            ,{ L_, 4294967297, 37, 27, 159072861481481481 }
            ,{ L_, 249031044957, 37, 27, 9223372034074074074 }
            ,{ L_, 249031044958, 37, 27, 9223372034111111111 }
            // Frequency: 28, Initial Time: 39
            ,{ L_, 40, 39, 28, 35714285 }
            ,{ L_, 4294967294, 39, 28, 153391687678571428 }
            ,{ L_, 4294967295, 39, 28, 153391687714285714 }
            ,{ L_, 4294967296, 39, 28, 153391687750000000 }
            ,{ L_, 4294967297, 39, 28, 153391687785714285 }
            ,{ L_, 258254416991, 39, 28, 9223372034000000000 }
            ,{ L_, 258254416992, 39, 28, 9223372034035714285 }
            // Frequency: 26, Initial Time: 36
            ,{ L_, 37, 36, 26, 38461538 }
            ,{ L_, 4294967294, 36, 26, 165191048384615384 }
            ,{ L_, 4294967295, 36, 26, 165191048423076923 }
            ,{ L_, 4294967296, 36, 26, 165191048461538461 }
            ,{ L_, 4294967297, 36, 26, 165191048500000000 }
            ,{ L_, 239807672921, 36, 26, 9223372034038461538 }
            ,{ L_, 239807672922, 36, 26, 9223372034076923076 }
            // Frequency: 81, Initial Time: 113
            ,{ L_, 114, 113, 81, 12345679 }
            ,{ L_, 4294967294, 113, 81, 53024286185185185 }
            ,{ L_, 4294967295, 113, 81, 53024286197530864 }
            ,{ L_, 4294967296, 113, 81, 53024286209876543 }
            ,{ L_, 4294967297, 113, 81, 53024286222222222 }
            ,{ L_, 747093134871, 113, 81, 9223372034049382716 }
            ,{ L_, 747093134872, 113, 81, 9223372034061728395 }
            // Frequency: 82, Initial Time: 114
            ,{ L_, 115, 114, 82, 12195121 }
            ,{ L_, 4294967294, 114, 82, 52377648536585365 }
            ,{ L_, 4294967295, 114, 82, 52377648548780487 }
            ,{ L_, 4294967296, 114, 82, 52377648560975609 }
            ,{ L_, 4294967297, 114, 82, 52377648573170731 }
            ,{ L_, 756316506907, 114, 82, 9223372034060975609 }
            ,{ L_, 756316506908, 114, 82, 9223372034073170731 }
            // Frequency: 80, Initial Time: 112
            ,{ L_, 113, 112, 80, 12500000 }
            ,{ L_, 4294967294, 112, 80, 53687089775000000 }
            ,{ L_, 4294967295, 112, 80, 53687089787500000 }
            ,{ L_, 4294967296, 112, 80, 53687089800000000 }
            ,{ L_, 4294967297, 112, 80, 53687089812500000 }
            ,{ L_, 737869762835, 112, 80, 9223372034037500000 }
            ,{ L_, 737869762836, 112, 80, 9223372034050000000 }
            // Frequency: 243, Initial Time: 340
            ,{ L_, 341, 340, 243, 4115226 }
            ,{ L_, 4294967294, 340, 243, 17674761127572016 }
            ,{ L_, 4294967295, 340, 243, 17674761131687242 }
            ,{ L_, 4294967296, 340, 243, 17674761135802469 }
            ,{ L_, 4294967297, 340, 243, 17674761139917695 }
            ,{ L_, 2241279404614, 340, 243, 9223372034049382716 }
            ,{ L_, 2241279404615, 340, 243, 9223372034053497942 }
            // Frequency: 244, Initial Time: 341
            ,{ L_, 342, 341, 244, 4098360 }
            ,{ L_, 4294967294, 341, 244, 17602323577868852 }
            ,{ L_, 4294967295, 341, 244, 17602323581967213 }
            ,{ L_, 4294967296, 341, 244, 17602323586065573 }
            ,{ L_, 4294967297, 341, 244, 17602323590163934 }
            ,{ L_, 2250502776650, 341, 244, 9223372034053278688 }
            ,{ L_, 2250502776651, 341, 244, 9223372034057377049 }
            // Frequency: 242, Initial Time: 338
            ,{ L_, 339, 338, 242, 4132231 }
            ,{ L_, 4294967294, 338, 242, 17747797338842975 }
            ,{ L_, 4294967295, 338, 242, 17747797342975206 }
            ,{ L_, 4294967296, 338, 242, 17747797347107438 }
            ,{ L_, 4294967297, 338, 242, 17747797351239669 }
            ,{ L_, 2232056032579, 338, 242, 9223372034053719008 }
            ,{ L_, 2232056032580, 338, 242, 9223372034057851239 }
            // Frequency: 729, Initial Time: 1020
            ,{ L_, 1021, 1020, 729, 1371742 }
            ,{ L_, 4294967294, 1020, 729, 5891586109739368 }
            ,{ L_, 4294967295, 1020, 729, 5891586111111111 }
            ,{ L_, 4294967296, 1020, 729, 5891586112482853 }
            ,{ L_, 4294967297, 1020, 729, 5891586113854595 }
            ,{ L_, 6723838213846, 1020, 729, 9223372034054869684 }
            ,{ L_, 6723838213847, 1020, 729, 9223372034056241426 }
            // Frequency: 730, Initial Time: 1022
            ,{ L_, 1023, 1022, 730, 1369863 }
            ,{ L_, 4294967294, 1022, 730, 5883515441095890 }
            ,{ L_, 4294967295, 1022, 730, 5883515442465753 }
            ,{ L_, 4294967296, 1022, 730, 5883515443835616 }
            ,{ L_, 4294967297, 1022, 730, 5883515445205479 }
            ,{ L_, 6733061585880, 1022, 730, 9223372034052054794 }
            ,{ L_, 6733061585881, 1022, 730, 9223372034053424657 }
            // Frequency: 728, Initial Time: 1019
            ,{ L_, 1020, 1019, 728, 1373626 }
            ,{ L_, 4294967294, 1019, 728, 5899678949175824 }
            ,{ L_, 4294967295, 1019, 728, 5899678950549450 }
            ,{ L_, 4294967296, 1019, 728, 5899678951923076 }
            ,{ L_, 4294967297, 1019, 728, 5899678953296703 }
            ,{ L_, 6714614841810, 1019, 728, 9223372034053571428 }
            ,{ L_, 6714614841811, 1019, 728, 9223372034054945054 }
            // Frequency: 2187, Initial Time: 3061
            ,{ L_, 3062, 3061, 2187, 457247 }
            ,{ L_, 4294967294, 3061, 2187, 1963861103337905 }
            ,{ L_, 4294967295, 3061, 2187, 1963861103795153 }
            ,{ L_, 4294967296, 3061, 2187, 1963861104252400 }
            ,{ L_, 4294967297, 3061, 2187, 1963861104709647 }
            ,{ L_, 20171514641539, 3061, 2187, 9223372034054869684 }
            ,{ L_, 20171514641540, 3061, 2187, 9223372034055326931 }
            // Frequency: 2188, Initial Time: 3063
            ,{ L_, 3064, 3063, 2188, 457038 }
            ,{ L_, 4294967294, 3063, 2188, 1962963542504570 }
            ,{ L_, 4294967295, 3063, 2188, 1962963542961608 }
            ,{ L_, 4294967296, 3063, 2188, 1962963543418647 }
            ,{ L_, 4294967297, 3063, 2188, 1962963543875685 }
            ,{ L_, 20180738013574, 3063, 2188, 9223372034054387568 }
            ,{ L_, 20180738013575, 3063, 2188, 9223372034054844606 }
            // Frequency: 2186, Initial Time: 3060
            ,{ L_, 3061, 3060, 2186, 457456 }
            ,{ L_, 4294967294, 3060, 2186, 1964759484903934 }
            ,{ L_, 4294967295, 3060, 2186, 1964759485361390 }
            ,{ L_, 4294967296, 3060, 2186, 1964759485818847 }
            ,{ L_, 4294967297, 3060, 2186, 1964759486276303 }
            ,{ L_, 20162291269503, 3060, 2186, 9223372034054437328 }
            ,{ L_, 20162291269504, 3060, 2186, 9223372034054894784 }
            // Frequency: 6561, Initial Time: 9185
            ,{ L_, 9186, 9185, 6561, 152415 }
            ,{ L_, 4294967294, 9185, 6561, 654619434385002 }
            ,{ L_, 4294967295, 9185, 6561, 654619434537418 }
            ,{ L_, 4294967296, 9185, 6561, 654619434689833 }
            ,{ L_, 4294967297, 9185, 6561, 654619434842249 }
            ,{ L_, 60514543924618, 9185, 6561, 9223372034054717268 }
            ,{ L_, 60514543924619, 9185, 6561, 9223372034054869684 }
            // Frequency: 6562, Initial Time: 9186
            ,{ L_, 9187, 9186, 6562, 152392 }
            ,{ L_, 4294967294, 9186, 6562, 654519675099055 }
            ,{ L_, 4294967295, 9186, 6562, 654519675251447 }
            ,{ L_, 4294967296, 9186, 6562, 654519675403840 }
            ,{ L_, 4294967297, 9186, 6562, 654519675556232 }
            ,{ L_, 60523767296654, 9186, 6562, 9223372034054861322 }
            ,{ L_, 60523767296655, 9186, 6562, 9223372034055013715 }
            // Frequency: 6560, Initial Time: 9184
            ,{ L_, 9185, 9184, 6560, 152439 }
            ,{ L_, 4294967294, 9184, 6560, 654719224085365 }
            ,{ L_, 4294967295, 9184, 6560, 654719224237804 }
            ,{ L_, 4294967296, 9184, 6560, 654719224390243 }
            ,{ L_, 4294967297, 9184, 6560, 654719224542682 }
            ,{ L_, 60505320552582, 9184, 6560, 9223372034054573170 }
            ,{ L_, 60505320552583, 9184, 6560, 9223372034054725609 }
            // Frequency: 19683, Initial Time: 27556
            ,{ L_, 27557, 27556, 19683, 50805 }
            ,{ L_, 4294967294, 27556, 19683, 218205544784839 }
            ,{ L_, 4294967295, 27556, 19683, 218205544835644 }
            ,{ L_, 4294967296, 27556, 19683, 218205544886450 }
            ,{ L_, 4294967297, 27556, 19683, 218205544937255 }
            ,{ L_, 181543631773855, 27556, 19683, 9223372034054717268 }
            ,{ L_, 181543631773856, 27556, 19683, 9223372034054768073 }
            // Frequency: 19684, Initial Time: 27557
            ,{ L_, 27558, 27557, 19684, 50802 }
            ,{ L_, 4294967294, 27557, 19684, 218194459307051 }
            ,{ L_, 4294967295, 27557, 19684, 218194459357854 }
            ,{ L_, 4294967296, 27557, 19684, 218194459408656 }
            ,{ L_, 4294967297, 27557, 19684, 218194459459459 }
            ,{ L_, 181552855145891, 27557, 19684, 9223372034054765291 }
            ,{ L_, 181552855145892, 27557, 19684, 9223372034054816094 }
            // Frequency: 19682, Initial Time: 27554
            ,{ L_, 27555, 27554, 19682, 50807 }
            ,{ L_, 4294967294, 27554, 19682, 218216631439894 }
            ,{ L_, 4294967295, 27554, 19682, 218216631490702 }
            ,{ L_, 4294967296, 27554, 19682, 218216631541510 }
            ,{ L_, 4294967297, 27554, 19682, 218216631592317 }
            ,{ L_, 181534408401820, 27554, 19682, 9223372034054770856 }
            ,{ L_, 181534408401821, 27554, 19682, 9223372034054821664 }
            // Frequency: 59049, Initial Time: 82668
            ,{ L_, 82669, 82668, 59049, 16935 }
            ,{ L_, 4294967294, 82668, 59049, 72734248268387 }
            ,{ L_, 4294967295, 82668, 59049, 72734248285322 }
            ,{ L_, 4294967296, 82668, 59049, 72734248302257 }
            ,{ L_, 4294967297, 82668, 59049, 72734248319192 }
            ,{ L_, 544630895321568, 82668, 59049, 9223372034054768073 }
            ,{ L_, 544630895321569, 82668, 59049, 9223372034054785009 }
            // Frequency: 59050, Initial Time: 82670
            ,{ L_, 82671, 82670, 59050, 16934 }
            ,{ L_, 4294967294, 82670, 59050, 72733016494496 }
            ,{ L_, 4294967295, 82670, 59050, 72733016511430 }
            ,{ L_, 4294967296, 82670, 59050, 72733016528365 }
            ,{ L_, 4294967297, 82670, 59050, 72733016545300 }
            ,{ L_, 544640118693603, 82670, 59050, 9223372034054750211 }
            ,{ L_, 544640118693604, 82670, 59050, 9223372034054767146 }
            // Frequency: 59048, Initial Time: 82667
            ,{ L_, 82668, 82667, 59048, 16935 }
            ,{ L_, 4294967294, 82667, 59048, 72735480067064 }
            ,{ L_, 4294967295, 82667, 59048, 72735480083999 }
            ,{ L_, 4294967296, 82667, 59048, 72735480100934 }
            ,{ L_, 4294967297, 82667, 59048, 72735480117870 }
            ,{ L_, 544621671949532, 82667, 59048, 9223372034054752066 }
            ,{ L_, 544621671949533, 82667, 59048, 9223372034054769001 }
            // Frequency: 177147, Initial Time: 248005
            ,{ L_, 248006, 248005, 177147, 5645 }
            ,{ L_, 4294967294, 248005, 177147, 24243816090591 }
            ,{ L_, 4294967295, 248005, 177147, 24243816096236 }
            ,{ L_, 4294967296, 248005, 177147, 24243816101881 }
            ,{ L_, 4294967297, 248005, 177147, 24243816107526 }
            ,{ L_, 1633892685964706, 248005, 177147, 9223372034054773719 }
            ,{ L_, 1633892685964707, 248005, 177147, 9223372034054779364 }
            // Frequency: 177148, Initial Time: 248007
            ,{ L_, 248008, 248007, 177148, 5644 }
            ,{ L_, 4294967294, 248007, 177148, 24243679223022 }
            ,{ L_, 4294967295, 248007, 177148, 24243679228667 }
            ,{ L_, 4294967296, 248007, 177148, 24243679234312 }
            ,{ L_, 4294967297, 248007, 177148, 24243679239957 }
            ,{ L_, 1633901909336741, 248007, 177148, 9223372034054767764 }
            ,{ L_, 1633901909336742, 248007, 177148, 9223372034054773409 }
            // Frequency: 177146, Initial Time: 248004
            ,{ L_, 248005, 248004, 177146, 5645 }
            ,{ L_, 4294967294, 248004, 177146, 24243952954060 }
            ,{ L_, 4294967295, 248004, 177146, 24243952959705 }
            ,{ L_, 4294967296, 248004, 177146, 24243952965350 }
            ,{ L_, 4294967297, 248004, 177146, 24243952970995 }
            ,{ L_, 1633883462592671, 248004, 177146, 9223372034054774028 }
            ,{ L_, 1633883462592672, 248004, 177146, 9223372034054779673 }
            // Frequency: 531441, Initial Time: 744017
            ,{ L_, 744018, 744017, 531441, 1881 }
            ,{ L_, 4294967294, 744017, 531441, 8080338696111 }
            ,{ L_, 4294967295, 744017, 531441, 8080338697992 }
            ,{ L_, 4294967296, 744017, 531441, 8080338699874 }
            ,{ L_, 4294967297, 744017, 531441, 8080338701756 }
            ,{ L_, 4901678057894120, 744017, 531441, 9223372034054773719 }
            ,{ L_, 4901678057894121, 744017, 531441, 9223372034054775600 }
            // Frequency: 531442, Initial Time: 744018
            ,{ L_, 744019, 744018, 531442, 1881 }
            ,{ L_, 4294967294, 744018, 531442, 8080323489675 }
            ,{ L_, 4294967295, 744018, 531442, 8080323491556 }
            ,{ L_, 4294967296, 744018, 531442, 8080323493438 }
            ,{ L_, 4294967297, 744018, 531442, 8080323495320 }
            ,{ L_, 4901687281266156, 744018, 531442, 9223372034054775497 }
            ,{ L_, 4901687281266157, 744018, 531442, 9223372034054777379 }
            // Frequency: 531440, Initial Time: 744016
            ,{ L_, 744017, 744016, 531440, 1881 }
            ,{ L_, 4294967294, 744016, 531440, 8080353902604 }
            ,{ L_, 4294967295, 744016, 531440, 8080353904485 }
            ,{ L_, 4294967296, 744016, 531440, 8080353906367 }
            ,{ L_, 4294967297, 744016, 531440, 8080353908249 }
            ,{ L_, 4901668834522085, 744016, 531440, 9223372034054773822 }
            ,{ L_, 4901668834522086, 744016, 531440, 9223372034054775703 }
            // Frequency: 1594323, Initial Time: 2232052
            ,{ L_, 2232053, 2232052, 1594323, 627 }
            ,{ L_, 4294967294, 2232052, 1594323, 2692512898578 }
            ,{ L_, 4294967295, 2232052, 1594323, 2692512899205 }
            ,{ L_, 4294967296, 2232052, 1594323, 2692512899832 }
            ,{ L_, 4294967297, 2232052, 1594323, 2692512900459 }
            ,{ L_, 14705034173682363, 2232052, 1594323, 9223372034054774973 }
            ,{ L_, 14705034173682364, 2232052, 1594323, 9223372034054775600 }
            // Frequency: 1594324, Initial Time: 2232053
            ,{ L_, 2232054, 2232053, 1594324, 627 }
            ,{ L_, 4294967294, 2232053, 1594324, 2692511209139 }
            ,{ L_, 4294967295, 2232053, 1594324, 2692511209766 }
            ,{ L_, 4294967296, 2232053, 1594324, 2692511210393 }
            ,{ L_, 4294967297, 2232053, 1594324, 2692511211021 }
            ,{ L_, 14705043397054399, 2232053, 1594324, 9223372034054775566 }
            ,{ L_, 14705043397054400, 2232053, 1594324, 9223372034054776193 }
            // Frequency: 1594322, Initial Time: 2232050
            ,{ L_, 2232051, 2232050, 1594322, 627 }
            ,{ L_, 4294967294, 2232050, 1594322, 2692514588646 }
            ,{ L_, 4294967295, 2232050, 1594322, 2692514589273 }
            ,{ L_, 4294967296, 2232050, 1594322, 2692514589900 }
            ,{ L_, 4294967297, 2232050, 1594322, 2692514590528 }
            ,{ L_, 14705024950310328, 2232050, 1594322, 9223372034054775635 }
            ,{ L_, 14705024950310329, 2232050, 1594322, 9223372034054776262 }
            // Frequency: 4782969, Initial Time: 6696156
            ,{ L_, 6696157, 6696156, 4782969, 209 }
            ,{ L_, 4294967294, 6696156, 4782969, 896570966276 }
            ,{ L_, 4294967295, 6696156, 4782969, 896570966485 }
            ,{ L_, 4294967296, 6696156, 4782969, 896570966694 }
            ,{ L_, 4294967297, 6696156, 4782969, 896570966903 }
            ,{ L_, 44115102521047093, 6696156, 4782969, 9223372034054775809 }
            ,{ L_, 44115102521047094, 6696156, 4782969, 9223372034054776018 }
            // Frequency: 4782970, Initial Time: 6696158
            ,{ L_, 6696159, 6696158, 4782970, 209 }
            ,{ L_, 4294967294, 6696158, 4782970, 896570778407 }
            ,{ L_, 4294967295, 6696158, 4782970, 896570778616 }
            ,{ L_, 4294967296, 6696158, 4782970, 896570778825 }
            ,{ L_, 4294967297, 6696158, 4782970, 896570779034 }
            ,{ L_, 44115111744419128, 6696158, 4782970, 9223372034054775589 }
            ,{ L_, 44115111744419129, 6696158, 4782970, 9223372034054775798 }
            // Frequency: 4782968, Initial Time: 6696155
            ,{ L_, 6696156, 6696155, 4782968, 209 }
            ,{ L_, 4294967294, 6696155, 4782968, 896571153936 }
            ,{ L_, 4294967295, 6696155, 4782968, 896571154145 }
            ,{ L_, 4294967296, 6696155, 4782968, 896571154354 }
            ,{ L_, 4294967297, 6696155, 4782968, 896571154563 }
            ,{ L_, 44115093297675057, 6696155, 4782968, 9223372034054775612 }
            ,{ L_, 44115093297675058, 6696155, 4782968, 9223372034054775821 }
            // Frequency: 14348907, Initial Time: 20088469
            ,{ L_, 20088470, 20088469, 14348907, 69 }
            ,{ L_, 4294967294, 20088469, 14348907, 297923655439 }
            ,{ L_, 4294967295, 20088469, 14348907, 297923655509 }
            ,{ L_, 4294967296, 20088469, 14348907, 297923655578 }
            ,{ L_, 4294967297, 20088469, 14348907, 297923655648 }
            ,{ L_, 132345307563141280, 20088469, 14348907,
                                                          9223372034054775809 }
            ,{ L_, 132345307563141281, 20088469, 14348907,
                                                          9223372034054775879 }
            // Frequency: 14348908, Initial Time: 20088471
            ,{ L_, 20088472, 20088471, 14348908, 69 }
            ,{ L_, 4294967294, 20088471, 14348908, 297923634537 }
            ,{ L_, 4294967295, 20088471, 14348908, 297923634606 }
            ,{ L_, 4294967296, 20088471, 14348908, 297923634676 }
            ,{ L_, 4294967297, 20088471, 14348908, 297923634746 }
            ,{ L_, 132345316786513315, 20088471, 14348908,
                                                          9223372034054775736 }
            ,{ L_, 132345316786513316, 20088471, 14348908,
                                                          9223372034054775805 }
            // Frequency: 14348906, Initial Time: 20088468
            ,{ L_, 20088469, 20088468, 14348906, 69 }
            ,{ L_, 4294967294, 20088468, 14348906, 297923676271 }
            ,{ L_, 4294967295, 20088468, 14348906, 297923676341 }
            ,{ L_, 4294967296, 20088468, 14348906, 297923676411 }
            ,{ L_, 4294967297, 20088468, 14348906, 297923676480 }
            ,{ L_, 132345298339769244, 20088468, 14348906,
                                                          9223372034054775743 }
            ,{ L_, 132345298339769245, 20088468, 14348906,
                                                          9223372034054775813 }
            // Frequency: 43046721, Initial Time: 60265409
            ,{ L_, 60265410, 60265409, 43046721, 23 }
            ,{ L_, 4294967294, 60265409, 43046721, 98374551803 }
            ,{ L_, 4294967295, 60265409, 43046721, 98374551827 }
            ,{ L_, 4294967296, 60265409, 43046721, 98374551850 }
            ,{ L_, 4294967297, 60265409, 43046721, 98374551873 }
            ,{ L_, 397035922689423841, 60265409, 43046721,
                                                          9223372034054775786 }
            ,{ L_, 397035922689423842, 60265409, 43046721,
                                                          9223372034054775809 }
            // Frequency: 43046722, Initial Time: 60265410
            ,{ L_, 60265411, 60265410, 43046722, 23 }
            ,{ L_, 4294967294, 60265410, 43046722, 98374549495 }
            ,{ L_, 4294967295, 60265410, 43046722, 98374549518 }
            ,{ L_, 4294967296, 60265410, 43046722, 98374549541 }
            ,{ L_, 4294967297, 60265410, 43046722, 98374549565 }
            ,{ L_, 397035931912795877, 60265410, 43046722,
                                                          9223372034054775808 }
            ,{ L_, 397035931912795878, 60265410, 43046722,
                                                          9223372034054775831 }
            // Frequency: 43046720, Initial Time: 60265408
            ,{ L_, 60265409, 60265408, 43046720, 23 }
            ,{ L_, 4294967294, 60265408, 43046720, 98374554112 }
            ,{ L_, 4294967295, 60265408, 43046720, 98374554135 }
            ,{ L_, 4294967296, 60265408, 43046720, 98374554158 }
            ,{ L_, 4294967297, 60265408, 43046720, 98374554182 }
            ,{ L_, 397035913466051805, 60265408, 43046720,
                                                          9223372034054775764 }
            ,{ L_, 397035913466051806, 60265408, 43046720,
                                                          9223372034054775787 }
            // Frequency: 129140163, Initial Time: 180796228
            ,{ L_, 180796229, 180796228, 129140163, 7 }
            ,{ L_, 4294967294, 180796228, 129140163, 31858183933 }
            ,{ L_, 4294967295, 180796228, 129140163, 31858183940 }
            ,{ L_, 4294967296, 180796228, 129140163, 31858183948 }
            ,{ L_, 4294967297, 180796228, 129140163, 31858183956 }
            ,{ L_, 1191107768068271526, 180796228, 129140163,
                                                          9223372034054775802 }
            ,{ L_, 1191107768068271527, 180796228, 129140163,
                                                          9223372034054775809 }
            // Frequency: 129140164, Initial Time: 180796229
            ,{ L_, 180796230, 180796229, 129140164, 7 }
            ,{ L_, 4294967294, 180796229, 129140164, 31858183678 }
            ,{ L_, 4294967295, 180796229, 129140164, 31858183686 }
            ,{ L_, 4294967296, 180796229, 129140164, 31858183694 }
            ,{ L_, 4294967297, 180796229, 129140164, 31858183701 }
            ,{ L_, 1191107777291643561, 180796229, 129140164,
                                                          9223372034054775801 }
            ,{ L_, 1191107777291643562, 180796229, 129140164,
                                                          9223372034054775809 }
            // Frequency: 129140162, Initial Time: 180796226
            ,{ L_, 180796227, 180796226, 129140162, 7 }
            ,{ L_, 4294967294, 180796226, 129140162, 31858184195 }
            ,{ L_, 4294967295, 180796226, 129140162, 31858184202 }
            ,{ L_, 4294967296, 180796226, 129140162, 31858184210 }
            ,{ L_, 4294967297, 180796226, 129140162, 31858184218 }
            ,{ L_, 1191107758844899491, 180796226, 129140162,
                                                          9223372034054775810 }
            ,{ L_, 1191107758844899492, 180796226, 129140162,
                                                          9223372034054775817 }
            // Frequency: 387420489, Initial Time: 542388684
            ,{ L_, 542388685, 542388684, 387420489, 2 }
            ,{ L_, 4294967294, 542388684, 387420489, 9686061312 }
            ,{ L_, 4294967295, 542388684, 387420489, 9686061314 }
            ,{ L_, 4294967296, 542388684, 387420489, 9686061317 }
            ,{ L_, 4294967297, 542388684, 387420489, 9686061319 }
            ,{ L_, 3573323304204814580, 542388684, 387420489,
                                                          9223372034054775807 }
            ,{ L_, 3573323304204814581, 542388684, 387420489,
                                                          9223372034054775809 }
            // Frequency: 387420490, Initial Time: 542388686
            ,{ L_, 542388687, 542388686, 387420490, 2 }
            ,{ L_, 4294967294, 542388686, 387420490, 9686061281 }
            ,{ L_, 4294967295, 542388686, 387420490, 9686061284 }
            ,{ L_, 4294967296, 542388686, 387420490, 9686061287 }
            ,{ L_, 4294967297, 542388686, 387420490, 9686061289 }
            ,{ L_, 3573323313428186614, 542388686, 387420490,
                                                          9223372034054775801 }
            ,{ L_, 3573323313428186615, 542388686, 387420490,
                                                          9223372034054775804 }
            // Frequency: 387420488, Initial Time: 542388683
            ,{ L_, 542388684, 542388683, 387420488, 2 }
            ,{ L_, 4294967294, 542388683, 387420488, 9686061339 }
            ,{ L_, 4294967295, 542388683, 387420488, 9686061342 }
            ,{ L_, 4294967296, 542388683, 387420488, 9686061344 }
            ,{ L_, 4294967297, 542388683, 387420488, 9686061347 }
            ,{ L_, 3573323294981442544, 542388683, 387420488,
                                                          9223372034054775804 }
            ,{ L_, 3573323294981442545, 542388683, 387420488,
                                                          9223372034054775807 }
            // Frequency: 1162261467, Initial Time: 1627166053
            ,{ L_, 4294967294, 1627166053, 1162261467, 2295353770 }
            ,{ L_, 4294967295, 1627166053, 1162261467, 2295353771 }
            ,{ L_, 4294967296, 1627166053, 1162261467, 2295353772 }
            ,{ L_, 4294967297, 1627166053, 1162261467, 2295353773 }
            ,{ L_, 9223372036854775806, 1627166053, 1162261467,
                                                          7935711797307317728 }
            ,{ L_, 9223372036854775807, 1627166053, 1162261467,
                                                          7935711797307317729 }
            // Frequency: 1162261468, Initial Time: 1627166055
            ,{ L_, 4294967294, 1627166055, 1162261468, 2295353767 }
            ,{ L_, 4294967295, 1627166055, 1162261468, 2295353768 }
            ,{ L_, 4294967296, 1627166055, 1162261468, 2295353768 }
            ,{ L_, 4294967297, 1627166055, 1162261468, 2295353769 }
            ,{ L_, 9223372036854775806, 1627166055, 1162261468,
                                                          7935711790479497984 }
            ,{ L_, 9223372036854775807, 1627166055, 1162261468,
                                                          7935711790479497985 }
            // Frequency: 1162261466, Initial Time: 1627166052
            ,{ L_, 4294967294, 1627166052, 1162261466, 2295353773 }
            ,{ L_, 4294967295, 1627166052, 1162261466, 2295353774 }
            ,{ L_, 4294967296, 1627166052, 1162261466, 2295353775 }
            ,{ L_, 4294967297, 1627166052, 1162261466, 2295353776 }
            ,{ L_, 9223372036854775806, 1627166052, 1162261466,
                                                          7935711804135137483 }
            ,{ L_, 9223372036854775807, 1627166052, 1162261466,
                                                          7935711804135137484 }
            // Frequency: 3486784401, Initial Time: 4881498161
            ,{ L_, 9223372036854775806, 4881498161, 3486784401,
                                                          2645237264835772576 }
            ,{ L_, 9223372036854775807, 4881498161, 3486784401,
                                                          2645237264835772576 }
            // Frequency: 3486784402, Initial Time: 4881498162
            ,{ L_, 9223372036854775806, 4881498162, 3486784402,
                                                          2645237264077125937 }
            ,{ L_, 9223372036854775807, 4881498162, 3486784402,
                                                          2645237264077125937 }
            // Frequency: 3486784400, Initial Time: 4881498160
            ,{ L_, 9223372036854775806, 4881498160, 3486784400,
                                                          2645237265594419215 }
            ,{ L_, 9223372036854775807, 4881498160, 3486784400,
                                                          2645237265594419215 }

        };

        const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        {
            if (verbose) cout << "\nCheck fakeConvertRawTime arithmetic"
                              << "\n-----------------------------------"
                              << endl;

            for (int si = 0; si < NUM_DATA; ++si) {
                const int LINE = DATA[si].d_line;
                const bsls::Types::Int64 INPUT        = DATA[si].d_input;
                const bsls::Types::Int64 INITIAL_TIME = DATA[si].d_initialTime;
                const bsls::Types::Int64 FREQUENCY    = DATA[si].d_frequency;
                const bsls::Types::Int64 OUTPUT       = DATA[si].d_output;

                if (veryVerbose) {
                    T_();
                    P_(LINE);
                    P64_(INPUT);
                    P64_(INITIAL_TIME);
                    P64_(FREQUENCY);
                    P64(OUTPUT)
                }

                LOOP5_ASSERT(LINE,
                             INPUT,
                             INITIAL_TIME,
                             FREQUENCY,
                             OUTPUT,
                             OUTPUT == fakeConvertRawTime(INPUT,
                                                         INITIAL_TIME,
                                                         FREQUENCY));
            }
        }

        {
            if (verbose)
                cout << "\nCompare fakeConvertRawTime to convertRawTime"
                     << " with static data"
                     << "\n--------------------------------------------"
                     << "-----------------"
                     << endl;

            const bsls::Types::Int64 frequency = getFrequency();
            const bsls::Types::Int64 testPeriod = getTestPeriod(frequency);

            if (veryVerbose) {
                T_(); P64(frequency);
            }

            TU::OpaqueNativeTime startTime;
            TU::initialize();
            TU::getTimerRaw(&startTime);
            for (int exponent = 0; exponent < 63; ++exponent) {

                const bsls::Types::Int64 offset = (1LL << exponent);
                if (offset > testPeriod - startTime.d_opaque) {
                    // endTime would not be expressible in nanoseconds
                    break;
                }

                if (veryVerbose) {
                    T_(); P_(exponent); P64(offset);
                }

                compareRealToFakeConvertRawTime(startTime, offset, frequency);
            }

            compareRealToFakeConvertRawTime(startTime, 1, frequency);
            const bsls::Types::Int64 limit32Bits = 1LL << 32;
            if (startTime.d_opaque < limit32Bits - 2) {
                compareRealToFakeConvertRawTime(
                                          startTime,
                                          limit32Bits - 2 - startTime.d_opaque,
                                          frequency);
                compareRealToFakeConvertRawTime(
                                          startTime,
                                          limit32Bits - 1 - startTime.d_opaque,
                                          frequency);
                compareRealToFakeConvertRawTime(startTime,
                                                limit32Bits - 2,
                                                frequency);
                compareRealToFakeConvertRawTime(
                                          startTime,
                                          limit32Bits + 1 - startTime.d_opaque,
                                          frequency);
            }
            compareRealToFakeConvertRawTime(
                                           startTime,
                                           testPeriod - 1 - startTime.d_opaque,
                                           frequency);
            compareRealToFakeConvertRawTime(startTime,
                                            testPeriod - startTime.d_opaque,
                                            frequency);
        }

        {
            if (verbose)
                cout << "\nCompare fakeConvertRawTime "
                        "to convertRawTime with random data"
                     << "\n---------------------------"
                        "----------------------------------"
                     << endl;

            srand((unsigned) time(NULL));

            const bsls::Types::Int64 frequency = getFrequency();
            const bsls::Types::Int64 testPeriod = getTestPeriod(frequency);

            if (veryVerbose) {
                T_(); P64(frequency);
            }

            TU::OpaqueNativeTime startTime;
            TU::initialize();
            TU::getTimerRaw(&startTime);


            const Int64 NUM_TESTS  = verbose
                ? (veryVerbose ? 1L : 1000LL) * atoi(argv[2])
                : 100;
            for (int bits = 0;
                 bits < 64 &&
                 (bits == 0 ||
                     (bsls::Types::Int64) getBitMask(bits - 1) < testPeriod);
                 ++bits) {

                if (veryVerbose) { P(bits); }

                for (int iterations = 0;
                     iterations < NUM_TESTS && iterations < 1 << bits;
                     ++iterations) {

                    const bsls::Types::Int64 offset
                        = (getRand64() & getBitMask(bits)) % testPeriod;

                    if (veryVerbose) {
                        T_(); P_(bits); P_(iterations); P64(offset);
                    }

                    compareRealToFakeConvertRawTime(startTime,
                                                    offset,
                                                    frequency);
                }
            }
        }
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING getTimerRaw() and convertRawTime()
        //
        // Concerns:
        //   The return values from two successive calls to 'getTimerRaw()'
        //   separated by zero or more simple operations, converted to
        //   nanoseconds and subtracted, should produce non-negative results.
        //   The concern of "reasonable" results is difficult to program and
        //   for now will be observed manually.
        //
        // Plan:
        //   Construct blocks containing two calls to 'getTimerRaw()'
        //   bracketing various non-trivial (i.e., non-removable by an
        //   optimizer) statements.  ASSERT that the differences of the
        //   converted return values are always non-negative.  In verbose mode,
        //   print the elapsed times so that they can be observed to be
        //   "reasonable"
        //
        // Testing:
        //   bsls::TimeUtil::getTimerRaw()
        //   bsls::TimeUtil::convertRawTime(bsls::TimeUtil::OpaqueNativeTime)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Raw methods"
                          << "\n===================" << endl;

        {
            // ---------------------------------------------------------------
            // This "warmup" section added to explore strange initial results
            // on Sun in particular, where the first block, timing one
            // division, was reproducibly slower than the same block but with
            // two divisions.
            TU::OpaqueNativeTime idle;
            TU::getTimerRaw(&idle);
            double               primeThePump = 12345/67 / 8.901;
            if (veryVeryVerbose)
                std::cout << "Warmup: " << primeThePump << std::endl;
            // ---------------------------------------------------------------
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;

            TU::getTimerRaw(&t1);
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "0: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x;

            TU::getTimerRaw(&t1);
            x  = argc * 355.0 / 113.0;
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "1: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:"
                                       << x << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y;

            TU::getTimerRaw(&t1);
            x = argc * 355.2 / 113.3;
            y = argc * x / 12.0;
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "2: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:"
                                       << x << "  " << y << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y, z;

            TU::getTimerRaw(&t1);
            x = argc * 355.1 / 113.7;
            y = (x * x) / (argc * 12.2);
            z = (x + 3) / (x * y + 4) + 2.0 * x;
            TU::getTimerRaw(&t2);

            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "3: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:" << x << "  "
                                       << y << "  " << z << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y, z;

            TU::getTimerRaw(&t1);
            x = argc * 355.1 / 113.7;
            for (int i = 0; i < 10; ++i) {
                y = (x * 1.2) / 12.2;
                z = (x + 3) / (x * y + 4) + 2.0 * x;
                x = y + z;
            }
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "10: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:" << x << "  "
                                       << y << "  " << z << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y, z;

            TU::getTimerRaw(&t1);
            x = argc * 355.1 / 113.7;
            for (int i = 0; i < 100; ++i) {
                y = (x * 1.2) / 12.2;
                z = (x + 3) / (x * y + 4) + 2.0 * x;
                x = y + z;
            }
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "100: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:" << x << "  "
                                       << y << "  " << z << std::endl;
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** Windows only ***: bsls::TimeUtil::getTimer()
        //
        // Plan:
        //   Call the method and check that the return value is not negative
        //   (the uninitialized values for s_initialTime and s_timerFrequency
        //   are -1).
        //
        // Testing:
        //   bsls::TimeUtil::getTimer()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS
        if (verbose)
            cout << "\nTesting TimeUtil initialization (wall timer)"
                 << "\n============================================" << endl;

        Int64 t = TU::getTimer();
        if (verbose) { T_(); P64(t); }
        ASSERT(t >= 0);
#endif

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** UNIX only ***: bsls::TimeUtil::getProcessTimers()
        //
        // Plan:
        //   Call the method and check that the values returned are not
        //   negative (the uninitialized factor value is -1).
        //
        // Testing:
        //   bsls::TimeUtil::getProcessTimers()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose)
            cout << "\nTesting TimeUtil initialization (process timers)"
                 << "\n================================================"
                 << endl;

        Int64 ts, tu; TU::getProcessTimers(&ts, &tu);
        if (verbose) { T_(); P64_(ts); P64(tu); }
        ASSERT(ts >= 0); ASSERT(tu >= 0);
#endif

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** UNIX only ***: bsls::TimeUtil::getProcessUserTimer()
        //
        // Plan:
        //   Call the method and check that the return value is not negative
        //   (the uninitialized factor value is -1).
        //
        // Testing:
        //   bsls::TimeUtil::getProcessUserTimer()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose)
            cout << "\nTesting TimeUtil initialization (user timer)"
                 << "\n============================================" << endl;

        Int64 t = TU::getProcessUserTimer();
        if (verbose) { T_(); P64(t); }
        ASSERT(t >= 0);
#endif

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** UNIX only ***: bsls::TimeUtil::getProcessSystemTimer()
        //
        // Plan:
        //   Call the method and check that the return value is not negative
        //   (the uninitialized factor value is -1).
        //
        // Testing:
        //   bsls::TimeUtil::getProcessSystemTimer()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose)
            cout << "\nTesting TimeUtil initialization (system timer)"
                 << "\n==============================================" << endl;

        Int64 t = TU::getProcessSystemTimer();
        if (verbose) { T_(); P64(t); }
        ASSERT(t >= 0);
#endif

      } break;
      case 5: {
#if defined (BSLS_PLATFORM__OS_SOLARIS) || defined (BSLS_PLATFORM__OS_HPUX)
        // --------------------------------------------------------------------
        // PERFORMANCE TEST 'gethrtime()' *** Sun and HP ONLY ***
        //   Test whether successive calls ever return non-increasing values.
        //
        // Plan:
        //   Call 'gethrtime()' two times within a loop, and compare the return
        //   values each time.
        //
        // Testing:
        //   'gethrtime()'
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'gethrtime()' statistical correctness"
                 << "\n=============================================" << endl;

        if (verbose)
            cout << "\nCall 'gethrtime()' twice in a large loop."
                 << endl;
        {
            const Int64 NUM_TESTS = verbose ? 100LL * atoi(argv[2]) : 100;

            const Int64 t0 = TU::getTimer();
            Int64       t1, t2;
            int         numSame  = 0;
            int         numWrong = 0;

            for (Int64 i = 1; i < NUM_TESTS; ++i) {
                t1 = (Int64) gethrtime();
                t2 = (Int64) gethrtime();
                // Int64 dt = t2 - t1;  // these lines retained for
                // P64(dt);             //  non-production testing.
                if (t2 <= t1) {  // Optimize for CORRECT behavior
                    if (t2 == t1) {
                        ++numSame;
                        if (verbose) { T_();  P_(i);  P(numSame); }
                    }
                    else {
                        ++numWrong;
                        if (verbose) {
                            T_();      P64_(i);   P_(numWrong);
                            P64_(t1);  P64_(t2);  P64(t2 - t1);
                        }
                    }
                }
            }
            double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
            double numCalls    = ((double) NUM_TESTS) * 1.0e-6;
            double timePerCall = elapsedTime / (numCalls * 2.0); //(microsec.)
            if (verbose) {
                cout << "\telapsed time   = " << elapsedTime << " (sec)\n"
                     << "\tnum tests     = " << numCalls << " x 10^6\n"
                     << "\ttime per call  = " << timePerCall << " (nsec)\n"
                     << "\n\tnum non-monotonic pairs   = " << numWrong
                     << "\n\tnum pairs with same value = " << numSame << endl;
            }
        }

#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // HOOKING TEST
        // Concerns:
        //   That the wall time, user time, and system time calls are making
        //   the correct calls into the OS.
        //
        // Plan:
        //   1. Take all the values, then sleep, then check how they changed.
        //   Verify that the values returned to us are compatible with the
        //   OS timers that the calls should be returning.
        //   2. Make some repeated time calls; check how they changed.  Verify
        //   that the values returned to us are compatible with the OS timers
        //   that the calls should be returning.
        //   3. Burn some CPU time, then check how they changed.  Verify that
        //   the values returned to us are compatible with the OS timers that
        //   the calls should be returning.
        //   (These are imperfect tests; we can sleep longer than we expect,
        //   and interrupt and scheduling activities can add to the user and
        //   system times.)
        //
        // Testing:
        //   bsls::Types::Int64 bsls::TimeUtil::getTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessSystemTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessUserTimer();
        // --------------------------------------------------------------------

        const Int64 nsecsPerSec = 1000LL * 1000LL * 1000LL;

        //  The initialization of 'timeQuantum' varies from OS to OS.
        //  'timeQuantum' is to be the minimum increment visible in a
        //  timer, expressed as a number of nanoseconds.  POSIX wants it
        //  to be 100 milliseconds.
#if defined BSLS_PLATFORM__OS_SOLARIS || defined BSLS_PLATFORM__OS_FREEBSD
        const Int64 timeQuantum = nsecsPerSec / CLK_TCK;
#elif defined BSLS_PLATFORM__OS_LINUX || defined BSLS_PLATFORM__OS_AIX \
   || defined BSLS_PLATFORM__OS_HPUX || defined(BSLS_PLATFORM__OS_DARWIN)
        const Int64 timeQuantum = nsecsPerSec / sysconf(_SC_CLK_TCK);
                                        // On our local flavor of Linux, old
                                        // POSIX requirements and immoderate
                                        // file system cleverness combine to
                                        // make the symbol that is available
                                        // wrong (CLOCKS_PER_SEC == 1000000)
                                        // and the symbol that is correct
                                        // (CLK_TCK) unavailable.
                                        // (AIX just walks its own path.)
#elif defined BSLS_PLATFORM__OS_WINDOWS
        const Int64 timeQuantum = 100;  // Hard-coded from Windows
                                        // documentation.  We need to test
                                        // (not pre-accept)
                                        // 'UnixTimerUtil::s_nsecsPerSecond'.

        const Int64 windowsFudge = 15 * 1000000LL;
                                        // This interval (15 milliseconds) is
                                        // an experimentally determined
                                        // allowance for lack of
                                        // synchronization between the Windows
                                        // wall-time timer and the Windows
                                        // processor time counts.
#else
# error Need platform info to find time quantum!
        const Int64 timeQuantum[0];     // There is an error here, but
                                        // the '#error' directive is not
                                        // available everywhere.
#endif

        // First subtest: Get values, sleep, get values, check that
        // they make sense for the OS counters they are supposed to be.
        {

            if (verbose)
                cout << "\nHooking Test: System time requests across sleep."
                     << "\n================================================"
                     << endl;

            if (veryVerbose)
                cout << "timeQuantum = " << timeQuantum << endl;

            const unsigned shortSleep = 2;
            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            osSleep(shortSleep);

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose)
                cout
                    << "wt1: " << wt1 << " "
                    << "ut1: " << ut1 << " "
                    << "st1: " << st1 << std::endl
                    << "wt2: " << wt2 << " "
                    << "ut2: " << ut2 << " "
                    << "st2: " << st2 << std::endl
                    << "wt2 - wt1: " << wt2 - wt1 << std::endl;

            // System and user time differences must be zero mod quantum, or
            // quantum is wrong.

            ASSERT(0 == (ut2 - ut1) % timeQuantum);
            ASSERT(0 == (st2 - st1) % timeQuantum);

            // Comparisons on times must be written in such a way that rollover
            // can never corrupt the result.  (With 'long long' values, this is
            // unlikely, but not eventually impossible.)  This means that
            // the times must be grouped on one side of the inequality and
            // their differences taken, and those differences compared to
            // whatever constants are involved.

#if !defined(BSLS_PLATFORM__OS_WINDOWS)
            ASSERT(wt2 - wt1 >= shortSleep * nsecsPerSec);
#else
            ASSERT(wt2 - wt1 + windowsFudge >= shortSleep * nsecsPerSec);
#endif
                                               // Did we sleep long enough?

            ASSERT(ut2 - ut1 >= 0);            // User time must not go
                                               // backwards ...
            ASSERT(ut2 - ut1 <= timeQuantum);  // ... nor forwards by too much.
            ASSERT(st2 - st1 >= 0);            // System time must not go
                                               // backwards ...
            ASSERT(st2 - st1 <= timeQuantum);  // ... nor forwards by too much.

        }

        // Second subtest: Make repeated time calls; check for consistency.
        {
            if (verbose)
                cout << "\nHooking Test: System time requests repeated."
                     << "\n============================================"
                     << endl;

            enum { NUM_INTERVALS = 10,
                   NUM_SAMPLES = NUM_INTERVALS + 1 };
            struct sample {
                Int64 d_wt;  // wall time
                Int64 d_ut;  // user time
                Int64 d_st;  // system time
            } samples[NUM_SAMPLES];

            samples[0].d_wt = TU::getTimer();
            samples[0].d_ut = TU::getProcessUserTimer();
            samples[0].d_st = TU::getProcessSystemTimer();

            // Time differences must remain sane over repeated calls.

            Int64 uQuantsSpent = 0;
            Int64 sQuantsSpent = 0;
            for (int i = 0; i < NUM_INTERVALS; ++i) {
                sample& s0 = samples[i];
                sample& s1 = samples[i+1];

                s1.d_wt = TU::getTimer();
                s1.d_ut = TU::getProcessUserTimer();
                s1.d_st = TU::getProcessSystemTimer();

                // Sys and user differences must be zero mod quantum, or
                // quantum is wrong.

                ASSERT(0 == (s1.d_ut - s0.d_ut) % timeQuantum);
                ASSERT(0 == (s1.d_st - s0.d_st) % timeQuantum);

                uQuantsSpent += (s1.d_ut - s0.d_ut) / timeQuantum;
                sQuantsSpent += (s1.d_st - s0.d_st) / timeQuantum;

                // We should not have spent much time in those, except possibly
                // for wall time: we might have been descheduled.  In any case,
                // time must not go backward.

                ASSERT(s1.d_wt - s0.d_wt >= 0);
                ASSERT(s1.d_ut - s0.d_ut >= 0
                    && s1.d_ut - s0.d_ut <= timeQuantum);
                ASSERT(s1.d_st - s0.d_st >= 0
                    && s1.d_st - s0.d_st <= timeQuantum);
            }

            // We should not have spent much time getting the time ten times.

            if (veryVerbose)
                cout << "uQuantsSpent: " << uQuantsSpent << "    "
                     << "sQuantsSpent: " << sQuantsSpent << endl;

            ASSERT(uQuantsSpent <= 1);
            ASSERT(sQuantsSpent <= 1);

            if (veryVerbose) {
                cout << " 0: " << "wt: " << samples[0].d_wt << endl
                     << " 0: " << "ut: " << samples[0].d_ut << endl
                     << " 0: " << "st: " << samples[0].d_st << endl;

                for (int i = 1; i < NUM_SAMPLES; ++i) {
                    sample& s0 = samples[i - 1];
                    sample& s1 = samples[i];

                    cout << std::setw(2) << i << ": "
                                             << "wt: " << s1.d_wt
                                             << " - "  << s0.d_wt
                                             << " = "  << s1.d_wt - s0.d_wt
                                                                    << endl;
                    cout << std::setw(2) << i << ": "
                                            << "ut: " << s1.d_ut
                                            << " - "  << s0.d_ut
                                            << " = "  << s1.d_ut - s0.d_ut
                                                                    << endl;
                    cout << std::setw(2) << i << ": "
                                            << "st: " << s1.d_st
                                            << " - "  << s0.d_st
                                            << " = "  << s1.d_st - s0.d_st
                                                                    << endl;
                }
            }

        }

        // Third subtest: Consume time; verify that it appears in the counts.
        {
            const unsigned longLoop = 8 * 1000L * 1000L;

            // Burn a little processor time--at least one timeQuantum.
            // The 'volatile' should force every access to memory and slow
            // things up a little, or a lot.  It may make the test less
            // dependent on ever-increasing processor speeds.

            if (verbose)
                cout << "\nHooking Test: System time requests over long loop."
                     << "\n=================================================="
                     << endl;

            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            for (volatile unsigned u = 0; u < longLoop; ++u) {
                ++u; --u;
            }

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose)
                cout << "wt2: " << wt2       << " -  "
                     << "wt1: " << wt1       << " = "
                                << wt2 - wt1 << std::endl
                     << "ut2: " << ut2       << " -  "
                     << "ut1: " << ut1       << " = "
                                << ut2 - ut1 << std::endl
                     << "st2: " << st2       << " -  "
                     << "st1: " << st1       << " = "
                                << st2 - st1 << std::endl << endl;

            ASSERT(wt2 - wt1 >= timeQuantum);
                                        // Our wall time is over a timeQuantum.

            ASSERT(ut2 - ut1 >= timeQuantum);
                                        // We ran at least a timeQuantum.

            ASSERT(st2 - st1 >= 0);     // And system time did not go backward.

#if !defined(BSLS_PLATFORM__OS_WINDOWS)
            ASSERT((wt2 - wt1) - (ut2 - ut1) - (st2 - st1) +
                                                         2 * timeQuantum >= 0);
                                        // And our wall time was greater than
                                        // our user and system time together,
                                        // allowing for quantization error
                                        // (in both user and system time).
#else
            ASSERT((wt2 - wt1) - (ut2 - ut1) - (st2 - st1)
                                  + 10 * windowsFudge + 2 * timeQuantum >= 0);
#endif

        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //   Test whether successive calls ever return the same value.
        //
        // Plan:
        //   Call each method two times within a loop, and compare the return
        //   values each time.
        //
        // Testing:
        //   bsls::Types::Int64 bsls::TimeUtil::getTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessSystemTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessUserTimer();
        //   void bsls::TimeUtil::getProcessTimers(bsls::Types::Int64,
        //                                        bsls::Types::Int64);
        // --------------------------------------------------------------------

        struct {
            TimerMethod d_method;
            std::string d_methodName;
            bool        d_reportSame;
                // don't report same values for user and system timers
        }
        TimerMethods[] = {
            { TU::getTimer,                 "getTimer",                true  },
            { TU::getProcessSystemTimer,    "getProcessSystemTimer",   false },
            { TU::getProcessUserTimer,      "getProcessUserTimer",     false },
            { callGetProcessTimersRetSystem,"getProcessTimers(system)",false },
            { callGetProcessTimersRetUser,  "getProcessTimers(user)",  false }
        };
        size_t const TimerMethodsCount =
                                  sizeof TimerMethods / sizeof TimerMethods[0];

        for (size_t t = 0; t < TimerMethodsCount; ++t) {
            if (verbose)
                cout << "\nTesting '"
                    << TimerMethods[t].d_methodName
                    << "()' statistical correctness"
                    << "\n===========================================" << endl;

            if (verbose)
                cout << "\nCall 'bsls::TimeUtil::"
                    << TimerMethods[t].d_methodName
                    << "()' twice in a large loop."
                    << endl;
            {
                const Int64 NUM_TESTS  = verbose ? 100LL * atoi(argv[2]) : 100;

                const Int64 t0 = TU::getTimer(); //measure the tool with itself
                Int64       t1, t2;
                int         numSame  = 0;
                int         numWrong = 0;

                for (Int64 i = 1; i < NUM_TESTS; ++i) {
                    t1 = TimerMethods[t].d_method();
                    t2 = TimerMethods[t].d_method();

                    //t1 = (Int64) gethrtime();  // These are saved for future
                    //t2 = (Int64) gethrtime();  // "paranoid" tests.

                    if (t2 <= t1) {  // Optimize for CORRECT behavior
                        if (t2 == t1) {
                            if (TimerMethods[t].d_reportSame) {
                                ++numSame;
                                if (verbose) { T_();  P_(i);  P(numSame); }
                            }
                        }
                        else {
                            ++numWrong;
                            if (verbose) {
                                T_();      P64_(i);   P_(numWrong);
                                P64_(t1);  P64_(t2);  P64(t2 - t1);
                            }
                        }
                    }
                }
                LOOP_ASSERT(t, 0 == numWrong);

                double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
                double numCalls    = ((double) NUM_TESTS) * 1.0e-6;
                double timePerCall = elapsedTime / (numCalls * 2.0);//microsec
                if (verbose) {
                    cout << "\telapsed time   = " << elapsedTime << " (sec)\n"
                        << "\tnum tests     = " << numCalls << " x 10^6\n"
                        << "\ttime per call  = " << timePerCall << " (nsec)\n"
                        << "\n\tnum non-monotonic pairs   = " << numWrong
                        << "\n\tnum pairs with same value = " << numSame
                        << endl;
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //   Time the (platform-dependent) call.
        //
        // Plan:
        //   Call each method in a loop.
        //
        // Testing:
        //   bsls::Types::Int64 bsls::TimeUtil::getTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessSystemTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessUserTimer();
        //   void bsls::TimeUtil::getProcessTimers(bsls::Types::Int64,
        //                                        bsls::Types::Int64);
        // --------------------------------------------------------------------

        struct {
            TimerMethod d_method;
            std::string d_methodName;
        }
        TimerMethods[] = {
            { TU::getTimer,                  "getTimer"                 },
            { TU::getProcessSystemTimer,     "getProcessSystemTimer"    },
            { TU::getProcessUserTimer,       "getProcessUserTimer"      },
            { callGetProcessTimersRetSystem, "getProcessTimers(system)" },
            { callGetProcessTimersRetUser,   "getProcessTimers(user)"   }
        };
        size_t const TimerMethodsCount =
                                  sizeof TimerMethods / sizeof TimerMethods[0];

        for (size_t t = 0; t < TimerMethodsCount; ++t) {
            if (verbose)
                cout << "\nTesting 'bsls::TimeUtil::"
                    << TimerMethods[t].d_methodName
                    << " Performance()'"
                    << "\n==============================================="
                    << endl;

            if (verbose)
                cout << "\nCall 'bsls::TimeUtil::"
                    << TimerMethods[t].d_methodName
                    << "()' in a large loop."
                    << endl;
            {
                const Int64 NUM_STEPS  = verbose ? 10LL * atoi(argv[2]) : 10;
                const Int64 t0 = TU::getTimer(); //measure the tool with itself

                for (Int64 i = 1; i < NUM_STEPS; ++i) {
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                }
                double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
                double numCalls    = ((double) NUM_STEPS * 10.0) * 1.0e-6;
                double timePerCall = elapsedTime / numCalls; // (microsec.)
                if (verbose) {
                    cout << "\telapsed time  = " << elapsedTime << " (sec)\n"
                        << "\tnum calls    = " << numCalls << " x 10^6\n"
                        << "\ttime per call = " << timePerCall << " (usec)"
                        << endl;
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC FUNCTIONALITY
        //   The methods should return non-decreasing values of type
        //   'bsls::Types::Int64'.
        //
        // Plan:
        //   Verify that the *temporary* returned by each method is at least 8
        //   bytes long.  Then invoke the method several times in sequence,
        //   separated by delay loops of increasing duration, and assert that
        //   the return value does not decrease.  Print results and differences
        //   in 'veryVerbose' mode.
        //
        // Testing:
        //   bsls::Types::Int64 bsls::TimeUtil::getTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessSystemTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessUserTimer();
        //   void bsls::TimeUtil::getProcessTimers(bsls::Types::Int64,
        //                                        bsls::Types::Int64);
        // --------------------------------------------------------------------

        struct {
            TimerMethod d_method;
            std::string d_methodName;
        }
        TimerMethods[] = {
            { TU::getTimer,                  "getTimer"                 },
            { TU::getProcessSystemTimer,     "getProcessSystemTimer"    },
            { TU::getProcessUserTimer,       "getProcessUserTimer"      },
            { callGetProcessTimersRetSystem, "getProcessTimers(system)" },
            { callGetProcessTimersRetUser,   "getProcessTimers(user)"   }
        };
        size_t const TimerMethodsCount =
                                  sizeof TimerMethods / sizeof TimerMethods[0];

        for (size_t t = 0; t < TimerMethodsCount; ++t) {
            if (verbose)
                cout << "\nTesting '"
                     << TimerMethods[t].d_methodName
                     << "()'"
                     << "\n====================" << endl;

            if (verbose)
                cout << "\nConfirm sizeof ("
                     << TimerMethods[t].d_methodName
                     << "())."
                     << endl;
            {
                int numBytes = sizeof(TimerMethods[t].d_method());
                if (veryVerbose) { T_(); P(numBytes); }
                LOOP_ASSERT(t, 8 <= numBytes);
            }

            if (verbose)
                cout << "\nExercise '"
                     << TimerMethods[t].d_methodName
                     << "()'." << endl;
            {
                const int NUM_CALLS   = 10;
                const int DELAY_COUNT = 1000000;
                Int64     result[NUM_CALLS];
                double    totalElapsedTime = 0.0;

                result[0] = TimerMethods[t].d_method();
                if (veryVerbose) { T_();  P64(result[0]); }
                for (int i = 1; i < NUM_CALLS; ++i) {
                    for (int j = 0; j < DELAY_COUNT * i; ++j) {
                        i = i + 1;
                        i = i - 1;
                    }
                    result[i] = TimerMethods[t].d_method();
                    Int64 difference = result[i] - result[i - 1];
                    double seconds = (double) difference / 1.0e9;
                    totalElapsedTime += seconds;
                    if (veryVerbose) {
                        T_();  P64_(result[i]);  P64_(difference);  P(seconds);
                    }
                    LOOP2_ASSERT(t, i, (Int64) 0 <= difference);//cast--dg bug
                }
                if (veryVerbose) { cout << endl;  T_();  P(totalElapsedTime); }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008, 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

