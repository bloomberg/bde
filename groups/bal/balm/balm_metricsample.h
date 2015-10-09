// balm_metricsample.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_METRICSAMPLE
#define INCLUDED_BALM_METRICSAMPLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for a sample of collected metric records.
//
//@CLASSES:
// balm::MetricSampleGroup: a group of records describing the same time period
//      balm::MetricSample: a sample of collected metric records
//
//@SEE_ALSO: balm_publisher, balm_metricrecord
//
//@DESCRIPTION: This component provides a container used to store a sample of
// recorded metric information.  A 'balm::MetricSample' contains a collection
// of addresses to (external) 'balm::MetricRecord' objects containing the
// aggregated record values for a series of metrics.  The records in a sample
// are broken into a series of groups, each group is represented by a
// 'balm::MetricSampleGroup' object.  Each 'balm::MetricSampleGroup' contains
// a sequence of records and an elapsed time value, indicating the time period
// over which those records were collected.  Finally, a 'balm::MetricSample'
// object contains a timestamp value used to indicate when the sample was
// taken.
//
///Thread Safety
///-------------
// 'balm::MetricSample' and 'balm::MetricSampleGroup' are both *const*
// *thread-safe*, meaning that accessors may be invoked concurrently from
// different threads, but it is not safe to access or modify an object in one
// thread while another thread modifies the same object.
//
///Usage
///-----
// The following example demonstrates how to create and use a metric sample.
// We start by initializing several 'balm::MetricRecord' values, which we will
// add to the sample.  Note that in this example we create the 'balm::MetricId'
// objects by hand; however, in practice ids should be obtained from a
// 'balm::MetricRegistry' object (such as the one owned by a
// 'balm::MetricsManager').
//..
//  bslma::Allocator *allocator = bslma::Default::allocator(0);
//
//  balm::Category myCategory("MyCategory");
//  balm::MetricDescription descA(&myCategory, "MetricA");
//  balm::MetricDescription descB(&myCategory, "MetricB");
//  balm::MetricDescription descC(&myCategory, "MetricC");
//
//  balm::MetricId metricA(&descA);
//  balm::MetricId metricB(&descB);
//  balm::MetricId metricC(&descC);
//
//  const int TZ = 0;  // UTC time zone offset
//
//  bdlt::DatetimeTz timeStamp(bdlt::Datetime(2008, 3, 26, 13, 30, 0, 0), TZ);
//  balm::MetricRecord recordA(metricA, 0, 0, 0, 0);
//  balm::MetricRecord recordB(metricB, 1, 2, 3, 4);
//  balm::MetricRecord recordC(metricC, 4, 3, 2, 1);
//..
// Now we create the two arrays of metric records whose addresses we will
// later add to the metric sample:
//..
//  balm::MetricRecord              buffer1[] = { recordA, recordB };
//  bsl::vector<balm::MetricRecord> buffer2(allocator);
//  buffer2.push_back(recordC);
//..
// Next we create a 'balm::MetricSample' object, 'sample', and set its
// timestamp property.  Then we add two groups of records (containing the
// addresses of our two record arrays) to the sample we have created.  Since
// the records were not actually collected over a period of time, we supply an
// arbitrary elapsed time value of 1 second and 2 seconds (respectively) for
// the two groups added to the sample.  Note that these arrays must remain
// valid for the lifetime of 'sample'.
//..
//  balm::MetricSample sample(allocator);
//  sample.setTimeStamp(timeStamp);
//  sample.appendGroup(buffer1,
//                     sizeof(buffer1) / sizeof(*buffer1),
//                     bsls::TimeInterval(1.0));
//  sample.appendGroup(buffer2.data(),
//                     buffer2.size(),
//                     bsls::TimeInterval(2.0));
//..
// We can verify the basic properties of our sample:
//..
//       assert(timeStamp             == sample.timeStamp());
//       assert(2                     == sample.numGroups());
//       assert(3                     == sample.numRecords());
//       assert(bsls::TimeInterval(1) == sample.sampleGroup(0).elapsedTime());
//       assert(buffer1               == sample.sampleGroup(0).records());
//       assert(2                     == sample.sampleGroup(0).numRecords());
//       assert(bsls::TimeInterval(2) == sample.sampleGroup(1).elapsedTime());
//       assert(buffer2.data()        == sample.sampleGroup(1).records());
//       assert(1                     == sample.sampleGroup(1).numRecords());
//..
// Finally we can obtain an iterator over the sample's sequence of groups.  In
// this simple example, we iterate over the groups of records in the sample
// and, for each group, iterate over the records in that group, writing those
// records to the console.
//..
//  balm::MetricSample::const_iterator sampleIt = sample.begin();
//  for ( ; sampleIt != sample.end(); ++sampleIt) {
//      balm::MetricSampleGroup::const_iterator groupIt = sampleIt->begin();
//      for ( ; groupIt != sampleIt->end(); ++groupIt) {
//          bsl::cout << *groupIt << bsl::endl;
//      }
//  }
//..
// The output will look like:
//..
//  [ MyCategory.MetricA: 0 0 0 0 ]
//  [ MyCategory.MetricB: 1 2 3 4 ]
//  [ MyCategory.MetricC: 4 3 2 1 ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_METRICRECORD
#include <balm_metricrecord.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace balm {
                          // =======================
                          // class MetricSampleGroup
                          // =======================

