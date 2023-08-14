// bslstl_stoptoken.t.cpp                                             -*-C++-*-
#include <bslstl_stoptoken.h>

#include <bslma_bufferallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructionutil.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocatorargt.h>
#include <bslmf_util.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>

#include <bslstl_optional.h>

#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <condition_variable>
#include <mutex>
#include <thread>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// 'stop_token', 'stop_source', and 'stop_callback' provide a C++20-compliant
// API for functionality that is implemented by 'bslstl::StopState' and (for
// ownership thereof) 'bsl::shared_ptr'.  All public member functions and
// public free functions in the 'bslstl_stoptoken' component must be tested,
// but complicated configurations involving multiple threads, which are tested
// extensively in 'bslstl_stopstate.t' and 'bslstl_sharedptr.t', will not be
// tested here again.
// ----------------------------------------------------------------------------
// CONSTANTS
// [ 2] inline constexpr nostopstate_t nostopstate;
//
// TYPES
// [21] stop_callback<F>::allocator_type
// [21] stop_callback<F>::callback_type
//
// CREATORS
// [ 7] stop_token::stop_token() noexcept;
// [ 6] stop_token::stop_token(const stop_token&) noexcept;
// [ 8] stop_token::stop_token(stop_token&&) noexcept;
// [ 8] stop_token::stop_token(MovableRef<stop_token>) noexcept;
// [ 2] stop_token::~stop_token();
// [ 2] stop_source::stop_source(nostopstate_t) noexcept;
// [ 2] stop_source::stop_source();
// [13] stop_source::stop_source(const stop_source&) noexcept;
// [14] stop_source::stop_source(stop_source&&) noexcept;
// [14] stop_source::stop_source(MovableRef<stop_source>) noexcept;
// [ 9] stop_source::stop_source(allocator<char>);
// [ 2] stop_source::~stop_source();
// [19] stop_callback(const stop_token&, T&&, allocator_type = {});
// [19] stop_callback(stop_token&&, T&&, allocator_type = {});
// [22] stop_callback(const stop_token&, T&, allocator_type = {});
// [22] stop_callback(stop_token&&, T&, allocator_type = {});
// [20] stop_callback::~stop_callback();
//
// MANIPULATORS
// [10] stop_token& stop_token::operator=(const stop_token&) noexcept;
// [11] stop_token& stop_token::operator=(stop_token&&) noexcept;
// [12] void stop_token::swap(stop_token&) noexcept;
// [16] stop_source& stop_source::operator=(const stop_source&) noexcept;
// [17] stop_source& stop_source::operator=(stop_source&&) noexcept;
// [18] void stop_source::swap(stop_source&) noexcept;
// [ 2] bool stop_source::request_stop() noexcept;
//
// ACCESSORS
// [ 4] bool stop_token::stop_possible() const noexcept;
// [ 3] bool stop_token::stop_requested() const noexcept;
// [ 2] bool stop_source::stop_possible() const noexcept;
// [ 3] bool stop_source::stop_requested() const noexcept;
// [ 2] stop_token stop_source::get_token() const noexcept;
// [21] allocator_type stop_callback::get_allocator() const;
//
// FREE OPERATORS
// [ 5] bool operator==(const stop_token&, const stop_token&) noexcept;
// [ 5] bool operator!=(const stop_token&, const stop_token&) noexcept;
// [12] void swap(stop_token&, stop_token&) noexcept;
// [18] void swap(stop_source&, stop_source&) noexcept;
// [15] bool operator==(const stop_source&, const stop_source&) noexcept;
// [15] bool operator!=(const stop_source&, const stop_source&) noexcept;
//
// CLASS TEMPLATE DEDUCTION GUIDES
// [23] stop_callback(stop_token, T) -> stop_callback(T);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [24] USAGE EXAMPLE
// [22] CONCERN: 'stop_callback' supports const and reference types.
// [ *] CONCERN: In no case does memory come from the global allocator.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
typedef bsl::stop_token       Token;
typedef bslmf::MovableRefUtil MoveUtil;

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;

const int k_BUFFER_SIZE = 1 << 12;  // 4 KiB

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void *assertOnOOM(int bytesRequested)
    // Trigger an assertion, then allocate the specified 'bytesRequested'
    // anyway in order to avoid possible UB.  This function is used as the
    // callback for 'bslma::BufferAllocator' in tests that intentionally do not
    // call the destructor of 'stop_callback'.
{
    printf("Buffer does not have %d bytes left\n", bytesRequested);
    ASSERT(false);
    return malloc(bytesRequested);
}

class GenericCallback {
    // This callback type increments an atomic counter upon invocation and is
    // used to verify that it has been executed the correct number of times (0
    // or 1) depending on the situation.  It is also the base class for various
    // other callbacks that provide additional functionality.

  private:
    bsls::AtomicInt *d_numInvocations_p;

  public:
    GenericCallback(bsls::AtomicInt *numInvocations)
    : d_numInvocations_p(numInvocations)
    {
    }

    void operator()() const
    {
        d_numInvocations_p->add(1);
    }
};
}  // close unnamed namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
namespace usage_example {
///Usage
///-----
// This section illustrates intended use of this component.
//
// 'bsl::stop_token' can be used to implement a condition variable wrapper that
// allows a wait to be interrupted by a stop.  (In C++20, such functionality is
// available as 'std::condition_variable_any'.)  The wrapper must hold a
// 'bsl::stop_token' object that is used to check whether a stop has been
// requested, before entering a wait.  It is also necessary to ensure that the
// thread that requests a stop is able to actually wake up any threads that are
// waiting; for this reason, a 'bsl::stop_callback' must be used to notify the
// waiting threads automatically when a stop is requested.  For simplicity, we
// will only implement one signature for the 'wait' method.

class InterruptibleCV {
  private:
    std::condition_variable d_condvar;

  public:
    void notify_one()
    {
        d_condvar.notify_one();
    }

    void notify_all()
    {
        d_condvar.notify_all();
    }

    template <class t_PREDICATE>
    void wait(std::unique_lock<std::mutex>& lock,
              t_PREDICATE                   pred,
              bsl::stop_token               stopToken)
    {
        auto cb = [this] { notify_all(); };

        bsl::stop_callback<decltype(cb)> stopCb(stopToken, cb);
        while (!stopToken.stop_requested()) {
            if (pred()) {
                return;                                               // RETURN
            }
            d_condvar.wait(lock);
        }
    }
};

// The 'bsl::stop_token' object passed to 'InterruptibleCV::wait' will reflect
// that a stop has been requested only after 'request_stop' is called on a
// 'bsl::stop_source' object from which the 'bsl::stop_token' was derived (or a
// copy of that 'bsl::stop_source').
//
// In the 'UsageExample' class below, the child thread will wait until the
// value of 'd_counter' is at least 50.  However, because the main thread
// requests a stop after setting 'd_counter' to 10, the child thread wakes up.

struct UsageExample {
    std::condition_variable d_startCv;
    InterruptibleCV         d_stopCv;
    std::mutex              d_mutex;
    long long               d_counter;
    bool                    d_ready;

    void threadFunc(bsl::stop_token stopToken)
    {
        std::unique_lock<std::mutex> lg(d_mutex);
        d_ready = true;
        lg.unlock();
        d_startCv.notify_one();

        lg.lock();
        d_stopCv.wait(lg, [this] { return d_counter >= 50; },
                      std::move(stopToken));

        ASSERT(d_counter >= 10 && d_counter < 50);
    }

    UsageExample()
    : d_counter(0)
    , d_ready(false)
    {
        bsl::stop_source stopSource;

        std::thread t(&UsageExample::threadFunc,
                      this,
                      stopSource.get_token());

        std::unique_lock<std::mutex> lg(d_mutex);
        d_startCv.wait(lg, [this] { return d_ready; });
        lg.unlock();

        for (int i = 0; i < 10; i++) {
            lg.lock();
            ++d_counter;
            lg.unlock();
        }

        ASSERT(stopSource.request_stop());

        t.join();
    }
};
}  // close namespace usage_example
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

// ============================================================================
//                                TEST CASE 22
// ----------------------------------------------------------------------------
namespace test_case_22 {
class AssertingCallback : public GenericCallback {
    // This callback asserts upon invocation if it was created by copy or move
    // rather than having been passed a 'bsls::AtomicInt*' directly.

  private:
    AssertingCallback *d_original_p;
        // If 'Callback' is copied or moved, the new object's 'd_self_p'
        // pointer will point to the source of the copy or move.

  public:
    AssertingCallback(bsls::AtomicInt* numInvocations)
    : GenericCallback(numInvocations)
    , d_original_p(this)
    {
    }

    void operator()() const
    {
        ASSERT(this == d_original_p);
        GenericCallback::operator()();
    }
};

class NonConstCallback : public AssertingCallback {
    // This callback can only be called through a non-'const' instance.  It is
    // used to check that 'bsl::stop_callback', even in C++03, forwards
    // non-const lvalues correctly.

  public:
    NonConstCallback(bsls::AtomicInt* numInvocations)
    : AssertingCallback(numInvocations)
    {
    }

    void operator()()
    {
        AssertingCallback::operator()();
    }

    void operator()() const BSLS_KEYWORD_DELETED;
};
}  // close namespace test_case_22

// ============================================================================
//                                TEST CASE 21
// ----------------------------------------------------------------------------
namespace test_case_21 {
struct Callback {
    // This class is a bare-bones allocator-aware callable type that does not
    // have any effects when invoked.  It is used purely to test
    // 'bsl::stop_callback' support for allocator-aware callbacks.

    typedef allocator<char> allocator_type;

    allocator_type d_alloc;

    // CREATORS
    explicit Callback(allocator_type alloc = allocator_type())
    : d_alloc(alloc)
    {
    }

    Callback(const Callback&,
             allocator_type alloc = allocator_type())
    : d_alloc(alloc)
    {
    }

    Callback(bslmf::MovableRef<Callback> other,
             allocator_type              alloc = allocator_type())
    : d_alloc(alloc.mechanism() ? alloc : MoveUtil::access(other).d_alloc)
    {
    }

    // NOT IMPLEMENTED
    Callback& operator=(const Callback&) BSLS_KEYWORD_DELETED;

    // ACCESSORS
    void operator()() const
    {
    }

    allocator_type get_allocator() const
    {
        return d_alloc;
    }

    BSLMF_NESTED_TRAIT_DECLARATION(Callback, bslma::UsesBslmaAllocator);
};
}  // close namespace test_case_21

// ============================================================================
//                                TEST CASE 20
// ----------------------------------------------------------------------------
namespace test_case_20 {
class DestroyingCallback : public GenericCallback {
    // This class is instantiated twice in test case 20.  The first instance to
    // be invoked will destroy the 'stop_callback' that contains itself, as
    // well as the 'stop_callback' that contains the other instance of it, in
    // order to verify that 'stop_callback's can destroy themselves without
    // deadlock and that the invocation of a callback can be prevented (by
    // destroying its 'stop_callback' object) even while other callbacks have
    // begun executing.

  private:
    typedef stop_callback<DestroyingCallback> StopCallback;

    StopCallback *d_stopCallback1_p;
    StopCallback *d_stopCallback2_p;

