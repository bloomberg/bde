// bsls_performancehint.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#define INCLUDED_BSLS_PERFORMANCEHINT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide performance hints for code optimization.
//
//@CLASSES:
//  bsls::PerformanceHint: namespace for performance optimization hints
//
//@MACROS:
//  BSLS_PERFORMANCEHINT_PREDICT_LIKELY(X): 'X' probably evaluates to non-zero
//  BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(X): 'X' probably evaluates to zero
//  BSLS_PERFORMANCEHINT_PREDICT_EXPECT(X, Y): 'X' probably evaluates to 'Y'
//  BSLS_PERFORMANCEHINT_UNLIKELY_HINT: annotate block unlikely to be taken
//  BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE: prevent compiler optimizations
//
//@DESCRIPTION: This component provides performance hints for the compiler or
// hardware.  There are currently two types of hints that are supported:
//: o branch prediction
//: o data cache prefetching
//
///Branch Prediction
///-----------------
// The three macros provided, 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY',
// 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY', and
// 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT', can be used to optimize compiler
// generated code for branch prediction.  The compiler, when given the hint
// under *optimized* mode (i.e., with 'BDE_BUILD_TARGET_OPT' defined) will
// rearrange the assembly instructions it generates to minimize the number of
// jumps needed.
//
// The following describes the macros provided by this component:
//..
//                Macro Name                          Description of Macro
// ----------------------------------------       -----------------------------
// BSLS_PERFORMANCEHINT_PREDICT_LIKELY(expr)      Hint to the compiler that the
//                                                specified *integral* 'expr'
//                                                expression is likely to
//                                                evaluate to non-zero.
//                                                Returns 'true' or 'false'
//                                                depending on the result of
//                                                the expression.
//
// BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(expr)    Hint to the compiler that the
//                                                specified *integral* 'expr'
//                                                expression is likely to
//                                                evaluate to zero.  Returns
//                                                'true' or 'false' depending
//                                                on the result of the
//                                                expression.
//
// BSLS_PERFORMANCEHINT_PREDICT_EXPECT(expr, value)
//                                                Hint to the compiler that the
//                                                specified *integral* 'expr'
//                                                expression is likely to
//                                                evaluate to the specified
//                                                'value'.  Returns the result
//                                                of the expression.
//
// BSLS_PERFORMANCEHINT_UNLIKELY_HINT             Hint to the compiler that the
//                                                block which contains the hint
//                                                is unlikely chosen.  Use this
//                                                in conjunction with the
//                                                'PREDICT_UNLIKELY' clause for
//                                                maximum portability.
//..
//
///Warning
///- - - -
// Please use the macros provided in this component *with* *caution*.  Always
// profile your code to get an idea of actual usage before attempting to
// optimize with these macros.  Furthermore, these macros are merely *hints* to
// the compiler.  Whether or not they will have visible effect on performance
// is not guaranteed.  Note that one can perform similar optimization with a
// profile-based compilation.  When compiled with the proper options, the
// compiler can collect usage information of the code, and such information can
// then be passed back to recompile the code in a more optimized form.  Please
// refer to the compiler manual for more information.
//
///Limitations
///- - - - - -
// There is a bug in gcc 4.2, 4.3, and 4.4 such that when using the branch
// prediction macros with multiple conditions, the generated code might not be
// properly optimized.  For example:
//..
//  if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(a && b)) {
//      // ...
//  }
//..
// The work-around is simply to split the conditions:
//..
//  if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(a)
//   && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(b)) {
//      // ...
//  }
//..
// This applies to all of the "likely", "unlikely", and "expect" macros defined
// in this component.  Note that a bug report has been filed:
//..
//  http://gcc.gnu.org/bugzilla/show_bug.cgi?id=42233
//..
//
///Data Cache Prefetching
///----------------------
// The two functions provided in the 'bsls::PerformanceHint' 'struct' are
// 'prefetchForReading' and 'prefetchForWriting'.  Use of these functions will
// cause the compiler to generate prefetch instructions to prefetch one cache
// line worth of data at the specified address into the cache line to minimize
// processor stalls.
//..
//       Function Name                       Description of Function
//  ------------------------         ------------------------------------------
//  prefetchForReading(address)      Prefetches one cache line worth of data at
//                                   the specified 'address' for reading.
//
//  prefetchForWriting(address)      Prefetches one cache line worth of data at
//                                   the specified 'address' for writing.
//..
//
///Warning
///- - - -
// These functions must be used *with* *caution*.  Inappropriate use of these
// functions degrades performance.  Note that there should be sufficient time
// for the prefetch instruction to finish before the specified address is
// accessed, otherwise prefetching will be pointless.  A profiler should be
// used to understand the program's behavior before attempting to optimize with
// these functions.
//
///Optimization Fence
///------------------
// The macro 'BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE' prevents some compiler
// optimizations, particularly compiler instruction reordering.  This fence
// does *not* map to a CPU instruction and has no impact on processor
// instruction re-ordering, and therefore should not be used to synchronize
// memory between threads.  The fence may be useful in unusual contexts, like
// performing benchmarks, or working around bugs identified in the compiler's
// optimizer.
//
///Warning
///- - - -
// This macro should be used *with* *caution*.  The macro will generally
// decrease the performance of code on which it is applied, and is not
// implemented on all platforms.
//
///Usage
///-----
// The following series of examples illustrates use of the macros and functions
// provided by this component.
//
///Example 1: Using the Branch Prediction Macros
///- - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates the use of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY'
// and 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' to generate more efficient
// assembly instructions.  Note the use of 'BSLS_PERFORMANCEHINT_UNLIKELY_HINT'
// inside the 'if' branch for maximum portability.
//..
//  int global;
//
//  void foo()
//  {
//      global = 1;
//  }
//
//  void bar()
//  {
//      global = 2;
//  }
//
//  int main(int argc, char **argv)
//  {
//      argc = std::atoi(argv[1]);
//
//      for (int x = 0; x < argc; ++x) {
//          int y = std::rand() % 10;
//
//          // Correct usage of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' since
//          // there are nine of ten chance that this branch is taken.
//
//          if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(8 != y)) {
//              foo();
//          }
//          else {
//              BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
//              bar();
//          }
//      }
//      return 0;
//  }
//..
// An excerpt of the assembly code generated using 'xlC' Version 10 on AIX from
// this small program is:
//..
//  b8:   2c 00 00 08     cmpwi   r0,8
//  bc:   41 82 00 38     beq-    f4 <.main+0xb4>
//                         ^
//                         Note that if register r0 (y) equals 8, branch to
//                         instruction f4 (a jump).  The '-' after 'beq'
//                         indicates that the branch is unlikely to be taken.
//                         The predicted code path continues the 'if'
//                         statement, which calls 'foo' below.
//
//  c0:   4b ff ff 41     bl      0 <.foo__Fv>
//  ...
//  f4:   4b ff ff 2d     bl      20 <.bar__Fv>
//..
// Now, if 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' is changed to
// 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY', and the
// 'BSLS_PERFORMANCEHINT_UNLIKELY_HINT' is moved to the first branch, the
// following assembly code will be generated:
//..
//  b8:   2c 00 00 08     cmpwi   r0,8
//  bc:   40 c2 00 38     bne-    f4 <.main+0xb4>
//                         ^
//                         Note that the test became a "branch not equal"
//                         test.  The predicted code path now continues to the
//                         'else' statement, which calls 'bar' below.
//
//  c0:   4b ff ff 61     bl      20 <.bar__Fv>
//  ...
//  f4:   4b ff ff 0d     bl      0 <.foo__Fv>
//..
// A timing analysis shows that effective use of branch prediction can have a
// material effect on code efficiency:
//..
//  $time ./unlikely.out 100000000
//
//  real    0m2.022s
//  user    0m2.010s
//  sys     0m0.013s
//
//  $time ./likely.out 100000000
//
//  real    0m2.159s
//  user    0m2.149s
//  sys     0m0.005s
//..
//
///Example 2: Using 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is essentially the same as the '__builtin_expect(expr, value)'
// macro that is provided by some compilers.  This macro allows the user to
// define more complex hints to the compiler, such as the optimization of
// 'switch' statements.  For example, given:
//..
//  int x = std::rand() % 4;
//..
// the following is incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT',
// since the probability of getting a 3 is equivalent to the other
// possibilities ( 0, 1, 2 ):
//..
//  switch (BSLS_PERFORMANCEHINT_PREDICT_EXPECT(x, 3)) {
//    case 1: //..
//            break;
//    case 2: //..
//            break;
//    case 3: //..
//            break;
//    default: break;
//  }
//..
// However, this is sufficient to illustrate the intent of this macro.
//
///Example 3: Cache Line Prefetching
///- - - - - - - - - - - - - - - - -
// The following demonstrates use of 'prefetchForReading' and
// 'prefetchForWriting' to prefetch data cache lines:
//..
//  const int SIZE = 10 * 1024 * 1024;
//
//  void add(int *arrayA, int *arrayB)
//  {
//      for (int i = 0; i < SIZE / 8; ++i){
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//      }
//  }
//
//  int array1[SIZE];
//  int array2[SIZE];
//
//  int main()
//  {
//      BloombergLP::bsls::Stopwatch timer;
//      timer.start();
//      for (int i = 0; i < 10; ++i) {
//          add(array1, array2);
//      }
//      std::printf("time: %f\n", timer.elapsedTime());
//      return 0;
//  }
//..
// The above code simply adds two arrays together multiple times.  Using
// 'bsls::Stopwatch', we recorded the running time and printed it to 'stdout':
//..
//  $./prefetch.sundev1.tsk
//  time: 8.446806
//..
// Now, we can observe that in the 'add' function, 'arrayA' and 'arrayB' are
// accessed sequentially for the majority of the program.  'arrayA' is used for
// writing and 'arrayB' is used for reading.  Making use of prefetch, we add
// calls to 'prefetchForReading' and 'prefetchForWriting':
//..
//  void add(int *arrayA, int *arrayB)
//  {
//      for (int i = 0; i < SIZE / 8; ++i){
//          bsls::PerformanceHint::prefetchForWriting((int *) arrayA + 16);
//          bsls::PerformanceHint::prefetchForReading((int *) arrayB + 16);
//
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//          *(arrayA++) = *arrayA + *(arrayB++);
//      }
//  }
//..
// Adding the prefetch improves the program's efficiency:
//..
//  $./prefetch.sundev1.tsk
//  time: 6.442100
//..
// Note that we prefetch the address '16 * sizeof(int)' bytes away from
// 'arrayA'.  This is such that the prefetch instruction has sufficient time to
// finish before the data is actually accessed.  To see the difference, if we
// changed '+ 16' to '+ 4':
//..
//  $./prefetch.sundev1.tsk
//  time: 6.835928
//..
// And we get less of an improvement in speed.  Similarly, if we prefetch too
// far away from the data use, the data might be removed from the cache before
// it is looked at and the prefetch is wasted.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)