class MetricSampleGroup {
    // This class provides an *in-core* value-semantic representation of a
    // group of metric record values.  This class contains the address of an
    // array of (externally managed) 'MetricRecord' objects, the number of
    // records in that array, and an elapsed time value (used to indicate the
    // time span over which the metric values were aggregated).

    // DATA
    const MetricRecord *d_records_p;   // array of records (held, not
                                       // owned)

    int                 d_numRecords;  // number of records in array

    bsls::TimeInterval  d_elapsedTime; // interval described by records

  public:
    // PUBLIC TYPES
    typedef const MetricRecord *const_iterator;
        // A 'const_iterator' is an alias for an iterator over the
        // non-modifiable records referenced in a 'MetricSampleGroup'.

    // CREATORS
    MetricSampleGroup();
        // Create an empty sample group.  By default, the 'records()' address
        // is 0, 'numRecords()' is 0, and the 'elapsedTime()' is the default-
        // constructed 'bsls::TimeInterval'.

    MetricSampleGroup(const MetricRecord             *records,
                           int                        numRecords,
                           const bsls::TimeInterval&  elapsedTime);
        // Create a sample group containing the specified sequence of
        // 'records' of specified length 'numRecords', recorded over a period
        // whose duration is the specified 'elapsedTime'.  The behavior is
        // undefined unless '0 <= numRecords' and 'records' points to a
        // contiguous sequence of (at least) 'numRecords' metric records.  Note
        // that the contents of 'records' is *not* copied and the supplied
        // array must remain valid for the productive lifetime of this object
        // or until the records are set to a different sequence by calling the
        // 'setRecords' manipulator.

    MetricSampleGroup(const MetricSampleGroup& original);
        // Create a sample group having the same (in-core) value as the
        // specified 'original' sample group.

    // ~MetricSampleGroup();
        // Destroy this sample group.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    MetricSampleGroup& operator=(const MetricSampleGroup& rhs);
        // Assign to this sample group the value of the specified 'rhs' sample
        // group, and return a reference to this modifiable sample group.
        // Note that only the pointer to the 'MetricRecord' array and the
        // length are copied, and not the records themselves.

    void setElapsedTime(const bsls::TimeInterval& elapsedTime);
        // Set the elapsed time (used to indicate the time span over which
        // this object's metric records were aggregated) to the specified
        // 'elapsedTime'.

    void setRecords(const MetricRecord *records, int numRecords);
        // Set the sequence of records referred to by this sample group to the
        // specified sequence of 'records' of specified length 'numRecords'.
        // The behavior is undefined unless '0 <= numRecords', and 'records'
        // refers to a contiguous sequence of (at least) 'numRecords'.  Note
        // that the contents of 'records' is *not* copied and the supplied
        // array must remain valid for the productive lifetime of this object
        // or until the records are set to a different sequence by calling the
        // 'setRecords' manipulator.

    // ACCESSORS
    const MetricRecord *records() const;
        // Return the address of the contiguous sequence of non-modifiable
        // records of length 'numRecords()'.

