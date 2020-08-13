// bslstl_ostringstream.t.cpp                                         -*-C++-*-
#include <bslstl_ostringstream.h>

#include <bslstl_string.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>

#include <ios>
#include <ostream>

#include <stdio.h>
#include <stdlib.h>

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bsl::basic_ostringstream' IS-A 'std::basic_ostream' that uses
// 'bsl::basic_stringbuf' as an output buffer.  All of the functionality and
// state of a 'bsl::basic_ostringstream' object are provided by base class
// sub-objects.  Therefore, it is not necessary to test all of the
// functionality derived from 'std::basic_ostream'.  It is sufficient to test
// that the various constructors initialize object state as expected, and that
// the manipulator and two accessors, all of which are trivial, work as
// expected.
//
// Note that the 'str' accessor may use the default allocator since it returns
// its string result by value; therefore, tests of allocator usage must take
// this into account.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] ostringstream(const A& a = A());
// [ 3] ostringstream(const ostringstream&& original);
// [10] ostringstream(const ostringstream&& original, const A& a);
// [ 5] ostringstream(openmode mask, const A& a = A());
// [ 6] ostringstream(const STRING& s, const A& a = A());
// [ 7] ostringstream(const STRING& s, openmode mask, const A& a = A());
//
// MANIPULATORS
// [ 3] operator=(const ostringstream&& original);
// [ 4] void str(const StringType& value);
// [11] void swap(basic_ostringstream& other);
//
// ACCESSORS
// [ 4] StringType str() const;
// [ 2] StreamBufType *rdbuf() const;
// [ 9] allocator_type get_allocator() const;
//
// FREE FUNCTIONS
// [11] void swap(basic_ostringstream& a, basic_ostringstream& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ 8] CONCERN: Standard allocator can be used
// [ *] CONCERN: In no case does memory come from the global allocator.

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsl::ostringstream  Obj;
typedef bsl::wostringstream WObj;

typedef std::ios_base       IosBase;
typedef IosBase::openmode   Mode;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define the length of a 'bsl::string' value long enough to ensure dynamic
// memory allocation.

const int LENGTH_OF_SUFFICIENTLY_LONG_STRING =
#ifdef BSLS_PLATFORM_CPU_32_BIT
                                               33;
#else                 // 64_BIT
                                               65;
#endif

BSLMF_ASSERT(LENGTH_OF_SUFFICIENTLY_LONG_STRING >
                                        static_cast<int>(sizeof(bsl::string)));

struct StrlenDataRow {
    int  d_line;    // source line number
    int  d_length;  // string length
    char d_mem;     // expected allocation: 'Y', 'N', '?'
};

static
const StrlenDataRow STRLEN_DATA[] =
{
    //LINE  LENGTH                              MEM
    //----  ----------------------------------  ---
    { L_,   0,                                  'N'   },
    { L_,   1,                                  'N'   },
    { L_,   LENGTH_OF_SUFFICIENTLY_LONG_STRING, 'Y'   }
};
const int NUM_STRLEN_DATA = sizeof STRLEN_DATA / sizeof *STRLEN_DATA;

struct OpenModeDataRow {
    int  d_line;  // source line number
    Mode d_mode;
};

static
const OpenModeDataRow OPENMODE_DATA[] =
{
    //LINE  OPENMODE
    //----  ------------------------------------------
    { L_,   IosBase::in                                },
    { L_,   IosBase::out                               },
    { L_,   IosBase::in  | IosBase::out                },
    { L_,   IosBase::ate                               },
    { L_,   IosBase::in  | IosBase::ate                },
    { L_,   IosBase::out | IosBase::ate                },
    { L_,   IosBase::in  | IosBase::out | IosBase::ate }
};
const int NUM_OPENMODE_DATA = sizeof OPENMODE_DATA / sizeof *OPENMODE_DATA;

//=============================================================================
//                               TEST FACILITIES
//-----------------------------------------------------------------------------

namespace {

template <class StringT>
void loadString(StringT *value, int length)
    // Load into the specified 'value' a character string having the specified
    // 'length'.  The behavior is undefined unless 'length >= 0'.
{
    value->resize(length);

    for (int i = 0; i < length; ++i) {
        (*value)[i] =
                     static_cast<typename StringT::value_type>('a' + (i % 26));
    }
}
template <class StreamT, class StringT>
void testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING SWAP
    //   Since 'basic_ostringstream' doesn't have its own data fields, the
    //   'swap' method just calls similar methods of the parent classes. So we
    //   need to verify that these methods are indeed called.
    //
    // Concerns:
    //: 1 The 'swap' method of the 'BaseType' ('StringBufContainer') class is
    //:   called by the 'basic_ostringstream::swap' method.
    //:
    //: 2 The 'swap' method of the 'BaseStream' ('basic_istream') class is
    //:   called by the 'basic_ostringstream::swap' method.
    //:
    //: 3 Swap free function works the same way as the 'swap' method.
    //:
    //: 4 Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create two output streams.
    //:
    //: 2 Write different strings to streams from P-1 to have their
    //:   'BaseStream' ('basic_istream') states changed and set different
    //:   'iostate' values to modify 'BaseType' ('StringBufContainer') states.
    //:
    //: 3 Swap two objects.
    //:
    //: 4 Using the 'str()' member verify that the 'BaseType'
    //:   ('StringBufContainer') part has been swapped.  (C-1)
    //:
    //: 5 Using the 'rdstate()' member verify that the 'BaseStream'
    //:   ('basic_istream') part has been swapped.  (C-2)
    //:
    //: 6 Swap objects once again using free function and verify that objects
    //:   obtain their initial states.  (C-3)
    //:
    //: 7 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones.  (C-4)
    //
    // Testing:
    //   void swap(basic_ostringstream& other);
    //   void swap(basic_ostringstream& a, basic_ostringstream& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTESTING SWAP"
                        "\n============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using namespace BloombergLP;

