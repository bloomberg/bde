// bslmt_readerwritermutexassert.h                                    -*-C++-*-

#ifndef INCLUDED_BSLMT_READERWRITERMUTEXASSERT
#define INCLUDED_BSLMT_READERWRITERMUTEXASSERT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an assert macro for verifying reader-writer mutex status.
//
//@CLASSES:
//
//@MACROS:
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED:            test in non-opt modes
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE:       test in safe    mode
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT:        test in all     modes
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ:       test in non-opt modes
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE:  test in safe    mode
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT:   test in all     modes
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE:      test in non-opt modes
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE: test in safe    mode
//  BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT:  test in all     modes
//
//@SEE_ALSO: bslmt_mutexassert bslmt_readerwritermutex
//
//@DESCRIPTION: This component provides macros for asserting that a
// reader-writer mutex is locked.  It does not distinguish between locks held
// by the current thread or other threads.  If the macro is active in the
// current build mode, when the macro is called, if the supplied mutex is
// unlocked, the assert handler installed for 'BSLS_ASSERT' will be called.
// The assert handler installed by default will report an error and abort the
// task.
//
// The nine macros defined by the component are analogous to the macros defined
// by 'BSLS_ASSERT':
//..
//  +----------------------------------------------------+------------------=+
//  | Macro                                              | When Active       |
//  +====================================================+===================+
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED'           | When              |
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ'      | 'BSLS_ASSERT'     |
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE'     | is active.        |
//  +----------------------------------------------------+-------------------+
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE'      | When              |
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE' | 'BSLS_ASSERT_SAFE'|
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE'| is active.        |
//  +----------------------------------------------------+-------------------+
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT'       | When              |
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT'  | 'BSLS_ASSERT_OPT' |
//  |'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT' | is active.        |
//  +----------------------------------------------------+-------------------+
//..
// In build modes where any one of these macros is not active, the presence of
// the macros has no effect.
//
// If any of these asserts are in effect and fail (because the reader-writer
// mutex in question was unlocked), the behavior parallels the behavior of the
// assertion macros defined in 'bsls_assert.h' -- 'bsls::Assert::invokeHandler'
// is called, with a source code expression, the name of the source file, and
// the line number in the source file where the macro was called.  If the
// default handler is installed, this will result in an error message and an
// abort.
//
///Caveat: False Positives
///-----------------------
// Preconditions on locks typically require that the lock exist *and* *is*
// *held* *by* *the* *calling* *thread*.  Unfortunately, lock ownership is not
// recorded in the lock and cannot be confirmed.  The absence of any lock when
// the calling thread should hold one is certainly a problem; however, the
// existence of a lock does not guarantee that the complete precondition is
// met.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Checking Consistency Within a Private Method
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Clone from TD.

#include <bslscm_version.h>

#include <bslmt_readerwritermutex.h>

#include <bsls_assert.h>

#include <cstring>  // 'native_std::strcmp'

// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(mutex_p) do {             \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLocked(                   \
                     (mutex_p),                                               \
                     "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(" #mutex_p ")", \
                     __FILE__,                                                \
                     __LINE__,                                                \
                     bsls::Assert::k_LEVEL_ASSERT); } while (false)
#else
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
     #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(mutex_p) do {       \
         bslmt::ReaderWriterMutexAssert_Imp::assertIsLocked(                  \
                (mutex_p),                                                    \
                "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(" #mutex_p ")", \
                __FILE__,                                                     \
                __LINE__,                                                     \
                bsls::Assert::k_LEVEL_SAFE); } while (false)
#else
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(mutex_p) do {         \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLocked(                   \
                 (mutex_p),                                                   \
                 "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(" #mutex_p ")", \
                 __FILE__,                                                    \
                 __LINE__,                                                    \
                  bsls::Assert::k_LEVEL_OPT); } while (false)
#else
    #define BSLMT_MUTEXASSERT_IS_LOCKED_OPT(mutex_p) ((void) 0)
#endif

// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(mutex_p) do {        \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLockedRead(               \
                (mutex_p),                                                    \
                "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(" #mutex_p ")", \
                __FILE__,                                                     \
                __LINE__,                                                     \
                bsls::Assert::k_LEVEL_ASSERT); } while (false)
#else
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(mutex_p) do {   \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLockedRead(               \
           (mutex_p),                                                         \
           "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(" #mutex_p ")", \
           __FILE__,                                                          \
           __LINE__,                                                          \
           bsls::Assert::k_LEVEL_SAFE); } while (false)
#else
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(mutex_p)        \
                                                                     ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT(mutex_p) do {    \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLockedRead(               \
            (mutex_p),                                                        \
            "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT(" #mutex_p ")", \
            __FILE__,                                                         \
            __LINE__,                                                         \
           bsls::Assert::k_LEVEL_OPT); } while (false)
#else
    #define BSLMT_MUTEXASSERT_IS_LOCKED_READ_OPT(mutex_p) ((void) 0)
#endif

// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(mutex_p) do {       \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLockedWrite(              \
              (mutex_p),                                                      \
              "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(" #mutex_p ")",  \
              __FILE__,                                                       \
              __LINE__,                                                       \
              bsls::Assert::k_LEVEL_ASSERT); } while (false)
#else
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(mutex_p) do {  \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLockedWrite(              \
         (mutex_p),                                                           \
         "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(" #mutex_p ")",  \
           __FILE__,                                                          \
           __LINE__,                                                          \
           bsls::Assert::k_LEVEL_SAFE); } while (false)
#else
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(mutex_p)      \
                                                                     ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT(mutex_p) do {   \
        bslmt::ReaderWriterMutexAssert_Imp::assertIsLockedWrite(              \
          (mutex_p),                                                          \
          "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT(" #mutex_p ")",  \
          __FILE__,                                                           \
          __LINE__,                                                           \
          bsls::Assert::k_LEVEL_OPT); } while (false)
#else
    #define BSLMT_MUTEXASSERT_IS_LOCKED_WRITE_OPT(mutex_p) ((void) 0)
#endif

// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslmt {

                          // =================================
                          // class ReaderWriterMutexAssert_Imp
                          // =================================

struct ReaderWriterMutexAssert_Imp {
    // This 'struct' provides a (component private) namespace for
    // implementation functions of the assert macros defined in this component.
    // This class should *not* be used directly in client code.

    // CLASS METHODS
    static void assertIsLocked(ReaderWriterMutex *rwMutex,
                               const char        *text,
                               const char        *file,
                               int                line,
                               const char        *level);
        // If the specified 'rwMutex' is not locked (i.e., neither a read lock
        // or a write lock), call 'bsls::Assert::invokeHandler' with the
        // specified 'text', 'file', 'line', and 'level', where 'text' is text
        // describing the assertion being performed, 'file' is the name of the
        // source file that called the macro, 'line' is the line number in the
        // file where the macro was called, and 'level' is one of the
        // 'bslsAssert::k_LEVEL_*' string literals.  This function is intended
        // to implement 'BSLMT_READERWRITEMUTEXASSERT_IS_LOCKED',
        // 'BSLMT_READERWRITEMUTEXASSERT_IS_LOCKED_SAFE', and
        // 'BSLMT_READERWRITEMUTEXASSERT_IS_LOCKED_OPT' and should not
        // otherwise be called directly.

    static void assertIsLockedRead(ReaderWriterMutex *rwMutex,
                                   const char        *text,
                                   const char        *file,
                                   int                line,
                                   const char        *level);
        // If the specified 'rwMutex' is not locked for reading, call
        // 'bsls::Assert::invokeHandler' with the specified 'text', 'file',
        // 'line', and 'level', where 'text' is text describing the assertion
        // being performed, 'file' is the name of the source file that called
        // the macro, 'line' is the line number in the file where the macro was
        // called, and 'level' is one of the 'bslsAssert::k_LEVEL_*' string
        // literals.  This function is intended to implement
        // 'BSLMT_READERWRITEMUTEXASSERT_IS_LOCKED_READ',
        // 'BSLMT_READERWRITEMUTEXASSERT_IS_LOCKED_READ_SAFE', and
        // 'BSLMT_READERWRITEMUTEXASSERT_IS_LOCKED_READ_OPT' and should not
        // otherwise be called directly.

