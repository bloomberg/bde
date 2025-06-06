// ball_thresholdaggregate.h                                          -*-C++-*-
#ifndef INCLUDED_BALL_THRESHOLDAGGREGATE
#define INCLUDED_BALL_THRESHOLDAGGREGATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an aggregate of the four logging threshold levels.
//
//@CLASSES:
//  ball::ThresholdAggregate: aggregate of four logging threshold levels
//  ball::ThresholdAggregateUtil: `ball::ThresholdAggregate` helpers
//
//@SEE_ALSO: ball_severity
//
//@DESCRIPTION: This component implements a class, `ball::ThresholdAggregate`,
// that aggregates four threshold levels: record level, pass-through level,
// trigger level, and trigger-all level.  Each of these levels must in the
// range `[0 .. 255]`, and represents a threshold which, if exceeded, will
// invoke a certain action.  `ball::ThresholdAggregateUtil` provides helper
// methods for packing/unpacking `ball::ThresholdAggregate` objects to/from
// integers.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following code fragment illustrates some of the basic operations on
// threshold levels provided by `ball::ThresholdAggregate`.
// ```
//   ball::ThresholdAggregate levels(192, 160, 128, 96);
//   assert(192 == levels.recordLevel());
//   assert(160 == levels.passLevel());
//   assert(128 == levels.triggerLevel());
//   assert( 96 == levels.triggerAllLevel());
//
//   levels.setLevels(160, 128, 96, 64);
//   assert(160 == levels.recordLevel());
//   assert(128 == levels.passLevel());
//   assert( 96 == levels.triggerLevel());
//   assert( 64 == levels.triggerAllLevel());
// ```

#include <balscm_version.h>

#include <bsl_cstring.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ball {

                        // ========================
                        // class ThresholdAggregate
                        // ========================

/// This class implements an aggregate of the four logging threshold levels.
class ThresholdAggregate {

    // DATA
    unsigned char d_recordLevel;      // record
    unsigned char d_passLevel;        // pass
    unsigned char d_triggerLevel;     // trigger
    unsigned char d_triggerAllLevel;  // trigger-all

    // FRIENDS
    friend bool operator==(const ThresholdAggregate&,
                           const ThresholdAggregate&);
  public:
    // CLASS METHODS

    /// Return a hash value calculated from the specified threshold
    /// `aggregate` using the specified `size` as the number of slots.  The
    /// hash value is guaranteed to be in the range `[0, size - 1]`.  The
    /// behavior is undefined unless `0 < size`.
    static int hash(const ThresholdAggregate& aggregate, int size);

    /// Return `true` if each of the specified `recordLevel`, `passLevel`,
    /// `triggerLevel`, and `triggerAllLevel` threshold values is in the
    /// range `[0 .. 255]`, and `false` otherwise.
    static bool areValidThresholdLevels(int recordLevel,
                                        int passLevel,
                                        int triggerLevel,
                                        int triggerAllLevel);

    /// Return the numerically greatest of the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` values.
    static int maxLevel(int recordLevel,
                        int passLevel,
                        int triggerLevel,
                        int triggerAllLevel);

    /// Return the numerically greatest of the four logging threshold levels
    /// held by the specified threshold `aggregate`.
    static int maxLevel(const ThresholdAggregate& aggregate);

    // CREATORS

    /// Create a threshold aggregate whose threshold levels are all 0.
    ThresholdAggregate();

    /// Create a threshold aggregate whose threshold levels are the
    /// specified `recordLevel`, `passLevel`, `triggerLevel`, and
    /// `triggerAllLevel`, respectively.  The behavior is undefined if any
    /// threshold level is not in the range `[0 .. 255]`.
    ThresholdAggregate(int recordLevel,
                       int passLevel,
                       int triggerLevel,
                       int triggerAllLevel);

    /// Create a threshold aggregate having the same value as that of the
    /// specified `original` threshold aggregate.
    ThresholdAggregate(const ThresholdAggregate& original);

