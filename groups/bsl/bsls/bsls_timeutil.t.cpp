// bsls_timeutil.t.cpp                                                -*-C++-*-
#include <bsls_timeutil.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#ifdef BSLS_PLATFORM_OS_UNIX
    #include <stdio.h>     // file I/O operations
    #include <sys/time.h>  // 'gethrtime()'
    #include <sys/times.h> // struct tms, times()
    #include <time.h>      // NOTE: <ctime> conflicts with <sys/time.h>
    #include <unistd.h>    // 'sleep'
#ifdef BSLS_PLATFORM_OS_SOLARIS   // Solaris OR late SunOS!
    #include <limits.h>    // 'CLK_TCK', for Sun (on FreeBSD, in <sys/time.h>)
#endif
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
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
// just an alias to one of the other three), which are timer functions based on
// platform-dependent system clocks.
//
// It is not possible to completely test a system-dependent function, but we
// address basic concerns to probe both our own code for consistent behavior
// and the system results for plausible correct behavior.
//-----------------------------------------------------------------------------
// [ 8] Int64 convertRawTime(OpaqueNativeTime rawTime);
// [ 1] Int64 getProcessSystemTimer();
// [ 1] void getProcessTimers(Int64);
// [ 1] Int64 getTimer();
// [ 1] Int64 getProcessUserTimer();
// [ 8] OpaqueNativeTime getTimerRaw();
//-----------------------------------------------------------------------------
// [10] USAGE
// [ 2] Performance Test
// [ 3] Successive timer values do not repeat
// [ 4] Forwarding of methods to underlying OS APIs
// [ 5] Forwarding of methods to underlying OS APIs (Unix only)
// [ 6] Test of 'gethrtime()' (Sun and HP only -- statistical)
// [ 7] Initialization test: getTimer (Windows only)
// [ 9] convertRawTime() arithmetic *** Windows Only ***
// [-1] getProcessTimers(*Int64, *Int64) (Unix only)
// [-1] Int64 getTimer() (Unix only)
// [-2] CONCERN: Timer results respect invariants
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

#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV
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

// ========================================================================
//                          PRINTF FORMAT MACROS
// ------------------------------------------------------------------------

#define TI64 BSLS_BSLTESTUTIL_FORMAT_I64

//=============================================================================
//                 GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::TimeUtil     TU;
typedef bsls::Types::Int64 Int64;

typedef Int64 (*TimerMethod) ();

const int   nsecsPerMicrosecond = 1000;
const int   nsecsPerMillisecond = 1000 * 1000;
const Int64 nsecsPerSecond = 1000LL * 1000LL * 1000LL;

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

#if defined(BSLS_PLATFORM_OS_UNIX)
static void osMillisleep(unsigned milliseconds) {

    timespec interval;

    interval.tv_sec = 0;
    interval.tv_nsec = milliseconds * nsecsPerMillisecond;
    nanosleep(&interval, 0);
}
#elif defined (BSLS_PLATFORM_OS_WINDOWS)
static void osMillisleep(unsigned milliseconds) {
    Sleep(milliseconds);
}
#else
    #error "Do not know how to sleep on this platform"
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)
const Int64 MODEL_CPU_TICK_DURATION = nsecsPerSecond / ::sysconf(_SC_CLK_TCK);
    // length of a system clock tick, in nanoseconds

void getModelCpuTime(Int64 *systemTime, Int64 *userTime)
    // Return CPU times (system and user) for current process, acquired from
    // 'times()' function, that is used as a model.
{
    struct tms processTimes;
    if (static_cast<clock_t>(-1) == ::times(&processTimes)) {
         *systemTime = 0;
         *userTime   = 0;
         return;                                                      // RETURN
    }

    *systemTime = static_cast<bsls::Types::Int64>(processTimes.tms_stime)
                                                     * MODEL_CPU_TICK_DURATION;
    *userTime   = static_cast<bsls::Types::Int64>(processTimes.tms_utime)
                                                     * MODEL_CPU_TICK_DURATION;

}

Int64 getModelSystemTime()
    // Return CPU system time for current process, acquired from 'times()'
    // function, that is used as a model.
{
    struct tms processTimes;
    if (static_cast<clock_t>(-1) == ::times(&processTimes)) {
         return 0;                                                    // RETURN
    }

    return static_cast<bsls::Types::Int64>(processTimes.tms_stime)
                                                     * MODEL_CPU_TICK_DURATION;
}

Int64 getModelUserTime()
    // Return CPU user time for current process, acquired from 'times()'
    // function, that is used as a model.
{
    struct tms processTimes;
    if (static_cast<clock_t>(-1) == ::times(&processTimes)) {
         return 0;                                                    // RETURN
    }

    return static_cast<bsls::Types::Int64>(processTimes.tms_utime)
                                                     * MODEL_CPU_TICK_DURATION;
}

void getModelWallTime(Int64 *wallTime)
    // Return wall time for current process, acquired from 'gettimeofday()'
    // function, that is used as a model.
{
    struct timeval now;
    int            rv = gettimeofday(&now, NULL);
    if (-1 == rv) {
        *wallTime = 0;
        return;                                                       // RETURN
    }

    *wallTime = now.tv_sec * nsecsPerSecond +
                now.tv_usec * nsecsPerMicrosecond;
}

enum Scenario {
 // This enumeration provide values to identify actions to be
 // performed to demonstrate differences of CPU times alterations.

    SLEEP_SCENARIO
  , USER_TIME_SCENARIO
  , SYSTEM_TIME_SCENARIO
  , NUM_SCENARIOS
};

void printBanner(Scenario scenario)
    // Print banner to the logs in accordance with current test scenario.
{
    switch (scenario) {
      case SLEEP_SCENARIO: {
        printf("\nHooking Test: Neither user nor system time burning."
               "\n====================================================\n");
      } break;
      case USER_TIME_SCENARIO: {
        printf("\nHooking Test: User time burning."
               "\n================================\n");
      } break;
      case SYSTEM_TIME_SCENARIO: {
        printf("\nHooking Test: System time burning."
               "\n==================================\n");
      } break;
      default: {
        printf("\nUnknown scenario.\n");
      }
    }
}

void runTestScenario(Scenario scenario, bool printMessages)
    // Perform necessary time burning actions in accordance with the test
    // scenario.
{
    switch (scenario) {
      case SLEEP_SCENARIO: {
        // Sleeping process should't burn neither 'user' nor 'system' CPU time.

        const unsigned shortSleep = 50;
        osMillisleep(shortSleep);
      } break;
      case USER_TIME_SCENARIO: {
        // Burn user processor time.  Simple incrementing/decrementing of the
        // local variable doesn't do any system calls, so 'user' CPU time will
        // increase, while 'system' CPU time remain the same.  The 'volatile'
        // should force every access to memory and slow things up a little, or
        // a lot.  It may make the test less dependent on ever-increasing
        // processor speeds.

        const unsigned longLoop = 8 * 1000L * 1000L;
        for (volatile unsigned u = 0; u < longLoop; ++u) {
            ++u; --u;
        }
      } break;
      case SYSTEM_TIME_SCENARIO: {
        // Burn system processor time.  Single BLOB read/write operations will
        // increase 'system' CPU time, but shouldn't significantly increase
        // 'user' CPU time.

        FILE *file = tmpfile();
        if (file)
        {
            const int ARRAY_SIZE = 10 * 1024 * 1024;
            char *temp = new char[ARRAY_SIZE];

            fwrite(temp, sizeof(char), ARRAY_SIZE, file);
            fseek(file, 0, SEEK_SET);
            size_t rc = fread(temp, sizeof(char), ARRAY_SIZE, file);
            (void) rc; // silence unused variable warning

            fclose(file);
            // File generated by 'tmpfile' automatically removed on close.
        } else {
            if (printMessages) {
                printf("\nUnable to open file.\n");
            }
        }
      } break;
      default: {
        if (printMessages) {
            printf("\nUnknown scenario.\n");
        }
      }
    }
}

#else
// For "TIMER INVARIANTS TEST" only

const Int64 MODEL_CPU_TICK_DURATION = nsecsPerSecond / 100;
#endif


#if defined(BSLS_PLATFORM_OS_WINDOWS)
typedef HANDLE SystemTimeResource;
char fname[MAX_PATH];
#else
typedef int SystemTimeResource;
char fnTemplate[24] = {
    'b','s','l','s','_',
    't','i','m','e','u','t','i','l','_',
    't','_',
    'X','X','X','X','X','X','\0'
};
#endif

SystemTimeResource acquireSystemTimeResource()
{

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    int rc = GetTempFileName(".", "bsls_timeutil_t_", 0, fname);
    ASSERT(0 != rc);

    if (0 == rc) {
        return INVALID_HANDLE_VALUE;
    }

    HANDLE fd = CreateFile(fname,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,                               // default security
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,              // normal file
                    NULL);                              // no attr

    return fd;
#else
    return mkstemp(fnTemplate);
#endif
}

