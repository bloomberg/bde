// ball_filteringobserver.h                                           -*-C++-*-
#ifndef INCLUDED_BALL_FILTERINGOBSERVER
#define INCLUDED_BALL_FILTERINGOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an observer that filters log records.
//
//@CLASSES:
//    ball::FilteringObserver: observer that filters log records
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@AUTHOR: Oleg Subbotin
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for receiving and processing log records:
//..
//               ,-----------------------.
//              ( ball::FilteringObserver )
//               `-----------------------'
//                           |              ctor
//                           V
//                    ,--------------.
//                   ( ball::Observer )
//                    `--------------'
//                                          publish
//                                          dtor
//..
// 'ball::FilteringObserver' processes the log records it receives through its
// 'publish' method and conditionally forwards them to the inner observer
// supplied at construction.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example shows how to use filtering observer to route some log records
// to a designated observer.  A common use case is routing all log records for
// a given category (or category pattern) to a separate log file.
//
// First, we create a filter that will match a log record's category against
// the pattern "EQUITY.*":
//..
//  bool categoryFilter(const ball::Record& record, const ball::Context&)
//  {
//      return ball::PatternUtil::isMatch(record.fixedFields().category(),
//                                        "EQUITY.*");
//  }
//..
// Then, we create the observer that will receive filtered log records and
// create a filtering observer:
//..
//  bsl::shared_ptr<ball::TestObserver> innerObserver(
//                                         new ball::TestObserver(&bsl::cout));
//
//  ball::FilteringObserver filteringObserver(innerObserver, categoryFilter);
//..
// Next, we issue a series of log records and verify that only records with the
// category matching the pattern are published to the inner observer:
//..
//  const ball::Context context(ball::Transmission::e_PASSTHROUGH, 0, 1);
//
//  ball::RecordAttributes fixedFields;
//
//  bsl::shared_ptr<ball::Record> record;
//  record.createInplace();
//
//  fixedFields.setCategory("CURNCY.USDGBP");
//  record->setFixedFields(fixedFields);
//  filteringObserver.publish(record, context);
//
//  assert(0 == innerObserver->numPublishedRecords());  // dropped
//
//  fixedFields.setCategory("EQUITY.NYSE");
//  record->setFixedFields(fixedFields);
//  filteringObserver.publish(record, context);
//
//  assert(1 == innerObserver->numPublishedRecords());  // forwarded
//
//  fixedFields.setCategory("EQUIT.");
//  record->setFixedFields(fixedFields);
//  filteringObserver.publish(record, context);
//
//  assert(1 == innerObserver->numPublishedRecords());  // dropped
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_STDALLOCATOR
#include <bslma_stdallocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ALLOCATORARGT
#include <bslmf_allocatorargt.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {
namespace ball {

class Context;
class Record;

                           // =======================
                           // class FilteringObserver
                           // =======================

class FilteringObserver : public Observer {
    // This class provides a concrete implementation of the 'Observer' protocol
    // that filters the 'Record' and 'Context' objects passed to its 'publish'
    // method based on a callback (function or functor) supplied at
    // construction.  The callback must be convertible to:
    //..
    //  bsl::function<bool(const Record&, const Context&)>
    //..
    // If the callback returns 'true' for the 'Record' and 'Context', then they
    // are forwarded to the 'publish' method of an observer also supplied at
    // construction.  If the callback returns 'false', then the 'Record' and
    // 'Context' are ignored (i.e., they are filtered out by this observer).

  public:
    // PUBLIC TYPES
    typedef bsl::function<bool(const Record&, const Context&)>
                                                          RecordFilterCallback;
    // 'RecordFilterCallback' is the type of a user-supplied callback functor
    // used to filter the supplied 'Record' and 'Context' objects based on the
    // user-defined criteria provided by the functor.

  private:
    // DATA
    bsl::shared_ptr<Observer> d_innerObserver;  // inner observer
    RecordFilterCallback      d_recordFilter;   // record filter callback

    // NOT IMPLEMENTED
    FilteringObserver(const FilteringObserver&);
    FilteringObserver& operator=(const FilteringObserver&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FilteringObserver,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    FilteringObserver(const bsl::shared_ptr<Observer>&  observer,
                      const RecordFilterCallback&       recordFilterCallback,
                      bslma::Allocator                 *basicAllocator = 0);
        // Create a filtering observer that conditionally passes log records to
        // the specified 'observer' based on the specified
        // 'recordFilterCallback'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if 'observer'
        // is 0 or a cycle is created among observers.

    virtual ~FilteringObserver();
        // Destroy this filtering observer.

    // MANIPULATORS
    using Observer::publish;

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.  Forward 'record' and 'context' to the 'publish' method
        // of the observer supplied at construction if the filter callback
        // supplied at construction returns 'true' for 'record' and 'context',
        // and ignore (filter out) 'record' and 'context' otherwise.  The
        // behavior is undefined if 'record' or 'context' is modified during
        // the execution of this method.

    virtual void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method, and is held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared-pointers must be released.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // -----------------------
                           // class FilteringObserver
                           // -----------------------

// CREATORS
inline
FilteringObserver::FilteringObserver(
                        const bsl::shared_ptr<Observer>&  observer,
                        const RecordFilterCallback&       recordFilterCallback,
                        bslma::Allocator                 *basicAllocator)
: d_innerObserver(observer)
, d_recordFilter(bsl::allocator_arg_t(),
                 bsl::allocator<RecordFilterCallback>(basicAllocator),
                 recordFilterCallback)
{
    BSLS_ASSERT_SAFE(observer);
}

// MANIPULATORS
inline
void FilteringObserver::releaseRecords()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
