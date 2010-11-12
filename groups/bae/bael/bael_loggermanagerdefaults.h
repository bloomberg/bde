// bael_loggermanagerdefaults.h                                       -*-C++-*-
#ifndef INCLUDED_BAEL_LOGGERMANAGERDEFAULTS
#define INCLUDED_BAEL_LOGGERMANAGERDEFAULTS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide constrained default attributes for the logger manager.
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@CLASSES:
// bael_LoggerManagerDefaults: default parameters for logger manager
//
//@SEE_ALSO: bael_loggermanagerconfiguration
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
// The attributes contained by a 'bael_LoggerManagerDefaults' object and
// the attribute constraints are given, respectively, in two tables below.
// The attributes are as follows:
//..
//   TYPE          NAME                DESCRIPTION
//   -----------   ----------------    -------------------------------------
//   int           recordBufferSize    size in bytes of *default* logger's
//                                     record buffer
//   int           loggerBufferSize    default size in bytes of *each*
//                                     logger's "scratch" buffer (for macros)
//   char          recordLevel         default record severity level
//   char          passLevel           default passthrough severity level
//   char          triggerLevel        default trigger severity level
//   char          triggerAllLevel     default triggerAll severity level
//..
// The constraints are as follows:
//..
//    NAME                 CONSTRAINT
//   +--------------------+---------------------------------------------+
//   | recordBufferSize   | 1 <= recordBufferSize                       |
//   +--------------------+---------------------------------------------+
//   | loggerBufferSize   | 1 <= loggerBufferSize                       |
//   +--------------------+---------------------------------------------+
//   | recordLevel        | 0 <= recordLevel     <= 255                 |
//   | passLevel          | 0 <= passLevel       <= 255                 |
//   | triggerLevel       | 0 <= triggerLevel    <= 255                 |
//   | triggerAllLevel    | 0 <= triggerAllLevel <= 255                 |
//   +--------------------+---------------------------------------------+
//..
// Although the numerical constraints on the four severity levels are
// independent of one another, they are treated as logically coupled, and must
// be set as a group.  If any one value cannot be successfully set, then the
// whole set operation fails with no effect on the object value.  This may be
// viewed as an auxiliary constraint.
//
///Thread Safety
///-------------
// This constrained-attribute component is *thread-safe* but not
// *thread-enabled*, and requires explicit synchronization in the user space.
//
///Design Note
///-----------
// This component provides a BDE constrained-attribute type designed to be
// *useful* to 'bael_LoggerManager'.  However, 'bael_LoggerManagerDefaults' is
// not 'bael_LoggerManagerConfiguration', the actual configuration object used
// by the logger manager.  'bael_LoggerManagerConfiguration' is not a value
// semantic type, because it holds functors.  'bael_LoggerManagerDefaults',
// which is used by 'bael_LoggerManagerConfiguration', may be regarded as a
// factored detail of the actual configuration object.
//
///Usage
///-----
// The following snippets of code illustrate how to use a
// 'bael_LoggerManagerDefaults' object.  First, create a configuration object
// 'lmd'.  Note that it is necessarily configured to valid but unpublished
// defaults.
//..
//    bael_LoggerManagerDefaults lmd;
//..
// Next, set each attribute.  Note that the four severity threshold levels must
// be set atomically (i.e., with a single four-argument "set" method).  Note
// also that each "set" method will fail if the argument or set of arguments is
// not valid, and so each method returns a status value.
//..
//    assert(    0 == lmd.setDefaultRecordBufferSizeIfValid(32768));
//    assert(32768 == lmd.defaultRecordBufferSize());
//
//    assert(   0 == lmd.setDefaultLoggerBufferSizeIfValid(2048));
//    assert(2048 == lmd.defaultLoggerBufferSize());
//
//    assert(  0 == lmd.setDefaultThresholdLevelsIfValid(192, 64, 48, 32));
//    assert(192 == lmd.defaultRecordLevel());
//    assert( 64 == lmd.defaultPassLevel());
//    assert( 48 == lmd.defaultTriggerLevel());
//    assert( 32 == lmd.defaultTriggerAllLevel());
//..
// The configuration object is now validly configured with our choice of
// parameters.  If, however, we attempt to set an invalid configuration, the
// "set" method will fail (with a non-zero return status), and the
// configuration will be left unchanged.
//..
//    assert(  0 != lmd.setDefaultThresholdLevelsIfValid(256, 90, 60, 30));
//    assert(192 == lmd.defaultRecordLevel());
//    assert( 64 == lmd.defaultPassLevel());
//    assert( 48 == lmd.defaultTriggerLevel());
//    assert( 32 == lmd.defaultTriggerAllLevel());
//..
// Finally, we can print the configuration value to 'stdout'.
//..
//    bsl::cout << lmd << bsl::endl;
//..
// This produces the following (multi-line) output:
//..
// [
//     recordBufferSize : 32768
//     loggerBufferSize : 2048
//     recordLevel      : 192
//     passLevel        : 64
//     triggerLevel     : 48
//     triggerAllLevel  : 32
// ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bael_LoggerManagerDefaults {
    // This class provides constrained configuration parameters for a logger
    // manager.  The constraints are maintained as class invariants; it is not
    // possible to obtain an invalid object through this interface.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.)  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

  private:
    int d_recordBufferSize; // size of default logger's record buffer
    int d_loggerBufferSize; // size of logger "scratchpad" buffer (for macros)

    int d_defaultRecordLevel;
    int d_defaultPassLevel;
    int d_defaultTriggerLevel;
    int d_defaultTriggerAllLevel;
        // constrained set of default threshold levels for logger manager

    friend bsl::ostream& operator<<(bsl::ostream&,
                                    const bael_LoggerManagerDefaults&);
    friend bool operator==(const bael_LoggerManagerDefaults&,
                           const bael_LoggerManagerDefaults&);

  public:
    // CLASS METHODS
    static bool isValidDefaultRecordBufferSize(int numBytes);
        // Return 'true' if the specified 'numBytes' is a valid default-logger
        // record-buffer size attribute, and 'false' otherwise.  'numBytes' is
        // valid if 0 < numBytes.

    static bool isValidDefaultLoggerBufferSize(int numBytes);
        // Return 'true' if the specified 'numBytes' is a valid default
        // logger-message-buffer size attribute, and 'false' otherwise.
        // 'numBytes' is valid if 0 < numBytes.

    static bool areValidDefaultThresholdLevels(int recordLevel,
                                               int passLevel,
                                               int triggerLevel,
                                               int triggerAllLevel);
        // Return 'true' if each of the specified 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' values is a valid default
        // severity threshold level attributes, and 'false' otherwise.  Valid
        // severity threshold level values are in the range [0 .. 255].

    static int defaultDefaultRecordBufferSize();
        // Return the implementation-defined default-logger record-buffer size
        // value for this class.

    static int defaultDefaultLoggerBufferSize();
        // Return the implementation-defined default logger-message-buffer size
        // value for this class.

    static int defaultDefaultRecordLevel();
        // Return the implementation-defined default record threshold level
        // value for this class.

    static int defaultDefaultPassLevel();
        // Return the implementation-defined default pass threshold level value
        // for this class.

    static int defaultDefaultTriggerLevel();
        // Return the implementation-defined default trigger threshold level
        // value for this class.

    static int defaultDefaultTriggerAllLevel();
        // Return the implementation-defined default trigger-all threshold
        // value for this class.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    bael_LoggerManagerDefaults();
        // Create a logger manager default-values constrained-attribute object
        // having valid default values for all attributes.

    bael_LoggerManagerDefaults(const bael_LoggerManagerDefaults& original);
        // Create a logger manager default-values constrained-attribute object
        // having the value of the specified 'original' object.

    ~bael_LoggerManagerDefaults();
        // Destroy this logger manager default-values constrained-attribute
        // object.

    // MANIPULATORS
    bael_LoggerManagerDefaults&
    operator=(const bael_LoggerManagerDefaults& rhs);
        // Assign to this logger manager default-values constrained-attribute
        // object the value of the specified 'rhs' object.

    int setDefaultRecordBufferSizeIfValid(int numBytes);
        // Set the default-logger record-buffer size attribute of this object
        // to the specified 'numBytes' if 0 < numBytes.  Return 0 on success,
        // and a non-zero value (with no effect on the state of this object)
        // otherwise.

    int setDefaultLoggerBufferSizeIfValid(int numBytes);
        // Set the default logger-message-buffer size attribute of this object
        // to the specified 'numBytes' if 0 < numBytes.  Return 0 on success,
        // and a non-zero value (with no effect on the state of this object)
        // otherwise.

    int setDefaultThresholdLevelsIfValid(int recordLevel,
                                         int passLevel,
                                         int triggerLevel,
                                         int triggerAllLevel);
        // Set the quadruplet of default severity threshold level attributes of
        // this object to the specified 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' values if each level is in the
        // range [0 .. 255].  Return 0 on success, and a non-zero value (with
        // no effect on the state of this object) otherwise.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    int defaultRecordBufferSize() const;
        // Return the default-logger record-buffer size attribute of this
        // object.

    int defaultLoggerBufferSize() const;
        // Return the default logger-message-buffer size attribute of this
        // object.

    int defaultRecordLevel() const;
        // Return the default record threshold level attribute of this object.

    int defaultPassLevel() const;
        // Return the default pass threshold level attribute of this
        // object.

    int defaultTriggerLevel() const;
        // Return the default trigger threshold level attribute of this object.

    int defaultTriggerAllLevel() const;
        // Return the default trigger-all threshold level attribute of this
        // object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, suppress all indentation AND format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

};