void releaseSystemTimeResource(SystemTimeResource resource)
{

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    CloseHandle(resource);
    DeleteFile(fname);
#else
    close(resource);
    unlink(fnTemplate);
#endif
}

void burnSystemTime(SystemTimeResource fd)
    // Consume some arbitrary amount of system time, at least observably
    // greater than the user time consumed.
{
    // Writing to a file seems to consume system time more reliably across all
    // platforms than other alternatives considered, including: forking
    // processes or calling 'lstat' on the currently-running executable.
    //
    // This method of burning system time is still not 100% efficient, and on
    // Sun often results in the test driver consuming only twice as much system
    // time as user time.  Adequate for our purposes, but there is definitely
    // room for improvement.

    static const char out[2048] = {};

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    DWORD nWritten;
    OVERLAPPED overlapped;
    LARGE_INTEGER li;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 100; ++j) {
            if (0 == WriteFile(fd, out, sizeof(out), &nWritten, &overlapped)) {
                break;
            }
        }
        li.QuadPart = 0;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(fd,
                                                       li.LowPart,
                                                       &li.HighPart,
                                                       FILE_BEGIN)) {
            break;
        }
    }
#else
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 100; ++j) {
            if (-1 == write(fd, (const void *) out, sizeof(out))) {
                break;
            }
        }
        if (lseek(fd, 0, SEEK_SET)) {
            break;
        }
    }
#endif
}

void burnUserTime(volatile int *u)
    // Consume some arbitrary amount of user time, at least observably greater
    // than the system time consumed.
{
    enum { NUM_SPINS = 10000000 };

    for (int j = 0; j < NUM_SPINS; ++j) {
        ++*u;
    }
}

