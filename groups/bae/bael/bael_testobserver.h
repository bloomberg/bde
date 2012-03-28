// bael_testobserver.h                                                -*-C++-*-
#ifndef INCLUDED_BAEL_TESTOBSERVER
#define INCLUDED_BAEL_TESTOBSERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an instrumented observer for testing.
//
//@CLASSES:
//    bael_TestObserver: instrumented observer for testing
//
//@SEE_ALSO: bael_record, bael_context, bael_observer
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'bael_Observer' protocol that is instrumented for testing systems that use
// 'bael_Observer' objects.
//..
//                   ( bael_TestObserver )
//                            |             static numInstances
//                            |             ctor
//                            |             setVerbose
//                            |             id
//                            |             numPublishedRecords
//                            |             lastPublishedRecord
//                            |             lastPublishedContext
//                            V
//                    ( bael_Observer )
//                                          dtor
//                                          publish
//..
// 'bael_TestObserver' ascribes to each instance (within a process) a unique
// integer identifier (accessible via the 'id' method), and each instance keeps
// a count of the total number of records that it has published (accessible via
// the 'numPublishedRecords' method).  In addition, the test observer maintains
// a copy of the record and context data from the most recent call to
// 'publish'; that information is available via the accessors
// 'lastPublishedRecord' and 'lastPublishedContext'.
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
///USAGE
///-----
// The following snippets of code illustrate the basic usage of testing
// observer objects.  The example focuses on the individual features that are
// useful for testing larger systems (not shown) that use observers derived
// from 'bael_Observer'.
//
// First create a 'bael_Record' object 'record' and a 'bael_Context' object
// 'context'.  Note that the default values for these objects (or their
// contained objects) are perfectly suitable for testing purposes, since the
// testing observer's purpose is simply to report what has been presented to
// its 'publish' method.
//..
//     bael_RecordAttributes attributes;
//     bdem_List             list;
//     bael_Record           record(attributes, list);
//     bael_Context          context;
//..
// Next, create three test observers 'to1', to2', and 'to3', each with
// 'bsl::cout' as the held stream.  Note that each instance will be given a
// unique integer identifier by the constructor.
//..
//                                        assert(0 == bael_TestObserver::
//                                                             numInstances());
//     bael_TestObserver to1(bsl::cout);  assert(1 == to1.id());
//                                        assert(1 == bael_TestObserver::
//                                                             numInstances());
//     bael_TestObserver to2(bsl::cout);  assert(2 == to2.id());
//                                        assert(2 == bael_TestObserver::
//                                                             numInstances());
//     bael_TestObserver to3(bsl::cout);  assert(3 == to3.id());
//                                        assert(3 == bael_TestObserver::
//                                                             numInstances());
//
//                                      assert(0 == to1.numPublishedRecords());
//                                      assert(0 == to2.numPublishedRecords());
//                                      assert(0 == to3.numPublishedRecords());
//..
// Finally, set 'to1' to "verbose mode" and publish 'record' and 'context' to
// 'to1' and also to 'to2', but not to 'to3'.
//..
//     to1.setVerbose(1);
//     to1.publish(record, context);  assert(1 == to1.numPublishedRecords());
//     to2.publish(record, context);  assert(1 == to2.numPublishedRecords());
//                                    assert(0 == to3.numPublishedRecords());
//..
// This will produce the following output on 'stdout':
//..
//     Test Observer ID 1 publishing record number 1
//     Context: cause = PASSTHROUGH
//              count = 1  of an expected 1 total records
//..
// Note that 'to2' produces no output, although its 'publish' method executed
// (as verified by the 'numPublishedRecords' method); only 'to1' produces a
// printed output, because it has been set to verbose mode.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_CONTEXT
#include <bael_context.h>
#endif

#ifndef INCLUDED_BAEL_OBSERVER
#include <bael_observer.h>
#endif

#ifndef INCLUDED_BAEL_RECORD
#include <bael_record.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                         // =======================
                         // class bael_TestObserver
                         // =======================

