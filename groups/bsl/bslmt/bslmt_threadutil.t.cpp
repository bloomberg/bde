// bslmt_threadutil.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmt_configuration.h>
#include <bslmt_threadattributes.h>
#include <bslmt_platform.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_c_limits.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_set.h>

#include <errno.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS
#include <pthread.h>
#include <sys/wait.h>  // wait
#include <unistd.h>    // fork

#ifdef BSLS_PLATFORM_OS_SOLARIS
#include <sys/utsname.h>
#endif

#endif

#ifndef BSLS_PLATFORM_OS_WINDOWS
#include <alloca.h>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <thread>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::ThreadUtil       Obj;
typedef bslmt::ThreadAttributes Attr;
typedef bsls::Types::IntPtr     IntPtr;

int verbose;
int veryVerbose;
int veryVeryVerbose;

bool isPost_5_10;    // On Solaris, is OS post-Solaris 5.0

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_AIX)
// On Windows, the thread name will only be set if we're running on Windows 10,
// version 1607 or later, otherwise it will be empty. AIX does not support
// thread naming.
static const bool k_threadNameCanBeEmpty = true;
#else
static const bool k_threadNameCanBeEmpty = false;
#endif

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_FAIL_RAW(expr)  BSLS_ASSERTTEST_ASSERT_FAIL_RAW(expr)
#define ASSERT_PASS_RAW(expr)  BSLS_ASSERTTEST_ASSERT_PASS_RAW(expr)

#if !defined(BSLS_PLATFORM_OS_CYGWIN)
    const int MIN_GUARD_SIZE = 0;
#else
    const int MIN_GUARD_SIZE = 1;
#endif

// ============================================================================
//                       GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

IntPtr intPtrAbs(IntPtr a)
{
    return a >= 0 ? a : -a;
}

bsl::ostream& operator<<(bsl::ostream&                             stream,
                         bslmt::ThreadAttributes::SchedulingPolicy policy)
{
    switch (policy) {
#undef CASE
#define CASE(x) case bslmt::ThreadAttributes::x: stream << #x; break
      CASE(e_SCHED_OTHER);
      CASE(e_SCHED_FIFO);
      CASE(e_SCHED_RR);
      CASE(e_SCHED_DEFAULT);
      default:  stream << "<UNKNOWN>";
#undef CASE
    }

    return stream;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN03

            // ==================
            // class AnotherClock
            // ==================

class AnotherClock {
    // 'AnotherClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The only difference is that it uses a
    // different epoch; it begins 10000 "ticks" after the beginning of
    // 'steady_clock's epoch.

  private:
    // PRIVATE TYPES
    typedef bsl::chrono::steady_clock base_clock;

  public:
    // TYPES
    typedef base_clock::duration                  duration;
    typedef base_clock::rep                       rep;
    typedef base_clock::period                    period;
    typedef bsl::chrono::time_point<AnotherClock> time_point;

    // PUBLIC CLASS DATA
    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
AnotherClock::time_point AnotherClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return AnotherClock::time_point(ret - duration(10000));
}

            // ===============
            // class HalfClock
            // ===============

class HalfClock {
    // 'HalfClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The difference is that it runs "half as
    // fast" as 'steady_clock'.

  private:
    // PRIVATE TYPES
    typedef bsl::chrono::steady_clock base_clock;

  public:
    // TYPES
    typedef base_clock::duration               duration;
    typedef base_clock::rep                    rep;
    typedef base_clock::period                 period;
    typedef bsl::chrono::time_point<HalfClock> time_point;

    // PUBLIC CLASS DATA
    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
HalfClock::time_point HalfClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return HalfClock::time_point(ret/2);
}

// BDE_VERIFY pragma: pop

template <class REP_TYPE, class PERIOD_TYPE>
bool SleepOnADuration(
                 const bsl::chrono::duration<REP_TYPE, PERIOD_TYPE>& sleepTime)
    // Sleep for the specified 'sleepTime'.  Return 'true' if the elapsed time
    // is greater than 'sleepTime', and 'false' otherwise.
{
    using namespace bsl::chrono;

    const steady_clock::time_point tp = steady_clock::now();
    bslmt::ThreadUtil::sleep(sleepTime);
    return (steady_clock::now() - tp) >= sleepTime;
}

template <class CLOCK>
bool SleepOnAClock(int secondsToSleep)
    // Sleep until the specified 'CLOCK' has counted off the specified
    // 'secondsToSleep' seconds.  Return 'true' if the elapsed time on the
    // 'CLOCK' has elapsed, and 'false' otherwise.
{
    typename CLOCK::time_point tp = CLOCK::now() +
                                          bsl::chrono::seconds(secondsToSleep);
    bslmt::ThreadUtil::sleepUntil(tp);
    return CLOCK::now() >= tp;
}
#endif

bool SleepOnATimeInterval(bsls::Types::Int64 seconds, int nanoseconds)
    // Convert the specified 'seconds' and 'nanoseconds' into a
    // 'bsls::TimeInterval'.  Sleep for that amount of time.  Return 'true' if
    // the elapsed time is greater than 'sleepTime', and 'false' otherwise.
{
    ASSERT(bsls::TimeInterval::isValid(seconds, nanoseconds));
    bsls::TimeInterval sleepTime(seconds, nanoseconds);
    bsls::Stopwatch    timer;
    const double       k_TOLERANCE = 0.001;    // 1 ms
    const double       minTime = 1.0e-9 * nanoseconds +
                                                  static_cast<double>(seconds);
    timer.start();
    bslmt::ThreadUtil::sleep(sleepTime);
    const double elapsedTime = timer.elapsedTime();

    return elapsedTime > minTime - k_TOLERANCE;
}

//=============================================================================

#ifdef BSLS_PLATFORM_OS_WINDOWS

// Platform-specific implementation starts here.

// Forward declare the Windows functions used by test implementation.

struct _RTL_CRITICAL_SECTION;
typedef struct _RTL_CRITICAL_SECTION CRITICAL_SECTION, *LPCRITICAL_SECTION;
typedef int BOOL;
typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) BOOL __stdcall InitializeCriticalSectionAndSpinCount(
                                    LPCRITICAL_SECTION lpCriticalSection,
                                    DWORD dwSpinCount);

    __declspec(dllimport) void __stdcall DeleteCriticalSection(
                                LPCRITICAL_SECTION lpCriticalSection);

    __declspec(dllimport) void __stdcall EnterCriticalSection(
                                LPCRITICAL_SECTION lpCriticalSection);

    __declspec(dllimport) void __stdcall LeaveCriticalSection(
                                LPCRITICAL_SECTION lpCriticalSection);

}  // extern "C"

namespace {
namespace u {

                            // =====
                            // Mutex
                            // =====

class Mutex {
    // It provides an efficient proxy for Windows critical sections, and
    // related operations.  Note that the Mutex implemented in this class is
    // *not* error checking, and is non-recursive.

  public:
    enum {
        // Size of the buffer allocated for the critical section, in
        // pointer-sized elements.  We have to make it public so we could
        // access it in a .cpp file to verify the size.

#ifdef BSLS_PLATFORM_CPU_64_BIT
        // 5*8 = 40 bytes
        k_CRITICAL_SECTION_BUFFER_SIZE = 5
#else
        // 6*4 = 24 bytes
        k_CRITICAL_SECTION_BUFFER_SIZE = 6
#endif
    };

  private:
    enum {
        // A Windows critical section has a configurable spin count.  A lock
        // operation spins this many iterations (on, presumably, some atomic
        // integer) before sleeping on the underlying primitive.

        k_SPIN_COUNT = 30
    };

    // DATA
    void *d_lock[k_CRITICAL_SECTION_BUFFER_SIZE];

  private:
    // NOT IMPLEMENTED
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

  public:
    // CREATORS
    Mutex()
        // Create a mutex initialized to an unlocked state.
    {
        InitializeCriticalSectionAndSpinCount(
          reinterpret_cast<_RTL_CRITICAL_SECTION *>(d_lock), k_SPIN_COUNT);
    }

    ~Mutex()
        // Destroy this mutex object.
    {
        DeleteCriticalSection(
                             reinterpret_cast<_RTL_CRITICAL_SECTION*>(d_lock));
    }

    // MANIPULATORS
    void lock()
        // Acquire a lock on this mutex object.  If this object is currently
        // locked, then suspend execution of the current thread until a lock
        // can be acquired.  Note that the behavior is undefined if the calling
        // thread already owns the lock on this mutex, and will likely result
        // in a deadlock.
    {
        EnterCriticalSection(reinterpret_cast<_RTL_CRITICAL_SECTION*>(d_lock));
    }

    void unlock()
        // Release a lock on this mutex that was previously acquired through a
        // successful call to 'lock', or 'tryLock'.  The behavior is undefined,
        // unless the calling thread currently owns the lock on this mutex.
    {
        LeaveCriticalSection(reinterpret_cast<_RTL_CRITICAL_SECTION*>(d_lock));
    }
};

#else
namespace {
namespace u {

                            // =====
                            // Mutex
                            // =====

class Mutex {
    // This class provides a full specialization of 'Mutex' for pthreads.  It
    // provides a efficient proxy for the 'pthread_mutex_t' pthreads type, and
    // related operations.  Note that the mutex implemented in this class is
    // *not* error checking, and is non-recursive.

    // DATA
    pthread_mutex_t d_lock;

    // NOT IMPLEMENTED
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

  public:
    // CREATORS
    Mutex() { pthread_mutex_init(&d_lock, 0); }

    ~Mutex() { pthread_mutex_destroy(&d_lock); }

    // MANIPULATORS
    void lock() { pthread_mutex_lock(&d_lock); }

    void unlock() { pthread_mutex_unlock(&d_lock); }
};

#endif

}  // close namespace u
}  // close unnamed namespace

//-----------------------------------------------------------------------------
//                               All Create Test
//-----------------------------------------------------------------------------

namespace BSLMT_THREADUTIL_ALL_CREATE_TEST {

enum CreateMode {
    e_CREATE_MODE_START,

    e_NO_ALLOC_FUNCTOR_NO_ATTR = e_CREATE_MODE_START,
    e_NO_ALLOC_FUNCTOR_ATTR,
    e_NO_ALLOC_NO_ATTR,
    e_NO_ALLOC_ATTR,

    e_ALLOC_FUNCTOR_NO_ATTR,
    e_ALLOC_FUNCTOR_ATTR,
    e_ALLOC_NO_ATTR,
    e_ALLOC_ATTR,

    e_NO_ALLOC_FUNCTOR_ATTR_NAME,
    e_NO_ALLOC_ATTR_NAME,

    e_ALLOC_FUNCTOR_ATTR_NAME,
    e_ALLOC_ATTR_NAME,

    e_NUM_CREATE_MODES
};

const char *toAscii(CreateMode cm)
{
#undef  CASE
#define CASE(x)    case x:    return #x

    switch (cm) {
      CASE(e_NO_ALLOC_FUNCTOR_NO_ATTR);
      CASE(e_NO_ALLOC_FUNCTOR_ATTR);
      CASE(e_NO_ALLOC_NO_ATTR);
      CASE(e_NO_ALLOC_ATTR);
      CASE(e_ALLOC_FUNCTOR_NO_ATTR);
      CASE(e_ALLOC_FUNCTOR_ATTR);
      CASE(e_ALLOC_NO_ATTR);
      CASE(e_ALLOC_ATTR);
      CASE(e_NO_ALLOC_FUNCTOR_ATTR_NAME);
      CASE(e_NO_ALLOC_ATTR_NAME);
      CASE(e_ALLOC_FUNCTOR_ATTR_NAME);
      CASE(e_ALLOC_ATTR_NAME);
      CASE(e_NUM_CREATE_MODES);
      default: {
        LOOP_ASSERT(static_cast<int>(cm), 0);
        return "";                                                    // RETURN
      }
   }

#undef CASE
}

bsl::ostream& operator<<(bsl::ostream& stream, CreateMode cm)
{
    stream << toAscii(cm);

    return stream;
}

class AllCreateTestFunctor {
    // This 'class' is used in the allocator test.

    enum { k_BUFFER_SIZE = 20 << 10 };

    // DATA
    char                    d_buffer[k_BUFFER_SIZE];
    bool                    d_created;
    u::Mutex        * const d_mutex;
    bsls::AtomicInt * const d_numToInc;
    bsl::string     * const d_name;

  public:
    // CREATORS
    AllCreateTestFunctor(u::Mutex        *mutex,
                         bsls::AtomicInt *numToInc,
                         bsl::string     *name)
    : d_created(true)
    , d_mutex(mutex)
    , d_numToInc(numToInc)
    , d_name(name)
    {
        (void) d_buffer;    // suppress 'unused' warnings
    }

    // MANIPULATORS
    void operator()()
    {
        ++*d_numToInc;
        d_mutex->lock();
        BSLS_ASSERT(d_created);
        d_mutex->unlock();
        ++*d_numToInc;

        Obj::getThreadName(d_name);
    }

    ~AllCreateTestFunctor()
    {
        d_created = false;
    }
};

extern "C"
void *allCreateTestFunction(void *arg)
{
    AllCreateTestFunctor *p = static_cast<AllCreateTestFunctor *>(arg);
    (*p)();

    return p;
}

}  // close namespace BSLMT_THREADUTIL_ALL_CREATE_TEST

//-----------------------------------------------------------------------------
//                             Unnamed Namespace
//-----------------------------------------------------------------------------

namespace {