    int numRecords() const;
        // Return the number of records (referenced to by 'records()') in this
        // object.

    const bsls::TimeInterval& elapsedTime() const;
        // Return a reference to the non-modifiable elapsed time interval over
        // which this object's metric records were aggregated.

    const_iterator begin() const;
        // Return an iterator positioned at the beginning of the sequence of
        // 'MetricRecord' objects referenced by this object.

    const_iterator end() const;
        // Return an iterator positioned one past the final 'MetricRecord'
        // object in the sequence of records referenced by this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const MetricSampleGroup& lhs,
                const MetricSampleGroup& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sample groups have the
    // same value, and 'false' otherwise.  Two sample groups have the same
    // value if the respective record sequence-addresses, number of records,
    // and elapsed time are the same.

bool operator!=(const MetricSampleGroup& lhs,
                const MetricSampleGroup& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sample groups do not
    // have the same value, and 'false' otherwise.  Two sample groups do not
    // have the same value if any of the respective record-sequence addresses,
    // number of records, or elapsed time, are not the same.

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const MetricSampleGroup& rhs);
    // Write a formatted description of the specified 'rhs' to the specified
    // 'stream' and return a reference to the modifiable 'stream'.

                             // ==================
                             // class MetricSample
                             // ==================

class MetricSample {
    // This class provides an *in-core* value-semantic representation of a
    // sample of metric values.  The class contains a collection of addresses
    // to (external) 'MetricRecord' objects holding the values for their
    // respective metrics (aggregated over some period of time).  The metric
    // records contained by a sample are broken into a series of groups, which
    // are represented by 'MetricSampleGroup' objects.  Each group contains a
    // sequence of records and an elapsed time value, indicating the period of
    // time over which those records were taken.  This class also provides a
    // timestamp value, used to indicate when the sample was collected.  The
    // class provides a method, 'appendGroups', that appends a group of metric
    // records to the sample.  Arrays supplied using 'appendGroups' must be
    // valid for the productive lifetime of the 'MetricSample' object or until
    // they are removed by calling 'removeAllRecords'.

    // PRIVATE TYPES
    typedef MetricSampleGroup SampleGroup;

    // DATA
    bdlt::DatetimeTz           d_timeStamp;   // time the records were
                                             // collected

    bsl::vector<SampleGroup>  d_records;     // vector of groups of records

    int                       d_numRecords;  // total number of records

    // FRIENDS
    friend bool operator==(const MetricSample& lhs,
                           const MetricSample& rhs);

  public:
    // PUBLIC TYPES
    typedef bsl::vector<MetricSampleGroup>::const_iterator const_iterator;
        // A 'const_iterator' is an alias for an iterator over the
        // non-modifiable sample groups contained in a 'MetricSample'.

    // PUBLIC TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricSample, bslma::UsesBslmaAllocator);

    // CREATORS
    MetricSample(bslma::Allocator *basicAllocator = 0);
        // Create an empty metric sample.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    MetricSample(const MetricSample&  original,
                 bslma::Allocator    *basicAllocator = 0);
        // Create a metric sample containing the same value as the specified
        // 'original' sample.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that copying the contained
        // 'MetricSampleGroup' objects copies only the pointers to their
        // respective 'MetricRecord' arrays, and does not copy the records
        // themselves; hence, these record arrays must remain valid for the
        // productive lifetimes of all copied objects or until the records are
        // removed by calling 'removeAllRecords'.

    ~MetricSample();
        // Destroy this metric sample.

    // MANIPULATORS
    MetricSample& operator=(const MetricSample& rhs);
        // Assign to this sample the value of the specified 'rhs' sample and
        // return a reference to this modifiable sample.  Note that copying
        // the 'MetricSampleGroup' objects contained in 'rhs' copies only the
        // pointers to their respective 'MetricRecord' arrays, and does not
        // copy records themselves; hence, these record arrays must remain
        // valid for the productive lifetimes of all copied objects or until
        // records are removed by calling 'removeAllRecords'.

    void setTimeStamp(const bdlt::DatetimeTz& timeStamp);
        // Set the timestamp (used to indicate when the sample was taken) to
        // the specified 'timeStamp'.