    if (verbose) printf("\n\tTesting basic behavior.\n");
    {
        StreamT        mX1;
        const StreamT& X1 = mX1;
        StreamT        mX2;
        const StreamT& X2 = mX2;

        const size_t strLength1 = 1;
        const size_t strLength2 = 2;
        StringT      str1;
        StringT      str2;
        loadString(&str1, strLength1);
        loadString(&str2, strLength2);

        // 'StringBufContainer' part.

        mX1.write(str1.data(), strLength1);
        mX2.write(str2.data(), strLength2);

        // 'native_std::basic_ostream' part.

        mX1.setstate(IosBase::failbit);
        mX2.setstate(IosBase::badbit);

        ASSERT(str1             == X1.str()    );
        ASSERT(str2             == X2.str()    );
        ASSERT(X1.str()         != X2.str()    );
        ASSERT(IosBase::failbit == X1.rdstate());
        ASSERT(IosBase::badbit  == X2.rdstate());

        mX1.swap(mX2);

        ASSERT(str2             == X1.str()    );
        ASSERT(str1             == X2.str()    );
        ASSERT(IosBase::badbit  == X1.rdstate());
        ASSERT(IosBase::failbit == X2.rdstate());

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        bsl::swap(mX1, mX2);

        ASSERT(str1             == X1.str()    );
        ASSERT(str2             == X2.str()    );
        ASSERT(IosBase::failbit == X1.rdstate());
        ASSERT(IosBase::badbit  == X2.rdstate());
#endif
    }

    if (verbose) printf("\n\tNegative Testing.\n");
    {
        typedef typename StreamT::char_type CharType;

        typedef bsltf::StdStatefulAllocator<
                                       CharType,
                                       true,  // ON_CONTAINER_COPY_CONSTRUCTION
                                       true,  // ON_CONTAINER_COPY_ASSIGNMENT
                                       true,  // ON_CONTAINER_SWAP
                                       true>  // ON_CONTAINER_MOVE_ASSIGNMENT
                                            PropagatingStdAlloc;

        typedef bsltf::StdStatefulAllocator<
                                      CharType,
                                      true,   // ON_CONTAINER_COPY_CONSTRUCTION
                                      true,   // ON_CONTAINER_COPY_ASSIGNMENT
                                      false,  // ON_CONTAINER_SWAP
                                      true>   // ON_CONTAINER_MOVE_ASSIGNMENT
                                            NonPropagatingStdAlloc;

        typedef bsl::basic_ostringstream<CharType,
                                         bsl::char_traits<CharType>,
                                         PropagatingStdAlloc>
                                            PropagatingObj;
        typedef bsl::basic_ostringstream<CharType,
                                         bsl::char_traits<CharType>,
                                         NonPropagatingStdAlloc>
                                            NonPropagatingObj;

        bsls::AssertTestHandlerGuard guard;

        bslma::TestAllocator   ta1;
        bslma::TestAllocator   ta2;
        PropagatingStdAlloc    pa1(&ta1);
        PropagatingStdAlloc    pa2(&ta2);
        NonPropagatingStdAlloc npa1(&ta1);
        NonPropagatingStdAlloc npa2(&ta2);

        PropagatingObj    poEqualAlloc1(pa1);
        PropagatingObj    poEqualAlloc2(pa1);
        PropagatingObj    poNonEqualAlloc(pa2);

        NonPropagatingObj npoEqualAlloc1(npa1);
        NonPropagatingObj npoEqualAlloc2(npa1);
        NonPropagatingObj npoNonEqualAlloc(npa2);

        ASSERT_PASS(poEqualAlloc1.swap(poEqualAlloc2  ));
        ASSERT_PASS(poEqualAlloc1.swap(poNonEqualAlloc));

        ASSERT_PASS(npoEqualAlloc1.swap(npoEqualAlloc2  ));
        ASSERT_FAIL(npoEqualAlloc1.swap(npoNonEqualAlloc));
    }
#else
    if (verbose) {
        printf("\tThis functionality is not supported.\n");
    }
#endif
}

template <class StreamT, class StringT>
void testCase10()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CTOR WITH ALLOCATOR
    //
    // Concerns:
    //: 1 An object created with the move constructor has the state of the
    //:   moved-from object.
    //:
    //: 2 The supplied allocator is assigned to the object created with the
    //:   move constructor.
    //:
    //: 3 Move construction allocates no memory from the default allocator.
    //:
    //: 4 The moved-from object is in a valid state.
    //
    // Plan:
    //: 1 Create an output stream, using test allocator.
    //:
    //: 2 Write some string to have a state change in the 'BaseStream'
    //:   ('basic_istream') and 'BaseType' ('StringBufContainer').
    //:
    //: 3 Move-construct a stream, using the allocator different from the
    //:   allocator from P-1.
    //:
    //: 4 Using 'get_allocator' method, verify that internal memory management
    //:   system hooked up properly.
    //:
    //: 5 Verify that the 'BaseType' ('StringBufContainer') has been moved.
    //:   The verification is done using the 'str()' member.
    //:
    //: 6 Verify that the 'BaseStream' ('basic_istream') has been moved.  The
    //:   verification is done using the 'fail()' and 'tellp()' members.
    //
    // Testing:
    //   ostringstream(const ostringstream&& original, const A& a);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTESTING MOVE CTOR WITH ALLOCATOR"
                        "\n================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    using namespace BloombergLP;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator oa("object",   veryVeryVeryVerbose);
    bslma::TestAllocator ba("buffer",   veryVeryVeryVerbose);
    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

    for (int i = 0; i <= LENGTH_OF_SUFFICIENTLY_LONG_STRING; ++i)
    {
        StreamT movedFrom(IosBase::out, &oa);

        // Set some state for both 'BaseType' and 'BaseStream'

        StringT str(&ba);
        loadString(&str, i);

        // Change state in 'BaseType'.

        movedFrom.write(str.data(), i);
        ASSERTV(movedFrom.tellp(), i == movedFrom.tellp());

        static const typename StreamT::pos_type seekPos = i ? i -1 : 0;

        movedFrom.seekp(seekPos);
        ASSERTV(movedFrom.tellp(), seekPos == movedFrom.tellp());

         // Change state in 'BaseStream'.

        movedFrom.setstate(IosBase::failbit);
        ASSERT(movedFrom.fail());

        bsls::Types::Int64 DEFAULT_NUM_BYTES_TOTAL = da.numBytesTotal();

        // Move the stream

        StreamT movedTo(std::move(movedFrom), &sa);

        ASSERTV(da.numBytesTotal(),
                DEFAULT_NUM_BYTES_TOTAL == da.numBytesTotal());

        ASSERTV(movedTo.str().c_str(), str == movedTo.str());
        ASSERTV(movedTo.get_allocator().mechanism(),
                &sa == movedTo.get_allocator().mechanism());

        ASSERT(movedTo.fail());
        movedTo.clear();
        ASSERTV(movedTo.tellp(), seekPos == movedTo.tellp());
    }