    // unnamed namespace for local resources

                             // ================
                             // class BigFunctor
                             // ================

class BigFunctor {
    // This 'class' is used in the allocator test.

    enum { k_BUFFER_SIZE = 20 << 10 };

    // DATA
    char buffer[k_BUFFER_SIZE];

  public:
    void operator()()
    {
        for (char *pc = buffer; pc < buffer + k_BUFFER_SIZE; ++pc) {
            *pc = 0x3f;
        }
    }
};

                           // ===================
                           // class ThreadChecker
                           // ===================

class ThreadChecker {

    bsls::AtomicInt d_count;

  public:
    // TYPES
    class ThreadCheckerFunctor {
        ThreadChecker *d_checker_p;

      public:
        explicit
        ThreadCheckerFunctor(ThreadChecker *checker)
        {
            d_checker_p = checker;
        }

        void operator()()
        {
            d_checker_p->eval();
        }
    };

    // CREATORS
    ThreadChecker() : d_count(0) {}

    // MANIPULATORS
    void eval()
    {
       ++d_count;
    }

    ThreadCheckerFunctor getFunctor()
    {
        return ThreadCheckerFunctor(this);
    }

    // ACCESSORS
    int count() const
    {
       return d_count;
    }
};

extern "C" void *myThreadFunction(void *)
    // Print to standard output "Another second has passed" every second for
    // five seconds.
{
    for (int i = 0; i < 3; ++i) {
        bslmt::ThreadUtil::microSleep(0, 1);
        if (verbose) bsl::cout << "Another second has passed\n";
    }

    return 0;
}

extern "C" void *mySmallStackThreadFunction(void *threadArg)
    // Initialize a small object on the stack and do some work.
{
    char *initValue = (char *)threadArg;
    char Small[8];
    memset(&Small[0], *initValue, 8);
    // do some work ...
    return 0;
}

void createSmallStackSizeThread()
    // Create a detached thread with the small stack size and perform some work
{
    enum { k_STACK_SIZE = 16384 };
    bslmt::ThreadAttributes attributes;
    attributes.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);
    attributes.setStackSize(k_STACK_SIZE);

    char initValue = 1;
    bslmt::ThreadUtil::Handle handle;
    bslmt::ThreadUtil::create(&handle,
                             attributes,
                             mySmallStackThreadFunction,
                             &initValue);
}

const char *policyToString(Attr::SchedulingPolicy policy)
{
    if      (Attr::e_SCHED_OTHER   == policy) {
        return "other";                                               // RETURN
    }
    else if (Attr::e_SCHED_FIFO    == policy) {
        return "fifo";                                                // RETURN
    }
    else if (Attr::e_SCHED_RR      == policy) {
        return "rr";                                                  // RETURN
    }
    else if (Attr::e_SCHED_DEFAULT == policy) {
        return "default";                                             // RETURN
    }
    else {
        ASSERT(0);
        return "unrecognized policy";                                 // RETURN
    }
}

void printNumberOfLogicalProcessorsFromCommandLine()
{
    if (verbose) cout << "Data received from the command line:"
                      << endl;

    int ret = 0;

#ifdef BSLS_PLATFORM_OS_LINUX
    ret = execl("/bin/grep", "grep", "-c", "^processor", "/proc/cpuinfo", 0);
    if (-1 == ret) {
        if (verbose) cout << "Failed to launch process for grep" << endl;
    }

#elif defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_SOLARIS)

    int pipefd[2];

    // get a pipe (buffer and fd pair) from the OS

    if (pipe(pipefd)) {
        if (verbose) cout << "Failed to open pipe" << endl;
    } else {
        // We are the child process, but since we have TWO commands to exec we
        // need to have two disposable processes, so fork again.

        switch (fork()) {
          case -1: {
            // fork failure

            if (verbose) cout << "Failed to fork third process." << endl;
          } break;
          case 0: {
            // child process
            // Do redirections and close the wrong side of the pipe.

            close(pipefd[0]);    // the other side of the pipe
            dup2(pipefd[1], 1);  // automatically closes previous fd 1
            close(pipefd[1]);    // cleanup

    #if defined(BSLS_PLATFORM_OS_AIX)
            ret = execl("/bin/lparstat", "lparstat", (char *)NULL);
    #elif defined(BSLS_PLATFORM_OS_SOLARIS)
            ret = execl("/bin/psrinfo", "psrinfo", (char *)NULL);
    #endif

            if (-1 == ret) {
                if (verbose) {
                    cout << "Failed to launch process for the first command"
                         << endl;
                }
            }
          } break;
          default: {
            // parent process

            close(pipefd[1]);    // the other side of the pipe
            dup2(pipefd[0], 0);  // automatically closes previous fd 0
            close(pipefd[0]);    // cleanup

    #if defined(BSLS_PLATFORM_OS_AIX)
            ret = execl("/bin/grep", "grep", "lcpu", (char *)NULL);
    #elif defined(BSLS_PLATFORM_OS_SOLARIS)
            ret = execl("/bin/wc", "wc", "-l", (char *)NULL);
    #endif

            if (-1 == ret) {
                if (verbose) {
                    cout << "Failed to launch process for the second command"
                         << endl;
                }
            }
          }
        }
    }
#else
    if (verbose) cout << "Test is not supported on this platform. "
                      << "Only Linux, AIX and Solaris are supported."
                          << endl;
#endif
}

namespace u {

#ifdef BSLS_PLATFORM_OS_UNIX

// 16-byte length limit (include '\0') on Unix platforms.

const char *nonDefaultThreadName = "Homer Bart Lisa";

#elif  BSLS_PLATFORM_OS_WINDOWS

// Very long length limit on Windows.

const char *nonDefaultThreadName =
            "To be, or not to be, that is the question:\n"
            "Whether 'tis nobler in the mind to suffer\n"
            "The slings and arrows of outrageous fortune,\n"
            "Or to take Arms against a Sea of troubles,\n"
            "And by opposing end them: to die, to sleep\n"
            "No more; and by a sleep, to say we end\n"
            "The heart-ache, and the thousand natural shocks\n"
            "That Flesh is heir to? 'Tis a consummation\n"
            "Devoutly to be wished. To die, to sleep,\n"
            "To sleep, perchance to Dream; aye, there's the rub,\n"
            "For in that sleep of death, what dreams may come,\n"
            "When we have shuffled off this mortal coil,\n"
            "Must give us pause. There's the respect\n"
            "That makes Calamity of so long life:\n"
            "For who would bear the Whips and Scorns of time,\n"
            "The Oppressor's wrong, the proud man's Contumely, [F: poore]\n"
            "The pangs of dispised Love, the Law's delay, [F: dispriz'd]\n"
            "The insolence of Office, and the spurns\n"
            "That patient merit of th'unworthy takes,\n"
            "When he himself might his Quietus make\n"
            "With a bare Bodkin? Who would Fardels bear, [F: these Fardels]\n"
            "To grunt and sweat under a weary life,\n"
            "But that the dread of something after death,\n"
            "The undiscovered country, from whose bourn\n"
            "No traveller returns, puzzles the will,\n"
            "And makes us rather bear those ills we have,\n"
            "Than fly to others that we know not of?\n"
            "Thus conscience does make cowards of us all,\n"
            "And thus the native hue of Resolution\n"
            "Is sicklied o'er, with the pale cast of Thought,\n"
            "And enterprises of great pitch and moment, [F: pith]\n"
            "With this regard their Currents turn awry, [F: away]\n"
            "And lose the name of Action. Soft you now,\n"
            "The fair Ophelia? Nymph, in thy Orisons\n"
            "Be all my sins remember'd.\n";
#else
# error Unrecognized platform
#endif

}  // close namespace u
}  // close unnamed namespace

//-----------------------------------------------------------------------------
//                       Named Detached Threads Test Case
//-----------------------------------------------------------------------------

namespace NAMED_DETACHED_THREAD_TEST_CASE {

enum {  k_NUM_THREADS = 10,

#if   defined(BSLS_PLATFORM_OS_WINDOWS)
        // There is one allocation for the thread name in 'attr' variable in
        // this function, then for each thread there is:
        //: o One allocation for the 'u::NamedFuncPtrRecord' in the imp file.
        //:
        //: o One allocation for the string object in the
        //:   'u::NamedFuncPtrRecord' in the imp file.
        //:
        //: o One allocation for the string in 'TC::subThread' in this file.

        k_EXP_TA_ALLOCS = 1 + 3 * k_NUM_THREADS,

        // We expect each thread to do one allocation in the default allocator
        // in bslmt_threadutilimpl_windows.cpp when translating UTF-8 to
        // UTF-16.

        k_EXP_DEFAULT_ALLOCS = k_NUM_THREADS,

#elif   defined(BSLS_PLATFORM_OS_UNIX)
        // The max thread name is short enough to fit in a short string, but we
        // still have to allocate 'u::NamedFuncPtrRecord' for each thread.

        k_EXP_TA_ALLOCS = k_NUM_THREADS,

        // No unicode translation on Unix.

        k_EXP_DEFAULT_ALLOCS = 0,
#else
# error unrecognized platform
#endif
};

bsls::AtomicInt started(0);
bsls::AtomicInt stopped(-1);

extern "C" void *subThread(void *allocArg)
{
    bslma::Allocator *alloc = static_cast<bslma::Allocator *>(allocArg);

    ++started;

    bsl::string threadName(alloc);
    Obj::getThreadName(&threadName);
    ASSERT((k_threadNameCanBeEmpty && threadName.empty()) ||
           u::nonDefaultThreadName == threadName);

    while (stopped < 0) {
        Obj::yield();
    }

    ++stopped;

    return 0;
}

}  // close namespace NAMED_DETACHED_THREAD_TEST_CASE

//-----------------------------------------------------------------------------
//                    Multipriority Effectiveness Test Case
//
// Thread priorities are only usable on some platforms (see component doc), and
// even on those, priorities only seem to make any difference when multiple
// threads are waiting on a mutex and the os is deciding which of them is
// chosen to run first.  So priorities are not really testable until you have a
// mutex type.  Since it was felt undesirable to do the test in the mutex
// component, we implement our own mutex class here.
//-----------------------------------------------------------------------------

namespace MULTIPRIORITY_EFFECTIVENESS_TEST_CASE {

enum {  k_NUM_THREADS = 128  };  // must be an even number

bsls::AtomicInt s_priorityEffectivenessTest_start;
bsls::AtomicInt s_priorityEffectivenessTest_numFinished;
u::Mutex        s_priorityEffectivenessTest_mutex;

extern "C" void *priorityEffectivenessTestFunc(void *arg)
{
    enum {  k_ITER      =   128  };
    enum {  k_WORK_SIZE = 25000  };

    while (0 == s_priorityEffectivenessTest_start) {
        bslmt::ThreadUtil::yield();
    }

#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_DARWIN)

    // On some operating systems (e.g., Solaris) the thread priority has little
    // effect except on the exit order from a synchronization primitive.

    for (int i = 0; i < k_ITER; ++i) {
        s_priorityEffectivenessTest_mutex.lock();
        bslmt::ThreadUtil::yield();
        s_priorityEffectivenessTest_mutex.unlock();
    }

    *static_cast<bsls::AtomicInt *>(arg) +=
                                     ++s_priorityEffectivenessTest_numFinished;

#else

    // The expectation for thread priority is to be scheduled with higher
    // priority or with longer duration.

    int j = 0;

    for (int i = 0; i < k_ITER; ++i) {
        for (int k = -k_WORK_SIZE; k <= k_WORK_SIZE; ++k) {
            if (k * k < k * k * k) ++j;
        }
        bslmt::ThreadUtil::yield();
    }

    if (j > 0) {
        // This should always be true; used to help defeat optimizers.

        *static_cast<bsls::AtomicInt *>(arg) +=
                                     ++s_priorityEffectivenessTest_numFinished;
    }

#endif

    return 0;
}

void priorityEffectivenessTest()
    // Run the priority effectiveness test.
{
    typedef Attr::SchedulingPolicy Policy;

    static const struct {
        int    d_line;
        Policy d_policy;
    } DATA[] = {
        { L_, Attr::e_SCHED_DEFAULT },
        { L_, Attr::e_SCHED_OTHER   },
    };
    const int DATA_LEN = static_cast<int>(sizeof(DATA) / sizeof(*DATA));

    for (int i = 0; i < DATA_LEN; ++i) {
        const int    LINE   = DATA[i].d_line;
        const Policy POLICY = DATA[i].d_policy;

        const int MAX_PRIORITY = Obj::getMaxSchedulingPriority(POLICY);
        const int MIN_PRIORITY = Obj::getMinSchedulingPriority(POLICY);

        ASSERT(MAX_PRIORITY >= MIN_PRIORITY);

        if (veryVerbose) {
            P_(POLICY); P_(MAX_PRIORITY); P(MIN_PRIORITY);
        }

        // If there's no difference between max and min priority, there's no
        // point in doing the test.

        if (MAX_PRIORITY == MIN_PRIORITY) {
            continue;
        }

        // Create two 'Attr' objects, an 'urgent' one with max priority, and a
        // 'notUrgent' one with min priority.

        Attr urgentAttr;

        urgentAttr.setStackSize(64 * 1024);
        urgentAttr.setInheritSchedule(false);
        urgentAttr.setSchedulingPolicy(POLICY);
        urgentAttr.setSchedulingPriority(MAX_PRIORITY);

        Attr notUrgentAttr(urgentAttr);

        notUrgentAttr.setSchedulingPriority(MIN_PRIORITY);

        ASSERT(0 == k_NUM_THREADS % 2);

        Obj::Handle handles[k_NUM_THREADS];

        s_priorityEffectivenessTest_start       = 0;
        s_priorityEffectivenessTest_numFinished = 0;

        bsls::AtomicInt urgentSum(0);
        bsls::AtomicInt notUrgentSum(0);

        for (int j = 0; j < k_NUM_THREADS; ++j) {
            // Since barriers are not available at this level in the dependency
            // hierarchy, going to bias the test towards priority *failing* to
            // prove worthwhile by starting a not-urgent thread before a
            // corresponding urgent thread.

            int rc;
            errno = 0;

            if (j % 2) {
                rc = Obj::create(&handles[j],
                                 urgentAttr,
                                 priorityEffectivenessTestFunc,
                                 &urgentSum);
            }
            else {
                rc = Obj::create(&handles[j],
                                 notUrgentAttr,
                                 priorityEffectivenessTestFunc,
                                 &notUrgentSum);
            }

            LOOP4_ASSERT(LINE, rc, j, errno, 0 == rc);

            if (rc) {
                break;
            }
        }

        s_priorityEffectivenessTest_start = 1;

        for (int j = 0; j < k_NUM_THREADS; ++j) {
            int rc = Obj::join(handles[j]);

            LOOP3_ASSERT(LINE, rc, j, 0 == rc);

            if (rc) {
                break;
            }
        }

        if (verbose) {
            cout << "Policy " << policyToString(POLICY)
                 << ", Max pri " << setw(3) << MAX_PRIORITY
                 << ", Min pri " << setw(3) << MIN_PRIORITY
                 << ", urgentSum " << urgentSum
                 << ", notUrgentSum " << notUrgentSum
                 << endl;
        }

        ASSERTV(urgentSum, notUrgentSum, urgentSum + 10 <= notUrgentSum);
    }
}

}  // close namespace MULTIPRIORITY_EFFECTIVENESS_TEST_CASE

// ----------------------------------------------------------------------------
//                      Thread Policy Creation Test Case
// ----------------------------------------------------------------------------

namespace BSLMT_THREAD_POLICY_CREATION_TEST {

struct Touch {
    // functor

