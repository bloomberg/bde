// ball_loggerfunctorpayloads.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_LOGGERFUNCTORPAYLOADS
#define INCLUDED_BALL_LOGGERFUNCTORPAYLOADS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide a suite of useful logger manager functor payloads.
//
//@CLASSES
//  ball::LoggerFunctorPayloads: namespace for logger manager functor payloads
//
//@SEE_ALSO: ball_loggermanager, ball_categorymanager
//
//@DESCRIPTION: This component provides a suite of pure procedures, each of
// which may be used as the function body "payload" of one of the various
// 'bdef' functors used as callbacks in the 'ball_loggermanager' component.
// Each function provides a specific customization or convenience enhancement
// to the basic logger functionality.
//
// Functions in this component have at most nine parameters and match one of
// the four signatures:
//..
//    (ball::UserFields *, const ball::UserFieldsSchema &)
//    (ball::Transmission::Cause)
//    (int *, int *, int *, int *, const char *)
//..
// These signatures match the five 'typedef' definitions used in the
// 'ball_loggermanager' component, as shown below.  Note that
// 'ball::LoggerManager::PublishAllCallback' is just an alias for
// 'ball::Logger::PublishAllCallback', which is itself an alias for a
// particular 'bsl::function' specialization.
//..
//  ball::Logger Functors
//  --------------------
//  typedef bsl::function<void(ball::UserFields *, const ball::UserSchema&)>
//                                                                   Populator;
//      // 'Populator' is the type of a user-supplied callback functor used to
//      // populate the user-defined fields in each log record.  In particular,
//      // the first 'd_userSchema_p->length()' user-defined fields of each
//      // record must be type-consistent with 'd_userSchema_p'.
//
//  typedef bsl::function<void(ball::Transmission::Cause)> PublishAllCallback;
//      // 'PublishAllCallback' is the type of the functor that is invoked
//      // to publish all record buffers of all loggers that are allocated
//      // by the logger manager.
//
//  ball::LoggerManager Functors
//  ---------------------------
//  typedef bsl::function<void(int *, int *, int *, int *, const char *)>
//                                                   DefaultThresholdsCallback;
//      // 'DefaultThresholdsCallback' is the type of the functor that
//      // determines default threshold levels for categories added to the
//      // registry by the 'setCategory(const char *)' method.
//
//  typedef ball::Logger::PublishAllCallback PublishAllCallback;
//      // 'PublishAllCallback' is the type of the functor that is invoked
//      // to publish all record buffers of all active loggers (i.e., loggers
//      // allocated by the logger manager that have not yet been deallocated).
//..
//
///Synopsis
///--------
// The following is a list of functions available in this component, each
// followed by the callback 'typedef' for which it is appropriate.
//..
//    loadParentCategoryThresholdValues     DefaultThresholdsCallback
//..
//
///Support for Hierarchical Category Names
///---------------------------------------
// The 'ball' logging toolkit does not explicitly support any structure in the
// registry of category names; each unique sequence of characters defines a
// unique category that is, from the logger's perspective, a "peer" to all
// other categories.  The toolkit does, however, provides several callback
// mechanisms that facilitate customized naming conventions.
//
// In particular, 'ball::LoggerManager' can accept an argument of type
// 'DefaultThresholdsCallback' (see the 'typedef' definition above), which, if
// not null, is invoked to populate the four threshold levels whenever the user
// creates a category having the "default" thresholds.  This component provides
// the function 'loadParentCategoryThresholdValues', which can be used as the
// payload to 'DefaultThresholdsCallback'.  'loadParentCategoryThresholdValues'
// populates its four category threshold level arguments from a specified
// "child" category name and a 'char' delimiter by searching the category
// registry for a name that is "the most proximate parent category", assuming a
// hierarchical naming convention that uses the delimiter character.  If such a
// parent category is found, the "child" category will receive the threshold
// levels of the parent.  If no such parent exists, the new category will
// receive the standard "default" thresholds that it would have gotten had the
// callback been null.
//
///Usage
///-----
// The following code snippets illustrate how to use this component's
// 'loadParentCategoryThresholdValues' method to allow a newly-created "child"
// category to inherit the logging threshold levels from its most proximate
// parent category (if such a category already exists).  Note that the category
// "hierarchy" is by naming convention only, but that the callback makes it
// simple for the *user* to impose hierarchical meaning on names that are, from
// the *logger's* perspective, peers.  In this example, we will choose the dot
// character ('.') as the hierarchy delimiter; to the logger itself, '.' is not
// special.
//
// To keep this example transparent, we will create and inspect several
// categories within 'main' directly; some categories will be "declared" to be
// "parent" categories, and we will set the threshold levels explicitly, while
// other categories will act as "children", which is to say that they will
// obtain their threshold levels through the callback mechanism.  In a more
// realistic example, there would be no explicit distinction between "parent"
// and "child" categories, but rather as categories are dynamically
// administered by the user, newly created categories would pick up the changes
// made to existing parents.  As a practical matter, beginning of the function
// 'main' constitute the "usage" that the user must master to *install* the
// callback; the rest of this example merely illustrates the *consequences* of
// installing the callback.
//
// Before we can begin logging, we must first create an observer with which to
// initialize the logger manager.  For this example, a 'ball::TestObserver'
// initialized to write to 'stdout' will do just fine.
//..
//   // myapp.cpp
//   int main()
//   {
//       ball::TestObserver testObserver(bsl::cout);
//..
// Now, we load the logger manager 'configuration' with the desired "payload"
// function, 'ball::LoggerFunctorPayloads::loadParentCategoryThresholdValues',
// and use the trailing 'char' argument 'delimiter', set to the value '.',
// which will be bound into the functor and supplied back to the payload on
// each invocation.
//..
//       using namespace bdlf::PlaceHolders;
//
//       ball::LoggerManagerConfiguration configuration;
//       char delimiter = '.';
//       configuration.setDefaultThresholdLevelsCallback(
//           bdlf::BindUtil::bind(
//             &ball::LoggerFunctorPayloads::loadParentCategoryThresholdValues,
//             _1,
//             _2,
//             _3,
//             _4,
//             _5,
//             delimiter));
//..
// We are now ready to initialize the logger manager, using the observer and
// the callback defined above.
//..
//       ball::LoggerManagerScopedGuard guard(&testObserver, configuration);
//..
// The above code is all that the user needs to do to customize the logger to
// "inherit" thresholds from parents.  The rest of this example illustrates the
// consequences of having installed 'myCallback'.  For convenience in what
// follows, we define a reference, 'manager', to the singleton logger manager.
//..
//       ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// We now create two "parent" categories named "EQUITY.MARKET" and
// "EQUITY.GRAPHICS", and give them arbitrary but distinct threshold levels.
// We also set the default levels to distinct values in order to be able to
// verify exactly where "child" levels have come from later on.
//..
//       manager.setDefaultThresholdLevels(128, 96, 64, 32);
//
//       manager.addCategory("EQUITY.MARKET", 127, 95, 63, 31);
//       manager.addCategory("EQUITY.GRAPHICS", 129, 97, 65, 33);
//..
// Note that the call to 'addCategory', which takes the four 'int' threshold
// arguments, does not invoke the callback at all, but rather -- assuming that
// the named category does not yet exist -- sets the thresholds to the
// specified values directly.
//
// We can use the logger manager interface to verify that the levels have been
// set.  First, we use the 'lookupCategory' method to obtain the two parent
// categories (here assigned 'p1' and 'p2').
//..
//       const ball::Category *p1 = manager.lookupCategory("EQUITY.MARKET");
//       const ball::Category *p2 = manager.lookupCategory("EQUITY.GRAPHICS");
//..
// Next, we can use the appropriate 'ball::Category' accessor methods to
// 'assert' the expected results.  Recall that the ordered sequence of levels
// is "Record", "Pass", "Trigger", and "TriggerAll".
//..
//           assert(127 == p1->recordLevel());
//           assert( 95 == p1->passLevel());
//           assert( 63 == p1->triggerLevel());
//           assert( 31 == p1->triggerAllLevel());
//
//           assert(129 == p2->recordLevel());
//           assert( 97 == p2->passLevel());
//           assert( 65 == p2->triggerLevel());
//           assert( 33 == p2->triggerAllLevel());
//..
// Now, we will add several "child" categories using the 'setCategory' method
// taking a single argument, the 'char*' category name.  This method uses the
// callback in determining the "default" threshold levels to use.  The six
// statements are numbered for subsequent discussion.
//..
//       manager.setCategory("EQUITY.MARKET.NYSE");                      // (1)
//       manager.setCategory("EQUITY.MARKET.NASDAQ");                    // (2)
//       manager.setCategory("EQUITY.GRAPHICS.MATH.FACTORIAL");          // (3)
//       manager.setCategory("EQUITY.GRAPHICS.MATH.ACKERMANN");          // (4)
//       manager.setCategory("EQUITY.GRAPHICS.MATH");                    // (5)
//       manager.setCategory("EQUITY");                                  // (6)
//..
// Note that all six calls to 'setCategory' will succeed in adding new
// categories to the registry.  Calls (1)-(5) will "find" their parent's names
// and "inherit" the parent's levels.  Call (6), however, will not find a
// parent category, and so will receive the default threshold levels, just as
// if there were no callback installed.
//
// Note also that, although in this "static" (i.e., unadministered) example
// there is no significance to the order in which the above categories are
// created, in general (e.g., when categories are being dynamically
// administered) the order of creation *does* matter.  If line (5) were
// executed before line (4) then the call on line (4) would find the
// "EQUITY.GRAPHICS.MATH" category as its "parent" and inherit those threshold
// levels.  If, before line (4) executed, the thresholds of
// "EQUITY.GRAPHICS.MATH" were changed, then "EQUITY.GRAPHICS.MATH.FACTORIAL"
// and "EQUITY.GRAPHICS.MATH.ACKERMANN" would have different threshold levels
// despite their equivalent standing in the category hierarchy.
//
// Let us now verify some of the 24 threshold levels that have been set by the
// above calls.  We will verify the results of lines (1), (3), and (6) above.
//..
//       const ball::Category *c1, *c3, *c6;
//
//       c1 =  manager.lookupCategory("EQUITY.MARKET.NYSE");
//           assert(127 == c1->recordLevel());
//           assert( 95 == c1->passLevel());
//           assert( 63 == c1->triggerLevel());
//           assert( 31 == c1->triggerAllLevel());
//
//       c3 =  manager.lookupCategory("EQUITY.GRAPHICS.MATH.FACTORIAL");
//           assert(129 == c3->recordLevel());
//           assert( 97 == c3->passLevel());
//           assert( 65 == c3->triggerLevel());
//           assert( 33 == c3->triggerAllLevel());
//
//       c6 =  manager.lookupCategory("EQUITY");
//           assert(128 == c6->recordLevel());
//           assert( 96 == c6->passLevel());
//           assert( 64 == c6->triggerLevel());
//           assert( 32 == c6->triggerAllLevel());
//
//       return 0;
//   }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

namespace BloombergLP {

namespace ball {
                    // ============================
                    // struct LoggerFunctorPayloads
                    // ============================

struct LoggerFunctorPayloads {
    // This 'struct' provides a namespace for a suite of utility functions,
    // each of which may be used as function body for an appropriate 'bdef'
    // callback functor within 'ball_loggermanager'.

    // CLASS METHODS
    static
    void loadParentCategoryThresholdValues(int                *recordLevel,
                                           int                *passLevel,
                                           int                *triggerLevel,
                                           int                *triggerAllLevel,
                                           const char         *categoryName,
                                           char                delimiter);
        // Load into the specified 'recordLevel', 'passLevel', 'triggerLevel',
        // and 'triggerAllLevel' the respective threshold levels of the
        // category in the (singleton) registry of the 'ball' logger that is
        // the most proximate parent category among existing hierarchically
        // named categories in the registry, if such a parent category exists,
        // or the default thresholds otherwise; use the specified 'delimiter'
        // to define hierarchical category names.  The behavior is undefined
        // unless the 'LoggerManager' singleton has been initialized.
};
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