#else
    if (verbose) {
        printf("\tThis functionality is not supported.\n");
    }
#endif
}

template <class StreamT>
void testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING 'get_allocator'
    //
    // Concerns:
    //: 1 The 'get_allocator' method returns the allocator specified at
    //:   construction, and that is the default allocator at the time of
    //:   object's construction if none was specified at construction.
    //
    // Plan:
    //: 1 Create an object without passing an allocator reference, setup
    //:   temporary default allocator and verify that 'get_allocator' returns a
    //:   copy of the default allocator at the time of object's construction.
    //:
    //: 2 Create an object specifying the allocator and verify that
    //:  'get_allocator' returns a copy of the supplied allocator.
    //
    // Testing:
    //   allocator_type get_allocator() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTESTING 'get_allocator'"
                        "\n=======================\n");

    using namespace BloombergLP;

    bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);

    StreamT        mXD;
    const StreamT& XD = mXD;

    {
        bslma::TestAllocator         tda("temporary", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard tdag(&tda);

        ASSERTV(&da, XD.get_allocator().mechanism(),
                &da == XD.get_allocator().mechanism());
    }

    StreamT        mXS(&sa);
    const StreamT& XS = mXS;

    ASSERTV(&sa, XS.get_allocator().mechanism(),
            &sa == XS.get_allocator().mechanism());
}

template <class StreamT, class BaseT, class StringT, class CharT>
void testCase3()
{
    // ------------------------------------------------------------------------
    // MOVE CTOR AND ASSIGNMENT
    //
    // Concerns:
    //: 1 An object created with the move constructor or assigned with the
    //:   move-assignment operator has the state of the moved-from object.
    //:
    //: 2 Move construction/assignment allocates no memory.
    //:
    //: 3 The moved-from object is in a valid state.
    //
    // Plan:
    //: 1 Create an output stream.
    //:
    //: 2 Write some string to have a state change in the 'BaseStream'
    //:   ('basic_istream') and 'BaseType' ('StringBufContainer').
    //:
    //: 3 Move-construct/assign a stream.
    //:
    //: 4 Verify that the 'BaseType' ('StringBufContainer') has been moved.
    //:   The verification is done using the 'str()' member.
    //:
    //: 5 Verify that the 'BaseStream' ('basic_istream') has been moved.  The
    //:   verification is done using the 'fail()' and 'tellp()' members.
    //
    // Testing:
    //   ostringstream(const ostringstream&& original);
    //   operator=(const ostringstream&& original);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nMOVE CTOR AND ASSIGNMENT"
                        "\n========================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    using namespace BloombergLP;

    if (veryVerbose) {
        printf("\nTesting move construction with short string.\n");
    }

    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);
        StreamT movedFrom(IosBase::out, &oa);

        // Set some state for both 'BaseType' and 'BaseStream'

        static const size_t n = 3;
        StringT s(&sa);
        loadString(&s, n);
        movedFrom.write(s.data(), n);         // state change in 'BaseType'
        ASSERTV(movedFrom.tellp(), n == movedFrom.tellp());

        static const typename StreamT::pos_type seekPos = 1;

        movedFrom.seekp(seekPos);
        ASSERTV(movedFrom.tellp(), seekPos == movedFrom.tellp());

        movedFrom.setstate(IosBase::failbit); // state change in 'BaseStream'
        ASSERT(movedFrom.fail());

        // Move the stream

        StreamT movedTo(std::move(movedFrom));

        ASSERTV(movedTo.str().c_str(), s == movedTo.str());

        ASSERT(movedTo.fail());
        movedTo.clear();
        ASSERTV(movedTo.tellp(), seekPos == movedTo.tellp());
    }

    if (veryVerbose) {
        printf("\nTesting move assignment with short string.\n");
    }

    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);
        StreamT movedFrom(IosBase::out, &oa);

        // Set some state for both 'BaseType' and 'BaseStream'

        static const size_t n = 3;
        StringT s(&sa);
        loadString(&s, n);
        movedFrom.write(s.data(), n);         // state change in 'BaseType'
        ASSERTV(movedFrom.tellp(), n == movedFrom.tellp());

        static const typename StreamT::pos_type seekPos = 1;

        movedFrom.seekp(seekPos);
        ASSERTV(movedFrom.tellp(), seekPos == movedFrom.tellp());

        movedFrom.setstate(IosBase::failbit); // state change in 'BaseStream'
        ASSERT(movedFrom.fail());

        // Move the stream

        StreamT movedTo(&oa);
        movedTo = std::move(movedFrom);

        ASSERTV(movedTo.str().c_str(), s == movedTo.str());

        ASSERT(movedTo.fail());
        movedTo.clear();
        ASSERTV(movedTo.tellp(), seekPos == movedTo.tellp());
    }

    if (veryVerbose) {
        printf("\nTesting move construction with long string.\n");
    }

    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);
        StreamT movedFrom(IosBase::out, &oa);

        // Set some state for both 'BaseType' and 'BaseStream'

        static const size_t n = LENGTH_OF_SUFFICIENTLY_LONG_STRING;
        StringT s(&sa);
        loadString(&s, n);
        movedFrom.write(s.data(), n);         // state change in 'BaseType'
        ASSERTV(movedFrom.tellp(), n == movedFrom.tellp());

        static const typename StreamT::pos_type seekPos = n - 4;

        movedFrom.seekp(seekPos);
        ASSERTV(movedFrom.tellp(), seekPos == movedFrom.tellp());

        movedFrom.setstate(IosBase::failbit); // state change in 'BaseStream'
        ASSERT(movedFrom.fail());

        // Move the stream

        StreamT movedTo(std::move(movedFrom));

        ASSERTV(movedTo.str().c_str(), s == movedTo.str());

        ASSERT(movedTo.fail());
        movedTo.clear();
        ASSERTV(movedTo.tellp(), seekPos == movedTo.tellp());
    }

    if (veryVerbose) {
        printf("\nTesting move assignment with long string.\n");
    }

    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);
        StreamT movedFrom(IosBase::out, &oa);

        // Set some state for both 'BaseType' and 'BaseStream'

        static const size_t n = LENGTH_OF_SUFFICIENTLY_LONG_STRING;
        StringT s(&sa);
        loadString(&s, n);
        movedFrom.write(s.data(), n);         // state change in 'BaseType'
        ASSERTV(movedFrom.tellp(), n == movedFrom.tellp());

        static const typename StreamT::pos_type seekPos = n - 4;

        movedFrom.seekp(seekPos);
        ASSERTV(movedFrom.tellp(), seekPos == movedFrom.tellp());

        movedFrom.setstate(IosBase::failbit); // state change in 'BaseStream'
        ASSERT(movedFrom.fail());

        // Move the stream

        StreamT movedTo(&oa);
        movedTo = std::move(movedFrom);

        ASSERTV(movedTo.str().c_str(), s == movedTo.str());

        ASSERT(movedTo.fail());
        movedTo.clear();
        ASSERTV(movedTo.tellp(), seekPos == movedTo.tellp());
    }
