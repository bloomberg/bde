// bsls_bsllockimpl_pthread.h                                         -*-C++-*-
#ifndef INCLUDED_BSLS_BSLLOCKIMPL_PTHREAD
#define INCLUDED_BSLS_BSLLOCKIMPL_PTHREAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mutex for use below 'bslmt'.
//
//@CLASSES:
//  bsls::BslLockImpl_pthread: pthread mutex
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") by wrapping a suitable platform-specific mechanism.  The
// 'bsls::BslLockImpl_pthread' class provides 'lock' and 'unlock' operations.
// Note that 'bsls::BslLockImpl_pthread' is not intended for direct client use;
// see 'bslmt_mutex' instead.  Also note that 'bsls::BslLockImpl_pthread' is
// not recursive.
//
///Usage
///-----
// This component is an implementation detail of 'bsls' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bsls_buildtarget.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_UNIX

#include <pthread.h>

#ifdef BDE_BUILD_TARGET_SAFE
// This component needs to be below bsls_assert in the physical hierarchy, so
// 'BSLS_ASSERT' macros can't be used here.  To workaround this issue, we use
// the C 'assert' instead.

#include <assert.h>

#define BSLS_BSLLOCKIMPL_PTHREAD_ASSERT_SAFE(x) assert((x))
#else
#define BSLS_BSLLOCKIMPL_PTHREAD_ASSERT_SAFE(x)
#endif

namespace BloombergLP {
namespace bsls {

                              // =========================
                              // class BslLockImpl_pthread
                              // =========================

class BslLockImpl_pthread {
    // This 'class' implements a light-weight wrapper of an OS-level mutex to
    // support intra-process synchronization.  The mutex implemented by this
    // class is *non*-recursive.  Note that 'BslLockImpl_pthread' is *not*
    // intended for direct use by client code; it is meant for internal use
    // only.

    // DATA
    pthread_mutex_t  d_lock;  // 'pthreads' mutex object

  private:
    // NOT IMPLEMENTED
    BslLockImpl_pthread(const BslLockImpl_pthread&);             // = delete
    BslLockImpl_pthread& operator=(const BslLockImpl_pthread&);  // = delete

  public:
    // CREATORS
    BslLockImpl_pthread();
        // Create a lock object initialized to the unlocked state.

    ~BslLockImpl_pthread();
        // Destroy this lock object.  The behavior is undefined unless this
        // object is in the unlocked state.

    // MANIPULATORS
    void lock();
        // Acquire the lock on this object.  If the lock on this object is
        // currently held by another thread, then suspend execution of the
        // calling thread until the lock can be acquired.  The behavior is
        // undefined unless the calling thread does not already hold the lock
        // on this object.  Note that deadlock may result if this method is
        // invoked while the calling thread holds the lock on the object.

    void unlock();
        // Release the lock on this object that was previously acquired through
        // a call to 'lock', enabling another thread to acquire the lock.  The
        // behavior is undefined unless the calling thread holds the lock on
        // this object.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // -------------------------
                              // class BslLockImpl_pthread
                              // -------------------------

// CREATORS
inline
BslLockImpl_pthread::BslLockImpl_pthread()
{
    const int status = pthread_mutex_init(&d_lock, 0);
    (void)status;
    BSLS_BSLLOCKIMPL_PTHREAD_ASSERT_SAFE(0 == status);
}

inline
BslLockImpl_pthread::~BslLockImpl_pthread()
{
    const int status = pthread_mutex_destroy(&d_lock);
    (void)status;
    BSLS_BSLLOCKIMPL_PTHREAD_ASSERT_SAFE(0 == status);
}

// MANIPULATORS
inline
void BslLockImpl_pthread::lock()
{
    const int status = pthread_mutex_lock(&d_lock);
    (void)status;
    BSLS_BSLLOCKIMPL_PTHREAD_ASSERT_SAFE(0 == status);
}

inline
void BslLockImpl_pthread::unlock()
{
    const int status = pthread_mutex_unlock(&d_lock);
    (void)status;
    BSLS_BSLLOCKIMPL_PTHREAD_ASSERT_SAFE(0 == status);
}


}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_PLATFORM_OS_UNIX

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