    // DATA
    bool *d_finished;

    void operator()()
        // When called, touches '*d_finished' to prove it ran, and returns.
    {
        ASSERT(! *d_finished);
        *d_finished = true;
    }
};

}  // close namespace BSLMT_THREAD_POLICY_CREATION_TEST

// ----------------------------------------------------------------------------
//                       Multipriority Usage Test Case
// ----------------------------------------------------------------------------

namespace MULTIPRIORITY_USAGE_TEST_CASE {

// Note that in practice, thread priorities only seem to make a difference when
// multiple stopped threads are simultaneously ready to run, so we won't see
// any effect of the different priorities in this case.

extern "C"
void *MostUrgentThreadFunctor(void*)
{
    if (verbose) {
        bsl::printf("Most urgent\n");
    }
    return 0;
}

extern "C"
void *FairlyUrgentThreadFunctor(void*)
{
    if (verbose) {
        bsl::printf("Fairly urgent\n");
    }
    return 0;
}

extern "C"
void *LeastUrgentThreadFunctor(void*)
{
    if (verbose) {
        bsl::printf("Least urgent\n");
    }
    return 0;
}

}  // close namespace MULTIPRIORITY_USAGE_TEST_CASE

// ----------------------------------------------------------------------------
//                          CONFIGURATION TEST CASE
// ----------------------------------------------------------------------------

namespace BSLMT_CONFIGURATION_TEST_NAMESPACE {

struct Func {
    int         d_stackToUse;
    char       *d_lastBuf;
    static bool s_success;

    void recurser(char *base);
        // Recurse to create depth on stack

    void operator()();
        // Initialize, then call 'recurser', then set 'd_success'
};
bool Func::s_success;

void Func::recurser(char *base)
    // Consume greater than 'd_stackToUse' of stack depth
{
    char buf[5 * 1000];

    ASSERT(buf != d_lastBuf);    // make sure optimizer didn't remove recursion
    d_lastBuf = buf;

    bsl::memset(buf, 'a', sizeof(buf));

    if   (intPtrAbs(buf - base) < d_stackToUse
       && intPtrAbs(buf + sizeof(buf) - base) < d_stackToUse) {
        recurser(base);
    }

    for (char *pc = buf; pc < buf + sizeof(buf); ++pc) {
        ASSERT(*pc == 'a');
    }
}

void Func::operator()()
{
    if (verbose) P(d_stackToUse);

    d_lastBuf = 0;
    char base;
    recurser(&base);

    s_success = true;
}

}  // close namespace BSLMT_CONFIGURATION_TEST_NAMESPACE

extern "C"
void *configurationTestFunction(void *stackToUse)
{
    BSLMT_CONFIGURATION_TEST_NAMESPACE::Func func;

    func.d_stackToUse = (int) (IntPtr) stackToUse;
    func.s_success   = false;

    func();

    ASSERT(func.d_stackToUse == (int) (IntPtr) stackToUse);
    ASSERT(func.s_success);

    return 0;
}

// ----------------------------------------------------------------------------
//                            STACKSIZE TEST CASE
// ----------------------------------------------------------------------------

namespace STACKSIZE_TEST_CASE_NAMESPACE {

template <int BUFFER_SIZE>
struct Func {
    void operator()()
    {
        size_t  bufferSize = BUFFER_SIZE == 0 ? 1 : BUFFER_SIZE;
        char   *buffer     = new char[bufferSize];

        bsl::memset(buffer, 'a', bufferSize);

        delete [] buffer;
    }

    static
    void staticFunc()
    {
        Func func;
        func();
    }
};

typedef void (*CppFuncPtr)();

extern "C"
void *callCppFunction(void *function)
{
    CppFuncPtr funcPtr = (CppFuncPtr) function;

    (*funcPtr)();

    return 0;
}

template <int BUFFER_SIZE>
void testStackSize()
{
#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
    // In test cases -2 and -4, Linux was crashing about 4K away from the stack
    // in 32 & 64 bit.  All other unix platforms were running past the end of
    // the stack without crashing.

    enum { k_FUDGE_FACTOR = 8192 };
#else
    // In test case -4, the crash on 32 bit was further than 12K away from the
    // end of the stack stack, on 64 bit it was further than 16k away.

    enum { k_FUDGE_FACTOR = 8192 + 2048 * sizeof(void *) };
#endif

    bslmt::ThreadAttributes attr;
    attr.setStackSize(BUFFER_SIZE + k_FUDGE_FACTOR);
    attr.setGuardSize(MIN_GUARD_SIZE);

    Obj::Handle handle;

    int cRc = Obj::create(&handle, attr, Func<BUFFER_SIZE>());
    LOOP_ASSERT(BUFFER_SIZE, 0 == cRc);
    int jRc = 0;
    if (0 == cRc) {
        jRc = Obj::join(handle);
        ASSERT(0 == jRc);
    }

    if (verbose) {
        cout << "testStackSize<" << BUFFER_SIZE << ">() " <<
                                    ((cRc | jRc) ? "failed\n" : "succeeded\n");
    }

    cRc = Obj::create(&handle,
                      attr,
                      &callCppFunction,
                      (void *) &Func<BUFFER_SIZE>::staticFunc);
    LOOP_ASSERT(BUFFER_SIZE, 0 == cRc);
    jRc = 0;
    if (0 == cRc) {
        jRc = Obj::join(handle);
        ASSERT(0 == jRc);
    }

    if (verbose) {
        cout << "testStackSize<" << BUFFER_SIZE << ">::staticFunc() " <<
                                    ((cRc | jRc) ? "failed\n" : "succeeded\n");
    }
}

}  // close namespace STACKSIZE_TEST_CASE_NAMESPACE

// ----------------------------------------------------------------------------
//                                TEST CASE 6
// ----------------------------------------------------------------------------

extern "C" {

static
void TlsDestructor6_1(void *);
static
void TlsDestructor6_2(void *);

}  // extern "C"

namespace BSLMT_THREADUTIL_TLSKEY_TEST6 {

Obj::Key parentKey1;
Obj::Key parentKey2;
bsls::AtomicInt terminations1;
bsls::AtomicInt terminations2;

struct TlsKeyTestFunctor {
    IntPtr d_seed;

    // CREATOR
    explicit
    TlsKeyTestFunctor(int seed) : d_seed(seed) {}

    // ACCESSORS
    void operator()() const;
};

void TlsKeyTestFunctor::operator()() const
{
    namespace TC = BSLMT_THREADUTIL_TLSKEY_TEST6;

    int rc;

    ASSERT(0 == Obj::getSpecific(TC::parentKey1));
    ASSERT(0 == Obj::getSpecific(TC::parentKey2));

    rc = Obj::setSpecific(TC::parentKey1, (void *)  (5 * d_seed));
    ASSERT(0 == rc);
    rc = Obj::setSpecific(TC::parentKey2, (void *) (12 * d_seed));
    ASSERT(0 == rc);

    ASSERT((void *)  (5 * d_seed) == Obj::getSpecific(TC::parentKey1));
    ASSERT((void *) (12 * d_seed) == Obj::getSpecific(TC::parentKey2));

    rc = Obj::setSpecific(TC::parentKey1, (void *) 0);
    ASSERT(0 == rc);

    return;
}

}  // close namespace BSLMT_THREADUTIL_TLSKEY_TEST6

extern "C" {

static
void TlsDestructor6_1(void *)
{
    namespace TC = BSLMT_THREADUTIL_TLSKEY_TEST6;

    ++ TC::terminations1;
}

static
void TlsDestructor6_2(void *)
{
    namespace TC = BSLMT_THREADUTIL_TLSKEY_TEST6;

    ++ TC::terminations2;
}

}  // extern "C"

// ----------------------------------------------------------------------------
//                                TEST CASE 5
// ----------------------------------------------------------------------------

extern "C" {

static
void createKeyTestDestructor5(void *);

}  // extern "C"

namespace BSLMT_THREADUTIL_CREATEKEY_TEST5 {

Obj::Id childId;
Obj::Key parentKey;
Obj::Key childKey1;
Obj::Key childKey2;
int terminated = 0;

struct CreateKeyTestFunctor {
    bool d_doDestructor;    // do the destructor if and only if this is 'true'

    // CREATORS
    explicit
    CreateKeyTestFunctor(bool doDestructor) : d_doDestructor(doDestructor) {}

