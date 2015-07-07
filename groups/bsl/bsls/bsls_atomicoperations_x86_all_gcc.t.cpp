// bsls_atomicoperations_x86_all_gcc.t.cpp                            -*-C++-*-

#include <bsls_atomicoperations_x86_all_gcc.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(BSLS_PLATFORM_CPU_X86) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))

// For thread support
#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
typedef HANDLE thread_t;
#else
#include <pthread.h>
#include <unistd.h>
typedef pthread_t thread_t;
#endif

// For timer support
#ifdef BSLS_PLATFORM_OS_WINDOWS
    #include <sys/timeb.h> // ftime(struct timeb *)
#else
    #include <sys/time.h>
#endif


using namespace BloombergLP;

typedef void *(*thread_func)(void *arg);

typedef bsls::Atomic_TypeTraits<bsls::AtomicOperations_X86_ALL_GCC>::Int
                                                                   atomic_int;

struct thread_args
{
    atomic_int         *d_obj_p;
    bsls::Types::Int64  d_iterations;
    bsls::Types::Int64  d_runtimeMs;
};

bsls::Types::Int64 getTimerMs() {
    // It'd be nice to use TimeUtil here, but that would be a dependency
    // cycle.  Duplicating all the portable support for high-resolution
    // timing is more complication than is needed here.  We'll run enough
    // iterations that the basic portable lower-resolution timers will give
    // good results.
#if defined(BSLS_PLATFORM_OS_UNIX)
    timeval native;
    gettimeofday(&native, 0);
    return ((bsls::Types::Int64) native.tv_sec * 1000 +
            native.tv_usec / 1000);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    timeb t;
    ::ftime(&t);
    return static_cast<bsls::Types::Int64>(t.time) * 1000 + t.millitm;
#else
#error "Don't know how to get timer for this platform"
#endif
}

thread_t createThread(thread_func func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, 0);
#else
    thread_t thr;
    pthread_create(&thr, 0, func, arg);
    return thr;
#endif
}

void joinThread(thread_t thr)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(thr, INFINITE);
    CloseHandle(thr);
#else
    pthread_join(thr, 0);
#endif
}

struct atomic_get_mfence
{
    int operator()(atomic_int * obj)
    {
        int ret;

        asm volatile (
            "       mfence                  \n\t"
            "       movl %[obj], %[ret]     \n\t"

            : [ret] "=r" (ret)
            : [obj] "m"  (*obj)
            : "memory");

        return ret;
    }
};

struct atomic_get_free
{
    int operator()(const atomic_int * obj)
    {
        int ret;

        asm volatile (
            "       movl %[obj], %[ret]     \n\t"

            : [ret] "=r" (ret)
            : [obj] "m"  (*obj)
            : "memory");

        return ret;
    }
};

struct atomic_set_mfence
{
    void operator()(atomic_int * obj, int value)
    {
        asm volatile (
            "       movl %[val], %[obj]     \n\t"
            "       mfence                  \n\t"

            : [obj] "=m" (*obj)
            : [val] "r"  (value)
            : "memory");
    }
};

struct atomic_set_lock
{
    void operator()(atomic_int * obj, int value)
    {
        asm volatile (
            "       movl %[val], %[obj]     \n\t"
            "       lock addl $0, 0(%%esp)  \n\t"

            : [obj] "=m" (*obj)
            : [val] "r"  (value)
            : "memory", "cc");
    }
};

struct atomic_set_xchg
{
    void operator()(atomic_int * obj, int value)
    {
        asm volatile (
            "       xchgl %[obj], %[val]     \n\t"

            : [obj] "=m" (*obj)
            : [val] "r"  (value)
            : "memory");
    }
};