#endif
}

template <class StreamT, class BaseT, class StringT, class CharT>
void testCase2()
{
    // ------------------------------------------------------------------------
    // DEFAULT CTOR
    //   Ensure that we can use the default constructor to create an object
    //   having the expected default-constructed state, and use the primary
    //   manipulator to put that object into a state relevant for testing.
    //
    // Concerns:
    //: 1 An object created with the constructor under test has the specified
    //:   allocator.
    //:
    //: 2 The constructor allocates no memory.
    //:
    //: 3 Excepting the 'str' accessor, any memory allocation is from the
    //:   object allocator.
    //:
    //: 4 Excepting the 'str' accessor, there is no temporary allocation from
    //:   any allocator.
    //:
    //: 5 The 'rdbuf' accessor returns the expected (non-null) value.
    //:
    //: 6 The string buffer is initially empty.
    //:
    //: 7 The object is created with mode 'ios_base::out' in effect.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //   The table-driven technique is used.
    //
    //: 1 For each value, 'S', in a small set of string values:  (C-1..8)
    //:
    //:   1 For each allocator configuration:  (C-1..8)
    //:
    //:     1 Create an object using the default constructor and verify no
    //:       memory is allocated.  (C-2)
    //:
    //:     2 Use the 'rdbuf' accessor to verify that it returns the expected
    //:       (non-null) value.  (C-5)
    //:
    //:     3 Use the 'str' accessor to verify the initial value of the buffer
    //:       (empty).  (C-6)
    //:
    //:     4 For non-empty string values, invoke 'str(S)' and verify that the
    //:       correct allocator was used (if memory allocation is expected).
    //:       Additionally, verify the new value of the string buffer.
    //:       (C-1, 3)
    //:
    //:     5 Stream a character to the object (using the inherited '<<'
    //:       operator) and verify that the result is as expected.  (C-7)
    //:
    //:     6 Verify no temporary memory is allocated from the object allocator
    //:       when supplied.  (C-4)
    //:
    //:     7 Delete the object and verify all memory is deallocated.  (C-8)
    //
    // Testing:
    //   ostringstream(const A& a = A());
    //   StreamBufType *rdbuf() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nDEFAULT CTOR"
                        "\n============\n");

    using namespace BloombergLP;

    for (int ti = 0; ti < NUM_STRLEN_DATA; ++ti) {
        const int  LENGTH = STRLEN_DATA[ti].d_length;
        const char MEM    = STRLEN_DATA[ti].d_mem;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            if (veryVerbose) {
                printf("\nTesting with various allocator configurations.\n");
            }

            StreamT              *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) StreamT();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) StreamT(typename StreamT::allocator_type(0));
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) StreamT(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
                return;                                               // RETURN
              } break;
            }

            StreamT& mX = *objPtr;  const StreamT& X = mX;
                                    const BaseT&   B =  X;

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            ASSERTV(CONFIG, X.rdbuf());
            ASSERTV(CONFIG, X.rdbuf() == B.rdbuf());
            ASSERTV(CONFIG, X.str().empty());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            StringT mS(&scratch);  const StringT& S = mS;
            loadString(&mS, LENGTH);

            if (LENGTH > 0) {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.str(S);

                ASSERTV(CONFIG, ('Y' == MEM) == oam.isInUseUp());

                oam.reset();

                ASSERTV(CONFIG, X.str() == S);

                if ('c' == CONFIG) {
                    ASSERTV(CONFIG, oam.isInUseSame());
                }
            }

            mX << 'X';

            if (S.empty()) { mS.resize(1); }
            mS[0] = static_cast<CharT>('X');

            ASSERTV(CONFIG, X.str() == S);

            // Verify no temporary memory is allocated from the object
            // allocator when supplied.

            if ('c' == CONFIG) {
                ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG,  fa.numBlocksInUse(), 0 ==  fa.numBlocksInUse());
            ASSERTV(CONFIG,  oa.numBlocksInUse(), 0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksInUse(), 0 == noa.numBlocksInUse());
        }
    }
}

