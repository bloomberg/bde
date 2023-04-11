// bdlb_guidutil.t.cpp                                                -*-C++-*-
#include <bdlb_guidutil.h>

#include <bdlb_guid.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bslmt_latch.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_byteorder.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>

#include <bsl_cmath.h>
#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bslstl_pair.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>             // 'fork', 'pipe', 'close' and 'dup'.
#include <sys/wait.h>           // 'wait'
#else
#include <windows.h>
#include <fileapi.h>
#include <processthreadsapi.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component is a utility wrapping platform dependent GUID generation
// functionality.  Since it is mostly an adapter the concerns enforced on the
// properties of the GUID itself are not exhaustive, since those are the
// responsibility of the generators themselves.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] void generate(Guid *out, size_t numGuids)
// [ 1] void generate(unsigned char *out, size_t numGuids)
// [ 1] Guid generate()
// [ 2] int getVersion(const Guid& guid)
// [ 3] int guidFromString(Guid *result, bsl::string_view guidString)
// [ 3] Guid guidFromString(bsl::string_view guidString)
// [ 4] void guidToString(bsl::string *result, const Guid& guid)
// [ 4] void guidToString(std::string *result, const Guid& guid)
// [ 4] void guidToString(std::pmr::string *result, const Guid& guid)
// [ 4] bsl::string guidToString(const Guid& guid)
// [ 5] Uint64 getMostSignificantBits(const Guid& guid)
// [ 6] Uint64 getLeastSignificantBits(const Guid& guid)
// [ 7] void generateNonSecure(Guid *result, size_t numGuids)
// [ 7] void generateNonSecure(unsigned char *result, size_t numGuids)
// [ 7] Guid generateNonSecure()
// [ 8] Guid generateFromName(const Guid& nsId, const string_view& name)
// [ 9] Guid dnsNamespace()
// [ 9] Guid urlNamespace()
// [ 9] Guid oidNamespace()
// [ 9] Guid x500Namespace()
// ----------------------------------------------------------------------------
// [10] MULTI-THREADING TEST CASE
// [11] TESTING 'generateNonSecure' FROM FORK PER DRQS 168925481
// [12] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                          DEBUG PRINT SUPPORT
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bdlb {

void debugprint(const Guid& guid)
    // Print the specified 'guid' to the standard output stream.
{
    guid.print(bsl::cout);
}

void debugprint(const bsl::string& string)
    // Print the specified 'string' to the standard output stream.
{
    bsl::cout << string;
}

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::GuidUtil      Util;
typedef bdlb::Guid          Obj;
typedef const unsigned char Element[16];
struct UuidComponents {
    struct { unsigned char low[4], mid[2], high_vers[2]; } time;
    unsigned char                                          clock_seq[2];
    unsigned char                                          node_id[6];
};

const Element VALUES[] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },

    { 0x5c, 0x9d, 0x4e, 0x50, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x51, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x52, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x53, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x54, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x56, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
      0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 }
};

const Element &V0 = VALUES[0],            // V0, V1, ... are used in
              &V1 = VALUES[1],
              &V2 = VALUES[2],
              &V3 = VALUES[3],
              &V4 = VALUES[4],
              &V5 = VALUES[5],
              &V6 = VALUES[6],
              &V7 = VALUES[7],
              &V8 = VALUES[8];

namespace {

struct ThreadData {
    bool d_veryVerbose;
};

extern "C" void *threadFunction(void *data)
{
    ThreadData& threadData = *(ThreadData *) data;
    bool veryVerbose       = threadData.d_veryVerbose;

    ASSERT(Util::generateNonSecure() != Util::generateNonSecure());

    const int NUM_GUIDS = 200;
    Obj guids1[NUM_GUIDS] = {};
    Obj guids2[NUM_GUIDS] = {};
    Util::generateNonSecure(guids1, NUM_GUIDS);
    Util::generateNonSecure(guids2, NUM_GUIDS);
    for (int i = 0; i < NUM_GUIDS; ++i) {
        if (veryVerbose) { P_(i) P(guids1[i]) P(guids2[i]); }
        ASSERT(guids1[i] != guids2[i]);
    }

    return (void *) 0;
}

struct PerfThreadData {
    // A struct containg the parameters for individual guid generation threads,
    // used in benchmark testing (case -1).

    // DATA
    bsl::size_t   d_batchSize;     // size of each batch of generated guids

    bsl::size_t   d_iterations;    // number of iterations to perform

    bslmt::Latch *d_startLatch_p;  // latch waited upon before GUID generation

