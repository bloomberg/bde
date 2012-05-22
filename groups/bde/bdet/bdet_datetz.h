// bdet_datetz.h                                                      -*-C++-*-
#ifndef INCLUDED_BDET_DATETZ
#define INCLUDED_BDET_DATETZ

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a date with time zone offset.
//
//@CLASSES:
//     bdet_DateTz: local-date value with time zone offset from UTC
//
//@SEE_ALSO: bdet_date, bdet_datetimetz
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a date value and an offset from UTC.
// Specifically, the 'bdet_DateTz' class provided by this component represents
// a date value similar to 'bdet_Date', but explicitly as a local date; the
// offset of that local date from UTC (in minutes) is also part of the value of
// a 'bdet_DateTz' object.  For logical consistency, the date value and offset
// should correspond to a geographically valid time zone, but such consistency
// is the user's responsibility.  This component does not enforce logical
// constraints on any values.
//
// The 'localDate' method returns a 'bdet_Date' value corresponding to the
// local date.  The 'utcStartTime' method returns a 'bdet_Datetime' value
// corresponding to the UTC "point in time" when the local date starts (i.e.,
// 0000 hours local time).  In addition, the 'offset' method returns the time
// zone offset in integer minutes from UTC (i.e.,
// 'localDate() - offset() == utcStartTime()').
//
///A Note on Time Zone Support
///---------------------------
// 'bdet_DateTz' is a convenient, value-semantic class that provides some
// support for the notion of "time zone".  In some sense, 'bdet_DateTz'
// "extends" 'bdet_Date', which has no notion of "time zone" at all, but
// otherwise represents the same kind of value as 'bdet_DateTz'.  However, time
// zone "support" is limited to providing an offset value (in integer minutes).
// The rest is left to convention and to the interpretations of the user.
//
// A 'bdet_DateTz' value is intended to be interpreted as a value in some local
// time zone, along with the offset of that value from UTC.  However, there are
// some problems with this simple interpretation.  First of all, the offset
// value may not correspond to any time zone that has ever existed.  For
// example, the offset value could be set to one minute, or to 1,234 minutes.
// The meaning of the resulting "local time" value is always clear, but the
// local time might not correspond to any geographical or historical time zone.
//
// The second problem is more subtle.  A given offset from UTC might be "valid"
// in that it corresponds to a real time zone, but the actual date value might
// not exist in that time zone.  To make matters worse, a "valid" offset may
// not (indeed, rarely will) specify one time zone uniquely.  Moreover, the
// date value might be valid in one time zone corresponding to a given offset,
// and not in another time zone.
//
// For these reasons (and others), this component cannot and does not perform
// any validation relating to time zones or offsets.  The user must take care
// to honor the "local date" contract of this component.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use
// 'bdet_DateTz' objects.  First we will default construct an object 'dateTz1'.
// A default constructed object contains an offset of 0, implying that the
// object represents a date in the UTC time zone.  The value of the date is the
// same as that of a default constructed 'bdet_Date' object:
//..
//  bdet_DateTz dateTz1;
//  assert(0                   == dateTz1.offset());
//  assert(dateTz1.localDate() == dateTz1.utcStartTime().date());
//  assert(dateTz1.localDate() == bdet_Date());
//..
// Next we set 'dateTz1' to 12/31/2005 in the EST time zone (UTC-5):
//..
//  bdet_Date     date1(2005, 12, 31);
//  bdet_Datetime datetime1(date1, bdet_Time(0, 0, 0, 0));
//  int           offset1 = -5 * 60;
//
//  dateTz1.setDateTz(date1, offset1);
//  assert(offset1                == dateTz1.offset());
//  assert(dateTz1.localDate()    == dateTz1.utcStartTime().date());
//  assert(dateTz1.localDate()    == date1);
//  assert(dateTz1.utcStartTime() != datetime1);
//
//  datetime1.addMinutes(-offset1);
//  assert(dateTz1.utcStartTime() == datetime1);
//..
// Then we create 'dateTz2' as a copy of 'dateTz1':
//..
//  bdet_DateTz dateTz2(dateTz1);
//  assert(offset1                == dateTz2.offset());
//  assert(dateTz2.localDate()    == dateTz2.utcStartTime().date());
//  assert(dateTz2.localDate()    == date1);
//  assert(dateTz2.utcStartTime() == datetime1);
//..
// We now create a third object, 'dateTz3', representing the date 01/01/2001 in
// the PST time zone (UTC-8):
//..
//  bdet_Date     date2(2001, 01, 01);
//  bdet_Datetime datetime2(date2, bdet_Time(0, 0, 0, 0));
//  int           offset2 = -8 * 60;
//
//  bdet_DateTz dateTz3(date2, offset2);
//  assert(offset2                == dateTz3.offset());
//  assert(dateTz3.localDate()    == dateTz3.utcStartTime().date());
//  assert(dateTz3.localDate()    == date2);
//  assert(dateTz3.utcStartTime() != datetime2);
//
//  datetime2.addMinutes(-offset2);
//  assert(dateTz3.utcStartTime() == datetime2);
//..
// Finally we stream out the values of 'dateTz1', 'dateTz2' and 'dateTz3' to
// 'stdout':
//..
//  bsl::cout << dateTz1 << bsl::endl
//            << dateTz2 << bsl::endl
//            << dateTz3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  31DEC2005-0500
//  31DEC2005-0500
//  01JAN2001-0800
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                             // =================
                             // class bdet_DateTz
                             // =================