  public:
    DestroyingCallback(bsls::AtomicInt *numInvocations,
                       StopCallback    *stopCallback1,
                       StopCallback    *stopCallback2)
    : GenericCallback(numInvocations)
    , d_stopCallback1_p(stopCallback1)
    , d_stopCallback2_p(stopCallback2)
    {
    }

    void operator()()
    {
        GenericCallback::operator()();
        StopCallback *stopCallback1 = d_stopCallback1_p;
        StopCallback *stopCallback2 = d_stopCallback2_p;
        bslma::DestructionUtil::destroy(stopCallback1);
        bslma::DestructionUtil::destroy(stopCallback2);
    }
};

struct Latch {
    // Minimal latch for pre-C++20 build modes.
    explicit Latch(int count) : d_count(count) {}

    void arriveAndWait()
        // Atomically decrement 'd_count' and then wait for 'd_count' to become
        // zero.  When 'd_count' becomes zero, all waiting threads are
        // unblocked.
    {
        if (d_count.add(-1)) {
            do { } while (d_count.load());
        }
    }

    bsls::AtomicInt d_count;
};

// We're levelized below 'bslmt' and we can't use 'std::thread' in C++03, so we
// must implement a minimal abstraction over platform-specific threading
// utilities in order to test the behavior of 'stop_token', 'stop_source', and
// 'stop_callback' when they are accessed concurrently from multiple threads.
#if defined(BSLS_PLATFORM_OS_WINDOWS)
class Thread {
    // Abstraction over a Windows thread.
  private:
    struct Thunk {
        void (*d_fn_p)(void*);
        void  *d_arg_p;
        static DWORD WINAPI threadProc(void *arg)
        {
            Thunk *thunk = static_cast<Thunk*>(arg);
            thunk->d_fn_p(thunk->d_arg_p);
            return 0;
        }
    };

    Thunk  d_thunk;
    HANDLE d_handle;

  private:
    // NOT IMPLEMENTED
    Thread(const Thread&) BSLS_KEYWORD_DELETED;
    Thread& operator=(const Thread&) BSLS_KEYWORD_DELETED;

  public:
    Thread(void (*fn)(void*), void *arg)
    {
        d_thunk.d_fn_p = fn;
        d_thunk.d_arg_p = arg;
        d_handle = CreateThread(NULL, 0, &Thunk::threadProc, &d_thunk, 0, 0);
        if (!d_handle) {
            std::cout << "insufficient resources to create thread"
                      << std::endl;
            abort();
        }
    }

    ~Thread()
    {
        if (d_handle) {
            std::cout << "thread was not joined prior to being destroyed"
                      << std::endl;
            abort();
        }
    }

    void join()
    {
        if (WaitForSingleObject(d_handle, INFINITE) != WAIT_OBJECT_0) {
            std::cout << "join failed: " << GetLastError() << std::endl;
            abort();
        }
        CloseHandle(d_handle);
        d_handle = 0;
    }
};
#else
class Thread {
    // Abstraction over a Unix thread.
  private:
    struct Thunk {
        void (*d_fn_p)(void*);
        void  *d_arg_p;

        static void* startRoutine(void *arg)
        {
            Thunk *thunk = static_cast<Thunk*>(arg);
            thunk->d_fn_p(thunk->d_arg_p);
            return NULL;
        }
    };

    Thunk     d_thunk;
    pthread_t d_thread;
    bool      d_joined;

  private:
    // NOT IMPLEMENTED
    Thread(const Thread&) BSLS_KEYWORD_DELETED;
    Thread& operator=(const Thread&) BSLS_KEYWORD_DELETED;

  public:
    Thread(void (*fn)(void*), void *arg) : d_joined(false)
    {
        d_thunk.d_fn_p = fn;
        d_thunk.d_arg_p = arg;
        if (pthread_create(&d_thread, NULL, &Thunk::startRoutine, &d_thunk)) {
            std::cout << "insufficient resources to create thread"
                      << std::endl;
            abort();
        }
    }

    ~Thread()
    {
        if (!d_joined) {
            std::cout << "thread was not joined prior to being destroyed"
                      << std::endl;
            abort();
        }
    }

    void join()
    {
        if (pthread_join(d_thread, NULL)) {
            std::cout << "failed to join with thread" << std::endl;
            abort();
        }
        d_joined = true;
    }
};
#endif

class ArrivingCallback : public GenericCallback {
    // This callback type arrives at a latch upon invocation.

  private:
    Latch* d_latch_p;

  public:
    ArrivingCallback(bsls::AtomicInt *numInvocations,
                     Latch           *latch)
    : GenericCallback(numInvocations)
    , d_latch_p(latch)
    {
    }

    void operator()()
    {
        d_latch_p->arriveAndWait();
        GenericCallback::operator()();
    }
};

void threadFn(void *arg)
{
    stop_source *ss = static_cast<stop_source*>(arg);
    ss->request_stop();
}
}  // close namespace test_case_20

// ============================================================================
//                                TEST CASE 19
// ----------------------------------------------------------------------------
namespace test_case_19 {
enum AllocatorUsage {
    e_NO_ALLOCATOR,        // not allocator-aware
    e_TRAILING_ALLOCATOR,  // 'bslma::Allocator*' as last argument
    e_LEADING_ALLOCATOR    // leading (allocator_arg_t, allocator<char>)
};

enum ArgumentType {
    e_CALLBACK,
    e_OTHER_TYPE
};

enum ValueCategory {
    e_LVALUE,
    e_RVALUE
};

struct ConvertibleToCallback {
    // This struct is not a callback, but is accepted by the constructors of
    // 'Callback' (defined below).
    bsls::AtomicInt                       *d_numInvocations_p;
    bsls::AtomicPointer<bslma::Allocator> *d_allocator_p;

    ConvertibleToCallback(
                         bsls::AtomicInt                       *numInvocations,
                         bsls::AtomicPointer<bslma::Allocator> *allocator)
    : d_numInvocations_p(numInvocations)
    , d_allocator_p(allocator)
    {
    }
};

template <AllocatorUsage t_EXPECTED_ALLOC_USAGE,
          ArgumentType   t_EXPECTED_ARG_TYPE,
          ValueCategory  t_EXPECTED_ARG_CATEGORY>
class Callback {
    // This callback type defines a series of constructors that:
    //
    //: 1 assert that an allocator was passed in the correct manner, or that no
    //:   allocator was passed, as specified by the template parameter
    //:   't_ALLOCATOR_USAGE'
    //:
    //: 2 assert that that the type and value category of the value argument
    //:   are as expected (specified by the template parameters
    //:   't_EXPECTED_ARG_TYPE' and 't_EXPECTED_ARG_CATEGORY')
    //:
    //: 3 store a copy of the 'bslma::Allocator*' value that was passed (or a
    //:   null pointer if none was provided) so that the caller can compare
    //:   this stored value with the expected value.

  private:
    // DATA
    bsls::AtomicInt                       *d_numInvocations_p;
    bsls::AtomicPointer<bslma::Allocator> *d_allocator_p;

    // PRIVATE ACCESSORS
    void checkArg(const Callback&)
    {
        ASSERT(t_EXPECTED_ARG_TYPE == e_CALLBACK);
        ASSERT(t_EXPECTED_ARG_CATEGORY == e_LVALUE);
    }

    void checkArg(bslmf::MovableRef<Callback>)
    {
        ASSERT(t_EXPECTED_ARG_TYPE == e_CALLBACK);
        ASSERT(t_EXPECTED_ARG_CATEGORY == e_RVALUE);
    }

    void checkArg(const ConvertibleToCallback&)
    {
        ASSERT(t_EXPECTED_ARG_TYPE == e_OTHER_TYPE);
        ASSERT(t_EXPECTED_ARG_CATEGORY == e_LVALUE);
    }

    void checkArg(bslmf::MovableRef<ConvertibleToCallback>)
    {
        ASSERT(t_EXPECTED_ARG_TYPE == e_OTHER_TYPE);
        ASSERT(t_EXPECTED_ARG_CATEGORY == e_RVALUE);
    }

  public:
    // TYPES
    typedef typename conditional<t_EXPECTED_ALLOC_USAGE == e_NO_ALLOCATOR,
                                 void,
                                 allocator<char> >::type allocator_type;

    // CREATORS
    Callback(bsls::AtomicInt                       *numInvocations,
             bsls::AtomicPointer<bslma::Allocator> *allocator)
    : d_numInvocations_p(numInvocations)
    , d_allocator_p(allocator)
    {
    }

    Callback(const Callback& other)
    : d_numInvocations_p(other.d_numInvocations_p)
    {
        ASSERT(t_EXPECTED_ALLOC_USAGE == e_NO_ALLOCATOR);
        other.d_allocator_p->store(0);
        checkArg(other);
    }

    Callback(bslmf::MovableRef<Callback> other)
    : d_numInvocations_p(MoveUtil::access(other).d_numInvocations_p)
    {
        ASSERT(t_EXPECTED_ALLOC_USAGE == e_NO_ALLOCATOR);
        MoveUtil::access(other).d_allocator_p->store(0);
        checkArg(MoveUtil::move(other));
    }