    bslmt::Latch *d_endLatch_p;    // latch arrived at after GUID generation
};

extern "C" void *threadMultiPerfFunction(void *data)
    // The function executed by an individual thread for benchmark testing
    // (case -1).  The specified 'data' is a pointer to a 'PerfThreadData'
    // object.  Performs 'd_iterations' loops, each generating and discarding
    // 'd_batchSize' guids.  The 'd_startLatch_p' and 'd_endLatch_p' are used
    // to coordinate between threads and ensure the loops are executed
    // cotemporaneously.
{
    PerfThreadData& threadData = *(PerfThreadData *)data;

    ASSERT(Util::generateNonSecure() != Util::generateNonSecure());

    ASSERT(threadData.d_batchSize < 65536);

    const bsl::size_t NUM_GUIDS = threadData.d_batchSize;
    const uint64_t    LOOPS     = threadData.d_iterations / (NUM_GUIDS * 2);

    bsl::vector<Obj> guids1(65536);
    bsl::vector<Obj> guids2(65536);

    threadData.d_startLatch_p->arriveAndWait();

    for (uint64_t j = 0; j < LOOPS; j++) {
        Util::generateNonSecure(guids1.data(), NUM_GUIDS);
        Util::generateNonSecure(guids2.data(), NUM_GUIDS);
    }

    threadData.d_endLatch_p->arriveAndWait();

    return (void *)0;
}

void simulateRfc4122(
                  bsl::uint32_t (*val)[bdlb::GuidState_Imp::k_GENERATOR_COUNT])
    // Update the specified 'val' to fix the values of specific bits in such a
    // way that the randomness cannot exceed that of an RFC4122-compliant guid.
{
    ASSERT(val);
    ASSERT(*val);

    unsigned char *bytes = reinterpret_cast<unsigned char *>(*val);
    bytes[6] = static_cast<unsigned char>(0x40 | (bytes[6] & 0x0F));
    bytes[8] = static_cast<unsigned char>(0x80 | (bytes[8] & 0x3F));
}

#ifndef BSLS_PLATFORM_OS_WINDOWS

bsl::string tempFileName(bool verboseFlag)
    // Return the potential name for a temporary file.  The returned C-string
    // refers to a static memory buffer (so this method is not thread safe).
    // Additional output is generated based on the specified 'verboseFlag'.
{
    enum { k_MAX_LENGTH = 4096 };
    static char result[k_MAX_LENGTH];

    char *temp = tempnam(0, "bdlb_guidutil");
    strncpy(result, temp, k_MAX_LENGTH);
    result[k_MAX_LENGTH - 1] = '\0';
    free(temp);

    if (verboseFlag) printf("\tUse %s as a base filename.\n", result);

    return result;
}

static inline
int removeFile(const char *path)
    // Delete the file with the specified 'path'.  Return 0 on success and
    // non-zero otherwise.
{
    BSLS_ASSERT(path);

    return unlink(path);
}

#endif // BSLS_PLATFORM_OS_WINDOWS

template <bsl::size_t BITS>
struct BasicBloomFilter
    // A simple bloom filter, limited to taking inputs of k_GENERATOR_COUNT-int
    // arrays, which treats the values in that array as flatly distributed
    // random variables, thus does not perform any additional hashing.  The
    // number of bits set per insert is hard coded to k_GENERATOR_COUNT.
{
    BSLMF_ASSERT(0 == (BITS % 64ull));

    static const bsl::uint64_t k_BUCKETS = BITS / 64ull;
    static const bsl::uint64_t k_BUCKET_BITMASK = k_BUCKETS - 1ull;

    bsl::vector<uint64_t> d_filter;   // The filter of BITS bits.

    BasicBloomFilter() : d_filter(k_BUCKETS)
        // Construct a BasicBloomFilter object.
    {
    }

    inline
    bool check(
     const bsl::uint32_t (&randomInts)[bdlb::GuidState_Imp::k_GENERATOR_COUNT])
        // Check if the specified 'randomInts' array could have been added
        // previously.  Return false if 'randomInts' is not in the filter, and
        // true if it may have been in the filter.  If 'randomInts' has been
        // added previously then true is returned, otherwise false will
        // probably be returned, but true may be returned.
    {
        uint64_t rnd = randomInts[bdlb::GuidState_Imp::k_GENERATOR_COUNT - 1];

        for (int i = 0; i < bdlb::GuidState_Imp::k_GENERATOR_COUNT; i++)
        {
            rnd = ((rnd << 32) | randomInts[i]);

            uint64_t&      val = d_filter[(rnd / 64) & k_BUCKET_BITMASK];
            const uint64_t checkmask = 1ull << (rnd & 63);

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(!(val & checkmask))) {
                return false;                                         // RETURN
            }
        }
        return true;
    }

    inline
    bool
    checkAndAdd(
     const bsl::uint32_t (&randomInts)[bdlb::GuidState_Imp::k_GENERATOR_COUNT])
        // Check if the specified 'randomInts' array could have been added
        // previously.  Add 'randomInts' to the filter.  Return false if
        // 'randomInts' is not in the filter, and true if it may have been in
        // the filter.  If 'randomInts' has been added previously then true is
        // returned, otherwise false will probably be returned, but true may be
        // returned.
    {
        ASSERT(randomInts);

        bool     exists = true;
        uint64_t rnd = randomInts[bdlb::GuidState_Imp::k_GENERATOR_COUNT - 1];

        for (int i = 0; i < bdlb::GuidState_Imp::k_GENERATOR_COUNT; i++)
        {
            rnd = ((rnd << 32) | randomInts[i]);

            uint64_t&      val = d_filter[(rnd / 64) & k_BUCKET_BITMASK];
            const uint64_t checkmask = 1ull << (rnd & 63);

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(!(val & checkmask))) {
                val |= checkmask;
                exists = false;
            }
        }
        return exists;
    }
};

string allCharacters()
    // Return a string that contains every value of 'char' exactly once, sorted
    // in numerical order.
{
    string result;
    for (int c = -128; c != 128; ++c) {
        result.push_back(static_cast<char>(c));
    }
    return result;
}

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
// Suppose we are building a system for managing records for employees in a
// large international firm.  These records have no natural field which can be
// used as a unique ID, so a GUID must be created for each employee.
//
// First let us define a value-type for employees.
//..
    class MyEmployee {
        // This class provides a value-semantic type to represent an employee
        // record.  These records are for internal use only.
//..
// For the sake of brevity, we provide a limited amount of data in each record.
// We additionally show a very limited scope of functionality.
//..
        // DATA
        bsl::string d_name;    // name of the employee
        double      d_salary;  // salary in some common currency
        bdlb::Guid  d_guid;    // a GUID for the employee

      public:
        // CREATORS
        MyEmployee(const bsl::string& name, double salary);
            // Create an object with the specified 'name' and specified
            //'salary', generating a new GUID to represent the employee.

        // ...

        // ACCESSORS
        const bdlb::Guid& Guid() const;
            // Return the 'guid' of this object.

        const bsl::string& name() const;
            // Return the 'name' of this object.

        double salary() const;
            // Return the 'salary' of this object.
        // ...
 };
//..
// Next, we create free functions 'operator<' and 'operator==' to allow
// comparison of 'MyEmployee' objects.  We take advantage of the monotonically
// increasing nature of sequential GUIDs to implement these methods.
//..
bool operator== (const MyEmployee& lhs, const MyEmployee& rhs);
    // Return 'true' if the specified 'lhs' object has the same value as the
    // specified 'rhs' object, and 'false' otherwise.  Note that two
    // 'MyEmployee' objects have the same value if they have the same guid.

bool operator< (const MyEmployee& lhs, const MyEmployee& rhs);
    // Return 'true' if the value of the specified 'lhs' MyEmployee object is
    // less than the value of the specified 'rhs' MyEmployee object, and
    // 'false' otherwise.  A MyEmployee object is less than another if the
    // guid is less than the other.  Note that this is equivalent to saying
    // that one employee object was created before another.

// ...

// CREATORS
MyEmployee::MyEmployee(const string& name, double salary)
: d_name(name)
, d_salary(salary)
{
     bdlb::GuidUtil::generate(&d_guid);
}

// ACCESSORS
const bdlb::Guid& MyEmployee::Guid() const
{
    return d_guid;
}