    // ACCESSORS
    void operator()() const;
};

void CreateKeyTestFunctor::operator()() const
{
    namespace TC = BSLMT_THREADUTIL_CREATEKEY_TEST5;

    const IntPtr one = 1, zero = 0;

    TC::childId = Obj::selfId();

    int rc = Obj::createKey(&TC::childKey1,
                            d_doDestructor ? &createKeyTestDestructor5 : 0);
    ASSERT(0 == rc);
    ASSERT(TC::parentKey != TC::childKey1);
    ASSERT(0 == Obj::getSpecific(TC::childKey1));

    rc = Obj::setSpecific(TC::childKey1, (void *) 1);
    ASSERT(0 == rc);

    ASSERT((void *) 1 == Obj::getSpecific(TC::childKey1));

    rc = Obj::createKey(&TC::childKey2, &createKeyTestDestructor5);
    ASSERT(0 == rc);
    ASSERT(TC::childKey2 != TC::childKey1);
    ASSERT(TC::childKey2 != TC::parentKey);
    ASSERT(0 == Obj::getSpecific(TC::childKey2));

    rc = Obj::setSpecific(TC::childKey2,
                                       (void *) (d_doDestructor ? one : zero));
    ASSERT(0 == rc);

    ASSERT((void *) (d_doDestructor ? one : zero) ==
                                              Obj::getSpecific(TC::childKey2));

    return;
}

}  // close namespace BSLMT_THREADUTIL_CREATEKEY_TEST5

extern "C" {

static
void createKeyTestDestructor5(void *data)
{
    namespace TC = BSLMT_THREADUTIL_CREATEKEY_TEST5;

    ASSERT(Obj::selfId() == TC::childId);

    if (TC::terminated < 2)  {
        ASSERT((void *) 1 == data);
        int zeroCount = (0 == Obj::getSpecific(TC::childKey1)) +
                        (0 == Obj::getSpecific(TC::childKey2));
#ifdef BSLMT_PLATFORM_POSIX_THREADS
        ASSERT(zeroCount == TC::terminated + 1);
#else
        ASSERT(0 == zeroCount);
#endif
    }
    else {
#ifdef BSLMT_PLATFORM_POSIX_THREADS
        ASSERT((void *) 5 == data);
#else
        ASSERT(0);
#endif
    }

    if (1 == TC::terminated) {
        int rc = Obj::setSpecific(TC::childKey2, (void *) 5);
        ASSERT(0 == rc);
    }

    ++ TC::terminated;
}

}  // extern "C"

inline
long mymax(long a, long b)
{
    return a >= b ? a : b;
}

inline
long myAbs(long a)
{
    return a >= 0 ? a : -a;
}

void testCaseMinus1Recurser(const char *start)
{
    enum { k_BUF_LEN = 1024 };
    char buffer[k_BUF_LEN];
    static double lastDistance = 1;

    double distance = (double) mymax(myAbs(&buffer[0]             - start),
                                     myAbs(&buffer[k_BUF_LEN - 1] - start));
    if (distance / lastDistance > 1.02) {
        cout << static_cast<int>(distance) << endl << flush;
        lastDistance = distance;
    }

    testCaseMinus1Recurser(start);
}

extern "C" void *testCaseMinus1ThreadMain(void *)
{
    char c = 0;
    testCaseMinus1Recurser(&c);

    return 0;
}

// ----------------------------------------------------------------------------
//                                TEST CASE -2
// ----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS

enum { k_CLEARANCE_TEST_START  = 0,
       k_CLEARANCE_TEST_DONE   = 1234,
       k_CLEARANCE_BUFFER_SIZE = 64 * 1024 - 600 };

static int clearanceTestState;
static int clearanceTestAllocaSize;

extern "C" void *clearanceTest(void *)
{
    if (clearanceTestAllocaSize) {
        char *pc = (char *) alloca(clearanceTestAllocaSize);
        ASSERT(pc);

        pc[0] = 0;
        ++pc[0];
        pc[clearanceTestAllocaSize - 1] = 0;
        ++pc[clearanceTestAllocaSize - 1];
    }

    clearanceTestState = k_CLEARANCE_TEST_DONE;

    return 0;
}

#endif

// ----------------------------------------------------------------------------
//                                TEST CASE -3
// ----------------------------------------------------------------------------

bool stackGrowthIsNegative(char *pc)
{
    char c;

    return &c < pc;
}

// ----------------------------------------------------------------------------
//                                TEST CASE -4
// ----------------------------------------------------------------------------

extern "C" void *secondClearanceTest(void *vStackSize)
{
    static int growth;
    char c;
    growth = stackGrowthIsNegative(&c) ? -10 : 10;

    static int stackSize;
    stackSize = (int) (IntPtr) vStackSize;

    static char *pc;
    pc = &c;

    static int diff;

    for (;; pc += growth) {
        *pc = 0;
        diff =  stackSize - (int) myAbs(pc - &c);

        printf("%d\n", diff);
    }

    return 0;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#if defined(BSLS_PLATFORM_OS_SOLARIS)
    // Determine whether we are after Solaris 5.10.

    struct utsname utsName;
    uname(&utsName);

    int front, tail = 0;
    char *dot = bsl::strchr(utsName.release, '.');
    if (dot) {
        *dot = 0;
        tail = bsl::atoi(dot + 1);
    }
    front = bsl::atoi(utsName.release);

    isPost_5_10 = front > 5 || (5 == front && tail > 10);
#endif

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
        // --------------------------------------------------------------------
        // NAMED DETACHED THREADS
        //
        // Concern:
        //: 1 That named detached threads can be created without leaking
        //:   memory.  Note that this is only possible with the
        //:   'Obj::ThreadFunction' interface, it is not possible with the
        //:   'INVOKABLE' interface.
        //
        // Plan:
        //: 1 Create a bunch of named threads using the 'Obj::ThreadFunction'
        //:   interface, that increment atomic 'TC::started' at the beginning
        //:   of the function.
        //:
        //: 2 Once they are all started, verify that allocations have taken
        //:   place, and that the allocator no longer has any allocated memory.
        //:
        //: 3 Signal all threads to stop with the 'stopped' atomic variable.
        //:
        //: 4 Verify that no memory has been leaked and that all allocations
        //:   were performed using the test allocator.
        // --------------------------------------------------------------------

        namespace TC = NAMED_DETACHED_THREAD_TEST_CASE;

        bslma::TestAllocator ta;

        bslma::TestAllocator         taDefaultLocal;
        bslma::DefaultAllocatorGuard guard(&taDefaultLocal);

        {
            Attr attr(&ta);
            attr.setThreadName(u::nonDefaultThreadName);
            attr.setDetachedState(Attr::e_CREATE_DETACHED);

            Obj::Handle handles[TC::k_NUM_THREADS];
            for (int ii = 0; ii < TC::k_NUM_THREADS; ++ii) {
                int rc = Obj::createWithAllocator(&handles[ii],
                                                  attr,
                                                  &TC::subThread,
                                                  &ta,
                                                  &ta);
                ASSERT(0 == rc);
            }
        }

        while (TC::started < TC::k_NUM_THREADS) {
            Obj::yield();
        }

        // 'stopped' is statically initialized to -1, the threads don't start
        // incrementing it until it's non-negative.

        TC::stopped = 0;
        while (TC::stopped < TC::k_NUM_THREADS) {
            Obj::yield();
        }

        // Wait 2 seconds to be sure detacheds threads have finished and
        // released memory.

        bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();
        do {
            Obj::yield();
        } while ((bsls::SystemTime::nowMonotonicClock() - start).
                                                 totalSecondsAsDouble() < 2.0);

        ASSERTV(TC::k_EXP_DEFAULT_ALLOCS, taDefaultLocal.numAllocations(),
                  TC::k_EXP_DEFAULT_ALLOCS == taDefaultLocal.numAllocations());
        ASSERTV(taDefaultLocal.numBytesInUse(),
                0 == taDefaultLocal.numBytesInUse());

        ASSERTV(TC::k_EXP_TA_ALLOCS, ta.numAllocations(),
                                   TC::k_EXP_TA_ALLOCS == ta.numAllocations());
        ASSERTV(ta.numBytesInUse(), 0 == ta.numBytesInUse());
        ASSERTV(ta.numBlocksInUse(), 0 == ta.numBlocksInUse());
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'sleep'
        //   Ensure that the 'sleep' function actually pauses execution.
        //
        // Concerns:
        //: 1 'sleep' sleeps for at least the appropriate amount of time.
        //
        // Plan:
        //: 1 Capture a time_point from the monotonic system clock, then call
        //:   'sleep'.  After the call has returned. compare the current time
        //:   from the clock and ensure that at least that much time has
        //:   elapsed.
        //
        // Testing:
        //   void sleep(const bsls::TimeInterval& sleepTime)
        //   void sleep(const bsl::chrono::duration& sleepTime)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'sleep'\n"
                             "===============\n";

        ASSERT(SleepOnATimeInterval(2,         0));
        ASSERT(SleepOnATimeInterval(0, 700000000));  // 700ms
        ASSERT(SleepOnATimeInterval(0,  10000000));  //  10ms
        ASSERT(SleepOnATimeInterval(1, 699999999));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        using namespace bsl::chrono;

        ASSERT(SleepOnADuration(seconds(2)));
        ASSERT(SleepOnADuration(milliseconds(700)));
        ASSERT(SleepOnADuration(microseconds(10000)));
        ASSERT(SleepOnADuration(duration<double>(1.67)));
#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'hardwareConcurrency'
        //   As there is no universal alternative function to get number of
        //   logical processors (and so available threads), we will recon on
        //   C++11 realization on supporting platforms.  And perform formal
        //   check for AIX and Solaris, which compilers do not support C++11
        //   standard.  Negative manual test '-7' can be used for supplementary
        //   check on these platforms.
        //
        // Concerns:
        //: 1 The 'hardwareConcurrency' returns number of logical processors.
        //
        // Plan:
        //: 1 Compare value returned by the 'hardwareConcurrency' function with
        //:   the result returned by 'std::thread:hardware_concurrency'
        //:   function on platforms supporting C++11, and perform formal check
        //:   on the other platforms.
        //
        // Testing:
        //   unsigned int hardwareConcurrency();
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'hardwareConcurrency'\n"
                             "=============================\n";

        unsigned int threadsNum = Obj::hardwareConcurrency();

        if (veryVerbose) cout << "Number of available threads: "
                              << threadsNum
                              << endl;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        unsigned int modelValue = std::thread::hardware_concurrency();

        LOOP2_ASSERT(modelValue, threadsNum, modelValue == threadsNum);
#else
        LOOP_ASSERT(threadsNum, (1 <= threadsNum) && (2048 >= threadsNum));
#endif

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // ALL THREAD CREATE TEST
        //   Ensure that all the functions that create threads work properly.
        //
        // Concerns:
        //: 1 That all overloads of 'create' and 'createWithAllocator'
        //:   successfully spawn threads, whether any 'attribute' used
        //:   specifies a name or not.
        //:
        //: 2 If a name is specified on the attribute, the thread is named
        //:   correctly (on platforms supporting thread naming).
        //:
        //: 3 That the return 'status' returned by 'join' is as expected.
        //:
        //: 4 That the global allocator is not used if a different allocator
        //:   was passed to 'createWithAllocator' (except on Windows) and
        //:   the default allocator was not used.
        //
        // Plan:
        //: 1 Specify an enum, 'CreateMode', outlining the 1 possibilities
        //:   of calling the 8 overloads of 'create*', with or without a name
        //:   specified in a passed attribute object.
        //:
        //: 2 Iterate through all values of 'CreateMode', doing the appropriate
        //:   'create*' call.
        //:   1 After joining the created thread, observe that the thread
        //:     modified a variable, proving it ran.  (C-1)
        //:
        //:   2 Observe the value of a string the thread assigned to from
        //:     'bslmt::ThreadUtil::threadName()', observing that the thread
        //:     name was as expected.  (C-2)
        //:
        //:   3 Observe that the value returned in the 'status' field of 'join'
        //:     was as expected.  (C-3)
        //:
        //:   4 Observe that the default allocator and global allocator weren't
        //:     used.  (C-4)
        //
        // Testing:
        //   int create(Hdl *, const Inv&);
        //   int create(Hdl *, const Attr&, const Inv&);
        //   int create(Hdl *, Func, void *);
        //   int create(Hdl *, const Attr&, Func, void *);
        //   int createWithAllocator(Hdl *, const Inv&, All *);
        //   int createWithAllocator(Hdl *, const Attr&, const Inv&, All *);
        //   int createWithAllocator(Hdl *, Func, void *, All *);
        //   int createWithAllocator(Hdl *, const Attr&, Func, void *, All *);
        // --------------------------------------------------------------------

        if (verbose) cout << "ALL THREAD CREATE TEST\n"
                             "======================\n";

        using namespace BSLMT_THREADUTIL_ALL_CREATE_TEST;

        typedef bsls::Types::Int64 Int64;

        bslma::TestAllocator oai; // thread name inputs
        bslma::TestAllocator oao; // thread name outputs
        bslma::TestAllocator da;
        bslma::TestAllocator ga;
        bslma::TestAllocator ta;

        bslma::DefaultAllocatorGuard dag(&da);

        bslma::Default::setGlobalAllocator(&ga);

        // Only Linux and Darwin support thread names.  On Linux, the default
        // thread name is the (truncated) task name.  On Darwin, it's the empty
        // string.  On platforms that don't support thread names, the thread
        // name will always have a value of the empty string.

#if defined(BSLS_PLATFORM_OS_LINUX)
        bsl::string defaultThreadName("bslmt_threadutil.t", 15, &oai);
                                                         // basename of process
#else
        const bsl::string defaultThreadName(&oai);    // empty string
#endif

        u::Mutex mutex;
        bsls::AtomicInt numToInc;
        bsl::string threadName(&oao);

        for (int ii = e_CREATE_MODE_START; ii < e_NUM_CREATE_MODES; ++ii) {
            CreateMode cm = static_cast<CreateMode>(ii);

            bslmt::ThreadAttributes attr;
            attr.setStackSize(10 << 10);    // smaller than the functor object
            switch (cm) {
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME:
              case e_NO_ALLOC_ATTR_NAME:
              case e_ALLOC_FUNCTOR_ATTR_NAME:
              case e_ALLOC_ATTR_NAME: {
                attr.setThreadName(u::nonDefaultThreadName);
              } break;
              default: {
                ; // do nothing
              }
            }

            bslmt::ThreadUtil::Handle handle;
            int rc = 0;

            AllCreateTestFunctor functor(&mutex, &numToInc, &threadName);

            // The purpose of this mutex is so that we can block the thread and
            // view it in the debugger to observe thread names.

            mutex.lock();
            numToInc = 0;
            threadName = "";

            bsl::size_t threadNameEmptyCapacity = threadName.capacity();

            const Int64 daNumAllocations  = da.numAllocations();
            const Int64 oaiNumAllocations = oai.numAllocations();
            const Int64 oaoNumAllocations = oao.numAllocations();
            const Int64 gaNumAllocations  = ga.numAllocations();
            const Int64 taNumAllocations  = ta.numAllocations();

            switch (cm) {
              case e_NO_ALLOC_FUNCTOR_NO_ATTR: {
                rc = Obj::create(&handle, functor);
              } break;
              case e_NO_ALLOC_FUNCTOR_ATTR:
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME: {
                rc = Obj::create(&handle, attr, functor);
              } break;
              case e_NO_ALLOC_NO_ATTR: {
                rc = Obj::create(&handle, &allCreateTestFunction, &functor);
              } break;
              case e_NO_ALLOC_ATTR:
              case e_NO_ALLOC_ATTR_NAME: {
                rc = Obj::create(&handle,
                                 attr,
                                 &allCreateTestFunction,
                                 &functor);
              } break;
              case e_ALLOC_FUNCTOR_NO_ATTR: {
                rc = Obj::createWithAllocator(&handle, functor, &ta);
              } break;
              case e_ALLOC_FUNCTOR_ATTR:
              case e_ALLOC_FUNCTOR_ATTR_NAME: {
                rc = Obj::createWithAllocator(&handle, attr, functor, &ta);
              } break;
              case e_ALLOC_NO_ATTR: {
                rc = Obj::createWithAllocator(&handle,
                                              &allCreateTestFunction,
                                              &functor,
                                              &ta);
              } break;
              case e_ALLOC_ATTR:
              case e_ALLOC_ATTR_NAME: {
                rc = Obj::createWithAllocator(&handle,
                                              attr,
                                              &allCreateTestFunction,
                                              &functor,
                                              &ta);
              } break;
              default: {
                ASSERT(0);
              }
            }
            LOOP_ASSERT(cm, 0 == rc);

            while (!rc && !numToInc) {
                Obj::microSleep(10 * 1000);    // wait for thread
            };
            mutex.unlock();

            void *status = 0;
            rc = Obj::join(handle, &status);
            LOOP_ASSERT(cm, 0 == rc);

            if (veryVerbose) {
                P_(cm);    P(threadName);
            }

            switch (cm) {
              case e_NO_ALLOC_FUNCTOR_NO_ATTR:
              case e_NO_ALLOC_FUNCTOR_ATTR:
              case e_ALLOC_FUNCTOR_NO_ATTR:
              case e_ALLOC_FUNCTOR_ATTR:
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME:
              case e_ALLOC_FUNCTOR_ATTR_NAME: {
                LOOP_ASSERT(cm, 0 == status);
              } break;
              case e_NO_ALLOC_NO_ATTR:
              case e_NO_ALLOC_ATTR:
              case e_ALLOC_NO_ATTR:
              case e_ALLOC_ATTR:
              case e_NO_ALLOC_ATTR_NAME:
              case e_ALLOC_ATTR_NAME: {
                LOOP_ASSERT(cm, &functor == status);
              } break;
              default: {
                LOOP_ASSERT(cm, 0);
              }
            }

            LOOP_ASSERT(cm, 2 == numToInc);

            switch (cm) {
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME:
              case e_NO_ALLOC_ATTR_NAME:
              case e_ALLOC_FUNCTOR_ATTR_NAME:
              case e_ALLOC_ATTR_NAME: {
                LOOP2_ASSERT(cm,
                             threadName,
                             (k_threadNameCanBeEmpty && threadName.empty()) ||
                                 u::nonDefaultThreadName == threadName);
              } break;
              default: {
                LOOP2_ASSERT(cm,
                             threadName,
                             (k_threadNameCanBeEmpty && threadName.empty()) ||
                                 defaultThreadName == threadName);
              }
            }

            int expAllocs = 0;

#if defined(BSLS_PLATFORM_OS_WINDOWS)
            // Windows allocates a string using the default allocator when
            // performing the thread name unicode conversion.
            switch (cm) {
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME:
              case e_NO_ALLOC_ATTR_NAME:
              case e_ALLOC_FUNCTOR_ATTR_NAME:
              case e_ALLOC_ATTR_NAME: {
                expAllocs = 1;
              } break;
              default: {
              }
            }
#endif

            LOOP2_ASSERT(cm,
                         da.numAllocations() - daNumAllocations,
                         expAllocs == da.numAllocations() - daNumAllocations);
            LOOP2_ASSERT(cm,
                         oai.numAllocations() - oaiNumAllocations,
                         0 == oai.numAllocations() - oaiNumAllocations);
            switch (cm) {
              case e_ALLOC_FUNCTOR_NO_ATTR:
              case e_ALLOC_FUNCTOR_ATTR:
              case e_ALLOC_FUNCTOR_ATTR_NAME:
              case e_ALLOC_ATTR_NAME: {
#if !defined(BSLS_PLATFORM_OS_WINDOWS) || !defined(BSLS_PLATFORM_CPU_64_BIT)
                // Note that 64 bit windows uses the global allocator for an
                // internal map of threads, which we don't plan to fix.

                LOOP_ASSERT(cm, 0 == ga.numAllocations() - gaNumAllocations);
#endif
                LOOP_ASSERT(cm, 0 != ta.numAllocations() - taNumAllocations);
              } break;
              case e_NO_ALLOC_FUNCTOR_NO_ATTR:
              case e_NO_ALLOC_FUNCTOR_ATTR:
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME:
              case e_NO_ALLOC_ATTR_NAME: {
                LOOP_ASSERT(cm, 0 != ga.numAllocations() - gaNumAllocations);
                LOOP_ASSERT(cm, 0 == ta.numAllocations() - taNumAllocations);
              } break;
              case e_ALLOC_NO_ATTR:
              case e_ALLOC_ATTR:
              case e_NO_ALLOC_NO_ATTR:
              case e_NO_ALLOC_ATTR: {
#if !defined(BSLS_PLATFORM_OS_WINDOWS) || !defined(BSLS_PLATFORM_CPU_64_BIT)
                // Note that 64 bit windows uses the global allocator for an
                // internal map of threads, which we don't plan to fix.

                LOOP_ASSERT(cm, 0 == ga.numAllocations() - gaNumAllocations);
#endif
                LOOP_ASSERT(cm, 0 == ta.numAllocations() - taNumAllocations);
              } break;
              default: {
                ASSERT(0);
              }
            }

            switch (cm) {
              case e_NO_ALLOC_FUNCTOR_ATTR_NAME:{
                int expectedAllocations =
                   (threadName.capacity() > threadNameEmptyCapacity) ? 1 : 0;
                LOOP_ASSERT(cm, expectedAllocations ==
                                oao.numAllocations() - oaoNumAllocations);
              } break;
              case e_ALLOC_FUNCTOR_NO_ATTR:
              case e_ALLOC_FUNCTOR_ATTR:
              case e_ALLOC_ATTR_NAME:
              case e_NO_ALLOC_FUNCTOR_NO_ATTR:
              case e_NO_ALLOC_FUNCTOR_ATTR:
              case e_ALLOC_FUNCTOR_ATTR_NAME:
              case e_NO_ALLOC_ATTR_NAME:
              case e_ALLOC_NO_ATTR:
              case e_ALLOC_ATTR:
              case e_NO_ALLOC_NO_ATTR:
              case e_NO_ALLOC_ATTR: {
                LOOP_ASSERT(cm, 0 == oao.numAllocations() - oaoNumAllocations);
              } break;
              default: {
                ASSERT(0);
              }
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // CREATE ALLOCATION TEST
        //
        // Concerns:
        //   Get the 'create' function to stop using the global allocator on
        //   Unix.  Note this concern is only relevant to the 'create' methods
        //   that take a functor, the ones that take a function pointer don't
        //   do any memory allocation.
        //
        // Plan:
        //   Create a thread with each of the relevant 'create' functions,
        //   passing a functor that is very large that will necessitate memory
        //   allocation.  Observe that the default and global allocators are
        //   not used.
        // --------------------------------------------------------------------

        if (verbose) cout << "CREATE ALLOCATION TEST\n"
                             "======================\n";

        bslma::TestAllocator da;
        bslma::TestAllocator ga;
        bslma::TestAllocator ta;

        bslma::DefaultAllocatorGuard dag(&da);

        bslma::Default::setGlobalAllocator(&ga);

        bslmt::ThreadAttributes attr;
        attr.setStackSize(10 << 10);    // smaller than the functor object

        bslmt::ThreadUtil::Handle handles[2];
        int rc;

        rc = Obj::createWithAllocator(&handles[0],
                                      attr,
                                      BigFunctor(),
                                      &ta);
        ASSERT(0 == rc);

        rc = Obj::createWithAllocator(&handles[1],
                                      BigFunctor(),
                                      &ta);
        ASSERT(0 == rc);

        rc = Obj::join(handles[0]);
        ASSERT(0 == rc);

        rc = Obj::join(handles[1]);
        ASSERT(0 == rc);

        ASSERT(0 == ga.numAllocations());
        ASSERT(0 != ta.numAllocations());

        // 64 bit windows uses the global allocator for an internal map of
        // threads, which we don't plan to fix.

#if !defined(BSLS_PLATFORM_OS_WINDOWS) || !defined(BSLS_PLATFORM_CPU_64_BIT)
        ASSERT(0 == ga.numAllocations());
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PRIORITY EFFECTIVENESS TEST
        //
        // Concerns:
        //: 1 We want to observe thread priorities making a difference.  It has
        //:   been empirically determined that rarely will a more urgent thread
        //:   interrupt a less urgent thread that is running, the only place
        //:   where it seems to make a difference is when the threads are both
        //:   blocked -- then priority seems to make a difference in which one
        //:   is allowed to run first.
        //: 2 This test verifies that higher numbers signify more urgent
        //:   priorities.  The pthreads doc never seems to make that clear.
        //:   This test is to verify that priorities never work 'backwards' --
        //:   that is, whether we expect priorities to work or not, never do
        //:   lower-numbered priorities get treated with significantly more
        //:   urgency than higher numbered priorities.
        //: 3 We also verify that on platforms where the component doc promises
        //:   that priorities will work, they do work, and that a thread
        //:   specified as more urgent will complete a certain task more
        //:   quickly than less urgent threads.
        //
        // Plan:
        //: 1 We create a scenario where a large number of threads are
        //:   contending many times for a single mutex.  One of these threads
        //:   has max priority, all the others have min priority.  We verify
        //:   that the one thread with max priority is successful at getting
        //:   more rapid access to the mutex.  We have an atomic variable
        //:   's_finished' which counts how many threads have finished
        //:   acquiring the mutex a large number of times.  When the urgent
        //:   thread finishes, it sets the value 's_urgentPlace' to the value
        //:   of 's_finished' when it finished, so after all threads have
        //:   finished, 's_urgentPlace' tells us how many threads finished
        //:   before the urgent thread.
        //: 2 We do a number of trials, inserting the value of 's_urgentPlace'
        //:   into a multiset each time.
        //: 3 If priorities are working backwards (C-2), the best value of
        //:   's_urgentPlace' (which will be at the front of the multiset) will
        //:   place among the last threads to complete.  Verify that didn't
        //:   happen.
        //: 4 Examining the first few elements of this multiset tells us how
        //:   the best trials turned out (C-3):
        //:   o We confirm that 's_urgentPlace' was 0 for the best trial, and
        //:     near the front on subsequent trials.  The worst couple of
        //:     trials we ignore.
        //
        // Testing
        //: o Attr::setSchedulingPolicy
        //: o Attr::setSchedulingPriority
        // --------------------------------------------------------------------

        if (verbose) cout << "Thread Priorities Test\n"
                             "======================\n";

#if defined(BSLS_PLATFORM_OS_SOLARIS)
        if (isPost_5_10) {
            // We have been unsuccessful at reproducing any detectable effect
            // of thread priorities on Solaris after 5.10.

            break;
        }
#endif

        namespace TC = MULTIPRIORITY_EFFECTIVENESS_TEST_CASE;

        TC::priorityEffectivenessTest();
      }  break;
      case 13: {
        // --------------------------------------------------------------------
        // THREAD POLICY AND PRIORITY CREATION TEST
        //
        // Concerns:
        //: o Determine which policies and priority values it will be possible
        //:   create a thread at, and for which platform, and verify that our
        //:   component doc about which polices will work on which platform is
        //:   accurate.
        //
        // Plan:
        //: o Iterate through all possible thread policies.
        //: o For each policy, set the bool constant 'willFail' to indicate
        //:   whether the component doc predicts that thread creation will fail
        //:   for the current platform and policy.
        //: o Set a thread attributes object with the given policy.
        //: o Iterate variable 'priority' from the min to the max priority for
        //:   the given platform as specified by
        //:   'Obj::getMinSchedulingPriority' and
        //:   'Obj::getMaxSchedulingPriority'.
        //: o Attempt to spawn a thread running the 'Touch' functor, which if
        //:   it runs will set a boolean variable 'finished'.
        //: o Verify from the return code of 'Obj::create' that thread creation
        //:   either succeeded or failed as predicted by 'willFail'.
        //: o Verify from the value of 'finished' whether the spawned thread
        //:   indeed ran or not.
        //
        // Testing:
        //: o Obj::getMinSchedulingPriority
        //: o Obj::getMaxSchedulingPriority
        //: o Obj::create
        // --------------------------------------------------------------------

        if (verbose) cout << "Thread Policy and Priority Creation Test\n"
                             "========================================\n";

#if defined(BSLS_PLATFORM_CPU_32_BIT) \
    && defined(BSLS_PLATFORM_CMP_GNU) \
    && (BSLS_PLATFORM_CMP_VERSION >= 110000) \
    && (BSLS_PLATFORM_CMP_VERSION < 120000)
        if (veryVerbose) cout << "This test case is skipped for 32-bit builds"
            " on gcc-11, due to issues described in DRQS 168808291 and DRQS"
            " 168914766.  Once the issue is resolved, tests should be"
            " reinstated.\n";
#else


        namespace TC = BSLMT_THREAD_POLICY_CREATION_TEST;

        Attr::SchedulingPolicy policies[] = { Attr::e_SCHED_OTHER,
                                              Attr::e_SCHED_FIFO,
                                              Attr::e_SCHED_RR,
                                              Attr::e_SCHED_DEFAULT };
        const int NUM_POLICIES = static_cast<int>(sizeof policies
                                                / sizeof *policies);

        for (int i = 0; i < NUM_POLICIES; ++i) {
            const Attr::SchedulingPolicy policy = policies[i];

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_AIX)
            const bool willFail = (Attr::e_SCHED_FIFO == policy ||
                                   Attr::e_SCHED_RR   == policy);
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
            const bool willFail = isPost_5_10 &&
                                           (Attr::e_SCHED_FIFO == policy ||
                                            Attr::e_SCHED_RR   == policy);
#elif defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_DARWIN)
            const bool willFail = false;
#else
# error unrecognized platform
#endif
            const int minPri = Obj::getMinSchedulingPriority(policy);
            const int maxPri = Obj::getMaxSchedulingPriority(policy);
            ASSERT(willFail || minPri <= maxPri);

            if (verbose) {
                cout << policyToString(policy) << ": " <<
                        minPri << " - " << maxPri <<
                        (willFail ? " expected to fail\n"
                                  : " expected to succeed\n");
            }

            Attr attr;
            attr.setStackSize(50 * 1024);
            attr.setSchedulingPolicy(policy);
            attr.setInheritSchedule(0);

            bool finished;
            TC::Touch touch;
            touch.d_finished = &finished;

            for (int priority = minPri; priority <= maxPri; ++priority) {
                attr.setSchedulingPriority(priority);

                finished = false;

                errno = 0;
                Obj::Handle handle;
                int rc = Obj::create(&handle, attr, touch);
                if (willFail) {
                    ASSERTV(policyToString(policy), priority,
                                              0 != rc && "unexpected success");
                }
                else {
                    ASSERTV(policyToString(policy), priority, errno,
                                                                  rc, 0 == rc);
                }
                if (0 == rc) {
                    rc = Obj::join(handle);
                    ASSERT(0 == rc);
                    ASSERT(finished);
                }
                else {
                    Obj::yield();
                    ASSERT(!finished);
                }
            }
        }
#endif // gcc-11 bug workaround
      }  break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: sleepUntil
        //
        // Note that this is a system-call wrapper, and this test is intended
        // to ensure the system call is correctly called by the
        // 'bslmt_threadutil'.  This test specifically does *not* test the
        // accuracy of the underlying system call.  Also note that due to the
        // nature of the system call, testing values at the upper bound of the
        // valid range is not reasonable.  Test case -5, has been created and
        // run by hand to verify (slightly) longer time periods.
        //
        // Concerns:
        //: 1 'sleepUntil' suspends the current thread until the indicated
        //:    time in the future (within some reasonable limit).
        //:
        //: 2 'sleepUntil' does not suspend the current thread (or suspends it
        //:    very briefly), for the current time, or times in the past.
        //:
        //: 3 'sleepUntil' works as expected with the monotonic system clock.
        //:
        //: 4  QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Call 'sleepUntil' for a series of values less than a second in
        //:   the future, and verify that system time after sleeping is within
        //:   a reasonable range of the expected target time. (C-1)
        //:
        //: 2 Call 'sleepUntil' for a value in the past, and verify that
        //:   the function returns to the caller in a reasonably small amount
        //:   of time. (C-2)
        //:
        //: 3 Repeat these two tests for 'sleepUntil' with the monotonic
        //:   system clock specified.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time-interval values. (using the
        //:   'BSLS_ASSERTTEST_*' macros) (C-4)
        //
        // Testing:
        //   void sleepUntil(const bsls::TimeInterval& );
        //   void sleepUntil(const bsl::chrono::time_point& );
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "CLASS METHOD 'sleepUntil'" << endl
                 << "=========================" << endl;
        }