template <class StreamT, class BaseT, class StringT, class CharT>
void testCase4()
{
    // ------------------------------------------------------------------------
    // 'str' MANIPULATOR AND 'str' ACCESSOR
    //   Ensure that the 'str' manipulator and 'str' accessor work as expected.
    //
    // Concerns:
    //: 1 The 'str' manipulator has the expected effect on the contents of the
    //:   string buffer.
    //:
    //: 2 The 'str' accessor correctly reports the contents of the string
    //:   buffer.
    //
    // Plan:
    //   The table-driven technique is used.
    //
    //: 1 For each value, 'S', in a small set of string values:  (C-1..2)
    //:
    //:   1 Create an object using the default constructor (tested in case 2).
    //:
    //:   2 Use the 'str' accessor to verify the initial value of the buffer
    //:     (empty).  (C-2)
    //:
    //:   3 Use the 'str' manipulator to set the buffer value to 'S'.  (C-1)
    //:
    //:   4 Use the 'str' accessor to verify the new expected value of the
    //:     string buffer.  (C-2)
    //:
    //:   5 For each value, 'T', in a small set of string values:  (C-1..2)
    //:
    //:     1 Use the 'str' manipulator to set the buffer value to 'T'.  (C-1)
    //:
    //:     2 Use the 'str' accessor to verify the new expected value of the
    //:     string buffer.  (C-2)
    //
    // Testing:
    //   void str(const StringType& value);
    //   StringType str() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\n'str' MANIPULATOR AND 'str' ACCESSOR"
                        "\n====================================\n");

    using namespace BloombergLP;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_STRLEN_DATA; ++ti) {
        const int LENGTH_TI = STRLEN_DATA[ti].d_length;

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        StreamT mX(&sa);  const StreamT& X = mX;
        ASSERT(X.str().empty());
        ASSERT(&da == X.str().get_allocator().mechanism());

        StringT mS(&da);  const StringT& S = mS;
        loadString(&mS, LENGTH_TI);

        mX.str(S);

        ASSERT(S   == X.str());
        ASSERT(&da == X.str().get_allocator().mechanism());

        for (int tj = 0; tj < NUM_STRLEN_DATA; ++tj) {
            const int LENGTH_TJ = STRLEN_DATA[tj].d_length;

            StringT mT(&da);  const StringT& T = mT;
            loadString(&mT, LENGTH_TJ);

            mX.str(T);                             ASSERT(X.str() == T);
        }
    }
}

template <class StreamT, class BaseT, class StringT, class CharT>
void testCase5()
{
    // ------------------------------------------------------------------------
    // OPENMODE CTOR
    //   Ensure that an object created using the constructor that takes an
    //   'openmode' mask has the expected initial state, and use the primary
    //   manipulator to put that object into a state relevant for testing.
    //
    // Concerns:
    //: 1 An object created with the constructor under test has the specified
    //:   allocator.
    //:
    //: 2 The constructor allocates no memory.
    //:
    //: 3 Excepting the 'str' accessor, any memory allocation is from the
    //:   object allocator.
    //:
    //: 4 Excepting the 'str' accessor, there is no temporary allocation from
    //:   any allocator.
    //:
    //: 5 The 'rdbuf' accessor returns the expected (non-null) value.
    //:
    //: 6 The string buffer is initially empty.
    //:
    //: 7 The object is created with mode 'ios_base::out' in effect.
    //:
    //: 8 'ios_base::ate' has the desired effect if specified in 'modeBitMask'.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //   The table-driven technique is used.
    //
    //: 1 For each value, 'M', in a representative set of mode bit-masks:
    //:   (C-1..9)
    //:
    //:   1 For each value, 'S', in a small set of string values:  (C-1..9)
    //:
    //:     1 For each allocator configuration:  (C-1..9)
    //:
    //:       1 Create an object, supplying 'M' to the constructor, and verify
    //:         no memory is allocated.  (C-2)
    //:
    //:       2 Use the 'rdbuf' accessor to verify that it returns the expected
    //:         (non-null) value.  (C-5)
    //:
    //:       3 Use the 'str' accessor to verify the initial value of the
    //:         buffer (empty).  (C-6)
    //:
    //:       4 For non-empty string values, invoke 'str(S)' and verify that
    //:         the correct allocator was used (if memory allocation is
    //:         expected).  Additionally, verify the new value of the string
    //:         buffer.  (C-1, 3)
    //:
    //:       5 Stream a character to the object (using the inherited '<<'
    //:         operator) and verify that the result is as expected.  (C-7, 8)
    //:
    //:       6 Verify no temporary memory is allocated from the object
    //:         allocator when supplied.  (C-4)
    //:
    //:       7 Delete the object and verify all memory is deallocated.  (C-9)
    //
    // Testing:
    //   ostringstream(openmode mask, const A& a = A());
    // ------------------------------------------------------------------------

    if (verbose) printf("\nOPENMODE CTOR"
                        "\n=============\n");

    using namespace BloombergLP;

    for (int ti = 0; ti < NUM_OPENMODE_DATA; ++ti) {
        const Mode MODE = OPENMODE_DATA[ti].d_mode;

        for (int tj = 0; tj < NUM_STRLEN_DATA; ++tj) {
            const int  LENGTH = STRLEN_DATA[tj].d_length;
            const char MEM    = STRLEN_DATA[tj].d_mem;

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                if (veryVerbose) {
                    printf(
                         "\nTesting with various allocator configurations.\n");
                }

                StreamT              *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) StreamT(MODE);
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objPtr = new (fa) StreamT(
                                          MODE,
                                          typename StreamT::allocator_type(0));
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objPtr = new (fa) StreamT(MODE, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                    return;                                           // RETURN
                  } break;
                }

                StreamT& mX = *objPtr;  const StreamT& X = mX;
                                        const BaseT&   B =  X;

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                // Verify no allocation from the object/non-object allocators.

                ASSERTV(CONFIG,  oa.numBlocksTotal(),
                        0 ==  oa.numBlocksTotal());
                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                ASSERTV(CONFIG, X.rdbuf());
                ASSERTV(CONFIG, X.rdbuf() == B.rdbuf());
                ASSERTV(CONFIG, X.str().empty());

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                StringT mS(&scratch);  const StringT& S = mS;
                loadString(&mS, LENGTH);

                if (LENGTH > 0) {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.str(S);

                    ASSERTV(CONFIG, ('Y' == MEM) == oam.isInUseUp());

                    oam.reset();

                    ASSERTV(CONFIG, X.str() == S);

                    if ('c' == CONFIG) {
                        ASSERTV(CONFIG, oam.isInUseSame());
                    }
                }

                mX << 'X';

                if (MODE & IosBase::ate) {
                    mS.push_back(static_cast<CharT>('X'));
                }
                else {
                    if (S.empty()) { mS.resize(1); }
                    mS[0] = static_cast<CharT>('X');
                }

                ASSERTV(CONFIG, X.str() == S);

                // Verify no temporary memory is allocated from the object
                // allocator when supplied.

                if ('c' == CONFIG && !(MODE & IosBase::ate)) {
                    ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(CONFIG,  fa.numBlocksInUse(),
                        0 ==  fa.numBlocksInUse());
                ASSERTV(CONFIG,  oa.numBlocksInUse(),
                        0 ==  oa.numBlocksInUse());
                ASSERTV(CONFIG, noa.numBlocksInUse(),
                        0 == noa.numBlocksInUse());
            }
        }
    }
}

