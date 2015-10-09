// balm_metricsample.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricsample.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bdlt_currenttime.h>
#include <bdlt_dateutil.h>

#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// A 'balm::MetricSampleGroup' is a simple value containing three
// unconstrained properties, 'records' (and address to a sequence of records),
// 'numRecords' (the number of 'records' in the sequence), and 'elapsedTime' a
// time interval.  A 'balm::MetricSample' is a sequence of
// 'balm::MetricSampleGroup' objects with the additional restriction that empty
// groups are not permitted, and 'elapsedTime > bsls::TimeInterval(0, 0)'.
// ----------------------------------------------------------------------------
// balm::MetricSampleGroup
// CREATORS
// [ 3]  balm::MetricSampleGroup();
// [ 4]  balm::MetricSampleGroup(const balm::MetricRecord  *,
//                              int                       ,
//                              const bsls::TimeInterval&  );
// [ 6]  balm::MetricSampleGroup(const balm::MetricSampleGroup& );
// [ 3]  ~balm::MetricSampleGroup();
// MANIPULATORS
// [ 7]  balm::MetricSampleGroup& operator=(balm::MetricSampleGroup&);
// [ 3]  void setElapsedTime(const bsls::TimeInterval& );
// [ 3]  void setRecords(const balm::MetricRecord *, int );
// ACCESSORS
// [ 3]  const balm::MetricRecord *records() const;
// [ 3]  int numRecords() const;
// [ 3]  const bsls::TimeInterval& elapsedTime() const;
// [ 9]  const_iterator begin() const;
// [ 9]  const_iterator end() const;
// [ 8]  bsl::ostream& print(bsl::ostream& , int, int ) const;
// FREE OPERATORS
// [ 5]  bool operator==(const balm::MetricSampleGroup& ,
//                       const balm::MetricSampleGroup& );
// [ 5]  bool operator!=(const balm::MetricSampleGroup& ,
//                       const balm::MetricSampleGroup& );
// [ 8]  bsl::ostream& operator<<(bsl::ostream& ,
//                                const balm::MetricSampleGroup& );
// ----------------------------------------------------------------------------
// balm::MetricSample
// CREATORS
// [11]  balm::MetricSample(bslma::Allocator *);
// [13]  balm::MetricSample(const balm::MetricSample&  ,
//                         bslma::Allocator         *);
// [11]  ~balm::MetricSample();
// MANIPULATORS
// [14]  balm::MetricSample& operator=(const balm::MetricSample& );
// [11]  void setTimeStamp(const bdlt::DatetimeTz& );
// [11]  void appendGroup(const balm::MetricSampleGroup& );
// [18]  void appendGroup(const balm::MetricRecord  *,
//                        int                       ,
//                        const bsls::TimeInterval&  );
// [16]  void removeAllRecords();
// ACCESSORS
// [11]  const balm::MetricSampleGroup& sampleGroup(int ) const;
// [11]  const bdlt::DatetimeTz& timeStamp() const;
// [17]  const_iterator begin() const;
// [17]  const_iterator end() const;
// [11]  int numGroups() const;
// [11]  int numRecords() const;
// [15]   bsl::ostream& print(bsl::ostream&, int, int) const;
// FREE OPERATORS
// [12]  bool operator==(balm::MetricSample& , balm::MetricSample& );
// [12]  bool operator!=(balm::MetricSample& , balm::MetricSample& );
// [15]  bsl::ostream& operator<<(bsl::ostream&, const balm::MetricSample& );
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST: 'balm::MetricSampleGroup'
// [ 2] BREATHING TEST: 'balm::MetricSample'
// [ 2] HELPER TEST: 'gg'
// [19] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::MetricSample                      Obj;
typedef balm::MetricSampleGroup                 Group;
typedef balm::MetricRecord                      Rec;
typedef balm::MetricDescription                 Desc;
typedef balm::MetricId                          Id;
typedef bsl::vector<balm::MetricRecord>         RecVec;

// ============================================================================
//                             Helper Functions
// ============================================================================

bool stringDiff(const bsl::string& expectedValue,
                const bsl::string& actualValue)
    // Return 'true' if the specified 'expectedValue' equals the specified
    // 'actualValue', otherwise return 'false' and output to the console
    // information about the lowest index where the two values differ.
{
    const char *exp = expectedValue.c_str();
    const char *act = actualValue.c_str();

    int index = 0;
    for (; *exp && (*exp == *act); ++exp, ++act, ++index) {
    }
    if (*exp == *act) {
        return true;                                                  // RETURN
    }
    bsl::cout << "expcetedValue[" << index << "] = " << (int)*exp
              << " (" << *exp << ")" << bsl::endl
              << "actualValue[" << index << "] = " << (int)*act
              << " (" << *act << ")" << bsl::endl;

    bsl::cout << (int)*exp << "==" << (int)*act << "?\n";
    return false;
}

//=============================================================================
//                        GENERATOR LANGUAGE FOR gg
//=============================================================================
//
// The gg function interprets a given 'spec' in order from left to right to
// build up a schedule according to a custom language.
//
//  Spec      := (SampleGroup)*
//
//  SampleGroup := 'R' NumRecords  'E' ElapsedTime
//
//  NumRecords :=  '1'|'2'|'3'|'4'|'5'  %% # of records to place in the group
//                    |'6'|'7'|'8'|'9'
//
//  NumRecords :=  '1'|'2'|'3'|'4'|'5'  %% # Elapsed time in seconds for the
//                    |'6'|'7'|'8'|'9'  %% group
//
// 'gg' syntax usage examples:
//  Here are some examples, and sequence of actions that would be created:
//
// Spec String      Result Groups               Description
// -----------      -------------               -----------
// ""              [                            0 sample groups
//                 ]
//
// "R1E2"          [                            1 sample group with an elapsed
//                   (2, 0)                     time of 2 seconds, and a
//                   [ A.A: 1 1 1 1 ]           single record (with an
//                 ]                            unspecified value)
//
// "R2E1R1E4"      [                            2 samples groups, the first
//                   (1, 0)                     with an elapsed time of 1s
//                   [ A.A: 1 1 1 1 ]           and 2 records, the second with
//                   [ B.B: 2 2 2 2 ]           an elapsed time of 4s and 1
//                 ]                            record
//                 [
//
//                   (4, 0)
//                   [ C.C: 1 1 1 1 ]
//                 ]