#ifndef INCLUDED_BUILTINS
#include <builtins.h>      // for 'dcbt', '__builtin_expect'
#define INCLUDED_BUILTINS
#endif

#endif

#if defined(BSLS_PLATFORM_CMP_HP)

#ifndef INCLUDED_MACHINE_SYS_BUILTINS
#include <machine/sys/builtins.h>
#define INCLUDED_MACHINE_SYS_BUILTINS
#endif

#ifndef INCLUDED_MACHINE_SYS_INLINE
#include <machine/sys/inline.h>
#define INCLUDED_MACHINE_SYS_INLINE
#endif

#endif

#if defined(BSLS_PLATFORM_CMP_SUN)

#ifndef INCLUDED_SUN_PREFETCH
#include <sun_prefetch.h>  // for 'sparc_prefetch_write|read_many'
#define INCLUDED_SUN_PREFETCH
#endif

#if BSLS_PLATFORM_CMP_VERSION >= 0x5110
#ifndef INCLUDED_SUN_MBARRIER
#include <mbarrier.h>
#define INCLUDED_SUN_MBARRIER
#endif
#endif

#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)

#ifndef INCLUDED_XMMINTRIN
#include <xmmintrin.h>     // for '_mm_prefetch', '_MM_HINT_T0'
#define INCLUDED_XMMINTRIN
#endif

