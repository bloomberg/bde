// baem_metricsample.h              -*-C++-*-
#ifndef INCLUDED_BAEM_METRICSAMPLE
#define INCLUDED_BAEM_METRICSAMPLE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for a sample of collected metric records.
//
//@CLASSES:
// baem_MetricSampleGroup: a group of records describing the same time period
//      baem_MetricSample: a sample of collected metric records
//
//@SEE_ALSO: baem_publisher, baem_metricrecord
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a container used to store a sample of
// recorded metric information.  A 'baem_MetricSample' contains a collection
// of addresses to (external) 'baem_MetricRecord' objects containing
// the aggregated record values for a series of metrics.  The records in a
// sample are broken into a series of groups, each group is represented by
// a 'baem_MetricSampleGroup' object.  Each 'baem_MetricSampleGroup' contains a
// sequence of records and an elapsed time value, indicating the time period
// over which those records were collected.  Finally, a 'baem_MetricSample'
// object contains a timestamp value used to indicate when the sample was
// taken.
//
///Thread Safety
///-------------
// 'baem_MetricSample' and 'baem_MetricSampleGroup' are both *const*
// *thread-safe*, meaning that accessors may be invoked concurrently from
// different threads, but it is not safe to access or modify an object in
// one thread while another thread modifies the same object.
//
///Usage
///-----
// The following example demonstrates how to create and use a metric sample.
// We start by initializing several 'baem_MetricRecord' values, which we will
// add to the sample.  Note that in this example we create the 'baem_MetricId'
// objects by hand; however, in practice ids should be obtained from a
// 'baem_MetricRegistry' object (such as the one owned by a
// 'baem_MetricsManager').
//..
//  bslma_Allocator *allocator = bslma_Default::allocator(0);
//
//  baem_Category myCategory("MyCategory");
//  baem_MetricDescription descA(&myCategory, "MetricA");
//  baem_MetricDescription descB(&myCategory, "MetricB");
//  baem_MetricDescription descC(&myCategory, "MetricC");
//
//  baem_MetricId metricA(&descA);
//  baem_MetricId metricB(&descB);
//  baem_MetricId metricC(&descC);
//
//  const int TZ = 0;  // GMT time zone offset
//
//  bdet_DatetimeTz timeStamp(bdet_Datetime(2008, 3, 26, 13, 30, 0, 0), TZ);
//  baem_MetricRecord recordA(metricA, 0, 0, 0, 0);
//  baem_MetricRecord recordB(metricB, 1, 2, 3, 4);
//  baem_MetricRecord recordC(metricC, 4, 3, 2, 1);
//..
// Now we create the two arrays of metric records whose addresses we will
// later add to the metric sample:
//..
//  baem_MetricRecord buffer1[] = { recordA, recordB };
//  bsl::vector<baem_MetricRecord> buffer2(allocator);
//  buffer2.push_back(recordC);
//..
// Next we create a 'baem_MetricSample' object, 'sample', and set its timestamp
// property.  Then we add two groups of records (containing the addresses of
// our two record arrays) to the sample we have created.  Since the records
// were not actually collected over a period of time, we supply an arbitrary
// elapsed time value of 1 second and 2 seconds (respectively) for the two
// groups added to the sample.  Note that these arrays must remain valid for
// the lifetime of 'sample'.
//..
//  baem_MetricSample sample(allocator);
//  sample.setTimeStamp(timeStamp);
//  sample.appendGroup(buffer1,
//                     sizeof(buffer1) / sizeof(*buffer1),
//                     bdet_TimeInterval(1.0));
//  sample.appendGroup(buffer2.data(),
//                     buffer2.size(),
//                     bdet_TimeInterval(2.0));
//..
// We can verify the basic properties of our sample:
//..
//       assert(timeStamp            == sample.timeStamp());
//       assert(2                    == sample.numGroups());
//       assert(3                    == sample.numRecords());
//       assert(bdet_TimeInterval(1) == sample.sampleGroup(0).elapsedTime());
//       assert(buffer1              == sample.sampleGroup(0).records());
//       assert(2                    == sample.sampleGroup(0).numRecords());
//       assert(bdet_TimeInterval(2) == sample.sampleGroup(1).elapsedTime());
//       assert(buffer2.data()       == sample.sampleGroup(1).records());
//       assert(1                    == sample.sampleGroup(1).numRecords());
//..
// Finally we can obtain an iterator over the sample's sequence of groups.  In
// this simple example, we iterate over the groups of records in the sample
// and, for each group, iterate over the records in that group, writing those
// records to the console.
//..
//  baem_MetricSample::const_iterator sampleIt = sample.begin();
//  for ( ; sampleIt != sample.end(); ++sampleIt) {
//      baem_MetricSampleGroup::const_iterator groupIt = sampleIt->begin();
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_METRICRECORD
#include <baem_metricrecord.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class baem_MetricSampleGroup
                        // ============================