    template <class t_ARG>
    Callback(BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg)
    : d_numInvocations_p(MoveUtil::access(arg).d_numInvocations_p)
    {
        ASSERT(t_EXPECTED_ALLOC_USAGE == e_NO_ALLOCATOR);
        MoveUtil::access(arg).d_allocator_p->store(0);
        checkArg(BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
    }

    template <class t_ARG>
    Callback(allocator_arg_t,
             allocator<char>                          alloc,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg)
    : d_numInvocations_p(MoveUtil::access(arg).d_numInvocations_p)
    {
        ASSERT(t_EXPECTED_ALLOC_USAGE == e_LEADING_ALLOCATOR);
        MoveUtil::access(arg).d_allocator_p->store(alloc.mechanism());
        checkArg(BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
    }

    template <class t_ARG>
    Callback(BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg,
             bslma::Allocator                        *alloc)
    : d_numInvocations_p(MoveUtil::access(arg).d_numInvocations_p)
    {
        ASSERT(t_EXPECTED_ALLOC_USAGE == e_TRAILING_ALLOCATOR);
        MoveUtil::access(arg).d_allocator_p->store(alloc);
        checkArg(BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
    }

    // MANIPULATORS
    void operator()()
    {
        d_numInvocations_p->add(1);
    }

    // NESTED TRAIT DECLARATIONS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Callback,
                                      bslma::UsesBslmaAllocator,
                                      t_EXPECTED_ALLOC_USAGE !=
                                          e_NO_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Callback,
                                      bslmf::UsesAllocatorArgT,
                                      t_EXPECTED_ALLOC_USAGE ==
                                          e_LEADING_ALLOCATOR);
};

template <ValueCategory t_CATEGORY>
struct ForwardAs;

template <>
struct ForwardAs<e_LVALUE> {
    template <class t_TYPE>
    const t_TYPE& operator()(t_TYPE& r) const
    {
        return r;
    }
};

template <>
struct ForwardAs<e_RVALUE> {
    template <class t_TYPE>
    bslmf::MovableRef<t_TYPE> operator()(t_TYPE& r) const
    {
        return MoveUtil::move(r);
    }
};

template <class t_CALLBACK, class t_TOKEN, class t_ARG>
void constructStopCallback(
                       stop_callback<t_CALLBACK>                  *target,
                       true_type,
                       bslma::Allocator                           *allocPassed,
                       BSLS_COMPILERFEATURES_FORWARD_REF(t_TOKEN)  token,
                       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)    arg)
    // Construct a 'stop_callback' object at the specified 'target' address,
    // where the specified 'token' and 'arg' are forwarded to the constructor,
    // followed by the specified 'allocPassed' if non-null.  't_CALLBACK' shall
    // be allocator-aware.
{
    if (allocPassed) {
        new (target) stop_callback<t_CALLBACK>(
                                 BSLS_COMPILERFEATURES_FORWARD(t_TOKEN, token),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg),
                                 allocPassed);
    } else {
        new (target) stop_callback<t_CALLBACK>(
                                 BSLS_COMPILERFEATURES_FORWARD(t_TOKEN, token),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
    }
}

template <class t_CALLBACK, class t_TOKEN, class t_ARG>
void constructStopCallback(stop_callback<t_CALLBACK>                 *target,
                           false_type,
                           bslma::Allocator*,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_TOKEN) token,
                           BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg)
    // Construct a 'stop_callback' object at the specified 'target' address,
    // where the specified 'token' and 'arg' are forwarded to the constructor.
    // 't_CALLBACK' shall not be allocator-aware.
{
    new (target) stop_callback<t_CALLBACK>(
                             BSLS_COMPILERFEATURES_FORWARD(t_TOKEN, token),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <AllocatorUsage t_EXPECTED_ALLOC_USAGE,
          ArgumentType   t_ARG_TYPE,
          ValueCategory  t_ARG_CATEGORY,
          ValueCategory  t_TOKEN_CATEGORY>
void runTest()
{
    typedef Callback<t_EXPECTED_ALLOC_USAGE, t_ARG_TYPE, t_ARG_CATEGORY> F;

    typedef typename bsl::conditional<t_ARG_TYPE == e_CALLBACK,
                                      F,
                                      ConvertibleToCallback>::type ArgObjType;

    enum SubCase {
        k_NO_STOP_STATE = 0,
            // Stop token doesn't own a stop state.

        k_ALREADY_STOPPED = 1,
            // Stop token owns a stop state that has already been stopped at
            // the time that the 'stop_callback' is created.

        k_NOT_STOPPED_YET = 2,
            // Stop token owns a stop state that has not yet been stopped at
            // the time that the 'stop_callback' is created, but will be
            // stopped later.

        k_NUM_SUBCASES = 3
    };

    for (int i = 0; i < k_NUM_SUBCASES; i++) {
        for (int passAlloc = 0; passAlloc < 2; passAlloc++) {
            if (passAlloc == 1 && e_NO_ALLOCATOR == t_EXPECTED_ALLOC_USAGE) {
                // Allocator cannot be passed to 'stop_callback' constructor
                // when the type it wraps is not allocator-aware.
                continue;                                           // CONTINUE
            }

            if (veryVerbose) {
                T_ P_(t_EXPECTED_ALLOC_USAGE) P_(t_ARG_TYPE) P_(t_ARG_CATEGORY)
                   P_(t_TOKEN_CATEGORY) P_(i) P(passAlloc)
            }

            char                         buf[k_BUFFER_SIZE];
            bslma::BufferAllocator       bufferAllocator(buf,
                                                         k_BUFFER_SIZE,
                                                         assertOnOOM);
            bslma::TestAllocator         ta1("default",
                                             veryVeryVeryVerbose,
                                             &bufferAllocator);
            bslma::TestAllocator         ta2("other",
                                             veryVeryVeryVerbose,
                                             &bufferAllocator);
            bslma::DefaultAllocatorGuard guard(&ta1);
            // We aren't testing the destructor of 'stop_callback' yet, so the
            // stop state will be leaked when the allocator is destroyed.
            // However, the memory will be cleaned up when 'buf' goes out of
            // scope.
            ta1.setQuiet(true);
            ta2.setQuiet(true);

            bsls::AtomicPointer<bslma::Allocator> allocUsed;
            bsls::AtomicInt                       numInvocations;
            ArgObjType                            arg(&numInvocations,
                                                      &allocUsed);

            bslma::Allocator     *allocPassed   = passAlloc ? &ta2 : 0;
            bslma::TestAllocator *allocExpected = 0;
            if (t_EXPECTED_ALLOC_USAGE != e_NO_ALLOCATOR) {
                allocExpected = passAlloc ? &ta2 : &ta1;
            }

            stop_source                           source(allocPassed);
            stop_token                            t;
            bsls::ObjectBuffer<stop_callback<F> > c;

            if (i != k_NO_STOP_STATE) {
                t = source.get_token();
            }

            if (i == k_ALREADY_STOPPED) {
                source.request_stop();
            }

            integral_constant<bool, e_NO_ALLOCATOR != t_EXPECTED_ALLOC_USAGE>
                isAllocatorAware;

            constructStopCallback(c.address(),
                                  isAllocatorAware,
                                  allocPassed,
                                  ForwardAs<t_TOKEN_CATEGORY>()(t),
                                  ForwardAs<t_ARG_CATEGORY>()(arg));

            ASSERT(allocExpected == allocUsed.load());

            // If there is a stop state and it is already stopped, then the
            // callback should have been invoked already.  Otherwise, the
            // callback should not be invoked yet.
            ASSERT((i == k_ALREADY_STOPPED) == numInvocations.load());

            if (i == k_NOT_STOPPED_YET) {
                source.request_stop();
                ASSERT(1 == numInvocations.load());
            }
        }
    }
}
}  // close namespace test_case_19

// ============================================================================
//                                TEST CASE 2
// ----------------------------------------------------------------------------
namespace test_case_2 {
template <class t_TYPE>
bool is_nostopstate_t_const_pointer(t_TYPE*)
{
    return false;
}
bool is_nostopstate_t_const_pointer(const nostopstate_t*)
{
    return true;
}
}  // close namespace test_case_2

// ============================================================================
//                               BREATHING TEST
// ----------------------------------------------------------------------------
namespace test_case_1 {
bool invoked = false;
void callback()
{
    invoked = true;
}
}  // close namespace test_case_1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST %s CASE %d\n", __FILE__, test);

    // CONCERN: In no case does memory come from the global allocator.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 24: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown when the compiler supports C++11.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        usage_example::UsageExample usageExample;
#else
        if (verbose) printf("Skipping usage example in C++03...\n");
#endif
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // 'stop_callback' DEDUCTION GUIDE
        //
        // Concerns:
        //: 1 'stop_callback' supports class template argument deduction as
        //:   specified by the standard.
        //
        // Plan:
        //: 1 Create a 'stop_callback' object without specify explicit template
        //:   arguments and verify that the declared type is 'stop_token<F>',
        //:   where 'F' is the decayed type of the second argument.  (C-1)
        //
        // Testing:
        //   stop_callback(stop_token, T) -> stop_callback(T);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_callback' DEDUCTION GUIDE"
                            "\n===============================\n");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        const GenericCallback callback(0);

        stop_callback sc(stop_token(), callback);

