// bdlde_charconvertutf16.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlde_charconvertutf16.h>

#include <bdlde_charconvertstatus.h>
#include <bdlde_utf8util.h>

#include <bslalg_constructorproxy.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>

#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_c_ctype.h>
#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_cwchar.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//CONCERNS:
//  1) Do not overwrite memory (a) outside the assigned range, nor (b) beyond
//     what you say you have written.  ((b) overlaps with (2a).)
//
//  2) Provide the correct (a) direct return value and (b) return-through-
//     parameter values (when called for).  (c) The null/non-null distinction
//     for the return for parameter values must not affect the operation in
//     any other way.
//
//  3) Do not damage the input string.  (The parameter is 'const', so damage
//     would require deliberate action.)
//
//  4) Create a properly null-terminated string on the output whenever there
//     is room to write anything at all to the output.
//
//  5) Correctly convert single code points, of however many
//     bytes/words (in each direction).
//
//  6) (a) Correctly recognize every possible type of input string error
//     (forbidden range, invalid code sequence, etc.) and, when appropriate,
//     write the given error indicator into the output string.  (Note that
//     2a, 2b, and 2c apply here especially, since the error indicator's
//     presence or absence will change the write-counts.)  (b) Verify that
//     the preceding and subsequent bytes/words are handled properly.
//
//  7) Considering all code points that require a given encoding (2-byte,
//     single-word, etc.) to represent an equivalence class for that coding
//     sequence, handle all the possible 'digraphs' and 'trigraphs' of those
//     equivalence classes, and possibly larger sequences.  The test should
//     use varying code points (bit patterns) within each encoding class.
//
//  8) As in (7), but adding the various types of input string error to the
//     set of equivalence classes.  (A 2-byte encoding cut short before
//     another sequence is not the same as a 3-byte encoding cut short at the
//     end of the string, etc.)  This creates a very large 'alphabet' on which
//     to test strings, but it is important to test that re-sync works
//     properly and that every case consumes at least one input byte or word so
//     that no infinite loop can occur.
//
//  9) Performance: it is highly desirable that the single-byte, single-word
//     case run fast.  There should be a negative test that can be run to
//     confirm that (a) it is faster than the more interesting cases and (b)
//     its performance more closely resembles a byte-to-word or word-to-byte
//     copy than it resembles the performance of more interesting cases.
//
// 10) The combinatorics of these tests, especially (8), could lead to very
//     long run-times if they are not carefully designed.  (5) involves
//     1,114,112 possible iso10646 code values, of which one is null and
//     2048 are invalid (because in UTF-16 they represent words of two-word
//     encodings) (1,112,063 valid code values).  Once this full test is
//     accomplished, subsequent tests should involve boundary cases and
//     representative, varying values within equivalence classes.
//
//  Translating CONCERNS into tests:
//     Concerns 1 - 4 are basic correctness requirements, and should be
//     checked for all tests, except that 2c multiplies the number of test
//     cases, and should not be applied to tests with a large number of
//     cases.  This does not preclude running some of those cases with one
//     setting and some with another, but large case sets should not be
//     repeated to verify 2c.
//
//     Checking concern 3 requires making copies of the input string and
//     doing comparisons (or running any generating code as comparison code).
//     It probably should be run on all the basic cases and on a limited
//     number of the more complex cases.  (Adjacent memory areas should
//     be checked as well, i.e., the input region should be surrounded by
//     additional memory to be checked.)
//
//     Concerns 5-8 involve increasing complexity and number of cases.  Using
//     equivalence classes, boundary values, and varying non-boundary values
//     should provide strong coverage while holding the number of cases to
//     what can be run routinely.  It must be possible to verify that the
//     values are varying appropriately by using flags of high verbosity.
//
//     Concern 9 requires testing on a processor and operating system that
//     can accurately and reproducibly report processor usage.  (Linux and
//     Windows are disqualified.)  It also may require some control over
//     other things running at the time.  Thus it should be a 'negative'
//     test case intended for manual use only.
//

// --- Convert-forward/convert-back tests on large data sets are possible.
// --- (Generate valid iso10646, turn into UTF-16, convert to UTF-8 and back.)

//
//                                Overview
//                                --------
// Exercise boundary cases for both of the conversion mappings as well as
// handling of buffer capacity issues.
//-----------------------------------------------------------------------------
// [18] USAGE EXAMPLE 2
// [17] USAGE EXAMPLE 1
// [16] UTF-8 LENGTH CALCULATION TEST -- INCORRECT UNICODE
// [15] UTF-16 LENGTH CALCULATION TEST -- INCORRECT UNICODE
// [14] UTF-16 & UTF-8 LENGTH CALCULATION TEST -- CORRECT UNICODE
// [13] BACKWARDS BYTE ORDER TEST
// [12] EMBEDDED ZEROES TEST
// [11] UTF-16 -> UTF-8: THOROUGH BROKEN GLASS TEST
// 110] UTF-8 -> UTF-16: MORE THOROUGH BROKEN GLASS TEST
// [ 9] BROKEN GLASS TEST -- UTF8TOUTF16
// [ 8] TESTING ERROR SEQUENCES IN UTF8
// [ 7] TESTING CALLS WITH VECTOR / STRING / WSTRING DESTINATIONS
// [ 6] TRANSLATING MULTILINGUAL TEXT FROM UTF-8 TO UTF-16 AND BACK
// [ 5] CODER/DECODER SYNCHRONIZATION
// [ 4] VARIOUS UTF-8 ERRORS
// [ 3] CONVERT UTF-8 TO UTF-16 and UTF-16 to UTF-8 in strings.
// [ 2] SINGLE-VALUE, LEGAL VALUE TEST
// [ 1] BREATHING/USAGE TEST
//-----------------------------------------------------------------------------
// [16] computeRequiredUtf8Bytes(w_char_t *, w_char_t *, byteOrder);
// [16] computeRequiredUtf8Bytes(ushort *, ushort *, byteOrder);
// [15] computeRequiredUtf16Words(char *, char *);
// [14] computeRequiredUtf16Words(char *, char *);
// [14] computeRequiredUtf8Bytes(w_char_t *, w_char_t *, byteOrder);
// [14] computeRequiredUtf8Bytes(ushort *, ushort *, byteOrder);
// [13] utf8ToUtf16 (all container overloads)
// [13] utf16ToUtf8 (all container overloads)
// [12] utf8ToUtf16 (single container overload)
// [12] utf16ToUtf8 (single container overload)
// [11] utf16ToUtf8 (all overloads)
// [10] utf8ToUtf16 (all overloads)
// [ 9] utf8ToUtf16 (container overloads)
// [ 8] utf8ToUtf16 (container overloads)
// [ 7] utf8ToUtf16 (container overloads)
// [ 7] utf16ToUtf8 (container overloads)
// [ 6] utf8ToUtf16 (various overloads)
// [ 6] utf16ToUtf8 (various overloads)
// [ 5] strange template functions
// [ 4] strange template functions
// [ 3] strange template functions
// [ 2] strange template functions
// [ 1] strange template functions

// SERIOUS THING TO FIX: The tests use varying values for 'BUFFER_ZONE' and the
// value is NOT passed from the place that allocates the space to the place
// that does the work.

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

#define ASSERT_RET(exp)     ((exp) ? 1 : (aSsErT(0, #exp, __LINE__), 0))

// ============================================================================
//                         CUSTOM TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define R(X) #X " = " << (X)
#define R_(X) #X " = " << (X) << " "

// ============================================================================
//                              GLOBAL TYPEDEFS
// ----------------------------------------------------------------------------

typedef bdlde::CharConvertUtf16  Util;
typedef bdlde::CharConvertStatus Status;

// ============================================================================
//                             GLOBAL VARIABLES
// ----------------------------------------------------------------------------

bslma::Allocator& ta = bslma::NewDeleteAllocator::singleton();

static const bdlde::ByteOrder::Enum e_BACKWARDS =
                         bdlde::ByteOrder::e_HOST ==
                                                 bdlde::ByteOrder::e_BIG_ENDIAN
                         ? bdlde::ByteOrder::e_LITTLE_ENDIAN
                         : bdlde::ByteOrder::e_BIG_ENDIAN;

const char a5 = static_cast<char>(0xa5);

// ============================================================================
//                           CUSTOM TEST APPARATUS
// ----------------------------------------------------------------------------

void *hc(unsigned char c)
{
    // convert a char value into a 'void *' so '<<' will print it in hex

    return (void *) (size_t) c;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
const char16_t *usToC16(const unsigned short *us)
{
    return reinterpret_cast<const char16_t *>(us);
}
#endif

// ----------------------------------------------------------------------------
//                   Evaluate and combine: bothAnd, allAnd
// ----------------------------------------------------------------------------

inline
bool bothAnd(bool a, bool b)
    // Force the execution of BOTH of arguments and returns the logical AND of
    // the two.
{
    return a && b;
}

inline
bool allAnd(bool a, bool b)
    // Force the execution of BOTH of arguments and return the logical AND of
    // both arguments.  (The two-argument overload duplicates the 'bothAnd'
    // function.)
{
    return a && b;
}

inline
bool allAnd(bool a, bool b, bool c)
    // Force the execution of ALL three arguments, then returns the logical AND
    // all three arguments.
{
    return a && b && c;
}

inline
bool allAnd(bool a, bool b, bool c, bool d)
    // Forces the execution of ALL of four arguments, then returns the logical
    // AND of all four arguments.
{
    return a && b && c && d;
}

inline
bool allAnd(bool a, bool b, bool c, bool d, bool e)
    // Force the execution of ALL of five arguments, then returns the logical
    // AND of all five arguments.
{
    return a && b && c && d && e;
}

inline
bool allAnd(bool a, bool b, bool c, bool d, bool e, bool f)
    // Force the execution of ALL six arguments, then returns the logical AND
    // of all six arguments.
{
    return a && b && c && d && e && f;
}

inline
bool allAnd(bool a, bool b, bool c, bool d, bool e, bool f, bool g)
    // Force the execution of ALL seven arguments, then returns the logical AND
    // of all seven arguments.
{
    return a && b && c && d && e && f && g;
}

inline
bool allAnd(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h)
    // Forces the execution of ALL eight arguments, then returns the logical
    // AND of all eight arguments.
{
    return a && b && c && d && e && f && g && h;
}

// ----------------------------------------------------------------------------
//  Assertion-expression with value printing on equality test: EXPECTED_GOT
// ----------------------------------------------------------------------------

// The 'EXPECTED_GOT' macro compresses a great deal of testing and output in a
// small space, especially for compound types that support comparison and
// printing.  It is written as an expression so that it may in turn be used as
// an 'if' condition (which may control subsequent test execution or
// v{eryV}*erbose printing).  It evaluates to 1 on success, 0 on failure.  It
// requires that the two parameters be comparable (via ==) and printable (via
// ostream<<).
#define EXPECTED_GOT(EXP,GOT) \
   ( ((EXP) == (GOT)) ? 1 \
                      : ((cout << #EXP << "  (" << deChar(EXP) << ") != " \
                              << #GOT << " (" << deChar(GOT) << ")\n\t"), \
                         (aSsErT(1, #EXP " != " #GOT, __LINE__)), 0) )

// ----------------------------------------------------------------------------
//          FixedVector<>, approximating the c++09 Array<> template
// ----------------------------------------------------------------------------

// FixedVector<TYPE, N> replaces vector<TYPE> in a number of places where the
// execution time of vector<> was found to be a problem.  Rather than recode
// each place, a very limited container is provided, intended to be used the
// way the C++09 Array<> would be used.  It has a fixed maximum size and
// provides subscripting, size(), iterators and const iterators (pointers to
// the contained type) as well as push_back() and pop_back().  It is not
// clever about explicitly creating the contained objects in place and
// destroying them.  FixedVector<TYPE, N> requires that TYPE allow default-
// initialization.

template<class VALUE_TYPE, bsl::size_t ARRAY_SIZE>
class FixedVector {
  public:
    typedef VALUE_TYPE           value_type;
    typedef VALUE_TYPE&          reference;
    typedef VALUE_TYPE const&    const_reference;
    typedef value_type          *iterator;
    typedef const value_type    *const_iterator;
    typedef bsl::size_t          size_type;
    typedef bsl::ptrdiff_t       difference_type;

  private:
    enum { SIZE = ARRAY_SIZE };

    value_type d_array[ARRAY_SIZE];
    size_type  d_size;

  public:
    FixedVector()
    : d_size(0)
    {
    }

    explicit
    FixedVector(size_type initial_size)
    : d_size(initial_size)
    {
    }

    ~FixedVector()
    {
    }

    size_type size() const
    { return d_size; }

    const_iterator cbegin() const
    { return d_array; }

    const_iterator cend() const
    { return d_array + d_size; }

    const_reference operator[](size_type t) const
    { return d_array[t]; }

    size_type max_size() const
    { return SIZE; }

    size_type capacity() const
    { return SIZE; }

    void resize(size_type newSize)
    { d_size = newSize; }

    void push_back(value_type v)
    { d_array[d_size++] = v; }

    value_type pop_back()
    { return d_array[--d_size]; }

    iterator begin()
    { return d_array; }

    iterator end()
    { return d_array + d_size; }

    reference operator[](size_type t)
    { return d_array[t]; }
};

// ----------------------------------------------------------------------------
// ArrayRange<>: unified handling for array, FixedArray, pointer/length, etc.
// ----------------------------------------------------------------------------

//  To prevent template type proliferation with vectors or arrays of various
//  size, and to avoid making everything a vector with its (very) measurable
//  allocation overhead, we define an 'view' that's templatized on the value
//  type, but carries the size as a variable.
//  We'll have a 'const' and non-'const' version, and define appropriate
//  comparisons and conversions.
//  Non-'const':
template<class VALUE_TYPE>
struct ArrayRange {
    // ArrayRange<> allows iteration through an array.  It combines the
    // starting pointer and the length in one object.  It is used to unify the
    // handling of array, pointer/length, and FixedVector<> cases.

    typedef VALUE_TYPE           value_type;
    typedef VALUE_TYPE&          reference;
    typedef VALUE_TYPE const&    const_reference;
    typedef value_type          *iterator;
    typedef const value_type    *const_iterator;
    typedef bsl::size_t         size_type;
    typedef bsl::ptrdiff_t      difference_type;

    value_type      *d_array;
    const size_type  d_size;

    // CREATORS
    ArrayRange(value_type* array, size_type span)
        // Create an ArrayRange given a pointer to an array and a length to
        // span.
    : d_array(array), d_size(span)
    {
    }

    ArrayRange(value_type* aBegin, value_type* aEnd)
        // Create an ArrayRange that spans a given array.
    : d_array(aBegin), d_size(aEnd - aBegin)
    {
    }

    template<bsl::size_t LEN>
    explicit
    ArrayRange(FixedVector<VALUE_TYPE, LEN> &v)
        // Create an ArrayRange that spans a given FixedVector<>
    : d_array(&v[0]), d_size(v.size())
    {
    }

    template<bsl::size_t LEN>
    ArrayRange(VALUE_TYPE (&a)[LEN])
        // Create an ArrayRange that spans a given array.
    : d_array(&a[0]), d_size(LEN)
    {
    }

    size_type size() const
    { return d_size; }

    reference operator[](size_type t)
    { return d_array[t]; }
    const_reference operator[](size_type t) const
    { return d_array[t]; }

    iterator begin() const
    { return d_array; }

    iterator end() const
    { return d_array + d_size; }

    const_iterator cbegin() const
    { return d_array; }

    const_iterator cend() const
    { return d_array + d_size; }
};

//  Const:
template<class VALUE_TYPE>
struct ConstArrayRange {
    typedef VALUE_TYPE           value_type;
    typedef VALUE_TYPE const&    const_reference;
    typedef const value_type    *const_iterator;
    typedef bsl::size_t         size_type;
    typedef bsl::ptrdiff_t      difference_type;

    const value_type *d_array;
    const size_type   d_size;

    ConstArrayRange(const value_type* array, size_type size)
    : d_array(array), d_size(size)
    {
    }

    template<bsl::size_t LEN>
    explicit
    ConstArrayRange(const FixedVector<VALUE_TYPE, LEN> &v)
    : d_array(&v[0]), d_size(v.size())
    {
    }

    explicit
    ConstArrayRange(ArrayRange<VALUE_TYPE>& av)
    : d_array(av.d_array), d_size(av.d_size)
    {
    }

    template<bsl::size_t LEN>
    ConstArrayRange(const VALUE_TYPE (&a)[LEN])
    : d_array(&a[0]), d_size(LEN)
    {
    }

    size_type size() const
    { return d_size; }

    const_reference operator[](size_type t) const
    { return d_array[t]; }

    const_iterator begin() const
    { return d_array; }

    const_iterator cbegin() const
    { return d_array; }

    const_iterator end() const
    { return d_array + d_size; }

    const_iterator cend() const
    { return d_array + d_size; }
};

template<class VALUE_TYPE>
inline
bool operator!=(const ConstArrayRange<VALUE_TYPE>& lhs,
                const ConstArrayRange<VALUE_TYPE>& rhs)
{
    return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
//                         GenCheckArrRange<>:
//  Fill an array with, check an array against the contents of an ArrayRange.
//-----------------------------------------------------------------------------

// Generate/Check pseudo-concept.
//
// The tests apply multiple data sets to common test bodies.  The data sets are
// written into conversion buffers, and it should later be verified that the
// conversion buffers are unchanged.  In C++09, this might be described with a
// concept.  Here it is sufficient that the type provide these:
//    typename value_type
//        The (byte or word) type to be copied out and checked
//    [integral type] size() [const]
//        Returns the total number of bytes or words to be copied out or
//        checked (no null assumed).
//    void copy(value_type *toBuffer) [const]
//        Copies the code point sequence that it stores or generates into
//        the array beginning at 'toBuffer'
//    bool check(value_type const *checkBuffer) [const]
//        Returns 'true' if the code point sequence in the array beginning at
//        'checkBuffer' equals the code point sequence represented by this
//        object, and false otherwise.

// Generate/Check built from an ArrayRange
template<class ARRAY_RANGE>
struct GenCheckArrRange {
    typedef ARRAY_RANGE        RangeType;
    typedef typename RangeType::value_type value_type;
    typedef value_type&        reference;
    typedef value_type const&  const_reference;
    typedef value_type        *iterator;
    typedef const value_type  *const_iterator;
    typedef bsl::size_t        size_type;
    typedef bsl::ptrdiff_t     difference_type;

    RangeType& d_range;

    // CREATORS
    explicit
    GenCheckArrRange(RangeType& range)
    : d_range(range)
    {
    }

    size_type size() const
    {
        return d_range.size();
    }

    // 'value_type' may be a const type, which would prevent us from using it
    // as the parameter in 'fill()' So 'fill()' is a template on the copy-to
    // type, and we verify that they are comparable by doing one code point's
    // copy by assignment.

    template<class TO_TYPE>
    void fill(TO_TYPE* toBuffer) const
    {
        if (size() > 0) {
            *toBuffer = *d_range.begin();  // (Only compiles if 'TO_TYPE' and
                                           // 'value_type' are compatible.)
            memcpy(toBuffer + 1,
                   d_range.begin() + 1,
                   sizeof(value_type) * (size() - 1));
        }
    }

    // We also have to pun signed and unsigned types against each other.  Thus
    // we play the template game for the 'check' method just as for 'fill()'.
    // Note that in order to prevent signed/unsigned promotions on 'char's
    // and 'short's from messing up the tests, we have to force a static cast
    // to the value type in the equality comparison.

    template<class CHECK_TYPE>
    bool check(const CHECK_TYPE* checkBuffer) const
        // Return true if the contents of the array addressed by '*checkBuffer'
        // are identical to the contents of the source buffer held by
        // 'd_range', up to the length 'd_range.size()', and false otherwise.
    {
        return 0 == size()
            ||  (static_cast<value_type>(checkBuffer[0]) == d_range.begin()[0]
             && ( 1 == size()
               || 0 == memcmp(checkBuffer + 1,
                              d_range.begin() + 1,
                              sizeof(value_type) * (size() - 1))));
    }
};

// ----------------------------------------------------------------------------
//    deChar: Print and evaluate 'char's and 'unsigned char's as 'int's.
// ----------------------------------------------------------------------------

//  Template and overloads: 'deChar'
//     Code point types get printed as code points; this is undesirable because
//     not all are printable and because we are working with the coding of
//     octets so we WANT to see the numeric format.  But we can't go putting
//     conversions blindly into templates that handle a variety of types,
//     including some that are not numeric.  The function template and
//     overloads on the 'deChar' function will leave unchanged anything that
//     is not the exact type of the overloads (char and unsigned char) but
//     will convert those two to 'unsigned int' without sign extension.
//
//     The template must use a reference parameter to avoid copying a
//     who-knows-what.
//
//     When we get to C++09, with char, signed char, and unsigned char
//     as distinct types, we'll need another overload.

template<class SOURCE>
inline
const SOURCE& deChar(const SOURCE& i)
{
    return i;
}

unsigned int deChar(char c)
{
    return static_cast<unsigned>(static_cast<unsigned char>(c));
}

unsigned int deChar(unsigned char c)
{
    return static_cast<unsigned>(c);
}

// ----------------------------------------------------------------------------
//             Printing strings in hex or mixed hex and graphic
// ----------------------------------------------------------------------------

// Some "Print-what-I-point-to" objects, and ways to create and return them.
// The "hex" versions print all hexadecimal; the "mixed" versions print mixed
// graphic and hex.

template <class T>
struct HexPrImpl {
    ConstArrayRange<T> d_av;

    HexPrImpl(const T* ptr, bsl::size_t size)
    : d_av(ptr, size)
    { }
};

template <class T>
ostream &operator <<(ostream& os, const HexPrImpl<T>& t);

template <class T>
struct MixedPrImpl {
    ConstArrayRange<T> d_av;

    MixedPrImpl(const T* ptr, bsl::size_t size)
    : d_av(ptr, size)
    { }
};

template <class T>
ostream &operator <<(ostream& os, const MixedPrImpl<T>& t);

// Three forms of array range print-in-hex (using a HexPrImpl object)

template <class T>
HexPrImpl<T> prHexRange(const T *ptr, size_t size);
template <class T>
HexPrImpl<T> prHexRange(const T *first, const T *last);
template <class T, bsl::size_t N>
inline
HexPrImpl<T> prHexRange(const FixedVector<T, N>& v)
{
    return HexPrImpl<T>(v.begin(), v.size());
}

template <class T>
inline
HexPrImpl<T> prHexRange(const ArrayRange<T>& v)
{
    return HexPrImpl<T>(v.begin(), v.size());
}

// Three forms of array range print-in-mixed (using a MixedPrImpl object)

template <class T>
MixedPrImpl<T> prMixedRange(const T *ptr, size_t size);
template <class T>
MixedPrImpl<T> prMixedRange(const T *first, const T *last);
template <class T, bsl::size_t N>
inline
MixedPrImpl<T> prMixedRange(const FixedVector<T, N>& v)
{
    return MixedPrImpl<T>(v.begin(), v.size());
}

template <class T>
inline
MixedPrImpl<T> prMixedRange(
                                                        const ArrayRange<T>& v)
{
    return MixedPrImpl<T>(v.begin(), v.size());
}

// 'printStr' helper routines to print test strings at high verbosity levels.
// @@@ Replace this by the ArrayRange machinery ...
void printStr(const char *q);
void printStr(const unsigned short *p);

// ----------------------------------------------------------------------------
//                       fillArray<>(), checkArray<>()
// ----------------------------------------------------------------------------

// Array fill and fill-check.  Used to verify that unsanctioned writes have
// not occurred in a 'margin' around the intended output buffer.
template<class CHAR_TYPE, class ITER>
void fillArray(ITER      first,
               ITER      last,
               CHAR_TYPE ch);
template<class CHAR_TYPE, class ITER>
int checkFill(ITER      first,
              ITER      last,
              CHAR_TYPE ch);

// ----------------------------------------------------------------------------
//  OdomIter: Run multiple nested iterators by means of a single, flat loop
// ----------------------------------------------------------------------------

template<class ITER, int N_ITER_PARM>
struct OdomIter {
    // Compound "iterator" based on the odometer algorithm.  Note that this is
    // not really an iterator but a generator.  (It's big enough that to return
    // objects via 'begin' and 'end' is a good deal more costly than we expect
    // in an iterator.)
    //
    // 'OdomIter' is set up to work for N iterators of the same type.  (With
    // parameter packs we could do better.  Otherwise a common wrapper type can
    // be used to make dissimilar things work together, and a range type can be
    // used to wrap containers holding the same types but having a different
    // sizing parameter.  There are some 'const' issues as well.)  Note that
    // the iterators do NOT have to operate over the same range, or cover an
    // equal number of values.  Only the types must match.
    //
    // OdomIter works by cycling the last iterator through its range, then
    // resetting it and advancing the next-to-last, and when that reaches the
    // end of its range, it too is reset and the next-to-next-to-last iterator
    // is advanced, and so forth--like turning the wheels of an odometer.
    //
    // Why use it?  Well, we go four loops deep in places and that tends to
    // nest things off the page.  OdomIter flattens the loops out.  @+@+@+@
    // Need a range iterator to use this in the other places!

    typedef ITER Iter;
    enum { N_ITER = N_ITER_PARM };

    struct Wheel {
        // The Wheel represents a single 'odometer wheel'.  Each has its own
        // begin and end values, and its own current position.

        Iter d_begin;
        Iter d_end;
        Iter d_pos;
    };

    Wheel d_wheels[N_ITER];
        // The array of wheels.  The last wheel (N_ITER - 1) turns fastest, and
        // corresponds to the innermost loop.

    // The general algorithm is to turn the lowest wheel, then if it rolls
    // over, to turn the next wheel, and so forth.  But it's a little trickier
    // because we want the behavior of a fixed end state and explicit
    // return-to-beginning from the end state rather that just rolling over.

    template<class COLLECTION>
    explicit
    OdomIter(COLLECTION *const *init)
        // Create the Odom-Iter from the homogeneous list of collections
        // passed as 'COLLECTION** init'
    {
        for (int i = 0 ; i < N_ITER ; ++i) {
            d_wheels[i].d_begin = init[i]->begin();
            d_wheels[i].d_end = init[i]->end();
            d_wheels[i].d_pos = init[i]->begin();
        }
    }

    void reset()
        // Turn the "odometer" back to the initial position.
    {
        for (int i = 0; i < N_ITER ; i++) {
            d_wheels[i].d_pos = d_wheels[i].d_begin;
        }
    }

    bool done() const
        // Return 'true' if all wheels are at their end positions, otherwise
        // 'false'.
    {
        for (int i = 0 ; i < N_ITER ; i++) {
            if (d_wheels[i].d_pos != d_wheels[i].d_end)
                return false;                                         // RETURN
        }
        return true;
    }

    inline
    operator bool() const
        // Inverse sense of done(): returns true if there is more to go,
        // otherwise false.
    {
        return !done();
    }

    inline
    Iter operator[](int i)
        // Return the true iterator at position 'i'.
    {
        return d_wheels[i].d_pos;
    }

    inline
    const Iter operator[](int i) const
        // Return the true iterator at position 'i'.
    {
        return d_wheels[i].d_pos;
    }

    inline
    const Iter begin(int i) const
        // Return the begin() value extracted for the true iterator at position
        // 'i'.
    {
        return d_wheels[i].d_begin;
    }

    inline
    const Iter end(int i) const
        // Return the end() value extracted for the true iterator at position
        // 'i'.
    {
        return d_wheels[i].d_end;
    }

    bool next()
        // Returns false if the OdomIter is at the end; otherwise advances the
        // OdomIter, then returns true if it is not at the end of its cycle,
        // or false if it is at the end of its cycle.
    {
        int i;
        for (i = N_ITER - 1 ; ; --i) {
            Wheel& w = d_wheels[i];
            if (w.d_pos != w.d_end
             && ++w.d_pos != w.d_end) {
                break;
            }

            if (0 == i) {
                return false;                                         // RETURN
            }
        }

        for (++i; i < N_ITER; ++i) {
            d_wheels[i].d_pos = d_wheels[i].d_begin;
        }

        return true;
    }
};

// ----------------------------------------------------------------------------
// HighBit<> and BufferSizes<>: compile-time computations to help size arrays.
// ----------------------------------------------------------------------------

template<unsigned long X>
struct HighBit {
  private:
    // Isolate the highest bit set in X.  (Compile-time metaprogramming.)  Used
    // below in BufferSizes.  There need be no instances of this object, as all
    // its products are enums, and the only interesting one is HIGH_BIT.

    enum { K6 = X < ( 1 << 6 ) ? 0 :     X < ( 1 << 12 ) ? 6 :
                X < ( 1 << 18 ) ? 12 :   X < ( 1 << 24 ) ? 18 :
                X < ( 1 << 30 ) ? 24 :   30 };
    enum { R6 = X >> K6 };
    enum { X1 = R6 < 1 ? 0 :   R6 < 2 ? 1 :   R6 < 4 ? 2 :
                R6 < 8 ? 4 :   R6 < 16 ? 8 :  R6 < 32 ? 16 :  32 };

  public:
    enum { HIGH_BIT = X1 << K6 };
};

template<bsl::size_t N_CHARS_P,
         int         FROM_SIZE_P,
         int         TO_SIZE_P,
         int         MARGIN_P>
struct BufferSizes {
    // Calculate the sizes needed for various buffers.  Compile-time
    // metaprogramming.  There need be no instances of this object, as all its
    // products are enums.

    enum { N_CHARS   = N_CHARS_P,    // Number of code points, of whatever size
           FROM_SIZE = FROM_SIZE_P,  // Bytes or words per input char
           TO_SIZE   = TO_SIZE_P,    // Bytes or words per output char
           MARGIN    = MARGIN_P      // Bytes or words in the security buffer
                                     // zone, which is filled before the
                                     // conversion and checked afterwards.
    };

    enum { FROM_BUF_SIZE = N_CHARS * FROM_SIZE + 1,  // + 1 for the null.
           TO_BUF_SIZE   = HighBit<N_CHARS * TO_SIZE + 2 * MARGIN + 1>::
                                                                HIGH_BIT * 2
           // The TO_BUF should be a power of two that will hold the output
           // (including a null) plus the two margins.  This is imperfect; if
           // the minimum size is an exact power of two, it will nonetheless
           // double it.  (This is an unlikely case.)
    };
};

// ----------------------------------------------------------------------------
// ConvRslt: Summarizing the three return values from a conversion function.
// ----------------------------------------------------------------------------

//  The ConvRslt holds the three 'return' values from a call to one of the
//  conversion routines.  It can be printed and compared with its like for
//  equality and inequality, and is especially suited for use with the
//  EXPECTED_GOT macro.  The name is kept short because it will be used
//  frequently in expressions.

struct ConvRslt {
    int         d_retVal;   // Return value
    bsl::size_t d_symbols;  // Code Points of whatever size
    bsl::size_t d_units;    // No. of bytes/words written, including the null

    ConvRslt()
    : d_retVal(0x10), d_symbols( -1 ), d_units( -1 )
        // d_retVal's default init does NOT have the 1 or 2 bit set.
    {
    }

    ConvRslt( int aResult, int aSymbols, int aUnits )
    : d_retVal( aResult ), d_symbols( aSymbols ), d_units( aUnits )
    {
    }

    // These mutators apply a change and return a reference to *this, allowing
    // changes to be applied one after the other.  (Evolvers, not mutators?)

    ConvRslt& sym(bsl::size_t newSymbol)
    {
        d_symbols = newSymbol;
        return *this;
    }

    ConvRslt& unit(bsl::size_t newUnit)
    {
        d_units = newUnit;
        return *this;
    }
};

//  Equality, inequality, and output inserter ops on ConvRslt,
//  followed by a macro to compare them and complain if they don't match.
inline
bool operator==(const ConvRslt& lhs, const ConvRslt& rhs)
{
    return lhs.d_retVal == rhs.d_retVal
        && lhs.d_symbols == rhs.d_symbols
        && lhs.d_units == rhs.d_units;
}

inline
bool operator!=(const ConvRslt& lhs, const ConvRslt& rhs)
{
    return !(lhs == rhs);
}

ostream& operator<<(ostream& os, const ConvRslt& cvr);

// ----------------------------------------------------------------------------
//             SrcSpec: bundle parameters to a conversion test.
// ----------------------------------------------------------------------------

// SrcSpec gives the source data, error code point, and output buffer size to
// use for a test.  It does not contain the expected return and
// return-by-argument values, which are stored in ConvRslt.

template<class CHAR_TYPE>
struct SrcSpec {
    const CHAR_TYPE  *d_source;
    CHAR_TYPE   d_errorChar;
    bsl::size_t d_dstBufSize;

    SrcSpec(const CHAR_TYPE *source,
            CHAR_TYPE        errorChar,
            bsl::size_t      dstBufSize)
    : d_source(source)
    , d_errorChar(errorChar)
    , d_dstBufSize(dstBufSize)
    {
    }
};

// ----------------------------------------------------------------------------
// WorkPiece and BufferedWPiece: Stewards of the arena for a conversion test.
// ----------------------------------------------------------------------------

// The WorkPiece describes the output buffer and manages sanity tests.  It does
// not itself own the buffer because in some cases we run multiple tests and
// buffers with the same WorkPiece.

template<class CHAR_TYPE>
struct WorkPiece {
    bsl::size_t d_memLength;
    bsl::size_t d_margin;
    bsl::size_t d_winLength; // 2*d_margin + d_winLength <= d_memLength
    CHAR_TYPE   d_fillChar;

    WorkPiece(bsl::size_t memLength,
              bsl::size_t winLength,
              CHAR_TYPE   fillChar,
              bsl::size_t margin = 32)
    : d_memLength(memLength)
    , d_margin(margin)
    , d_winLength(winLength)
    , d_fillChar(fillChar)
    {
    }

    bsl::size_t size() const
    {
        return d_winLength;
    }

    CHAR_TYPE *begin(CHAR_TYPE* mem)
    {
        return &mem[d_margin];
    }

    CHAR_TYPE *end(CHAR_TYPE *mem)
    {
        return &mem[d_margin + d_winLength];
    }

    void fillMargins(CHAR_TYPE *mem);
    int checkMargins(const CHAR_TYPE *mem) const;

    int checkFinalNull(const CHAR_TYPE *mem) const
    {
        return 0 == d_winLength || 0 == mem[d_margin + d_winLength - 1];
    }
};

// The BufferedWPiece associates a buffer (given at construction time) with the
// WorkPiece.  The interface parallels that of the WorkPiece; there is no
// buffer pointer argument in the various individual member functions and the
// buffer pointer and WorkPiece are both accessible.

template<class CHAR_TYPE>
struct BufferedWPiece {
    CHAR_TYPE *const d_buf;
    WorkPiece<CHAR_TYPE> d_wp;

    BufferedWPiece(CHAR_TYPE   *bufp,
                   bsl::size_t  memLength,
                   bsl::size_t  winLength,
                   CHAR_TYPE    fillChar,
                   bsl::size_t  margin = 32)
    : d_buf(bufp), d_wp(memLength, winLength, fillChar, margin)
    {
    }

    CHAR_TYPE *buffer()
    {
        return d_buf;
    }

    WorkPiece<CHAR_TYPE>& workpiece()
    {
        return d_wp;
    }

    bsl::size_t size() const
    {
        return d_wp.size();
    }

    CHAR_TYPE *begin()
    {
        return d_wp.begin(d_buf);
    }

    CHAR_TYPE *end()
    {
        return d_wp.end(d_buf);
    }

    void fillMargins()
    {
        return d_wp.fillMargins(d_buf);
    }

    int checkMargins() const
    {
        return d_wp.checkMargins(d_buf);
    }

    int checkFinalNull() const
    {
        return d_wp.checkFinalNull(d_buf);
    }
};

//-----------------------------------------------------------------------------
// Surrogate Routines
//
// 'Conversion' (below) was written a long time ago based on the assumption
// that 'utf8ToUtf16' and 'utf16ToUtf8' had the same args except for varying
// 'TO_CHAR' and 'FROM_CHAR'.  When we started taking 'bslstl::StringRef's (and
// then replaced them with 'bsl::string_view's) this violated that assumption.
// These surrogate routines provide a layer to reinstate that assumption so we
// can still use the 'Conversion' template.
//-----------------------------------------------------------------------------

static
int surrogateUtf8ToUtf16(unsigned short         *dstBuffer,
                         bsl::size_t             dstCapacity,
                         const char             *srcBuffer,
                         bsl::size_t            *numCodePointsWritten,
                         bsl::size_t            *numWordsWritten,
                         unsigned short          errorWord,
                         bdlde::ByteOrder::Enum  byteOrder)
{
    return Util::utf8ToUtf16(dstBuffer,
                             dstCapacity,
                             srcBuffer,
                             numCodePointsWritten,
                             numWordsWritten,
                             errorWord,
                             byteOrder);
}

//-----------------------------------------------------------------------------
//                          Random Number Generator
//-----------------------------------------------------------------------------

class RandGen {
    typedef bsls::Types::Uint64 Uint64;

    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    Uint64   d_seed;
    Uint64   d_bitAccum;
    unsigned d_bitAccumNumBits;

    // PRIVATE MANIPULATORS
    void replenishBitAccum();
        // Replenish 'd_bitAccum' with fresh random bits.

  public:
    // CREATOR
    explicit
    RandGen(unsigned startSeed = 0);
        // Initialize the generator with the optionally specified 'startSeed'.

    // MANIPULATORS
    unsigned operator()();
        // Return the next random number in the series;

    unsigned bits(unsigned numBits);
        // Return a value where the low-order 'numBits' are random and all
        // other bits are 0.

    char getChar();
        // Return a random 'char'.
};

                        // ------------------------
                        // RandGen: INLINE FUNCIONS
                        // ------------------------

// MANIPULATORS
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

inline
unsigned RandGen::bits(unsigned numBits)
{
    BSLS_ASSERT_SAFE(0 < numBits && numBits < 32);

    if (d_bitAccumNumBits < numBits) {
        replenishBitAccum();
    }

    const unsigned ret = static_cast<unsigned>(
                                           d_bitAccum & ((1u << numBits) - 1));

    d_bitAccum        >>= numBits;
    d_bitAccumNumBits -=  numBits;

    return ret;
}

inline
char RandGen::getChar()
{
    return static_cast<char>(bits(8));
}

                        // ----------------------------
                        // RandGen: NON_INLINE FUNCIONS
                        // ----------------------------

// PRIVATE MANIPULATORS
void RandGen::replenishBitAccum()
{
    // This function is NOT inlined so that inline calls to 'bits' will have a
    // smaller code footprint.

    const Uint64 hi = (*this)();

    d_bitAccum = (hi << 32) | (*this)();

    d_bitAccumNumBits = 64;
}

// CREATOR
RandGen::RandGen(unsigned startSeed)
: d_seed(startSeed)
, d_bitAccumNumBits(0)
{
    // Rarely called, not worth inlining.

    for (unsigned uu = 0; uu < 5; ++uu) {
        (void) (*this)();
    }
}

// ----------------------------------------------------------------------------
// Conversion<>: bring SrcSpec, WorkPiece, and a conversion function together.
// ----------------------------------------------------------------------------

//  The Conversion template is written as a struct so that each conversion can
//  be represented by an object with a short local instance name.

template <class TO_CHAR, class FROM_CHAR>
struct Conversion {
    typedef int (*Function)(TO_CHAR                *dstBuf,
                            bsl::size_t             toSize,
                            const FROM_CHAR        *srcBuf,
                            bsl::size_t            *toSymbols,
                            bsl::size_t            *toUnits,
                            TO_CHAR                 errorChar,
                            bdlde::ByteOrder::Enum  byteOrder);

    typedef TO_CHAR toChar;
    typedef FROM_CHAR fromChar;

    Function d_converter;

    explicit
    Conversion(Function converter)
    : d_converter(converter)
    {
    }

    ConvRslt operator()(WorkPiece<TO_CHAR>&       to,
                        TO_CHAR*                  dstBuf,
                        const SrcSpec<FROM_CHAR>& from,
                        const ConvRslt&           expected)
    {
        enum { INF = (bsl::size_t) -1 };

        ConvRslt result;

        result.d_retVal = (d_converter)(to.begin(dstBuf),
                                        to.d_winLength,
                                        from.d_source,
                                        INF == expected.d_symbols ?
                                                     0 : &result.d_symbols,
                                        INF == expected.d_units ?
                                                     0 : &result.d_units,
                                        static_cast<TO_CHAR>(from.d_errorChar),
                                        bdlde::ByteOrder::e_HOST);

        if (INF == expected.d_symbols) {
            result.d_symbols = INF;
        }

        if (INF == expected.d_units) {
            result.d_units = INF;
        }

        return result;
    }
};

// ----------------------------------------------------------------------------
// ConversionArg<>: Metaprogram find of the function needed for a conversion
// ----------------------------------------------------------------------------

template<class TO_CHAR, class FROM_CHAR>
struct ConversionArg {
    // This template exists to be specialized; the specializations (below)
    // provide the arg needed for Conversion<>::Conversion.
};

template<>
struct ConversionArg<unsigned short, char> {
    // Specialization of 'ConversionArg' to provide (in template fashion) the
    // arg needed to initialize 'Conversion<unsigned short, char>'

    static Conversion<unsigned short, char>::Function arg()
    {
        return &surrogateUtf8ToUtf16;
    }
};

template<>
struct ConversionArg<char, unsigned short> {
    // Specialization of 'ConversionArg' to provide (in template fashion) the
    // arg needed to initialize 'Conversion<char, unsigned short>'

    static Conversion<char, unsigned short>::Function arg()
    {
        return &bdlde::CharConvertUtf16::utf16ToUtf8;
    }
};

// The 'RUN_AND_CHECK' macro sets up the workpiece and runs the basic checks
// against it.  (This could be made a member of 'WorkPiece' and
// 'BufferedWPiece', just as the core of the 'RUN_FOUR_WAYS' macro is
// 'FourWayRunner::runFourWays()'.

#define RUN_AND_CHECK(WP,MEM,RESULT,CONV,SOURCE,EXPECTED)   \
        ((WP).fillMargins(MEM),                             \
         (RESULT) = (CONV)((WP),(MEM),(SOURCE),(EXPECTED)), \
          bothAnd(EXPECTED_GOT((EXPECTED),(RESULT)),        \
                  ASSERT_RET((WP).checkMargins(MEM)) &&         \
                  ASSERT_RET((WP).checkFinalNull(MEM))))

// Given a vector of pointers to strings, compare them and determine
// equivalence classes among them.  Return a vector of vectors, containing the
// equivalence classes among the strings; each string is named by its index in
// the original vector.  (Note: this was intended to return the nested
// FixedVector<> that contains the result, but the Windows compiler generated
// bogus code when it was used in the debugging macros--which have also been
// simplified.)  @+@+@+@+@ Have to get a better way to print out failure.

template<class ARRAY_TYPE, bsl::size_t N_WAY>
void equivClasses( FixedVector<FixedVector<int, N_WAY>, N_WAY > *retVal,
                   const ARRAY_TYPE&                             sv);

// The 'RUN_FOUR_WAYS' macros contains the basic test sequence, set up four
// times to run with all four combinations of return-by-argument.  It does the
// basic sanity checks on each return.  (Using a macro keeps __LINE__'s value
// meaningful.)  It uses 'RUN_AND_CHECK_4' for each basic run/check.
// 'RUN_AND_CHECK_4' evaluates to true if the sanity test on the string passes;
// if all the sanity tests pass (and the expected length is not zero) the
// four-way comparison is safe to do.

// The run-four-way mechanism is a mixture of a struct (to propagate the types
// through the template and to hold state) with macros (to keep __LINE__
// useful.)

template <class TO_CHAR, class FROM_CHAR>
struct FourWayRunner {
    enum { N_WAY = 4 };
    typedef TO_CHAR ToChar;
    typedef FROM_CHAR FromChar;
    ToChar* tcpX;

    SrcSpec<FROM_CHAR>&             d_src;
    WorkPiece<TO_CHAR>              d_wp;
    TO_CHAR                        *d_outBuf[N_WAY];
    Conversion<TO_CHAR, FROM_CHAR>& d_conv;
    ConvRslt&                       d_expected;
    ConvRslt                        d_result[N_WAY];
    ConvRslt                        d_nExps[N_WAY];

    FixedVector<TO_CHAR*,N_WAY>     d_strSet;

    FixedVector<FixedVector<int,N_WAY>,N_WAY> d_strEqClasses; // push_back, etc

    template<bsl::size_t OUTPUT_LEN>
    FourWayRunner(
             TO_CHAR                         (&outputArray)[N_WAY][OUTPUT_LEN],
             TO_CHAR                           fillChar,
             bsl::size_t                       margin,
             SrcSpec<FROM_CHAR>&               src,
             Conversion<TO_CHAR, FROM_CHAR>&   conv,
             ConvRslt&                         exp)
    : d_src(src),
      d_wp(OUTPUT_LEN, src.d_dstBufSize, fillChar, margin),
      d_conv(conv),
      d_expected(exp)
    {
        for (int i = 0 ; i < N_WAY ; i++ ) {
            d_outBuf[i] = outputArray[i];
        }
    }

    void fillMargins(int n)
    { d_wp.fillMargins(d_outBuf[n]); }

    int checkMargins(int n)
    { return d_wp.checkMargins(d_outBuf[n]); }

    int checkFinalNull(int n)
    { return d_wp.checkFinalNull(d_outBuf[n]); }

    ArrayRange<TO_CHAR> begin(int n)
    {
        return ArrayRange<TO_CHAR>(d_wp.begin(d_outBuf[n]),
                              d_wp.end(d_outBuf[n]) - d_wp.begin(d_outBuf[n]));
    }

    bool runAndCheck(int bufferN, int line);
    bool runFourWays(int line);

    int cmpAllStrings()
    {
        equivClasses(&d_strEqClasses, d_strSet);
        return 1 == d_strEqClasses.size();
    }
};

// Note that the 'checkMargins' in this call goes through 'FourWayRunner' while
// in 'RUN_AND_CHECK' it goes through 'WorkPiece'.  ('FourWayRunner' forwards
// to 'WorkPiece', translating the arguments.)

#define RUN_AND_CHECK_4(RN,N) ((RN).runAndCheck(N,__LINE__))

#define RUN_FOUR_WAYS(RUNNER) ((RUNNER).runFourWays(__LINE__))

// Enumeration of the return codes expected from the functions being tested.
// The correctness of these values with respect to the documentation is tested
// in case 2.

enum {
    SUCCESS                  = 0,
    OK                       = 0,
    INVALID_INPUT_CODE_POINT = 1,
    BADC                     = 1,
    OUTPUT_BUFFER_TOO_SMALL  = 2,
    OBTS                     = 2,
    BOTH                     = 3
};

// Some useful multi-octet code points:

    // The 2 lowest 2-octet code points.
    #define U8_00080  "\xc2\x80"
    #define U8_00081  "\xc2\x81"

    // A traditional "interesting" code point, 0xff.
    #define U8_000ff  "\xc3\xbf"

    // The 2 highest 2-octet code points.
    #define U8_007fe  "\xdf\xbe"
    #define U8_007ff  "\xdf\xbf"

    // The 2 lowest 3-octet code points.
    #define U8_00800  "\xe0\xa0\x80"
    #define U8_00801  "\xe0\xa0\x81"

    // The 2 highest 3-octet code points.
    #define U8_0fffe  "\xef\xbf\xbe"
    #define U8_0ffff  "\xef\xbf\xbf"

    // The 2 lowest 4-octet code points.
    #define U8_10000  "\xf0\x80\x80\x80"
    #define U8_10001  "\xf0\x80\x80\x81"

    // The 2 highest 4-octet code points.
    #define U8_10fffe "\xf7\xbf\xbf\xbe"
    #define U8_10ffff "\xf7\xbf\xbf\xbf"

// We will try all combinations of the 'PRECOMPUTED_DATA' code points up to
// 'exhaustiveSearchDepth' in length.

const int exhaustiveSearchDepth = 4; // 5 works fine on AIX, but our Sun boxes
                                     // are slower

// Precomputed conversions for state space enumeration.  These will be
// enumerated by 'buildUpAndTestStringsU8ToU2' and
// 'buildUpAndTestStringsU2ToU8'.

const struct PrecomputedData {
    const char           *d_utf8CodePoint;
    const bsl::size_t     d_utf8CodePointLength;
    const unsigned short  d_utf16Word;
} PRECOMPUTED_DATA[] =
{
    // valid 1-octet code points:

    { "\x1",    1, 0x0001 },
    { "\x21",   1, 0x0021 },
    { "\x7e",   1, 0x007e },
    { "\x7f",   1, 0x007f },

    // valid 2-octet code points:

    { U8_00080, 2, 0x0080 },
    { U8_00081, 2, 0x0081 },
    { U8_007fe, 2, 0x07fe },
    { U8_007ff, 2, 0x07ff },

    // valid 3-octet code points:

    { U8_00800, 3, 0x0800 },
    { U8_00801, 3, 0x0801 },
    { U8_0fffe, 3, 0xfffe },
    { U8_0ffff, 3, 0xffff },
};

bsl::size_t precomputedDataCount = sizeof PRECOMPUTED_DATA
                                 / sizeof *PRECOMPUTED_DATA;

// Utility function validating that a 'utf16ToUtf8' conversion has the expected
// results.  The function will also test to make sure that insufficient
// 'dstCapacity' arguments for the conversion function return an
// 'OUTPUT_BUFFER_TOO_SMALL' result.

void checkForExpectedConversionResultsU2ToU8(unsigned short *input,
                                             char           *expected_output,
                                             bsl::size_t     totalOutputLength,
                                             unsigned short *codePointSizes,
                                             bsl::size_t     codePointCount,
                                             int             verbose,
                                             int             veryVerbose);

// This utility function for testing 'utf16ToUtf8' will *recursively* build up
// input strings in 'inputBuffer' and output strings in 'outputBuffer', and
// call 'checkForExpectedConversionResultsU2ToU8' to make sure that the results
// match.  'inputCursor' and 'outputCursor' point to the "current position" in
// the respective buffers where this level of the recursion will operate.  The
// recursion terminates once 'depth <= 0'.

void buildUpAndTestStringsU2ToU8(int             idx,
                                 int             depth,
                                 unsigned short *inputBuffer,
                                 char           *outputBuffer,
                                 unsigned short *codePointSizes,
                                 bsl::size_t     totalOutputLength,
                                 bsl::size_t     codePointCount,
                                 unsigned short *inputCursor,
                                 char           *outputCursor,
                                 int             verbose,
                                 int             veryVerbose);

// *Break* a copy of the input, manipulating the bits to make each code point
// in turn, and validating the reported 'numCodePointsWritten' and output
// string.

struct PerturbationDesc {
    unsigned char   d_octetToConvertTo;
    bool            d_isNewValid;
    unsigned short  d_newCodePoint;
    int             d_extraInvalidBefore;
    int             d_extraInvalidAfter;
};

// This utility function perturbs a single octet in 'input' and checks that
// only the effects specified in the 'perturb' description occur.

void testSingleOctetPerturbation(const char             *input,
                                 bsl::size_t             perturbationPos,
                                 bsl::size_t             perturbationChar,
                                 const unsigned short   *origExpectedOutput,
                                 bsl::size_t,
                                 unsigned short         *,
                                 bsl::size_t             codePointCount,
                                 const PerturbationDesc &perturb,
                                 int,
                                 int                     veryVerbose)
{
    char           inputBuffer[256];

    strcpy(inputBuffer, input);

    inputBuffer[perturbationPos] = perturb.d_octetToConvertTo;

    int before = perturb.d_extraInvalidBefore;
    int after  = perturb.d_extraInvalidAfter;
    int pos    = (int)perturbationChar;

    // Increment 'codePointCount' to account for additional error code points
    // before and after 'pos' and for the null terminator.

    codePointCount += before + after + 1;

    unsigned short outputBuffer[256] = { 0 };
    bsl::size_t    codePointsWritten = 0;

    // Make sure conversions where 'outputBuffer' is too small result in the
    // correct errors AND a null-terminated output

    int retVal = bdlde::CharConvertUtf16::utf8ToUtf16(outputBuffer,
                                                   codePointCount - 1,
                                                   inputBuffer,
                                                   &codePointsWritten);

    LOOP3_ASSERT( L_, OBTS,   retVal,
                      OBTS == retVal || BOTH == retVal );

    LOOP3_ASSERT( L_, 0,   outputBuffer[codePointCount-2],
                      0 == outputBuffer[codePointCount-2]);

    retVal = bdlde::CharConvertUtf16::utf8ToUtf16(outputBuffer,
                                               codePointCount,
                                               inputBuffer,
                                               &codePointsWritten);

    if (veryVerbose) {
        cout << "PERTURBING:"
             << "\n\tinputBuffer       =";
        printStr(inputBuffer);
        cout << "\n\toutputBuffer      =" << prHexRange(outputBuffer,
                                                              codePointCount)
             << "\n\tperturbation      = { '"
             <<            perturb.d_octetToConvertTo
             << "', '" << (perturb.d_isNewValid?"Y":"N")
             << "', '" <<  perturb.d_newCodePoint
             << "', "  <<  perturb.d_extraInvalidBefore
             << "', "  <<  perturb.d_extraInvalidAfter
             << "}"
             << "\n\torigInput         =";
        printStr(input);
        cout << "\n\torigExpectedOutput="
             << prHexRange(origExpectedOutput, codePointCount)
             << endl;
    }

    if (perturb.d_isNewValid && !before && !after) {
        LOOP3_ASSERT( L_, SUCCESS,   retVal,
                          SUCCESS == retVal );
    }
    else {
        LOOP3_ASSERT( L_, INVALID_INPUT_CODE_POINT,   retVal,
                          INVALID_INPUT_CODE_POINT == retVal );
    }

    ASSERT ( codePointsWritten == codePointCount );

    // Adjust the position in the output of the code point we changed by adding
    // 'before'.

    pos += before;

    for (int i = 0; i < (int) codePointCount; ++i) {
        if (i < pos - before) {
            LOOP3_ASSERT(i, outputBuffer[i],   origExpectedOutput[i],
                            outputBuffer[i] == origExpectedOutput[i]);
        }
        else if (before && i <  pos && i >= pos - before) {
            // We have introduced 'before' new '?'(s) before 'pos'.

            LOOP3_ASSERT(i, outputBuffer[i],   '?',
                            outputBuffer[i] == '?');
        }
        else if (i == pos) {
            // This is the perturbed code point position.

            LOOP3_ASSERT(i, outputBuffer[i],   perturb.d_newCodePoint,
                            outputBuffer[i] == perturb.d_newCodePoint);
        }
        else if (after && i >  pos && i <= pos + after) {
            // We have introduced 'after' new '?'(s) after 'pos'

            LOOP3_ASSERT(i, outputBuffer[i],   '?',
                            outputBuffer[i] == '?');
        }
        else {
            // we're beyond 'pos + after':

            int posInOrig = i - before - after;
            LOOP4_ASSERT(i, posInOrig,
                            outputBuffer[i],   origExpectedOutput[posInOrig],
                            outputBuffer[i] == origExpectedOutput[posInOrig]);
        }
    }
}

// This utility function perturbs each octet of each UTF-8 code point in
// 'input' into each possible alternative code point class, making sure that
// the correct errors are detected.

void perturbUtf8AndCheckConversionFailures(const char     *input,
                                           unsigned short *expected_output,
                                           bsl::size_t     totalInputLength,
                                           unsigned short *codePointSizes,
                                           bsl::size_t     codePointCount,
                                           int             verbose,
                                           int             veryVerbose)
{
    if (veryVerbose) {
        cout << "perturbUtf8AndCheckConversionFailures("
             <<  "\n\tinput             =";
        printStr(input);
        cout << ",\n\texpected_output   ="
             << prHexRange(expected_output, codePointCount+1)
             << ",\n\ttotalInputLength  ="
             << totalInputLength
             << ",\n\tcodePointSizes    ="
             << prHexRange(codePointSizes, codePointCount)
             << ",\n\tcodePointCount    ="
             << codePointCount
             << ");\n";
    }

    if (!totalInputLength) {
        return;                                                       // RETURN
    }

    // The perturbations we can apply to each UTF-8 input code point will
    // depend on its number of octets.  Depending on what permutation we apply
    // to each octet in a code point, some number of previously valid code
    // points will become '?' error indicators or new valid code points, and
    // additional error indicators may be created either before, after, or both
    // before and after the perturbed octet.  For example, changing the middle
    // octet of a 3-octet code point to a '!' code point changes the result
    // from some valid code point to an error, then a '!' code point, then
    // another error.
    //
    //    +----------+-----------+-------------------+----------------------+
    //    | CdPtLen  | Which     | Convert to        |       # of bytes     |
    //    |          | Octet     |                   | becoming   | extra ? |
    //    |          |           |                   |            | bef/aft |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 1-octet: | Octet 1   | illegal 1-octet   |          ? |   0   0 |
    //    |          |           | 2-octet octet 1   |          ? |   0   0 |
    //    |          |           | 3-octet octet 1   |          ? |   0   0 |
    //    |          |           | 4-octet octet 1   |          ? |   0   0 |
    //    |          |           | "extra octet"     |          ? |   0   0 |
    //    +----------+-----------+-------------------+------------+---------+
    //    +----------+-----------+-------------------+------------+---------+
    //    | 2-octet: | Octet 1   | illegal 1-octet   |          ? |   0   1 |
    //    |          |           | '!' 1-octet char  |          ! |   0   1 |
    //    |          |           | 3-octet octet 1   |          ? |   0   0 |
    //    |          |           | 4-octet octet 1   |          ? |   0   0 |
    //    |          |           | "extra octet"     |          ? |   0   1 |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 2-octet: | Octet 2   | illegal 1-octet   |          ? |   1   0 |
    //    |          |           | '!' 1-octet char  |          ! |   1   0 |
    //    |          |           | 2-octet octet 1   |          ? |   1   0 |
    //    |          |           | 3-octet octet 1   |          ? |   1   0 |
    //    |          |           | 4-octet octet 1   |          ? |   1   0 |
    //    +----------+-----------+-------------------+------------+---------+
    //    +----------+-----------+-------------------+------------+---------+
    //    | 3-octet: | Octet 1   | illegal 1-octet   |          ? |   0   2 |
    //    |          |           | '!' 1-octet char  |          ! |   0   2 |
    //    |          |           | 4-octet octet 1   |          ? |   0   0 |
    //    |          |           | "extra octet"     |          ? |   0   2 |
    //    |          |           | 2-octet octet 1   | New valid  |   0   1 |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 3-octet: | Octet 2   | illegal 1-octet   |          ? |   1   1 |
    //    |          |           | '!' 1-octet char  |          ! |   1   1 |
    //    |          |           | 3-octet octet 1   |          ? |   1   0 |
    //    |          |           | 4-octet octet 1   |          ? |   1   0 |
    //    |          |           | 2-octet octet 1   | New valid  |   1   0 |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 3-octet: | Octet 3   | illegal 1-octet   |          ? |   1   0 |
    //    |          |           | '!' 1-octet char  |          ! |   1   0 |
    //    |          |           | 2-octet octet 1   |          ? |   1   0 |
    //    |          |           | 3-octet octet 1   |          ? |   1   0 |
    //    |          |           | 4-octet octet 1   |          ? |   1   0 |
    //    +----------+-----------+-------------------+------------+---------+

    for (int currentCodePoint = 0; currentCodePoint < (int) codePointCount;
                                                          ++currentCodePoint) {
        int currentCodePointStart = 0;
        for (int i=0; i < currentCodePoint; ++i) {
            currentCodePointStart += codePointSizes[i];
        }

        switch (codePointSizes[currentCodePoint]) {
          case 1: {
            // perturbing 1-octet code point

            static const PerturbationDesc oneOctetCharOctetOne[] = {
                { 0xff, false, '?', 0, 0 },  // Illegal 1-octet
                { 0xc2, false, '?', 0, 0 },  // 2-octet char 1
                { 0xef, false, '?', 0, 0 },  // 3-octet char 1
                { 0xf0, false, '?', 0, 0 },  // 4-octet char 1
                { 0x80, false, '?', 0, 0 },  // "extra" octet
            };
            bsl::size_t testCount = sizeof oneOctetCharOctetOne /
                                    sizeof *oneOctetCharOctetOne;

            for (int i = 0; i < (int) testCount; ++i) {
                testSingleOctetPerturbation(input,
                                            currentCodePointStart,
                                            currentCodePoint,
                                            expected_output,
                                            totalInputLength,
                                            codePointSizes,
                                            codePointCount,
                                            oneOctetCharOctetOne[i],
                                            verbose,
                                            veryVerbose);
            }
          } break;

          case 2: {
            // perturbing 2-octet code point, octet 1

            {
                static const PerturbationDesc twoOctetCharOctetOne[] = {
                    { 0xff, false, '?', 0, 1 },  // Illegal 1-octet
                    {  '!',  true, '!', 0, 1 },  // '!' 1-octet char
                    { 0xef, false, '?', 0, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 0, 0 },  // 4-octet char 1
                    { 0x80, false, '?', 0, 1 },  // "extra" octet
                };
                static const
                bsl::size_t testCount = sizeof twoOctetCharOctetOne
                                      / sizeof *twoOctetCharOctetOne;

                for (int i = 0; i < (int) testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCodePointStart,
                                                currentCodePoint,
                                                expected_output,
                                                totalInputLength,
                                                codePointSizes,
                                                codePointCount,
                                                twoOctetCharOctetOne[i],
                                                verbose,
                                                veryVerbose);
                }
            }

            // perturbing 2-octet code point, octet 2

            {
                static const PerturbationDesc twoOctetCharOctetTwo[] = {
                    { 0xff, false, '?', 1, 0 },  // Illegal 1-octet
                    {  '!',  true, '!', 1, 0 },  // '!' 1-octet char
                    { 0xc2, false, '?', 1, 0 },  // 2-octet char 1
                    { 0xef, false, '?', 1, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 1, 0 },  // 4-octet char 1
                };
                static const
                bsl::size_t testCount = sizeof twoOctetCharOctetTwo
                                      / sizeof *twoOctetCharOctetTwo;

                for (int i = 0; i < (int) testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCodePointStart + 1,
                                                currentCodePoint,
                                                expected_output,
                                                totalInputLength,
                                                codePointSizes,
                                                codePointCount,
                                                twoOctetCharOctetTwo[i],
                                                verbose,
                                                veryVerbose);
                }
            }
          } break;

          case 3: {
            // perturbing 3-octet code point, octet 1

            {
                static const PerturbationDesc threeOctetCharOctetOne[] = {
                    { 0xff, false, '?', 0, 2 },  // Illegal 1-octet
                    {  '!',  true, '!', 0, 2 },  // '!' 1-octet char
                    { 0xf0, false, '?', 0, 0 },  // 4-octet char 1
                    { 0x80, false, '?', 0, 2 },  // "extra" octet
                };
                static const
                bsl::size_t testCount = sizeof threeOctetCharOctetOne /
                                        sizeof *threeOctetCharOctetOne;

                for (int i = 0; i < (int) testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCodePointStart,
                                                currentCodePoint,
                                                expected_output,
                                                totalInputLength,
                                                codePointSizes,
                                                codePointCount,
                                                threeOctetCharOctetOne[i],
                                                verbose,
                                                veryVerbose);
                }

                // Changing this byte to a "2-octet char 1" can't be
                // data-driven since we must compute the resulting code point.

                const unsigned short newChar = static_cast<unsigned short>(
                                      ((0xc2 & 0x1f) << 6 )
                                    | (input[currentCodePointStart+1] & 0x3f));
                const PerturbationDesc perturb =
                                             { 0xc2,  true, newChar, 0, 1 };

                testSingleOctetPerturbation(input,
                                            currentCodePointStart,
                                            currentCodePoint,
                                            expected_output,
                                            totalInputLength,
                                            codePointSizes,
                                            codePointCount,
                                            perturb,
                                            verbose,
                                            veryVerbose);
            }

            // perturbing 3-octet code point, octet 2
            {
                static const PerturbationDesc threeOctetCharOctetTwo[] = {
                    { 0xff, false, '?', 1, 1 },  // Illegal 1-octet
                    {  '!',  true, '!', 1, 1 },  // '!' 1-octet char
                    { 0xef, false, '?', 1, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 1, 0 },  // 4-octet char 1
                };
                static const
                bsl::size_t testCount = sizeof threeOctetCharOctetTwo /
                                        sizeof *threeOctetCharOctetTwo;

                for (int i = 0; i < (int) testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCodePointStart + 1,
                                                currentCodePoint,
                                                expected_output,
                                                totalInputLength,
                                                codePointSizes,
                                                codePointCount,
                                                threeOctetCharOctetTwo[i],
                                                verbose,
                                                veryVerbose);
                }

                // Changing this byte to a "2-octet char 1" can't be
                // data-driven since we must compute the resulting code point.
                const
                unsigned short newChar = static_cast<unsigned short>(
                                      ((0xc2 & 0x1f) << 6 )
                                    | (input[currentCodePointStart+2] & 0x3f));
                const
                PerturbationDesc perturb = { 0xc2,  true, newChar, 1, 0 };

                testSingleOctetPerturbation(input,
                                            currentCodePointStart + 1,
                                            currentCodePoint,
                                            expected_output,
                                            totalInputLength,
                                            codePointSizes,
                                            codePointCount,
                                            perturb,
                                            verbose,
                                            veryVerbose);
            }

            // perturbing 3-octet code point, octet 3

            {
                static const PerturbationDesc threeOctetCharOctetThree[] = {
                    { 0xff, false, '?', 1, 0 },  // Illegal 1-octet
                    {  '!',  true, '!', 1, 0 },  // '!' 1-octet char
                    { 0xc2, false, '?', 1, 0 },  // 2-octet char 1
                    { 0xef, false, '?', 1, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 1, 0 },  // 4-octet char 1
                };
                static const
                bsl::size_t testCount = sizeof threeOctetCharOctetThree
                                      / sizeof *threeOctetCharOctetThree;

                for (int i = 0; i < (int) testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCodePointStart + 2,
                                                currentCodePoint,
                                                expected_output,
                                                totalInputLength,
                                                codePointSizes,
                                                codePointCount,
                                                threeOctetCharOctetThree[i],
                                                verbose,
                                                veryVerbose);
                }
            }
          } break;

          default:// not perturbing 4-octet code points
          break;
        }
    }
}

// Utility function validating that a 'utf8ToUtf16' conversion has the expected
// results.  The function will also test to make sure that insufficient
// 'dstCapacity' arguments for the conversion function return an
// 'OUTPUT_BUFFER_TOO_SMALL' result.

void checkForExpectedConversionResultsU8ToU2(const char     *input,
                                             unsigned short *expected_output,
                                             bsl::size_t     totalInputLength,
                                             unsigned short *codePointSizes,
                                             bsl::size_t     codePointCount,
                                             int             verbose,
                                             int             veryVerbose)
{
    int retVal;

    if (veryVerbose) {
        cout << "checkForExpectedConversionResultsU8ToU2("
             <<  "\n\tinput             =";
        printStr(input);
        cout << ",\n\texpected_output   ="
             << prHexRange(expected_output, codePointCount+1)
             << ",\n\ttotalInputLength  ="
             << totalInputLength
             << ",\n\tcodePointSizes    ="
             << prHexRange(codePointSizes, codePointCount)
             << ",\n\tcodePointCount    ="
             << codePointCount
             << ");\n";
    }

    if (!totalInputLength) {
        return;                                                       // RETURN
    }

    perturbUtf8AndCheckConversionFailures(input,
                                          expected_output,
                                          totalInputLength,
                                          codePointSizes,
                                          codePointCount,
                                          verbose,
                                          veryVerbose);

    for (int bufSize = 0; bufSize < (int) codePointCount; ++bufSize) {
        unsigned short outputBuffer[256] = { 0 };
        bsl::size_t codePointsWritten = 0;

        retVal = bdlde::CharConvertUtf16::utf8ToUtf16(
                outputBuffer,
                bufSize,
                input,
                &codePointsWritten);

        LOOP5_ASSERT(L_, OUTPUT_BUFFER_TOO_SMALL,   retVal,
                         bufSize,                   codePointCount,
                         OUTPUT_BUFFER_TOO_SMALL == retVal);
        LOOP3_ASSERT(L_, codePointsWritten,   bufSize,
                         (int) codePointsWritten == bufSize);
    }

    unsigned short outputBuffer[256] = { 0 };
    bsl::size_t codePointsWritten = 0;

    retVal = bdlde::CharConvertUtf16::utf8ToUtf16(outputBuffer,
                                               codePointCount + 1,
                                               input,
                                               &codePointsWritten);

    LOOP3_ASSERT(L_, SUCCESS,   retVal,
                     SUCCESS == retVal);
    LOOP3_ASSERT(L_, codePointsWritten,   1 + codePointCount,
                     1 + codePointCount == codePointsWritten);

    LOOP5_ASSERT(L_, codePointCount + 1,
                     codePointsWritten,
                     prHexRange(outputBuffer,    codePointCount + 1),
                     prHexRange(expected_output, codePointsWritten),
                     0 == memcmp(outputBuffer,
                                 expected_output,
                                 codePointsWritten * sizeof *outputBuffer));
}

// This utility function for testing 'utf8ToUtf16' will *recursively* build up
// input strings in 'inputBuffer' and output strings in 'outputBuffer', and
// call 'checkForExpectedConversionResultsU8ToU2' to make sure that the results
// match.  'inputCursor' and 'outputCursor' point to the "current position" in
// the respective buffers where this level of the recursion will operate.  The
// recursion terminates once 'depth <= 0'.

void buildUpAndTestStringsU8ToU2(int             idx,
                                 int             depth,
                                 char           *inputBuffer,
                                 unsigned short *outputBuffer,
                                 unsigned short *codePointSizes,
                                 bsl::size_t     totalOutputLength,
                                 bsl::size_t     codePointCount,
                                 char           *inputCursor,
                                 unsigned short *outputCursor,
                                 int             verbose,
                                 int             veryVerbose)
{
    // Null-terminate input and expected output:

    *inputCursor  = 0;
    *outputCursor = 0;

    checkForExpectedConversionResultsU8ToU2(inputBuffer,
            outputBuffer,
            totalOutputLength,
            codePointSizes,
            codePointCount,
            verbose,
            veryVerbose);

    if (depth <= 0) {
        return;                                                       // RETURN
    }

    struct PrecomputedData const &d = PRECOMPUTED_DATA[idx];

    strcpy(inputCursor,      d.d_utf8CodePoint);
    inputCursor           += d.d_utf8CodePointLength;

    *outputCursor++        = d.d_utf16Word;
    *outputCursor          = 0;

    totalOutputLength += d.d_utf8CodePointLength;

    codePointSizes[codePointCount++] = static_cast<unsigned short>(
                                                      d.d_utf8CodePointLength);

    for (int i = 0; i < (int) precomputedDataCount; ++i) {
        buildUpAndTestStringsU8ToU2(i,
                                    depth - 1,
                                    inputBuffer,
                                    outputBuffer,
                                    codePointSizes,
                                    totalOutputLength,
                                    codePointCount,
                                    inputCursor,
                                    outputCursor,
                                    verbose,
                                    veryVerbose);
    }
}

int runPlainTextPerformanceTest(void);

//  Permuter<N> provides, in sequence, all possible permutations of the
//  integers [ 0 .. N ).  For not-very-small N, this can take a very
//  long time.

template<bsl::size_t N>
struct Permuter {
    // How this works: A recursive algorithm for generating the permutation is
    // flattened by an odometer-like data structure.
    //
    // The recursive procedure, which returns in the middle, works like this:
    //      Given a set containing N items (the integers 0 .. (N - 1), loop
    //      through the set, selecting each one in turn as the first element
    //      of the sequence.  In each loop, recurse, using the unselected
    //      elements to create the set for the recursion.  Thus, as the
    //      recursion proceeds downward, the top level has the first value of
    //      the permutation, the second level has the second value, and so
    //      forth.  When you recurse into an empty set, you must force the
    //      printing of all the values held by all the levels above.
    //
    // How we actually do the permuting:
    //      The set is stored in an array ('val[]').  Each level of recursion
    //      R is represented by the elements 0 .. R .  Thus at the top
    //      recursion level the selected value is in 'val[0]', at the first
    //      the selected value is in 'val[1]', and so forth.
    //      The initial selection, then, is with the values in numerical order.
    //
    //      Advancing to the next selection, then, means having the lowest
    //      recursion level realize that it does not have another value to try
    //      (since it had only one to begin with) and 'returning' to the
    //      previous level, which selects a different value in turn and
    //      recurses again.  When that second-to-last level has exhausted all
    //      the values it has in its working set, it too returns, to be called.
    //      again with a slightly different set.
    //
    //      We keep track of the state at each level of recursion with a set
    //      of wheels that make up a "conical odometer" whose first wheel has
    //      N positions, whose second wheel has N - 1 positions, and so forth.
    //      The position of the wheel indicates which value has been selected
    //      from the array of positions at and to the right of the wheel.  To
    //      select a value, we swap it from place P (wheel place) + N
    //      (wheel position) into place N, and swap the value at place N
    //      up into place N + P.  To de-select that element of the set
    //      available to the wheel at place P, we reverse (repeat) the swap.
    //      Then we advance the wheel in preparation for selecting the next
    //      element at that place.
    //
    //      If we have returned to wheel position zero, we have exhausted the
    //      elements available in this set.  We must un-recurse (move left to
    //      place P - 1), de-select, advance the wheel, and select again,
    //      before moving right (recursing) once more.
    //
    // How it is all implemented:
    //      Start with the set of values in order, 0 through (N - 1).  (Any
    //      order will do, actually.)  Start with all the wheels at position
    //      zero.  (This is necessary.)  We are now at the first permutation.
    //
    //      To advance, start at place P = N - 2.  (The wheel at N - 1 is
    //      always at position zero.)  First (un)swap the values at positions
    //      P and P + 'wheels'[P].  Then turn the wheel (add one, mod N - P).
    //      If the wheel has not returned to zero, swap the values at P
    //      and P + 'wheels'[P] (which is now one greater than it was).  We
    //      are done; the advance is successful and we have a new permutation.
    //      Return 'true'.
    //
    //      If the wheel has returned to zero, "recurse" by moving to the
    //      left (P -= 1).  If we move below zero (if we were at wheel zero
    //      to start with) we have exhausted the permutations; return 'false'.
    //      Otherwise, go back and continue at "To advance, start", above.
    //      After we have done all our moves-to-the-left, we might be expected
    //      to move right to select values, but since all the wheels to the
    //      right of P are at position zero, there are no swaps to be done.
    //
    // So ... There are two data structures involved.  One is an "odometer"
    // with decreasing wheel size (from N to zero).  Turning the odometers in
    // this 'd_wheel' array directs the swapping of values in the 'd_val'
    // array.  'd_val[]' contains every value from '[0 .. N)'.  (These could be
    // letters, names, colors, foods, whatever.)  The swaps and wheel advances
    // are carried out as above.
    //
    // As coded, 'Permuter' does NOT provide a stable end-of-sequence
    // indication.

    unsigned d_wheel[N]; // The 'odometer position' of the wheels
    unsigned d_val[N];   // The values array.  Each value in [ 0 .. N ) must
                         // occur exactly once.

    Permuter()
    {
        for (int i = 0; i < (int) N; ++i)
        {
            d_wheel[i] = 0;
            d_val[i] = i;
        }
    }

    unsigned int operator[](int i) const
    {
        return d_val[i];
    }

    int advance()
        // Returns true if the movement of the permuter that it has just
        // executed has NOT returned the permuter to the starting position,
        // false otherwise.  (In other words, keep going while 'advance()'
        // returns true.)
    {
        for (int n = N - 1 - 1; n >= 0; --n)
        {
            if (adv_wh(n)) {
                return true;                                          // RETURN
            }
        }
        return false;
    }

    ostream& print(ostream&) const;

    // Advance wheel N one position.  Return false if it has returned to its
    // original position (position zero).
    //
    //   With wheel n in position zero, there is a value V at that position.
    //   As it advances upwards, that value advances upwards, and the value
    //   it replaces (at n + wheel[n]) replaces it at n.  This is restored
    //   before we make the next advance.  Also, once we advance a wheel, we
    //   don't advance it again until everything at and above its position has
    //   returned to the state in which that wheel was last advanced.

    int adv_wh(int n)
    {
        exch(n, n + d_wheel[n]);
        d_wheel[n] = static_cast<unsigned>((d_wheel[n] + 1) % (N - n));
        exch(n, n + d_wheel[n]);

        return 0 != d_wheel[n];
    }

    void exch(int i, int j)  // Exchange the values at 'val[i]' and 'val[j]'
    {
        int t = d_val[i];
        d_val[i] = d_val[j];
        d_val[j] = t;
    }
};

template<bsl::size_t N>
inline
ostream& operator<<(ostream& os, const Permuter<N>& p)
{
    return p.print(os);
}

//  'u8OneByteCases', 'u8TwoByteCases', 'u8ThreeByteHdrCases',
//  u8FourByteHdrCases', and u8ContinByteCases' provide selected instances of
//  various UTF-8 octet types for building UTF-8 code sequences.  They are used
//  by several tests.  The enums 'THREE_BYTE_ZERO_NEEDS',
//  'FOUR_BYTE_ZERO_NEEDS', and 'FOUR_BYTE_ZERO_MAX' are also provided (and
//  explained) in this group.

const
unsigned char u8OneByteCases[] ={ '\x1',   '\x3',  '\x7',  '\xf',
                                  '\x1e', '\x3c', '\x78', '\x7f', };

const
unsigned char u8TwoByteHdrCases[] ={ '\x2',  '\x3',  '\x7',
                                     '\xe',  '\x18', '\x1a',
                                     '\x10', '\x15', '\x1b' };
const
unsigned char u8ThreeByteHdrCases[] ={ '\x0', '\x1', '\x3', '\x6',
                                       '\x8', '\x9', '\xc', '\xf', };
   // The Three Byte cases must avoid creating a value in the reserved range
   // of 0xd800 to 0xdfff.  The four bits contributed by the header represent
   // the high-order hex digit; we simply exclude 0xd, thereby avoiding the
   // whole range.  (We avoid some non-reserved values as well, but this is
   // only selective coverage, to test that the function calls and calling
   // sequences are well-behaved.)

enum { THREE_BYTE_ZERO_NEEDS = 0x20 };
    // If the first continuation is at least this, the content part of the
    // three-byte header may be zero.

const
unsigned char u8FourByteHdrCases[] ={ '\x0', '\x1', '\x2', '\x3', '\x4' };
    // The Four-byte header can hold three bits, but for iso10646 the maximum
    // value is four, and at that maximum the first continuation octet's
    // content must be less than 0x10.

const
unsigned char u8InvalidFourByteHdrCases[] ={ '\x5', '\x6', '\x7', };

enum { FOUR_BYTE_ZERO_NEEDS = 0x10 };
    // If the first continuation is at least this, the content part of the
    // four-byte header may be zero.

enum { FOUR_BYTE_FOUR_MAX = 0xf };
    // If the first continuation is no greater than this, the content part of
    // the four-byte header may be two.

const
unsigned char u8ContinByteCases[] ={ '\x0',  '\x1',  '\x3',
                                     '\x7',  '\xf',  '\x1e',
                                     '\x3a', '\x3c', '\x3f', };

const
unsigned char u8ContinValidFourByteMaxCases[] ={ '\x0',  '\x1',  '\x3',
                                                 '\x7',  '\xf', };

const
unsigned char u8ContinInvalidFourByteMaxCases[] ={ '\x10', '\x1e', '\x3c',
                                                   '\x3a', '\x3f', };

const
unsigned char u8ReservedRangeLowerContin[] ={ '\x20', '\x21', '\x22', '\x23',
                                              '\x24', '\x25', '\x26', '\x27',
                                              '\x28', '\x29', '\x2a', '\x2b',
                                              '\x2c', '\x2d', '\x2e', '\x2f',
                                            };
    // Used as the first continuation octet content with 0xd in the three-
    // octet header, these will produce code points in the (forbidden) lower
    // reserved range.

const
unsigned char u8ReservedRangeUpperContin[] ={ '\x30', '\x31', '\x32', '\x33',
                                              '\x34', '\x35', '\x36', '\x37',
                                              '\x38', '\x39', '\x3a', '\x3b',
                                              '\x3c', '\x3d', '\x3e', '\x3f',
                                            };
    // Used as the first continuation octet content with 0xd in the three-
    // octet header, these will produce code points in the (forbidden) upper
    // reserved range.

const
unsigned short u16UpperAndLower[] ={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x06,
                                     0x07, 0x08, 0x0c, 0x0d, 0x10, 0x18,
                                     0x1c, 0x20, 0x30, 0x38, 0x40, 0x60,
                                     0x70, 0x80, 0xc0, 0xd0, 0x100, 0x180,
                                     0x1c0, 0x200, 0x300, 0x380, 0x3ff,
                                   };
    // Used as the content part of the upper and lower words of 2-word UTF-16
    // code points.

typedef ArrayRange<const unsigned char> AvCharList;
    // 'AvCharList' provides an stl-like iterator to walk the lists of octet
    // contents for various tests.  (Using 'unsigned char' avoids sign
    // extension problems.)

typedef ArrayRange<const unsigned short> avWordList;
    // 'avWordList' provides an stl-like iterator to walk the lists of 16-bit
    // word contents for various tests.

namespace {
    // The test number and the verbosity level are out here so that they are
    // visible to all functions.  They are set at the beginning of 'main()'.
    int test;           // The number of the test being executed (from argv)
    int verbose;        // Nonzero iff one or more args after the test number
    int veryVerbose;    // Nonzero iff two or more args after the test number
    int veryVeryVerbose;        // " " three or more args after the test number
    int veryVeryVeryVerbose;    // " " four or more args after the test number
    int veryVeryVeryVeryVerbose;    // " " five or more args after the test no.
}  // close unnamed namespace

template<class TO_CHAR, class FROM_CHAR, class FILL_CHECK>
bool testOneErrorCharConversion(int                          line,
                                ArrayRange<TO_CHAR> const&   toBuf,
                                ArrayRange<FROM_CHAR> const& fromBuf,
                                FILL_CHECK&                  fillCheck);
    //: o 'line': '__LINE__' where this function is invoked
    //: o 'toBuf', fromBuf: Workspaces provided by our caller.  We depend on
    //:   the value of 'fromBuf.size()'
    //: o 'fillCheck': Source of the octet or word sequence under test.
    //
    // The 'testOneErrorCharConversion' function is the common part of all the
    // subtests in test 4.  It verifies that the error conversion occurs as
    // expected (using the 'RUN_AND_CHECK' macro, which also verifies that the
    // data surrounding the output buffer are unchanged) and that the source
    // buffer is unchanged.  It performs this verification for the error
    // sequence alone in a string and for the sequence surrounded by two
    // single-octet code points, and with and without error replacement
    // code points (a total of four tests).  It returns 'true' if all the tests
    // succeed, or 'false' if any test or tests fail.

template<class TO_CHAR,
         class TO_FILL_CHECK,
         class FR_CHAR,
         class FR_FILL_CHECK>
bool oneStringConversion(int                      line,
                         BufferedWPiece<TO_CHAR>& toBuf,
                         TO_FILL_CHECK&           toFillCheck,
                         ArrayRange<FR_CHAR>&     fromBuf,
                         FR_FILL_CHECK&           fromFillCheck,
                         const ConvRslt&          expected);
    //: o 'line': '__LINE__' of this call
    //: o 'toBuf': Destination workspace
    //: o 'toFillCheck': Reference for checking the output string.
    //: o 'fromBuf': Source workspace buffer
    //: o 'fromFillCheck': Source and reference for checking the input string.
    //: o 'expected': The expected set of return values from the conversion
    //:   function.
    //
    // The 'oneStringConversion' templated function invokes the conversions for
    // test 5.  It verifies that the conversion returns with the expected
    // values (using the 'RUN_AND_CHECK' macro, which also verifies that the
    // data surrounding the output buffer are unchanged) and that the source
    // buffer is unchanged.  It verifies that the output buffer contains the
    // expected result.  It returns 'true' if all the tests succeed, or 'false'
    // if any have failed.

template <class UTF16_CHAR, unsigned UTF16_CHAR_SIZE>
bool swappedEquals(UTF16_CHAR lhs, UTF16_CHAR rhs);

template <>
bool swappedEquals<unsigned short, 2>(unsigned short lhs, unsigned short rhs)
{
    return rhs == (((lhs & 0xff) << 8) | ((lhs & 0xff00) >> 8));
}

template <>
bool swappedEquals<wchar_t, 2>(wchar_t lhs, wchar_t rhs)
{
    return rhs == (((lhs & 0xff) << 8) | ((lhs & 0xff00) >> 8));
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <>
bool swappedEquals<char16_t, 2>(char16_t lhs, char16_t rhs)
{
    return rhs == (((lhs & 0xff) << 8) | ((lhs & 0xff00) >> 8));
}
#endif

template <>
bool swappedEquals<wchar_t, 4>(wchar_t lhs, wchar_t rhs)
{
    return static_cast<unsigned int>(rhs) ==
           (((lhs & 0xff)     << 24) |                 ((lhs & 0xff00) << 8) |
            ((lhs & 0xff0000) >>  8) | (static_cast<unsigned int>(lhs) >> 24));
}

template <class UTF16_CHAR>
bool swappedRangeEquals(const UTF16_CHAR *lhsBegin,
                        const UTF16_CHAR *lhsEnd,
                        const UTF16_CHAR *rhsBegin,
                        const UTF16_CHAR *rhsEnd)
{
    if (lhsEnd - lhsBegin != rhsEnd - rhsBegin) {
        return false;                                                 // RETURN
    }

    for (; lhsBegin < lhsEnd; ++lhsBegin, ++rhsBegin) {
        if (!swappedEquals<UTF16_CHAR, sizeof(UTF16_CHAR)>(*lhsBegin,
                                                           *rhsBegin)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

// Functions to get pointers to begin and end of containers.  The normal
// 'begin' and 'end' member functions may return some iterator type that isn't
// really a pointer.


char *sBegin(bsl::string& s)
{
    return &s[0];
}

wchar_t *sBegin(bsl::wstring& s)
{
    return &s[0];
}

char *sEnd(bsl::string& s)
{
    return &s[0] + s.length() + 1;
}

wchar_t *sEnd(bsl::wstring& s)
{
    return &s[0] + s.length() + 1;
}

template <class CHAR_TYPE>
CHAR_TYPE *vBegin(bsl::vector<CHAR_TYPE>& v)
{
    return &v.front();
}

template <class CHAR_TYPE>
CHAR_TYPE *vEnd(  bsl::vector<CHAR_TYPE>& v)
{
    return &v.front() + v.size();
}

template <class UTF16_CHAR, bsl::size_t UTF16_CHAR_SIZE = sizeof(UTF16_CHAR)>
struct SwapInPlace_Helper;

template <>
struct SwapInPlace_Helper<wchar_t, 2> {
    void operator()(wchar_t *word)
    {
        *word = static_cast<wchar_t>(
                (static_cast<unsigned short>(*word) >> 8) |
                ((*word & 0xff) << 8));
    }
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <>
struct SwapInPlace_Helper<char16_t, 2> {
    void operator()(char16_t *word)
    {
        *word = static_cast<char16_t>(
                (static_cast<unsigned short>(*word) >> 8) |
                ((*word & 0xff) << 8));
    }
};
#endif

template <>
struct SwapInPlace_Helper<wchar_t, 4> {
    void operator()(wchar_t *word)
    {
        const wchar_t w = *word;

        *word = static_cast<wchar_t>(
                (static_cast<unsigned int>(w) >> 24) |
                ((w & 0xff0000) >> 8) |
                ((w &   0xff00) << 8) |
                ((w &     0xff) << 24));
    }
};



template <class UTF16_CHAR>
void swapInPlace(UTF16_CHAR *word)
{
    SwapInPlace_Helper<UTF16_CHAR>()(word);
}

//-----------------------------------------------------------------------------
// The following is a sample of Multilingual UTF-8.  It is an amalgamation of
// prose in Chinese, Hindi, French, and Greek.
//     It was discovered that none of this natural language sample contained
// any four byte UTF-8 encodings, so several were added on the end by hand.
//-----------------------------------------------------------------------------

unsigned char utf8MultiLang[] = {
    239, 187, 191, 'C', 'h', 'i', 'n', 'e', 's', 'e', ':',  13,
     10,  13,  10, 228, 184, 173, 229, 141, 142, 228, 186, 186,
    230, 176, 145, 229, 133, 177, 229, 146, 140, 229, 155, 189,
    239, 188, 140, 233, 128, 154, 231, 167, 176, 228, 184, 173,
    229, 155, 189, '[', 230, 179, 168, ' ', '3', ']', 239, 188,
    140, 230, 152, 175, 228, 189, 141, 230, 150, 188, 228, 186,
    154, 230, 180, 178, 230, 157, 177, 233, 131, 168, 227, 128,
    129, 229, 164, 170, 229, 185, 179, 230, 180, 139, 232, 165,
    191, 229, 178, 184, 231, 154, 132, 228, 184, 128, 228, 184,
    170, 231, 164, 190, 228, 188, 154, 228, 184, 187, 228, 185,
    137, 229, 155, 189, 229, 174, 182, 227, 128, 130, 233, 166,
    150, 233, 131, 189, 231, 130, 186, 229, 140, 151, 228, 186,
    172, 227, 128, 130, 229, 133, 182, 233, 153, 134, 229, 156,
    176, 231, 150, 134, 229, 159, 159, 232, 136, 135, 229, 145,
    168, 233, 130, 138, '1', '4', 229, 128, 139, 229, 156, 139,
    229, 174, 182, 230, 142, 165, 229, 163, 164, 239, 188, 140,
    233, 153, 134, 229, 156, 176, 229, 143, 138, 230, 185, 150,
    230, 179, 138, 231, 154, 132, 230, 128, 187, 233, 157, 162,
    231, 169, 141, 231, 186, 166, '9', '6', '0', 232, 144, 172,
    229, 185, 179, 230, 150, 185, 229, 133, 172, 233, 135, 140,
    '[', '1', '1', ']', '[', '1', '2', ']', '[', '1', '3', ']',
    239, 188, 140, 230, 152, 175, 229, 133, 168, 228, 184, 150,
    231, 149, 140, 233, 153, 134, 229, 156, 176, 233, 157, 162,
    231, 167, 175, 231, 172, 172, 228, 186, 140, 229, 164, 167,
    231, 154, 132, 229, 155, 189, 229, 174, 182, 239, 188, 140,
    230, 128, 187, 233, 157, 162, 231, 167, 175, 231, 172, 172,
    228, 184, 137, 230, 136, 150, 231, 172, 172, 229, 155, 155,
    229, 164, 167, 231, 154, 132, 229, 155, 189, 229, 174, 182,
    227, 128, 130, 229, 133, 182, 228, 186, 186, 229, 143, 163,
    232, 182, 133, 233, 129, 142, '1', '3', 229, 132, 132, 239,
    188, 140, 231, 180, 132, 228, 189, 148, 229, 133, 168, 231,
    144, 131, 228, 186, 186, 229, 143, 163, 231, 154, 132, 228,
    186, 148, 229, 136, 134, 228, 185, 139, 228, 184, 128, 239,
    188, 140, 230, 152, 175, 228, 184, 150, 231, 149, 140, 228,
    184, 138, 228, 186, 186, 229, 143, 163, 230, 156, 128, 229,
    164, 154, 231, 154, 132, 229, 156, 139, 229, 174, 182, 227,
    128, 130,  13,  10,  13,  10, 228, 189, 156, 231, 130, 186,
    231, 164, 190, 228, 188, 154, 228, 184, 187, 228, 185, 137,
    229, 155, 189, 229, 174, 182, 239, 188, 140, 228, 184, 173,
    232, 143, 175, 228, 186, 186, 230, 176, 145, 229, 133, 177,
    229, 146, 140, 229, 156, 139, 228, 187, 165, 233, 169, 172,
    229, 133, 139, 230, 128, 157, 229, 136, 151, 229, 174, 129,
    228, 184, 187, 228, 185, 137, 231, 130, 186, 230, 132, 143,
    232, 173, 152, 229, 189, 162, 230, 133, 139, 239, 188, 140,
    228, 190, 157, 228, 184, 173, 229, 156, 139, 231, 137, 185,
    232, 137, 178, 231, 164, 190, 230, 156, 131, 228, 184, 187,
    231, 190, 169, 231, 144, 134, 232, 174, 186, 230, 140, 135,
    229, 176, 142, 230, 148, 191, 228, 186, 139, 239, 188, 140,
    229, 185, 182, 231, 148, 177, 230, 134, 178, 230, 179, 149,
    230, 137, 128, 232, 179, 166, 228, 186, 136, 228, 184, 173,
    229, 155, 189, 229, 133, 177, 228, 186, 167, 229, 133, 154,
    229, 159, 183, 230, 148, 191, 239, 188, 140, 229, 174, 158,
    232, 161, 140, 228, 184, 173, 229, 155, 189, 229, 133, 177,
    228, 186, 167, 229, 133, 154, 233, 162, 134, 229, 175, 188,
    231, 154, 132, 229, 164, 154, 229, 133, 154, 229, 144, 136,
    228, 189, 156, 229, 146, 140, 230, 148, 191, 230, 178, 187,
    229, 141, 143, 229, 149, 134, 229, 136, 182, 229, 186, 166,
    '[', '1', '4', ']', 227, 128, 130, '1', '9', '4', '9', 229,
    185, 180, '1', '0', 230, 156, 136, '1', 230, 151, 165, 231,
    154, 132, 229, 188, 128, 229, 155, 189, 229, 164, 167, 229,
    133, 184, 228, 184, 173, 239, 188, 140, 228, 184, 173, 229,
    141, 142, 228, 186, 186, 230, 176, 145, 229, 133, 177, 229,
    146, 140, 229, 155, 189, 228, 184, 173, 229, 164, 174, 228,
    186, 186, 230, 176, 145, 230, 148, 191, 229, 186, 156, 230,
    173, 163, 229, 188, 143, 229, 174, 163, 229, 145, 138, 230,
    136, 144, 231, 171, 139, '[', 230, 179, 168, ' ', '4', ']',
    227, 128, 130, 229, 133, 168, 229, 156, 139, 229, 138, 131,
    229, 136, 134, 231, 130, 186, '2', '3', 229, 128, 139, 231,
    156, 129, 239, 188, 136, 229, 133, 182, 228, 184, 173, 229,
    185, 182, 230, 178, 161, 230, 156, 137, 229, 175, 185, 229,
    143, 176, 230, 185, 190, 231, 156, 129, 229, 133, 168, 233,
    131, 168, 228, 184, 142, 231, 166, 143, 229, 187, 186, 231,
    156, 129, 227, 128, 129, 230, 181, 183, 229, 141, 151, 231,
    156, 129, 233, 131, 168, 229, 136, 134, 229, 156, 176, 229,
    140, 186, 229, 174, 158, 233, 153, 133, 231, 174, 161, 232,
    190, 150, 239, 188, 137, 227, 128, 129, '5', 229, 128, 139,
    232, 135, 170, 230, 178, 187, 229, 141, 128, 227, 128, 129,
    '4', 229, 128, 139, 231, 155, 180, 232, 190, 150, 229, 184,
    130, 229, 146, 140, '2', 229, 128, 139, 231, 137, 185, 229,
    136, 165, 232, 161, 140, 230, 148, 191, 229, 140, 186, 239,
    188, 136, 229, 141, 179, 233, 166, 153, 230, 184, 175, 232,
    136, 135, 230, 190, 179, 233, 150, 128, 239, 188, 137, 239,
    188, 140, 231, 156, 129, 231, 186, 167, 228, 186, 186, 230,
    176, 145, 230, 148, 191, 229, 186, 156, 229, 143, 151, 229,
    155, 189, 229, 138, 161, 233, 153, 162, 233, 162, 134, 229,
    175, 188, 239, 188, 140, 231, 137, 185, 229, 136, 165, 232,
    161, 140, 230, 148, 191, 229, 141, 128, 229, 137, 135, 230,
    160, 185, 230, 147, 154, 228, 184, 128, 229, 156, 139, 229,
    133, 169, 229, 136, 182, 230, 148, 191, 231, 173, 150, 229,
    175, 166, 232, 161, 140, 233, 171, 152, 229, 186, 166, 232,
    135, 170, 230, 178, 187, 227, 128, 130, 229, 133, 168, 229,
    155, 189, 232, 183, 168, 232, 182, 138, 228, 186, 148, 228,
    184, 170, 229, 156, 176, 231, 144, 134, 230, 151, 182, 229,
    140, 186, 239, 188, 140, 228, 189, 134, 229, 157, 135, 228,
    189, 191, 231, 148, 168, 228, 184, 173, 229, 156, 139, 230,
    168, 153, 230, 186, 150, 230, 153, 130, 233, 150, 147, 239,
    188, 136, 229, 141, 179, 'U', 'T', 'C', '+', '8', 239, 188,
    137, 227, 128, 130,  13,  10,  13,  10, 228, 184, 173, 232,
    143, 175, 228, 186, 186, 230, 176, 145, 229, 133, 177, 229,
    146, 140, 229, 156, 139, 230, 152, 175, 229, 164, 154, 230,
    176, 145, 230, 151, 143, 229, 155, 189, 229, 174, 182, 239,
    188, 140, 229, 133, 182, 228, 184, 173, 230, 177, 137, 230,
    151, 143, 228, 189, 148, 231, 184, 189, 228, 186, 186, 229,
    143, 163, 231, 154, 132, '9', '1', '.', '5', '9', '%', 239,
    188, 140, 229, 133, 182, 233, 164, 152, '5', '5', 228, 184,
    170, 230, 176, 145, 230, 151, 143, 231, 130, 186, 229, 176,
    145, 230, 149, 176, 230, 176, 145, 230, 151, 143, 239, 188,
    140, 229, 155, 189, 229, 174, 182, 232, 170, 141, 229, 174,
    154, 231, 154, 132, '5', '6', 229, 128, 139, 230, 176, 145,
    230, 151, 143, 229, 144, 136, 231, 167, 176, 226, 128, 156,
    228, 184, 173, 229, 141, 142, 230, 176, 145, 230, 151, 143,
    226, 128, 157, 227, 128, 130, 228, 184, 173, 229, 141, 142,
    228, 186, 186, 230, 176, 145, 229, 133, 177, 229, 146, 140,
    229, 155, 189, 230, 156, 137, '2', '4', 231, 167, 141, 230,
    176, 145, 230, 151, 143, 230, 150, 135, 229, 173, 151, 239,
    188, 140, 229, 133, 171, 229, 141, 129, 229, 164, 154, 231,
    167, 141, 230, 176, 145, 230, 151, 143, 232, 175, 173, 232,
    168, 128, 227, 128, 130, 228, 184, 173, 229, 141, 142, 228,
    186, 186, 230, 176, 145, 229, 133, 177, 229, 146, 140, 229,
    155, 189, 230, 178, 161, 230, 156, 137, 230, 152, 142, 231,
    161, 174, 232, 167, 132, 229, 174, 154, 231, 154, 132, 229,
    155, 189, 229, 174, 182, 232, 175, 173, 232, 168, 128, 239,
    188, 140, 228, 187, 165, 230, 177, 137, 232, 175, 173, 230,
    153, 174, 233, 128, 154, 232, 175, 157, 229, 146, 140, 232,
    167, 132, 232, 140, 131, 231, 174, 128, 229, 140, 150, 230,
    177, 137, 229, 173, 151, 228, 184, 186, 226, 128, 156, 229,
    155, 189, 229, 174, 182, 233, 128, 154, 231, 148, 168, 232,
    175, 173, 232, 168, 128, 230, 150, 135, 229, 173, 151, 226,
    128, 157, '[', 230, 179, 168, ' ', '5', ']', 227, 128, 130,
    228, 184, 173, 229, 155, 189, 228, 188, 160, 231, 187, 159,
    228, 184, 138, 230, 152, 175, 228, 187, 165, 231, 165, 150,
    229, 133, 136, 228, 191, 161, 228, 187, 176, 228, 184, 186,
    228, 184, 187, 231, 154, 132, 229, 155, 189, 229, 174, 182,
    239, 188, 140, 229, 185, 182, 229, 133, 183, 230, 156, 137,
    229, 132, 146, 233, 135, 138, 233, 129, 147, 228, 184, 137,
    230, 149, 153, 229, 144, 136, 230, 181, 129, 231, 154, 132,
    229, 174, 151, 230, 149, 153, 228, 191, 161, 228, 187, 176,
    228, 188, 160, 231, 187, 159, 229, 146, 140, 231, 137, 185,
    231, 130, 185, 239, 188, 140, 229, 144, 140, 230, 151, 182,
    229, 173, 152, 229, 156, 168, 229, 133, 182, 229, 174, 131,
    229, 164, 154, 231, 167, 141, 229, 174, 151, 230, 149, 153,
    227, 128, 130, 228, 184, 173, 229, 141, 142, 228, 186, 186,
    230, 176, 145, 229, 133, 177, 229, 146, 140, 229, 155, 189,
    229, 144, 142, 239, 188, 140, 229, 174, 152, 230, 150, 185,
    229, 165, 137, 232, 161, 140, 230, 151, 160, 231, 165, 158,
    232, 174, 186, 239, 188, 140, 229, 133, 182, 229, 144, 142,
    230, 155, 190, 229, 143, 145, 229, 138, 168, 231, 154, 132,
    230, 150, 135, 229, 140, 150, 229, 164, 167, 233, 157, 169,
    229, 145, 189, 229, 175, 185, 229, 144, 132, 231, 167, 141,
    229, 174, 151, 230, 149, 153, 233, 128, 160, 230, 136, 144,
    228, 184, 165, 233, 135, 141, 231, 160, 180, 229, 157, 143,
    239, 188, 140, 231, 155, 180, 229, 136, 176, 230, 148, 185,
    233, 157, 169, 229, 188, 128, 230, 148, 190, 229, 144, 142,
    230, 137, 141, 230, 156, 137, 230, 137, 128, 232, 189, 172,
    229, 143, 152, 227, 128, 130, 229, 189, 147, 228, 187, 138,
    228, 184, 173, 229, 155, 189, 230, 148, 191, 229, 186, 156,
    229, 175, 185, 229, 174, 151, 230, 149, 153, 228, 184, 142,
    228, 188, 160, 231, 187, 159, 228, 186, 139, 231, 137, 169,
    233, 135, 135, 229, 143, 150, 228, 191, 157, 230, 138, 164,
    231, 154, 132, 230, 128, 129, 229, 186, 166, 227, 128, 130,
     13,  10,  13,  10, 228, 184, 173, 229, 141, 142, 228, 186,
    186, 230, 176, 145, 229, 133, 177, 229, 146, 140, 229, 155,
    189, 230, 152, 175, 229, 155, 189, 233, 153, 133, 231, 164,
    190, 228, 188, 154, 231, 154, 132, 233, 135, 141, 232, 166,
    129, 228, 184, 128, 229, 145, 152, 239, 188, 140, 228, 185,
    159, 230, 152, 175, 228, 188, 151, 229, 164, 154, 230, 173,
    163, 229, 188, 143, 229, 146, 140, 233, 157, 158, 230, 173,
    163, 229, 188, 143, 231, 154, 132, 229, 164, 154, 232, 190,
    185, 231, 187, 132, 231, 187, 135, 231, 154, 132, 230, 136,
    144, 229, 145, 152, 239, 188, 140, 229, 140, 133, 230, 139,
    172, 232, 129, 148, 229, 144, 136, 229, 155, 189, 227, 128,
    129, 228, 184, 150, 231, 149, 140, 232, 180, 184, 230, 152,
    147, 231, 187, 132, 231, 187, 135, 227, 128, 129, 228, 186,
    154, 229, 164, 170, 231, 187, 143, 229, 144, 136, 231, 187,
    132, 231, 187, 135, 227, 128, 129, 233, 135, 145, 231, 160,
    150, 229, 155, 155, 229, 155, 189, 227, 128, 129, 228, 184,
    138, 230, 181, 183, 229, 144, 136, 228, 189, 156, 231, 187,
    132, 231, 187, 135, 229, 146, 140, '2', '0', 229, 155, 189,
    233, 155, 134, 229, 155, 162, 231, 173, 137, 239, 188, 140,
    228, 184, 186, 232, 129, 148, 229, 144, 136, 229, 155, 189,
    229, 174, 137, 229, 133, 168, 231, 144, 134, 228, 186, 139,
    228, 188, 154, 229, 184, 184, 228, 187, 187, 231, 144, 134,
    228, 186, 139, 229, 155, 189, 227, 128, 129, 228, 184, 150,
    231, 149, 140, 231, 172, 172, 228, 186, 140, 229, 164, 167,
    231, 187, 143, 230, 181, 142, 228, 189, 147, 239, 188, 140,
    230, 152, 175, 228, 184, 150, 231, 149, 140, 231, 172, 172,
    228, 184, 128, 229, 164, 167, 229, 135, 186, 229, 143, 163,
    229, 156, 139, 227, 128, 129, 228, 184, 150, 231, 149, 140,
    231, 172, 172, 228, 186, 140, 229, 164, 167, 233, 128, 178,
    229, 143, 163, 229, 156, 139, 239, 188, 140, 230, 147, 129,
    230, 156, 137, 230, 156, 128, 229, 164, 154, 231, 154, 132,
    229, 164, 150, 230, 177, 135, 229, 132, 178, 229, 130, 153,
    239, 188, 140, 230, 156, 128, 228, 184, 176, 229, 175, 140,
    231, 154, 132, 228, 184, 150, 231, 149, 140, 230, 150, 135,
    229, 140, 150, 233, 129, 151, 228, 186, 167, 239, 188, 140,
    228, 186, 166, 230, 152, 175, 228, 184, 150, 231, 149, 140,
    228, 184, 138, 231, 187, 143, 230, 181, 142, 230, 136, 144,
    233, 149, 183, 230, 156, 128, 229, 191, 171, 231, 154, 132,
    229, 156, 139, 229, 174, 182, 228, 185, 139, 228, 184, 128,
    227, 128, 130, 229, 143, 166, 229, 164, 150, 239, 188, 140,
    228, 184, 173, 229, 155, 189, 230, 139, 165, 230, 156, 137,
    228, 184, 150, 231, 149, 140, 228, 184, 138, 231, 142, 176,
    229, 189, 185, 229, 163, 171, 229, 133, 181, 230, 156, 128,
    229, 164, 154, 231, 154, 132, 229, 134, 155, 233, 152, 159,
    239, 188, 155, 229, 134, 155, 228, 186, 139, 229, 188, 128,
    230, 148, 175, 228, 184, 150, 231, 149, 140, 231, 172, 172,
    228, 186, 140, 239, 188, 140, 230, 139, 165, 230, 156, 137,
    230, 160, 184, 230, 173, 166, 229, 153, 168, 239, 188, 140,
    229, 185, 182, 229, 133, 183, 229, 164, 135, 229, 143, 145,
    229, 176, 132, 229, 141, 171, 230, 152, 159, 227, 128, 129,
    232, 175, 149, 233, 170, 140, 229, 158, 139, 231, 169, 186,
    233, 151, 180, 231, 171, 153, 229, 146, 140, 230, 156, 136,
    231, 144, 131, 229, 143, 138, 230, 183, 177, 231, 169, 186,
    230, 142, 162, 230, 181, 139, 229, 153, 168, 231, 154, 132,
    232, 131, 189, 229, 138, 155, 239, 188, 155, '2', '0', '0',
    '3', 229, 185, 180, 239, 188, 140, 228, 184, 173, 229, 155,
    189, 230, 136, 144, 228, 184, 186, 228, 184, 150, 231, 149,
    140, 231, 172, 172, 228, 184, 137, 228, 184, 170, 232, 135,
    170, 228, 184, 187, 230, 136, 144, 229, 138, 159, 229, 143,
    145, 229, 176, 132, 232, 189, 189, 228, 186, 186, 232, 136,
    170, 229, 164, 169, 229, 153, 168, 231, 154, 132, 229, 155,
    189, 229, 174, 182, 227, 128, 130, 228, 184, 173, 229, 155,
    189, 228, 186, 166, 230, 152, 175, 230, 189, 156, 229, 156,
    168, 232, 182, 133, 231, 186, 167, 229, 164, 167, 229, 155,
    189, 228, 185, 139, 228, 184, 128, 239, 188, 140, 232, 162,
    171, 232, 174, 164, 228, 184, 186, 230, 152, 175, 228, 184,
    139, 228, 184, 128, 228, 189, 141, 232, 182, 133, 231, 186,
    167, 229, 164, 167, 229, 155, 189, 231, 154, 132, 230, 156,
    137, 229, 138, 155, 229, 128, 153, 233, 128, 137, 228, 186,
    186, 227, 128, 130,  13,  10,  13,  10, 228, 184, 173, 229,
    141, 142, 228, 186, 186, 230, 176, 145, 229, 133, 177, 229,
    146, 140, 229, 155, 189, 231, 154, 132, 230, 173, 163, 229,
    188, 143, 229, 155, 189, 229, 144, 141, 228, 186, 142, '1',
    '9', '4', '9', 229, 185, 180, 231, 148, 177, 228, 184, 173,
    229, 156, 139, 228, 186, 186, 230, 176, 145, 230, 148, 191,
    230, 178, 187, 229, 141, 148, 229, 149, 134, 230, 156, 131,
    232, 173, 176, 231, 177, 140, 229, 130, 153, 230, 156, 131,
    232, 173, 176, 231, 162, 186, 229, 174, 154, 239, 188, 140,
    229, 189, 147, 229, 136, 157, 230, 155, 190, 229, 138, 160,
    232, 168, 187, 227, 128, 140, 231, 176, 161, 231, 168, 177,
    239, 188, 154, 228, 184, 173, 232, 143, 175, 230, 176, 145,
    229, 156, 139, 227, 128, 141, 239, 188, 140, 228, 189, 134,
    229, 143, 184, 229, 190, 146, 231, 190, 142, 229, 160, 130,
    231, 173, 137, 230, 176, 145, 228, 184, 187, 229, 133, 154,
    230, 180, 190, 228, 186, 186, 229, 163, 171, 232, 174, 164,
    228, 184, 186, 230, 150, 176, 228, 184, 173, 229, 155, 189,
    229, 186, 148, 231, 161, 174, 231, 171, 139, 230, 150, 176,
    229, 155, 189, 229, 144, 141, 239, 188, 140, 228, 187, 165,
    231, 164, 186, 228, 184, 164, 230, 172, 161, 233, 157, 169,
    229, 145, 189, 231, 154, 132, 230, 160, 185, 230, 156, 172,
    230, 132, 143, 228, 185, 137, 228, 184, 141, 229, 144, 140,
    '[', '1', '5', ']', 227, 128, 130, 229, 155, 160, 230, 173,
    164, 231, 155, 180, 232, 135, 179, '9', 230, 156, 136, '2',
    '7', 230, 151, 165, 230, 148, 191, 229, 141, 148, 229, 133,
    168, 233, 171, 148, 230, 156, 131, 232, 173, 176, 232, 161,
    168, 230, 177, 186, 233, 128, 154, 233, 129, 142, 228, 184,
    173, 229, 164, 174, 228, 186, 186, 230, 176, 145, 230, 148,
    191, 229, 186, 156, 231, 181, 132, 231, 185, 148, 230, 179,
    149, 230, 153, 130, 239, 188, 140, 230, 173, 163, 229, 188,
    143, 232, 173, 176, 230, 177, 186, 229, 142, 187, 233, 153,
    164, 230, 173, 164, 229, 138, 160, 232, 168, 187, '[', '1',
    '6', ']', 227, 128, 130,  13,  10,  13,  10, 229, 156, 168,
    229, 133, 168, 233, 131, 168, 229, 156, 139, 233, 154, 155,
    229, 160, 180, 229, 144, 136, 239, 188, 140, 228, 184, 173,
    232, 143, 175, 228, 186, 186, 230, 176, 145, 229, 133, 177,
    229, 146, 140, 229, 156, 139, 228, 184, 128, 232, 136, 172,
    231, 176, 161, 231, 168, 177, 231, 130, 186, 228, 184, 173,
    229, 156, 139, 239, 188, 140, 230, 156, 137, 230, 151, 182,
    229, 128, 153, 228, 185, 159, 229, 155, 160, 229, 133, 182,
    230, 137, 128, 229, 164, 132, 229, 156, 176, 231, 144, 134,
    228, 189, 141, 231, 189, 174, 232, 128, 140, 232, 162, 171,
    231, 167, 176, 228, 184, 186, 228, 184, 173, 229, 155, 189,
    229, 164, 167, 233, 153, 134, 227, 128, 130, 229, 156, 168,
    228, 184, 173, 229, 156, 139, 229, 156, 139, 229, 133, 167,
    239, 188, 140, 231, 149, 182, '1', '9', '4', '9', 229, 185,
    180, 229, 137, 141, 231, 154, 132, 228, 184, 173, 232, 143,
    175, 230, 176, 145, 229, 156, 139, 232, 136, 135, '1', '9',
    '4', '9', 229, 185, 180, 229, 190, 140, 231, 154, 132, 228,
    184, 173, 232, 143, 175, 228, 186, 186, 230, 176, 145, 229,
    133, 177, 229, 146, 140, 229, 156, 139, 229, 129, 154, 229,
    176, 141, 230, 175, 148, 230, 136, 150, 230, 156, 137, 230,
    173, 164, 230, 182, 181, 230, 140, 135, 230, 153, 130, 239,
    188, 140, 229, 137, 141, 232, 128, 133, 229, 184, 184, 232,
    162, 171, 231, 168, 177, 231, 130, 186, 232, 136, 138, 228,
    184, 173, 229, 156, 139, 239, 188, 136, 228, 186, 166, 231,
    168, 177, 232, 136, 138, 231, 164, 190, 230, 156, 131, 239,
    188, 137, 239, 188, 140, 232, 128, 140, 229, 190, 140, 232,
    128, 133, 229, 137, 135, 229, 184, 184, 232, 162, 171, 231,
    168, 177, 231, 130, 186, 230, 150, 176, 228, 184, 173, 229,
    156, 139, 227, 128, 130, 231, 155, 174, 229, 137, 141, 239,
    188, 140, 228, 184, 173, 232, 143, 175, 228, 186, 186, 230,
    176, 145, 229, 133, 177, 229, 146, 140, 229, 156, 139, 232,
    170, 141, 231, 130, 186, 228, 184, 173, 232, 143, 175, 230,
    176, 145, 229, 156, 139, 229, 183, 178, 232, 162, 171, 229,
    133, 182, 229, 143, 150, 228, 187, 163, 239, 188, 140, 228,
    184, 173, 232, 143, 175, 230, 176, 145, 229, 156, 139, 230,
    148, 191, 229, 186, 156, 229, 137, 135, 228, 184, 141, 230,
    137, 191, 232, 170, 141, 228, 184, 173, 232, 143, 175, 228,
    186, 186, 230, 176, 145, 229, 133, 177, 229, 146, 140, 229,
    156, 139, 231, 154, 132, 230, 173, 163, 231, 181, 177, 230,
    128, 167, 239, 188, 140, 231, 149, 182, 229, 156, 168, 228,
    184, 173, 229, 156, 139, 229, 164, 167, 233, 153, 184, 231,
    154, 132, 228, 184, 173, 232, 143, 175, 228, 186, 186, 230,
    176, 145, 229, 133, 177, 229, 146, 140, 229, 156, 139, 230,
    148, 191, 229, 186, 156, 232, 136, 135, 229, 156, 168, 229,
    143, 176, 231, 129, 163, 231, 154, 132, 228, 184, 173, 232,
    143, 175, 230, 176, 145, 229, 156, 139, 230, 148, 191, 229,
    186, 156, 229, 129, 154, 229, 176, 141, 230, 175, 148, 230,
    136, 150, 230, 156, 137, 230, 173, 164, 230, 182, 181, 230,
    140, 135, 230, 153, 130, 239, 188, 140, 229, 137, 141, 232,
    128, 133, 229, 184, 184, 232, 162, 171, 229, 190, 140, 232,
    128, 133, 231, 168, 177, 231, 130, 186, 229, 140, 151, 228,
    186, 172, 231, 149, 182, 229, 177, 128, 227, 128, 129, 229,
    164, 167, 233, 153, 184, 231, 149, 182, 229, 177, 128, 227,
    128, 129, 228, 184, 173, 229, 133, 177, 231, 149, 182, 229,
    177, 128, 227, 128, 129, 228, 184, 173, 229, 156, 139, 229,
    164, 167, 233, 153, 184, 230, 136, 150, 229, 164, 167, 233,
    153, 184, '[', '1', '7', ']', 239, 188, 140, 229, 190, 140,
    232, 128, 133, 229, 184, 184, 232, 162, 171, 229, 137, 141,
    232, 128, 133, 231, 168, 177, 231, 130, 186, 229, 143, 176,
    231, 129, 163, 231, 149, 182, 229, 177, 128, 227, 128, 129,
    229, 143, 176, 229, 140, 151, 231, 149, 182, 229, 177, 128,
    230, 136, 150, 229, 143, 176, 231, 129, 163, '[', '1', '8',
    ']', 227, 128, 130, 232, 136, 135, 230, 184, 175, 230, 190,
    179, 229, 156, 176, 229, 141, 128, 228, 184, 166, 231, 148,
    168, 230, 153, 130, 229, 137, 135, 231, 168, 177, 231, 130,
    186, 228, 184, 173, 229, 156, 139, 229, 133, 167, 229, 156,
    176, 227, 128, 129, 229, 133, 167, 229, 156, 176, '[', '1',
    '9', ']', 227, 128, 130,  13,  10,  13,  10, 231, 149, 182,
    228, 184, 173, 229, 156, 139, 229, 164, 167, 233, 153, 184,
    231, 154, 132, 228, 184, 173, 232, 143, 175, 228, 186, 186,
    230, 176, 145, 229, 133, 177, 229, 146, 140, 229, 156, 139,
    230, 148, 191, 229, 186, 156, 232, 136, 135, 229, 156, 168,
    229, 143, 176, 231, 129, 163, 231, 154, 132, 228, 184, 173,
    232, 143, 175, 230, 176, 145, 229, 156, 139, 230, 148, 191,
    229, 186, 156, 229, 129, 154, 229, 176, 141, 230, 175, 148,
    230, 136, 150, 229, 141, 128, 233, 154, 148, 228, 187, 139,
    231, 180, 185, 230, 153, 130, 239, 188, 140, 233, 128, 154,
    229, 184, 184, 230, 142, 161, 231, 148, 168, 229, 156, 176,
    231, 144, 134, 229, 144, 141, 232, 169, 158, 227, 128, 140,
    228, 184, 173, 229, 156, 139, 229, 164, 167, 233, 153, 184,
    227, 128, 141, 239, 188, 136, 'C', 'h', 'i', 'n', 'a', ' ',
    'M', 'a', 'i', 'n', 'l', 'a', 'n', 'd', 239, 188, 137, 230,
    136, 150, 228, 184, 173, 229, 155, 189, 239, 188, 136, 'C',
    'h', 'i', 'n', 'a', 239, 188, 137, 229, 129, 154, 231, 130,
    186, 228, 184, 173, 232, 143, 175, 228, 186, 186, 230, 176,
    145, 229, 133, 177, 229, 146, 140, 229, 156, 139, 231, 154,
    132, 231, 176, 161, 231, 168, 177, 239, 188, 140, 229, 176,
    141, 230, 150, 188, 228, 184, 173, 232, 143, 175, 230, 176,
    145, 229, 156, 139, 229, 137, 135, 231, 176, 161, 231, 168,
    177, 231, 130, 186, 228, 184, 173, 232, 143, 175, 229, 143,
    176, 229, 140, 151, 239, 188, 136, 'C', 'h', 'i', 'n', 'e',
    's', 'e', ' ', 'T', 'a', 'i', 'p', 'e', 'i', 239, 188, 137,
    230, 136, 150, 229, 143, 176, 231, 129, 163, 239, 188, 136,
    'T', 'a', 'i', 'w', 'a', 'n', 239, 188, 137, 227, 128, 130,
    232, 128, 140, 229, 143, 176, 230, 185, 190, 231, 154, 132,
    229, 170, 146, 228, 189, 147, 229, 137, 135, 229, 184, 184,
    228, 189, 191, 231, 148, 168, 227, 128, 140, 228, 184, 173,
    229, 133, 177, 227, 128, 141, 227, 128, 129, 227, 128, 140,
    229, 164, 167, 233, 153, 184, 229, 156, 176, 229, 141, 128,
    227, 128, 141, 227, 128, 129, 227, 128, 140, 229, 164, 167,
    233, 153, 184, 227, 128, 141, 230, 136, 150, 227, 128, 140,
    228, 184, 173, 229, 155, 189, 227, 128, 141, 230, 157, 165,
    228, 189, 156, 231, 130, 186, 228, 184, 173, 232, 143, 175,
    228, 186, 186, 230, 176, 145, 229, 133, 177, 229, 146, 140,
    229, 156, 139, 231, 154, 132, 231, 176, 161, 231, 168, 177,
    227, 128, 130, 233, 166, 153, 230, 184, 175, 233, 131, 168,
    229, 136, 134, 229, 170, 146, 233, 171, 148, 228, 185, 159,
    230, 156, 137, 228, 189, 191, 231, 148, 168, 227, 128, 140,
    228, 184, 173, 229, 156, 139, 227, 128, 141, 229, 146, 140,
    227, 128, 140, 228, 184, 173, 229, 133, 177, 227, 128, 141,
    228, 190, 134, 230, 140, 135, 228, 187, 163, 228, 184, 173,
    229, 156, 139, 229, 164, 167, 233, 153, 184, 227, 128, 130,
     13,  10,  13,  10, '1', '9', '4', '9', 229, 185, 180, 239,
    188, 140, 230, 173, 183, 230, 153, 130, 228, 184, 137, 229,
    185, 180, 231, 154, 132, 229, 156, 139, 229, 133, 177, 229,
    133, 167, 230, 136, 176, 228, 184, 187, 232, 166, 129, 230,
    136, 176, 229, 189, 185, 231, 181, 144, 230, 157, 159, 239,
    188, 140, 228, 184, 173, 229, 156, 139, 229, 133, 177, 231,
    148, 162, 233, 187, 168, 230, 137, 128, 233, 160, 152, 229,
    176, 142, 231, 154, 132, 228, 184, 173, 229, 156, 139, 228,
    186, 186, 230, 176, 145, 232, 167, 163, 230, 148, 190, 232,
    187, 141, 230, 136, 176, 229, 139, 157, 228, 186, 134, 228,
    184, 173, 229, 156, 139, 229, 156, 139, 230, 176, 145, 233,
    187, 168, 230, 137, 128, 233, 160, 152, 229, 176, 142, 231,
    154, 132, 228, 184, 173, 232, 143, 175, 230, 176, 145, 229,
    156, 139, 229, 155, 189, 232, 187, 141, '[', 230, 179, 168,
    ' ', '6', ']', 239, 188, 140, 228, 184, 166, 229, 183, 178,
    233, 128, 144, 230, 188, 184, 230, 142, 167, 229, 136, 182,
    228, 186, 134, 228, 184, 173, 229, 156, 139, 229, 164, 167,
    233, 153, 184, 229, 164, 167, 233, 131, 168, 229, 136, 134,
    231, 156, 129, 228, 187, 189, 229, 146, 140, 229, 156, 176,
    229, 140, 186, 227, 128, 130,  13,  10,  13,  10, 229, 144,
    140, 229, 185, 180, '9', 230, 156, 136, '2', '1', 230, 151,
    165, 232, 135, 179, '9', 230, 156, 136, '3', '0', 230, 151,
    165, 239, 188, 140, 231, 182, 147, 233, 129, 142, 230, 149,
    184, 230, 156, 136, 231, 154, 132, 231, 177, 140, 229, 130,
    153, 239, 188, 140, 228, 184, 173, 229, 156, 139, 228, 186,
    186, 230, 176, 145, 230, 148, 191, 230, 178, 187, 229, 141,
    148, 229, 149, 134, 230, 156, 131, 232, 173, 176, 231, 172,
    172, 228, 184, 128, 229, 177, 134, 229, 133, 168, 233, 171,
    148, 230, 156, 131, 232, 173, 176, 229, 156, 168, 229, 140,
    151, 229, 185, 179, 229, 143, 172, 233, 150, 139, 227, 128,
    130, '9', 230, 156, 136, '2', '1', 230, 151, 165, 239, 188,
    140, 228, 184, 173, 229, 156, 139, 228, 186, 186, 230, 176,
    145, 230, 148, 191, 230, 178, 187, 229, 141, 148, 229, 149,
    134, 230, 156, 131, 232, 173, 176, 231, 172, 172, 228, 184,
    128, 229, 177, 134, 229, 133, 168, 233, 171, 148, 230, 156,
    131, 232, 173, 176, 230, 173, 163, 229, 188, 143, 229, 174,
    163, 229, 184, 131, 230, 136, 144, 231, 171, 139, 228, 184,
    173, 229, 141, 142, 228, 186, 186, 230, 176, 145, 229, 133,
    177, 229, 146, 140, 229, 155, 189, '[', '2', '0', ']', 227,
    128, 130, 228, 188, 154, 232, 174, 174, 233, 128, 154, 233,
    129, 142, 228, 186, 134, 227, 128, 138, 228, 184, 173, 229,
    156, 139, 228, 186, 186, 230, 176, 145, 230, 148, 191, 230,
    178, 187, 229, 141, 148, 229, 149, 134, 230, 156, 131, 232,
    173, 176, 231, 181, 132, 231, 185, 148, 230, 179, 149, 227,
    128, 139, 227, 128, 129, 227, 128, 138, 228, 184, 173, 232,
    143, 175, 228, 186, 186, 230, 176, 145, 229, 133, 177, 229,
    146, 140, 229, 156, 139, 228, 184, 173, 229, 164, 174, 228,
    186, 186, 230, 176, 145, 230, 148, 191, 229, 186, 156, 231,
    181, 132, 231, 185, 148, 230, 179, 149, 227, 128, 139, 229,
    146, 140, 229, 133, 183, 230, 156, 137, 232, 135, 168, 230,
    153, 130, 230, 134, 178, 230, 179, 149, 230, 128, 167, 232,
    179, 170, 231, 154, 132, 227, 128, 138, 228, 184, 173, 229,
    156, 139, 228, 186, 186, 230, 176, 145, 230, 148, 191, 230,
    178, 187, 229, 141, 148, 229, 149, 134, 230, 156, 131, 232,
    173, 176, 229, 133, 177, 229, 144, 140, 231, 182, 177, 233,
    160, 152, 227, 128, 139, 239, 188, 140, 230, 177, 186, 229,
    174, 154, 228, 187, 165, 229, 140, 151, 229, 185, 179, 231,
    130, 186, 233, 166, 150, 233, 131, 189, 228, 184, 166, 230,
    148, 185, 229, 144, 141, 231, 136, 178, 229, 140, 151, 228,
    186, 172, 227, 128, 129, 228, 187, 165, 229, 133, 172, 229,
    133, 131, 231, 180, 128, 229, 185, 180, 227, 128, 129, 228,
    187, 165, 231, 190, 169, 229, 139, 135, 232, 187, 141, 233,
    128, 178, 232, 161, 140, 230, 155, 178, 231, 130, 186, 228,
    187, 163, 229, 156, 139, 230, 173, 140, 227, 128, 129, 228,
    187, 165, 228, 186, 148, 230, 152, 159, 231, 180, 133, 230,
    151, 151, 231, 130, 186, 229, 156, 139, 230, 151, 151, 239,
    188, 140, 233, 128, 154, 233, 129, 142, 228, 186, 134, 231,
    148, 177, '1', '8', '0', 228, 186, 186, 231, 181, 132, 230,
    136, 144, 231, 154, 132, 228, 184, 173, 229, 156, 139, 228,
    186, 186, 230, 176, 145, 230, 148, 191, 230, 178, 187, 229,
    141, 148, 229, 149, 134, 230, 156, 131, 232, 173, 176, 231,
    172, 172, 228, 184, 128, 229, 177, 134, 229, 133, 168, 229,
    156, 139, 229, 167, 148, 229, 147, 161, 230, 156, 131, 229,
    144, 141, 229, 150, 174, 239, 188, 140, 228, 184, 166, 233,
    129, 184, 232, 136, 137, 230, 175, 155, 230, 190, 164, 230,
    157, 177, 231, 130, 186, 228, 184, 173, 229, 164, 174, 228,
    186, 186, 230, 176, 145, 230, 148, 191, 229, 186, 156, 228,
    184, 187, 229, 184, 173, 227, 128, 129, 230, 156, 177, 229,
    190, 183, 227, 128, 129, 229, 136, 152, 229, 176, 145, 229,
    165, 135, 227, 128, 129, 229, 174, 139, 229, 186, 134, 233,
    190, 132, 227, 128, 129, 230, 157, 142, 230, 181, 142, 230,
    183, 177, 227, 128, 129, 229, 188, 160, 230, 190, 156, 227,
    128, 129, 233, 171, 152, 229, 178, 151, 231, 130, 186, 229,
    137, 175, 228, 184, 187, 229, 184, 173, 227, 128, 129, 229,
    143, 166, 229, 164, 150, '5', '6', 228, 189, 141, 231, 130,
    186, 228, 184, 173, 229, 164, 174, 228, 186, 186, 230, 176,
    145, 230, 148, 191, 229, 186, 156, 229, 167, 148, 229, 147,
    161, 227, 128, 130, '1', '0', 230, 156, 136, '1', 230, 151,
    165, 229, 188, 128, 229, 155, 189, 229, 164, 167, 229, 133,
    184, 229, 156, 168, 229, 140, 151, 228, 186, 172, 228, 184,
    190, 232, 161, 140, 239, 188, 140, 230, 175, 155, 230, 190,
    164, 230, 157, 177, 229, 156, 168, 229, 164, 169, 229, 174,
    137, 233, 151, 168, 229, 159, 142, 230, 165, 188, 229, 174,
    163, 229, 145, 138, 228, 184, 173, 229, 141, 142, 228, 186,
    186, 230, 176, 145, 229, 133, 177, 229, 146, 140, 229, 155,
    189, 228, 184, 173, 229, 164, 174, 228, 186, 186, 230, 176,
    145, 230, 148, 191, 229, 186, 156, 230, 136, 144, 231, 171,
    139, 239, 188, 155, '1', '2', 230, 156, 136, '7', 230, 151,
    165, 239, 188, 140, 228, 184, 173, 232, 143, 175, 230, 176,
    145, 229, 156, 139, 230, 148, 191, 229, 186, 156, 230, 173,
    163, 229, 188, 143, 231, 148, 177, 229, 155, 155, 229, 183,
    157, 231, 156, 129, 230, 136, 144, 233, 131, 189, 229, 184,
    130, 233, 129, 183, 229, 190, 128, 229, 143, 176, 230, 185,
    190, 231, 156, 129, 229, 143, 176, 229, 140, 151, 229, 184,
    130, 239, 188, 140, 228, 184, 166, 231, 185, 188, 231, 186,
    140, 231, 181, 177, 230, 178, 187, 229, 143, 176, 231, 129,
    163, 230, 156, 172, 229, 179, 182, 229, 143, 138, 230, 190,
    142, 230, 185, 150, 227, 128, 129, 233, 131, 168, 229, 136,
    134, 231, 166, 143, 229, 187, 186, 233, 155, 162, 229, 179,
    182, 227, 128, 129, 228, 184, 156, 230, 178, 153, 231, 190,
    164, 229, 178, 155, 227, 128, 129, 229, 164, 170, 229, 185,
    179, 229, 178, 155, 231, 173, 137, 232, 135, 179, 228, 187,
    138, 227, 128, 130, 232, 135, 179, 230, 173, 164, 239, 188,
    140, 228, 184, 173, 229, 156, 139, 230, 173, 183, 229, 143,
    178, 228, 184, 138, 230, 150, 188, 230, 181, 183, 229, 179,
    189, 229, 133, 169, 229, 178, 184, 229, 136, 134, 230, 178,
    187, 231, 154, 132, 230, 148, 191, 230, 178, 187, 230, 160,
    188, 229, 177, 128, 230, 173, 163, 229, 188, 143, 229, 189,
    162, 230, 136, 144, 227, 128, 130,  13,  10,  13,  10, 'H',
    'i', 'n', 'd', 'i', ':',  13,  10,  13,  10, 224, 164, 173,
    224, 164, 190, 224, 164, 176, 224, 164, 164, ',', ' ', 224,
    164, 170, 224, 165, 140, 224, 164, 176, 224, 164, 190, 224,
    164, 163, 224, 164, 191, 224, 164, 149, ' ', 224, 164, 156,
    224, 164, 174, 224, 165, 141, 224, 164, 172, 224, 165, 130,
    224, 164, 166, 224, 165, 141, 224, 164, 181, 224, 165, 128,
    224, 164, 170, ',', ' ', 224, 164, 134, 224, 164, 167, 224,
    165, 129, 224, 164, 168, 224, 164, 191, 224, 164, 149, ' ',
    224, 164, 166, 224, 164, 149, 224, 165, 141, 224, 164, 183,
    224, 164, 191, 224, 164, 163, ' ', 224, 164, 143, 224, 164,
    182, 224, 164, 191, 224, 164, 175, 224, 164, 190, ' ', 224,
    164, 174, 224, 165, 135, 224, 164, 130, ' ', 224, 164, 184,
    224, 165, 141, 224, 164, 165, 224, 164, 191, 224, 164, 164,
    ' ', 224, 164, 173, 224, 164, 190, 224, 164, 176, 224, 164,
    164, 224, 165, 128, 224, 164, 175, ' ', 224, 164, 137, 224,
    164, 170, 224, 164, 174, 224, 164, 185, 224, 164, 190, 224,
    164, 166, 224, 165, 141, 224, 164, 181, 224, 165, 128, 224,
    164, 170, ' ', 224, 164, 149, 224, 164, 190, ' ', 224, 164,
    184, 224, 164, 172, 224, 164, 184, 224, 165, 135, ' ', 224,
    164, 172, 224, 164, 161, 224, 164, 188, 224, 164, 190, ' ',
    224, 164, 166, 224, 165, 135, 224, 164, 182, ' ', 224, 164,
    185, 224, 165, 136, 224, 165, 164, ' ', 224, 164, 173, 224,
    164, 190, 224, 164, 176, 224, 164, 164, ' ', 224, 164, 149,
    224, 164, 190, ' ', 224, 164, 173, 224, 165, 140, 224, 164,
    151, 224, 165, 139, 224, 164, 178, 224, 164, 191, 224, 164,
    149, ' ', 224, 164, 171, 224, 165, 136, 224, 164, 178, 224,
    164, 190, 224, 164, 181, ' ', 224, 165, 174, 224, 165, 166,
    ' ', 224, 165, 170,'\'', ' ', 224, 164, 184, 224, 165, 135,
    ' ', 224, 165, 169, 224, 165, 173, 224, 165, 166, ' ', 224,
    165, 172,'\'', ' ', 224, 164, 137, 224, 164, 164, 224, 165,
    141, 224, 164, 164, 224, 164, 176, 224, 165, 128, ' ', 224,
    164, 133, 224, 164, 149, 224, 165, 141, 224, 164, 183, 224,
    164, 190, 224, 164, 130, 224, 164, 182, ' ', 224, 164, 164,
    224, 164, 149, ' ', 224, 164, 164, 224, 164, 165, 224, 164,
    190, ' ', 224, 165, 172, 224, 165, 174, 224, 165, 166, ' ',
    224, 165, 173,'\'', ' ', 224, 164, 184, 224, 165, 135, ' ',
    224, 165, 175, 224, 165, 173, 224, 165, 166, ' ', 224, 165,
    168, 224, 165, 171,'\'', 224, 164, 170, 224, 165, 130, 224,
    164, 176, 224, 165, 141, 224, 164, 181, 224, 165, 128, ' ',
    224, 164, 166, 224, 165, 135, 224, 164, 182, 224, 164, 190,
    224, 164, 168, 224, 165, 141, 224, 164, 164, 224, 164, 176,
    ' ', 224, 164, 164, 224, 164, 149, ' ', 224, 164, 185, 224,
    165, 136, 224, 165, 164, ' ', 224, 164, 173, 224, 164, 190,
    224, 164, 176, 224, 164, 164, ' ', 224, 164, 149, 224, 164,
    190, ' ', 224, 164, 181, 224, 164, 191, 224, 164, 184, 224,
    165, 141, 224, 164, 164, 224, 164, 190, 224, 164, 176, ' ',
    224, 164, 137, 224, 164, 164, 224, 165, 141, 224, 164, 164,
    224, 164, 176, ' ', 224, 164, 184, 224, 165, 135, ' ', 224,
    164, 166, 224, 164, 149, 224, 165, 141, 224, 164, 183, 224,
    164, 191, 224, 164, 163, ' ', 224, 164, 164, 224, 164, 149,
    ' ', 224, 164, 149, 224, 164, 191, '.', ' ', 224, 164, 174,
    224, 165, 128, '.', ' ', 224, 164, 148, 224, 164, 176, ' ',
    224, 164, 170, 224, 165, 130, 224, 164, 176, 224, 165, 141,
    224, 164, 181, ' ', 224, 164, 184, 224, 165, 135, ' ', 224,
    164, 170, 224, 164, 182, 224, 165, 141, 224, 164, 154, 224,
    164, 191, 224, 164, 174, ' ', 224, 164, 164, 224, 164, 149,
    ' ', 224, 165, 168, ',', 224, 165, 175, 224, 165, 169, 224,
    165, 169, ' ', 224, 164, 149, 224, 164, 191, '.', ' ', 224,
    164, 174, 224, 165, 128, '.', ' ', 224, 164, 185, 224, 165,
    136, 224, 165, 164, ' ', 224, 164, 173, 224, 164, 190, 224,
    164, 176, 224, 164, 164, ' ', 224, 164, 149, 224, 165, 128,
    ' ', 224, 164, 184, 224, 164, 174, 224, 165, 129, 224, 164,
    166, 224, 165, 141, 224, 164, 176, ' ', 224, 164, 164, 224,
    164, 159, ' ', 224, 164, 176, 224, 165, 135, 224, 164, 150,
    224, 164, 190, ' ', 224, 165, 173, 224, 165, 171, 224, 165,
    167, 224, 165, 172, '.', 224, 165, 172, ' ', 224, 164, 149,
    224, 164, 191, 224, 164, 178, 224, 165, 139, 224, 164, 174,
    224, 165, 128, 224, 164, 159, 224, 164, 176, ' ', 224, 164,
    178, 224, 164, 174, 224, 165, 141, 224, 164, 172, 224, 165,
    128, ' ', 224, 164, 185, 224, 165, 136, 224, 165, 164, ' ',
    224, 164, 173, 224, 164, 190, 224, 164, 176, 224, 164, 164,
    ',', ' ', 224, 164, 173, 224, 165, 140, 224, 164, 151, 224,
    165, 139, 224, 164, 178, 224, 164, 191, 224, 164, 149, ' ',
    224, 164, 166, 224, 165, 131, 224, 164, 183, 224, 165, 141,
    224, 164, 159, 224, 164, 191, ' ', 224, 164, 184, 224, 165,
    135, ' ', 224, 164, 181, 224, 164, 191, 224, 164, 182, 224,
    165, 141, 224, 164, 181, ' ', 224, 164, 174, 224, 165, 135,
    224, 164, 130, ' ', 224, 164, 184, 224, 164, 190, 224, 164,
    164, 224, 164, 181, 224, 164, 190, 224, 164, 129, ' ', 224,
    164, 184, 224, 164, 172, 224, 164, 184, 224, 165, 135, ' ',
    224, 164, 172, 224, 164, 161, 224, 164, 188, 224, 164, 190,
    ' ', 224, 164, 148, 224, 164, 176, ' ', 224, 164, 156, 224,
    164, 168, 224, 164, 184, 224, 164, 129, 224, 164, 150, 224,
    165, 141, 224, 164, 175, 224, 164, 190, ' ', 224, 164, 149,
    224, 165, 135, ' ', 224, 164, 166, 224, 165, 131, 224, 164,
    183, 224, 165, 141, 224, 164, 159, 224, 164, 191, 224, 164,
    149, 224, 165, 139, 224, 164, 163, ' ', 224, 164, 184, 224,
    165, 135, ' ', 224, 164, 166, 224, 165, 130, 224, 164, 184,
    224, 164, 176, 224, 164, 190, ' ', 224, 164, 184, 224, 164,
    172, ' ', 224, 164, 184, 224, 165, 135, ' ', 224, 164, 172,
    224, 164, 161, 224, 164, 188, 224, 164, 190, ' ', 224, 164,
    166, 224, 165, 135, 224, 164, 182, ' ', 224, 164, 185, 224,
    165, 136, 224, 165, 164, ' ', 224, 164, 173, 224, 164, 190,
    224, 164, 176, 224, 164, 164, ' ', 224, 164, 149, 224, 165,
    135, ' ', 224, 164, 170, 224, 164, 182, 224, 165, 141, 224,
    164, 154, 224, 164, 191, 224, 164, 174, ' ', 224, 164, 174,
    224, 165, 135, 224, 164, 130, ' ', 224, 164, 170, 224, 164,
    190, 224, 164, 149, 224, 164, 191, 224, 164, 184, 224, 165,
    141, 224, 164, 164, 224, 164, 190, 224, 164, 168, ',', ' ',
    224, 164, 137, 224, 164, 164, 224, 165, 141, 224, 164, 164,
    224, 164, 176, '-', 224, 164, 170, 224, 165, 130, 224, 164,
    176, 224, 165, 141, 224, 164, 181, ' ', 224, 164, 174, 224,
    165, 135, 224, 164, 130, ' ', 224, 164, 154, 224, 165, 128,
    224, 164, 168, ',', ' ', 224, 164, 168, 224, 165, 135, 224,
    164, 170, 224, 164, 190, 224, 164, 178, ',', ' ', 224, 164,
    148, 224, 164, 176, ' ', 224, 164, 173, 224, 165, 130, 224,
    164, 159, 224, 164, 190, 224, 164, 168, ' ', 224, 164, 148,
    224, 164, 176, ' ', 224, 164, 170, 224, 165, 130, 224, 164,
    176, 224, 165, 141, 224, 164, 181, ' ', 224, 164, 174, 224,
    165, 135, 224, 164, 130, ' ', 224, 164, 172, 224, 164, 190,
    224, 164, 130, 224, 164, 151, 224, 165, 141, 224, 164, 178,
    224, 164, 190, 224, 164, 166, 224, 165, 135, 224, 164, 182,
    ' ', 224, 164, 148, 224, 164, 176, ' ', 224, 164, 174, 224,
    165, 141, 224, 164, 175, 224, 164, 190, 224, 164, 168, 224,
    165, 141, 224, 164, 174, 224, 164, 190, 224, 164, 176, ' ',
    224, 164, 166, 224, 165, 135, 224, 164, 182, ' ', 224, 164,
    184, 224, 165, 141, 224, 164, 165, 224, 164, 191, 224, 164,
    164, ' ', 224, 164, 185, 224, 165, 136, 224, 164, 130, 224,
    165, 164, ' ', 224, 164, 185, 224, 164, 191, 224, 164, 168,
    224, 165, 141, 224, 164, 166, ' ', 224, 164, 174, 224, 164,
    185, 224, 164, 190, 224, 164, 184, 224, 164, 190, 224, 164,
    151, 224, 164, 176, ' ', 224, 164, 174, 224, 165, 135, 224,
    164, 130, ' ', 224, 164, 135, 224, 164, 184, 224, 164, 149,
    224, 165, 135, ' ', 224, 164, 166, 224, 164, 149, 224, 165,
    141, 224, 164, 183, 224, 164, 191, 224, 164, 163, ' ', 224,
    164, 170, 224, 164, 182, 224, 165, 141, 224, 164, 154, 224,
    164, 191, 224, 164, 174, ' ', 224, 164, 174, 224, 165, 135,
    224, 164, 130, ' ', 224, 164, 174, 224, 164, 190, 224, 164,
    178, 224, 164, 166, 224, 165, 128, 224, 164, 181, ',', ' ',
    224, 164, 166, 224, 164, 149, 224, 165, 141, 224, 164, 183,
    224, 164, 191, 224, 164, 163, ' ', 224, 164, 174, 224, 165,
    135, 224, 164, 130, ' ', 224, 164, 182, 224, 165, 141, 224,
    164, 176, 224, 165, 128, 224, 164, 178, 224, 164, 130, 224,
    164, 149, 224, 164, 190, ' ', 224, 164, 148, 224, 164, 176,
    ' ', 224, 164, 166, 224, 164, 149, 224, 165, 141, 224, 164,
    183, 224, 164, 191, 224, 164, 163, '-', 224, 164, 170, 224,
    165, 130, 224, 164, 176, 224, 165, 141, 224, 164, 181, ' ',
    224, 164, 174, 224, 165, 135, 224, 164, 130, ' ', 224, 164,
    135, 224, 164, 130, 224, 164, 161, 224, 165, 139, 224, 164,
    168, 224, 165, 135, 224, 164, 182, 224, 164, 191, 224, 164,
    175, 224, 164, 190, ' ', 224, 164, 185, 224, 165, 136, 224,
    164, 130, 224, 165, 164, ' ', 224, 164, 137, 224, 164, 164,
    224, 165, 141, 224, 164, 164, 224, 164, 176, '-', 224, 164,
    170, 224, 164, 182, 224, 165, 141, 224, 164, 154, 224, 164,
    191, 224, 164, 174, ' ', 224, 164, 174, 224, 165, 135, 224,
    164, 130, ' ', 224, 164, 133, 224, 164, 171, 224, 164, 188,
    224, 164, 151, 224, 164, 190, 224, 164, 168, 224, 164, 191,
    224, 164, 184, 224, 165, 141, 224, 164, 164, 224, 164, 190,
    224, 164, 168, ' ', 224, 164, 149, 224, 165, 135, ' ', 224,
    164, 184, 224, 164, 190, 224, 164, 165, ' ', 224, 164, 173,
    224, 164, 190, 224, 164, 176, 224, 164, 164, ' ', 224, 164,
    149, 224, 165, 128, ' ', 224, 164, 184, 224, 165, 128, 224,
    164, 174, 224, 164, 190, ' ', 224, 164, 185, 224, 165, 136,
    224, 165, 164, ' ', 224, 164, 135, 224, 164, 184, 224, 164,
    149, 224, 165, 135, ' ', 224, 164, 137, 224, 164, 164, 224,
    165, 141, 224, 164, 164, 224, 164, 176, ' ', 224, 164, 174,
    224, 165, 135, 224, 164, 130, ' ', 224, 164, 185, 224, 164,
    191, 224, 164, 174, 224, 164, 190, 224, 164, 178, 224, 164,
    175, ' ', 224, 164, 170, 224, 164, 176, 224, 165, 141, 224,
    164, 181, 224, 164, 164, ' ', 224, 164, 185, 224, 165, 136,
    ' ', 224, 164, 148, 224, 164, 176, ' ', 224, 164, 166, 224,
    164, 149, 224, 165, 141, 224, 164, 183, 224, 164, 191, 224,
    164, 163, ' ', 224, 164, 174, 224, 165, 135, 224, 164, 130,
    ' ', 224, 164, 185, 224, 164, 191, 224, 164, 168, 224, 165,
    141, 224, 164, 166, ' ', 224, 164, 174, 224, 164, 185, 224,
    164, 190, 224, 164, 184, 224, 164, 190, 224, 164, 151, 224,
    164, 176, ' ', 224, 164, 185, 224, 165, 136, 224, 165, 164,
    ' ', 224, 164, 170, 224, 165, 130, 224, 164, 176, 224, 165,
    141, 224, 164, 181, ' ', 224, 164, 174, 224, 165, 135, 224,
    164, 130, ' ', 224, 164, 172, 224, 164, 130, 224, 164, 151,
    224, 164, 190, 224, 164, 178, ' ', 224, 164, 149, 224, 165,
    128, ' ', 224, 164, 150, 224, 164, 190, 224, 164, 161, 224,
    164, 188, 224, 165, 128, ' ', 224, 164, 185, 224, 165, 136,
    ' ', 224, 164, 164, 224, 164, 165, 224, 164, 190, ' ', 224,
    164, 170, 224, 164, 182, 224, 165, 141, 224, 164, 154, 224,
    164, 191, 224, 164, 174, ' ', 224, 164, 174, 224, 165, 135,
    224, 164, 130, ' ', 224, 164, 133, 224, 164, 176, 224, 164,
    172, ' ', 224, 164, 184, 224, 164, 190, 224, 164, 151, 224,
    164, 176, 224, 164, 184, 224, 164, 174, 224, 165, 129, 224,
    164, 166, 224, 165, 141, 224, 164, 176, ' ', 224, 164, 185,
    224, 165, 136, 224, 164, 130, ' ', 224, 165, 164, ' ', 224,
    164, 173, 224, 164, 190, 224, 164, 176, 224, 164, 164, ' ',
    224, 164, 174, 224, 165, 135, 224, 164, 130, ' ', 224, 164,
    149, 224, 164, 136, ' ', 224, 164, 172, 224, 164, 161, 224,
    164, 188, 224, 165, 128, ' ', 224, 164, 168, 224, 164, 166,
    224, 164, 191, 224, 164, 175, 224, 164, 190, 224, 164, 129,
    ' ', 224, 164, 185, 224, 165, 136, 224, 164, 130, ' ', 224,
    165, 164, ' ', 224, 164, 151, 224, 164, 130, 224, 164, 151,
    224, 164, 190, ' ', 224, 164, 168, 224, 164, 166, 224, 165,
    128, ' ', 224, 164, 173, 224, 164, 190, 224, 164, 176, 224,
    164, 164, 224, 165, 128, 224, 164, 175, ' ', 224, 164, 184,
    224, 164, 130, 224, 164, 184, 224, 165, 141, 224, 164, 149,
    224, 165, 131, 224, 164, 164, 224, 164, 191, ' ', 224, 164,
    174, 224, 165, 135, 224, 164, 130, ' ', 224, 164, 133, 224,
    164, 164, 224, 165, 141, 224, 164, 175, 224, 164, 130, 224,
    164, 164, ' ', 224, 164, 170, 224, 164, 181, 224, 164, 191,
    224, 164, 164, 224, 165, 141, 224, 164, 176, ' ', 224, 164,
    174, 224, 164, 190, 224, 164, 168, 224, 165, 128, ' ', 224,
    164, 156, 224, 164, 190, 224, 164, 164, 224, 165, 128, ' ',
    224, 164, 185, 224, 165, 136, 224, 165, 164, ' ', 224, 164,
    133, 224, 164, 168, 224, 165, 141, 224, 164, 175, ' ', 224,
    164, 172, 224, 164, 161, 224, 164, 188, 224, 165, 128, ' ',
    224, 164, 168, 224, 164, 166, 224, 164, 191, 224, 164, 175,
    224, 164, 190, 224, 164, 129, ' ', 224, 164, 184, 224, 164,
    191, 224, 164, 168, 224, 165, 141, 224, 164, 167, 224, 165,
    129, ',', ' ', 224, 164, 168, 224, 164, 176, 224, 165, 141,
    224, 164, 174, 224, 164, 166, 224, 164, 190, ',', ' ', 224,
    164, 172, 224, 165, 141, 224, 164, 176, 224, 164, 185, 224,
    165, 141, 224, 164, 174, 224, 164, 170, 224, 165, 129, 224,
    164, 164, 224, 165, 141, 224, 164, 176, ',', ' ', 224, 164,
    175, 224, 164, 174, 224, 165, 129, 224, 164, 168, 224, 164,
    190, ',', ' ', 224, 164, 151, 224, 165, 139, 224, 164, 166,
    224, 164, 190, 224, 164, 181, 224, 164, 176, 224, 165, 128,
    ',', ' ', 224, 164, 149, 224, 164, 190, 224, 164, 181, 224,
    165, 135, 224, 164, 176, 224, 165, 128, ',', ' ', 224, 164,
    149, 224, 165, 131, 224, 164, 183, 224, 165, 141, 224, 164,
    163, 224, 164, 190, ',', ' ', 224, 164, 154, 224, 164, 174,
    224, 165, 141, 224, 164, 172, 224, 164, 178, ',', ' ', 224,
    164, 184, 224, 164, 164, 224, 164, 178, 224, 164, 156, ',',
    ' ', 224, 164, 181, 224, 165, 141, 224, 164, 175, 224, 164,
    190, 224, 164, 184, ' ', 224, 164, 134, 224, 164, 166, 224,
    164, 191, ' ', 224, 164, 185, 224, 165, 136, 224, 164, 130,
    224, 165, 164,  13,  10,  13,  10, 224, 164, 175, 224, 164,
    185, ' ', 224, 164, 181, 224, 164, 191, 224, 164, 182, 224,
    165, 141, 224, 164, 181, ' ', 224, 164, 149, 224, 164, 190,
    ' ', 224, 164, 184, 224, 164, 172, 224, 164, 184, 224, 165,
    135, ' ', 224, 164, 172, 224, 164, 161, 224, 164, 188, 224,
    164, 190, ' ', 224, 164, 178, 224, 165, 139, 224, 164, 149,
    224, 164, 164, 224, 164, 130, 224, 164, 164, 224, 165, 141,
    224, 164, 176, ' ', 224, 164, 185, 224, 165, 136, 224, 165,
    164, ' ', 224, 164, 175, 224, 164, 185, 224, 164, 190, 224,
    164, 129, ' ', 224, 165, 169, 224, 165, 166, 224, 165, 166,
    ' ', 224, 164, 184, 224, 165, 135, ' ', 224, 164, 133, 224,
    164, 167, 224, 164, 191, 224, 164, 149, ' ', 224, 164, 173,
    224, 164, 190, 224, 164, 183, 224, 164, 190, 224, 164, 143,
    224, 164, 129, ' ', 224, 164, 172, 224, 165, 139, 224, 164,
    178, 224, 165, 128, ' ', 224, 164, 156, 224, 164, 190, 224,
    164, 164, 224, 165, 128, ' ', 224, 164, 185, 224, 165, 136,
    224, 164, 130, ' ', '[', '1', ']', 224, 165, 164, ' ', 224,
    164, 175, 224, 164, 185, ' ', 224, 164, 181, 224, 164, 191,
    224, 164, 182, 224, 165, 141, 224, 164, 181, ' ', 224, 164,
    149, 224, 165, 128, ' ', 224, 164, 149, 224, 165, 129, 224,
    164, 155, ' ', 224, 164, 170, 224, 165, 141, 224, 164, 176,
    224, 164, 190, 224, 164, 154, 224, 165, 128, 224, 164, 168,
    224, 164, 164, 224, 164, 174, ' ', 224, 164, 184, 224, 164,
    173, 224, 165, 141, 224, 164, 175, 224, 164, 164, 224, 164,
    190, 224, 164, 147, 224, 164, 130, ' ', 224, 164, 149, 224,
    165, 128, ' ', 224, 164, 156, 224, 164, 168, 224, 164, 168,
    224, 165, 128, ' ', 224, 164, 176, 224, 164, 185, 224, 164,
    190, ' ', 224, 164, 185, 224, 165, 136, ' ', 224, 164, 156,
    224, 165, 136, 224, 164, 184, 224, 165, 135, ' ', '-', ' ',
    224, 164, 184, 224, 164, 191, 224, 164, 168, 224, 165, 141,
    224, 164, 167, 224, 165, 129, ' ', 224, 164, 152, 224, 164,
    190, 224, 164, 159, 224, 165, 128, ' ', 224, 164, 184, 224,
    164, 173, 224, 165, 141, 224, 164, 175, 224, 164, 164, 224,
    164, 190, ',', ' ', 224, 164, 148, 224, 164, 176, ' ', 224,
    164, 174, 224, 164, 185, 224, 164, 164, 224, 165, 141, 224,
    164, 181, 224, 164, 170, 224, 165, 130, 224, 164, 176, 224,
    165, 141, 224, 164, 163, ' ', 224, 164, 144, 224, 164, 164,
    224, 164, 191, 224, 164, 185, 224, 164, 190, 224, 164, 184,
    224, 164, 191, 224, 164, 149, ' ', 224, 164, 181, 224, 165,
    141, 224, 164, 175, 224, 164, 190, 224, 164, 170, 224, 164,
    190, 224, 164, 176, ' ', 224, 164, 170, 224, 164, 165, 224,
    165, 139, 224, 164, 130, ' ', 224, 164, 149, 224, 164, 190,
    ' ', 224, 164, 133, 224, 164, 173, 224, 164, 191, 224, 164,
    168, 224, 165, 141, 224, 164, 168, ' ', 224, 164, 133, 224,
    164, 130, 224, 164, 151, ' ', 224, 164, 173, 224, 165, 128,
    '.', ' ', 224, 164, 181, 224, 164, 191, 224, 164, 182, 224,
    165, 141, 224, 164, 181, ' ', 224, 164, 149, 224, 165, 135,
    ' ', 224, 164, 154, 224, 164, 190, 224, 164, 176, ' ', 224,
    164, 170, 224, 165, 141, 224, 164, 176, 224, 164, 174, 224,
    165, 129, 224, 164, 150, ' ', 224, 164, 167, 224, 164, 176,
    224, 165, 141, 224, 164, 174, ' ', ':', ' ', 224, 164, 184,
    224, 164, 168, 224, 164, 190, 224, 164, 164, 224, 164, 168,
    '-', 224, 164, 185, 224, 164, 191, 224, 164, 168, 224, 165,
    141, 224, 164, 166, 224, 165, 130, ',', ' ', 224, 164, 172,
    224, 165, 140, 224, 164, 166, 224, 165, 141, 224, 164, 167,
    ',', ' ', 224, 164, 156, 224, 165, 136, 224, 164, 168, ' ',
    224, 164, 164, 224, 164, 165, 224, 164, 190, ' ', 224, 164,
    184, 224, 164, 191, 224, 164, 150, ' ', 224, 164, 173, 224,
    164, 190, 224, 164, 176, 224, 164, 164, ' ', 224, 164, 174,
    224, 165, 135, 224, 164, 130, ' ', 224, 164, 185, 224, 165,
    128, ' ', 224, 164, 156, 224, 164, 168, 224, 165, 141, 224,
    164, 174, 224, 165, 135, ' ', 224, 164, 148, 224, 164, 176,
    ' ', 224, 164, 181, 224, 164, 191, 224, 164, 149, 224, 164,
    184, 224, 164, 191, 224, 164, 164, ' ', 224, 164, 185, 224,
    165, 129, 224, 164, 143, 224, 165, 164,  13,  10,  13,  10,
    224, 164, 173, 224, 164, 190, 224, 164, 176, 224, 164, 164,
    ' ', 224, 164, 173, 224, 165, 140, 224, 164, 151, 224, 165,
    139, 224, 164, 178, 224, 164, 191, 224, 164, 149, ' ', 224,
    164, 149, 224, 165, 141, 224, 164, 183, 224, 165, 135, 224,
    164, 164, 224, 165, 141, 224, 164, 176, 224, 164, 171, 224,
    164, 178, ' ', 224, 164, 149, 224, 165, 135, ' ', 224, 164,
    134, 224, 164, 167, 224, 164, 190, 224, 164, 176, ' ', 224,
    164, 170, 224, 164, 176, ' ', 224, 164, 181, 224, 164, 191,
    224, 164, 182, 224, 165, 141, 224, 164, 181, ' ', 224, 164,
    149, 224, 164, 190, ' ', 224, 164, 184, 224, 164, 190, 224,
    164, 164, 224, 164, 181, 224, 164, 190, 224, 164, 129, ' ',
    224, 164, 184, 224, 164, 172, 224, 164, 184, 224, 165, 135,
    ' ', 224, 164, 172, 224, 164, 161, 224, 164, 188, 224, 164,
    190, ' ', 224, 164, 176, 224, 164, 190, 224, 164, 183, 224,
    165, 141, 224, 164, 159, 224, 165, 141, 224, 164, 176, ' ',
    224, 164, 185, 224, 165, 136, 224, 165, 164, ' ', 224, 164,
    173, 224, 164, 190, 224, 164, 176, 224, 164, 164, ' ', 224,
    164, 149, 224, 165, 128, ' ', 224, 164, 176, 224, 164, 190,
    224, 164, 156, 224, 164, 167, 224, 164, 190, 224, 164, 168,
    224, 165, 128, ' ', 224, 164, 168, 224, 164, 136, ' ', 224,
    164, 166, 224, 164, 191, 224, 164, 178, 224, 165, 141, 224,
    164, 178, 224, 165, 128, ' ', 224, 164, 185, 224, 165, 136,
    224, 165, 164, ' ', 224, 164, 173, 224, 164, 190, 224, 164,
    176, 224, 164, 164, ' ', 224, 164, 149, 224, 165, 135, ' ',
    224, 164, 133, 224, 164, 168, 224, 165, 141, 224, 164, 175,
    ' ', 224, 164, 172, 224, 164, 161, 224, 164, 188, 224, 165,
    135, ' ', 224, 164, 174, 224, 164, 185, 224, 164, 190, 224,
    164, 168, 224, 164, 151, 224, 164, 176, ' ', 224, 164, 174,
    224, 165, 129, 224, 164, 174, 224, 165, 141, 224, 164, 172,
    224, 164, 136, ' ', '(', 224, 164, 172, 224, 164, 174, 224,
    165, 141, 224, 164, 172, 224, 164, 136, ')', ',', ' ', 224,
    164, 149, 224, 165, 139, 224, 164, 178, 224, 164, 149, 224,
    164, 190, 224, 164, 164, 224, 164, 190, ' ', '(', 224, 164,
    149, 224, 164, 178, 224, 164, 149, 224, 164, 164, 224, 165,
    141, 224, 164, 164, 224, 164, 190, ')', ' ', 224, 164, 148,
    224, 164, 176, ' ', 224, 164, 154, 224, 165, 135, 224, 164,
    168, 224, 165, 141, 224, 164, 168, 224, 164, 136, ' ', '(',
    224, 164, 174, 224, 164, 166, 224, 165, 141, 224, 164, 176,
    224, 164, 190, 224, 164, 184, ')', ' ', 224, 164, 185, 224,
    165, 136, 224, 164, 130, 224, 165, 164, ' ', 224, 165, 167,
    224, 165, 175, 224, 165, 170, 224, 165, 173, ' ', 224, 164,
    174, 224, 165, 135, 224, 164, 130, ' ', 224, 164, 184, 224,
    165, 141, 224, 164, 181, 224, 164, 164, 224, 164, 130, 224,
    164, 164, 224, 165, 141, 224, 164, 176, 224, 164, 164, 224,
    164, 190, ' ', 224, 164, 170, 224, 165, 141, 224, 164, 176,
    224, 164, 190, 224, 164, 170, 224, 165, 141, 224, 164, 164,
    224, 164, 191, ' ', 224, 164, 184, 224, 165, 135, ' ', 224,
    164, 170, 224, 165, 130, 224, 164, 176, 224, 165, 141, 224,
    164, 181, ' ', 224, 164, 172, 224, 165, 141, 224, 164, 176,
    224, 164, 191, 224, 164, 159, 224, 164, 191, 224, 164, 182,
    ' ', 224, 164, 173, 224, 164, 190, 224, 164, 176, 224, 164,
    164, ' ', 224, 164, 149, 224, 165, 135, ' ', 224, 164, 176,
    224, 165, 130, 224, 164, 170, ' ', 224, 164, 174, 224, 165,
    135, 224, 164, 130, ' ', 224, 164, 172, 224, 165, 141, 224,
    164, 176, 224, 164, 191, 224, 164, 159, 224, 164, 191, 224,
    164, 182, ' ', 224, 164, 184, 224, 164, 190, 224, 164, 174,
    224, 165, 141, 224, 164, 176, 224, 164, 190, 224, 164, 156,
    224, 165, 141, 224, 164, 175, ' ', 224, 164, 149, 224, 165,
    135, ' ', 224, 164, 170, 224, 165, 141, 224, 164, 176, 224,
    164, 174, 224, 165, 129, 224, 164, 150, ' ', 224, 164, 133,
    224, 164, 130, 224, 164, 151, ' ', 224, 164, 173, 224, 164,
    190, 224, 164, 176, 224, 164, 164, ' ', 224, 164, 168, 224,
    165, 135, ' ', 224, 164, 181, 224, 164, 191, 224, 164, 151,
    224, 164, 164, ' ', 224, 165, 168, 224, 165, 166, ' ', 224,
    164, 181, 224, 164, 176, 224, 165, 141, 224, 164, 183, ' ',
    224, 164, 174, 224, 165, 135, 224, 164, 130, ' ', 224, 164,
    184, 224, 164, 190, 224, 164, 176, 224, 165, 141, 224, 164,
    165, 224, 164, 149, ' ', 224, 164, 170, 224, 165, 141, 224,
    164, 176, 224, 164, 151, 224, 164, 164, 224, 164, 191, ' ',
    224, 164, 149, 224, 165, 128, ' ', 224, 164, 185, 224, 165,
    136, ',', ' ', 224, 164, 181, 224, 164, 191, 224, 164, 182,
    224, 165, 135, 224, 164, 183, ' ', 224, 164, 176, 224, 165,
    130, 224, 164, 170, ' ', 224, 164, 184, 224, 165, 135, ' ',
    224, 164, 134, 224, 164, 176, 224, 165, 141, 224, 164, 165,
    224, 164, 191, 224, 164, 149, ' ', 224, 164, 148, 224, 164,
    176, ' ', 224, 164, 173, 224, 164, 190, 224, 164, 176, 224,
    164, 164, 224, 165, 128, 224, 164, 175, ' ', 224, 164, 184,
    224, 165, 135, 224, 164, 168, 224, 164, 190, ' ', 224, 164,
    143, 224, 164, 149, ' ', 224, 164, 149, 224, 165, 141, 224,
    164, 183, 224, 165, 135, 224, 164, 164, 224, 165, 141, 224,
    164, 176, 224, 165, 128, 224, 164, 175, ' ', 224, 164, 182,
    224, 164, 149, 224, 165, 141, 224, 164, 164, 224, 164, 191,
    ' ', 224, 164, 148, 224, 164, 176, ' ', 224, 164, 181, 224,
    164, 191, 224, 164, 182, 224, 165, 141, 224, 164, 181, 224,
    164, 181, 224, 165, 141, 224, 164, 175, 224, 164, 190, 224,
    164, 170, 224, 164, 149, ' ', 224, 164, 182, 224, 164, 149,
    224, 165, 141, 224, 164, 164, 224, 164, 191, ' ', 224, 164,
    185, 224, 165, 136, 224, 165, 164, ' ', 224, 164, 173, 224,
    164, 190, 224, 164, 176, 224, 164, 164, ' ', 224, 164, 181,
    224, 164, 191, 224, 164, 182, 224, 165, 141, 224, 164, 181,
    ' ', 224, 164, 149, 224, 165, 128, ' ', 224, 164, 166, 224,
    164, 184, 224, 164, 181, 224, 165, 128, 224, 164, 130, ' ',
    224, 164, 184, 224, 164, 172, 224, 164, 184, 224, 165, 135,
    ' ', 224, 164, 172, 224, 164, 161, 224, 164, 188, 224, 165,
    128, ' ', 224, 164, 133, 224, 164, 176, 224, 165, 141, 224,
    164, 165, 224, 164, 181, 224, 165, 141, 224, 164, 175, 224,
    164, 181, 224, 164, 184, 224, 165, 141, 224, 164, 165, 224,
    164, 190, ' ', 224, 164, 185, 224, 165, 136, 224, 165, 164,
    ' ', 224, 164, 185, 224, 164, 190, 224, 164, 178, ' ', 224,
    164, 149, 224, 165, 135, ' ', 224, 164, 181, 224, 164, 176,
    224, 165, 141, 224, 164, 183, 224, 165, 139, 224, 164, 130,
    ' ', 224, 164, 174, 224, 165, 135, 224, 164, 130, ' ', 224,
    164, 173, 224, 164, 190, 224, 164, 176, 224, 164, 164, ' ',
    224, 164, 149, 224, 165, 128, ' ', 224, 164, 133, 224, 164,
    176, 224, 165, 141, 224, 164, 165, 224, 164, 181, 224, 165,
    141, 224, 164, 175, 224, 164, 181, 224, 164, 184, 224, 165,
    141, 224, 164, 165, 224, 164, 190, ' ', 224, 164, 168, 224,
    165, 135, ' ', 224, 164, 172, 224, 164, 185, 224, 165, 129,
    224, 164, 164, ' ', 224, 164, 170, 224, 165, 141, 224, 164,
    176, 224, 164, 151, 224, 164, 164, 224, 164, 191, ' ', 224,
    164, 149, 224, 165, 128, ' ', 224, 164, 185, 224, 165, 136,
    ',', ' ', 224, 164, 148, 224, 164, 176, ' ', 224, 164, 164,
    224, 164, 190, 224, 164, 156, 224, 164, 188, 224, 164, 190,
    ' ', 224, 164, 184, 224, 165, 141, 224, 164, 165, 224, 164,
    191, 224, 164, 164, 224, 164, 191, ' ', 224, 164, 174, 224,
    165, 135, 224, 164, 130, ' ', 224, 164, 173, 224, 164, 190,
    224, 164, 176, 224, 164, 164, ' ', 224, 164, 181, 224, 164,
    191, 224, 164, 182, 224, 165, 141, 224, 164, 181, ' ', 224,
    164, 174, 224, 165, 135, 224, 164, 130, ' ', 224, 164, 164,
    224, 165, 128, 224, 164, 184, 224, 164, 176, 224, 165, 135,
    '-', 224, 164, 154, 224, 165, 140, 224, 164, 165, 224, 165,
    135, ' ', 224, 164, 184, 224, 165, 141, 224, 164, 165, 224,
    164, 190, 224, 164, 168, ' ', 224, 164, 170, 224, 164, 176,
    ' ', 224, 164, 185, 224, 165, 139, 224, 164, 168, 224, 165,
    135, ' ', 224, 164, 149, 224, 164, 190, ' ', 224, 164, 166,
    224, 164, 190, 224, 164, 181, 224, 164, 190, ' ', 224, 164,
    149, 224, 164, 176, 224, 164, 164, 224, 164, 190, ' ', 224,
    164, 185, 224, 165, 136, ' ', 224, 165, 164,  13,  10,  13,
     10, 224, 164, 173, 224, 164, 190, 224, 164, 176, 224, 164,
    164, ' ', 224, 164, 149, 224, 165, 135, ' ', 224, 164, 166,
    224, 165, 139, ' ', 224, 164, 134, 224, 164, 167, 224, 164,
    191, 224, 164, 149, 224, 164, 190, 224, 164, 176, 224, 164,
    191, 224, 164, 149, ' ', 224, 164, 168, 224, 164, 190, 224,
    164, 174, ' ', 224, 164, 185, 224, 165, 136, 224, 164, 130,
    '-', ' ', 224, 164, 185, 224, 164, 191, 224, 164, 168, 224,
    165, 141, 224, 164, 166, 224, 165, 128, ' ', 224, 164, 174,
    224, 165, 135, 224, 164, 130, ' ', 224, 164, 173, 224, 164,
    190, 224, 164, 176, 224, 164, 164, ' ', 224, 164, 148, 224,
    164, 176, ' ', 224, 164, 133, 224, 164, 130, 224, 164, 151,
    224, 165, 141, 224, 164, 176, 224, 165, 135, 224, 164, 156,
    224, 164, 188, 224, 165, 128, ' ', 224, 164, 174, 224, 165,
    135, 224, 164, 130, ' ', 224, 164, 135, 224, 164, 163, 224,
    165, 141, 224, 164, 161, 224, 164, 191, 224, 164, 175, 224,
    164, 190, ' ', '(', 'I', 'n', 'd', 'i', 'a', ')', 224, 165,
    164, ' ', 224, 164, 135, 224, 164, 163, 224, 165, 141, 224,
    164, 161, 224, 164, 191, 224, 164, 175, 224, 164, 190, ' ',
    224, 164, 168, 224, 164, 190, 224, 164, 174, ' ', 224, 164,
    149, 224, 165, 128, ' ', 224, 164, 137, 224, 164, 164, 224,
    165, 141, 224, 164, 170, 224, 164, 164, 224, 165, 141, 224,
    164, 164, 224, 164, 191, ' ', 224, 164, 184, 224, 164, 191,
    224, 164, 168, 224, 165, 141, 224, 164, 167, 224, 165, 129,
    ' ', 224, 164, 168, 224, 164, 166, 224, 165, 128, ' ', 224,
    164, 149, 224, 165, 135, ' ', 224, 164, 133, 224, 164, 130,
    224, 164, 151, 224, 165, 141, 224, 164, 176, 224, 165, 135,
    224, 164, 156, 224, 165, 128, ' ', 224, 164, 168, 224, 164,
    190, 224, 164, 174, ' ', '"', 224, 164, 135, 224, 164, 163,
    224, 165, 141, 224, 164, 161, 224, 164, 184, '"', ' ', 224,
    164, 184, 224, 165, 135, ' ', 224, 164, 185, 224, 165, 129,
    224, 164, 136, ' ', 224, 164, 185, 224, 165, 136, 224, 165,
    164, ' ', 224, 164, 173, 224, 164, 190, 224, 164, 176, 224,
    164, 164, ' ', 224, 164, 168, 224, 164, 190, 224, 164, 174,
    ',', ' ', 224, 164, 143, 224, 164, 149, ' ', 224, 164, 170,
    224, 165, 141, 224, 164, 176, 224, 164, 190, 224, 164, 154,
    224, 165, 128, 224, 164, 168, ' ', 224, 164, 185, 224, 164,
    191, 224, 164, 168, 224, 165, 141, 224, 164, 166, 224, 165,
    130, ' ', 224, 164, 184, 224, 164, 174, 224, 165, 141, 224,
    164, 176, 224, 164, 190, 224, 164, 159, ' ', 224, 164, 173,
    224, 164, 176, 224, 164, 164, ' ', 224, 164, 156, 224, 165,
    139, ' ', 224, 164, 149, 224, 164, 191, ' ', 224, 164, 174,
    224, 164, 168, 224, 165, 129, ' ', 224, 164, 149, 224, 165,
    135, ' ', 224, 164, 181, 224, 164, 130, 224, 164, 182, 224,
    164, 156, ' ', 224, 164, 139, 224, 164, 183, 224, 164, 173,
    224, 164, 166, 224, 165, 135, 224, 164, 181, ' ', 224, 164,
    149, 224, 165, 135, ' ', 224, 164, 156, 224, 165, 141, 224,
    164, 175, 224, 165, 135, 224, 164, 183, 224, 165, 141, 224,
    164, 160, ' ', 224, 164, 170, 224, 165, 129, 224, 164, 164,
    224, 165, 141, 224, 164, 176, ' ', 224, 164, 165, 224, 165,
    135, ' ', 224, 164, 164, 224, 164, 165, 224, 164, 190, ' ',
    224, 164, 156, 224, 164, 191, 224, 164, 168, 224, 164, 149,
    224, 165, 128, ' ', 224, 164, 149, 224, 164, 165, 224, 164,
    190, ' ', 224, 164, 182, 224, 165, 141, 224, 164, 176, 224,
    165, 128, 224, 164, 174, 224, 164, 166, 224, 165, 141, 224,
    164, 173, 224, 164, 190, 224, 164, 151, 224, 164, 181, 224,
    164, 164, ' ', 224, 164, 174, 224, 164, 185, 224, 164, 190,
    224, 164, 170, 224, 165, 129, 224, 164, 176, 224, 164, 190,
    224, 164, 163, ' ', 224, 164, 174, 224, 165, 135, 224, 164,
    130, ' ', 224, 164, 185, 224, 165, 136, ',', ' ', 224, 164,
    149, 224, 165, 135, ' ', 224, 164, 168, 224, 164, 190, 224,
    164, 174, ' ', 224, 164, 184, 224, 165, 135, ' ', 224, 164,
    178, 224, 164, 191, 224, 164, 175, 224, 164, 190, ' ', 224,
    164, 151, 224, 164, 175, 224, 164, 190, ' ', 224, 164, 185,
    224, 165, 136, 224, 165, 164, ' ', 224, 164, 173, 224, 164,
    190, 224, 164, 176, 224, 164, 164, ' ', '(', 224, 164, 173,
    224, 164, 190, ' ', '+', ' ', 224, 164, 176, 224, 164, 164,
    ')', ' ', 224, 164, 182, 224, 164, 172, 224, 165, 141, 224,
    164, 166, ' ', 224, 164, 149, 224, 164, 190, ' ', 224, 164,
    174, 224, 164, 164, 224, 164, 178, 224, 164, 172, ' ', 224,
    164, 185, 224, 165, 136, ' ', 224, 164, 134, 224, 164, 168,
    224, 165, 141, 224, 164, 164, 224, 164, 176, 224, 164, 191,
    224, 164, 149, ' ', 224, 164, 170, 224, 165, 141, 224, 164,
    176, 224, 164, 149, 224, 164, 190, 224, 164, 182, ' ', 224,
    164, 175, 224, 164, 190, ' ', 224, 164, 181, 224, 164, 191,
    224, 164, 166, 224, 165, 135, 224, 164, 149, '-', 224, 164,
    176, 224, 165, 130, 224, 164, 170, 224, 165, 128, ' ', 224,
    164, 170, 224, 165, 141, 224, 164, 176, 224, 164, 149, 224,
    164, 190, 224, 164, 182, ' ', 224, 164, 174, 224, 165, 135,
    224, 164, 130, ' ', 224, 164, 178, 224, 165, 128, 224, 164,
    168, 224, 165, 164, ' ', 224, 164, 143, 224, 164, 149, ' ',
    224, 164, 164, 224, 165, 128, 224, 164, 184, 224, 164, 176,
    224, 164, 190, ' ', 224, 164, 168, 224, 164, 190, 224, 164,
    174, ' ', 224, 164, 185, 224, 164, 191, 224, 164, 168, 224,
    165, 141, 224, 164, 166, 224, 165, 129, 224, 164, 184, 224,
    165, 141, 224, 164, 164, 224, 164, 190, 224, 164, 168, ' ',
    224, 164, 173, 224, 165, 128, ' ', 224, 164, 185, 224, 165,
    136, ' ', 224, 164, 156, 224, 164, 191, 224, 164, 184, 224,
    164, 149, 224, 164, 190, ' ', 224, 164, 133, 224, 164, 176,
    224, 165, 141, 224, 164, 165, ' ', 224, 164, 185, 224, 164,
    191, 224, 164, 168, 224, 165, 141, 224, 164, 166, '(', 224,
    164, 185, 224, 164, 191, 224, 164, 168, 224, 165, 141, 224,
    164, 166, 224, 165, 130, ')', ' ', 224, 164, 149, 224, 165,
    128, ' ', 224, 164, 173, 224, 165, 130, 224, 164, 174, 224,
    164, 191, ' ', 224, 164, 185, 224, 165, 139, 224, 164, 164,
    224, 164, 190, ' ', 224, 164, 185, 224, 165, 136, ' ', 224,
    164, 156, 224, 165, 139, ' ', 224, 164, 149, 224, 164, 191,
    ' ', 224, 164, 170, 224, 165, 141, 224, 164, 176, 224, 164,
    190, 224, 164, 154, 224, 165, 128, 224, 164, 168, ' ', 224,
    164, 149, 224, 164, 190, 224, 164, 178, ' ', 224, 164, 139,
    224, 164, 183, 224, 164, 191, 224, 164, 175, 224, 165, 139,
    224, 164, 130, ' ', 224, 164, 166, 224, 165, 141, 224, 164,
    181, 224, 164, 190, 224, 164, 176, 224, 164, 190, ' ', 224,
    164, 166, 224, 164, 191, 224, 164, 175, 224, 164, 190, ' ',
    224, 164, 151, 224, 164, 175, 224, 164, 190, ' ', 224, 164,
    165, 224, 164, 190, 224, 165, 164, ' ', 224, 164, 170, 224,
    165, 141, 224, 164, 176, 224, 164, 190, 224, 164, 154, 224,
    165, 128, 224, 164, 168, ' ', 224, 164, 149, 224, 164, 190,
    224, 164, 178, ' ', 224, 164, 174, 224, 165, 135, 224, 164,
    130, ' ', 224, 164, 175, 224, 164, 185, ' ', 224, 164, 149,
    224, 164, 174, ' ', 224, 164, 170, 224, 165, 141, 224, 164,
    176, 224, 164, 175, 224, 165, 129, 224, 164, 149, 224, 165,
    141, 224, 164, 164, ' ', 224, 164, 185, 224, 165, 139, 224,
    164, 164, 224, 164, 190, ' ', 224, 164, 165, 224, 164, 190,
    ' ', 224, 164, 164, 224, 164, 165, 224, 164, 190, ' ', 224,
    164, 149, 224, 164, 190, 224, 164, 178, 224, 164, 190, 224,
    164, 168, 224, 165, 141, 224, 164, 164, 224, 164, 176, ' ',
    224, 164, 174, 224, 165, 135, 224, 164, 130, ' ', 224, 164,
    133, 224, 164, 167, 224, 164, 191, 224, 164, 149, ' ', 224,
    164, 170, 224, 165, 141, 224, 164, 176, 224, 164, 154, 224,
    164, 178, 224, 164, 191, 224, 164, 164, ' ', 224, 164, 185,
    224, 165, 129, 224, 164, 134, ' ', 224, 164, 181, 224, 164,
    191, 224, 164, 182, 224, 165, 135, 224, 164, 183, 224, 164,
    149, 224, 164, 176, ' ', 224, 164, 133, 224, 164, 176, 224,
    164, 172, '/', 224, 164, 136, 224, 164, 176, 224, 164, 190,
    224, 164, 168, ' ', 224, 164, 174, 224, 165, 135, 224, 164,
    130, 224, 165, 164, ' ', 224, 164, 173, 224, 164, 190, 224,
    164, 176, 224, 164, 164, ' ', 224, 164, 174, 224, 165, 135,
    224, 164, 130, ' ', 224, 164, 175, 224, 164, 185, ' ', 224,
    164, 168, 224, 164, 190, 224, 164, 174, ' ', 224, 164, 174,
    224, 165, 129, 224, 164, 151, 224, 164, 178, ' ', 224, 164,
    149, 224, 164, 190, 224, 164, 178, ' ', 224, 164, 184, 224,
    165, 135, ' ', 224, 164, 133, 224, 164, 167, 224, 164, 191,
    224, 164, 149, ' ', 224, 164, 170, 224, 165, 141, 224, 164,
    176, 224, 164, 154, 224, 164, 178, 224, 164, 191, 224, 164,
    164, ' ', 224, 164, 185, 224, 165, 129, 224, 164, 134, ' ',
    224, 164, 175, 224, 164, 166, 224, 165, 141, 224, 164, 175,
    224, 164, 170, 224, 164, 191, ' ', 224, 164, 135, 224, 164,
    184, 224, 164, 149, 224, 164, 190, ' ', 224, 164, 184, 224,
    164, 174, 224, 164, 149, 224, 164, 190, 224, 164, 178, 224,
    165, 128, 224, 164, 168, ' ', 224, 164, 137, 224, 164, 170,
    224, 164, 175, 224, 165, 139, 224, 164, 151, ' ', 224, 164,
    149, 224, 164, 174, ' ', 224, 164, 148, 224, 164, 176, ' ',
    224, 164, 170, 224, 165, 141, 224, 164, 176, 224, 164, 190,
    224, 164, 175, 224, 164, 131, ' ', 224, 164, 137, 224, 164,
    164, 224, 165, 141, 224, 164, 164, 224, 164, 176, 224, 165,
    128, ' ', 224, 164, 173, 224, 164, 190, 224, 164, 176, 224,
    164, 164, ' ', 224, 164, 149, 224, 165, 135, ' ', 224, 164,
    178, 224, 164, 191, 224, 164, 143, ' ', 224, 164, 185, 224,
    165, 139, 224, 164, 164, 224, 164, 190, ' ', 224, 164, 185,
    224, 165, 136, 224, 165, 164, ' ', 224, 164, 135, 224, 164,
    184, 224, 164, 149, 224, 165, 135, ' ', 224, 164, 133, 224,
    164, 164, 224, 164, 191, 224, 164, 176, 224, 164, 191, 224,
    164, 149, 224, 165, 141, 224, 164, 164, ' ', 224, 164, 173,
    224, 164, 190, 224, 164, 176, 224, 164, 164, 224, 164, 181,
    224, 164, 176, 224, 165, 141, 224, 164, 183, ' ', 224, 164,
    149, 224, 165, 139, ' ', 224, 164, 181, 224, 165, 136, 224,
    164, 166, 224, 164, 191, 224, 164, 149, ' ', 224, 164, 149,
    224, 164, 190, 224, 164, 178, ' ', 224, 164, 184, 224, 165,
    135, ' ', 224, 164, 134, 224, 164, 176, 224, 165, 141, 224,
    164, 175, 224, 164, 190, 224, 164, 181, 224, 164, 176, 224,
    165, 141, 224, 164, 164, ' ', '"', 224, 164, 156, 224, 164,
    174, 224, 165, 141, 224, 164, 172, 224, 165, 130, 224, 164,
    166, 224, 165, 141, 224, 164, 181, 224, 165, 128, 224, 164,
    170, '"', ' ', 224, 164, 148, 224, 164, 176, ' ', '"', 224,
    164, 133, 224, 164, 156, 224, 164, 168, 224, 164, 190, 224,
    164, 173, 224, 164, 166, 224, 165, 135, 224, 164, 182, '"',
    ' ', 224, 164, 149, 224, 165, 135, ' ', 224, 164, 168, 224,
    164, 190, 224, 164, 174, ' ', 224, 164, 184, 224, 165, 135,
    ' ', 224, 164, 173, 224, 165, 128, ' ', 224, 164, 156, 224,
    164, 190, 224, 164, 168, 224, 164, 190, ' ', 224, 164, 156,
    224, 164, 190, 224, 164, 164, 224, 164, 190, ' ', 224, 164,
    176, 224, 164, 185, 224, 164, 190, ' ', 224, 164, 185, 224,
    165, 136, 224, 165, 164, ' ', 224, 164, 172, 224, 164, 185,
    224, 165, 129, 224, 164, 164, ' ', 224, 164, 170, 224, 164,
    185, 224, 164, 178, 224, 165, 135, ' ', 224, 164, 175, 224,
    164, 185, ' ', 224, 164, 166, 224, 165, 135, 224, 164, 182,
    ' ','\'', 224, 164, 184, 224, 165, 139, 224, 164, 168, 224,
    165, 135, ' ', 224, 164, 149, 224, 165, 128, ' ', 224, 164,
    154, 224, 164, 191, 224, 164, 161, 224, 164, 188, 224, 164,
    191, 224, 164, 175, 224, 164, 190,'\'', ' ', 224, 164, 149,
    224, 165, 135, ' ', 224, 164, 176, 224, 165, 130, 224, 164,
    170, ' ', 224, 164, 174, 224, 165, 135, 224, 164, 130, ' ',
    224, 164, 156, 224, 164, 190, 224, 164, 168, 224, 164, 190,
    ' ', 224, 164, 156, 224, 164, 190, 224, 164, 164, 224, 164,
    190, ' ', 224, 164, 165, 224, 164, 190, 224, 165, 164, '[',
    '2', ']',  13,  10,  13,  10, 'F', 'r', 'e', 'n', 'c', 'h',
    ':',  13,  10,  13,  10, 'L', 'a', ' ', 'F', 'r', 'a', 'n',
    'c', 'e', ',', ' ', 'e', 'n', ' ', 'f', 'o', 'r', 'm', 'e',
    ' ', 'l', 'o', 'n', 'g', 'u', 'e', ' ', 'l', 'a', ' ', 'R',
    195, 169, 'p', 'u', 'b', 'l', 'i', 'q', 'u', 'e', ' ', 'f',
    'r', 'a', 'n', 195, 167, 'a', 'i', 's', 'e', ',', ' ', 'e',
    's', 't', ' ', 'u', 'n', 'e', ' ', 'r', 195, 169, 'p', 'u',
    'b', 'l', 'i', 'q', 'u', 'e', ' ', 'c', 'o', 'n', 's', 't',
    'i', 't', 'u', 't', 'i', 'o', 'n', 'n', 'e', 'l', 'l', 'e',
    ' ', 'u', 'n', 'i', 't', 'a', 'i', 'r', 'e', ' ', 'd', 'o',
    'n', 't', ' ', 'l', 'a', ' ', 'm', 'a', 'j', 'e', 'u', 'r',
    'e', ' ', 'p', 'a', 'r', 't', 'i', 'e', ' ', 'd', 'u', ' ',
    't', 'e', 'r', 'r', 'i', 't', 'o', 'i', 'r', 'e', ' ', 'e',
    't', ' ', 'd', 'e', ' ', 'l', 'a', ' ', 'p', 'o', 'p', 'u',
    'l', 'a', 't', 'i', 'o', 'n', ' ', 's', 'o', 'n', 't', ' ',
    's', 'i', 't', 'u', 195, 169, 's', ' ', 'e', 'n', ' ', 'E',
    'u', 'r', 'o', 'p', 'e', ' ', 'o', 'c', 'c', 'i', 'd', 'e',
    'n', 't', 'a', 'l', 'e', ',', ' ', 'm', 'a', 'i', 's', ' ',
    'q', 'u', 'i', ' ', 'c', 'o', 'm', 'p', 'r', 'e', 'n', 'd',
    ' ', 195, 169, 'g', 'a', 'l', 'e', 'm', 'e', 'n', 't', ' ',
    'p', 'l', 'u', 's', 'i', 'e', 'u', 'r', 's', ' ', 'r', 195,
    169, 'g', 'i', 'o', 'n', 's', ' ', 'e', 't', ' ', 't', 'e',
    'r', 'r', 'i', 't', 'o', 'i', 'r', 'e', 's', ' ', 'r', 195,
    169, 'p', 'a', 'r', 't', 'i', 's', ' ', 'd', 'a', 'n', 's',
    ' ', 'l', 'e', 's', ' ', 'A', 'm', 195, 169, 'r', 'i', 'q',
    'u', 'e', 's', ',', ' ', 'l', 226, 128, 153, 'o', 'c', 195,
    169, 'a', 'n', ' ', 'I', 'n', 'd', 'i', 'e', 'n', ' ', 'e',
    't', ' ', 'l','\'', 'o', 'c', 195, 169, 'a', 'n', ' ', 'P',
    'a', 'c', 'i', 'f', 'i', 'q', 'u', 'e', '.', ' ', 'E', 'l',
    'l', 'e', ' ', 'a', ' ', 'p', 'o', 'u', 'r', ' ', 'c', 'a',
    'p', 'i', 't', 'a', 'l', 'e', ' ', 'P', 'a', 'r', 'i', 's',
    ',', ' ', 'p', 'o', 'u', 'r', ' ', 'l', 'a', 'n', 'g', 'u',
    'e', ' ', 'o', 'f', 'f', 'i', 'c', 'i', 'e', 'l', 'l', 'e',
    ' ', 'l', 'e', ' ', 'f', 'r', 'a', 'n', 195, 167, 'a', 'i',
    's', ' ', 'e', 't', ' ', 'p', 'o', 'u', 'r', ' ', 'm', 'o',
    'n', 'n', 'a', 'i', 'e', ' ', 'l', 226, 128, 153, 'e', 'u',
    'r', 'o', '.', ' ', 'S', 'a', ' ', 'd', 'e', 'v', 'i', 's',
    'e', ' ', 'e', 's', 't', ' ', 194, 171, ' ', 'L', 'i', 'b',
    'e', 'r', 't', 195, 169, ',', ' ', 195, 137, 'g', 'a', 'l',
    'i', 't', 195, 169, ',', ' ', 'F', 'r', 'a', 't', 'e', 'r',
    'n', 'i', 't', 195, 169, ' ', 194, 187, ',', ' ', 'e', 't',
    ' ', 's', 'o', 'n', ' ', 'd', 'r', 'a', 'p', 'e', 'a', 'u',
    ' ', 'e', 's', 't', ' ', 'c', 'o', 'n', 's', 't', 'i', 't',
    'u', 195, 169, ' ', 'd', 'e', ' ', 't', 'r', 'o', 'i', 's',
    ' ', 'b', 'a', 'n', 'd', 'e', 's', ' ', 'v', 'e', 'r', 't',
    'i', 'c', 'a', 'l', 'e', 's', ' ', 'r', 'e', 's', 'p', 'e',
    'c', 't', 'i', 'v', 'e', 'm', 'e', 'n', 't', ' ', 'b', 'l',
    'e', 'u', 'e', ',', ' ', 'b', 'l', 'a', 'n', 'c', 'h', 'e',
    ' ', 'e', 't', ' ', 'r', 'o', 'u', 'g', 'e', '.', ' ', 'S',
    'o', 'n', ' ', 'h', 'y', 'm', 'n', 'e', ' ', 'e', 's', 't',
    ' ', 'L', 'a', ' ', 'M', 'a', 'r', 's', 'e', 'i', 'l', 'l',
    'a', 'i', 's', 'e', '.', ' ', 'S', 'o', 'n', ' ', 'p', 'r',
    'i', 'n', 'c', 'i', 'p', 'e', ' ', 'e', 's', 't', ' ', 'g',
    'o', 'u', 'v', 'e', 'r', 'n', 'e', 'm', 'e', 'n', 't', ' ',
    'd', 'u', ' ', 'p', 'e', 'u', 'p', 'l', 'e', ',', ' ', 'p',
    'a', 'r', ' ', 'l', 'e', ' ', 'p', 'e', 'u', 'p', 'l', 'e',
    ' ', 'e', 't', ' ', 'p', 'o', 'u', 'r', ' ', 'l', 'e', ' ',
    'p', 'e', 'u', 'p', 'l', 'e', '[', '3', ']', '.',  13,  10,
     13,  10, 'L', 'a', ' ', 'F', 'r', 'a', 'n', 'c', 'e', ' ',
    'e', 's', 't', ' ', 'u', 'n', ' ', 'p', 'a', 'y', 's', ' ',
    'a', 'n', 'c', 'i', 'e', 'n', ',', ' ', 'f', 'o', 'r', 'm',
    195, 169, ' ', 'a', 'u', ' ', 'H', 'a', 'u', 't', ' ', 'M',
    'o', 'y', 'e', 'n', ' ', 195, 130, 'g', 'e', '.', ' ', 'A',
    'u', ' ', 'X', 'I', 'X', 'e', ' ', 's', 'i', 195, 168, 'c',
    'l', 'e', ' ', 'e', 't', ' ', 'd', 'a', 'n', 's', ' ', 'l',
    'a', ' ', 'p', 'r', 'e', 'm', 'i', 195, 168, 'r', 'e', ' ',
    'm', 'o', 'i', 't', 'i', 195, 169, ' ', 'd', 'u', ' ', 'X',
    'X', 'e', ' ', 's', 'i', 195, 168, 'c', 'l', 'e', ',', ' ',
    'e', 'l', 'l', 'e', ' ', 'p', 'o', 's', 's', 195, 168, 'd',
    'e', ' ', 'u', 'n', ' ', 'v', 'a', 's', 't', 'e', ' ', 'e',
    'm', 'p', 'i', 'r', 'e', ' ', 'c', 'o', 'l', 'o', 'n', 'i',
    'a', 'l', '.', ' ', 195, 128, ' ', 'p', 'a', 'r', 't', 'i',
    'r', ' ', 'd', 'e', 's', ' ', 'a', 'n', 'n', 195, 169, 'e',
    's', ' ', '1', '9', '5', '0', ',', ' ', 'e', 'l', 'l', 'e',
    ' ', 'e', 's', 't', ' ', 'l', 226, 128, 153, 'u', 'n', ' ',
    'd', 'e', 's', ' ', 'a', 'c', 't', 'e', 'u', 'r', 's', ' ',
    'd', 'e', ' ', 'l', 'a', ' ', 'c', 'o', 'n', 's', 't', 'r',
    'u', 'c', 't', 'i', 'o', 'n', ' ', 'd', 'e', ' ', 'l', 226,
    128, 153, 'U', 'n', 'i', 'o', 'n', ' ', 'e', 'u', 'r', 'o',
    'p', 195, 169, 'e', 'n', 'n', 'e', '.', ' ', 'E', 'l', 'l',
    'e', ' ', 'e', 's', 't', ' ', 'u', 'n', 'e', ' ', 'p', 'u',
    'i', 's', 's', 'a', 'n', 'c', 'e', ' ', 'n', 'u', 'c', 'l',
    195, 169, 'a', 'i', 'r', 'e', ',', ' ', 'e', 't', ' ', 'l',
    226, 128, 153, 'u', 'n', ' ', 'd', 'e', 's', ' ', 'c', 'i',
    'n', 'q', ' ', 'm', 'e', 'm', 'b', 'r', 'e', 's', ' ', 'p',
    'e', 'r', 'm', 'a', 'n', 'e', 'n', 't', 's', ' ', 'd', 'u',
    ' ', 'C', 'o', 'n', 's', 'e', 'i', 'l', ' ', 'd', 'e', ' ',
    's', 195, 169, 'c', 'u', 'r', 'i', 't', 195, 169, ' ', 'd',
    'e', 's', ' ', 'N', 'a', 't', 'i', 'o', 'n', 's', ' ', 'u',
    'n', 'i', 'e', 's', '.', ' ', 'L', 'a', ' ', 'F', 'r', 'a',
    'n', 'c', 'e', ' ', 'j', 'o', 'u', 'e', ' ', 'u', 'n', ' ',
    'r', 195, 180, 'l', 'e', ' ', 'i', 'm', 'p', 'o', 'r', 't',
    'a', 'n', 't', ' ', 'd', 'a', 'n', 's', ' ', 'l', 226, 128,
    153, 'h', 'i', 's', 't', 'o', 'i', 'r', 'e', ' ', 'm', 'o',
    'n', 'd', 'i', 'a', 'l', 'e', ' ', 'p', 'a', 'r', ' ', 'l',
    226, 128, 153, 'i', 'n', 'f', 'l', 'u', 'e', 'n', 'c', 'e',
    ' ', 'd', 'e', ' ', 's', 'a', ' ', 'c', 'u', 'l', 't', 'u',
    'r', 'e', ' ', 'e', 't', ' ', 'd', 'e', ' ', 's', 'e', 's',
    ' ', 'v', 'a', 'l', 'e', 'u', 'r', 's', ' ', 'd', 195, 169,
    'm', 'o', 'c', 'r', 'a', 't', 'i', 'q', 'u', 'e', 's', ',',
    ' ', 'l', 'a', 195, 175, 'q', 'u', 'e', 's', ' ', 'e', 't',
    ' ', 'r', 195, 169, 'p', 'u', 'b', 'l', 'i', 'c', 'a', 'i',
    'n', 'e', 's', '.',  13,  10,  13,  10, 'L', 'a', ' ', 'F',
    'r', 'a', 'n', 'c', 'e', ' ', 'a', ',', ' ', 'e', 'n', ' ',
    '2', '0', '1', '0', ',', ' ', 'l', 'e', ' ', 'c', 'i', 'n',
    'q', 'u', 'i', 195, 168, 'm', 'e', ' ', 'p', 'l', 'u', 's',
    ' ', 'i', 'm', 'p', 'o', 'r', 't', 'a', 'n', 't', ' ', 'p',
    'r', 'o', 'd', 'u', 'i', 't', ' ', 'i', 'n', 't', 195, 169,
    'r', 'i', 'e', 'u', 'r', ' ', 'b', 'r', 'u', 't', ' ', 'a',
    'u', ' ', 'm', 'o', 'n', 'd', 'e', '.', ' ', 'S', 'o', 'n',
    ' ', 195, 169, 'c', 'o', 'n', 'o', 'm', 'i', 'e', ',', ' ',
    'd', 'e', ' ', 't', 'y', 'p', 'e', ' ', 'c', 'a', 'p', 'i',
    't', 'a', 'l', 'i', 's', 't', 'e', ' ', 'a', 'v', 'e', 'c',
    ' ', 'u', 'n', 'e', ' ', 'i', 'n', 't', 'e', 'r', 'v', 'e',
    'n', 't', 'i', 'o', 'n', ' ', 195, 169, 't', 'a', 't', 'i',
    'q', 'u', 'e', ' ', 'a', 's', 's', 'e', 'z', ' ', 'f', 'o',
    'r', 't', 'e', ',', ' ', 'f', 'a', 'i', 't', ' ', 'd', 226,
    128, 153, 'e', 'l', 'l', 'e', ' ', 'u', 'n', ' ', 'd', 'e',
    's', ' ', 'l', 'e', 'a', 'd', 'e', 'r', 's', ' ', 'm', 'o',
    'n', 'd', 'i', 'a', 'u', 'x', ' ', 'd', 'a', 'n', 's', ' ',
    'l', 'e', 's', ' ', 's', 'e', 'c', 't', 'e', 'u', 'r', 's',
    ' ', 'd', 'e', ' ', 'l', 226, 128, 153, 'a', 'g', 'r', 'o',
    'a', 'l', 'i', 'm', 'e', 'n', 't', 'a', 'i', 'r', 'e', ',',
    ' ', 'd', 'e', ' ', 'l', 226, 128, 153, 'a', 195, 169, 'r',
    'o', 'n', 'a', 'u', 't', 'i', 'q', 'u', 'e', ',', ' ', 'd',
    'e', ' ', 'l', 226, 128, 153, 'a', 'u', 't', 'o', 'm', 'o',
    'b', 'i', 'l', 'e', ',', ' ', 'd', 'e', 's', ' ', 'p', 'r',
    'o', 'd', 'u', 'i', 't', 's', ' ', 'd', 'e', ' ', 'l', 'u',
    'x', 'e', ',', ' ', 'd', 'u', ' ', 't', 'o', 'u', 'r', 'i',
    's', 'm', 'e', ' ', 'e', 't', ' ', 'd', 'u', ' ', 'n', 'u',
    'c', 'l', 195, 169, 'a', 'i', 'r', 'e', '.',  13,  10,  13,
     10, 'P', 'e', 'u', 'p', 'l', 195, 169, 'e', ' ', 'd', 'e',
    ' ', '6', '5', ',', '3', ' ', 'm', 'i', 'l', 'l', 'i', 'o',
    'n', 's', ' ', 'd', 226, 128, 153, 'h', 'a', 'b', 'i', 't',
    'a', 'n', 't', 's', ' ', 'a', 'u', ' ', '1', 'e', 'r', ' ',
    'j', 'a', 'n', 'v', 'i', 'e', 'r', ' ', '2', '0', '1', '2',
    '[', '4', ']', ',', ' ', 'l', 'a', ' ', 'F', 'r', 'a', 'n',
    'c', 'e', ' ', 'e', 's', 't', ' ', 'u', 'n', ' ', 'p', 'a',
    'y', 's', ' ', 'd', 195, 169, 'v', 'e', 'l', 'o', 'p', 'p',
    195, 169, ',', ' ', 'a', 'v', 'e', 'c', ' ', 'u', 'n', ' ',
    'i', 'n', 'd', 'i', 'c', 'e', ' ', 'd', 'e', ' ', 'd', 195,
    169, 'v', 'e', 'l', 'o', 'p', 'p', 'e', 'm', 'e', 'n', 't',
    ' ', 'h', 'u', 'm', 'a', 'i', 'n', ' ', 't', 'r', 195, 168,
    's', ' ', 195, 169, 'l', 'e', 'v', 195, 169, '[', '5', ']',
    '.',  13,  10,  13,  10, 'L', 'a', ' ', 'F', 'r', 'a', 'n',
    'c', 'e', ' ', 'm', 195, 169, 't', 'r', 'o', 'p', 'o', 'l',
    'i', 't', 'a', 'i', 'n', 'e', ' ', 'e', 's', 't', ' ', 's',
    'i', 't', 'u', 195, 169, 'e', ' ', 195, 160, ' ', 'l', 226,
    128, 153, 'u', 'n', 'e', ' ', 'd', 'e', 's', ' ', 'e', 'x',
    't', 'r', 195, 169, 'm', 'i', 't', 195, 169, 's', ' ', 'o',
    'c', 'c', 'i', 'd', 'e', 'n', 't', 'a', 'l', 'e', 's', ' ',
    'd', 'e', ' ', 'l', 226, 128, 153, 'E', 'u', 'r', 'o', 'p',
    'e', '.', ' ', 'E', 'l', 'l', 'e', ' ', 'e', 's', 't', ' ',
    'b', 'o', 'r', 'd', 195, 169, 'e', ' ', 'p', 'a', 'r', ' ',
    'l', 'a', ' ', 'm', 'e', 'r', ' ', 'd', 'u', ' ', 'N', 'o',
    'r', 'd', ' ', 'a', 'u', ' ', 'n', 'o', 'r', 'd', ',', ' ',
    'l', 'a', ' ', 'M', 'a', 'n', 'c', 'h', 'e', ' ', 'a', 'u',
    ' ', 'n', 'o', 'r', 'd', '-', 'o', 'u', 'e', 's', 't', ',',
    ' ', 'l', 226, 128, 153, 'o', 'c', 195, 169, 'a', 'n', ' ',
    'A', 't', 'l', 'a', 'n', 't', 'i', 'q', 'u', 'e', ' ', 195,
    160, ' ', 'l', 226, 128, 153, 'o', 'u', 'e', 's', 't', ' ',
    'e', 't', ' ', 'l', 'a', ' ', 'm', 'e', 'r', ' ', 'M', 195,
    169, 'd', 'i', 't', 'e', 'r', 'r', 'a', 'n', 195, 169, 'e',
    ' ', 'a', 'u', ' ', 's', 'u', 'd', '-', 'e', 's', 't', '.',
    ' ', 'E', 'l', 'l', 'e', ' ', 'e', 's', 't', ' ', 'f', 'r',
    'o', 'n', 't', 'a', 'l', 'i', 195, 168, 'r', 'e', ' ', 'd',
    'e', ' ', 'l', 'a', ' ', 'B', 'e', 'l', 'g', 'i', 'q', 'u',
    'e', ' ', 'e', 't', ' ', 'd', 'u', ' ', 'L', 'u', 'x', 'e',
    'm', 'b', 'o', 'u', 'r', 'g', ' ', 'a', 'u', ' ', 'n', 'o',
    'r', 'd', '-', 'e', 's', 't', ',', ' ', 'd', 'e', ' ', 'l',
    226, 128, 153, 'A', 'l', 'l', 'e', 'm', 'a', 'g', 'n', 'e',
    ' ', 'e', 't', ' ', 'd', 'e', ' ', 'l', 'a', ' ', 'S', 'u',
    'i', 's', 's', 'e', ' ', 195, 160, ' ', 'l', 226, 128, 153,
    'e', 's', 't', ',', ' ', 'd', 'e', ' ', 'l', 226, 128, 153,
    'I', 't', 'a', 'l', 'i', 'e', ' ', 'e', 't', ' ', 'd', 'e',
    ' ', 'M', 'o', 'n', 'a', 'c', 'o', ' ', 'a', 'u', ' ', 's',
    'u', 'd', '-', 'e', 's', 't', ',', ' ', 'd', 'e', ' ', 'l',
    226, 128, 153, 'E', 's', 'p', 'a', 'g', 'n', 'e', ' ', 'e',
    't', ' ', 'd', 226, 128, 153, 'A', 'n', 'd', 'o', 'r', 'r',
    'e', ' ', 'a', 'u', ' ', 's', 'u', 'd', '-', 'o', 'u', 'e',
    's', 't', '.', ' ', 'S', 'i', ' ', 'l', 'e', 's', ' ', 'f',
    'r', 'o', 'n', 't', 'i', 195, 168, 'r', 'e', 's', ' ', 'd',
    'u', ' ', 's', 'u', 'd', ' ', 'd', 'u', ' ', 'p', 'a', 'y',
    's', ' ', 'c', 'o', 'r', 'r', 'e', 's', 'p', 'o', 'n', 'd',
    'e', 'n', 't', ' ', 195, 160, ' ', 'd', 'e', 's', ' ', 'm',
    'a', 's', 's', 'i', 'f', 's', ' ', 'm', 'o', 'n', 't', 'a',
    'g', 'n', 'e', 'u', 'x', ',', ' ', 'l', 'e', 's', ' ', 'f',
    'r', 'o', 'n', 't', 'i', 195, 168, 'r', 'e', 's', ' ', 'd',
    'u', ' ', 'n', 'o', 'r', 'd', '-', 'e', 's', 't', ' ', 'n',
    'e', ' ', 'c', 'o', 'r', 'r', 'e', 's', 'p', 'o', 'n', 'd',
    'e', 'n', 't', ' ', 195, 160, ' ', 'a', 'u', 'c', 'u', 'n',
    'e', ' ', 'l', 'i', 'm', 'i', 't', 'e', ' ', 'g', 195, 169,
    'o', 'g', 'r', 'a', 'p', 'h', 'i', 'q', 'u', 'e', '[', 'n',
    'o', 't', 'e', ' ', '6', ']', ' ', 'n', 'i', ' ', 'l', 'i',
    'n', 'g', 'u', 'i', 's', 't', 'i', 'q', 'u', 'e', '[', 'n',
    'o', 't', 'e', ' ', '7', ']', '.', ' ', 'L', 'a', ' ', 'F',
    'r', 'a', 'n', 'c', 'e', ' ', 'm', 195, 169, 't', 'r', 'o',
    'p', 'o', 'l', 'i', 't', 'a', 'i', 'n', 'e', ' ', 'c', 'o',
    'm', 'p', 'r', 'e', 'n', 'd', ' ', 'p', 'l', 'u', 's', 'i',
    'e', 'u', 'r', 's', ' ', 195, 174, 'l', 'e', 's', ',', ' ',
    'n', 'o', 't', 'a', 'm', 'm', 'e', 'n', 't', ' ', 'l', 'a',
    ' ', 'C', 'o', 'r', 's', 'e', ' ', 'e', 't', ' ', 'd', 'e',
    's', ' ', 195, 174, 'l', 'e', 's', ' ', 'c', 195, 180, 't',
    'i', 195, 168, 'r', 'e', 's', '.', ' ', 'L', 'a', ' ', 'm',
    195, 169, 't', 'r', 'o', 'p', 'o', 'l', 'e', ' ', 'e', 's',
    't', ' ', 'c', 'o', 'm', 'p', 'r', 'i', 's', 'e', ' ', 'e',
    'n', 't', 'r', 'e', ' ', 'l', 'e', 's', ' ', 'l', 'a', 't',
    'i', 't', 'u', 'd', 'e', 's', ' ', '4', '2', 194, 176, '1',
    '9','\'', '4', '6', '"', ' ', 'N', ' ', 'e', 't', ' ', '5',
    '1', 194, 176, '5','\'', '4', '7', '"', ' ', 'N', ',', ' ',
    'a', 'i', 'n', 's', 'i', ' ', 'q', 'u', 'e', ' ', 'l', 'e',
    's', ' ', 'l', 'o', 'n', 'g', 'i', 't', 'u', 'd', 'e', 's',
    ' ', '4', 194, 176, '4', '6','\'', ' ', 'O', ' ', 'e', 't',
    ' ', '8', 194, 176, '1', '4','\'', '4', '2', '"', ' ', 'E',
    '.',  13,  10,  13,  10, 'L', 'a', ' ', 'F', 'r', 'a', 'n',
    'c', 'e', ' ', 'c', 'o', 'm', 'p', 'r', 'e', 'n', 'd', ' ',
    195, 169, 'g', 'a', 'l', 'e', 'm', 'e', 'n', 't', ' ', 'd',
    'e', ' ', 'n', 'o', 'm', 'b', 'r', 'e', 'u', 'x', ' ', 't',
    'e', 'r', 'r', 'i', 't', 'o', 'i', 'r', 'e', 's', ' ', 's',
    'i', 't', 'u', 195, 169, 's', ' ', 'e', 'n', '-', 'd', 'e',
    'h', 'o', 'r', 's', ' ', 'd', 'u', ' ', 'c', 'o', 'n', 't',
    'i', 'n', 'e', 'n', 't', ' ', 'e', 'u', 'r', 'o', 'p', 195,
    169, 'e', 'n', ',', ' ', 'c', 'o', 'u', 'r', 'a', 'm', 'm',
    'e', 'n', 't', ' ', 'a', 'p', 'p', 'e', 'l', 195, 169, 's',
    ' ', 't', 'e', 'r', 'r', 'i', 't', 'o', 'i', 'r', 'e', 's',
    ' ', 'd', 226, 128, 153, 'o', 'u', 't', 'r', 'e', '-', 'm',
    'e', 'r', ',', ' ', 'n', 'a', 'g', 'u', 195, 168, 'r', 'e',
    ' ', 'D', 'O', 'M', '-', 'T', 'O', 'M', ',', ' ', 'q', 'u',
    'i', ' ', 'l', 'u', 'i', ' ', 'p', 'e', 'r', 'm', 'e', 't',
    't', 'e', 'n', 't', ' ', 'd', 226, 128, 153, 195, 170, 't',
    'r', 'e', ' ', 'p', 'r', 195, 169, 's', 'e', 'n', 't', 'e',
    ' ', 'd', 'a', 'n', 's', ' ', 't', 'o', 'u', 's', ' ', 'l',
    'e', 's', ' ', 'o', 'c', 195, 169, 'a', 'n', 's', '.', ' ',
    'C', 'e', 's', ' ', 't', 'e', 'r', 'r', 'i', 't', 'o', 'i',
    'r', 'e', 's', ' ', 'a', 'u', 'x', ' ', 's', 't', 'a', 't',
    'u', 't', 's', ' ', 'v', 'a', 'r', 'i', 195, 169, 's', ' ',
    's', 'o', 'n', 't', '[', '6', ']', ' ', ':',  13,  10,  13,
     10, 's', 'u', 'r', ' ', 'l', 'e', ' ', 'c', 'o', 'n', 't',
    'i', 'n', 'e', 'n', 't', ' ', 's', 'u', 'd', '-', 'a', 'm',
    195, 169, 'r', 'i', 'c', 'a', 'i', 'n', ' ', ':', ' ', 'l',
    'a', ' ', 'G', 'u', 'y', 'a', 'n', 'e', ' ', ';',  13,  10,
    'd', 'a', 'n', 's', ' ', 'l', 226, 128, 153, 'o', 'c', 195,
    169, 'a', 'n', ' ', 'A', 't', 'l', 'a', 'n', 't', 'i', 'q',
    'u', 'e', ' ', '(', 'A', 'n', 't', 'i', 'l', 'l', 'e', 's',
    ')', ' ', ':', ' ', 'l', 'a', ' ', 'G', 'u', 'a', 'd', 'e',
    'l', 'o', 'u', 'p', 'e', ',', ' ', 'l', 'a', ' ', 'M', 'a',
    'r', 't', 'i', 'n', 'i', 'q', 'u', 'e', ',', ' ', 'S', 'a',
    'i', 'n', 't', '-', 'P', 'i', 'e', 'r', 'r', 'e', '-', 'e',
    't', '-', 'M', 'i', 'q', 'u', 'e', 'l', 'o', 'n', ',', ' ',
    'S', 'a', 'i', 'n', 't', '-', 'M', 'a', 'r', 't', 'i', 'n',
    ' ', 'e', 't', ' ', 'S', 'a', 'i', 'n', 't', '-', 'B', 'a',
    'r', 't', 'h', 195, 169, 'l', 'e', 'm', 'y', ' ', ';',  13,
     10, 'd', 'a', 'n', 's', ' ', 'l', 226, 128, 153, 'o', 'c',
    195, 169, 'a', 'n', ' ', 'P', 'a', 'c', 'i', 'f', 'i', 'q',
    'u', 'e', ' ', ':', ' ', 'l', 'a', ' ', 'P', 'o', 'l', 'y',
    'n', 195, 169, 's', 'i', 'e', ' ', 'f', 'r', 'a', 'n', 195,
    167, 'a', 'i', 's', 'e', ',', ' ', 'l', 'a', ' ', 'N', 'o',
    'u', 'v', 'e', 'l', 'l', 'e', '-', 'C', 'a', 'l', 195, 169,
    'd', 'o', 'n', 'i', 'e', ',', ' ', 'W', 'a', 'l', 'l', 'i',
    's', '-', 'e', 't', '-', 'F', 'u', 't', 'u', 'n', 'a', ' ',
    'e', 't', ' ', 'C', 'l', 'i', 'p', 'p', 'e', 'r', 't', 'o',
    'n', ' ', ';',  13,  10, 'd', 'a', 'n', 's', ' ', 'l', 226,
    128, 153, 'o', 'c', 195, 169, 'a', 'n', ' ', 'I', 'n', 'd',
    'i', 'e', 'n', ' ', ':', ' ', 'L', 'a', ' ', 'R', 195, 169,
    'u', 'n', 'i', 'o', 'n', ',', ' ', 'M', 'a', 'y', 'o', 't',
    't', 'e', ',', ' ', 'l', 'e', 's', ' ', 195, 142, 'l', 'e',
    's', ' ', 195, 137, 'p', 'a', 'r', 's', 'e', 's', ',', ' ',
    'l', 'e', 's', ' ', 195, 142, 'l', 'e', 's', ' ', 'C', 'r',
    'o', 'z', 'e', 't', ',', ' ', 'l', 'e', 's', ' ', 195, 142,
    'l', 'e', 's', ' ', 'K', 'e', 'r', 'g', 'u', 'e', 'l', 'e',
    'n', ' ', 'e', 't', ' ', 'S', 'a', 'i', 'n', 't', '-', 'P',
    'a', 'u', 'l', '-', 'e', 't', '-', 'A', 'm', 's', 't', 'e',
    'r', 'd', 'a', 'm', ' ', ';',  13,  10, 'e', 'n', ' ', 'A',
    'n', 't', 'a', 'r', 'c', 't', 'i', 'q', 'u', 'e', ' ', ':',
    ' ', 'l', 'a', ' ', 'T', 'e', 'r', 'r', 'e', ' ', 'A', 'd',
    195, 169, 'l', 'i', 'e', '[', 'n', 'o', 't', 'e', ' ', '8',
    ']', '.',  13,  10, 195, 128, ' ', 't', 'r', 'a', 'v', 'e',
    'r', 's', ' ', 's', 'e', 's', ' ', 'c', 'o', 'l', 'l', 'e',
    'c', 't', 'i', 'v', 'i', 't', 195, 169, 's', ' ', 'u', 'l',
    't', 'r', 'a', '-', 'm', 'a', 'r', 'i', 'n', 'e', 's', ',',
    ' ', 'l', 'a', ' ', 'F', 'r', 'a', 'n', 'c', 'e', ' ', 'p',
    'o', 's', 's', 195, 168, 'd', 'e', ' ', 195, 169, 'g', 'a',
    'l', 'e', 'm', 'e', 'n', 't', ' ', 'd', 'e', 's', ' ', 'f',
    'r', 'o', 'n', 't', 'i', 195, 168, 'r', 'e', 's', ' ', 't',
    'e', 'r', 'r', 'e', 's', 't', 'r', 'e', 's', ' ', 'a', 'v',
    'e', 'c', ' ', 'l', 'e', ' ', 'B', 'r', 195, 169, 's', 'i',
    'l', ' ', 'e', 't', ' ', 'l', 'e', ' ', 'S', 'u', 'r', 'i',
    'n', 'a', 'm', 'e', ',', ' ', 'a', 'i', 'n', 's', 'i', ' ',
    'q', 'u', 226, 128, 153, 'a', 'v', 'e', 'c', ' ', 'l', 'e',
    's', ' ', 'P', 'a', 'y', 's', '-', 'B', 'a', 's', ' ', 'v',
    'i', 'a', ' ', 'l', 'a', ' ', 'p', 'a', 'r', 't', 'i', 'e',
    ' ', 'f', 'r', 'a', 'n', 195, 167, 'a', 'i', 's', 'e', ' ',
    'd', 'e', ' ', 'S', 'a', 'i', 'n', 't', '-', 'M', 'a', 'r',
    't', 'i', 'n', '.',  13,  10,  13,  10, 'L', 'a', ' ', 's',
    'u', 'p', 'e', 'r', 'f', 'i', 'c', 'i', 'e', ' ', 'd', 'e',
    ' ', 'l', 'a', ' ', 'F', 'r', 'a', 'n', 'c', 'e', ' ', 'e',
    's', 't', ' ', 'd', 'e', ' ', '6', '7', '0', ' ', '9', '2',
    '2', ' ', 'k', 'm', 194, 178, ',', ' ', 'o', 'u', ' ', '5',
    '4', '7', ' ', '0', '3', '0', ' ', 's', 'a', 'n', 's', ' ',
    'c', 'o', 'm', 'p', 't', 'a', 'b', 'i', 'l', 'i', 's', 'e',
    'r', ' ', 'l', 226, 128, 153, 'o', 'u', 't', 'r', 'e', '-',
    'm', 'e', 'r', '[', '7', ']', '.', ' ', 'E', 'l', 'l', 'e',
    ' ', 'e', 's', 't', ' ', 'l', 'e', ' ', '4', '1', 'e', ' ',
    'p', 'l', 'u', 's', ' ', 'g', 'r', 'a', 'n', 'd', ' ', 195,
    137, 't', 'a', 't', ' ', 'd', 'u', ' ', 'm', 'o', 'n', 'd',
    'e', ' ', 'p', 'a', 'r', ' ', 's', 'a', ' ', 's', 'u', 'r',
    'f', 'a', 'c', 'e', ' ', 't', 'e', 'r', 'r', 'e', 's', 't',
    'r', 'e', '[', '7', ']', ' ', 'e', 't', ' ', 'l', 'e', ' ',
    'd', 'e', 'u', 'x', 'i', 195, 168, 'm', 'e', ' ', 'p', 'a',
    'r', ' ', 's', 'a', ' ', 'z', 'o', 'n', 'e', ' ', 195, 169,
    'c', 'o', 'n', 'o', 'm', 'i', 'q', 'u', 'e', ' ', 'e', 'x',
    'c', 'l', 'u', 's', 'i', 'v', 'e', '[', '8', ']', '.', ' ',
    'E', 'l', 'l', 'e', ' ', 'e', 's', 't', ' ', 'e', 'n', ' ',
    'o', 'u', 't', 'r', 'e', ' ', 'l', 'e', ' ', 't', 'r', 'o',
    'i', 's', 'i', 195, 168, 'm', 'e', ' ', 'p', 'l', 'u', 's',
    ' ', 'g', 'r', 'a', 'n', 'd', ' ', 'p', 'a', 'y', 's', ' ',
    'd', 226, 128, 153, 'E', 'u', 'r', 'o', 'p', 'e', ',', ' ',
    'a', 'p', 'r', 195, 168, 's', ' ', 'l', 'a', ' ', 'R', 'u',
    's', 's', 'i', 'e', ' ', 'e', 't', ' ', 'l', 226, 128, 153,
    'U', 'k', 'r', 'a', 'i', 'n', 'e', ',', ' ', 'd', 'e', 'u',
    'x', 'i', 195, 168, 'm', 'e', ' ', 's', 'i', ' ', 'o', 'n',
    ' ', 'i', 'n', 'c', 'l', 'u', 't', ' ', 'l', 'e', 's', ' ',
    'd', 195, 169, 'p', 'a', 'r', 't', 'e', 'm', 'e', 'n', 't',
    's', ' ', 'u', 'l', 't', 'r', 'a', '-', 'm', 'a', 'r', 'i',
    'n', 's', ',', ' ', 'e', 't', ' ', 'l', 'e', ' ', 'p', 'l',
    'u', 's', ' ', 'g', 'r', 'a', 'n', 'd', ' ', 'd', 'e', ' ',
    'l', 226, 128, 153, 'U', 'n', 'i', 'o', 'n', ' ', 'e', 'u',
    'r', 'o', 'p', 195, 169, 'e', 'n', 'n', 'e', '[', '7', ']',
    '.', ' ', 'S', 'o', 'n', ' ', 't', 'e', 'r', 'r', 'i', 't',
    'o', 'i', 'r', 'e', ' ', 'm', 195, 169, 't', 'r', 'o', 'p',
    'o', 'l', 'i', 't', 'a', 'i', 'n', ' ', 'c', 'o', 'n', 't',
    'i', 'n', 'e', 'n', 't', 'a', 'l', ' ', 's', 226, 128, 153,
    195, 169, 't', 'e', 'n', 'd', ' ', 's', 'u', 'r', ' ', 'e',
    'n', 'v', 'i', 'r', 'o', 'n', ' ', '1', ' ', '0', '0', '0',
    ' ', 'k', 'm', ' ', 'd', 'u', ' ', 'n', 'o', 'r', 'd', ' ',
    'a', 'u', ' ', 's', 'u', 'd', ' ', 'e', 't', ' ', 'd', 226,
    128, 153, 'e', 's', 't', ' ', 'e', 'n', ' ', 'o', 'u', 'e',
    's', 't', '.', ' ', 'L', 226, 128, 153, 195, 169, 't', 'e',
    'n', 'd', 'u', 'e', ' ', 'd', 'e', ' ', 's', 'o', 'n', ' ',
    'l', 'i', 't', 't', 'o', 'r', 'a', 'l', ',', ' ', 'o', 'u',
    't', 'r', 'e', '-', 'm', 'e', 'r', ' ', 'i', 'n', 'c', 'l',
    'u', 's', ',', ' ', 'e', 's', 't', ' ', 'd', 'e', ' ', '8',
    ' ', '2', '4', '5', ' ', 'k', 'm', '[', '9', ']', '.',  13,
     10,  13,  10, 'G', 'r', 'e', 'e', 'k', ':',  13,  10,  13,
     10, 206, 151, ' ', 206, 149, 206, 187, 206, 187, 206, 172,
    206, 180, 206, 177, ' ', '(', 207, 128, 206, 177, 206, 187,
    206, 177, 206, 185, 207, 140, 207, 132, 206, 181, 207, 129,
    206, 177, ':', ' ', 225, 188, 153, 206, 187, 206, 187, 206,
    172, 207, 130, ',', ' ', 206, 181, 207, 128, 206, 175, 207,
    131, 206, 183, 206, 188, 206, 177, ':', ' ', 206, 149, 206,
    187, 206, 187, 206, 183, 206, 189, 206, 185, 206, 186, 206,
    174, ' ', 206, 148, 206, 183, 206, 188, 206, 191, 206, 186,
    207, 129, 206, 177, 207, 132, 206, 175, 206, 177, ')', ' ',
    206, 181, 206, 175, 206, 189, 206, 177, 206, 185, ' ', 207,
    135, 207, 142, 207, 129, 206, 177, ' ', 207, 128, 206, 191,
    207, 133, ' ', 206, 178, 207, 129, 206, 175, 207, 131, 206,
    186, 206, 181, 207, 132, 206, 177, 206, 185, ' ', 207, 131,
    207, 132, 206, 183, ' ', 206, 189, 206, 191, 207, 132, 206,
    185, 206, 191, 206, 177, 206, 189, 206, 177, 207, 132, 206,
    191, 206, 187, 206, 185, 206, 186, 206, 174, ' ', 206, 149,
    207, 133, 207, 129, 207, 142, 207, 128, 206, 183, ',', ' ',
    207, 131, 207, 132, 206, 191, ' ', 206, 189, 206, 191, 207,
    132, 206, 185, 207, 140, 207, 132, 206, 181, 207, 129, 206,
    191, ' ', 206, 172, 206, 186, 207, 129, 206, 191, ' ', 207,
    132, 206, 183, 207, 130, ' ', 206, 146, 206, 177, 206, 187,
    206, 186, 206, 177, 206, 189, 206, 185, 206, 186, 206, 174,
    207, 130, ' ', 207, 135, 206, 181, 207, 129, 207, 131, 206,
    191, 206, 189, 206, 174, 207, 131, 206, 191, 207, 133, ',',
    ' ', 207, 131, 207, 132, 206, 183, 206, 189, ' ', 206, 145,
    206, 189, 206, 177, 207, 132, 206, 191, 206, 187, 206, 185,
    206, 186, 206, 174, ' ', 206, 156, 206, 181, 207, 131, 207,
    140, 206, 179, 206, 181, 206, 185, 206, 191, '.', 206, 160,
    207, 129, 207, 137, 207, 132, 206, 181, 207, 141, 206, 191,
    207, 133, 207, 131, 206, 177, ' ', 207, 132, 206, 183, 207,
    130, ' ', 206, 149, 206, 187, 206, 187, 206, 172, 206, 180,
    206, 191, 207, 130, ' ', 206, 186, 206, 177, 206, 185, ' ',
    206, 188, 206, 181, 206, 179, 206, 177, 206, 187, 207, 141,
    207, 132, 206, 181, 207, 129, 206, 183, ' ', 207, 128, 207,
    140, 206, 187, 206, 183, ' ', 206, 181, 206, 175, 206, 189,
    206, 177, 206, 185, ' ', 206, 183, ' ', 206, 145, 206, 184,
    206, 174, 206, 189, 206, 177, '.',  13,  10,  13,  10, 206,
    163, 207, 133, 206, 189, 206, 191, 207, 129, 206, 181, 207,
    141, 206, 181, 206, 185, ' ', 207, 131, 207, 132, 206, 177,
    ' ', 206, 178, 206, 191, 207, 129, 206, 181, 206, 185, 206,
    191, 206, 180, 207, 133, 207, 132, 206, 185, 206, 186, 206,
    172, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183, 206,
    189, ' ', 206, 145, 206, 187, 206, 178, 206, 177, 206, 189,
    206, 175, 206, 177, ',', ' ', 207, 131, 207, 132, 206, 177,
    ' ', 206, 178, 207, 140, 207, 129, 206, 181, 206, 185, 206,
    177, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183, ' ',
    206, 146, 206, 191, 207, 133, 206, 187, 206, 179, 206, 177,
    207, 129, 206, 175, 206, 177, ' ', 206, 186, 206, 177, 206,
    185, ' ', 207, 132, 206, 183, 206, 189, ' ', 207, 128, 207,
    129, 207, 142, 206, 183, 206, 189, ' ', 206, 147, 206, 185,
    206, 191, 207, 133, 206, 179, 206, 186, 206, 191, 207, 131,
    206, 187, 206, 177, 206, 178, 206, 185, 206, 186, 206, 174,
    ' ', 206, 148, 206, 183, 206, 188, 206, 191, 206, 186, 207,
    129, 206, 177, 207, 132, 206, 175, 206, 177, ' ', 207, 132,
    206, 183, 207, 130, ' ', 206, 156, 206, 177, 206, 186, 206,
    181, 206, 180, 206, 191, 206, 189, 206, 175, 206, 177, 207,
    130, ' ', '(', 207, 128, '.', 206, 147, '.', 206, 148, '.',
    206, 156, '.', ')', ' ', 206, 186, 206, 177, 206, 185, ' ',
    207, 131, 207, 132, 206, 177, ' ', 206, 178, 206, 191, 207,
    129, 206, 181, 206, 185, 206, 191, 206, 177, 206, 189, 206,
    177, 207, 132, 206, 191, 206, 187, 206, 185, 206, 186, 206,
    172, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183, 206,
    189, ' ', 206, 164, 206, 191, 207, 133, 207, 129, 206, 186,
    206, 175, 206, 177, '.', ' ', 206, 146, 207, 129, 206, 173,
    207, 135, 206, 181, 207, 132, 206, 177, 206, 185, ' ', 207,
    131, 207, 132, 206, 177, ' ', 206, 177, 206, 189, 206, 177,
    207, 132, 206, 191, 206, 187, 206, 185, 206, 186, 206, 172,
    ' ', 206, 177, 207, 128, 207, 140, ' ', 207, 132, 206, 191,
    ' ', 206, 145, 206, 185, 206, 179, 206, 177, 206, 175, 206,
    191, ' ', 206, 160, 206, 173, 206, 187, 206, 177, 206, 179,
    206, 191, 207, 130, ',', ' ', 207, 131, 207, 132, 206, 177,
    ' ', 206, 180, 207, 133, 207, 132, 206, 185, 206, 186, 206,
    172, ' ', 206, 177, 207, 128, 207, 140, ' ', 207, 132, 206,
    191, ' ', 206, 153, 207, 140, 206, 189, 206, 185, 206, 191,
    ' ', 206, 186, 206, 177, 206, 185, ' ', 206, 189, 207, 140,
    207, 132, 206, 185, 206, 177, ' ', 206, 177, 207, 128, 207,
    140, ' ', 207, 132, 206, 183, ' ', 206, 156, 206, 181, 207,
    131, 207, 140, 206, 179, 206, 181, 206, 185, 206, 191, ' ',
    206, 152, 206, 172, 206, 187, 206, 177, 207, 131, 207, 131,
    206, 177, '.', 206, 151, ' ', 206, 149, 206, 187, 206, 187,
    206, 172, 206, 180, 206, 177, ' ', 206, 186, 206, 177, 207,
    132, 206, 173, 207, 135, 206, 181, 206, 185, ' ', 207, 132,
    206, 183, 206, 189, ' ', '1', '1', 206, 183, ' ', 206, 184,
    206, 173, 207, 131, 206, 183, ' ', 207, 131, 207, 132, 206,
    185, 207, 130, ' ', 207, 135, 207, 142, 207, 129, 206, 181,
    207, 130, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183,
    ' ', 206, 188, 206, 181, 206, 179, 206, 177, 206, 187, 207,
    141, 207, 132, 206, 181, 207, 129, 206, 183, ' ', 206, 177,
    206, 186, 207, 132, 206, 191, 206, 179, 207, 129, 206, 177,
    206, 188, 206, 188, 206, 174, ' ', 207, 131, 207, 132, 206,
    177, ' ', '1', '3', '.', '6', '7', '6', ' ', 207, 135, 206,
    185, 206, 187, 206, 185, 207, 140, 206, 188, 206, 181, 207,
    132, 207, 129, 206, 177, ' ', 206, 186, 206, 177, 206, 184,
    207, 142, 207, 130, ' ', 206, 181, 207, 135, 206, 181, 206,
    185, ' ', 207, 128, 206, 191, 206, 187, 206, 187, 206, 172,
    ' ', 206, 189, 206, 183, 207, 131, 206, 185, 206, 172, ' ',
    '(', 207, 128, 206, 181, 207, 129, 206, 175, 207, 128, 206,
    191, 207, 133, ' ', '1', '.', '4', '0', '0', ',', ' ', 206,
    181, 206, 186, 207, 132, 207, 137, 206, 189, ' ', 206, 191,
    207, 128, 206, 191, 206, 175, 207, 137, 206, 189, ' ', 207,
    132, 206, 177, ' ', '2', '2', '7', ' ', 206, 186, 206, 177,
    207, 132, 206, 191, 206, 185, 206, 186, 206, 191, 207, 133,
    206, 189, 207, 132, 206, 177, 206, 185, ')', ',', ' ', 207,
    131, 207, 133, 206, 188, 207, 128, 206, 181, 207, 129, 206,
    185, 206, 187, 206, 177, 206, 188, 206, 178, 206, 177, 206,
    189, 206, 191, 206, 188, 206, 173, 206, 189, 207, 137, 206,
    189, ' ', 207, 132, 206, 183, 207, 130, ' ', 206, 154, 207,
    129, 206, 183, 207, 132, 206, 183, 207, 130, ',', ' ', 207,
    132, 207, 137, 206, 189, ' ', 206, 148, 207, 137, 206, 180,
    206, 181, 206, 186, 206, 177, 206, 189, 206, 174, 207, 131,
    207, 137, 206, 189, ',', ' ', 207, 132, 207, 137, 206, 189,
    ' ', 206, 154, 207, 133, 206, 186, 206, 187, 206, 172, 206,
    180, 207, 137, 206, 189, ',', ' ', 207, 132, 207, 137, 206,
    189, ' ', 206, 149, 207, 128, 207, 132, 206, 177, 206, 189,
    206, 174, 207, 131, 207, 137, 206, 189, ' ', 206, 186, 206,
    177, 206, 185, ' ', 207, 128, 206, 191, 206, 187, 206, 187,
    207, 142, 206, 189, ' ', 206, 172, 206, 187, 206, 187, 207,
    137, 206, 189, '.', 206, 164, 206, 191, ' ', 207, 136, 206,
    183, 206, 187, 207, 140, 207, 132, 206, 181, 207, 129, 206,
    191, ' ', 206, 178, 206, 191, 207, 133, 206, 189, 207, 140,
    ' ', 206, 181, 206, 175, 206, 189, 206, 177, 206, 185, ' ',
    206, 191, ' ', 206, 140, 206, 187, 207, 133, 206, 188, 207,
    128, 206, 191, 207, 130, ' ', 206, 186, 206, 177, 206, 185,
    ' ', 207, 132, 206, 191, ' ', 206, 188, 206, 181, 206, 179,
    206, 177, 206, 187, 207, 141, 207, 132, 206, 181, 207, 129,
    206, 191, ' ', 207, 128, 206, 191, 207, 132, 206, 172, 206,
    188, 206, 185, ' ', 206, 191, ' ', 206, 145, 206, 187, 206,
    185, 206, 172, 206, 186, 206, 188, 206, 191, 206, 189, 206,
    177, 207, 130, '.',  13,  10,  13,  10, 206, 136, 207, 135,
    206, 181, 206, 185, ' ', 206, 188, 206, 177, 206, 186, 207,
    129, 206, 172, ' ', 206, 186, 206, 177, 206, 185, ' ', 207,
    128, 206, 187, 206, 191, 207, 141, 207, 131, 206, 185, 206,
    177, ' ', 206, 185, 207, 131, 207, 132, 206, 191, 207, 129,
    206, 175, 206, 177, ' ', 206, 186, 206, 177, 207, 132, 206,
    172, ' ', 207, 132, 206, 183, 206, 189, ' ', 206, 191, 207,
    128, 206, 191, 206, 175, 206, 177, ' ', 206, 172, 207, 131,
    206, 186, 206, 183, 207, 131, 206, 181, ' ', 206, 188, 206,
    181, 206, 179, 206, 172, 206, 187, 206, 183, ' ', 207, 128,
    206, 191, 206, 187, 206, 185, 207, 132, 206, 185, 207, 131,
    206, 188, 206, 185, 206, 186, 206, 174, ' ', 206, 181, 207,
    128, 206, 175, 206, 180, 207, 129, 206, 177, 207, 131, 206,
    183, ' ', 207, 131, 206, 181, ' ', 207, 132, 207, 129, 206,
    181, 206, 185, 207, 130, ' ', 206, 183, 207, 128, 206, 181,
    206, 175, 207, 129, 206, 191, 207, 133, 207, 130, '.', 206,
    149, 206, 180, 207, 142, ' ', 206, 179, 206, 181, 206, 189,
    206, 189, 206, 174, 206, 184, 206, 183, 206, 186, 206, 181,
    ' ', 206, 183, ' ', 206, 180, 206, 183, 206, 188, 206, 191,
    206, 186, 207, 129, 206, 177, 207, 132, 206, 175, 206, 177,
    ' ', 206, 186, 206, 177, 206, 185, ' ', 206, 183, ' ', 207,
    134, 206, 185, 206, 187, 206, 191, 207, 131, 206, 191, 207,
    134, 206, 175, 206, 177, '.', 206, 145, 206, 186, 207, 140,
    206, 188, 206, 188, 206, 177, ' ', 206, 183, ' ', 206, 149,
    206, 187, 206, 187, 206, 172, 206, 180, 206, 177, ' ', 206,
    181, 206, 175, 206, 189, 206, 177, 206, 185, ' ', 206, 191,
    ' ', 207, 132, 207, 140, 207, 128, 206, 191, 207, 130, ' ',
    206, 179, 206, 173, 206, 189, 206, 189, 206, 183, 207, 131,
    206, 183, 207, 130, ' ', 207, 132, 207, 137, 206, 189, ' ',
    206, 159, 206, 187, 207, 133, 206, 188, 207, 128, 206, 185,
    206, 177, 206, 186, 207, 142, 206, 189, ' ', 206, 145, 206,
    179, 207, 142, 206, 189, 207, 137, 206, 189, ',', 207, 132,
    206, 191, 207, 133, ' ', 206, 180, 207, 129, 206, 172, 206,
    188, 206, 177, 207, 132, 206, 191, 207, 130, ',', ' ', 207,
    132, 206, 183, 207, 130, ' ', 207, 132, 207, 129, 206, 177,
    206, 179, 207, 137, 206, 180, 206, 175, 206, 177, 207, 130,
    ' ', 206, 186, 206, 177, 206, 185, ' ', 207, 132, 206, 183,
    207, 130, ' ', 206, 186, 207, 137, 206, 188, 206, 188, 207,
    137, 206, 180, 206, 175, 206, 177, 207, 130, ' ', '.',  13,
     10,  13,  10, 206, 151, ' ', 206, 149, 206, 187, 206, 187,
    206, 172, 206, 180, 206, 177, ' ', 206, 181, 206, 175, 206,
    189, 206, 177, 206, 185, ' ', 206, 188, 206, 173, 206, 187,
    206, 191, 207, 130, ' ', 207, 132, 207, 137, 206, 189, ' ',
    206, 149, 207, 133, 207, 129, 207, 137, 207, 128, 206, 177,
    207, 138, 206, 186, 207, 142, 206, 189, ' ', 206, 154, 206,
    191, 206, 185, 206, 189, 206, 191, 207, 132, 206, 174, 207,
    132, 207, 137, 206, 189, '/', 206, 149, 207, 133, 207, 129,
    207, 137, 207, 128, 206, 177, 207, 138, 206, 186, 206, 174,
    207, 130, ' ', 206, 136, 206, 189, 207, 137, 207, 131, 206,
    183, 207, 130, ' ', 206, 177, 207, 128, 207, 140, ' ', 207,
    132, 206, 191, ' ', '1', '9', '8', '1', ',', ' ', 207, 132,
    206, 183, 207, 130, ' ', 206, 149, 207, 133, 207, 129, 207,
    137, 206, 182, 207, 142, 206, 189, 206, 183, 207, 130, ' ',
    206, 177, 207, 128, 207, 140, ' ', 207, 132, 206, 191, ' ',
    '2', '0', '0', '1', ',', ' ', 207, 132, 206, 191, 207, 133,
    ' ', 206, 157, 206, 145, 206, 164, 206, 159, ' ', 206, 177,
    207, 128, 207, 140, ' ', 207, 132, 206, 191, ' ', '1', '9',
    '5', '2', ' ', 206, 186, 206, 177, 206, 185, ' ', 206, 185,
    206, 180, 207, 129, 207, 133, 207, 132, 206, 185, 206, 186,
    207, 140, ' ', 206, 188, 206, 173, 206, 187, 206, 191, 207,
    130, ' ', 207, 132, 206, 191, 207, 133, ' ', 206, 159, 206,
    151, 206, 149, ' ', '(', '1', '9', '4', '5', ')', '.', ' ',
    206, 151, ' ', 206, 149, 206, 187, 206, 187, 206, 172, 206,
    180, 206, 177, ' ', 206, 181, 206, 175, 206, 189, 206, 177,
    206, 185, ' ', 206, 188, 206, 185, 206, 177, ' ', 206, 177,
    206, 189, 206, 181, 207, 128, 207, 132, 207, 133, 206, 179,
    206, 188, 206, 173, 206, 189, 206, 183, ' ', 207, 135, 207,
    142, 207, 129, 206, 177, ' ', 206, 188, 206, 181, ' ', 207,
    133, 207, 136, 206, 183, 206, 187, 207, 140, ' ', 206, 186,
    206, 177, 207, 132, 206, 172, ' ', 206, 186, 206, 181, 207,
    134, 206, 177, 206, 187, 206, 174, 206, 189, ' ', 206, 181,
    206, 185, 207, 131, 207, 140, 206, 180, 206, 183, 206, 188,
    206, 177, ' ', 206, 186, 206, 177, 206, 185, ' ', 207, 128,
    206, 191, 206, 187, 207, 141, ' ', 207, 133, 207, 136, 206,
    183, 206, 187, 207, 140, ' ', 206, 180, 206, 181, 206, 175,
    206, 186, 207, 132, 206, 183, ' ', 206, 177, 206, 189, 206,
    184, 207, 129, 207, 142, 207, 128, 206, 185, 206, 189, 206,
    183, 207, 130, ' ', 206, 177, 206, 189, 206, 172, 207, 128,
    207, 132, 207, 133, 206, 190, 206, 183, 207, 130, '.', ' ',
    206, 154, 206, 177, 207, 132, 206, 173, 207, 135, 206, 181,
    206, 185, ' ', 207, 132, 206, 183, 206, 189, ' ', '2', '2',
    206, 183, ' ', 206, 186, 206, 177, 206, 187, 207, 141, 207,
    132, 206, 181, 207, 129, 206, 183, ' ', 207, 128, 206, 191,
    206, 185, 207, 140, 207, 132, 206, 183, 207, 132, 206, 177,
    ' ', 206, 182, 207, 137, 206, 174, 207, 130, ' ', 207, 131,
    207, 132, 206, 191, 206, 189, ' ', 206, 186, 207, 140, 207,
    131, 206, 188, 206, 191, '.', '[', '4', ']',  13,  10,  13,
     10, 206, 151, ' ', 206, 149, 206, 187, 206, 187, 206, 172,
    206, 180, 206, 177, ' ', '(', 207, 128, 206, 177, 206, 187,
    206, 177, 206, 185, 207, 140, 207, 132, 206, 181, 207, 129,
    206, 177, ':', ' ', 225, 188, 153, 206, 187, 206, 187, 206,
    172, 207, 130, ',', ' ', 206, 181, 207, 128, 206, 175, 207,
    131, 206, 183, 206, 188, 206, 177, ':', ' ', 206, 149, 206,
    187, 206, 187, 206, 183, 206, 189, 206, 185, 206, 186, 206,
    174, ' ', 206, 148, 206, 183, 206, 188, 206, 191, 206, 186,
    207, 129, 206, 177, 207, 132, 206, 175, 206, 177, ')', ' ',
    206, 181, 206, 175, 206, 189, 206, 177, 206, 185, ' ', 207,
    135, 207, 142, 207, 129, 206, 177, ' ', 207, 128, 206, 191,
    207, 133, ' ', 206, 178, 207, 129, 206, 175, 207, 131, 206,
    186, 206, 181, 207, 132, 206, 177, 206, 185, ' ', 207, 131,
    207, 132, 206, 183, ' ', 206, 189, 206, 191, 207, 132, 206,
    185, 206, 191, 206, 177, 206, 189, 206, 177, 207, 132, 206,
    191, 206, 187, 206, 185, 206, 186, 206, 174, ' ', 206, 149,
    207, 133, 207, 129, 207, 142, 207, 128, 206, 183, ',', ' ',
    207, 131, 207, 132, 206, 191, ' ', 206, 189, 206, 191, 207,
    132, 206, 185, 207, 140, 207, 132, 206, 181, 207, 129, 206,
    191, ' ', 206, 172, 206, 186, 207, 129, 206, 191, ' ', 207,
    132, 206, 183, 207, 130, ' ', 206, 146, 206, 177, 206, 187,
    206, 186, 206, 177, 206, 189, 206, 185, 206, 186, 206, 174,
    207, 130, ' ', 207, 135, 206, 181, 207, 129, 207, 131, 206,
    191, 206, 189, 206, 174, 207, 131, 206, 191, 207, 133, ',',
    ' ', 207, 131, 207, 132, 206, 183, 206, 189, ' ', 206, 145,
    206, 189, 206, 177, 207, 132, 206, 191, 206, 187, 206, 185,
    206, 186, 206, 174, ' ', 206, 156, 206, 181, 207, 131, 207,
    140, 206, 179, 206, 181, 206, 185, 206, 191, '.', 206, 160,
    207, 129, 207, 137, 207, 132, 206, 181, 207, 141, 206, 191,
    207, 133, 207, 131, 206, 177, ' ', 207, 132, 206, 183, 207,
    130, ' ', 206, 149, 206, 187, 206, 187, 206, 172, 206, 180,
    206, 191, 207, 130, ' ', 206, 186, 206, 177, 206, 185, ' ',
    206, 188, 206, 181, 206, 179, 206, 177, 206, 187, 207, 141,
    207, 132, 206, 181, 207, 129, 206, 183, ' ', 207, 128, 207,
    140, 206, 187, 206, 183, ' ', 206, 181, 206, 175, 206, 189,
    206, 177, 206, 185, ' ', 206, 183, ' ', 206, 145, 206, 184,
    206, 174, 206, 189, 206, 177, '.',  13,  10,  13,  10, 206,
    163, 207, 133, 206, 189, 206, 191, 207, 129, 206, 181, 207,
    141, 206, 181, 206, 185, ' ', 207, 131, 207, 132, 206, 177,
    ' ', 206, 178, 206, 191, 207, 129, 206, 181, 206, 185, 206,
    191, 206, 180, 207, 133, 207, 132, 206, 185, 206, 186, 206,
    172, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183, 206,
    189, ' ', 206, 145, 206, 187, 206, 178, 206, 177, 206, 189,
    206, 175, 206, 177, ',', ' ', 207, 131, 207, 132, 206, 177,
    ' ', 206, 178, 207, 140, 207, 129, 206, 181, 206, 185, 206,
    177, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183, ' ',
    206, 146, 206, 191, 207, 133, 206, 187, 206, 179, 206, 177,
    207, 129, 206, 175, 206, 177, ' ', 206, 186, 206, 177, 206,
    185, ' ', 207, 132, 206, 183, 206, 189, ' ', 207, 128, 207,
    129, 207, 142, 206, 183, 206, 189, ' ', 206, 147, 206, 185,
    206, 191, 207, 133, 206, 179, 206, 186, 206, 191, 207, 131,
    206, 187, 206, 177, 206, 178, 206, 185, 206, 186, 206, 174,
    ' ', 206, 148, 206, 183, 206, 188, 206, 191, 206, 186, 207,
    129, 206, 177, 207, 132, 206, 175, 206, 177, ' ', 207, 132,
    206, 183, 207, 130, ' ', 206, 156, 206, 177, 206, 186, 206,
    181, 206, 180, 206, 191, 206, 189, 206, 175, 206, 177, 207,
    130, ' ', '(', 207, 128, '.', 206, 147, '.', 206, 148, '.',
    206, 156, '.', ')', ' ', 206, 186, 206, 177, 206, 185, ' ',
    207, 131, 207, 132, 206, 177, ' ', 206, 178, 206, 191, 207,
    129, 206, 181, 206, 185, 206, 191, 206, 177, 206, 189, 206,
    177, 207, 132, 206, 191, 206, 187, 206, 185, 206, 186, 206,
    172, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183, 206,
    189, ' ', 206, 164, 206, 191, 207, 133, 207, 129, 206, 186,
    206, 175, 206, 177, '.', ' ', 206, 146, 207, 129, 206, 173,
    207, 135, 206, 181, 207, 132, 206, 177, 206, 185, ' ', 207,
    131, 207, 132, 206, 177, ' ', 206, 177, 206, 189, 206, 177,
    207, 132, 206, 191, 206, 187, 206, 185, 206, 186, 206, 172,
    ' ', 206, 177, 207, 128, 207, 140, ' ', 207, 132, 206, 191,
    ' ', 206, 145, 206, 185, 206, 179, 206, 177, 206, 175, 206,
    191, ' ', 206, 160, 206, 173, 206, 187, 206, 177, 206, 179,
    206, 191, 207, 130, ',', ' ', 207, 131, 207, 132, 206, 177,
    ' ', 206, 180, 207, 133, 207, 132, 206, 185, 206, 186, 206,
    172, ' ', 206, 177, 207, 128, 207, 140, ' ', 207, 132, 206,
    191, ' ', 206, 153, 207, 140, 206, 189, 206, 185, 206, 191,
    ' ', 206, 186, 206, 177, 206, 185, ' ', 206, 189, 207, 140,
    207, 132, 206, 185, 206, 177, ' ', 206, 177, 207, 128, 207,
    140, ' ', 207, 132, 206, 183, ' ', 206, 156, 206, 181, 207,
    131, 207, 140, 206, 179, 206, 181, 206, 185, 206, 191, ' ',
    206, 152, 206, 172, 206, 187, 206, 177, 207, 131, 207, 131,
    206, 177, '.', 206, 151, ' ', 206, 149, 206, 187, 206, 187,
    206, 172, 206, 180, 206, 177, ' ', 206, 186, 206, 177, 207,
    132, 206, 173, 207, 135, 206, 181, 206, 185, ' ', 207, 132,
    206, 183, 206, 189, ' ', '1', '1', 206, 183, ' ', 206, 184,
    206, 173, 207, 131, 206, 183, ' ', 207, 131, 207, 132, 206,
    185, 207, 130, ' ', 207, 135, 207, 142, 207, 129, 206, 181,
    207, 130, ' ', 206, 188, 206, 181, ' ', 207, 132, 206, 183,
    ' ', 206, 188, 206, 181, 206, 179, 206, 177, 206, 187, 207,
    141, 207, 132, 206, 181, 207, 129, 206, 183, ' ', 206, 177,
    206, 186, 207, 132, 206, 191, 206, 179, 207, 129, 206, 177,
    206, 188, 206, 188, 206, 174, ' ', 207, 131, 207, 132, 206,
    177, ' ', '1', '3', '.', '6', '7', '6', ' ', 207, 135, 206,
    185, 206, 187, 206, 185, 207, 140, 206, 188, 206, 181, 207,
    132, 207, 129, 206, 177, ' ', 206, 186, 206, 177, 206, 184,
    207, 142, 207, 130, ' ', 206, 181, 207, 135, 206, 181, 206,
    185, ' ', 207, 128, 206, 191, 206, 187, 206, 187, 206, 172,
    ' ', 206, 189, 206, 183, 207, 131, 206, 185, 206, 172, ' ',
    '(', 207, 128, 206, 181, 207, 129, 206, 175, 207, 128, 206,
    191, 207, 133, ' ', '1', '.', '4', '0', '0', ',', ' ', 206,
    181, 206, 186, 207, 132, 207, 137, 206, 189, ' ', 206, 191,
    207, 128, 206, 191, 206, 175, 207, 137, 206, 189, ' ', 207,
    132, 206, 177, ' ', '2', '2', '7', ' ', 206, 186, 206, 177,
    207, 132, 206, 191, 206, 185, 206, 186, 206, 191, 207, 133,
    206, 189, 207, 132, 206, 177, 206, 185, ')', ',', ' ', 207,
    131, 207, 133, 206, 188, 207, 128, 206, 181, 207, 129, 206,
    185, 206, 187, 206, 177, 206, 188, 206, 178, 206, 177, 206,
    189, 206, 191, 206, 188, 206, 173, 206, 189, 207, 137, 206,
    189, ' ', 207, 132, 206, 183, 207, 130, ' ', 206, 154, 207,
    129, 206, 183, 207, 132, 206, 183, 207, 130, ',', ' ', 207,
    132, 207, 137, 206, 189, ' ', 206, 148, 207, 137, 206, 180,
    206, 181, 206, 186, 206, 177, 206, 189, 206, 174, 207, 131,
    207, 137, 206, 189, ',', ' ', 207, 132, 207, 137, 206, 189,
    ' ', 206, 154, 207, 133, 206, 186, 206, 187, 206, 172, 206,
    180, 207, 137, 206, 189, ',', ' ', 207, 132, 207, 137, 206,
    189, ' ', 206, 149, 207, 128, 207, 132, 206, 177, 206, 189,
    206, 174, 207, 131, 207, 137, 206, 189, ' ', 206, 186, 206,
    177, 206, 185, ' ', 207, 128, 206, 191, 206, 187, 206, 187,
    207, 142, 206, 189, ' ', 206, 172, 206, 187, 206, 187, 207,
    137, 206, 189, '.', 206, 164, 206, 191, ' ', 207, 136, 206,
    183, 206, 187, 207, 140, 207, 132, 206, 181, 207, 129, 206,
    191, ' ', 206, 178, 206, 191, 207, 133, 206, 189, 207, 140,
    ' ', 206, 181, 206, 175, 206, 189, 206, 177, 206, 185, ' ',
    206, 191, ' ', 206, 140, 206, 187, 207, 133, 206, 188, 207,
    128, 206, 191, 207, 130, ' ', 206, 186, 206, 177, 206, 185,
    ' ', 207, 132, 206, 191, ' ', 206, 188, 206, 181, 206, 179,
    206, 177, 206, 187, 207, 141, 207, 132, 206, 181, 207, 129,
    206, 191, ' ', 207, 128, 206, 191, 207, 132, 206, 172, 206,
    188, 206, 185, ' ', 206, 191, ' ', 206, 145, 206, 187, 206,
    185, 206, 172, 206, 186, 206, 188, 206, 191, 206, 189, 206,
    177, 207, 130, '.',  13,  10,  13,  10, 206, 136, 207, 135,
    206, 181, 206, 185, ' ', 206, 188, 206, 177, 206, 186, 207,
    129, 206, 172, ' ', 206, 186, 206, 177, 206, 185, ' ', 207,
    128, 206, 187, 206, 191, 207, 141, 207, 131, 206, 185, 206,
    177, ' ', 206, 185, 207, 131, 207, 132, 206, 191, 207, 129,
    206, 175, 206, 177, ' ', 206, 186, 206, 177, 207, 132, 206,
    172, ' ', 207, 132, 206, 183, 206, 189, ' ', 206, 191, 207,
    128, 206, 191, 206, 175, 206, 177, ' ', 206, 172, 207, 131,
    206, 186, 206, 183, 207, 131, 206, 181, ' ', 206, 188, 206,
    181, 206, 179, 206, 172, 206, 187, 206, 183, ' ', 207, 128,
    206, 191, 206, 187, 206, 185, 207, 132, 206, 185, 207, 131,
    206, 188, 206, 185, 206, 186, 206, 174, ' ', 206, 181, 207,
    128, 206, 175, 206, 180, 207, 129, 206, 177, 207, 131, 206,
    183, ' ', 207, 131, 206, 181, ' ', 207, 132, 207, 129, 206,
    181, 206, 185, 207, 130, ' ', 206, 183, 207, 128, 206, 181,
    206, 175, 207, 129, 206, 191, 207, 133, 207, 130, '.', 206,
    149, 206, 180, 207, 142, ' ', 206, 179, 206, 181, 206, 189,
    206, 189, 206, 174, 206, 184, 206, 183, 206, 186, 206, 181,
    ' ', 206, 183, ' ', 206, 180, 206, 183, 206, 188, 206, 191,
    206, 186, 207, 129, 206, 177, 207, 132, 206, 175, 206, 177,
    ' ', 206, 186, 206, 177, 206, 185, ' ', 206, 183, ' ', 207,
    134, 206, 185, 206, 187, 206, 191, 207, 131, 206, 191, 207,
    134, 206, 175, 206, 177, '.', 206, 145, 206, 186, 207, 140,
    206, 188, 206, 188, 206, 177, ' ', 206, 183, ' ', 206, 149,
    206, 187, 206, 187, 206, 172, 206, 180, 206, 177, ' ', 206,
    181, 206, 175, 206, 189, 206, 177, 206, 185, ' ', 206, 191,
    ' ', 207, 132, 207, 140, 207, 128, 206, 191, 207, 130, ' ',
    206, 179, 206, 173, 206, 189, 206, 189, 206, 183, 207, 131,
    206, 183, 207, 130, ' ', 207, 132, 207, 137, 206, 189, ' ',
    206, 159, 206, 187, 207, 133, 206, 188, 207, 128, 206, 185,
    206, 177, 206, 186, 207, 142, 206, 189, ' ', 206, 145, 206,
    179, 207, 142, 206, 189, 207, 137, 206, 189, ',', 207, 132,
    206, 191, 207, 133, ' ', 206, 180, 207, 129, 206, 172, 206,
    188, 206, 177, 207, 132, 206, 191, 207, 130, ',', ' ', 207,
    132, 206, 183, 207, 130, ' ', 207, 132, 207, 129, 206, 177,
    206, 179, 207, 137, 206, 180, 206, 175, 206, 177, 207, 130,
    ' ', 206, 186, 206, 177, 206, 185, ' ', 207, 132, 206, 183,
    207, 130, ' ', 206, 186, 207, 137, 206, 188, 206, 188, 207,
    137, 206, 180, 206, 175, 206, 177, 207, 130, ' ', '.',  13,
     10,  13,  10, 206, 151, ' ', 206, 149, 206, 187, 206, 187,
    206, 172, 206, 180, 206, 177, ' ', 206, 181, 206, 175, 206,
    189, 206, 177, 206, 185, ' ', 206, 188, 206, 173, 206, 187,
    206, 191, 207, 130, ' ', 207, 132, 207, 137, 206, 189, ' ',
    206, 149, 207, 133, 207, 129, 207, 137, 207, 128, 206, 177,
    207, 138, 206, 186, 207, 142, 206, 189, ' ', 206, 154, 206,
    191, 206, 185, 206, 189, 206, 191, 207, 132, 206, 174, 207,
    132, 207, 137, 206, 189, '/', 206, 149, 207, 133, 207, 129,
    207, 137, 207, 128, 206, 177, 207, 138, 206, 186, 206, 174,
    207, 130, ' ', 206, 136, 206, 189, 207, 137, 207, 131, 206,
    183, 207, 130, ' ', 206, 177, 207, 128, 207, 140, ' ', 207,
    132, 206, 191, ' ', '1', '9', '8', '1', ',', ' ', 207, 132,
    206, 183, 207, 130, ' ', 206, 149, 207, 133, 207, 129, 207,
    137, 206, 182, 207, 142, 206, 189, 206, 183, 207, 130, ' ',
    206, 177, 207, 128, 207, 140, ' ', 207, 132, 206, 191, ' ',
    '2', '0', '0', '1', ',', ' ', 207, 132, 206, 191, 207, 133,
    ' ', 206, 157, 206, 145, 206, 164, 206, 159, ' ', 206, 177,
    207, 128, 207, 140, ' ', 207, 132, 206, 191, ' ', '1', '9',
    '5', '2', ' ', 206, 186, 206, 177, 206, 185, ' ', 206, 185,
    206, 180, 207, 129, 207, 133, 207, 132, 206, 185, 206, 186,
    207, 140, ' ', 206, 188, 206, 173, 206, 187, 206, 191, 207,
    130, ' ', 207, 132, 206, 191, 207, 133, ' ', 206, 159, 206,
    151, 206, 149, ' ', '(', '1', '9', '4', '5', ')', '.', ' ',
    206, 151, ' ', 206, 149, 206, 187, 206, 187, 206, 172, 206,
    180, 206, 177, ' ', 206, 181, 206, 175, 206, 189, 206, 177,
    206, 185, ' ', 206, 188, 206, 185, 206, 177, ' ', 206, 177,
    206, 189, 206, 181, 207, 128, 207, 132, 207, 133, 206, 179,
    206, 188, 206, 173, 206, 189, 206, 183, ' ', 207, 135, 207,
    142, 207, 129, 206, 177, ' ', 206, 188, 206, 181, ' ', 207,
    133, 207, 136, 206, 183, 206, 187, 207, 140, ' ', 206, 186,
    206, 177, 207, 132, 206, 172, ' ', 206, 186, 206, 181, 207,
    134, 206, 177, 206, 187, 206, 174, 206, 189, ' ', 206, 181,
    206, 185, 207, 131, 207, 140, 206, 180, 206, 183, 206, 188,
    206, 177, ' ', 206, 186, 206, 177, 206, 185, ' ', 207, 128,
    206, 191, 206, 187, 207, 141, ' ', 207, 133, 207, 136, 206,
    183, 206, 187, 207, 140, ' ', 206, 180, 206, 181, 206, 175,
    206, 186, 207, 132, 206, 183, ' ', 206, 177, 206, 189, 206,
    184, 207, 129, 207, 142, 207, 128, 206, 185, 206, 189, 206,
    183, 207, 130, ' ', 206, 177, 206, 189, 206, 172, 207, 128,
    207, 132, 207, 133, 206, 190, 206, 183, 207, 130, '.', ' ',
    206, 154, 206, 177, 207, 132, 206, 173, 207, 135, 206, 181,
    206, 185, ' ', 207, 132, 206, 183, 206, 189, ' ', '2', '2',
    206, 183, ' ', 206, 186, 206, 177, 206, 187, 207, 141, 207,
    132, 206, 181, 207, 129, 206, 183, ' ', 207, 128, 206, 191,
    206, 185, 207, 140, 207, 132, 206, 183, 207, 132, 206, 177,
    ' ', 206, 182, 207, 137, 206, 174, 207, 130, ' ', 207, 131,
    207, 132, 206, 191, 206, 189, ' ', 206, 186, 207, 140, 207,
    131, 206, 188, 206, 191, '.', '[', '4', ']',  13,  10,  13,
     10, 'R', 'a', 'n', 'd', 'o', 'm', ' ', 'Q', 'u', 'a', 'd',
    ' ', 'V', 'a', 'l', 'u', 'e', 's',  13,  10, 240, 144, 128,
    128, 240, 152, 166, 171, 240, 158, 187, 174, 240, 154, 170,
    170, 240, 154, 132, 163, 240, 155, 132, 163, 243, 187, 174,
    187, 244, 128, 128, 128, 243, 174, 187, 174, 242, 187, 174,
    187,  13,  10,   0
};
const char * const charUtf8MultiLang = (const char *) utf8MultiLang;
enum { k_MULTI_LANG_LEN = sizeof(utf8MultiLang) - 1 };

template <class UTF16_WORD>
static
int localUtf16Cmp(const UTF16_WORD *a, const UTF16_WORD *b)
{
    int diff;
    while (0 == (diff = *a - *b) && *a) {
        ++a;
        ++b;
    }

    return diff;
}

template <class UTF16_WORD>
static
bsl::size_t localUtf16Len(const UTF16_WORD *str)
{
    const UTF16_WORD *pws = str;
    while (*pws) {
        ++pws;
    }

    ASSERT(pws >= str);
    return pws - str;
}

static
bsl::string displayUtf8(const bsl::string& str)
    // Return another string that is a human-readable version of the specified
    // 'str', with non-printable characters translated at '\x??' where '?' is
    // '[0-9]'.
{
    bsl::string ret("\"");

    const unsigned char *begin = reinterpret_cast<const unsigned char *>(
                                                                  str.c_str());
    for (const unsigned char *pc = begin; *pc; ++pc) {
        if ((0xc0u & *pc) != 0x80u && begin < pc) {
            ret.push_back(' ');
        }
        if ((0x80 & *pc) || !isprint(*pc)) {
            ret.push_back('\\');
            ret.push_back('x');
            unsigned nybble = *pc >> 4;
            nybble = nybble < 10 ? '0' + nybble : 'a' + nybble - 10;
            ret.push_back(static_cast<char>(nybble));
            nybble = *pc & 0xf;
            nybble = nybble < 10 ? '0' + nybble : 'a' + nybble - 10;
            ret.push_back(static_cast<char>(nybble));
        }
        else {
            ret.push_back(*pc);
        }
    }

    ret.push_back('"');

    return ret;
}

template <class UTF16_WORD>
static
bsl::string displayUtf16(const UTF16_WORD       *utf16Str,
                         bdlde::ByteOrder::Enum  byteOrder)
    // Return a bsl::string that is a human-readable version of the specified
    // 'utf16Str', with non-printable characters translated at '\x<hex><hex>'
    // where '<hex>' is a hex digit.
{
    bsl::string ret("\"");

    for (const UTF16_WORD *pw = utf16Str; *pw; ++pw) {
        const UTF16_WORD w = bdlde::ByteOrder::e_HOST == byteOrder
                           ? *pw
                           : bsls::ByteOrderUtil::swapBytes(*pw);

        if (utf16Str < pw) {
            ret.push_back(' ');
        }
        if (w < 128 && isprint(static_cast<char>(w))) {
            ret.push_back('\'');
            ret.push_back(static_cast<char>(w));
            ret.push_back('\'');
        }
        else {
            ret.push_back('\\');
            ret.push_back('x');
            for (int ii = 3; 0 <= ii; --ii) {
                unsigned nybble = (w >> (ii * 4)) & 0xf;
                unsigned c = nybble < 10 ? '0' + nybble : 'a' + nybble - 10;
                ret.push_back(static_cast<char>(c));
            }
        }
    }

    ret.push_back('"');

    return ret;
}

//=============================================================================
//                              TEST DRIVER
//=============================================================================

class TestDriver {
    enum Validity { e_VALID, e_INVALID };

    enum Utf8Bits {
        // Masks and shifts used to assemble and dismantle UTF-8 encodings.

        ONE_OCT_CONT_WID   = 7,               // Content in a one-octet coding
        ONE_OCTET_MASK     = 0xff & (~0u << ONE_OCT_CONT_WID),
        ONE_OCTET_TAG      = 0xff & 0,        // Compare this to mask'd bits

        CONTINUE_CONT_WID  = 6,               // Content in a continuation
                                              //                      octet
        CONTINUE_MASK      = 0xff & (~0u << CONTINUE_CONT_WID),
        CONTINUE_TAG       = ONE_OCTET_MASK,  // Compare this to mask'd bits

        TWO_OCT_CONT_WID   = 5,               // Content in a two-octet header
        TWO_OCTET_MASK     = 0xff & (~0u << TWO_OCT_CONT_WID),
        TWO_OCTET_TAG      = CONTINUE_MASK,   // Compare this to mask'd bits

        THREE_OCT_CONT_WID = 4,               // Content in a 3-octet header
        THREE_OCTET_MASK   = 0xff & (~0u << THREE_OCT_CONT_WID),
        THREE_OCTET_TAG    = TWO_OCTET_MASK,  // Compare this to mask'd bits

        FOUR_OCT_CONT_WID  = 3,               // Content in a four-octet header
        FOUR_OCTET_MASK    = 0xff & (~0u << FOUR_OCT_CONT_WID),
        FOUR_OCTET_TAG     = THREE_OCTET_MASK // Compare this to mask'd bits
    };

    enum {
        // In functions where we are making a random choice between
        // possibilities, which happens frequently in test cases 14-16, when we
        // are choosing between 'N' possibilities where 'N' is not a power of
        // two, we take 'M > N' when 'M' is the next higher power of two than
        // N, where 'M == (1 << BITS)' and then calculate
        // 'k_RAND_NUM_BITS == BITS + k_RAND_FUDGE_BITS' that when we calculate
        //..
        // x = s_randGen.bits(k_RAND_NUM_BITS) % N;
        //..
        // and we get roughly equal probability of each choice in the range
        // '[ 0 .. N-1 ]'.  We can test that our fudge factor is adequate with
        //..
        // BSLMF_ASSERT(N * k_RAND_FUDGE_MULTIPE < (1 << K_RAND_NUM_BITS));
        //..

        k_RAND_FUDGE_BITS     = 3,
        k_RAND_FUDGE_MULTIPLE = 1u << k_RAND_FUDGE_BITS
    };

    enum { k_LO_SURROGATE_BASE = 0xd800u,    // low UTF-16 surrogates are in
                                             // the range
                                             // '[ 0xd800 .. 0xdbff ]'
           k_HI_SURROGATE_BASE = 0xdc00u,    // high UTF-16 surrogates are in
                                             // the range
                                             // '[ 0xdc00 .. 0xdfff ]'
           k_SURROGATE_CEILING = 0xe000u,

           k_SURROGATE_RAND_BITS = 10u };

    BSLMF_ASSERT(k_LO_SURROGATE_BASE + (1u << k_SURROGATE_RAND_BITS) ==
                                                          k_HI_SURROGATE_BASE);
    BSLMF_ASSERT(k_HI_SURROGATE_BASE + (1u << k_SURROGATE_RAND_BITS) ==
                                                          k_SURROGATE_CEILING);

    typedef unsigned UnicodeCodePoint;

    // CLASS DATA
    static RandGen s_randGen;                // Random number generator

    // PRIVATE CLASS METHODS
    static void copyUtf16ToWstring(
                                bsl::wstring                       *wStr,
                                const bsl::vector<unsigned short>&  utf16In,
                                bdlde::ByteOrder::Enum              byteOrder);
        // Copy the specified UTF-16 sequence from 'utf16In' to the specified
        // 'wStr', interpret the byte order of both input and output as the
        // specified 'byteOrder'.  This is useful when we've created invalid
        // UTF-16 in a 'vector<unsigned short>' with 'randomInvalidUtf16Vector'
        // (below) and we want to also test on a 'wstring' without having to
        // templatize 'randomInvalidUtf16Vector' or create another version of
        // it that operates on a 'wstring' instead of a 'vector'.

    static unsigned numBitsSet(unsigned value);
        // Return the number of bits in the specified 'value' that are set.

    static unsigned calculateRandomNumOctets();
        // Calculate a random number of octets with the following
        // probabilities: 1 octet: 12.5%, 2 octets: 37.5%, 3 octets: 37.5%, 4
        // octets: 12.5%.  Note that single-octet sequences are not very
        // interesting for testing, and quad octets are normally extremely rare
        // in UTF-8.

    static
    bool fitsInSingleOctet(UnicodeCodePoint uc);
        // Return 'true' if the specified Unicode code point 'uc' will fit in a
        // single octet of UTF-8, and 'false' otherwise.

    static
    bool fitsInTwoOctets(UnicodeCodePoint uc);
        // Return 'true' if the specified Unicode code point 'uc' will fit in
        // two octets of UTF-8, and 'false' otherwise.

    static
    bool fitsInThreeOctets(UnicodeCodePoint uc);
        // Return 'true' if the specified Unicode code point 'uc' will fit in
        // three octets of UTF-8, and 'false' otherwise.

    // The functions:
    //
    //: 1 'encodeTwoOctets',
    //:
    //: 2 'encodeThreeOctets', and
    //:
    //: 3 'encodeFourOctets'
    //
    // encode the iso10646 code point passed as their second argument into the
    // specified number of octets in the UTF-8 byte array addressed by their
    // first argument.  The behavior is undefined if there is not enough room
    // in the array.  The result may be incorrect UTF-8 if the specified
    // encoding is not the correct coding for the code point given (we desire
    // this behavior when creating invalid UTF-8 to test on).  Note that the
    // single-byte encoding is accomplished by a direct copy, for which no
    // function is provided here.

    static
    void encodeTwoOctets(char *octBuf, UnicodeCodePoint isoBuf);
        // Assume the specified 'isoBuf' will fit in 2 octets of UTF-8 and
        // encode it at the position indicated by the specified 'octBuf'.

    static
    void encodeThreeOctets(char *octBuf, UnicodeCodePoint isoBuf);
        // Assume the specified 'isoBuf' will fit in 3 octets of UTF-8 and
        // encode it at the position indicated by the specified 'octBuf'.

    static
    void encodeFourOctets(char *octBuf, UnicodeCodePoint isoBuf);
        // Assume the specified 'isoBuf' will fit in 4 octets of UTF-8 and
        // encode it at the position indicated by the specified 'octBuf'.

    static
    bool isContinuation(char c);
        // Return 'true' if the specified 'c' is a continuation byte and
        // 'false' otherwise.

    static
    void appendRandomInvalidSingleOctet(bsl::string *string);
        // Append a random invalid single byte to the specified '*string'.
        // Note that the byte appended may be legal if it is a continuation
        // byte and the preceding contents '*string' is a truncated sequence.

    static
    void appendRandomInvalidTwoOctets(bsl::string *string);
        // Append a random invalid two-octet code point to the specified
        // '*string'.

    static
    void appendRandomInvalidThreeOctets(bsl::string *string);
        // Append a random invalid three-octet code point to the specified
        // '*string'.

    static
    void appendRandomInvalidFourOctets(bsl::string *string);
        // Append a random invalid four-octet code point to the specified
        // '*string'.

    static
    void appendRandomValidSingleOctet(bsl::string *string);
        // Append a random non-zero ASCII byte to the specified '*string'.

    static
    void appendRandomValidTwoOctets(bsl::string *string);
        // Append a random valid two octet sequence to the specified '*string'.

    static
    void appendRandomValidThreeOctets(bsl::string *string);
        // Append a random valid three octet sequence to the specified
        // '*string'.

    static
    void appendRandomValidFourOctets(bsl::string *string);
        // Append a random valid four octet sequence to the specified
        // '*string'.

    static
    void appendRandomValidUtf8CodePoint(bsl::string *string);
        // Append a random valid UTF-8 code point to the specified '*string'.
        // Make 2 and 3 byte code points 3 times as likely as 1 or 4 byte
        // code points.

    static void appendRandomInvalidUtf8CodePoint(bsl::string *string);
        // Append random invalid UTF-8 to the specified '*string'.  Note that
        // the appended sequence will usually consistitute one code point, but
        // in some cases where a truncated sequence is followed by the first
        // char of another truncated sequence, it will consitute 2 invalid code
        // points.

    static void randomUtf8String(bsl::string *result,
                                 unsigned     numSequences,
                                 Validity     validity);
        // Create a UTF-8 string and return it in the specified 'result'
        // containing 'numSequences' sequences, where the terminating '\0'
        // counts as one sequence.  If the specified 'validity' is 'e_VALID'
        // the UTF-8 will be valid.  If the specified 'validity' is
        // 'e_INVALID', the string will be invalid.  The behavior is undefined
        // if 'numSequences < 1' or if 'numSequences < 2' and
        // 'validity == e_INVALID'.  Note that in the case of invalid UTF-8,
        // the number of code points may exceed 'numSequences'.

    static unsigned short randomUtf16NonSurrogate();
        // Return a random non-zero, non-surrogate UTF-16 word.

    static unsigned short randomUtf16SurrogateLo();
        // Return a random UTF-8 low-surrogate word.

    static unsigned short randomUtf16SurrogateHi();
        // Return a random UTF-8 high-surrogate word.

    static void randomInvalidUtf16Vector(
                                       bsl::vector<unsigned short> *result,
                                       unsigned                     numWords,
                                       bdlde::ByteOrder::Enum       byteOrder);
        // Populate the specified '*result' with an invalid UTF-16 sequence the
        // specified 'numWords' long.  Note that 'numWords' includes the
        // terminating 0, and the behavior is undefined if 'numWords < 2'.


                                 // TEST CASES

  public:
    // PUBLIC CLASS METHODS
    static void testCase16(bdlde::ByteOrder::Enum byteOrder);
        // Test the calculations of length estimates for UTF-16 containing
        // errors translated into UTF-8.  The UTF-16 is to be of the specified
        // 'byteOrder'.

    static void testCase15(bdlde::ByteOrder::Enum byteOrder);
        // Test the calculations of length estimates for UTF-8 containing
        // errors translated into UTF-16.  The UTF-16 is to be of the specified
        // 'byteOrder'.

    static void testCase14(bdlde::ByteOrder::Enum byteOrder);
        // Test length methods on valid random unicode strings, both for UTF-8
        // translated to UTF-16, and for UTF-16 translated to UTF-8, where the
        // byte order of the UTF-16 is determined by the specified 'byteOrder'.

    template <class STRING,
              class WSTRING,
              class VECTOR_C,
              class VECTOR_WC,
              class VECTOR_US>
    static void testCase13();
        // Verify backwards byte order translation for 'std' cotainers.

    template<class STRING, class WSTRING>
    static void testCase12();
        // Verify translation of strings containing embedded zeros for 'std'
        // containers.

    template <class VECTOR, class STRING, class WIDE_CHAR>
    static void testCase11();
        // Verify utf16 to utf8 translation.

    template <class VECTOR,
              class WSTRING>
    static void testCase10();
        // Verify utf8 to utf16 translation.
};
RandGen TestDriver::s_randGen;

inline
unsigned TestDriver::numBitsSet(unsigned value)
{
    unsigned ret;
    for (ret = 0; 0 != value; ++ret) {
        value &= value - 1;    // clears low-order bit
    }

    return ret;
}

inline
unsigned TestDriver::calculateRandomNumOctets()
{
    // By taking 3 random bits, counting the number that are set, and adding 1,
    // we get 'P(1)' == P(4) == 12.5%' and 'P(2) == P(3) == 37%'.  Single
    // octets are not very interesting, so we don't want a lot of them, and
    // quadruple octets are normally very rare in UTF-8.

    unsigned rand3 = s_randGen.bits(3);

    unsigned numOctets = 1 + numBitsSet(rand3);
    BSLS_ASSERT_SAFE(0 < numOctets && numOctets <= 4);

    return numOctets;
}

inline
bool TestDriver::fitsInSingleOctet(UnicodeCodePoint uc)
{
    return 0 == (uc & (~UnicodeCodePoint(0) << ONE_OCT_CONT_WID));
}

inline
bool TestDriver::fitsInTwoOctets(UnicodeCodePoint uc)
{
    return 0 == (uc & (~UnicodeCodePoint(0) << (TWO_OCT_CONT_WID +
                                                          CONTINUE_CONT_WID)));
}

inline
bool TestDriver::fitsInThreeOctets(UnicodeCodePoint uc)
{
    return 0 == (uc & (~UnicodeCodePoint(0) << (THREE_OCT_CONT_WID +
                                                      2 * CONTINUE_CONT_WID)));
}

inline
void TestDriver::encodeTwoOctets(char *octBuf, UnicodeCodePoint isoBuf)
{
    BSLS_ASSERT_SAFE(fitsInTwoOctets(isoBuf));

    octBuf[1] = static_cast<char>(CONTINUE_TAG |
                             (isoBuf                       & ~CONTINUE_MASK));
    octBuf[0] = static_cast<char>(TWO_OCTET_TAG |
                            ((isoBuf >> CONTINUE_CONT_WID) & ~TWO_OCTET_MASK));
}

inline
void TestDriver::encodeThreeOctets(char *octBuf, UnicodeCodePoint isoBuf)
{
    BSLS_ASSERT_SAFE(fitsInThreeOctets(isoBuf));

    for (unsigned uu = 0; uu < 2; ++uu) {
        octBuf[2 - uu] = static_cast<char>(CONTINUE_TAG |
                        ((isoBuf >> uu * CONTINUE_CONT_WID) & ~CONTINUE_MASK));
    }
    octBuf[0] = static_cast<char>(THREE_OCTET_TAG |
                      ((isoBuf >> 2 * CONTINUE_CONT_WID) & ~THREE_OCTET_MASK));
}

inline
void TestDriver::encodeFourOctets(char *octBuf, UnicodeCodePoint isoBuf)
{
    for (unsigned uu = 0; uu < 3; ++uu) {
        octBuf[3 - uu] = static_cast<char>(CONTINUE_TAG |
                        ((isoBuf >> uu * CONTINUE_CONT_WID) & ~CONTINUE_MASK));
    }
    octBuf[0] = static_cast<char>(FOUR_OCTET_TAG |
                       ((isoBuf >> 3 * CONTINUE_CONT_WID) & ~FOUR_OCTET_MASK));
}

inline
bool TestDriver::isContinuation(char c)
{
    return (c & CONTINUE_MASK) == CONTINUE_TAG;
}

                            // ---------------------------
                            // PRIVATE OUT-OF-LINE METHODS
                            // ---------------------------

// PRIVATE CLASS METHODS
void TestDriver::copyUtf16ToWstring(
                                 bsl::wstring                       *wStr,
                                 const bsl::vector<unsigned short>&  utf16In,
                                 bdlde::ByteOrder::Enum              byteOrder)
{
    const bsl::size_t len = utf16In.size() - 1;

    wStr->clear();
    wStr->resize(len);

    wchar_t *pwc = wStr->data();
    const unsigned short *pu = utf16In.data(), * const end = pu + len;
    if (sizeof(*pwc) == sizeof(*pu) || bdlde::ByteOrder::e_HOST == byteOrder) {
        for (; pu < end; ++pu, ++pwc) {
            *pwc = *pu;
        }
    }
    else {
        for (; pu < end; ++pu, ++pwc) {
            const wchar_t w = bsls::ByteOrderUtil::swapBytes(*pu);
            *pwc = bsls::ByteOrderUtil::swapBytes(w);
        }
    }
    ASSERT(bsl::wcslen(wStr->c_str()) == len);        // no embedded '\0's
}

void TestDriver::appendRandomInvalidSingleOctet(bsl::string *string)
    // Append a random invalid single byte to the specified '*string'.  Note
    // that the byte appended may be legal if it is a continuation byte and the
    // preceding contents of '*string' is a truncated sequence.
{
    enum ErrorType { e_ILLEGAL_CHAR,
                     e_UNEXPECTED_CONTINUATION,
                     k_NUM_ERROR_TYPES,

                     k_RAND_BITS = 1u };

    BSLMF_ASSERT((1u << k_RAND_BITS) == k_NUM_ERROR_TYPES);

    const ErrorType errorType = static_cast<ErrorType>(
                                                  s_randGen.bits(k_RAND_BITS));

    const char c = e_ILLEGAL_CHAR == errorType
                 ? // Bytes with high 5 bits all set '11111xxx' are never legal
                   // characters in UTF-8.

                   static_cast<char>(0xf8u | s_randGen.bits(3))
                 : // unexpected continuation '10xxxxxx'.

                   static_cast<char>(0x80u | s_randGen.bits(6));

    BSLS_ASSERT_SAFE(isContinuation(c) ==
                                     (errorType == e_UNEXPECTED_CONTINUATION));

    *string += c;
}

void TestDriver::appendRandomInvalidTwoOctets(bsl::string *string)
    // Append a random invalid two-octet code point to the specified '*string'.
{
    enum ErrorType { e_TRUNC,        // wrong byte appended before sequence
                                     // completes
                     e_END,          // sequence ends without finishing

                     e_NON_MINIMAL,  // value should have been represented as
                                     // single byte

                     k_NUM_ERROR_TYPES,

                     k_RAND_BITS = 2 + k_RAND_FUDGE_BITS };

    BSLMF_ASSERT(k_NUM_ERROR_TYPES * k_RAND_FUDGE_MULTIPLE <
                                                          (1u << k_RAND_BITS));

    const ErrorType errorType = static_cast<ErrorType>(
                              s_randGen.bits(k_RAND_BITS) % k_NUM_ERROR_TYPES);

    switch (errorType) {
      case e_TRUNC:
      case e_END: {
        // truncated two-byte sequence

        // first byte is correct start to 2-byte sequence '110xxxxx'

        *string += static_cast<char>(TWO_OCTET_TAG | s_randGen.bits(5));

        if (e_TRUNC == errorType) {
            unsigned char wrongByte;
            do {
                wrongByte = static_cast<unsigned char>(s_randGen.getChar());
            } while (!wrongByte || isContinuation(wrongByte));

            *string += static_cast<char>(wrongByte);
        }
      } break;
      case e_NON_MINIMAL: {
        // non-minimal two-byte encoding

        const bsl::size_t len = string->length();
        string->resize(len + 2);

        const unsigned char asciiByte = static_cast<char>(s_randGen.bits(7));
        encodeTwoOctets(string->data() + len, asciiByte);
      } break;
      default: {
        ASSERT(0);
      }
    }
}

void TestDriver::appendRandomInvalidThreeOctets(bsl::string *string)
    // Append a random invalid three-octet code point to the specified
    // '*string'.
{
    enum ErrorType { e_TRUNC,        // wrong byte appended before sequence
                                     // completes

                     e_END,          // sequence ends without finishing

                     e_NON_MINIMAL,  // value should have been represented as
                                     // two or fewer octers

                     e_SURROGATE,    // value reserved for surrogate planes in
                                     // UTF-16

                     k_NUM_ERROR_TYPES,

                     k_RAND_BITS = 2u };

    BSLMF_ASSERT(k_NUM_ERROR_TYPES == (1u << k_RAND_BITS));

    // '% k_NUM_ERROR_TYPES' would be redundant here

    const ErrorType errorType = static_cast<ErrorType>(
                                                  s_randGen.bits(k_RAND_BITS));

    switch (errorType) {
      case e_TRUNC:
      case e_END: {
        // three-byte sequence truncated before completion

        // first byte is correct start to 3-byte sequence '1110xxxx'

        *string += static_cast<char>(0xe0u | s_randGen.bits(4));

        const bool contByte = s_randGen.bits(1);

        if (contByte) {
            // append correct continuation

            *string += static_cast<char>(s_randGen.bits(6) | 0x80u);
        }

        if (e_TRUNC == errorType) {
            // append non-continuation to interrupt, cause error

            unsigned char wrongByte;
            do {
                wrongByte = s_randGen.getChar();
            } while (!wrongByte || isContinuation(wrongByte));
            *string += static_cast<char>(wrongByte);
        }
      } break;
      case e_NON_MINIMAL: {
        enum { k_DATA_BITS = TWO_OCT_CONT_WID + CONTINUE_CONT_WID,

               k_RAND_UC_BITS = k_DATA_BITS };

        const unsigned uc = s_randGen.bits(k_RAND_UC_BITS);

        ASSERT(fitsInTwoOctets(uc));

        const bsl::size_t len = string->length();
        string->resize(len + 3);

        encodeThreeOctets(string->data() + len , uc);
      } break;
      case e_SURROGATE: {
        enum { k_MIN_SURROGATE = 0xd800u,    // reserved values for
               k_MAX_SURROGATE = 0xdfffu,    // multi-word UTF-16 sequences

               k_FLOOR = k_MIN_SURROGATE,

               k_MOD   = 1 + k_MAX_SURROGATE - k_FLOOR,

               k_RAND_UC_BITS = 11u };

        BSLMF_ASSERT(k_MOD == (1u << k_RAND_UC_BITS));

        // '% k_MOD' would be redundant below.

        unsigned uc = k_FLOOR + s_randGen.bits(k_RAND_UC_BITS);
        BSLS_ASSERT_SAFE(uc >= k_MIN_SURROGATE);
        BSLS_ASSERT_SAFE(uc <= k_MAX_SURROGATE);

        ASSERT(!fitsInTwoOctets(uc));
        ASSERT(fitsInThreeOctets(uc));

        const bsl::size_t len = string->length();
        string->resize(len + 3);

        encodeThreeOctets(string->data() + len , uc);
      } break;
      default: {
        ASSERT(0);
      }
    }
}

void TestDriver::appendRandomInvalidFourOctets(bsl::string *string)
    // Append a random invalid four-octet code point to the specified
    // '*string'.
{
    enum ErrorType { e_TRUNC,        // wrong byte appended before sequence
                                     // completes

                     e_END,          // sequence ends without finishing

                     e_NON_MINIMAL,  // value should have been represented as
                                     // three or fewer octers

                     e_TOO_HIGH,     // value was above valid unicode

                     k_NUM_ERROR_TYPES,

                     k_RAND_BITS = 2u };

    BSLMF_ASSERT((1u << k_RAND_BITS) == k_NUM_ERROR_TYPES);

    // '% k_NUM_ERROR_TYPES' would be redundant below.

    const ErrorType errorType = static_cast<ErrorType>(
                                                  s_randGen.bits(k_RAND_BITS));

    switch (errorType) {
      case e_TRUNC:
      case e_END: {
        // first byte of 4-byte sequence '11110xxx'

        *string += static_cast<char>(FOUR_OCTET_TAG | s_randGen.bits(3));

        // Append valid continuation bytes.  The number of continuation bytes
        // to append is random in the range '[ 0 .. 2 ]'.

        enum { k_CONT_MOD = 3,
               k_RAND_CONT_BITS = 2 + k_RAND_FUDGE_BITS };

        BSLMF_ASSERT(k_CONT_MOD * k_RAND_FUDGE_MULTIPLE <
                                                     (1u << k_RAND_CONT_BITS));

        const unsigned contBytes = s_randGen.bits(k_RAND_CONT_BITS) %
                                                                    k_CONT_MOD;
        ASSERT(contBytes <= 2);

        for (unsigned uu = 0; uu < contBytes; ++uu) {
            // continuation byte: '10xxxxxx'

            *string += static_cast<char>(CONTINUE_TAG | s_randGen.bits(6));
        }

        if (e_TRUNC == errorType) {
            // append non-continuation to interrupt, cause error

            unsigned char wrongByte;
            do {
                wrongByte = s_randGen.getChar();
            } while (!wrongByte || isContinuation(wrongByte));
            *string += static_cast<char>(wrongByte);
        }
      } break;
      case e_NON_MINIMAL: {
        enum { k_3_OCT_BITS   = THREE_OCT_CONT_WID + 2 * CONTINUE_CONT_WID,
               k_MOD          = 1u << k_3_OCT_BITS,
               k_RAND_UC_BITS = k_3_OCT_BITS };

        BSLMF_ASSERT(k_MOD == (1u << k_RAND_UC_BITS));

        unsigned uc = s_randGen.bits(k_RAND_UC_BITS);

        ASSERT(fitsInThreeOctets(uc));

        const bsl::size_t len = string->length();
        string->resize(len + 4);

        encodeFourOctets(string->data() + len, uc);
      } break;
      case e_TOO_HIGH: {
        enum { k_MAX_LEGAL_UNICODE = 0x10ffffu,
               k_FLOOR = k_MAX_LEGAL_UNICODE + 1,
               k_DATA_BITS  = FOUR_OCT_CONT_WID + 3 * CONTINUE_CONT_WID,
               k_MOD = (1 << k_DATA_BITS) - k_FLOOR,

               k_RAND_BITS = k_DATA_BITS + k_RAND_FUDGE_BITS };

        BSLMF_ASSERT(k_RAND_BITS < 32);
        BSLMF_ASSERT(k_MOD * k_RAND_FUDGE_MULTIPLE < (1u << k_RAND_BITS));

        const unsigned uc = k_FLOOR + s_randGen.bits(k_RAND_BITS) % k_MOD;

        ASSERT(uc < (1 << k_DATA_BITS));

        const bsl::size_t len = string->length();
        string->resize(len + 4);

        encodeFourOctets(string->data() + len, uc);
      } break;
      default: {
        ASSERT(0);
      }
    }
}

void TestDriver::appendRandomInvalidUtf8CodePoint(bsl::string *string)
    // Append random invalid UTF-8 to the specified '*string'.  Note that the
    // appended sequence will usually consistitute one code point, but in some
    // cases where a truncated sequence is followed by the first char of
    // another truncated sequence, it will consitute 2 invalid code points.
{
    const unsigned numOctets = calculateRandomNumOctets();

    // The optimizer will very probably chain the following calls

    switch (numOctets) {
      case 1: {
        appendRandomInvalidSingleOctet(string);
      } break;
      case 2: {
        appendRandomInvalidTwoOctets(  string);
      } break;
      case 3: {
        appendRandomInvalidThreeOctets(string);
      } break;
      case 4: {
        appendRandomInvalidFourOctets( string);
      } break;
      default: {
        ASSERT(0);
      }
    }
}

void TestDriver::appendRandomValidSingleOctet(bsl::string *string)
{
    char c;
    do {
        c = static_cast<char>(s_randGen.bits(7));
    } while (!c);

    *string += c;
}

void TestDriver::appendRandomValidTwoOctets(bsl::string *string)
{
    enum { k_FLOOR     = CONTINUE_TAG,
           k_DATA_BITS = TWO_OCT_CONT_WID + CONTINUE_CONT_WID,
           k_MOD       = (1u << k_DATA_BITS) - k_FLOOR,

           k_RAND_BITS = k_DATA_BITS + 3 };
    BSLMF_ASSERT(k_MOD * 8 < (1u << k_RAND_BITS));

    const UnicodeCodePoint uc = k_FLOOR + s_randGen.bits(k_RAND_BITS) % k_MOD;

    ASSERT(k_FLOOR <= uc);
    ASSERT(fitsInTwoOctets(uc));

    const bsl::size_t len = string->length();
    string->resize(len + 2);

    encodeTwoOctets(string->data() + len , uc);
}

void TestDriver::appendRandomValidThreeOctets(bsl::string *string)
{
    enum { k_FLOOR     = 1 << (TWO_OCT_CONT_WID + CONTINUE_CONT_WID),
           k_DATA_BITS = THREE_OCT_CONT_WID + 2 * CONTINUE_CONT_WID,
           k_MOD       = (1 << k_DATA_BITS) - k_FLOOR,

           k_RAND_BITS = k_DATA_BITS + 3,

           k_MIN_UTF_16_DOUBLE_PLANE = 0xd800u,
           k_MAX_UTF_16_DOUBLE_PLANE = 0xdfffu };

    BSLMF_ASSERT(k_MOD * 8u < (1u << k_RAND_BITS));

    UnicodeCodePoint uc;
    do {
        uc = k_FLOOR + s_randGen.bits(k_RAND_BITS) % k_MOD;
    } while (k_MIN_UTF_16_DOUBLE_PLANE <= uc &&
                                              uc <= k_MAX_UTF_16_DOUBLE_PLANE);

    ASSERT(!fitsInTwoOctets(uc));
    ASSERT(fitsInThreeOctets(uc));

    const bsl::size_t len = string->length();
    string->resize(len + 3);

    encodeThreeOctets(string->data() + len, uc);
}

void TestDriver::appendRandomValidFourOctets(bsl::string *string)
{
    enum { k_FLOOR_BITS = THREE_OCT_CONT_WID + 2 * CONTINUE_CONT_WID,
           k_FLOOR      = 1 << k_FLOOR_BITS,
           k_DATA_BITS  = FOUR_OCT_CONT_WID  + 3 * CONTINUE_CONT_WID,
           k_MOD        = (1 << k_DATA_BITS) - k_FLOOR,

           k_RAND_BITS  = k_DATA_BITS + k_RAND_FUDGE_BITS,

           k_MAX_LEGAL_UNICODE_POINT = 0x10ffffu };

    BSLMF_ASSERT(k_RAND_BITS < 32);
    BSLMF_ASSERT(k_MOD * k_RAND_FUDGE_MULTIPLE < (1u << k_RAND_BITS));

    UnicodeCodePoint uc;
    do {
        uc = k_FLOOR + s_randGen.bits(k_RAND_BITS) % k_MOD;
    } while (k_MAX_LEGAL_UNICODE_POINT < uc);

    ASSERT(!fitsInThreeOctets(uc));

    const bsl::size_t len = string->length();
    string->resize(len + 4);

    encodeFourOctets(string->data() + len, uc);
}

void TestDriver::appendRandomValidUtf8CodePoint(bsl::string *string)
{
    const unsigned numOctets = calculateRandomNumOctets();

    switch (numOctets) {
      case 1: {
        appendRandomValidSingleOctet(string);
      } break;
      case 2: {
        appendRandomValidTwoOctets(string);
      } break;
      case 3: {
        appendRandomValidThreeOctets(string);
      } break;
      case 4: {
        appendRandomValidFourOctets(string);
      } break;
      default: {
        ASSERT(0);
      }
    }
}

void TestDriver::randomUtf8String(bsl::string *result,
                                  unsigned     numSequences,
                                  Validity     validity)
{
    BSLS_ASSERT_SAFE(0 < numSequences);
    --numSequences;

    switch (validity) {
      case e_VALID: {
        result->clear();

        for (unsigned uu = 0; uu < numSequences; ++uu) {
            appendRandomValidUtf8CodePoint(result);
        }
      } break;
      case e_INVALID: {
        BSLS_ASSERT_SAFE(0 < numSequences);

        if (1 == numSequences) {
            result->clear();

            appendRandomInvalidUtf8CodePoint(result);
        }
        else {
            // We want to make an invalid UTF-8 string with multiple code
            // points, so we want a random mix of valid and invalid code
            // points.  We always want '*result' to contain an invalid sequence
            // when this function finishes, but we may randomly not have any
            // errors, so we are ready to repeat until we get that outcome.

            bool isValid;
            do {
                result->clear();

                // These two variables monitor the process of building the
                // string, and verify whether our assumptions about correct vs
                // incorrect strings are accurate.

                int  prevErrorIdx   = -1;
                bool adjacentErrors = false;

                for (unsigned uu = 0; uu < numSequences; ++uu) {
                    if (s_randGen.bits(1)) {
                        appendRandomInvalidUtf8CodePoint(result);
                        adjacentErrors |=
                                  0 <= prevErrorIdx && prevErrorIdx + 1u == uu;
                        prevErrorIdx = uu;
                    }
                    else {
                        appendRandomValidUtf8CodePoint(result);
                    }
                }

                // There should be no embedded '\0's.

                BSLS_ASSERT_SAFE(bsl::strlen(result->c_str()) ==
                                                             result->length());

                // The string might not have any errors if we never randomly
                // chose to put any invalid code points in, or if two adjacent
                // incomplete code points merged to make a valid code point.
                // Otherwise the result should always be invalid.

                isValid = bdlde::Utf8Util::isValid(result->c_str());

                BSLS_ASSERT_SAFE(0 <= prevErrorIdx ? !isValid || adjacentErrors
                                                   :  isValid);
            } while (isValid);

            return;                                                   // RETURN
         }
      } break;
      default: {
        ASSERT(0);
      }
    }

    // There should be no embedded '\0's.

    BSLS_ASSERT_SAFE(bsl::strlen(result->c_str()) == result->length());
}

unsigned short TestDriver::randomUtf16NonSurrogate()
{
    // In UTf-16, a non-surrogate value is any non-zero 16-bit value outside
    // the range '[ k_LO_SURROGATE_BASE .. k_SURROGATE_CEILING )'.

    unsigned short ret;
    bool           isSurrogate;
    do {
        ret = static_cast<unsigned short>(s_randGen.bits(16));

        isSurrogate = k_LO_SURROGATE_BASE <= ret && ret < k_SURROGATE_CEILING;
    } while (!ret || isSurrogate);

    return ret;
}

unsigned short TestDriver::randomUtf16SurrogateLo()
{
    return static_cast<unsigned short>(
                  k_LO_SURROGATE_BASE + s_randGen.bits(k_SURROGATE_RAND_BITS));
}

unsigned short TestDriver::randomUtf16SurrogateHi()
{
    return static_cast<unsigned short>(
                  k_HI_SURROGATE_BASE + s_randGen.bits(k_SURROGATE_RAND_BITS));
}

void TestDriver::randomInvalidUtf16Vector(
                                        bsl::vector<unsigned short> *result,
                                        unsigned                     numWords,
                                        bdlde::ByteOrder::Enum       byteOrder)
{
    // The rules for correct UTF-16 are extremely simple -- surrogates are only
    // allowed to occur in pairs, a lo immediately followed by a hi.  Other
    // than that, single words can occur anywhere.

    enum WordType { e_SINGLE_WORD,
                    e_LO_SURROGATE,
                    e_HI_SURROGATE,

                    k_NUM_WORD_TYPES };

    BSLS_ASSERT_SAFE(1 < numWords);
    --numWords;

    if (1 == numWords) {
        // Only way to make an invalid sequence is one surrogate.

        result->clear();
        const unsigned short w = s_randGen.bits(1) ? randomUtf16SurrogateLo()
                                                   : randomUtf16SurrogateHi();
        result->push_back(bdlde::ByteOrder::e_HOST == byteOrder
                        ? w
                        : bsls::ByteOrderUtil::swapBytes(w));
    }
    else {
        // We just generate a random sequence of UTF-16 words until we get one
        // that has at least one error.  The variable 'errorPresent' monitors
        // the sequence as it's created to see if any errors occurred.

        bool errorPresent;
        do {
            result->clear();

            errorPresent = false;
            WordType prevWordType = e_SINGLE_WORD;  // this sets us up so that
                                                    // if the first word is a
                                                    // hi surrogate, we detect
                                                    // the error

            for (unsigned uu = 0; uu < numWords; ++uu) {
                // This gives 'e_SINGLE_WORD' twice the probability of the
                // other 2 word types.  In real UTF-16, surrogates are pretty
                // rare.

                const WordType thisWordType = static_cast<WordType>(
                                         s_randGen.bits(2) % k_NUM_WORD_TYPES);

                const unsigned short w = e_SINGLE_WORD == thisWordType
                                       ? randomUtf16NonSurrogate()
                                       : e_LO_SURROGATE == thisWordType
                                       ? randomUtf16SurrogateLo()
                                       : randomUtf16SurrogateHi();

                result->push_back(bdlde::ByteOrder::e_HOST == byteOrder
                                ? w
                                : bsls::ByteOrderUtil::swapBytes(w));

                // If prev was a lo surrogate, and this is not a hi surrogate,
                // it's an error.  If prev was not a lo surrogate, and this is
                // a hi surrogate, it's an error.

                errorPresent |= (e_LO_SURROGATE == prevWordType) !=
                                              (e_HI_SURROGATE == thisWordType);

                prevWordType = thisWordType;
            }

            // if the last word in the sequence was a lo surrogate, it's an
            // error

            errorPresent |= e_LO_SURROGATE == prevWordType;
        } while (!errorPresent);
    }

    BSLS_ASSERT_SAFE(bsl::find(result->begin(), result->end(), 0) ==
                                                                result->end());
    BSLS_ASSERT_SAFE(result->size() == numWords);

    result->push_back(0);
}


                            // -----------------
                            // PUBLIC TEST CASES
                            // -----------------

// PUBLIC CLASS METHODS
void TestDriver::testCase16(bdlde::ByteOrder::Enum byteOrder)
    // ------------------------------------------------------------------------
    // UTF-8 LENGTH CALCULATION TEST -- INCORRECT UNICODE
    //
    // Concerns:
    //: 1 That 'computeRequiredUtf8Bytes' (both overloads) correctly predicts
    //:   the length of output when the unicode input has errors.
    //
    // Plan:
    //: 1 Call 'randomInvalidUtf16Vector' to create a 'vector<unsigned short>'
    //:   contain invalid UTF-16.
    //:
    //: 2 Call 'computeRequiredUtf8Bytes' both ways (passing an end ptr, and
    //:   null-terminated) on the UTF-16 vector to predict the length of the
    //:   UTF-8.
    //:   o observe that the values returned by both overloads match.
    //:
    //:   o translate back to a new UTF-8 'string' and observe that its length
    //:     matches the predicted length, and that the string doesn't match the
    //:     original valid UTF-8 'string'.
    //:
    //: 3 Copy the UTF-16 'vector<unsigned short>' to a 'wstring' using
    //:   'copyUtf16ToWstring'.
    //:
    //: 4 Call 'computeRequiredUtf8Bytes' both ways (passing an end ptr, and
    //:   null-terminated) on the UTF-16 'wstring' to predict the length of the
    //:   UTF-8.
    //:   o observe that the values returned by both overloads match the values
    //:     predicted when the input was a 'vector'.
    //:
    //:   o translate back to a new UTF-8 'string' and observe that its length
    //:     matches the predicted length, and that it doesn't match the
    //:     original valid UTF-8 'string', and that the string does match the
    //:     utf8 string translated back from a 'vector'.
    //
    // Testing:
    //   computeRequiredUtf8Bytes(ushort *, ushort *, byteOrder);
    //   computeRequiredUtf8Bytes(w_char_t *, w_char_t *, byteOrder);
    // ------------------------------------------------------------------------
{
    // No reason to use 'bslma::TestAllocator' in this test -- length
    // calculation allocates no memory, so we have no need for all the test
    // allocator's time-wasting recordkeeping.

    bslma::Allocator *alloc = &bslma::NewDeleteAllocator::singleton();

    enum { k_ITERATIONS = 300 * 1000 };
    for (int ii = 0; ii < k_ITERATIONS; ++ii) {
        bsl::vector<unsigned short> utf16(alloc);
        utf16.reserve(4092);
        const unsigned RANDOM_NUM_WORDS = ii < 10000
                                        ? (ii < 1000 ? 2 : 3)
                                        : 4 + s_randGen.bits(3);

        randomInvalidUtf16Vector(&utf16, RANDOM_NUM_WORDS, byteOrder);
        ASSERT(utf16.size() == RANDOM_NUM_WORDS);

        if (veryVerbose) cout << displayUtf16(utf16.data(), byteOrder) << endl;

        // Calculate the size to translate back to UTF8.

        const bsl::size_t PREDICTED_UTF8_LEN =
                                    Util::computeRequiredUtf8Bytes(
                                               utf16.data(),
                                               utf16.data() + utf16.size() - 1,
                                               byteOrder);
        const bsl::size_t PREDICTED_UTF8_LEN_B =
                                        Util::computeRequiredUtf8Bytes(
                                              utf16.data(),
                                              static_cast<unsigned short *>(0),
                                              byteOrder);
        ASSERTV(displayUtf16(utf16.data(), byteOrder),
                                      PREDICTED_UTF8_LEN, PREDICTED_UTF8_LEN_B,
                                   PREDICTED_UTF8_LEN == PREDICTED_UTF8_LEN_B);

        // Translate the utf16 back to UTF-8

        bsl::size_t utf8CodePointsB = -1;
        bsl::string utf8B(alloc);
        utf8B.reserve(4092);
        int rc = Util::utf16ToUtf8(&utf8B,
                                   utf16.data(),
                                   &utf8CodePointsB,
                                   '?',
                                   byteOrder);
        ASSERTV(rc, displayUtf16(utf16.data(), byteOrder), byteOrder,
                                                               sizeof(wchar_t),
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(utf8CodePointsB <= RANDOM_NUM_WORDS);
        ASSERT(PREDICTED_UTF8_LEN == utf8B.length() + 1);

        // Translate the utf16 back to UTF-8, passing input length

        utf8CodePointsB = -1;
        utf8B.clear();
        rc = Util::utf16ToUtf8(&utf8B,
                               utf16.data(),
                               utf16.size() - 1,
                               &utf8CodePointsB,
                               '?',
                               byteOrder);
        ASSERTV(rc, displayUtf16(utf16.data(), byteOrder), byteOrder,
                                                               sizeof(wchar_t),
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(utf8CodePointsB <= RANDOM_NUM_WORDS);
        ASSERT(PREDICTED_UTF8_LEN == utf8B.length() + 1);

        // Translate to 'std::string'.

        bsl::size_t utf8CodePointsC = -1;
        std::string utf8C;
        utf8C.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8C,
                               utf16.data(),
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(rc, displayUtf16(utf16.data(), byteOrder), byteOrder,
                                                               sizeof(wchar_t),
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(utf8CodePointsB <= RANDOM_NUM_WORDS);
        ASSERT(utf8CodePointsC == utf8CodePointsB);
        ASSERT(PREDICTED_UTF8_LEN == utf8C.length() + 1);
        ASSERT(utf8B == utf8C);

        // Translate to 'std::string' passing input length.

        utf8CodePointsC = -1;
        utf8C.clear();
        rc = Util::utf16ToUtf8(&utf8C,
                               utf16.data(),
                               utf16.size() - 1,
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(rc, displayUtf16(utf16.data(), byteOrder), byteOrder,
                                                               sizeof(wchar_t),
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(utf8CodePointsB <= RANDOM_NUM_WORDS);
        ASSERT(utf8CodePointsC == utf8CodePointsB);
        ASSERT(PREDICTED_UTF8_LEN == utf8C.length() + 1);
        ASSERT(utf8B == utf8C);

        // Copy UTF-16 'vector' to 'wstring utf16W'

        bsl::wstring utf16W(alloc);
        copyUtf16ToWstring(&utf16W, utf16, byteOrder);

        // Run both overloads of length prediction of UTF-8, see that they
        // match the predictions made when predicted based on 'vector'.

        bsl::size_t len = Util::computeRequiredUtf8Bytes(
                                               utf16W.data(),
                                               utf16W.data() + utf16W.length(),
                                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);
        len = Util::computeRequiredUtf8Bytes(utf16W.data(),
                                             (wchar_t *) 0,
                                             byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);

        // Translate 'wstring' back to UTF-8, observe that the length of the
        // result matches the calculations, and the value of the UTF-8
        // matches the value from translating the 'vector'.

        utf8CodePointsC = -1;
        utf8C.clear();
        rc = Util::utf16ToUtf8(&utf8C,
                               utf16W.data(),
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8C.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8C);

        utf8CodePointsC = -1;
        utf8C.clear();
        rc = Util::utf16ToUtf8(&utf8C,
                               utf16W,
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8C.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8C);

        utf8CodePointsC = -1;
        utf8B.clear();
        rc = Util::utf16ToUtf8(&utf8B,
                               utf16W.data(),
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8B.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8C);

        utf8CodePointsC = -1;
        utf8B.clear();
        rc = Util::utf16ToUtf8(&utf8B,
                               utf16W,
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8B.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8C);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        utf8CodePointsC = -1;
        std::pmr::string utf8P(std::pmr::new_delete_resource());
        utf8P.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8P,
                               utf16.data(),
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8P.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8P);

        utf8CodePointsC = -1;
        utf8P.clear();
        rc = Util::utf16ToUtf8(&utf8P,
                               utf16.data(),
                               utf16.size() - 1,
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8P.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8P);

        utf8CodePointsC = -1;
        utf8P.clear();
        rc = Util::utf16ToUtf8(&utf8P,
                               utf16W,
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8P.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8P);

        utf8CodePointsC = -1;
        utf8P.clear();
        rc = Util::utf16ToUtf8(&utf8P,
                               utf16W.c_str(),
                               &utf8CodePointsC,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8P.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsC);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8C), utf8B == utf8P);
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        const bsl::u16string utf16U16(usToC16(utf16.data()), alloc);

        len = Util::computeRequiredUtf8Bytes(
                                           utf16U16.data(),
                                           utf16U16.data() + utf16U16.length(),
                                           byteOrder);

        ASSERTV(displayUtf16(utf16U16.data(), byteOrder),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);

        bsl::size_t utf8CodePointsD = -1;
        std::string utf8D;
        utf8D.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8D,
                               utf16U16.data(),
                               &utf8CodePointsD,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16U16.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8D.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsD);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8D), utf8B == utf8D);

        utf8CodePointsD = -1;
        bsl::string utf8E(&ta);
        utf8E.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8E,
                               utf16U16.data(),
                               &utf8CodePointsD,
                               '?',
                               byteOrder);
        ASSERTV(displayUtf16(utf16U16.data(), byteOrder), rc, byteOrder,
                          bdlde::CharConvertStatus::k_INVALID_INPUT_BIT == rc);
        ASSERT(PREDICTED_UTF8_LEN == utf8E.length() + 1);
        ASSERT(utf8CodePointsB == utf8CodePointsD);
        ASSERTV(displayUtf8(utf8B), displayUtf8(utf8D), utf8B == utf8E);
#endif
    }
}

void TestDriver::testCase15(bdlde::ByteOrder::Enum byteOrder)
    // ------------------------------------------------------------------------
    // UTF-16 LENGTH CALCULATION TEST -- INCORRECT UNICODE
    //
    // Concerns:
    //: 1 That 'computeRequiredUtf16Words' called both ways (passing an end
    //:   ptr, and null-terminated) correctly predicts the length of output
    //:   when the input has UTF-8 errors.
    //
    // Plan:
    //: 1 Use 'randomUtf8String' to get a random substring from the
    //:   'utf8MultiLang' example containing invalid UTF-8.
    //:
    //: 2 Call both overloads 'computeRequiredUtf16Words' both ways, save the
    //:   values and make sure they match.
    //:
    //: 3 Translate to a 'vector<unsigned short>' containing UTF-16 with the
    //:   error character as '?' and the specified 'byteOrder'
    //:   o Observe that the error was detected
    //:
    //:   o Observe the length of the UTF-16 output matches the predictions.
    //:
    //: 4 Translate to a 'wstring' containing UTF-16 with the error character
    //:   as '?' and the specified 'byteOrder'
    //:   o Observe that the error was detected
    //:
    //:   o Observe the length of the UTF-16 output matches the predictions.
    //
    // Testing:
    //   computeRequiredUtf16Words(char *, char *);
    // ------------------------------------------------------------------------
{
    // No reason to use 'bslma::TestAllocator' in this test -- length
    // calculation allocates no memory, so we have no need for all the test
    // allocator's time-wasting recordkeeping.

    bslma::Allocator *alloc = &bslma::NewDeleteAllocator::singleton();

    enum { k_ITERATIONS = 400 * 1000 };
    for (int ii = 0; ii < k_ITERATIONS; ++ii) {
        bsl::string utf8(alloc);
        utf8.reserve(4092);

        // Create a random UTF-8 string with at least one error in it.

        const unsigned RANDOM_CODE_POINTS = ii < 1000
                                          ? 2
                                          : 3 + s_randGen.bits(3);

        randomUtf8String(&utf8, RANDOM_CODE_POINTS, e_INVALID);

        if (veryVerbose) cout << displayUtf8(utf8) << endl;

        // predict length of UTF-16 string needed.

        const bsl::size_t PREDICTED_UTF16_LEN =
                         Util::computeRequiredUtf16Words(
                                     utf8.data(), utf8.data() + utf8.length());
        const bsl::size_t PREDICTED_UTF16_LEN_B =
                                 Util::computeRequiredUtf16Words(utf8.c_str());
        ASSERT(PREDICTED_UTF16_LEN == PREDICTED_UTF16_LEN_B);

        // translate to a UTF-16 'vector<unsigned short>'.  Note that the
        // number of code points may be less than 'RANDOM_CODE_POINTS' due to
        // adjacent incomplete code points merging to make single valid points,
        // or more than 'RANDOM_CODE_POINTS' due to truncated code points being
        // terminted by truncated start point.

        bsl::vector<unsigned short> utf16(alloc);
        utf16.reserve(4092);
        bsl::size_t utf16CodePoints = -1;
        int rc = Util::utf8ToUtf16(&utf16,
                                   utf8,
                                   &utf16CodePoints,
                                   '?',
                                   byteOrder);
        ASSERT(0 != rc);
        ASSERT((utf16.size() - 1) / 2 <= utf16CodePoints - 1);
        ASSERT(utf16CodePoints <= utf16.size());

        // confirm length prediction was accurate

        ASSERT(utf16.size() == PREDICTED_UTF16_LEN);

        // translate to a UTF-16 'wstring'

        bsl::wstring utf16W(alloc);
        utf16W.reserve(4092);
        bsl::size_t utf16WCodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16W,
                               utf8,
                               &utf16WCodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 != rc);

        // confirm length prediction was accurate

        ASSERT(utf16W.length() + 1 == PREDICTED_UTF16_LEN);
        ASSERT(utf16CodePoints == utf16WCodePoints);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        // Translate to 'u16string'.

        bsl::u16string utf16U16(alloc);
        utf16U16.reserve(4092);
        bsl::size_t utf16U16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16,
                               utf8,
                               &utf16U16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 != rc);

        // confirm length prediction was accurate

        ASSERT(utf16U16.length() + 1 == PREDICTED_UTF16_LEN);
        ASSERT(utf16CodePoints == utf16U16CodePoints);

        // data should exactly match

        ASSERT(sizeof(unsigned short) == sizeof(char16_t));
        ASSERT(0 == bsl::memcmp(utf16.data(),
                                utf16U16.data(),
                                sizeof(char16_t) * utf16.size()));
#endif
    }
}

void TestDriver::testCase14(bdlde::ByteOrder::Enum byteOrder)
    // ------------------------------------------------------------------------
    // UTF-16 & UTF-8 LENGTH CALCULATION TEST -- CORRECT UNICODE
    //
    // Concerns:
    //: 1 That 'computeRequiredUtf16Words' called both ways (passing an end
    //:   ptr, and null-terminated) correctly predicts the length of output
    //:   when the unicode is correct.
    //:
    //: 2 That 'computeRequiredUtf8Bytes' called both ways (passing an end ptr,
    //:   and null-terminated) correctly predicts the length of output when the
    //:   unicode is correct.
    //
    // Plan:
    //: 1 Use 'randomUtf8String' to get a random substring from the
    //:   'utf8MultiLang' example containing valid UTF-8.
    //:
    //: 2 Call both 'utf16BuferLength' both ways, save the values and make
    //:   sure they match.
    //:
    //: 3 Translate to a 'vector<unsigned short>' containing UTF-16 with the
    //:   error character as '?' and the specified 'byteOrder'.
    //:   o Observe there were no errors from the return code.
    //:
    //:   o Observe the length of the UTF-16 output matches the predictions.
    //:
    //: 4 Call 'computeRequiredUtf8Bytes' both ways on the UTF-16 vector to
    //:   predict the length of the UTF-8, observe that it matches the length
    //:   of the original UTF-8.
    //:   o translate back to a new UTF-8 'string' and observe that it matches
    //:     the original UTF-8 'string'.
    //:
    //: 5 Copy the UTF-16 'vector<unsigned short>' to a 'wstring'.
    //:
    //: 6 Call 'computeRequiredUtf8Bytes' both ways on the UTF-16 'wstring' to
    //:   predict the length of the UTF-8, observe that it matches the length
    //:   of the original UTF-8.
    //:   o translate back to a new UTF-8 'string' and observe that it matches
    //:     the original UTF-8 'string'.
    //
    // Testing:
    //   computeRequiredUtf16Words(char *, char *, byteOrder);
    //   computeRequiredUtf8Bytes(w_char_t *, w_char_t *, byteOrder);
    //   computeRequiredUtf8Bytes(ushort *, ushort *, byteOrder);
    // ------------------------------------------------------------------------
{
    // No reason to use 'bslma::TestAllocator' in this test -- length
    // calculation allocates no memory, so we have no need for all the test
    // allocator's time-wasting recordkeeping.

    bslma::Allocator *alloc = &bslma::NewDeleteAllocator::singleton();

    enum { k_ITERATIONS = 125 * 1000 };
    for (int ii = 0; ii < k_ITERATIONS; ++ii) {
        bsl::string utf8(alloc);
        utf8.reserve(4092);

        // Generate random correct UTF-8 string.

        // 'RANDOM_CODE_POINTS' is the number of code points in the string,
        // including the terminating '\0'.

        const unsigned RANDOM_CODE_POINTS = ii < 1000
                                          ? (0 == ii ? 1 : 2)
                                          : 3 + s_randGen.bits(3);

        randomUtf8String(&utf8, RANDOM_CODE_POINTS, e_VALID);

        if (veryVerbose) cout << displayUtf8(utf8) << endl;

        // Check that our string is as we expect.

        const char *begin = utf8.data();
        const char *end   = begin + utf8.length();
        ASSERT(bsl::strlen(begin) == utf8.length());
        ASSERT(bdlde::Utf8Util::isValid(utf8));
        {
            int sts;
            const char *endTraverse;
            ASSERT(RANDOM_CODE_POINTS - 1 == bdlde::Utf8Util::advanceIfValid(
                                                      &sts,
                                                      &endTraverse,
                                                      begin,
                                                      RANDOM_CODE_POINTS - 1));
            ASSERT(0 == sts);
            ASSERT(endTraverse == end);
        }
        const bsl::size_t UTF8_LEN = utf8.length() + 1;

        // predict length of UTF-16 string needed.

        const bsl::size_t PREDICTED_UTF16_LEN =
                                   Util::computeRequiredUtf16Words(begin, end);
        const bsl::size_t PREDICTED_UTF16_LEN_B =
                                   Util::computeRequiredUtf16Words(begin);
        ASSERT(PREDICTED_UTF16_LEN == PREDICTED_UTF16_LEN_B);

        // translate to a UTF-16 'vector<unsigned short>'

        bsl::vector<unsigned short> utf16(alloc);
        utf16.reserve(4092);
        bsl::size_t utf16CodePoints;
        int rc = Util::utf8ToUtf16(&utf16,
                                   utf8,
                                   &utf16CodePoints,
                                   '?',
                                   byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(utf16.size() == PREDICTED_UTF16_LEN);

        if (veryVerbose) cout << displayUtf16(utf16.data(), byteOrder) <<endl;

        utf16.clear();
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16,
                               utf8.c_str(),
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(utf16.size() == PREDICTED_UTF16_LEN);

        if (veryVerbose) cout << displayUtf16(utf16.data(), byteOrder) <<endl;

        std::vector<unsigned short> utf16s;
        utf16s.reserve(4092);
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16s,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16s.size());
        ASSERT(0 == bsl::memcmp(utf16.data(), &utf16s[0],
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(&utf16s[0], byteOrder) <<endl;

        utf16s.clear();
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16s,
                               utf8.c_str(),
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16s.size());
        ASSERT(0 == bsl::memcmp(utf16.data(), &utf16s[0],
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(&utf16s[0], byteOrder) <<endl;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        std::pmr::vector<unsigned short> utf16p(
                                              std::pmr::new_delete_resource());
        utf16p.reserve(4092);
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16p,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(utf16.size() == utf16p.size());
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16p.data(),
                                       sizeof(unsigned short) * utf16.size()));

        utf16p.clear();
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16p,
                               utf8.c_str(),
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(utf16.size() == utf16p.size());
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16p.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16p.data(), byteOrder) <<endl;
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        bsl::u16string utf16U16b(alloc);
        utf16U16b.reserve(4092);
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16b,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16U16b.length() + 1);
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16U16b.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16U16b.data(), byteOrder) <<
                                                                          endl;
        utf16U16b.clear();
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16b,
                               utf8.c_str(),
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16U16b.length() + 1);
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16U16b.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16U16b.data(), byteOrder) <<
                                                                          endl;

        std::u16string utf16U16s;
        utf16U16s.reserve(4092);
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16s,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16U16s.length() + 1);
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16U16s.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16U16s.data(), byteOrder) <<
                                                                          endl;

        utf16U16s.clear();
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16s,
                               utf8.c_str(),
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16U16s.length() + 1);
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16U16s.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16U16s.data(), byteOrder) <<
                                                                          endl;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        std::pmr::u16string utf16U16sp(std::pmr::new_delete_resource());
        utf16U16sp.reserve(4092);
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16sp,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16U16sp.length() + 1);
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16U16sp.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16U16sp.data(), byteOrder) <<
                                                                          endl;

        utf16U16sp.clear();
        utf16CodePoints = -1;
        rc = Util::utf8ToUtf16(&utf16U16sp,
                               utf8.c_str(),
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERTV(displayUtf8(utf8), RANDOM_CODE_POINTS, utf16CodePoints,
                                        RANDOM_CODE_POINTS == utf16CodePoints);
        ASSERT(PREDICTED_UTF16_LEN == utf16U16sp.length() + 1);
        ASSERT(0 == bsl::memcmp(utf16.data(), utf16U16sp.data(),
                                       sizeof(unsigned short) * utf16.size()));

        if (veryVerbose) cout << displayUtf16(utf16U16sp.data(), byteOrder) <<
                                                                          endl;
#endif

        // confirm length prediction was accurate

        ASSERT(utf16.size() == PREDICTED_UTF16_LEN);

        // Calculate the size to translate back.

        const bsl::size_t PREDICTED_UTF8_LEN =
                                    Util::computeRequiredUtf8Bytes(
                                               utf16.data(),
                                               utf16.data() + utf16.size() - 1,
                                               byteOrder);
        ASSERTV(displayUtf16(utf16.data(), byteOrder), displayUtf8(utf8),
                                             PREDICTED_UTF8_LEN, utf8.length(),
                                      PREDICTED_UTF8_LEN == utf8.length() + 1);
        const bsl::size_t PREDICTED_UTF8_LEN_B =
                    bdlde::ByteOrder::e_HOST == byteOrder
                    ? Util::computeRequiredUtf8Bytes(utf16.data())
                    : Util::computeRequiredUtf8Bytes(
                                              utf16.data(),
                                              static_cast<unsigned short *>(0),
                                              byteOrder);
        ASSERTV(displayUtf16(utf16.data(), byteOrder), displayUtf8(utf8),
                                      PREDICTED_UTF8_LEN, PREDICTED_UTF8_LEN_B,
                                   PREDICTED_UTF8_LEN == PREDICTED_UTF8_LEN_B);

        // confirm prediction matches length or original string

        ASSERT(UTF8_LEN == PREDICTED_UTF8_LEN);

        // Translate utf16 back to UTF-8, observe that the result matches

        bsl::size_t utf8CodePoints = -1;
        std::string utf8B;
        utf8B.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8B,
                               utf16.data(),
                               &utf8CodePoints,
                               0,
                               byteOrder);
        ASSERT(0 == rc);
        ASSERT(RANDOM_CODE_POINTS == utf8CodePoints);
        ASSERTV(displayUtf8(utf8), displayUtf8(utf8B), utf8 == utf8B);

        // translate 'utf8' to a 'wstring'

        bsl::wstring utf16W(alloc);
        utf16W.reserve(4092);
        rc = Util::utf8ToUtf16(&utf16W,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERT(RANDOM_CODE_POINTS  == utf16CodePoints);
        ASSERT(utf16W.length() + 1 == utf16.size());

        // Calculate the size to translate the 'wstring' back.

        bsl::size_t len = Util::computeRequiredUtf8Bytes(
                                               utf16W.data(),
                                               utf16W.data() + utf16W.length(),
                                               byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), displayUtf8(utf8),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);
        len = bdlde::ByteOrder::e_HOST == byteOrder
            ? Util::computeRequiredUtf8Bytes(utf16W.data())
            : Util::computeRequiredUtf8Bytes(utf16W.data(),
                                             (wchar_t *) 0,
                                             byteOrder);
        ASSERTV(displayUtf16(utf16W.data(), byteOrder), displayUtf8(utf8),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);

        // Translate utf16W back to UTF-8, observe that the result matches

        utf8CodePoints = -1;
        std::string utf8C;
        utf8C.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8C,
                               utf16W.data(),
                               &utf8CodePoints,
                               0,
                               byteOrder);
        ASSERT(0 == rc);
        ASSERT(RANDOM_CODE_POINTS == utf8CodePoints);
        ASSERTV(displayUtf8(utf8), displayUtf8(utf8C), utf8 == utf8C);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        // translate 'utf8' to a 'u16string'

        utf16CodePoints = -1;
        bsl::u16string utf16U16(alloc);
        utf16U16.reserve(4092);
        rc = Util::utf8ToUtf16(&utf16U16,
                               utf8,
                               &utf16CodePoints,
                               '?',
                               byteOrder);
        ASSERT(0 == rc);
        ASSERT(RANDOM_CODE_POINTS  == utf16CodePoints);
        ASSERT(utf16U16.length() + 1 == utf16.size());

        const bsl::u16string& EXP = bsl::u16string(usToC16(utf16.data()),
                                                   alloc);
        ASSERT(EXP == utf16U16);

        // Calculate the size to translate the 'wstring' back.

        len = Util::computeRequiredUtf8Bytes(
                                           utf16U16.data(),
                                           utf16U16.data() + utf16U16.length(),
                                           byteOrder);
        ASSERTV(displayUtf16(utf16U16.data(), byteOrder), displayUtf8(utf8),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);
        len = bdlde::ByteOrder::e_HOST == byteOrder
            ? Util::computeRequiredUtf8Bytes(utf16U16.data())
            : Util::computeRequiredUtf8Bytes(utf16U16.data(),
                                             (char16_t *) 0,
                                             byteOrder);
        ASSERTV(displayUtf16(utf16U16.data(), byteOrder), displayUtf8(utf8),
                           PREDICTED_UTF8_LEN, len, PREDICTED_UTF8_LEN == len);

        // Translate utf16W back to UTF-8, observe that the result matches

        utf8CodePoints = -1;
        std::string utf8D;
        utf8D.reserve(4092);
        rc = Util::utf16ToUtf8(&utf8D,
                               utf16U16.data(),
                               &utf8CodePoints,
                               0,
                               byteOrder);
        ASSERT(0 == rc);
        ASSERT(RANDOM_CODE_POINTS == utf8CodePoints);
        ASSERTV(displayUtf8(utf8), displayUtf8(utf8D), utf8 == utf8D);
        ASSERT(utf8D.length() + 1 == len);
#endif
    }
}

template <class STRING,
          class WSTRING,
          class VECTOR_C,
          class VECTOR_WC,
          class VECTOR_US>
void TestDriver::testCase13()
{
    bslma::TestAllocator ca("container");
    if (veryVerbose)
        cout << "\t\tOutput to wstring\n";
    {
        bslalg::ConstructorProxy<WSTRING>  dstStrProxy(&ca);
        WSTRING&                           dstStr = dstStrProxy.object();
        bslalg::ConstructorProxy<WSTRING>  dstStrBackProxy(&ca);
        WSTRING&                           dstStrBack =
                                                      dstStrBackProxy.object();

        int rc = Util::utf8ToUtf16(&dstStr, charUtf8MultiLang, 0, '?');
        ASSERT(0 == rc);

        rc = Util::utf8ToUtf16(&dstStrBack,
                               bsl::string_view(charUtf8MultiLang),
                               0,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);

        ASSERT(
            swappedRangeEquals(&dstStr[0],
                               &dstStr[0] + dstStr.length() + 1,
                               &dstStrBack[0],
                               &dstStrBack[0] + dstStrBack.length() + 1));

        dstStrBack.clear();
        rc = Util::utf8ToUtf16(
            &dstStrBack, charUtf8MultiLang, 0, '?', e_BACKWARDS);
        ASSERT(0 == rc);

        ASSERT(
            swappedRangeEquals(&dstStr[0],
                               &dstStr[0] + dstStr.length() + 1,
                               &dstStrBack[0],
                               &dstStrBack[0] + dstStrBack.length() + 1));

        bslalg::ConstructorProxy<STRING> rStrProxy(&ca);
        STRING&                          rStr = rStrProxy.object();

        rc = Util::utf16ToUtf8(&rStr, dstStrBack.c_str(), 0, '?', e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(rStr == charUtf8MultiLang);

        typedef typename WSTRING::value_type wchar_type;
        typedef bsl::basic_string_view<wchar_type> WideStringView;

        rStr.clear();
        rc = Util::utf16ToUtf8(&rStr,
                               WideStringView(dstStrBack),
                               0,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(rStr == charUtf8MultiLang);

        bslalg::ConstructorProxy<VECTOR_C> vectorCProxy(&ca);
        VECTOR_C&                          vectorC = vectorCProxy.object();

        rc = Util::utf16ToUtf8(
                    &vectorC, WideStringView(dstStrBack), 0, '?', e_BACKWARDS);
        ASSERT(0 == rc);

        ASSERT(vectorC.size() == bsl::strlen(charUtf8MultiLang) + 1);
        ASSERT(!bsl::strcmp(&vectorC[0], charUtf8MultiLang));
    }

    if (veryVerbose)
        cout << "\t\tOutput to vector<unsigned short>\n";
    {
        bslalg::ConstructorProxy<VECTOR_US> dstVecProxy(&ca);
        VECTOR_US&                          dstVec = dstVecProxy.object();
        bslalg::ConstructorProxy<VECTOR_US> dstVecBackProxy(&ca);
        VECTOR_US&                          dstVecBack =
                                                      dstVecBackProxy.object();

        int rc = Util::utf8ToUtf16(&dstVec, charUtf8MultiLang, 0, '?');
        ASSERT(0 == rc);

        rc = Util::utf8ToUtf16(&dstVecBack,
                               bsl::string_view(charUtf8MultiLang),
                               0,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);

        typename VECTOR_US::value_type *dstVecData     = &dstVec[0];
        typename VECTOR_US::value_type *dstVecBackData = &dstVecBack[0];
        ASSERT(swappedRangeEquals(dstVecData,
                                  dstVecData + dstVec.size(),
                                  dstVecBackData,
                                  dstVecBackData + dstVecBack.size()));

        dstVecBack.clear();
        rc = Util::utf8ToUtf16(
            &dstVecBack, charUtf8MultiLang, 0, '?', e_BACKWARDS);
        ASSERT(0 == rc);

        ASSERT(swappedRangeEquals(dstVecData,
                                  dstVecData + dstVec.size(),
                                  dstVecBackData,
                                  dstVecBackData + dstVecBack.size()));
        bslalg::ConstructorProxy<STRING> rStrProxy(&ca);
        STRING&                          rStr = rStrProxy.object();
        rc = Util::utf16ToUtf8(&rStr, &dstVecBack[0], 0, '?', e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(rStr == charUtf8MultiLang);
    }

    if (veryVerbose)
        cout << "\t\tOutput to unsigned short memory segment\n";
    {
        bslalg::ConstructorProxy<VECTOR_US> dstVecUSProxy(&ca);
        VECTOR_US&                          dstVecUS = dstVecUSProxy.object();
        bslalg::ConstructorProxy<VECTOR_C>  dstVecCProxy(&ca);
        VECTOR_C&                           dstVecC = dstVecCProxy.object();

        enum { SEG_SIZE = 1 << 20 };

        ASSERT(SEG_SIZE > sizeof(utf8MultiLang));

        dstVecUS.resize(SEG_SIZE);
        unsigned short *dstVecUSData = &dstVecUS[0];

        bsl::size_t nc = -1, nw = -1;
        int         rc = Util::utf8ToUtf16(
                    dstVecUSData, SEG_SIZE, charUtf8MultiLang, &nc, &nw);
        ASSERT(0 == rc);
        ASSERT(0 == dstVecUS[nw - 1]);
        ASSERT(nw > nc);
        ASSERT(bsl::find(dstVecUSData, dstVecUSData + SEG_SIZE, 0) ==
               dstVecUSData + (nw - 1));

        bslalg::ConstructorProxy<VECTOR_US> dstVecUSProxy2(&ca);
        VECTOR_US&                          dstVecUS2 =
                                                       dstVecUSProxy2.object();

        dstVecUS2.resize(SEG_SIZE);
        unsigned short *dstVecUSData2 = &dstVecUS2[0];

        bsl::size_t ncb = -1, nwb = -1;
        rc = Util::utf8ToUtf16(dstVecUSData2,
                               SEG_SIZE,
                               charUtf8MultiLang,
                               &ncb,
                               &nwb,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(nwb == nw);
        ASSERT(ncb == nc);
        ASSERT(0 == dstVecUS[nwb - 1]);
        ASSERT(bsl::find(dstVecUSData2, dstVecUSData2 + SEG_SIZE, 0) ==
               dstVecUSData2 + (nwb - 1));

        ASSERT(swappedRangeEquals(dstVecUSData,
                                  dstVecUSData + nw,
                                  dstVecUSData2,
                                  dstVecUSData2 + nwb));

        dstVecC.resize(SEG_SIZE);
        char *dstVecCData = &dstVecC[0];

        ncb = -1, nwb = -1;
        rc = Util::utf16ToUtf8(dstVecCData,
                               SEG_SIZE,
                               dstVecUSData2,
                               &ncb,
                               &nwb,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(ncb == nc);
        ASSERT(nwb > ncb);
        ASSERT(nwb > nw);
        ASSERT(nwb - 1 == bsl::strlen(dstVecCData));

        ASSERT(0 == bsl::strcmp(dstVecCData, charUtf8MultiLang));
    }

    if (veryVerbose)
        cout << "\t\tOutput to wchar_t memory segment\n";
    {
        bslalg::ConstructorProxy<VECTOR_WC> dstVecWCProxy(&ca);
        VECTOR_WC&                          dstVecWC = dstVecWCProxy.object();
        bslalg::ConstructorProxy<VECTOR_C>  dstVecCProxy(&ca);
        VECTOR_C&                           dstVecC = dstVecCProxy.object();

        enum { SEG_SIZE = 1 << 20 };

        ASSERT(SEG_SIZE > sizeof(utf8MultiLang));

        dstVecWC.resize(SEG_SIZE);
        wchar_t *dstVecWCData = &dstVecWC[0];

        bsl::size_t nc = -1, nw = -1;
        int         rc = Util::utf8ToUtf16(
                    dstVecWCData, SEG_SIZE, charUtf8MultiLang, &nc, &nw);
        ASSERT(0 == rc);
        ASSERT(0 == dstVecWC[nw - 1]);
        ASSERT(nw > nc);
        ASSERT(bsl::find(dstVecWCData, dstVecWCData + SEG_SIZE, 0) ==
               dstVecWCData + (nw - 1));

        bslalg::ConstructorProxy<VECTOR_WC> dstVecWCProxy2(&ca);
        VECTOR_WC&                          dstVecWC2 =
                                                       dstVecWCProxy2.object();

        dstVecWC2.resize(SEG_SIZE);
        wchar_t *dstVecWCData2 = &dstVecWC2[0];

        bsl::size_t ncb = -1, nwb = -1;
        rc = Util::utf8ToUtf16(dstVecWCData2,
                               SEG_SIZE,
                               charUtf8MultiLang,
                               &ncb,
                               &nwb,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(nwb == nw);
        ASSERT(ncb == nc);
        ASSERT(0 == dstVecWC[nwb - 1]);
        ASSERT(bsl::find(dstVecWCData2, dstVecWCData2 + SEG_SIZE, 0) ==
               dstVecWCData2 + (nwb - 1));

        ASSERT(swappedRangeEquals(dstVecWCData,
                                  dstVecWCData + nw,
                                  dstVecWCData2,
                                  dstVecWCData2 + nwb));

        dstVecC.resize(SEG_SIZE);
        char *dstVecCData = &dstVecC[0];

        ncb = -1, nwb = -1;
        rc = Util::utf16ToUtf8(dstVecCData,
                               SEG_SIZE,
                               dstVecWCData2,
                               &ncb,
                               &nwb,
                               '?',
                               e_BACKWARDS);
        ASSERT(0 == rc);
        ASSERT(ncb == nc);
        ASSERT(nwb > ncb);
        ASSERT(nwb > nw);
        ASSERT(nwb - 1 == bsl::strlen(dstVecCData));

        ASSERT(0 == bsl::strcmp(dstVecCData, charUtf8MultiLang));
    }
}

template <class STRING, class WSTRING>
void TestDriver::testCase12()
{
    typedef bdlde::CharConvertUtf16 Obj;

    enum {
        k_ASCII   = 0,
        k_GREEK   = 1,
        k_CHINESE = 2,
        k_HINDI   = 3,
        k_QUAD    = 4,
        k_ZERO    = 5
    };
    enum { k_NUM_SNIPPETS = k_ZERO + 1 };

    bsl::vector<bsl::string> utf8s(&ta);
    utf8s.resize(k_NUM_SNIPPETS);
    utf8s[k_ASCII]   = "Hello";
    utf8s[k_GREEK]   = "\xce\x97\xce\x95\xce\xbb";
    utf8s[k_CHINESE] = "\xe4\xb8\xad\xe5\x8d\x8e";
    utf8s[k_HINDI]   = "\xe0\xa4\xad\xe0\xa4\xbe";
    utf8s[k_QUAD]    = "\xf2\x94\xb4\xa5\xf3\xb8\xac\x83";
    utf8s[k_ZERO].push_back(0);

    bsl::vector<WSTRING> utf16s(&ta);
    utf16s.resize(k_NUM_SNIPPETS);
    ASSERT(0 == Obj::utf8ToUtf16(&utf16s[k_ASCII], utf8s[k_ASCII]));
    ASSERT(0 == Obj::utf8ToUtf16(&utf16s[k_GREEK], utf8s[k_GREEK]));
    ASSERT(0 == Obj::utf8ToUtf16(&utf16s[k_CHINESE], utf8s[k_CHINESE]));
    ASSERT(0 == Obj::utf8ToUtf16(&utf16s[k_HINDI], utf8s[k_HINDI]));
    ASSERT(0 == Obj::utf8ToUtf16(&utf16s[k_QUAD], utf8s[k_QUAD]));
    ASSERT(0 == Obj::utf8ToUtf16(&utf16s[k_ZERO], utf8s[k_ZERO]));

    const bsl::size_t numCodePoints[] = {5, 3, 2, 2, 2, 1};

    for (int ti = 0; ti < k_NUM_SNIPPETS; ++ti) {
        ASSERT(!utf8s[ti].empty());
        ASSERT(utf8s[ti].length() >= numCodePoints[ti]);
        ASSERT(!utf16s[ti].empty());
        if (k_QUAD != ti) {
            ASSERT(utf16s[ti].length() == numCodePoints[ti]);
        }
        else {
            ASSERT(utf16s[ti].length() == 2 * numCodePoints[ti]);
        }
    }

    bslma::TestAllocator ca("container");

    for (int ti = 0; ti < k_NUM_SNIPPETS; ++ti) {
        for (int tj = 0; tj < k_NUM_SNIPPETS; ++tj) {
            for (int tk = 0; tk < k_NUM_SNIPPETS; ++tk) {
                for (int tm = 0; tm < k_NUM_SNIPPETS; ++tm) {
                    for (int tn = 0; tn < k_NUM_SNIPPETS; ++tn) {
                        const bool zeroThere = k_ZERO == ti || k_ZERO == tj ||
                                               k_ZERO == tk || k_ZERO == tm ||
                                               k_ZERO == tn;

                        if (!zeroThere)
                            continue;

                        if (veryVerbose) {
                            P_(ti) P_(tj) P_(tk) P_(tm) P(tn);
                        }

                        const bsl::size_t NUM_CODE_POINTS =
                            1 + numCodePoints[ti] + numCodePoints[tj] +
                            numCodePoints[tk] + numCodePoints[tm] +
                            numCodePoints[tn];

                        bsl::string utf8In(&ta);
                        utf8In += utf8s[ti];
                        utf8In += utf8s[tj];
                        utf8In += utf8s[tk];
                        utf8In += utf8s[tm];
                        utf8In += utf8s[tn];
                        ASSERT(utf8In.length() ==
                               utf8s[ti].length() + utf8s[tj].length() +
                                   utf8s[tk].length() + utf8s[tm].length() +
                                   utf8s[tn].length());
                        ASSERT(zeroThere == (bsl::strlen(utf8In.c_str()) <
                                             utf8In.length()));

                        bslalg::ConstructorProxy<WSTRING> expectedUtf16Proxy(
                                                                          &ca);
                        WSTRING&                          expectedUtf16 =
                                                   expectedUtf16Proxy.object();

                        expectedUtf16 += utf16s[ti];
                        expectedUtf16 += utf16s[tj];
                        expectedUtf16 += utf16s[tk];
                        expectedUtf16 += utf16s[tm];
                        expectedUtf16 += utf16s[tn];
                        ASSERT(expectedUtf16.length() ==
                               utf16s[ti].length() + utf16s[tj].length() +
                                   utf16s[tk].length() + utf16s[tm].length() +
                                   utf16s[tn].length());

                        bslalg::ConstructorProxy<WSTRING> utf16Proxy(&ca);
                        WSTRING&                          utf16 =
                                                           utf16Proxy.object();

                        bsl::size_t nc = -1;
                        ASSERT(0 == Obj::utf8ToUtf16(&utf16, utf8In, &nc));
                        ASSERT(expectedUtf16 == utf16);
                        ASSERTV(nc,
                                NUM_CODE_POINTS,
                                ti,
                                tj,
                                tk,
                                tn,
                                NUM_CODE_POINTS == nc);

                        bslalg::ConstructorProxy<STRING> utf8OutProxy(&ca);
                        STRING&                          utf8Out =
                                                         utf8OutProxy.object();
                        nc = -1;
                        ASSERT(0 == Obj::utf16ToUtf8(&utf8Out, utf16, &nc));

                        // TODO: Replace the following two assertions with the
                        // single one: 'ASSERT(utf8In == utf8Out);' as soon as
                        // 'bsl::string::operator==' overload for
                        // 'std::pmr:string' is implemented.

                        ASSERT(utf8In.length() == utf8Out.length())
                        if (utf8In.length() == utf8Out.length()) {
                            for (bsl::size_t i = 0; i < utf8In.length(); ++i) {
                                ASSERT(utf8In[i] == utf8Out[i]);
                            }
                        }

                        ASSERTV(nc,
                                NUM_CODE_POINTS,
                                ti,
                                tj,
                                tk,
                                tn,
                                NUM_CODE_POINTS == nc);
                    }
                }
            }
        }
    }
}

template <class VECTOR, class STRING, class WIDE_CHAR>
void TestDriver::testCase11()
{
    typedef bsl::basic_string<WIDE_CHAR>      WString;
    typedef bsl::basic_string_view<WIDE_CHAR> WStringView;

    WIDE_CHAR DATA[][3] = {
        {'H', 0, 0},          // -- Valid ASCII
        {0x397, 0, 0},        // -- Valid Double
        {0x4e2d, 0, 0},       // -- Valid Triple
        {0xda13, 0xdd25, 0},  // -- Valid Quad

        // Sequences above here are definitely OK.  Sequences following
        // might be bad, might be OK, depending what's around them.

        {0xdd25, 0, 0},  // unexpected cont
        {0xda13, 0, 0},  // incomplete quad

        // Sequences after this will always cause errors, regardless of
        // what precedes or follows them.

        {'a', 0xdd25, 0},   // unexpected cont
        {0xda13, 'b', 0}};  // incomplete quad

    enum { NUM_DATA = sizeof DATA / (3 * sizeof(wchar_t)) };

    bslma::TestAllocator             ca("container");
    bslalg::ConstructorProxy<VECTOR> dstVecProxy(&ca);
    VECTOR&                          dstVec = dstVecProxy.object();
    bslalg::ConstructorProxy<STRING> dstStrProxy(&ca);
    STRING&                          dstStr = dstStrProxy.object();
    bsl::vector<unsigned short>      srcVec(&ta);

    bsl::size_t nc, nc2, nw, nw2;

    if (veryVerbose)
        cout << "Unswapped broken glass test\n";

    for (int ii = 0; ii < NUM_DATA; ++ii) {
        const WIDE_CHAR *IISTRING = &DATA[ii][0];
        for (int jj = 0; jj < NUM_DATA; ++jj) {
            const WIDE_CHAR *JJSTRING = &DATA[jj][0];
            for (int kk = 0; kk < NUM_DATA; ++kk) {
                const WIDE_CHAR *KKSTRING = &DATA[kk][0];

                WString WSTRING(&ta);
                WSTRING += IISTRING;
                WSTRING += JJSTRING;
                WSTRING += KKSTRING;
                WSTRING.push_back(0);

                WIDE_CHAR *START = WSTRING.begin();
                WIDE_CHAR *END   = START + WSTRING.length() - 1;
                for (WIDE_CHAR *start = START; start < END; ++start) {
                    for (WIDE_CHAR *end = start; end <= END; ++end) {
                        for (int e = 0; e < 2; ++e) {
                            const char errorByte = 0 == e ? '?' : 0;
                            nc                   = (bsl::size_t)-1;
                            int rc = Util::utf16ToUtf8(
                                &dstVec,
                                WStringView(start, end - start),
                                &nc,
                                errorByte);
                            ASSERT(0 == (rc & bdlde::CharConvertStatus::
                                                  k_OUT_OF_SPACE_BIT));
                            ASSERT(start > START || end < END ||
                                   (ii < 6 && jj < 6 && kk < 6) ||
                                   bdlde::CharConvertStatus::
                                           k_INVALID_INPUT_BIT == rc);
                            ASSERT(start > START || end < END || ii >= 4 ||
                                   jj >= 4 || kk >= 4 || 0 == rc);
                            ASSERT(ii >= 3 || jj >= 3 || kk >= 3 || 0 == rc);
                            ASSERT(nc <= dstVec.size());
                            typename VECTOR::iterator found =
                                bsl::find(dstVec.begin(), dstVec.end(), '?');
                            if (!dstVec.empty() && errorByte &&
                                (rc & bdlde::CharConvertStatus::
                                          k_INVALID_INPUT_BIT)) {
                                ASSERT(found < dstVec.end());
                            }
                            else {
                                ASSERT(found == dstVec.end());
                            }
                            ASSERT(nc <= dstVec.size());

                            nc2     = (bsl::size_t)-1;
                            int rc2 = Util::utf16ToUtf8(
                                &dstStr,
                                WStringView(start, end - start),
                                &nc2,
                                errorByte);
                            ASSERT(rc2 == rc);
                            ASSERT(dstVec.size() == dstStr.length() + 1);

                            char *dstVecData = &dstVec[0];
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            bsl::size_t dstCap = dstVec.size();

                            WIDE_CHAR save = *end;
                            *end           = 0;

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf16ToUtf8(
                                &dstStr, start, &nc2, errorByte);
                            ASSERT(rc2 == rc);
                            ASSERT(dstVec.size() == dstStr.length() + 1);
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            *end = save;

                            srcVec.clear();
                            for (const WIDE_CHAR *pw = start; pw < end; ++pw) {
                                srcVec.push_back((unsigned short)*pw);
                            }
                            srcVec.push_back(0);

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf16ToUtf8(
                                &dstVec, srcVec.begin(), &nc2, errorByte);
                            ASSERT(rc2 == rc);
                            LOOP2_ASSERT(dstVec.size(),
                                         dstCap,
                                         dstVec.size() == dstCap);
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf16ToUtf8(
                                &dstStr, srcVec.begin(), &nc2, errorByte);
                            ASSERT(rc2 == rc);
                            ASSERT(dstStr.length() + 1 == dstCap);
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            dstVec.push_back(0);
                            dstVec.push_back(0);

                            dstVecData = &dstVec[0];
                            for (bsl::size_t cap = 0; cap <= dstCap + 1;
                                 ++cap) {
                                nc = nw = (bsl::size_t)-1;
                                bsl::memset(
                                    dstVecData, (char)-1, dstVec.size());
                                rc2 = Util::utf16ToUtf8(
                                    dstVecData,
                                    cap,
                                    WStringView(start, end - start),
                                    &nc,
                                    &nw,
                                    errorByte);
                                ASSERT((cap < dstCap) ==
                                       !!(rc2 & bdlde::CharConvertStatus::
                                                    k_OUT_OF_SPACE_BIT));
                                ASSERT(cap < dstCap || rc2 == rc);
                                ASSERT(nc <= nw);
                                ASSERT(nw <= cap);
                                ASSERT(nw <= dstCap);
                                ASSERT((char)-1 == dstVec[nw]);
                                ASSERT(!cap ||
                                       (nw && nc && 0 == dstVec[nw - 1]));
                                found = bsl::find(dstVec.begin(),
                                                  dstVec.begin() + nw,
                                                  '?');
                                if (0 != nw && errorByte &&
                                    (rc2 & bdlde::CharConvertStatus::
                                               k_INVALID_INPUT_BIT)) {
                                    ASSERT(found < dstVec.begin() + nw);
                                }
                                else {
                                    ASSERT(found == dstVec.begin() + nw);
                                }

                                nc2 = nw2 = (bsl::size_t)-1;
                                bsl::memset(
                                    dstVecData, (char)-1, dstVec.size());
                                rc2 = Util::utf16ToUtf8(dstVecData,
                                                        cap,
                                                        srcVec.begin(),
                                                        &nc2,
                                                        &nw2,
                                                        errorByte);
                                ASSERT((cap < dstCap) ==
                                       !!(rc2 & bdlde::CharConvertStatus::
                                                    k_OUT_OF_SPACE_BIT));
                                ASSERT(cap < dstCap || rc2 == rc);
                                ASSERT(nc2 == nc);
                                ASSERT(nw2 == nw);
                                found = bsl::find(dstVec.begin(),
                                                  dstVec.begin() + nw2,
                                                  '?');
                                if (0 != nw2 && errorByte &&
                                    (rc2 & bdlde::CharConvertStatus::
                                               k_INVALID_INPUT_BIT)) {
                                    ASSERT(found < dstVec.begin() + nw2);
                                }
                                else {
                                    ASSERT(found == dstVec.begin() + nw2);
                                }
                            }

                            // Make sure we didn't accidentally embed any holes
                            // in our input.

                            {
                                const WIDE_CHAR *ps = START;
                                while (*ps) {
                                    ++ps;
                                }
                                ASSERT(END == ps);
                            }
                            {
                                const unsigned short *ps = srcVec.begin();
                                while (*ps) {
                                    ++ps;
                                }
                                ASSERT(ps == &srcVec.back());
                            }
                        }
                    }
                }
            }
        }
    }

    if (veryVerbose)
        cout << "Swapped broken glass test\n";

    for (int si = 0; si < NUM_DATA; ++si) {
        for (int sii = 0; sii < 3; ++sii) {
            swapInPlace(&DATA[si][sii]);
        }
    }

    for (int ii = 0; ii < NUM_DATA; ++ii) {
        const WIDE_CHAR *IISTRING = &DATA[ii][0];
        for (int jj = 0; jj < NUM_DATA; ++jj) {
            const WIDE_CHAR *JJSTRING = &DATA[jj][0];
            for (int kk = 0; kk < NUM_DATA; ++kk) {
                const WIDE_CHAR *KKSTRING = &DATA[kk][0];

                WString WSTRING(&ta);
                WSTRING += IISTRING;
                WSTRING += JJSTRING;
                WSTRING += KKSTRING;
                WSTRING.push_back(0);

                WIDE_CHAR *START = WSTRING.begin();
                WIDE_CHAR *END   = START + WSTRING.length() - 1;
                for (WIDE_CHAR *start = START; start < END; ++start) {
                    for (WIDE_CHAR *end = start; end <= END; ++end) {
                        for (int e = 0; e < 2; ++e) {
                            const char errorByte = 0 == e ? '?' : 0;

                            if (veryVeryVeryVeryVerbose) {
                                P_(ii)
                                P_(jj) P_(kk) P_(start - START) P_(END - end)
                                    P(end - start);
                            }

                            nc     = (bsl::size_t)-1;
                            int rc = Util::utf16ToUtf8(
                                &dstVec,
                                WStringView(start, end - start),
                                &nc,
                                errorByte,
                                e_BACKWARDS);
                            ASSERT(0 == (rc & bdlde::CharConvertStatus::
                                                  k_OUT_OF_SPACE_BIT));
                            ASSERT(start > START || end < END ||
                                   (ii < 6 && jj < 6 && kk < 6) ||
                                   bdlde::CharConvertStatus::
                                           k_INVALID_INPUT_BIT == rc);
                            ASSERT(start > START || end < END || ii >= 4 ||
                                   jj >= 4 || kk >= 4 || 0 == rc);
                            ASSERT(ii >= 3 || jj >= 3 || kk >= 3 || 0 == rc);
                            ASSERT(nc <= dstVec.size());
                            typename VECTOR::iterator found =
                                bsl::find(dstVec.begin(), dstVec.end(), '?');
                            if (!dstVec.empty() && errorByte &&
                                (rc & bdlde::CharConvertStatus::
                                          k_INVALID_INPUT_BIT)) {
                                ASSERT(found < dstVec.end());
                            }
                            else {
                                ASSERT(found == dstVec.end());
                            }
                            ASSERT(nc <= dstVec.size());

                            nc2     = (bsl::size_t)-1;
                            int rc2 = Util::utf16ToUtf8(
                                &dstStr,
                                WStringView(start, end - start),
                                &nc2,
                                errorByte,
                                e_BACKWARDS);
                            ASSERT(rc2 == rc);
                            ASSERT(dstVec.size() == dstStr.length() + 1);

                            char *dstVecData = &dstVec[0];
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            bsl::size_t dstCap = dstVec.size();

                            WIDE_CHAR save = *end;
                            *end           = 0;

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf16ToUtf8(
                                &dstStr, start, &nc2, errorByte, e_BACKWARDS);
                            ASSERT(rc2 == rc);
                            ASSERT(dstVec.size() == dstStr.length() + 1);
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            *end = save;

                            srcVec.clear();
                            enum { SHIFT_DOWN = 8 * (sizeof(WIDE_CHAR) - 2) };
                            for (const WIDE_CHAR *pw = start; pw < end; ++pw) {
                                srcVec.push_back(
                                    (unsigned short)(*pw >> SHIFT_DOWN));
                            }
                            srcVec.push_back(0);

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf16ToUtf8(&dstVec,
                                                    srcVec.begin(),
                                                    &nc2,
                                                    errorByte,
                                                    e_BACKWARDS);
                            ASSERT(rc2 == rc);
                            LOOP5_ASSERT(ii,
                                         jj,
                                         kk,
                                         dstVec.size(),
                                         dstCap,
                                         dstVec.size() == dstCap);
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf16ToUtf8(&dstStr,
                                                    srcVec.begin(),
                                                    &nc2,
                                                    errorByte,
                                                    e_BACKWARDS);
                            ASSERT(rc2 == rc);
                            ASSERT(dstStr.length() + 1 == dstCap);
                            ASSERT(0 == bsl::memcmp(dstVecData,
                                                    dstStr.c_str(),
                                                    dstVec.size()));
                            LOOP2_ASSERT(nc2, nc, nc2 == nc);

                            dstVec.push_back(0);
                            dstVec.push_back(0);

                            dstVecData = &dstVec[0];
                            for (bsl::size_t cap = 0; cap <= dstCap + 1;
                                 ++cap) {
                                nc = nw = (bsl::size_t)-1;
                                bsl::memset(
                                    dstVecData, (char)-1, dstVec.size());
                                rc2 = Util::utf16ToUtf8(
                                    dstVecData,
                                    cap,
                                    WStringView(start, end - start),
                                    &nc,
                                    &nw,
                                    errorByte,
                                    e_BACKWARDS);
                                ASSERT((cap < dstCap) ==
                                       !!(rc2 & bdlde::CharConvertStatus::
                                                    k_OUT_OF_SPACE_BIT));
                                ASSERT(cap < dstCap || rc2 == rc);
                                ASSERT(nc <= nw);
                                ASSERT(nw <= cap);
                                ASSERT(nw <= dstCap);
                                ASSERT((char)-1 == dstVec[nw]);
                                ASSERT(!cap ||
                                       (nw && nc && 0 == dstVec[nw - 1]));
                                found = bsl::find(dstVec.begin(),
                                                  dstVec.begin() + nw,
                                                  '?');
                                if (0 != nw && errorByte &&
                                    (rc2 & bdlde::CharConvertStatus::
                                               k_INVALID_INPUT_BIT)) {
                                    ASSERT(found < dstVec.begin() + nw);
                                }
                                else {
                                    ASSERT(found == dstVec.begin() + nw);
                                }

                                nc2 = nw2 = (bsl::size_t)-1;
                                bsl::memset(
                                    dstVecData, (char)-1, dstVec.size());
                                rc2 = Util::utf16ToUtf8(dstVecData,
                                                        cap,
                                                        srcVec.begin(),
                                                        &nc2,
                                                        &nw2,
                                                        errorByte,
                                                        e_BACKWARDS);
                                ASSERT((cap < dstCap) ==
                                       !!(rc2 & bdlde::CharConvertStatus::
                                                    k_OUT_OF_SPACE_BIT));
                                ASSERT(cap < dstCap || rc2 == rc);
                                ASSERT(nc2 == nc);
                                ASSERT(nw2 == nw);
                                found = bsl::find(dstVec.begin(),
                                                  dstVec.begin() + nw2,
                                                  '?');
                                if (0 != nw2 && errorByte &&
                                    (rc2 & bdlde::CharConvertStatus::
                                               k_INVALID_INPUT_BIT)) {
                                    ASSERT(found < dstVec.begin() + nw2);
                                }
                                else {
                                    ASSERT(found == dstVec.begin() + nw2);
                                }
                            }

                            // Make sure we didn't accidentally embed any holes
                            // in our input.

                            {
                                const WIDE_CHAR *ps = START;
                                while (*ps) {
                                    ++ps;
                                }
                                ASSERT(END == ps);
                            }
                            {
                                const unsigned short *ps = srcVec.begin();
                                while (*ps) {
                                    ++ps;
                                }
                                ASSERT(ps == &srcVec.back());
                            }
                        }
                    }
                }
            }
        }
    }
}

template <class VECTOR,
          class WSTRING>
void TestDriver::testCase10()
{
    typedef typename WSTRING::value_type WIDE_CHAR;

    struct {
        int         d_line;
        const char *d_string;
    } DATA[] = {
        {L_, "H"},                 // -- Valid ASCII
        {L_, "\xce\x97"},          // -- Valid Greek
        {L_, "\xe4\xb8\xad"},      // -- Valid Chinese
        {L_, "\xf2\x94\xb4\xa5"},  // -- Valid Quad

        // All sequences before this are valid, all sequences after this are
        // invalid.

        {L_, "\xed\xa0\x85"},  // illegal UTF-16 0xd805
        {L_, "\xed\xb6\xa3"},  // illegal UTF-16 0xdda3

        {L_, "\x83\x83\x83\x83\x83\x83"},      // unexpected cont
        {L_, "\xf9\x83\x83\x83\x83"},          // 5 byte (invalid)
        {L_, "\xfd\x83\x83\x83\x83\x83"},      // 6 byte (invalid)
        {L_, "\xfe\xa3\x83\x83\x83\x83\x83"},  // 7 byte (invalid)
        {L_, "\xce"},                          // incomplete Greek
        {L_, "\xe8\xb8"},                      // incomplete Chinese
        {L_, "\xe8"},                          // incomplete Chinese
        {L_, "\xf2\x94\xb4"},                  // incomplete quad
        {L_, "\xf2\x94"},                      // incomplete quad
        {L_, "\xf2"},                          // incomplete quad
        {L_, "\xc0\x8f"},                      // non-minimal 2
        {L_, "\xe0\x80\x8f"},                  // non-minimal 3
        {L_, "\xf0\x80\x80\x8f"}};             // non-minimal 4

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    bslma::TestAllocator              ca("container");
    bslalg::ConstructorProxy<VECTOR>  dstVecProxy(&ca);
    VECTOR&                           dstVec = dstVecProxy.object();
    bslalg::ConstructorProxy<VECTOR>  dstVecProxyB(&ca);
    VECTOR&                           dstVecB = dstVecProxyB.object();
    bslalg::ConstructorProxy<WSTRING> dstStrProxy(&ca);
    WSTRING&                          wDstStr = dstStrProxy.object();
    bslalg::ConstructorProxy<WSTRING> dstStrProxyB(&ca);
    WSTRING&                          wDstStrB = dstStrProxyB.object();

    for (int i = 0; i < NUM_DATA; ++i) {
        const char *ISTRING = DATA[i].d_string;

        for (int j = 0; j < 2; ++j) {
            const unsigned short errorWord = 0 == j ? '?' : 0;

            for (int k = 0; k < NUM_DATA; k += 2) {
                const char *KSTRING = DATA[k].d_string;

                for (int m = 1; m < NUM_DATA; m += 2) {
                    const char *MSTRING = DATA[m].d_string;

                    bsl::string INPUT(&ta);
                    INPUT += ISTRING;
                    INPUT += KSTRING;
                    INPUT += MSTRING;
                    INPUT += 'a';

                    char *END = INPUT.begin() + INPUT.length() - 1;
                    for (char *start = INPUT.begin(); start < END; ++start) {
                        bsl::size_t max16Len = 0;
                        for (char *end = END; end >= start; --end) {
                            bsl::size_t nc, nc2, nw, nw2;

                            int rc = Util::utf8ToUtf16(
                                &dstVec,
                                bsl::string_view(start, end - start),
                                &nc,
                                errorWord);
                            LOOP4_ASSERT(rc,
                                         i,
                                         k,
                                         m,
                                         INPUT.c_str() < start || END > end ||
                                             (i < 4 && k < 4 && m < 4) ||
                                             bdlde::CharConvertStatus::
                                                     k_INVALID_INPUT_BIT ==
                                                 rc);
                            ASSERT(0 == (rc & bdlde::CharConvertStatus::
                                                  k_OUT_OF_SPACE_BIT));
                            ASSERT(nc <= dstVec.size());

                            max16Len = bsl::max(max16Len, dstVec.size());

                            typename VECTOR::iterator found =
                                bsl::find(dstVec.begin(),
                                          dstVec.end(),
                                          (unsigned short)'?');
                            ASSERT(errorWord && rc ? found < dstVec.end()
                                                   : found == dstVec.end());

                            char save = *end;
                            *end      = 0;

                            int rc2 = Util::utf8ToUtf16(
                                &dstVecB, start, &nc2, errorWord);
                            ASSERT(rc2 == rc);
                            ASSERT(dstVecB == dstVec);
                            ASSERT(nc2 == nc);

                            *end = save;

                            found = bsl::find(dstVecB.begin(),
                                              dstVecB.end(),
                                              (unsigned short)'?');
                            ASSERT(errorWord && rc ? found < dstVecB.end()
                                                   : found == dstVecB.end());

                            rc2 = Util::utf8ToUtf16(
                                &wDstStr,
                                bsl::string_view(start, end - start),
                                &nc2,
                                (WIDE_CHAR)errorWord);
                            ASSERT(rc2 == rc);
                            ASSERT(nc2 == nc);

                            ASSERT(dstVec.size() == wDstStr.length() + 1);
                            ASSERT(0 == dstVec.back());
                            for (bsl::size_t ii = 0; ii < wDstStr.length();
                                 ++ii) {
                                ASSERT(wDstStr[ii] == (WIDE_CHAR)dstVec[ii]);
                            }

                            save = *end;
                            *end = 0;

                            rc2 = Util::utf8ToUtf16(
                                &wDstStrB, start, &nc2, (WIDE_CHAR)errorWord);
                            ASSERT(rc2 == rc);
                            ASSERT(nc2 == nc);
                            ASSERT(wDstStrB == wDstStr);

                            // Don't test 'rc2', the implanted '\0' might have
                            // inflicted invalid code points.

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf8ToUtf16(
                                &dstVecB,
                                bsl::string_view(start, END - start),
                                &nc2,
                                errorWord);
                            ASSERT(END == end ? nc2 == nc : nc2 > nc);
                            ASSERT((END == end) == (dstVecB == dstVec));

                            nc2 = (bsl::size_t)-1;
                            rc2 = Util::utf8ToUtf16(
                                &wDstStrB,
                                bsl::string_view(start, END - start),
                                &nc2,
                                errorWord);
                            ASSERT(END == end ? nc2 == nc : nc2 > nc);
                            ASSERT((END == end) == (wDstStrB == wDstStr));

                            *end = save;

                            bsl::size_t dstCap = dstVec.size();
                            dstVec.push_back(0);
                            dstVec.push_back(0);

                            wDstStr.push_back(0);
                            wDstStr.push_back(0);
                            wDstStr.push_back(0);

                            dstVecB.push_back(0);
                            dstVecB.push_back(0);

                            wDstStrB.push_back(0);
                            wDstStrB.push_back(0);
                            wDstStrB.push_back(0);

                            ASSERTV(wDstStr.length() == dstVec.size());

                            for (bsl::size_t cap = 1; cap <= dstCap; ++cap) {
                                bsl::fill(dstVec.begin(),
                                          dstVec.end(),
                                          (unsigned short)-1);

                                unsigned short *dstVecData = &dstVec[0];

                                nc = nw = (bsl::size_t)-1;
                                rc      = Util::utf8ToUtf16(
                                         dstVecData,
                                         cap,
                                         bsl::string_view(start, end - start),
                                         &nc,
                                         &nw,
                                         errorWord);
                                ASSERT(nc <= nw);
                                ASSERT(nw <= cap);
                                ASSERT(nw <= dstCap);
                                ASSERT(!cap || (nw && nc));
                                ASSERT(!nw || 0 == dstVec[nw - 1]);
                                ASSERT((unsigned short)-1 == dstVec[nw]);
                                ASSERT((cap < dstCap) ==
                                       !!(rc & bdlde::CharConvertStatus::
                                                   k_OUT_OF_SPACE_BIT));
                                LOOP4_ASSERT(rc,
                                             i,
                                             k,
                                             m,
                                             cap < dstCap ||
                                                 INPUT.c_str() < start ||
                                                 END > end ||
                                                 (i < 4 && k < 4 && m < 4) ||
                                                 (bdlde::CharConvertStatus::
                                                      k_INVALID_INPUT_BIT &
                                                  rc));
                                bsl::fill(dstVec.begin(),
                                          dstVec.end(),
                                          (unsigned short)-1);

                                save = *end;
                                *end = 0;

                                nc = nw = (bsl::size_t)-1;
                                rc      = Util::utf8ToUtf16(dstVecData,
                                                            cap,
                                                            start,
                                                            &nc,
                                                            &nw,
                                                            errorWord);
                                ASSERT(nc <= nw);
                                ASSERT(nw <= cap);
                                ASSERT(nw <= dstCap);
                                ASSERT(!cap || (nw && nc));
                                ASSERT(!nw || 0 == dstVec[nw - 1]);
                                ASSERT((unsigned short)-1 == dstVec[nw]);
                                ASSERT((cap < dstCap) ==
                                       !!(rc & bdlde::CharConvertStatus::
                                                   k_OUT_OF_SPACE_BIT));
                                LOOP4_ASSERT(rc,
                                             i,
                                             k,
                                             m,
                                             cap < dstCap ||
                                                 INPUT.c_str() < start ||
                                                 END > end ||
                                                 (i < 4 && k < 4 && m < 4) ||
                                                 (bdlde::CharConvertStatus::
                                                      k_INVALID_INPUT_BIT &
                                                  rc));

                                *end = save;

                                if (cap >= wDstStr.length()) {
                                    wDstStr.resize(cap + 1);
                                }
                                bsl::fill(wDstStr.begin(),
                                          wDstStr.end(),
                                          (WIDE_CHAR)-1);

                                WIDE_CHAR *wDstStrData = &wDstStr[0];
                                nc2 = nw2 = (bsl::size_t)-1;
                                rc2       = Util::utf8ToUtf16(
                                          wDstStrData,
                                          cap,
                                          bsl::string_view(start, end - start),
                                          &nc2,
                                          &nw2,
                                          errorWord);
                                ASSERT(rc2 == rc);
                                ASSERT(nc2 == nc);
                                ASSERT(nw2 == nw);
                                ASSERT((WIDE_CHAR)-1 == wDstStr[cap]);

                                for (bsl::size_t ii = 0; ii < nw; ++ii) {
                                    ASSERT((WIDE_CHAR)dstVec[ii] ==
                                                                  wDstStr[ii]);
                                }

                                save = *end;
                                *end = 0;

                                ASSERT(wDstStr.length() > cap);
                                bsl::fill(wDstStr.begin(),
                                          wDstStr.end(),
                                          (WIDE_CHAR)-1);

                                nc2 = nw2 = (bsl::size_t)-1;
                                rc2       = Util::utf8ToUtf16(wDstStrData,
                                                              cap,
                                                              start,
                                                              &nc2,
                                                              &nw2,
                                                              errorWord);
                                ASSERT(rc2 == rc);
                                ASSERT(nc2 == nc);
                                ASSERT(nw2 == nw);
                                ASSERT((WIDE_CHAR)-1 == wDstStr[cap]);

                                for (bsl::size_t ii = 0; ii < nw; ++ii) {
                                    ASSERT((WIDE_CHAR)dstVec[ii] ==
                                                                  wDstStr[ii]);
                                }

                                *end = save;
                            }

                            ASSERT(bsl::strlen(INPUT.c_str()) ==
                                   INPUT.length());
                        }
                    }
                }
            }
        }
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char**argv)
{
    test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;
    veryVeryVeryVeryVerbose = argc > 6;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE 2\n"
                             "===============\n";

        bslma::DefaultAllocatorGuard daGuard(&ta);

// The following snippets of code illustrate a typical use of the
// 'bdlde::CharConvertUtf16' struct's utility functions, first converting from
// UTF-8 to UTF-16, and then converting back to make sure the round trip
// returns the same value, translating to STL containers in both directions.
//
// First, we declare a string of UTF-8 containing single-, double-, triple-,
// and quadruple-octet code points:
//..
    const char utf8MultiLang[] = {
        "Hello"                                         // -- ASCII
        "\xce\x97"         "\xce\x95"       "\xce\xbb"  // -- Greek
        "\xe4\xb8\xad"     "\xe5\x8d\x8e"               // -- Chinese
        "\xe0\xa4\xad"     "\xe0\xa4\xbe"               // -- Hindi
        "\xf2\x94\xb4\xa5" "\xf3\xb8\xac\x83" };        // -- Quad octets
//..
// Then, we declare an 'enum' summarizing the counts of code points in the
// string and verify that the counts add up to the length of the string:
//..
    enum { NUM_ASCII_CODE_POINTS   = 5,
           NUM_GREEK_CODE_POINTS   = 3,
           NUM_CHINESE_CODE_POINTS = 2,
           NUM_HINDI_CODE_POINTS   = 2,
           NUM_QUAD_CODE_POINTS    = 2 };
//
    ASSERT(1 * NUM_ASCII_CODE_POINTS +
           2 * NUM_GREEK_CODE_POINTS +
           3 * NUM_CHINESE_CODE_POINTS +
           3 * NUM_HINDI_CODE_POINTS +
           4 * NUM_QUAD_CODE_POINTS == bsl::strlen(utf8MultiLang));
//..
// Next, we declare the vector where our UTF-16 output will go, and a variable
// into which the number of code points (not bytes or words) written will be
// stored.  It is not necessary to initialize 'utf16CodePointsWritten':
//..
    bsl::vector<unsigned short> v16;
    bsl::size_t utf16CodePointsWritten;
//..
// Note that for performance, we should 'v16.reserve(sizeof(utf8MultiLang))',
// but it's not strictly necessary -- the vector will automatically be grown to
// the correct size.  Also note that if 'v16' were not empty, that wouldn't be
// a problem -- any contents will be discarded.
//
// Then, we do the translation to UTF-16:
//..
    int retVal = bdlde::CharConvertUtf16::utf8ToUtf16(&v16,
                                                      utf8MultiLang,
                                                      &utf16CodePointsWritten);
//
    ASSERT(0 == retVal);        // verify success
    ASSERT(0 == v16.back());    // verify null terminated
//..
// Next, we verify that the number of code points (not bytes or words) that was
// returned is correct:
//..
    enum { EXPECTED_CODE_POINTS_WRITTEN =
                        NUM_ASCII_CODE_POINTS + NUM_GREEK_CODE_POINTS +
                        NUM_CHINESE_CODE_POINTS + NUM_HINDI_CODE_POINTS +
                        NUM_QUAD_CODE_POINTS  + 1 };
//
    ASSERT(EXPECTED_CODE_POINTS_WRITTEN == utf16CodePointsWritten);
//..
// Then, we verify that the number of 16-bit words written was correct.  The
// quad octet code points each require 2 'short' words of output:
//..
    enum { EXPECTED_UTF16_WORDS_WRITTEN =
                        NUM_ASCII_CODE_POINTS + NUM_GREEK_CODE_POINTS +
                        NUM_CHINESE_CODE_POINTS + NUM_HINDI_CODE_POINTS +
                        NUM_QUAD_CODE_POINTS * 2 + 1 };
//
    ASSERT(EXPECTED_UTF16_WORDS_WRITTEN == v16.size());
//..
// Next, we calculate and confirm the difference between the number of UTF-16
// words output and the number of bytes input.  The ASCII code points will take
// 1 16-bit word apiece, the Greek code points are double octets that will
// become single 'short' values, the Chinese code points are encoded as UTF-8
// triple octets that will turn into single 16-bit words, the same for the
// Hindi code points, and the quad code points are quadruple octets that will
// turn into double 'short' values:
//..
    enum { SHRINKAGE = NUM_ASCII_CODE_POINTS   * (1-1) +
                       NUM_GREEK_CODE_POINTS   * (2-1) +
                       NUM_CHINESE_CODE_POINTS * (3-1) +
                       NUM_HINDI_CODE_POINTS   * (3-1) +
                       NUM_QUAD_CODE_POINTS    * (4-2) };
//
    ASSERT(v16.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Then, we go on to do the reverse 'utf16ToUtf8' transform to turn it back
// into UTF-8, and we should get a result identical to our original input.  We
// declare a 'bsl::string' for our output, and a variable to count the number
// of code points (not bytes or words) translated:
//..
    bsl::string s;
    bsl::size_t uf8CodePointsWritten;
//..
// Again, note that for performance, we should ideally
// 's.reserve(3 * v16.size())' but it's not really necessary.
//
// Now, we do the reverse transform:
//..
    retVal = bdlde::CharConvertUtf16::utf16ToUtf8(&s,
                                                  v16.begin(),
                                                  &uf8CodePointsWritten);
//..
// Finally, we verify that a successful status was returned, that the output of
// the reverse transform was identical to the original input, and that the
// number of code points translated was as expected:
//..
    ASSERT(0 == retVal);
    ASSERT(utf8MultiLang == s);
    ASSERT(s.length() + 1               == sizeof(utf8MultiLang));
//
    ASSERT(EXPECTED_CODE_POINTS_WRITTEN == uf8CodePointsWritten);
    ASSERT(utf16CodePointsWritten       == uf8CodePointsWritten);
//..
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE 1\n"
                             "===============\n";

// In this example, we will translate a string containing a non-ASCII code
// point from UTF-16 to UTF-8 and back using fixed-length buffers.
//
// First, we create a UTF-16 string spelling 'ecole' in French, which begins
// with '0xc9', a non-ASCII 'e' with an accent over it:
//..
    unsigned short utf16String[] = { 0xc9, 'c', 'o', 'l', 'e', 0 };
//..
// Then, we create a byte buffer to store the UTF-8 result of the translation
// in, and variables to monitor counts of code points and bytes translated:
//..
    char utf8String[7];
    bsl::size_t numCodePoints, numBytes;
    numCodePoints = numBytes = -1;    // garbage
//..
// Next, we call 'utf16ToUtf8' to do the translation:
//..
    int rc = bdlde::CharConvertUtf16::utf16ToUtf8(utf8String,
                                                  sizeof(utf8String),
                                                  utf16String,
                                                  &numCodePoints,
                                                  &numBytes);
//..
// Then, we observe that no errors or warnings occurred, and that the numbers
// of code points and bytes were as expected.  Note that both 'numCodePoints'
// and 'numBytes' include the terminating 0:
//..
    ASSERT(0 == rc);
    ASSERT(6 == numCodePoints);
    ASSERT(7 == numBytes);
//..
// Next, we examine the length of the translated string:
//..
    ASSERT(numBytes - 1 == bsl::strlen(utf8String));
//..
// Then, we examine the individual bytes of the translated UTF-8:
//..
    ASSERT((char)0xc3 == utf8String[0]);
    ASSERT((char)0x89 == utf8String[1]);
    ASSERT('c' ==        utf8String[2]);
    ASSERT('o' ==        utf8String[3]);
    ASSERT('l' ==        utf8String[4]);
    ASSERT('e' ==        utf8String[5]);
    ASSERT(0   ==        utf8String[6]);
//..
// Next, in preparation for translation back to UTF-16, we create a buffer of
// 'short' values and the variable 'numWords' to track the number of UTF-16
// words occupied by the result:
//..
    unsigned short secondUtf16String[6];
    bsl::size_t numWords;
    numCodePoints = numWords = -1;    // garbage
//..
// Then, we do the reverse translation:
//..
    rc = bdlde::CharConvertUtf16::utf8ToUtf16(secondUtf16String,
                                              6,
                                              utf8String,
                                              &numCodePoints,
                                              &numWords);
//..
// Next, we observe that no errors or warnings were reported, and that the
// number of code points and words were as expected.  Note that 'numCodePoints'
// and 'numWords' both include the terminating 0:
//..
    ASSERT(0 == rc);
    ASSERT(6 == numCodePoints);
    ASSERT(6 == numWords);
//..
// Now, we observe that our output is identical to the original UTF-16 string:
//..
    ASSERT(0 == bsl::memcmp(utf16String,
                            secondUtf16String,
                            sizeof(utf16String)));
//..
// Finally, we examine the individual words of the reverse translation:
//..
    ASSERT(0xc9 == secondUtf16String[0]);
    ASSERT('c'  == secondUtf16String[1]);
    ASSERT('o'  == secondUtf16String[2]);
    ASSERT('l'  == secondUtf16String[3]);
    ASSERT('e'  == secondUtf16String[4]);
    ASSERT(0    == secondUtf16String[5]);
//..
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // UTF-8 LENGTH CALCULATION TEST -- INCORRECT UNICODE
        //
        // Documentation at start of 'void testCase16'.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "UTF-8 LENGTH CALCULATION TEST -- INCORRECT UNICODE\n"
                        "==================================================\n";

        TestDriver::testCase16(bdlde::ByteOrder::e_LITTLE_ENDIAN);
        TestDriver::testCase16(bdlde::ByteOrder::e_BIG_ENDIAN);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // UTF-16 LENGTH CALCULATION TEST -- INCORRECT UNICODE
        //
        // Documentation at start of 'void testCase15'.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                       "UTF-16 LENGTH CALCULATION TEST -- INCORRECT UNICODE\n"
                       "===================================================\n";

        TestDriver::testCase15(bdlde::ByteOrder::e_LITTLE_ENDIAN);
        TestDriver::testCase15(bdlde::ByteOrder::e_BIG_ENDIAN);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // UTF-16 & UTF-8 LENGTH CALCULATION TEST -- CORRECT UNICODE
        //
        // Documentation at start of 'void testCase14'.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                 "UTF-16 & UTF-8 LENGTH CALCULATION TEST -- CORRECT UNICODE\n"
                 "=========================================================\n";

        TestDriver::testCase14(bdlde::ByteOrder::e_LITTLE_ENDIAN);
        TestDriver::testCase14(bdlde::ByteOrder::e_BIG_ENDIAN);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // BACKWARDS BYTE ORDER TEST
        //
        // Concern:
        //: o That the translators work properly when translating to or from
        //:   the opposite of the host byte order.
        //
        // Plan:
        //: o The template functions 'swappedRangeEquals', 'sBegin', and
        //:   'sBack' (all defined in this file) make it possible to compare
        //:   two UTF-16 sequences and confirm that they match except that they
        //:   are in opposite byte order.  The enum 'e_BACKWARDS' (defined in
        //:   this file) is always defined to be opposite to the host byte
        //:   order.  Use these tools to create byte-swapped utf16 and verify
        //:   that it is the mirror image of known-good host byte order utf16.
        //
        // Testing:
        //    utf8ToUtf16 (all container overloads)
        //    utf16ToUtf8 (all container overloads)
        // --------------------------------------------------------------------

        if (verbose) cout << "BACKWARDS BYTE ORDER TEST\n"
                             "=========================\n";

        if (verbose) cout << "\tTesting bsl containers\n";

        TestDriver::testCase13<bsl::string,
                               bsl::wstring,
                               bsl::vector<char>,
                               bsl::vector<wchar_t>,
                               bsl::vector<unsigned short> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) cout << "\tTesting bsl containers\n";

        TestDriver::testCase13<bsl::string,
                               bsl::u16string,
                               bsl::vector<char>,
                               bsl::vector<wchar_t>,
                               bsl::vector<unsigned short> >();
#endif

        if (verbose) cout << "\tTesting std containers\n";

        TestDriver::testCase13<std::string,
                               std::wstring,
                               std::vector<char>,
                               std::vector<wchar_t>,
                               std::vector<unsigned short> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        if (verbose)
            cout << "\tTesting pmr containers\n";

        TestDriver::testCase13<std::pmr::string,
                               std::pmr::wstring,
                               std::pmr::vector<char>,
                               std::pmr::vector<wchar_t>,
                               std::pmr::vector<unsigned short> >();

        if (verbose)
            cout << "\tTesting pmr containers\n";

        TestDriver::testCase13<std::pmr::string,
                               std::pmr::u16string,
                               std::pmr::vector<char>,
                               std::pmr::vector<wchar_t>,
                               std::pmr::vector<unsigned short> >();
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // EMBEDDED ZEROES TEST
        //
        // Concern:
        //: o That the translators correctly translate sequences containing
        //:   embedded zeroes.
        //: o That embedded zeroes at inopportune places don't cause segfaults
        //:   or failed asserts.
        //
        // Plan:
        //: o Construct some snippet sequences with valid, random sequences of
        //:   Unicode, and randomly mix them up, with occasional zeroes in
        //:   between.
        //: o Insert zeroes at places all along the string, and observe whether
        //:   segfaults or failed asserts occur.
        //
        // Testing:
        //   utf8ToUtf16 (single container overload)
        //   utf16ToUtf8 (single container overload)
        // --------------------------------------------------------------------

        if (verbose) cout << "EMBEDDED ZEROES TEST\n"
                             "====================\n";

        if (verbose) cout << "\tTesting bsl containers\n";

        TestDriver::testCase12<bsl::string, bsl::wstring>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        TestDriver::testCase12<bsl::string, bsl::u16string>();
#endif

        if (verbose) cout << "\tTesting std containers\n";

        TestDriver::testCase12<std::string, std::wstring>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        TestDriver::testCase12<std::string, bsl::u16string>();
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        if (verbose) cout << "\tTesting pmr containers\n";

        TestDriver::testCase12<std::pmr::string, std::pmr::wstring>();
        TestDriver::testCase12<std::pmr::string, bsl::u16string>();
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // UTF-16 -> UTF-8: THOROUGH BROKEN GLASS TEST
        //
        // Concern:
        //   Subject the UTF-16 to UTF-8 translator to a wide variety of rugged
        //   inputs, including invalid inputs.
        //
        // Plan:
        //   The array 'DATA' contains a collection of every possible type of
        //   invalid UTF-8, along with a few varieties of valid sequences.  We
        //   take every possible combination of 3 of these snippets, and
        //   concatenate them into a single input string.  We then take all
        //   possible substrings of this combination, and feed them into the
        //   translator, and make predictions about what has to be true, and
        //   verify these predictions with assertions.
        //
        //   The effectiveness of these tests are enhanced by running them in
        //   safe mode, so we see that internal asserts within the translator
        //   are not triggering.
        //
        //   Note when comparing this TC to the one before it that there are
        //   far fewer ways for UTF-16 to go wrong than there are for UTF-8.
        //
        // Testing:
        //   utf16ToUtf8 (all overloads)
        // --------------------------------------------------------------------

        if (verbose) cout << "UTF-16 -> UTF-8: PILE OF BROKEN GLASS TEST\n"
                             "==========================================\n";

        // Common concern for all static functions of 'CharConvertUtf16' is
        // that no additional memory is allocated from default allocator.  At
        // the same time 'std' containers do not accept allocators for
        // construction.  To preserve common behaviour in the template test
        // function and supply bsl containers with an allocator different from
        // the default we have to create objects separately and then pass
        // references to the template test function.

        if (verbose) cout << "\tTesting bsl containers\n";

        TestDriver::testCase11<bsl::vector<char>, bsl::string, wchar_t>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        TestDriver::testCase11<bsl::vector<char>, bsl::string, char16_t>();
#endif

        if (verbose) cout << "\tTesting std containers\n";

        TestDriver::testCase11<std::vector<char>, std::string, wchar_t>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        TestDriver::testCase11<std::vector<char>, std::string, char16_t>();
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        if (verbose) cout << "\tTesting pmr containers\n";

        TestDriver::testCase11<std::pmr::vector<char>,
                               std::pmr::string,
                               wchar_t>();
        TestDriver::testCase11<std::pmr::vector<char>,
                               std::pmr::string,
                               char16_t>();
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // UTF-8 -> UTF-16: MORE THOROUGH BROKEN GLASS TEST
        //
        // Concern:
        //   Subject the UTF-8 to UTF-16 translator to a wide variety of rugged
        //   inputs, including invalid inputs.
        //
        // Plan:
        //   The array 'DATA' contains a collection of every possible type of
        //   invalid UTF-8, along with a few varieties of valid sequences.  We
        //   take every possible combination of 3 of these snippets, and
        //   concatenate them into a single input string.  We then take all
        //   possible substrings of this combination, and feed them into the
        //   translator, and make predictions about what has to be true, and
        //   verify these predictions with assertions.
        //
        //   The effectiveness of these tests are enhanced by running them in
        //   safe mode, so we see that internal asserts within the translator
        //   are not triggering.
        // --------------------------------------------------------------------

        if (verbose) cout << "UTF-8 -> UTF-16: PILE OF BROKEN GLASS TEST\n"
                             "==========================================\n";

        // Common concern for all static functions of 'CharConvertUtf16' is
        // that no additional memory is allocated from default allocator.  At
        // the same time 'std' containers do not accept allocators for
        // construction.  To preserve common behaviour in the template test
        // function and supply bsl containers with an allocator different from
        // the default we have to create objects separately and then pass
        // references to the template test function.

        if (verbose) cout << "\tTesting bsl containers\n";

        TestDriver::testCase10<bsl::vector<unsigned short>, bsl::wstring>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        TestDriver::testCase10<bsl::vector<unsigned short>, bsl::u16string>();
#endif

        if (verbose) cout << "\tTesting std containers\n";

        TestDriver::testCase10<std::vector<unsigned short>, std::wstring>();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        TestDriver::testCase10<std::vector<unsigned short>, std::u16string>();
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        if (verbose) cout << "\tTesting pmr containers\n";

        TestDriver::testCase10<std::pmr::vector<unsigned short>,
                               std::pmr::wstring>();
        TestDriver::testCase10<std::pmr::vector<unsigned short>,
                               bsl::u16string>();
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // BROKEN GLASS TEST -- UTF8TOUTF16
        //
        // Concern:
        //   That the length estimator always estimates a length >= the actual
        //   length required.
        //
        // Plan:
        //   Set up da "broken glass playground" consisting of all possible
        //   types of errors and valid sequences, then call a translation to
        //   vector on every subset of the playground.  Internal asserts within
        //   those routines will make sure the size estimation estimated at
        //   least the necessary size, and that if there were no errors,
        //   exactly the necessary size.
        // --------------------------------------------------------------------

        const char *utf8Broken[] = {
            "H"                                         // -- Valid ASCII
            "\xce\x97"                                  // -- Valid Greek
            "\xe4\xb8\xad"                              // -- Valid Chinese
            "\xf2\x94\xb4\xa5"                          // -- Valid Quad

            "\xed\xa0\x85"                 // illegal UTF-16 0xd805
            "\xed\xb6\xa3"                 // illegal UTF-16 0xdda3

            "\x83\x83\x83\x83\x83\x83"     // unexpected cont
            "\xf9\x83\x83\x83\x83"         // 5 byte (invalid)
            "\xfd\x83\x83\x83\x83\x83"     // 6 byte (invalid)
            "\xfe\xa3\x83\x83\x83\x83\x83" // 7 byte (invalid)
            "\xce"                         // incomplete Greek
            "\xe8\xb8"                     // incomplete Chinese
            "\xe8"                         // incomplete Chinese
            "\xf2\x94\xb4"                 // incomplete quad
            "\xf2\x94"                     // incomplete quad
            "\xf2"                         // incomplete quad
            "\xc0\x8f"                     // non-minimal 2
            "\xe0\x80\x8f"                 // non-minimal 3
            "\xf0\x80\x80\x8f" };          // non-minimal 4

        const char ** const utf8BrokenEnd = utf8Broken +
                                         (sizeof(utf8Broken) / sizeof(char *));

        bsl::vector<unsigned short> dstVec(&ta), dstVecB(&ta);

        for (const char **start = utf8Broken; start < utf8BrokenEnd; ++start) {
            const char *pcStart = *start;
            const char *pcEnd   = pcStart + bsl::strlen(pcStart);

            for (int i = 0; i < 2; ++i) {
                const unsigned short errorWord = 0 == i ? '?' : 0;

                bdlde::CharConvertUtf16::utf8ToUtf16(&dstVec,
                                                     pcStart,
                                                     0,
                                                     errorWord);

                bdlde::CharConvertUtf16::utf8ToUtf16(
                                             &dstVecB,
                                             bsl::string_view(pcStart,
                                                              pcEnd - pcStart),
                                             0,
                                             errorWord);

                ASSERT(dstVec == dstVecB);
            }
        }

        bsl::wstring dstW(&ta), dstWB(&ta);

        for (const char **start = utf8Broken; start < utf8BrokenEnd; ++start) {
            const char *pcStart = *start;
            const char *pcEnd   = pcStart + bsl::strlen(pcStart);

            for (int i = 0; i < 2; ++i) {
                const unsigned short errorWord = 0 == i ? '?' : 0;

                bdlde::CharConvertUtf16::utf8ToUtf16(&dstW,
                                                     pcStart,
                                                     0,
                                                     errorWord);

                bdlde::CharConvertUtf16::utf8ToUtf16(
                                             &dstWB,
                                             bsl::string_view(pcStart,
                                                              pcEnd - pcStart),
                                             0,
                                             errorWord);

                ASSERT(dstW == dstWB);
            }
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        bsl::u16string dstU16(&ta), dstU16B(&ta);

        for (const char **start = utf8Broken; start < utf8BrokenEnd; ++start) {
            const char *pcStart = *start;
            const char *pcEnd   = pcStart + bsl::strlen(pcStart);

            for (int i = 0; i < 2; ++i) {
                const unsigned short errorWord = 0 == i ? '?' : 0;

                bdlde::CharConvertUtf16::utf8ToUtf16(&dstU16,
                                                     pcStart,
                                                     0,
                                                     errorWord);

                bdlde::CharConvertUtf16::utf8ToUtf16(
                                             &dstU16B,
                                             bsl::string_view(pcStart,
                                                              pcEnd - pcStart),
                                             0,
                                             errorWord);

                ASSERT(dstU16 == dstU16B);
            }
        }
#endif

        if (verbose) cout << "With aeroes at random locations\n";

        for (const char **start = utf8Broken; start < utf8BrokenEnd; ++start) {
            const char   *pcStart = *start;
            const size_t  len     = bsl::strlen(pcStart);

            bsl::string s(&ta);
            s.reserve(200);

            for (unsigned uu = 0; uu < len; ++uu) {
                s = pcStart;
                s[uu] = '\0';

                for (int i = 0; i < 2; ++i) {
                    const unsigned short errorWord = 0 == i ? '?' : 0;

                    bdlde::CharConvertUtf16::utf8ToUtf16(&dstW,
                                                         s,
                                                         0,
                                                         errorWord);

                    bdlde::CharConvertUtf16::utf8ToUtf16(
                                                  &dstWB,
                                                  bsl::string_view(s.data(),
                                                                   s.length()),
                                                  0,
                                                  errorWord);

                    ASSERT(dstW == dstWB);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ERROR SEQUENCES IN UTF8
        //
        // Concerns:
        //   That sequences with error code points are handled correctly.
        // --------------------------------------------------------------------

        if (verbose) cout << "Error Sequences\n"
                             "===============\n";

        const unsigned char errorUnsignedIn[] = {  'a', 237, 160, 138, 'b',
                237, 164, 139, 'c', 237, 168, 147, 'd', 237, 174, 148, 'e',
                237, 178, 166, 'f', 237, 182, 183, 237, 187, 136, 'g',
                237, 191, 153, 'h', 240, 138, 170, 170, 'i',
                224, 158, 162, 'j', 192, 176, 'k', 132, 'm', 234, 170, 'n',
                193, 'o',   0 };

        const unsigned short expectedUtf16[] = { 'a', 'x', 'b', 'x',
                   'c', 'x', 'd', 'x', 'e', 'x', 'f', 'x', 'x', 'g',
                   'x', 'h', 'x', 'i', 'x', 'j', 'x', 'k', 'x', 'm',
                   'x', 'n', 'x', 'o',  0 };
        const unsigned short expectedUtf16Zero[] = { 'a', 'b', 'c', 'd', 'e',
                   'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o',  0 };
        enum { NUM_EXPECTED_UTF16 =
                        sizeof expectedUtf16     / sizeof *expectedUtf16,
               NUM_EXPECTED_UTF16_ZERO =
                        sizeof expectedUtf16Zero / sizeof *expectedUtf16Zero };

        const unsigned short errorUtf16InOrig[] = { 'a', 0xd81a, 'b',
                    0xdcb2, 'c', 0xd8a2, 0xda21, 'd', 0xdc34, 0xdc23, 'e', 0 };
        enum { NUM_ERROR_UTF16_IN_ORIG = sizeof errorUtf16InOrig /
                                                    sizeof *errorUtf16InOrig };

        for (int e = 0; e < 2; ++e) {
            char errorWord = 0 == e ? 'x' : 0;

            bsl::size_t numExpectedWords =
                      errorWord ? NUM_EXPECTED_UTF16 : NUM_EXPECTED_UTF16_ZERO;
            bsl::wstring expectedW(&ta);
            expectedW.assign(
                    errorWord ? expectedUtf16
                              : expectedUtf16Zero,
                    errorWord ? expectedUtf16 +     NUM_EXPECTED_UTF16     -1
                              : expectedUtf16Zero + NUM_EXPECTED_UTF16_ZERO-1);
            ASSERT(expectedW.length() + 1 == numExpectedWords);

            bsl::vector<unsigned short> expectedV(&ta);
            {
                const wchar_t *wPtr = expectedW.c_str();
                do {
                    expectedV.push_back((unsigned short) *wPtr);
                } while (0 != *wPtr++);
            }
            bsl::wstring utf16Wstring(&ta);

            {
                bsl::size_t nCodePoints;
                int rc = Util::utf8ToUtf16(&utf16Wstring,
                                           (const char *) errorUnsignedIn,
                                           &nCodePoints,
                                           errorWord);
                ASSERT(Status::k_INVALID_INPUT_BIT == rc);
                LOOP2_ASSERT(nCodePoints, numExpectedWords,
                                              numExpectedWords == nCodePoints);
                LOOP2_ASSERT(utf16Wstring.length(), numExpectedWords,
                                utf16Wstring.length() + 1 == numExpectedWords);

                ASSERT(expectedW == utf16Wstring);
                if    (expectedW != utf16Wstring) {
                    for (unsigned i = 0; i <= utf16Wstring.length(); ++i) {
                        wchar_t w = utf16Wstring[i];
                        if (w < 128) {
                            cout << (char) w << endl;
                        }
                        else {
                            cout << +w << endl;
                        }
                    }
                }

                utf16Wstring.clear();
                nCodePoints = -1;
                rc = Util::utf8ToUtf16(
                    &utf16Wstring,
                    bsl::string_view((const char *)errorUnsignedIn,
                                     (int)sizeof(errorUnsignedIn) - 1),
                    &nCodePoints,
                    errorWord);
                ASSERT(Status::k_INVALID_INPUT_BIT == rc);
                LOOP2_ASSERT(nCodePoints, numExpectedWords,
                                              numExpectedWords == nCodePoints);
                LOOP2_ASSERT(utf16Wstring.length(), numExpectedWords,
                                numExpectedWords == utf16Wstring.length() + 1);

                ASSERT(expectedW == utf16Wstring);
            }

            {
                bsl::vector<unsigned short> utf16Vec(&ta);
                bsl::size_t nCodePoints;
                int rc = Util::utf8ToUtf16(&utf16Vec,
                                           (const char *) errorUnsignedIn,
                                           &nCodePoints,
                                           errorWord);
                ASSERT(Status::k_INVALID_INPUT_BIT == rc);
                LOOP2_ASSERT(nCodePoints, numExpectedWords,
                                              numExpectedWords == nCodePoints);
                LOOP2_ASSERT(utf16Vec.size(), numExpectedWords,
                                          numExpectedWords == utf16Vec.size());
                LOOP2_ASSERT(expectedV.size(), utf16Vec.size(),
                                          expectedV.size() == utf16Vec.size());
                ASSERT(expectedV == utf16Vec);
                if (expectedV != utf16Vec) {
                    for (unsigned i = 0; i < utf16Vec.size(); ++i) {
                        unsigned short s = utf16Vec[i];
                        if (s < 128) {
                            cout << (char) s << endl;
                        }
                        else {
                            cout << s << endl;
                        }
                    }
                }

                utf16Vec.clear();
                nCodePoints = -1;
                rc = Util::utf8ToUtf16(
                          &utf16Vec,
                          bsl::string_view((const char *) errorUnsignedIn,
                                           (int) sizeof(errorUnsignedIn) - 1),
                          &nCodePoints,
                          errorWord);
                ASSERT(Status::k_INVALID_INPUT_BIT == rc);
                LOOP2_ASSERT(nCodePoints, numExpectedWords,
                                              numExpectedWords == nCodePoints);
                LOOP2_ASSERT(utf16Vec.size(), numExpectedWords,
                                          numExpectedWords == utf16Vec.size());
                LOOP2_ASSERT(expectedV.size(), utf16Vec.size(),
                                          expectedV.size() == utf16Vec.size());
                ASSERT(expectedV == utf16Vec);

                const wchar_t *wstr = utf16Wstring.c_str();

                for (unsigned i = 0; i < utf16Vec.size(); ++i) {
                    ASSERT(wstr[i] == utf16Vec[i]);
                }
            }
        }

        for (int e = 0; e < 2; ++e) {
            char errorByte = 0 == e ? 'x' : 0;

            const char *expected = errorByte ? "axbxcxxdxxe" : "abcde";
            const bsl::size_t expectedCodePoints = bsl::strlen(expected) + 1;

            {
                bsl::vector<char> utf8Vec(&ta);
                bsl::size_t nCodePoints;
                int rc = Util::utf16ToUtf8(&utf8Vec,
                                           errorUtf16InOrig,
                                           &nCodePoints,
                                           errorByte);
                ASSERT(Status::k_INVALID_INPUT_BIT == rc);
                ASSERT(expectedCodePoints == nCodePoints);
                ASSERT(expectedCodePoints == utf8Vec.size());

                ASSERT(0 == bsl::strcmp(&utf8Vec[0], expected));
            }

            {
                bsl::string utf8String(&ta);
                bsl::size_t nCodePoints, nBytes;
                const unsigned short *errorUtf16In = errorUtf16InOrig;
                int rc = Util::utf16ToUtf8(&utf8String,
                                           errorUtf16In,
                                           &nCodePoints,
                                           errorByte);
                nBytes = utf8String.length() + 1;

                ASSERT(Status::k_INVALID_INPUT_BIT == rc);
                ASSERT(expectedCodePoints == nCodePoints);
                ASSERT(expectedCodePoints == nBytes);
                ASSERT(expectedCodePoints == utf8String.length() + 1);

                ASSERT(0 == bsl::strcmp(utf8String.c_str(), expected));
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING CALLS WITH VECTOR / STRING / WSTRING DESTINATIONS
        //
        // Concerns:
        //   That the vector methods work.
        //
        // Plan:
        //   Test our long multilingual example
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING VECTOR METHODS\n"
                             "======================\n";

        enum { CAPACITY = 1 << 20 };

        unsigned short *utf16S = (unsigned short *)
                                ta.allocate(CAPACITY * sizeof(unsigned short));
        bsl::memset(utf16S, a5, CAPACITY * sizeof(unsigned short));
        wchar_t *utf16W = (wchar_t *) ta.allocate(CAPACITY * sizeof(wchar_t));
        bsl::memset(utf16W, a5, CAPACITY * sizeof(wchar_t));

        bsl::size_t numCodePoints16 = 0, numWords16S = 0, numWords16W = 0;

        int rc = Util::utf8ToUtf16(utf16S,
                                   CAPACITY,
                                   (const char *) utf8MultiLang,
                                   &numCodePoints16,
                                   &numWords16S);
        ASSERT(0 == rc);

        bsl::size_t nc;
        rc = Util::utf8ToUtf16(utf16W,
                               CAPACITY,
                               (const char *) utf8MultiLang,
                               &nc,
                               &numWords16W);
        ASSERT(0 == rc);
        ASSERT(numWords16S == numWords16W);
        ASSERT(numCodePoints16 == nc);
        for (int i = 0; i < (int) numWords16S; ++i) {
            ASSERT(utf16W[i] == (wchar_t) utf16S[i]);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        char16_t *utf16C16 = (char16_t *) ta.allocate(
                                                  CAPACITY * sizeof(char16_t));
        bsl::memset(utf16C16, a5, CAPACITY * sizeof(char16_t));
        bsl::size_t numWords16C16 = 0;

        nc = -1;
        rc = Util::utf8ToUtf16(utf16C16,
                               CAPACITY,
                               (const char *) utf8MultiLang,
                               &nc,
                               &numWords16C16);
        ASSERT(0 == rc);
        ASSERT(numWords16S == numWords16C16);
        ASSERT(numCodePoints16 == nc);
        ASSERT(sizeof(unsigned short) == sizeof(char16_t));
        ASSERT(0 == bsl::memcmp(utf16S, utf16C16,
                                                 CAPACITY * sizeof(char16_t)));
#endif
        numCodePoints16 = numWords16S = -1;

        bsl::memset(utf16S, a5, CAPACITY * sizeof(unsigned short));
        rc = Util::utf8ToUtf16(utf16S,
                               CAPACITY,
                               bsl::string_view((const char *) utf8MultiLang),
                               &numCodePoints16,
                               &numWords16S);
        ASSERT(0 == rc);
        ASSERT(numWords16S == numWords16W);
        ASSERT(numCodePoints16 == nc);
        for (int i = 0; i < (int) numWords16S; ++i) {
            ASSERT(utf16W[i] == (wchar_t) utf16S[i]);
        }

        numCodePoints16 = numWords16W = -1;

        bsl::memset(utf16W, a5, CAPACITY * sizeof(wchar_t));
        rc = Util::utf8ToUtf16(utf16W,
                               CAPACITY,
                               bsl::string_view((const char *) utf8MultiLang),
                               &numCodePoints16,
                               &numWords16W);
        ASSERT(0 == rc);
        ASSERT(numWords16S == numWords16W);
        ASSERT(numCodePoints16 == nc);
        for (int i = 0; i < (int) numWords16S; ++i) {
            ASSERT(utf16W[i] == (wchar_t) utf16S[i]);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        bsl::memset(utf16C16, a5, CAPACITY * sizeof(char16_t));
        numWords16C16 = 0;

        nc = -1;
        rc = Util::utf8ToUtf16(utf16C16,
                               CAPACITY,
                               bsl::string_view((const char *) utf8MultiLang),
                               &nc,
                               &numWords16C16);
        ASSERT(0 == rc);
        ASSERT(numWords16S == numWords16C16);
        ASSERT(numCodePoints16 == nc);
        ASSERT(sizeof(unsigned short) == sizeof(char16_t));
        ASSERT(0 == bsl::memcmp(utf16S, utf16C16,
                                                 CAPACITY * sizeof(char16_t)));
#endif

        {
            bsl::string buf(&ta);

            buf.resize(sizeof(utf8MultiLang), a5);

            bsl::size_t numBytesWritten = -1;
            nc = -1;
            rc = Util::utf16ToUtf8(buf.data(),
                                   sizeof(utf8MultiLang),
                                   utf16S,
                                   &nc,
                                   &numBytesWritten);
            ASSERT(0 == rc);
            ASSERT(numCodePoints16 == nc);
            ASSERT(!bsl::strcmp(buf.c_str(), charUtf8MultiLang));
            ASSERT(sizeof(utf8MultiLang) == numBytesWritten);

            bsl::fill(buf.begin(), buf.end(), a5);
            numBytesWritten = nc = -1;
            rc = Util::utf16ToUtf8(buf.data(),
                                   sizeof(utf8MultiLang),
                                   utf16S,
                                   numWords16S - 1,
                                   &nc,
                                   &numBytesWritten);
            ASSERT(0 == rc);
            ASSERT(numCodePoints16 == nc);
            ASSERT(!bsl::strcmp(buf.c_str(), charUtf8MultiLang));
            ASSERT(sizeof(utf8MultiLang) == numBytesWritten);
        }

        {
            bsl::string buf(&ta);

            buf.resize(sizeof(utf8MultiLang), a5);

            bsl::size_t numBytesWritten = -1;
            nc = -1;
            rc = Util::utf16ToUtf8(buf.data(),
                                   sizeof(utf8MultiLang),
                                   utf16W,
                                   &nc,
                                   &numBytesWritten);
            ASSERT(0 == rc);
            ASSERT(numCodePoints16 == nc);
            ASSERT(!bsl::strcmp(buf.c_str(), charUtf8MultiLang));
            ASSERT(sizeof(utf8MultiLang) == numBytesWritten);

            bsl::fill(buf.begin(), buf.end(), a5);
            numBytesWritten = nc = -1;
            rc = Util::utf16ToUtf8(buf.data(),
                                   sizeof(utf8MultiLang),
                                   bsl::wstring_view(utf16W,
                                                     numWords16W - 1),
                                   &nc,
                                   &numBytesWritten);
            ASSERT(0 == rc);
            ASSERT(numCodePoints16 == nc);
            ASSERT(!bsl::strcmp(buf.c_str(), charUtf8MultiLang));
            ASSERT(sizeof(utf8MultiLang) == numBytesWritten);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        {
            bsl::string buf(&ta);

            buf.resize(sizeof(utf8MultiLang), a5);

            bsl::size_t numBytesWritten = -1;
            nc = -1;
            rc = Util::utf16ToUtf8(buf.data(),
                                   sizeof(utf8MultiLang),
                                   utf16C16,
                                   &nc,
                                   &numBytesWritten);
            ASSERT(0 == rc);
            ASSERT(numCodePoints16 == nc);
            ASSERT(!bsl::strcmp(buf.c_str(), charUtf8MultiLang));
            ASSERT(sizeof(utf8MultiLang) == numBytesWritten);

            bsl::fill(buf.begin(), buf.end(), a5);
            numBytesWritten = nc = -1;
            rc = Util::utf16ToUtf8(buf.data(),
                                   sizeof(utf8MultiLang),
                                   bsl::u16string_view(utf16C16,
                                                       numWords16C16 - 1),
                                   &nc,
                                   &numBytesWritten);
            ASSERT(0 == rc);
            ASSERT(numCodePoints16 == nc);
            ASSERT(!bsl::strcmp(buf.c_str(), charUtf8MultiLang));
            ASSERT(sizeof(utf8MultiLang) == numBytesWritten);
        }
#endif

        {
            bsl::vector<unsigned short> utf16Vec(&ta);

            bsl::size_t numCodePoints16Vec = 0;

            rc = Util::utf8ToUtf16(&utf16Vec,
                                   (const char *) utf8MultiLang,
                                   &numCodePoints16Vec);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Vec == numCodePoints16);
            ASSERT(numWords16S   == utf16Vec.size());

            unsigned short *z16Ptr =
                                bsl::find(utf16Vec.begin(), utf16Vec.end(), 0);
            ASSERT(utf16Vec.end() != z16Ptr);
            ASSERT(&utf16Vec[utf16Vec.size() - 1] == z16Ptr);
            ASSERT(0 == *z16Ptr);

            ASSERT(0 == localUtf16Cmp(utf16Vec.begin(), utf16S));
            ASSERT(numWords16S - 1 == localUtf16Len(utf16Vec.begin()));

            rc = Util::utf8ToUtf16(
                                &utf16Vec,
                                bsl::string_view((const char *) utf8MultiLang),
                                &numCodePoints16Vec);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Vec == numCodePoints16);
            ASSERT(numWords16S   == utf16Vec.size());

            z16Ptr = bsl::find(utf16Vec.begin(), utf16Vec.end(), 0);
            ASSERT(utf16Vec.end() != z16Ptr);
            ASSERT(&utf16Vec[utf16Vec.size() - 1] == z16Ptr);
            ASSERT(0 == *z16Ptr);

            ASSERT(0 == localUtf16Cmp(utf16Vec.begin(), utf16S));
            ASSERT(numWords16S - 1 == localUtf16Len(utf16Vec.begin()));
        }

        {
            bsl::wstring utf16Wstring(&ta);

            bsl::size_t numCodePoints16Wstring = 0;

            rc = Util::utf8ToUtf16(&utf16Wstring,
                                   charUtf8MultiLang,
                                   &numCodePoints16Wstring);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Wstring == numCodePoints16);
            ASSERT(numWords16S       == utf16Wstring.length() + 1);
            ASSERT(localUtf16Len(utf16Wstring.c_str()) ==
                                                        utf16Wstring.length());
            ASSERT(utf16W == utf16Wstring);

            utf16Wstring.resize(utf16Wstring.length() / 2);
            bsl::fill(utf16Wstring.begin(), utf16Wstring.end(), a5);
            numCodePoints16Wstring = -1;
            rc = Util::utf8ToUtf16(
                               &utf16Wstring,
                               bsl::string_view((const char *) utf8MultiLang),
                               &numCodePoints16Wstring);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Wstring == numCodePoints16);
            ASSERT(numWords16S       == utf16Wstring.length() + 1);
            ASSERT(localUtf16Len(utf16Wstring.c_str()) ==
                                                        utf16Wstring.length());

            ASSERT(utf16W == utf16Wstring);
        }

        {
            std::wstring utf16Wstring;

            bsl::size_t numCodePoints16Wstring = 0;

            rc = Util::utf8ToUtf16(&utf16Wstring,
                                   charUtf8MultiLang,
                                   &numCodePoints16Wstring);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Wstring == numCodePoints16);
            ASSERT(numWords16S == utf16Wstring.length() + 1);
            ASSERT(localUtf16Len(utf16Wstring.c_str()) ==
                                                        utf16Wstring.length());

            ASSERT(utf16W == utf16Wstring);

            utf16Wstring.resize(utf16Wstring.length() / 2);
            bsl::fill(utf16Wstring.begin(), utf16Wstring.end(), a5);
            numCodePoints16Wstring = -1;
            rc = Util::utf8ToUtf16(&utf16Wstring,
                                   bsl::string_view(charUtf8MultiLang),
                                   &numCodePoints16Wstring);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Wstring == numCodePoints16);
            ASSERT(numWords16S == utf16Wstring.length() + 1);
            ASSERT(localUtf16Len(utf16Wstring.c_str()) ==
                                                        utf16Wstring.length());

            ASSERT(utf16W == utf16Wstring);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        {
            std::pmr::wstring utf16Wstring(std::pmr::new_delete_resource());

            bsl::size_t numCodePoints16Wstring = 0;

            rc = Util::utf8ToUtf16(&utf16Wstring,
                                   charUtf8MultiLang,
                                   &numCodePoints16Wstring);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Wstring == numCodePoints16);
            ASSERT(numWords16S == utf16Wstring.length() + 1);
            ASSERT(localUtf16Len(utf16Wstring.c_str()) ==
                                                        utf16Wstring.length());

            ASSERT(utf16W == utf16Wstring);

            utf16Wstring.resize(utf16Wstring.length() / 2);
            bsl::fill(utf16Wstring.begin(), utf16Wstring.end(), a5);
            numCodePoints16Wstring = -1;
            rc = Util::utf8ToUtf16(&utf16Wstring,
                                   bsl::string_view(charUtf8MultiLang),
                                   &numCodePoints16Wstring);
            ASSERT(0 == rc);

            ASSERT(numCodePoints16Wstring == numCodePoints16);
            ASSERT(numWords16S == utf16Wstring.length() + 1);
            ASSERT(localUtf16Len(utf16Wstring.c_str()) ==
                                                        utf16Wstring.length());

            ASSERT(utf16W == utf16Wstring);
        }
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        {
            bsl::u16string utf16U16Str(&ta);

            rc = Util::utf8ToUtf16(&utf16U16Str,
                                   (const char *) utf8MultiLang,
                                   &nc);
            ASSERT(0 == rc);

            ASSERT(nc == numCodePoints16);
            ASSERT(numWords16S == utf16U16Str.length() + 1);
            ASSERT(localUtf16Len(utf16U16Str.c_str()) == utf16U16Str.length());
            ASSERT(sizeof(utf16U16Str[0]) == sizeof(*utf16S));
            ASSERT(0 == bsl::memcmp(utf16S, utf16U16Str.data(),
                                (utf16U16Str.length() + 1) * sizeof(*utf16S)));

            utf16U16Str.resize(utf16U16Str.length() / 2);
            bsl::fill(utf16U16Str.begin(), utf16U16Str.end(), a5);

            nc = -1;
            rc = Util::utf8ToUtf16(
                                &utf16U16Str,
                                bsl::string_view((const char *) utf8MultiLang),
                                &nc);
            ASSERT(0 == rc);

            ASSERT(nc == numCodePoints16);
            ASSERT(numWords16S == utf16U16Str.length() + 1);
            ASSERT(localUtf16Len(utf16U16Str.c_str()) == utf16U16Str.length());
            ASSERT(0 == bsl::memcmp(utf16S, utf16U16Str.data(),
                                (utf16U16Str.length() + 1) * sizeof(*utf16S)));
        }
#endif

        {
            std::vector<char> utf8Vec;
            {
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16S,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(&utf8Vec[0]) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(&utf8Vec[0], charUtf8MultiLang));


                utf8Vec.resize(utf8Vec.size() / 2);
                bsl::memset(&utf8Vec[0], a5, utf8Vec.size());
                numCodePoints8 = -1;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16S,
                                       numWords16S - 1,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(&utf8Vec[0]) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(&utf8Vec[0], charUtf8MultiLang));
            }
            {
                utf8Vec.resize(utf8Vec.size() / 2);
                bsl::memset(&utf8Vec[0], a5, utf8Vec.size());
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16W,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(&utf8Vec[0]) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(&utf8Vec[0], charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                utf8Vec.resize(utf8Vec.size() / 2);
                bsl::memset(&utf8Vec[0], a5, utf8Vec.size());
                numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       bsl::wstring_view(
                                                         utf16W,
                                                         (int)numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(&utf8Vec[0]) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(&utf8Vec[0], charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);
            }
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        {
            std::pmr::vector<char> utf8VecP(std::pmr::new_delete_resource());
            {
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8VecP,
                                       utf16S,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8VecP.size());

                ASSERT(bsl::strlen(&utf8VecP[0]) + 1 == utf8VecP.size());
                ASSERT(!bsl::strcmp(&utf8VecP[0], charUtf8MultiLang));


                utf8VecP.resize(utf8VecP.size() / 2);
                bsl::memset(&utf8VecP[0], a5, utf8VecP.size());
                numCodePoints8 = -1;

                rc = Util::utf16ToUtf8(&utf8VecP,
                                       utf16S,
                                       numWords16S - 1,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8VecP.size());

                ASSERT(bsl::strlen(&utf8VecP[0]) + 1 == utf8VecP.size());
                ASSERT(!bsl::strcmp(&utf8VecP[0], charUtf8MultiLang));
            }
            {
                utf8VecP.resize(utf8VecP.size() / 2);
                bsl::memset(&utf8VecP[0], a5, utf8VecP.size());
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8VecP,
                                       utf16W,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8VecP.size());

                ASSERT(bsl::strlen(&utf8VecP[0]) + 1 == utf8VecP.size());
                ASSERT(!bsl::strcmp(&utf8VecP[0], charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                rc = Util::utf16ToUtf8(&utf8VecP,
                                       bsl::wstring_view(
                                                         utf16W,
                                                         (int)numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8VecP.size());

                ASSERT(bsl::strlen(&utf8VecP[0]) + 1 == utf8VecP.size());
                ASSERT(!bsl::strcmp(&utf8VecP[0], charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);
            }
        }
#endif

        {
            bsl::vector<char> utf8Vec(&ta);
            {
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16S,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(utf8Vec.data()) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(utf8Vec.data(), charUtf8MultiLang));
            }
            {
                utf8Vec.clear();
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16W,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(utf8Vec.data()) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(utf8Vec.data(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                bsl::vector<char> utf8VecB(&ta);
                rc = Util::utf16ToUtf8(&utf8VecB,
                                       bsl::wstring_view(
                                                         utf16W,
                                                         (int)numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(utf8VecB == utf8Vec);
            }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            {
                utf8Vec.clear();
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16C16,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(utf8Vec.data()) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(utf8Vec.data(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                numCodePoints8 = -1;

                bsl::vector<char> utf8VecB(&ta);
                rc = Util::utf16ToUtf8(&utf8VecB,
                                       bsl::u16string_view(
                                                       utf16C16,
                                                       (int)numWords16C16 - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(utf8VecB == utf8Vec);
            }
            {
                std::vector<char> utf8Vecs;
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vecs,
                                       utf16C16,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vecs.size());

                ASSERT(bsl::strlen(utf8Vecs.data()) + 1 == utf8Vecs.size());
                ASSERT(!bsl::strcmp(utf8Vecs.data(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                numCodePoints8 = -1;

                std::vector<char> utf8VecB;
                rc = Util::utf16ToUtf8(&utf8VecB,
                                       bsl::u16string_view(
                                                       utf16C16,
                                                       (int)numWords16C16 - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(utf8VecB == utf8Vecs);
            }
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            {
                std::pmr::vector<char> utf8Vecs(
                                              std::pmr::new_delete_resource());
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vecs,
                                       utf16C16,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vecs.size());

                ASSERT(bsl::strlen(utf8Vecs.data()) + 1 == utf8Vecs.size());
                ASSERT(!bsl::strcmp(utf8Vecs.data(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                numCodePoints8 = -1;

                std::pmr::vector<char> utf8VecB(
                                              std::pmr::new_delete_resource());
                rc = Util::utf16ToUtf8(&utf8VecB,
                                       bsl::u16string_view(
                                                       utf16C16,
                                                       (int)numWords16C16 - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(utf8VecB == utf8Vecs);
            }
# endif
#endif

            {
                bsl::string utf8String(&ta);

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16S,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.data(), charUtf8MultiLang));
            }
            {
                bsl::string utf8String(&ta);

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16W,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.data(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                bsl::string utf8StringB(&ta);
                rc = Util::utf16ToUtf8(&utf8StringB,
                                       bsl::wstring_view(
                                                        utf16W,
                                                        (int) numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);
                ASSERT(utf8String == utf8StringB);
            }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            {
                bsl::string utf8String(&ta);

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16C16,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.c_str(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                numCodePoints8 = -1;
                bsl::string utf8StringB(&ta);
                rc = Util::utf16ToUtf8(&utf8StringB,
                                       bsl::u16string_view(
                                                        utf16C16,
                                                        (int) numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);
                ASSERT(utf8String == utf8StringB);
            }
#endif
            {
                std::string utf8String;

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16S,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.data(), charUtf8MultiLang));
            }
            {
                std::string utf8String;

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16W,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.data(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                bsl::string utf8StringB(&ta);
                rc = Util::utf16ToUtf8(&utf8StringB,
                                       bsl::wstring_view(
                                                        utf16W,
                                                        (int) numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);
                ASSERT(utf8String == utf8StringB);
            }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            {
                std::string utf8String;

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16C16,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.c_str(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                numCodePoints8 = -1;
                std::string utf8StringB;
                rc = Util::utf16ToUtf8(&utf8StringB,
                                       bsl::u16string_view(
                                                        utf16C16,
                                                        (int) numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);
                ASSERT(utf8String == utf8StringB);
            }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            {
                std::pmr::string utf8String(std::pmr::new_delete_resource());

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16C16,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.c_str(), charUtf8MultiLang));

                ASSERT(0 == utf16W[numWords16W - 1]);

                numCodePoints8 = -1;
                std::pmr::string utf8StringB(std::pmr::new_delete_resource());
                rc = Util::utf16ToUtf8(&utf8StringB,
                                       bsl::u16string_view(
                                                        utf16C16,
                                                        (int) numWords16W - 1),
                                       &numCodePoints8);
                ASSERT(0 == rc);
                ASSERT(utf8String == utf8StringB);
            }
#endif

            utf16S[numWords16S - 1] = 1001;
            utf16S[numWords16S + 0] = 1002;
            utf16S[numWords16S + 1] = 1003;
            utf16S[numWords16S + 2] = 1004;

            {
                bsl::fill(utf8Vec.begin(), utf8Vec.end(), a5);
                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8Vec,
                                       utf16S,
                                       numWords16S - 1,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(sizeof(utf8MultiLang) == utf8Vec.size());

                ASSERT(bsl::strlen(utf8Vec.begin()) + 1 == utf8Vec.size());
                ASSERT(!bsl::strcmp(utf8Vec.begin(), charUtf8MultiLang));
            }
            {
                bsl::string utf8String(&ta);

                bsl::size_t numCodePoints8 = 0;

                rc = Util::utf16ToUtf8(&utf8String,
                                       utf16S,
                                       numWords16S - 1,
                                       &numCodePoints8);
                ASSERT(0 == rc);

                bsl::size_t numBytes8 = utf8String.length() + 1;

                ASSERT(numCodePoints8 == numCodePoints16);
                ASSERT(numBytes8 == sizeof(utf8MultiLang));

                ASSERT(bsl::strlen(utf8String.c_str()) + 1 == numBytes8);
                ASSERT(!bsl::strcmp(utf8String.data(), charUtf8MultiLang));
            }
        }

        ta.deallocate(utf16S);
        ta.deallocate(utf16W);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        ta.deallocate(utf16C16);
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TRANSLATING MULTILINGUAL TEXT FROM UTF-8 TO UTF-16 AND BACK
        //
        // Concern:
        //   None of the other test case translated a long sequence of prose
        //   that would really exercise UTF.  Having 'Pride and Prejudice' be
        //   the only long test case here is unbelievably lame.
        //
        // Plan:
        //   The array 'utf8MultiLang' is a long sequence of UTF-8 Chinese,
        //   Hindi, French, and Greek copied from the internet.  Use the tools
        //   here to see if we can translate it from UTF-8 to UTF-16 and back.
        //
        // Testing:
        //   utf8ToUtf16 (various overloads)
        //   utf16ToUtf8 (various overloads)
        // --------------------------------------------------------------------

        if (verbose) cout << "Long UTF-8 Multilingual test case\n"
                             "=================================\n";

        enum { CAPACITY = 1 << 20 };

        // size up text

        int singleBytes = 0;
        int doubleHeaders = 0;
        int tripleHeaders = 0;
        int quadHeaders = 0;
        int contBytes = 0;
        int errorBytes = 0;

        for (unsigned char *pc = utf8MultiLang; *pc; ++pc) {
            if      (   0 == (0x80 & *pc)) ++singleBytes;
            else if (0xc0 == (0xe0 & *pc)) ++doubleHeaders;
            else if (0xe0 == (0xf0 & *pc)) ++tripleHeaders;
            else if (0xf0 == (0xf8 & *pc)) ++quadHeaders;
            else if (0x80 == (0xc0 & *pc)) ++contBytes;
            else                           ++errorBytes;
        }

        const unsigned CODE_POINTS = singleBytes + doubleHeaders +
                                               tripleHeaders + quadHeaders + 1;
        const unsigned UTF16_WORDS = singleBytes + doubleHeaders +
                                           tripleHeaders + 2 * quadHeaders + 1;

        if (verbose) {
            P_(sizeof(utf8MultiLang)) P_(singleBytes) P(doubleHeaders);
            P_(tripleHeaders) P_(quadHeaders) P_(contBytes) P(errorBytes);
        }

        ASSERT(CAPACITY > sizeof(utf8MultiLang));
        ASSERT(doubleHeaders + tripleHeaders * 2 + quadHeaders * 3
                                                                 == contBytes);
        ASSERT(0 == errorBytes);

        bslma::TestAllocator ta;
        unsigned short *utf16S = (unsigned short *)
                                ta.allocate(CAPACITY * sizeof(unsigned short));
        std::size_t numCodePoints16 = 0, numWords16S = 0;

        int rc = Util::utf8ToUtf16(utf16S,
                                   CAPACITY,
                                   charUtf8MultiLang,
                                   &numCodePoints16,
                                   &numWords16S);

        if (verbose) {
            Q(utf8ToUtf16:);
            P_(rc) P_(numCodePoints16) P(numWords16S);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16  < sizeof(utf8MultiLang));
        ASSERTV(CODE_POINTS, numCodePoints16, CODE_POINTS == numCodePoints16);
        ASSERT(numWords16S < sizeof(utf8MultiLang));
        ASSERT(numWords16S >= numCodePoints16);
        ASSERTV(UTF16_WORDS, numWords16S, UTF16_WORDS == numWords16S);

        unsigned short *utf16SB = static_cast<unsigned short *>(
                               ta.allocate(CAPACITY * sizeof(unsigned short)));
        std::size_t numCodePoints16B = 0, numWords16SB = 0;

        rc = Util::utf8ToUtf16(utf16SB,
                               CAPACITY,
                               bsl::string_view(charUtf8MultiLang),
                               &numCodePoints16B,
                               &numWords16SB);

        if (verbose) {
            Q(utf8ToUtf16:);
            P_(rc) P_(numCodePoints16B) P(numWords16SB);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16B  < sizeof(utf8MultiLang));
        ASSERTV(CODE_POINTS, numCodePoints16B,
                                              CODE_POINTS == numCodePoints16B);
        ASSERT(numWords16SB < sizeof(utf8MultiLang));
        ASSERT(numWords16SB >= numCodePoints16B);
        ASSERTV(UTF16_WORDS, numWords16SB, UTF16_WORDS == numWords16SB);

        ASSERT(numCodePoints16B == numCodePoints16);
        ASSERT(numWords16SB     == numWords16S);
        ASSERT(0 == std::memcmp(utf16S,
                                utf16SB,
                                numWords16S * sizeof(unsigned short)));

        char *utf8 = (char *) ta.allocate(CAPACITY);
        std::memset(utf8, a5, CAPACITY);

        std::size_t numCodePoints8 = 0, numBytes8 = 0;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        char16_t *utf16C = (char16_t *)
                                      ta.allocate(CAPACITY * sizeof(char16_t));
        std::size_t numCodePoints16C = 0, numWords16C = 0;

        rc = Util::utf8ToUtf16(utf16C,
                               CAPACITY,
                               charUtf8MultiLang,
                               &numCodePoints16C,
                               &numWords16C);

        if (verbose) {
            Q(utf8ToUtf16:);
            P_(rc) P_(numCodePoints16C) P(numWords16C);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16C  < sizeof(utf8MultiLang));
        ASSERTV(CODE_POINTS, numCodePoints16C,
                                              CODE_POINTS == numCodePoints16C);
        ASSERT(numWords16C < sizeof(utf8MultiLang));
        ASSERT(numWords16C >= numCodePoints16C);
        ASSERTV(UTF16_WORDS, numWords16C, UTF16_WORDS == numWords16C);

        char16_t *utf16CB = static_cast<char16_t *>(
                                     ta.allocate(CAPACITY * sizeof(char16_t)));
        numCodePoints16C = 0, numWords16C = 0;

        rc = Util::utf8ToUtf16(utf16CB,
                               CAPACITY,
                               bsl::string_view(charUtf8MultiLang),
                               &numCodePoints16C,
                               &numWords16C);

        if (verbose) {
            Q(utf8ToUtf16:);
            P_(rc) P_(numCodePoints16C) P(numWords16C);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16C  < sizeof(utf8MultiLang));
        ASSERTV(CODE_POINTS, numCodePoints16C,
                                              CODE_POINTS == numCodePoints16C);
        ASSERT(numWords16C < sizeof(utf8MultiLang));
        ASSERT(numWords16C >= numCodePoints16C);
        ASSERTV(UTF16_WORDS, numWords16C, UTF16_WORDS == numWords16C);

        ASSERT(numCodePoints16C == numCodePoints16C);
        ASSERT(numWords16C      == numWords16C);
        ASSERT(0 == std::memcmp(utf16C,
                                utf16CB,
                                numWords16C * sizeof(char16_t)));

        ASSERT(0 == std::memcmp(utf16S,
                                utf16C,
                                numWords16C * sizeof(char16_t)));

        numCodePoints16C = 0;
        rc = Util::utf16ToUtf8(utf8,
                               CAPACITY,
                               utf16C,
                               &numCodePoints16C,
                               &numBytes8);
        ASSERT(0 == rc);

        ASSERTV(CODE_POINTS, numCodePoints16C,
                                              CODE_POINTS == numCodePoints16C);
        ASSERTV(UTF16_WORDS, numBytes8, sizeof(utf8MultiLang) == numBytes8);

        ASSERT(0 == std::memcmp(utf8, utf8MultiLang, sizeof(utf8MultiLang)));

        numCodePoints16C = 0, numBytes8 = 0;
        std::memset(utf8, a5, CAPACITY);

        rc = Util::utf16ToUtf8(utf8,
                               CAPACITY,
                               bsl::u16string_view(utf16C),
                               &numCodePoints16C,
                               &numBytes8);
        ASSERT(0 == rc);

        ASSERTV(CODE_POINTS, numCodePoints16C,
                                              CODE_POINTS == numCodePoints16C);
        ASSERTV(UTF16_WORDS, numBytes8, sizeof(utf8MultiLang) == numBytes8);

        ASSERT(0 == bsl::memcmp(utf8, utf8MultiLang, sizeof(utf8MultiLang)));
#endif

        wchar_t *utf16W = static_cast<wchar_t *>(
                                      ta.allocate(CAPACITY * sizeof(wchar_t)));
        bsl::size_t numWords16W = 0;
        numCodePoints16 = 0;

        rc = Util::utf8ToUtf16(utf16W,
                               CAPACITY,
                               charUtf8MultiLang,
                               &numCodePoints16,
                               &numWords16W);

        if (verbose) {
            Q(utf8ToUtf16:);
            P_(rc) P_(numCodePoints16) P(numWords16W);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16  < sizeof(utf8MultiLang));
        ASSERTV(CODE_POINTS, numCodePoints16, CODE_POINTS == numCodePoints16);
        ASSERT(numWords16W < sizeof(utf8MultiLang));
        ASSERT(numWords16W >= numCodePoints16);
        ASSERT(numWords16W == numWords16S);
        ASSERTV(UTF16_WORDS, numWords16W, UTF16_WORDS == numWords16W);

        for (unsigned ii = 0; ii < numWords16W; ++ii) {
            if (utf16S[ii] != utf16W[ii]) {
                ASSERTV(ii, utf16S[ii], utf16W[ii], utf16S[ii] != utf16W[ii]);
                break;
            }
        }

        wchar_t *utf16WB = static_cast<wchar_t *>(
                                      ta.allocate(CAPACITY * sizeof(wchar_t)));
        bsl::size_t numWords16WB = 0;
        numCodePoints16B = 0;

        rc = Util::utf8ToUtf16(utf16WB,
                               CAPACITY,
                               bsl::string_view(charUtf8MultiLang),
                               &numCodePoints16B,
                               &numWords16WB);

        if (verbose) {
            Q(utf8ToUtf16:);
            P_(rc) P_(numCodePoints16) P(numWords16W);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16B  < sizeof(utf8MultiLang));
        ASSERTV(CODE_POINTS, numCodePoints16B,
                                              CODE_POINTS == numCodePoints16B);
        ASSERT(numWords16WB < sizeof(utf8MultiLang));
        ASSERT(numWords16WB >= numCodePoints16);
        ASSERT(numWords16WB == numWords16S);
        ASSERTV(UTF16_WORDS, numWords16WB, UTF16_WORDS  == numWords16WB);

        ASSERT(numCodePoints16B == numCodePoints16);
        ASSERT(numWords16WB     == numWords16W);

        ASSERT(0 == bsl::memcmp(utf16W,
                                utf16WB,
                                numWords16W * sizeof(wchar_t)));

        numCodePoints8 = 0, numBytes8 = 0;

        bsl::memset(utf8, 0xa5, CAPACITY);

        rc = Util::utf16ToUtf8(utf8,
                               CAPACITY,
                               utf16S,
                               &numCodePoints8,
                               &numBytes8);

        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P_(numCodePoints8) P(numBytes8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(CODE_POINTS == numCodePoints8);
        ASSERT(numBytes8  == sizeof(utf8MultiLang));
        ASSERT(bsl::strlen(utf8) + 1 == numBytes8);
        ASSERT(!bsl::strcmp(utf8, charUtf8MultiLang));

        numCodePoints8 = 0;
        numBytes8 = 0;
        bsl::memset(utf8, 0xa5, CAPACITY);

        numCodePoints8 = 0;
        bsl::string s(&ta);

        rc = Util::utf16ToUtf8(&s,
                               utf16S,
                               &numCodePoints8);

        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P(numCodePoints8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(CODE_POINTS == numCodePoints8);
        ASSERT(s == charUtf8MultiLang);
        ASSERT(s.length() + 1 == sizeof(utf8MultiLang));

        numCodePoints8 = 0;
        bsl::vector<char> v(&ta);

        rc = Util::utf16ToUtf8(&v,
                               utf16S,
                               &numCodePoints8);

        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P(numCodePoints8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(CODE_POINTS == numCodePoints8);
        ASSERT(!bsl::strcmp(&v[0], charUtf8MultiLang));
        ASSERT(v.size() == sizeof(utf8MultiLang));

        utf16S[numWords16W - 1] = 'a';
        utf16S[numWords16W + 0] = 'b';
        utf16S[numWords16W + 1] = 'c';
        utf16S[numWords16W + 2] = 'd';

        numCodePoints8 = 0;
        numBytes8 = 0;
        bsl::memset(utf8, 0xa5, CAPACITY);

        rc = Util::utf16ToUtf8(utf8,
                               CAPACITY,
                               utf16S,
                               numWords16W - 1,
                               &numCodePoints8,
                               &numBytes8);
        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P_(numCodePoints8) P(numBytes8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(CODE_POINTS == numCodePoints8);
        ASSERT(numBytes8  == sizeof(utf8MultiLang));
        ASSERT(bsl::strlen(utf8) + 1 == numBytes8);
        ASSERT(!bsl::strcmp(utf8, charUtf8MultiLang));

        numCodePoints8 = 0;
        bsl::memset(&s[0], 0xaf, s.length());
        s.clear();

        rc = Util::utf16ToUtf8(&s,
                               utf16S,
                               numWords16S - 1,
                               &numCodePoints8);
        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P(numCodePoints8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(CODE_POINTS == numCodePoints8);
        ASSERT(s == charUtf8MultiLang);
        ASSERT(s.length() + 1 == sizeof(utf8MultiLang));


        numCodePoints8 = 0;
        bsl::memset(&v[0], 0xa5, v.size());
        v.clear();

        rc = Util::utf16ToUtf8(&v,
                               utf16S,
                               numWords16S - 1,
                               &numCodePoints8);

        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P(numCodePoints8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(CODE_POINTS == numCodePoints8);
        ASSERT(!bsl::strcmp(&v[0], charUtf8MultiLang));
        ASSERT(v.size() == sizeof(utf8MultiLang));

        numCodePoints8 = 0;
        numBytes8 = 0;
        bsl::memset(utf8, 0xa5, CAPACITY);

        rc = Util::utf16ToUtf8(utf8,
                               CAPACITY,
                               utf16W,
                               &numCodePoints8,
                               &numBytes8);

        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P_(numCodePoints8) P(numBytes8);
        }

        ASSERT(0 == rc);
        ASSERT(numCodePoints16 == numCodePoints8);
        ASSERT(numBytes8  == sizeof(utf8MultiLang));
        ASSERT(bsl::strlen(utf8) + 1 == numBytes8);
        ASSERT(!bsl::strcmp(utf8, charUtf8MultiLang));

        ASSERT(0 == utf16W[numWords16W - 1]);

        rc = Util::utf16ToUtf8(utf8,
                               CAPACITY,
                               bsl::wstring_view(utf16W,
                                                 (int) numWords16W - 1),
                               &numCodePoints8,
                               &numBytes8);

        if (verbose) {
            Q(utf16ToUtf8:);
            P_(rc) P_(numCodePoints8) P(numBytes8);
        }

        ASSERT(0 == rc);
        LOOP2_ASSERT(numCodePoints16, numCodePoints8,
                                            numCodePoints16 == numCodePoints8);
        LOOP2_ASSERT(numBytes8, sizeof(utf8MultiLang),
                                           numBytes8 == sizeof(utf8MultiLang));
        LOOP2_ASSERT(bsl::strlen(utf8) + 1, numBytes8,
                                           bsl::strlen(utf8) + 1 == numBytes8);
        ASSERT(!bsl::strcmp(utf8, charUtf8MultiLang));

        ta.deallocate(utf16S);
        ta.deallocate(utf16SB);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ta.deallocate(utf16C);
        ta.deallocate(utf16CB);
#endif
        ta.deallocate(utf16W);
        ta.deallocate(utf16WB);
        ta.deallocate(utf8);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CODER/DECODER SYNCHRONIZATION
        //   Test the ability of the coder/decoder to remain synchronized
        //   with the octet/word stream as it encounters different coding
        //   cases in sequence.
        //
        // Concerns:
        //
        // Plan:
        //   Guided by one or more 'coding-case strings' indicating the various
        //   coding cases (single-octet, two-octet ... single-word, two-word),
        //   generate code point sequences that have all possible coding-case
        //   trigraphs--not trigraphs on the code points themselves, but on the
        //   coding cases.  Start and end of string are treated as a coding
        //   type and are represented in the trigraphs (but only at the
        //   beginning or end) and coding case trigraphs consisting of a single
        //   valid coding between beginning and end are not covered, because
        //   they are covered in Test 2.
        //
        //   For each generated UTF-8 string:
        //   Convert the string to UTF-16.  Verify that this initial conversion
        //   is correct.  Verify that the original string is undamaged.
        //   Convert back.  Verify that the result of this second conversion
        //   is identical with the original string.  Verify that the input
        //   string in this second conversion is unaltered.
        //
        //   Because the case boundary for UTF-16 (single-word/double-word)
        //   aligns with a case boundary in UTF-8 (less-than-four-octet/
        //   four-octet) anything which exercises all the UTF-8 sequences must
        //   necessarily exercise all the UTF-8 sequences.
        //
        // Testing:
        //   A set of control sequences governs the test, which creates strings
        //   of UTF-8 code points according to their coding.  The control
        //   sequences ensure that all coding-case trigraphs are tested,
        //   including those that include beginning-of-string and end-of-
        //   string.  This test is run three-to-the-fourth times five factorial
        //   times: three-to-the-fourth because three different values are used
        //   for the single-octet code point and each of the header content
        //   fields; five factorial because in each combination of single-octet
        //   and header contents, all permutations of five different
        //   continuation octets are used.  (Actually, six are needed, but the
        //   runtime with the extra permutation would approach ten seconds, so
        //   one of the continuation octets has its contents chosen in a way
        //   that depends on the others.)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTest 5: Coder/decoder synchronization, no error cases."
                    "\n======================================================"
                 << endl;
        }

        const char* u8CodingCases[] ={
            // The code points 'a', 'b', 'c', and 'd' in these strings
            // represent not themselves but any valid single-, two-, three-, or
            // four-octet code point, respectively.  These strings together
            // contain all coding-case trigraphs: trigraphs of
            // Begin-/End-of-String, 'a', 'b', 'c', and 'd', except for the
            // single-code point Begin-'a'-End, Begin-'b'-End, etc.  And of
            // course Begin and End occur only at the beginning and end,
            // respectively.
            //
            // This sequence list was generated with the aid of the manual
            // version of the 'ng' program (ng13.cpp).  A copy of ng13.cpp is
            // included in comments near the end of this file.

            "aaabbbcccdddaddbddcdbacbdacababcbcadccbbd",
            "adaacca",
            "acdcaa",
            "badb",
            "dab",
            "cbaad",
            "bcd",
            "dbb",
            "bba",
            "cc",
            "dc",
            "bdbc",
            "cda",
            "dd",
            "cac",
            "abdcb",
        };

        enum { MAX_NCHARS = 5 * 5 * 5,   // String contains no repetitions of
                                         // trigraphs, could be no more than
                                         // number of distinct symbols to power
                                         // of length of n-graph -- five to the
                                         // third.  In fact, it's NOT all one
                                         // string, but several (null
                                         // code points the place of the
                                         // string-breaks) and and omits some
                                         // the break-letter- break trigraphs.
                                         // This allocation is safe and not
                                         // excessively large.
               MAX_NOCTETS = MAX_NCHARS * 4 + 1,   // Excess again; only one
                                                   // char in three will be a
                                                   // four octet char.  But
                                                   // this is safe and cheap.
               MAX_NWORDS = MAX_NCHARS * 2 + 1     // Excess again, only
                                                   // 4-octet UTF-8 code points
                                                   // will translate to UTF-16.
        };

        // The test sets are created in two stages, using a four-wheel
        // OdomIter, three places per wheel, one wheel for the single-byte
        // code point and one for each of the headers, and a Permuter with five
        // places to shift the continuation parts around.  Since we need six
        // different continuation contents but the permuter only permutes five
        // (to keep the execution time reasonable) we re-use one permuter value
        // in the middle continuation octet of the four-octet sequence.  It
        // can't do any mischief there.
        //
        // Since the OdomIter is cheaper to operate than the Permuter, the
        // Permuter is the outer loop.

        const unsigned char OneByteForTest5[] ={
            '\x1', '\x21', '\x73',      // ^A, !, s
        };

        const unsigned char ThreeByteForTest5[] ={
            '\x1', '\x6', '\xe',        // Avoids the various nasty cases.
        };

        AvCharList OneFor5(OneByteForTest5);
        AvCharList TwoFor5(u8TwoByteHdrCases + 3, u8TwoByteHdrCases + 6);
        AvCharList ThreeFor5(ThreeByteForTest5);
        AvCharList FourFor5(u8FourByteHdrCases + 1, u8FourByteHdrCases + 4);

        // The one-byte case iterator is placed last so it will change fastest.
        // Since the single-byte case is the easiest to spot in veryVeryVerbose
        // output, this should make it easier to verify that the changes are
        // taking place.

        AvCharList *const wheelsFive[] ={ &FourFor5,
                                          &ThreeFor5,
                                          &TwoFor5,
                                          &OneFor5, };

        Permuter<5> ptx5;
        do
        {
            for (OdomIter<AvCharList::iterator, 4> wheelset(wheelsFive);
                                                wheelset; wheelset.next()) {

                unsigned char single       = *wheelset[3] ;

                unsigned char twoHdr       = *wheelset[2];
                unsigned char twoContin    = u8ContinByteCases[1 + ptx5[0]];

                unsigned char threeHdr     = *wheelset[1];
                unsigned char threeContin1 = u8ContinByteCases[1 + ptx5[1]];
                unsigned char threeContin2 = u8ContinByteCases[1 + ptx5[2]];

                unsigned char fourHdr      = *wheelset[0];
                unsigned char fourContin1  = u8ContinByteCases[1 + ptx5[3]];
                unsigned char fourContin2  = u8ContinByteCases[8 - ptx5[1]];
                unsigned char fourContin3  = u8ContinByteCases[1 + ptx5[4]];

                // We fill and check a bunch of arrays.  It will get confusing.

                // First, we generate a UTF-8 string and store it in the
                // 'generator' array.  We store its UTF-16 counterpart in the
                // 'image' array.
                //
                // Then we call the 'oneStringConversion' templated function
                // with 'generator', 'image', a 'source' array and a 'dest'
                // array (not in that order, and the arrays are passed via
                // wrappers).  The 'oneStringConversion' templated function
                // will copy 'generator' to 'source', convert from 'source' to
                // 'dest', and then verify 'source' against 'generator'
                // (testing source array not changed) and 'dest' against
                // 'image' (testing conversion correct).
                //
                // If this works, it is repeated in reverse, using 'dest' in
                // place of 'generator' and 'generator' in place of 'image',
                // supplying an 'invSource' and 'invDest'.   (Where the first
                // call was UTF-8 to UTF-16, this will be UTF-16 to UTF-8, and
                // we will have compared the final result to the original
                // string stored in 'generator'.)

                char generator[MAX_NOCTETS];
                unsigned short image[MAX_NWORDS];

                for (int ic = 0;
                   ic < (int) (sizeof(u8CodingCases)/sizeof(u8CodingCases[0]));
                                                                        ++ic) {
                    int nSymbols = 0;

                    char* genp = generator;
                    unsigned short* imgp = image;

                    if (veryVeryVerbose) {
                        cout << "Base test case (a = single-octet, "
                                "b = two-octet...):" << endl
                             << "       [" << u8CodingCases[ic] << "]" << endl;
                    }

                    for (const char *ccase=u8CodingCases[ic]; *ccase; ++ccase)
                    {
                        // Create both the source and check arrays.
                        switch (*ccase) {
                          case 'a': {

                            // One-byte code point:
                            *genp++ = single;
                            *imgp++ = single;

                            ++nSymbols;

                          } break;
                          case 'b': {
                            // Two-byte code point:
                            *genp++ = static_cast<unsigned short>(
                                                      0xc0 | twoHdr);
                            *genp++ = static_cast<unsigned short>(
                                                      0x80 | twoContin);
                            *imgp++ = static_cast<unsigned short>(
                                                      twoHdr << 6 | twoContin);

                            ++nSymbols;

                          } break;
                          case 'c': {

                            // Three-byte code point:
                            *genp++ = static_cast<unsigned short>(
                                                          0xe0 | threeHdr);
                            *genp++ = static_cast<unsigned short>(
                                                          0x80 | threeContin1);
                            *genp++ = static_cast<unsigned short>(
                                                          0x80 | threeContin2);
                            *imgp++ = static_cast<unsigned short>(
                                            threeHdr << 12 | threeContin1 << 6
                                                               | threeContin2);

                            ++nSymbols;

                          } break;
                          case 'd': {

                            // four-byte code point:
                            *genp++ = 0xf0 | fourHdr;
                            *genp++ = 0x80 | fourContin1;
                            *genp++ = 0x80 | fourContin2;
                            *genp++ = 0x80 | fourContin3;
                            {
                              unsigned int isoChar = ( fourHdr << 18
                                                     | fourContin1 << 12
                                                     | fourContin2 << 6
                                                     | fourContin3 ) - 0x10000;
                              *imgp++ = static_cast<unsigned short>(
                                                   0xd800 |  isoChar >> 10);
                              *imgp++ = static_cast<unsigned short>(
                                                   0xdc00 | (isoChar & 0x3ff));
                            }

                            ++nSymbols;

                          } break;
                          default:
                            cerr << "Internal error in Test 5: "
                                    "Invalid case coding code point "
                                  << deChar(*ccase) << endl;
                            exit(1);
                        }
                    }

                    *genp++ = '\0';
                    *imgp++ = '\0';
                    ++nSymbols;

                    int nOctets = static_cast<int>(genp - generator);
                    int nWords  = static_cast<int>(imgp - image);

                    if (veryVerbose ) {

                        cout << "String " << R_(nSymbols) << R_(nOctets)
                             << R(nWords) << endl ;
                        if (veryVeryVerbose ) {
                            cout << prHexRange( generator, genp ) << endl;
                        }
                    }

                    // Convert 'generator' through 'source' to 'dest' and check
                    // against 'image'.

                    enum { BUFFER_ZONE = 32     // Margin to fill and check for
                                                // damage around output
                                                // buffers.
                    };

                    ArrayRange<char> genArray(generator, nOctets);
                    GenCheckArrRange<ArrayRange<char> > genGenCh(genArray);

                    ArrayRange<unsigned short> imageArray(image, nWords);
                    GenCheckArrRange<ArrayRange<unsigned short> >
                                                        imageGenCh(imageArray);

                    char source[MAX_NOCTETS];
                    ArrayRange<char> sourceArray(source, nOctets);

                    unsigned short dest[MAX_NWORDS + 2 * BUFFER_ZONE];
                    BufferedWPiece<unsigned short> bwp(dest,
                                                       MAX_NWORDS +
                                                               2 * BUFFER_ZONE,
                                                       nWords,
                                                       0xffff,
                                                       BUFFER_ZONE);

                    if (! oneStringConversion(__LINE__,
                                              bwp,
                                              imageGenCh,
                                              sourceArray,
                                              genGenCh,
                                              ConvRslt(0, nSymbols, nWords))) {
                        cout << "(Error converting UTF-8 to UTF-16.)" << endl;

                        if (veryVeryVerbose) {
                            cout << "Expected " << R(prHexRange(imageArray))
                                 << endl;
                        }

                        if (veryVerbose
                         && !veryVeryVerbose) {
                            // If veryVeryVerbose is set, we have already
                            //   printed this above.
                            cout << "Base test case (a = single-octet, "
                                    "b = two-octet...):" << endl
                                 << "       [" << u8CodingCases[ic] << "]"
                                 << endl;
                        }

                        if (verbose) {
                            cout << "Skipping remainder of test "
                                    "with this string." << endl;
                            continue;
                        }
                    }

                    // Use the content of the BufferedWPiece to define our
                    // backwards generator.
                    ArrayRange<unsigned short> invGenArray(bwp.begin(),
                                                           bwp.end());
                    GenCheckArrRange<ArrayRange<unsigned short> >
                                                        invGenGen(invGenArray);

                    unsigned short invSource[MAX_NWORDS];
                    ArrayRange<unsigned short> invSourceArray(invSource,
                                                              nWords);

                    char invDest[MAX_NOCTETS + 2 * BUFFER_ZONE];
                    BufferedWPiece<char> invBwp(invDest,
                                                MAX_NOCTETS + 2 * BUFFER_ZONE,
                                                nOctets,
                                                (char) 0xff,
                                                BUFFER_ZONE);

                    if (! oneStringConversion(__LINE__,
                                          invBwp,
                                          genGenCh,
                                          invSourceArray,
                                          invGenGen,
                                          ConvRslt(0, nSymbols, nOctets))) {
                        cout << "(Error converting UTF-16 to UTF-8.)" << endl;
                        if (veryVeryVerbose) {
                            cout << "Expected " << R(prHexRange(genArray))
                                 << endl;
                        }

                        if (veryVerbose
                         && !veryVeryVerbose) {
                            // If veryVeryVerbose is set, we have already
                            //   printed this above.
                            cout << "Base test case (a = single-octet, "
                                    "b = two-octet..." << endl
                                 << "       [" << u8CodingCases[ic] << "]"
                                 << endl;
                        }
                    }
                }
            }  // 'OdomIter' loop over header contents
        } while (ptx5.advance()); // 'Permuter' loop over continuation contents
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VARIOUS UTF-8 ERRORS
        //   ...
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // Note:
        //   This needed change.  The original code was so convoluted to
        //   achieve simple testing that it was decided to rewrite the tests
        //   from scratch.
        // --------------------------------------------------------------------

        {
            if (verbose) cout << "Disallowed Octets:\n";

            const unsigned char DISALLOWED[] = {
                0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 0 };

            for (const unsigned char *pc = DISALLOWED; *pc; ++pc) {
                char input[] = { "abcd" };
                unsigned short output[10];

                bsl::size_t nc, nw;

                input[1] = *pc;
                int rc = Util::utf8ToUtf16(output,
                                           sizeof(output),
                                           input,
                                           &nc,
                                           &nw,
                                           0);
                LOOP2_ASSERT(pc - DISALLOWED, rc, 1 == rc);
                LOOP2_ASSERT(pc - DISALLOWED, nc, 4 == nc);
                LOOP2_ASSERT(pc - DISALLOWED, nw, 4 == nw);
            }
        }

        {
            if (verbose) cout << "Continuation Octets out of place:\n";

            for (int i = 0; i < 64; ++i) {
                char input[] = { "abcd" };
                unsigned short output[10];

                bsl::size_t nc, nw;

                input[1] = static_cast<char>(0x80 + i);
                int rc = Util::utf8ToUtf16(output,
                                           sizeof(output),
                                           input,
                                           &nc,
                                           &nw,
                                           0);
                ASSERT(1 == rc);
                ASSERT(4 == nc);
                ASSERT(4 == nw);
            }
        }

        {
            if (verbose) cout << "Continuation Octets out of place:\n";

            for (int i = 0; i < 64; ++i) {
                char input[] = { "abcd" };
                unsigned short output[10];

                bsl::size_t nc, nw;

                input[1] = static_cast<char>(0x80 + i);
                int rc = Util::utf8ToUtf16(output,
                                           sizeof(output),
                                           input,
                                           &nc,
                                           &nw,
                                           0);
                LOOP_ASSERT(rc, 1 == rc);
                LOOP_ASSERT(nc, 4 == nc);
                LOOP_ASSERT(nw, 4 == nw);
            }
        }

        // The above code covers all cases covered below

        if (verbose) cout << "Missing continuation octets\n";

        {
            unsigned char HEADERS[] = { 0, 0, 0xc3, 0xe1, 0xf1 };

            for (unsigned h = 2; h <= 4; ++h) {
                for (unsigned c = 0; c < h-1; ++c) {
                    char input[] = { "abcde" };
                    unsigned short output[20];

                    bsl::size_t nc, nw;

                    input[0] = HEADERS[h];
                    for (unsigned i = 0; i < c; ++i) {
                        input[1 + i] = (char) 0xbf;
                    }

                    int rc = Util::utf8ToUtf16(output,
                                               sizeof(output),
                                               input,
                                               &nc,
                                               &nw,
                                               0);
                    ASSERT(1 == rc);
                    ASSERT(6 - 1 - c == nc);
                    ASSERT(6 - 1 - c == nw);
                    ASSERT((unsigned) input[1 + c] == output[0]);

                    rc = Util::utf8ToUtf16(output,
                                           sizeof(output),
                                           bsl::string_view(input),
                                           &nc,
                                           &nw,
                                           0);
                    ASSERT(1 == rc);
                    ASSERT(6 - 1 - c == nc);
                    ASSERT(6 - 1 - c == nw);
                    ASSERT((unsigned) input[1 + c] == output[0]);
                }
            }
        }

        if (verbose) {
            cout << "\nTest 4 complete." << endl;
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONVERT UTF-8 TO UTF-16 and UTF-16 to UTF-8 in strings.
        //
        // Concerns:
        //   Ability to convert UTF-8 to UTF-16 correctly.  This test uses
        //   runs of code points, each of the same size.  Mixing sizes and
        //   introducing errors come in higher level tests.
        //
        // Plan:
        //
        // Testing:
        //   utf8ToUtf16
        //   utf16ToUtf8
        // --------------------------------------------------------------------

        int nEightToSixteen = 0;
        int nSixteenToEight = 0;
            // Totals kept on code points processed.  We'll check that we have
            // done enough to cover the whole range.

        enum { BUFFER_ZONE = 128   // These tests create a space of BUFFER_ZONE
                                   // memory units (bytes or words) around the
                                   // output string space and fill it before
                                   // testing the conversion.  They check
                                   // afterwards that the values stored are
                                   // still present.  Making this larger slows
                                   // the tests.
        };

        // Test 3a1: UTF-8 => UTF-16, one-octet code points.  A string of all
        // legal one-octet code points is converted from UTF-8 to UTF-16 and
        // checked.

        if (verbose) {
            cout << "\nTest 3a1: UTF-8 => UTF-16, single-octet"
                 << "\n=======================================" << endl ;
        }

        {
            typedef BufferSizes<127,    // Source code points
                                1,      // Source char size
                                1,      // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            char           u8[Sizes::FROM_BUF_SIZE];
            unsigned short u16[Sizes::TO_BUF_SIZE];

            // Create a code point string with all legal single-octet u8
            // code points.

            for (unsigned u8c = 1 ; u8c < 0x80 ; ++u8c ) {
                u8[u8c - 1] = static_cast<char>(u8c);
                ++nEightToSixteen;
            }

            u8[Sizes::FROM_BUF_SIZE - 1] = 0;

            SrcSpec<char> source(u8, 0, Sizes::N_CHARS + 1);
            ConvRslt expected(0, 128, 128);  // 128 symbols, 128 words copied.

            WorkPiece<unsigned short> wp(Sizes::TO_BUF_SIZE,
                                         source.d_dstBufSize,
                                         0xffff,
                                         BUFFER_ZONE);
            ConvRslt result;

            if (RUN_AND_CHECK(wp, u16, result, u8ToU16, source, expected)) {
                for (unsigned u8c = 1; u8c < 0x80; ++u8c) {
                    if (! EXPECTED_GOT(u8c,wp.begin(u16)[u8c - 1])) {
                        cout << "\tat "; P(u8c - 1);
                    }
                    if (! EXPECTED_GOT(u8c,
                                       (unsigned) u8[u8c - 1])) {
                        cout << "\tSource string damaged at "; P(u8c - 1);
                    }
                }

                if (! EXPECTED_GOT('\0', u8[0x80 - 1])) {
                    cout << "\tNull code point missing at end of source"
                            " string.\n";
                }
            }
        }

        if (verbose) {
            cout << "\nTest 3a2: UTF-8 => UTF-16, two-octet"
                 << "\n====================================" << endl ;
        }

        {
            typedef BufferSizes<0x800 - 0x80,   // Source code points
                                2,              // Source char size
                                1,              // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            enum { TWO_OCTET_POW_TWO = 1 << 11  // Five bits in the header
                                                // octet, six in the
                                                // continuation.
            };

            char           u8[Sizes::FROM_BUF_SIZE];
            unsigned short u16[Sizes::TO_BUF_SIZE];

            // Create a code point string with all legal two-octet u8
            // code points.

            for (unsigned u8c = 0x80 ; u8c < 0x800 ; ++u8c ) {
                unsigned pos = u8c - 128;
                u8[2 * pos] =     static_cast<char>(0xc0 | (u8c >> 6));
                u8[2 * pos + 1] = static_cast<char>(0x80 | (u8c & 0x3f));
                    // Extra parentheses here to silence g++ warning.
                ++nEightToSixteen;
            }

            u8[2 * (0x800 - 0x80)] = 0;
// cout << prHexRange(u8, 0x800 - 0x80 + 1) << endl ;

            SrcSpec<char> source(u8, 0, Sizes::N_CHARS + 1);
            ConvRslt expected(0, Sizes::N_CHARS + 1, Sizes::N_CHARS + 1);
                                            // Same # of symbols and words ...

            WorkPiece<unsigned short> wp(Sizes::TO_BUF_SIZE,
                                         source.d_dstBufSize,
                                         0xffff,
                                         BUFFER_ZONE);
            ConvRslt result;

            if (RUN_AND_CHECK(wp, u16, result, u8ToU16, source, expected)) {
                for (unsigned u8c = 0x80; u8c < 0x800; ++u8c) {
                    if (! EXPECTED_GOT(u8c, wp.begin(u16)[u8c - 0x80])) {
                        cout << "\tat "; P(u8c - 0x80);
                    }
                }

                // Verify that the source is undamaged.
                for (unsigned u8c = 0x80 ; u8c < 0x800 ; ++u8c ) {
                    unsigned pos = u8c - 0x80;
                    if (! EXPECTED_GOT(0xc0 | (u8c >> 6),
                                       deChar(u8[2 * pos]))) {
                        cout << "\tat "; P(u8c);
                    }
                    if (! EXPECTED_GOT(0x80 | (u8c & 0x3f),
                                       deChar(u8[2 * pos + 1]))) {
                        cout << "\tat "; P(u8c);
                    }
                }

                if (! EXPECTED_GOT(0, deChar(u8[2 * (0x800 - 0x80)]))) {
                    cout << "\tat "; P(2 * (0x800 - 80));
                }
            }
        }

        if (verbose) {
            cout << "\nTest 3a3: UTF-8 => UTF-16, three-octet"
                 << "\n======================================" << endl ;
        }

        {
            enum { THREE_OCTET_POW_TWO = 1 << 16,  // Four bits in the header
                                                   // octet, six in each
                                                   // continuation
                   THREE_OCTET_LIM = 1 << 4,       // Four content bits in a
                                                   // three-octet header
                   CONTIN_LIM = 1 << 6             // Six content bits in a
                                                   // continuation octet
            };
            typedef BufferSizes<CONTIN_LIM * CONTIN_LIM,   // Source code
                                                           // points
                                3,                         // Source char size
                                1,                         // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            char           u8[Sizes::FROM_BUF_SIZE];
            unsigned short u16[Sizes::TO_BUF_SIZE];

            for (unsigned iFirst = 0x0 ; iFirst < THREE_OCTET_LIM ; ++iFirst) {
                // With zero in the header, we have to stay above what a two-
                // octet coding can handle; with 0xd in the header, we need to
                // stay below 0x20 where the reserved range sits.  (16-bit
                // patterns in that range are the upper and lower halves of
                // two-word codings.)

                unsigned rangeStart = (0x0 == iFirst) ? 0x20  : 0x0 ;
                unsigned rangeLimit = (0xd == iFirst) ? 0x20 : CONTIN_LIM ;

                int pos = 0;
                for (unsigned iSecond = rangeStart ; iSecond < rangeLimit ;
                                                                ++iSecond) {
                    for (unsigned iThird = 0x0 ; iThird < CONTIN_LIM ;
                                                                    ++iThird) {
                        u8[pos++] = static_cast<char>(0xe0 | iFirst);
                        u8[pos++] = static_cast<char>(0x80 | iSecond);
                        u8[pos++] = static_cast<char>(0x80 | iThird);

                        ++nEightToSixteen;
                    }
                }

                u8[pos] = 0;
// cout << R_(iFirst) << prHexRange(u8, pos) << endl ;

                int nchar = pos / 3 + 1; // All the code points we built, plus
                                         // the null.  Note that all the 16-bit
                                         // code points will be single-word.

                SrcSpec<char> source(u8, 0, nchar);
                ConvRslt expected(0, nchar, nchar);

                WorkPiece<unsigned short> wp(Sizes::TO_BUF_SIZE,
                                          source.d_dstBufSize,
                                          0xffff,
                                          BUFFER_ZONE);
                ConvRslt result;

                if (! RUN_AND_CHECK(wp,
                                    u16,
                                    result,
                                    u8ToU16,
                                    source,
                                    expected)) {
                    cout << "\t" << R(iFirst) << endl ;
                }
                else {
                    pos = 0;
                    for (unsigned iSecond = rangeStart ; iSecond < rangeLimit;
                                                                ++iSecond) {
                        for (unsigned iThird = 0x0 ; iThird < CONTIN_LIM;
                                                                    ++iThird) {
                            unsigned val = iFirst << 12
                                         | iSecond << 6
                                         | iThird;
                            unsigned short at = static_cast<unsigned short>(
                                                                        pos++);

                            if (! EXPECTED_GOT(val, wp.begin(u16)[at])) {
                                cout << R_(iFirst) << R_(iSecond)
                                     << R_(iThird) << R(at) << endl;
                            }

                            if (! EXPECTED_GOT(0xe0 | iFirst,
                                               deChar(u8[3 * at + 0]))) {
                                cout << "\tdamaged source code point at"
                                     << 3 * pos + 0 << endl;
                            }

                            if (! EXPECTED_GOT(0x80 | iSecond,
                                               deChar(u8[3 * at + 1]))) {
                                cout << "\tdamaged source code point at"
                                     << 3 * pos + 1 << endl;
                            }

                            if (! EXPECTED_GOT(0x80 | iThird,
                                               deChar(u8[3 * at + 2]))) {
                                cout << "\tdamaged source code point at"
                                     << 3 * pos + 2 << endl;
                            }
                        }
                    }

                    if (! EXPECTED_GOT(0, deChar(u8[3 * pos]))) {
                        cout << "\tat " << 3 * pos << endl;
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTest 3a4: UTF-8 => UTF-16, four-octet"
                 << "\n=====================================" << endl ;
        }

        {
            enum { FOUR_OCTET_POW_TWO = 1 << 21,  // Three bits in the header
                                                   // octet, six in each
                                                   // continuation
                   CONTIN_LIM = 1 << 6             // Six content bits in a
                                                   // continuation octet
            };

            typedef BufferSizes<CONTIN_LIM * CONTIN_LIM,  // Source code points
                                4,                        // Source char size
                                2,                        // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

// The header octet is zero through four.  With the value four, the first
// continuation is limited to < 1 << 4 .  With the value zero, the first
// continuation must be >= 0x10.
            char           u8[Sizes::FROM_BUF_SIZE];
            unsigned short u16[Sizes::TO_BUF_SIZE];

            for (unsigned iFirst = 0x0; iFirst <= 0x4; ++iFirst) {
                unsigned rangeStart = (0x0 == iFirst) ? 0x10  : 0x0;
                unsigned rangeLimit = (0x4 == iFirst) ? 0x10 : CONTIN_LIM;

                for (unsigned iSecond = rangeStart; iSecond < rangeLimit;
                                                                ++iSecond) {
                    int pos = 0;
                    for (unsigned iThird = 0x0; iThird < CONTIN_LIM;
                                                                ++iThird) {
                        for (unsigned iFourth = 0x0 ; iFourth < CONTIN_LIM;
                                                                   ++iFourth) {
                            u8[pos++] = static_cast<char>(0xf0 | iFirst);
                            u8[pos++] = static_cast<char>(0x80 | iSecond);
                            u8[pos++] = static_cast<char>(0x80 | iThird);
                            u8[pos++] = static_cast<char>(0x80 | iFourth);

                            ++nEightToSixteen;
                        }
                    }

                    u8[pos] = 0;

                    int nchar = pos / 4 + 1; // All the code points we built,
                                             // plus the null.  All our
                                             // code points, except for the
                                             // null, will require two words.
                    SrcSpec<char> source(u8, 0, nchar * 2 - 1);
                    ConvRslt expected(0, nchar, nchar * 2 - 1); // The null is
                                                                // still just
                                                                // one word.

                    WorkPiece<unsigned short> wp(Sizes::FROM_BUF_SIZE,
                                                 source.d_dstBufSize,
                                                 0xffff,
                                                 BUFFER_ZONE);
                    ConvRslt result;

                    if (! RUN_AND_CHECK(wp,
                                       u16,
                                       result,
                                       u8ToU16,
                                       source,
                                       expected)) {
                        cout << "\t" << R(iFirst) << endl ;
                    }
                    else {
                        unsigned at = 0;
                        for (unsigned iThird = 0x0 ; iThird < CONTIN_LIM;
                                                                    ++iThird) {
                            for (unsigned iFourth = 0x0 ; iFourth < CONTIN_LIM;
                                                                ++iFourth) {
                                unsigned val = iFirst << 18
                                             | iSecond << 12
                                             | iThird << 6
                                             | iFourth;
                                cout << hex;
                                if (! EXPECTED_GOT(
                                        ((val - 0x10000) >> 10) | 0xd800,
                                            // Needless parentheses here to
                                            // silence g++ warning.
                                        wp.begin(u16)[at])) {
                                    cout << R_(iFirst) << R_(iSecond)
                                         << R_(iThird) << R_(iFourth)
                                         << R(at * 2) << endl;
                                    cout << R_(val) << R(val - 0x10000)
                                         << endl;
                                }
                                if (! EXPECTED_GOT(
                                        ((val - 0x10000) & 0x3ff) | 0xdc00,
                                        wp.begin(u16)[at + 1])) {
                                    cout << R_(iFirst) << R_(iSecond)
                                         << R_(iThird) << R_(iFourth)
                                         << R(at * 2 + 1) << endl;
                                    cout << R_(val) << R(val - 0x10000)
                                         << endl;
                                }

                                if (! EXPECTED_GOT(0xf0 | iFirst,
                                                   deChar(u8[2 * at + 0]))) {
                                    cout << "\tdamaged source code point at"
                                         << 2 * at + 0 << endl;
                                }

                                if (! EXPECTED_GOT(0x80 | iSecond,
                                                   deChar(u8[2 * at + 1]))) {
                                    cout << "\tdamaged source code point at"
                                         << 2 * at + 1 << endl;
                                }

                                if (! EXPECTED_GOT(0x80 | iThird,
                                                   deChar(u8[2 * at + 2]))) {
                                    cout << "\tdamaged source code point at"
                                         << 2 * at + 2 << endl;
                                }

                                if (! EXPECTED_GOT(0x80 | iFourth,
                                                   deChar(u8[2 * at + 3]))) {
                                    cout << "\tdamaged source code point at"
                                         << 2 * at + 3 << endl;
                                }

                                cout << dec;
                                at += 2;
                            }
                        }

                        if (! EXPECTED_GOT(0, deChar(u8[2 * at]))) {
                            cout << "\tdamaged source code point at"
                                 << 2 * at << endl;
                        }
                    }
                }
            }
        }

        // Test 3b1: UTF-16 => UTF-8, one-octet code points.

        if (verbose) {
            cout << "\nTest 3b1: UTF-16 => UTF-8, single-octet"
                 << "\n=======================================" << endl ;
        }

        {
            typedef BufferSizes<127,    // Source code points
                                1,      // Source char size
                                1,      // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            unsigned short u16[Sizes::FROM_BUF_SIZE];
            char           u8[Sizes::TO_BUF_SIZE];

            // Create a code point string with all legal single-octet u8
            // code points.

            for (unsigned u16c = 1 ; u16c < 0x80 ; ++u16c ) {
                u16[u16c - 1] = static_cast<unsigned short>(u16c);
                ++nSixteenToEight;
            }
// cout << prHexRange(u16, sizeof(u16)/sizeof(u16[0])) << endl ;
            u16[127] = 0;

            SrcSpec<unsigned short> source(u16, 0, Sizes::N_CHARS + 1);
            ConvRslt expected(0, 128, 128);  // 128 symbols, 128 octets copied.

            WorkPiece<char> wp(Sizes::TO_BUF_SIZE,
                               source.d_dstBufSize,
                               (char) 0xff,
                               BUFFER_ZONE);
            ConvRslt result;

            if (RUN_AND_CHECK(wp, u8, result, u16ToU8, source, expected)) {
// cout << prHexRange(wp.begin(u8), wp.end(u8)) << endl ;
                for (unsigned u16c = 1; u16c < 0x80; ++u16c) {
                    if (! EXPECTED_GOT(u16c,
                                       (unsigned) wp.begin(u8)[u16c - 1])) {
                        cout << "\tat "; P(u16c - 1);
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTest 3b2: UTF-16 => UTF-8, two-octet"
                 << "\n====================================" << endl ;
        }

        {
            typedef BufferSizes<0x800 - 0x80,   // Source code points
                                1,              // Source char size
                                2,              // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            enum { TWO_OCTET_POW_TWO = 1 << 11  // Five bits in the header
                                                // octet, six in the
                                                // continuation.
            };

// cout << "3b2 " << R_(0x800 - 0x80) << R_(Sizes::TO_BUF_SIZE)
//      << R(Sizes::FROM_BUF_SIZE) << endl ;
            unsigned short u16[Sizes::FROM_BUF_SIZE];
            char           u8[Sizes::TO_BUF_SIZE];

            for (unsigned u16c = 0x80 ; u16c < 0x800 ; ++u16c ) {
                 u16[u16c - 0x80] = static_cast<unsigned short>(u16c);
                ++nSixteenToEight;
            }
// enum for the 0x800 - 127 constant
            u16[0x800 - 0x80] = 0;
// cout << prHexRange(u16, 0x800 - 0x80 + 1) << endl ;

            SrcSpec<unsigned short> source(u16, 0, 2 * Sizes::N_CHARS + 1);
            ConvRslt expected(0, Sizes::N_CHARS + 1, 2 * Sizes::N_CHARS + 1);
                // Excluding the null, twice as many octets as symbols

            WorkPiece<char> wp(Sizes::TO_BUF_SIZE,
                               source.d_dstBufSize,
                               (char) 0xff,
                               BUFFER_ZONE);
            ConvRslt result;

            if (RUN_AND_CHECK(wp, u8, result, u16ToU8, source, expected)) {
                for (unsigned u8c = 0x80; u8c < 0x800; ++u8c) {
                    unsigned at = (u8c - 0x80) * 2;

                    if (! EXPECTED_GOT(deChar(wp.begin(u8)[at]),
                                       0xc0 | u8c >> 6)) {
                        cout << "\tat "; P(u8c - 0x80);
                    }
                    if (! EXPECTED_GOT(deChar(wp.begin(u8)[at + 1]),
                                       0x80 | (u8c & 0x3f))) {
                        cout << "\tat "; P(u8c - 0x80);
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTest 3b3: UTF-16 => UTF-8, three-octet"
                 << "\n======================================" << endl ;
        }

        {
            enum { THREE_OCTET_POW_TWO = 1 << 16,  // Four bits in the header
                                                   // octet, six in each
                                                   // continuation
                   THREE_OCTET_LIM = 1 << 4,       // Four content bits in a
                                                   // three-octet header
                   CONTIN_LIM = 1 << 6             // Six content bits in a
                                                   // continuation octet
            };
            typedef BufferSizes<CONTIN_LIM * CONTIN_LIM,   // Source code
                                                           // points
                                1,                         // Source char size
                                3,                         // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            unsigned short u16[Sizes::FROM_BUF_SIZE];
            char           u8[Sizes::TO_BUF_SIZE];

            for (unsigned iFirst = 0x0 ; iFirst < THREE_OCTET_LIM ; ++iFirst) {
                // With zero in the header, we have to stay above what a two-
                // octet coding can handle; with 0xd in the header, we need to
                // stay below 0x20 where the reserved range sits.  (16-bit
                // patterns in that range are the upper and lower halves of
                // two-word codings.)

                unsigned rangeStart = (0x0 == iFirst) ? 0x20  : 0x0 ;
                unsigned rangeLimit = (0xd == iFirst) ? 0x20 : CONTIN_LIM ;

// cout << hex << R_(iFirst) << R_(rangeStart) << R(rangeLimit) << dec << endl;
                int pos = 0;
                for (unsigned iSecond = rangeStart ; iSecond < rangeLimit ;
                                                                ++iSecond) {
                    for (unsigned iThird = 0x0 ; iThird < CONTIN_LIM ;
                                                                    ++iThird) {
// cout << hex << R_(iSecond) << R_(iThird) << R(pos) << dec << endl ;
                        u16[pos++] = static_cast<unsigned short>(
                                                           deChar(iFirst) << 12
                                                         | deChar(iSecond) << 6
                                                         | deChar(iThird));

                        ++nSixteenToEight;
                    }
                }

                u16[pos] = 0;

                int nchar = pos + 1; // All the code points we built, plus the
                                     // null.  Note that all the 16-bit code
                                     // points will be single-word.

                SrcSpec<unsigned short> source(u16, 0, 3 * pos + 1);
                ConvRslt expected(0, nchar, pos * 3 + 1);

                WorkPiece<char> wp(Sizes::TO_BUF_SIZE,
                                source.d_dstBufSize,
                                (char) 0xff,
                                BUFFER_ZONE);
                ConvRslt result;

                if (! RUN_AND_CHECK(wp,
                                   u8,
                                   result,
                                   u16ToU8,
                                   source,
                                   expected)) {
                    cout << "\t" << R(iFirst) << endl ;
                }
                else {
                    pos = 0;

                    for (unsigned iSecond = rangeStart ; iSecond < rangeLimit ;
                                                                   ++iSecond) {
                        for (unsigned iThird = 0x0 ; iThird < CONTIN_LIM ;
                                                                    ++iThird) {
// cout << hex << R_(iSecond) << R_(iThird) << R(pos) << dec << endl ;
                            if (! EXPECTED_GOT(0xe0 | iFirst,
                                               deChar(wp.begin(u8)[pos]))) {
                                cout << "\tat " << R_(iFirst) << R_(iSecond)
                                     << R(iThird) << endl;
                            }
                            if (! EXPECTED_GOT(0x80 | iSecond,
                                              deChar(wp.begin(u8)[pos + 1]))) {
                                cout << "\tat " << R_(iFirst) << R_(iSecond)
                                     << R(iThird) << endl;
                            }
                            if (! EXPECTED_GOT(0x80 | iThird,
                                              deChar(wp.begin(u8)[pos + 2]))) {
                                cout << "\tat " << R_(iFirst) << R_(iSecond)
                                     << R(iThird) << endl;
                            }

                            pos += 3;
                        }
                    }

// cout << R(iFirst) << " " << prHexRange(u8, pos) << endl ;
                }
            }
        }

        if (verbose) {
            cout << "\nTest 3b4: UTF-16 => UTF-8, four-octet"
                 << "\n=====================================" << endl ;
        }

        {
            enum { FOUR_OCTET_POW_TWO = 1 << 21,  // Three bits in the header
                                                   // octet, six in each
                                                   // continuation
                   CONTIN_LIM = 1 << 6             // Six content bits in a
                                                   // continuation octet
            };

            typedef BufferSizes<CONTIN_LIM * CONTIN_LIM,  // Source code points
                                2,                        // Source char size
                                4,                        // Dest char size
                                BUFFER_ZONE> Sizes;

            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

// The header octet is zero through four.  With the value four, the first
// continuation is limited to < 1 << 4 .  With the value zero, the first
// continuation must be >= 0x10.
            unsigned short u16[Sizes::FROM_BUF_SIZE];
            char           u8[Sizes::TO_BUF_SIZE];

            for (unsigned iFirst = 0x0; iFirst <= 0x4; ++iFirst) {
                unsigned rangeStart = (0x0 == iFirst) ? 0x10  : 0x0;
                unsigned rangeLimit = (0x4 == iFirst) ? 0x10 : CONTIN_LIM;

                for (unsigned iSecond = rangeStart; iSecond < rangeLimit;
                                                                ++iSecond) {
                    int pos = 0;
                    for (unsigned iThird = 0x0; iThird < CONTIN_LIM;
                                                                ++iThird) {
                        for (unsigned iFourth = 0x0 ; iFourth < CONTIN_LIM;
                                                                   ++iFourth) {
// cout << hex << R_(iThird) << R_(iFourth) << R(pos) << dec << endl ;
                            unsigned long convBuf = ((iFirst << 18)
                                                   | (iSecond << 12)
                                                   | (iThird << 6)
                                                   | iFourth) - 0x10000;
                            u16[pos++] = static_cast<unsigned short>(
                                                   0xd800 | (convBuf >> 10));
                            u16[pos++] = static_cast<unsigned short>(
                                                   0xdc00 | (convBuf & 0x3ff));

                            ++nSixteenToEight;
                        }
                    }

                    u16[pos] = 0;
// cout << R_(iFirst) << prHexRange(u8, pos) << endl ;

                    int nchar = pos / 2 + 1; // All the code points we built,
                                             // plus the null.  All our
                                             // code points, except for the
                                             // null, will require two words.
                    SrcSpec<unsigned short> source(u16,
                                                   0,
                                                   4 * Sizes::N_CHARS + 1);
                    ConvRslt expected(0, nchar, pos * 2 + 1); // The null is
                                                              // still just
                                                              // one word.

                    WorkPiece<char> wp(Sizes::TO_BUF_SIZE,
                                       source.d_dstBufSize,
                                       (char) 0xff,
                                       BUFFER_ZONE);
                    ConvRslt result;

                    if (! RUN_AND_CHECK(wp,
                                        u8,
                                        result,
                                        u16ToU8,
                                        source,
                                        expected)) {
                        cout << "\t" << R(iFirst) << endl ;
                    }
                    else {
                        unsigned at = 0;
                        for (unsigned iThird = 0x0 ; iThird < CONTIN_LIM;
                                                                    ++iThird) {
                            for (unsigned iFourth = 0x0 ; iFourth < CONTIN_LIM;
                                                                ++iFourth) {
                                cout << hex;
                                if (!EXPECTED_GOT(deChar(wp.begin(u8)[at]),
                                                   0xf0 | iFirst)) {
                                    cout << R_(iFirst) << R_(iSecond)
                                         << R_(iThird) << R_(iFourth)
                                         << R(at * 2) << endl;
                                }
                                if (!EXPECTED_GOT(deChar(wp.begin(u8)[at + 1]),
                                                   0x80 | iSecond)) {
                                    cout << R_(iFirst) << R_(iSecond)
                                         << R_(iThird) << R_(iFourth)
                                         << R(at * 2) << endl;
                                }
                                if (!EXPECTED_GOT(deChar(wp.begin(u8)[at + 2]),
                                                   0x80 | iThird)) {
                                    cout << R_(iFirst) << R_(iSecond)
                                         << R_(iThird) << R_(iFourth)
                                         << R(at * 2) << endl;
                                }
                                if (!EXPECTED_GOT(deChar(wp.begin(u8)[at + 3]),
                                                   0x80 | iFourth)) {
                                    cout << R_(iFirst) << R_(iSecond)
                                         << R_(iThird) << R_(iFourth)
                                         << R(at * 2) << endl;
                                }
                                cout << dec;

                                at += 4;
                            }
                        }
                    }
                }
            }
        }

        if (verbose) {
            cout << "Non-nulls converted: " << R(nEightToSixteen)
                                    << "; " << R(nSixteenToEight) << endl;
        }

        if (! EXPECTED_GOT(nEightToSixteen, (0x110000 - 1 - 0x800))
           && verbose) {
            cout << "\t(Not enough UTF-8 to UTF-16 conversions to "
                    "cover the whole range.)" << endl;
        }

        if (! EXPECTED_GOT(nSixteenToEight, (0x110000 - 1 - 0x800))
           && verbose) {
            cout << "\t(Not enough UTF-16 to UTF-8 conversions to "
                    "cover the whole range.)" << endl;
        }

        if (verbose) {
            cout << "Test 3 complete." << endl;
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SINGLE-VALUE, LEGAL VALUE TEST
        //
        // Concerns:
        //   - That the conversion functions do not overwrite memory
        //     adjacent to or nearby the output buffer: (1).
        //   - That the conversion functions properly null-terminate the string
        //     in the output buffer whenever there is room to do so: (4).
        //   - That the conversion functions return the correct values,
        //     both by the value of the expression and via the code point
        //     count and byte/word count pointer parameters: (2)
        //   - That the conversion functions can correctly handle all single
        //     code point values (whether single- or multi-byte/word): (5)
        //
        // Plan:
        //   For UTF-8 to UTF-16 and for UTF-16 to UTF-8, generate all legal
        //   code points (in the iso10646 domain supported by UTF-8 and UTF-16)
        //   one at a time.  Place each code point (of however many
        //   bytes/words) in a source string and apply the conversion
        //   functions.  Convert the string.  Verify the converted output as
        //   well as the return values generated.  (This will necessarily
        //   verify that each input code point's encoding was correctly
        //   recognized, and that each output code point received the correct
        //   encoding).
        //
        // Note:
        //
        // Testing:
        //   ............
        // --------------------------------------------------------------------
        // Run intense testing on a selected group of code points.

        enum { BUFFER_ZONE = 32,   // These tests create a space of BUFFER_ZONE
                                   // memory units (bytes or words) around the
                                   // output string space and fill it before
                                   // testing the conversion.  They check
                                   // afterwards that the values stored are
                                   // still present.  Making this larger slows
                                   // the tests.

// TBD --- Change this over to use the BufferSize<> stuff!

               WORKPAD_SIZE = 256  // WORKPAD_SIZE is the size of the output
                                   // string buffer for this group of tests.
                                   // It must be at least twice the size of
                                   // the margin plus the size of the text to
                                   // be converted (including the null, three
                                   // words for UTF-16 or five bytes for
                                   // UTF-8.)  It could be computed as a
                                   // parameter, but choosing a power of two
                                   // might just make things run a hair faster.
                                   // It's not enough space to worry about, but
                                   // this relationship should be checked
                                   // (below).
        };

        if (verbose) {
            cout << "\nTest 2a1: UTF-8 => UTF-16, selected "
                    "single-octet code points individually, four ways."
                 << "\n===================================="
                    "===========================================" << endl;
        }

        // Subtest 2a1: UTF-8 => UTF-16, single byte
        {
            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            char           u8[5];
            unsigned short u16[4][WORKPAD_SIZE];

            AvCharList casesOne(u8OneByteCases);

            for (AvCharList::iterator c1i = casesOne.begin();
                                           casesOne.end() != c1i ; ++c1i ) {
                u8[0] = *c1i;
                u8[1] = 0;

                SrcSpec<char> source(u8, 0, 2);
                ConvRslt expected(0, 2, 2);

                FourWayRunner<unsigned short, char> runner(u16,
                                                  0xffff,
                                                  BUFFER_ZONE,
                                                  source,
                                                  u8ToU16,
                                                  expected);
                RUN_FOUR_WAYS(runner);
                EXPECTED_GOT(*c1i, runner.begin(0)[0]);
            }
        }

// cout << "Single-octet cases done." << endl ;

        if (verbose) {
            cout << "\nTest 2a2: UTF-8 => UTF-16, selected "
                    "two-octet code points individually, four ways."
                 << "\n===================================="
                 << "========================================" << endl;
        }

        // Subtest 2a2: UTF-8 => UTF-16, two byte
        {
            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            char           u8[5];
            unsigned short u16[4][WORKPAD_SIZE];

            AvCharList casesTwo(u8TwoByteHdrCases);
            AvCharList casesContin(u8ContinByteCases);

            AvCharList *outerIters[2] = { &casesTwo, &casesContin };
            OdomIter<AvCharList::iterator, 2> c2C(outerIters);

            for ( ; c2C ; c2C.next() ) {
                AvCharList::iterator c2i = c2C[0];
                AvCharList::iterator cCi = c2C[1];

                u8[0] = *c2i | 0xc0;
                u8[1] = *cCi | 0x80;
                u8[2] = 0;

                SrcSpec<char> source(u8, 0, 2);
                ConvRslt expected(0, 2, 2);

                FourWayRunner<unsigned short, char> runner(u16,
                                                           0xffff,
                                                           BUFFER_ZONE,
                                                           source,
                                                           u8ToU16,
                                                           expected);
                RUN_FOUR_WAYS(runner);

                unsigned int high = deChar(*c2i);
                unsigned int low = deChar(*cCi);

                EXPECTED_GOT((high << 6 | low), runner.begin(0)[0]);
            }
        }
// cout << "Two-octet cases done." << endl ;

        if (verbose) {
            cout << "\nTest 2a3: UTF-8 => UTF-16, selected "
                    "three-octet code points individually, four ways."
                 << "\n===================================="
                 << "==========================================" << endl;
        }

        // Subtest 2a3: UTF-8 => UTF-16, three byte
        {
            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            char           u8[5];
            unsigned short u16[4][WORKPAD_SIZE];

            AvCharList casesThree(u8ThreeByteHdrCases);
            AvCharList casesContin(u8ContinByteCases);

            AvCharList *outerIters[2] = { &casesThree, &casesContin };
            OdomIter<AvCharList::iterator, 2> c3C2(outerIters);

            for ( ; c3C2 ; c3C2.next() ) {
                AvCharList::iterator c3i = c3C2[0];
                AvCharList::iterator cC2i = c3C2[1];

                if (0 == *c3i ) {
                    if (*cC2i < THREE_BYTE_ZERO_NEEDS) {
// cout << hex << "skipping " << deChar(*c3i) << ", "
//      << deChar(*cC2i) << ", *" << dec << endl ;
                        continue;
                    }
// cout << hex << "doing " << deChar(*c3i) << ", "
//      << deChar(*cC2i) << ", *" << dec << endl ;
                }

                for (AvCharList::iterator cC3i = casesContin.begin();
                                                casesContin.end() != cC3i;
                                                               ++cC3i ) {
                    u8[0] = *c3i | 0xe0;
                    u8[1] = *cC2i | 0x80;
                    u8[2] = *cC3i | 0x80;
                    u8[3] = 0;

                    SrcSpec<char> source(u8, 0, 2);
                    ConvRslt expected(0, 2, 2);

                    FourWayRunner<unsigned short, char> runner(u16,
                                                               0xffff,
                                                               BUFFER_ZONE,
                                                               source,
                                                               u8ToU16,
                                                               expected);
                    RUN_FOUR_WAYS(runner);

                    unsigned int high = deChar(*c3i);
                    unsigned int mid = deChar(*cC2i);
                    unsigned int low = deChar(*cC3i);

                    EXPECTED_GOT((high << 12 | mid << 6 | low),
                                 runner.begin(0)[0]);
                }
            }
        }
// cout << "Three-octet cases done." << endl ;

        if (verbose) {
            cout << "\nTest 2a4: UTF-8 => UTF-16, selected "
                    "four-octet code points individually, four ways."
                 << "\n===================================="
                    "=========================================" << endl;
        }

        {
            Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

            char           u8[5];
            unsigned short u16[4][WORKPAD_SIZE];

            AvCharList casesFour(u8FourByteHdrCases);
            AvCharList casesContin(u8ContinByteCases);

            AvCharList *outerIters[2] = { &casesFour, &casesContin };
            OdomIter<AvCharList::iterator, 2> c4C2(outerIters);

            for ( ; c4C2 ; c4C2.next() ) {
                AvCharList::iterator c4i = c4C2[0];
                AvCharList::iterator cC2i = c4C2[1];

                if (0 == *c4i) {
                    if (*cC2i < FOUR_BYTE_ZERO_NEEDS) {
                        continue;
                    }
                }
                else if (4 == *c4i) {
                    if (*cC2i >= FOUR_BYTE_FOUR_MAX) {
                        continue;
                    }
                }

                OdomIter<AvCharList::iterator, 2> c3C4(outerIters);

                for ( ; c3C4 ; c3C4.next() ) {
                    AvCharList::iterator cC3i = c3C4[0];
                    AvCharList::iterator cC4i = c3C4[1];
                        u8[0] = *c4i | 0xf0;
                        u8[1] = *cC2i | 0x80;
                        u8[2] = *cC3i | 0x80;
                        u8[3] = *cC4i | 0x80;
                        u8[4] = 0;

                        SrcSpec<char> source(u8, 0, 3);
                        ConvRslt expected(0, 2, 3);

                        FourWayRunner<unsigned short, char> runner(u16,
                                                          0xffff,
                                                          BUFFER_ZONE,
                                                          source,
                                                          u8ToU16,
                                                          expected);
                        if (! RUN_FOUR_WAYS(runner) ) {
                            if (veryVerbose) {
                                ArrayRange<char> a(u8);
                                cout << "==========" << endl << hex
                                     << R_(deChar(*c4i)) << R_(deChar(*cC2i))
                                     << R_(deChar(*cC3i)) << R(deChar(*cC4i))
                                     << dec << endl
                                     << prHexRange(a) << endl;
                            }
                        }

                        unsigned long high = deChar(*c4i);
                        unsigned long mid = deChar(*cC2i);
                        unsigned long low = deChar(*cC3i);
                        unsigned long lowest = deChar(*cC4i);

                        unsigned long iso10646Char =
                            high << 18 | mid << 12 | low << 6 | lowest ;
                        unsigned long utf16Conv = iso10646Char - 0x10000;

                        EXPECTED_GOT((utf16Conv >> 10) | 0xd800,
                                     runner.begin(0)[0]);
                        EXPECTED_GOT((utf16Conv & 0x3ff) | 0xdc00,
                                     runner.begin(0)[1]);
                }
            }
        }
// cout << "Four-octet cases done." << endl ;

        if (verbose) {
            cout << "\nTest 2b1: UTF-16 => UTF-8, selected "
                    "single-octet code points individually, four ways."
                 << "\n===================================="
                 << "===========================================" << endl;
        }

        // Subtest 2b1: UTF-16 => UTF-8, single byte
        {
            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            unsigned short u16[3];
            char           u8[4][WORKPAD_SIZE];

            AvCharList casesOne(u8OneByteCases);

            for (AvCharList::iterator c1i = casesOne.begin();
                                           casesOne.end() != c1i ; ++c1i ) {
                u16[0] = *c1i;
                u16[1] = 0;

                SrcSpec<unsigned short> source(u16, 0, 2);
                ConvRslt expected(0, 2, 2);

                FourWayRunner<char, unsigned short> runner(u8,
                                                  (char) 0xff,
                                                  BUFFER_ZONE,
                                                  source,
                                                  u16ToU8,
                                                  expected);
                RUN_FOUR_WAYS(runner);
                EXPECTED_GOT(*c1i, runner.begin(0)[0]);
            }
        }
// cout << "One-octet UTF-16=>UTF-8 case done." << endl ;

        if (verbose) {
            cout << "\nTest 2b2: UTF-16 => UTF-8, selected "
                    "two-octet code points individually, four ways."
                 << "\n===================================="
                 << "========================================" << endl;
        }

        // Subtest 2b2: UTF-16 => UTF-8, two-byte
        {
            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            unsigned short u16[5];
            char           u8[4][WORKPAD_SIZE];

            AvCharList casesTwo(u8TwoByteHdrCases);
            AvCharList casesContin(u8ContinByteCases);

            // The OdomIter walks through the nested iterators, odometer-style.
            // It collapses the loop and keeps things from nesting off the
            // page.
            AvCharList *outerIters[2] = { &casesTwo, &casesContin };
            OdomIter<AvCharList::iterator, 2> c2C(outerIters);

            for ( ; c2C ; c2C.next() ) {
                AvCharList::iterator c2i = c2C[0];
                AvCharList::iterator cCi = c2C[1];
// cout << hex << "*c2i " << deChar(*c2i) << "  *cCi "
//      << deChar(*cCi) << dec << endl ;
                u16[0] = static_cast<unsigned short>(*c2i << 6 | *cCi);
                u16[1] = 0;
// cout << hex << "u16[0] " << u16[0] << dec << endl ;

                SrcSpec<unsigned short> source(u16, 0, 3);
                ConvRslt expected(0, 2, 3);

                FourWayRunner<char, unsigned short> runner(u8,
                                                           (char) 0xff,
                                                           BUFFER_ZONE,
                                                           source,
                                                           u16ToU8,
                                                           expected);
                RUN_FOUR_WAYS(runner);

                EXPECTED_GOT((unsigned) deChar(0xc0 | *c2i),
                             (unsigned) deChar(runner.begin(0)[0]));
                EXPECTED_GOT((unsigned) deChar(0x80 | *cCi),
                             (unsigned) deChar(runner.begin(0)[1]));
            }
        }
// cout << "Two-octet UTF-16=>UTF-8 cases done." << endl ;

        if (verbose) {
            cout << "\nTest 2b3: UTF-16 => UTF-8, selected "
                    "three-octet code points individually, four ways."
                 << "\n===================================="
                    "==========================================" << endl;
        }

        // Subtest 2b3: UTF-16 => UTF-8, three-byte
        {
            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            unsigned short u16[5];
            char           u8[4][WORKPAD_SIZE];

            AvCharList casesThree(u8ThreeByteHdrCases);
            AvCharList casesContin(u8ContinByteCases);

            AvCharList *outerIters[2] = { &casesThree, &casesContin };
            OdomIter<AvCharList::iterator, 2> c3C2(outerIters);

            for ( ; c3C2 ; c3C2.next() ) {
                AvCharList::iterator c3i = c3C2[0];
                AvCharList::iterator cC2i = c3C2[1];

                if (0 == *c3i ) {
                    if (*cC2i < THREE_BYTE_ZERO_NEEDS) {
// cout << hex << "skipping " << deChar(*c3i) << ", "
//      << deChar(*cC2i) << ", *" << dec << endl ;
                        continue;
                    }
// cout << hex << "doing " << deChar(*c3i) << ", "
//      << deChar(*cC2i) << ", *" << dec << endl ;
                }

                for (AvCharList::iterator cC3i = casesContin.begin();
                                                casesContin.end() != cC3i;
                                                               ++cC3i ) {
                    u16[0] = static_cast<unsigned short>(
                                              *c3i << 12 | *cC2i << 6 | *cC3i);
                    u16[1] = 0;

                    SrcSpec<unsigned short> source(u16, 0, 4);
                    ConvRslt expected(0, 2, 4);

                    FourWayRunner<char, unsigned short> runner(u8,
                                                               (char) 0xff,
                                                               BUFFER_ZONE,
                                                               source,
                                                               u16ToU8,
                                                               expected);
                    RUN_FOUR_WAYS(runner);

                    EXPECTED_GOT((unsigned) deChar(0xe0 | *c3i),
                                 (unsigned) deChar(runner.begin(0)[0]));
                    EXPECTED_GOT((unsigned) deChar(0x80 | *cC2i),
                                 (unsigned) deChar(runner.begin(0)[1]));
                    EXPECTED_GOT((unsigned) deChar(0x80 | *cC3i),
                                 (unsigned) deChar(runner.begin(0)[2]));
                }
            }
        }
// cout << "Three-octet UTF-16=>UTF-8 cases done." << endl ;

        if (verbose) {
            cout << "\nTest 2b4: UTF-16 => UTF-8, selected "
                    "four-octet code points individually, four ways."
                 << "\n===================================="
                    "=========================================" << endl;
        }

        // Subtest 2b4: UTF-16 => UTF-8, four byte
        {
            Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

            unsigned short u16[5];
            char           u8[4][WORKPAD_SIZE];

            AvCharList casesFour(u8FourByteHdrCases);
            AvCharList casesContin(u8ContinByteCases);

            int nCases = 0;

            // The OdomIter collapses two iterating loops into one, holding
            // down the depth of nesting.  It cycles the last iterator fastest.
            AvCharList *outerIters[2] = { &casesFour, &casesContin };
            OdomIter<AvCharList::iterator, 2> c4C2(outerIters);

            for ( ; c4C2 ; c4C2.next() ) {
                AvCharList::iterator c4i = c4C2[0];
                AvCharList::iterator cC2i = c4C2[1];

                if (0 == *c4i ) {
                    if (*cC2i < FOUR_BYTE_ZERO_NEEDS) {
                        continue;
                    }
                }
                else if (4 == *c4i) {
                    if (*cC2i >= FOUR_BYTE_FOUR_MAX) {
                        continue;
                    }
                }

                AvCharList *innerIters[2] = { &casesContin, &casesContin };
                OdomIter<AvCharList::iterator, 2> c3C4(innerIters);

                for ( ; c3C4 ; c3C4.next() ) {
                    AvCharList::iterator cC3i = c3C4[0];
                    AvCharList::iterator cC4i = c3C4[1];

                    unsigned long iso10646 = (*c4i << 18 |
                                              *cC2i << 12 |
                                              *cC3i << 6 |
                                              *cC4i ) - 0x10000 ;
                    u16[0] = static_cast<unsigned short>(
                                                 0xd800 | ( iso10646 >> 10 ));
                    u16[1] = static_cast<unsigned short>(
                                                 0xdc00 | ( iso10646 & 0x3ff));
                    u16[2] = static_cast<unsigned short>(0);

                    SrcSpec<unsigned short> source(u16, 0, 5);
                    ConvRslt expected(0, 2, 5);

                    FourWayRunner<char, unsigned short> runner(u8,
                                                               (char) 0xff,
                                                               BUFFER_ZONE,
                                                               source,
                                                               u16ToU8,
                                                               expected);
                    if (! RUN_FOUR_WAYS(runner)) {
                        if (veryVerbose) {
                            ArrayRange<unsigned short> a(u16);
                            cout << "==========" << endl << hex
                                 << R_(deChar(*c4i)) << R_(deChar(*cC2i))
                                 << R_(deChar(*cC3i)) << R(deChar(*cC4i))
                                 << dec << endl
                                 << prHexRange(a) << endl;
                        }
                    }

                    EXPECTED_GOT((unsigned) deChar(0xf0 | *c4i),
                                 (unsigned) deChar(runner.begin(0)[0]));
                    EXPECTED_GOT((unsigned) deChar(0x80 | *cC2i),
                                 (unsigned) deChar(runner.begin(0)[1]));
                    EXPECTED_GOT((unsigned) deChar(0x80 | *cC3i),
                                 (unsigned) deChar(runner.begin(0)[2]));
                    EXPECTED_GOT((unsigned) deChar(0x80 | *cC4i),
                                 (unsigned) deChar(runner.begin(0)[3]));
                    nCases++;
                }
            }
            if (verbose) {
                cout << "nCases " << nCases << endl;
            }
        }

        if (verbose) {
            cout << "Test 2 complete." << endl;
        }
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   - That the enumerations used herein for return values match the
        //     written return values.
        //   - That the conversion functions handle a null input
        //     string with both zero-length and non-zero-length
        //     write buffers: (1), (2), (3).
        //   - That the conversion functions do not overwrite memory
        //     adjacent to or nearby the output buffer: (1).
        //   - That the conversion functions properly null-terminate the string
        //     in the output buffer whenever there is room to do so: (4).
        //   - That the conversion functions return the correct values,
        //     both by the value of the expression and via the code point
        //     count and byte/word count pointer parameters: (2)
        //
        // Plan:
        //   Verify the enumerations explicitly.
        //
        //   Run the conversion functions from UTF-8 to UTF-16 and vice versa
        //   o first with an empty input string and a one-memory-unit output
        //     buffer,
        //   o then with an empty input string and a zero-length output buffer.
        //   In each case, place the output buffer in the middle of an array
        //   written with a known pattern; verify that the pattern in the area
        //   surrounding the output buffer retains that pattern.
        //   Verify the return value in each case.
        //   Verify in the first case that one null symbol is written and in
        //   the second that nothing is written.
        //
        //   Run each test four times with the conversion function arguments
        //   set to return
        //     o the symbol and memory count,
        //     o the memory count alone,
        //     o the symbol count alone,
        //     o and neither count, verifying in each case that the expected
        //   value is placed in the variable(s) pointed to by the argument(s).
        //
        // Testing:
        //   Enumerations used in implementation relative to documented values
        //   UTF-8 to UTF-16, empty string
        //   UTF-16 to UTF-8, empty string
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nVerifying enumerations"
                 << "\n======================"
                 << endl ;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            cout << "PMR\n";
#else
            cout << "non-PMR\n";
#endif
        }

        if (veryVerbose) P(sizeof(wchar_t));

        // Check return value enumeration:

        // "Return 0 on success, ..."

        ASSERT(0 == SUCCESS);
        ASSERT(0 == OK);

        // "... 1 on invalid input, ..."

        ASSERT(1 == INVALID_INPUT_CODE_POINT);
        ASSERT(1 == BADC);

        // "...  2 if 'dstCapacity' is insufficient to hold the complete
        // conversion, ..."

        ASSERT(2 == OUTPUT_BUFFER_TOO_SMALL);
        ASSERT(2 == OBTS);

        // "and 3 if both types of error occur."

        ASSERT(3 == (OUTPUT_BUFFER_TOO_SMALL | INVALID_INPUT_CODE_POINT));
        ASSERT(3 == BOTH);

        if (verbose) {
            cout << "\nBreathing Test: Empty and short strings, short and "
                                                "zero-length output buffers."
                 << "\n==================================================="
                                                "==========================="
                 << endl ;
        }

// VERBOSE-IFY ...

        Conversion<char, unsigned short>
                                u16ToU8(bdlde::CharConvertUtf16::utf16ToUtf8);

        Conversion<unsigned short, char>
                                u8ToU16(surrogateUtf8ToUtf16);

        enum { BUFFER_ZONE = 256        // How much memory to check before and
                                        // after the output buffer, in order to
                                        // detect stray writes.  In this test
                                        // we use a large BUFFER_ZONE.  Some
                                        // machines will dump core if a local
                                        // array is too large.  This works on
                                        // the machines at hand.
        };

// @+@+@ MaT --- should find a way to get veryVerbose checks inside
//               RUN_FOUR_WAYS
        {
            // 8 => 16, null string, room for the null
            if (veryVerbose) {
                cout << "UTF-8 => utf16, null string, room for the null ..."
                     << endl;
            }

            typedef BufferSizes<0,  // Source code points
                                1,                        // Source char size
                                1,                        // Dest char size
                                BUFFER_ZONE> Sizes;

            unsigned short u16[4][Sizes::TO_BUF_SIZE];

            ConvRslt returned;    // This name gets printed by ASSERTs and
                                  // should indicate that its value is the one
                                  // actually generated.

            SrcSpec<char> source("", 0, 1);

            ConvRslt expected(0, 1, 1);

            FourWayRunner<unsigned short, char> runner(u16,
                                                       0xffff,
                                                       BUFFER_ZONE,
                                                       source,
                                                       u8ToU16,
                                                       expected);
            RUN_FOUR_WAYS(runner);

            if (veryVerbose) {
                cout << " done." << endl ;
            }
        }
        {
            // 16 => 8, null string, room for the null
            if (veryVerbose) {
                cout << "UTF-16 => UTF-8, null string, room for the null ..."
                     << endl;
            }

            typedef BufferSizes<0,  // Source code points
                                1,                        // Source char size
                                1,                        // Dest char size
                                BUFFER_ZONE> Sizes;

            char u8[4][Sizes::TO_BUF_SIZE];

            ConvRslt returned;    // This name gets printed by ASSERTs and
                                  // should indicate that its value is the one
                                  // actually generated.

            unsigned short emptyString[1] = { 0, };
            SrcSpec<unsigned short> source(emptyString, 0, 1);

            ConvRslt expected(0, 1, 1);

            FourWayRunner<char, unsigned short> runner(u8,
                                                       static_cast<char>(0xff),
                                                       BUFFER_ZONE,
                                                       source,
                                                       u16ToU8,
                                                       expected);
            RUN_FOUR_WAYS(runner);

            if (veryVerbose) {
                cout << " done." << endl ;
            }
        }
        {
            // 8 => 16, null string, no room for the null
            if (veryVerbose) {
                cout <<
                    "UTF-8 => UTF-16, null string, no room for the null ...\n";
            }

            typedef BufferSizes<0,  // Source code points
                                1,                        // Source char size
                                1,                        // Dest char size
                                BUFFER_ZONE> Sizes;

            unsigned short u16[4][Sizes::TO_BUF_SIZE];

            ConvRslt returned;    // This name gets printed by ASSERTs and
                                  // should indicate that its value is the one
                                  // actually generated.

            SrcSpec<char> source("", 0, 0);

            ConvRslt expected(2, 0, 0);

            FourWayRunner<unsigned short, char> runner(u16,
                                                       0xffff,
                                                       BUFFER_ZONE,
                                                       source,
                                                       u8ToU16,
                                                       expected);
            RUN_FOUR_WAYS(runner);

            if (veryVerbose) {
                cout << " done." << endl ;
            }
        }
        {
            // 16 => 8, null string, no room for the null
            if (veryVerbose) {
                cout <<
                    "UTF-16 => UTF-8, null string, no room for the null ...\n";
            }

            typedef BufferSizes<0,  // Source code points
                                1,                        // Source char size
                                1,                        // Dest char size
                                BUFFER_ZONE> Sizes;

            char u8[4][Sizes::TO_BUF_SIZE];

            ConvRslt returned;    // This name gets printed by ASSERTs and
                                  // should indicate that its value is the one
                                  // actually generated.

            unsigned short emptyString[1] = { 0, };
            SrcSpec<unsigned short> source(emptyString, 0, 0);

            ConvRslt expected(2, 0, 0);

            FourWayRunner<char, unsigned short> runner(u8,
                                                       static_cast<char>(0xff),
                                                       BUFFER_ZONE,
                                                       source,
                                                       u16ToU8,
                                                       expected);
            RUN_FOUR_WAYS(runner);

            if (veryVerbose) {
                cout << " done." << endl ;
            }
        }
      } break;

      case -1: {
          runPlainTextPerformanceTest();
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    bsls::Types::Int64 numAllocations = da.numAllocations();
    LOOP_ASSERT(numAllocations, 0 == numAllocations);

    return testStatus;
}

template <class T>
HexPrImpl<T> prHexRange(const T* ptr, size_t size)
{
    return HexPrImpl<T>(ptr, size);
}

template <class T>
HexPrImpl<T> prHexRange(const T *first, const T *last)
{
    return HexPrImpl<T>(first, last - first);
}

template <class T>
ostream &operator <<(ostream &os, const HexPrImpl<T> &t)
{
    const ios_base::fmtflags flags = os.flags();
    const char fill = os.fill();
    os << hex << bsl::internal << "[";
    for (int i = 0; i < (int) t.d_av.size(); ++i) {
        os << " "
           << bsl::setw(6)
           << deChar(t.d_av[i]);
    }
    os.fill(fill);
    os.flags(flags);
    return os << " ]";
}

template <class T>
MixedPrImpl<T> prMixedRange(const T* ptr, size_t size)
{
    return MixedPrImpl<T>(ptr, size);
}

template <class T>
MixedPrImpl<T> prMixedRange(const T *first, const T *last)
{
    return MixedPrImpl<T>(first, last - first);
}

template <class T>
ostream &operator <<(ostream &os, const MixedPrImpl<T> &t)
{
    const ios_base::fmtflags flags = os.flags();
    const char fill = os.fill();

    os << hex << bsl::right << bsl::showbase << "[";

    // The tricky part here is that we want to print graphic code points as
    // code points and everything else as hex.  The settings have to be
    // restored!  This is set up to change the base between runs of graphic and
    // non-graphic code points; that's probably excessive.  Other things are
    // restored at the end.  ('graphic' means ( 'printable' and not the space
    // code point ).)

// @@@@ MaT --- should adjust width to bit-size of type.

    for (int i = 0; i < t.d_av.size(); ) {
        for (; i < t.d_size
          && static_cast<unsigned int>(t.d_array[i]) < 0x80
          && isgraph(t.d_ptr[i]) ; ++i) {
            os << ' ' << setw(6) << (signed char) t.d_ptr[i] ;
        }

        os << hex;
        for (; i < t.d_av.size()
            && ! ( static_cast<unsigned int>(t.d_array[i]) < 0x80
               && isgraph(t.d_ptr[i])) ; ++i) {
            if (t.d_array[i]) {
                os << ' ' << setw( 6 )
                          << deChar(t.d_array[i]);
            }
            else {
                os << "    0x0" ;
            }
        }
        os << dec;
    }

    os.fill(fill);
    os.flags(flags);
    return os << " ]";
}

// 'printStr' helper routines to print test strings at high verbosity levels.
// @@@ Make this into an ostream-able object.
void printStr(const char *q)
{
    const unsigned char *p = (const unsigned char*)q;
    printf("[");
    while (*p) {
        if (*p > ' ' && *p < 127) {
            printf("    '%c'",*p);
        }
        else {
            printf(" 0x%04X",*p);
        }

        ++p;
    }

    printf(" 0x00");

    printf(" ]");
}

// @@@ Make this into an ostream-able object.
void printStr(const unsigned short *p)
{
    printf("[");
    while (*p) {
        if (*p >= ' ' && *p < 127) {
            printf("    '%c'",*p);
        }
        else {
            printf("  0x%04X",*p);
        }

        ++p;
    }

    printf(" 0x0000");

    printf(" ]");
}

// strcmp() on arbitrary types.  It's concerned only with equality; it does not
// define a partial order or a partitioning.  It requires that the end of the
// string be marked by a sentinel that compares equal to zero.  Returns true if
// the two strings are equal, false otherwise.

template<class CHAR_TYPE>
int strEq(const CHAR_TYPE *lhs,
          const CHAR_TYPE *rhs)
{
    for ( ; ; ++lhs, ++rhs) {
        if (*lhs != *rhs) {
            return 0;                                                 // RETURN
        }
        if (! *lhs) {
            return 1;                                                 // RETURN
        }
    }
}

//--   4-way  here ------------------------------------------------
template <class TO_CHAR, class FROM_CHAR>
bool FourWayRunner<TO_CHAR, FROM_CHAR>::runAndCheck(int bufN, int line)
{
    fillMargins(bufN);
    d_result[bufN] = d_conv(d_wp, d_outBuf[bufN], d_src, d_nExps[bufN]);

// @+@+@ MaT --- should find a way to get veryVerbose checks inside
//               RUN_FOUR_WAYS
    bool failed = false;

    if (! EXPECTED_GOT(d_nExps[bufN], d_result[bufN])) {
        cout << "   From line " << line << endl;
        failed = true;
    }

    const bool c1 = checkMargins(bufN), c2 = checkFinalNull(bufN);
    ASSERT(c1);
    ASSERT(c2);

    if (! (c1 && c2)) {
        cout << "   From line " << line << endl;
        failed = true;
    }

    return !failed;
}

template <class TO_CHAR, class FROM_CHAR>
bool FourWayRunner<TO_CHAR, FROM_CHAR>::runFourWays(int line)
{
    // A rough guide to the 'runFourWays' function:
    //   The "four ways" are (a) with valid pointers for both the number of
    //   code points and the number of memory units, (b) with a valid pointer
    //   for the number of code points and a null for the number of memory
    //   units, (c) with a null for the number of code points (symbols) and a
    //   valid pointer for the number of memory units, and (d) with nulls for
    //   both the number of code points and the number of memory units.
    //
    //   For each of these cases, a clean copy of the expected 'ConvRslt' is
    //   made and the 'runAndCheck' function is run, with the second parameter
    //   identifying which of four workpiece buffers and four returned
    //   'ConvRslt''s is to be used.
    //
    //   For each case, if the conversion and check succeed, a pointer to the
    //   output buffer is pushed onto a "string set".
    //
    //   If all four cases have succeeded, then
    //     If the output buffer length is zero, then the strings must be
    //     identical; we have succeeded.  Otherwise all four strings are
    //     compared and sorted into equivalence classes.  If all four are in
    //     the same class, then 'RUN_FOUR_WAYS' returns 'true'; if not,
    //     'false'.  The tests are redundant; cmpAllStrings() returns true if
    //     everything is in one equivalence class; the 'EXPECTED_GOT' macro
    //     tests that the first class contains four strings.  But if all four
    //     are not in the same class, it may be useful to get the output from
    //     the 'EXPECTED_GOT' macro.
    bool failed = false;
// @+@+@ MaT --- should find a way to get veryVerbose checks inside
//               RUN_FOUR_WAYS

    d_nExps[0] = d_expected;
    if (runAndCheck(0, line)) {
        d_strSet.push_back(begin(0).begin());
    }
    else {
        cout << "   From line " << line << "/" << __LINE__ << endl;
        failed = true;
    }

    d_nExps[1] = d_expected;
    d_nExps[1].unit(-1);
    if (runAndCheck(1, line)) {
        d_strSet.push_back(begin(1).begin());
    }
    else {
        cout << "   From line " << line << "/" << __LINE__ << endl;
        failed = true;
    }

    d_nExps[2] = d_expected;
    d_nExps[2].sym(-1);
    if (runAndCheck(2, line)) {
        d_strSet.push_back(begin(2).begin());
    }
    else {
        cout << "   From line " << line << "/" << __LINE__ << endl;
        failed = true;
    }

    d_nExps[3] = d_expected;
    d_nExps[3].unit(-1).sym(-1);
    if (runAndCheck(3, line)) {
        d_strSet.push_back(begin(3).begin());
    }
    else {
        cout << "   From line " << line << "/" << __LINE__ << endl;
        failed = true;
    }

    if (failed) {
        return !failed;                                               // RETURN
    }

    if (0 == d_wp.d_winLength) {
        return true;                                                  // RETURN
    }


    if (!cmpAllStrings()) {
        ASSERT(cmpAllStrings());
        cout << "   From line " << line << "/" << __LINE__ << endl;
        failed = true;
    }

    if (!EXPECTED_GOT(4,d_strEqClasses[0].size())) {
        cout << "   From line " << line << "/" << __LINE__ << endl;
        failed = true;
    }

    return !failed;
}

// @+@+@+@ Not necessarily FixedVector<>() ... but it must act like an array
// Given a FixedVector of arrays of (CHAR_TYPE*), compare them for equality and
// construct a set of equivalence classes on them.  Return the set of classes
// as a vector of vectors, with each string identified by its index in the
// original vector.  (What we want most of all is to find that they are all
// equal, but if they are not, it will be good to know which are not, and how.)
//
// (This function originally returned its result directly, but that operation
// did not work on Windows.  It returned what looked like a default-initialized
// instance.)

template<class ARRAY_TYPE, bsl::size_t N_WAY>
void equivClasses( FixedVector<FixedVector<int, N_WAY>, N_WAY > *retVal,
                   const ARRAY_TYPE&                             sv)
{
    FixedVector<FixedVector<int, N_WAY>, N_WAY >& eqClasses = *retVal;
    eqClasses.resize(N_WAY);
    for (int i = 0; i < (int) N_WAY; ++i) {
        eqClasses[i].resize(0);
    }
    eqClasses.resize(0);

    if (sv.size() == 0) {
        return;                                                       // RETURN
    }

    // Put the first thing in a class of its own.
    eqClasses.resize(1);
    eqClasses[0].push_back(0);

    // Then, for each remaining string 'n', scan the equivalence classes.
    for (int n = 1; n < (int) sv.size(); ++n) {

        // Check string 'n' against each existing equivalence class in turn.
        for (int eqCl = 0; ; ++eqCl) {

            // If we have run out of existing equivalence classes in which to
            // look for string 'n', string 'n' must go in a new equivalence
            // class.
            if (eqCl >= (int) eqClasses.size()) {
                eqClasses.resize(eqClasses.size() + 1);
                eqClasses[eqClasses.size() - 1].push_back(n);
                break;
            }

            // Otherwise if string 'n' matches a string in this-the-next
            // equivalence class, then string 'n' belongs in that class.
            if (strEq(sv[n], sv[eqClasses[eqCl][0]])) {
                eqClasses[eqCl].push_back(n);
                break;
            }
        }
    }

    return;
}

//-----------------------------------------------------------------------------

void checkForExpectedConversionResultsU2ToU8(unsigned short *input,
                                             char           *expected_output,
                                             bsl::size_t     totalOutputLength,
                                             unsigned short *codePointSizes,
                                             bsl::size_t     codePointCount,
                                             int,
                                             int             veryVerbose)
{
    int retVal;

    if (veryVerbose) {
        cout << "checkForExpectedConversionResultsU2ToU8("
             <<  "\n\tinput             ="
             << prHexRange(input, codePointCount+1)
             << ",\n\texpected_output   =";
        printStr(expected_output);
        cout << ",\n\ttotalOutputLength ="
             << totalOutputLength
             << ",\n\tcodePointSizes    ="
             << prHexRange(codePointSizes, codePointCount)
             << ",\n\tcodePointCount    ="
             << codePointCount
             << ");\n";
    }

    if (!totalOutputLength) {
        return;                                                       // RETURN
    }

    for (int bufSize = 0; bufSize < (int) totalOutputLength; ++bufSize) {
        char outputBuffer[256] = { 0 };
        bsl::size_t bytesWritten = 0;
        bsl::size_t codePointsWritten = 0;

        retVal = bdlde::CharConvertUtf16::utf16ToUtf8(
                outputBuffer,
                bufSize,
                input,
                &codePointsWritten,
                &bytesWritten);

        LOOP5_ASSERT(L_, OUTPUT_BUFFER_TOO_SMALL,   retVal,
                         bufSize,                   totalOutputLength,
                         OUTPUT_BUFFER_TOO_SMALL == retVal);
        LOOP3_ASSERT(L_, codePointsWritten,   codePointCount,
                         codePointsWritten <= codePointCount);
        LOOP3_ASSERT(L_, bytesWritten,   totalOutputLength,
                         bytesWritten <= totalOutputLength);
    }

    char outputBuffer[256] = { 0 };
    bsl::size_t bytesWritten = 0;
    bsl::size_t codePointsWritten = 0;

    retVal = bdlde::CharConvertUtf16::utf16ToUtf8(
                                    outputBuffer,
                                    totalOutputLength + 1,
                                    input,
                                    &codePointsWritten,
                                    &bytesWritten);

    LOOP3_ASSERT(L_, SUCCESS,   retVal,
                     SUCCESS == retVal);
    LOOP3_ASSERT(L_, codePointsWritten,   1 + codePointCount,
                     1 + codePointCount == codePointsWritten);
    LOOP3_ASSERT(L_, bytesWritten,   1 + totalOutputLength,
                     1 + totalOutputLength == bytesWritten);

    LOOP5_ASSERT(L_, totalOutputLength + 1,
                     bytesWritten,
                     prHexRange(outputBuffer, totalOutputLength + 1),
                     prHexRange(expected_output, bytesWritten),
                     0 == strcmp(outputBuffer, expected_output));
}

void buildUpAndTestStringsU2ToU8(int             idx,
                                 int             depth,
                                 unsigned short *inputBuffer,
                                 char           *outputBuffer,
                                 unsigned short *codePointSizes,
                                 bsl::size_t     totalOutputLength,
                                 bsl::size_t     codePointCount,
                                 unsigned short *inputCursor,
                                 char           *outputCursor,
                                 int             verbose,
                                 int             veryVerbose)
{
    // Null-terminate input and expected output:

    *inputCursor  = 0;
    *outputCursor = 0;

    checkForExpectedConversionResultsU2ToU8(inputBuffer,
            outputBuffer,
            totalOutputLength,
            codePointSizes,
            codePointCount,
            verbose,
            veryVerbose);

    if (depth <= 0) {
        return;                                                       // RETURN
    }

    struct PrecomputedData const &d = PRECOMPUTED_DATA[idx];

    *inputCursor++         = d.d_utf16Word;

    // Null-terminate input:

    *inputCursor           = 0;

    strcpy(outputCursor,    d.d_utf8CodePoint);
    outputCursor         += d.d_utf8CodePointLength;
    totalOutputLength    += d.d_utf8CodePointLength;

    codePointSizes[codePointCount++] =
                          static_cast<unsigned short>(d.d_utf8CodePointLength);

    for (int i = 0; i < (int) precomputedDataCount; ++i) {
        buildUpAndTestStringsU2ToU8(i,
                                    depth - 1,
                                    inputBuffer,
                                    outputBuffer,
                                    codePointSizes,
                                    totalOutputLength,
                                    codePointCount,
                                    inputCursor,
                                    outputCursor,
                                    verbose,
                                    veryVerbose);
    }
}

// This utility function for performance testing 'utf8ToUtf16' and
// 'utf16ToUtf8' will repeatedly convert a long ASCII corpus to UTF-16 and back
// again.  It returns 0 on success, and non-zero otherwise.

int runPlainTextPerformanceTest(void)
{
    // The entire text of _Pride and Prejudice_, by Jane Austen
    const char prideAndPrejudice[] = {
        "The Project Gutenberg EBook of Pride and Prejudice, by Jane\n"
        "Austen\n"
        "\n"
        "This eBook is for the use of anyone anywhere at no cost and\n"
        "with almost no restrictions whatsoever.  You may copy it,\n"
        "give it away or re-use it under the terms of the Project\n"
        "Gutenberg License included with this eBook or online at\n"
        "www.gutenberg.org\n"
        "\n"
        "\n"
        "Title: Pride and Prejudice\n"
        "\n"
        "Author: Jane Austen\n"
        "\n"
        "Posting Date: August 26, 2008 [EBook #1342] Release Date:\n"
        "June, 1998\n"
        "\n"
        "Language: English\n"
        "\n"
        "Character set encoding: ASCII\n"
        "\n"
        "*** START OF THIS PROJECT GUTENBERG EBOOK PRIDE AND\n"
        "PREJUDICE ***\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "Produced by Anonymous Volunteers\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "PRIDE AND PREJUDICE\n"
        "\n"
        "By Jane Austen\n"
        "\n"
        "\n"
        "\n"
        "Chapter 1\n"
        "\n"
        "\n"
        "It is a truth universally acknowledged, that a single man in\n"
        "possession of a good fortune, must be in want of a wife.\n"
        "\n"
        "However little known the feelings or views of such a man may\n"
        "be on his first entering a neighbourhood, this truth is so\n"
        "well fixed in the minds of the surrounding families, that he\n"
        "is considered the rightful property of some one or other of\n"
        "their daughters.\n"
        "\n"
        "\"My dear Mr. Bennet,\" said his lady to him one day, \"have\n"
        "you heard that Netherfield Park is let at last?\"\n"
        "\n"
        "Mr. Bennet replied that he had not.\n"
        "\n"
        "\"But it is,\" returned she; \"for Mrs. Long has just been\n"
        "here, and she told me all about it.\"\n"
        "\n"
        "Mr. Bennet made no answer.\n"
        "\n"
        "\"Do you not want to know who has taken it?\" cried his wife\n"
        "impatiently.\n"
        "\n"
        "\"_You_ want to tell me, and I have no objection to hearing\n"
        "it.\"\n"
        "\n"
        "This was invitation enough.\n"
        "\n"
        "\"Why, my dear, you must know, Mrs. Long says that\n"
        "Netherfield is taken by a young man of large fortune from\n"
        "the north of England; that he came down on Monday in a\n"
        "chaise and four to see the place, and was so much delighted\n"
        "with it, that he agreed with Mr. Morris immediately; that he\n"
        "is to take possession before Michaelmas, and some of his\n"
        "servants are to be in the house by the end of next week.\"\n"
        "\n"
        "\"What is his name?\"\n"
        "\n"
        "\"Bingley.\"\n"
        "\n"
        "\"Is he married or single?\"\n"
        "\n"
        "\"Oh! Single, my dear, to be sure! A single man of large\n"
        "fortune; four or five thousand a year. What a fine thing for\n"
        "our girls!\"\n"
        "\n"
        "\"How so? How can it affect them?\"\n"
        "\n"
        "\"My dear Mr. Bennet,\" replied his wife, \"how can you be so\n"
        "tiresome!  You must know that I am thinking of his marrying\n"
        "one of them.\"\n"
        "\n"
        "\"Is that his design in settling here?\"\n"
        "\n"
        "\"Design! Nonsense, how can you talk so! But it is very\n"
        "likely that he _may_ fall in love with one of them, and\n"
        "therefore you must visit him as soon as he comes.\"\n"
        "\n"
        "\"I see no occasion for that. You and the girls may go, or\n"
        "you may send them by themselves, which perhaps will be still\n"
        "better, for as you are as handsome as any of them, Mr.\n"
        "Bingley may like you the best of the party.\"\n"
        "\n"
        "\"My dear, you flatter me. I certainly _have_ had my share of\n"
        "beauty, but I do not pretend to be anything extraordinary\n"
        "now. When a woman has five grown-up daughters, she ought to\n"
        "give over thinking of her own beauty.\"\n"
        "\n"
        "\"In such cases, a woman has not often much beauty to think\n"
        "of.\"\n"
        "\n"
        "\"But, my dear, you must indeed go and see Mr. Bingley when\n"
        "he comes into the neighbourhood.\"\n"
        "\n"
        "\"It is more than I engage for, I assure you.\"\n"
        "\n"
        "\"But consider your daughters. Only think what an\n"
        "establishment it would be for one of them. Sir William and\n"
        "Lady Lucas are determined to go, merely on that account, for\n"
        "in general, you know, they visit no newcomers. Indeed you\n"
        "must go, for it will be impossible for _us_ to visit him if\n"
        "you do not.\"\n"
        "\n"
        "\"You are over-scrupulous, surely. I dare say Mr. Bingley\n"
        "will be very glad to see you; and I will send a few lines by\n"
        "you to assure him of my hearty consent to his marrying\n"
        "whichever he chooses of the girls; though I must throw in a\n"
        "good word for my little Lizzy.\"\n"
        "\n"
        "\"I desire you will do no such thing. Lizzy is not a bit\n"
        "better than the others; and I am sure she is not half so\n"
        "handsome as Jane, nor half so good-humoured as Lydia. But\n"
        "you are always giving _her_ the preference.\"\n"
        "\n"
        "\"They have none of them much to recommend them,\" replied he;\n"
        "\"they are all silly and ignorant like other girls; but Lizzy\n"
        "has something more of quickness than her sisters.\"\n"
        "\n"
        "\"Mr. Bennet, how _can_ you abuse your own children in such a\n"
        "way? You take delight in vexing me. You have no compassion\n"
        "for my poor nerves.\"\n"
        "\n"
        "\"You mistake me, my dear. I have a high respect for your\n"
        "nerves. They are my old friends. I have heard you mention\n"
        "them with consideration these last twenty years at least.\"\n"
        "\n"
        "\"Ah, you do not know what I suffer.\"\n"
        "\n"
        "\"But I hope you will get over it, and live to see many young\n"
        "men of four thousand a year come into the neighbourhood.\"\n"
        "\n"
        "\"It will be no use to us, if twenty such should come, since\n"
        "you will not visit them.\"\n"
        "\n"
        "\"Depend upon it, my dear, that when there are twenty, I will\n"
        "visit them all.\"\n"
        "\n"
        "Mr. Bennet was so odd a mixture of quick parts, sarcastic\n"
        "humour, reserve, and caprice, that the experience of\n"
        "three-and-twenty years had been insufficient to make his\n"
        "wife understand his character. _Her_ mind was less difficult\n"
        "to develop. She was a woman of mean understanding, little\n"
        "information, and uncertain temper. When she was\n"
        "discontented, she fancied herself nervous. The business of\n"
        "her life was to get her daughters married; its solace was\n"
        "visiting and news.\n"
        "\n"
        "\n"
        "\n"
        "Chapter 2\n"
        "\n"
        "\n"
        "Mr. Bennet was among the earliest of those who waited on Mr.\n"
        "Bingley.  He had always intended to visit him, though to the\n"
        "last always assuring his wife that he should not go; and\n"
        "till the evening after the visit was paid she had no\n"
        "knowledge of it. It was then disclosed in the following\n"
        "manner. Observing his second daughter employed in trimming a\n"
        "hat, he suddenly addressed her with:\n"
        "\n"
        "\"I hope Mr. Bingley will like it, Lizzy.\"\n"
        "\n"
        "\"We are not in a way to know _what_ Mr. Bingley likes,\" said\n"
        "her mother resentfully, \"since we are not to visit.\"\n"
        "\n"
        "\"But you forget, mamma,\" said Elizabeth, \"that we shall meet\n"
        "him at the assemblies, and that Mrs. Long promised to\n"
        "introduce him.\"\n"
        "\n"
        "\"I do not believe Mrs. Long will do any such thing. She has\n"
        "two nieces of her own. She is a selfish, hypocritical woman,\n"
        "and I have no opinion of her.\"\n"
        "\n"
        "\"No more have I,\" said Mr. Bennet; \"and I am glad to find\n"
        "that you do not depend on her serving you.\"\n"
        "\n"
        "Mrs. Bennet deigned not to make any reply, but, unable to\n"
        "contain herself, began scolding one of her daughters.\n"
        "\n"
        "\"Don't keep coughing so, Kitty, for Heaven's sake! Have a\n"
        "little compassion on my nerves. You tear them to pieces.\"\n"
        "\n"
        "\"Kitty has no discretion in her coughs,\" said her father;\n"
        "\"she times them ill.\"\n"
        "\n"
        "\"I do not cough for my own amusement,\" replied Kitty\n"
        "fretfully. \"When is your next ball to be, Lizzy?\"\n"
        "\n"
        "\"To-morrow fortnight.\"\n"
        "\n"
        "\"Aye, so it is,\" cried her mother, \"and Mrs. Long does not\n"
        "come back till the day before; so it will be impossible for\n"
        "her to introduce him, for she will not know him herself.\"\n"
        "\n"
        "\"Then, my dear, you may have the advantage of your friend,\n"
        "and introduce Mr. Bingley to _her_.\"\n"
        "\n"
        "\"Impossible, Mr. Bennet, impossible, when I am not\n"
        "acquainted with him myself; how can you be so teasing?\"\n"
        "\n"
        "\"I honour your circumspection. A fortnight's acquaintance is\n"
        "certainly very little. One cannot know what a man really is\n"
        "by the end of a fortnight. But if _we_ do not venture\n"
        "somebody else will; and after all, Mrs. Long and her\n"
        "daughters must stand their chance; and, therefore, as she\n"
        "will think it an act of kindness, if you decline the office,\n"
        "I will take it on myself.\"\n"
        "\n"
        "The girls stared at their father. Mrs. Bennet said only,\n"
        "\"Nonsense, nonsense!\"\n"
        "\n"
        "\"What can be the meaning of that emphatic exclamation?\"\n"
        "cried he. \"Do you consider the forms of introduction, and\n"
        "the stress that is laid on them, as nonsense? I cannot quite\n"
        "agree with you _there_. What say you, Mary? For you are a\n"
        "young lady of deep reflection, I know, and read great books\n"
        "and make extracts.\"\n"
        "\n"
        "Mary wished to say something sensible, but knew not how.\n"
        "\n"
        "\"While Mary is adjusting her ideas,\" he continued, \"let us\n"
        "return to Mr.  Bingley.\"\n"
        "\n"
        "\"I am sick of Mr. Bingley,\" cried his wife.\n"
        "\n"
        "\"I am sorry to hear _that_; but why did not you tell me that\n"
        "before?  If I had known as much this morning I certainly\n"
        "would not have called on him. It is very unlucky; but as I\n"
        "have actually paid the visit, we cannot escape the\n"
        "acquaintance now.\"\n"
        "\n"
        "The astonishment of the ladies was just what he wished; that\n"
        "of Mrs.  Bennet perhaps surpassing the rest; though, when\n"
        "the first tumult of joy was over, she began to declare that\n"
        "it was what she had expected all the while.\n"
        "\n"
        "\"How good it was in you, my dear Mr. Bennet! But I knew I\n"
        "should persuade you at last. I was sure you loved your girls\n"
        "too well to neglect such an acquaintance. Well, how pleased\n"
        "I am! and it is such a good joke, too, that you should have\n"
        "gone this morning and never said a word about it till now.\"\n"
        "\n"
        "\"Now, Kitty, you may cough as much as you choose,\" said Mr.\n"
        "Bennet; and, as he spoke, he left the room, fatigued with\n"
        "the raptures of his wife.\n"
        "\n"
        "\"What an excellent father you have, girls!\" said she, when\n"
        "the door was shut. \"I do not know how you will ever make him\n"
        "amends for his kindness; or me, either, for that matter. At\n"
        "our time of life it is not so pleasant, I can tell you, to\n"
        "be making new acquaintances every day; but for your sakes,\n"
        "we would do anything. Lydia, my love, though you _are_ the\n"
        "youngest, I dare say Mr. Bingley will dance with you at the\n"
        "next ball.\"\n"
        "\n"
        "\"Oh!\" said Lydia stoutly, \"I am not afraid; for though I\n"
        "_am_ the youngest, I'm the tallest.\"\n"
        "\n"
        "The rest of the evening was spent in conjecturing how soon\n"
        "he would return Mr. Bennet's visit, and determining when\n"
        "they should ask him to dinner.\n"
        "\n"
        "\n"
        "\n"
        "Chapter 3\n"
        "\n"
        "\n"
        "Not all that Mrs. Bennet, however, with the assistance of\n"
        "her five daughters, could ask on the subject, was sufficient\n"
        "to draw from her husband any satisfactory description of Mr.\n"
        "Bingley. They attacked him in various ways--with barefaced\n"
        "questions, ingenious suppositions, and distant surmises; but\n"
        "he eluded the skill of them all, and they were at last\n"
        "obliged to accept the second-hand intelligence of their\n"
        "neighbour, Lady Lucas. Her report was highly favourable. Sir\n"
        "William had been delighted with him. He was quite young,\n"
        "wonderfully handsome, extremely agreeable, and, to crown the\n"
        "whole, he meant to be at the next assembly with a large\n"
        "party. Nothing could be more delightful! To be fond of\n"
        "dancing was a certain step towards falling in love; and very\n"
        "lively hopes of Mr. Bingley's heart were entertained.\n"
        "\n"
        "\"If I can but see one of my daughters happily settled at\n"
        "Netherfield,\" said Mrs. Bennet to her husband, \"and all the\n"
        "others equally well married, I shall have nothing to wish\n"
        "for.\"\n"
        "\n"
        "In a few days Mr. Bingley returned Mr. Bennet's visit, and\n"
        "sat about ten minutes with him in his library. He had\n"
        "entertained hopes of being admitted to a sight of the young\n"
        "ladies, of whose beauty he had heard much; but he saw only\n"
        "the father. The ladies were somewhat more fortunate, for\n"
        "they had the advantage of ascertaining from an upper window\n"
        "that he wore a blue coat, and rode a black horse.\n"
        "\n"
        "An invitation to dinner was soon afterwards dispatched; and\n"
        "already had Mrs. Bennet planned the courses that were to do\n"
        "credit to her housekeeping, when an answer arrived which\n"
        "deferred it all. Mr.  Bingley was obliged to be in town the\n"
        "following day, and, consequently, unable to accept the\n"
        "honour of their invitation, etc.  Mrs. Bennet was quite\n"
        "disconcerted. She could not imagine what business he could\n"
        "have in town so soon after his arrival in Hertfordshire; and\n"
        "she began to fear that he might be always flying about from\n"
        "one place to another, and never settled at Netherfield as he\n"
        "ought to be. Lady Lucas quieted her fears a little by\n"
        "starting the idea of his being gone to London only to get a\n"
        "large party for the ball; and a report soon followed that\n"
        "Mr. Bingley was to bring twelve ladies and seven gentlemen\n"
        "with him to the assembly.  The girls grieved over such a\n"
        "number of ladies, but were comforted the day before the ball\n"
        "by hearing, that instead of twelve he brought only six with\n"
        "him from London--his five sisters and a cousin. And when the\n"
        "party entered the assembly room it consisted of only five\n"
        "altogether--Mr. Bingley, his two sisters, the husband of the\n"
        "eldest, and another young man.\n"
        "\n"
        "Mr. Bingley was good-looking and gentlemanlike; he had a\n"
        "pleasant countenance, and easy, unaffected manners. His\n"
        "sisters were fine women, with an air of decided fashion. His\n"
        "brother-in-law, Mr. Hurst, merely looked the gentleman; but\n"
        "his friend Mr. Darcy soon drew the attention of the room by\n"
        "his fine, tall person, handsome features, noble mien, and\n"
        "the report which was in general circulation within five\n"
        "minutes after his entrance, of his having ten thousand a\n"
        "year.  The gentlemen pronounced him to be a fine figure of a\n"
        "man, the ladies declared he was much handsomer than Mr.\n"
        "Bingley, and he was looked at with great admiration for\n"
        "about half the evening, till his manners gave a disgust\n"
        "which turned the tide of his popularity; for he was\n"
        "discovered to be proud; to be above his company, and above\n"
        "being pleased; and not all his large estate in Derbyshire\n"
        "could then save him from having a most forbidding,\n"
        "disagreeable countenance, and being unworthy to be compared\n"
        "with his friend.\n"
        "\n"
        "Mr. Bingley had soon made himself acquainted with all the\n"
        "principal people in the room; he was lively and unreserved,\n"
        "danced every dance, was angry that the ball closed so early,\n"
        "and talked of giving one himself at Netherfield. Such\n"
        "amiable qualities must speak for themselves. What a contrast\n"
        "between him and his friend! Mr. Darcy danced only once with\n"
        "Mrs. Hurst and once with Miss Bingley, declined being\n"
        "introduced to any other lady, and spent the rest of the\n"
        "evening in walking about the room, speaking occasionally to\n"
        "one of his own party.  His character was decided. He was the\n"
        "proudest, most disagreeable man in the world, and everybody\n"
        "hoped that he would never come there again.  Amongst the\n"
        "most violent against him was Mrs.  Bennet, whose dislike of\n"
        "his general behaviour was sharpened into particular\n"
        "resentment by his having slighted one of her daughters.\n"
        "\n"
        "Elizabeth Bennet had been obliged, by the scarcity of\n"
        "gentlemen, to sit down for two dances; and during part of\n"
        "that time, Mr. Darcy had been standing near enough for her\n"
        "to hear a conversation between him and Mr.  Bingley, who\n"
        "came from the dance for a few minutes, to press his friend\n"
        "to join it.\n"
        "\n"
        "\"Come, Darcy,\" said he, \"I must have you dance. I hate to\n"
        "see you standing about by yourself in this stupid manner.\n"
        "You had much better dance.\"\n"
        "\n"
        "\"I certainly shall not. You know how I detest it, unless I\n"
        "am particularly acquainted with my partner. At such an\n"
        "assembly as this it would be insupportable. Your sisters are\n"
        "engaged, and there is not another woman in the room whom it\n"
        "would not be a punishment to me to stand up with.\"\n"
        "\n"
        "\"I would not be so fastidious as you are,\" cried Mr.\n"
        "Bingley, \"for a kingdom! Upon my honour, I never met with so\n"
        "many pleasant girls in my life as I have this evening; and\n"
        "there are several of them you see uncommonly pretty.\"\n"
        "\n"
        "\"_You_ are dancing with the only handsome girl in the room,\"\n"
        "said Mr.  Darcy, looking at the eldest Miss Bennet.\n"
        "\n"
        "\"Oh! She is the most beautiful creature I ever beheld! But\n"
        "there is one of her sisters sitting down just behind you,\n"
        "who is very pretty, and I dare say very agreeable. Do let me\n"
        "ask my partner to introduce you.\"\n"
        "\n"
        "\"Which do you mean?\" and turning round he looked for a\n"
        "moment at Elizabeth, till catching her eye, he withdrew his\n"
        "own and coldly said: \"She is tolerable, but not handsome\n"
        "enough to tempt _me_; I am in no humour at present to give\n"
        "consequence to young ladies who are slighted by other men.\n"
        "You had better return to your partner and enjoy her smiles,\n"
        "for you are wasting your time with me.\"\n"
        "\n"
        "Mr. Bingley followed his advice. Mr. Darcy walked off; and\n"
        "Elizabeth remained with no very cordial feelings toward him.\n"
        "She told the story, however, with great spirit among her\n"
        "friends; for she had a lively, playful disposition, which\n"
        "delighted in anything ridiculous.\n"
        "\n"
        "The evening altogether passed off pleasantly to the whole\n"
        "family. Mrs.  Bennet had seen her eldest daughter much\n"
        "admired by the Netherfield party. Mr. Bingley had danced\n"
        "with her twice, and she had been distinguished by his\n"
        "sisters. Jane was as much gratified by this as her mother\n"
        "could be, though in a quieter way. Elizabeth felt Jane's\n"
        "pleasure. Mary had heard herself mentioned to Miss Bingley\n"
        "as the most accomplished girl in the neighbourhood; and\n"
        "Catherine and Lydia had been fortunate enough never to be\n"
        "without partners, which was all that they had yet learnt to\n"
        "care for at a ball. They returned, therefore, in good\n"
        "spirits to Longbourn, the village where they lived, and of\n"
        "which they were the principal inhabitants. They found Mr.\n"
        "Bennet still up. With a book he was regardless of time; and\n"
        "on the present occasion he had a good deal of curiosity as\n"
        "to the events of an evening which had raised such splendid\n"
        "expectations. He had rather hoped that his wife's views on\n"
        "the stranger would be disappointed; but he soon found out\n"
        "that he had a different story to hear.\n"
        "\n"
        "\"Oh! my dear Mr. Bennet,\" as she entered the room, \"we have\n"
        "had a most delightful evening, a most excellent ball. I wish\n"
        "you had been there.  Jane was so admired, nothing could be\n"
        "like it. Everybody said how well she looked; and Mr. Bingley\n"
        "thought her quite beautiful, and danced with her twice! Only\n"
        "think of _that_, my dear; he actually danced with her twice!\n"
        "and she was the only creature in the room that he asked a\n"
        "second time. First of all, he asked Miss Lucas. I was so\n"
        "vexed to see him stand up with her! But, however, he did not\n"
        "admire her at all; indeed, nobody can, you know; and he\n"
        "seemed quite struck with Jane as she was going down the\n"
        "dance. So he inquired who she was, and got introduced, and\n"
        "asked her for the two next. Then the two third he danced\n"
        "with Miss King, and the two fourth with Maria Lucas, and the\n"
        "two fifth with Jane again, and the two sixth with Lizzy, and\n"
        "the _Boulanger_--\"\n"
        "\n"
        "\"If he had had any compassion for _me_,\" cried her husband\n"
        "impatiently, \"he would not have danced half so much! For\n"
        "God's sake, say no more of his partners. O that he had\n"
        "sprained his ankle in the first place!\"\n"
        "\n"
        "\"Oh! my dear, I am quite delighted with him. He is so\n"
        "excessively handsome! And his sisters are charming women. I\n"
        "never in my life saw anything more elegant than their\n"
        "dresses. I dare say the lace upon Mrs.  Hurst's gown--\"\n"
        "\n"
        "Here she was interrupted again. Mr. Bennet protested against\n"
        "any description of finery. She was therefore obliged to seek\n"
        "another branch of the subject, and related, with much\n"
        "bitterness of spirit and some exaggeration, the shocking\n"
        "rudeness of Mr. Darcy.\n"
        "\n"
        "\"But I can assure you,\" she added, \"that Lizzy does not lose\n"
        "much by not suiting _his_ fancy; for he is a most\n"
        "disagreeable, horrid man, not at all worth pleasing. So high\n"
        "and so conceited that there was no enduring him! He walked\n"
        "here, and he walked there, fancying himself so very great!\n"
        "Not handsome enough to dance with! I wish you had been\n"
        "there, my dear, to have given him one of your set-downs. I\n"
        "quite detest the man.\"\n"
        "\n"
        "\n"
        "\n"
        "Chapter 4\n"
        "\n"
        "\n"
        "When Jane and Elizabeth were alone, the former, who had been\n"
        "cautious in her praise of Mr. Bingley before, expressed to\n"
        "her sister just how very much she admired him.\n"
        "\n"
        "\"He is just what a young man ought to be,\" said she,\n"
        "\"sensible, good-humoured, lively; and I never saw such happy\n"
        "manners!--so much ease, with such perfect good breeding!\"\n"
        "\n"
        "\"He is also handsome,\" replied Elizabeth, \"which a young man\n"
        "ought likewise to be, if he possibly can. His character is\n"
        "thereby complete.\"\n"
        "\n"
        "\"I was very much flattered by his asking me to dance a\n"
        "second time. I did not expect such a compliment.\"\n"
        "\n"
        "\"Did not you? I did for you. But that is one great\n"
        "difference between us. Compliments always take _you_ by\n"
        "surprise, and _me_ never. What could be more natural than\n"
        "his asking you again? He could not help seeing that you were\n"
        "about five times as pretty as every other woman in the room.\n"
        "No thanks to his gallantry for that. Well, he certainly is\n"
        "very agreeable, and I give you leave to like him. You have\n"
        "liked many a stupider person.\"\n"
        "\n"
        "\"Dear Lizzy!\"\n"
        "\n"
        "\"Oh! you are a great deal too apt, you know, to like people\n"
        "in general.  You never see a fault in anybody. All the world\n"
        "are good and agreeable in your eyes. I never heard you speak\n"
        "ill of a human being in your life.\"\n"
        "\n"
        "\"I would not wish to be hasty in censuring anyone; but I\n"
        "always speak what I think.\"\n"
        "\n"
        "\"I know you do; and it is _that_ which makes the wonder.\n"
        "With _your_ good sense, to be so honestly blind to the\n"
        "follies and nonsense of others! Affectation of candour is\n"
        "common enough--one meets with it everywhere. But to be\n"
        "candid without ostentation or design--to take the good of\n"
        "everybody's character and make it still better, and say\n"
        "nothing of the bad--belongs to you alone. And so you like\n"
        "this man's sisters, too, do you? Their manners are not equal\n"
        "to his.\"\n"
        "\n"
        "\"Certainly not--at first. But they are very pleasing women\n"
        "when you converse with them. Miss Bingley is to live with\n"
        "her brother, and keep his house; and I am much mistaken if\n"
        "we shall not find a very charming neighbour in her.\"\n"
        "\n"
        "Elizabeth listened in silence, but was not convinced; their\n"
        "behaviour at the assembly had not been calculated to please\n"
        "in general; and with more quickness of observation and less\n"
        "pliancy of temper than her sister, and with a judgement too\n"
        "unassailed by any attention to herself, she was very little\n"
        "disposed to approve them. They were in fact very fine\n"
        "ladies; not deficient in good humour when they were pleased,\n"
        "nor in the power of making themselves agreeable when they\n"
        "chose it, but proud and conceited. They were rather\n"
        "handsome, had been educated in one of the first private\n"
        "seminaries in town, had a fortune of twenty thousand pounds,\n"
        "were in the habit of spending more than they ought, and of\n"
        "associating with people of rank, and were therefore in every\n"
        "respect entitled to think well of themselves, and meanly of\n"
        "others. They were of a respectable family in the north of\n"
        "England; a circumstance more deeply impressed on their\n"
        "memories than that their brother's fortune and their own had\n"
        "been acquired by trade.\n"
        "\n"
        "Mr. Bingley inherited property to the amount of nearly a\n"
        "hundred thousand pounds from his father, who had intended to\n"
        "purchase an estate, but did not live to do it. Mr. Bingley\n"
        "intended it likewise, and sometimes made choice of his\n"
        "county; but as he was now provided with a good house and the\n"
        "liberty of a manor, it was doubtful to many of those who\n"
        "best knew the easiness of his temper, whether he might not\n"
        "spend the remainder of his days at Netherfield, and leave\n"
        "the next generation to purchase.\n"
        "\n"
        "His sisters were anxious for his having an estate of his\n"
        "own; but, though he was now only established as a tenant,\n"
        "Miss Bingley was by no means unwilling to preside at his\n"
        "table--nor was Mrs. Hurst, who had married a man of more\n"
        "fashion than fortune, less disposed to consider his house as\n"
        "her home when it suited her. Mr. Bingley had not been of age\n"
        "two years, when he was tempted by an accidental\n"
        "recommendation to look at Netherfield House. He did look at\n"
        "it, and into it for half-an-hour--was pleased with the\n"
        "situation and the principal rooms, satisfied with what the\n"
        "owner said in its praise, and took it immediately.\n"
        "\n"
        "Between him and Darcy there was a very steady friendship, in\n"
        "spite of great opposition of character. Bingley was endeared\n"
        "to Darcy by the easiness, openness, and ductility of his\n"
        "temper, though no disposition could offer a greater contrast\n"
        "to his own, and though with his own he never appeared\n"
        "dissatisfied. On the strength of Darcy's regard, Bingley had\n"
        "the firmest reliance, and of his judgement the highest\n"
        "opinion.  In understanding, Darcy was the superior. Bingley\n"
        "was by no means deficient, but Darcy was clever. He was at\n"
        "the same time haughty, reserved, and fastidious, and his\n"
        "manners, though well-bred, were not inviting. In that\n"
        "respect his friend had greatly the advantage. Bingley was\n"
        "sure of being liked wherever he appeared, Darcy was\n"
        "continually giving offense.\n"
        "\n"
        "The manner in which they spoke of the Meryton assembly was\n"
        "sufficiently characteristic. Bingley had never met with more\n"
        "pleasant people or prettier girls in his life; everybody had\n"
        "been most kind and attentive to him; there had been no\n"
        "formality, no stiffness; he had soon felt acquainted with\n"
        "all the room; and, as to Miss Bennet, he could not conceive\n"
        "an angel more beautiful. Darcy, on the contrary, had seen a\n"
        "collection of people in whom there was little beauty and no\n"
        "fashion, for none of whom he had felt the smallest interest,\n"
        "and from none received either attention or pleasure. Miss\n"
        "Bennet he acknowledged to be pretty, but she smiled too\n"
        "much.\n"
        "\n"
        "Mrs. Hurst and her sister allowed it to be so--but still\n"
        "they admired her and liked her, and pronounced her to be a\n"
        "sweet girl, and one whom they would not object to know more\n"
        "of. Miss Bennet was therefore established as a sweet girl,\n"
        "and their brother felt authorized by such commendation to\n"
        "think of her as he chose.\n"
        "\n"
        "\n"
        "\n"
        "Chapter 5\n"
        "\n"
        "\n"
        "Within a short walk of Longbourn lived a family with whom\n"
        "the Bennets were particularly intimate. Sir William Lucas\n"
        "had been formerly in trade in Meryton, where he had made a\n"
        "tolerable fortune, and risen to the honour of knighthood by\n"
        "an address to the king during his mayoralty.  The\n"
        "distinction had perhaps been felt too strongly. It had given\n"
        "him a disgust to his business, and to his residence in a\n"
        "small market town; and, in quitting them both, he had\n"
        "removed with his family to a house about a mile from\n"
        "Meryton, denominated from that period Lucas Lodge, where he\n"
        "could think with pleasure of his own importance, and,\n"
        "unshackled by business, occupy himself solely in being civil\n"
        "to all the world. For, though elated by his rank, it did not\n"
        "render him supercilious; on the contrary, he was all\n"
        "attention to everybody. By nature inoffensive, friendly, and\n"
        "obliging, his presentation at St.  James's had made him\n"
        "courteous.\n"
        "\n"
        "Lady Lucas was a very good kind of woman, not too clever to\n"
        "be a valuable neighbour to Mrs. Bennet. They had several\n"
        "children. The eldest of them, a sensible, intelligent young\n"
        "woman, about twenty-seven, was Elizabeth's intimate friend.\n"
        "\n"
        "That the Miss Lucases and the Miss Bennets should meet to\n"
        "talk over a ball was absolutely necessary; and the morning\n"
        "after the assembly brought the former to Longbourn to hear\n"
        "and to communicate.\n"
        "\n"
        "\"_You_ began the evening well, Charlotte,\" said Mrs. Bennet\n"
        "with civil self-command to Miss Lucas. \"_You_ were Mr.\n"
        "Bingley's first choice.\"\n"
        "\n"
        "\"Yes; but he seemed to like his second better.\"\n"
        "\n"
        "\"Oh! you mean Jane, I suppose, because he danced with her\n"
        "twice. To be sure that _did_ seem as if he admired\n"
        "her--indeed I rather believe he _did_--I heard something\n"
        "about it--but I hardly know what--something about Mr.\n"
        "Robinson.\"\n"
        "\n"
        "\"Perhaps you mean what I overheard between him and Mr.\n"
        "Robinson; did not I mention it to you? Mr. Robinson's asking\n"
        "him how he liked our Meryton assemblies, and whether he did\n"
        "not think there were a great many pretty women in the room,\n"
        "and _which_ he thought the prettiest?  and his answering\n"
        "immediately to the last question: 'Oh! the eldest Miss\n"
        "Bennet, beyond a doubt; there cannot be two opinions on that\n"
        "point.'\"\n"
        "\n"
        "\"Upon my word! Well, that is very decided indeed--that does\n"
        "seem as if--but, however, it may all come to nothing, you\n"
        "know.\"\n"
        "\n"
        "\"_My_ overhearings were more to the purpose than _yours_,\n"
        "Eliza,\" said Charlotte. \"Mr. Darcy is not so well worth\n"
        "listening to as his friend, is he?--poor Eliza!--to be only\n"
        "just _tolerable_.\"\n"
        "\n"
        "\"I beg you would not put it into Lizzy's head to be vexed by\n"
        "his ill-treatment, for he is such a disagreeable man, that\n"
        "it would be quite a misfortune to be liked by him. Mrs. Long\n"
        "told me last night that he sat close to her for half-an-hour\n"
        "without once opening his lips.\"\n"
        "\n"
        "\"Are you quite sure, ma'am?--is not there a little mistake?\"\n"
        "said Jane.  \"I certainly saw Mr. Darcy speaking to her.\"\n"
        "\n"
        "\"Aye--because she asked him at last how he liked\n"
        "Netherfield, and he could not help answering her; but she\n"
        "said he seemed quite angry at being spoke to.\"\n"
        "\n"
        "\"Miss Bingley told me,\" said Jane, \"that he never speaks\n"
        "much, unless among his intimate acquaintances. With _them_\n"
        "he is remarkably agreeable.\"\n"
        "\n"
        "\"I do not believe a word of it, my dear. If he had been so\n"
        "very agreeable, he would have talked to Mrs. Long. But I can\n"
        "guess how it was; everybody says that he is eat up with\n"
        "pride, and I dare say he had heard somehow that Mrs. Long\n"
        "does not keep a carriage, and had come to the ball in a hack\n"
        "chaise.\"\n"
        "\n"
        "\"I do not mind his not talking to Mrs. Long,\" said Miss\n"
        "Lucas, \"but I wish he had danced with Eliza.\"\n"
        "\n"
        "\"Another time, Lizzy,\" said her mother, \"I would not dance\n"
        "with _him_, if I were you.\"\n"
        "\n"
        "\"I believe, ma'am, I may safely promise you _never_ to dance\n"
        "with him.\"\n"
        "\n"
        "\"His pride,\" said Miss Lucas, \"does not offend _me_ so much\n"
        "as pride often does, because there is an excuse for it. One\n"
        "cannot wonder that so very fine a young man, with family,\n"
        "fortune, everything in his favour, should think highly of\n"
        "himself. If I may so express it, he has a _right_ to be\n"
        "proud.\"\n"
        "\n"
        "\"That is very true,\" replied Elizabeth, \"and I could easily\n"
        "forgive _his_ pride, if he had not mortified _mine_.\"\n"
        "\n"
        "\"Pride,\" observed Mary, who piqued herself upon the solidity\n"
        "of her reflections, \"is a very common failing, I believe. By\n"
        "all that I have ever read, I am convinced that it is very\n"
        "common indeed; that human nature is particularly prone to\n"
        "it, and that there are very few of us who do not cherish a\n"
        "feeling of self-complacency on the score of some quality or\n"
        "other, real or imaginary. Vanity and pride are different\n"
        "things, though the words are often used synonymously. A\n"
        "person may be proud without being vain. Pride relates more\n"
        "to our opinion of ourselves, vanity to what we would have\n"
        "others think of us.\"\n"
        "\n"
        "\"If I were as rich as Mr. Darcy,\" cried a young Lucas, who\n"
        "came with his sisters, \"I should not care how proud I was. I\n"
        "would keep a pack of foxhounds, and drink a bottle of wine a\n"
        "day.\"\n"
        "\n"
        "\"Then you would drink a great deal more than you ought,\"\n"
        "said Mrs.  Bennet; \"and if I were to see you at it, I should\n"
        "take away your bottle directly.\"\n"
        "\n"
        "The boy protested that she should not; she continued to\n"
        "declare that she would, and the argument ended only with the\n"
        "visit.\n"
        "\n"
        "\n"
        "\n"
// xlC8 has an internal compiler error if this is too long..., so we're
// skipping the rest of the book...
        "********************************************************...\n"
        "*********** rest of book skipped to avoid compile errors...\n"
        "********************************************************...\n"
        "\n"
        "\n"
        "End of the Project Gutenberg EBook of Pride and Prejudice,\n"
        "by Jane Austen\n"
        "\n"
        "*** END OF THIS PROJECT GUTENBERG EBOOK PRIDE AND PREJUDICE\n"
        "***\n"
        "\n"
        "***** This file should be named 1342.txt or 1342.zip *****\n"
        "This and all associated files of various formats will be\n"
        "found in: http://www.gutenberg.org/1/3/4/1342/\n"
        "\n"
        "Produced by Anonymous Volunteers\n"
        "\n"
        "Updated editions will replace the previous one--the old\n"
        "editions will be renamed.\n"
        "\n"
        "Creating the works from public domain print editions means\n"
        "that no one owns a United States copyright in these works,\n"
        "so the Foundation (and you!) can copy and distribute it in\n"
        "the United States without permission and without paying\n"
        "copyright royalties.  Special rules, set forth in the\n"
        "General Terms of Use part of this license, apply to copying\n"
        "and distributing Project Gutenberg-tm electronic works to\n"
        "protect the PROJECT GUTENBERG-tm concept and trademark.\n"
        "Project Gutenberg is a registered trademark, and may not be\n"
        "used if you charge for the eBooks, unless you receive\n"
        "specific permission.  If you do not charge anything for\n"
        "copies of this eBook, complying with the rules is very easy.\n"
        "You may use this eBook for nearly any purpose such as\n"
        "creation of derivative works, reports, performances and\n"
        "research.  They may be modified and printed and given\n"
        "away--you may do practically ANYTHING with public domain\n"
        "eBooks.  Redistribution is subject to the trademark license,\n"
        "especially commercial redistribution.\n"
        "\n"
        "\n"
        "\n"
        "*** START: FULL LICENSE ***\n"
        "\n"
        "THE FULL PROJECT GUTENBERG LICENSE PLEASE READ THIS BEFORE\n"
        "YOU DISTRIBUTE OR USE THIS WORK\n"
        "\n"
        "To protect the Project Gutenberg-tm mission of promoting the\n"
        "free distribution of electronic works, by using or\n"
        "distributing this work (or any other work associated in any\n"
        "way with the phrase \"Project Gutenberg\"), you agree to\n"
        "comply with all the terms of the Full Project Gutenberg-tm\n"
        "License (available with this file or online at\n"
        "http://gutenberg.org/license).\n"
        "\n"
        "\n"
        "Section 1.  General Terms of Use and Redistributing Project\n"
        "Gutenberg-tm electronic works\n"
        "\n"
        "1.A.  By reading or using any part of this Project\n"
        "Gutenberg-tm electronic work, you indicate that you have\n"
        "read, understand, agree to and accept all the terms of this\n"
        "license and intellectual property (trademark/copyright)\n"
        "agreement.  If you do not agree to abide by all the terms of\n"
        "this agreement, you must cease using and return or destroy\n"
        "all copies of Project Gutenberg-tm electronic works in your\n"
        "possession.  If you paid a fee for obtaining a copy of or\n"
        "access to a Project Gutenberg-tm electronic work and you do\n"
        "not agree to be bound by the terms of this agreement, you\n"
        "may obtain a refund from the person or entity to whom you\n"
        "paid the fee as set forth in paragraph 1.E.8.\n"
        "\n"
        "1.B.  \"Project Gutenberg\" is a registered trademark.  It may\n"
        "only be used on or associated in any way with an electronic\n"
        "work by people who agree to be bound by the terms of this\n"
        "agreement.  There are a few things that you can do with most\n"
        "Project Gutenberg-tm electronic works even without complying\n"
        "with the full terms of this agreement.  See paragraph 1.C\n"
        "below.  There are a lot of things you can do with Project\n"
        "Gutenberg-tm electronic works if you follow the terms of\n"
        "this agreement and help preserve free future access to\n"
        "Project Gutenberg-tm electronic works.  See paragraph 1.E\n"
        "below.\n"
        "\n"
        "1.C.  The Project Gutenberg Literary Archive Foundation\n"
        "(\"the Foundation\" or PGLAF), owns a compilation copyright in\n"
        "the collection of Project Gutenberg-tm electronic works.\n"
        "Nearly all the individual works in the collection are in the\n"
        "public domain in the United States.  If an individual work\n"
        "is in the public domain in the United States and you are\n"
        "located in the United States, we do not claim a right to\n"
        "prevent you from copying, distributing, performing,\n"
        "displaying or creating derivative works based on the work as\n"
        "long as all references to Project Gutenberg are removed.  Of\n"
        "course, we hope that you will support the Project\n"
        "Gutenberg-tm mission of promoting free access to electronic\n"
        "works by freely sharing Project Gutenberg-tm works in\n"
        "compliance with the terms of this agreement for keeping the\n"
        "Project Gutenberg-tm name associated with the work.  You can\n"
        "easily comply with the terms of this agreement by keeping\n"
        "this work in the same format with its attached full Project\n"
        "Gutenberg-tm License when you share it without charge with\n"
        "others.\n"
        "\n"
        "1.D.  The copyright laws of the place where you are located\n"
        "also govern what you can do with this work.  Copyright laws\n"
        "in most countries are in a constant state of change.  If you\n"
        "are outside the United States, check the laws of your\n"
        "country in addition to the terms of this agreement before\n"
        "downloading, copying, displaying, performing, distributing\n"
        "or creating derivative works based on this work or any other\n"
        "Project Gutenberg-tm work.  The Foundation makes no\n"
        "representations concerning the copyright status of any work\n"
        "in any country outside the United States.\n"
        "\n"
        "1.E.  Unless you have removed all references to Project\n"
        "Gutenberg:\n"
        "\n"
        "1.E.1.  The following sentence, with active links to, or\n"
        "other immediate access to, the full Project Gutenberg-tm\n"
        "License must appear prominently whenever any copy of a\n"
        "Project Gutenberg-tm work (any work on which the phrase\n"
        "\"Project Gutenberg\" appears, or with which the phrase\n"
        "\"Project Gutenberg\" is associated) is accessed, displayed,\n"
        "performed, viewed, copied or distributed:\n"
        "\n"
        "This eBook is for the use of anyone anywhere at no cost and\n"
        "with almost no restrictions whatsoever.  You may copy it,\n"
        "give it away or re-use it under the terms of the Project\n"
        "Gutenberg License included with this eBook or online at\n"
        "www.gutenberg.org\n"
        "\n"
        "1.E.2.  If an individual Project Gutenberg-tm electronic\n"
        "work is derived from the public domain (does not contain a\n"
        "notice indicating that it is posted with permission of the\n"
        "copyright holder), the work can be copied and distributed to\n"
        "anyone in the United States without paying any fees or\n"
        "charges.  If you are redistributing or providing access to a\n"
        "work with the phrase \"Project Gutenberg\" associated with or\n"
        "appearing on the work, you must comply either with the\n"
        "requirements of paragraphs 1.E.1 through 1.E.7 or obtain\n"
        "permission for the use of the work and the Project\n"
        "Gutenberg-tm trademark as set forth in paragraphs 1.E.8 or\n"
        "1.E.9.\n"
        "\n"
        "1.E.3.  If an individual Project Gutenberg-tm electronic\n"
        "work is posted with the permission of the copyright holder,\n"
        "your use and distribution must comply with both paragraphs\n"
        "1.E.1 through 1.E.7 and any additional terms imposed by the\n"
        "copyright holder.  Additional terms will be linked to the\n"
        "Project Gutenberg-tm License for all works posted with the\n"
        "permission of the copyright holder found at the beginning of\n"
        "this work.\n"
        "\n"
        "1.E.4.  Do not unlink or detach or remove the full Project\n"
        "Gutenberg-tm License terms from this work, or any files\n"
        "containing a part of this work or any other work associated\n"
        "with Project Gutenberg-tm.\n"
        "\n"
        "1.E.5.  Do not copy, display, perform, distribute or\n"
        "redistribute this electronic work, or any part of this\n"
        "electronic work, without prominently displaying the sentence\n"
        "set forth in paragraph 1.E.1 with active links or immediate\n"
        "access to the full terms of the Project Gutenberg-tm\n"
        "License.\n"
        "\n"
        "1.E.6.  You may convert to and distribute this work in any\n"
        "binary, compressed, marked up, nonproprietary or proprietary\n"
        "form, including any word processing or hypertext form.\n"
        "However, if you provide access to or distribute copies of a\n"
        "Project Gutenberg-tm work in a format other than \"Plain\n"
        "Vanilla ASCII\" or other format used in the official version\n"
        "posted on the official Project Gutenberg-tm web site\n"
        "(www.gutenberg.org), you must, at no additional cost, fee or\n"
        "expense to the user, provide a copy, a means of exporting a\n"
        "copy, or a means of obtaining a copy upon request, of the\n"
        "work in its original \"Plain Vanilla ASCII\" or other form.\n"
        "Any alternate format must include the full Project\n"
        "Gutenberg-tm License as specified in paragraph 1.E.1.\n"
        "\n"
        "1.E.7.  Do not charge a fee for access to, viewing,\n"
        "displaying, performing, copying or distributing any Project\n"
        "Gutenberg-tm works unless you comply with paragraph 1.E.8 or\n"
        "1.E.9.\n"
        "\n"
        "1.E.8.  You may charge a reasonable fee for copies of or\n"
        "providing access to or distributing Project Gutenberg-tm\n"
        "electronic works provided that\n"
        "\n"
        "- You pay a royalty fee of 20% of the gross profits you\n"
        "  derive from the use of Project Gutenberg-tm works\n"
        "  calculated using the method you already use to calculate\n"
        "  your applicable taxes.  The fee is owed to the owner of\n"
        "  the Project Gutenberg-tm trademark, but he has agreed to\n"
        "  donate royalties under this paragraph to the Project\n"
        "  Gutenberg Literary Archive Foundation.  Royalty payments\n"
        "  must be paid within 60 days following each date on which\n"
        "  you prepare (or are legally required to prepare) your\n"
        "  periodic tax returns.  Royalty payments should be clearly\n"
        "  marked as such and sent to the Project Gutenberg Literary\n"
        "  Archive Foundation at the address specified in Section 4,\n"
        "  \"Information about donations to the Project Gutenberg\n"
        "  Literary Archive Foundation.\"\n"
        "\n"
        "- You provide a full refund of any money paid by a user who\n"
        "  notifies you in writing (or by e-mail) within 30 days of\n"
        "  receipt that s/he does not agree to the terms of the full\n"
        "  Project Gutenberg-tm License.  You must require such a\n"
        "  user to return or destroy all copies of the works\n"
        "  possessed in a physical medium and discontinue all use of\n"
        "  and all access to other copies of Project Gutenberg-tm\n"
        "  works.\n"
        "\n"
        "- You provide, in accordance with paragraph 1.F.3, a full\n"
        "  refund of any money paid for a work or a replacement copy,\n"
        "  if a defect in the electronic work is discovered and\n"
        "      reported to you within 90 days of receipt of the work.\n"
        "\n"
        "- You comply with all other terms of this agreement for free\n"
        "  distribution of Project Gutenberg-tm works.\n"
        "\n"
        "1.E.9.  If you wish to charge a fee or distribute a Project\n"
        "Gutenberg-tm electronic work or group of works on different\n"
        "terms than are set forth in this agreement, you must obtain\n"
        "permission in writing from both the Project Gutenberg\n"
        "Literary Archive Foundation and Michael Hart, the owner of\n"
        "the Project Gutenberg-tm trademark.  Contact the Foundation\n"
        "as set forth in Section 3 below.\n"
        "\n"
        "1.F.\n"
        "\n"
        "1.F.1.  Project Gutenberg volunteers and employees expend\n"
        "considerable effort to identify, do copyright research on,\n"
        "transcribe and proofread public domain works in creating the\n"
        "Project Gutenberg-tm collection.  Despite these efforts,\n"
        "Project Gutenberg-tm electronic works, and the medium on\n"
        "which they may be stored, may contain \"Defects,\" such as,\n"
        "but not limited to, incomplete, inaccurate or corrupt data,\n"
        "transcription errors, a copyright or other intellectual\n"
        "property infringement, a defective or damaged disk or other\n"
        "medium, a computer virus, or computer codes that damage or\n"
        "cannot be read by your equipment.\n"
        "\n"
        "1.F.2.  LIMITED WARRANTY, DISCLAIMER OF DAMAGES - Except for\n"
        "the \"Right of Replacement or Refund\" described in paragraph\n"
        "1.F.3, the Project Gutenberg Literary Archive Foundation,\n"
        "the owner of the Project Gutenberg-tm trademark, and any\n"
        "other party distributing a Project Gutenberg-tm electronic\n"
        "work under this agreement, disclaim all liability to you for\n"
        "damages, costs and expenses, including legal fees.  YOU\n"
        "AGREE THAT YOU HAVE NO REMEDIES FOR NEGLIGENCE, STRICT\n"
        "LIABILITY, BREACH OF WARRANTY OR BREACH OF CONTRACT EXCEPT\n"
        "THOSE PROVIDED IN PARAGRAPH F3.  YOU AGREE THAT THE\n"
        "FOUNDATION, THE TRADEMARK OWNER, AND ANY DISTRIBUTOR UNDER\n"
        "THIS AGREEMENT WILL NOT BE LIABLE TO YOU FOR ACTUAL, DIRECT,\n"
        "INDIRECT, CONSEQUENTIAL, PUNITIVE OR INCIDENTAL DAMAGES EVEN\n"
        "IF YOU GIVE NOTICE OF THE POSSIBILITY OF SUCH DAMAGE.\n"
        "\n"
        "1.F.3.  LIMITED RIGHT OF REPLACEMENT OR REFUND - If you\n"
        "discover a defect in this electronic work within 90 days of\n"
        "receiving it, you can receive a refund of the money (if any)\n"
        "you paid for it by sending a written explanation to the\n"
        "person you received the work from.  If you received the work\n"
        "on a physical medium, you must return the medium with your\n"
        "written explanation.  The person or entity that provided you\n"
        "with the defective work may elect to provide a replacement\n"
        "copy in lieu of a refund.  If you received the work\n"
        "electronically, the person or entity providing it to you may\n"
        "choose to give you a second opportunity to receive the work\n"
        "electronically in lieu of a refund.  If the second copy is\n"
        "also defective, you may demand a refund in writing without\n"
        "further opportunities to fix the problem.\n"
        "\n"
        "1.F.4.  Except for the limited right of replacement or\n"
        "refund set forth in paragraph 1.F.3, this work is provided\n"
        "to you 'AS-IS' WITH NO OTHER WARRANTIES OF ANY KIND, EXPRESS\n"
        "OR IMPLIED, INCLUDING BUT NOT LIMITED TO WARRANTIES OF\n"
        "MERCHANTIBILITY OR FITNESS FOR ANY PURPOSE.\n"
        "\n"
        "1.F.5.  Some states do not allow disclaimers of certain\n"
        "implied warranties or the exclusion or limitation of certain\n"
        "types of damages.  If any disclaimer or limitation set forth\n"
        "in this agreement violates the law of the state applicable\n"
        "to this agreement, the agreement shall be interpreted to\n"
        "make the maximum disclaimer or limitation permitted by the\n"
        "applicable state law.  The invalidity or unenforceability of\n"
        "any provision of this agreement shall not void the remaining\n"
        "provisions.\n"
        "\n"
        "1.F.6.  INDEMNITY - You agree to indemnify and hold the\n"
        "Foundation, the trademark owner, any agent or employee of\n"
        "the Foundation, anyone providing copies of Project\n"
        "Gutenberg-tm electronic works in accordance with this\n"
        "agreement, and any volunteers associated with the\n"
        "production, promotion and distribution of Project\n"
        "Gutenberg-tm electronic works, harmless from all liability,\n"
        "costs and expenses, including legal fees, that arise\n"
        "directly or indirectly from any of the following which you\n"
        "do or cause to occur: (a) distribution of this or any\n"
        "    Project Gutenberg-tm work, (b) alteration, modification,\n"
        "    or additions or deletions to any Project Gutenberg-tm\n"
        "    work, and (c) any Defect you cause.\n"
        "\n"
        "\n"
        "Section  2.  Information about the Mission of Project\n"
        "Gutenberg-tm\n"
        "\n"
        "Project Gutenberg-tm is synonymous with the free\n"
        "distribution of electronic works in formats readable by the\n"
        "widest variety of computers including obsolete, old,\n"
        "middle-aged and new computers.  It exists because of the\n"
        "efforts of hundreds of volunteers and donations from people\n"
        "in all walks of life.\n"
        "\n"
        "Volunteers and financial support to provide volunteers with\n"
        "the assistance they need, is critical to reaching Project\n"
        "Gutenberg-tm's goals and ensuring that the Project\n"
        "Gutenberg-tm collection will remain freely available for\n"
        "generations to come.  In 2001, the Project Gutenberg\n"
        "Literary Archive Foundation was created to provide a secure\n"
        "and permanent future for Project Gutenberg-tm and future\n"
        "generations.  To learn more about the Project Gutenberg\n"
        "Literary Archive Foundation and how your efforts and\n"
        "donations can help, see Sections 3 and 4 and the Foundation\n"
        "web page at http://www.pglaf.org.\n"
        "\n"
        "\n"
        "Section 3.  Information about the Project Gutenberg Literary\n"
        "Archive Foundation\n"
        "\n"
        "The Project Gutenberg Literary Archive Foundation is a non\n"
        "profit 501(c)(3) educational corporation organized under the\n"
        "laws of the state of Mississippi and granted tax exempt\n"
        "status by the Internal Revenue Service.  The Foundation's\n"
        "EIN or federal tax identification number is 64-6221541.  Its\n"
        "501(c)(3) letter is posted at http://pglaf.org/fundraising.\n"
        "Contributions to the Project Gutenberg Literary Archive\n"
        "Foundation are tax deductible to the full extent permitted\n"
        "by U.S. federal laws and your state's laws.\n"
        "\n"
        "The Foundation's principal office is located at 4557 Melan\n"
        "Dr. S.  Fairbanks, AK, 99712., but its volunteers and\n"
        "employees are scattered throughout numerous locations.  Its\n"
        "business office is located at 809 North 1500 West, Salt Lake\n"
        "City, UT 84116, (801) 596-1887, email business@pglaf.org.\n"
        "Email contact links and up to date contact information can\n"
        "be found at the Foundation's web site and official page at\n"
        "http://pglaf.org\n"
        "\n"
        "For additional contact information: Dr. Gregory B. Newby\n"
        "Chief Executive and Director gbnewby@pglaf.org\n"
        "\n"
        "\n"
        "Section 4.  Information about Donations to the Project\n"
        "Gutenberg Literary Archive Foundation\n"
        "\n"
        "Project Gutenberg-tm depends upon and cannot survive without\n"
        "wide spread public support and donations to carry out its\n"
        "mission of increasing the number of public domain and\n"
        "licensed works that can be freely distributed in machine\n"
        "readable form accessible by the widest array of equipment\n"
        "including outdated equipment.  Many small donations ($1 to\n"
        "$5,000) are particularly important to maintaining tax exempt\n"
        "status with the IRS.\n"
        "\n"
        "The Foundation is committed to complying with the laws\n"
        "regulating charities and charitable donations in all 50\n"
        "states of the United States.  Compliance requirements are\n"
        "not uniform and it takes a considerable effort, much\n"
        "paperwork and many fees to meet and keep up with these\n"
        "requirements.  We do not solicit donations in locations\n"
        "where we have not received written confirmation of\n"
        "compliance.  To SEND DONATIONS or determine the status of\n"
        "compliance for any particular state visit http://pglaf.org\n"
        "\n"
        "While we cannot and do not solicit contributions from states\n"
        "where we have not met the solicitation requirements, we know\n"
        "of no prohibition against accepting unsolicited donations\n"
        "from donors in such states who approach us with offers to\n"
        "donate.\n"
        "\n"
        "International donations are gratefully accepted, but we\n"
        "cannot make any statements concerning tax treatment of\n"
        "donations received from outside the United States.  U.S.\n"
        "laws alone swamp our small staff.\n"
        "\n"
        "Please check the Project Gutenberg Web pages for current\n"
        "donation methods and addresses.  Donations are accepted in a\n"
        "number of other ways including checks, online payments and\n"
        "credit card donations.  To donate, please visit:\n"
        "http://pglaf.org/donate\n"
        "\n"
        "\n"
        "Section 5.  General Information About Project Gutenberg-tm\n"
        "electronic works.\n"
        "\n"
        "Professor Michael S. Hart is the originator of the Project\n"
        "Gutenberg-tm concept of a library of electronic works that\n"
        "could be freely shared with anyone.  For thirty years, he\n"
        "produced and distributed Project Gutenberg-tm eBooks with\n"
        "only a loose network of volunteer support.\n"
        "\n"
        "\n"
        "Project Gutenberg-tm eBooks are often created from several\n"
        "printed editions, all of which are confirmed as Public\n"
        "Domain in the U.S.  unless a copyright notice is included.\n"
        "Thus, we do not necessarily keep eBooks in compliance with\n"
        "any particular paper edition.\n"
        "\n"
        "\n"
        "Most people start at our Web site which has the main PG\n"
        "search facility:\n"
        "\n"
        "     http://www.gutenberg.org\n"
        "\n"
        "This Web site includes information about Project\n"
        "Gutenberg-tm, including how to make donations to the Project\n"
        "Gutenberg Literary Archive Foundation, how to help produce\n"
        "our new eBooks, and how to subscribe to our email newsletter\n"
        "to hear about new eBooks.\n"
    };

    bsl::size_t prideLen = sizeof(prideAndPrejudice);

    unsigned short *utf16Buffer_p = new unsigned short[prideLen];
    char           *utf8Buffer_p = new char[prideLen];
    bsl::size_t     codePointsWritten = 0;
    bsl::size_t     bytesWritten = 0;

    const int       iterLimit    = 1000;

    bsls::Stopwatch s;

    s.start();

    for (int i = 0; i < iterLimit; ++i) {
        ASSERT(SUCCESS == bdlde::CharConvertUtf16::utf8ToUtf16(
                                                          utf16Buffer_p,
                                                          prideLen,
                                                          prideAndPrejudice,
                                                          &codePointsWritten));
        ASSERT(codePointsWritten == prideLen);

        ASSERT(SUCCESS == bdlde::CharConvertUtf16::utf8ToUtf16(
                                          utf16Buffer_p,
                                          prideLen,
                                          bsl::string_view(prideAndPrejudice),
                                          &codePointsWritten));
        ASSERT(codePointsWritten == prideLen);

        ASSERT(0 == bdlde::CharConvertUtf16::utf16ToUtf8(utf8Buffer_p,
                                                        prideLen,
                                                        utf16Buffer_p,
                                                        &codePointsWritten,
                                                        &bytesWritten));
        ASSERT(codePointsWritten == prideLen);
        ASSERT(bytesWritten == prideLen);
    }

    s.stop();

    cout << "Performance test, converted " << prideLen << " code points "
         << "back and forth " << iterLimit << " times in " << s.elapsedTime()
         << " seconds." << endl;

    ASSERT(0 == strcmp(utf8Buffer_p, prideAndPrejudice));

    delete [] utf8Buffer_p;
    delete [] utf16Buffer_p;

    return 0;
}

template< class CHAR_TYPE, class ITER >
void fillArray(ITER first, ITER last, CHAR_TYPE ch)
{
    for ( ; first != last; ++first) {
        *first = ch;
    }
}

template< class CHAR_TYPE, class ITER >
int checkFill(ITER first, ITER last, CHAR_TYPE ch)
{
    for ( ; first != last; ++first) {
        if (ch != *first) {
            return 0;                                                 // RETURN
        }
    }
    return 1;
}

ostream& operator<<(ostream& os, const ConvRslt& cvr)
{
    ios_base::fmtflags flags = os.flags();

    return os << "ret 0x" << hex << cvr.d_retVal
              << setbase( flags )
              << " symbols " << cvr.d_symbols << " units " << cvr.d_units ;
}

template<class CHAR_TYPE>
void WorkPiece<CHAR_TYPE>::fillMargins(CHAR_TYPE* mem)
{
    fillArray(&mem[0], &mem[d_margin], d_fillChar);
           // from start up to winStart

    fillArray(&mem[d_margin + d_winLength],
              &mem[2 * d_margin + d_winLength],
              d_fillChar) ;
        // from just beyond the working memory window to vector's end.
}

template<class CHAR_TYPE>
int WorkPiece<CHAR_TYPE>::checkMargins(const CHAR_TYPE* mem) const
{
    return checkFill(&mem[0], &mem[d_margin], d_fillChar)
           // from start up to winStart
        && checkFill(&mem[d_margin + d_winLength],
                     &mem[2 * d_margin + d_winLength],
                     d_fillChar);
           // from just beyond the working memory window to margin's end.
}

template<class CHAR_TYPE>
bool operator==(const ConstArrayRange<CHAR_TYPE>& lhs,
                const ConstArrayRange<CHAR_TYPE>& rhs)
{
    return lhs.d_size != rhs.d_size ?
                        0           :
                        0 == memcmp(lhs.d_array, rhs.d_array, lhs.d_size);
}

template<class CHAR_TYPE>
ostream& operator<<(ostream&                     os,
                    const ArrayRange<CHAR_TYPE>& sv)
{
    ios_base::fmtflags flags = os.flags();
    char fill = os.fill( ' ' );
    os << bsl::right << "[";

    // The tricky part here is that we want to print graphic code points as
    // code points and everything else as hex.  The settings have to be
    // restored!  This is set up to change the base between runs of graphic and
    // non-graphic code points; that's probably excessive.  Other things are
    // restored at the end.  ('graphic' means ( 'printable' and not the space
    // code point ).)

    for (const CHAR_TYPE* cp = sv.d_arrayr; *cp;) {
        for (; *cp
           && static_cast<unsigned int>(*cp) < 0x80
           && isgraph(*cp); ++cp) {
            os << ' ' << setw(4) << static_cast<signed char>(*cp);
        }
        if (! *cp) {
            break;
        }
        os << hex;
        for (; static_cast<unsigned int>(*cp) >= 0x80
            ||   (*cp
              && ! isgraph(*cp)); ++cp) {
            os << ' ' << setw(4) << deChar(*cp);
        }
        os << dec;
    }
    os << " ]";

    os.fill(fill);
    os.flags(flags);
    return os;
}

template<class TO_CHAR, class FROM_CHAR, class FILL_CHECK>
bool testOneErrorCharConversion(int                          line,
                                ArrayRange<TO_CHAR> const&   toBuf,
                                ArrayRange<FROM_CHAR> const& fromBuf,
                                FILL_CHECK&                  fillCheck)
    //: o 'line': '__LINE__' where this function is invoked
    //: o 'toBuf', 'fromBuf': Workspaces provided by our caller.  We depend on
    //:   the value of 'fromBuf.size()'.
    //: o 'fillCheck': Source of the octet or word sequence being tested.
    //
    // The 'testOneErrorCharConversion' templated function is the common part
    // of all the subtests in test 4.  It verifies that the error conversion
    // occurs as expected (using the 'RUN_AND_CHECK' macro, which also verifies
    // that the data surrounding the output buffer are unchanged) and that the
    // source buffer is unchanged.  It performs this verification for the error
    // sequence alone in a string and for the sequence surrounded by two
    // single-octet code points, and with and without error replacement
    // code points (a total of four tests).  It returns 'true' if all the tests
    // succeed, or 'false' if any have failed.
{
    enum {
        BUFFER_ZONE = 32     // Margin to fill and check on the output buffer.
    };

    bool failed = false;

    ConvRslt result;

    // First, by itself, nothing around it.

    if (veryVerbose) {
        cout << " - Single octet, no replacement char" << endl;
    }

    Conversion<TO_CHAR, FROM_CHAR> conversion(
                                ConversionArg<TO_CHAR, FROM_CHAR>::arg());

    FROM_CHAR* from = fromBuf.begin();

    fillCheck.fill(from);
    from[fillCheck.size()] = 0;

    SrcSpec<FROM_CHAR> source(fromBuf.begin(), 0, 1);  // No error char, output
                                                       // length 1 (the null)
    ConvRslt expected(0x1, 1, 1);    // Illegal octet, one symbol, one memory
                                     // unit (the null)

    WorkPiece<TO_CHAR> wp(toBuf.size(),
                          source.d_dstBufSize,
                          (TO_CHAR) ~ (TO_CHAR) 0,
                          BUFFER_ZONE);

    TO_CHAR* to = toBuf.begin();
    TO_CHAR* window = wp.begin(to);
    TO_CHAR* winEnd = wp.end(to);
    if (! RUN_AND_CHECK(wp, to, result, conversion, source, expected)) {
        cout << "\tFailed on converting " << prHexRange(fromBuf)
             << " from line " << line << "/" << __LINE__ << "." << endl;

        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }
    else if (! EXPECTED_GOT(0,window[0])) {
        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }

    if (!fillCheck.check(from)) {
        ASSERT(fillCheck.check(from));
        cout << "\t(Source array damaged at line " << line << "/" << __LINE__
             << ".)" << endl;

        if (veryVerbose) {
            cout << prHexRange(from, from + fillCheck.size()) << endl;
        }

        failed = true;
    }

    // Second, surrounded by one single-octet char on each side.

    if (veryVerbose) {
        cout << " - Surrounded by 'A' and 'B', no replacement char" << endl;
    }

    from[0] = 'A';
    fillCheck.fill(from + 1);
    FROM_CHAR* after = from + 1 + fillCheck.size();
    after[0] = 'B';
    after[1] = '\0';

    source = SrcSpec<FROM_CHAR>(from, 0, 3); // No error char, output length 3
    expected = ConvRslt(0x1, 3, 3);   // Illegal octet, three symbols, three
                                      // memory units AB\0

    wp = WorkPiece<TO_CHAR>(toBuf.size(),
                            source.d_dstBufSize,
                            (TO_CHAR) ~ (TO_CHAR) 0,
                            BUFFER_ZONE);

    if (! RUN_AND_CHECK(wp, to, result, conversion, source, expected)) {
        cout << "\tFailed on converting " << prHexRange(fromBuf)
             << " from line " << line << "/" << __LINE__
             << " surrounded by 'A' and 'B'." << endl;

        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }
    else if (! allAnd(EXPECTED_GOT((TO_CHAR) 'A',window[0]),
                      EXPECTED_GOT((TO_CHAR) 'B',window[1]),
                      EXPECTED_GOT((TO_CHAR) 0,window[2]))) {
        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }

    if (!fillCheck.check(from + 1)) {
        ASSERT(fillCheck.check(from + 1));
        cout << "Source array damaged at line " << line << "/" << __LINE__
             << "." << endl;

        if (veryVerbose) {
            cout << prHexRange(from + 1, from + 1 + fillCheck.size()) << endl;
        }

        failed = true;
    }

    // Third, by itself, nothing around it, replacement code point '$'.

    if (veryVerbose) {
        cout << " - Single octet, replacement char '$'" << endl;
    }

    fillCheck.fill(from);
    from[fillCheck.size()] = 0;

    source = SrcSpec<FROM_CHAR>(from, '$', 2);  //  Error char '$',
                                                     //  output length 2
    expected = ConvRslt(0x1, 2, 2);  // Illegal octet, two symbols two words
                                     // ('$' null)

    wp = WorkPiece<TO_CHAR>(toBuf.size(),
                            source.d_dstBufSize,
                            (TO_CHAR) ~ (TO_CHAR) 0,
                            BUFFER_ZONE);

    if (! RUN_AND_CHECK(wp, to, result, conversion, source, expected)) {
        cout << "\tFailed on converting " << prHexRange(fromBuf)
             << " from line " << line << "/" << __LINE__ << "." << endl;

        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }
    else if (! allAnd(EXPECTED_GOT((TO_CHAR) '$',window[0]),
                      EXPECTED_GOT((TO_CHAR) 0,window[1]))) {

        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }

    if (!fillCheck.check(from)) {
        ASSERT(fillCheck.check(from));
        cout << "Source array damaged at line " << line << "/" << __LINE__
             << "." << endl;

        if (veryVerbose) {
            cout << prHexRange(from, from + fillCheck.size()) << endl;
        }

        failed = true;
    }

    // Then surrounded by one single-octet char on each side.

    if (veryVerbose) {
        cout << " - Surrounded by 'A' and 'B', replacement char '#'"
                                                        << endl;
    }

    from[0] = 'A';
    fillCheck.fill(from + 1);
    after[0] = 'B';
    after[1] = '\0';

    source = SrcSpec<FROM_CHAR>(from, '#', 4); // Error char '#', output length
                                               // 4
        expected = ConvRslt(0x1, 4, 4);        // Illegal octet, four symbols,
                                               // four words A#B\0

    wp = WorkPiece<TO_CHAR>(toBuf.size(),
                            source.d_dstBufSize,
                            (TO_CHAR) ~ (TO_CHAR) 0,
                            BUFFER_ZONE);

    if (! RUN_AND_CHECK(wp, to, result, conversion, source, expected)) {
        cout << "\tFailed on converting " << prHexRange(fromBuf)
             << " from line " << line << "/" << __LINE__
             << " surrounded by 'A' and 'B'." << endl;

        if (veryVerbose) {
            cout << prHexRange(window, winEnd) << endl;
        }

        failed = true;
    }
    else if (! allAnd(EXPECTED_GOT((TO_CHAR) 'A',window[0]),
                      EXPECTED_GOT((TO_CHAR) '#',window[1]),
                      EXPECTED_GOT((TO_CHAR) 'B',window[2]),
                      EXPECTED_GOT((TO_CHAR) 0,window[3]))) {
        if (veryVerbose) {
             cout << prHexRange(window, winEnd) << endl;
         }

        failed = true;
    }

    if (!fillCheck.check(from + 1)) {
        ASSERT(fillCheck.check(from + 1));
        cout << "Source array damaged at line " << line << "/" << __LINE__
             << "." << endl;

        if (veryVerbose) {
            cout << prHexRange(from + 1, from + 1 + fillCheck.size()) << endl;
        }

        failed = true;
    }

    return !failed;
}

template<class TO_CHAR,
         class TO_FILL_CHECK,
         class FR_CHAR,
         class FR_FILL_CHECK>
bool oneStringConversion(
            int                      line,          // '__LINE__' of this call
            BufferedWPiece<TO_CHAR>& bwp,           // Destination workspace
            TO_FILL_CHECK&           toFillCheck,   // Reference for checking
                                                    //   the output string.
            ArrayRange<FR_CHAR>&     fromBuf,       // Source workspace buffer
            FR_FILL_CHECK&           fromFillCheck, // Source and reference
                                                    // for checking the input
                                                    // string.
            const ConvRslt&          expected)      // The expected set of
                                                    // return values from the
                                                    // conversion function.
    // The 'oneStringConversion' templated function invokes the conversions
    // for test 5.  It verifies that the conversion returns with the expected
    // values (using the 'RUN_AND_CHECK' macro, which also verifies that the
    // data surrounding the output buffer are unchanged) and that the source
    // buffer is unchanged.  It verifies that the output buffer contains the
    // expected result.  It returns 'true' if all the tests succeed, or 'false'
    // if any have failed.
{
    bool failed = false;

    if (veryVerbose) {
// @+@+@+@+@ decent message here!
        cout << " - " << endl;
// @+@+@+@+@ veryVeryVerbose input string dump?
    }

    Conversion<TO_CHAR, FR_CHAR> conversion(
                                ConversionArg<TO_CHAR, FR_CHAR>::arg());

    FR_CHAR* from = fromBuf.begin();

    fromFillCheck.fill(from);
    from[fromFillCheck.size()] = 0;

    // Use an error char so that the location of errors can be pinpointed.
    // (Note: # is 0x23 .)
    SrcSpec<FR_CHAR> source(fromBuf.begin(), '#', bwp.size());

    ConvRslt result;

    if (! RUN_AND_CHECK(bwp.workpiece(),
                        bwp.buffer(),
                        result,
                        conversion,
                        source,
                        expected)) {
        cout << "\tFailed on converting at line "
             << line << "/" << __LINE__ << "." << endl;

        if (veryVeryVerbose) {
            cout << "From " << prHexRange(fromBuf.begin(), fromBuf.end())
                 << endl;
            cout << "To " << prHexRange(bwp.begin(), bwp.end()) << endl;
        }

        failed = true;
    }
    else if (!toFillCheck.check(bwp.begin())) {
        ASSERT(toFillCheck.check(bwp.begin()));
        cout << "\t(Conversion error at line " << line << "/" << __LINE__
             << ".)" << endl;

        if (veryVeryVerbose) {
            cout << R(prHexRange(bwp.begin(), bwp.end())) << endl;
        }

        failed = true;
    }
// @+@+@+@+@ else { veryVeryVerbose input string dump? }

    if (!fromFillCheck.check(from)) {
        ASSERT(fromFillCheck.check(from));
        cout << "\t(Source array damaged at line " << line << "/" << __LINE__
             << ".)" << endl;

        if (veryVerbose) {
            cout << prHexRange(from, from + fromFillCheck.size()) << endl;
        }

        failed = true;
    }

    return !failed;
}

template<bsl::size_t N>
ostream&
Permuter<N>::print(ostream& os) const
{
    os << "(" ;
    for (int i = 0; i < N; ++i )
            os << " " << d_val[i];
    return os << " )";
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