// FREE OPERATORS
bool operator==(const bael_LoggerManagerDefaults& lhs,
                const bael_LoggerManagerDefaults& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects objects
    // have the same value if each respective attribute has the same value.

bool operator!=(const bael_LoggerManagerDefaults& lhs,
               const bael_LoggerManagerDefaults& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const bael_LoggerManagerDefaults& defaults);
    // Write the specified 'defaults' object to the specified output 'stream'
    // in a reasonable multi-line format.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// MANIPULATORS
template <class STREAM>
STREAM& bael_LoggerManagerDefaults::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the version
          case 1: {
            int recordBufferSize;
            stream.getInt32(recordBufferSize);
            if (!stream || 0 >= recordBufferSize) {
                stream.invalidate();
                return stream;
            }

            int loggerBufferSize;
            stream.getInt32(loggerBufferSize);
            if (!stream || 0 >= loggerBufferSize) {
                stream.invalidate();
                return stream;
            }

            int recordLevel;
            stream.getInt32(recordLevel);
            if (!stream || 0 > recordLevel || 255 < recordLevel) {
                stream.invalidate();
                return stream;
            }

            int passLevel;
            stream.getInt32(passLevel);
            if (!stream || 0 > passLevel || 255 < passLevel) {
                stream.invalidate();
                return stream;
            }

            int triggerLevel;
            stream.getInt32(triggerLevel);
            if (!stream || 0 > triggerLevel || 255 < triggerLevel) {
                stream.invalidate();
                return stream;
            }

            int triggerAllLevel;
            stream.getInt32(triggerAllLevel);
            if (!stream || 0 > triggerAllLevel || 255 < triggerAllLevel) {
                stream.invalidate();
                return stream;
            }

            d_recordBufferSize       = recordBufferSize;
            d_loggerBufferSize       = loggerBufferSize;
            d_defaultRecordLevel     = recordLevel;
            d_defaultPassLevel       = passLevel;
            d_defaultTriggerLevel    = triggerLevel;
            d_defaultTriggerAllLevel = triggerAllLevel;
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
STREAM& bael_LoggerManagerDefaults::bdexStreamOut(STREAM& stream,
                                                  int     version) const
{
    switch (version) {
      case 1: {
        stream.putInt32(d_recordBufferSize);
        stream.putInt32(d_loggerBufferSize);

        stream.putInt32(d_defaultRecordLevel);
        stream.putInt32(d_defaultPassLevel);
        stream.putInt32(d_defaultTriggerLevel);
        stream.putInt32(d_defaultTriggerAllLevel);
      } break;
    }
    return stream;
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