template <class StreamT, class BaseT, class StringT, class CharT>
void testCase6()
{
    // ------------------------------------------------------------------------
    // STRING CTOR
    //   Ensure that an object created using the constructor that takes a
    //   string value has the expected initial state.
    //
    // Concerns:
    //: 1 An object created with the constructor under test has the specified
    //:   allocator.
    //:
    //: 2 The constructor allocates memory if and only if the string supplied
    //:   at construction is sufficiently long to guarantee memory allocation.
    //:
    //: 3 Excepting the 'str' accessor, any memory allocation is from the
    //:   object allocator.
    //:
    //: 4 Excepting the 'str' accessor, there is no temporary allocation from
    //:   any allocator.
    //:
    //: 5 The 'rdbuf' accessor returns the expected (non-null) value.
    //:
    //: 6 The string buffer initially has the same value as that of the string
    //:   supplied at construction.
    //:
    //: 7 The object is created with mode 'ios_base::out' in effect.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //   The table-driven technique is used.
    //
    //: 1 For each value, 'S', in a small set of string values:  (C-1..8)
    //:
    //:   1 For each allocator configuration:  (C-1..8)
    //:
    //:     1 Create an object, supplying 'S' to the constructor, and verify
    //:       that the correct allocator was used (if memory allocation is
    //:       expected).  Additionally, verify the initial value of the string
    //:       buffer.  (C-1..3, 6)
    //:
    //:     2 Use the 'rdbuf' accessor to verify that it returns the expected
    //:       (non-null) value.  (C-5)
    //:
    //:     3 Stream a character to the object (using the inherited '<<'
    //:       operator) and verify that the result is as expected.  (C-7)
    //:
    //:     4 Verify no temporary memory is allocated from the object allocator
    //:       when supplied.  (C-4)
    //:
    //:     5 Delete the object and verify all memory is deallocated.  (C-8)
    //
    // Testing:
    //   ostringstream(const STRING& s, const A& a = A());
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSTRING CTOR"
                        "\n===========\n");

    using namespace BloombergLP;

    for (int ti = 0; ti < NUM_STRLEN_DATA; ++ti) {
        const int  LENGTH = STRLEN_DATA[ti].d_length;
        const char MEM    = STRLEN_DATA[ti].d_mem;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            if (veryVerbose) {
                printf("\nTesting with various allocator configurations.\n");
            }

            StreamT              *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            StringT mS(&scratch);  const StringT& S = mS;
            loadString(&mS, LENGTH);

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) StreamT(S);
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) StreamT(S,
                                          typename StreamT::allocator_type(0));
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) StreamT(S, &sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
                return;                                               // RETURN
              } break;
            }

            StreamT& mX = *objPtr;  const StreamT& X = mX;
                                    const BaseT&   B =  X;

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify allocations from the object/non-object allocators.

            if ('N' == MEM) {
                ASSERTV(CONFIG,  oa.numBlocksTotal(),
                        0 ==  oa.numBlocksTotal());
                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());
            }
            else {
                ASSERTV(CONFIG,  oa.numBlocksTotal(),
                        0 !=  oa.numBlocksTotal());
            }

            ASSERTV(CONFIG, X.rdbuf());
            ASSERTV(CONFIG, X.rdbuf() == B.rdbuf());
            ASSERTV(CONFIG, X.str()   == S);

            mX << 'X';

            if (S.empty()) { mS.resize(1); }
            mS[0] = static_cast<CharT>('X');

            ASSERTV(CONFIG, X.str() == S);

            // Verify no temporary memory is allocated from the object
            // allocator when supplied.

            if ('c' == CONFIG) {
                ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG,  fa.numBlocksInUse(), 0 ==  fa.numBlocksInUse());
            ASSERTV(CONFIG,  oa.numBlocksInUse(), 0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksInUse(), 0 == noa.numBlocksInUse());
        }
    }
}