        if (veryVerbose) {
            cout << "sleepUntil for times in the future" << endl;
        }

        for (int i = 0; i < 5; ++i) {
            if (veryVeryVerbose) {
                cout << "sleepUntil for " << i * 100 << "ms" << endl;
            }

            bsls::TimeInterval expectedTime;
            bsls::TimeInterval actualTime;
            {
                // realtime clock
                if (veryVeryVerbose) {
                    cout << "sleepUntil realtime" << endl;
                }
                expectedTime = bsls::SystemTime::nowRealtimeClock();
                expectedTime.addMilliseconds(i * 100);
                Obj::sleepUntil(expectedTime);
                actualTime = bsls::SystemTime::nowRealtimeClock();
                ASSERTV(actualTime, expectedTime, actualTime >= expectedTime);
                LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                            (actualTime - expectedTime).totalMilliseconds()
                                                                         < 50);
            }
            {
                // monotonic clock
                if (veryVeryVerbose) {
                    cout << "sleepUntil monotonic" << endl;
                }
                expectedTime = bsls::SystemTime::nowMonotonicClock();
                expectedTime.addMilliseconds(i * 100);
                Obj::sleepUntil(expectedTime,
                                bsls::SystemClockType::e_MONOTONIC);
                actualTime = bsls::SystemTime::nowMonotonicClock();
                ASSERTV(actualTime, expectedTime, actualTime >= expectedTime);
                LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                            (actualTime - expectedTime).totalMilliseconds()
                                                                         < 50);
            }
        }

        if (veryVerbose) {
            cout << "sleepUntil for times in the past" << endl;
        }
        {
            bsls::TimeInterval expectedTime;
            bsls::TimeInterval actualTime;
            { // realtime clock
                expectedTime = bsls::SystemTime::nowRealtimeClock();
                Obj::sleepUntil(expectedTime - bsls::TimeInterval(1));
                actualTime = bsls::SystemTime::nowRealtimeClock();
                ASSERTV(actualTime, expectedTime, actualTime >= expectedTime);
                LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                            (actualTime - expectedTime).totalMilliseconds()
                                                                         < 50);
            }
            { // monotonic clock
                expectedTime = bsls::SystemTime::nowMonotonicClock();
                Obj::sleepUntil(expectedTime - bsls::TimeInterval(1),
                                bsls::SystemClockType::e_MONOTONIC);
                actualTime = bsls::SystemTime::nowMonotonicClock();
                ASSERTV(actualTime, expectedTime, actualTime >= expectedTime);
                LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                            (actualTime - expectedTime).totalMilliseconds()
                                                                         < 50);
            }
        }
        if (verbose) {
            cout << "Negative Testing." << endl;
        }
        {
            bsls::AssertTestHandlerGuard guard;

            // Note that we must use 'RAW' handlers as the assertions are
            // generated by implementation components.

            ASSERT_PASS(Obj::sleepUntil(bsls::TimeInterval(0)));
            ASSERT_FAIL_RAW(Obj::sleepUntil(bsls::TimeInterval(-1, 0)));
            ASSERT_FAIL_RAW(Obj::sleepUntil(bsls::TimeInterval(0, -1)));
            ASSERT_FAIL_RAW(Obj::sleepUntil(bsls::TimeInterval(253402300800LL,
                                                              0)));
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) {
            cout << "Testing chrono clocks." << endl;
        }
        {
            using namespace bsl::chrono;

            ASSERT(SleepOnAClock<system_clock>(2));
            ASSERT(SleepOnAClock<steady_clock>(2));
            ASSERT(SleepOnAClock<AnotherClock>(2));
            ASSERT(SleepOnAClock<HalfClock>(2));
        }
