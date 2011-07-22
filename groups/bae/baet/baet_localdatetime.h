// baet_localdatetime.h                                               -*-C++-*-
#ifndef INCLUDED_BAET_LOCALDATETIME
#define INCLUDED_BAET_LOCALDATETIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $ $CSID: $")

//@PURPOSE: Provide an attribute class for time-zone-aware datetime values.
//
//@CLASSES:
//  baet_LocalDatetime: time-zone-aware datetime type
//
//@AUTHOR: Stefano Pacifico (spacifico1),
//         Mike Giroux (mgiroux),
//         Henry Verschell (hverschell),
//         Steven Breitstein (sbreitstein)
//
//@SEE_ALSO: bdet_datetimetz
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'baet_LocalDatetime', that is used to
// encapsulate a date, time, offset from UTC (Coordinated Universal Time), and
// a time zone (string) identifier.  The date, time, and offset from UTC are
// contained within a 'bdet_DatetimeTz' object, which together represents a
// wall-clock time in a given time zone.  This component differs from
// 'bdet_datetimetz' in that it provides, as part of the value, a string
// identifier for the corresponding time zone.
//
///Attributes
///----------
//..
//  Name          Type               Default
//  ----------    ---------------    ----------------------------------
//  datetimeTz    bdet_DatetimeTz    January 1, 0001, 24:00:00.000+0000
//  timeZoneId    bsl::string        ""
//..
//: o 'datetimeTz': date, time, and offset from UTC of the local time.
//:
//: o 'timeZoneId': unique identifier representing the local time zone.
//
// For example, in New York on January 1, 2011, at 10 a.m. the local offset
// from UTC is -5 hours, and a standard time zone identifier for New York is
// "America/New_York".  We can represent this information using a
// 'baet_LocalDatetime' object whose 'datetimeTz' attribute is
// "01JAN2011_10:00:00.000-0005" and whose 'timeZoneId' attribute is
// "America/New_York".
//
// Note that it is up to the user to ensure that the 'datetimeTz' and
// 'timeZoneId' attributes are consistent as the 'baet_LocalDatetime' object
// itself does not maintain any invariants with respect to their values.
//
///Usage
///-----
// In this usage example we illustrate how to create and use a
// 'baet_LocalDatetime' object:
//
// First, we create a default-initialized 'baet_LocalDatetime':
//..
//  baet_LocalDatetime localDatetime;
//..
// Next, we update the time referred to by 'localDatetime' to the New York
// time "December 25, 2009, 11:00" with the time-zone identifier set to
// "America/New_York":
//..
//  bdet_Datetime   datetime(2009, 12, 25, 11, 00, 00);
//  bdet_DatetimeTz datetimeTz(datetime, -5 * 60);  // offset is specified
//                                                  // in minutes from UTC
//
//  bsl::string     timeZoneId("America/New_York");
//  localDatetime.setDatetimeTz(datetimeTz);
//  localDatetime.setTimeZoneId(timeZoneId.c_str());
//
//  assert(datetimeTz == localDatetime.datetimeTz());
//  assert(timeZoneId == localDatetime.timeZoneId());
//..
// Next, we change the time-zone identifier to another string, for example
// "Europe/Berlin":
//..
//  bsl::string anotherTimeZoneId("Europe/Berlin");
//  localDatetime.setTimeZoneId(anotherTimeZoneId.c_str());
//
//  assert(anotherTimeZoneId == localDatetime.timeZoneId());
//  assert(datetimeTz        == localDatetime.datetimeTz());
//..
// Finally, we stream 'localDatetime' to 'bsl::cout':
//..
//  bsl::cout << localDatetime << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  [ datetimeTz = 25DEC2009_11:00:00.000-0500 timeZoneId = "Europe/Berlin" ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
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

                        // ========================
                        // class baet_LocalDatetime
                        // ========================

class baet_LocalDatetime {
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
    // For terminology: bsldoc_terminology

    // DATA
    bdet_DatetimeTz d_datetimeTz;  // local date-time and offset from UTC
    bsl::string     d_timeZoneId;  // local time-zone identifier

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(baet_LocalDatetime,
                                  bslalg_TypeTraitBitwiseMoveable,
                                  bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
                        // Aspects

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit baet_LocalDatetime(bslma_Allocator *basicAllocator = 0);
        // Create a 'baet_LocalDatetime' object having the (default) attribute
        // values:
        //..
        //  datetimeTz() == bdet_DatetimeTz()
        //  timeZoneId() == ""
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    baet_LocalDatetime(const bdet_DatetimeTz&  datetimeTz,
                       const char             *timeZoneId,
                       bslma_Allocator        *basicAllocator = 0);
        // Create a 'baet_LocalDatetime' object having the specified
        // 'datetimeTz' and 'timeZoneId' attribute values.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    baet_LocalDatetime(const baet_LocalDatetime&  original,
                       bslma_Allocator           *basicAllocator = 0);
        // Create a 'baet_LocalDatetime' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~baet_LocalDatetime() = default;
        // Destroy this object.