#ifndef INCLUDED_INTRIN
#include <intrin.h>
#define INCLUDED_INTRIN
#endif

#endif

namespace BloombergLP {

                        // ============================
                        // BSLS_PERFORMANCEHINT_PREDICT
                        // ============================

// These macros are effective in *optimized* mode only, and *only* on platforms
// that support '__builtin_expect'.

#if defined(BDE_BUILD_TARGET_OPT) &&                                          \
   (defined(BSLS_PLATFORM_CMP_CLANG) ||                                       \
    defined(BSLS_PLATFORM_CMP_GNU)   ||                                       \
    (defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION >= 0x0900))

    #define BSLS_PERFORMANCEHINT_PREDICT_LIKELY(expr)                         \
                                              __builtin_expect(!!(expr), 1)
    #define BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(expr)                       \
                                              __builtin_expect((expr), 0)
    #define BSLS_PERFORMANCEHINT_PREDICT_EXPECT(expr, value)                  \
                                              __builtin_expect((expr), (value))
#else

    #define BSLS_PERFORMANCEHINT_PREDICT_LIKELY(expr)           (expr)
    #define BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(expr)         (expr)
    #define BSLS_PERFORMANCEHINT_PREDICT_EXPECT(expr, value)    (expr)

#endif

// Define the 'BSLS_PERFORMANCEHINT_HAS_ATTRIBUTE_COLD' and
// 'BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD' macros.

#if defined(BSLS_PLATFORM_CMP_CLANG)
    #if __has_attribute(cold)
    #define BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD  __attribute__((cold))
    #endif
#elif (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40300)
    #define BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD  __attribute__((cold))
#endif

#if !defined(BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD)
    #define BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD
#else
    #define BSLS_PERFORMANCEHINT_HAS_ATTRIBUTE_COLD 1
#endif


// Define the 'BSLS_PERFORMANCEHINT_UNLIKELY_HINT' macro.

#if defined(BDE_BUILD_TARGET_OPT) && defined(BSLS_PLATFORM_CMP_SUN)
    #define BSLS_PERFORMANCEHINT_UNLIKELY_HINT                                \
                             BloombergLP::bsls::PerformanceHint::rarelyCalled()
#elif defined(BDE_BUILD_TARGET_OPT) &&                                        \
   (defined(BSLS_PLATFORM_CMP_IBM) || BSLS_PERFORMANCEHINT_HAS_ATTRIBUTE_COLD)
    #define BSLS_PERFORMANCEHINT_UNLIKELY_HINT                                \
                             BloombergLP::bsls::PerformanceHint::lowFrequency()
#else
    #define BSLS_PERFORMANCEHINT_UNLIKELY_HINT
#endif