#endif
      }  break;
      case 11: {
        // --------------------------------------------------------------------
        // Usage Example 3: MULTIPLE PRIORITY THREADS
        //
        // Concern:
        //   Need to demonstrate setting priorities for threads.  Note that we
        //   don't demonstrate the different priorities actually having an
        //   effect, we just demonstrate how to set them.
        //
        // Plan:
        //   Spawn 3 threads with different priorities.  Note that this test
        //   works on Windows even though priorities are ignored there, and on
        //   Linux even though the max priority equals the min priority.  This
        //   is because we don't do any testing of which thread runs first, in
        //   fact, since there are only 3 threads and they are all EXTREMELY
        //   short jobs, there probably won't be any contention for processors
        //   at all.  This is just to demonstrate the interface.
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_CYGWIN)
        // Spawning threads fails on Cygwin unless 'inheritSchedule == true'.

        if (verbose) {
            cout << "Skipping case 11 on Cygwin..." << endl;
        }
#else
        using namespace MULTIPRIORITY_USAGE_TEST_CASE;

        enum { k_NUM_THREADS = 3 };

        bslmt::ThreadUtil::Handle handles[k_NUM_THREADS];
        bslmt_ThreadFunction functions[k_NUM_THREADS] = {
                                                  MostUrgentThreadFunctor,
                                                  FairlyUrgentThreadFunctor,
                                                  LeastUrgentThreadFunctor };
        double priorities[k_NUM_THREADS] = { 1.0, 0.5, 0.0 };

        bslmt::ThreadAttributes attributes;
        attributes.setInheritSchedule(false);
        const bslmt::ThreadAttributes::SchedulingPolicy policy =
                                    bslmt::ThreadAttributes::e_SCHED_OTHER;
        attributes.setSchedulingPolicy(policy);

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            attributes.setSchedulingPriority(
                 bslmt::ThreadUtil::convertToSchedulingPriority(policy,
                                                               priorities[i]));
            int rc = bslmt::ThreadUtil::create(&handles[i],
                                               attributes,
                                               functions[i], 0);
            ASSERT(0 == rc);
        }

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc = bslmt::ThreadUtil::join(handles[i]);
            ASSERT(0 == rc);
        }