template <typename AtomicGet>
void * test_atomics_get_thread(void * args)
{
    thread_args * thr_args = reinterpret_cast<thread_args *>(args);
    atomic_int *obj = thr_args->d_obj_p;
    AtomicGet get_fun;

    bsls::Types::Int64 i;
    bsls::Types::Int64 start = getTimerMs();

    for (i = 0; get_fun(obj) != -1; ++i)
        ;

    thr_args->d_runtimeMs = getTimerMs() - start;

    thr_args->d_iterations = i;
    return 0;
}

template <typename AtomicGet>
void * test_atomics_set_thread(void * args)
{
    thread_args * thr_args = reinterpret_cast<thread_args *>(args);
    atomic_int *obj = thr_args->d_obj_p;
    AtomicGet set_fun;

    bsls::Types::Int64 start = getTimerMs();

    for (bsls::Types::Int64 i = thr_args->d_iterations; i > 0; --i)
        set_fun(obj, (int)i);

    // signal finish
    set_fun(obj, -1);

    thr_args->d_runtimeMs = getTimerMs() - start;
    return 0;
}

template <typename AtomicGet, typename AtomicSet>
void test_atomics(bsls::Types::Int64 iterations) {

    enum {
        NUM_READERS = 3
    };
    thread_t    readers[NUM_READERS];
    thread_args readerArgs[NUM_READERS];

    thread_args args;
    args.d_obj_p = new atomic_int;
    args.d_iterations = iterations;

    for (int i = 0 ; i < NUM_READERS; ++i) {
        readerArgs[i] = args;
        readers[i] = createThread(&test_atomics_get_thread<AtomicGet>,
                                  &readerArgs[i]);
    }
    thread_t writer;
    writer = createThread(&test_atomics_set_thread<AtomicSet>, &args);

    for (int i = 0; i < NUM_READERS; ++i) {
        joinThread(readers[i]);
    }
    joinThread(writer);

    bsls::Types::Int64 totalReadIter = 0, totalReadTime = 0;
    for (int i = 0; i < NUM_READERS; ++i) {
        totalReadIter += readerArgs[i].d_iterations;
        totalReadTime += readerArgs[i].d_runtimeMs;
    }
    static const double MS_PER_SEC = 1000;
    double readPerSec = (double)totalReadIter / totalReadTime * MS_PER_SEC;
    double writePerSec = (double)iterations / args.d_runtimeMs * MS_PER_SEC;

    printf(" %f" " / %f\n", readPerSec, writePerSec);

    delete args.d_obj_p;
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    switch (test) { case 0:
        return 0;

      case -1: {
          ////////////////////////////////////////////
          // Benchmark test
          //
          // Time get/set operations for several possible implementations
          // of get and set.  To simulate the typical reader/writer
          // configuration, run 1 "setter" thread and 3 "getter" threads.
          // The "setter" thread will run a fixed number of iterations and
          // the "getter" threads will run until they see a value indicating
          // the setter has stopped.
          ///////////////////////////////////////////
          enum {
              NUM_ITER = 30 * 1000 * 1000
          };
          printf("get mfence / set mfence: ");
          test_atomics<atomic_get_mfence, atomic_set_mfence>(NUM_ITER);

          printf("get free / set mfence: ");
          test_atomics<atomic_get_free, atomic_set_mfence>(NUM_ITER);

          printf("get free / set lock: ");
          test_atomics<atomic_get_free, atomic_set_lock>(NUM_ITER);

          printf("get free / set xchg: ");
          test_atomics<atomic_get_free, atomic_set_xchg>(NUM_ITER);


      } break;

      default:
        return -1;
    }
}

#else // BSLS_PLATFORM_CPU_X86 &&
      // (BSLS_PLATFORM_CMP_GNU || BSLS_PLATFORM_CMP_CLANG)

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    switch (test) { case 0:
        return 0;
      default:
        return -1;
    }
}

#endif // BSLS_PLATFORM_CPU_X86 &&
       // (BSLS_PLATFORM_CMP_GNU || BSLS_PLATFORM_CMP_CLANG)

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