    void appendGroup(const MetricSampleGroup& group);
        // Append the specified 'group' of records to the sequence of groups
        // maintained by this sample.  If 'group.numRecords()' is 0 this method
        // has no effect.  The behavior is undefined unless
        // 'group.elapsedTime() > bsls::TimeInterval(0, 0)'.  Note that the
        // 'MetricRecord' objects referred to by 'records' are *not* copied:
        // hence, the supplied array must remain valid for the productive
        // lifetime of this object or until the group is removed by calling
        // 'removeAllRecords()'.

    void appendGroup(const MetricRecord        *records,
                     int                        numRecords,
                     const bsls::TimeInterval&  elapsedTime);
        // Append to the sequence of groups maintained by this sample a new
        // group containing the specified sequence of 'records' of specified
        // length 'numRecords' measuring the specified 'elapsedTime'.  The
        // behavior is undefined unless '0 <= numRecords', 'records' refers to
        // a contiguous sequence of size (at least) 'numRecords', and
        // 'elapsedTime > bsls::TimeInterval(0, 0)'.  Note that 'records' is
        // *not* copied: hence, the supplied array must remain valid for the
        // lifetime of this object or until the records are removed by calling
        // 'removeAllRecords()'.  This method is functionally equivalent to:
        //..
        //  appendGroup(MetricSampleGroup(records, numRecords, elapsedTime));
        //..

    void removeAllRecords();
        // Remove all metric records from this sample.

    // ACCESSORS
    const MetricSampleGroup& sampleGroup(int index) const;
        // Return a reference to the non-modifiable 'MetricSampleGroup' object
        // at the specified 'index' in this sample.  The behavior is undefined
        // unless '0 <= index < numGroups()'.  Note that the returned
        // reference will remain valid until this sample is modified by
        // invoking 'appendGroup' or 'removeAllRecords()'.

    const bdlt::DatetimeTz& timeStamp() const;
        // Return a reference to the non-modifiable timestamp for this sample.

    const_iterator begin() const;
        // Return an iterator positioned at the beginning of the sequence of
        // 'MetricSampleGroup' objects contained by this object.  Note that the
        // iterator will remain valid until this sample is modified by invoking
        // either 'appendGroups' or 'removeAllRecords()'.

    const_iterator end() const;
        // Return an iterator positioned one one past the final
        // 'MetricSampleGroup' object in the sequence of sample groups
        // contained by this object.  Note that the iterator will remain valid
        // until this sample is modified by invoking 'appendGroup' or
        // 'removeAllRecords()'.

    int numGroups() const;
        // Return the number of record groups (i.e., 'MetricSampleGroup'
        // objects) that are contained in this object.