void burnWallTime()
    // Consume some arbitrary amount of wall time.
{
    // Sleeping process should't burn neither 'user' nor 'system' CPU time.

    const unsigned shortSleep = 50;
    osMillisleep(shortSleep);
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
//                         HELPER FUNCTIONS FOR CASE 10
//-----------------------------------------------------------------------------

// Data Generation Script for Case 10
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
//:   The frequencies sampled include hardcoded frequencies typically seen on
//:   Windows machines, the highest possible 32-bit frequency, and all of the
//:   powers of a given base that are valid 32-bit numbers.  By choosing a base
//:   that is relatively prime to 1 billion and (1 << 32), we can get
//:   representative frequencies that cover the 32-bit range well and that
//:   consistently leave a remainder when calculating the constant factors used
//:   in the Windows implementation of 'convertRawTimer'.
//:
//:   For each frequency, we calculate nanoseconds based on the following
//:   values:
//:     o Lowest valid clock tick value
//:     o Clock tick values around the 32-bit limit
//:     o Clock tick values near the value that generates the maximum number of
//:       nanoseconds that can be represented for this frequency.
//:
//:   Additionally, a full range of tick counts that are powers of a given base
//:   are generated.
//:
//:   Any generated values that include frequencies, tick counts or nanosecond
//:   values that cannot be represented by 'bsls::Types::Int64' are discarded.
//..
//  #!/usr/bin/env python
//
//  max64 = (1 << 63) - 1
//  billion = long(1000 * 1000 * 1000)
//
//  class Transform:
//      """Provide a converter to transform a number of clock ticks and a
//      frequency to a time expressed in nanoseconds"""
//
//      def __init__(self, frequency, initialTime):
//          self.frequency = frequency
//          self.initialTime = initialTime
//
//      def nanoseconds(self, ticks):
//          return ((ticks - self.initialTime) * billion) / self.frequency
//
//      def ticks(self, nanoseconds):
//          return ((nanoseconds * self.frequency) / billion +
//              self.initialTime)
//
//  class Generator:
//      """Provide a driver to generate nanosecond conversions of a number of
//      frequencies and clock tick values"""
//      frequencies = [
//          (1 << 32) - 1
//          ,2992530000
//          ,3579545
//      ]
//
//      def __init__(self, base, verbose):
//          self.verbose = verbose
//
//          count = 0
//          limit = 1 << 32
//          n = base
//
//          while n < limit:
//              if n < 1000:
//                  n *= base
//                  continue
//              self.frequencies.append(n)
//              ++count
//              n *= base
//
//      def generateTicks(self, base, initialTime):
//          result = []
//          limit = max64
//
//          n = base
//          while n < limit:
//              result.append(n)
//              n *= base
//
//          return result
//
//      def maxTicks(self, frequency, initialTime):
//          maxticks = (max64 * frequency) / billion - initialTime
//
//          return min(maxticks, max64)
//
//      def report(self, frequency, ticks, initialTime):
//          if initialTime < ticks:
//              t = Transform(frequency, initialTime)
//
//              nanoseconds = t.nanoseconds(ticks)
//              if (nanoseconds != 0 and nanoseconds <= max64):
//                  if self.verbose:
//                      print ('Init: f=%d, t=%d, i=%d' %
//                          (frequency, ticks, initialTime))
//
//                      print 'Ticks: %d' % (t.ticks(nanoseconds))
//                      print 'Nanoseconds: %d' % (nanoseconds)
//
//                  print (',{ L_, %d, %d, %d, %d }' %
//                      (ticks, initialTime, frequency, nanoseconds))
//
//                  if self.verbose:
//                      print
//              elif self.verbose:
//                  if nanoseconds == 0:
//                      print ('SKIP: %s f=%d, t=%d, i=%d' %
//                          ("zero", frequency, ticks, initialTime))
//                  else:
//                      print ('SKIP: %s f=%d, t=%d, i=%d' %
//                          ("overflow", frequency, ticks, initialTime))
//          elif self.verbose:
//              print ('SKIP: bad init f=%d, t=%d, i=%d' %
//                  (frequency, ticks, initialTime))
//
//      def generate(self):
//          for frequency in self.frequencies:
//              initialTime = ((frequency) * 7) / 5
//              print
//              print ('// Frequency: %d, Initial Time: %d' %
//                  (frequency, initialTime))
//              self.report(frequency, initialTime + 1, initialTime)
//              self.report(frequency,
//                          (1 << 32) - 1 + initialTime,
//                          initialTime)
//              self.report(frequency,
//                          (1 << 32) + initialTime,
//                          initialTime)
//              self.report(frequency,
//                          self.maxTicks(frequency, initialTime),
//                          initialTime)
//
//              for baseValue in [7]:
//                  for ticks in self.generateTicks(baseValue, initialTime):
//                      self.report(frequency, ticks, initialTime)
//
//  g = Generator(7, False)
//  g.generate()
//..

#if defined BSLS_PLATFORM_OS_WINDOWS

bsls::Types::Int64 fakeConvertRawTime(bsls::Types::Int64 rawTime,
                                      bsls::Types::Int64 initialTime,
                                      bsls::Types::Int64 timerFrequency)
    // Convert the specified raw interval ('initialTime', 'rawTime'] to a value
    // in nanoseconds, by dividing the number of clock ticks in the raw
    // interval by the specified 'timerFrequency', and return the result of the
    // conversion.  Note that this method is thread-safe only if 'initialize'
    // has been called before.

    // This function is copied from 'WindowsTimerUtil::convertRawTime' in
    // bsls_timeutil.cpp.  It is used as a stand-in for that method by which we
    // can check the accuracy of the calculations in
    // 'WindowsTimerUtil::convertRawTime' against known values.  Other tests
    // will compare the behavior of this function to that of
    // 'WindowsTimerUtil::convertRawTime' to confirm that they both behave the
    // same.
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

        const bsls::Types::Int64 high32Bits =
            static_cast<bsls::Types::Int64>(rawTime >> 32);
        const bsls::Types::Uint64 low32Bits  =
            static_cast<bsls::Types::Uint64> (rawTime & LOW_MASK);

        return high32Bits * highPartDivisionFactor +
              (high32Bits * highPartRemainderFactor) / timerFrequency +
              (low32Bits * G) / timerFrequency;
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
    // Calculate the nanosecond length of an interval of the specified 'offset'
    // clock ticks on a timer with the specified 'frequency', using both
    // 'TU::convertRawTime' and 'fakeConvertRawTime', where
    // 'fakeConvertRawTime' uses the specified 'startTime' for the initial time
    // of the interval and 'TU::convertRawTime' uses its own internal initial
    // time.  Assert that both functions calculate the same length.  Note that
    // because the two functions base their calculations on different initial
    // times, their results may differ by at most 1 due to rounding error.
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

    // 'fakeConvertRawTime' and 'TU::convertRawTime' have been calculated with
    // integer arithmetic, including two division operations.  Due to the loss
    // of fractional remainders in integer division, the result of either
    // function will be either floor(v) or floor(v) - 1, where v is the actual
    // (infinite precision floating point) number of nanoseconds corresponding
    // to the input.  Similarly, the difference between two results will be in
    // the range floor(v1 - v2) +- 1, and the difference between two intervals
    // will be in the range floor((e1 - s1) - (e2 - s2)) +- 2.

    LOOP5_ASSERT(offset,
                 realEnd,
                 fakeEnd,
                 (realEnd - realStart),
                 (fakeEnd - fakeStart),
                 (realEnd - realStart) + 2 > (fakeEnd - fakeStart) - 2
                 &&
                 (fakeEnd - fakeStart) + 2 > (realEnd - realStart) - 2);
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
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header must build and
        //   run with a normal exit status.
        //
        // Plan:
        //:  Copy the implementation portion of the Usage Example to the
        //:  reserved space above, and copy the executable portion below,
        //:  adding any needed supporting code.
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
      case 9: {
        // --------------------------------------------------------------------
        // TESTING convertRawTime() arithmetic *** Windows Only ***
        //
        // Concerns:
        //:  When the 'QueryPerformanceCounter' interface is available on
        //:  Windows platforms, the conversion arithmetic is non-obvious and
        //:  cannot be validated simply by reading the code.  Verify that the
        //:  conversion is accurate.
        //:
        //:  Subconcerns include:
        //:   o Underflow due to a small time interval and high frequency does
        //:     not result in inaccurate calculation.
        //:
        //:   o Overflow due to a large interval and low frequency does not
        //:     result in overflow.
        //:
        //:   o The minimum possible interval (one clock tick) is accurately
        //:     handled.
        //:
        //:   o The maximum possible interval (frequency dependent, up to the
        //:     maximum number of nanoseconds that can be represented by
        //:     'bsls::Types::Int64' is accurately handled.
        //:
        //:   o Because calculation of the contributions from the high part and
        //:     low part of the ''bsls::Types::Int64' representing the number
        //:     of clock ticks in an interval are done separately, intervals
        //:     just above or below the value '1 << 32' might not be handled
        //:     correctly.
        //
        // Plan:
        //:  'convertRawTime' cannot be tested directly because it relies on
        //:  two hidden values: 's_timerFrequency' and 's_initialTime'.  We
        //:  note, however, that the value of 's_timerFrequency' can be
        //:  retrieved directly from the machine, and that the value of
        //:  's_initialTime' will cancel itself out on any comparison between
        //:  two converted times.  Therefore, use a copy of the Windows
        //:  implementation of 'convertRawTime' to test the correctness of the
        //:  arithmetic.  This copy must be kept in sync with the actual
        //:  component code.
        //:
        //:  Test the correctness of the arithmetic by using the fake
        //:  'convertRawTime' and a table-based strategy to convert a number of
        //:  raw values for which the output value is known.
        //:
        //:  Test that the fake 'convertRawTime' behaves the same as the real
        //:  'convertRawTime' by using both to measure real time intervals and
        //:  compare the nanosecond results.  Test using fixed data based on
        //:  the concerns above, as well as random data distributed over the
        //:  entire range of valid clock tick values.
        //
        // Testing:
        //   bsls::TimeUtil::convertRawTime(bsls::TimeUtil::OpaqueNativeTime)
        // --------------------------------------------------------------------

#if defined BSLS_PLATFORM_OS_WINDOWS

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
            ,{ L_, 10307921508, 6012954213, 4294967295, 1000000000 }
            ,{ L_, 10307921509, 6012954213, 4294967295, 1000000000 }
            ,{ L_, 9223372036854775807, 6012954213, 4294967295,
                                                          2147483647099999999 }
            ,{ L_, 13841287201, 6012954213, 4294967295, 1822675808 }
            ,{ L_, 96889010407, 6012954213, 4294967295, 21158730661 }
            ,{ L_, 678223072849, 6012954213, 4294967295, 156511114629 }
            ,{ L_, 4747561509943, 6012954213, 4294967295, 1103977802403 }
            ,{ L_, 33232930569601, 6012954213, 4294967295, 7736244616826 }
            ,{ L_, 232630513987207, 6012954213, 4294967295, 54162112317782 }
            ,{ L_, 1628413597910449, 6012954213, 4294967295, 379143186224479 }
            ,{ L_, 11398895185373143, 6012954213, 4294967295,
                                                             2654010703571359 }
            ,{ L_, 79792266297612001, 6012954213, 4294967295,
                                                            18578083324999518 }
            ,{ L_, 558545864083284007, 6012954213, 4294967295,
                                                           130046591674996629 }
            ,{ L_, 3909821048582988049, 6012954213, 4294967295,
                                                           910326150124976408 }

            // Frequency: 2992530000, Initial Time: 4189542000
            ,{ L_, 8484509295, 4189542000, 2992530000, 1435229486 }
            ,{ L_, 8484509296, 4189542000, 2992530000, 1435229486 }
            ,{ L_, 9223372036854775807, 4189542000, 2992530000,
                                                          3082131852534555645 }
            ,{ L_, 13841287201, 4189542000, 2992530000, 3225279345 }
            ,{ L_, 96889010407, 4189542000, 2992530000, 30976955421 }
            ,{ L_, 678223072849, 4189542000, 2992530000, 225238687949 }
            ,{ L_, 4747561509943, 4189542000, 2992530000, 1585070815645 }
            ,{ L_, 33232930569601, 4189542000, 2992530000, 11103895709517 }
            ,{ L_, 232630513987207, 4189542000, 2992530000, 77735669966619 }
            ,{ L_, 1628413597910449, 4189542000, 2992530000, 544158089766334 }
            ,{ L_, 11398895185373143, 4189542000, 2992530000,
                                                             3809115028364341 }
            ,{ L_, 79792266297612001, 4189542000, 2992530000,
                                                            26663813598550390 }
            ,{ L_, 558545864083284007, 4189542000, 2992530000,
                                                           186646703589852735 }
            ,{ L_, 3909821048582988049, 4189542000, 2992530000,
                                                          1306526933528969149 }

            // Frequency: 3579545, Initial Time: 5011363
            ,{ L_, 5011364, 5011363, 3579545, 279 }
            ,{ L_, 4299978658, 5011363, 3579545, 1199864031601 }
            ,{ L_, 4299978659, 5011363, 3579545, 1199864031881 }
            ,{ L_, 33015475252651965, 5011363, 3579545, 9223372034054775676 }
            ,{ L_, 5764801, 5011363, 3579545, 210484293 }
            ,{ L_, 40353607, 5011363, 3579545, 9873390053 }
            ,{ L_, 282475249, 5011363, 3579545, 77513730376 }
            ,{ L_, 1977326743, 5011363, 3579545, 550996112634 }
            ,{ L_, 13841287201, 5011363, 3579545, 3865372788440 }
            ,{ L_, 96889010407, 5011363, 3579545, 27066009519086 }
            ,{ L_, 678223072849, 5011363, 3579545, 189470466633608 }
            ,{ L_, 4747561509943, 5011363, 3579545, 1326301666435259 }
            ,{ L_, 33232930569601, 5011363, 3579545, 9284120065046814 }
            ,{ L_, 232630513987207, 5011363, 3579545, 64988848855327702 }
            ,{ L_, 1628413597910449, 5011363, 3579545, 454921950387293915 }
            ,{ L_, 11398895185373143, 5011363, 3579545, 3184453661111057410 }

            // Frequency: 2401, Initial Time: 3361
            ,{ L_, 3362, 3361, 2401, 416493 }
            ,{ L_, 4294970656, 3361, 2401, 1788824362765514 }
            ,{ L_, 4294970657, 3361, 2401, 1788824363182007 }
            ,{ L_, 22145316257127, 3361, 2401, 9223372034054977092 }
            ,{ L_, 16807, 3361, 2401, 5600166597 }
            ,{ L_, 117649, 3361, 2401, 47600166597 }
            ,{ L_, 823543, 3361, 2401, 341600166597 }
            ,{ L_, 5764801, 3361, 2401, 2399600166597 }
            ,{ L_, 40353607, 3361, 2401, 16805600166597 }
            ,{ L_, 282475249, 3361, 2401, 117647600166597 }
            ,{ L_, 1977326743, 3361, 2401, 823541600166597 }
            ,{ L_, 13841287201, 3361, 2401, 5764799600166597 }
            ,{ L_, 96889010407, 3361, 2401, 40353605600166597 }
            ,{ L_, 678223072849, 3361, 2401, 282475247600166597 }
            ,{ L_, 4747561509943, 3361, 2401, 1977326741600166597 }

            // Frequency: 16807, Initial Time: 23529
            ,{ L_, 23530, 23529, 16807, 59499 }
            ,{ L_, 4294990824, 23529, 16807, 255546337537930 }
            ,{ L_, 4294990825, 23529, 16807, 255546337597429 }
            ,{ L_, 155017213799889, 23529, 16807, 9223372034054858094 }
            ,{ L_, 117649, 23529, 16807, 5600047599 }
            ,{ L_, 823543, 23529, 16807, 47600047599 }
            ,{ L_, 5764801, 23529, 16807, 341600047599 }
            ,{ L_, 40353607, 23529, 16807, 2399600047599 }
            ,{ L_, 282475249, 23529, 16807, 16805600047599 }
            ,{ L_, 1977326743, 23529, 16807, 117647600047599 }
            ,{ L_, 13841287201, 23529, 16807, 823541600047599 }
            ,{ L_, 96889010407, 23529, 16807, 5764799600047599 }
            ,{ L_, 678223072849, 23529, 16807, 40353605600047599 }
            ,{ L_, 4747561509943, 23529, 16807, 282475247600047599 }
            ,{ L_, 33232930569601, 23529, 16807, 1977326741600047599 }

            // Frequency: 117649, Initial Time: 164708
            ,{ L_, 164709, 164708, 117649, 8499 }
            ,{ L_, 4295132003, 164708, 117649, 36506619648275 }
            ,{ L_, 4295132004, 164708, 117649, 36506619656775 }
            ,{ L_, 1085120496599219, 164708, 117649, 9223372034054781596 }
            ,{ L_, 823543, 164708, 117649, 5600005099 }
            ,{ L_, 5764801, 164708, 117649, 47600005099 }
            ,{ L_, 40353607, 164708, 117649, 341600005099 }
            ,{ L_, 282475249, 164708, 117649, 2399600005099 }
            ,{ L_, 1977326743, 164708, 117649, 16805600005099 }
            ,{ L_, 13841287201, 164708, 117649, 117647600005099 }
            ,{ L_, 96889010407, 164708, 117649, 823541600005099 }
            ,{ L_, 678223072849, 164708, 117649, 5764799600005099 }
            ,{ L_, 4747561509943, 164708, 117649, 40353605600005099 }
            ,{ L_, 33232930569601, 164708, 117649, 282475247600005099 }
            ,{ L_, 232630513987207, 164708, 117649, 1977326741600005099 }

            // Frequency: 823543, Initial Time: 1152960
            ,{ L_, 1152961, 1152960, 823543, 1214 }
            ,{ L_, 4296120255, 1152960, 823543, 5215231378325 }
            ,{ L_, 4296120256, 1152960, 823543, 5215231379539 }
            ,{ L_, 7595843476194532, 1152960, 823543, 9223372034054775524 }
            ,{ L_, 5764801, 1152960, 823543, 5600000242 }
            ,{ L_, 40353607, 1152960, 823543, 47600000242 }
            ,{ L_, 282475249, 1152960, 823543, 341600000242 }
            ,{ L_, 1977326743, 1152960, 823543, 2399600000242 }
            ,{ L_, 13841287201, 1152960, 823543, 16805600000242 }
            ,{ L_, 96889010407, 1152960, 823543, 117647600000242 }
            ,{ L_, 678223072849, 1152960, 823543, 823541600000242 }
            ,{ L_, 4747561509943, 1152960, 823543, 5764799600000242 }
            ,{ L_, 33232930569601, 1152960, 823543, 40353605600000242 }
            ,{ L_, 232630513987207, 1152960, 823543, 282475247600000242 }
            ,{ L_, 1628413597910449, 1152960, 823543, 1977326741600000242 }

            // Frequency: 5764801, Initial Time: 8070721
            ,{ L_, 8070722, 8070721, 5764801, 173 }
            ,{ L_, 4303038016, 8070721, 5764801, 745033054046 }
            ,{ L_, 4303038017, 8070721, 5764801, 745033054219 }
            ,{ L_, 53170904333361727, 8070721, 5764801, 9223372034054775871 }
            ,{ L_, 40353607, 8070721, 5764801, 5600000069 }
            ,{ L_, 282475249, 8070721, 5764801, 47600000069 }
            ,{ L_, 1977326743, 8070721, 5764801, 341600000069 }
            ,{ L_, 13841287201, 8070721, 5764801, 2399600000069 }
            ,{ L_, 96889010407, 8070721, 5764801, 16805600000069 }
            ,{ L_, 678223072849, 8070721, 5764801, 117647600000069 }
            ,{ L_, 4747561509943, 8070721, 5764801, 823541600000069 }
            ,{ L_, 33232930569601, 8070721, 5764801, 5764799600000069 }
            ,{ L_, 232630513987207, 8070721, 5764801, 40353605600000069 }
            ,{ L_, 1628413597910449, 8070721, 5764801, 282475247600000069 }
            ,{ L_, 11398895185373143, 8070721, 5764801, 1977326741600000069 }

            // Frequency: 40353607, Initial Time: 56495049
            ,{ L_, 56495050, 56495049, 40353607, 24 }
            ,{ L_, 4351462344, 56495049, 40353607, 106433293435 }
            ,{ L_, 4351462345, 56495049, 40353607, 106433293459 }
            ,{ L_, 372196330333532089, 56495049, 40353607,
                                                          9223372034054775822 }
            ,{ L_, 282475249, 56495049, 40353607, 5600000019 }
            ,{ L_, 1977326743, 56495049, 40353607, 47600000019 }
            ,{ L_, 13841287201, 56495049, 40353607, 341600000019 }
            ,{ L_, 96889010407, 56495049, 40353607, 2399600000019 }
            ,{ L_, 678223072849, 56495049, 40353607, 16805600000019 }
            ,{ L_, 4747561509943, 56495049, 40353607, 117647600000019 }
            ,{ L_, 33232930569601, 56495049, 40353607, 823541600000019 }
            ,{ L_, 232630513987207, 56495049, 40353607, 5764799600000019 }
            ,{ L_, 1628413597910449, 56495049, 40353607, 40353605600000019 }
            ,{ L_, 11398895185373143, 56495049, 40353607, 282475247600000019 }
            ,{ L_, 79792266297612001, 56495049, 40353607, 1977326741600000019 }

            // Frequency: 282475249, Initial Time: 395465348
            ,{ L_, 395465349, 395465348, 282475249, 3 }
            ,{ L_, 4690432643, 395465348, 282475249, 15204756205 }
            ,{ L_, 4690432644, 395465348, 282475249, 15204756208 }
            ,{ L_, 2605374312334724624, 395465348, 282475249,
                                                          9223372034054775807 }
            ,{ L_, 1977326743, 395465348, 282475249, 5600000002 }
            ,{ L_, 13841287201, 395465348, 282475249, 47600000002 }
            ,{ L_, 96889010407, 395465348, 282475249, 341600000002 }
            ,{ L_, 678223072849, 395465348, 282475249, 2399600000002 }
            ,{ L_, 4747561509943, 395465348, 282475249, 16805600000002 }
            ,{ L_, 33232930569601, 395465348, 282475249, 117647600000002 }
            ,{ L_, 232630513987207, 395465348, 282475249, 823541600000002 }
            ,{ L_, 1628413597910449, 395465348, 282475249, 5764799600000002 }
            ,{ L_, 11398895185373143, 395465348, 282475249, 40353605600000002 }
            ,{ L_, 79792266297612001, 395465348, 282475249,
                                                           282475247600000002 }
            ,{ L_, 558545864083284007, 395465348, 282475249,
                                                          1977326741600000002 }

            // Frequency: 1977326743, Initial Time: 2768257440
            ,{ L_, 7063224735, 2768257440, 1977326743, 2172108029 }
            ,{ L_, 7063224736, 2768257440, 1977326743, 2172108029 }
            ,{ L_, 9223372036854775807, 2768257440, 1977326743,
                                                          4664566474275677344 }
            ,{ L_, 13841287201, 2768257440, 1977326743, 5600000000 }
            ,{ L_, 96889010407, 2768257440, 1977326743, 47600000000 }
            ,{ L_, 678223072849, 2768257440, 1977326743, 341600000000 }
            ,{ L_, 4747561509943, 2768257440, 1977326743, 2399600000000 }
            ,{ L_, 33232930569601, 2768257440, 1977326743, 16805600000000 }
            ,{ L_, 232630513987207, 2768257440, 1977326743, 117647600000000 }
            ,{ L_, 1628413597910449, 2768257440, 1977326743, 823541600000000 }
            ,{ L_, 11398895185373143, 2768257440, 1977326743,
                                                             5764799600000000 }
            ,{ L_, 79792266297612001, 2768257440, 1977326743,
                                                            40353605600000000 }
            ,{ L_, 558545864083284007, 2768257440, 1977326743,
                                                           282475247600000000 }
            ,{ L_, 3909821048582988049, 2768257440, 1977326743,
                                                          1977326741600000000 }
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

                // 'OUTPUT' has been calculated with integer arithmetic,
                // including one division operation.  Due to the loss of
                // fractional remainders in integer division, OUTPUT ==
                // floor(v), where v is the actual (infinite precision floating
                // point) number of nanoseconds corresponding to the input.
                // Because 'fakeConvertRawTime' performs two integer divisions,
                // its result will be either floor(v) or floor(v) - 1.

                LOOP5_ASSERT(LINE,
                             INPUT,
                             INITIAL_TIME,
                             FREQUENCY,
                             OUTPUT,
                             OUTPUT == fakeConvertRawTime(INPUT,
                                                             INITIAL_TIME,
                                                             FREQUENCY)
                             ||
                             OUTPUT - 1 == fakeConvertRawTime(INPUT,
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
                compareRealToFakeConvertRawTime(
                                              startTime,
                                              limit32Bits - startTime.d_opaque,
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
      case 8: {
        // --------------------------------------------------------------------
        // TESTING getTimerRaw() and convertRawTime()
        //
        // Concerns:
        //:  The return values from two successive calls to 'getTimerRaw()'
        //:  separated by zero or more simple operations, converted to
        //:  nanoseconds and subtracted, should produce non-negative results.
        //:  The concern of "reasonable" results is difficult to program and
        //:  for now will be observed manually.
        //
        // Plan:
        //:  Construct blocks containing two calls to 'getTimerRaw()'
        //:  bracketing various non-trivial (i.e., non-removable by an
        //:  optimizer) statements.  ASSERT that the differences of the
        //:  converted return values are always non-negative.  In verbose mode,
        //:  print the elapsed times so that they can be observed to be
        //:  "reasonable"
        //
        // Testing:
        //   Int64 convertRawTime(OpaqueNativeTime rawTime);
        //   OpaqueNativeTime getTimerRaw();
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

            if (verbose)
                printf("0: Elapsed time under test: " TI64 " nsec\n", dT);
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

            if (verbose)
                 printf("1: Elapsed time under test: " TI64 " nsec\n", dT);
            if (veryVerbose)
                 printf("Computed Values: %g\n", x);
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

            if (verbose)
                printf("2: Elapsed time under test: " TI64 " nsec\n", dT);
            if (veryVerbose)
                printf("Computed Values: %g %g\n", x, y);
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

            if (verbose)
                printf("3: Elapsed time under test: " TI64 " nsec\n", dT);
            if (veryVerbose)
                printf("Computed Values: %g %g %g\n", x, y, z);
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
                printf("10: Elapsed time under test: " TI64 " nsec\n", dT);
            if (veryVerbose)
                printf("Computed Values: %g %g %g\n", x, y, z);
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
                printf("100: Elapsed time under test: " TI64 " nsec\n", dT);
            if (veryVerbose)
                printf("Computed Values: %g %g %g\n", x, y, z);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** Windows only ***: bsls::TimeUtil::getTimer()
        //
        // Plan:
        //:  Call the method and check that the return value is not negative
        //:  (the uninitialized values for s_initialTime and s_timerFrequency
        //:  are -1).
        //
        // Testing:
        //   Initialization test: getTimer (Windows only)
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS
        if (verbose)
            printf("\nTesting TimeUtil initialization (wall timer)"
                   "\n============================================\n");

        Int64 t = TU::getTimer();
        if (verbose) { T_; P_(t); }
        ASSERT(t >= 0);
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST 'gethrtime()' *** Sun and HP ONLY ***
        //   Test whether successive calls ever return non-increasing values.
        //
        // Plan:
        //:  Call 'gethrtime()' two times within a loop, and compare the return
        //:  values each time.
        //
        // Testing:
        //   Test of 'gethrtime()' (Sun and HP only -- statistical)
        // --------------------------------------------------------------------

#if defined (BSLS_PLATFORM_OS_SOLARIS)
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
      case 5: {
        // --------------------------------------------------------------------
        // NON-WALL TIMERS ON UNIX
        //
        //   Verify that the 'get*Timer' functions are returning values from
        //   the correct clock sources.
        //
        //   Since the 'getProcessSystemTimer' and 'getProcessUserTimer'
        //   functions simply wrap a system function, we can have confidence in
        //   the values returned as long as we can confirm each function is
        //   wrapping the *correct* system function.  In older versions of this
        //   test driver, and on Windows, this was done by comparing changes in
        //   all of the timer functions in scenarios where we know that one of
        //   the three timer resources (system and user time) is being heavily
        //   used while the other two are only lightly used.  Such a method is
        //   error-prone, and subject to false positives on systems under heavy
        //   load.
        //
        //   Since the conversion of the implementation from calling 'times' to
        //   calling 'getrusage' for system and user times, we can instead use
        //   'times' itself as an oracle.  If the functions under test return
        //   the same values as 'times' -- albeit at a very different precision
        //   -- then we know that they are wrapping the correct system
        //   functions.
        //
        //   This approach is complicated by the fact that 'getrusage' and
        //   'times' have different precisions, and that their values can never
        //   guarantee that a call to the function under test will happen at
        //   exactly the same time as a given call to 'times'.  Instead, we can
        //   rely on the fact that subseuqent calls to either 'times' or the
        //   function under test must show a monotonically non-decreasing use
        //   of the same time source.  If we alternately read from 'times' and
        //   from 'getrusage', and each alternating call shows a time greater
        //   than or equal to the previous call, then we can have confidence
        //   that both are measuring the same time source.
        //
        // Concerns:
        //:  1. Values returned by 'getProcessSystemTimer' should come from
        //:     'getrusage'.
        //:
        //:  2. Values returned by 'getProcessUserTimer' should come from
        //:     'getrusage'.
        //:
        //:  3. Values returned by 'getProcessSystemTimer' returns system CPU
        //:     time.
        //:
        //:  4. Values returned by 'getProcessUserTimer' returns user CPU time.
        //
        // Plan:
        //:  1. For each timer (system and user):
        //:
        //:     1. Take initial measurements of appropriate model and utility
        //:        timers as baseline values.
        //:
        //:     2. Execute an inner loop. On each iteration measure the model
        //:        timer value and then the utility timer value.  Observe that
        //:        the two timers leapfrog each other as new values are
        //:        collected.  (C-1..4)
        //:
        //:     3. Note that, because the model timer value has a lower
        //:        precision than the utility timer value, it will be necessary
        //:        to:
        //:
        //:        1. Take successive model timer value measurements as we wait
        //:           to observe a change in the model value.
        //:
        //:        2. When a new model timer value is measured, compare the
        //:           *end* of that tick interval with the previously measured
        //:           utility timer value.  This accounts for the following
        //:           relationship between the (high precision) utility timer
        //:           value and the (low precision) model timer value:
        //:
        //:            < ... model TICK_INTERVAL ... >
        //:           |-------------------------------|
        //:                                 ^   ^
        //:           1                     u   m     2
        //:
        //:           The model tick value 'm', measured after the utility
        //:           tick value 'u', will report that we are on tick '1' even
        //:           though we have almost arrived at tick '2'.  Therefore, we
        //:           can assert that 'u < m + TICK_INTERVAL', but not
        //:           necessarily that 'u < m'.
        //
        // Testing:
        //   Forwarding of methods to underlying OS APIs (Unix only)
        // --------------------------------------------------------------------

#if    defined(BSLS_PLATFORM_OS_UNIX)                                         \
    && !(defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CPU_32_BIT))

        if (verbose) {
            printf("\nNON-WALL TIMERS ON UNIX."
                   "\n========================\n");
        }

        if (veryVeryVerbose) {
            printf("\tModel cpu time tick duration:  " TI64 " nanoseconds.\n",
                   MODEL_CPU_TICK_DURATION);
        }

        const int NUM_INTERVALS = 10;

        // For each time (system and user) we call 'utility' and 'model'
        // functions in turn to show that the following relationship exists:
        //
        //   model values:  a    b    c    d    e    f
        //
        //   util values:     A    B    C    D    E     F
        //
        //   a <= A <= b <= B <= c <= C <= d <= D <= e <= E <= f
        //
        // It is the *chain* of inequalities that establishes that the two
        // clocks are measuring the same quantity.

        if (verbose) {
            printf("\nTesting 'getProcessSystemTimer()'.\n\n");
        }

        {
            SystemTimeResource resource = acquireSystemTimeResource();

            Int64 modelSystemTime = getModelSystemTime();
            Int64 utilSystemTime = TU::getProcessSystemTimer();

            // Collect initial values for sanity check

            Int64 initialModelSystemTime = modelSystemTime;
            Int64 initialModelUserTime = getModelUserTime();

            if (veryVeryVerbose) {
                T_ T_ P_(modelSystemTime) P(utilSystemTime)
            }

            ASSERT(modelSystemTime <= utilSystemTime);

            for (int i = 0; i < NUM_INTERVALS; i++) {
                if (veryVerbose) {
                    T_ P_(i)
                }

                modelSystemTime = getModelSystemTime();
                ASSERTV(i, modelSystemTime, utilSystemTime,
                        utilSystemTime <=
                                    modelSystemTime + MODEL_CPU_TICK_DURATION);

                do {
                    if (veryVerbose) {
                        printf(".");
                    }

                    // Burn some system time

                    burnSystemTime(resource);

                    modelSystemTime = getModelSystemTime();
                    ASSERTV(i, modelSystemTime, utilSystemTime,
                            utilSystemTime <=
                                    modelSystemTime + MODEL_CPU_TICK_DURATION);
                } while (modelSystemTime <= utilSystemTime);

                if (veryVerbose) {
                    printf("\n");
                }

                utilSystemTime = TU::getProcessSystemTimer();
                ASSERTV(i, modelSystemTime, utilSystemTime,
                        modelSystemTime <= utilSystemTime);

                if (veryVeryVerbose) {
                    T_ T_ P_(modelSystemTime) P(utilSystemTime)
                }
            }

            // Sanity check: system time consumed more than user time

            Int64 finalModelUserTime = getModelUserTime();
            Int64 finalModelSystemTime = modelSystemTime;
            Int64 elapsedUserTime =
                                 finalModelUserTime - initialModelUserTime;
            Int64 elapsedSystemTime =
                                 finalModelSystemTime - initialModelSystemTime;

            if (veryVeryVerbose) {
                T_ P_(initialModelSystemTime) P_(finalModelSystemTime)
                T_ P_(initialModelUserTime) P_(finalModelUserTime)
                T_ P_(elapsedSystemTime) P(elapsedUserTime)
            }

            ASSERTV(initialModelSystemTime,
                    finalModelSystemTime,
                    initialModelUserTime,
                    finalModelUserTime,
                    elapsedUserTime < elapsedSystemTime);

            if (veryVeryVerbose) {
                if (elapsedUserTime) {
                    P(elapsedSystemTime / elapsedUserTime);
                }
                else {
                    P("no elapsed user time")
                }
            }

            releaseSystemTimeResource(resource);
        }

        if (verbose) {
            printf("\nTesting 'getProcessUserTimer()'.\n\n");
        }

        {
            Int64 modelUserTime = getModelUserTime();
            Int64 utilUserTime = TU::getProcessUserTimer();

            // Collect initial values for sanity check

            Int64 initialModelUserTime = modelUserTime;
            Int64 initialModelSystemTime = getModelSystemTime();

            if (veryVeryVerbose) {
                T_ T_ P_(modelUserTime) P(utilUserTime)
            }

            ASSERT(modelUserTime <= utilUserTime);

            for (int i = 0; i < NUM_INTERVALS; i++) {
                if (veryVerbose) {
                    T_ P_(i)
                }

                modelUserTime = getModelUserTime();
                ASSERTV(i, modelUserTime, utilUserTime,
                        utilUserTime <=
                                      modelUserTime + MODEL_CPU_TICK_DURATION);

                do {
                    if (veryVerbose) {
                        printf(".");
                    }


                    // Burn some user time

                    volatile int u = 0;
                    burnUserTime(&u);

                    modelUserTime = getModelUserTime();
                    ASSERTV(i, modelUserTime, utilUserTime,
                            utilUserTime <=
                                      modelUserTime + MODEL_CPU_TICK_DURATION);
                } while (modelUserTime <= utilUserTime);

                if (veryVerbose) {
                    printf("\n");
                }

                utilUserTime = TU::getProcessUserTimer();
                ASSERTV(i, modelUserTime, utilUserTime,
                        modelUserTime <= utilUserTime);

                if (veryVeryVerbose) {
                    T_ T_ P_(modelUserTime) P(utilUserTime)
                }
            }

            // Sanity check: system time consumed more than user time

            Int64 finalModelSystemTime = getModelSystemTime();
            Int64 finalModelUserTime = modelUserTime;
            Int64 elapsedSystemTime =
                                 finalModelSystemTime - initialModelSystemTime;
            Int64 elapsedUserTime =
                                 finalModelUserTime - initialModelUserTime;

            if (veryVeryVerbose) {
                T_ P_(initialModelSystemTime) P_(finalModelSystemTime)
                T_ P_(initialModelUserTime) P_(finalModelUserTime)
                T_ P_(elapsedSystemTime) P(elapsedUserTime)
            }

            ASSERTV(initialModelUserTime,
                    finalModelUserTime,
                    initialModelSystemTime,
                    finalModelSystemTime,
                    elapsedSystemTime < elapsedUserTime);

            if (veryVeryVerbose) {
                if (elapsedSystemTime) {
                    P(elapsedUserTime / elapsedSystemTime);
                }
                else {
                    P("no elapsed system time")
                }
            }
        }
#endif
      } break;
      case 4: {
        // -------------------------------------------------------------
        // HOOKING TEST
        //
        // Concerns:
        //:  That the wall time, user time, and system time calls are making
        //:  the correct calls into the OS.
        //
        // Plan:
        //:  1. Take all the values, then sleep, then check how they changed.
        //:     Verify that the values returned to us are compatible with the
        //:     OS timers that the calls should be returning.
        //:
        //:  2. Make some repeated time calls; check how they changed.  Verify
        //:     that the values returned to us are compatible with the OS
        //:     timers that the calls should be returning.
        //:
        //:  3. Burn some CPU time, then check how they changed.  Verify that
        //:     the values returned to us are compatible with the OS timers
        //:     that the calls should be returning.  (These are imperfect
        //:     tests; we can sleep longer than we expect, and interrupt and
        //:     scheduling activities can add to the user and system times.)
        //
        // Testing:
        //   Forwarding of methods to underlying OS APIs
        // --------------------------------------------------------------------

        if (verbose) printf("\nHOOKING TEST"
                            "\n============\n");

        //  The initialization of 'timeQuantum' varies from OS to OS.
        //  'timeQuantum' is to be the minimum increment visible in a
        //  timer, expressed as a number of nanoseconds.  POSIX wants it
        //  to be 100 milliseconds.
#if defined BSLS_PLATFORM_OS_SOLARIS || defined BSLS_PLATFORM_OS_FREEBSD
        const Int64 timeQuantum = nsecsPerSecond / CLK_TCK;
#elif defined BSLS_PLATFORM_OS_LINUX || defined BSLS_PLATFORM_OS_AIX    \
   || defined BSLS_PLATFORM_OS_DARWIN
        const Int64 timeQuantum = nsecsPerSecond / sysconf(_SC_CLK_TCK);
                                        // On our local flavor of Linux, old
                                        // POSIX requirements and immoderate
                                        // file system cleverness combine to
                                        // make the symbol that is available
                                        // wrong (CLOCKS_PER_SEC == 1000000)
                                        // and the symbol that is correct
                                        // (CLK_TCK) unavailable.
                                        // (AIX just walks its own path.)
#elif defined BSLS_PLATFORM_OS_WINDOWS || defined BSLS_PLATFORM_OS_CYGWIN
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

            const unsigned shortSleep = 200;
            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            osMillisleep(shortSleep);

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose) {
                P_(wt1) P_(ut1) P(st1)
                P_(wt2) P_(ut2) P(st2)
                P(wt2 - wt1)
            }

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
            // System and user time differences must be zero mod quantum, or
            // quantum is wrong.

            ASSERT(0 == (ut2 - ut1) % timeQuantum);
            ASSERT(0 == (st2 - st1) % timeQuantum);
#else
            // On Unix systems, we now measure system time and user time with
            // calls that do not have an explicit quantum.  'timeQuantum' is
            // still marginally useful on those platforms as a notion of
            // "little time used", but it has no deeper mathematical
            // relationship with the values we expect to observe.
#endif

            // Comparisons on times must be written in such a way that rollover
            // can never corrupt the result.  (With 'long long' values, this is
            // unlikely, but not eventually impossible.)  This means that
            // the times must be grouped on one side of the inequality and
            // their differences taken, and those differences compared to
            // whatever constants are involved.

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
            ASSERT(wt2 - wt1 + windowsFudge >=
                                             shortSleep * nsecsPerMillisecond);
#else
            ASSERT(wt2 - wt1 >= shortSleep * nsecsPerMillisecond);
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
            struct Sample {
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
                Sample& s0 = samples[i];
                Sample& s1 = samples[i+1];

                s1.d_wt = TU::getTimer();
                s1.d_ut = TU::getProcessUserTimer();
                s1.d_st = TU::getProcessSystemTimer();

                // Sys and user differences must be zero mod quantum, or
                // quantum is wrong.

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
                ASSERT(0 == (s1.d_ut - s0.d_ut) % timeQuantum);
                ASSERT(0 == (s1.d_st - s0.d_st) % timeQuantum);
#else
                // On Unix systems, we now measure system time and user time
                // with calls that do not have an explicit quantum.
                // 'timeQuantum' is still marginally useful on those platforms
                // as a notion of "little time used", but it has no deeper
                // mathematical relationship with the values we expect to
                // observe.
#endif

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
                    Sample& s0 = samples[i - 1];
                    Sample& s1 = samples[i];

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

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
            ASSERT((wt2 - wt1) - (ut2 - ut1) - (st2 - st1)
                                   + 10 * windowsFudge + 2 * timeQuantum >= 0);
#else
            ASSERT((wt2 - wt1) - (ut2 - ut1) - (st2 - st1) +
                                                         2 * timeQuantum >= 0);
                                        // And our wall time was greater than
                                        // our user and system time together,
                                        // allowing for quantization error
                                        // (in both user and system time).
#endif
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SANITY CHECK: VALUES DO NOT REPEAT
        //   Test whether successive calls ever return the same value.
        //
        // Plan:
        //:  Call each method two times within a loop, and compare the return
        //:  values each time.
        //
        // Testing:
        //   Successive timer values do not repeat
        // --------------------------------------------------------------------

        if (verbose) printf("\nSANITY CHECK: VALUES DO NOT REPEAT"
                            "\n==================================\n");

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
                printf("\nCall 'bsls::TimeUtil::%s()' twice in a large loop."
                           "\n",
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
        //:  Call each method in a loop.
        //
        // Testing:
        //   Performance Test
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

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
        //:  Verify that the *temporary* returned by each method is at least 8
        //:  bytes long.  Then invoke the method several times in sequence,
        //:  separated by delay loops of increasing duration, and assert that
        //:  the return value does not decrease.  Print results and differences
        //:  in 'veryVerbose' mode.
        //
        // Testing:
        //   bsls::Types::Int64 bsls::TimeUtil::getTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessSystemTimer();
        //   bsls::Types::Int64 bsls::TimeUtil::getProcessUserTimer();
        //   void bsls::TimeUtil::getProcessTimers(bsls::Types::Int64,
        //                                        bsls::Types::Int64);
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC FUNCTIONALITY"
                            "\n===================\n");

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
      case -1: {
        // --------------------------------------------------------------------
        // UNIX TIMERS (comparison with model)
        //
        //   Test that the timers measure the correct resources on UNIX.  This
        //   test relies on comparing the behavior of utility functions to that
        //   of an oracle.  This is possible on UNIX because all supported UNIX
        //   systems provide 'times', which is not used in the current
        //   implementation.  For wall time, the oracle is 'gettimeofday',
        //   which is not used by the implementation on *most* platforms.  On
        //   platforms where 'gettimeofday' is used by the implementation, this
        //   test is weak.  For these reasons, this test is not included in the
        //   automatically-run cases "above the line".
        //
        // Concerns:
        //:  1. The first argument to 'getProcessTimers' measures system time.
        //:
        //:  2. The second argument to 'getProcessTimers' measures user time.
        //:
        //:  3. 'getTimer' measures wall time.
        //:
        //:  4. All three timers are in sync with the values reported by native
        //:     system calls.
        //
        // Plan:
        //:  1. For each of the time resources in question (system, user and
        //:     wall time), consume that resource while making successive
        //:     measurements with the utility functions 'getProcessTimers' and
        //:     'getTimer' on the one hand, and the system calls 'times' and
        //:     'gettimeofday' on the other hand.
        //:
        //:  2. Observe that the values observed by the utility functions are
        //:     monotonically non-decreasing, and that each timer value
        //:     observed by the utility functions are within an epsilon of the
        //:     corresponding timer value observed by the system calls.
        //:     (C-1..4)
        //
        // Testing:
        //   getProcessTimers(*Int64, *Int64) (Unix only)
        //   Int64 getTimer() (Unix only)
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_UNIX)
        if (verbose) printf("\nUNIX TIMERS (comparison with model)"
                            "\n===================================\n");

        {
            // We are going to perform some actions, measure them with two
            // different functions (used by tested utility and another system
            // function) and compare results.  First of all we need to
            // determine acceptable difference.

            // The most that the model and utility functions could diverge is
            // double the resolution of the lowest-resolution measurement.

            const Int64 cpuEpsilon = 2 * MODEL_CPU_TICK_DURATION;

            // Pessimistic assumption: a wall timer resolution may be as coarse
            // as the low-resolution CPU timers.

            const Int64 wallEpsilon = 2 * MODEL_CPU_TICK_DURATION;

            enum { NUM_SAMPLES = 10 };

            struct Sample {
                Int64 d_wt;  // wall time
                Int64 d_ut;  // user time
                Int64 d_st;  // system time
            };

            if (veryVerbose) {
                printf("Wall time epsilon: " TI64 "\n"
                       "CPU  time epsilon: " TI64 "\n",
                       wallEpsilon,
                       cpuEpsilon);
            }

            for (int i = 0; i < NUM_SCENARIOS; ++i) {
                if (veryVerbose) {
                    printBanner(static_cast<Scenario>(i));
                }
                Sample utilSamples[NUM_SAMPLES];
                Sample modelSamples[NUM_SAMPLES];

                // Collecting initial values.

                utilSamples[0].d_wt = TU::getTimer();
                getModelWallTime(&modelSamples[0].d_wt);

                TU::getProcessTimers(&utilSamples[0].d_st,
                                     &utilSamples[0].d_ut);
                getModelCpuTime(&modelSamples[0].d_st, &modelSamples[0].d_ut);

                for (int j = 1; j < NUM_SAMPLES; j++) {
                    // Time burning action.

                    // TODO: keep running scenarios until we see at least two
                    // low-resolution ticks of difference in the
                    // high-resolution measurement for the resoiurce we are
                    // targeting.

                    runTestScenario(static_cast<Scenario>(i), veryVerbose);

                    // Results collecting.

                    utilSamples[j].d_wt = TU::getTimer();
                    getModelWallTime(&modelSamples[j].d_wt);

                    TU::getProcessTimers(&utilSamples[j].d_st,
                                         &utilSamples[j].d_ut);
                    getModelCpuTime(&modelSamples[j].d_st,
                                    &modelSamples[j].d_ut);
                }

                for (int j = 1; j < NUM_SAMPLES; ++j) {
                    // Results analysing.

                    Int64 uWTDelta = utilSamples[j].d_wt -
                                                         utilSamples[j-1].d_wt;
                    Int64 uUTDelta = utilSamples[j].d_ut -
                                                         utilSamples[j-1].d_ut;
                    Int64 uSTDelta = utilSamples[j].d_st -
                                                         utilSamples[j-1].d_st;

                    Int64 mWTDelta = modelSamples[j].d_wt -
                                                        modelSamples[j-1].d_wt;
                    Int64 mUTDelta = modelSamples[j].d_ut -
                                                        modelSamples[j-1].d_ut;
                    Int64 mSTDelta = modelSamples[j].d_st -
                                                        modelSamples[j-1].d_st;

                    if (veryVerbose) {
                        P(j)
                        T_ P_(utilSamples[j].d_wt) P(utilSamples[j-1].d_wt)
                        T_ P_(modelSamples[j].d_wt) P(modelSamples[j-1].d_wt)
                        T_ P_(utilSamples[j].d_ut) P(utilSamples[j-1].d_ut)
                        T_ P_(modelSamples[j].d_ut) P(modelSamples[j-1].d_ut)
                        T_ P_(utilSamples[j].d_st) P(utilSamples[j-1].d_st)
                        T_ P_(modelSamples[j].d_st) P(modelSamples[j-1].d_st)
                    }

                    ASSERTV(i, j, utilSamples[j].d_wt, utilSamples[j-1].d_wt,
                            0 <= uWTDelta);
                    ASSERTV(i, j, utilSamples[j].d_ut, utilSamples[j-1].d_ut,
                            0 <= uUTDelta);
                    ASSERTV(i, j, utilSamples[j].d_st, utilSamples[j-1].d_st,
                            0 <= uSTDelta);

                    ASSERTV(i, j, mWTDelta, uWTDelta,
                            wallEpsilon >= llabs(mWTDelta - uWTDelta));
                    ASSERTV(i, j, mUTDelta, uUTDelta,
                            cpuEpsilon  >= llabs(mUTDelta - uUTDelta));
                    ASSERTV(i, j, mSTDelta, uSTDelta,
                            cpuEpsilon  >= llabs(mSTDelta - uSTDelta));
                }
            }
        }
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // TIMER INVARIANTS TEST
        //   This test is an attempt at demonstrating in a simple
        //   cross-platform manner that the timers provided by the component
        //   under test each measure the correct resource.  Since not all
        //   platforms provide alternative ways to measure the resources, and
        //   because it is often difficult to make direct comparisons of values
        //   returned by different system calls, this test does not use an
        //   oracle, and instead asserts that certain expected properties hold
        //   for the system, user and wall times measured by the component.
        //
        //   However, we note that every system call can consume arbitrary
        //   amounts of system, user and wall time, due to the possibility of
        //   being swapped out in favor of another process, or spinning (in
        //   either user or kernel mode) while waiting for access to the
        //   underlying system resource.  For this reason, there are very few
        //   properties that we can confidently assert.  Furthermore, the
        //   properties that we do assert in this test case may occasionally
        //   fail to hold.  For this reason, this test is not included in the
        //   automatically-run tests "above the line".
        //
        //   This test should be run manually to confirm that the timers behave
        //   sanely on a new platform.  If a *consistent* failure is observed,
        //   then there is a problem with the implementation on that platform.
        //   Otherwise (i.e. if there are only intermittent failures) this test
        //   demonstrates that the component is returning values provided by
        //   the correct underlying system calls.
        //
        // Concerns:
        //:  1. The values for system, user and wall time measured by the
        //:     component under test respect the expected invariants for the
        //:     underlying resources:
        //:
        //:     1. Observed system time use is greater when system time is
        //:        being intensively consumed than when the process sleeps.
        //:
        //:     2. Observed system time use is minimal when user time is being
        //:        intensively consumed.
        //:
        //:     3. Observed system time use is minimal when wall time is being
        //:        intensively consumed.
        //:
        //:     4. Observed user time use is greater than observed system time
        //:        use when user time is being intensively consumed.
        //:
        //:     5. Observed user time use is greater when user time is
        //:        being intensively consumed than when the process sleeps.
        //:
        //:     6. Observed user time use is minimal when wall time is being
        //:        intensively consumed.
        //:
        //:     7. Observed wall time use is greater than observed user time
        //:        use when wall time is being intensively consumed.
        //:
        //:     8. Observed wall time use is greater than observed system time
        //:        use when wall time is being intensively consumed.
        //:
        //:     9. Observed wall time use is greater than observed system time
        //:        use when user time is being intensively consumed.
        //:
        //:     10. Observed wall time use is greater than observed user time
        //:         use when system time is being intensively consumed.
        //
        // Plan:
        //:  1. For each type of time resource (system, user and wall), try to
        //:     burn only that resource, until a measurable amount of the
        //:     resource has been used.
        //:
        //:  2. Measure the amount used of each of the three time resources.
        //:
        //:  3. Assert each of the invariants listed in the concerns. (C-1)
        //
        // Testing:
        //   CONCERN: Timer results respect invariants
        // --------------------------------------------------------------------

        if (verbose) printf("\nTIMER INVARIANTS TEST"
                            "\n=====================\n");

        {
            enum { e_SYSTEM = 0, e_USER, e_WALL, k_NUM_CLOCKS };

            Int64 sample [k_NUM_CLOCKS][k_NUM_CLOCKS] = { };

            SystemTimeResource resource = acquireSystemTimeResource();

            // Sample clock deltas for different scenarios

            for (int i = 0; i < k_NUM_CLOCKS; ++i) {
                if (veryVerbose) {
                    T_ P(i)
                }

                Int64 initial[k_NUM_CLOCKS] = { };
                Int64 measuredST = 0;
                Int64 measuredUT = 0;
                Int64 measuredWT = 0;

                TU::getProcessTimers(&initial[e_SYSTEM], // system time
                                     &initial[e_USER]);  // user time
                initial[e_WALL] = TU::getTimer();        // wall time

                do {
                    switch (i) {
                      case e_SYSTEM: {
                        burnSystemTime(resource);
                      } break;
                      case e_USER: {
                        volatile int u = 0;
                        burnUserTime(&u);
                      } break;
                      case e_WALL: {
                        burnWallTime();
                      } break;
                      default: {
                        ASSERT(false);
                      }
                    }

                    // Collect resource measurements as quickly as possible.

                    TU::getProcessTimers(&measuredST,  // system time
                                         &measuredUT); // user time
                    measuredWT = TU::getTimer();       // wall time

                    sample[i][e_SYSTEM] = measuredST - initial[e_SYSTEM];
                    sample[i][e_USER]   = measuredUT - initial[e_USER];
                    sample[i][e_WALL]   = measuredWT - initial[e_WALL];

                    // Keep going until the resource we are interested in
                    // registers a change.

                } while(0 == sample[i][i]);

                if (veryVeryVerbose) {
                    T_ T_ P_(sample[i][e_SYSTEM])
                          P_(sample[i][e_USER])
                          P(sample[i][e_WALL])
                }
            }

            // System clock should be used more than user clock on the
            // 'e_SYSTEM' pass, and should account for most of the wall time
            // used:

            //  Fails on some platforms.  Spinning in user mode waiting for
            //  system counter to become available?  S[u] < S[s]

            // ASSERTV(sample[e_USER  ][e_SYSTEM],
            //         sample[e_SYSTEM][e_SYSTEM],
            //         sample[e_USER  ][e_SYSTEM] <=
            //                                     sample[e_SYSTEM][e_SYSTEM]);

            // C-1.1

            ASSERTV(sample[e_WALL  ][e_SYSTEM],
                    sample[e_SYSTEM][e_SYSTEM],
                    sample[e_WALL  ][e_SYSTEM] < sample[e_SYSTEM][e_SYSTEM]);

            // Fails on some platforms.  Multi-threaded implementation of
            // 'getrusage'?: S[s] < W[s]

            //ASSERTV(sample[e_SYSTEM][e_WALL  ],
            //        sample[e_SYSTEM][e_SYSTEM],
            //        sample[e_SYSTEM][e_SYSTEM] < sample[e_SYSTEM][e_WALL  ]);

            // 'MODEL_CPU_TICK_DURATION' is a bad candidate for upper bound for
            // unused resources, since even *used* resources (like S[s] or
            // U[u]) might not reach this level of usage.  But in the absence
            // of a 'getrusage' tick size there is no reasonable alternate
            // bound available.

            // C-1.2

            ASSERTV(sample[e_USER  ][e_SYSTEM],
                    sample[e_USER  ][e_SYSTEM] < MODEL_CPU_TICK_DURATION);

            // C-1.3

            ASSERTV(sample[e_WALL  ][e_SYSTEM],
                    sample[e_WALL  ][e_SYSTEM] < MODEL_CPU_TICK_DURATION);

            // User clock should be used more than the system clock on the
            // 'e_USER' pass, and should account for most of the wall time
            // used:

            // C-1.4

            ASSERTV(sample[e_USER  ][e_SYSTEM],
                    sample[e_USER  ][e_USER  ],
                    sample[e_USER  ][e_SYSTEM] < sample[e_USER][e_USER]);

            // C-1.5

            ASSERTV(sample[e_WALL  ][e_USER],
                    sample[e_USER  ][e_USER],
                    sample[e_WALL  ][e_USER] < sample[e_USER][e_USER]);

            // Fails on some platforms.  Multi-threaded implementation of
            // 'getrusage'?: U[u] < W[u]

            // ASSERTV(sample[e_USER  ][e_WALL  ],
            //         sample[e_USER  ][e_USER  ],
            //         sample[e_USER  ][e_USER  ] < sample[e_USER][e_WALL  ]);

            // C-1.6

            ASSERTV(sample[e_WALL  ][e_USER  ],
                    sample[e_WALL  ][e_USER  ] < MODEL_CPU_TICK_DURATION);

            // Wall clock should dominate on the 'e_WALL' pass:

            // C-1.7

            ASSERTV(sample[e_WALL  ][e_USER  ],
                    sample[e_WALL  ][e_WALL  ],
                    sample[e_WALL  ][e_USER  ] < sample[e_WALL][e_WALL]);

            // C-1.8

            ASSERTV(sample[e_WALL  ][e_SYSTEM],
                    sample[e_WALL  ][e_WALL  ],
                    sample[e_WALL  ][e_SYSTEM] < sample[e_WALL][e_WALL]);

            // C-1.9

            ASSERTV(sample[e_USER  ][e_SYSTEM],
                    sample[e_USER  ][e_WALL  ],
                    sample[e_USER  ][e_SYSTEM] < sample[e_USER][e_WALL]);

            // C-1.10

            ASSERTV(sample[e_SYSTEM][e_USER  ],
                    sample[e_SYSTEM][e_WALL  ],
                    sample[e_SYSTEM][e_USER  ] < sample[e_SYSTEM][e_WALL]);

            releaseSystemTimeResource(resource);
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

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
