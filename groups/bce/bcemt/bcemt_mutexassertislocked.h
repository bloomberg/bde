// bcemt_mutexassertislocked.h                                        -*-C++-*-
#ifndef INCLUDED_BCEMT_MUTEXASSERTISLOCKED
#define INCLUDED_BCEMT_MUTEXoASSERTISLOCKED

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent mutex.
//
//@CLASSES:
//  bcemt_MutexAssertLocked: namespace for implementation functions for macros
//
//@MACROS:
//  BCEMT_MUTEX_ASSERT_IS_LOCKED: check mutex is locked in non-opt modes
//  BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED: check mutex is locked in safe mode
//  BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED: check mutex is locked in all modes
//
//@SEE_ALSO: bcemt_mutex
//
//@DESCRIPTION: This component provides macros for asserting that a mutex is
// locked.  It does not distinguish between locks held by the current thread or
// other threads.  If the macro is active in the current build mode and, when
// the macro is called, the mutex is unlocked, the assert handler installed for
// 'BSLS_ASSERT' will be called.  The handler installed by default will print a
// report and abort.
//
// The three macros defined by the component are analogous to the macros
// defined by BSLS_ASSERT:
//: o BCEMT_MUTEX_ASSERT_IS_LOCKED: active when 'BSLS_ASSERT' is active
//: o BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED: active when 'BSLS_ASSERT_SAFE' is
//:   active
//: o BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED: active when 'BSLS_ASSERT_OPT' is active
// In build modes where any one of these macros is not active, calling it will
// have no effect.
//
// If any of these asserts are in effect and fail (because the mutex in
// question was unlocked), the bhavior is almost identical to that if the
// corresponding 'BSLS_ASSERT*' failed -- 'bsls::Assert::invokeHandler' is
// called, with a source code expression, the name of the source file, and the
// line number in the source file where the macro was called.  If the default
// handler is installed, this will result in an error message and an abort.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BCEMT_MUTEX_ASSERT_IS_LOCKED(mutex_p) do {                        \
        bcemt_MutexAssertIsLocked::assertIsLockedImpl(                        \
                           (mutex_p),                                         \
                           "BCEMT_MUTEX_ASSERT_IS_LOCKED(" #mutex_p ")",      \
                           __FILE__,                                          \
                           __LINE__); } while (false)
#else
    #define BCEMT_MUTEX_ASSERT_IS_LOCKED(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(mutex_p) do {                   \
        bcemt_MutexAssertIsLocked::assertIsLockedImpl(                        \
                           (mutex_p),                                         \
                           "BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(" #mutex_p ")", \
                           __FILE__,                                          \
                           __LINE__); } while (false)
#else
    #define BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(mutex_p) do {                    \
        bcemt_MutexAssertIsLocked::assertIsLockedImpl(                        \
                           (mutex_p),                                         \
                           "BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(" #mutex_p ")",  \
                           __FILE__,                                          \
                           __LINE__); } while (false)
#else
    #define BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(mutex_p) ((void) 0)
#endif

namespace BloombergLP {

class bcemt_Mutex;

                       // ===============================
                       // class bcemt_MutexAssertIsLocked
                       // ===============================

struct bcemt_MutexAssertIsLocked {
    // CLASS METHODS
    static
    void assertIsLockedImpl(bcemt_Mutex *mutex,
                            const char  *text,
                            const char  *file,
                            int          line);
        // If the specified '*mutex' is not locked, call
        // 'bsls::Assert::invokeHandler' with the specified 'text', 'file', and
        // 'line', where 'text' is the source for the macro expression call,
        // 'file' is the name of the source file that called the macro, and
        // 'line' is the line number in the file where the macro was called.
        // Always leave this mutex in the state it was initially in.  This
        // function is used to implement 'BCEMT_MUTEX_ASSERT_IS_LOCKED',
        // 'BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED', and
        // 'BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED' and should not otherwise be
        // called directly.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
