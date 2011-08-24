// baetzo_localtimedescriptor.h                                       -*-C++-*-
#ifndef INCLUDED_BAETZO_LOCALTIMEDESCRIPTOR
#define INCLUDED_BAETZO_LOCALTIMEDESCRIPTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for characterizing local time values.
//
//@CLASSES:
//  baetzo_LocalTimeDescriptor: attributes characterizing a local time
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@SEE_ALSO: baetzo_zoneinfo
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baetzo_LocalTimeDescriptor', that is used
// to characterize subsets of local time values within time zones.  Note that
// this class is consistent with the "local-time types" found in the "Zoneinfo"
// representation of a time zone (see 'baetzo_zoneinfo').
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
// 'baetzo_LocalTimeDescriptor' object whose 'description' is "EST",
// 'dstInEffectFlag' is 'false', and 'utcOffsetInSeconds' is -18,000 (-5 * 60
// * 60).  Note that 'description' is *not* canonical, and is intended for
// development and debugging only.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Converting Between UTC and Local Times
///- - - - - - - - - - - - - - - - - - - - - - - - -
// When using the "Zoneinfo" database, we want to represent and access the
// local time information contained in the "Zoneinfo" binary data files.  Once
// we have obtained this information, we can use it to convert times from one
// time zone to another.  The following code illustrates how to perform such
// conversions using 'baetzo_LocalTimeDescriptor'.
//
// First, we define a 'baetzo_LocalTimeDescriptor' object that characterizes
// the local time in effect for New York Daylight-Saving Time in 2010:
//..
//  enum { NEW_YORK_DST_OFFSET = -4 * 60 * 60 };  // -4 hours in seconds
//
//  baetzo_LocalTimeDescriptor newYorkDst(NEW_YORK_DST_OFFSET, true, "EDT");
//
//  assert(NEW_YORK_DST_OFFSET == newYorkDst.utcOffsetInSeconds());
//  assert(               true == newYorkDst.dstInEffectFlag());
//  assert(              "EDT" == newYorkDst.description());
//..
// Then, we create a 'bdet_Datetime' representing the time
// "Jul 20, 2010 11:00" in New York:
//..
//  bdet_Datetime newYorkDatetime(2010, 7, 20, 11, 0, 0);
//..
// Next, we convert 'newYorkDatetime' to its corresponding UTC value using the
// 'newYorkDst' descriptor (created above); note that, when converting from a
// local time to a UTC time, the *signed* offset from UTC is *subtracted* from
// the local time:
//..
//  bdet_Datetime utcDatetime = newYorkDatetime;
//  utcDatetime.addSeconds(-newYorkDst.utcOffsetInSeconds());
//..
// Then, we verify that the result corresponds to the expected UTC time,
// "Jul 20, 2010 15:00":
//..
//  assert(bdet_Datetime(2010, 7, 20, 15, 0, 0) == utcDatetime);
//..
// Next, we define a 'baetzo_LocalTimeDescriptor' object that describes the
// local time in effect for Rome in the summer of 2010:
//..
//  enum { ROME_DST_OFFSET = 2 * 60 * 60 };  // 2 hours in seconds
//
//  baetzo_LocalTimeDescriptor romeDst(ROME_DST_OFFSET, true, "CEST");
//
//  assert(ROME_DST_OFFSET == romeDst.utcOffsetInSeconds());
//  assert(           true == romeDst.dstInEffectFlag());
//  assert(         "CEST" == romeDst.description());
//..
// Now, we convert 'utcDatetime' to its corresponding local-time value in Rome
// using the 'romeDst' descriptor (created above):
//..
//  bdet_Datetime romeDatetime = utcDatetime;
//  romeDatetime.addSeconds(romeDst.utcOffsetInSeconds());
//..
// Notice that, when converting from UTC time to local time, the signed
// offset from UTC is *added* to UTC time rather than subtracted.
//
// Finally, we verify that the result corresponds to the expected local time,
// "Jul 20, 2010 17:00":
//..
//  assert(bdet_Datetime(2010, 7, 20, 17, 0, 0) == romeDatetime);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ================================
                        // class baetzo_LocalTimeDescriptor
                        // ================================

class baetzo_LocalTimeDescriptor {
    // This simply constrained (value-semantic) attribute class characterizes a
    // subset of local time values.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.  Note that the class invariants are identically the
    // constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic) TBD
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    int         d_utcOffsetInSeconds;  // *signed* offset *from* UTC