    /// Destroy this object.
    //! ~ThresholdAggregate() = default;

    // MANIPULATORS

    /// Assign the value of the specified `rhs` threshold aggregate to this
    /// aggregate, and return a reference to this modifiable aggregate.
    ThresholdAggregate& operator=(const ThresholdAggregate& rhs);

    /// Set the threshold levels of this threshold aggregate to the
    /// specified `recordLevel`, `passLevel`, `triggerLevel`, and
    /// `triggerAllLevel` values, respectively, if each of the specified
    /// values is in the range `[0 .. 255]`.  Return 0 on success, and a
    /// non-zero value otherwise (with no effect on the threshold levels of
    /// this aggregate).
    int setLevels(int recordLevel,
                  int passLevel,
                  int triggerLevel,
                  int triggerAllLevel);

    /// Set the record level of this threshold aggregate to the specified
    /// `recordLevel`.
    void setRecordLevel(int recordLevel);

    /// Set the pass level of this threshold aggregate to the specified
    /// `passLevel`.
    void setPassLevel(int passLevel);

    /// Set the trigger level of this threshold aggregate to the specified
    /// `triggerLevel`.
    void setTriggerLevel(int triggerLevel);

    /// Set the trigger-all level of this threshold aggregate to the
    /// specified `triggerAllLevel`.
    void setTriggerAllLevel(int triggerAllLevel);

    // ACCESSORS

    /// Return the record level of this threshold aggregate.
    int recordLevel() const;

    /// Return the pass level of this threshold aggregate.
    int passLevel() const;

    /// Return the trigger level of this threshold aggregate.
    int triggerLevel() const;

    /// Return the trigger-all level of this threshold aggregate.
    int triggerAllLevel() const;

    /// Format this object to the specified output `stream` at the
    /// (absolute value of) the optionally specified indentation `level`
    /// and return a reference to `stream`.  If `level` is specified,
    /// optionally specify `spacesPerLevel`, the number of spaces per
    /// indentation level for this and all of its nested objects.  If
    /// `level` is negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, format the entire output on one line,
    /// suppressing all but the initial indentation (as governed by
    /// `level`).  If `stream` is not valid on entry, this operation has no
    /// effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

                      // ============================
                      // class ThresholdAggregateUtil
                      // ============================

/// This class provides helper methods for working with
/// `ball::ThresholdAggregate` objects.
class ThresholdAggregateUtil {
  public:
    // CLASS METHODS

    /// Return an unsigned integer holding a packed representation of the
    /// specified `thresholds`.
    static unsigned pack(ThresholdAggregate thresholds);

