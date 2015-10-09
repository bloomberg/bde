// ball_loggermanagerconfiguration.h                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_LOGGERMANAGERCONFIGURATION
#define INCLUDED_BALL_LOGGERMANAGERCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a constrained-attribute class for the logger manager.
//
//@CLASSES:
// ball::LoggerManagerConfiguration: configuration spec for a logger manager
//
//@SEE_ALSO: ball_loggermanagerdefaults,
//
//@DESCRIPTION:  This component provides a constrained-attribute class that
// contains a set of attributes (objects and parameters) of particular use to
// logger managers.  The constraints are actively maintained by the class.  In
// particular, the "set" methods for constrained values will fail if their
// arguments are not consistent with the constraints.  Also, the constructor
// does not take any constrained arguments, but rather sets those values to
// valid defaults unconditionally.  This behavior avoids "silent failures",
// since the constructor cannot explicitly return a status value.
//
// The attributes of a 'ball::LoggerManagerConfiguration' object and their
// constraints are given, respectively, in two tables below.  Before listing
// the attributes, however, we list the types that are provided by the
// 'ball::LoggerManagerConfiguration' class to simply the definition of some of
// the attributes:
//..
//  TYPE                                         'typedef' alias
//  ------------------------------------------   ------------------------------
//  bsl::function<void(ball::UserFields*, const ball::UserFieldsSchema&)>
//                                               UserFieldsPopulatorCallback;
//
//  bsl::function<void(bsl::string *, const char *)>
//                                               CategoryNameFilterCallback
//
//  bsl::function<void(int *, int *, int *, int *, const char *)>
//                                               DefaultThresholdLevelsCallback
//..
// The attributes contained by a 'ball::LoggerManagerConfiguration' object
// follow:
//..
//  TYPE                                         NAME
//  ------------------------------------------   ------------------------------
//  ball::LoggerManagerDefaults                  defaults
//
//  ball::UserFieldsSchema                       userSchema
//
//  UserFieldsPopulatorCallback                  userFieldsPopulatorCallback
//
//  CategoryNameFilterCallback                   categoryNameFilterCallback
//
//  DefaultThresholdLevelsCallback               defaultThresholdLevelsCallback
//
//  LogOrder                                     logOrder
//
//  TriggerMarkers                               triggerMarkers
//
//  NAME                            DESCRIPTION
//  -------------------             -------------------------------------------
//  defaults                        constrained defaults for buffer size and
//                                  thresholds
//
//  userSchema                      defines optional user-defined fields in a
//                                  log record
//
//  userFieldsPopulatorCallback     populates user-defined fields in a log
//                                  record
//
//  categoryNameFilterCallback      invoked on category names, e.g., to re-map
//                                  characters
//
//  defaultThresholdLevelsCallback  sets category severity threshold levels (by
//                                  default)
//
//  logOrder                        defines the order in which log messages are
//                                  published for Trigger and Trigger-All
//                                  events; default is LIFO (last-in first-out)
//
//  triggerMarkers                  defines whether text will be written to the
//                                  log to indicate whether a series of log
//                                  records were logged due to either a Trigger
//                                  or Trigger-All event; if this attribute is
//                                  'e_BEGIN_END_MARKERS', then
//                                  "BEGIN RECORD DUMP" and "END RECORD DUMP"
//                                  will be written before and after each
//                                  sequence of records logged due to a Trigger
//                                  or Trigger-All event; default is
//                                  'e_BEGIN_END_MARKERS'.
//..
// The constraints are as follows:
//..
//    NAME                             CONSTRAINT
//  +--------------------------------+--------------------------------+
//  | defaults                       | (a constrained-attribute type) |
//  +--------------------------------+--------------------------------+
//  | userSchema                     | (single attribute)             |
//  | userFieldsPopulatorCallback    |                                |
//  +--------------------------------+--------------------------------+
//  | categoryNameFilterCallback     | (none)                         |
//  +--------------------------------+--------------------------------+
//  | defaultThresholdLevelsCallback | (none)                         |
//  +--------------------------------+--------------------------------+
//  | logOrder                       | (none)                         |
//  +--------------------------------+--------------------------------+
//  | triggerMarkers                 | (none)                         |
//  +--------------------------------+--------------------------------+
//..
// For convenience, the 'ball::LoggerManagerConfiguration' interface contains
// manipulators and accessors to configure and inspect the value of its
// contained 'ball::LoggerManagerDefaults' object; these methods are identical
// to those of 'ball::LoggerManagerDefaults'.  See the
// 'ball_loggermanagerdefaults' component for details on the defaults and
// their constraints.
//
///Thread Safety
///-------------
// This constrained-attribute component is *thread-safe* but not
// *thread-enabled*, and requires explicit synchronization in the user space.
// Note that any of the contained user-defined callbacks may be invoked from
// *any* thread, and the user must account for that.
//
///Usage
///-----
// The following snippets of code illustrate how to use a
// 'ball::LoggerManagerConfiguration' object.
//
// First we define a simple function that will serve as a
// 'UserFieldsPopulatorCallback', a callback that will be invoked for each
// logged message to populate user defined fields for the log record:
//..
//  void exampleCallback(ball::UserFields              *fields,
//                       const ball::UserFieldsSchema&  schema)
//  {
//    // Verify the schema matches this callbacks expectations.
//
//    BSLS_ASSERT(1                             == schema.length());
//    BSLS_ASSERT(ball::UserFieldType::e_STRING == schema.type(0));
//    BSLS_ASSERT("example"                     == schema.name(0));
//
//    fields->appendString("example user field value");
//  }
//..
// Next, we define a function 'inititialize' in which we will create and
// configure a 'ball::LoggerManagerConfiguration' object (see
// {'ball_loggermanager'} for an example of how to create the logger-manager
// singleton object):
//..
//  void initializeConfiguration(bool verbose)
//  {
//    ball::LoggerManagerConfiguration config;
//
//..
// Here, we configure the default record buffer size, logger buffer size, and
// the various logging thresholds (see {'ball_loggermanager'} for more
// information on the various threshold levels):
//..
//    if (0 != config.setDefaultRecordBufferSizeIfValid(32768) ||
//        0 != config.setDefaultLoggerBufferSizeIfValid(1024)  ||
//        0 != config.setDefaultThresholdLevelsIfValid(0, 64, 0, 0)) {
//       bsl::cerr << "Failed set log configuration defaults." << bsl::endl;
//       bsl::exit(-1);
//    }
//
//    ASSERT(32768 == config.defaultRecordBufferSize());
//    ASSERT( 1024 == config.defaultLoggerBufferSize());
//    ASSERT(    0 == config.defaultRecordLevel());
//    ASSERT(   64 == config.defaultPassLevel());
//    ASSERT(    0 == config.defaultTriggerLevel());
//    ASSERT(    0 == config.defaultTriggerAllLevel());
//..
// Next, we create a user field schema, that will be used with the user field
// populator callback 'exampleCallback':
//..
//    ball::UserFieldsSchema schema;
//    schema.appendFieldDescription("example", ball::UserFieldType::e_STRING);
//..
// Now, we set populate the configuration options in our schema (note that the
// following methods cannot fail and return 'void'):
//..
//    config.setUserFieldsSchema(schema, &exampleCallback);
//    config.setLogOrder(ball::LoggerManagerConfiguration::e_FIFO);
//    config.setTriggerMarkers(
//                          ball::LoggerManagerConfiguration::e_NO_MARKERS);
//..
// Then, we verify the options are configured correctly:
//..
//    ASSERT(schema == config.userFieldsSchema());
//    ASSERT(ball::LoggerManagerConfiguration::e_FIFO == config.logOrder());
//    ASSERT(ball::LoggerManagerConfiguration::e_NO_MARKERS
//                                                 == config.triggerMarkers());
//..
// Finally, we print the configuration value to 'stdout' and return:
//..
//    if (verbose) {
//      bsl::cout << config << bsl::endl;
//    }
//  }
//..
// This produces the following (multi-line) output:
//..
//  [
//      Defaults:
//      [
//          recordBufferSize : 32768
//          loggerBufferSize : 1024
//          recordLevel      : 0
//          passLevel        : 64
//          triggerLevel     : 0
//          triggerAllLevel  : 0
//      ]
//      User Fields Schema:
//      [
//          example = STRING
//      ]
//      User Fields Populator functor is not null
//      Category Name Filter functor is null
//      Default Threshold Callback functor is null
//      Logging order is FIFO
//      Trigger markers are NO_MARKERS
//  ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_LOGGERMANAGERDEFAULTS
#include <ball_loggermanagerdefaults.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDSSCHEMA
#include <ball_userfieldsschema.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace ball {

