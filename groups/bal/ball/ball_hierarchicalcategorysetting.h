// ball_hierarchicalcategorysetting.h                                 -*-C++-*-
#ifndef INCLUDED_BALL_HIERARCHICALCATEGORYSETTING
#define INCLUDED_BALL_HIERARCHICALCATEGORYSETTING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for a name prefix and associated thresholds.
//
//@CLASSES:
//  ball::HierarchicalCategorySetting: contains prefix and its threshold levels
//
//@SEE_ALSO: ball_categorymanager
//
//@DESCRIPTION: This component provides the class
// `ball::HierarchicalCategorySetting` that represents the properties of a
// hierarchical logging category setting with a category name prefix and the 4
// threshold levels.  See {`ball_loggermanager`} for a description of the
// purpose of the various thresholds.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `ball::HierarchicalCategorySetting`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and use a
// `ball::HierarchicalCategorySetting` object to configure logging thresholds
// for categories with a specific name prefix.
//
// First, we create a hierarchical category setting for all categories starting
// with "EQUITY":
// ```
//  ball::HierarchicalCategorySetting setting(
//      "EQUITY",
//      192,  // recordLevel
//      128,  // passLevel
//      96,   // triggerLevel
//      64);  // triggerAllLevel
// ```
//
// Then, we can query the category prefix and threshold levels:
// ```
//  assert(setting.categoryPrefix() == "EQUITY");
//  assert(setting.recordLevel()     == 192);
//  assert(setting.passLevel()       == 128);
//  assert(setting.triggerLevel()    == 96);
//  assert(setting.triggerAllLevel() == 64);
// ```
//
// Finally, we can modify the threshold levels:
// ```
//  int rc = setting.setLevels(160, 96, 64, 32);
//  assert(0 == rc);
//  assert(setting.recordLevel() == 160);
// ```

#include <balscm_version.h>

#include <ball_thresholdaggregate.h>

#include <bslma_allocator.h>

#include <bslmf_movableref.h>

#include <bsls_keyword.h>

#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace ball {

                     // =================================
                     // class HierarchicalCategorySetting
                     // =================================

/// This class represents a hierarchical category setting that consists of a
/// category name prefix and 4 threshold level settings for categories whose
/// name starts with that prefix.  Instances of `HierarchicalCategorySetting`
/// are created and manipulated by `LoggerManager`.  All threshold levels are
/// integral values in the range `[0 .. 255]`.
class HierarchicalCategorySetting {
  private:
    // PRIVATE TYPES

    /// `MoveUtil` is an alias for `bslmf::MovableRefUtil`.
    typedef bslmf::MovableRefUtil MoveUtil;

  public:
    // TYPES
    typedef bsl::allocator<> allocator_type;

  private:
    // DATA
    bsl::string         d_categoryPrefix;    // category name prefix

    unsigned int        d_thresholdLevels;   // record, pass, trigger, and
                                             // trigger-all levels

    // FRIENDS
    friend bool operator==(const HierarchicalCategorySetting&,
                           const HierarchicalCategorySetting&);
    friend bool operator!=(const HierarchicalCategorySetting&,
                           const HierarchicalCategorySetting&);

  public:
    // CREATORS

    /// Create a hierarchical category setting object having the specified
    /// `categoryPrefix` and the specified `recordLevel`, `passLevel`,
    /// `triggerLevel`, and `triggerAllLevel` threshold values, respectively.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.  The behavior is undefined unless each of the specified threshold
    /// levels is in the range `[0 .. 255]`.
    HierarchicalCategorySetting(
                         const bsl::string_view& categoryPrefix,
                         int                     recordLevel,
                         int                     passLevel,
                         int                     triggerLevel,
                         int                     triggerAllLevel,
                         allocator_type          allocator = allocator_type());
    HierarchicalCategorySetting(
                       const bsl::string_view&   categoryPrefix,
                       const ThresholdAggregate& levels,
                       allocator_type            allocator = allocator_type());

    /// Create a hierarchical category setting object initialized to the value
    /// of the specified `original` object.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply memory;
    /// otherwise, the default allocator is used.
    HierarchicalCategorySetting(
              const HierarchicalCategorySetting& original,
              const allocator_type&              allocator = allocator_type());

    /// Create a hierarchical category setting object having the same value as
    /// the specified `original` object, and adopting all outstanding memory
    /// allocations and the allocator associated with the `original` object.
    /// `original` is left in a valid but unspecified state.
    HierarchicalCategorySetting(
                       bslmf::MovableRef<HierarchicalCategorySetting> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Create a hierarchical category setting object having the same value as
    /// the specified `original` object.  The value of `original` is moved to
    /// the new object, and all outstanding memory allocations and the
    /// specified `allocator` are adopted if
    /// `allocator == original.get_allocator()`.  `original` is left in a valid
    /// but unspecified state.
    HierarchicalCategorySetting(
                     bslmf::MovableRef<HierarchicalCategorySetting> original,
                     const allocator_type&                          allocator);

    /// Destroy this object.
    //! ~HierarchicalCategorySetting() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    HierarchicalCategorySetting& operator=(
                                       const HierarchicalCategorySetting& rhs);