    // MANIPULATORS
    baet_LocalDatetime& operator=(const baet_LocalDatetime& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDatetimeTz(const bdet_DatetimeTz& value);
        // Set the 'datetimeTz' attribute of this object to the specified
        // 'value'.

    void setTimeZoneId(const char *value);
        // Set the 'timeZoneId' attribute of this object to the specified
        // 'value'.

                        // Aspects

    void swap(baet_LocalDatetime& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as was 'other'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // (See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.)

    // ACCESSORS
    const bdet_DatetimeTz& datetimeTz() const;
        // Return the value of the 'datetimeTz' attribute of this object.

    const bsl::string& timeZoneId() const;
        // Return a reference providing non-modifiable access to the
        // 'timeZoneId' attribute of this object.

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to the modifiable
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated.
        // Note that in no event is 'version' written to 'stream'.  (See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.)
};

// FREE OPERATORS
bool operator==(const baet_LocalDatetime& lhs,
                const baet_LocalDatetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baet_LocalDatetime' objects have the
    // same value if the corresponding values of their 'datetimeTz' and
    // 'timeZoneId' attributes are the same.

bool operator!=(const baet_LocalDatetime& lhs,
                const baet_LocalDatetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baet_LocalDatetime'
    // objects do not have the same value if the corresponding values of
    // their 'datetimeTz' or 'timeZoneId' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&             stream,
                         const baet_LocalDatetime& localDatetime);
    // Write the value of the specified 'object' to the specified
    // output 'stream' in a single-line format, and return a reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified
    // and can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)'.

// FREE FUNCTIONS
void swap(baet_LocalDatetime& a, baet_LocalDatetime& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class baet_LocalDatetime
                        // ------------------------

// CLASS METHODS
                        // Aspects

inline
int baet_LocalDatetime::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
baet_LocalDatetime::baet_LocalDatetime(bslma_Allocator *basicAllocator)
: d_datetimeTz()
, d_timeZoneId(basicAllocator)
{
}

inline
baet_LocalDatetime::baet_LocalDatetime(const bdet_DatetimeTz&  datetimeTz,
                                       const char             *timeZoneId,
                                       bslma_Allocator        *basicAllocator)
: d_datetimeTz(datetimeTz)
, d_timeZoneId(timeZoneId, basicAllocator)
{
}

inline
baet_LocalDatetime::baet_LocalDatetime(
                                     const baet_LocalDatetime&  original,
                                     bslma_Allocator           *basicAllocator)
: d_datetimeTz(original.d_datetimeTz)
, d_timeZoneId(original.d_timeZoneId, basicAllocator)
{
}

// MANIPULATORS
inline
baet_LocalDatetime&
baet_LocalDatetime::operator=(const baet_LocalDatetime& rhs)
{
    d_timeZoneId = rhs.d_timeZoneId;  // must be first
    d_datetimeTz = rhs.d_datetimeTz;
    return *this;
}

inline
void baet_LocalDatetime::setDatetimeTz(const bdet_DatetimeTz& value)
{
    d_datetimeTz = value;
}

inline
void baet_LocalDatetime::setTimeZoneId(const char *value)
{
    d_timeZoneId.assign(value);
}

                        // Aspects

inline
void baet_LocalDatetime::swap(baet_LocalDatetime& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bsl::swap(d_datetimeTz, other.d_datetimeTz);
    bsl::swap(d_timeZoneId, other.d_timeZoneId);
}

template <class STREAM>
STREAM& baet_LocalDatetime::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_datetimeTz, 1);
            bdex_InStreamFunctions::streamIn(stream, d_timeZoneId, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
bslma_Allocator *baet_LocalDatetime::allocator() const
{
    return d_timeZoneId.get_allocator().mechanism();
}

inline
const bdet_DatetimeTz& baet_LocalDatetime::datetimeTz() const
{
    return d_datetimeTz;
}

inline
const bsl::string& baet_LocalDatetime::timeZoneId() const
{
    return d_timeZoneId;
}

                        // Aspects

template <class STREAM>
STREAM& baet_LocalDatetime::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_OutStreamFunctions::streamOut(stream, d_datetimeTz, 1);
            bdex_OutStreamFunctions::streamOut(stream, d_timeZoneId, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator==(const baet_LocalDatetime& lhs, const baet_LocalDatetime& rhs)
{
    return lhs.datetimeTz() == rhs.datetimeTz()
        && lhs.timeZoneId() == rhs.timeZoneId();
}

inline
bool operator!=(const baet_LocalDatetime& lhs, const baet_LocalDatetime& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const baet_LocalDatetime& localDatetime)
{
    return localDatetime.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void swap(baet_LocalDatetime& a, baet_LocalDatetime& b)
{
    a.swap(b);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