#endif
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLMT_CONFIGURATION TEST
        //
        // Concern:
        //   That bslmt::Configuration can really affect stack size.
        //
        // Plan:
        //   Configure a stack size several times the native default, then
        //   verify the stack is at least about that size.  If the stack is
        //   only about the native size, we should get a stack overflow.
        // --------------------------------------------------------------------

        const int stackSize =
                      5 * bslmt::Configuration::nativeDefaultThreadStackSize();
        const int stackToUse =
                      4 * bslmt::Configuration::nativeDefaultThreadStackSize();
        bslmt::Configuration::setDefaultThreadStackSize(stackSize);

        bslmt::ThreadUtil::Handle handle;

        if (verbose) Q(Test functor with no attributes);
        {
            BSLMT_CONFIGURATION_TEST_NAMESPACE::Func func;

            func.d_stackToUse = stackToUse;
            func.s_success    = false;

            ASSERT(func.d_stackToUse >
                     3 * bslmt::Configuration::nativeDefaultThreadStackSize());

            int rc = bslmt::ThreadUtil::create(&handle, func);
            ASSERT(0 == rc);

            rc = bslmt::ThreadUtil::join(handle);
            ASSERT(0 == rc);

            ASSERT(func.s_success);
            ASSERT(func.d_stackToUse == stackToUse);
        }

        if (verbose) Q(Test functor with default attributes);
        {
            BSLMT_CONFIGURATION_TEST_NAMESPACE::Func func;

            func.d_stackToUse = stackToUse;
            func.s_success    = false;

            bslmt::ThreadAttributes attr;
            int rc = bslmt::ThreadUtil::create(&handle, attr, func);
            ASSERT(0 == rc);

            rc = bslmt::ThreadUtil::join(handle);
            ASSERT(0 == rc);

            ASSERT(func.s_success);
            ASSERT(func.d_stackToUse == stackToUse);
        }

        if (verbose) Q(Test C function with no attributes);
        {
            int rc = bslmt::ThreadUtil::create(&handle,
                                               &configurationTestFunction,
                                               (void *) (IntPtr) stackToUse);
            ASSERT(0 == rc);

            rc = bslmt::ThreadUtil::join(handle);
            ASSERT(0 == rc);
        }

        if (verbose) Q(Test C function with default attributes object);
        {
            bslmt::ThreadAttributes attr;
            int rc = bslmt::ThreadUtil::create(&handle,
                                               attr,
                                               &configurationTestFunction,
                                               (void *) (IntPtr) stackToUse);
            ASSERT(0 == rc);

            rc = bslmt::ThreadUtil::join(handle);
            ASSERT(0 == rc);
        }
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: convertToSchedulingPriority
        //
        // Concern:
        //   That 'convertToSchedulingPriority' works as expected.
        //
        // Plan:
        //   Call 'get{Min,Max}SchedPriority' and compare the results they
        //   return to results returned by 'convertToSchedulingPriority'.
        // --------------------------------------------------------------------

        typedef bslmt::ThreadAttributes Attr;

        Attr::SchedulingPolicy policies[] = { Attr::e_SCHED_OTHER,
                                              Attr::e_SCHED_FIFO,
                                              Attr::e_SCHED_RR,
                                              Attr::e_SCHED_DEFAULT };
        const int NUM_POLICIES = static_cast<int>(sizeof policies
                                                / sizeof *policies);

        for (int i = 0; i < NUM_POLICIES; ++i) {
            const Attr::SchedulingPolicy POLICY = policies[i];

            const int minPri = Obj::getMinSchedulingPriority(POLICY);
            const int maxPri = Obj::getMaxSchedulingPriority(POLICY);

            if (veryVerbose) {
                P_(policyToString(POLICY)); P_(minPri); P(maxPri);
            }

            const int loPri =  Obj::convertToSchedulingPriority(POLICY, 0.0);
            LOOP2_ASSERT(loPri, minPri, loPri == minPri);

            const int midPri = Obj::convertToSchedulingPriority(POLICY, 0.5);
#if !defined(BSLS_PLATFORM_OS_CYGWIN)
            LOOP2_ASSERT(midPri, minPri, midPri >= minPri);
            LOOP2_ASSERT(midPri, maxPri, midPri <= maxPri);
#else
            LOOP2_ASSERT(midPri, minPri, midPri <= minPri);
            LOOP2_ASSERT(midPri, maxPri, midPri >= maxPri);
#endif

            const int hiPri =  Obj::convertToSchedulingPriority(POLICY, 1.0);
            LOOP2_ASSERT(hiPri, maxPri, hiPri == maxPri);

            if (hiPri != loPri) {
#if !defined(BSLS_PLATFORM_OS_CYGWIN)
                LOOP2_ASSERT(hiPri,  loPri, hiPri >= loPri + 2);

                LOOP2_ASSERT(midPri, hiPri, midPri < hiPri);
                LOOP2_ASSERT(midPri, loPri, midPri > loPri);
#else
                LOOP2_ASSERT(hiPri,  loPri, hiPri <= loPri + 2);

                LOOP2_ASSERT(midPri, hiPri, midPri > hiPri);
                LOOP2_ASSERT(midPri, loPri, midPri < loPri);
#endif
            }
            else {
#if defined(BSLS_PLATFORM_OS_SOLARIS)
                ASSERT(Attr::e_SCHED_FIFO == POLICY ||
                       Attr::e_SCHED_RR   == POLICY);
#elif !defined(BSLS_PLATFORM_OS_LINUX) && !defined(BSLS_PLATFORM_OS_WINDOWS)
                // This should only happen on Linux and Windows
                ASSERT(0);
#endif

                LOOP2_ASSERT(midPri, hiPri, midPri == hiPri);
                LOOP2_ASSERT(midPri, loPri, midPri == loPri);
            }
        }
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // STACK SIZE
        //
        // Concern:
        //   Does setting 'stackSize' allow one to create a buffer of nearly
        //   that size in the thread?
        //
        // Plan:
        //   For various stack sizes, create threads with that stack size and
        //   create a buffer of nearly that size in the thread (minus a
        //   platform-dependent fudge factor), and see if we crash.  Note that
        //   this test is not guaranteed to fail if there is a problem -- it is
        //   possible that if there is a bug and invalid memory access occurs,
        //   it will just abuse the heap but not result in a crash.
        // --------------------------------------------------------------------

        namespace TC = STACKSIZE_TEST_CASE_NAMESPACE;

        enum { k_K = 1024 };

        if (verbose) {
#ifdef PTHREAD_STACK_MIN
            P(PTHREAD_STACK_MIN);
#else
            cout << "'PTHREAD_STACK_MIN' undefined\n";
#endif
        }

        TC::testStackSize<    0      >();
        TC::testStackSize<    1 * k_K>();
        TC::testStackSize<    2 * k_K>();
        TC::testStackSize<    3 * k_K>();
        TC::testStackSize<    4 * k_K>();
        TC::testStackSize<    7 * k_K>();
        TC::testStackSize<    8 * k_K>();
        TC::testStackSize<    9 * k_K>();
        TC::testStackSize<   10 * k_K>();
        TC::testStackSize<   12 * k_K>();
        TC::testStackSize<   14 * k_K>();
        TC::testStackSize<   15 * k_K>();
        TC::testStackSize<   16 * k_K>();
        TC::testStackSize<   17 * k_K>();
        TC::testStackSize<   18 * k_K>();
        TC::testStackSize<   20 * k_K>();
        TC::testStackSize<   24 * k_K>();
        TC::testStackSize<   28 * k_K>();
        TC::testStackSize<   31 * k_K>();
        TC::testStackSize<   32 * k_K>();
        TC::testStackSize<   33 * k_K>();
        TC::testStackSize<   36 * k_K>();
        TC::testStackSize<   40 * k_K>();
        TC::testStackSize<   44 * k_K>();
        TC::testStackSize<   48 * k_K>();
        TC::testStackSize<   52 * k_K>();
        TC::testStackSize<   56 * k_K>();
        TC::testStackSize<   58 * k_K>();
        TC::testStackSize<   60 * k_K>();
        TC::testStackSize<   62 * k_K>();
        TC::testStackSize<   63 * k_K>();
        TC::testStackSize<   64 * k_K>();
        TC::testStackSize<   65 * k_K>();
        TC::testStackSize<   68 * k_K>();
        TC::testStackSize<   72 * k_K>();
        TC::testStackSize<   76 * k_K>();
        TC::testStackSize<   80 * k_K>();
        TC::testStackSize<   84 * k_K>();
        TC::testStackSize<   88 * k_K>();
        TC::testStackSize<   92 * k_K>();
        TC::testStackSize<   96 * k_K>();
        TC::testStackSize<  100 * k_K>();
        TC::testStackSize<  104 * k_K>();
        TC::testStackSize<  108 * k_K>();
        TC::testStackSize<  112 * k_K>();
        TC::testStackSize<  116 * k_K>();
        TC::testStackSize<  120 * k_K>();
        TC::testStackSize<  124 * k_K>();
        TC::testStackSize<  127 * k_K>();
        TC::testStackSize<  128 * k_K>();
        TC::testStackSize<  129 * k_K>();
        TC::testStackSize<  255 * k_K>();
        TC::testStackSize<  256 * k_K>();
        TC::testStackSize<  257 * k_K>();
        TC::testStackSize<  511 * k_K>();
        TC::testStackSize<  512 * k_K>();
        TC::testStackSize<  513 * k_K>();
        TC::testStackSize< 1023 * k_K>();
        TC::testStackSize< 1024 * k_K>();
        TC::testStackSize< 1025 * k_K>();
        TC::testStackSize< 2047 * k_K>();
        TC::testStackSize< 2048 * k_K>();
        TC::testStackSize< 2049 * k_K>();
        TC::testStackSize< 4095 * k_K>();
        TC::testStackSize< 4096 * k_K>();
        TC::testStackSize< 4097 * k_K>();
        TC::testStackSize< 8191 * k_K>();
        TC::testStackSize< 8192 * k_K>();
        TC::testStackSize< 8193 * k_K>();
        TC::testStackSize<16383 * k_K>();
        TC::testStackSize<16384 * k_K>();
        TC::testStackSize<16385 * k_K>();
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // DELETEKEY, THREAD-SPECIFICITY OF DATA TEST ON TLS
        //
        // Concerns:
        //   That 'deleteKey' deletes a TLS key for ALL threads, that data
        //   associated with a key by 'setSpecific' is thread-specific.
        //
        // Plan: Create 2 keys in the parent thread.  Verify both exist in the
        // child thread -- delete 1 in the child thread, verify the data set in
        // one thread is not visible from another thread and does not affect
        // the data set in another thread.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                       "'deleteKey', THREAD SPECIFICITY OF DATA IN TLS TEST\n"
                       "===================================================\n";

        namespace TC = BSLMT_THREADUTIL_TLSKEY_TEST6;

        Obj::createKey(&TC::parentKey1, &TlsDestructor6_1);
        Obj::createKey(&TC::parentKey2, &TlsDestructor6_2);

        int rc;
        rc = Obj::setSpecific(TC::parentKey1, (void *) 1);
        ASSERT(0 == rc);
        rc = Obj::setSpecific(TC::parentKey2, (void *) 2);
        ASSERT(0 == rc);

        bslmt::ThreadUtil::Handle handle;
        bslmt::ThreadUtil::create(&handle, TC::TlsKeyTestFunctor(10));
        bslmt::ThreadUtil::join(handle);

        bslmt::ThreadUtil::create(&handle, TC::TlsKeyTestFunctor(20));
        bslmt::ThreadUtil::join(handle);

        bslmt::ThreadUtil::create(&handle, TC::TlsKeyTestFunctor(30));
        bslmt::ThreadUtil::join(handle);

        rc = Obj::deleteKey(TC::parentKey1);
        ASSERT(0 == rc);
        rc = Obj::deleteKey(TC::parentKey1);
        ASSERT(0 != rc);

        ASSERT(0 == TC::terminations1);
        ASSERT(3 == TC::terminations2);

#if 0
        // parentKey1 is deleted -- this is undefined behavior

        ASSERT((void *) 1 == Obj::getSpecific(TC::parentKey1));
        ASSERT(0 == Obj::setSpecific(TC::parentKey1, (void *) 7));
#endif

        // check 'parentKey2; is unaffected
        ASSERT((void *) 2 == Obj::getSpecific(TC::parentKey2));
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING CREATEKEY, SETSPECIFIC, AND GETSPECIFIC
        //
        // Concerns: That the destructor function passed to 'createKey' is
        //   executed by the thread that is terminating, that 'createKey',
        //   'setSpecific', // and 'getSpecific' all work in the thread as well
        //   as in the destructor function.
        //
        // Plan:
        //   Spawn a thread, and within that thread create 2 keys and associate
        //   non-zero values with them, and both keys associated with the same
        //   destructor.  Verify that the destructor is called twice.  Repeat
        //   the process passing 0 in the 'destructor' fields of the keys, and
        //   verify that the destructor is not called.
        //       The second time the destructor is terminated, associate a
        //   non-zero value with one of the keys and verify this results in its
        //   being called again.
        // --------------------------------------------------------------------

        namespace TC = BSLMT_THREADUTIL_CREATEKEY_TEST5;

        Obj::Key parentKey;
        int rc = Obj::createKey(&parentKey, 0);
        ASSERT(0 == rc);

        void *data = Obj::getSpecific(parentKey);
        ASSERT(0 == data);

        rc = Obj::setSpecific(parentKey, (void *) 2);
        ASSERT(0 == rc);

        ASSERT((void *) 2 == Obj::getSpecific(parentKey));

        bslmt::ThreadUtil::Handle handle;
        rc =
            bslmt::ThreadUtil::create(&handle, TC::CreateKeyTestFunctor(true));
        ASSERT(0 == rc);
        bslmt::ThreadUtil::join(handle);

#if defined(BSLMT_PLATFORM_POSIX_THREADS) && !defined(BSLS_PLATFORM_OS_CYGWIN)
        ASSERT(3 == TC::terminated);
#else
        ASSERT(2 == TC::terminated);
#endif

        ASSERT(parentKey != TC::childKey1);
        ASSERT(Obj::selfId() != TC::childId);

        ASSERT((void *) 2 == Obj::getSpecific(parentKey));
        ASSERT(0 == Obj::getSpecific(TC::childKey1));

        TC::terminated = 0;
        TC::childKey1 = parentKey;
        TC::childId = Obj::selfId();

        bslmt::ThreadUtil::create(&handle, TC::CreateKeyTestFunctor(false));
        bslmt::ThreadUtil::join(handle);

        ASSERT(0 == TC::terminated);
        ASSERT(parentKey != TC::childKey1);
        ASSERT(Obj::selfId() != TC::childId);

        ASSERT((void *) 2 == Obj::getSpecific(parentKey));
        ASSERT(0 == Obj::getSpecific(TC::childKey1));
        ASSERT(0 == Obj::getSpecific(TC::childKey2));
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MICROSLEEP
        //
        // Concerns:
        //   That 'microSleep' always sleeps at least the given amount of
        //   time, and does not sleep an unreasonable amount of extra time.
        //
        // Plan:
        //   Several times, sleep for 0.3 seconds, then check how much time
        //   has passed and verify that it is within acceptable boundaries.
        // --------------------------------------------------------------------

        enum { k_SLEEP_MICROSECONDS = 300 * 1000 };
        const double SLEEP_SECONDS =
                              static_cast<double>(k_SLEEP_MICROSECONDS) * 1e-6;
        const double OVERSHOOT_MIN = -1e-5;
#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_LINUX)
            const double TOLERANCE = 2;   // microSleep is obscenely imprecise
                                          // on Sun and to a lesser extent
                                          // Linux
#else
            const double TOLERANCE = 0.05;