class baem_MetricSampleGroup {
    // This class provides an *in-core* value-semantic representation of a
    // group of metric record values.  This class contains the address of an
    // array of (externally managed) 'baem_MetricRecord' objects, the number
    // of records in that array, and an elapsed time value (used to indicate
    // the time span over which the metric values were aggregated).

    // DATA
    const baem_MetricRecord *d_records_p;   // array of records (held, not
                                            // owned)

    int                      d_numRecords;  // number of records in array

    bdet_TimeInterval        d_elapsedTime; // interval described by records

  public:
    // PUBLIC TYPES
    typedef const baem_MetricRecord *const_iterator;
        // A 'const_iterator' is an alias for an iterator over the
        // non-modifiable records referenced in a 'baem_MetricSampleGroup'.

    // CREATORS
    baem_MetricSampleGroup();
        // Create an empty sample group.  By default, the 'records()' address
        // is 0, 'numRecords()' is 0, and the 'elapsedTime()' is
        // the default constructed 'bdet_TimeInterval'.

    baem_MetricSampleGroup(const baem_MetricRecord  *records,
                           int                       numRecords,
                           const bdet_TimeInterval&  elapsedTime);
        // Create a sample group containing the specified sequence of
        // 'records' of length 'numRecords', recorded over a period whose
        // duration is the specified 'elapsedTime'.  The behavior is undefined
        // unless '0 <= numRecords' and 'records' points to a contiguous
        // sequence of (at least) 'numRecords' metric records.  Note that the
        // contents of 'records' is *not* copied and the supplied array must
        // remain valid for the productive lifetime of this object or until
        // the records are set to a different sequence -- i.e., by calling
        // the 'setRecords' manipulator.

    baem_MetricSampleGroup(const baem_MetricSampleGroup& original);
        // Create a sample group having the same (in-core) value as the
        // specified 'original' sample group.

    // ~baem_MetricSampleGroup();
        // Destroy this sample group.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    baem_MetricSampleGroup& operator=(const baem_MetricSampleGroup& rhs);
        // Assign to this sample group the value of the specified 'rhs' sample
        // group, and return a reference to this modifiable sample group.
        // Note that only the pointer to the 'baem_MetricRecord' array and the
        // length are copied, and not the records themselves.

    void setElapsedTime(const bdet_TimeInterval& elapsedTime);
        // Set the elapsed time (used to indicate the time span over which
        // this object's metric records were aggregated) to the specified
        // 'elapsedTime'.

    void setRecords(const baem_MetricRecord *records, int numRecords);
        // Set the sequence of records referred to by this sample group to the
        // specified sequence of 'records' of length 'numRecords'.  The
        // behavior is undefined unless '0 <= numRecords', and 'records'
        // refers to a contiguous sequence of (at least) 'numRecords'.  Note
        // that the contents of 'records' is *not* copied and the supplied
        // array must remain valid for the productive lifetime of this object
        // or until the records are set to a different sequence -- i.e., by
        // calling the 'setRecords' manipulator.

    // ACCESSORS
    const baem_MetricRecord *records() const;
        // Return the address of the contiguous sequence of non-modifiable
        // records of length 'numRecords()'.

    int numRecords() const;
        // Return the number of records (referenced to by 'records()') in this
        // object.

    const bdet_TimeInterval& elapsedTime() const;
        // Return a reference to the non-modifiable elapsed time interval over
        // which this object's metric records were aggregated.

    const_iterator begin() const;
        // Return an iterator positioned at the beginning of the sequence of
        // 'baem_MetricRecord' objects referenced by this object.