                        // =======================================
                        // BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
                        // =======================================


#if defined(BSLS_PLATFORM_CMP_IBM)

    #define BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE __fence()

#elif defined(BSLS_PLATFORM_CMP_MSVC)

    #pragma intrinsic(_ReadWriteBarrier)
    #define BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE _ReadWriteBarrier()

#elif defined(BSLS_PLATFORM_CMP_HP)

    #define BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE                           \
                             _Asm_sched_fence(_UP_MEM_FENCE|_DOWN_MEM_FENCE)

#elif (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5110)

    #define BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE __compiler_barrier()

#elif defined(BSLS_PLATFORM_CMP_GNU)                                          \
   || defined(BSLS_PLATFORM_CMP_CLANG)                                        \
   || (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5100)

    #define BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE asm volatile("":::"memory")

#else
    #error "BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE not implemented"
#endif

namespace bsls {

                        // ======================
                        // struct PerformanceHint
                        // ======================

struct PerformanceHint {
    // This 'struct' provides a namespace for a suite of functions that give
    // performance hints to the compiler or hardware.

    // CLASS METHODS
    static void prefetchForReading(const void *address);
        // Prefetch one cache line worth of data at the specified 'address' for
        // reading if the compiler built-in is available (see to the component
        // level document for limitations).  Otherwise this method has no
        // effect.

