// baltzo_localtimedescriptor.h                                       -*-C++-*-
#ifndef INCLUDED_BALTZO_LOCALTIMEDESCRIPTOR
#define INCLUDED_BALTZO_LOCALTIMEDESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for characterizing local time values.
//
//@CLASSES:
//  baltzo::LocalTimeDescriptor: attributes characterizing a local time
//
//@SEE_ALSO: baltzo_zoneinfo
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baltzo::LocalTimeDescriptor', that is
// used to characterize subsets of local time values within time zones.  Note
// that this class is consistent with the "local-time types" found in the
// "Zoneinfo" representation of a time zone (see 'baltzo_zoneinfo').
//
///Attributes
///----------
//..
//  Name                Type         Default  Simple Constraints
//  ------------------  -----------  -------  ------------------
//  description         bsl::string  ""       none
//  dstInEffectFlag     bool         false    none
//  utcOffsetInSeconds  int          0        [-86399 .. 86399]
//..
//: o 'description': non-canonical, non-localized name (intended for
//:   debugging).
//:
//: o 'dstInEffectFlag': 'true' if the described local times are
//:   Daylight-Saving-Time (DST) values.
//:
//: o 'utcOffsetInSeconds': offset from UTC of the described local times.
//
// For example, in New York on January 1, 2011, the local time is Eastern
// Standard Time, Daylight-Saving Time (DST) is not in effect, and the offset
// from UTC is -5 hours.  We can represent this information using a
// 'baltzo::LocalTimeDescriptor' object whose 'description' is "EST",
// 'dstInEffectFlag' is 'false', and 'utcOffsetInSeconds' is -18,000 (-5 * 60
// * 60).  Note that 'description' is *not* canonical, and is intended for
// development and debugging only.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting Between UTC and Local Times
///- - - - - - - - - - - - - - - - - - - - - - - - -
// When using the "Zoneinfo" database, we want to represent and access the
// local time information contained in the "Zoneinfo" binary data files.  Once
// we have obtained this information, we can use it to convert times from one
// time zone to another.  The following code illustrates how to perform such
// conversions using 'baltzo::LocalTimeDescriptor'.
//
// First, we define a 'baltzo::LocalTimeDescriptor' object that characterizes
// the local time in effect for New York Daylight-Saving Time in 2010:
//..
//  enum { NEW_YORK_DST_OFFSET = -4 * 60 * 60 };  // -4 hours in seconds
//
//  baltzo::LocalTimeDescriptor newYorkDst(NEW_YORK_DST_OFFSET, true, "EDT");
//
//  assert(NEW_YORK_DST_OFFSET == newYorkDst.utcOffsetInSeconds());
//  assert(               true == newYorkDst.dstInEffectFlag());
//  assert(              "EDT" == newYorkDst.description());
//..
// Then, we create a 'bdlt::Datetime' representing the time
// "Jul 20, 2010 11:00" in New York:
//..
//  bdlt::Datetime newYorkDatetime(2010, 7, 20, 11, 0, 0);
//..
// Next, we convert 'newYorkDatetime' to its corresponding UTC value using the
// 'newYorkDst' descriptor (created above); note that, when converting from a
// local time to a UTC time, the *signed* offset from UTC is *subtracted* from
// the local time:
//..
//  bdlt::Datetime utcDatetime = newYorkDatetime;
//  utcDatetime.addSeconds(-newYorkDst.utcOffsetInSeconds());
//..
// Then, we verify that the result corresponds to the expected UTC time,
// "Jul 20, 2010 15:00":
//..
//  assert(bdlt::Datetime(2010, 7, 20, 15, 0, 0) == utcDatetime);
//..
// Next, we define a 'baltzo::LocalTimeDescriptor' object that describes the
// local time in effect for Rome in the summer of 2010:
//..
//  enum { ROME_DST_OFFSET = 2 * 60 * 60 };  // 2 hours in seconds
//
//  baltzo::LocalTimeDescriptor romeDst(ROME_DST_OFFSET, true, "CEST");
//
//  assert(ROME_DST_OFFSET == romeDst.utcOffsetInSeconds());
//  assert(           true == romeDst.dstInEffectFlag());
//  assert(         "CEST" == romeDst.description());
//..
// Now, we convert 'utcDatetime' to its corresponding local-time value in Rome
// using the 'romeDst' descriptor (created above):
//..
//  bdlt::Datetime romeDatetime = utcDatetime;
//  romeDatetime.addSeconds(romeDst.utcOffsetInSeconds());
//..
// Notice that, when converting from UTC time to local time, the signed offset
// from UTC is *added* to UTC time rather than subtracted.
//
// Finally, we verify that the result corresponds to the expected local time,
// "Jul 20, 2010 17:00":
//..
//  assert(bdlt::Datetime(2010, 7, 20, 17, 0, 0) == romeDatetime);
//..