template <class StreamT, class BaseT, class StringT, class CharT>
void testCase7()
{
    // ------------------------------------------------------------------------
    // STRING & OPENMODE CTOR
    //   Ensure that an object created using the constructor that takes a
    //   string value and 'openmode' mask has the expected initial state.
    //
    // Concerns:
    //: 1 An object created with the constructor under test has the specified
    //:   allocator.
    //:
    //: 2 The constructor allocates memory if and only if the string supplied
    //:   at construction is sufficiently long to guarantee memory allocation.
    //:
    //: 3 Excepting the 'str' accessor, any memory allocation is from the
    //:   object allocator.
    //:
    //: 4 Excepting the 'str' accessor, there is no temporary allocation from
    //:   any allocator.
    //:
    //: 5 The 'rdbuf' accessor returns the expected (non-null) value.
    //:
    //: 6 The string buffer initially has the same value as that of the string
    //:   supplied at construction.
    //:
    //: 7 The object is created with mode 'ios_base::out' in effect.
    //:
    //: 8 'ios_base::ate' has the desired effect if specified in 'modeBitMask'.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //   The table-driven technique is used.
    //
    //: 1 For each value, 'M', in a representative set of mode bit-masks:
    //:   (C-1..9)
    //:
    //:   1 For each value, 'S', in a small set of string values:  (C-1..9)
    //:
    //:     1 For each allocator configuration:  (C-1..9)
    //:
    //:       1 Create an object, supplying 'S' and 'M' to the constructor, and
    //:         verify that the correct allocator was used (if memory
    //:         allocation is expected).  Additionally, verify the initial
    //:         value of the string buffer.  (C-1..3, 6)
    //:
    //:       2 Use the 'rdbuf' accessor to verify that it returns the expected
    //:         (non-null) value.  (C-5)
    //:
    //:       3 Stream a character to the object (using the inherited '<<'
    //:         operator) and verify that the result is as expected.  (C-7, 8)
    //:
    //:       4 Verify no temporary memory is allocated from the object
    //:         allocator when supplied.  (C-4)
    //:
    //:       5 Delete the object and verify all memory is deallocated.  (C-9)
    //
    // Testing:
    //   ostringstream(const STRING& s, openmode mask, const A& a = A());
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSTRING & OPENMODE CTOR"
                        "\n======================\n");

    using namespace BloombergLP;

    for (int ti = 0; ti < NUM_OPENMODE_DATA; ++ti) {
        const Mode MODE = OPENMODE_DATA[ti].d_mode;

        for (int tj = 0; tj < NUM_STRLEN_DATA; ++tj) {
            const int  LENGTH = STRLEN_DATA[tj].d_length;
            const char MEM    = STRLEN_DATA[tj].d_mem;

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                if (veryVerbose) {
                    printf(
                         "\nTesting with various allocator configurations.\n");
                }

                StreamT              *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                StringT mS(&scratch);  const StringT& S = mS;
                loadString(&mS, LENGTH);

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) StreamT(S, MODE);
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objPtr = new (fa) StreamT(
                                          S,
                                          MODE,
                                          typename StreamT::allocator_type(0));
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objPtr = new (fa) StreamT(S, MODE, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                    return;                                           // RETURN
                  } break;
                }

                StreamT& mX = *objPtr;  const StreamT& X = mX;
                                        const BaseT&   B =  X;

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                // Verify allocations from the object/non-object allocators.

                if ('N' == MEM) {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            0 ==  oa.numBlocksTotal());
                    ASSERTV(CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());
                }
                else {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            0 !=  oa.numBlocksTotal());
                }

                ASSERTV(CONFIG, X.rdbuf());
                ASSERTV(CONFIG, X.rdbuf() == B.rdbuf());
                ASSERTV(CONFIG, X.str()   == S);

                mX << 'X';

                if (MODE & IosBase::ate) {
                    mS.push_back(static_cast<CharT>('X'));
                }
                else {
                    if (S.empty()) { mS.resize(1); }
                    mS[0] = static_cast<CharT>('X');
                }

                ASSERTV(CONFIG, X.str() == S);

                // Verify no temporary memory is allocated from the object
                // allocator when supplied.

                if ('c' == CONFIG && !(MODE & IosBase::ate)) {
                    ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(CONFIG,  fa.numBlocksInUse(),
                        0 ==  fa.numBlocksInUse());
                ASSERTV(CONFIG,  oa.numBlocksInUse(),
                        0 ==  oa.numBlocksInUse());
                ASSERTV(CONFIG, noa.numBlocksInUse(),
                        0 == noa.numBlocksInUse());
            }
        }
    }
}