int gg(bsl::vector<balm::MetricSampleGroup>   *groups,
        const char                            *specification,
        const bsl::vector<balm::MetricRecord>&  recordBuffer)
    // Append to the specified 'groups', the sample groups indicated by the
    // specified 'specification' (as described by the generator language 'gg',
    // documented above) using the specified 'ids' to provide metric ids;
    // append any records created to the specified 'recordBuffer'.  Behavior
    // is undefined unless 'recordBuffer' is at least as long as the largest
    // group in 'specification'.  Note that which particular records from
    // 'recordBuffer' are added to a particular group in 'groups' is
    // unspecified, but deterministic.
{
    const char *c = specification;

    int totalNumRecords = 0;
    const balm::MetricRecord *head    = recordBuffer.data();
    const balm::MetricRecord *current = head;
    while (*c) {
        if (!*c || !*(c+1) || !(*c+2) || !(*c+3)) {
            bsl::cout << "Invalid termination for specification: "
                      << specification << bsl::endl;
        }
        if (*c != 'R') {
            bsl::cout << "Unexpected char in specification: " << specification
                      << "  expecting: 'R'   found: '" << *c << "'"
                      << bsl::endl;
            BSLS_ASSERT(false);
        }

        int numRecords = *(c + 1) - '0';
        if (numRecords < 0 || numRecords > 9) {
            bsl::cout << "Invalid number of records in specification: "
                      << specification << "  found: '" << *c << "'"
                      << bsl::endl;
            BSLS_ASSERT(false);
        }

        if (*(c + 2) != 'E') {
            bsl::cout << "Unexpected char in specification: " << specification
                      << "  expecting: 'E'   found: '" << *c << "'"
                      << bsl::endl;
            BSLS_ASSERT(false);
        }

        int elapsedTime = *(c + 3) - '0';
        if (elapsedTime < 0 || elapsedTime > 9) {
            bsl::cout << "Invalid elapsed time in specification: "
                      << specification << "  found: '" << *c << "'"
                      << bsl::endl;
            BSLS_ASSERT(false);
        }

        if (head + recordBuffer.size() < current + numRecords) {
            current = head;
        }
        groups->push_back(Group(current,
                                numRecords,
                                bsls::TimeInterval(elapsedTime, 0)));

        current         += numRecords;
        totalNumRecords += numRecords;
        c += 4;
    }
    return totalNumRecords;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma::TestAllocator testAllocator, defaultAllocator;
    bslma::Allocator *Z = &testAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    balm::Category cat_A("A", true);
    Desc desc_A(&cat_A, "A"); const Desc *DESC_A = &desc_A;
    Desc desc_B(&cat_A, "B"); const Desc *DESC_B = &desc_B;
    Desc desc_C(&cat_A, "C"); const Desc *DESC_C = &desc_C;
    Desc desc_D(&cat_A, "D"); const Desc *DESC_D = &desc_D;
    Desc desc_E(&cat_A, "E"); const Desc *DESC_E = &desc_E;
    Desc desc_F(&cat_A, "F"); const Desc *DESC_F = &desc_F;
    Desc desc_G(&cat_A, "G"); const Desc *DESC_G = &desc_G;

    Id id_A(DESC_A); const Id& ID_A = id_A;
    Id id_B(DESC_B); const Id& ID_B = id_B;
    Id id_C(DESC_C); const Id& ID_C = id_C;
    Id id_D(DESC_D); const Id& ID_D = id_D;
    Id id_E(DESC_E); const Id& ID_E = id_E;
    Id id_F(DESC_F); const Id& ID_F = id_F;
    Id id_G(DESC_G); const Id& ID_G = id_G;

    RecVec rv1(Z); const RecVec& RV1 = rv1;
    RecVec rv2(Z); const RecVec& RV2 = rv2;
    RecVec rv3(Z); const RecVec& RV3 = rv3;
    RecVec rv4(Z); const RecVec& RV4 = rv4;

    rv1.push_back(balm::MetricRecord(Id(0), 1, 2, 3, 4));
    rv1.push_back(balm::MetricRecord( ID_A, 3, 2, 1, 0));
    rv2.push_back(balm::MetricRecord( ID_B, 6, 7, 8, 9));
    rv3.push_back(balm::MetricRecord( ID_C, 8, 7, 6, 5));
    rv4.push_back(balm::MetricRecord( ID_D, 3, 3, 3, 3));
    rv4.push_back(balm::MetricRecord( ID_E, 4, 4, 4, 4));
    rv4.push_back(balm::MetricRecord( ID_F, 5, 5, 5, 5));

    RecVec recordBuffer(Z); const RecVec& RECORD_BUFFER = recordBuffer;
    recordBuffer.push_back(balm::MetricRecord(Id(0), 1, 1, 1, 1));
    recordBuffer.push_back(balm::MetricRecord( ID_A, 2, 2, 2, 2));
    recordBuffer.push_back(balm::MetricRecord( ID_B, 3, 3, 3, 3));
    recordBuffer.push_back(balm::MetricRecord( ID_C, 4, 4, 4, 4));
    recordBuffer.push_back(balm::MetricRecord( ID_D, 5, 5, 5, 5));
    recordBuffer.push_back(balm::MetricRecord( ID_E, 6, 6, 6, 6));
    recordBuffer.push_back(balm::MetricRecord( ID_F, 7, 7, 7, 7));
    recordBuffer.push_back(balm::MetricRecord( ID_G, 8, 8, 8, 8));
    recordBuffer.push_back(balm::MetricRecord( ID_A, 9, 9, 9, 9));

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// The following example demonstrates how to create and use a metric sample.
// We start by initializing several 'balm::MetricRecord' values, which we will
// add to the sample.  Note that in this example we create the 'balm::MetricId'
// objects by hand; however, in practice ids should be obtained from a
// 'balm::MetricRegistry' object (such as the one owned by a
// 'balm::MetricsManager').
//..
    bslma::Allocator *allocator = bslma::Default::allocator(0);

    balm::Category myCategory("MyCategory");
    balm::MetricDescription descA(&myCategory, "MetricA");
    balm::MetricDescription descB(&myCategory, "MetricB");
    balm::MetricDescription descC(&myCategory, "MetricC");

    balm::MetricId metricA(&descA);
    balm::MetricId metricB(&descB);
    balm::MetricId metricC(&descC);

    const int TZ = 0;  // UTC time zone offset

    bdlt::DatetimeTz timeStamp(bdlt::Datetime(2008, 3, 26, 13, 30, 0, 0), TZ);
    balm::MetricRecord recordA(metricA, 0, 0, 0, 0);
    balm::MetricRecord recordB(metricB, 1, 2, 3, 4);
    balm::MetricRecord recordC(metricC, 4, 3, 2, 1);
//..
// Now we create the two arrays of metric records whose addresses we will
// later add to the metric sample:
//..
    balm::MetricRecord              buffer1[] = { recordA, recordB };
    bsl::vector<balm::MetricRecord> buffer2(allocator);
    buffer2.push_back(recordC);
//..
// Next we create a 'balm::MetricSample' object, 'sample', and set its
// timestamp property.  Then we add two groups of records (containing the
// addresses of our two record arrays) to the sample we have created.  Since
// the records were not actually collected over a period of time, we supply an
// arbitrary elapsed time value of 1 second and 2 seconds (respectively) for
// the two groups added to the sample.  Note that these arrays must remain
// valid for the lifetime of 'sample'.
//..
    balm::MetricSample sample(allocator);
    sample.setTimeStamp(timeStamp);
    sample.appendGroup(buffer1,
                       sizeof(buffer1) / sizeof(*buffer1),
                       bsls::TimeInterval(1.0));
    sample.appendGroup(buffer2.data(),
                       buffer2.size(),
                       bsls::TimeInterval(2.0));
//..
// We can verify the basic properties of our sample:
//..
         ASSERT(timeStamp             == sample.timeStamp());
         ASSERT(2                     == sample.numGroups());
         ASSERT(3                     == sample.numRecords());
         ASSERT(bsls::TimeInterval(1) == sample.sampleGroup(0).elapsedTime());
         ASSERT(buffer1               == sample.sampleGroup(0).records());
         ASSERT(2                     == sample.sampleGroup(0).numRecords());
         ASSERT(bsls::TimeInterval(2) == sample.sampleGroup(1).elapsedTime());
         ASSERT(buffer2.data()        == sample.sampleGroup(1).records());
         ASSERT(1                     == sample.sampleGroup(1).numRecords());
//..
// Finally we can obtain an iterator over the sample's sequence of groups.  In
// this simple example, we iterate over the groups of records in the sample
// and, for each group, iterate over the records in that group, writing those
// records to the console.
//..
    balm::MetricSample::const_iterator sampleIt = sample.begin();
    for ( ; sampleIt != sample.end(); ++sampleIt) {
        balm::MetricSampleGroup::const_iterator groupIt = sampleIt->begin();
        for ( ; groupIt != sampleIt->end(); ++groupIt) {
            bsl::cout << *groupIt << bsl::endl;
        }
    }
//..
// The output will look like:
//..
//  [ MyCategory.MetricA: 0 0 0 0 ]
//  [ MyCategory.MetricB: 1 2 3 4 ]
//  [ MyCategory.MetricC: 4 3 2 1 ]
//..
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'appendGroup(const balm::MetricRecord *, int, ...):
        //                                              'balm::MetricSample'
        //
        // Plan:
        //   For a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        //
        // Testing:
        //  void appendGroup(const balm::MetricRecord *,
        //                   int,
        //                   const bsls::TimeInterval& );
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nTesting 'appendGroup(const balm::MetricRecord *,...)': "
                 << "'balm::MetricSample'" << endl;
        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            int size = gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj mX(Z); const Obj& MX = mX;
            mX.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                mX.appendGroup(groups[j].records(),
                               groups[j].numRecords(),
                               groups[j].elapsedTime());
            }

            if (veryVeryVerbose) {
                P_(i); P(MX);
            }

            // Test the sample.
            ASSERT(timeStamp      == MX.timeStamp());
            ASSERT(groups.size()  == MX.numGroups());
            ASSERT(size           == MX.numRecords());
            for (int j = 0; j < MX.numGroups(); ++j) {
                ASSERT(groups[j] == MX.sampleGroup(j));
            }
            ASSERT( 0 == defaultAllocator.numBytesInUse() );
        }
        ASSERT( 0 == defaultAllocator.numBytesInUse() );
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'begin()', 'end()':  'balm::MetricSample'
        //
        // Plan:
        //   Test that accessors 'begin()' and 'end()' return iterators at the
        //   first element and one past the last element in the sequence of
        //   sample groups.
        //
        // Testing:
        //   const_iterator begin() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting begin() and end(): "
                          << " 'balm::MetricSample'" << endl;

        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            int size = gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj mX(Z); const Obj& MX = mX;
            mX.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                mX.appendGroup(groups[j]);
            }

            Obj::const_iterator it = MX.begin();
            for (int j = 0; j < groups.size(); ++j) {
                ASSERT(MX.end()  != it);
                ASSERT(groups[j] == *it);
                ++it;
            }
            ASSERT(MX.end() == it);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATOR 'removeAllRecords': 'balm::MetricSample'
        //
        // Concerns:
        //   That the removeAllRecords manipulator removes all the records from
        //   the sample and has no side effects.
        //
        // Plan:
        //   For a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Call removeAllRecords, verify that the
        //   sample has no records and there are no side-effects.
        //
        // Testing:
        //  void removeAllRecords();
        // --------------------------------------------------------------------

       if (verbose) cout << "Test for removeAllRecords(): "
                         << "'balm::MetricSample'\n" ;
        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            int size = gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj mX(Z); const Obj& MX = mX;
            mX.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                mX.appendGroup(groups[j]);
            }

            ASSERT(size          == MX.numRecords());
            ASSERT(groups.size() == MX.numGroups());
            ASSERT(timeStamp     == MX.timeStamp());

            mX.removeAllRecords();

            ASSERT(0             == MX.numRecords());
            ASSERT(0             == MX.numGroups());
            ASSERT(timeStamp     == MX.timeStamp());
        }
        ASSERT( 0 == defaultAllocator.numBytesInUse() );
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT (<<) OPERATOR: 'balm::MetricSample'
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   operator<<(ostream&, const balm::MetricSample&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                            "\nTesting 'print': 'balm::MetricSample'." << endl;

        bdlt::Date         date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                                    20080101));
        bdlt::DatetimeTz   timeStamp(bdlt::Datetime(date), 0);

        Obj mX(Z); const Obj& MX = mX;
        mX.setTimeStamp(timeStamp);
        mX.appendGroup(RV1.data(), RV1.size(), bsls::TimeInterval(1,0));
        mX.appendGroup(RV2.data(), RV2.size(), bsls::TimeInterval(2,0));

        bsl::ostringstream buf1, buf2;

        const char *EXP_1 =
            "[ 01JAN2008_00:00:00.000+0000  "
            "[   [ (1, 0)    [     [ INVALID_ID: 1 2 3 4 ]    "
            " [ A.A: 3 2 1 0 ]    ]   ]   [ (2, 0)   "
            " [     [ A.B: 6 7 8 9 ]    ]   ]  ] ] ";

        const char *EXP_2 =
            "   [ 01JAN2008_00:00:00.000+0000\n"
            "      [\n"
            "         [ (1, 0)\n"
            "            [\n"
            "               [ INVALID_ID: 1 2 3 4 ]\n"
            "               [ A.A: 3 2 1 0 ]\n"
            "            ]\n"
            "         ]\n"
            "         [ (2, 0)\n"
            "            [\n"
            "               [ A.B: 6 7 8 9 ]\n"
            "            ]\n"
            "         ]\n"
            "      ]\n"
            "   ]\n";

        buf1 << MX;
        MX.print(buf2, 1, 3);

        bsl::string val1 = buf1.str();
        bsl::string val2 = buf2.str();

        if (veryVerbose) {
            P(val1);
            P(EXP_1);
            P(val2);
            P(EXP_2);
        }

        ASSERT(bsl::string(EXP_1) == val1);
        ASSERT(bsl::string(EXP_2) == val2);
        if (veryVerbose) {
            stringDiff(EXP_1, val1);
            stringDiff(EXP_2, val2);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR: 'balm::MetricSample'
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   balm::MetricSample& operator=(const balm::MetricSample& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator: "
                          << "'balm::MetricSample'" << endl;

        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj u(Z); const Obj& U = u;
            u.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                u.appendGroup(groups[j]);
            }

            for (int j = 0; j < NUM_VALUES; ++j) {
                bsl::vector<balm::MetricSampleGroup> groups(Z);

                bdlt::Date    date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[j].d_date));
                bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
                gg(&groups, VALUES[j].d_groupSpec, RECORD_BUFFER);

                // Create the sample.
                Obj v(Z); const Obj& V = v;
                v.setTimeStamp(timeStamp);
                for (int k = 0; k < groups.size(); ++k) {
                    v.appendGroup(groups[k]);
                }

                Obj w(V, Z); const Obj& W = w;
                u = v;
                ASSERT(V == U);
                ASSERT(W == U);

                ASSERT(0 == defaultAllocator.numBytesInUse());
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj u(Z); const Obj& U = u;
            u.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                u.appendGroup(groups[j]);
            }

            Obj w(U, Z);  const Obj& W = w;              // control
            u = u;
            LOOP_ASSERT(i, W == U);

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT( 0 == defaultAllocator.numBytesInUse() );
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR: 'balm::MetricSample'
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricSample(const balm::MetricSample& );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor: "
                          << "'balm::MetricSample'" << endl;

        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj mX(Z); const Obj& MX = mX;
            mX.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                mX.appendGroup(groups[j]);
            }

            Obj mY(Z); const Obj& MY = mY;
            mY.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                mY.appendGroup(groups[j]);
            }

            Obj mZ(MX, Z); const Obj& MZ = mZ;

            ASSERT(MX == MY);
            ASSERT(MX == MZ);
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR: 'balm::MetricSample'
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Basic Test:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(balm::MetricSample&, balm::MetricSample&);
        //   bool operator!=(balm::MetricSample&, balm::MetricSample&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                            "\nTesting equality: 'balm::MetricSample'" << endl;

        if (veryVerbose) cout << "\tTest basic equality." << endl;

        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj u(Z); const Obj& U = u;
            u.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                u.appendGroup(groups[j]);
            }

            for (int j = 0; j < NUM_VALUES; ++j) {
                bsl::vector<balm::MetricSampleGroup> groups(Z);

                bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[j].d_date));
                bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
                gg(&groups, VALUES[j].d_groupSpec, RECORD_BUFFER);

                // Create the sample.
                Obj v(Z); const Obj& V = v;
                v.setTimeStamp(timeStamp);
                for (int k = 0; k < groups.size(); ++k) {
                    v.appendGroup(groups[k]);
                }

                bool isEqual = i == j;
                LOOP2_ASSERT(i, j, isEqual  == (U == V));
                LOOP2_ASSERT(i, j, !isEqual == (U != V));
                ASSERT( 0 == defaultAllocator.numBytesInUse() );
            }
        }

      } break;
     case 11: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //                                          'balm::MetricSample'
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        //
        // Testing:
        //  balm::MetricSample
        //  -----------------
        //  balm::MetricSample(bslma::Allocator *basicAllocator = 0);
        //  ~balm::MetricSample();
        //
        //  void appendGroup(const balm::MetricSampleGroup& );
        //  void setTimeStamp(const bdlt::DatetimeTz& );
        //
        //  const balm::MetricSampleGroup& sampleGroup(int ) const;
        //  const bdlt::DatetimeTz& timeStamp() const;
        //  int numRecords() const;
        //  int numGroup() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting primary mainpulators and accessors: "
                          << "'balm::MetricSample'" << endl;

        if (veryVerbose) cout << "\tdefault constructor\n";
        {
            Obj mX(Z); const Obj& MX = mX;
            ASSERT(0                   == MX.numRecords());
            ASSERT(bdlt::DatetimeTz()   == MX.timeStamp());
            ASSERT(0                   == MX.numGroups());
        }

        if (veryVerbose) cout << "\ttesting primary modifiers and accesors\n";

        struct {
            int         d_date;
            const char *d_groupSpec;
        } VALUES [] = {
            { 19000101, "" },
            { 19990101, "R1E1" },
            { 19990201, "R2E2" },
            { 20010909, "R1E1R2E2" },
            { 20010909, "R3E3R3E3" },
            { 20090909, "R2E4R1E1" },
            { 20010909, "R1E1R2E2R3E3" },
            { 20010909, "R4E1R3E2R2E3R1E4" },
            { 20010909, "R1E1R2E2R1E1R2E2R1E1R2E1R1E2" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            bdlt::Date       date(bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                            VALUES[i].d_date));
            bdlt::DatetimeTz timeStamp(bdlt::Datetime(date), 0);
            int size = gg(&groups, VALUES[i].d_groupSpec, RECORD_BUFFER);

            // Create the sample.
            Obj mX(Z); const Obj& MX = mX;
            mX.setTimeStamp(timeStamp);
            for (int j = 0; j < groups.size(); ++j) {
                mX.appendGroup(groups[j]);
            }

            if (veryVeryVerbose) {
                P_(i); P(MX);
            }

            // Test the sample.
            ASSERT(timeStamp      == MX.timeStamp());
            ASSERT(groups.size()  == MX.numGroups());
            ASSERT(size           == MX.numRecords());
            for (int j = 0; j < MX.numGroups(); ++j) {
                ASSERT(groups[j] == MX.sampleGroup(j));
            }
            ASSERT( 0 == defaultAllocator.numBytesInUse() );
        }
        ASSERT( 0 == defaultAllocator.numBytesInUse() );
      } break;
      case 10 : {
        // --------------------------------------------------------------------
        // TESTING HELPERS: 'gg' function
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST HELPERS: gg()\n"
            << "==================\n";

        const balm::MetricRecord *BEGIN = RECORD_BUFFER.data();
        const balm::MetricRecord *END   = BEGIN + RECORD_BUFFER.size();
        {
            if (veryVerbose) cout << "\tTesting empty spec\n";
            const char *specification = "";
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            gg(&groups, specification, RECORD_BUFFER);
            ASSERT(groups.empty());
        }
        {
            if (veryVerbose) cout << "\tTesting 1 element spec\n";
            const char *specification = "R4E5";
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            gg(&groups, specification, RECORD_BUFFER);

            ASSERT(1                       == groups.size());

            ASSERT(bsls::TimeInterval(5, 0) == groups[0].elapsedTime());
            ASSERT(4                       == groups[0].numRecords());
            ASSERT(BEGIN   <= groups[0].records() &&
                   END - 4 >= groups[0].records());
        }
        {
            if (veryVerbose) cout << "\tTesting 2 element spec\n";
            const char *specification = "R6E2R3E7";
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            gg(&groups, specification, RECORD_BUFFER);

            ASSERT(2                       == groups.size());

            ASSERT(bsls::TimeInterval(2, 0) == groups[0].elapsedTime());
            ASSERT(6                       == groups[0].numRecords());
            ASSERT(BEGIN   <= groups[0].records() &&
                   END - 6 >= groups[0].records());

            ASSERT(bsls::TimeInterval(7, 0) == groups[1].elapsedTime());
            ASSERT(3                       == groups[1].numRecords());
            ASSERT(BEGIN   <= groups[1].records() &&
                   END - 3 >= groups[1].records());
        }
        {
            if (veryVerbose) cout << "\tTesting 4 element spec\n";
            const char *specification = "R3E2R4E3R7E6R1E1";
            bsl::vector<balm::MetricSampleGroup> groups(Z);

            gg(&groups, specification, RECORD_BUFFER);

            ASSERT(4                       == groups.size());

            ASSERT(bsls::TimeInterval(2, 0) == groups[0].elapsedTime());
            ASSERT(3                       == groups[0].numRecords());
            ASSERT(BEGIN   <= groups[0].records() &&
                   END - 3 >= groups[0].records());

            ASSERT(bsls::TimeInterval(3, 0) == groups[1].elapsedTime());
            ASSERT(4                       == groups[1].numRecords());
            ASSERT(BEGIN   <= groups[1].records() &&
                   END - 4 >= groups[1].records());

            ASSERT(bsls::TimeInterval(6, 0) == groups[2].elapsedTime());
            ASSERT(7                       == groups[2].numRecords());
            ASSERT(BEGIN   <= groups[2].records() &&
                   END - 7 >= groups[2].records());

            ASSERT(bsls::TimeInterval(1, 0) == groups[3].elapsedTime());
            ASSERT(1                       == groups[3].numRecords());
            ASSERT(BEGIN   <= groups[3].records() &&
                   END - 1 >= groups[3].records());

        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'begin()', 'end()':  'balm::MetricSampleGroup'
        //
        // Plan:
        //   Test that accessors 'begin()' and 'end()' return iterators at the
        //   first element and one past the last element in the array of
        //   records.
        //
        // Testing:
        //   const_iterator begin() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting begin() and end(): "
                          << " 'balm::MetricSampleGroup'" << endl;

        struct {
            const RecVec *d_records;
            int           d_time;
        } VALUES[] = { { &RV1, 2 },
                       { &RV2, 3 },
                       { &RV3, 4 },
                       { &RV4, 5 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        Group mX; const Group& MX = mX;
        ASSERT(0 == MX.begin());
        ASSERT(0 == MX.end());
        for (int i = 0; i < NUM_VALUES; ++i) {
            Group mX(VALUES[i].d_records->data(),
                     VALUES[i].d_records->size(),
                     bsls::TimeInterval(VALUES[i].d_time, 0));
            const Group& MX = mX;

            ASSERT(VALUES[i].d_records->begin() == MX.begin());
            ASSERT(VALUES[i].d_records->end()   == MX.end());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT (<<) OPERATOR:  'balm::MetricSampleGroup'
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   operator<<(ostream&, const balm::MetricSampleGroup&);
        // --------------------------------------------------------------------

        if (veryVerbose) cout << "Testing 'print':  'balm::MetricSampleGroup'"
                              << endl;
        bsl::ostringstream buf1, buf2;

        Group mX(RV1.data(), RV1.size(), bsls::TimeInterval(1, 0));
        const Group& MX = mX;

        const char *EXP_1 =
            "[ (1, 0)  [   [ INVALID_ID: 1 2 3 4 ]   [ A.A: 3 2 1 0 ]  ] ] ";
        const char *EXP_2 =
            "   [ (1, 0)\n"
            "      [\n"
            "         [ INVALID_ID: 1 2 3 4 ]\n"
            "         [ A.A: 3 2 1 0 ]\n"
            "      ]\n"
            "   ]\n";

        buf1 << MX;
        MX.print(buf2, 1, 3);

        bsl::string val1 = buf1.str();
        bsl::string val2 = buf2.str();

        if (veryVerbose) {
            P(val1);
            P(EXP_1);
            P(val2);
            P(EXP_2);
        }

        ASSERT(bsl::string(EXP_1) == val1);
        ASSERT(bsl::string(EXP_2) == val2);
        if (veryVerbose) {
            stringDiff(EXP_1, val1);
            stringDiff(EXP_2, val2);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:  'balm::MetricSampleGroup'
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   balm::MetricSampleGroup& operator=(balm::MetricSampleGroup&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator: "
                          << " 'balm::MetricSampleGroup'" << endl;

        struct {
            const RecVec *d_records;
            int           d_time;
        } VALUES[] = { { &RV1, 1 },
                       { &RV2, 2 },
                       { &RV3, 3 },
                       { &RV4, 4 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                Group u(VALUES[i].d_records->data(),
                        VALUES[i].d_records->size(),
                        bsls::TimeInterval(VALUES[i].d_time, 0));
                const Group& U = u;
                Group v(VALUES[j].d_records->data(),
                        VALUES[j].d_records->size(),
                        bsls::TimeInterval(VALUES[j].d_time, 0));
                const Group& V = v;

                Group w(v); const Group& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Group u(VALUES[i].d_records->data(),
                    VALUES[i].d_records->size(),
                    bsls::TimeInterval(VALUES[i].d_time, 0));
            const Group& U = u;
            Group w(U);  const Group& W = w;              // control
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR: 'balm::MetricSampleGroup'
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricSampleGroup(const balm::MetricSampleGroup& );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor: "
                            << "'balm::MetricSampleGroup'" << endl;

        struct {
            const RecVec *d_records;
            int           d_time;
        } VALUES[] = { { &RV1, 1 },
                       { &RV2, 2 },
                       { &RV3, 3 },
                       { &RV4, 4 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            Group x(VALUES[i].d_records->data(),
                    VALUES[i].d_records->size(),
                    bsls::TimeInterval(VALUES[i].d_time, 0));
            const Group& X = x;

            Group w(VALUES[i].d_records->data(),
                    VALUES[i].d_records->size(),
                    bsls::TimeInterval(VALUES[i].d_time, 0));
            const Group& W = w;

            Group y(X); const Group& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR: 'balm::MetricSampleGroup'
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(const balm::MetricSampleGroup&,
        //                   const balm::MetricSampleGroup&);
        //   bool operator!=(const balm::MetricSampleGroup&,
        //                   const balm::MetricSampleGroup&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting equality: 'balm::MetricSampleGroup'"
                          << endl;

        struct {
            const RecVec *d_records;
            int           d_time;
        } VALUES[] = { { &RV1, 1 },
                       { &RV2, 2 },
                       { &RV3, 3 },
                       { &RV4, 4 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsls::TimeInterval interval(VALUES[i].d_time, 0);
            Group u(VALUES[i].d_records->data(),
                    VALUES[i].d_records->size(),
                    interval);
            const Group& U = u;

            for (int j = 0; j < NUM_VALUES; ++j) {
                bsls::TimeInterval interval(VALUES[j].d_time, 0);
                Group v(VALUES[j].d_records->data(),
                        VALUES[j].d_records->size(),
                        interval);
                const Group& V = v;

                bool isEqual = i == j;
                LOOP2_ASSERT(i, j, isEqual  == (U == V));
                LOOP2_ASSERT(i, j, !isEqual == (U != V))
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS: 'balm::MetricSampleGroup'
        //
        // Concerns:
        //   Test the alternative constructors
        //
        // Plan:
        //   For a sequence of independent test values, use the
        //   alternative constructor to create and object with a specific value
        //   and verify the values using a basic accessor.
        //
        // Testing:
        //  balm::MetricSampleGroup(const balm::MetricRecord *,
        //                         int,
        //                         const bsls::TimeInterval& );
        // --------------------------------------------------------------------

        if (veryVerbose) cout << "\tTesting secondary constructor: "
                              << "'balm::MetricSampleGroup'" << endl;

        struct {
            const RecVec *d_records;
            int           d_time;
        } VALUES[] = { { &RV1, 1 },
                       { &RV2, 2 },
                       { &RV3, 3 },
                       { &RV4, 4 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsls::TimeInterval interval(VALUES[i].d_time, 0);
            Group mX(VALUES[i].d_records->data(),
                     VALUES[i].d_records->size(),
                     interval);
            const Group& MX = mX;

            ASSERT(interval                    == MX.elapsedTime());
            ASSERT(VALUES[i].d_records->data() == MX.records());
            ASSERT(VALUES[i].d_records->size() == MX.numRecords());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());

      } break;
     case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //                                        'balm::MetricSampleGroup'
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        //   Note that the balm::MetricSampleIterator is the only available
        //   method to access the records in a sample, so it is incorporated
        //   into this test.
        //
        // Testing:
        //  balm::MetricSampleGroup
        //  ----------------------
        //  balm::MetricSampleGroup();
        //  ~balm::MetricSampleGroup();
        //
        //  void setElapsedTime(const bsls::TimeInterval& );
        //  void setRecords(const balm::MetricRecord *, int );
        //
        //  const balm::MetricRecord *records() const;
        //  int numRecords() const;
        //  const bsls::TimeInterval& elapsedTime() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting primary mainpulators and accessors: "
                          << "'balm::MetricSampleGroup'" << endl;

        if (veryVerbose) cout << "\tdefault constructor\n";
        {
            Group mX; const Group& MX = mX;
            ASSERT(bsls::TimeInterval() == MX.elapsedTime());
            ASSERT(0                   == MX.numRecords());
            ASSERT(0                   == MX.records());

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (veryVerbose) cout << "\ttesting primary modifiers and accesors\n";

        struct {
            const RecVec *d_records;
            int           d_time;
        } VALUES[] = { { &RV1, 1 },
                       { &RV2, 2 },
                       { &RV3, 3 },
                       { &RV4, 4 }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            bsls::TimeInterval interval(VALUES[i].d_time, 0);

            Group mX; const Group& MX = mX;
            mX.setElapsedTime(interval);
            mX.setRecords(VALUES[i].d_records->data(),
                          VALUES[i].d_records->size());

            ASSERT(interval                    == MX.elapsedTime());
            ASSERT(VALUES[i].d_records->data() == MX.records());
            ASSERT(VALUES[i].d_records->size() == MX.numRecords());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST: 'balm::MetricSample'
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1.                       x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1.           x1:A x2:
        //    4. Append the same element value A to x2.     x1:A x2:A
        //    5. Append another element value B to x2.      x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3.                  x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST: balm::MetricSample" << endl
                          << "=================================" << endl;

        bdlt::DatetimeTz timeStamp1(bdlt::CurrentTime::utc(), 0);
        bdlt::DatetimeTz timeStamp2(bdlt::CurrentTime::utc(), 1);
        bdlt::DatetimeTz timeStamp3(bdlt::CurrentTime::utc(), 2);

        bsls::TimeInterval interval1(1,0);
        bsls::TimeInterval interval2(2,0);
        bsls::TimeInterval interval3(3,0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(Z); const Obj& X1 = mX1;

        mX1.setTimeStamp(timeStamp1);
        mX1.appendGroup(RV1.data(), RV1.size(), interval1);
        mX1.appendGroup(RV2.data(), RV2.size(), interval2);

        if (veryVerbose) { cout << '\t';  P(X1); }
        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;

        ASSERT(timeStamp1   == X1.timeStamp());
        ASSERT(3            == X1.numRecords());
        ASSERT(2            == X1.numGroups());
        ASSERT(2            == X1.sampleGroup(0).numRecords());
        ASSERT(RV1.data()   == X1.sampleGroup(0).records());
        ASSERT(interval1    == X1.sampleGroup(0).elapsedTime());
        ASSERT(1            == X1.sampleGroup(1).numRecords());
        ASSERT(RV2.data()   == X1.sampleGroup(1).records());
        ASSERT(interval2    == X1.sampleGroup(1).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1, Z);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(timeStamp1   == X2.timeStamp());
        ASSERT(3            == X2.numRecords());
        ASSERT(2            == X2.numGroups());
        ASSERT(2            == X2.sampleGroup(0).numRecords());
        ASSERT(RV1.data()   == X2.sampleGroup(0).records());
        ASSERT(interval1    == X2.sampleGroup(0).elapsedTime());
        ASSERT(1            == X2.sampleGroup(1).numRecords());
        ASSERT(RV2.data()   == X2.sampleGroup(1).records());
        ASSERT(interval2    == X2.sampleGroup(1).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;

        mX1.setTimeStamp(timeStamp2);
        mX1.removeAllRecords();
        mX1.appendGroup(RV3.data(), RV3.size(), interval3);

        if (veryVerbose) { cout << '\t';  P(X1); }
        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(timeStamp2   == X1.timeStamp());
        ASSERT(1            == X1.numRecords());
        ASSERT(1            == X1.numGroups());
        ASSERT(1            == X1.sampleGroup(0).numRecords());
        ASSERT(RV3.data()   == X1.sampleGroup(0).records());
        ASSERT(interval3    == X1.sampleGroup(0).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3(Z); const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t';  P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;

        ASSERT(bdlt::DatetimeTz() == X3.timeStamp());
        ASSERT(0                 == X3.numRecords());
        ASSERT(0                 == X3.numGroups());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3, Z);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t';  P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(bdlt::DatetimeTz() == X4.timeStamp());
        ASSERT(0                 == X4.numRecords());
        ASSERT(0                 == X4.numGroups());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;

        mX3.removeAllRecords();
        mX3.setTimeStamp(timeStamp3);
        mX3.appendGroup(RV3.data(), RV3.size(), interval3);
        mX3.appendGroup(RV2.data(), RV2.size(), interval2);
        mX3.appendGroup(RV1.data(), RV1.size(), interval1);

        if (veryVerbose) { cout << '\t';  P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;

        ASSERT(timeStamp3   == X3.timeStamp());
        ASSERT(4            == X3.numRecords());
        ASSERT(3            == X3.numGroups());
        ASSERT(1            == X3.sampleGroup(0).numRecords());
        ASSERT(RV3.data()   == X3.sampleGroup(0).records());
        ASSERT(interval3    == X3.sampleGroup(0).elapsedTime());
        ASSERT(1            == X3.sampleGroup(1).numRecords());
        ASSERT(RV2.data()   == X3.sampleGroup(1).records());
        ASSERT(interval2    == X3.sampleGroup(1).elapsedTime());
        ASSERT(2            == X3.sampleGroup(2).numRecords());
        ASSERT(RV1.data()   == X3.sampleGroup(2).records());
        ASSERT(interval1    == X3.sampleGroup(2).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(timeStamp2   == X2.timeStamp());
        ASSERT(1            == X2.numRecords());
        ASSERT(1            == X2.numGroups());
        ASSERT(1            == X2.sampleGroup(0).numRecords());
        ASSERT(RV3.data()   == X2.sampleGroup(0).records());
        ASSERT(interval3    == X2.sampleGroup(0).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(timeStamp3   == X2.timeStamp());
        ASSERT(4            == X2.numRecords());
        ASSERT(3            == X2.numGroups());
        ASSERT(1            == X2.sampleGroup(0).numRecords());
        ASSERT(RV3.data()   == X2.sampleGroup(0).records());
        ASSERT(interval3    == X2.sampleGroup(0).elapsedTime());
        ASSERT(1            == X2.sampleGroup(1).numRecords());
        ASSERT(RV2.data()   == X2.sampleGroup(1).records());
        ASSERT(interval2    == X2.sampleGroup(1).elapsedTime());
        ASSERT(2            == X2.sampleGroup(2).numRecords());
        ASSERT(RV1.data()   == X2.sampleGroup(2).records());
        ASSERT(interval1    == X2.sampleGroup(2).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (veryVerbose) { cout << '\t';  P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(timeStamp2   == X1.timeStamp());
        ASSERT(1            == X1.numRecords());
        ASSERT(1            == X1.numGroups());
        ASSERT(1            == X1.sampleGroup(0).numRecords());
        ASSERT(RV3.data()   == X1.sampleGroup(0).records());
        ASSERT(interval3    == X1.sampleGroup(0).elapsedTime());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST: 'balm::MetricSampleGroup'
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1 (dctor + =).           x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1).          x1:A x2:
        //    4. Append the same element value A to x2).    x1:A x2:A
        //    5. Append another element value B to x2).     x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3 (dctor + =)       x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST: balm::MetricSampleGroup" << endl
                          << "======================================" << endl;

        bsls::TimeInterval interval1(1,0);
        bsls::TimeInterval interval2(2,0);
        bsls::TimeInterval interval3(3,0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Group mX1; const Group& X1 = mX1;

        mX1.setElapsedTime(interval1);
        mX1.setRecords(RV1.data(), RV1.size());

        if (veryVerbose) { cout << '\t';  P(X1); }
        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;

        ASSERT(interval1    == X1.elapsedTime());
        ASSERT(2            == X1.numRecords());
        ASSERT(RV1.data()   == X1.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Group mX2(X1);  const Group& X2 = mX2;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check the initial state of x2." << endl;

        ASSERT(interval1    == X2.elapsedTime());
        ASSERT(2            == X2.numRecords());
        ASSERT(RV1.data()   == X2.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;

        mX1.setElapsedTime(interval2);
        mX1.setRecords(RV2.data(), RV2.size());

        if (veryVerbose) { cout << '\t';  P(X1); }
        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(interval2  == X1.elapsedTime());
        ASSERT(1          == X1.numRecords());
        ASSERT(RV2.data() == X1.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Group mX3; const Group& X3 = mX3;
        if (veryVerbose) { cout << '\t';  P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;

        ASSERT(bsls::TimeInterval() == X3.elapsedTime());
        ASSERT(0                   == X3.numRecords());
        ASSERT(0                   == X3.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Group mX4(X3);  const Group& X4 = mX4;
        if (veryVerbose) { cout << '\t';  P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(bsls::TimeInterval() == X4.elapsedTime());
        ASSERT(0                   == X4.numRecords());
        ASSERT(0                   == X4.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;

        mX3.setElapsedTime(interval3);
        mX3.setRecords(RV3.data(), RV3.size());

        if (veryVerbose) { cout << '\t';  P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;

        ASSERT(interval3  == X3.elapsedTime());
        ASSERT(1          == X3.numRecords());
        ASSERT(RV3.data() == X3.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(interval2  == X2.elapsedTime());
        ASSERT(1          == X2.numRecords());
        ASSERT(RV2.data() == X2.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(interval3  == X2.elapsedTime());
        ASSERT(1          == X2.numRecords());
        ASSERT(RV3.data() == X2.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (veryVerbose) { cout << '\t';  P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(interval2  == X1.elapsedTime());
        ASSERT(1          == X1.numRecords());
        ASSERT(RV2.data() == X1.records());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

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