    static void prefetchForWriting(void *address);
        // Prefetch one cache line worth of data at the specified 'address' for
        // writing if the compiler built-in is available (see to the component
        // level document for limitations).  Otherwise this method has no
        // effect.

    static void rarelyCalled();
        // This is an empty function that is marked as rarely called using
        // pragmas.  If this function is placed in a block of code inside a
        // branch, the compiler will optimize the assembly code generated and
        // mark the block as unlikely.  Note that this function is
        // intentionally not inlined.

#if defined(BDE_BUILD_TARGET_OPT)
#if defined(BSLS_PLATFORM_CMP_SUN)

// Pragma to flag the function as rarely called.
#pragma rarely_called(rarelyCalled)

// Pragma to flag the function as no side effect.  This is necessary because a
// function that is marked as rarely called cannot be inlined without losing
// the 'rarely_called' characteristics.  When marked as no side effect, even an
// out-of-line function will not trigger a function call.
#pragma no_side_effect(rarelyCalled)

#endif  // BSLS_PLATFORM_CMP_SUN
#endif  // BDE_BUILD_TARGET_OPT

    BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD
    static void lowFrequency();
        // This is an empty function that is marked with low execution
        // frequency using pragmas.  If this function is placed in a block of
        // code inside a branch, the compiler will optimize the assembly code
        // generated and mark the block as unlikely.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // ----------------------
                        // struct PerformanceHint
                        // ----------------------

// CLASS METHODS
inline
void PerformanceHint::prefetchForReading(const void *address)
{
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

    __builtin_prefetch(address, 0);

#elif defined(BSLS_PLATFORM_CMP_IBM)

    __dcbt(const_cast<void *>(address));

#elif defined(BSLS_PLATFORM_CMP_SUN)

    sparc_prefetch_read_many(const_cast<void *>(address));

#elif defined(BSLS_PLATFORM_CMP_MSVC)

    _mm_prefetch(static_cast<const char*>(address), _MM_HINT_T0);
        // '_MM_HINT_T0' fetches data to all levels of cache.

#elif defined(BSLS_PLATFORM_CMP_HP)

    _Asm_lfetch(_LFTYPE_NONE, _LFHINT_NTA, address);

#else

    // no-op

#endif
}

inline
void PerformanceHint::prefetchForWriting(void *address)
{
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

    __builtin_prefetch(address, 1);

#elif (defined(BSLS_PLATFORM_CMP_IBM) &&                                     \
       BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900)

    __dcbtst(address);

#elif defined(BSLS_PLATFORM_CMP_SUN)

    sparc_prefetch_write_many(address);

#elif defined(BSLS_PLATFORM_CMP_MSVC)

    _mm_prefetch(static_cast<const char*>(address), _MM_HINT_T0);
        // '_MM_HINT_T0' fetches data to all levels of cache.

#elif defined(BSLS_PLATFORM_CMP_HP)

    _Asm_lfetch_excl(_LFTYPE_NONE, _LFHINT_NTA, address);

#else

    // no-op

#endif
}

// This function must be inlined for the pragma to take effect on the branch
// prediction in IBM xlC.

BSLS_PERFORMANCEHINT_ATTRIBUTE_COLD
inline
void PerformanceHint::lowFrequency()
{
#if defined(BDE_BUILD_TARGET_OPT) && defined(BSLS_PLATFORM_CMP_IBM)

#pragma execution_frequency(very_low)

#endif
}

}  // close package namespace


}  // close enterprise namespace

#endif

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