#endif

        for (int i = 0; i < 8; ++i) {
            double start   =
                   bsls::SystemTime::nowRealtimeClock().totalSecondsAsDouble();
            bslmt::ThreadUtil::microSleep(k_SLEEP_MICROSECONDS);
            double elapsed =
                    bsls::SystemTime::nowRealtimeClock().totalSecondsAsDouble()
                                                                       - start;

            double overshoot = elapsed - SLEEP_SECONDS;

            if (veryVerbose) P(overshoot);

            LOOP_ASSERT( overshoot, overshoot >= OVERSHOOT_MIN);
            LOOP2_ASSERT(overshoot, TOLERANCE, overshoot < TOLERANCE);
        }
    }  break;
    case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE Example 2
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSmall stack usage example" << endl;

        createSmallStackSizeThread();    // usage example 2
    }  break;
    case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE Example 1
        //
        // Concern: that the usage examples (including those that were
        // previously in the 'bslmt_thread' component) compile and work
        // properly.
        // --------------------------------------------------------------------

        // BASIC EXAMPLE
        if (verbose) cout << "\nBasic thread utilities example" << endl;

        bslmt::Configuration::setDefaultThreadStackSize(
                    bslmt::Configuration::recommendedDefaultThreadStackSize());

        bslmt::ThreadAttributes attr;
        attr.setStackSize(1024 * 1024);

        bslmt::ThreadUtil::Handle handle;
        int rc = bslmt::ThreadUtil::create(&handle, attr, myThreadFunction, 0);
        ASSERT(0 == rc);
        bslmt::ThreadUtil::yield();
        rc = bslmt::ThreadUtil::join(handle);
        ASSERT(0 == rc);

        if (verbose) bsl::cout << "A three second interval has elapsed\n";
    }  break;
    case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isEqual'
        //
        // Concerns: A valid thread handle is equal to itself.  Two valid
        // thread handles are equal.  An invalid thread handle is unequal to
        // any other thread handle, including itself.
        //
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "\n isEqual test" << endl;
        }

        bslmt::ThreadUtil::Handle validH1, validH2;
        bslmt::ThreadUtil::create(&validH1, myThreadFunction, 0);
        bslmt::ThreadUtil::create(&validH2, myThreadFunction, 0);
        bslmt::ThreadUtil::Handle validH1copy = validH1;

        ASSERT(1 == bslmt::ThreadUtil::isEqual(validH1, validH1));
        ASSERT(1 == bslmt::ThreadUtil::isEqual(validH1, validH1copy));
        ASSERT(0 == bslmt::ThreadUtil::isEqual(validH1, validH2));
        ASSERT(0 == bslmt::ThreadUtil::isEqual(
                                            bslmt::ThreadUtil::invalidHandle(),
                                            validH2));
        ASSERT(0 == bslmt::ThreadUtil::isEqual(
                                            bslmt::ThreadUtil::invalidHandle(),
                                            validH1copy));
        ASSERT(0 == bslmt::ThreadUtil::isEqual(validH1,
                                          bslmt::ThreadUtil::invalidHandle()));
        ASSERT(1 == bslmt::ThreadUtil::isEqual(
                                          bslmt::ThreadUtil::invalidHandle(),
                                          bslmt::ThreadUtil::invalidHandle()));

        bslmt::ThreadUtil::join(validH1);
        bslmt::ThreadUtil::join(validH2);
    }  break;
    case 1: {
        // --------------------------------------------------------------------
        // Invokable functor test
        // --------------------------------------------------------------------

       if (verbose) {
          cout << "\nInvokable functor test" << endl;
       }

       enum { k_THREAD_COUNT = 10 }; // Actually twice this many

       bslmt::ThreadAttributes detached;
       detached.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);

       ThreadChecker joinableChecker;
       ThreadChecker detachedChecker;
       bslmt::ThreadUtil::Handle handles[k_THREAD_COUNT], dummy;
       for (int i = 0; i < k_THREAD_COUNT; ++i) {
          ASSERT(0 == bslmt::ThreadUtil::create(&handles[i],
                                               joinableChecker.getFunctor()));
          ASSERT(0 == bslmt::ThreadUtil::create(&dummy,
                                               detached,
                                               detachedChecker.getFunctor()));
       }

       // Join the joinable threads
       for (int i = 0; i < k_THREAD_COUNT; ++i) {
          ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
       }

       int iterations = 100;
       while ((k_THREAD_COUNT != joinableChecker.count() ||
               k_THREAD_COUNT != detachedChecker.count()) &&
              0 < --iterations)
       {
           bslmt::ThreadUtil::microSleep(100 * 1000);  // 100 msec
           bslmt::ThreadUtil::yield();
       }

       ASSERT(k_THREAD_COUNT == joinableChecker.count());
       ASSERT(k_THREAD_COUNT == detachedChecker.count());
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // STACK OVERFLOW TEST
        //
        // Concern:
        //   How big is the stack, really?  The main concern here is that the
        //   stack size is being properly adjusted so that the specified stack
        //   size really is close to the effective stack size.
        //
        // Plan:
        //   Recurse, printing out how deep we are, until we overflow.
        //
        // Observations:
        //   So: Solaris
        //   AI: AIX
        //   Li: Linux
        //   Wi: Windows
        //
        //   Results accurate to 2% or worse
        //
        //   Note that on AIX, the stack overflow causes the thread to die
        //   without any warning messages, and the thread attempting to join
        //   it just hangs.  On other Unix platforms, the stack overflow causes
        //   a segfault.
        //
        //   Native Limit:
        //   -------------
        //
        //   So 32:  1035871
        //   So 64:  2082903
        //
        //   AI 32:   112128
        //   AI 64:   213760
        //
        //   Li 32: 66691239
        //   Li 64: 66359287
        //
        //   Wi 32:  1022207
        //
        //   export CASE_MINUS_1_STACK_SIZE=1000000:
        //   ---------------------------------------
        //
        //   So 32:   994111
        //   So 64:  1005879
        //
        //   AI 32:  1020832
        //   AI 64:  1118720
        //
        //   Li 32:   976567
        //   Li 64:   991159
        //
        //   Wi 32:  1022207
        //
        //   export CASE_MINUS_1_DEFAULT_SIZE=800000:
        //   ----------------------------------------
        //
        //   So 32:   789119
        //   So 64:   787479
        //
        //   AI 32:   884176
        //   AI 64:   806400
        //
        //   Li 32:   780391
        //   Li 64:   775735
        //
        //   Wi 32:   834239
        // --------------------------------------------------------------------

        bslmt::ThreadAttributes attr;

        const char *stackSizeString = bsl::getenv("CASE_MINUS_1_STACK_SIZE");
        if (stackSizeString) {
            attr.setStackSize(atoi(stackSizeString));
            P(attr.stackSize());
        }

        const char *defaultSizeString =
                                      bsl::getenv("CASE_MINUS_1_DEFAULT_SIZE");
        if (defaultSizeString) {
            BSLS_ASSERT_OPT(!stackSizeString);
            bslmt::Configuration::setDefaultThreadStackSize(
                                                      atoi(defaultSizeString));
            P(bslmt::Configuration::defaultThreadStackSize());
        }

        bslmt::ThreadUtil::Handle handle;
        if (stackSizeString) {
            bslmt::ThreadUtil::create(&handle, attr, &testCaseMinus1ThreadMain,
                                     0);
        }
        else {
            bslmt::ThreadUtil::create(&handle, &testCaseMinus1ThreadMain, 0);
        }

        bslmt::ThreadUtil::join(handle);
      }  break;
#ifndef BSLS_PLATFORM_OS_WINDOWS
      case -2: {
        // --------------------------------------------------------------------
        // CLEARANCE TEST
        //
        // Concern:
        //   What is the minimum amount of stack needed for a thread to
        //   function?
        //
        // Plan:
        //   Specify a stack size at run time and verify that threads can be
        //   spawned with that stack size.  Since this test uses 'alloca.h',
        //   which doesn't exist on Windows, the test is disabled there.
        // --------------------------------------------------------------------

        bslmt::ThreadAttributes attr;
        ASSERT(verbose);
        int clearanceTestStackSize = bsl::atoi(argv[2]);
        P(clearanceTestStackSize);
        attr.setStackSize(clearanceTestStackSize);
        attr.setGuardSize(MIN_GUARD_SIZE);

        clearanceTestAllocaSize = 0;
        clearanceTestState = k_CLEARANCE_TEST_START;
        bslmt::ThreadUtil::Handle handle;
        int rc = bslmt::ThreadUtil::create(&handle, attr, &clearanceTest, 0);
        ASSERT(0 == rc);
        rc = bslmt::ThreadUtil::join(handle);
        ASSERT(0 == rc);
        ASSERT(k_CLEARANCE_TEST_DONE == clearanceTestState);

        Q(Test 0 Completed);

        for (clearanceTestAllocaSize = clearanceTestStackSize / 2;;
                                              clearanceTestAllocaSize += 100) {
            int diff = clearanceTestStackSize - clearanceTestAllocaSize;
            P(diff);

            clearanceTestState = k_CLEARANCE_TEST_START;
            rc = bslmt::ThreadUtil::create(&handle, attr, &clearanceTest, 0);
            ASSERT(0 == rc);
            rc = bslmt::ThreadUtil::join(handle);
            ASSERT(0 == rc);

            ASSERT(k_CLEARANCE_TEST_DONE == clearanceTestState);
        }

        Q(Alloca Test Completed);
      }  break;
#endif
      case -3: {
        // --------------------------------------------------------------------
        // STACK GROWTH DIRECTION TEST
        //
        // Concern:
        //   Determine whether the stack grows in a positive or negative
        //   direction.
        //
        // Plan:
        //   Declare two automatic variables in two different stack frames,
        //   subtract pointers between the two to determine direction of stack
        //   growth.
        // --------------------------------------------------------------------

        char c;

        cout << (stackGrowthIsNegative(&c) ? "negative" : "positive") << endl;
      }  break;
      case -4: {
        // --------------------------------------------------------------------
        // SECOND STACK CLEARANCE TEST
        //
        // Concern:
        //   Determine stack size by, rather than recursing, just accessing
        //   memory further and further up the stack.
        // --------------------------------------------------------------------

        setbuf(stdout, 0);

        bslmt::ThreadAttributes attr;
        ASSERT(verbose);

#ifdef PTHREAD_STACK_MIN
        int stackSize = (IntPtr) PTHREAD_STACK_MIN;
#else
        int stackSize = 1 << 17;
#endif
        if (verbose) {
            stackSize += atoi(argv[2]);
        }
        printf("stackSize = %d\n", stackSize);

        attr.setStackSize(stackSize);
        attr.setGuardSize(MIN_GUARD_SIZE);

        bslmt::ThreadUtil::Handle handle;
        int rc = bslmt::ThreadUtil::create(&handle,
                                           attr,
                                           &secondClearanceTest,
                                           (void *) (IntPtr) stackSize);
        ASSERT(0 == rc);
        rc = bslmt::ThreadUtil::join(handle);
      }  break;
      case -5: {
        // --------------------------------------------------------------------
        // TESTING: sleepUntil (Longer duration)
        //
        // Note that this test case is intended to be run manually, and is an
        // extension to test case 2 that tests for longer durations than should
        // be run in a typical build cycle.
        //
        // Concerns:
        //: 1 'sleepUntil' suspends the current thread until the indicated
        //:    time in the future (within some reasonable limit).  For
        //:    times > 1s in the future.
        //
        // Plan:
        //: 1 Call 'sleepUntil' for a series of values less than a second in
        //:   the future, and verify that system time after sleeping is within
        //:   a reasonable range of the expected target time. (C-1)
        //
        // Testing:
        //   void sleepUntil(const bsls::TimeInterval& );
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "CLASS METHOD 'sleepUntil'" << endl
                 << "=========================" << endl;
        }

        if (veryVerbose) {
            cout << "sleepUntil for times in the future" << endl;
        }

        for (int i = 0; i < 5; ++i) {
            if (veryVeryVerbose) {
                cout << "sleepUntil for " << i * 30 << "s" << endl;
            }

            bsls::TimeInterval expectedTime =
                                          bsls::SystemTime::nowRealtimeClock();

            expectedTime.addSeconds(i * 30);

            Obj::sleepUntil(expectedTime);

            bsls::TimeInterval actualTime =
                                          bsls::SystemTime::nowRealtimeClock();

            ASSERT(actualTime >= expectedTime);
            LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                        (actualTime - expectedTime).totalMilliseconds() < 50);
        }
      } break;
      case -6: {
        // --------------------------------------------------------------------
        // PRIORITY EFFECTIVENESS TEST
        //
        // Concerns:
        //: 1 The priority effectiveness test cannot be run nightly on Solaris
        //:   since it takes prohibitive time on that platform, so we enable
        //:   its running as a negative test case here.
        //
        // Testing
        //: o Attr::setSchedulingPolicy
        //: o Attr::setSchedulingPriority
        // --------------------------------------------------------------------

        if (verbose) cout << "Thread Priorities Test\n"
                             "======================\n";

        namespace TC = MULTIPRIORITY_EFFECTIVENESS_TEST_CASE;

        TC::priorityEffectivenessTest();
      }  break;
      case -7: {
        // --------------------------------------------------------------------
        // 'hardwareConcurrency' MANUAL TEST
        //
        // Concerns:
        //: 1 The 'hardwareConcurrency' test cannot be run nightly on AIX and
        //:   Solaris since it uses C++11 code to verify values, so we give an
        //:   opportunity to verify them manually as a negative test case here.
        //
        // Testing
        //: unsigned int hardwareConcurrency();
        // --------------------------------------------------------------------

        if (verbose) cout << "'hardwareConcurrency' MANUAL TEST\n"
                             "=================================\n";
#ifdef BSLS_PLATFORM_OS_UNIX

        pid_t pid = fork();
        if (0 == pid) {                                  // child process
            printNumberOfLogicalProcessorsFromCommandLine();
        } else if (pid < 0) {                            // process run failure
            if (verbose) cout << "Failed to fork second process." << endl;
        } else {                                         // parent process
            int status;
            waitpid(pid, &status, 0);

            if (verbose) cout << endl
                              << "Value received from the function under test:"
                              << endl;

            if (verbose) cout << "Number of available threads: "
                              << bslmt::ThreadUtil::hardwareConcurrency()
                              << endl << endl;
        }

#else
        if (verbose) cout << "\tTest is not supported on this platform."
                          << endl;
#endif

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
