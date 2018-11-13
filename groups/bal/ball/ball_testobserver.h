// ball_testobserver.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_TESTOBSERVER
#define INCLUDED_BALL_TESTOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an instrumented observer for testing.
//
//@CLASSES:
//  ball::TestObserver: instrumented observer for testing
//
//@SEE_ALSO: ball_record, ball_context, ball_observer
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol, 'ball::TestObserver', that is instrumented for
// testing systems that use 'ball::Observer' objects.
//..
//                   ( ball::TestObserver )
//                            |             static numInstances
//                            |             ctor
//                            |             setVerbose
//                            |             id
//                            |             lastPublishedContext
//                            |             lastPublishedRecord
//                            |             numPublishedRecords
//                            |             numReleases
//                            V
//                    ( ball::Observer )
//                                          dtor
//                                          publish
//                                          releaseRecords
//..
// 'ball::TestObserver' ascribes to each instance (within a process) a unique
// integer identifier (accessible via the 'id' method), and each instance keeps
// a count of the total number of records that it has published (accessible via
// the 'numPublishedRecords' method).  In addition, the test observer maintains
// a copy of the record and context data from the most recent call to
// 'publish'; that information is available via the accessors
// 'lastPublishedRecord' and 'lastPublishedContext'.
//
///Thread Safety
///-------------
// The 'ball::TestObserver' provides a 'publish' method that is *thread-safe*,
// meaning that the test observer may be used to log records from multiple
// threads.  However, the 'ball::TestObserver' accessors 'lastPublished' method
// and 'lastPublishedContext' provide references to internal data structures
// (for backwards compatibility) and are therefore *not* *thread-safe*.
//
///Verbose Mode
///------------
// The 'publish' method, in addition to making a local copy of the published
// data for future inspection, is capable of printing an appropriate diagnostic
// message to the 'ostream' held by the test observer instance.  By default,
// this printing is suppressed, but the behavior can be altered by calling the
// 'setVerbose' method.  A call to 'setVerbose' with a non-zero argument (e.g.,
// 'setVerbose(1)') places the object in verbose mode, which enables diagnostic
// printing.  A subsequent call of 'setVerbose(0)' restores the default "quiet"
// behavior.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of testing
// observer objects.  The example focuses on the individual features that are
// useful for testing larger systems (not shown) that use observers derived
// from 'ball::Observer'.
//
// First create a 'ball::Record' object 'record' and a 'ball::Context' object
// 'context'.  Note that the default values for these objects (or their
// contained objects) are perfectly suitable for testing purposes, since the
// testing observer's purpose is simply to report what has been presented to
// its 'publish' method.
//..
//  ball::RecordAttributes attributes;
//  ball::UserFields       fieldValues;
//  ball::Context          context;
//
//  bslma::Allocator *ga = bslma::Default::globalAllocator(0);
//  const bsl::shared_ptr<const ball::Record>
//             record(new (*ga) ball::Record(attributes, fieldValues, ga), ga);
//..
// Next, create three test observers 'to1', to2', and 'to3', each with
// 'bsl::cout' as the held stream.  Note that each instance will be given a
// unique integer identifier by the constructor.
//..
//  assert(0 == ball::TestObserver::numInstances());
//
//  ball::TestObserver to1(&bsl::cout);
//  assert(1 == to1.id());
//  assert(1 == ball::TestObserver::numInstances());
//
//  ball::TestObserver to2(&bsl::cout);
//  assert(2 == to2.id());
//  assert(2 == ball::TestObserver::numInstances());
//
//  ball::TestObserver to3(&bsl::cout);
//  assert(3 == to3.id());
//  assert(3 == ball::TestObserver::numInstances());
//
//  assert(0 == to1.numPublishedRecords());
//  assert(0 == to2.numPublishedRecords());
//  assert(0 == to3.numPublishedRecords());
//..
// Finally, set 'to1' to "verbose mode" and publish 'record' and 'context' to
// 'to1' and also to 'to2', but not to 'to3'.
//..
//  to1.setVerbose(1);
//  to1.publish(record, context);
//  assert(1 == to1.numPublishedRecords());
//
//  to2.publish(record, context);
//  assert(1 == to2.numPublishedRecords());
//  assert(0 == to3.numPublishedRecords());
//..
// This will produce the following output on 'stdout':
//..
//  Test Observer ID 1 publishing record number 1
//  Context: cause = PASSTHROUGH
//           count = 1 of an expected 1 total records.
//..
// Note that 'to2' produces no output, although its 'publish' method executed
// (as verified by the 'numPublishedRecords' method); only 'to1' produces a
// printed output, because it has been set to verbose mode.

#include <balscm_version.h>

#include <ball_context.h>
#include <ball_observer.h>
#include <ball_record.h>

#include <bslma_allocator.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_atomicoperations.h>
#include <bsls_review.h>

#include <bsl_iosfwd.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ball {

                         // ==================
                         // class TestObserver
                         // ==================

class TestObserver : public Observer {
    // This class provides an instrumented implementation of the 'Observer'
    // protocol suitable for testing systems that use 'Observer'.  Each
    // instance receives a unique (per process) integer identifier at
    // construction, and keeps count of the number of records that it has
    // published, as well as the contents of the most recently published record
    // and context.
    //
    // By default, the 'publish' method prints no diagnostic information to the
    // 'bsl::ostream' supplied at construction.  This diagnostic information
    // can be enabled by a call to 'setVerbose' with a non-zero argument.  A
    // subsequent call to 'setVerbose' with a zero argument will restore the
    // default behavior of 'publish'.