        static_assert(is_same_v<decltype(sc), stop_callback<GenericCallback>>);
#else
        if (verbose) printf("Skipping case 23 in C++14 and below...\n");
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // CONST AND REFERENCE CALLBACKS
        //
        // Concerns:
        //: 1 The template argument for 'stop_callback' can be a const type
        //:   that has a 'const'-qualified function call operator.
        //:
        //: 2 The template argument for 'stop_callback' can be a reference
        //:   type, 'F&', if an lvalue of 'F' is callable.
        //
        // Plan:
        //: 1 Create a 'stop_callback<const C>' object registered to a stop
        //:   state, where 'C' has a const-qualified 'operator()'.  Request a
        //:   stop, and verify that the callback has been invoked.  (C-1)
        //:
        //: 2 Create a 'stop_callback<const C&>' object registered to a stop
        //:   state, where 'C' is a type that, upon invocation, will detect
        //:   whether it has been copied; pass the original 'C' object to the
        //:   'stop_callback' constructor.  Request a stop, and verify that the
        //:   callback has been invoked.  (C-2)
        //:
        //: 3 Create a 'stop_callback<C&>' object registered to a stop state,
        //:   where 'C' not only can detect whether it has been copied, but
        //:   also may only be called through a non-'const' instance.  Request
        //:   a stop, and verify that the callback has been invoked.  (C-2)
        //
        // Testing:
        //   stop_callback(const stop_token&, T&, allocator_type = {});
        //   stop_callback(stop_token&&, T&, allocator_type = {});
        //   CONCERN: 'stop_callback' supports const and reference types.
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONST AND REFERENCE CALLBACKS"
                            "\n=============================\n");

        using namespace test_case_22;

        // P-1
        {
            bslma::TestAllocator                 ta("test",
                                                    veryVeryVeryVerbose);
            stop_source                          ss(&ta);
            bsls::AtomicInt                      numInvocations;
            stop_callback<const GenericCallback> sc(ss.get_token(),
                                                    &numInvocations);

            ss.request_stop();
            ASSERT(1 == numInvocations.load());
        }


        // P-2
        {
            bslma::TestAllocator                    ta("test",
                                                       veryVeryVeryVerbose);
            stop_source                             ss(&ta);
            bsls::AtomicInt                         numInvocations;
            AssertingCallback                       cb(&numInvocations);
            stop_callback<const AssertingCallback&> sc(ss.get_token(), cb);

            ss.request_stop();
            ASSERT(1 == numInvocations.load());
        }

        // P-3
        {
            bslma::TestAllocator             ta("test", veryVeryVeryVerbose);
            stop_source                      ss(&ta);
            bsls::AtomicInt                  numInvocations;
            NonConstCallback                 cb(&numInvocations);
            const stop_token                 token = ss.get_token();
            stop_callback<NonConstCallback&> sc(token, cb);

            ss.request_stop();
            ASSERT(1 == numInvocations.load());
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // 'stop_callback' TYPES AND ACCESSORS
        //
        // Concern:
        //: 1 If 'F' is allocator-aware, then
        //:   'stop_callback<F>::allocator_type' is 'bsl::allocator<char>'.
        //:
        //: 2 If 'F' is allocator-aware, then
        //:   'stop_callback<F>::get_allocator' is a 'const' member function
        //:   that returns the allocator used to construct the object.
        //:
        //: 3 'stop_callback<F>::callback_type' is always a typedef for 'F'.
        //
        // Plan:
        //: 1 Define a callback type, 'C', that is allocator-aware.
        //:
        //: 2 Verify that 'stop_callback<C>::allocator_type' is
        //:   'bsl::allocator<char>'.  (C-1)
        //:
        //: 3 Construct a 'const stop_callback<C>' object, where the stored
        //:   callback is constructed from a 'C' object that has a specified
        //:   allocator.  Call 'get_allocator' and verify that the result is
        //:   equal to the allocator that was passed in.  (C-2)
        //:
        //: 4 Verify that 'stop_callback<C>::callback_type' is 'C'.  (C-3)
        //
        // Testing:
        //   stop_callback<F>::allocator_type
        //   stop_callback<F>::callback_type
        //   allocator_type stop_callback::get_allocator() const;
        // --------------------------------------------------------------------
        if (verbose)
            printf("\n'stop_callback' TYPES AND ACCESSORS"
                   "\n===================================\n");

        typedef test_case_21::Callback C;

        BSLMF_ASSERT((is_same<stop_callback<C>::allocator_type,
                              allocator<char> >::value));

        BSLMF_ASSERT((is_same<stop_callback<C>::callback_type, C>::value));

        bslma::TestAllocator   ta;
        const allocator<char>  alloc(&ta);
        const C                callback;
        const stop_callback<C> sc(stop_token(), callback, alloc);
        ASSERT(alloc == sc.get_allocator());
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // 'stop_callback' DESTRUCTOR
        //
        // Concerns:
        //: 1 Destroying a 'stop_callback' relinquishes ownership of any stop
        //:   state that it owns.
        //:
        //: 2 If the stored callback has not yet been invoked, the destructor
        //:   deregisters the callback from any stop state that the
        //:   'stop_callback' might own, and does not invoke the stored
        //:   callback.
        //:
        //: 3 If, while a thread is invoking a 'stop_callback', a different
        //:   thread calls the 'stop_callback' object's destructor, the
        //:   destructor does not return until the invocation has completed.
        //:
        //: 4 A callback destroying the 'stop_callback' object that owns itself
        //:   does not cause a deadlock.
        //
        // Plan:
        //: 1 Create a test allocator that will be used to supply memory for
        //:   the steps below.  Note that the test allocator will abort upon
        //:   destruction if any memory remains allocated.  (C-1)
        //:
        //: 2 Create and destroy a 'stop_callback' that doesn't own a stop
        //:   state.  Verify that the stored callback was not invoked.  (C-2)
        //:
        //: 3 Create a 'stop_callback' that owns a stop state, and request a
        //:   stop.  Finally, allow the 'stop_callback' and 'stop_source' to
        //:   go out of scope.
        //:
        //: 4 Create a 'stop_callback' that is registered to a stop state, but
        //:   do not request a stop.  Destroy the 'stop_source' and then the
        //:   'stop_callback', and verify that the stored callback was not
        //:   invoked.  (C-2)
        //:
        //: 5 Repeat step 3, but have the 'stop_source' relinquish ownership of
        //:   the stop state before the 'stop_callback' is destroyed.  (C-2)
        //:
        //: 6 Create two 'stop_callback's, both of which are registered with
        //:   the same stop state, and each of which, upon invocation, will
        //:   destroy both 'stop_callback' objects.  Request a stop, then
        //:   verify that the second callback was not invoked.  (C-2,4)
        //:
        //: 7 Create a 'stop_callback' that is registered with a stop state and
        //:   that, upon invocation, will arrive at a latch.  In a separate
        //:   thread, request a stop.  In the main thread, arrive at the latch
        //:   and then destroy the 'stop_callback' object, then verify that the
        //:   invocation has completed.  (C-3)
        //
        // Testing:
        //   stop_callback::~stop_callback();
        // --------------------------------------------------------------------
        if (verbose) printf("\n'stop_callback' DESTRUCTOR"
                            "\n==========================\n");

        using namespace test_case_20;

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // P-2
        {
            bsls::AtomicInt numInvocations;

            (void)stop_callback<GenericCallback>(stop_token(),
                                                 &numInvocations);
            ASSERT(0 == numInvocations.load());
        }

        // P-3
        {
            bsls::AtomicInt                numInvocations;
            stop_source                    ss(&ta);
            stop_callback<GenericCallback> sc(ss.get_token(), &numInvocations);

            ss.request_stop();
        }

        // P-4
        {
            bsls::AtomicInt numInvocations;
            {
                const stop_source ss(&ta);

                (void)stop_callback<GenericCallback>(ss.get_token(),
                                                     &numInvocations);
            }
            ASSERT(0 == numInvocations.load());
        }

        // P-5
        {
            bsls::AtomicInt numInvocations;
            {
                stop_source ss(&ta);

                stop_callback<GenericCallback> sc(ss.get_token(),
                                                  &numInvocations);

                stop_source(nostopstate).swap(ss);
            }
            ASSERT(0 == numInvocations.load());
        }

        // P-6
        {
            bsls::AtomicInt numInvocations;
            {
                bsls::ObjectBuffer<stop_callback<DestroyingCallback> > buf1;
                bsls::ObjectBuffer<stop_callback<DestroyingCallback> > buf2;

                stop_source ss(&ta);

                DestroyingCallback cb(&numInvocations,
                                      buf1.address(),
                                      buf2.address());

                new (buf1.buffer()) stop_callback<DestroyingCallback>(
                                                                ss.get_token(),
                                                                cb);
                new (buf2.buffer()) stop_callback<DestroyingCallback>(
                                                                ss.get_token(),
                                                                cb);

                ss.request_stop();
            }
            ASSERT(1 == numInvocations.load());
        }

        // P-7
        {
            bsls::AtomicInt  numInvocations;
            Latch            latch(2);
            ArrivingCallback cb(&numInvocations, &latch);

            stop_source                                ss(&ta);
            optional<stop_callback<ArrivingCallback> > sc;
            sc.emplace(ss.get_token(), cb);

            Thread thread(&threadFn, &ss);
            latch.arriveAndWait();
            sc.reset();
            ASSERT(1 == numInvocations.load());
            thread.join();
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // 'stop_callback' CONSTRUCTORS
        //
        // Concerns:
        //: 1 The 'stop_callback' constructors construct the stored callback by
        //:   perfect forwarding from the argument 'arg'.
        //:
        //: 2 If 't_CALLBACK' is allocator-aware, the allocator argument will
        //:   be used to perform uses-allocator construction of 't_CALLBACK'
        //:   (or the currently installed default allocator if that argument is
        //:   omitted).  Otherwise, an allocator argument does not need to be
        //:   supplied.
        //:
        //: 3 If the specified 'stop_token' does not own a stop state, the
        //:   callback will not be invoked.
        //:
        //: 4 If the specified 'stop_token' owns a stop state and
        //:   'stop_requested()' is 'true', the callback will be invoked before
        //:   the constructor returns.
        //:
        //: 5 If the specified 'stop_token' owns a stop state and
        //:   'stop_requested()' is 'false', the constructor registers the
        //:   callback to be invoked when a stop is requested.
        //
        // Plan:
        //: 1 Define a callback type parametrized by template parameters that
        //:   specify the uses-allocator construction style (not
        //:   allocator-aware, trailing allocator argument, or leading
        //:   'allocator_arg_t' and allocator argument) and the type and value
        //:   category of the argument that are expected to be passed to
        //:   specify the callback state, and that will assert that the actual
        //:   arguments passed to its constructor match the expected arguments.
        //:
        //: 2 For each possible combination of user-allocator construction
        //:   style, lvalue or rvalue 'stop_token', and lvalue or rvalue of
        //:   either the callback type or of a type convertible thereto:
        //:
        //:   1 Create a 'stop_callback' object using a null pointer as the
        //:     allocator and a 'stop_token' that doesn't own a stop state,
        //:     whose callback type is the specialization of the template
        //:     defined in P-1 that expects the current combination of
        //:     arguments.  Verify that the 'bslma::Allocator*' value used when
        //:     constructing the callback object stored in the 'stop_callback'
        //:     is the currently installed default allocator.  Verify that the
        //:     callback is not invoked and that the 'stop_token' is not moved
        //:     from.  (C-1..3)
        //:
        //:   2 Repeat step P-2.1, but with a specified allocator.  Verify that
        //:     this allocator was passed to the stored callback's constructor
        //:     if and only if the stored callback is allocator-aware.
        //:     (C-1..3)
        //:
        //:   3 Repeat steps P-2.1..2, but with a 'stop_token' owning a stop
        //:     state that is already stopped.  Verify that the callback is
        //:     invoked before the constructor returns.  (C-1,2,4)
        //:
        //:   4 Repeat steps P-2.1..2, but with a 'stop_token' owning a stop
        //:     state that is not yet stopped.  Verify that the callback is not
        //:     invoked until 'request_stop' is called.  (C-1,2,5)
        //
        // Testing:
        //   stop_callback(const stop_token&, T&&, allocator_type = {});
        //   stop_callback(stop_token&&, T&&, allocator_type = {});
        // --------------------------------------------------------------------
        if (verbose) printf("\n'stop_callback' CONSTRUCTORS"
                            "\n============================\n");

        using namespace test_case_19;

        runTest<e_NO_ALLOCATOR,       e_CALLBACK,   e_LVALUE, e_LVALUE>();
        runTest<e_NO_ALLOCATOR,       e_CALLBACK,   e_LVALUE, e_RVALUE>();
        runTest<e_NO_ALLOCATOR,       e_CALLBACK,   e_RVALUE, e_LVALUE>();
        runTest<e_NO_ALLOCATOR,       e_CALLBACK,   e_RVALUE, e_RVALUE>();
        runTest<e_NO_ALLOCATOR,       e_OTHER_TYPE, e_LVALUE, e_LVALUE>();
        runTest<e_NO_ALLOCATOR,       e_OTHER_TYPE, e_LVALUE, e_RVALUE>();
        runTest<e_NO_ALLOCATOR,       e_OTHER_TYPE, e_RVALUE, e_LVALUE>();
        runTest<e_NO_ALLOCATOR,       e_OTHER_TYPE, e_RVALUE, e_RVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_CALLBACK,   e_LVALUE, e_LVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_CALLBACK,   e_LVALUE, e_RVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_CALLBACK,   e_RVALUE, e_LVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_CALLBACK,   e_RVALUE, e_RVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_OTHER_TYPE, e_LVALUE, e_LVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_OTHER_TYPE, e_LVALUE, e_RVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_OTHER_TYPE, e_RVALUE, e_LVALUE>();
        runTest<e_TRAILING_ALLOCATOR, e_OTHER_TYPE, e_RVALUE, e_RVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_CALLBACK,   e_LVALUE, e_LVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_CALLBACK,   e_LVALUE, e_RVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_CALLBACK,   e_RVALUE, e_LVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_CALLBACK,   e_RVALUE, e_RVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_OTHER_TYPE, e_LVALUE, e_LVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_OTHER_TYPE, e_LVALUE, e_RVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_OTHER_TYPE, e_RVALUE, e_LVALUE>();
        runTest<e_LEADING_ALLOCATOR,  e_OTHER_TYPE, e_RVALUE, e_RVALUE>();
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // 'stop_source' SWAP
        //
        // Concerns:
        //: 1 The member and non-member 'swap' functions of 'stop_source'
        //:   exchange the values of the two operands, and no stop states are
        //:   leaked in the process.
        //:
        //: 2 A 'stop_source' can be swapped with itself, with no effect.
        //:
        //: 3 The member and non-member 'swap' functions of 'stop_source' are
        //:   'noexcept'.
        //
        // Plan:
        //: 1 Create three 'stop_source's, namely 's0', 's1', 's2', where 's0'
        //:   and 's1' are created using a specified test allocator, and 's2'
        //:   doesn't own a stop state.  Create 's3', 's4', and 's5' as copies
        //:   of 's0', 's1', and 's2', respectively.  For each ordered pair of
        //:   'stop_source's:  (C-1,2)
        //:
        //:   1 Call the non-member 'swap' function and verify that after the
        //:     swap, the two objects swapped compare equal to the values they
        //:     had before the swap.  Then call 'swap' again to restore them to
        //:     the values they had before the swap.
        //:
        //:   2 Repeat P-1.1, but using member 'swap'.
        //:
        //: 2 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-3)
        //
        // Testing:
        //   void stop_source::swap(stop_source&) noexcept;
        //   void swap(stop_source&, stop_source&) noexcept;
        // --------------------------------------------------------------------
        if (verbose) printf("\n'stop_source' SWAP"
                            "\n==================\n");

        bslma::TestAllocator alloc("test", veryVeryVeryVerbose);

        stop_source  s0(&alloc);
        stop_source  s1(&alloc);
        stop_source  s2(nostopstate);
        stop_source  s3(s0);
        stop_source  s4(s1);
        stop_source  s5(s2);
        stop_source *sources[6] = {&s0, &s1, &s2, &s3, &s4, &s5};

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (veryVerbose) { T_ P_(i) P(j) }

                stop_source&      lhs     = *sources[i];
                stop_source&      rhs     = *sources[j];
                const stop_source lhsCopy = lhs;
                const stop_source rhsCopy = rhs;

                swap(lhs, rhs);
                ASSERT(lhs == rhsCopy);
                ASSERT(rhs == lhsCopy);

                swap(lhs, rhs);

                lhs.swap(rhs);
                ASSERT(lhs == rhsCopy);
                ASSERT(rhs == lhsCopy);

                rhs.swap(lhs);
            }
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(swap(*sources[0], *sources[0])));
        ASSERT(noexcept(sources[0]->swap(*sources[0])));