    /// Assign to this object the hierarchical category setting of the
    /// specified `rhs` object, and return a reference providing modifiable
    /// access to this object.  The settings of `rhs` are moved to this object,
    /// and all outstanding memory allocations and the allocator associated
    /// with `rhs` are adopted if `get_allocator() == rhs.get_allocator()`.
    /// `rhs` is left in a valid but unspecified state.
    HierarchicalCategorySetting& operator=(
                           bslmf::MovableRef<HierarchicalCategorySetting> rhs);

    /// Set the threshold levels of this hierarchical category settings object
    /// to the specified `recordLevel`, `passLevel`, `triggerLevel`, and
    /// `triggerAllLevel` values, respectively, if each of the specified values
    /// is in the range `[0 .. 255]`.  Return 0 on success, and a non-zero
    /// value otherwise (with no effect on the threshold levels of this
    /// object).
    int setLevels(int recordLevel,
                  int passLevel,
                  int triggerLevel,
                  int triggerAllLevel);

    // ACCESSORS

    /// Return the name prefix of this hierarchical category setting.
    bsl::string_view categoryPrefix() const;

    /// Return the record level of this hierarchical category setting.
    int recordLevel() const;

    /// Return the pass level of this hierarchical category setting.
    int passLevel() const;

    /// Return the trigger level of this hierarchical category setting.
    int triggerLevel() const;

    /// Return the trigger-all level of this hierarchical category setting.
    int triggerAllLevel() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.
    allocator_type get_allocator() const;

    /// Format this object to the specified output `stream` at the (absolute
    /// value of) the optionally specified indentation `level` and return a
    /// reference to `stream`.  If `level` is specified, optionally specify
    /// `spacesPerLevel`, the number of spaces per indentation level for this
    /// and all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the initial
    /// indentation (as governed by `level`).  If `stream` is not valid on
    /// entry, this operation has no effect.  Note that the format is not
    /// fully specified, and can change without notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` hierarchical category
/// settings have the same value, and `false` otherwise.  Two such objects have
/// the same value if they have the same category name prefix and threshold
/// values.
bool operator==(const HierarchicalCategorySetting& lhs,
                const HierarchicalCategorySetting& rhs);

/// Return `true` if the specified `lhs` and `rhs` hierarchical category
/// settings do not have the same value, and `false` otherwise.  Two such
/// objects differ in value if their category name prefix or threshold values
/// differ.
bool operator!=(const HierarchicalCategorySetting& lhs,
                const HierarchicalCategorySetting& rhs);

/// Write the value of the specified `object` to the specified output `stream`
/// in a single-line format, and return a non-`const` reference to `stream`.
/// If `stream` is not valid on entry, this operation has no effect.  Note that
/// this human-readable format is not fully specified and can change without
/// notice.  Also note that this method has the same behavior as
/// `object.print(stream, 0, -1)`, but with the attribute names elided.
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const HierarchicalCategorySetting& object);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // class HierarchicalCategorySetting
                     // ---------------------------------

// MANIPULATORS
inline
HierarchicalCategorySetting& HierarchicalCategorySetting::operator=(
                                        const HierarchicalCategorySetting& rhs)
{
    if (this != &rhs) {
        d_categoryPrefix  = rhs.d_categoryPrefix;
        d_thresholdLevels = rhs.d_thresholdLevels;
    }
    return *this;
}

inline
HierarchicalCategorySetting& HierarchicalCategorySetting::operator=(
                           bslmf::MovableRef<HierarchicalCategorySetting> rhs)
{
    if (this != &MoveUtil::access(rhs)) {
        d_categoryPrefix =
                        MoveUtil::move(MoveUtil::access(rhs).d_categoryPrefix);
        d_thresholdLevels =
                       MoveUtil::move(MoveUtil::access(rhs).d_thresholdLevels);
    }
    return *this;
}

// ACCESSORS
inline
bsl::string_view HierarchicalCategorySetting::categoryPrefix() const
{
    return d_categoryPrefix;
}

inline
int HierarchicalCategorySetting::recordLevel() const
{
    return ThresholdAggregateUtil::unpack(d_thresholdLevels).recordLevel();
}

inline
int HierarchicalCategorySetting::passLevel() const
{
    return ThresholdAggregateUtil::unpack(d_thresholdLevels).passLevel();
}

inline
int HierarchicalCategorySetting::triggerLevel() const
{
    return ThresholdAggregateUtil::unpack(d_thresholdLevels).triggerLevel();
}

inline
int HierarchicalCategorySetting::triggerAllLevel() const
{
    return ThresholdAggregateUtil::unpack(d_thresholdLevels).triggerAllLevel();
}

                                  // Aspects

inline
HierarchicalCategorySetting::allocator_type
HierarchicalCategorySetting::get_allocator() const
{
    return d_categoryPrefix.get_allocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const HierarchicalCategorySetting& lhs,
                      const HierarchicalCategorySetting& rhs)
{
    return lhs.d_thresholdLevels == rhs.d_thresholdLevels &&
           lhs.d_categoryPrefix  == rhs.d_categoryPrefix;
}
inline
bool ball::operator!=(const HierarchicalCategorySetting& lhs,
                      const HierarchicalCategorySetting& rhs)
{
    return lhs.d_thresholdLevels != rhs.d_thresholdLevels ||
           lhs.d_categoryPrefix  != rhs.d_categoryPrefix;
}

}  // close enterprise namespace

#endif  // INCLUDED_BALL_HIERARCHICALCATEGORYSETTING

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