    bool        d_dstInEffectFlag;     // 'true' if Daylight-Saving Time
                                       // is in effect, and 'false' otherwise

    bsl::string d_description;         // *non-canonical* identifier for this
                                       // descriptor

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(baetzo_LocalTimeDescriptor,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  bslalg_TypeTraitBitwiseMoveable);

    // CLASS METHODS
    static bool isValidUtcOffsetInSeconds(int value);
        // Return 'true' if the specified 'value' is in the range
        // '[-86399 .. 86399]', and 'false' otherwise.

    // CREATORS
    explicit baetzo_LocalTimeDescriptor(bslma_Allocator *basicAllocator = 0);
        // Create a 'baetzo_LocalTimeDescriptor' object having the (default)
        // attribute values:
        //..
        //  utcOffsetInSeconds() == 0
        //  dstInEffectFlag()    == false
        //  description()        == ""
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    baetzo_LocalTimeDescriptor(int                     utcOffsetInSeconds,
                               bool                    dstInEffectFlag,
                               const bdeut_StringRef&  description,
                               bslma_Allocator        *basicAllocator = 0);
        // Create a 'baetzo_LocalTimeDescriptor' object having the specified
        // 'utcOffsetInSeconds', 'dstInEffectFlag', and 'description' attribute
        // values.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '-86339 <= utcOffsetInSeconds <= 86399'.

    baetzo_LocalTimeDescriptor(
                        const baetzo_LocalTimeDescriptor&  original,
                        bslma_Allocator                   *basicAllocator = 0);
        // Create a 'baetzo_LocalTimeDescriptor' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~baetzo_LocalTimeDescriptor();
        // Destroy this object.
#endif