const bsl::string& MyEmployee::name() const
{
    return d_name;
}

double MyEmployee::salary() const
{
    return d_salary;
}

// FREE FUNCTIONS
bool operator==(const MyEmployee& lhs, const MyEmployee& rhs)
{
    return lhs.Guid() == rhs.Guid();
}

bool operator<(const MyEmployee& lhs, const MyEmployee& rhs)
{
     return lhs.Guid() < rhs.Guid();
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? bsl::atoi(argv[1]) : -1;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;
    switch (test)  { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
//..
//  Next, we create some employees:
//..
        MyEmployee e1("Foo Bar"     , 1011970);
        MyEmployee e2("John Doe"    , 12345);
        MyEmployee e3("Joe Six-pack", 1);
//..
//  Finally, we verify that the generated GUIDs are unique.
///..
        ASSERT(e1 < e2 || e2 < e1);
        ASSERT(e2 < e3 || e3 < e2);
        ASSERT(e1 < e3 || e3 < e1);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'generateNonSecure' FROM FORK PER DRQS 168925481
        //
        // Concerns:
        //: 1 That, when a fork happens, the random number generator in the
        //:   child will be reseeded, thus shall not generate overlapping
        //:   guids.
        //:
        //: 2 Note: On Windows, this is neither an issue nor is testable, as
        //:   Windows does not use the 'fork' mechanism for process creation.
        //
        // Plan:
        //: 1 Generate a set of guids.
        //: 2 Call 'fork'.
        //: 3 In the parent process generate a post-fork set of guids
        //: 4 In the child process generate a post-fork set of guids.
        //: 5 In the child process write the generated guids to a temp file.
        //: 6 In the child process call 'exit'.
        //: 7 In the parent process wait for the child process to terminate.
        //: 8 In the parent process read the child's guids from the temp file.
        //: 9 In the parent process verify that all of the guids generated in
        //:   steps 1, 3 and 4 are distinct.
        //
        // Testing:
        //   TESTING 'generateNonSecure' FROM FORK PER DRQS 168925481
        // --------------------------------------------------------------------
        if (verbose) cout
               << endl
               << "TESTING 'generateNonSecure' FROM FORK PER DRQS 168925481"
               << endl
               << "========================================================"
               << endl;

#ifndef BSLS_PLATFORM_OS_WINDOWS
        enum {
            k_GUID_STR_SIZE = 36,
            k_GUID_COUNT    = 16
        };

        bsl::string fileName = tempFileName(veryVerbose);

        if (veryVeryVerbose) {
            P(fileName);
        }

        bsl::vector<Obj> preForkGuids(k_GUID_COUNT);
        bsl::vector<Obj> postForkParentGuids(k_GUID_COUNT);

        // Create a set of pre-fork guids
        bdlb::GuidUtil::generateNonSecure(preForkGuids.data(),
                                          preForkGuids.size());

        if (pid_t childPid = fork()) {
            // Parent process.
            ASSERT(childPid > 0);
            // Parent creates post-fork guids
            bdlb::GuidUtil::generateNonSecure(postForkParentGuids.data(),
                                              postForkParentGuids.size());
        }
        else {
            // Child process.
            bsl::ofstream output(fileName.c_str());
            int errorNum = errno;
            ASSERTV(fileName, errorNum, strerror(errorNum), output);
            if (veryVerbose) {
                bsl::cout << "will write guids to file: " << fileName
                          << bsl::endl;
            }

            // Child creates post-fork guids and writes to file
            bsl::vector<Obj> tempChildBuffer(k_GUID_COUNT);
            bdlb::GuidUtil::generateNonSecure(tempChildBuffer.data(),
                                              tempChildBuffer.size());
            for (bsl::vector<Obj>::const_iterator it =
                                                      tempChildBuffer.cbegin();
                 it != tempChildBuffer.cend();
                 ++it)
            {
                bsl::string guidStr;
                bdlb::GuidUtil::guidToString(&guidStr, *it);
                if (veryVerbose) {
                    bsl::cout << "generated child guid: " << guidStr
                              << bsl::endl;
                }
                if (output) {
                    output << guidStr << bsl::endl;
                }
            }

            ASSERTV(fileName, !output.bad());

            // Return from 'main' and thus exit the child process.
            return 0;                                                 // RETURN
        }

        while(wait(NULL) > 0);

        bsl::vector<Obj> finalList;

        {
            ifstream input(fileName.c_str());
            int      errorNum = errno;
            ASSERTV(fileName, errorNum, strerror(errorNum), input);
            ASSERTV(fileName, input.is_open());

            for (bsl::string guidStr; bsl::getline(input, guidStr);) {
                if (veryVerbose) {
                    bsl::cout << "guid in file: " << guidStr << bsl::endl;
                }
                Obj guid = bdlb::GuidUtil::guidFromString(guidStr);
                finalList.push_back(guid);
                if (veryVerbose) {
                    bsl::cout << "child guid parsed:  " << guid << bsl::endl;
                }
            }

            ASSERTV(fileName, !input.bad());
        }

        ASSERTV(fileName, 0 == removeFile(fileName.c_str()));

        ASSERTV(finalList.size(), k_GUID_COUNT == finalList.size());

        ASSERTV(preForkGuids.size(), k_GUID_COUNT == preForkGuids.size());

        for (bsl::vector<Obj>::const_iterator it = preForkGuids.cbegin();
             it != preForkGuids.cend();
             ++it) {
            finalList.push_back(*it);
            if (veryVerbose) {
                bsl::cout << "checking pre fork guid: " << *it << bsl::endl;
            }
        }

        ASSERTV(finalList.size(), k_GUID_COUNT * 2 == finalList.size());

        ASSERTV(postForkParentGuids.size(),
                k_GUID_COUNT == postForkParentGuids.size());

        for (bsl::vector<Obj>::const_iterator it =
                                                  postForkParentGuids.cbegin();
             it != postForkParentGuids.cend();
             ++it) {

            finalList.push_back(*it);
            if (veryVerbose) {
                bsl::cout << "checking post fork parent guid: " << *it
                          << bsl::endl;
            }
        }

        ASSERTV(finalList.size(), k_GUID_COUNT * 3 == finalList.size());

        bsl::unordered_set<Obj> check(finalList.begin(), finalList.end());

        ASSERTV(check.size(), k_GUID_COUNT * 3 == check.size());
#else
        if (verbose)
            cout << "Skipping fork test as not applicable to Windows" << endl;
#endif // BSLS_PLATFORM_OS_WINDOWS

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'generateNonSecure' FROM MULTIPLE THREADS
        //
        // Concerns:
        //: 1 Calls to 'generateNonSecure' in separate threads behave
        //:   correctly.
        //
        // Plan:
        //: 1 Create multiple threads.
        //:
        //: 2 Exercise all 'generateNonSecure' methods in multiple threads to
        //:   test that there is no deadlock and sane results are produced.
        //
        // Testing:
        //   MULTI-THREADING TEST CASE
        // --------------------------------------------------------------------
        if (verbose) cout
               << endl
               << "TESTING 'generateNonSecure' FROM MULTIPLE THREADS"
               << endl
               << "================================================="
               << endl;

        ThreadData threadData;
        threadData.d_veryVerbose = veryVerbose;

        const int                 NUM_THREADS = 20;
        bslmt::ThreadUtil::Handle handles[NUM_THREADS];

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(
                            &handles[i], &threadFunction, &threadData));
        }

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PREDEFINED NAMESPACE IDS
        //
        // Concerns:
        //: 1 The functions 'dnsNamespace', 'urlNamespace', 'oidNamespace', and
        //:   'x500Namespace' defined in this component return the correct
        //:   values defined in RFC 4122, Appendix C.
        //
        // Plan:
        //: 1 Convert the string form of each of the predefined namespace IDs
        //:   defined in Appendix C to a 'bdlb::Guid' value, and verify that it
        //:   is the same as the value returned by the corresponding method.
        //:   (C-1)
        //
        // Testing:
        //   Guid dnsNamespace()
        //   Guid urlNamespace()
        //   Guid oidNamespace()
        //   Guid x500Namespace()
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING PREDEFINED NAMESPACE IDS" << endl
                          << "================================" << endl;

        const string k_DNS_NS_STR  = "6ba7b8109dad11d180b400c04fd430c8";
        const string k_URL_NS_STR  = "6ba7b8119dad11d180b400c04fd430c8";
        const string k_OID_NS_STR  = "6ba7b8129dad11d180b400c04fd430c8";
        const string k_X500_NS_STR = "6ba7b8149dad11d180b400c04fd430c8";
        ASSERT(Util::guidFromString(k_DNS_NS_STR) == Util::dnsNamespace());
        ASSERT(Util::guidFromString(k_URL_NS_STR) == Util::urlNamespace());
        ASSERT(Util::guidFromString(k_OID_NS_STR) == Util::oidNamespace());
        ASSERT(Util::guidFromString(k_X500_NS_STR) == Util::x500Namespace());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromName'
        //
        // Concerns:
        //: 1 The class method 'generateFromName' generates the unique correct
        //:   UUID when given a namespace ID and name string as input according
        //:   to the specification in RFC 4122, version 5.
        //
        // Plan:
        //: 1 Specify a set of namespace IDs generated from (hard-coded)
        //:   randomly generated UUID strings.  Also specify a set of name
        //:   strings.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Specify a set of rows representing all possible combinations of
        //:     a namespace ID and a name string selected from the sets
        //:     specified in P-1.
        //:
        //:   2 Additionally provide a column, 'EXPECTED', that specifies the
        //:     string form of the UUID that is expected to be generated from
        //:     the namespace ID and name string.  Note that the expected
        //:     values were generated using a canonical implementation provided
        //:     by the 'libuuid' library.
        //:
        //: 3 For each row (representing a distinct combination of a namespace
        //:   ID 'ID' and name string 'N') in the table described in P-2:
        //:   (C-1)
        //:
        //:   1 Generate a 'Guid' using 'Util::generateFromName' supplied with
        //:     the namespace ID 'ID' and name 'N'.
        //:
        //:   2 Convert the expected string representation of the expected
        //:     value into a 'Guid'.
        //:
        //:   3 Verify that the generated value equals the expected value.
        //:
        //:   4 Specifically verify that the generated value has the correct
        //:     4-bit version field, namely '0101', representing a version 5
        //:     UUID.
        //:
        //:   5 Specifically verify that bits 6 and 7 of the
        //:     'clock_seq_hi_and_reserved' field are 0 and 1, respectively, as
        //:     specified in RFC 4122.
        //
        // Note: One of the important properties of UUIDv5 is that two UUIDs
        // generated from the same name, but two different name space IDs,
        // should be different with high probability.  We won't explicitly test
        // this, but it can be observed from the table (P-2) that this property
        // holds for the combinations of namespace ID and name contained
        // therein.
        //
        // Testing:
        //   Guid generateFromName(const Guid& nsId, const string_view& name)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'generateFromName'" << endl
                          << "==========================" << endl;
        const char *nsIdStrings[] = {
            "88515314-9c0d-44ac-bf6d-9a4cb5b2ec89",
            "80d90399-1dbe-498c-ae89-4855ebfa548f",
            "adde14a7-e049-4dfd-90da-d680b724a49c"
        };

        const int k_NUM_NAMESPACE_IDS = sizeof nsIdStrings /
                                        sizeof *nsIdStrings;

        Obj nsIds[k_NUM_NAMESPACE_IDS];
        for (int i = 0; i < k_NUM_NAMESPACE_IDS; ++i) {
            ASSERT(0 == Util::guidFromString(nsIds + i, nsIdStrings[i]));
        }

        const string names[3] = {
            "",
            allCharacters(),
            "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
        };

        const struct {
            int    d_lineNum;
            Obj    d_namespaceId;
            string d_name;
            string d_expected;
        } DATA[] = {
            //LINE NS ID     NAME      EXPECTED
            //---- --------  --------  --------------------------------------
            { L_,  nsIds[0], names[0], "c2faa822-7fb4-5a20-bfc4-b2e9bb2008ae"},
            { L_,  nsIds[0], names[1], "59bb581d-a13f-5364-ad3b-bb4d19d96bb6"},
            { L_,  nsIds[0], names[2], "f1c5e75d-7f35-52bf-9674-bc17338d1348"},
            { L_,  nsIds[1], names[0], "227dd609-6bf8-52dc-b42f-85ae08e202d4"},
            { L_,  nsIds[1], names[1], "0895d110-355e-5a67-9c0e-0808ef78c75a"},
            { L_,  nsIds[1], names[2], "4fa19270-8c13-532c-bbba-323c4084b86d"},
            { L_,  nsIds[2], names[0], "4a9789c2-0bac-5459-9f4a-d0d46de51355"},
            { L_,  nsIds[2], names[1], "cc0c75e9-8d09-5ba4-a0d5-e7433cc45a69"},
            { L_,  nsIds[2], names[2], "a7af84f1-c949-533f-bb5c-dac800b23d0f"},
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE         = DATA[i].d_lineNum;
            const Obj    NAMESPACE_ID = DATA[i].d_namespaceId;
            const string NAME         = DATA[i].d_name;
            const string EXPECTED     = DATA[i].d_expected;
            if (veryVerbose) {
                T_ P_(LINE) P_(NAME) P_(NAMESPACE_ID) P(EXPECTED)
            }
            const Obj result = Util::generateFromName(NAMESPACE_ID, NAME);
            const Obj expected = Util::guidFromString(EXPECTED);
            ASSERTV(LINE,
                    Util::guidToString(result),
                    Util::guidToString(expected),
                    result == expected);
            ASSERT(5 == result.version());
            ASSERT(2 == (result.clockSeqHiRes() >> 6));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'generateNonSecure'
        //
        // Concerns:
        //: 1 A single GUID can be passed and loaded.
        //: 2 If 'numGuids' is passed, 'numGuids' GUIDs are returned.
        //: 3 The correct type of GUID is returned.
        //: 4 Memory outside the designated range is left unchanged.
        //
        // Plan:
        //: 1 Call the 'generateNonSecure' method with 'numGuids' set to 1, and
        //:   call the single-value 'generateNonSecure' method.  (C-1)
        //:
        //: 2 Call the 'generateNonSecure' method with different 'numGuids'
        //:   values.  (C-2)
        //:
        //: 3 Check the internal structure of returned GUIDs to verify that
        //:   they are the right type.  (C-3)
        //:
        //: 4 Inspect memory areas just before and after the region that
        //:   receives GUIDs to verify that it is unchanged.  (C-4)
        //
        // Testing:
        //   void generateNonSecure(Guid *result, size_t numGuids)
        //   void generateNonSecure(unsigned char *result, size_t numGuids)
        //   Guid generateNonSecure()
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'generateNonSecure'" << endl
                          << "===========================" << endl;
        enum  { NUM_ITERS = 15 };

        // Create arrays two larger to allow checking for overflows at both
        // ends.
        Obj guids[NUM_ITERS + 2];
        Obj prevGuids[NUM_ITERS + 2];
        cout << dec;
        bsl::memset(guids, 0, sizeof(guids));
        if (veryVerbose) {
            cout << endl
                 << "A single GUID can be passed and loaded." << endl
                 << "---------------------------------------" << endl;
        }
        for (bsl::size_t i = 1; i < NUM_ITERS + 1; ++i) {
            if (0 == i % 2) {
                Util::generateNonSecure(&guids[i], 1);
            }
            else {
                guids[i] = Util::generateNonSecure();
            }
            prevGuids[i] = guids[i];
            if (veryVerbose) { P_(i) P(guids[i]); }
            bsl::size_t j;
            for (j = 1; j <= i; ++j) {
                if (veryVeryVerbose) { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j]     != Obj());
                ASSERTV(i, j, prevGuids[j] == guids[j]);
            }
            for (; j < NUM_ITERS + 2; ++j) {
                if (veryVeryVerbose) { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j] == Obj());
            }
            ASSERTV(i, guids[0] == Obj());
        }

        if (veryVerbose) {
            cout << endl
                 << "Get multiple generateNonSecure (PCG) GUIDs." << endl
                 << "-------------------------------------------" << endl;
        }
        for (int i = 1; i < NUM_ITERS + 1; ++i) {
            bsl::memset(guids, 0, sizeof(guids));
            Util::generateNonSecure(guids + 1, i - 1);
            if (veryVerbose)  {
                int idx = i ? i - 1 : 0;
                P_(idx) P(guids[idx]);
            }
            int j;
            for (j = 1; j < i; ++j) {
                if (veryVeryVerbose)  { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j] != Obj());
            }
            for (; j < NUM_ITERS + 1; ++j) {
                if (veryVeryVerbose)  { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j] == Obj());
            }
            ASSERTV(i, guids[0] == Obj());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'getLeastSignificantBits'
        //
        // Concerns:
        //: 1 The least significant bytes are returned.
        //: 2 The method does not modify the passed in 'Guid'.
        //
        // Plan:
        //: 1 Generate 'NUM_ITERS' Guids of each type, verifying the version
        //:   for each.
        //
        // Testing:
        //   Uint64 getLeastSignificantBits(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getLeastSignificantBits'" << endl
                          << "=================================" << endl;
            static const struct {
                  int            d_lineNum;  // source line number
                  const Element *d_array;    // byte array
            } DATA[] = {
                // Line             // Array
                { L_,                &V0},
                { L_,                &V1},
                { L_,                &V2},
                { L_,                &V3},
                { L_,                &V4},
                { L_,                &V5},
                { L_,                &V6},
                { L_,                &V7},
                { L_,                &V8},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE  = DATA[ti].d_lineNum;
                const Element& ARRAY = *DATA[ti].d_array;

                Obj       mX(ARRAY);
                const Obj  X = mX;

                bsls::Types::Uint64 lsb = Util::getLeastSignificantBits(X);
                ASSERT(X == mX);
                if (veryVerbose) {
                    P(LINE);
                    P(X);
                    cout << "lsb = " << hex
                         << BSLS_BYTEORDER_BE_U64_TO_HOST(lsb)
                         << dec      << endl;
                }
                ASSERT(0 == ::memcmp(&X[8], &lsb, 8));
            }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'getMostSignificantBits'
        //
        // Concerns:
        //: 1 The most significant bytes are returned.
        //: 2 The method does not modify the passed in 'Guid'.
        //
        // Plan:
        //: 1 Generate various 'Guids' with different values, request the most
        //:   significant bytes and verify that the match the expected value.
        //
        // Testing:
        //   Uint64 getMostSignificantBits(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getMostSignificantBits'" << endl
                          << "================================" << endl;
            static const struct {
                  int            d_lineNum;  // source line number
                  const Element *d_array;    // byte array
            } DATA[] = {
                // Line             // Array
                { L_,                &V0},
                { L_,                &V1},
                { L_,                &V2},
                { L_,                &V3},
                { L_,                &V4},
                { L_,                &V5},
                { L_,                &V6},
                { L_,                &V7},
                { L_,                &V8},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE  = DATA[ti].d_lineNum;
                const Element& ARRAY = *DATA[ti].d_array;

                Obj       mX(ARRAY);
                const Obj  X = mX;

                bsls::Types::Uint64 msb = Util::getMostSignificantBits(X);
                ASSERT(X == mX);
                if (veryVerbose) {
                    P(LINE);
                    P(X);
                    cout << "msb = " << hex
                         << BSLS_BYTEORDER_BE_U64_TO_HOST(msb)
                         << dec      << endl;
                }
                ASSERT(0 == ::memcmp(&X, &msb, 8));
            }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'guidToString'
        //
        // Concerns:
        //: 1 The guid loaded GUID makes the expected value.
        //
        // Plan:
        //: 1 Compose various guid strings of various types, and check the
        //:   return values.
        //
        // Testing:
        //  void guidToString(bsl::string *result, const Guid& guid)
        //  void guidToString(std::string *result, const Guid& guid)
        //  void guidToString(std::pmr::string *result, const Guid& guid)
        //  bsl::string guidToString(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'guidToString'" << endl
                          << "======================" << endl;

        static const struct {
            int           d_lineNum;       // source line number
            const Element d_guidArray;     // an array with the same value as
                                           // the expected guid
            const char *  d_expGuidStr_p;  // specification string
        } DATA[] = {
        //    Line Array          Spec
        //    ---- -------------- ------------------------------------------
            { L_,  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
                                  "00010203-0405-0607-0809-0a0b0c0d0e0f" },
            { L_,  { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                     0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 },
                                  "00102030-4050-6070-8090-a0b0c0d0e0f0" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            const int         LINE       = DATA[ti].d_lineNum;
            const char *const GUID_STR   = DATA[ti].d_expGuidStr_p;
            const Element&    GUID_ARRAY = DATA[ti].d_guidArray;

            const Obj          guid(GUID_ARRAY);
            {
                bsl::string        result;
                const bsl::string  EXP(GUID_STR);
                Util::guidToString(&result, guid);
                if (veryVeryVerbose) { P_(LINE) P(result.c_str()) }
                LOOP_ASSERT(LINE, EXP == result);
                result = Util::guidToString(guid);
                if (veryVeryVerbose) { P_(LINE) P(result.c_str()) }
                LOOP_ASSERT(LINE, EXP == result);
            }

            {
                std::string        result;
                const std::string  EXP(GUID_STR);
                Util::guidToString(&result, guid);
                if (veryVeryVerbose) { P_(LINE) P(result.c_str()) }
                LOOP_ASSERT(LINE, EXP == result);
            }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            {
                std::pmr::string        result;
                const std::pmr::string  EXP(GUID_STR);
                Util::guidToString(&result, guid);
                if (veryVeryVerbose) { P_(LINE) P(result.c_str()) }
                LOOP_ASSERT(LINE, EXP == result);
            }
#endif
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'guidFromString'
        //
        // Concerns:
        //:  1 Each of the various GUID formats are accepted, with the correct
        //:    Guid and return code.
        //:
        //:  2 If a string does not match the grammar it is rejected.
        //:
        //:  3 If a string is rejected, the result is unchanged.
        //:
        //:  4 If a string is rejected, the return code is non-zero.
        //
        // Plan:
        //:  1 Compose various guid strings of various types, and check the
        //:    return values.
        //
        // Testing:
        //   int guidFromString(Guid *result, bsl::string_view guidString)
        //   Guid guidFromString(bsl::string_view guidString)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'guidFromString''" << endl
                          << "=========================" << endl;

        static const struct {
            int           d_lineNum;       // source line number
            const char *  d_guidStr_p;     // specification string
            int           d_returnCode;    // return status
            const Element d_expGuidArray;  // an array with the same value as
                                           // the expected guid
        } DATA[] = {
        //  line   spec                                         code  expected
        //  ----   -------------------------------------------- ----  --------
        // valid guids
            { L_,  "000102030405060708090a0b0c0d0e0f",          0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[000102030405060708090a0b0c0d0e0f]",        0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[ 000102030405060708090a0b0c0d0e0f ]",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[000102030405060708090a0b0c0d0e0f]",        0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[ 000102030405060708090a0b0c0d0e0f ]",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{000102030405060708090a0b0c0d0e0f}",        0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{ 000102030405060708090a0b0c0d0e0f }",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
        // dashed
            { L_,  "00010203-0405-0607-0809-0a0b0c0d0e0f",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[00010203-0405-0607-0809-0a0b0c0d0e0f]",    0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[ 00010203-0405-0607-0809-0a0b0c0d0e0f ]",  0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[00010203-0405-0607-0809-0a0b0c0d0e0f]",    0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[ 00010203-0405-0607-0809-0a0b0c0d0e0f ]",  0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{00010203-0405-0607-0809-0a0b0c0d0e0f}",    0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{ 00010203-0405-0607-0809-0a0b0c0d0e0f }",  0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
        // different values
            { L_,  "00102030405060708090a0b0c0d0e0f0",          0,    {
                            0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                            0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0} },
            { L_,  "00102030405060708090A0B0C0D0E0F0",          0,    {
                            0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                            0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0} },
            { L_,  "AA1A2A3A4A5A6A7A8A9AAABACADAEAFA",          0,    {
                            0xaa, 0x1a, 0x2a, 0x3a, 0x4a, 0x5a, 0x6a, 0x7a,
                            0x8a, 0x9a, 0xaa, 0xba, 0xca, 0xda, 0xea, 0xfa} },
        // invalid guids
            { L_,  "",                                          1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
            { L_,  "{ 00010203-0405-0607-0809-0a0b0c0d0e0f } ", 1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
            { L_,  "{ 00010203-0405-0607-0809-0a0b0c0d0e0f ]",  1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
            { L_,  "{ 00010203-0405-060708090a0b0c0d0e0f }",    1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            const int         LINE       = DATA[ti].d_lineNum;
            const char *const GUID_STR   = DATA[ti].d_guidStr_p;
            const int         RET        = DATA[ti].d_returnCode;
            const Element&    GUID_ARRAY = DATA[ti].d_expGuidArray;

            const Obj  EXP_GUID = Obj(GUID_ARRAY);
            Obj        mX;
            const Obj& X = mX;

            // double bang to send 0 -> 0  and !0 -> 1
            LOOP_ASSERT(LINE, !!RET == !!Util::guidFromString(&mX, GUID_STR));
            if (veryVerbose) { P_(X) P(EXP_GUID) }
            LOOP3_ASSERT(LINE, X, EXP_GUID, mX == EXP_GUID);
            if (RET == 0) {
                LOOP_ASSERT(LINE, EXP_GUID == Util::guidFromString(GUID_STR));
            }
            else {
                LOOP_ASSERT(LINE, Obj() == Util::guidFromString(GUID_STR));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getVersion'
        //
        // Concerns:
        //: 1 The correct type is returned for a generated GUID.
        //
        // Plan:
        //: 1 Generate 'NUM_ITERS' Guids, verifying the version for each.
        //
        // Testing:
        //   int getVersion(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getVersion'" << endl
                          << "====================" << endl;

        enum  { NUM_ITERS = 25 };
        for (bsl::size_t i = 0; i < NUM_ITERS; ++i) {
            Obj g;
            Util::generate(&g);
            if (veryVeryVerbose) { P_(i) P(g) }
            ASSERT(4 == Util::getVersion(g));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'generate'
        //
        // Concerns:
        //: 1 A single GUID can be passed and loaded.
        //: 2 If 'count' is passed, 'count' GUIDs are returned.
        //: 3 The correct type of GUID is returned.
        //: 4 Memory outside the designated range is left unchanged.
        //
        // Plan:
        //: 1 Call the 'generate' method with a count of 1, and call the
        //:   single-value 'generate' method.  (C-1)
        //:
        //: 2 Call the 'generate' method with different count values.  (C-2)
        //:
        //: 3 Check the internal structure of returned GUIDs to verify that
        //:   they are the right type.  (C-3)
        //:
        //: 4 Inspect memory areas just before and after the region that
        //:   receives GUIDs to verify that it is unchanged.
        //
        // Testing:
        //   void generate(Guid *out, size_t numGuids)
        //   void generate(unsigned char *out, size_t numGuids)
        //   Guid generate()
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'generate'" << endl
                          << "==================" << endl;
        enum  { NUM_ITERS = 15 };

        Obj guids[NUM_ITERS + 1]; // one larger to be allow checking bounds.
        Obj prev_guids[NUM_ITERS + 1];
        cout << dec;
        bsl::memset(guids, 0, sizeof(guids));
        if (veryVerbose) {
            cout << endl
                 << "A single GUID can be passed and loaded." << endl
                 << "---------------------------------------" << endl;
        }
        for (bsl::size_t i = 0; i < NUM_ITERS; ++i) {
            if (i % 3 == 0) {
                Util::generate(&guids[i], 1);
            }
            else if (i % 3 == 1) {
                Util::generate(reinterpret_cast<unsigned char*>(&guids[i]), 1);
            }
            else if (i % 3 == 2) {
                guids[i] = Util::generate();
            }
            prev_guids[i] = guids[i];
            if (veryVerbose) { P_(i) P(guids[i]); }
            bsl::size_t j;
            for (j = 0; j <= i; ++j) {
                if (veryVeryVerbose) { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j]      != Obj());
                ASSERTV(i, j, prev_guids[j] == guids[j]);
            }
            for (; j < NUM_ITERS + 1; ++j) {
                if (veryVeryVerbose) { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j] == Obj());
            }
        }
        if (veryVerbose) {
            cout << endl
                 << "Get multiple GUIDs." << endl
                 << "-------------------" << endl;
        }
        for (bsl::size_t i = 0; i < NUM_ITERS; ++i) {
            bsl::memset(guids, 0, sizeof(guids));
            if (i & 1) {
                Util::generate(guids, i);
            }
            else {
                Util::generate(reinterpret_cast<unsigned char *>(guids), i);
            }
            if (veryVerbose)  {
                bsl::size_t idx = i ? i - 1 : 0;
                P_(idx) P(guids[idx]);
            }
            bsl::size_t j;
            for (j = 0; j < i; ++j) {
                if (veryVeryVerbose)  { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j] != Obj());
            }
            for (; j < NUM_ITERS + 1; ++j) {
                if (veryVeryVerbose)  { P_(j) P(guids[j]); }
                ASSERTV(i, j, guids[j] == Obj());
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TESTING 'generateNonSecure' FROM MULTIPLE THREADS
        //
        // Concerns:
        //: 1 Performance benchmark of 'generateNonSecure'.
        //
        // Plan:
        //: 1 For various batch sizes and thread counts:
        //:
        //:   1 Create multiple threads.
        //:
        //:   2 Exercise all 'generateNonSecure' methods in batches.
        //:
        //:   3 Output stopwatch results.
        //
        // Testing:
        //   PERFORMANCE TESTING 'generateNonSecure' FROM MULTIPLE THREADS
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "PERFORMANCE TESTING 'generateNonSecure' FROM MULTIPLE "
                    "THREADS"
                 << endl
                 << "========================================================="
                    "===="
                 << endl;

        PerfThreadData threadData;

        cout << "\n\n";
        cout << "batchSize"
             << ", threadCount"
             << ", executable"
             << ", userTime"
             << ", systemTime"
             << ", wallTime"
             << ", cpuTime" << endl;

        const int MAX_THREADS_POWER_OF_2 = 3;
        const int MAX_THREADS            = 1 << MAX_THREADS_POWER_OF_2;

        const int MAX_BATCH_POWER_OF_2 = 7;

        for (int thp = 0; thp <= MAX_THREADS_POWER_OF_2; thp++) {
            for (int bsp = 0; bsp <= MAX_BATCH_POWER_OF_2; bsp++) {

                const int numThreads = 1 << thp;

                ASSERTV(numThreads, MAX_THREADS, numThreads <= MAX_THREADS);

                bslmt::ThreadUtil::Handle handles[MAX_THREADS];

                bslmt::Latch startLatch(numThreads + 1);
                bslmt::Latch endLatch(numThreads + 1);

                threadData.d_batchSize    = 1 << bsp;
                threadData.d_iterations   = 32 * 65536;
                threadData.d_startLatch_p = &startLatch;
                threadData.d_endLatch_p   = &endLatch;

                for (int i = 0; i < numThreads; ++i) {
                    ASSERT(0 ==
                           bslmt::ThreadUtil::create(&handles[i],
                                                     &threadMultiPerfFunction,
                                                     &threadData));
                }

                bsls::Stopwatch sw;

                startLatch.arriveAndWait();
                sw.start(true);

                endLatch.arriveAndWait();
                sw.stop();

                const double userTime =
                           round(1e9 * (sw.accumulatedUserTime()) /
                                 static_cast<double>(threadData.d_iterations));
                const double systemTime =
                           round(1e9 * (sw.accumulatedSystemTime()) /
                                 static_cast<double>(threadData.d_iterations));
                const double wallTime =
                           round(1e9 * (sw.accumulatedWallTime()) /
                                 static_cast<double>(threadData.d_iterations));

                const double cpuTime = userTime + systemTime;

                cout << threadData.d_batchSize
                     << ", " << numThreads << ", " << argv[0]
                     << ", " << userTime << ", " << systemTime
                     << ", " << wallTime << ", " << cpuTime << endl;

                for (int i = 0; i < numThreads; ++i) {
                    ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
                }
            }
        }
        cout << "\n" << endl;

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // COLLISION TESTING 'GuidState_Imp'
        //
        // Concerns:
        //: 1 'GuidState_Imp' will, when 'generate' is called a large number of
        //:   times, result in the expected number of collisions.
        //
        // Plan:
        //: 1 Seed a 'GuidState_Imp' with known seed values (such as zero).
        //:
        //: 2 Perform a large number of calls to 'generate' on
        //:   'GuidState_Imp' counts
        //:
        //: 3 For each of the results produced in step 2, check that result to
        //:   a bloom filter.
        //:
        //: 4 If the bloom filter check in step 3 indicates the guid may have
        //:   previously been generated, add that guid into a set.
        //:
        //: 5 Reset 'GuidState_Imp' with the same seeds as used in step 1
        //:
        //: 6 Repeat step 2.
        //:
        //: 7 For each of the guids generated in step 6, if that guid appears
        //:   in the set populated in step 4, keep count of the number of
        //:   appearances, printing a message where that count is greater than
        //:   one.
        //
        // Testing:
        //   COLLISION TESTING 'GuidState_Imp'
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "COLLISION TESTING 'GuidState_Imp'"
                 << endl
                 << "================================="
                 << endl;

        // Find duplicates; with only 2^64 unique GUIDs, there should be a 99%
        // chance of hitting a duplicate after only 13,034,599,789 iterations
        // given truly random inputs.
        static const uint64_t ITERATIONS = 13034599789ull;

#ifdef BSLS_PLATFORM_CPU_32_BIT
        // This is undersized but the best we can do on a 32-bit platform.
        static const uint64_t INITIAL_SIEVE_BITS = (1ull << 31);
        // This is undersized but the best we can do on a 32-bit platform.
        static const uint64_t CHECK_SIEVE_BITS = (1ull << 31);
#else
        // As we have 4 random numbers the optimum size is just under 8 bits
        // per item, but has to be a power of 2.
        static const uint64_t INITIAL_SIEVE_BITS = (1ull << 35) * 8ull;
        // We don't know what the hash size will be, so checkSieve is
        // deliberately oversized to speed up phase 2.
        static const uint64_t CHECK_SIEVE_BITS = (1ull << 32) * 8ull;
#endif

        bdlb::GuidState_Imp guidState;

        bsl::array<uint64_t, bdlb::GuidState_Imp::k_GENERATOR_COUNT> seeds =
                                                                  {0, 0, 0, 0};
        guidState.seed(seeds);

        bsl::unordered_map<bdlb::Guid, uint32_t> check;
        BasicBloomFilter<CHECK_SIEVE_BITS> checkSieve;

        {
            BasicBloomFilter<INITIAL_SIEVE_BITS> initialSieve;

            for (uint64_t i = 0; i <= ITERATIONS; ++i) {
                if (ITERATIONS == i) {
                    // Force an artificial collision to ensure the test
                    // apparatus is functioning.
                    guidState.seed(seeds);
                }
                bsl::uint32_t
                    randomInts[bdlb::GuidState_Imp::k_GENERATOR_COUNT];
                guidState.generateRandomBits(&randomInts);
                simulateRfc4122(&randomInts);

                bool exists = initialSieve.checkAndAdd(randomInts);
                if (exists) {
                    // Create test guid:
                    bdlb::Guid guid;
                    bsl::memcpy(reinterpret_cast<unsigned char *>(&guid),
                                randomInts,
                                bdlb::Guid::k_GUID_NUM_BYTES);
                    check.insert(bsl::make_pair(guid, 0));
                    checkSieve.checkAndAdd(randomInts);
                }

                if ((i & ((1ull << 23) - 1)) == 0) {
                    printf("Phase 1: Potential collision sieve... %0.1f%% "
                           "complete... hash contains %lu\r",
                           (static_cast<double>(i) * 100.0 / ITERATIONS),
                           static_cast<unsigned long>(check.size()));
                    fflush(stdout);
                }
            }
            printf("\n");
        }

        // Rewind
        guidState.seed(seeds);

        for (uint64_t i = 0; i <= ITERATIONS; ++i) {
            if (ITERATIONS == i) {
                // Force an artificial collision to ensure the test apparatus
                // is functioning.
                guidState.seed(seeds);
            }
            // Create test guid:
            bsl::uint32_t randomInts[bdlb::GuidState_Imp::k_GENERATOR_COUNT];
            guidState.generateRandomBits(&randomInts);
            simulateRfc4122(&randomInts);

            if (checkSieve.check(randomInts))
            {
                bdlb::Guid guid;
                bsl::memcpy(reinterpret_cast<unsigned char *>(&guid),
                            randomInts,
                            bdlb::Guid::k_GUID_NUM_BYTES);

                bsl::unordered_map<bdlb::Guid, uint32_t>::iterator it =
                    check.find(guid);
                if (it != check.end()) {
                    if (++it->second > 1) {
                        bsl::string guidStr;
                        bdlb::GuidUtil::guidToString(&guidStr, guid);
                        printf("Collision encountered at iteration #%llu with "
                               "GUID %s\n",
                               static_cast<long long unsigned int>(i),
                               guidStr.c_str());
                    }
                }
            }

            if ((i & ((1ull << 23) - 1)) == 0) {
                printf("Phase 2: Searching for GUID collisions... %0.1f%% "
                       "complete\r",
                       (static_cast<double>(i) * 100.0 / ITERATIONS));
                fflush(stdout);
            }
        }
        printf("\n");

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
