// baltzo_localdatetime.h                                               -*-C++-*-
#ifndef INCLUDED_BALTZO_LOCALDATETIME
#define INCLUDED_BALTZO_LOCALDATETIME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $ $CSID: $")

//@PURPOSE: Provide an attribute class for time-zone-aware datetime values.
//
//@CLASSES:
//  baltzo::LocalDatetime: time-zone-aware datetime type
//
//@AUTHOR: Stefano Pacifico (spacifico1),
//         Mike Giroux (mgiroux),
//         Henry Verschell (hverschell),
//         Steven Breitstein (sbreitstein)
//
//@SEE_ALSO: bdet_datetimetz
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'baltzo::LocalDatetime', that is used to
// encapsulate a date, time, offset from UTC (Coordinated Universal Time), and
// a time zone (string) identifier.  The date, time, and offset from UTC are
// contained within a 'bdlt::DatetimeTz' object, which together represents a
// wall-clock time in a given time zone.  This component differs from
// 'bdet_datetimetz' in that it provides, as part of the value, a string
// identifier for the corresponding time zone.
//
///Attributes
///----------
//..
//  Name          Type               Default
//  ----------    ---------------    ----------------------------------
//  datetimeTz    bdlt::DatetimeTz    January 1, 0001, 24:00:00.000+0000
//  timeZoneId    bsl::string        ""
//..
//: o 'datetimeTz': date, time, and offset from UTC of the local time.
//:
//: o 'timeZoneId': unique identifier representing the local time zone.
//
// For example, in New York on January 1, 2011, at 10 a.m. the local offset
// from UTC is -5 hours, and a standard time zone identifier for New York is
// "America/New_York".  We can represent this information using a
// 'baltzo::LocalDatetime' object whose 'datetimeTz' attribute is
// "01JAN2011_10:00:00.000-0005" and whose 'timeZoneId' attribute is
// "America/New_York".
//
// Note that it is up to the user to ensure that the 'datetimeTz' and
// 'timeZoneId' attributes are consistent as the 'baltzo::LocalDatetime' object
// itself does not maintain any invariants with respect to their values.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creation and Use of a 'baltzo::LocalDatetime' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we default-construct a 'baltzo::LocalDatetime' object:
//..
//  baltzo::LocalDatetime localDatetime;
//..
// Next, we update the time referred to by 'localDatetime' to the New York
// time "December 25, 2009, 11:00" with the time-zone identifier set to
// "America/New_York":
//..
//  bdlt::Datetime   datetime(2009, 12, 25, 11, 00, 00);
//  bdlt::DatetimeTz datetimeTz(datetime, -5 * 60);  // offset is specified
//                                                  // in minutes from UTC
//  bsl::string     timeZoneId("America/New_York");
//  localDatetime.setDatetimeTz(datetimeTz);
//  localDatetime.setTimeZoneId(timeZoneId);
//
//  assert(datetimeTz == localDatetime.datetimeTz());
//  assert(timeZoneId == localDatetime.timeZoneId());
//..
// Now, we change the time-zone identifier to another string, for example
// "Europe/Berlin":
//..
//  bsl::string anotherTimeZoneId("Europe/Berlin");
//  localDatetime.setTimeZoneId(anotherTimeZoneId);
//
//  assert(datetimeTz        == localDatetime.datetimeTz());
//  assert(anotherTimeZoneId == localDatetime.timeZoneId());
//..
// Finally, we stream 'localDatetime' to 'bsl::cout':
//..
//  bsl::cout << localDatetime << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  [ 25DEC2009_11:00:00.000-0500 "Europe/Berlin" ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLB_XXXSTRINGREF
#include <bdlb_xxxstringref.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMFUNCTIONS
#include <bdlxxxx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_OUTSTREAMFUNCTIONS
#include <bdlxxxx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

namespace baltzo {
                        // ========================
                        // class LocalDatetime
                        // ========================

class LocalDatetime {
    // This unconstrained (value-semantic) attribute class characterizes a date
    // time, offset from UTC, and a time zone identifier represented by a
    // string.  See the Attributes section under @DESCRIPTION in the
    // component-level documentation.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    bdlt::DatetimeTz d_datetimeTz;  // local date-time and offset from UTC
    bsl::string     d_timeZoneId;  // local time-zone identifier

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(LocalDatetime,
                                  bslalg::TypeTraitBitwiseMoveable,
                                  bslalg::TypeTraitUsesBslmaAllocator);

    // CLASS METHODS

                        // Aspects

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit LocalDatetime(bslma::Allocator *basicAllocator = 0);
        // Create a 'LocalDatetime' object having the (default) attribute
        // values:
        //..
        //  datetimeTz() == bdlt::DatetimeTz()
        //  timeZoneId() == ""
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    LocalDatetime(const bdlt::DatetimeTz&  datetimeTz,
                       const bdlb::StringRef&  timeZoneId,
                       bslma::Allocator       *basicAllocator = 0);
        // Create a 'LocalDatetime' object having the specified
        // 'datetimeTz' and 'timeZoneId' attribute values.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    LocalDatetime(const LocalDatetime&  original,
                       bslma::Allocator          *basicAllocator = 0);
        // Create a 'LocalDatetime' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~LocalDatetime() = default;
        // Destroy this object.