class bdet_DateTz {
    // This 'class' represents a date value explicitly in a local time zone;
    // the offset of that time (in minutes) from UTC is also part of the value
    // of this class.  The offset is available via the 'offset' method, and is
    // defined by the relationship: 'localDate() - offset() == utcStartTime()'.
    // The date and offset values are logically assumed to correspond to
    // geographically valid values, however, this constraint is not enforced.
    // The default output format is 'ddMONyyyy[+-]hhmm'.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two objects have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing objects, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // PRIVATE TYPES
    typedef bslalg_PassthroughTrait<bdet_Date,
                                    bslalg_TypeTraitBitwiseCopyable> DateTrait;
    enum ValidOffsetRange {
        // Enumeration used to hold the boundaries of the interval of validity
        // for the offset.

        BDET_DATETZ_MAX_OFFSET =  1440
      , BDET_DATETZ_MIN_OFFSET = -1440
    };

    // DATA
    bdet_Date d_localDate;  // date, local to the timezone specified by
                            // 'd_offset'

    int       d_offset;     // offset from UTC (in minutes)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_DateTz, DateTrait);

    // CLASS METHODS
    static bool isValid(const bdet_Date& localDate, int offset);
        // Return 'true' if the specified 'localDate' and time zone 'offset'
        // represent a valid 'bdet_DateTz' value, and 'false' otherwise.  A
        // 'localDate' and 'offset' represent a valid 'bdet_DateTz' value if
        // 'offset' is in the range '( -1440 .. 1440 )'.  Note that the
        // returned value does not depend on the value of 'localDate'.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value semantic types.)

    // CREATORS
    bdet_DateTz();
        // Create a valid 'bdet_DateTz' object having an implementation-
        // dependent value.

    bdet_DateTz(const bdet_Date& localDate, int offset);
        // Create a 'bdet_DateTz' object having a local date value equal to the
        // specified 'localDate' and a time zone offset value from UTC equal
        // to the specified 'offset' (in minutes).  The behavior is undefined
        // unless 'offset' is in the range '( -1440 .. 1440 )'.  Note that this
        // method provides no validation, and it is the user's responsibility
        // to assure the consistency of the resulting value.

    bdet_DateTz(const bdet_DateTz& original);
        // Construct a 'bdet_DateTz' object having the value of the specified
        // 'original' 'bdet_DateTz' object.

    //! ~bdet_DateTz();
        // Destroy this 'bdet_DateTz' object.  Note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    bdet_DateTz& operator=(const bdet_DateTz& rhs);
        // Assign to this 'bdet_DateTz' object the value of the specified 'rhs'
        // 'bdet_DateTz' object, and return a reference to this modifiable
        // object.

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

    void setDateTz(const bdet_Date& localDate, int offset);
        // Set the local date and the time zone offset of this object to the
        // specified 'localDate' and 'offset' values, respectively.  The
        // behavior is undefined unless 'offset' is in the range
        // '( -1440 .. 1440 )'.  Note that this method provides no validation,
        // and it is the user's responsibility to assure the consistency of the
        // resulting value.

    int validateAndSetDateTz(const bdet_Date& localDate, int offset);
        // Set the date value of this 'bdet_DateTz' object to the specified
        // 'localDate' and the time zone offset value to the specified 'offset'
        // if 'localDate' and 'offset' represent a valid 'bdet_DateTz' value.
        // Return 0 on success, and a non-zero value with no effect on this
        // 'bdet_DateTz' object otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.

    bdet_Datetime utcStartTime() const;
        // Return a 'bdet_Datetime' object having the value of the UTC "point
        // in time" when the local date starts (i.e., 0000 hours local time).
        // The behavior is undefined unless the local date starting time
        // represents a valid 'bdet_Datetime' value for the UTC timezone.  Note
        // that the returned value is equal to:
        //..
        //  bdet_Datetime(localDate()).addMinutes(-offset());
        //..

