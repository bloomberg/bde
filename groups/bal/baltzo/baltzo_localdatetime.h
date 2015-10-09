// baltzo_localdatetime.h                                             -*-C++-*-
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
//  datetimeTz    bdlt::DatetimeTz   January 1, 0001, 24:00:00.000+0000
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
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//                                                   // in minutes from UTC
//  bsl::string      timeZoneId("America/New_York");
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

#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
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
                            // ===================
                            // class LocalDatetime
                            // ===================

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
    bsl::string      d_timeZoneId;  // local time-zone identifier

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(LocalDatetime,
                                  bslalg::TypeTraitBitwiseMoveable,
                                  bslalg::TypeTraitUsesBslmaAllocator);

    // CLASS METHODS

                        // Aspects

    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

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

    LocalDatetime(const bdlt::DatetimeTz&   datetimeTz,
                  const bslstl::StringRef&  timeZoneId,
                  bslma::Allocator         *basicAllocator = 0);
    LocalDatetime(const bdlt::DatetimeTz&   datetimeTz,
                  const char               *timeZoneId,
                  bslma::Allocator         *basicAllocator = 0);
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  If 'timeZoneId'
        // is passed as a null pointer, it is treated as an empty string.

    LocalDatetime(const LocalDatetime&  original,
                  bslma::Allocator     *basicAllocator = 0);
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

    void setTimeZoneId(const bslstl::StringRef&  value);
    void setTimeZoneId(const char               *value);
        // Set the 'timeZoneId' attribute of this object to the specified
        // 'value'.  If 'value' is null, it is treated as an empty string.

                        // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

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
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

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
bool operator==(const LocalDatetime& lhs, const LocalDatetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'LocalDatetime' objects have the
    // same value if all of the corresponding values of their 'datetimeTz' and
    // 'timeZoneId' attributes are the same.

bool operator!=(const LocalDatetime& lhs, const LocalDatetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'LocalDatetime' objects do not
    // have the same value if any of the corresponding values of their
    // 'datetimeTz' or 'timeZoneId' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&        stream,
                         const LocalDatetime& localDatetime);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)' with the
    // attribute names elided.

// FREE FUNCTIONS
void swap(LocalDatetime& a, LocalDatetime& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class LocalDatetime
                            // -------------------

// CLASS METHODS

                        // Aspects

inline
int baltzo::LocalDatetime::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
baltzo::LocalDatetime::LocalDatetime(bslma::Allocator *basicAllocator)
: d_datetimeTz()
, d_timeZoneId(basicAllocator)
{
}

inline
baltzo::LocalDatetime::LocalDatetime(const bdlt::DatetimeTz&   datetimeTz,
                                     const bslstl::StringRef&  timeZoneId,
                                     bslma::Allocator         *basicAllocator)
: d_datetimeTz(datetimeTz)
, d_timeZoneId(timeZoneId.begin(), timeZoneId.end(), basicAllocator)
{
}

inline
baltzo::LocalDatetime::LocalDatetime(const bdlt::DatetimeTz&   datetimeTz,
                                     const char               *timeZoneId,
                                     bslma::Allocator         *basicAllocator)
: d_datetimeTz(datetimeTz)
, d_timeZoneId(basicAllocator)
{
    if (timeZoneId) {
        bsl::string(timeZoneId, basicAllocator).swap(d_timeZoneId);
    }
}

inline
baltzo::LocalDatetime::LocalDatetime(const LocalDatetime&  original,
                                     bslma::Allocator     *basicAllocator)
: d_datetimeTz(original.d_datetimeTz)
, d_timeZoneId(original.d_timeZoneId, basicAllocator)
{
}

// MANIPULATORS
inline
baltzo::LocalDatetime&
baltzo::LocalDatetime::operator=(const LocalDatetime& rhs)
{
    d_timeZoneId = rhs.d_timeZoneId;  // first to allow strong guarantee
    d_datetimeTz = rhs.d_datetimeTz;
    return *this;
}

inline
void baltzo::LocalDatetime::setDatetimeTz(const bdlt::DatetimeTz& value)
{
    d_datetimeTz = value;
}

inline
void baltzo::LocalDatetime::setTimeZoneId(const bslstl::StringRef& value)
{
    d_timeZoneId.assign(value.begin(), value.end());
}

inline
void baltzo::LocalDatetime::setTimeZoneId(const char *value)
{
    if (value) {
        bsl::string(value, d_timeZoneId.allocator()).swap(d_timeZoneId);
    }
    else {
        d_timeZoneId.clear();
    }
}

                        // Aspects

template <class STREAM>
STREAM& baltzo::LocalDatetime::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bslx::InStreamFunctions::bdexStreamIn(stream, d_timeZoneId, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_datetimeTz, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

inline
void baltzo::LocalDatetime::swap(LocalDatetime& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    using bsl::swap;

    swap(d_datetimeTz, other.d_datetimeTz);
    swap(d_timeZoneId, other.d_timeZoneId);
}

// ACCESSORS
inline
const bdlt::DatetimeTz& baltzo::LocalDatetime::datetimeTz() const
{
    return d_datetimeTz;
}

inline
const bsl::string& baltzo::LocalDatetime::timeZoneId() const
{
    return d_timeZoneId;
}

                        // Aspects

inline
bslma::Allocator *baltzo::LocalDatetime::allocator() const
{
    return d_timeZoneId.get_allocator().mechanism();
}

template <class STREAM>
STREAM& baltzo::LocalDatetime::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            bslx::OutStreamFunctions::bdexStreamOut(stream, d_timeZoneId, 1);
            bslx::OutStreamFunctions::bdexStreamOut(stream, d_datetimeTz, 1);
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
void baltzo::swap(LocalDatetime& a, LocalDatetime& b)
{
    a.swap(b);
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