    // MANIPULATORS
    LocalDatetime& operator=(const LocalDatetime& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDatetimeTz(const bdlt::DatetimeTz& value);
        // Set the 'datetimeTz' attribute of this object to the specified
        // 'value'.

    void setTimeZoneId(const bdlb::StringRef& value);
        // Set the 'timeZoneId' attribute of this object to the specified
        // 'value'.

                        // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // providing modifiable access to 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    void swap(LocalDatetime& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as was 'other'.

    // ACCESSORS
    const bdlt::DatetimeTz& datetimeTz() const;
        // Return a reference providing non-modifiable access to the
        // 'datetimeTz' attribute of this object.

    const bsl::string& timeZoneId() const;
        // Return a reference providing non-modifiable access to the
        // 'timeZoneId' attribute of this object.

                        // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference providing
        // modifiable access to 'stream'.  If 'stream' is initially invalid,
        // this operation has no effect.  If 'version' is not supported,
        // 'stream' is invalidated.  Note that in no event is 'version' written
        // to 'stream'.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference providing modifiable
        // access to 'stream'.  Optionally specify an initial indentation
        // 'level', whose absolute value is incremented recursively for nested
        // objects.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', whose absolute value indicates the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.  Note that the format is not fully specified, and can change
        // without notice.
};

// FREE OPERATORS
bool operator==(const LocalDatetime& lhs,
                const LocalDatetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'LocalDatetime' objects have the
    // same value if all of the corresponding values of their 'datetimeTz' and
    // 'timeZoneId' attributes are the same.

bool operator!=(const LocalDatetime& lhs,
                const LocalDatetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'LocalDatetime'
    // objects do not have the same value if any of the corresponding values of
    // their 'datetimeTz' or 'timeZoneId' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&             stream,
                         const LocalDatetime& localDatetime);
}  // close package namespace
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)' with the
    // attribute names elided.

// FREE FUNCTIONS
void swap(baltzo::LocalDatetime& a, baltzo::LocalDatetime& b);

namespace baltzo {    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class LocalDatetime
                        // ------------------------

// CLASS METHODS

                        // Aspects

inline
int LocalDatetime::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
LocalDatetime::LocalDatetime(bslma::Allocator *basicAllocator)
: d_datetimeTz()
, d_timeZoneId(basicAllocator)
{
}

inline
LocalDatetime::LocalDatetime(const bdlt::DatetimeTz&  datetimeTz,
                                       const bdlb::StringRef&  timeZoneId,
                                       bslma::Allocator       *basicAllocator)
: d_datetimeTz(datetimeTz)
, d_timeZoneId(timeZoneId.begin(), timeZoneId.end(), basicAllocator)
{
}

inline
LocalDatetime::LocalDatetime(
                                     const LocalDatetime&  original,
                                     bslma::Allocator          *basicAllocator)
: d_datetimeTz(original.d_datetimeTz)
, d_timeZoneId(original.d_timeZoneId, basicAllocator)
{
}

// MANIPULATORS
inline
LocalDatetime&
LocalDatetime::operator=(const LocalDatetime& rhs)
{
    d_timeZoneId = rhs.d_timeZoneId;  // first to allow strong guarantee
    d_datetimeTz = rhs.d_datetimeTz;
    return *this;
}

inline
void LocalDatetime::setDatetimeTz(const bdlt::DatetimeTz& value)
{
    d_datetimeTz = value;
}

inline
void LocalDatetime::setTimeZoneId(const bdlb::StringRef& value)
{
    d_timeZoneId.assign(value.begin(), value.end());
}

                        // Aspects

template <class STREAM>
STREAM& LocalDatetime::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_timeZoneId, 1);
            bdex_InStreamFunctions::streamIn(stream, d_datetimeTz, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

inline
void LocalDatetime::swap(LocalDatetime& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bsl::swap(d_datetimeTz, other.d_datetimeTz);
    bsl::swap(d_timeZoneId, other.d_timeZoneId);
}

// ACCESSORS
inline
const bdlt::DatetimeTz& LocalDatetime::datetimeTz() const
{
    return d_datetimeTz;
}

inline
const bsl::string& LocalDatetime::timeZoneId() const
{
    return d_timeZoneId;
}

                        // Aspects

inline
bslma::Allocator *LocalDatetime::allocator() const
{
    return d_timeZoneId.get_allocator().mechanism();
}

template <class STREAM>
STREAM& LocalDatetime::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_OutStreamFunctions::streamOut(stream, d_timeZoneId, 1);
            bdex_OutStreamFunctions::streamOut(stream, d_datetimeTz, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool baltzo::operator==(const LocalDatetime& lhs, const LocalDatetime& rhs)
{
    return lhs.datetimeTz() == rhs.datetimeTz()
        && lhs.timeZoneId() == rhs.timeZoneId();
}

inline
bool baltzo::operator!=(const LocalDatetime& lhs, const LocalDatetime& rhs)
{
    return lhs.datetimeTz() != rhs.datetimeTz()
        || lhs.timeZoneId() != rhs.timeZoneId();
}

// FREE FUNCTIONS
inline
void swap(baltzo::LocalDatetime& a, baltzo::LocalDatetime& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