    bdet_Datetime gmtStartTime() const;
        // Return a 'bdet_Datetime' object having the value of the UTC "point
        // in time" when the local date starts (i.e., 0000 hours local time).
        // The behavior is undefined unless the local date starting time
        // represents a valid 'bdet_Datetime' value for the UTC timezone.  Note
        // that the returned value is equal to:
        //..
        //  bdet_Datetime(localDate()).addMinutes(-offset());
        //..
        // DEPRECATED: replaced by 'utcStartTime'.

    bdet_Date localDate() const;
        // Return a 'bdet_Date' object having the value of the local date
        // represented by this object.  Note that this is the 'bdet_Date'
        // supplied at construction and may not correspond to the actual time
        // zone offset of the local system.

    int offset() const;
        // Return the time zone offset of this 'bdet_DateTz' object.  Note that
        // the offset is in minutes from UTC.

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
};

// FREE OPERATORS
bool operator==(const bdet_DateTz& lhs, const bdet_DateTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdet_DateTz' objects
    // have the same value, and 'false' otherwise.  Two 'bdet_DateTz' objects
    // have the same value if they have the same local date value and the same
    // time zone offset value.

bool operator!=(const bdet_DateTz& lhs, const bdet_DateTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdet_DateTz' objects do
    // not have the same value, and 'false' otherwise.  Two 'bdet_DateTz'
    // objects differ in value if they differ in local date value or in time
    // zone offset value, or in both values.

bsl::ostream& operator<<(bsl::ostream& stream, const bdet_DateTz& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in the
    // following 14-character format:
    //..
    //  ddMONyyyy[+-]hhmm
    //..
    // and return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class bdet_DateTz
                             // -----------------

// CLASS METHODS
inline
bool bdet_DateTz::isValid(const bdet_Date&, int offset)
{
     return BDET_DATETZ_MIN_OFFSET < offset
         && BDET_DATETZ_MAX_OFFSET > offset;
}

inline
int bdet_DateTz::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdet_DateTz::bdet_DateTz()
: d_localDate()
, d_offset(0)
{
}

inline
bdet_DateTz::bdet_DateTz(const bdet_Date& localDate, int offset)
: d_localDate(localDate)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(isValid(localDate, offset));
}

inline
bdet_DateTz::bdet_DateTz(const bdet_DateTz& original)
: d_localDate(original.d_localDate)
, d_offset(original.d_offset)
{
}

// MANIPULATORS
inline
bdet_DateTz& bdet_DateTz::operator=(const bdet_DateTz& rhs)
{
    d_localDate = rhs.d_localDate;
    d_offset    = rhs.d_offset;

    return *this;
}

template <class STREAM>
STREAM& bdet_DateTz::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            d_localDate.bdexStreamIn(stream, 1);
            stream.getInt32(d_offset);

            if (!stream
             || BDET_DATETZ_MAX_OFFSET <= d_offset
             || BDET_DATETZ_MIN_OFFSET >= d_offset) {
                // Assign an arbitrary valid value to guarantee that the object
                // is in a valid state.

                d_localDate = bdet_Date(1, 1, 3);
                d_offset = -3;

                stream.invalidate();
                return stream;                                        // RETURN
            }
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

inline
void bdet_DateTz::setDateTz(const bdet_Date& localDate, int offset)
{
    BSLS_ASSERT_SAFE(isValid(localDate, offset));

    d_localDate = localDate;
    d_offset    = offset;
}

inline
int bdet_DateTz::validateAndSetDateTz(const bdet_Date& localDate, int offset)
{
    if (isValid(localDate, offset)) {
        setDateTz(localDate, offset);
        return 0;                                                     // RETURN
    }
    return -1;
}

// ACCESSORS
template <class STREAM>
STREAM& bdet_DateTz::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_localDate.bdexStreamOut(stream, 1);
        stream.putInt32(d_offset);
      } break;
    }
    return stream;
}

inline
bdet_Datetime bdet_DateTz::utcStartTime() const
{
    bdet_Datetime utc(d_localDate, bdet_Time(0,0,0,0));
    utc.addMinutes(-d_offset);
    return utc;
}

inline
bdet_Datetime bdet_DateTz::gmtStartTime() const
{
    return utcStartTime();
}

inline
bdet_Date bdet_DateTz::localDate() const
{
    return d_localDate;
}

inline
int bdet_DateTz::offset() const
{
    return d_offset;
}

// FREE OPERATORS
inline
bool operator==(const bdet_DateTz& lhs, const bdet_DateTz& rhs)
{
    return lhs.offset()    == rhs.offset()
        && lhs.localDate() == rhs.localDate();
}

inline
bool operator!=(const bdet_DateTz& lhs, const bdet_DateTz& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdet_DateTz& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