#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // 'stop_source' MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1 The move assignment operator of 'stop_source' makes the left
        //:   operand own the stop state that the right operand owned,
        //:   relinquishes ownership of any stop state previously owned by
        //:   either operand, and returns a reference to the left operand.
        //:
        //: 2 Self-move-assignment of 'stop_source' is a no-op.
        //:
        //: 3 The move assignment operator of 'stop_source' is 'noexcept'.
        //
        // Plan:
        //: 1 Create two 'stop_source's, 's1' and 's2', that both do not own
        //:   any stop state.  Evaluate 's1 = move(s2)' and verify that the
        //:   address of the result of this expression is the address of 's1'.
        //:   Verify that both 's1' and 's2' equal 'stop_source(nostopstate)'.
        //:   (C-1)
        //:
        //: 2 Create two 'stop_source's, 's1' and 's2', where 's2' owns a stop
        //:   state and is created using a specified test allocator (so we can
        //:   verify that all memory is deallocated by the end of the test).
        //:   Let 's3' be a copy of 's2'.  Evaluate 's1 = move(s2)' and verify
        //:   that the address of the result of this expression is the address
        //:   of 's1'.  Verify that 's1 == s3' and that
        //:   's2 == stop_source(nostopstate)'.  (C-1)
        //:
        //: 3 Repeat step 1 with an 's1' that owns a stop state.  (C-1)
        //:
        //: 4 Repeat step 2 with an 's1' that owns a different stop state from
        //:   's2'.  (C-1)
        //:
        //: 5 Create a 'stop_state', 's', that does not own a stop state, and
        //:   move-assign it to itself.  Verify that the address of the result
        //:   of the move-assignment is the address of the operand, and that
        //:   's' equals 'stop_source(nostopstate)' afterward.  (C-2)
        //:
        //: 6 Repeat step 5 with a 'stop_source' that owns a stop state.
        //:   Verify that, after the move assignment, it equals a copy that was
        //:   made beforehand.  (C-2)
        //:
        //: 7 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-3)
        //
        // Testing:
        //   stop_source& stop_source::operator=(stop_source&&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_source' MOVE ASSIGNMENT"
                            "\n=============================\n");

        bslma::TestAllocator alloc("test", veryVeryVeryVerbose);

        // P-1
        {
            stop_source s1(nostopstate);
            stop_source s2(nostopstate);
            ASSERT(&s1 == &(s1 = MoveUtil::move(s2)));
            ASSERT(stop_source(nostopstate) == s1);
            ASSERT(stop_source(nostopstate) == s2);
        }

        // P-2
        {
            stop_source       s1(nostopstate);
            stop_source       s2(&alloc);
            const stop_source s3 = s2;
            ASSERT(&s1 == &(s1 = MoveUtil::move(s2)));
            ASSERT(s1 == s3);
            ASSERT(s2 == stop_source(nostopstate));
        }

        // P-3
        {
            stop_source s1(&alloc);
            stop_source s2(nostopstate);
            ASSERT(&s1 == &(s1 = MoveUtil::move(s2)));
            ASSERT(stop_source(nostopstate) == s1);
            ASSERT(stop_source(nostopstate) == s2);
        }

        // P-4
        {
            stop_source       s1(&alloc);
            stop_source       s2(&alloc);
            const stop_source s3 = s2;
            ASSERT(&s1 == &(s1 = MoveUtil::move(s2)));
            ASSERT(s1 == s3);
            ASSERT(s2 == stop_source(nostopstate));
        }

        // P-5
        {
            stop_source s(nostopstate);
            ASSERT(&s == &(s = MoveUtil::move(s)));
            ASSERT(stop_source(nostopstate) == s);
        }

        // P-6
        {
            stop_source       s(&alloc);
            const stop_source s2 = s;
            ASSERT(&s == &(s = MoveUtil::move(s)));
            ASSERT(s == s2);
        }

        // P-7
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(
                 bslmf::Util::declval<stop_source&>() =
                     bslmf::Util::declval<bslmf::MovableRef<stop_source> >()));
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'stop_source' COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 The copy assignment operator of 'stop_source' makes the left
        //:   operand own the same stop state as the right operand,
        //:   relinquishes ownership of any stop state previously held by the
        //:   left operand, and returns a reference to the left operand.
        //:
        //: 2 The copy assignment operator of 'stop_source' is 'noexcept'.
        //
        // Plan:
        //: 1 Create two 'stop_source's, 's1' and 's2', neither of which owns a
        //:   stop state.  Evaluate 's1 = s2' and verify that the address of
        //:   the result of this expression is the address of 's1'.  Verify
        //:   that 's1 == s2'.  (C-1)
        //:
        //: 2 Repeat step 1, but with a 's2' that owns a stop state that is
        //:   allocated using a specified test allocator (so we can verify that
        //:   all memory is deallocated by the end of the test).  (C-1)
        //:
        //: 3 Repeat step 1, but with a 's1' that owns a stop state.  (C-1)
        //:
        //: 4 Repeat step 2, but with a 's1' that owns a different stop state
        //:   from 's2'.  (C-1)
        //:
        //: 5 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   stop_source& stop_source::operator=(const stop_source&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_source' COPY ASSIGNMENT"
                            "\n=============================\n");

        bslma::TestAllocator alloc("test", veryVeryVeryVerbose);

        // P-1
        {
            stop_source       s1(nostopstate);
            const stop_source s2(nostopstate);
            ASSERT(&s1 == &(s1 = s2));
            ASSERT(s1 == s2);
        }

        // P-2
        {
            stop_source       s1(nostopstate);
            const stop_source s2(&alloc);
            ASSERT(&s1 == &(s1 = s2));
            ASSERT(s1 == s2);
        }

        // P-3
        {
            stop_source       s1(&alloc);
            const stop_source s2(nostopstate);
            ASSERT(&s1 == &(s1 = s2));
            ASSERT(s1 == s2);
        }

        // P-4
        {
            stop_source       s1(&alloc);
            const stop_source s2(&alloc);
            ASSERT(&s1 == &(s1 = s2));
            ASSERT(s1 == s2);
        }

        // P-5
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(bslmf::Util::declval<stop_source&>() =
                                  bslmf::Util::declval<const stop_source&>()));
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // 'stop_source' COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 'stop_source' can be compared with itself for equality.  If both
        //:   'stop_source' objects don't own a stop state or both
        //:   'stop_source' objects own the same stop state, the result is
        //:   'true'; otherwise, the result is 'false'.
        //:
        //: 2 'stop_source' supports the '!=' operator.  The result of applying
        //:   the '!=' operator is the logical negation of the result of
        //:   applying the '==' operator.
        //:
        //: 3 The '==' and '!=' operators are 'noexcept'.
        //
        // Plan:
        //: 1 Create three different 'stop_source' objects: 's0' and 's1',
        //:   which are default-constructed, and 's2', which does not own a
        //:   stop state.  Let 's3' be a copy of 's0', 's4' be a copy of 's1',
        //:   and 's5' be a copy of 's2'.  Verify that {'t0', 't3'},
        //:   {'t1', 't4'}, and {'t2', 't5'} are equivalence classes of
        //:   'operator==' and that 'operator!=' returns the negated result.
        //:   (C-1)
        //:
        //: 2 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   bool operator==(const stop_source&, const stop_source&) noexcept;
        //   bool operator!=(const stop_source&, const stop_source&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_source' COMPARISON OPERATORS"
                            "\n==================================\n");

        const stop_source  s0;
        const stop_source  s1;
        const stop_source  s2(nostopstate);
        const stop_source  s3 = s0;
        const stop_source  s4 = s1;
        const stop_source  s5 = s2;
        const stop_source *sources[6] = {&s0, &s1, &s2, &s3, &s4, &s5};

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (veryVerbose) { T_ P_(i) P(j) }

                const bool expectEqual = (i == j) ||
                                         (i == j + 3) ||
                                         (j == i + 3);

                ASSERT(expectEqual == (*sources[i] == *sources[j]));
                ASSERT(expectEqual != (*sources[i] != *sources[j]));
            }
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(s0 == s0));
        ASSERT(noexcept(s0 != s0));
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'stop_source' MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The move constructor of 'stop_source' creates a 'stop_source'
        //:   that owns the stop state originally owned by the original object,
        //:   and leaves the original object with no ownership of a stop state.
        //:
        //: 2 The move constructor of 'stop_source' is 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_source' object, 's1', using the default
        //:   constructor, and let 't1' be 's1.get_token()'.  Create a
        //:   'stop_source' object, 's2', by move construction.  Verify that
        //:   's2.get_token() == t1' and that 's1.get_token() == stop_token()'.
        //:   (C-1)
        //:
        //: 2 Create a 'stop_source' object, 's1', that doesn't own a stop
        //:   state, and 's2', which is move-constructed from 's1'.  Verify
        //:   that 's1.get_token()' and 's2.get_token()' both equal
        //:   'stop_token()'.  (C-1)
        //:
        //: 3 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   stop_source::stop_source(stop_source&&) noexcept;
        //   stop_source::stop_source(MovableRef<stop_source>) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_source' MOVE CONSTRUCTOR"
                            "\n==============================\n");

        // P-1
        {
            stop_source       s1;
            const stop_token  t1 = s1.get_token();
            const stop_source s2(MoveUtil::move(s1));
            ASSERT(s1.get_token() == stop_token());
            ASSERT(s2.get_token() == t1);
        }

        // P-2
        {
            stop_source       s1(nostopstate);
            const stop_source s2(MoveUtil::move(s1));
            ASSERT(s1.get_token() == stop_token());
            ASSERT(s2.get_token() == stop_token());
        }

        // P-3
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(stop_source(
                    bslmf::Util::declval<bslmf::MovableRef<stop_source> >())));
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'stop_source' COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The copy constructor of 'stop_source' creates a 'stop_source'
        //:   that is equal to the original object.
        //:
        //: 2 The copy constructor of 'stop_source' is 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_source' object using the default constructor.
        //:   Create a 'stop_source' object by copy construction.  Verify that
        //:   'stop_token's obtained by calling 'get_token()' on each
        //:   'stop_source' are equal to each other.  (C-1)
        //:
        //: 2 Repeat step 1 with a 'stop_source' object that does not own a
        //:   stop state.  (C-1)
        //:
        //: 3 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   stop_source::stop_source(const stop_source&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_source' COPY CONSTRUCTOR"
                            "\n==============================\n");

        // P-1
        {
            const stop_source s1;
            stop_source       s2(s1);
            ASSERT(s1.get_token() == s2.get_token());
        }

        // P-2
        {
            const stop_source s1(nostopstate);
            stop_source       s2(s1);
            ASSERT(s1.get_token() == s2.get_token());
        }

        // P-3
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(
            noexcept(stop_source(bslmf::Util::declval<const stop_source&>())));
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'stop_token' SWAP
        //
        // Concerns:
        //: 1 The member and non-member 'swap' functions of 'stop_token'
        //:   exchange the values of the two operands, and no stop states are
        //:   leaked in the process.
        //:
        //: 2 A 'stop_token' can be swapped with itself, with no effect.
        //:
        //: 3 The member and non-member 'swap' functions of 'stop_token' are
        //:   'noexcept'.
        //
        // Plan:
        //: 1 Create three 'stop_source's, namely 's0', 's1', 's2', where 's0'
        //:   and 's1' are created using a specified test allocator, and 's2'
        //:   doesn't own a stop state.  Create a set of 6 'stop_tokens': 't0'
        //:   and 't3' obtained from 's0', 't1' and 't4' obtained from 's1',
        //:   and 't2' and 't5' obtained from 's2'.  For each ordered pair of
        //:   'stop_token's:  (C-1,2)
        //:
        //:   1 Call the non-member 'swap' function and verify that after the
        //:     swap, the two tokens swapped compare equal to the values they
        //:     had before the swap.  Then call 'swap' again to restore them to
        //:     the values they had before the swap.
        //:
        //:   2 Repeat P-1.1, but using member 'swap'.
        //:
        //: 2 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-3)
        //
        // Testing:
        //   void stop_token::swap(stop_token&) noexcept;
        //   void swap(stop_token&, stop_token&) noexcept;
        // --------------------------------------------------------------------
        if (verbose) printf("\n'stop_token' SWAP"
                            "\n=================\n");

        bslma::TestAllocator alloc("test", veryVeryVeryVerbose);

        const stop_source s0(&alloc);
        const stop_source s1(&alloc);
        const stop_source s2(nostopstate);
        stop_token        t[6] =
                              {s0.get_token(), s1.get_token(), s2.get_token(),
                               s0.get_token(), s1.get_token(), s2.get_token()};

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (veryVerbose) { T_ P_(i) P(j) }

                stop_token&       lhs     = t[i];
                stop_token&       rhs     = t[j];
                const stop_token  lhsCopy = lhs;
                const stop_token  rhsCopy = rhs;

                swap(lhs, rhs);
                ASSERT(lhs == rhsCopy);
                ASSERT(rhs == lhsCopy);

                swap(lhs, rhs);

                lhs.swap(rhs);
                ASSERT(lhs == rhsCopy);
                ASSERT(rhs == lhsCopy);

                rhs.swap(lhs);
            }
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(swap(t[0], t[0])));
        ASSERT(noexcept(t[0].swap(t[0])));
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'stop_token' MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1 The move assignment operator of 'stop_token' makes the left
        //:   operand own the stop state that the right operand owned,
        //:   relinquishes ownership of any stop state previously owned by
        //:   the left operand, makes the right operand equivalent to a
        //:   default-constructed 'stop_token', and returns a reference to the
        //:   left operand.
        //:
        //: 2 Self-move-assignment of 'stop_token' is a no-op.
        //:
        //: 3 The move assignment operator of 'stop_token' is 'noexcept'.
        //
        // Plan:
        //: 1 Create two default-constructed 'stop_token's, 't1' and 't2'.
        //:   Evaluate 't1 = move(t2)' and verify that the address of the
        //:   result of this expression is the address of 't1'.  Verify that
        //:   't1' and 't2' are both equal to 'stop_token()'.  (C-1)
        //:
        //: 2 Repeat step 1 with a 't2' that owns a stop state, obtained from a
        //:   stop source 's2' by calling 'get_token', and where 's2' is
        //:   created using a specified test allocator (so we can verify that
        //:   all memory is deallocated by the end of the test).  In this case,
        //:   't1' should equal 's2.get_token()' after the move-assignment.
        //:   (C-1)
        //:
        //: 3 Repeat step 1 with a 't1' that owns a stop state.  (C-1)
        //:
        //: 4 Repeat step 2 with a 't1' that owns a stop state obtained from a
        //:   different stop source, 's1'.  (C-1)
        //:
        //: 5 Create a default-constructed 'stop_token' and move-assign it to
        //:   itself.  Verify that the address of the result of the
        //:   move-assignment is the address of the operand, and that the
        //:   'stop_token' object still equals 'stop_token()' afterward.  (C-2)
        //:
        //: 6 Repeat step 5 with a 'stop_token' that owns a stop state.  Verify
        //:   that, after the move assignment, it equals a copy that was made
        //:   beforehand.  (C-2)
        //:
        //: 7 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-3)
        //
        // Testing:
        //   stop_token& stop_token::operator=(stop_token&&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token' MOVE ASSIGNMENT"
                            "\n============================\n");

        bslma::TestAllocator alloc("test", veryVeryVeryVerbose);

        // P-1
        {
            stop_token t1;
            stop_token t2;
            ASSERT(&t1 == &(t1 = MoveUtil::move(t2)));
            ASSERT(stop_token() == t1);
            ASSERT(stop_token() == t2);
        }

        // P-2
        {
            stop_token        t1;
            const stop_source s2(&alloc);
            stop_token        t2 = s2.get_token();
            ASSERT(&t1 == &(t1 = MoveUtil::move(t2)));
            ASSERT(s2.get_token() == t1);
            ASSERT(stop_token() == t2);
        }

        // P-3
        {
            const stop_source s1(&alloc);
            stop_token        t1 = s1.get_token();
            stop_token        t2;
            ASSERT(&t1 == &(t1 = MoveUtil::move(t2)));
            ASSERT(stop_token() == t1);
            ASSERT(stop_token() == t2);
        }

        // P-4
        {
            const stop_source s1(&alloc);
            const stop_source s2(&alloc);
            stop_token        t1 = s1.get_token();
            stop_token        t2 = s2.get_token();
            ASSERT(&t1 == &(t1 = MoveUtil::move(t2)));
            ASSERT(s2.get_token() == t1);
            ASSERT(stop_token() == t2);
        }

        // P-5
        {
            stop_token t;
            ASSERT(&t == &(t = MoveUtil::move(t)));
            ASSERT(stop_token() == t);
        }

        // P-6
        {
            const stop_source s(&alloc);
            stop_token        t = s.get_token();
            ASSERT(&t == &(t = MoveUtil::move(t)));
            ASSERT(s.get_token() == t);
        }

        // P-7
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(
                  bslmf::Util::declval<stop_token&>() =
                      bslmf::Util::declval<bslmf::MovableRef<stop_token> >()));
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // 'stop_token' COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 The copy assignment operator of 'stop_token' makes the left
        //:   operand own the same stop state as the right operand,
        //:   relinquishes ownership of any stop state previously held by the
        //:   left operand, and returns a reference to the left operand.
        //:
        //: 2 The left and right operands can own stop states that were created
        //:   using different allocators.
        //:
        //: 3 The copy assignment operator of 'stop_token' is 'noexcept'.
        //
        // Plan:
        //: 1 Create two default-constructed 'stop_token's, 't1' and 't2'.
        //:   Evaluate 't1 = t2' and verify that the address of the result of
        //:   this expression is the address of 't1'.  Verify that 't1 == t2'.
        //:   (C-1)
        //:
        //: 2 Repeat step 1, but with a 't2' that owns a stop state that is
        //:   allocated using a specified test allocator (so we can verify that
        //:   all memory is deallocated by the end of the test).  (C-1)
        //:
        //: 3 Repeat step 1, but with a 't1' that owns a stop state.  (C-1)
        //:
        //: 4 Repeat step 2, but with a 't1' that owns a different stop state
        //:   from 't2'.  (C-1)
        //:
        //: 5 Repeat step 4, but with two stop states that were created using
        //:   different allocators.  (C-1,2)
        //:
        //: 6 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-3)
        //
        // Testing:
        //   stop_token& stop_token::operator=(const stop_token&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token' COPY ASSIGNMENT"
                            "\n============================\n");

        bslma::TestAllocator alloc("test", veryVeryVeryVerbose);

        // P-1
        {
            stop_token       t1;
            const stop_token t2;
            ASSERT(&t1 == &(t1 = t2));
            ASSERT(t1 == t2);
        }

        // P-2
        {
            stop_token       t1;
            stop_source      s2(&alloc);
            const stop_token t2 = s2.get_token();
            const stop_token t3 = t2;
            ASSERT(&t1 == &(t1 = t2));
            ASSERT(t1 == t2);
        }

        // P-3
        {
            stop_source      s1(&alloc);
            stop_token       t1 = s1.get_token();
            const stop_token t2;
            ASSERT(&t1 == &(t1 = t2));
            ASSERT(t1 == t2);
        }

        // P-4
        {
            stop_source      s1(&alloc);
            stop_source      s2(&alloc);
            stop_token       t1 = s1.get_token();
            const stop_token t2 = s2.get_token();
            const stop_token t3 = t2;
            ASSERT(&t1 == &(t1 = t2));
            ASSERT(t1 == t2);
        }

        // P-5
        {
            bslma::TestAllocator otherAlloc("other", veryVeryVeryVerbose);
            {
                stop_source      s1(&alloc);
                stop_source      s2(&otherAlloc);
                stop_token       t1 = s1.get_token();
                const stop_token t2 = s2.get_token();
                const stop_token t3 = t2;
                ASSERT(&t1 == &(t1 = t2));
                ASSERT(t1 == t2);
            }
        }

        // P-6
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(bslmf::Util::declval<stop_token&>() =
                                   bslmf::Util::declval<const stop_token&>()));
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'stop_source' ALLOCATOR CONSTRUCTOR
        //
        // Concerns:
        //: 1 'stop_source' has a constructor that accepts
        //:   'bsl::allocator<char>' and uses the given allocator to allocate
        //:   memory for the stop state.
        //
        // Plan:
        //: 1 Set the default allocator to a test allocator, 'a1'.  Create a
        //:   'stop_source' object using a second test allocator, 'a2'.  Verify
        //:   that memory was allocated by 'a2' and not by 'a1'.  (C-1)
        //
        // Testing:
        //   stop_source::stop_source(allocator<char>);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_source' ALLOCATOR CONSTRUCTOR"
                            "\n===================================\n");

        bslma::TestAllocator a1("a1", veryVeryVeryVerbose);
        bslma::TestAllocator a2("a2", veryVeryVeryVerbose);
        {
            bslma::DefaultAllocatorGuard guard(&a1);

            stop_source source(&a2);
            ASSERT(0 == a1.numAllocations());
            ASSERT(0  < a2.numAllocations());
        }
        ASSERT(0 == a2.numBlocksInUse());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'stop_token' MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The move constructor of 'stop_token' performs a move operation
        //:   and leaves the source object without ownership of a stop state.
        //:
        //: 2 The move constructor of 'stop_token' is 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_source' object, 's', using the default
        //:   constructor.  Create a 'stop_token', 't1', that is a copy of
        //:   's.get_token()'.  Create a 'stop_token', 't2', that is
        //:   move-constructed from 't1'.  Verify that 't1== stop_token()' and
        //:   't2 == s.get_token()'.  (C-1)
        //:
        //: 2 Create a 'stop_token', 't1', that doesn't own a stop state.
        //:   Move-construct a 'stop_token', 't2', from 't1'.  Verify that
        //:   't1' and 't2' both equal 'stop_token()'.
        //:
        //: 3 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   stop_token::stop_token(stop_token&&) noexcept;
        //   stop_token::stop_token(MovableRef<stop_token>) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token' MOVE CONSTRUCTOR"
                            "\n=============================\n");

        // P-1
        {
            stop_source       source;
            const stop_token& tr = source.get_token();
            stop_token        t1(tr);
            const stop_token  t2(MoveUtil::move(t1));
            ASSERT(source.get_token() == t2);
            ASSERT(stop_token() == t1);
        }

        // P-2
        {
            stop_token       t1;
            const stop_token t2(MoveUtil::move(t1));
            ASSERT(stop_token() == t1);
            ASSERT(stop_token() == t2);
        }

        // P-3
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(stop_token(
                     bslmf::Util::declval<bslmf::MovableRef<stop_token> >())));
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'stop_token' DEFAULT CONSTRUCTOR
        //
        // Concerns:
        //: 1 'stop_token' is default constructible.  The default constructor
        //:   creates a 'stop_token' object that doesn't own a stop state.
        //:
        //: 2 The default constructor of 'stop_token' is 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_token' object using the default constructor, and
        //:   verify that it compares equal with 's.get_token()' where 's' is a
        //:   'stop_source' that doesn't own a stop state.  (C-1)
        //:
        //: 2 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   stop_token::stop_token() noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token' DEFAULT CONSTRUCTOR"
                            "\n================================\n");

        const stop_source s(nostopstate);
        ASSERT(s.get_token() == stop_token());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(stop_token()));
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'stop_token' COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The copy constructor of 'stop_token' creates a 'stop_token' that
        //:   is equal to the original object.
        //:
        //: 2 The copy constructor of 'stop_token' is 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_source' object using the default constructor.
        //:   Create a 'stop_token', 't1', using 'get_token()'.  Create a
        //:   'stop_token', 't2', using the copy constructor, passing 't1' as
        //:   the argument.  Verify that 't1 == t2'.  (C-1)
        //:
        //: 2 Repeat step 1 with a 'stop_source' object that does not own a
        //:   stop state.  (C-2)
        //:
        //: 3 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   stop_token::stop_token(const stop_token&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token' COPY CONSTRUCTOR"
                            "\n=============================\n");

        // P-1
        {
            const stop_source source;
            const stop_token& t1 = source.get_token();
            const stop_token  t2 = t1;
            ASSERT(t1 == t2);
        }

        // P-2
        {
            const stop_source source(nostopstate);
            const stop_token& t1 = source.get_token();
            const stop_token  t2 = t1;
            ASSERT(t1 == t2);
        }

        // P-3
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(
              noexcept(stop_token(bslmf::Util::declval<const stop_token&>())));
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'stop_token' COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 'stop_token' can be compared with itself for equality.  If both
        //:   'stop_token' objects don't own a stop state or both 'stop_token'
        //:   objects own the same stop state, the result is 'true'; otherwise,
        //:   the result is 'false'.
        //:
        //: 2 'stop_token' supports the '!=' operator.  The result of applying
        //:   the '!=' operator is the logical negation of the result of
        //:   applying the '==' operator.
        //:
        //: 3 The '==' and '!=' operators are 'noexcept'.
        //
        // Plan:
        //: 1 Create three different 'stop_source' objects: 's0' and 's1',
        //:   which are default-constructed, and 's2', which does not own a
        //:   stop state.  Create 'stop_token' objects 't0', 't1', 't2', which
        //:   are obtained by calls to 'get_token' on 's0', 's1,' and, 's2',
        //:   respectively.  Create 'stop_token' objects 't3', 't4', and 't5',
        //:   which are initialized in the same way as 't0', 't1', and 't2',
        //:   respectively.  Verify that {'t0', 't3'}, {'t1', 't4'}, and
        //:   {'t2', 't5'} are equivalence classes of 'operator==' and that
        //:   'operator!=' returns the negated result.  (C-1)
        //:
        //: 2 Repeat step 1, but destroy the 'stop_source' objects prior to
        //:   performing the comparisons.  (C-1)
        //:
        //: 3 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-2)
        //
        // Testing:
        //   bool operator==(const stop_token&, const stop_token&) noexcept;
        //   bool operator!=(const stop_token&, const stop_token&) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token' COMPARISON OPERATORS"
                            "\n=================================\n");

        for (int destroy = 0; destroy <= 1; destroy++) {
            optional<stop_source> s0;
            optional<stop_source> s1;
            optional<stop_source> s2;

            s0.emplace();
            s1.emplace();
            s2.emplace(nostopstate);

            const stop_token& t0 = s0->get_token();
            const stop_token& t1 = s1->get_token();
            const stop_token& t2 = s2->get_token();
            const stop_token& t3 = s0->get_token();
            const stop_token& t4 = s1->get_token();
            const stop_token& t5 = s2->get_token();
            const stop_token *tokens[6] = {&t0, &t1, &t2, &t3, &t4, &t5};

            if (destroy) {
                s0.reset();
                s1.reset();
                s2.reset();
            }

            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 6; j++) {
                    if (veryVerbose) { T_ P_(destroy) P_(i) P(j) }

                    const bool expectEqual = (i == j) ||
                                             (i == j + 3) ||
                                             (j == i + 3);

                    ASSERT(expectEqual == (*tokens[i] == *tokens[j]));
                    ASSERT(expectEqual != (*tokens[i] != *tokens[j]));
                }
            }
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(bslmf::Util::declval<const stop_token&>() ==
                        bslmf::Util::declval<const stop_token&>()));
        ASSERT(noexcept(bslmf::Util::declval<const stop_token&>() !=
                        bslmf::Util::declval<const stop_token&>()));
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'stop_token::stop_possible'
        //
        // Concerns:
        //: 1 'stop_token::stop_possible' returns 'false' if the object does
        //:   not own a stop state.
        //:
        //: 2 'stop_token::stop_possible' returns 'true' if the object owns a
        //:   stop state on which a stop was requested.
        //:
        //: 3 'stop_token::stop_possible' returns 'true' if the object owns a
        //:   stop state that is also owned by a 'stop_source'.
        //:
        //: 4 Otherwise, 'stop_token::stop_possible' returns 'false'.
        //:
        //: 5 The 'stop_token::stop_possible' method is 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_source' object that does not own a stop state.
        //:   Create a 'stop_token' that is a copy of 'get_token()'.  Call
        //:   'stop_possible' on the 'stop_token' and verify that the result is
        //:   'false'.  (C-1)
        //:
        //: 2 Create a 'stop_source' object that owns a stop state.  Create a
        //:   'stop_token' that is a copy of 'get_token()'.  Call
        //:   'stop_possible' on the 'stop_token' and verify that the result is
        //:   'true'.  Call 'request_stop', then call 'stop_possible' on the
        //:   'stop_token' a second time and verify that the result is again
        //:   'true'.  Destroy the 'stop_source', and verify that a third call
        //:   to 'stop_token::stop_possible' also yields 'true'.  (C-2,3)
        //:
        //: 3 Create a 'stop_source' object that owns a stop state.  Create a
        //:   'stop_token' that is a copy of 'get_token()'.  Destroy the
        //:   'stop_source' object.  Call 'stop_possible' and verify that the
        //:   result is 'false'.  (C-4)
        //:
        //: 4 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-5)
        //
        // Testing:
        //   bool stop_token::stop_possible() const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_token::stop_possible'"
                            "\n===========================\n");

        // P-1
        {
            stop_source       source(nostopstate);
            const stop_token& token = source.get_token();
            ASSERT(!token.stop_possible());
        }

        // P-2
        {
            optional<stop_source> oSource;
            oSource.emplace();
            const stop_token& token = oSource->get_token();
            ASSERT(token.stop_possible());

            oSource->request_stop();
            ASSERT(token.stop_possible());

            oSource.reset();
            ASSERT(token.stop_possible());
        }

        // P-3
        {
            optional<stop_source> oSource;
            oSource.emplace();
            const stop_token& token = oSource->get_token();
            oSource.reset();
            ASSERT(!token.stop_possible());
        }

        // P-4
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(
                   bslmf::Util::declval<const stop_token&>().stop_possible()));
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'stop_requested'
        //
        // Concerns:
        //: 1 'stop_source::stop_requested' returns 'true' if the object owns a
        //:   stop state and a stop has been requested; 'false' otherwise.
        //:
        //: 2 'stop_token::stop_requested' returns 'true' if the object owns a
        //:   stop state and a stop has been requested using a 'stop_source'
        //:   object that owns the same stop state; 'false' otherwise.
        //:
        //: 3 The 'stop_requested' methods of 'stop_source' and 'stop_token'
        //:   are 'noexcept'.
        //
        // Plan:
        //: 1 Create a 'stop_source' object using the default constructor.
        //:   Create a 'stop_token' using 'get_token()'.  Call 'stop_requested'
        //:   and verify that the result is 'false' in both cases.  Call
        //:   'request_stop', then verify that 'stop_requested' returns 'true'
        //:   for both the 'stop_source' and 'stop_token'.  (C-1..2)
        //:
        //: 2 Repeat P-1 using a 'stop_source' object that does not own a stop
        //:   state.  In this case, verify that all four calls to
        //:   'stop_requested' return 'false'.  (C-1..2)
        //:
        //: 3 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-3)
        //
        // Testing:
        //   bool stop_source::stop_requested() const noexcept;
        //   bool stop_token::stop_requested() const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'stop_requested'"
                            "\n===============\n");

        // P-1
        {
            stop_source        mSource;
            const stop_source& source = mSource;
            const stop_token&  token = source.get_token();

            ASSERT(!source.stop_requested());
            ASSERT(!token.stop_requested());

            mSource.request_stop();

            ASSERT(source.stop_requested());
            ASSERT(token.stop_requested());
        }


        // P-2
        {
            stop_source        mSource(nostopstate);
            const stop_source& source = mSource;
            const stop_token&  token = source.get_token();

            ASSERT(!source.stop_requested());
            ASSERT(!token.stop_requested());

            mSource.request_stop();

            ASSERT(!source.stop_requested());
            ASSERT(!token.stop_requested());
        }

        // P-3
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(
                 bslmf::Util::declval<const stop_source&>().stop_requested()));
        ASSERT(noexcept(
                  bslmf::Util::declval<const stop_token&>().stop_requested()));
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BOOTSTRAP TEST
        //   This case tests a minimal set of methods that cannot be validated
        //   without calling other methods in the set because their
        //   postconditions are expressed in terms of those other methods,
        //   implying that the methods depended upon must be validated at the
        //   same time.  These dependencies are explained in detail below.
        //   Note that we do not yet test the destructor of 'stop_callback',
        //   instead choosing to leak the 'stop_callback' objects created here.
        //   Also note that we will only validate a particular special case of
        //   'stop_callback' construction here, leaving full validation of each
        //   constructor signature to a higher-numbered test case.
        //
        // Concerns:
        //: 1 The default constructor for 'stop_source' creates an object that
        //:   refers to a stop state ('stop_possible' returns 'true') that is
        //:   distinct from any other existing stop state (a stop request on
        //:   one does not affect the other).  Memory is allocated using the
        //:   currently installed default allocator.
        //:
        //: 2 The 'nostopstate' object exists and has type
        //:   'const nostopstate_t'.
        //:
        //: 3 The 'nostopstate_t' constructor for 'stop_source' creates an
        //:   object that does not refer to a stop state ('stop_possible'
        //:   returns 'false').  No memory is allocated.
        //:
        //: 4 For a 'stop_source' object that owns a stop state,
        //:   'stop_possible' returns 'true' both before and after
        //:   'request_stop' is called.
        //:
        //: 5 For a 'stop_source' object that doesn't own a stop state,
        //:   'stop_possible' returns 'false' both before and after
        //:   'request_stop' is called.
        //:
        //: 6 'request_stop' can be called on a 'stop_source' object that owns
        //:   a stop state.  The first such call returns 'true' and executes
        //:   all stop callbacks registered to that stop state before
        //:   returning; a second call returns 'false'.
        //:
        //: 7 'request_stop' can be called on a 'stop_source' object that
        //:   doesn't own a stop state.  The return value is 'false'.
        //:
        //: 8 'get_token' can be called on a 'stop_source' object that owns a
        //:   stop state or a 'stop_source' object that doesn't own a stop
        //:   state.  In each case, the returned 'stop_token' owns the same
        //:   stop state as the 'stop_source' (if any).  No memory is allocated
        //:   to construct the 'stop_token'.
        //:
        //: 9 'stop_callback<F>' can be constructed from a 'stop_token' lvalue
        //:   and an 'F' lvalue.  If the 'stop_token' owns a stop state for
        //:   which 'stop_requested' is 'true', the callback is invoked before
        //:   the constructor returns; if the 'stop_token' owns a stop state
        //:   for which 'stop_requested' is 'false', the callback is registered
        //:   with that stop state.
        //:
        //:10 A stop state that is owned by one or more 'stop_source' or
        //:   'stop_token' objects is deallocated when the last 'stop_source'
        //:   or 'stop_token' object referring to it is destroyed.
        //:
        //:11 Methods indicated as 'noexcept' below are actually 'noexcept'.
        //
        // Plan:
        //: 1 Set the default allocator to a test allocator.  Create a
        //:   'stop_source' object using the default constructor and verify
        //:   that memory was allocated from the test allocator.  Verify that
        //:   'stop_possible' returns 'true', then call 'request_stop' and
        //:   verify that 'stop_possible' still returns 'true'.  Finally,
        //:   verify that memory does not remain allocated after the object is
        //:   destroyed.  (C-1,4)
        //:
        //: 2 Check that '&nostopstate' has type 'const nostopstate_t*'.  (C-2)
        //:
        //: 3 Create a 'stop_source' object by passing 'nostopstate' as the
        //:   first (and only) constructor argument.  Call 'stop_possible', and
        //:   verify that it returns 'false'.  Create a 'stop_callback' object,
        //:   'c', using the 'const stop_token&' constructor with a token
        //:   obtained by calling 'get_token'.  Call 'request_stop', and verify
        //:   that it returns 'false'.  Call 'stop_possible' a second time on
        //:   the 'stop_source', and verify that it returns 'false'.  Verify
        //:   that the callback was not invoked, and that no memory was
        //:   allocated.  (C-3,5,7..9)
        //:
        //: 4 Set the default allocator to a test allocator.  Create two
        //:   'stop_source' objects, 's1' and 's2', using the default
        //:   constructor; record the number of allocations requested thus far.
        //:   Create a 'stop_callback' object, 'c1', using the
        //:   'const stop_token&' constructor with 's1.get_token()'.  Call
        //:   's2.request_stop()', and verify that it returns 'true'; then call
        //:   it a second time and verify that it returns 'false'.  Verify that
        //:   'c1' has not been invoked.  Call 's1.request_stop()', then verify
        //:   that 'c1' has been invoked.  Create a second callback, 'c2',
        //:   again using 's1.get_token()', and verify that 'c2' is invoked
        //:   before the constructor returns.  Call 's1.request_stop()' a
        //:   second time, and verify that it returns 'false' and that 'c1' is
        //:   not invoked a second time.  Verify that no memory was allocated
        //:   after the construction of 's1' and 's2'.  (C-1,6,8,9)
        //:
        //: 5 Construct a 'stop_source' object using the default constructor,
        //:   and using a reference, extend the lifetime of a 'stop_token'
        //:   object returned by 'get_token()'.  Destroy the 'stop_source'
        //:   object and verify that memory remains allocated.  Then destroy
        //:   the 'stop_token' object and verify that no memory remains
        //:   allocated.  (C-10)
        //:
        //: 6 Repeat P-5, but destroy the two objects in the other order.
        //:   (C-10)
        //:
        //: 7 Apply the 'noexcept' operator to appropriate expressions and
        //:   verify that the result is 'true' in each case.  (C-11)
        //
        // TESTING:
        //   inline constexpr nostopstate_t nostopstate;
        //   stop_source::stop_source(nostopstate_t) noexcept;
        //   stop_source::stop_source();
        //   stop_source::~stop_source();
        //   bool stop_source::stop_possible() const noexcept;
        //   stop_token stop_source::get_token() const noexcept;
        //   bool stop_source::request_stop() noexcept;
        //   stop_token::~stop_token();
        // --------------------------------------------------------------------

        if (verbose) printf("\nBOOTSTRAP TEST"
                            "\n==============\n");
        // P-1
        {
            bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&testAllocator);

                stop_source        mSource;
                const stop_source& source = mSource;
                ASSERT(0 < testAllocator.numAllocations());
                ASSERT(source.stop_possible());
                mSource.request_stop();
                ASSERT(source.stop_possible());
            }
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        // P-2
        ASSERT(test_case_2::is_nostopstate_t_const_pointer(&nostopstate));

        // P-3
        {
            // We aren't testing the destructor of 'stop_callback' yet, so the
            // stop state will be leaked when this allocator is destroyed.
            // However, the memory will be cleaned up when 'buf' goes out of
            // scope.
            char                   buf[k_BUFFER_SIZE];
            bslma::BufferAllocator bufferAllocator(buf,
                                                   k_BUFFER_SIZE,
                                                   assertOnOOM);
            bslma::TestAllocator   testAllocator("test",
                                                 veryVeryVeryVerbose,
                                                 &bufferAllocator);
            testAllocator.setQuiet(true);
            bslma::DefaultAllocatorGuard guard(&testAllocator);

            stop_source        mSource(nostopstate);
            const stop_source& source = mSource;
            ASSERT(!source.stop_possible());

            bsls::AtomicInt       numInvocations;
            const GenericCallback c(&numInvocations);

            bsls::ObjectBuffer<stop_callback<GenericCallback> > cBuf;
            new (cBuf.buffer()) stop_callback<GenericCallback>(
                            static_cast<const stop_token&>(source.get_token()),
                            c);

            ASSERT(!mSource.request_stop());
            ASSERT(!source.stop_possible());
            ASSERT(0 == numInvocations.load());

            ASSERT(0 == testAllocator.numAllocations());
        }

        // P-4
        {
            // We aren't testing the destructor of 'stop_callback' yet, so the
            // stop state will be leaked when this allocator is destroyed.
            // However, the memory will be cleaned up when 'buf' goes out of
            // scope.
            char                   buf[k_BUFFER_SIZE];
            bslma::BufferAllocator bufferAllocator(buf,
                                                   k_BUFFER_SIZE,
                                                   assertOnOOM);
            bslma::TestAllocator   testAllocator("test",
                                                 veryVeryVeryVerbose,
                                                 &bufferAllocator);
            testAllocator.setQuiet(true);
            bslma::DefaultAllocatorGuard guard(&testAllocator);

            stop_source s1;
            stop_source s2;

            const bsls::Types::Int64 numAllocations =
                                                testAllocator.numAllocations();
            ASSERT(0 < numAllocations);

            const stop_token& t1 = s1.get_token();
            const stop_token& t2 = s2.get_token();

            bsls::AtomicInt       count1;
            bsls::AtomicInt       count2;
            const GenericCallback c1(&count1);
            const GenericCallback c2(&count2);

            bsls::ObjectBuffer<stop_callback<GenericCallback> > cbBuf1;
            bsls::ObjectBuffer<stop_callback<GenericCallback> > cbBuf2;
            new (cbBuf1.buffer()) stop_callback<GenericCallback>(t1, c1);
            ASSERT(0 == count1.load());

            ASSERT(s2.request_stop());
            ASSERT(!s2.request_stop());
            ASSERT(0 == count1.load());

            ASSERT(s1.request_stop());
            ASSERT(1 == count1.load());
            new (cbBuf2.buffer()) stop_callback<GenericCallback>(t2, c2);
            ASSERT(1 == count2.load());
            ASSERT(!s1.request_stop());

            ASSERT(numAllocations == testAllocator.numAllocations());
        }

        // P-5
        {
            bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&testAllocator);

                optional<stop_source> oSource;
                oSource.emplace();
                const stop_token& token = oSource->get_token();
                ASSERT(0 < testAllocator.numAllocations());
                oSource.reset();
                ASSERT(0 < testAllocator.numBlocksInUse());
            }
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        // P-6
        {
            bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&testAllocator);

                stop_source source;
                {
                    const stop_token& token = source.get_token();
                    ASSERT(0 < testAllocator.numAllocations());
                }
                ASSERT(0 < testAllocator.numBlocksInUse());
            }
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        // P-7
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(stop_source(nostopstate)));
        ASSERT(noexcept(
                  bslmf::Util::declval<const stop_source&>().stop_possible()));
        ASSERT(
             noexcept(bslmf::Util::declval<const stop_source&>().get_token()));
        ASSERT(noexcept(bslmf::Util::declval<stop_source&>().request_stop()));
        ASSERT(
              noexcept(stop_token(bslmf::Util::declval<const stop_token&>())));
#endif
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
        //: 1 Create a 'stop_source' object and obtain a 'stop_token' from it,
        //:   then create a 'stop_callback' object from that 'stop_token'.
        //:   Verify that the 'stop_source' and 'stop_token' have the
        //:   appropriate properties before and after calling 'request_stop',
        //:   and that the 'stop_token' callback is invoked.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        stop_source              source;
        stop_token               token = source.get_token();
        stop_callback<void(*)()> callback(token, test_case_1::callback);

        ASSERT(!source.stop_requested());
        ASSERT(source.stop_possible());
        ASSERT(!token.stop_requested());
        ASSERT(token.stop_possible());
        ASSERT(!test_case_1::invoked);

        ASSERT(source.request_stop());

        ASSERT(source.stop_requested());
        ASSERT(source.stop_possible());
        ASSERT(token.stop_requested());
        ASSERT(token.stop_possible());
        ASSERT(test_case_1::invoked);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