#include <balscm_version.h>

#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace baltzo {

                         // =========================
                         // class LocalTimeDescriptor
                         // =========================

class LocalTimeDescriptor {
    // This simply constrained (value-semantic) attribute class characterizes a
    // subset of local time values.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.  Note that the class invariants are identically the
    // constraints on the individual attributes.

    // DATA
    int         d_utcOffsetInSeconds;  // *signed* offset *from* UTC

    bool        d_dstInEffectFlag;     // 'true' if Daylight-Saving Time is in
                                       // effect, and 'false' otherwise

    bsl::string d_description;         // *non-canonical* identifier for this
                                       // descriptor

    // FRIENDS
    friend void swap(LocalTimeDescriptor&, LocalTimeDescriptor&);

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(LocalTimeDescriptor,
                                   bslma::UsesBslmaAllocator);

    BSLMF_NESTED_TRAIT_DECLARATION(LocalTimeDescriptor,
                                   bslmf::IsBitwiseMoveable);

    // CLASS METHODS
    static bool isValidUtcOffsetInSeconds(int value);
        // Return 'true' if the specified 'value' is in the range
        // '[-86399 .. 86399]', and 'false' otherwise.

    // CREATORS
    LocalTimeDescriptor();
    explicit LocalTimeDescriptor(const allocator_type& allocator);
        // Create a 'LocalTimeDescriptor' object having the (default)
        // attribute values:
        //..
        //  utcOffsetInSeconds() == 0
        //  dstInEffectFlag()    == false
        //  description()        == ""
        //..
        // Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    LocalTimeDescriptor(int                      utcOffsetInSeconds,
                        bool                     dstInEffectFlag,
                        const bslstl::StringRef& description,
                        const allocator_type&    allocator = allocator_type());
        // Create a 'LocalTimeDescriptor' object having the specified
        // 'utcOffsetInSeconds', 'dstInEffectFlag', and 'description'
        // attribute values.  Optionally specify an 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory; otherwise,
        // the default allocator is used.  The behavior is undefined unless
        // '-86339 <= utcOffsetInSeconds <= 86399'.

    LocalTimeDescriptor(const LocalTimeDescriptor& original,
                        const allocator_type&      allocator=allocator_type());
        // Create a 'LocalTimeDescriptor' object having the same value as the
        // specified 'original' object.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    LocalTimeDescriptor(bslmf::MovableRef<LocalTimeDescriptor> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'LocalTimeDescriptor' object having the same value and the
        // same allocator as the specified 'original' object.  The value of
        // 'original' becomes unspecified but valid, and its allocator remains
        // unchanged.

    LocalTimeDescriptor(bslmf::MovableRef<LocalTimeDescriptor> original,
                        const allocator_type&                  allocator);
        // Create a 'LocalTimeDescriptor' object having the same value as the
        // specified 'original' object, using the specified 'allocator' (e.g.,
        // the address of a 'bslma::Allocator' object) to supply memory.  The
        // allocator of 'original' remains unchanged.  If 'original' and the
        // newly created object have the same allocator then the value of
        // 'original' becomes unspecified but valid, and no exceptions will be
        // thrown; otherwise 'original' is unchanged and an exception may be
        // thrown.

    ~LocalTimeDescriptor();
        // Destroy this object.

    // MANIPULATORS
    LocalTimeDescriptor& operator=(const LocalTimeDescriptor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    LocalTimeDescriptor& operator=(bslmf::MovableRef<LocalTimeDescriptor> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  If 'rhs' and this
        // object have the same allocator then the value of 'rhs' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'rhs' is unchanged (and an exception may be thrown).

    void setDescription(const bslstl::StringRef& value);
        // Set the 'description' attribute of this object to the specified
        // 'value'.  Note that 'value' is not canonical, and is intended for
        // debugging only.

    void setDstInEffectFlag(bool value);
        // Set the 'dstInEffectFlag' attribute of this object to the specified
        // 'value'.  Note that 'true' implies Daylight-Saving Time (DST) is in
        // effect.

    void setUtcOffsetInSeconds(int value);
        // Set the 'utcOffsetInSeconds' attribute of this object to the
        // specified 'value'.  The behavior is undefined unless
        // '-86399 <= value <= 86399'.

                                  // Aspects

    void swap(LocalTimeDescriptor& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const bsl::string& description() const;
        // Return a 'const' reference to the 'description' attribute of this
        // object.  Note that 'description' is not canonical, and is intended
        // for debugging only.

    bool dstInEffectFlag() const;
        // Return the value of the 'dstInEffectFlag' attribute of this object.
        // Note that 'true' implies Daylight-Saving Time (DST) is in effect.

    int utcOffsetInSeconds() const;
        // Return the value of the 'utcOffsetInSeconds' attribute of this
        // object.  Note that this value is in the range '[-86399 .. 86399]'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // !DEPRECATED!: Use 'get_allocator()' instead.
        //
        // Return 'get_allocator().mechanism()'.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a non-'const' reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const LocalTimeDescriptor& lhs,
                const LocalTimeDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'LocalTimeDescriptor' objects have
    // the same value if all of the corresponding values of their
    // 'utcOffsetInSeconds', 'dstInEffectFlag', and 'description' attributes
    // are the same.

bool operator!=(const LocalTimeDescriptor& lhs,
                const LocalTimeDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'LocalTimeDescriptor' objects do
    // not have the same value if any of the corresponding values of their
    // 'utcOffsetInSeconds', 'dstInEffectFlag', or 'description' attributes are
    // not the same.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const LocalTimeDescriptor& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a non-'const' reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified and
    // can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)', but with the attribute names
    // elided.

// FREE FUNCTIONS
void swap(LocalTimeDescriptor& a, LocalTimeDescriptor& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // class LocalTimeDescriptor
                         // -------------------------

// CLASS METHODS
inline
bool LocalTimeDescriptor::isValidUtcOffsetInSeconds(int value)
{
    return value >= -86399 && value <= 86399;
}

// CREATORS
inline
LocalTimeDescriptor::LocalTimeDescriptor()
: d_utcOffsetInSeconds(0)
, d_dstInEffectFlag(false)
, d_description()
{
}

inline
LocalTimeDescriptor::LocalTimeDescriptor(const allocator_type& allocator)
: d_utcOffsetInSeconds(0)
, d_dstInEffectFlag(false)
, d_description(allocator)
{
}

inline
LocalTimeDescriptor::LocalTimeDescriptor(
                                   int                      utcOffsetInSeconds,
                                   bool                     dstInEffectFlag,
                                   const bslstl::StringRef& description,
                                   const allocator_type&    allocator)
: d_utcOffsetInSeconds(utcOffsetInSeconds)
, d_dstInEffectFlag(dstInEffectFlag)
, d_description(description.begin(), description.end(), allocator)
{
    BSLS_ASSERT(isValidUtcOffsetInSeconds(utcOffsetInSeconds));
}

inline
LocalTimeDescriptor::LocalTimeDescriptor(const LocalTimeDescriptor& original,
                                         const allocator_type&      allocator)
: d_utcOffsetInSeconds(original.d_utcOffsetInSeconds)
, d_dstInEffectFlag(original.d_dstInEffectFlag)
, d_description(original.d_description, allocator)
{
}

inline
LocalTimeDescriptor::LocalTimeDescriptor(
         bslmf::MovableRef<LocalTimeDescriptor> original) BSLS_KEYWORD_NOEXCEPT
: d_utcOffsetInSeconds(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_utcOffsetInSeconds)),
  d_dstInEffectFlag(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_dstInEffectFlag)),
  d_description(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_description))
{
}

inline
LocalTimeDescriptor::LocalTimeDescriptor(
                        bslmf::MovableRef<LocalTimeDescriptor> original,
                        const allocator_type&                  allocator)
: d_utcOffsetInSeconds(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_utcOffsetInSeconds))
, d_dstInEffectFlag(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_dstInEffectFlag))
, d_description(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_description), allocator)
{
}