    static void assertIsLockedWrite(ReaderWriterMutex *rwMutex,
                                    const char        *text,
                                    const char        *file,
                                    int                line,
                                    const char        *level);
        // If the specified 'rwMutex' is not locked for writing, call
        // 'bsls::Assert::invokeHandler' with the specified 'text', 'file',
        // 'line', and 'level', where 'text' is text describing the assertion
        // being performed, 'file' is the name of the source file that called
        // the macro, 'line' is the line number in the file where the macro was
        // called, and 'level' is one of the 'bsls::Assert::k_LEVEL_*' string
        // literals.  This function is intended to implement
        // 'BSLMT_WRITEERWRITEMUTEXASSERT_IS_LOCKED_READ',
        // 'BSLMT_WRITEERWRITEMUTEXASSERT_IS_LOCKED_READ_SAFE', and
        // 'BSLMT_WRITEERWRITEMUTEXASSERT_IS_LOCKED_READ_OPT' and should not
        // otherwise be called directly.

    static bool isValidLevel(const char *level);
        // Return 'true' if the specified 'level' compares equal to one of the
        // 'bsls::Assert::k_LEVEL_*' string literals, and 'false' otherwise.
};

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                        // ---------------------------------
                        // class ReaderWriterMutexAssert_Imp
                        // ---------------------------------


// CLASS METHODS
inline
void ReaderWriterMutexAssert_Imp::assertIsLocked(ReaderWriterMutex *rwMutex,
                                                 const char        *text,
                                                 const char        *file,
                                                 int                line,
                                                 const char        *level)
{
    BSLS_ASSERT(rwMutex);
    BSLS_ASSERT(text);
    BSLS_ASSERT(file);
    BSLS_ASSERT(level);
    BSLS_ASSERT(isValidLevel(level));

    if (!rwMutex->isLocked()) {
        bsls::AssertViolation violation(text, file, line, level);
        bsls::Assert::invokeHandler(violation);
    }
}

inline
void ReaderWriterMutexAssert_Imp::assertIsLockedRead(
                                                    ReaderWriterMutex *rwMutex,
                                                    const char        *text,
                                                    const char        *file,
                                                    int                line,
                                                    const char        *level)
{
    BSLS_ASSERT(rwMutex);
    BSLS_ASSERT(text);
    BSLS_ASSERT(file);
    BSLS_ASSERT(level);
    BSLS_ASSERT(isValidLevel(level));

    if (!rwMutex->isLockedRead()) {
        bsls::AssertViolation violation(text, file, line, level);
        bsls::Assert::invokeHandler(violation);
    }
}

inline
void ReaderWriterMutexAssert_Imp::assertIsLockedWrite(
                                                    ReaderWriterMutex *rwMutex,
                                                    const char        *text,
                                                    const char        *file,
                                                    int                line,
                                                    const char        *level)
{
    BSLS_ASSERT(rwMutex);
    BSLS_ASSERT(text);
    BSLS_ASSERT(file);
    BSLS_ASSERT(level);
    BSLS_ASSERT(isValidLevel(level));

    if (!rwMutex->isLockedWrite()) {
        bsls::AssertViolation violation(text, file, line, level);
        bsls::Assert::invokeHandler(violation);
    }
}

inline
bool ReaderWriterMutexAssert_Imp::isValidLevel(const char *level)
{
    BSLS_ASSERT(level);

    return 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_SAFE)
        || 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_OPT)
        || 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_ASSERT)
        || 0 == native_std::strcmp(level, bsls::Assert::k_LEVEL_INVOKE);

}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