    // TYPES
    typedef bsls::AtomicOperations                   AtomicOps;
    typedef bsls::AtomicOperations::AtomicTypes::Int AtomicInt;

    // CLASS DATA
    static AtomicInt      s_count;        // number of instances created

    // DATA
    bsl::ostream         *d_stream_p;     // target of 'publish' method
                                          // diagnostic information

    Record                d_record;       // most-recently-published record

    Context               d_context;      // most-recently-published context

    int                   d_id;           // unique (per process) id

    int                   d_verboseFlag;  // "verbosity" mode on 'publish'

    int                   d_numRecords;   // total number of published records

    int                   d_numReleases;  // total number of calls to
                                          // 'releaseRecords'

    mutable bslmt::Mutex  d_mutex;        // serializes concurrent calls to
                                          // 'publish' and protects concurrent
                                          // access to other class members

    // NOT IMPLEMENTED
    TestObserver(const TestObserver&);
    TestObserver& operator=(const TestObserver&);

  public:
    // CLASS METHODS
    static int numInstances();
        // Return the total number of instances of this class that have been
        // created since this process has begun.

    // CREATORS
    explicit
    TestObserver(bsl::ostream *stream, bslma::Allocator *basicAllocator = 0);
        // Create a test observer having a unique integer identifier, whose
        // 'publish' method will print diagnostic information (if any) to the
        // specified 'stream'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  By default, this observer prints nothing
        // to 'stream'.  Note that the 'setVerbose' method can affect this
        // default behavior.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    explicit
    TestObserver(bsl::ostream& stream, bslma::Allocator *basicAllocator = 0);
        // Create a test observer having a unique integer identifier, whose
        // 'publish' method will print diagnostic information (if any) to the
        // specified 'stream'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  By default, this observer prints nothing
        // to 'stream'.  Note that the 'setVerbose' method can affect this
        // default behavior.
        //
        // !DEPRECATED!: Use the constructor taking 'bsl::ostream *' instead.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    virtual ~TestObserver();
        // Destroy this test observer.

    // MANIPULATORS
    virtual void publish(const Record& record, const Context& context);
        // Store as the most recently published data the specified log 'record'
        // and publishing 'context'.  If this test observer is in verbose mode,
        // print an appropriate diagnostic message to the stream specified at
        // construction.  Note that at construction test observers are not in
        // verbose mode, but that the 'setVerbose' method can affect this mode,
        // and thus the behavior of this method.
        //
        // !DEPRECATED!: Use the alternative 'publish' overload instead.

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Store as the most recently published data the specified log 'record'
        // and publishing 'context'.  If this test observer is in verbose mode,
        // print an appropriate diagnostic message to the stream specified at
        // construction.  Note that at construction test observers are not in
        // verbose mode, but that the 'setVerbose' method can affect this mode,
        // and thus the behavior of this method.

    virtual void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method, and is held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared-pointers must be released.

    void setVerbose(int flagValue);
        // Set the internal verbose mode of this test observer to the specified
        // (boolean) 'flagValue'.  The default mode is *not* verbose.  If
        // 'flagValue' is non-zero, calls to 'publish' will print appropriate
        // diagnostics to the 'bsl::ostream' associated with this test
        // observer.  This printing is suppressed if 'flagValue' is 0.

    // ACCESSORS
    int id() const;
        // Return the unique (per process) integer identifier of this test
        // observer.

    const Context& lastPublishedContext() const;
        // Return a reference to the context most recently published by this
        // test observer.  The behavior is undefined unless 'publish' has been
        // called at least once, and no other thread is manipulating this
        // object concurrently (i.e., this function is *not* thread safe).

    const Record& lastPublishedRecord() const;
        // Return a reference to the record most recently published by this
        // test observer.  The behavior is undefined unless 'publish' has been
        // called at least once, and no other thread is manipulating this
        // object concurrently (i.e., this function is *not* thread safe).

    int numPublishedRecords() const;
        // Return the total number of records that this test observer has
        // published.

    int numReleases() const;
        // Return the total number of times that 'releaseRecords' has been
        // called on this test observer.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                         // ------------------
                         // class TestObserver
                         // ------------------

// CLASS METHODS
inline
int TestObserver::numInstances()
{
    return AtomicOps::getIntRelaxed(&s_count);
}

// CREATORS
inline
TestObserver::TestObserver(bsl::ostream     *stream,
                           bslma::Allocator *basicAllocator)
: d_stream_p(stream)
, d_record(basicAllocator)
, d_context(basicAllocator)
, d_id(AtomicOps::incrementIntNvAcqRel(&s_count))
, d_verboseFlag(0)
, d_numRecords(0)
, d_numReleases(0)
{
    BSLS_REVIEW(d_stream_p);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
TestObserver::TestObserver(bsl::ostream&     stream,
                           bslma::Allocator *basicAllocator)
: d_stream_p(&stream)
, d_record(basicAllocator)
, d_context(basicAllocator)
, d_id(AtomicOps::incrementIntNvAcqRel(&s_count))
, d_verboseFlag(0)
, d_numRecords(0)
, d_numReleases(0)
{
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

// MANIPULATORS
inline
void TestObserver::releaseRecords()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    ++d_numReleases;
}

inline
void TestObserver::setVerbose(int flagValue)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_verboseFlag = flagValue;
}

// ACCESSORS
inline
int TestObserver::id() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_id;
}

inline
const Context& TestObserver::lastPublishedContext() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_context;
}

inline
const Record& TestObserver::lastPublishedRecord() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_record;
}

inline
int TestObserver::numPublishedRecords() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_numRecords;
}

inline
int TestObserver::numReleases() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_numReleases;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