inline
LocalTimeDescriptor::~LocalTimeDescriptor()
{
    BSLS_ASSERT(isValidUtcOffsetInSeconds(d_utcOffsetInSeconds));
}

// MANIPULATORS
inline
LocalTimeDescriptor& LocalTimeDescriptor::operator=(
                                                const LocalTimeDescriptor& rhs)
{
    d_description        = rhs.d_description;         // first for strong
                                                      // exception guarantee
    d_utcOffsetInSeconds = rhs.d_utcOffsetInSeconds;
    d_dstInEffectFlag    = rhs.d_dstInEffectFlag;
    return *this;
}

inline
LocalTimeDescriptor& LocalTimeDescriptor::operator=(
                                    bslmf::MovableRef<LocalTimeDescriptor> rhs)
{
    // Move 'd_description' first for strong exception guarantee.

    d_description = bslmf::MovableRefUtil::move(
        bslmf::MovableRefUtil::access(rhs).d_description);

    d_utcOffsetInSeconds = bslmf::MovableRefUtil::move(
        bslmf::MovableRefUtil::access(rhs).d_utcOffsetInSeconds);

    d_dstInEffectFlag = bslmf::MovableRefUtil::move(
        bslmf::MovableRefUtil::access(rhs).d_dstInEffectFlag);

    return *this;
}