    int numRecords() const;
        // Return the total number of records in this sample (i.e., the sum of
        // the lengths of all the appended record groups).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const MetricSample& lhs, const MetricSample& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' samples have the same
    // value, and 'false' otherwise.  Two samples have the same value if they
    // have the same timestamp value, contain the same number of record
    // groups, and if the respective groups of records at each index position
    // have the same value.

bool operator!=(const MetricSample& lhs, const MetricSample& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' samples do not have the
    // same value, and 'false' otherwise.  Two samples do not have the same
    // value if they have different values for their timestamps, or number of
    // record groups, or if any of the groups of records at corresponding
    // indices have different values.

bsl::ostream& operator<<(bsl::ostream& stream, const MetricSample& rhs);
    // Write a formatted description of the specified 'rhs' to the specified
    // 'stream' and return a reference to the modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class MetricSampleGroup
                          // -----------------------

// CREATORS
inline
MetricSampleGroup::MetricSampleGroup()
: d_records_p(0)
, d_numRecords(0)
, d_elapsedTime()
{
}

inline
MetricSampleGroup::MetricSampleGroup(const MetricRecord        *records,
                                     int                        numRecords,
                                     const bsls::TimeInterval&  elapsedTime)
: d_records_p(records)
, d_numRecords(numRecords)
, d_elapsedTime(elapsedTime)
{
}

inline
MetricSampleGroup::MetricSampleGroup(const MetricSampleGroup& original)
: d_records_p(original.d_records_p)
, d_numRecords(original.d_numRecords)
, d_elapsedTime(original.d_elapsedTime)
{
}

// MANIPULATORS
inline
MetricSampleGroup& MetricSampleGroup::operator=(const MetricSampleGroup& rhs)
{
    d_records_p   = rhs.d_records_p;
    d_numRecords  = rhs.d_numRecords;
    d_elapsedTime = rhs.d_elapsedTime;
    return *this;
}

inline
void MetricSampleGroup::setElapsedTime(const bsls::TimeInterval& elapsedTime)
{
    d_elapsedTime = elapsedTime;
}

inline
void MetricSampleGroup::setRecords(const MetricRecord *records,
                                   int                 numRecords)
{
    d_records_p  = records;
    d_numRecords = numRecords;
}

// ACCESSORS
inline
const MetricRecord *MetricSampleGroup::records() const
{
    return d_records_p;
}

inline
int MetricSampleGroup::numRecords() const
{
    return d_numRecords;
}

inline
const bsls::TimeInterval& MetricSampleGroup::elapsedTime() const
{
    return d_elapsedTime;
}

inline
MetricSampleGroup::const_iterator MetricSampleGroup::begin() const
{
    return d_records_p;
}

inline
MetricSampleGroup::const_iterator MetricSampleGroup::end() const
{
    return d_records_p + d_numRecords;
}

}  // close package namespace

// FREE OPERATORS
inline
bool balm::operator==(const MetricSampleGroup& lhs,
                      const MetricSampleGroup& rhs)
{
    return lhs.records()     == rhs.records()
        && lhs.numRecords()  == rhs.numRecords()
        && lhs.elapsedTime() == rhs.elapsedTime();
}

inline
bool balm::operator!=(const MetricSampleGroup& lhs,
                      const MetricSampleGroup& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balm::operator<<(bsl::ostream&            stream,
                               const MetricSampleGroup& rhs)
{
    return rhs.print(stream, 0, -1);
}

namespace balm {
                             // ------------------
                             // class MetricSample
                             // ------------------

// CREATORS
inline
MetricSample::MetricSample(bslma::Allocator *basicAllocator)
: d_timeStamp()
, d_records(basicAllocator)
, d_numRecords(0)
{
}

inline
MetricSample::~MetricSample()
{
}

// MANIPULATORS
inline
void MetricSample::setTimeStamp(const bdlt::DatetimeTz& timeStamp)
{
    d_timeStamp = timeStamp;
}

inline
void MetricSample::appendGroup(const MetricSampleGroup& group)
{
    if (0 < group.numRecords()) {
        d_records.push_back(group);
        d_numRecords += group.numRecords();
    }
}

inline
void MetricSample::appendGroup(const MetricRecord        *records,
                               int                        numRecords,
                               const bsls::TimeInterval&  elapsedTime)
{
    if (0 < numRecords) {
        d_records.push_back(SampleGroup(records, numRecords, elapsedTime));
        d_numRecords += numRecords;
    }
}

inline
void MetricSample::removeAllRecords()
{
    d_records.clear();
    d_numRecords = 0;
}

// ACCESSORS
inline
const MetricSampleGroup& MetricSample::sampleGroup(int index) const
{
    return d_records[index];
}

inline
const bdlt::DatetimeTz& MetricSample::timeStamp() const
{
    return d_timeStamp;
}

inline
MetricSample::const_iterator MetricSample::begin() const
{
    return d_records.begin();
}

inline
MetricSample::const_iterator MetricSample::end() const
{
    return d_records.end();
}

inline
int MetricSample::numGroups() const
{
    return static_cast<int>(d_records.size());
}

inline
int MetricSample::numRecords() const
{
    return d_numRecords;
}
}  // close package namespace

// FREE OPERATORS
inline
bool balm::operator==(const MetricSample& lhs, const MetricSample& rhs)
{
    return lhs.d_timeStamp == rhs.d_timeStamp
        && lhs.d_records   == rhs.d_records;
}

inline
bool balm::operator!=(const MetricSample& lhs, const MetricSample& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balm::operator<<(bsl::ostream& stream, const MetricSample& rhs)
{
    return rhs.print(stream, 0, -1);
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
