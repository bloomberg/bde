// bcemt_threadimpl.h              -*-C++-*-

#ifndef INCLUDED_BCEMT_THREADIMPL
#define INCLUDED_BCEMT_THREADIMPL

//@PURPOSE: Provide declarations for template implementation classes.
//
//@CLASSES:
//         bcemt_AttributeImpl: declaration of thread attribute template class
//        bcemt_ThreadUtilImpl: declaration of threads utility template class
//             bcemt_MutexImpl: declaration of mutex template class
//    bcemt_RecursiveMutexImpl: declaration of recursive mutex template class
//         bcemt_ConditionImpl: declaration of condition template class
//
//@SEE_ALSO:
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@DESCRIPTION: This component declares five template clasess that are
// parameterized with one parameter named 'THREAD_POLICY'.  These declarations
// allow for the ability to provide concrete template specializations for
// specific platforms (e.g., posix threads, native Windows threads, etc.)
// Please see the other components in this package for more details.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif


namespace BloombergLP {
                          // =========================
                          // class bcemt_AttributeImpl
                          // =========================

template <typename THREAD_POLICY>
class bcemt_AttributeImpl;

                          // ==========================
                          // class bcemt_ThreadUtilImpl
                          // ==========================

template <typename THREAD_POLICY>
struct bcemt_ThreadUtilImpl;


                          // =====================
                          // class bcemt_MutexImpl
                          // =====================

template <typename THREAD_POLICY>
class bcemt_MutexImpl;

                         // ===============================
                         // class bcemti_RecursiveMutexImpl
                         // ===============================

template <typename THREAD_POLICY>
class bcemt_RecursiveMutexImpl;

                         // =========================
                         // class bcemt_ConditionImpl
                         // =========================

template <typename THREAD_POLICY>
class bcemt_ConditionImpl;

template <typename THREAD_POLICY>
class bcemt_SemaphoreImpl;

}  // close namespace BloombergLP

#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