    // MANIPULATORS
    baetzo_LocalTimeDescriptor& operator=(
                                        const baetzo_LocalTimeDescriptor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDescription(const bdeut_StringRef& value);
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

    void swap(baetzo_LocalTimeDescriptor& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const bsl::string& description() const;
        // Return a reference providing non-modifiable access to the
        // 'description' attribute of this object.  Note that 'description' is
        // not canonical, and is intended for debugging only.

    bool dstInEffectFlag() const;
        // Return the value of the 'dstInEffectFlag' attribute of this object.
        // Note that 'true' implies Daylight-Saving Time (DST) is in effect.

    int utcOffsetInSeconds() const;
        // Return the value of the 'utcOffsetInSeconds' attribute of this
        // object.  Note that this value is in the range '[-86399 .. 86399]'.

                                  // Aspects

    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const baetzo_LocalTimeDescriptor& lhs,
                const baetzo_LocalTimeDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baetzo_LocalTimeDescriptor' objects
    // have the same value if all of the corresponding values of their
    // 'utcOffsetInSeconds', 'dstInEffectFlag', and 'description' attributes
    // are the same.

bool operator!=(const baetzo_LocalTimeDescriptor& lhs,
                const baetzo_LocalTimeDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_LocalTimeDescriptor'
    // objects do not have the same value if any of the corresponding values of
    // their 'utcOffsetInSeconds', 'dstInEffectFlag', or 'description'
    // attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const baetzo_LocalTimeDescriptor& object);
    // Write the value of the specified 'object' to the specified
    // output 'stream' in a single-line format, and return a reference
    // providing modifiable access to 'stream'.  If 'stream' is not valid on
    // entry, this operation has no effect.  Note that this human-readable
    // format is not fully specified and can change without notice.  Also note
    // that this method has the same behavior as 'object.print(stream, 0, -1)',
    // but with the attribute names elided.

// FREE FUNCTIONS
void swap(baetzo_LocalTimeDescriptor& a, baetzo_LocalTimeDescriptor& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------------
                        // class baetzo_LocalTimeDescriptor
                        // --------------------------------

// CLASS METHODS
inline
bool baetzo_LocalTimeDescriptor::isValidUtcOffsetInSeconds(int value)
{
    return value >= -86399 && value <= 86399;
}

// CREATORS
inline
baetzo_LocalTimeDescriptor::baetzo_LocalTimeDescriptor(
                                               bslma_Allocator *basicAllocator)
: d_utcOffsetInSeconds(0)
, d_dstInEffectFlag(false)
, d_description(basicAllocator)
{
}

inline
baetzo_LocalTimeDescriptor::baetzo_LocalTimeDescriptor(
                                    int                     utcOffsetInSeconds,
                                    bool                    dstInEffectFlag,
                                    const bdeut_StringRef&  description,
                                    bslma_Allocator        *basicAllocator)
: d_utcOffsetInSeconds(utcOffsetInSeconds)
, d_dstInEffectFlag(dstInEffectFlag)
, d_description(description.begin(), description.end(), basicAllocator)
{
    BSLS_ASSERT_SAFE(isValidUtcOffsetInSeconds(utcOffsetInSeconds));
    BSLS_ASSERT_SAFE(description.isBound());
}

inline
baetzo_LocalTimeDescriptor::baetzo_LocalTimeDescriptor(
                             const baetzo_LocalTimeDescriptor&  original,
                             bslma_Allocator                   *basicAllocator)
: d_utcOffsetInSeconds(original.d_utcOffsetInSeconds)
, d_dstInEffectFlag(original.d_dstInEffectFlag)
, d_description(original.d_description, basicAllocator)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
baetzo_LocalTimeDescriptor::~baetzo_LocalTimeDescriptor()
{
    BSLS_ASSERT_SAFE(isValidUtcOffsetInSeconds(d_utcOffsetInSeconds));
}
#endif

// MANIPULATORS
inline
baetzo_LocalTimeDescriptor& baetzo_LocalTimeDescriptor::operator=(
                                         const baetzo_LocalTimeDescriptor& rhs)
{
    d_description        = rhs.d_description;         // first for strong
                                                      // exception guarantee
    d_utcOffsetInSeconds = rhs.d_utcOffsetInSeconds;
    d_dstInEffectFlag    = rhs.d_dstInEffectFlag;
    return *this;
}

inline
void baetzo_LocalTimeDescriptor::setDescription(const bdeut_StringRef& value)
{
    BSLS_ASSERT_SAFE(value.isBound());

    d_description.assign(value.begin(), value.end());
}

inline
void baetzo_LocalTimeDescriptor::setDstInEffectFlag(bool value)
{
    d_dstInEffectFlag = value;
}

inline
void baetzo_LocalTimeDescriptor::setUtcOffsetInSeconds(int value)
{
    BSLS_ASSERT_SAFE(isValidUtcOffsetInSeconds(value));

    d_utcOffsetInSeconds = value;
}

                                  // Aspects

inline
void baetzo_LocalTimeDescriptor::swap(baetzo_LocalTimeDescriptor& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg_SwapUtil::swap(&d_description,        &other.d_description);
    bslalg_SwapUtil::swap(&d_dstInEffectFlag,    &other.d_dstInEffectFlag);
    bslalg_SwapUtil::swap(&d_utcOffsetInSeconds, &other.d_utcOffsetInSeconds);
}

// ACCESSORS
inline
const bsl::string& baetzo_LocalTimeDescriptor::description() const
{
    return d_description;
}

inline
bool baetzo_LocalTimeDescriptor::dstInEffectFlag() const
{
    return d_dstInEffectFlag;
}

inline
int baetzo_LocalTimeDescriptor::utcOffsetInSeconds() const
{
    return d_utcOffsetInSeconds;
}

                                  // Aspects

inline
bslma_Allocator *baetzo_LocalTimeDescriptor::allocator() const
{
    return d_description.get_allocator().mechanism();
}

// FREE OPERATORS
inline
bool operator==(const baetzo_LocalTimeDescriptor& lhs,
                const baetzo_LocalTimeDescriptor& rhs)
{
    return lhs.utcOffsetInSeconds() == rhs.utcOffsetInSeconds()
        && lhs.dstInEffectFlag()    == rhs.dstInEffectFlag()
        && lhs.description()        == rhs.description();
}

inline
bool operator!=(const baetzo_LocalTimeDescriptor& lhs,
                const baetzo_LocalTimeDescriptor& rhs)
{
    return lhs.utcOffsetInSeconds() != rhs.utcOffsetInSeconds()
        || lhs.dstInEffectFlag()    != rhs.dstInEffectFlag()
        || lhs.description()        != rhs.description();
}

// FREE FUNCTIONS
inline
void swap(baetzo_LocalTimeDescriptor& a, baetzo_LocalTimeDescriptor& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