class UserFields;

                    // ================================
                    // class LoggerManagerConfiguration
                    // ================================

class LoggerManagerConfiguration {
    // This class provides constrained configuration parameters for a logger
    // manager.  The constraints are maintained as class invariants; it is not
    // possible to obtain an invalid object through this interface.
    //
    // This class support *in-core* *value-semantic* operations, including
    // copy construction, assignment, and equality comparison; 'ostream'
    // printing is supported, but in a modified form to handle functors, and
    // 'bdex' serialization cannot be supported at all.  (A precise operational
    // definition of when two instances have the same (in-core) value can be
    // found in the description of 'operator==' for the class.)  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void(ball::UserFields *,
                               const ball::UserFieldsSchema&)>
                                                   UserFieldsPopulatorCallback;
        // 'UserFieldsPopulatorCallback' is the type of a user-supplied
        // callback functor used to populate the user-defined fields in each
        // log record.  Note that the user-defined fields of each record must
        // be type-consistent with the 'UserFieldsSchema' of the user
        // populator callback.


    typedef bsl::function<void(bsl::string *, const char *)>
                                                    CategoryNameFilterCallback;
        // 'CategoryNameFilterCallback' is the type of the user-supplied
        // functor that translates external category names to internal names.

    typedef bsl::function<void(int *, int *, int *, int *, const char *)>
                                                DefaultThresholdLevelsCallback;
        // 'DefaultThresholdLevelsCallback' is the type of the functor that
        // determines default threshold levels for categories added to the
        // registry by the 'setCategory(const char *)' method.

    enum LogOrder {
        // The 'LogOrder' enumeration defines the order in which messages will
        // be published to the underlying observer in the case of Trigger and
        // Trigger-All events.

        e_FIFO,  // oldest logged messages are published first
        e_LIFO   // newest logged messages are published first

    };

    enum TriggerMarkers {
        // The 'TriggerMarkers' enumeration defines whether text will be
        // written to the log to indicate whether a series of log records were
        // logged due to either a Trigger or Trigger-All event.  If this
        // attribute is 'e_BEGIN_END_MARKERS', then "BEGIN RECORD DUMP" and
        // "END RECORD DUMP" will be written before and after each sequence of
        // records logged due to a Trigger or Trigger-All event.  The default
        // value of this attribute is 'e_BEGIN_END_MARKERS'.

        e_NO_MARKERS,        // don't print any markers

        e_BEGIN_END_MARKERS  // print "BEGIN RECORD DUMP" and
                             // "END RECORD DUMP" markers (default)

    };

  private:
    // DATA
    LoggerManagerDefaults d_defaults;             // default buffer size for
                                                  // logger (for macros) and
                                                  // set of constrained default
                                                  // severity threshold levels
                                                  // for logger manager

    ball::UserFieldsSchema
                          d_userFieldsSchema;     // describes the fields
                                                  // returned by
                                                  // 'd_userPopulatorCallback'

    UserFieldsPopulatorCallback
                          d_userPopulator;        // user callback to add
                                                  // optional user-defined
                                                  // fields to a log record

    CategoryNameFilterCallback
                          d_categoryNameFilter;   // callback to modify input
                                                  // category names for
                                                  // internal storage

    DefaultThresholdLevelsCallback
                          d_defaultThresholdsCb;  // callback to supply default
                                                  // severity thresholds for
                                                  // categories

    LogOrder              d_logOrder;             // logging order

    TriggerMarkers        d_triggerMarkers;       // trigger marker

    bslma::Allocator     *d_allocator_p;          // memory allocator (held,
                                                  // not owned)

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&,
                                    const LoggerManagerConfiguration&);

    friend bool operator==(const LoggerManagerConfiguration&,
                           const LoggerManagerConfiguration&);

  public:
    // CLASS METHODS
    static bool isValidDefaultRecordBufferSize(int numBytes);
        // Return 'true' if the specified 'numBytes' is a valid default-logger
        // record-buffer size value, and 'false' otherwise.  'numBytes' is
        // valid if '0 < numBytes'.

    static bool isValidDefaultLoggerBufferSize(int numBytes);
        // Return 'true' if the specified 'numBytes' is a valid default
        // logger-message-buffer size value, and 'false' otherwise.  'numBytes'
        // is valid if '0 < numBytes'.

    static bool areValidDefaultThresholdLevels(int recordLevel,
                                               int passLevel,
                                               int triggerLevel,
                                               int triggerAllLevel);
        // Return 'true' if each of the specified 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' values is a valid default
        // severity threshold level, and 'false' otherwise.  Valid severity
        // threshold levels are in the range '[0 .. 255]'.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(LoggerManagerConfiguration,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit LoggerManagerConfiguration(bslma::Allocator *basicAllocator = 0);
        // Create a logger manager configuration constrained-attribute object
        // having valid default values for all attributes.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    LoggerManagerConfiguration(
                        const LoggerManagerConfiguration&  original,
                        bslma::Allocator                  *basicAllocator = 0);
        // Create a logger manager configuration constrained-attribute object
        // having the in-core value of the specified 'original' object.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~LoggerManagerConfiguration();
        // Destroy this logger manager configuration constrained-attribute
        // object.

    // MANIPULATORS
    LoggerManagerConfiguration&
                         operator=(const LoggerManagerConfiguration& rhs);
        // Assign to this logger manager configuration constrained-attribute
        // object the in-core value of the specified 'rhs' object.

    void setDefaultValues(const LoggerManagerDefaults& defaults);
        // Set the default values attribute of this object to the specified
        // 'defaults'.  Note that, since 'defaults' is itself a
        // constrained-attribute object, this method cannot fail.

    int setDefaultRecordBufferSizeIfValid(int numBytes);
        // Set the default-logger record-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object to the specified
        // 'numBytes' if '0 < numBytes'.  Return 0 on success, and a non-zero
        // value otherwise with no effect on this object.

    int setDefaultLoggerBufferSizeIfValid(int numBytes);
        // Set the default logger-message-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object to the specified
        // 'numBytes' if '0 < numBytes'.  Return 0 on success, and a non-zero
        // value otherwise with no effect on this object.

    int setDefaultThresholdLevelsIfValid(int passLevel);
        // Set the passthrough severity threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object to the specified
        // 'passLevel', if it is in the range '[0 .. 255]', and set all the
        // other threshold levels (recordLevel, triggerLevel, triggerAllLevel)
        // to 0.  Return 0 on success, and a non-zero value otherwise with no
        // effect on this object.

    int setDefaultThresholdLevelsIfValid(int recordLevel,
                                         int passLevel,
                                         int triggerLevel,
                                         int triggerAllLevel);
        // Set the quadruplet of default severity threshold level attributes of
        // the 'LoggerManagerDefaults' attribute of this object to the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' values if each level is in the range '[0 .. 255]'.
        // Return 0 on success, and a non-zero value otherwise with no effect
        // on this object.

    void setUserFieldsSchema(
                       const ball::UserFieldsSchema       fieldDescriptions,
                       const UserFieldsPopulatorCallback& populatorCallback);
        // Set the user-defined-fields attributes of this object such that the
        // specified 'populatorCallback' will be invoked and supplied the
        // specified 'fieldDescriptions'.  Note that this method cannot fail
        // per se, but it is the user's responsibility to make sure that
        // 'populatorCallback' can populate a 'ball::UserFields' object in a
        // way consistent with the 'fieldDescriptions'.

    void setCategoryNameFilterCallback(
                                 const CategoryNameFilterCallback& nameFilter);
        // Set the category name-filter callback functor attribute of this
        // object to the specified 'nameFilter'.  Note that this method cannot
        // fail, and that 'nameFilter' can be a "null" (i.e., unpopulated)
        // functor object.

    void setDefaultThresholdLevelsCallback(
                           const DefaultThresholdLevelsCallback& thresholdsCb);
        // Set the default thresholds callback functor attribute of this object
        // to the specified 'thresholdsCb'.  Note that this method cannot fail,
        // and that 'thresholdsCb' can be a "null" (i.e., unpopulated) functor
        // object.

    void setLogOrder(LogOrder value);
        // Set the log order attribute of this object to the specified 'value'.

    void setTriggerMarkers(TriggerMarkers value);
        // Set the trigger marker attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    const LoggerManagerDefaults& defaults() const;
        // Return a reference to the non-modifiable defaults object attribute
        // of this object.

    int defaultRecordBufferSize() const;
        // Return the default-logger record-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultLoggerBufferSize() const;
        // Return the default logger-message-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultRecordLevel() const;
        // Return the default Record threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultPassLevel() const;
        // Return the default Pass threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultTriggerLevel() const;
        // Return the default Trigger threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultTriggerAllLevel() const;
        // Return the default Trigger-All threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.


    const ball::UserFieldsSchema& userFieldsSchema() const;
        // Return a reference to the non-modifiable descriptors for user
        // fields.

    const UserFieldsPopulatorCallback& userFieldsPopulatorCallback() const;
        // Return a reference to the non-modifiable user populator functor
        // attribute of this object.

    const CategoryNameFilterCallback& categoryNameFilterCallback() const;
        // Return a reference to the non-modifiable category name filter
        // functor attribute of this object.

    const DefaultThresholdLevelsCallback&
                                        defaultThresholdLevelsCallback() const;
        // Return a reference to the non-modifiable default severity-level
        // thresholds functor attribute of this object.

    LogOrder logOrder() const;
        // Return the log order attribute of this object.  See attributes
        // description for effects of the log order.

    TriggerMarkers triggerMarkers() const;
        // Return the trigger marker attribute of this object.  See attributes
        // description for effects of the trigger markers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format a reasonable representation of this object to the specified
        // output 'stream' at the (absolute value of) the optionally specified
        // indentation 'level' and return a reference to 'stream'.  If 'level'
        // is specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress all indentation AND format
        // the entire output on one line.  If 'stream' is not valid on entry,
        // this operation has no effect.  The format will indicate whether the
        // contained functors are or are not "null", but will not otherwise
        // attempt to represent functor "values".
};

// FREE OPERATORS
bool operator==(const LoggerManagerConfiguration& lhs,
                const LoggerManagerConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same (in-core) value, and 'false' otherwise.  Two attribute objects
    // objects have the same in-core value if each respective attribute has the
    // same in-core value.  Note that functor attributes must have identically
    // the same representation to have the same in-core value.

bool operator!=(const LoggerManagerConfiguration& lhs,
                const LoggerManagerConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same (in-core) value, and 'false' otherwise.  Two attribute
    // objects do not have the same in-core value if one or more respective
    // attributes differ in in-core values.  Note that functor attributes will
    // have different in-core values if their representations are not
    // identical.

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const LoggerManagerConfiguration& configuration);
    // Write a reasonable representation of the specified 'configuration'
    // object to the specified output 'stream', indicating whether the
    // contained functors are or are not "null".

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

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