template <class CharT>
void testCase8()
{
    // ------------------------------------------------------------------------
    // CONCERN: Standard allocator can be used
    //
    // Concerns:
    //: 1 An object can be created using a standard-compliant allocator.
    //:
    //: 2 The object can successfully obtain memory from that allocator.
    //:
    //: 3 There is no allocation from either the default or global allocator.
    //
    // Plan:
    //: 1 For each of the four constructors, in turn:
    //:   1 Create a 'bslma::TestAllocator' object, and install it as the
    //:     default allocator (note that a ubiquitous test allocator is already
    //:     installed as the global allocator).
    //:
    //:   2 Create an object, using the constructor under test, that uses a
    //:     standard-compliant allocator with minimal features.  (C-1)
    //:
    //:   3 Use the 'str' accessor to verify the initial state of the string
    //:     buffer.  (C-2)
    //:
    //:   4 In the case of the two constructors that do not take a string
    //:     value, use the 'str' manipulator to set the buffer value to 'S', a
    //:     string of sufficient length to guarantee memory allocation; use the
    //:     'str' accessor to verify the new expected value of the string
    //:     buffer.  (C-2)
    //:
    //:   5 Use the test allocator from P-1 to verify that no memory is ever
    //:     allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   CONCERN: Standard allocator can be used.
    // ------------------------------------------------------------------------

    if (verbose) printf("\nCONCERN: Standard allocator can be used"
                        "\n=======================================\n");

    using namespace BloombergLP;

    typedef bsltf::StdTestAllocator<CharT>                            StdAlloc;

    typedef bsl::basic_ostringstream<CharT, bsl::char_traits<CharT>, StdAlloc>
                                                                        Stream;

    typedef bsl::basic_string<CharT, bsl::char_traits<CharT>, StdAlloc> String;

    const Mode MODE = IosBase::out;

    {
        StdAlloc A;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        String mS;     const String& S = mS;
        loadString(&mS, LENGTH_OF_SUFFICIENTLY_LONG_STRING);

        Stream mX(A);  const Stream& X = mX;           ASSERT(X.str().empty());

        mX.str(S);                                     ASSERT(X.str() == S);

        ASSERT(0 == da.numBlocksTotal());
    }

    {
        StdAlloc A;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        String mS;     const String& S = mS;
        loadString(&mS, LENGTH_OF_SUFFICIENTLY_LONG_STRING);

        Stream mX(MODE, A);  const Stream& X = mX;     ASSERT(X.str().empty());

        mX.str(S);                                     ASSERT(X.str() == S);

        ASSERT(0 == da.numBlocksTotal());
    }

    {
        StdAlloc A;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        String mS;        const String& S = mS;
        loadString(&mS, LENGTH_OF_SUFFICIENTLY_LONG_STRING);

        Stream mX(S, A);  const Stream& X = mX;        ASSERT(X.str() == S);

        ASSERT(0 == da.numBlocksTotal());
    }

    {
        StdAlloc A;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        String mS;     const String& S = mS;
        loadString(&mS, LENGTH_OF_SUFFICIENTLY_LONG_STRING);

        Stream mX(S, MODE, A);  const Stream& X = mX;  ASSERT(X.str() == S);

        ASSERT(0 == da.numBlocksTotal());
    }
}

}  // close unnamed namespace

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Output Operations
/// - - - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::ostringstream' to write
// data of various types into a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from a generic type,
// 'TYPE', to 'bsl::string'.  We use 'bsl::ostringstream' to implement the
// 'toString' function.  We write the data into the stream with 'operator<<'
// and then use the 'str' method to retrieve the resulting string from the
// stream:
//..
    template <class TYPE>
    bsl::string toString(const TYPE& what)
    {
        bsl::ostringstream out;
        out << what;
        return out.str();
    }
//..

}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    using namespace BloombergLP;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    setbuf(stdout, NULL);    // Use unbuffered output.

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
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
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we verify that our 'toString' function works on some simple test
// cases:
//..
    ASSERT(toString(1234) == "1234");
//
    ASSERT(toString<short>(-5) == "-5");
//
    ASSERT(toString("abc") == "abc");
//..

      } break;
      case 11: {
        testCase11<Obj,  bsl::string >();
        testCase11<WObj, bsl::wstring>();
      } break;
      case 10: {
        testCase10<Obj,  bsl::string >();
        testCase10<WObj, bsl::wstring>();
      } break;
      case 9: {
        testCase9<Obj>();
        testCase9<WObj>();
      } break;
      case 8: {
        testCase8<char>();
        testCase8<wchar_t>();
      } break;
      case 7: {
        testCase7<Obj,  std::ostream,  bsl::string,  char>();
        testCase7<WObj, std::wostream, bsl::wstring, wchar_t>();
      } break;
      case 6: {
        testCase6<Obj,  std::ostream,  bsl::string,  char>();
        testCase6<WObj, std::wostream, bsl::wstring, wchar_t>();
      } break;
      case 5: {
        testCase5<Obj,  std::ostream,  bsl::string,  char>();
        testCase5<WObj, std::wostream, bsl::wstring, wchar_t>();
      } break;
      case 4: {
        testCase4<Obj,  std::ostream,  bsl::string,  char>();
        testCase4<WObj, std::wostream, bsl::wstring, wchar_t>();
      } break;
      case 3: {
        testCase3<Obj,  std::ostream,  bsl::string,  char>();
        testCase3<WObj, std::wostream, bsl::wstring, wchar_t>();
      } break;
      case 2: {
        testCase2<Obj,  std::ostream,  bsl::string,  char>();
        testCase2<WObj, std::wostream, bsl::wstring, wchar_t>();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic functionality.
        //
        // Plan:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj mX;  const Obj& X = mX;

        const std::ostream& B = X;

        ASSERT(X.rdbuf());
        ASSERT(X.rdbuf() == B.rdbuf());
        ASSERT(X.str().empty());

        const bsl::string S("ab");
        mX.str(S);

        ASSERT(X.rdbuf());
        ASSERT(X.rdbuf() == B.rdbuf());
        ASSERT(X.str()   == S);

        mX << 'x';

        ASSERT(X.rdbuf());
        ASSERT(X.rdbuf() == B.rdbuf());
        ASSERT(X.str()   == "xb");

        mX << 'y';

        ASSERT(X.rdbuf());
        ASSERT(X.rdbuf() == B.rdbuf());
        ASSERT(X.str()   == "xy");

        const bsl::string T("s");
        mX.str(T);

        ASSERT(X.rdbuf());
        ASSERT(X.rdbuf() == B.rdbuf());
        ASSERT(X.str()   == T);

        mX << 't';

        ASSERT(X.rdbuf());
        ASSERT(X.rdbuf() == B.rdbuf());
        ASSERT(X.str()   == "t");

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