class bael_TestObserver : public bael_Observer {
    // This class provides an instrumented implementation of the
    // 'bael_Observer' protocol suitable for testing systems that use
    // 'bael_Observer'.  Each instance receives a unique (per process) integer
    // identifier at construction, and keeps count of the number of records
    // that it has published, as well as the contents of the most recently
    // published record and context.
    //
    // By default, instances publish appropriate diagnostic information, either
    // to 'stdout' or to an optional 'bsl::ostream'.  This diagnostic
    // information can be suppressed by a call to 'setVerbose' with a non-zero
    // argument.  A subsequent call to 'setVerbose' with a zero argument will
    // restore the default behavior of 'publish'.

    // CLASS DATA
    static bces_AtomicInt
                        s_count;        // number of instances created

    // DATA
    bsl::ostream&       d_stream;       // target of 'publish' method
    bael_Record         d_record;       // most-recently-published record
    bael_Context        d_context;      // most-recently-published context

    int                 d_count;        // unique (per process) id
    volatile int        d_verboseFlag;  // "verbosity" mode on 'publish'
    volatile int        d_numRecords;   // total number of published records

    mutable bcemt_Mutex d_mutex;        // serializes concurrent calls to
                                        // 'publish' and protects concurrent
                                        // access to other class members

    // NOT IMPLEMENTED
    bael_TestObserver(const bael_TestObserver&);
    bael_TestObserver& operator=(const bael_TestObserver&);

  public:
    // CLASS METHODS
    static int numInstances();
        // Return the total number of instances of this class that have been
        // created since this process has begun.

    // CREATORS
    bael_TestObserver(bsl::ostream&    stream,
                      bslma_Allocator *basicAllocator = 0);
        // Create a test observer having a unique integer identifier, whose
        // 'publish' method will send it's output (if any) to the specified
        // 'stream'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  By default, this observer prints nothing to
        // 'stream'.  Note that the 'setVerbose' method will affect this
        // default behavior.

    virtual ~bael_TestObserver();
        // Destroy this test observer.

    // MANIPULATORS
    virtual void publish(const bael_Record&  record,
                         const bael_Context& context);
        // Store as the most recently published data the specified log 'record'
        // and publishing 'context'.  If this test observer is in verbose mode,
        // print an appropriate diagnostic message to the stream specified at
        // construction.  Note that at construction test observers are not in
        // verbose mode, but that the 'setVerbose' method will affect this
        // mode, and thus the behavior of this method.

    void setVerbose(int flagValue);
        // Set the internal verbose mode of this test observer to the specified
        // (boolean) 'flagValue'.  The default mode is *not* verbose.  If
        // 'flagValue' is non-zero, calls to 'publish' will print appropriate
        // diagnostics to the 'bsl::ostream' associated with this test
        // observer.  This printing is suppressed if flagValue == 0.

    // ACCESSORS
    int id() const;
        // Return the unique (per process) integer identifier of this test
        // observer.

    int numPublishedRecords() const;
        // Return the total number of records that this test observer has
        // published.

    const bael_Record& lastPublishedRecord() const;
        // Return a reference to the record most recently published by this
        // test observer.  The behavior is undefined unless 'publish' has been
        // called at least once.

    const bael_Context& lastPublishedContext() const;
        // Return a reference to the context most recently published by this
        // test observer.  The behavior is undefined unless 'publish' has been
        // called at least once.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // -----------------------
                         // class bael_TestObserver
                         // -----------------------

// CLASS METHODS
inline
int bael_TestObserver::numInstances()
{
    return s_count;
}

// CREATORS
inline
bael_TestObserver::bael_TestObserver(bsl::ostream&    stream,
                                     bslma_Allocator *basicAllocator)
: d_stream(stream)
, d_record(basicAllocator)
, d_context(basicAllocator)
, d_count(++s_count)
, d_verboseFlag(0)
, d_numRecords(0)
{
}

// MANIPULATORS
inline
void bael_TestObserver::setVerbose(int flagValue)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_verboseFlag = flagValue;
}

// ACCESSORS
inline
int bael_TestObserver::id() const
{
    return d_count;
}

inline
int bael_TestObserver::numPublishedRecords() const
{
    return d_numRecords;
}

inline
const bael_Record& bael_TestObserver::lastPublishedRecord() const
{
    return d_record;
}

inline
const bael_Context& bael_TestObserver::lastPublishedContext() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_context;
}

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
