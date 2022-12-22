// bslmt_threadutil.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_threadutil_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>

#include <bsls_platform.h>

#include <bsl_cmath.h>

#include <bsl_c_limits.h>

namespace {
namespace u {

// We can't just package a 'bslmt_ThreadFunction' and 'userData' into a simple
// functor and pass that to one of the inline 'createWithAllocator' template
// functions because the contract for threads created by calling with a
// "C"-style function requires the function to be able to return a 'void *'
// which the calling thread can access via 'join'.  The signature of the
// functor passed to the inline 'createWithAllocator' template functions,
// however, does not provide a way to return a value.

using namespace BloombergLP;

struct NamedFuncPtrRecord {
    // This 'struct' stores the information necessary to call the 'extern "C"'
    // function 'd_threadFunction' passing it 'void *' 'd_userData' after
    // a thread is created and named 'd_threadName'.

    // DATA
    bslmt_ThreadFunction  d_threadFunction;    // extern "C" func ptr
    void                 *d_userData;          // 'void *' to be passed to func
    bsl::string           d_threadName;        // thread name

  public:
    // CREATOR
    NamedFuncPtrRecord(bslmt_ThreadFunction      threadFunction,
                       void                     *userData,
                       const bslstl::StringRef&  threadName,
                       bslma::Allocator         *allocator)
    : d_threadFunction(threadFunction)
    , d_userData(userData)
    , d_threadName(threadName, allocator)
        // Create a 'NamedFuncPtrRecord' containing the specified
        // 'threadFunction', 'userData', and 'threadName', and using the
        // specified 'allocator' to supply memory.
    {
    }
};

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {

extern "C"
void *bslmt_threadutil_namedFuncPtrThunk(void *arg)
    // C-linkage routine that allows us to call a C-style function and name the
    // thread, using information in the specified 'arg', which points to a
    // 'u::NamedFuncPtrRecord' that must be freed by this function.  The
    // behavior is undefined if the thread name is empty.
{
    u::NamedFuncPtrRecord *nfpr_p = static_cast<u::NamedFuncPtrRecord *>(arg);

    BSLS_ASSERT(! nfpr_p->d_threadName.empty());  // This function should never
                                                  // be called unless the
                                                  // thread is named.

    bslmt::ThreadUtil::setThreadName(nfpr_p->d_threadName);

    bslma::Allocator      *alloc_p =
                              nfpr_p->d_threadName.get_allocator().mechanism();
    bslmt_ThreadFunction   threadFunction = nfpr_p->d_threadFunction;
    void                  *userData       = nfpr_p->d_userData;

    alloc_p->deleteObjectRaw(nfpr_p);   // Release 'NameFuncPtrRecord's
                                        // memory, particularly important if
                                        // thread is detached and may outlive
                                        // allocator '*alloc_p'.

    return (*threadFunction)(userData);
}

                            // -----------------
                            // struct ThreadUtil
                            // -----------------

// CLASS METHODS
int bslmt::ThreadUtil::convertToSchedulingPriority(
               ThreadAttributes::SchedulingPolicy policy,
               double                             normalizedSchedulingPriority)
{
    BSLS_ASSERT_OPT((int) policy >= ThreadAttributes::e_SCHED_MIN);
    BSLS_ASSERT_OPT((int) policy <= ThreadAttributes::e_SCHED_MAX);

    BSLS_ASSERT_OPT(normalizedSchedulingPriority >= 0.0);
    BSLS_ASSERT_OPT(normalizedSchedulingPriority <= 1.0);

    const int minPri = getMinSchedulingPriority(policy);
    const int maxPri = getMaxSchedulingPriority(policy);

    if (minPri == ThreadAttributes::e_UNSET_PRIORITY ||
        maxPri == ThreadAttributes::e_UNSET_PRIORITY) {
        return ThreadAttributes::e_UNSET_PRIORITY;                    // RETURN
    }

#if !defined(BSLS_PLATFORM_OS_CYGWIN)
    double ret = (maxPri - minPri) * normalizedSchedulingPriority +
                                                                  minPri + 0.5;
#else
    // On Cygwin, a lower numerical value implies a higher thread priority:
    //   minSchedPriority = 15, maxSchedPriority = -14

    double ret = - ((minPri - maxPri) * normalizedSchedulingPriority - minPri)
                                                                         + 0.5;
#endif

    return static_cast<int>(bsl::floor(ret));
}

int bslmt::ThreadUtil::create(Handle                  *handle,
                              const ThreadAttributes&  attributes,
                              ThreadFunction           function,
                              void                    *userData)
{
    BSLS_ASSERT(handle);

    if (false == attributes.threadName().isEmpty()) {
        // Named thread.  Only 'createWithAllocator' can name threads.

        return createWithAllocator(handle,
                                   attributes,
                                   function,
                                   userData,
                                   bslma::Default::globalAllocator());// RETURN
    }

    // Unnamed thread.

    return Imp::create(handle, attributes, function, userData);
}

int bslmt::ThreadUtil::createWithAllocator(Handle                  *handle,
                                           const ThreadAttributes&  attributes,
                                           ThreadFunction           function,
                                           void                    *userData,
                                           bslma::Allocator        *allocator)
{
    BSLS_ASSERT(handle);
    BSLS_ASSERT_OPT(allocator);

    if (false == attributes.threadName().isEmpty()) {
        // Named thread.

        bslma::ManagedPtr<u::NamedFuncPtrRecord> nfpr_m(
                new (*allocator) u::NamedFuncPtrRecord(function,
                                                       userData,
                                                       attributes.threadName(),
                                                       allocator),
                allocator);

        int rc = Imp::create(handle,
                             attributes,
                             bslmt_threadutil_namedFuncPtrThunk,
                             nfpr_m.ptr());
        if (0 == rc) {
            nfpr_m.release();
        }

        return rc;                                                    // RETURN
    }

    // Unnamed thread.

    return Imp::create(handle, attributes, function, userData);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
