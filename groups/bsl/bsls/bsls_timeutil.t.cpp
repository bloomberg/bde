// bsls_timeutil.t.cpp                                                -*-C++-*-
#include <bsls_timeutil.h>

#include <bsls_bsltestutil.h>
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

// limit ourselves to the "C" library for packages below 'bslstl'
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // for srand

using namespace BloombergLP;

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

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

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
//              #self.frequencies.append(n + 1)
//              #self.frequencies.append(n - 1)
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
//              self.report(frequency,
//                          (1 << 32) - 2 + initialTime,
//                          initialTime)
//              self.report(frequency,
//                          (1 << 32) - 1 + initialTime,
//                          initialTime)
//              self.report(frequency,
//                          (1 << 32) + initialTime,
//                          initialTime)
//              self.report(frequency,
//                          (1 << 32) + 1 + initialTime,
//                          initialTime)
//              self.report(frequency,
//                          self.maxTicks(frequency, initialTime) - 1,
//                          initialTime)
//              self.report(frequency,
//                          self.maxTicks(frequency, initialTime),
//                          initialTime)
//
//              #for baseValue in [2, 5, 7]:
//              for baseValue in [5]:
//                  for ticks in self.generateTicks(baseValue, initialTime):
//                      self.report(frequency, ticks, initialTime)
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
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

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

        if (verbose) printf("\nTesting Usage Example"
                            "\n=====================\n");

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
                printf("elapsed wall time: %g\n"
                       "elapsed user time: %g\n"
                       "elapsed system time: %g\n",
                       dTw, dTu, dTs);
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

        if (verbose) printf("\nTesting convertRawTime arithmetic"
                            "\n=================================\n");

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
            ,{ L_, 10307921507, 6012954213, 4294967295, 999999999 }
            ,{ L_, 10307921508, 6012954213, 4294967295, 1000000000 }
            ,{ L_, 10307921509, 6012954213, 4294967295, 1000000000 }
            ,{ L_, 10307921510, 6012954213, 4294967295, 1000000000 }
            ,{ L_, 9223372036854775806, 6012954213, 4294967295,
                                                          2147483647099999999 }
            ,{ L_, 9223372036854775807, 6012954213, 4294967295,
                                                          2147483647099999999 }
            ,{ L_, 6103515625, 6012954213, 4294967295, 21085471 }
            ,{ L_, 30517578125, 6012954213, 4294967295, 5705427359 }
            ,{ L_, 152587890625, 6012954213, 4294967295, 34127136796 }
            ,{ L_, 762939453125, 6012954213, 4294967295, 176235683981 }
            ,{ L_, 3814697265625, 6012954213, 4294967295, 886778419906 }
            ,{ L_, 19073486328125, 6012954213, 4294967295, 4439492099534 }
            ,{ L_, 95367431640625, 6012954213, 4294967295, 22203060497673 }
            ,{ L_, 476837158203125, 6012954213, 4294967295, 111020902488365 }
            ,{ L_, 2384185791015625, 6012954213, 4294967295, 555110112441825 }
            ,{ L_, 11920928955078125, 6012954213, 4294967295,
                                                          2775556162209126 }
            ,{ L_, 59604644775390625, 6012954213, 4294967295,
                                                          13877786411045631 }
            ,{ L_, 298023223876953125, 6012954213, 4294967295,
                                                          69388937655228155 }
            ,{ L_, 1490116119384765625, 6012954213, 4294967295,
                                                          346944693876140775 }
            ,{ L_, 7450580596923828125, 6012954213, 4294967295,
                                                          1734723474980703877 }

            // Frequency: 2992530000, Initial Time: 4189542000
            ,{ L_, 8484509294, 4189542000, 2992530000, 1435229486 }
            ,{ L_, 8484509295, 4189542000, 2992530000, 1435229486 }
            ,{ L_, 8484509296, 4189542000, 2992530000, 1435229486 }
            ,{ L_, 8484509297, 4189542000, 2992530000, 1435229487 }
            ,{ L_, 9223372036854775806, 4189542000, 2992530000,
                                                          3082131852534555645 }
            ,{ L_, 9223372036854775807, 4189542000, 2992530000,
                                                          3082131852534555645 }
            ,{ L_, 6103515625, 4189542000, 2992530000, 639583771 }
            ,{ L_, 30517578125, 4189542000, 2992530000, 8797918859 }
            ,{ L_, 152587890625, 4189542000, 2992530000, 49589594298 }
            ,{ L_, 762939453125, 4189542000, 2992530000, 253547971490 }
            ,{ L_, 3814697265625, 4189542000, 2992530000, 1273339857453 }
            ,{ L_, 19073486328125, 4189542000, 2992530000, 6372299287266 }
            ,{ L_, 95367431640625, 4189542000, 2992530000, 31867096436334 }
            ,{ L_, 476837158203125, 4189542000, 2992530000, 159341082181674 }
            ,{ L_, 2384185791015625, 4189542000, 2992530000, 796711010908370 }
            ,{ L_, 11920928955078125, 4189542000, 2992530000,
                                                          3983560654541850 }
            ,{ L_, 59604644775390625, 4189542000, 2992530000,
                                                          19917808872709254 }
            ,{ L_, 298023223876953125, 4189542000, 2992530000,
                                                          99589049963546271 }
            ,{ L_, 1490116119384765625, 4189542000, 2992530000,
                                                          497945255417731359 }
            ,{ L_, 7450580596923828125, 4189542000, 2992530000,
                                                          2489726282688656797 }

            // Frequency: 3579545, Initial Time: 5011363
            ,{ L_, 5011364, 5011363, 3579545, 279 }
            ,{ L_, 4299978657, 5011363, 3579545, 1199864031322 }
            ,{ L_, 4299978658, 5011363, 3579545, 1199864031601 }
            ,{ L_, 4299978659, 5011363, 3579545, 1199864031881 }
            ,{ L_, 4299978660, 5011363, 3579545, 1199864032160 }
            ,{ L_, 33015475252651964, 5011363, 3579545, 9223372034054775397 }
            ,{ L_, 33015475252651965, 5011363, 3579545, 9223372034054775676 }
            ,{ L_, 9765625, 5011363, 3579545, 1328174949 }
            ,{ L_, 48828125, 5011363, 3579545, 12240874748 }
            ,{ L_, 244140625, 5011363, 3579545, 66804373740 }
            ,{ L_, 1220703125, 5011363, 3579545, 339621868701 }
            ,{ L_, 6103515625, 5011363, 3579545, 1703709343505 }
            ,{ L_, 30517578125, 5011363, 3579545, 8524146717529 }
            ,{ L_, 152587890625, 5011363, 3579545, 42626333587648 }
            ,{ L_, 762939453125, 5011363, 3579545, 213137267938243 }
            ,{ L_, 3814697265625, 5011363, 3579545, 1065691939691217 }
            ,{ L_, 19073486328125, 5011363, 3579545, 5328465298456088 }
            ,{ L_, 95367431640625, 5011363, 3579545, 26642332092280443 }
            ,{ L_, 476837158203125, 5011363, 3579545, 133211666061402217 }
            ,{ L_, 2384185791015625, 5011363, 3579545, 666058335907011086 }
            ,{ L_, 11920928955078125, 5011363, 3579545, 3330291685135055433 }

            // Frequency: 3, Initial Time: 4
            ,{ L_, 5, 4, 3, 333333333 }
            ,{ L_, 4294967298, 4, 3, 1431655764666666666 }
            ,{ L_, 4294967299, 4, 3, 1431655765000000000 }
            ,{ L_, 4294967300, 4, 3, 1431655765333333333 }
            ,{ L_, 4294967301, 4, 3, 1431655765666666666 }
            ,{ L_, 27670116105, 4, 3, 9223372033666666666 }
            ,{ L_, 27670116106, 4, 3, 9223372034000000000 }
            ,{ L_, 5, 4, 3, 333333333 }
            ,{ L_, 25, 4, 3, 7000000000 }
            ,{ L_, 125, 4, 3, 40333333333 }
            ,{ L_, 625, 4, 3, 207000000000 }
            ,{ L_, 3125, 4, 3, 1040333333333 }
            ,{ L_, 15625, 4, 3, 5207000000000 }
            ,{ L_, 78125, 4, 3, 26040333333333 }
            ,{ L_, 390625, 4, 3, 130207000000000 }
            ,{ L_, 1953125, 4, 3, 651040333333333 }
            ,{ L_, 9765625, 4, 3, 3255207000000000 }
            ,{ L_, 48828125, 4, 3, 16276040333333333 }
            ,{ L_, 244140625, 4, 3, 81380207000000000 }
            ,{ L_, 1220703125, 4, 3, 406901040333333333 }
            ,{ L_, 6103515625, 4, 3, 2034505207000000000 }

            // Frequency: 9, Initial Time: 12
            ,{ L_, 13, 12, 9, 111111111 }
            ,{ L_, 4294967306, 12, 9, 477218588222222222 }
            ,{ L_, 4294967307, 12, 9, 477218588333333333 }
            ,{ L_, 4294967308, 12, 9, 477218588444444444 }
            ,{ L_, 4294967309, 12, 9, 477218588555555555 }
            ,{ L_, 83010348318, 12, 9, 9223372034000000000 }
            ,{ L_, 83010348319, 12, 9, 9223372034111111111 }
            ,{ L_, 25, 12, 9, 1444444444 }
            ,{ L_, 125, 12, 9, 12555555555 }
            ,{ L_, 625, 12, 9, 68111111111 }
            ,{ L_, 3125, 12, 9, 345888888888 }
            ,{ L_, 15625, 12, 9, 1734777777777 }
            ,{ L_, 78125, 12, 9, 8679222222222 }
            ,{ L_, 390625, 12, 9, 43401444444444 }
            ,{ L_, 1953125, 12, 9, 217012555555555 }
            ,{ L_, 9765625, 12, 9, 1085068111111111 }
            ,{ L_, 48828125, 12, 9, 5425345888888888 }
            ,{ L_, 244140625, 12, 9, 27126734777777777 }
            ,{ L_, 1220703125, 12, 9, 135633679222222222 }
            ,{ L_, 6103515625, 12, 9, 678168401444444444 }
            ,{ L_, 30517578125, 12, 9, 3390842012555555555 }

            // Frequency: 27, Initial Time: 37
            ,{ L_, 38, 37, 27, 37037037 }
            ,{ L_, 4294967331, 37, 27, 159072862740740740 }
            ,{ L_, 4294967332, 37, 27, 159072862777777777 }
            ,{ L_, 4294967333, 37, 27, 159072862814814814 }
            ,{ L_, 4294967334, 37, 27, 159072862851851851 }
            ,{ L_, 249031044957, 37, 27, 9223372034074074074 }
            ,{ L_, 249031044958, 37, 27, 9223372034111111111 }
            ,{ L_, 125, 37, 27, 3259259259 }
            ,{ L_, 625, 37, 27, 21777777777 }
            ,{ L_, 3125, 37, 27, 114370370370 }
            ,{ L_, 15625, 37, 27, 577333333333 }
            ,{ L_, 78125, 37, 27, 2892148148148 }
            ,{ L_, 390625, 37, 27, 14466222222222 }
            ,{ L_, 1953125, 37, 27, 72336592592592 }
            ,{ L_, 9765625, 37, 27, 361688444444444 }
            ,{ L_, 48828125, 37, 27, 1808447703703703 }
            ,{ L_, 244140625, 37, 27, 9042244000000000 }
            ,{ L_, 1220703125, 37, 27, 45211225481481481 }
            ,{ L_, 6103515625, 37, 27, 226056132888888888 }
            ,{ L_, 30517578125, 37, 27, 1130280669925925925 }
            ,{ L_, 152587890625, 37, 27, 5651403355111111111 }

            // Frequency: 81, Initial Time: 113
            ,{ L_, 114, 113, 81, 12345679 }
            ,{ L_, 4294967407, 113, 81, 53024287580246913 }
            ,{ L_, 4294967408, 113, 81, 53024287592592592 }
            ,{ L_, 4294967409, 113, 81, 53024287604938271 }
            ,{ L_, 4294967410, 113, 81, 53024287617283950 }
            ,{ L_, 747093134871, 113, 81, 9223372034049382716 }
            ,{ L_, 747093134872, 113, 81, 9223372034061728395 }
            ,{ L_, 125, 113, 81, 148148148 }
            ,{ L_, 625, 113, 81, 6320987654 }
            ,{ L_, 3125, 113, 81, 37185185185 }
            ,{ L_, 15625, 113, 81, 191506172839 }
            ,{ L_, 78125, 113, 81, 963111111111 }
            ,{ L_, 390625, 113, 81, 4821135802469 }
            ,{ L_, 1953125, 113, 81, 24111259259259 }
            ,{ L_, 9765625, 113, 81, 120561876543209 }
            ,{ L_, 48828125, 113, 81, 602814962962962 }
            ,{ L_, 244140625, 113, 81, 3014080395061728 }
            ,{ L_, 1220703125, 113, 81, 15070407555555555 }
            ,{ L_, 6103515625, 113, 81, 75352043358024691 }
            ,{ L_, 30517578125, 113, 81, 376760222370370370 }
            ,{ L_, 152587890625, 113, 81, 1883801117432098765 }

            // Frequency: 243, Initial Time: 340
            ,{ L_, 341, 340, 243, 4115226 }
            ,{ L_, 4294967634, 340, 243, 17674762526748971 }
            ,{ L_, 4294967635, 340, 243, 17674762530864197 }
            ,{ L_, 4294967636, 340, 243, 17674762534979423 }
            ,{ L_, 4294967637, 340, 243, 17674762539094650 }
            ,{ L_, 2241279404614, 340, 243, 9223372034049382716 }
            ,{ L_, 2241279404615, 340, 243, 9223372034053497942 }
            ,{ L_, 625, 340, 243, 1172839506 }
            ,{ L_, 3125, 340, 243, 11460905349 }
            ,{ L_, 15625, 340, 243, 62901234567 }
            ,{ L_, 78125, 340, 243, 320102880658 }
            ,{ L_, 390625, 340, 243, 1606111111111 }
            ,{ L_, 1953125, 340, 243, 8036152263374 }
            ,{ L_, 9765625, 340, 243, 40186358024691 }
            ,{ L_, 48828125, 340, 243, 200937386831275 }
            ,{ L_, 244140625, 340, 243, 1004692530864197 }
            ,{ L_, 1220703125, 340, 243, 5023468251028806 }
            ,{ L_, 6103515625, 340, 243, 25117346851851851 }
            ,{ L_, 30517578125, 340, 243, 125586739855967078 }
            ,{ L_, 152587890625, 340, 243, 627933704876543209 }
            ,{ L_, 762939453125, 340, 243, 3139668529979423868 }

            // Frequency: 729, Initial Time: 1020
            ,{ L_, 1021, 1020, 729, 1371742 }
            ,{ L_, 4294968314, 1020, 729, 5891587508916323 }
            ,{ L_, 4294968315, 1020, 729, 5891587510288065 }
            ,{ L_, 4294968316, 1020, 729, 5891587511659807 }
            ,{ L_, 4294968317, 1020, 729, 5891587513031550 }
            ,{ L_, 6723838213846, 1020, 729, 9223372034054869684 }
            ,{ L_, 6723838213847, 1020, 729, 9223372034056241426 }
            ,{ L_, 3125, 1020, 729, 2887517146 }
            ,{ L_, 15625, 1020, 729, 20034293552 }
            ,{ L_, 78125, 1020, 729, 105768175582 }
            ,{ L_, 390625, 1020, 729, 534437585733 }
            ,{ L_, 1953125, 1020, 729, 2677784636488 }
            ,{ L_, 9765625, 1020, 729, 13394519890260 }
            ,{ L_, 48828125, 1020, 729, 66978196159122 }
            ,{ L_, 244140625, 1020, 729, 334896577503429 }
            ,{ L_, 1220703125, 1020, 729, 1674488484224965 }
            ,{ L_, 6103515625, 1020, 729, 8372448017832647 }
            ,{ L_, 30517578125, 1020, 729, 41862245685871056 }
            ,{ L_, 152587890625, 1020, 729, 209311234026063100 }
            ,{ L_, 762939453125, 1020, 729, 1046556175727023319 }
            ,{ L_, 3814697265625, 1020, 729, 5232780884231824417 }

            // Frequency: 2187, Initial Time: 3061
            ,{ L_, 3062, 3061, 2187, 457247 }
            ,{ L_, 4294970355, 3061, 2187, 1963862502972107 }
            ,{ L_, 4294970356, 3061, 2187, 1963862503429355 }
            ,{ L_, 4294970357, 3061, 2187, 1963862503886602 }
            ,{ L_, 4294970358, 3061, 2187, 1963862504343850 }
            ,{ L_, 20171514641539, 3061, 2187, 9223372034054869684 }
            ,{ L_, 20171514641540, 3061, 2187, 9223372034055326931 }
            ,{ L_, 3125, 3061, 2187, 29263831 }
            ,{ L_, 15625, 3061, 2187, 5744855967 }
            ,{ L_, 78125, 3061, 2187, 34322816643 }
            ,{ L_, 390625, 3061, 2187, 177212620027 }
            ,{ L_, 1953125, 3061, 2187, 891661636945 }
            ,{ L_, 9765625, 3061, 2187, 4463906721536 }
            ,{ L_, 48828125, 3061, 2187, 22325132144490 }
            ,{ L_, 244140625, 3061, 2187, 111631259259259 }
            ,{ L_, 1220703125, 3061, 2187, 558161894833104 }
            ,{ L_, 6103515625, 3061, 2187, 2790815072702331 }
            ,{ L_, 30517578125, 3061, 2187, 13954080962048468 }
            ,{ L_, 152587890625, 3061, 2187, 69770410408779149 }
            ,{ L_, 762939453125, 3061, 2187, 348852057642432556 }
            ,{ L_, 3814697265625, 3061, 2187, 1744260293810699588 }
            ,{ L_, 19073486328125, 3061, 2187, 8721301474652034750 }

            // Frequency: 6561, Initial Time: 9185
            ,{ L_, 9186, 9185, 6561, 152415 }
            ,{ L_, 4294976479, 9185, 6561, 654620834324035 }
            ,{ L_, 4294976480, 9185, 6561, 654620834476451 }
            ,{ L_, 4294976481, 9185, 6561, 654620834628867 }
            ,{ L_, 4294976482, 9185, 6561, 654620834781283 }
            ,{ L_, 60514543924618, 9185, 6561, 9223372034054717268 }
            ,{ L_, 60514543924619, 9185, 6561, 9223372034054869684 }
            ,{ L_, 15625, 9185, 6561, 981557689 }
            ,{ L_, 78125, 9185, 6561, 10507544581 }
            ,{ L_, 390625, 9185, 6561, 58137479042 }
            ,{ L_, 1953125, 9185, 6561, 296287151348 }
            ,{ L_, 9765625, 9185, 6561, 1487035512879 }
            ,{ L_, 48828125, 9185, 6561, 7440777320530 }
            ,{ L_, 244140625, 9185, 6561, 37209486358786 }
            ,{ L_, 1220703125, 9185, 6561, 186053031550068 }
            ,{ L_, 6103515625, 9185, 6561, 930270757506477 }
            ,{ L_, 30517578125, 9185, 6561, 4651359387288523 }
            ,{ L_, 152587890625, 9185, 6561, 23256802536198750 }
            ,{ L_, 762939453125, 9185, 6561, 116284018280749885 }
            ,{ L_, 3814697265625, 9185, 6561, 581420097003505563 }
            ,{ L_, 19073486328125, 9185, 6561, 2907100490617283950 }

            // Frequency: 19683, Initial Time: 27556
            ,{ L_, 27557, 27556, 19683, 50805 }
            ,{ L_, 4294994850, 27556, 19683, 218206944774678 }
            ,{ L_, 4294994851, 27556, 19683, 218206944825483 }
            ,{ L_, 4294994852, 27556, 19683, 218206944876289 }
            ,{ L_, 4294994853, 27556, 19683, 218206944927094 }
            ,{ L_, 181543631773855, 27556, 19683, 9223372034054717268 }
            ,{ L_, 181543631773856, 27556, 19683, 9223372034054768073 }
            ,{ L_, 78125, 27556, 19683, 2569171366 }
            ,{ L_, 390625, 27556, 19683, 18445816186 }
            ,{ L_, 1953125, 27556, 19683, 97829040288 }
            ,{ L_, 9765625, 27556, 19683, 494745160798 }
            ,{ L_, 48828125, 27556, 19683, 2479325763349 }
            ,{ L_, 244140625, 27556, 19683, 12402228776101 }
            ,{ L_, 1220703125, 27556, 19683, 62016743839861 }
            ,{ L_, 6103515625, 27556, 19683, 310089319158664 }
            ,{ L_, 30517578125, 27556, 19683, 1550452195752679 }
            ,{ L_, 152587890625, 27556, 19683, 7752266578722755 }
            ,{ L_, 762939453125, 27556, 19683, 38761338493573134 }
            ,{ L_, 3814697265625, 27556, 19683, 193806698067825026 }
            ,{ L_, 19073486328125, 27556, 19683, 969033495939084489 }
            ,{ L_, 95367431640625, 27556, 19683, 4845167485295381801 }

            // Frequency: 59049, Initial Time: 82668
            ,{ L_, 82669, 82668, 59049, 16935 }
            ,{ L_, 4295049962, 82668, 59049, 72735648258226 }
            ,{ L_, 4295049963, 82668, 59049, 72735648275161 }
            ,{ L_, 4295049964, 82668, 59049, 72735648292096 }
            ,{ L_, 4295049965, 82668, 59049, 72735648309031 }
            ,{ L_, 544630895321568, 82668, 59049, 9223372034054768073 }
            ,{ L_, 544630895321569, 82668, 59049, 9223372034054785009 }
            ,{ L_, 390625, 82668, 59049, 5215278836 }
            ,{ L_, 1953125, 82668, 59049, 31676353536 }
            ,{ L_, 9765625, 82668, 59049, 163981727040 }
            ,{ L_, 48828125, 82668, 59049, 825508594557 }
            ,{ L_, 244140625, 82668, 59049, 4133142932141 }
            ,{ L_, 1220703125, 82668, 59049, 20671314620061 }
            ,{ L_, 6103515625, 82668, 59049, 103362173059662 }
            ,{ L_, 30517578125, 82668, 59049, 516816465257667 }
            ,{ L_, 152587890625, 82668, 59049, 2584087926247692 }
            ,{ L_, 762939453125, 82668, 59049, 12920445231197818 }
            ,{ L_, 3814697265625, 82668, 59049, 64602231755948449 }
            ,{ L_, 19073486328125, 82668, 59049, 323011164379701603 }
            ,{ L_, 95367431640625, 82668, 59049, 1615055827498467374 }
            ,{ L_, 476837158203125, 82668, 59049, 8075279143092296228 }

            // Frequency: 177147, Initial Time: 248005
            ,{ L_, 248006, 248005, 177147, 5645 }
            ,{ L_, 4295215299, 248005, 177147, 24245216086075 }
            ,{ L_, 4295215300, 248005, 177147, 24245216091720 }
            ,{ L_, 4295215301, 248005, 177147, 24245216097365 }
            ,{ L_, 4295215302, 248005, 177147, 24245216103010 }
            ,{ L_, 1633892685964706, 248005, 177147, 9223372034054773719 }
            ,{ L_, 1633892685964707, 248005, 177147, 9223372034054779364 }
            ,{ L_, 390625, 248005, 177147, 805094074 }
            ,{ L_, 1953125, 248005, 177147, 9625452307 }
            ,{ L_, 9765625, 248005, 177147, 53727243475 }
            ,{ L_, 48828125, 248005, 177147, 274236199314 }
            ,{ L_, 244140625, 248005, 177147, 1376780978509 }
            ,{ L_, 1220703125, 248005, 177147, 6889504874482 }
            ,{ L_, 6103515625, 248005, 177147, 34453124354349 }
            ,{ L_, 30517578125, 248005, 177147, 172271221753684 }
            ,{ L_, 152587890625, 248005, 177147, 861361708750359 }
            ,{ L_, 762939453125, 248005, 177147, 4306814143733735 }
            ,{ L_, 3814697265625, 248005, 177147, 21534076318650612 }
            ,{ L_, 19073486328125, 248005, 177147, 107670387193234996 }
            ,{ L_, 95367431640625, 248005, 177147, 538351941566156920 }
            ,{ L_, 476837158203125, 248005, 177147, 2691759713430766538 }

            // Frequency: 531441, Initial Time: 744017
            ,{ L_, 744018, 744017, 531441, 1881 }
            ,{ L_, 4295711311, 744017, 531441, 8081738695358 }
            ,{ L_, 4295711312, 744017, 531441, 8081738697240 }
            ,{ L_, 4295711313, 744017, 531441, 8081738699121 }
            ,{ L_, 4295711314, 744017, 531441, 8081738701003 }
            ,{ L_, 4901678057894120, 744017, 531441, 9223372034054773719 }
            ,{ L_, 4901678057894121, 744017, 531441, 9223372034054775600 }
            ,{ L_, 1953125, 744017, 531441, 2275150016 }
            ,{ L_, 9765625, 744017, 531441, 16975747072 }
            ,{ L_, 48828125, 744017, 531441, 90478732352 }
            ,{ L_, 244140625, 744017, 531441, 457993658750 }
            ,{ L_, 1220703125, 744017, 531441, 2295568290741 }
            ,{ L_, 6103515625, 744017, 531441, 11483441450697 }
            ,{ L_, 30517578125, 744017, 531441, 57422807250475 }
            ,{ L_, 152587890625, 744017, 531441, 287119636249367 }
            ,{ L_, 762939453125, 744017, 531441, 1435603781243825 }
            ,{ L_, 3814697265625, 744017, 531441, 7178024506216118 }
            ,{ L_, 19073486328125, 744017, 531441, 35890128131077579 }
            ,{ L_, 95367431640625, 744017, 531441, 179450646255384887 }
            ,{ L_, 476837158203125, 744017, 531441, 897253236876921426 }
            ,{ L_, 2384185791015625, 744017, 531441, 4486266189984604123 }

            // Frequency: 1594323, Initial Time: 2232052
            ,{ L_, 2232053, 2232052, 1594323, 627 }
            ,{ L_, 4297199346, 2232052, 1594323, 2693912898452 }
            ,{ L_, 4297199347, 2232052, 1594323, 2693912899080 }
            ,{ L_, 4297199348, 2232052, 1594323, 2693912899707 }
            ,{ L_, 4297199349, 2232052, 1594323, 2693912900334 }
            ,{ L_, 14705034173682363, 2232052, 1594323, 9223372034054774973 }
            ,{ L_, 14705034173682364, 2232052, 1594323, 9223372034054775600 }
            ,{ L_, 9765625, 2232052, 1594323, 4725248898 }
            ,{ L_, 48828125, 2232052, 1594323, 29226243991 }
            ,{ L_, 244140625, 2232052, 1594323, 151731219458 }
            ,{ L_, 1220703125, 2232052, 1594323, 764256096788 }
            ,{ L_, 6103515625, 2232052, 1594323, 3826880483440 }
            ,{ L_, 30517578125, 2232052, 1594323, 19140002416699 }
            ,{ L_, 152587890625, 2232052, 1594323, 95705612082996 }
            ,{ L_, 762939453125, 2232052, 1594323, 478533660414483 }
            ,{ L_, 3814697265625, 2232052, 1594323, 2392673902071913 }
            ,{ L_, 19073486328125, 2232052, 1594323, 11963375110359067 }
            ,{ L_, 95367431640625, 2232052, 1594323, 59816881151794837 }
            ,{ L_, 476837158203125, 2232052, 1594323, 299084411358973683 }
            ,{ L_, 2384185791015625, 2232052, 1594323, 1495422062394867915 }
            ,{ L_, 11920928955078125, 2232052, 1594323, 7477110317574339076 }

            // Frequency: 4782969, Initial Time: 6696156
            ,{ L_, 6696157, 6696156, 4782969, 209 }
            ,{ L_, 4301663450, 6696156, 4782969, 897970966150 }
            ,{ L_, 4301663451, 6696156, 4782969, 897970966360 }
            ,{ L_, 4301663452, 6696156, 4782969, 897970966569 }
            ,{ L_, 4301663453, 6696156, 4782969, 897970966778 }
            ,{ L_, 44115102521047093, 6696156, 4782969, 9223372034054775809 }
            ,{ L_, 44115102521047094, 6696156, 4782969, 9223372034054776018 }
            ,{ L_, 9765625, 6696156, 4782969, 641749716 }
            ,{ L_, 48828125, 6696156, 4782969, 8808748080 }
            ,{ L_, 244140625, 6696156, 4782969, 49643739902 }
            ,{ L_, 1220703125, 6696156, 4782969, 253818699013 }
            ,{ L_, 6103515625, 6696156, 4782969, 1274693494563 }
            ,{ L_, 30517578125, 6696156, 4782969, 6379067472316 }
            ,{ L_, 152587890625, 6696156, 4782969, 31900937361082 }
            ,{ L_, 762939453125, 6696156, 4782969, 159510286804911 }
            ,{ L_, 3814697265625, 6696156, 4782969, 797557034024054 }
            ,{ L_, 19073486328125, 6696156, 4782969, 3987790770119772 }
            ,{ L_, 95367431640625, 6696156, 4782969, 19938959450598362 }
            ,{ L_, 476837158203125, 6696156, 4782969, 99694802852991311 }
            ,{ L_, 2384185791015625, 6696156, 4782969, 498474019864956055 }
            ,{ L_, 11920928955078125, 6696156, 4782969, 2492370104924779775 }

            // Frequency: 14348907, Initial Time: 20088469
            ,{ L_, 20088470, 20088469, 14348907, 69 }
            ,{ L_, 4315055763, 20088469, 14348907, 299323655383 }
            ,{ L_, 4315055764, 20088469, 14348907, 299323655453 }
            ,{ L_, 4315055765, 20088469, 14348907, 299323655523 }
            ,{ L_, 4315055766, 20088469, 14348907, 299323655592 }
            ,{ L_, 132345307563141280, 20088469, 14348907,
                                                          9223372034054775809 }
            ,{ L_, 132345307563141281, 20088469, 14348907,
                                                          9223372034054775879 }
            ,{ L_, 48828125, 20088469, 14348907, 2002916040 }
            ,{ L_, 244140625, 20088469, 14348907, 15614579981 }
            ,{ L_, 1220703125, 20088469, 14348907, 83672899684 }
            ,{ L_, 6103515625, 20088469, 14348907, 423964498201 }
            ,{ L_, 30517578125, 20088469, 14348907, 2125422490786 }
            ,{ L_, 152587890625, 20088469, 14348907, 10632712453708 }
            ,{ L_, 762939453125, 20088469, 14348907, 53169162268317 }
            ,{ L_, 3814697265625, 20088469, 14348907, 265851411341365 }
            ,{ L_, 19073486328125, 20088469, 14348907, 1329262656706604 }
            ,{ L_, 95367431640625, 20088469, 14348907, 6646318883532801 }
            ,{ L_, 476837158203125, 20088469, 14348907, 33231600017663784 }
            ,{ L_, 2384185791015625, 20088469, 14348907, 166158005688318699 }
            ,{ L_, 11920928955078125, 20088469, 14348907, 830790034041593272 }
            ,{ L_, 59604644775390625, 20088469, 14348907, 4153950175807966139 }

            // Frequency: 43046721, Initial Time: 60265409
            ,{ L_, 60265410, 60265409, 43046721, 23 }
            ,{ L_, 4355232703, 60265409, 43046721, 99774551794 }
            ,{ L_, 4355232704, 60265409, 43046721, 99774551817 }
            ,{ L_, 4355232705, 60265409, 43046721, 99774551841 }
            ,{ L_, 4355232706, 60265409, 43046721, 99774551864 }
            ,{ L_, 397035922689423841, 60265409, 43046721,
                                                          9223372034054775786 }
            ,{ L_, 397035922689423842, 60265409, 43046721,
                                                          9223372034054775809 }
            ,{ L_, 244140625, 60265409, 43046721, 4271526651 }
            ,{ L_, 1220703125, 60265409, 43046721, 26957633219 }
            ,{ L_, 6103515625, 60265409, 43046721, 140388166057 }
            ,{ L_, 30517578125, 60265409, 43046721, 707540830252 }
            ,{ L_, 152587890625, 60265409, 43046721, 3543304151226 }
            ,{ L_, 762939453125, 60265409, 43046721, 17722120756096 }
            ,{ L_, 3814697265625, 60265409, 43046721, 88616203780445 }
            ,{ L_, 19073486328125, 60265409, 43046721, 443086618902192 }
            ,{ L_, 95367431640625, 60265409, 43046721, 2215438694510924 }
            ,{ L_, 476837158203125, 60265409, 43046721, 11077199072554585 }
            ,{ L_, 2384185791015625, 60265409, 43046721, 55386000962772890 }
            ,{ L_, 11920928955078125, 60265409, 43046721, 276930010413864414 }
            ,{ L_, 59604644775390625, 60265409, 43046721, 1384650057669322037 }
            ,{ L_, 298023223876953125, 60265409, 43046721,
                                                          6923250293946610149 }

            // Frequency: 129140163, Initial Time: 180796228
            ,{ L_, 180796229, 180796228, 129140163, 7 }
            ,{ L_, 4475763522, 180796228, 129140163, 33258183931 }
            ,{ L_, 4475763523, 180796228, 129140163, 33258183939 }
            ,{ L_, 4475763524, 180796228, 129140163, 33258183947 }
            ,{ L_, 4475763525, 180796228, 129140163, 33258183954 }
            ,{ L_, 1191107768068271526, 180796228, 129140163,
                                                          9223372034054775802 }
            ,{ L_, 1191107768068271527, 180796228, 129140163,
                                                          9223372034054775809 }
            ,{ L_, 244140625, 180796228, 129140163, 490508882 }
            ,{ L_, 1220703125, 180796228, 129140163, 8052544404 }
            ,{ L_, 6103515625, 180796228, 129140163, 45862722017 }
            ,{ L_, 30517578125, 180796228, 129140163, 234913610082 }
            ,{ L_, 152587890625, 180796228, 129140163, 1180168050407 }
            ,{ L_, 762939453125, 180796228, 129140163, 5906440252030 }
            ,{ L_, 3814697265625, 180796228, 129140163, 29537801260147 }
            ,{ L_, 19073486328125, 180796228, 129140163, 147694606300729 }
            ,{ L_, 95367431640625, 180796228, 129140163, 738478631503639 }
            ,{ L_, 476837158203125, 180796228, 129140163, 3692398757518193 }
            ,{ L_, 2384185791015625, 180796228, 129140163, 18461999387590961 }
            ,{ L_, 11920928955078125, 180796228, 129140163, 92310002537954803 }
            ,{ L_, 59604644775390625, 180796228, 129140163,
                                                          461550018289774010 }
            ,{ L_, 298023223876953125, 180796228, 129140163,
                                                          2307750097048870048 }

            // Frequency: 387420489, Initial Time: 542388684
            ,{ L_, 542388685, 542388684, 387420489, 2 }
            ,{ L_, 4837355978, 542388684, 387420489, 11086061310 }
            ,{ L_, 4837355979, 542388684, 387420489, 11086061313 }
            ,{ L_, 4837355980, 542388684, 387420489, 11086061315 }
            ,{ L_, 4837355981, 542388684, 387420489, 11086061318 }
            ,{ L_, 3573323304204814580, 542388684, 387420489,
                                                          9223372034054775807 }
            ,{ L_, 3573323304204814581, 542388684, 387420489,
                                                          9223372034054775809 }
            ,{ L_, 1220703125, 542388684, 387420489, 1750848135 }
            ,{ L_, 6103515625, 542388684, 387420489, 14354240673 }
            ,{ L_, 30517578125, 542388684, 387420489, 77371203361 }
            ,{ L_, 152587890625, 542388684, 387420489, 392456016803 }
            ,{ L_, 762939453125, 542388684, 387420489, 1967880084011 }
            ,{ L_, 3814697265625, 542388684, 387420489, 9845000420050 }
            ,{ L_, 19073486328125, 542388684, 387420489, 49230602100244 }
            ,{ L_, 95367431640625, 542388684, 387420489, 246158610501214 }
            ,{ L_, 476837158203125, 542388684, 387420489, 1230798652506065 }
            ,{ L_, 2384185791015625, 542388684, 387420489, 6153998862530321 }
            ,{ L_, 11920928955078125, 542388684, 387420489, 30769999912651602 }
            ,{ L_, 59604644775390625, 542388684, 387420489,
                                                          153850005163258004 }
            ,{ L_, 298023223876953125, 542388684, 387420489,
                                                          769250031416290017 }
            ,{ L_, 1490116119384765625, 542388684, 387420489,
                                                          3846250162681450079 }

            // Frequency: 1162261467, Initial Time: 1627166053
            ,{ L_, 5922133347, 1627166053, 1162261467, 3695353770 }
            ,{ L_, 5922133348, 1627166053, 1162261467, 3695353771 }
            ,{ L_, 5922133349, 1627166053, 1162261467, 3695353771 }
            ,{ L_, 5922133350, 1627166053, 1162261467, 3695353772 }
            ,{ L_, 9223372036854775806, 1627166053, 1162261467,
                                                          7935711797307317728 }
            ,{ L_, 9223372036854775807, 1627166053, 1162261467,
                                                          7935711797307317729 }
            ,{ L_, 6103515625, 1627166053, 1162261467, 3851413558 }
            ,{ L_, 30517578125, 1627166053, 1162261467, 24857067787 }
            ,{ L_, 152587890625, 1627166053, 1162261467, 129885338934 }
            ,{ L_, 762939453125, 1627166053, 1162261467, 655026694670 }
            ,{ L_, 3814697265625, 1627166053, 1162261467, 3280733473350 }
            ,{ L_, 19073486328125, 1627166053, 1162261467, 16409267366748 }
            ,{ L_, 95367431640625, 1627166053, 1162261467, 82051936833738 }
            ,{ L_, 476837158203125, 1627166053, 1162261467, 410265284168688 }
            ,{ L_, 2384185791015625, 1627166053, 1162261467, 2051332020843440 }
            ,{ L_, 11920928955078125, 1627166053, 1162261467,
                                                          10256665704217200 }
            ,{ L_, 59604644775390625, 1627166053, 1162261467,
                                                          51283334121086001 }
            ,{ L_, 298023223876953125, 1627166053, 1162261467,
                                                          256416676205430005 }
            ,{ L_, 1490116119384765625, 1627166053, 1162261467,
                                                          1282083386627150026 }
            ,{ L_, 7450580596923828125, 1627166053, 1162261467,
                                                          6410416938735750130 }

            // Frequency: 3486784401, Initial Time: 4881498161
            ,{ L_, 9176465455, 4881498161, 3486784401, 1231784590 }
            ,{ L_, 9176465456, 4881498161, 3486784401, 1231784590 }
            ,{ L_, 9176465457, 4881498161, 3486784401, 1231784590 }
            ,{ L_, 9176465458, 4881498161, 3486784401, 1231784590 }
            ,{ L_, 9223372036854775806, 4881498161, 3486784401,
                                                          2645237264835772576 }
            ,{ L_, 9223372036854775807, 4881498161, 3486784401,
                                                          2645237264835772576 }
            ,{ L_, 6103515625, 4881498161, 3486784401, 350471185 }
            ,{ L_, 30517578125, 4881498161, 3486784401, 7352355929 }
            ,{ L_, 152587890625, 4881498161, 3486784401, 42361779644 }
            ,{ L_, 762939453125, 4881498161, 3486784401, 217408898223 }
            ,{ L_, 3814697265625, 4881498161, 3486784401, 1092644491116 }
            ,{ L_, 19073486328125, 4881498161, 3486784401, 5468822455582 }
            ,{ L_, 95367431640625, 4881498161, 3486784401, 27349712277912 }
            ,{ L_, 476837158203125, 4881498161, 3486784401, 136754161389562 }
            ,{ L_, 2384185791015625, 4881498161, 3486784401, 683776406947813 }
            ,{ L_, 11920928955078125, 4881498161, 3486784401,
                                                          3418887634739066 }
            ,{ L_, 59604644775390625, 4881498161, 3486784401,
                                                          17094443773695333 }
            ,{ L_, 298023223876953125, 4881498161, 3486784401,
                                                          85472224468476668 }
            ,{ L_, 1490116119384765625, 4881498161, 3486784401,
                                                          427361127942383342 }
            ,{ L_, 7450580596923828125, 4881498161, 3486784401,
                                                          2136805645311916710 }
        };

        const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

        {
            if (verbose) printf("\nCheck fakeConvertRawTime arithmetic"
                                "\n-----------------------------------\n");

            for (int si = 0; si < NUM_DATA; ++si) {
                const int LINE = DATA[si].d_line;
                const bsls::Types::Int64 INPUT        = DATA[si].d_input;
                const bsls::Types::Int64 INITIAL_TIME = DATA[si].d_initialTime;
                const bsls::Types::Int64 FREQUENCY    = DATA[si].d_frequency;
                const bsls::Types::Int64 OUTPUT       = DATA[si].d_output;

                if (veryVerbose) {
                    T_;
                    P_(LINE);
                    P_(INPUT);
                    P_(INITIAL_TIME);
                    P_(FREQUENCY);
                    P(OUTPUT)
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
                printf("\nCompare fakeConvertRawTime to convertRawTime"
                       " with static data"
                       "\n--------------------------------------------"
                       "-----------------\n");

            const bsls::Types::Int64 frequency = getFrequency();
            const bsls::Types::Int64 testPeriod = getTestPeriod(frequency);

            if (veryVerbose) {
                T_; P(frequency);
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
                    T_; P_(exponent); P(offset);
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
                printf("\nCompare fakeConvertRawTime "
                       "to convertRawTime with random data"
                       "\n---------------------------"
                       "----------------------------------\n");

            srand((unsigned) time(NULL));

            const bsls::Types::Int64 frequency = getFrequency();
            const bsls::Types::Int64 testPeriod = getTestPeriod(frequency);

            if (veryVerbose) {
                T_; P(frequency);
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
                        T_; P_(bits); P_(iterations); P(offset);
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

        if (verbose) printf("\nTesting Raw methods"
                            "\n===================\n");

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
                printf("Warmup: %g\n", primeThePump);
            // ---------------------------------------------------------------
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;

            TU::getTimerRaw(&t1);
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) printf("0: Elapsed time under test: %lld nsec\n", dT);
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

            if (verbose) printf("1: Elapsed time under test: %lld nsec\n", dT);
            if (veryVerbose) printf("Computed Values: %g\n", x);
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

            if (verbose) printf("2: Elapsed time under test: %lld nsec\n", dT);
            if (veryVerbose) printf("Computed Values: %g %g\n", x, y);
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

            if (verbose) printf("3: Elapsed time under test: %lld nsec\n", dT);
            if (veryVerbose) printf("Computed Values: %g %g %g\n", x, y, z);
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

            if (verbose)
                printf("10: Elapsed time under test: %lld nsec\n", dT);
            if (veryVerbose) printf("Computed Values: %g %g %g\n", x, y, z);
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

            if (verbose)
                printf("100: Elapsed time under test: %lld nsec\n", dT);
            if (veryVerbose) printf("Computed Values: %g %g %g\n", x, y, z);
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
            printf("\nTesting TimeUtil initialization (wall timer)"
                   "\n============================================\n");

        Int64 t = TU::getTimer();
        if (verbose) { T_; P_(t); }
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
            printf("\nTesting TimeUtil initialization (process timers)"
                   "\n================================================\n");

        Int64 ts, tu; TU::getProcessTimers(&ts, &tu);
        if (verbose) { T_; P_(ts); P_(tu); }
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
            printf("\nTesting TimeUtil initialization (user timer)"
                   "\n============================================\n");

        Int64 t = TU::getProcessUserTimer();
        if (verbose) { T_; P_(t); }
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
            printf("\nTesting TimeUtil initialization (system timer)"
                   "\n==============================================\n");

        Int64 t = TU::getProcessSystemTimer();
        if (verbose) { T_; P_(t); }
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
            printf("\nTesting 'gethrtime()' statistical correctness"
                   "\n=============================================\n");

        if (verbose) printf("\nCall 'gethrtime()' twice in a large loop.\n");
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
                // P(dt);             //  non-production testing.
                if (t2 <= t1) {  // Optimize for CORRECT behavior
                    if (t2 == t1) {
                        ++numSame;
                        if (verbose) { T_;  P_(i);  P(numSame); }
                    }
                    else {
                        ++numWrong;
                        if (verbose) {
                            T_;      P_(i);   P_(numWrong);
                            P_(t1);  P_(t2);  P(t2 - t1);
                        }
                    }
                }
            }
            double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
            double numCalls    = ((double) NUM_TESTS) * 1.0e-6;
            double timePerCall = elapsedTime / (numCalls * 2.0); //(microsec.)
            if (verbose) {
                printf("\telapsed time  = %g (sec)\n"
                       "\tnum calls     = %g x 10^6\n"
                       "\ttime per call = %g (usec)\n\n"
                       "\tnum non-monotonic pairs   = %d\n"
                       "\tnum pairs with same value = %d\n",
                       elapsedTime,
                       numCalls,
                       timePerCall,
                       numWrong,
                       numSame);
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
                printf("\nHooking Test: System time requests across sleep."
                       "\n================================================\n");

            if (veryVerbose) printf("timeQuantum = %lld\n", timeQuantum);

            const unsigned shortSleep = 2;
            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            osSleep(shortSleep);

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose) {
                P_(wt1) P_(ut1) P(st1)
                P_(wt2) P_(ut2) P(st2)
                P(wt2 - wt1)
            }

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
                printf("\nHooking Test: System time requests repeated."
                       "\n============================================\n");

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

            if (veryVerbose) { P_(uQuantsSpent) P(sQuantsSpent) }

            ASSERT(uQuantsSpent <= 1);
            ASSERT(sQuantsSpent <= 1);

            if (veryVerbose) {
                printf( " 0: wt: %lld\n"
                        " 0: ut: %lld\n"
                        " 0: st: %lld\n",
                        samples[0].d_wt,
                        samples[0].d_ut,
                        samples[0].d_st);

                for (int i = 1; i < NUM_SAMPLES; ++i) {
                    sample& s0 = samples[i - 1];
                    sample& s1 = samples[i];

                    printf("%2d: wt: %lld - %lld = %lld\n"
                           "%2d: ut: %lld - %lld = %lld\n"
                           "%2d: st: %lld - %lld = %lld\n",
                           i, s1.d_wt, s0.d_wt, s1.d_wt - s0.d_wt,
                           i, s1.d_ut, s0.d_ut, s1.d_ut - s0.d_ut,
                           i, s1.d_st, s0.d_st, s1.d_st - s0.d_st);
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
                printf("\nHooking Test: System time requests over long loop.\n"
                       "==================================================\n");

            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            for (volatile unsigned u = 0; u < longLoop; ++u) {
                ++u; --u;
            }

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose) {
                printf("wt2: %lld -  wt1: %lld = %lld\n", wt2, wt1, wt2 - wt1);
                printf("ut2: %lld -  ut1: %lld = %lld\n", ut2, ut1, ut2 - ut1);
                printf("st2: %lld -  st1: %lld = %lld\n", st2, st1, st2 - st1);
            }

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
            const char *d_methodName;
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
                printf("\nTesting '%s()' statistical correctness"
                       "\n===========================================\n",
                       TimerMethods[t].d_methodName);

            if (verbose)
                printf("\nCall 'bsls::TimeUtil::%s()' twice in a large loop.\n",
                       TimerMethods[t].d_methodName);
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
                                if (verbose) { T_;  P_(i);  P(numSame); }
                            }
                        }
                        else {
                            ++numWrong;
                            if (verbose) {
                                T_;      P_(i);   P_(numWrong);
                                P_(t1);  P_(t2);  P(t2 - t1);
                            }
                        }
                    }
                }
                LOOP_ASSERT(t, 0 == numWrong);

                double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
                double numCalls    = ((double) NUM_TESTS) * 1.0e-6;
                double timePerCall = elapsedTime / (numCalls * 2.0);//microsec
                if (verbose) {
                    printf("\telapsed time  = %g (sec)\n"
                           "\tnum calls     = %g x 10^6\n"
                           "\ttime per call = %g (usec)\n\n"
                           "\tnum non-monotonic pairs   = %d\n"
                           "\tnum pairs with same value = %d\n",
                           elapsedTime,
                           numCalls,
                           timePerCall,
                           numWrong,
                           numSame);
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
            const char *d_methodName;
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
                printf("\nTesting 'bsls::TimeUtil::%s Performance()'"
                       "\n===============================================\n",
                       TimerMethods[t].d_methodName);

            if (verbose)
                printf("\nCall 'bsls::TimeUtil::%s()' in a large loop.\n",
                        TimerMethods[t].d_methodName);
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
                    printf("\telapsed time  = %g (sec)\n"
                           "\tnum calls     = %g x 10^6\n"
                           "\ttime per call = %g (usec)\n",
                           elapsedTime, numCalls, timePerCall);
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
            const char *d_methodName;
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
                printf("\nTesting '%s()'"
                       "\n====================",
                       TimerMethods[t].d_methodName);

            if (verbose)
                 printf("\nConfirm sizeof (%s()).\n",
                        TimerMethods[t].d_methodName);
            {
                int numBytes = sizeof(TimerMethods[t].d_method());
                if (veryVerbose) { T_; P(numBytes); }
                LOOP_ASSERT(t, 8 <= numBytes);
            }

            if (verbose)
                printf("\nExercise '%s()'.\n", TimerMethods[t].d_methodName);
            {
                const int NUM_CALLS   = 10;
                const int DELAY_COUNT = 1000000;
                Int64     result[NUM_CALLS];
                double    totalElapsedTime = 0.0;

                result[0] = TimerMethods[t].d_method();
                if (veryVerbose) { T_;  P(result[0]); }
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
                        T_;  P_(result[i]);  P_(difference);  P(seconds);
                    }
                    LOOP2_ASSERT(t, i, (Int64) 0 <= difference);//cast--dg bug
                }
                if (veryVerbose) {  printf("\n");  T_;  P(totalElapsedTime); }
            }
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