    const_iterator end() const;
        // Return an iterator positioned one past the final
        // 'baem_MetricRecord' object in the sequence of records referenced by
        // this object.

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
bool operator==(const baem_MetricSampleGroup& lhs,
                const baem_MetricSampleGroup& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sample groups have
    // the same value, and 'false' otherwise.  Two sample groups
    // have the same value if the respective record sequence-addresses, number
    // of records, and elapsed time are the same.

inline
bool operator!=(const baem_MetricSampleGroup& lhs,
                const baem_MetricSampleGroup& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sample groups do not
    // have the same value, and 'false' otherwise.  Two sample groups do not
    // have the same value if any of the respective record-sequence addresses,
    // number of records, or elapsed time, are not the same.

inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const baem_MetricSampleGroup& rhs);
    // Write a formatted description of the specified 'rhs' to the specified
    // 'stream' and return a reference to the modifiable 'stream'.

                        // =======================
                        // class baem_MetricSample
                        // =======================

class baem_MetricSample {
    // This class provides an *in-core* value-semantic representation of a
    // sample of metric values.  The class contains a collection of addresses
    // to (external) 'baem_MetricRecord' objects holding the values for their
    // respective metrics (aggregated over some period of time).  The metric
    // records contained by a sample are broken into a series of groups, which
    // are represented by 'baem_MetricSampleGroup' objects.  Each group
    // contains a sequence of records and an elapsed time value, indicating the
    // period of time over which those records were taken.  This class also
    // provides a timestamp value, used to indicate when the sample was
    // collected.  The class provides a method, 'appendGroups', that appends a
    // group of metric records to the sample.  Arrays supplied using
    // 'appendGroups' must be valid for the productive lifetime of the
    // 'baem_MetricSample' object or until they are removed -- i.e., by calling
    // 'removeAllRecords'.

    // PRIVATE TYPES
    typedef baem_MetricSampleGroup SampleGroup;

    // DATA
    bdet_DatetimeTz           d_timeStamp;   // time the records were
                                             // collected

    bsl::vector<SampleGroup>  d_records;     // vector of groups of records

    int                       d_numRecords;  // total number of records

    // FRIENDS
    friend bool operator==(const baem_MetricSample& lhs,
                           const baem_MetricSample& rhs);

  public:
    // PUBLIC TYPES
    typedef bsl::vector<baem_MetricSampleGroup>::const_iterator const_iterator;
        // A 'const_iterator' is an alias for an iterator over the
        // non-modifiable sample groups contained in a 'baem_MetricSample'.

    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_MetricSample,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_MetricSample(bslma_Allocator *basicAllocator = 0);
        // Create an empty metric sample.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    baem_MetricSample(const baem_MetricSample&  original,
                      bslma_Allocator          *basicAllocator = 0);
        // Create a metric sample containing the same value as the specified
        // 'original' sample.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that copying the contained
        // 'baem_MetricSampleGroup' objects copies only the pointers to their
        // respective 'baem_MetricRecord' arrays, and does not copy the
        // records themselves; hence, these record arrays must remain
        // valid for the productive lifetimes of all copied objects or until
        // the records are removed -- i.e., by calling 'removeAllRecords'.

    ~baem_MetricSample();
        // Destroy this metric sample.

    // MANIPULATORS
    baem_MetricSample& operator=(const baem_MetricSample& rhs);
        // Assign to this sample the value of the specified 'rhs' sample and
        // return a reference to this modifiable sample.  Note that copying
        // the 'baem_MetricSampleGroup' objects contained in 'rhs' copies only
        // the pointers to their respective 'baem_MetricRecord' arrays, and
        // does not not copy records themselves; hence, these record arrays
        // must remain valid for the productive lifetimes of all copied
        // objects or until the records are removed -- i.e., by calling
        // 'removeAllRecords'.

    void setTimeStamp(const bdet_DatetimeTz& timeStamp);
        // Set the timestamp (used to indicate when the sample was taken) to
        // the specified 'timeStamp'.

    void appendGroup(const baem_MetricSampleGroup& group);
        // Append the specified 'group' of records to the sequence of groups
        // maintained by this sample.  If 'group.numRecords()' is 0 this
        // method has no effect.  The behavior is undefined unless
        // 'group.elapsedTime() > bdet_TimeInterval(0, 0)'.  Note that the
        // 'baem_MetricRecord' objects referred to by 'records' are *not*
        // copied: hence, the supplied array must remain valid for the
        // productive lifetime of this object or until the group is removed
        // -- i.e., by calling 'removeAllRecords()'.

    void appendGroup(const baem_MetricRecord  *records,
                     int                       numRecords,
                     const bdet_TimeInterval&  elapsedTime);
        // Append to the sequence of groups maintained by this sample a new
        // group containing the specified sequence of 'records' of length
        // 'numRecords' measuring the specified 'elapsedTime'.  The behavior is
        // undefined unless '0 <= numRecords', 'records' refers to a
        // contiguous sequence of (at least) 'numRecords', and
        // 'elapsedTime > bdet_TimeInterval(0, 0)'.  Note that 'records' is
        // *not* copied: hence, the supplied array must remain valid for the
        // productive lifetime of this object or until the records are removed
        // -- i.e., by calling 'removeAllRecords()'.  This method is
        // functionally equivalent to:
        //..
        //  appendGroup(baem_MetricSampleGroup(records,
        //                                     numRecords,
        //                                     elapsedTime));
        //..

    void removeAllRecords();
        // Remove all metric records from this sample.

    // ACCESSORS
    const baem_MetricSampleGroup& sampleGroup(int index) const;
        // Return a reference to the non-modifiable 'baem_MetricSampleGroup'
        // object at the specified 'index' in this sample.  The behavior is
        // undefined unless '0 <= index < numGroups()'.  Note that the
        // returned reference will remain valid until this sample is modified
        // -- e.g., by invoking 'appendGroup' or 'removeAllRecords()'.

    const bdet_DatetimeTz& timeStamp() const;
        // Return a reference to the non-modifiable timestamp for this
        // sample.

    const_iterator begin() const;
        // Return an iterator positioned at the beginning of the sequence of
        // 'baem_MetricSampleGroup' objects contained by this object.  Note
        // that the iterator will remain valid until this sample is modified
        // -- e.g., by invoking either 'appendGroups' or
        // 'removeAllRecords()'.

    const_iterator end() const;
        // Return an iterator positioned one one past the final
        // 'baem_MetricSampleGroup' object in the sequence of sample groups
        // contained by this object.  Note that the iterator will remain valid
        // until this sample is modified -- e.g., by invoking 'appendGroup'
        // or 'removeAllRecords()'.

    int numGroups() const;
        // Return the number of record groups (i.e., 'baem_MetricSampleGroup'
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
bool operator==(const baem_MetricSample& lhs, const baem_MetricSample& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' samples have the same
    // value, and 'false' otherwise.  Two samples have the same value if they
    // have the same timestamp value, contain the same number of record
    // groups, and if the respective groups of records at each index position
    // have the same value.

inline
bool operator!=(const baem_MetricSample& lhs, const baem_MetricSample& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' samples do not have the
    // same value, and 'false' otherwise.  Two samples do not have the same
    // value if they have different values for their timestamps, or number of
    // record groups, or if any of the groups of records at corresponding
    // indices have different values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_MetricSample& rhs);
    // Write a formatted description of the specified 'rhs' to the specified
    // 'stream' and return a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ----------------------------
                        // class baem_MetricSampleGroup
                        // ----------------------------

// CREATORS
inline
baem_MetricSampleGroup::baem_MetricSampleGroup()
: d_records_p(0)
, d_numRecords(0)
, d_elapsedTime()
{
}

inline
baem_MetricSampleGroup::baem_MetricSampleGroup(
                                         const baem_MetricRecord  *records,
                                         int                       numRecords,
                                         const bdet_TimeInterval&  elapsedTime)
: d_records_p(records)
, d_numRecords(numRecords)
, d_elapsedTime(elapsedTime)
{
}

inline
baem_MetricSampleGroup::baem_MetricSampleGroup(
                                        const baem_MetricSampleGroup& original)
: d_records_p(original.d_records_p)
, d_numRecords(original.d_numRecords)
, d_elapsedTime(original.d_elapsedTime)
{
}

// MANIPULATORS
inline
baem_MetricSampleGroup& baem_MetricSampleGroup::operator=(
                                             const baem_MetricSampleGroup& rhs)
{
    d_records_p   = rhs.d_records_p;
    d_numRecords  = rhs.d_numRecords;
    d_elapsedTime = rhs.d_elapsedTime;
    return *this;
}

inline
void baem_MetricSampleGroup::setElapsedTime(
                                          const bdet_TimeInterval& elapsedTime)
{
    d_elapsedTime = elapsedTime;
}

inline
void baem_MetricSampleGroup::setRecords(const baem_MetricRecord *records,
                                        int                      numRecords)
{
    d_records_p  = records;
    d_numRecords = numRecords;
}

// ACCESSORS
inline
const baem_MetricRecord *baem_MetricSampleGroup::records() const
{
    return d_records_p;
}

inline
int baem_MetricSampleGroup::numRecords() const
{
    return d_numRecords;
}

inline
const bdet_TimeInterval& baem_MetricSampleGroup::elapsedTime() const
{
    return d_elapsedTime;
}

inline
baem_MetricSampleGroup::const_iterator baem_MetricSampleGroup::begin() const
{
    return d_records_p;
}

inline
baem_MetricSampleGroup::const_iterator baem_MetricSampleGroup::end() const
{
    return d_records_p + d_numRecords;
}

// FREE OPERATORS
inline
bool operator==(const baem_MetricSampleGroup& lhs,
                const baem_MetricSampleGroup& rhs)
{
    return lhs.records()     == rhs.records()
        && lhs.numRecords()  == rhs.numRecords()
        && lhs.elapsedTime() == rhs.elapsedTime();
}

inline
bool operator!=(const baem_MetricSampleGroup& lhs,
                const baem_MetricSampleGroup& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const baem_MetricSampleGroup& rhs)
{
    return rhs.print(stream, 0, -1);
}

                         // -----------------------
                         // class baem_MetricSample
                         // -----------------------

// CREATORS
inline
baem_MetricSample::baem_MetricSample(bslma_Allocator *basicAllocator)
: d_timeStamp()
, d_records(basicAllocator)
, d_numRecords(0)
{
}

inline
baem_MetricSample::~baem_MetricSample()
{
}

// MANIPULATORS
inline
void baem_MetricSample::setTimeStamp(const bdet_DatetimeTz& timeStamp)
{
    d_timeStamp = timeStamp;
}

inline
void baem_MetricSample::appendGroup(const baem_MetricSampleGroup& records)
{
    if (0 < records.numRecords()) {
        d_records.push_back(records);
        d_numRecords += records.numRecords();
    }
}

inline
void baem_MetricSample::appendGroup(const baem_MetricRecord  *records,
                                    int                       numRecords,
                                    const bdet_TimeInterval&  elapsedTime)
{
    if (0 < numRecords) {
        d_records.push_back(SampleGroup(records, numRecords, elapsedTime));
        d_numRecords += numRecords;
    }
}

inline
void baem_MetricSample::removeAllRecords()
{
    d_records.clear();
    d_numRecords = 0;
}

// ACCESSORS
inline
const baem_MetricSampleGroup& baem_MetricSample::sampleGroup(int index) const
{
    return d_records[index];
}

inline
const bdet_DatetimeTz& baem_MetricSample::timeStamp() const
{
    return d_timeStamp;
}

inline
baem_MetricSample::const_iterator baem_MetricSample::begin() const
{
    return d_records.begin();
}

inline
baem_MetricSample::const_iterator baem_MetricSample::end() const
{
    return d_records.end();
}

inline
int baem_MetricSample::numGroups() const
{
    return static_cast<int>(d_records.size());
}

inline
int baem_MetricSample::numRecords() const
{
    return d_numRecords;
}

// FREE OPERATORS
inline
bool operator==(const baem_MetricSample& lhs, const baem_MetricSample& rhs)
{
    return lhs.d_timeStamp == rhs.d_timeStamp
        && lhs.d_records   == rhs.d_records;
}

inline
bool operator!=(const baem_MetricSample& lhs, const baem_MetricSample& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_MetricSample& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