inline
void LocalTimeDescriptor::setDescription(const bslstl::StringRef& value)
{
    d_description.assign(value.begin(), value.end());
}

inline
void LocalTimeDescriptor::setDstInEffectFlag(bool value)
{
    d_dstInEffectFlag = value;
}

inline
void LocalTimeDescriptor::setUtcOffsetInSeconds(int value)
{
    BSLS_ASSERT(isValidUtcOffsetInSeconds(value));

    d_utcOffsetInSeconds = value;
}

                                  // Aspects

inline
void LocalTimeDescriptor::swap(LocalTimeDescriptor& other)
{
    BSLS_ASSERT(get_allocator() == other.get_allocator());

    bslalg::SwapUtil::swap(&d_description,        &other.d_description);
    bslalg::SwapUtil::swap(&d_dstInEffectFlag,    &other.d_dstInEffectFlag);
    bslalg::SwapUtil::swap(&d_utcOffsetInSeconds, &other.d_utcOffsetInSeconds);
}

// ACCESSORS
inline
const bsl::string& LocalTimeDescriptor::description() const
{
    return d_description;
}

inline
bool LocalTimeDescriptor::dstInEffectFlag() const
{
    return d_dstInEffectFlag;
}

inline
int LocalTimeDescriptor::utcOffsetInSeconds() const
{
    return d_utcOffsetInSeconds;
}

                                  // Aspects

inline
bslma::Allocator *LocalTimeDescriptor::allocator() const
{
    return get_allocator().mechanism();
}

inline
LocalTimeDescriptor::allocator_type LocalTimeDescriptor::get_allocator() const
{
    return d_description.get_allocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool baltzo::operator==(const LocalTimeDescriptor& lhs,
                        const LocalTimeDescriptor& rhs)
{
    return lhs.utcOffsetInSeconds() == rhs.utcOffsetInSeconds()
        && lhs.dstInEffectFlag()    == rhs.dstInEffectFlag()
        && lhs.description()        == rhs.description();
}

inline
bool baltzo::operator!=(const LocalTimeDescriptor& lhs,
                        const LocalTimeDescriptor& rhs)
{
    return lhs.utcOffsetInSeconds() != rhs.utcOffsetInSeconds()
        || lhs.dstInEffectFlag()    != rhs.dstInEffectFlag()
        || lhs.description()        != rhs.description();
}

// FREE FUNCTIONS
inline
void baltzo::swap(LocalTimeDescriptor& a, LocalTimeDescriptor& b)
{
    bslalg::SwapUtil::swap(&a.d_description,        &b.d_description);
    bslalg::SwapUtil::swap(&a.d_dstInEffectFlag,    &b.d_dstInEffectFlag);
    bslalg::SwapUtil::swap(&a.d_utcOffsetInSeconds, &b.d_utcOffsetInSeconds);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