    /// Return a threshold aggregate having the value equivalent to one that
    /// was converted into the specified `packed` representation.  The behavior
    /// is undefined unless the value for `packed` originated from a call to
    /// `pack` in the same process instance.
    static ThresholdAggregate unpack(unsigned packed);
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` threshold aggregates have
/// the same value, and `false` otherwise.  Two threshold aggregates have
/// the same value if all four of their respective threshold levels are the
/// same.
bool operator==(const ThresholdAggregate& lhs, const ThresholdAggregate& rhs);

/// Return `true` if the specified `lhs` and `rhs` threshold aggregates do
/// not have the same value, and `false` otherwise.  Two threshold
/// aggregates do not have the same value if any of their four respective
/// threshold levels differ.
bool operator!=(const ThresholdAggregate& lhs, const ThresholdAggregate& rhs);

/// Write the value of the specified threshold `aggregate` to the specified
/// output `stream` and return a reference to the modifiable `stream`.
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const ThresholdAggregate& aggregate);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class ThresholdAggregate
                        // ------------------------

// ACCESSORS USED INLINE BY OTHER METHODS
inline
int ThresholdAggregate::recordLevel() const
{
    return d_recordLevel;
}

inline
int ThresholdAggregate::passLevel() const
{
    return d_passLevel;
}

inline
int ThresholdAggregate::triggerLevel() const
{
    return d_triggerLevel;
}

inline
int ThresholdAggregate::triggerAllLevel() const
{
    return d_triggerAllLevel;
}

// CLASS METHODS
inline
bool ThresholdAggregate::areValidThresholdLevels(int recordLevel,
                                                 int passLevel,
                                                 int triggerLevel,
                                                 int triggerAllLevel)
{
    enum { k_BITS_PER_CHAR = 8 };

    return !((recordLevel | passLevel | triggerLevel | triggerAllLevel)
             >> k_BITS_PER_CHAR);
}

inline
int ThresholdAggregate::maxLevel(const ThresholdAggregate& aggregate)
{
    return maxLevel(aggregate.recordLevel(),  aggregate.passLevel(),
                    aggregate.triggerLevel(), aggregate.triggerAllLevel());
}

// CREATORS
inline
ThresholdAggregate::ThresholdAggregate()
: d_recordLevel(0)
, d_passLevel(0)
, d_triggerLevel(0)
, d_triggerAllLevel(0)
{
}

inline
ThresholdAggregate::ThresholdAggregate(int recordLevel,
                                       int passLevel,
                                       int triggerLevel,
                                       int triggerAllLevel)
: d_recordLevel(static_cast<unsigned char>(recordLevel))
, d_passLevel(static_cast<unsigned char>(passLevel))
, d_triggerLevel(static_cast<unsigned char>(triggerLevel))
, d_triggerAllLevel(static_cast<unsigned char>(triggerAllLevel))
{
}

inline
ThresholdAggregate::ThresholdAggregate(const ThresholdAggregate& original)
: d_recordLevel(original.d_recordLevel)
, d_passLevel(original.d_passLevel)
, d_triggerLevel(original.d_triggerLevel)
, d_triggerAllLevel(original.d_triggerAllLevel)
{
}

// MANIPULATORS
inline
void ThresholdAggregate::setRecordLevel(int recordLevel)
{
    d_recordLevel = static_cast<unsigned char>(recordLevel);
}

inline
void ThresholdAggregate::setPassLevel(int passLevel)
{
    d_passLevel = static_cast<unsigned char>(passLevel);
}

inline
void ThresholdAggregate::setTriggerLevel(int triggerLevel)
{
    d_triggerLevel = static_cast<unsigned char>(triggerLevel);
}

inline
void ThresholdAggregate::setTriggerAllLevel(int triggerAllLevel)
{
    d_triggerAllLevel = static_cast<unsigned char>(triggerAllLevel);
}

                      // ----------------------------
                      // class ThresholdAggregateUtil
                      // ----------------------------

// CLASS METHODS
inline
unsigned ThresholdAggregateUtil::pack(ThresholdAggregate thresholds)
{
    unsigned char x[4] = {
        static_cast<unsigned char>(thresholds.recordLevel()),
        static_cast<unsigned char>(thresholds.passLevel()),
        static_cast<unsigned char>(thresholds.triggerLevel()),
        static_cast<unsigned char>(thresholds.triggerAllLevel())
    };
    unsigned packed;
    memcpy(&packed, x, sizeof(x));
    return packed;
}

inline
ThresholdAggregate ThresholdAggregateUtil::unpack(unsigned packed)
{
    unsigned char x[4];
    memcpy(x, &packed, sizeof(x));
    return ThresholdAggregate(x[0], x[1], x[2], x[3]);
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const ThresholdAggregate& lhs,
                      const ThresholdAggregate& rhs)
{
    return lhs.d_recordLevel     == rhs.d_recordLevel
        && lhs.d_passLevel       == rhs.d_passLevel
        && lhs.d_triggerLevel    == rhs.d_triggerLevel
        && lhs.d_triggerAllLevel == rhs.d_triggerAllLevel;
}

inline
bool ball::operator!=(const ThresholdAggregate& lhs,
                      const ThresholdAggregate& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&             stream,
                               const ThresholdAggregate& aggregate)
{
    return aggregate.print(stream, 0, -1);
}

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
