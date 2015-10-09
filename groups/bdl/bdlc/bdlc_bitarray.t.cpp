// bdlc_bitarray.t.cpp                                                -*-C++-*-

#include <bdlc_bitarray.h>

#include <bdlb_bitstringutil.h>

#include <bslim_testutil.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>

#include <bslalg_swaputil.h>

#include <bslmf_assert.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bdlma_bufferedsequentialallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_new.h>         // placement syntax

#include <bsl_cctype.h>      // isspace, tolower
#include <bsl_climits.h>     // CHAR_BIT
#include <bsl_cstddef.h>     // size_t
#include <bsl_cstdio.h>      // printf (needed by exception macros)
#include <bsl_cstdlib.h>     // atoi(), rand, RAND_MAX
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()


#undef SS  // Solaris 5.10/x86 sys/regset.h via stdlib.h
#undef ES

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::size_t;
using bsl::strlen;
using bsl::uint64_t;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'append' and 'removeAll' methods to be used by the generator functions 'g'
// and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that each manipulator
// must support aliasing, and those that perform memory allocation must be
// tested for exception neutrality via the 'bslma_testallocator' component.
// Exception neutrality involving streaming is verified using
// 'bslx::testinstream' (and 'bslx::testoutstream').
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [10] int maxSupportedBdexVersion(int);
// [ 2] BitArray(bslma::Allocator *ba);
// [11] BitArray(size_t iLen, *ba = 0);
// [11] BitArray(size_t iLen, bool iVal, *ba = 0);
// [ 7] BitArray(const BitArray& original, *ba = 0);
// [ 2] ~BitArray();
// [ 9] BitArray& operator=(const BitArray& rhs);
// [26] BitArray& operator&=(const BitArray& rhs);
// [26] BitArray& operator-=(const BitArray& rhs);
// [26] BitArray& operator|=(const BitArray& rhs);
// [26] BitArray& operator^=(const BitArray& rhs);
// [20] BitArray& operator<<=(size_t numBits);
// [20] BitArray& operator>>=(size_t numBits);
// [21] void andEqual(size_t index, bool value);
// [25] void andEqual(dstIndex, srcArray, srcIndex, numBits);
// [13] void append(bool value);
// [13] void append(bool value, size_t numBits);
// [13] void append(const BitArray& sa);
// [13] void append(const BitArray& sa, size_t si, size_t nb);
// [14] void assign(size_t di, bool value);
// [14] void assign(size_t di, bool value, size_t numBits);
// [14] void assign(size_t di, const BitArray& sa, size_t si, size_t ne);
// [17] void assign0(size_t index);
// [17] void assign0(size_t index, size_t numBits);
// [17] void assign1(size_t index);
// [17] void assign1(size_t index, size_t numBits);
// [17] void assignAll(bool value);
// [17] void assignAll0();
// [17] void assignAll1();
// [29] void assignBits(size_t index, uint64_t srcBits, size_t numBits);
// [13] void insert(size_t di, bool value);
// [13] void insert(size_t di, bool value, size_t numBits);
// [13] void insert(size_t di, const BitArray& sa);
// [13] void insert(size_t di, const BitArray& sa, size_t si, size_t ne);
// [22] void minusEqual(size_t index, bool value);
// [25] void minusEqual(dstIndex, srcArray, srcIndex, numBits);
// [23] void orEqual(size_t index, bool value);
// [25] void orEqual(dstIndex, srcArray, srcIndex, numBits);
// [13] void remove(size_t index);
// [13] void remove(size_t index, size_t ne);
// [ 2] void removeAll();
// [16] void reserveCapacity(size_t ne);
// [19] void rotateLeft(size_t numBits);
// [19] void rotateRight(size_t numBits);
// [12] void setLength(size_t newLength, bit iVal = false);
// [15] void swapBits(size_t index1, size_t index2);
// [18] void toggle(size_t index);
// [18] void toggle(size_t index, size_t numBits);
// [18] void toggleAll();
// [24] void xorEqual(size_t index, bool value);
// [25] void xorEqual(dstIndex, srcArray, srcIndex, numBits);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 8] void swap(BitArray& other);
// [ 4] bool operator[](size_t index) const;
// [ 4] uint64_t bits(size_t index, size_t numBits) const;
// [27] size_t find0AtMaxIndex(size_t begin, size_t end) const;
// [28] size_t find0AtMinIndex(size_t begin, size_t end) const;
// [27] size_t find1AtMaxIndex(size_t begin, size_t end) const;
// [28] size_t find1AtMinIndex(size_t begin, size_t end) const;
// [ 4] bool isAny0() const;
// [ 4] bool isAny1() const;
// [ 4] bool isEmpty() const;
// [ 4] size_t length() const;
// [ 4] size_t num0() const;
// [30] size_t num0(size_t begin, size_t end);
// [ 4] size_t num1() const;
// [30] size_t num1(size_t begin, size_t end);
// [ 4] bslma::Allocator *allocator() const();
// [10] STREAM& bdexStreamOut(STREAM& stream, version) const;
// [ 5] ostream& print(ostream& stream, int level, int spacesPerLevel);
// [ 6] operator==(const BitArray&, const BitArray&);
// [ 6] operator!=(const BitArray&, const BitArray&);
// [18] operator~(const BitArray& bitArray);
// [26] operator&(const BitArray&, const BitArray&);
// [26] operator-(const BitArray&, const BitArray&);
// [26] operator|(const BitArray&, const BitArray&);
// [26] operator^(const BitArray&, const BitArray&);
// [20] BitArray operator<<(const BitArray& bitArray, size_t numBits);
// [20] BitArray operator>>(const BitArray& bitArray, size_t numBits);
// [ 5] ostream& operator<<(ostream&, const BitArray&);
// [ 8] void swap(BitArray& lhs, BitArray& rhs);
//-----------------------------------------------------------------------------
// [31] USAGE EXAMPLE
// [ 3] BitArray gDispatch(const char *spec);
// [ 3] BitArray& gg(BitArray* object, const char *spec);
// [ 3] BitArray& ggDispatch(BitArray* object, const char *spec);
// [ 3] int ggg(BitArray *object, const char *spec, int vF = 1);
// [ 3] int gggDispatch(BitArray *object, const char *spec, int vF = 1);
// [ 3] void stretchRemoveAll(Obj *object, size_t size);
// [ 3] bsl::string binSpec(const BitArray& object);
// [ 3] bsl::string randSpec(size_t length);
// [ 2] BOOTSTRAP: void append(bool value); // no aliasing
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlc::BitArray                Obj;
typedef bdlb::BitStringUtil           Util;
typedef bsls::Types::Int64            Int64;
typedef bsls::Types::IntPtr           IntPtr;
typedef const Int64                   CInt64;
typedef bool                          Element;

static const uint64_t zero     =     0;
static const uint64_t minusOne = ~zero;
static const uint64_t one      =     1;

static const size_t   k_INVALID_INDEX = Obj::k_INVALID_INDEX;

enum { k_BITS_PER_UINT64 = sizeof(uint64_t) * CHAR_BIT };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
void stretchRemoveAll(Obj *object, size_t size)
    // Using only primary manipulators, extend the capacity of the specified
    // 'object' to (at least) the specified 'size'; then remove all elements
    // leaving 'object' empty.  The behavior is undefined unless '0 <= size'.
{
    ASSERT(object);

    const size_t initSize = object->length();
    for (size_t i = 0; i < size; ++i) {
        object->append(0);
    }
    ASSERT(object->length() == initSize + size);
    object->removeAll();
    ASSERT(0 == object->length());
}

static
bsl::string binSpec(const Obj& object)
    // Output a string that is the binary spec corresponding to the state of
    // the specified 'object'.
{
    size_t len = object.length();

    bsl::string ret;
    ret.reserve(len);

    for (size_t ii = 0; ii < len; ++ii) {
        ret.push_back(object[ii] ? '1' : '0');
    }

    return ret;
}

static
void outerP(const char *leader, const Obj &X)
    // This function is used to avert an uncaught exception on Windows during
    // 'bslma' exception testing.  This can happen, e.g., in test cases with
    // large DATA sets.  Print the specified 'leader', followed by the
    // specified 'X'.
{
    cout << leader; P(X);
}

static inline
void incInt(int *x, const int maxVal)
    // This function is to be used to increment a specified loop counter '*x'
    // with increasing rapidity as it grows, eventually setting it to EXACTLY
    // the specified 'maxVal', after which it will be set to 'maxVal + 1',
    // causing the loop to terminate.
{
    if (maxVal <= *x) {
        ASSERT(maxVal == *x);

        ++*x;
        return;                                                       // RETURN
    }

    *x += bsl::max((*x / 4), 1);
    if (*x > maxVal) {
        *x = maxVal;
    }
}

static inline
void incSizeT(size_t *x, const size_t maxVal)
    // This function is to be used to increment a specified loop counter '*x'
    // with increasing rapidity as it grows, eventually setting it to EXACTLY
    // the specified 'maxVal', after which it will be set to 'maxVal + 1',
    // causing the loop to terminate.
{
    if (maxVal <= *x) {
        ASSERT(maxVal == *x);

        ++*x;
        return;                                                       // RETURN
    }

    *x += bsl::max<size_t>((*x / 4), 1);
    if (*x > maxVal) {
        *x = maxVal;
    }
}

static
size_t countOnesOracle(const Obj& object, size_t begin, size_t end)
    // Return the number of set bits in the specified 'object' in the specified
    // range '[begin .. end)'.  This function uses a much simpler and less
    // efficient algorithm than that employed by 'Obj::num1'.
{
    ASSERT(begin <= end);
    ASSERT(         end <= object.length());

    size_t ret = 0;

    for (size_t ii = begin; ii < end; ++ii) {
        ret += object[ii];
    }

    return ret;
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//..
    template <class TYPE>
    class NullableVector {
        // This class implements a sequential container of elements of the
        // template parameter 'TYPE'.

        // DATA
        bsl::vector<TYPE>  d_values;       // data elements
        bdlc::BitArray     d_nullFlags;    // 'true' indicates i'th element is
                                           // null

      private:
        // NOT IMPLEMENTED
        NullableVector(const NullableVector&);
        NullableVector& operator=(const NullableVector&);

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(NullableVector,
                                       bslma::UsesBslmaAllocator);

      public:
        // CREATORS
        explicit
        NullableVector(bsl::size_t       initialLength,
                       bslma::Allocator *basicAllocator = 0);
            // Construct a vector having the specified 'initialLength' null
            // elements.  Optionally specify a 'basicAllocator' used to supply
            // memory.  If 'basicAllocator' is 0, the currently supplied
            // default allocator is used.

        // ...

        ~NullableVector();
            // Destroy this vector.

        // MANIPULATORS
        void appendNullElement();
            // Append a null element to this vector.  Note that the appended
            // element will have the same value as a default constructed 'TYPE'
            // object.

        void appendElement(const TYPE& value);
            // Append an element having the specified 'value' to the end of
            // this vector.

        void makeNonNull(bsl::size_t index);
            // Make the element at the specified 'index' in this vector
            // non-null.  The behavior is undefined unless 'index < length()'.

        void makeNull(bsl::size_t index);
            // Make the element at the specified 'index' in this vector null.
            // The behavior is undefined unless 'index < length()'.  Note that
            // the new value of the element will be the default constructed
            // value for 'TYPE'.

        TYPE& modifiableElement(bsl::size_t index);
            // Return a reference providing modifiable access to the (valid)
            // element at the specified 'index' in this vector.  The behavior
            // is undefined unless 'index < length()'.  Note that if the
            // element at 'index' is null then the nullness flag is reset and
            // the returned value is the default constructed value for 'TYPE'.

        void removeElement(bsl::size_t index);
            // Remove the element at the specified 'index' in this vector.  The
            // behavior is undefined unless 'index < length()'.

        // ACCESSORS
        const TYPE& constElement(bsl::size_t index) const;
            // Return a reference providing non-modifiable access to the
            // element at the specified 'index' in this vector.  The behavior
            // is undefined unless 'index < length()'.  Note that if the
            // element at 'index' is null then the nullness flag is not reset
            // and the returned value is the default constructed value for
            // 'TYPE'.

        bool isAnyElementNonNull() const;
            // Return 'true' if any element in this vector is non-null, and
            // 'false' otherwise.

        bool isAnyElementNull() const;
            // Return 'true' if any element in this vector is null, and 'false'
            // otherwise.

        bool isElementNull(bsl::size_t index) const;
            // Return 'true' if the element at the specified 'index' in this
            // vector is null, and 'false' otherwise.  The behavior is
            // undefined unless 'index < length()'.

        bsl::size_t length() const;
            // Return the number of elements in this vector.

        bsl::size_t numNullElements() const;
            // Return the number of null elements in this vector.
    };
//..
// Then, we implement, in turn, each of the methods declared above:
//..
                     // --------------------
                     // class NullableVector
                     // --------------------

    // CREATORS
    template <class TYPE>
    NullableVector<TYPE>::NullableVector(bsl::size_t       initialLength,
                                         bslma::Allocator *basicAllocator)
    : d_values(initialLength, TYPE(), basicAllocator)
    , d_nullFlags(initialLength, true, basicAllocator)
    {
    }

    template <class TYPE>
    NullableVector<TYPE>::~NullableVector()
    {
        BSLS_ASSERT(d_values.size() == d_nullFlags.length());
    }

    // MANIPULATORS
    template <class TYPE>
    inline
    void NullableVector<TYPE>::appendElement(const TYPE& value)
    {
        d_values.push_back(value);
        d_nullFlags.append(false);
    }

    template <class TYPE>
    inline
    void NullableVector<TYPE>::appendNullElement()
    {
        d_values.push_back(TYPE());
        d_nullFlags.append(true);
    }

    template <class TYPE>
    inline
    void NullableVector<TYPE>::makeNonNull(bsl::size_t index)
    {
        BSLS_ASSERT_SAFE(index < length());

        d_nullFlags.assign(index, false);
    }

    template <class TYPE>
    inline
    void NullableVector<TYPE>::makeNull(bsl::size_t index)
    {
        BSLS_ASSERT_SAFE(index < length());

        d_values[index] = TYPE();
        d_nullFlags.assign(index, true);
    }

    template <class TYPE>
    inline
    TYPE& NullableVector<TYPE>::modifiableElement(bsl::size_t index)
    {
        BSLS_ASSERT_SAFE(index < length());

        d_nullFlags.assign(index, false);
        return d_values[index];
    }

    template <class TYPE>
    inline
    void NullableVector<TYPE>::removeElement(bsl::size_t index)
    {
        BSLS_ASSERT_SAFE(index < length());

        d_values.erase(d_values.begin() + index);
        d_nullFlags.remove(index);
    }

    // ACCESSORS
    template <class TYPE>
    inline
    const TYPE& NullableVector<TYPE>::constElement(bsl::size_t index) const
    {
        BSLS_ASSERT_SAFE(index < length());

        return d_values[index];
    }

    template <class TYPE>
    inline
    bool NullableVector<TYPE>::isAnyElementNonNull() const
    {
        return d_nullFlags.isAny0();
    }

    template <class TYPE>
    inline
    bool NullableVector<TYPE>::isAnyElementNull() const
    {
        return d_nullFlags.isAny1();
    }

    template <class TYPE>
    inline
    bool NullableVector<TYPE>::isElementNull(bsl::size_t index) const
    {
        BSLS_ASSERT_SAFE(index < length());

        return d_nullFlags[index];
    }

    template <class TYPE>
    inline
    bsl::size_t NullableVector<TYPE>::length() const
    {
        return d_values.size();
    }

    template <class TYPE>
    inline
    bsl::size_t NullableVector<TYPE>::numNullElements() const
    {
        return d_nullFlags.num1();
    }
//..

static
int gggHex(bdlc::BitArray *bitArray, const char *spec)
    // Configure the specified 'bitArray' with the nibbles specified by the
    // characters in the specified 'spec', which is in hex.  Return -1 on
    // success, and the index of the problem otherwise.  Whitespace in 'spec'
    // is ignored.  Any previous contents of 'bitArray' are discarded unless an
    // error is detected, in which case 'bitArray' is unmodified.  The behavior
    // is undefined unless the characters in 'spec' are either valid hex
    // digits, white space, or 'modifier' characters as defined below.
    //
    // Unlike the 'g', 'gg', and 'ggg' functions, the most significant nibbles
    // come first.  The input string is case-insensitive.  In addition to valid
    // hex characters, the following characters are also permitted, case-
    // insensitive:
    //
    //: o 'y' -- bYte -- 8-bits -- repeat following hex nibble twice.
    //: o 'q' -- Quarter word -- 16 bits -- repeat following hex nibble 4 times
    //: o 'h' -- Half word -- 32 bits -- repeat following hex nibble 8 times
    //: o 'w' -- Word -- 64 bits -- repeat following hex nibble 16 times.
    //
    // These modifiers can be repeated or combined.  Modifiers sum, they do not
    // multiply.  So "ww9" means 128 bits of nibble 9, binary 1001, or
    // "wwwhy3b" means '64 + 64 + 64 + 32 + 8 == 232' bits of nibble 3, binary
    // 0011, followed by a 'b' nibble, binary 1011.  It is an error for
    // modifiers to occur if there is no following hex digit before the end of
    // the string.
    //
    // Note that it is impossible to build a bit array whose length is not a
    // multiple of 4 using this function.
{
    enum { SUCCESS = -1 };

    const int NUM_CHARS = static_cast<int>(strlen(spec));

    // First, check for errors and return the index of the problem, which will
    // either be an unrecognized character or a modifier that has no hex digit
    // following it.

    int unresolvedModifierIndex = -1;
    for (const char *pc = spec; *pc; ++pc) {
        int c = bsl::tolower(*pc);
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')) {
            unresolvedModifierIndex = -1;
            continue;
        }
        if (bsl::strchr("yqhw", c)) {
            unresolvedModifierIndex = static_cast<int>(pc - spec);
            continue;
        }
        if (bsl::isspace(c)) {
            continue;
        }

        // unrecognized character

        return static_cast<int>(pc - spec);                           // RETURN
    }
    if (unresolvedModifierIndex >= 0) {
        // modifier not followed by hex digit

        return unresolvedModifierIndex;                               // RETURN
    }

    // We should have detected all possible errors by this point, so the input
    // string should be valid.  Any errors detected after this will be detected
    // by 'BSLS_ASSERT_OPT'.

    bitArray->removeAll();

    uint64_t nibble = 16;
    bool     lastCharWasHex = false;
    uint64_t lastNibble     = 17;
    for (int numChars = NUM_CHARS - 1; numChars >= 0;
                                             --numChars, lastNibble = nibble) {
        unsigned char currValue = spec[numChars];
        if (bsl::isspace(currValue)) {
            continue;
        }
        BSLS_ASSERT_OPT(0 == (currValue & 0x80));    // ASCII

        currValue = static_cast<unsigned char>(bsl::tolower(currValue));

        BSLMF_ASSERT('a' > '9');

        int repeat = 1;
        nibble = currValue <= '9'
                 ? (currValue >= '0' ? currValue - '0' : 16)
                 : (currValue >= 'a'
                    ? (currValue <= 'f' ? 10 + currValue - 'a'
                                        : 16)
                    : 16);
        if (nibble < 16) {
            lastCharWasHex = true;
        }
        else {
            ASSERT(16 == nibble);
            ASSERT(lastNibble < 16);
            nibble = lastNibble;
            switch (currValue) {
              case 'y': {    // byte (8 bits)
                repeat = 1;
              } break;
              case 'q': {    // quarter-word (16 bits)
                repeat = 3;
              } break;
              case 'h': {    // half-word (32 bits)
                repeat = 7;
              } break;
              case 'w': {    // word (64 bits)
                repeat = 15;
              } break;
              default: {
                BSLS_ASSERT_OPT(0 && "Unrecognized char in"
                                                      " populateBitStringHex");
              } break;
            }
            if (!lastCharWasHex) {
                ++repeat;
            }
            lastCharWasHex = false;
        }

        BSLS_ASSERT_OPT(repeat >= 1);
        BSLS_ASSERT_OPT(nibble < 16);

        for (; repeat > 0; --repeat) {
            for (int j = 0; j < 4; ++j) {
                bitArray->append(nibble & (1 << j));
            }
        }
    }

    return SUCCESS;
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  The numbers
// 0 and 1 correspond to bit values to be appended to the 'bdlc::BitArray'
// object.  A tilde ('~') indicates that the logical (but not necessarily
// physical) state of the object is to be set to its initial, empty state (via
// the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= '0' | '1'
//
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "0"          Append the value 0.
// "00"         Append two 0 values.
// "011"        Append three values corresponding to 0, 1, and 1.
// "011~"       Append three values corresponding to 0, 1, and 1 and then
//              remove all the elements (set array length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "011~00"     Append three values corresponding to 0, 1, and 1; empty
//              the object; and append values corresponding to 0 and 0.
//
//-----------------------------------------------------------------------------

static
int ggg(bdlc::BitArray *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('0' == spec[i]) {
            object->append(0);
        }
        else if ('1' == spec[i]) {
            object->append(1);
        }
        else if ('~' == spec[i]) {
            object->removeAll();
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad character ('" << spec[i] << "') in spec \""
                     << spec << "\" at position " << i << '.' << endl;
            }
            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

static
bdlc::BitArray& gg(bdlc::BitArray *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

static
int gggDispatch(bdlc::BitArray *object, const char *spec, bool verbose = false)
    // If the first character of the specified 'spec' is 'x', the rest of
    // 'spec' is to be interpreted by 'gggHex', otherwise, all of 'spec' is to
    // be interpreted by 'ggg'.  If there's an error in 'spec' and the
    // optionally specified 'verbose' is 'true', print a warning.  The
    // specified 'object' is to be configured according to 'spec'.  Return a
    // negative status on success and the index of the character where a
    // problem occurred otherwise.
{
    if ('x' == *spec || 'X' == *spec) {
        int rc = gggHex(object, spec + 1);
        return rc < 0 ? rc : rc + 1;                                  // RETURN
    }

    return ggg(object, spec, verbose);
}

static
bdlc::BitArray& ggDispatch(bdlc::BitArray *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(gggDispatch(object, spec) < 0);
    return *object;
}

static
bdlc::BitArray gDispatch(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdlc::BitArray object;
    return ggDispatch(&object, spec);
}

bsl::string randSpec(size_t length)
    // Return a random binary spec of the optionally specified 'length'
    // suitable for passing to 'ggg' or 'gggDispatch'.{
{
    bsl::string ret;
    ret.reserve(length);

    enum { BITS_IN_RAND = 10 };

    BSLMF_ASSERT(RAND_MAX > 4 * (1 << BITS_IN_RAND));

    size_t bpi;    // Bits Per Iteration
    for (size_t ii = length; ii > 0; ii -= bpi) {
        int randWord = bsl::rand();

        bpi = bsl::min<size_t>(ii, BITS_IN_RAND);
        for (size_t jj = 0; jj < bpi; ++jj) {
            ret.push_back((randWord & (1 << jj)) ? '1' : '0');
        }
    }
    ASSERT(ret.length() == length);

    return ret;
}

static
void testUsage()
    // Test the usage example (see call in main 'switch', test case 31).  This
    // code had to be moved out of the main 'switch', which had grown too
    // large, causing the AIX optimizing compiler to crash.
{
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Creating a 'NullableVector' Class
/// - - - - - - - - - - - - - - - - - - - - - -
// An efficient implementation of an arbitrary precision bit sequence container
// has myriad applications.  For example, a 'bdlc::BitArray' can be used
// effectively as a parallel array of flags indicating some special property,
// such as, 'isNull', 'isBusinessDay', etc.; its use is especially indicated
// when (1) the number of elements of the primary array can grow large, and
// (2) the individual elements do not have the capacity or capability to store
// the information directly.
//
// As a simple example, we'll implement a (heavily elided) value-semantic
// template class, 'NullableVector<TYPE>', that behaves like a
// 'bsl::vector<TYPE>' but additionally allows storing a nullness flag to
// signify that the corresponding element was not specified.  Elements added to
// a 'NullableVector' are null by default, although there are manipulator
// functions that allow appending a non-null element.  Each null element
// stores the default value for 'TYPE'.
//
// Note that this class has a minimal interface (suitable for illustration
// purpose only) that allows users to either append a (non-null) 'TYPE' value
// or a null value.  A real 'NullableVector' class would support a complete set
// of *value* *semantic* operations, including copy construction, assignment,
// equality comparison, 'ostream' printing, and 'bdex' serialization.
//
// First, we define the interface of 'NullableVector':
//..

    // See class definition is above 'main'.

//..
// Next, we create an empty 'NullableVector':
//..
    NullableVector<int>        array(0);
    const NullableVector<int>& ARRAY       = array;
    const int                  DEFAULT_INT = 0;

    ASSERT(0       == ARRAY.length());
    ASSERT(0       == ARRAY.numNullElements());
    ASSERT(false   == ARRAY.isAnyElementNonNull());
    ASSERT(false   == ARRAY.isAnyElementNull());
//..
// Then, we append a non-null element to it:
//..
    array.appendElement(5);
    ASSERT(1       == ARRAY.length());
    ASSERT(5       == ARRAY.constElement(0));
    ASSERT(false   == ARRAY.isElementNull(0));
    ASSERT(0       == ARRAY.numNullElements());
    ASSERT(true    == ARRAY.isAnyElementNonNull());
    ASSERT(false   == ARRAY.isAnyElementNull());
//..
// Next, we append a null element:
//..
    array.appendNullElement();
    ASSERT(2           == ARRAY.length());
    ASSERT(5           == ARRAY.constElement(0));
    ASSERT(DEFAULT_INT == ARRAY.constElement(1));
    ASSERT(false       == ARRAY.isElementNull(0));
    ASSERT(true        == ARRAY.isElementNull(1));
    ASSERT(1           == ARRAY.numNullElements());
    ASSERT(true        == ARRAY.isAnyElementNonNull());
    ASSERT(true        == ARRAY.isAnyElementNull());
//..
// Then, we make the null element non-null:
//..
    array.makeNonNull(1);
    ASSERT(2           == ARRAY.length());
    ASSERT(5           == ARRAY.constElement(0));
    ASSERT(DEFAULT_INT == ARRAY.constElement(1));
    ASSERT(false       == ARRAY.isElementNull(0));
    ASSERT(false       == ARRAY.isElementNull(1));
    ASSERT(0           == ARRAY.numNullElements());
    ASSERT(true        == ARRAY.isAnyElementNonNull());
    ASSERT(false       == ARRAY.isAnyElementNull());
//..
// Next, we make the first element null:
//..
    array.makeNull(0);
    ASSERT(2           == ARRAY.length());
    ASSERT(DEFAULT_INT == ARRAY.constElement(0));
    ASSERT(DEFAULT_INT == ARRAY.constElement(1));
    ASSERT(true        == ARRAY.isElementNull(0));
    ASSERT(false       == ARRAY.isElementNull(1));
    ASSERT(1           == ARRAY.numNullElements());
    ASSERT(true        == ARRAY.isAnyElementNonNull());
    ASSERT(true        == ARRAY.isAnyElementNull());
//..
// Now, we remove the front element:
//..
    array.removeElement(0);
    ASSERT(1           == ARRAY.length());
    ASSERT(DEFAULT_INT == ARRAY.constElement(0));
    ASSERT(false       == ARRAY.isElementNull(0));
    ASSERT(0           == ARRAY.numNullElements());
    ASSERT(true        == ARRAY.isAnyElementNonNull());
    ASSERT(false       == ARRAY.isAnyElementNull());
//..
// Finally, we remove the last remaining element and observe that the object is
// empty again:
//..
    array.removeElement(0);
    ASSERT(0       == ARRAY.length());
    ASSERT(0       == ARRAY.numNullElements());
    ASSERT(false   == ARRAY.isAnyElementNonNull());
    ASSERT(false   == ARRAY.isAnyElementNull());
//..
}

static
void testNum0Num1()
    // Test the 'num0' and 'num1' methods when applied to subranges.  The
    // previous test of them only tested when passed default arguments.
{
    bslma::TestAllocator testAllocator(veryVeryVerbose);

    {
        const Obj& X = gDispatch("xwwwwq0");    // 272 bits of 0
        enum { LENGTH = 272 };
        ASSERT(LENGTH == X.length());

        for (int begin = 0; begin <= LENGTH;incInt(&begin, LENGTH)) {
            for (int end = begin; end <= LENGTH; ++end) {
                ASSERT(static_cast<size_t>(end - begin) == X.num0(begin, end));
                ASSERT(0                                == X.num1(begin, end));
            }
        }
    }

    {
        const Obj& X = gDispatch("xwwwwqf");    // 272 bits of 1
        enum { LENGTH = 272 };
        ASSERT(LENGTH == X.length());

        for (int begin = 0; begin <= LENGTH;incInt(&begin, LENGTH)) {
            for (int end = begin; end <= LENGTH; ++end) {
                ASSERT(static_cast<size_t>(end - begin) == X.num1(begin, end));
                ASSERT(0                                == X.num0(begin, end));
            }
        }
    }

    {
        Obj        mX(&testAllocator);
        const Obj& X = ggDispatch(&mX, "xwwwwq5");   // 272 bits of alternating
        enum { LENGTH = 272 };
        ASSERT(LENGTH == X.length());

        for (int ii = 0; ii < 2; ++ii) {
            for (int begin = 0; begin <= LENGTH;incInt(&begin, LENGTH)) {
                for (int end = begin; end <= LENGTH; ++end) {
                    const bool   oddLength = (end - begin) & 1;
                    const bool   oddSet    = oddLength && X[begin];
                    const size_t EXP1      = (end - begin) / 2 + oddSet;
                    const size_t EXP0      = end - begin - EXP1;

                    ASSERT(EXP1 == X.num1(begin, end));
                    ASSERT(EXP0 == X.num0(begin, end));
                }
            }

            mX.toggleAll();
        }
    }

    {
        const Obj& X = gDispatch("xwwyfwwy0"); // high-order 1's, low-order 0's
        enum { LENGTH = 272 };
        ASSERT(X.length() == LENGTH);
        const int halfLength = LENGTH / 2;

        for (int begin = 0; begin <= LENGTH; incInt(&begin, LENGTH)) {
            for (int end = begin; end <= LENGTH; ++end) {
                const size_t EXP0 = bsl::max(0,
                                             bsl::min(halfLength,end) - begin);
                const size_t EXP1 = (end - begin) - EXP0;

                ASSERT(EXP0 == X.num0(begin, end));
                ASSERT(EXP1 == X.num1(begin, end));
            }
        }
    }

    {
        const Obj& X = gDispatch("xwwy0wwyf"); // high-order 0's, low-order 1's
        enum { LENGTH = 272 };
        ASSERT(X.length() == LENGTH);
        const int halfLength = LENGTH / 2;

        for (int begin = 0; begin <= LENGTH; incInt(&begin, LENGTH)) {
            for (int end = begin; end <= LENGTH; ++end) {
                const size_t EXP1 = bsl::max(0,
                                             bsl::min(halfLength,end) - begin);
                const size_t EXP0 = (end - begin) - EXP1;

                ASSERT(EXP1 == X.num1(begin, end));
                ASSERT(EXP0 == X.num0(begin, end));
            }
        }
    }

    {
        const size_t LENGTHS[] = { 0, 16, 97, 147, 207, 272, k_INVALID_INDEX };

        size_t length;
        for (size_t li = 0; k_INVALID_INDEX != (length = LENGTHS[li]); ++li) {
            for (int ti = 0; ti < 10; ++ti) {
                const bsl::string& SPEC = randSpec(length);
                const Obj&         X = gDispatch(SPEC.c_str());
                ASSERT(X.length() == length);

                for (size_t begin = 0; begin <= length;
                                                    incSizeT(&begin, length)) {
                    for (size_t end = begin; end <= length; ++end) {
                        const size_t EXP1 = countOnesOracle(X, begin, end);
                        const size_t EXP0 = (end - begin) - EXP1;

                        ASSERT(EXP1 == X.num1(begin, end));
                        ASSERT(EXP0 == X.num0(begin, end));

                        if (length == end) {
                            ASSERT(EXP1 == X.num1(begin));
                            ASSERT(EXP0 == X.num0(begin));

                            if (0 == begin) {
                                ASSERT(EXP1 == X.num1());
                                ASSERT(EXP0 == X.num0());
                            }
                        }
                    }
                }
            }
        }
    }

    {
        Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

        bsls::AssertTestHandlerGuard guard;

        size_t len = X.length();

        ASSERT_PASS(X.num0(0, 0));
        ASSERT_PASS(X.num1(0, 0));
        ASSERT_PASS(X.num0(0, len));
        ASSERT_PASS(X.num1(0, len));
        ASSERT_PASS(X.num0(0, len / 2));
        ASSERT_PASS(X.num1(0, len / 2));
        ASSERT_PASS(X.num0(len / 2, len / 2));
        ASSERT_PASS(X.num1(len / 2, len / 2));
        ASSERT_PASS(X.num0(len, len));
        ASSERT_PASS(X.num1(len, len));

        ASSERT_SAFE_FAIL(X.num0(len / 2, len / 2 - 1));
        ASSERT_SAFE_FAIL(X.num1(len / 2, len / 2 - 1));
        ASSERT_SAFE_FAIL(X.num0(0, len + 1));
        ASSERT_SAFE_FAIL(X.num1(0, len + 1));
    }
}

static
void testAssignBits()
    // Test the 'assignBits' method.  See documentation in main switch, test
    // case 29.  This code had to be moved out of the main 'switch', which had
    // grown too large, causing the AIX optimizing compiler to crash.
{
        bslma::TestAllocator testAllocator(veryVeryVerbose);

        const char *SPECS[] = {
           "0",      "01",     "011",    "0110",   "01100", "111111",
           "00001000111000010011000001001101",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawd",
           0}; // Null string required as last element.

        bool flipX = false;
        for (int ti = 0; true; ++ti) {
            if (!SPECS[ti]) {
                if (flipX) {
                    break;
                }

                flipX = true;
                ti    = 0;
            }

            Obj mX;    const Obj& X = ggDispatch(&mX, SPECS[ti]);
            if (flipX) {
                mX.toggleAll();
            }
            const bsl::string& xSpec = binSpec(X);

            // Check case where '0 == numBits'.

            {
                Obj mR(X, &testAllocator);    const Obj& R = mR;

                const Int64 B = testAllocator.numBlocksTotal();
                for (size_t di = 0; di <= X.length(); ++di) {
                    mR.assignBits(di, minusOne, 0);

                    ASSERT(R == X);
                }
                ASSERT(B == testAllocator.numBlocksTotal());
            }

            static const int DEST_IDXS[] = { 0, 1, 2, 3, 5, 16, 32, 48, 63,
                                             64, 65, 96, 112, 127, 128, 129 };
            enum { NUM_DEST_IDXS = sizeof DEST_IDXS / sizeof *DEST_IDXS };

            for (int tk = 0; tk < NUM_DEST_IDXS; ++tk) {
                const size_t di = DEST_IDXS[tk];
                if (di >= X.length()) {
                    break;
                }

                for (int tm = 0; tm < 4; ++tm) {
                    const bsl::string& bitsSpec = randSpec(64);
                    const Obj&         bitsObj  = gDispatch(bitsSpec.c_str());
                    const uint64_t     bits     = bitsObj.bits(0, 64);

                    if (veryVerbose) {
                        P_(di);    P(bitsSpec);
                    }

                    const int maxWriteBits = static_cast<int>(
                                          bsl::min<size_t>(k_BITS_PER_UINT64,
                                                           X.length() - di));
                    for (int writeBits = 1; writeBits <= maxWriteBits;
                                                                 ++writeBits) {
                        Obj mR(X, &testAllocator);    const Obj& R = mR;

                        const Int64 B = testAllocator.numBlocksTotal();
                        mR.assignBits(di, bits, writeBits);
                        ASSERT(B == testAllocator.numBlocksTotal());

                        const bsl::string& expSpec =
                                                xSpec.substr(0, di) +
                                                bitsSpec.substr(0, writeBits) +
                                                xSpec.substr(di + writeBits);

                        const Obj& E = gDispatch(expSpec.c_str());

                        ASSERT(E.length() == X.length());
                        ASSERT(E == R);
                    }
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwwa");

            bsls::AssertTestHandlerGuard guard;

            size_t       len = X.length();
            const size_t ww  = sizeof(uint64_t) * CHAR_BIT;
                                                                  // Word Width

            ASSERT_SAFE_PASS(mX.assignBits(    0, minusOne,      0));
            ASSERT_SAFE_PASS(mX.assignBits(    0, minusOne,     ww));
            ASSERT_SAFE_PASS(mX.assignBits(  len, minusOne,      0));
            ASSERT_SAFE_PASS(mX.assignBits(len/2, minusOne,     ww));

            ASSERT_SAFE_FAIL(mX.assignBits(     len, minusOne,      1));
            ASSERT_SAFE_FAIL(mX.assignBits(       0, minusOne, ww + 1));
            ASSERT_SAFE_FAIL(mX.assignBits( len + 1, minusOne,      0));
            ASSERT_SAFE_FAIL(mX.assignBits(len-ww+1, minusOne,     ww));
        }
}

static
void testFind1AtMinIndex()
    // Test all overloads of the 'find1AtMinIndex' methods.  See documentation
    // in case 28 of the main 'switch' statement.  This code had to be moved
    // out of the main 'switch', which had grown too large, causing the AIX
    // optimizing compiler to crash.
{
        bslma::TestAllocator testAllocator(veryVeryVerbose);

        const char *SPECS[] = {
           "0",      "01",     "011",    "0110",   "01100", "111111",
           "0110001",         "01100011",         "011000110",
           "011000110001100", "0110001100011000", "01100011000110001",
           "0000100011100001001100000100110",
           "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
           "xh01wwww0", "xwwww0h01h0", "xww0h01h0ww0",
           0}; // Null string required as last element.

        {
            // Verify results for empty array.

            const Obj X;
            ASSERT(k_INVALID_INDEX == X.find0AtMinIndex());
            ASSERT(k_INVALID_INDEX == X.find1AtMinIndex());
        }

        for (int ti = 0; SPECS[ti]; ++ti) {
            for (int flip = 0; flip < 2; ++flip) {
                const char *const DST = SPECS[ti];

                Obj          mX;
                const Obj&   X      = ggDispatch(&mX, DST);
                const size_t curLen = X.length();

                if (flip) {
                    mX.toggleAll();
                }

                const Obj XX(X, &testAllocator);
                ASSERT(XX.length() == curLen);

                const Int64 BB = testAllocator.numBlocksTotal();

                for (size_t begin = 0; begin <= curLen; ++begin) {
                    for (size_t end = begin; end <= curLen; ++end) {
                        size_t exp0 = k_INVALID_INDEX;
                        size_t exp1 = k_INVALID_INDEX;

                        for (size_t ii = begin; ii < end; ++ii) {
                            if (! X[ii]) {
                                exp0 = ii;
                                break;
                            }
                        }
                        for (size_t ii = begin; ii < end; ++ii) {
                            if (X[ii]) {
                                exp1 = ii;
                                break;
                            }
                        }

                        ASSERT(X.find0AtMinIndex(begin, end) == exp0);
                        ASSERT(X.find1AtMinIndex(begin, end) == exp1);

                        if (curLen == end) {
                            ASSERT(X.find0AtMinIndex(begin) == exp0);
                            ASSERT(X.find1AtMinIndex(begin) == exp1);

                            if (0 == begin) {
                                ASSERT(X.find0AtMinIndex() == exp0);
                                ASSERT(X.find1AtMinIndex() == exp1);
                            }
                        }

                        ASSERT(XX == X);
                    }
                }

                ASSERT(BB == testAllocator.numBlocksTotal());
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(X.find0AtMinIndex());
            ASSERT_SAFE_PASS(X.find0AtMinIndex(      0));
            ASSERT_SAFE_PASS(X.find0AtMinIndex(len / 2));
            ASSERT_SAFE_PASS(X.find0AtMinIndex(    len));
            ASSERT_SAFE_PASS(X.find0AtMinIndex(      0,       0));
            ASSERT_SAFE_PASS(X.find0AtMinIndex(      0,     len));
            ASSERT_SAFE_PASS(X.find0AtMinIndex(    len,     len));
            ASSERT_SAFE_PASS(X.find0AtMinIndex(len / 2, len / 2));

            ASSERT_SAFE_FAIL(X.find0AtMinIndex(len + 1));
            ASSERT_SAFE_FAIL(X.find0AtMinIndex(len / 2, len/2-1));
            ASSERT_SAFE_FAIL(X.find0AtMinIndex(    len, len - 1));
            ASSERT_SAFE_FAIL(X.find0AtMinIndex(      0, len + 1));

            ASSERT_SAFE_PASS(X.find1AtMinIndex());
            ASSERT_SAFE_PASS(X.find1AtMinIndex(      0));
            ASSERT_SAFE_PASS(X.find1AtMinIndex(len / 2));
            ASSERT_SAFE_PASS(X.find1AtMinIndex(    len));
            ASSERT_SAFE_PASS(X.find1AtMinIndex(      0,       0));
            ASSERT_SAFE_PASS(X.find1AtMinIndex(      0,     len));
            ASSERT_SAFE_PASS(X.find1AtMinIndex(    len,     len));
            ASSERT_SAFE_PASS(X.find1AtMinIndex(len / 2, len / 2));

            ASSERT_SAFE_FAIL(X.find1AtMinIndex(len + 1));
            ASSERT_SAFE_FAIL(X.find1AtMinIndex(len / 2, len/2-1));
            ASSERT_SAFE_FAIL(X.find1AtMinIndex(    len, len - 1));
            ASSERT_SAFE_FAIL(X.find1AtMinIndex(      0, len + 1));
        }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    char LONG_SPEC_1[33];
    char LONG_SPEC_2[65];
    char LONG_SPEC_3[97];
    char LONG_SPEC_4[129];
    char LONG_SPEC_5[161];
    char LONG_SPEC_6[193];
    char LONG_SPEC_7[225];
    char LONG_SPEC_8[257];
    char LONG_SPEC_9[289];

    strcpy(LONG_SPEC_1, "00001000111000010011000001001101");

    strcpy(LONG_SPEC_2, LONG_SPEC_1);
    strcat(LONG_SPEC_2, LONG_SPEC_1);

    strcpy(LONG_SPEC_3, LONG_SPEC_2);
    strcat(LONG_SPEC_3, LONG_SPEC_1);

    strcpy(LONG_SPEC_4, LONG_SPEC_3);
    strcat(LONG_SPEC_4, LONG_SPEC_1);

    strcpy(LONG_SPEC_5, LONG_SPEC_4);
    strcat(LONG_SPEC_5, LONG_SPEC_1);

    strcpy(LONG_SPEC_6, LONG_SPEC_5);
    strcat(LONG_SPEC_6, LONG_SPEC_1);

    strcpy(LONG_SPEC_7, LONG_SPEC_6);
    strcat(LONG_SPEC_7, LONG_SPEC_1);

    strcpy(LONG_SPEC_8, LONG_SPEC_7);
    strcat(LONG_SPEC_8, LONG_SPEC_1);

    strcpy(LONG_SPEC_9, LONG_SPEC_8);
    strcat(LONG_SPEC_9, LONG_SPEC_1);

    switch (test) { case 0:  // Zero is always the leading case.
      case 31: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLE\n"
                               "=====================\n";

        testUsage();
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING RANGE-BASED NUM0, NUM1
        //
        // Concerns:
        //: 1 That the functions return the right values.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Generate a number of objects with their bits set up in a variety
        //:   of regular patterns:
        //:   o Vary 'begin' and 'end' over most of the range of possible
        //:     ranges for such an object.
        //:   o Calculate expected values for 'num1' and 'num0' over that
        //:     range.
        //:   o Verify that the functions return their expected values.
        //:
        //: 2 For a sequence of possible lengths:
        //:   o Generate a number of random specs for that length.
        //:   o Create a bit array object from that spec.
        //:   o Vary 'begin' and 'end' over most of the range of possible
        //:     ranges for such an object.
        //:   o Use 'countOnesOracle' to calculate expected values to be
        //:     returned from 'num0' and 'num1' for that range.
        //:   o Verify the results are as expected.  (C-1)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   size_t num0(size_t begin, size_t end);
        //   size_t num1(size_t begin, size_t end);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING RANGE-BASED NUM0, NUM1\n"
                               "==============================\n";

        testNum0Num1();
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING 'assignBits'
        //   Ensure the method returns the expected value.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 No memory is allocated.
        //:
        //: 3 High-order bits above the low order 'numBits' assigned are
        //:   ignored.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate over a sequence of specifications, initializing 'mX', a
        //:   bit array object, from them.
        //:
        //: 2 Nested within that, iterate over the same sequence, initializing
        //:   object 'Y', to be assigned from.
        //:
        //: 3 Nested within that, iterate int 'numBits' from 1 to 64.
        //:
        //: 4 Nested within that, iterate int 'di' from 0 to
        //:   'X.length() - numBits'.
        //:
        //: 5 Nested within that, iterate int 'si' from 0 to
        //:   'Y.length() - numBits'.
        //:
        //: 6 Through string operations, create the spec of the expected
        //:   result, and create object 'E' from that spec.
        //:
        //: 7 Copy construct an object 'mR' from 'X'.
        //:
        //: 8 Apply 'mR.assignBits(di, Y.bits(0, 64), numBits)', making
        //:   sure no memory is allocated.  (C-2) (C-3)
        //:
        //: 9 Verify that 'E == R'.  (C-1)
        //:
        //: 10 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void assignBits(size_t index, uint64_t srcBits, size_t numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'assignBits'\n"
                             "====================\n";


        testAssignBits();
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING FIND[01]ATMININDEX METHODS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 The object is unchanged.
        //:
        //: 3 Memory is not allocated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of specifications and their complements,
        //:   create an object X from that specification.
        //:
        //: 2 Copy construct 'XX' from 'X', to be compared later to ensure 'X'
        //:   hasn't changed.
        //:
        //: 3 Iterate int 'begin' from 0 to the length of 'X'.
        //:
        //: 4 Iterate int 'end' from 'begin' to the length of 'X'
        //:
        //: 5 By iterating and using the '[]' operator, find the min positions,
        //:   if any, of set and clear bits in the range '[begin .. end)'.
        //:
        //: 6 Call the function, allowing optional args to default if the
        //:   default values match 'begin' or 'end', and observe that the
        //:   result returned is as expected.  (C-1).
        //:
        //: 7 Verify that 'XX == X'.  (C-2).
        //:
        //: 8 After the loops, verify that no memory has been allocated since
        //:   'XX' was created.  (C-3).
        //:
        //: 9 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   size_t find0AtMinIndex(size_t begin, size_t end) const;
        //   size_t find1AtMinIndex(size_t begin, size_t end) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING FIND[01]ATMININDEX METHODS\n"
                               "==================================\n";


        testFind1AtMinIndex();
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING FIND[01]ATMAXINDEX METHODS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 The object is unchanged.
        //:
        //: 3 Memory is not allocated.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of specifications and their complements,
        //:   create an object X from that specification.
        //:
        //: 2 Copy construct 'XX' from 'X', to be compared later to ensure 'X'
        //:   hasn't changed.
        //:
        //: 3 Iterate int 'begin' from 0 to the length of 'X'.
        //:
        //: 4 Iterate int 'end' from 'begin' to the length of 'X'
        //:
        //: 5 By iterating and using the '[]' operator, find the max positions,
        //:   if any, of set and clear bits in the range '[begin .. end)'.
        //:
        //: 6 Call the function, allowing optional args to default if the
        //:   default values match 'begin' or 'end', and observe that the
        //:   result returned is as expected.  (C-1).
        //:
        //: 7 Verify that 'XX == X'.  (C-2).
        //:
        //: 8 After the loops, verify that no memory has been allocated since
        //:   'XX' was created.  (C-3).
        //:
        //: 9 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   size_t find0AtMaxIndex(size_t begin, size_t end) const;
        //   size_t find1AtMaxIndex(size_t begin, size_t end) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING FIND[01]ATMAXINDEX METHODS\n"
                               "==================================\n";

        const char *SPECS[] = {
           "0",      "01",     "011",    "0110",   "01100", "111111",
           "0110001",         "01100011",         "011000110",
           "011000110001100", "0110001100011000", "01100011000110001",
           "0000100011100001001100000100110",
           "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
           "xq01wwww0", "xwwww0h010", "xww0h01h0ww0",
           0}; // Null string required as last element.

        {
            // Verify results for empty array.

            const Obj X;
            ASSERT(k_INVALID_INDEX == X.find0AtMaxIndex());
            ASSERT(k_INVALID_INDEX == X.find1AtMaxIndex());
        }

        for (int ti = 0; SPECS[ti]; ++ti) {
            for (int flip = 0; flip < 2; ++flip) {
                const char *const DST = SPECS[ti];

                Obj          mX;
                const Obj&   X      = ggDispatch(&mX, DST);
                const size_t curLen = X.length();
                if (flip) {
                    mX.toggleAll();
                }

                const Obj XX(X);
                ASSERT(XX.length() == curLen);

                const Int64 BB = testAllocator.numBlocksTotal();

                for (size_t begin = 0; begin <= curLen; ++begin) {
                    for (size_t end = begin; end <= curLen; ++end) {
                        size_t exp0 = k_INVALID_INDEX;
                        size_t exp1 = k_INVALID_INDEX;

                        for (size_t ii = end; ii > begin; ) {
                            if (! X[--ii]) {
                                exp0 = ii;
                                break;
                            }
                        }
                        for (size_t ii = end; ii > begin; ) {
                            if (X[--ii]) {
                                exp1 = ii;
                                break;
                            }
                        }

                        ASSERT(X.find0AtMaxIndex(begin, end) == exp0);
                        ASSERT(X.find1AtMaxIndex(begin, end) == exp1);

                        if (curLen == end) {
                            ASSERT(X.find0AtMaxIndex(begin) == exp0);
                            ASSERT(X.find1AtMaxIndex(begin) == exp1);

                            if (0 == begin) {
                                ASSERT(X.find0AtMaxIndex() == exp0);
                                ASSERT(X.find1AtMaxIndex() == exp1);
                            }
                        }

                        ASSERT(XX == X);
                    }
                }

                ASSERT(BB == testAllocator.numBlocksTotal());
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(X.find0AtMaxIndex());
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(      0));
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(len / 2));
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(    len));
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(      0,       0));
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(      0,     len));
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(    len,     len));
            ASSERT_SAFE_PASS(X.find0AtMaxIndex(len / 2, len / 2));

            ASSERT_SAFE_FAIL(X.find0AtMaxIndex(len + 1));
            ASSERT_SAFE_FAIL(X.find0AtMaxIndex(len / 2, len/2-1));
            ASSERT_SAFE_FAIL(X.find0AtMaxIndex(    len, len - 1));
            ASSERT_SAFE_FAIL(X.find0AtMaxIndex(      0, len + 1));

            ASSERT_SAFE_PASS(X.find1AtMaxIndex());
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(      0));
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(len / 2));
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(    len));
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(      0,       0));
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(      0,     len));
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(    len,     len));
            ASSERT_SAFE_PASS(X.find1AtMaxIndex(len / 2, len / 2));

            ASSERT_SAFE_FAIL(X.find1AtMaxIndex(len + 1));
            ASSERT_SAFE_FAIL(X.find1AtMaxIndex(len / 2, len/2-1));
            ASSERT_SAFE_FAIL(X.find1AtMaxIndex(    len, len - 1));
            ASSERT_SAFE_FAIL(X.find1AtMaxIndex(      0, len + 1));
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING BIT-WISE OPERATORS
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 The value of the parameters that are not to be modified stay
        //:   unchanged.
        //:
        //: 3 For the assignment operators, the additional concern(s) are:
        //:   o No allocations are performed.
        //:   o alias-safe
        //:   o A reference to the modifiable object assigned to is returned.
        //:
        //: 4 For the non-assignment operators, an additional concern is:
        //:   o exception-neutrality
        //
        // Note that there is no possibility of undefined behavior, no
        // preconditions need testing.
        //
        // Plan:
        //: 1 For each of a sequence of specs, create an object 'X' which is
        //:   the template of the objects to be assigned to, and copy construct
        //:   an object 'XX' from 'X', to be compared later to ensure that
        //:   'X' has not changed.
        //:
        //: 2 Nested within that, for each of the same sequence of specs,
        //:   create an object 'Y' which will serve as the object to be
        //:   assigned from.  Create 'YY', a copy of 'Y', to be compared with
        //:   later to ensure that 'Y' has not changed.
        //:
        //: 3 In blocks for each of the operations '&', '-', '|', and '^':
        //:   o Create an object 'E' by applying bitwise operations, one bit
        //:     at a time, between 'X' and 'Y'.
        //:   o Create 'mR', a copy of 'X' and bit-wise assign to it,
        //:     monitoring the test allocator to ensure no allocation happened
        //:     and verify that the result is as expected. (C-1)
        //:   o Observe that no memory was allocated by the bitwise assignment
        //:     operator.  (C-3-1)
        //:   o Keep a reference 'RRR' to the result of the assignment and
        //:     verify that its address matches that of 'R'.  (C-3-3)
        //:   o Observe that 'Y' is unchanged.  (C-2)
        //:   o Do the bitwise non-assignment operator between 'X' and 'Y' and
        //:     bind a const reference to the temporary objected returned, and
        //:     verify its value matched 'E'.  (C-1)
        //:   o Set the default allocator to the test allocator, and do the
        //:     infix bit-wise operation in a 'BEGIN' - 'END' block to ensure
        //:     ensure expectation neutrality, and check that the result is as
        //:     expected.  (C-4)
        //:   o Observe that 'X' and 'Y' and unchanged.  (C-2)
        //:
        //: 4 In blocks for each of the operations '&', '-', '|', and '^':
        //:   o Copy-construct an object 'mR' from 'X' to be assigned to in an
        //:     aliasing operation using the bitwise-assignment operator, and
        //:     verify the result is as expected.  (C-1)  (C-3-2)
        //:   o Copy-construct an object 'S' from 'X', then use the bitwise
        //:     non-assignment operator to alias S with itself, and observe the
        //:     result is as expected.  (C-1)  (C-3-2)
        //
        // Testing:
        //   BitArray& operator&=(const BitArray& rhs);
        //   BitArray& operator-=(const BitArray& rhs);
        //   BitArray& operator|=(const BitArray& rhs);
        //   BitArray& operator^=(const BitArray& rhs);
        //   operator&(const BitArray&, const BitArray&);
        //   operator-(const BitArray&, const BitArray&);
        //   operator|(const BitArray&, const BitArray&);
        //   operator^(const BitArray&, const BitArray&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BIT-WISE OPERATORS\n"
                               "==========================\n";

        const char *SPECS[] = {
           "0",      "01",     "011",    "0110",   "01100",
           "0110001",         "01100011",         "011000110",
           "011000110001100", "0110001100011000", "01100011000110001",
           "0000100011100001001100000100110",
           "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
           "xww0", "xwwf",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
           0}; // Null string required as last element.

        if (verbose) cout <<
            "\nTesting 'operator&=', et al.; not aliased" << endl;
        {
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const Obj&        X   = gDispatch(DST);
                const Obj         XX(X);

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const SRC = SPECS[tj];
                    const Obj&        Y   = gDispatch(SRC);          // control
                    const Obj         YY = Y;

                    if (veryVerbose) {
                        P(X) P(Y)
                    }

                    { // and
                        Obj    mE(X, &testAllocator);    const Obj& E = mE;
                        size_t i;
                        for (i = 0; i < X.length() && i < Y.length(); ++i) {
                            mE.andEqual(i, Y[i]);
                        }
                        mE.assign0(i, X.length() - i);

                        Obj mR(X, &testAllocator);    const Obj& R = mR;

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        Obj& RRR = (mR &= Y);
                        ASSERT(&RRR == &R);

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        ASSERT(YY == Y);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);

                        LOOP4_ASSERT(ti, tj, E, R, E == R);

                        bslma::DefaultAllocatorGuard dag(&testAllocator);
                        {
                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Obj& Z = X & Y;

                            LOOP2_ASSERT(ti, tj, E == Z);
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }

                        ASSERT(XX == X);
                        ASSERT(YY == Y);
                    }

                    { // minus
                        Obj    mE(X, &testAllocator);    const Obj& E = mE;
                        size_t i;
                        for (i = 0; i < X.length() && i < Y.length(); ++i) {
                            mE.minusEqual(i, Y[i]);
                        }

                        Obj mR(X, &testAllocator);    const Obj& R = mR;

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        Obj& RRR = (mR -= Y);
                        ASSERT(&RRR == &R);

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        ASSERT(YY == Y);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);

                        LOOP4_ASSERT(ti, tj, E, R, E == R);

                        bslma::DefaultAllocatorGuard dag(&testAllocator);
                        {
                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Obj& Z = X - Y;

                            LOOP2_ASSERT(ti, tj, E == Z);
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }

                        ASSERT(XX == X);
                        ASSERT(YY == Y);
                    }

                    { // or
                        Obj    mE(X, &testAllocator);    const Obj& E = mE;
                        size_t i;
                        for (i = 0; i < X.length() && i < Y.length(); ++i) {
                            mE.orEqual(i, Y[i]);
                        }

                        Obj mR(X, &testAllocator);    const Obj& R = mR;

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        Obj& RRR = (mR |= Y);
                        ASSERT(&RRR == &R);

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        ASSERT(YY == Y);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);

                        LOOP4_ASSERT(ti, tj, E, R, E == R);

                        bslma::DefaultAllocatorGuard dag(&testAllocator);
                        {
                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Obj& Z = X | Y;

                            LOOP2_ASSERT(ti, tj, E == Z);
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }

                        ASSERT(XX == X);
                        ASSERT(YY == Y);
                    }

                    { // xor
                        Obj    mE(X, &testAllocator);    const Obj& E = mE;
                        size_t i;
                        for (i = 0; i < X.length() && i < Y.length(); ++i) {
                            mE.xorEqual(i, Y[i]);
                        }

                        Obj mR(X, &testAllocator);    const Obj& R = mR;

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        Obj& RRR = (mR ^= Y);
                        ASSERT(&RRR == &R);

                        if (veryVerbose) {
                            P(R) P(Y)
                        }

                        ASSERT(YY == Y);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);

                        LOOP4_ASSERT(ti, tj, E, R, E == R);

                        bslma::DefaultAllocatorGuard dag(&testAllocator);
                        {
                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Obj& Z = X ^ Y;

                            LOOP2_ASSERT(ti, tj, E == Z);
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }

                        ASSERT(XX == X);
                        ASSERT(YY == Y);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'operator&=', et al.; aliased" << endl;
        {
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const Obj&        X   = gDispatch(DST);
                const Obj         Z(X.length(), false);

                if (veryVerbose) {
                    P(X)
                }

                { // and
                    Obj mR(X, &testAllocator);    const Obj& R = mR;

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64 B  = testAllocator.numBlocksInUse();

                    mR &= R;

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64 A  = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    LOOP3_ASSERT(ti, X, R, X == R);

                    Obj mS(X);    const Obj& S = mS;
                    const Obj& RR = S & S;

                    LOOP3_ASSERT(ti, X, S, X == S);
                    LOOP3_ASSERT(ti, X, S, X == RR);
                }

                { // minus
                    Obj mR(X, &testAllocator);    const Obj& R = mR;

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64 B  = testAllocator.numBlocksInUse();

                    mR -= R;

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64 A  = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    ASSERT(R.length() == X.length());
                    ASSERT(! R.isAny1());
                    LOOP3_ASSERT(ti, Z, R, Z == R);

                    Obj mS(X);    const Obj& S = mS;
                    const Obj& RR = S - S;

                    LOOP3_ASSERT(ti, X, S, X == S);
                    LOOP3_ASSERT(ti, X, S, Z == RR);
                }

                { // or
                    Obj mR(X, &testAllocator);    const Obj& R = mR;

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64 B  = testAllocator.numBlocksInUse();

                    mR |= R;

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64 A  = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    LOOP3_ASSERT(ti, X, R, X == R);

                    Obj mS(X);    const Obj& S = mS;
                    const Obj& RR = S | S;

                    LOOP3_ASSERT(ti, X, S, X == S);
                    LOOP3_ASSERT(ti, X, S, X == RR);
                }

                { // xor
                    Obj mR(X, &testAllocator);    const Obj& R = mR;

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64 B  = testAllocator.numBlocksInUse();

                    mR ^= R;

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64 A  = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    ASSERT(R.length() == X.length());
                    ASSERT(! R.isAny1());
                    LOOP3_ASSERT(ti, Z, R, Z == R);

                    Obj mS(X);    const Obj& S = mS;
                    const Obj& RR = S ^ S;

                    LOOP3_ASSERT(ti, X, S, X == S);
                    LOOP3_ASSERT(ti, X, S, Z == RR);
                }
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING BIT-WISE LOGICAL ASSIGNMENT METHODS
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 No allocations are performed.
        //:
        //: 3 The methods are alias-safe.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate through a sequence of test specs, creating an object
        //:   'X' that will be the template for the object assigned to.
        //:
        //: 2 Nested within that, iterate through the same sequence of test
        //:   specs, creating an object 'Y' that will be the assigned from.
        //:   Copy construct 'YY' from 'Y'.
        //:
        //: 3 Nested within that, iterate through different values of 'NB", the
        //:   number of bits to be operated in.
        //:
        //: 4 In separate blocks, create a local copy 'mR' of 'X', and apply
        //:   the four bit-wise logical operations to them from 'Y'.  Also
        //:   create 'E', the expected result, by doing repetitive single-bit
        //:   operations on it.  Do the operation between 'mR' and 'Y'.
        //:   Observe that no memory was allocated by the operation.  After the
        //:   operation, compare 'E' and 'R'.  Then compare 'YY == Y', showing
        //:   'Y' is unchanged.  (C-1) (C-2)
        //:
        //: 5 In separate blocks, create a local copy 'mR' of 'X', and apply
        //:   the four bit-wise logical operations to them from 'R' (aliasing).
        //:   Also create 'E', the expected result, by doing repetitive
        //:   single-bit operations on it.  Observe that no memory was
        //:   allocated by the operation.  After the operation, compare 'E' and
        //:   'R'.  (C-1) (C-2) (C-3)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void andEqual(dstIndex, srcArray, srcIndex, numBits);
        //   void minusEqual(dstIndex, srcArray, srcIndex, numBits);
        //   void orEqual(dstIndex, srcArray, srcIndex, numBits);
        //   void xorEqual(dstIndex, srcArray, srcIndex, numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING BIT-WISE LOGICAL ASSIGNMENT METHODS" << endl
                      << "===========================================" << endl;

        if (verbose) cout <<
         "\nTesting 'andEqual(dstIndex, srcArray, srcIndex, numBits)', et al."
                          << endl << "\t\tno aliasing"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
             "000010001110000100110000010011010000100011100001001100000100110",
            "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
            0}; // Null string required as last element.

            static const int NUMBITS[] = {
                0, 1, 5, 32, 65, 129
            };
            enum { NUM_NUMBITS = sizeof NUMBITS / sizeof *NUMBITS };

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const Obj&        X   = gDispatch(DST);              // control

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const SRC = SPECS[tj];
                    const Obj&        Y   = gDispatch(SRC);          // control
                    const Obj         YY  = Y;

                    for (int tk = 0; tk < NUM_NUMBITS; ++tk) {
                        const size_t NB = NUMBITS[tk];

                        if (NB > bsl::min(X.length(), Y.length())) {
                            continue;
                        }

                        const size_t maxDi = X.length() - NB;
                        for (size_t di = 0; di <= maxDi;
                                                        incSizeT(&di, maxDi)) {
                            const size_t maxSi = Y.length() - NB;
                            for (size_t si = 0; si <= maxSi;
                                                        incSizeT(&si, maxSi)) {
                                if (veryVerbose) {
                                    P_(X); P_(Y); P_(di); P_(si); P(NB);
                                }

                                { // and
                                    Obj        mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (size_t i = 0; i < NB; ++i) {
                                        mE.andEqual(di + i, Y[si + i]);
                                    }

                                    Obj        mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const Int64 B =
                                                testAllocator.numBlocksTotal();

                                    mR.andEqual(di, Y, si, NB);

                                    const Int64 A =
                                                testAllocator.numBlocksTotal();
                                    ASSERT(B == A);

                                    if (veryVerbose) {
                                        cout << "\t\tand:   ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);

                                    ASSERT(YY == Y);
                                }

                                { // minus
                                    Obj        mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (size_t i = 0; i < NB; ++i) {
                                        mE.minusEqual(di + i, Y[si + i]);
                                    }

                                    Obj        mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const Int64 BB =
                                                testAllocator.numBlocksTotal();
                                    const Int64 B  =
                                                testAllocator.numBlocksInUse();

                                    mR.minusEqual(di, Y, si, NB);

                                    const Int64 AA =
                                                testAllocator.numBlocksTotal();
                                    const Int64 A  =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\tminus: ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);

                                    ASSERT(YY == Y);
                                }

                                { // or
                                    Obj        mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (size_t i = 0; i < NB; ++i) {
                                        mE.orEqual(di + i, Y[si + i]);
                                    }

                                    Obj        mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const Int64 BB =
                                                testAllocator.numBlocksTotal();
                                    const Int64 B  =
                                                testAllocator.numBlocksInUse();

                                    mR.orEqual(di, Y, si, NB);

                                    const Int64 AA =
                                                testAllocator.numBlocksTotal();
                                    const Int64 A  =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\tor:    ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);

                                    ASSERT(YY == Y);
                                }

                                { // xor
                                    Obj        mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (size_t i = 0; i < NB; ++i) {
                                        mE.xorEqual(di + i, Y[si + i]);
                                    }

                                    Obj        mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const Int64 BB =
                                                testAllocator.numBlocksTotal();
                                    const Int64 B  =
                                                testAllocator.numBlocksInUse();

                                    mR.xorEqual(di, Y, si, NB);

                                    const Int64 AA =
                                                testAllocator.numBlocksTotal();
                                    const Int64 A  =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\txor:   ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);

                                    ASSERT(YY == Y);
                                }
                            }
                        }
                    }

                    ASSERT(YY == Y);
                }
            }
        }

        if (verbose) cout <<
         "\nTesting 'andEqual(dstIndex, srcArray, srcIndex, numBits)', et al."
                          << endl << "\t\taliased"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
            0}; // Null string required as last element.

            static const int NUMBITS[]   = {
                0, 1, 5, 32, 65, 129
            };
            enum { NUM_NUMBITS = sizeof NUMBITS / sizeof *NUMBITS };

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];

                const Obj& X = gDispatch(DST);                  // control

                for (int tk = 0; tk < NUM_NUMBITS; ++tk) {
                    const size_t NB = NUMBITS[tk];

                    if (NB > X.length()) {
                        continue;
                    }

                    const size_t maxDi = X.length() - NB;
                    for (size_t di = 0; di <= maxDi; incSizeT(&di, maxDi)) {
                        const size_t maxSi = maxDi;
                        for (size_t si = 0; si <= maxSi; incSizeT(&si, maxSi)){

                            { // and
                                Obj        mE (X, &testAllocator);
                                const Obj& E = mE;
                                for (size_t i = 0; i < NB; ++i) {
                                    mE.andEqual(di + i, X[si + i]);
                                }

                                Obj        mR(X, &testAllocator);
                                const Obj& R = mR;

                                const Int64 BB =testAllocator.numBlocksTotal();

                                mR.andEqual(di, R, si, NB);

                                const Int64 AA =testAllocator.numBlocksTotal();
                                ASSERT(BB == AA);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                            { // minus
                                Obj        mE (X, &testAllocator);
                                const Obj& E = mE;
                                for (size_t i = 0; i < NB; ++i) {
                                    mE.minusEqual(di + i, X[si + i]);
                                }

                                Obj        mR(X, &testAllocator);
                                const Obj& R = mR;

                                const Int64 BB =testAllocator.numBlocksTotal();

                                mR.minusEqual(di, R, si, NB);

                                const Int64 AA =testAllocator.numBlocksTotal();
                                ASSERT(BB == AA);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                            { // or
                                Obj        mE(X, &testAllocator);
                                const Obj& E = mE;
                                for (size_t i = 0; i < NB; ++i) {
                                    mE.orEqual(di + i, X[si + i]);
                                }

                                Obj        mR(X, &testAllocator);
                                const Obj& R = mR;

                                const Int64 BB =testAllocator.numBlocksTotal();

                                mR.orEqual(di, R, si, NB);

                                const Int64 AA =testAllocator.numBlocksTotal();
                                ASSERT(BB == AA);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                            { // xor
                                Obj        mE(X, &testAllocator);
                                const Obj& E = mE;
                                for (size_t i = 0; i < NB; ++i) {
                                    mE.xorEqual(di + i, X[si + i]);
                                }

                                Obj        mR(X, &testAllocator);
                                const Obj& R = mR;

                                const Int64 BB =testAllocator.numBlocksTotal();

                                mR.xorEqual(di, R, si, NB);

                                const Int64 AA =testAllocator.numBlocksTotal();
                                ASSERT(BB == AA);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }
                        }
                    }
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.andEqual(      0, X,       0,       0));
            ASSERT_SAFE_PASS(mX.andEqual(      0, X,       0,     len));
            ASSERT_SAFE_PASS(mX.andEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.andEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.andEqual(len - 1, X, len - 1,       1));
            ASSERT_SAFE_PASS(mX.andEqual(      1, X,       1, len - 1));

            ASSERT_SAFE_FAIL(mX.andEqual(    len, X,       0,       1));
            ASSERT_SAFE_FAIL(mX.andEqual(len +99, X,       0,       0));
            ASSERT_SAFE_FAIL(mX.andEqual(      0, X,     len,       1));
            ASSERT_SAFE_FAIL(mX.andEqual(      0, X, len +99,       0));
            ASSERT_SAFE_FAIL(mX.andEqual(      0, X,       0, len  +1));
            ASSERT_SAFE_FAIL(mX.andEqual(      0, X,       0, len +99));
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.minusEqual(      0, X,       0,       0));
            ASSERT_SAFE_PASS(mX.minusEqual(      0, X,       0,     len));
            ASSERT_SAFE_PASS(mX.minusEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.minusEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.minusEqual(len - 1, X, len - 1,       1));
            ASSERT_SAFE_PASS(mX.minusEqual(      1, X,       1, len - 1));

            ASSERT_SAFE_FAIL(mX.minusEqual(    len, X,       0,       1));
            ASSERT_SAFE_FAIL(mX.minusEqual(len +99, X,       0,       0));
            ASSERT_SAFE_FAIL(mX.minusEqual(      0, X,     len,       1));
            ASSERT_SAFE_FAIL(mX.minusEqual(      0, X, len +99,       0));
            ASSERT_SAFE_FAIL(mX.minusEqual(      0, X,       0, len  +1));
            ASSERT_SAFE_FAIL(mX.minusEqual(      0, X,       0, len +99));
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.orEqual(      0, X,       0,       0));
            ASSERT_SAFE_PASS(mX.orEqual(      0, X,       0,     len));
            ASSERT_SAFE_PASS(mX.orEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.orEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.orEqual(len - 1, X, len - 1,       1));
            ASSERT_SAFE_PASS(mX.orEqual(      1, X,       1, len - 1));

            ASSERT_SAFE_FAIL(mX.orEqual(    len, X,       0,       1));
            ASSERT_SAFE_FAIL(mX.orEqual(len +99, X,       0,       0));
            ASSERT_SAFE_FAIL(mX.orEqual(      0, X,     len,       1));
            ASSERT_SAFE_FAIL(mX.orEqual(      0, X, len +99,       0));
            ASSERT_SAFE_FAIL(mX.orEqual(      0, X,       0, len  +1));
            ASSERT_SAFE_FAIL(mX.orEqual(      0, X,       0, len +99));
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.xorEqual(      0, X,       0,       0));
            ASSERT_SAFE_PASS(mX.xorEqual(      0, X,       0,     len));
            ASSERT_SAFE_PASS(mX.xorEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.xorEqual(len / 2, X, len / 2, len / 2));
            ASSERT_SAFE_PASS(mX.xorEqual(len - 1, X, len - 1,       1));
            ASSERT_SAFE_PASS(mX.xorEqual(      1, X,       1, len - 1));

            ASSERT_SAFE_FAIL(mX.xorEqual(    len, X,       0,       1));
            ASSERT_SAFE_FAIL(mX.xorEqual(len +99, X,       0,       0));
            ASSERT_SAFE_FAIL(mX.xorEqual(      0, X,     len,       1));
            ASSERT_SAFE_FAIL(mX.xorEqual(      0, X, len +99,       0));
            ASSERT_SAFE_FAIL(mX.xorEqual(      0, X,       0, len  +1));
            ASSERT_SAFE_FAIL(mX.xorEqual(      0, X,       0, len +99));
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING XOREQUAL(INDEX, VALUE) METHOD
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 No allocations are performed.
        //:
        //: 3 The method does not depend upon capacity.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of test vectors:
        //:   o Reserve a varying amount of padding to verify that this makes
        //:     no difference.  (C-3)
        //:   o Verify that the methods give the expected value.  (C-1)
        //:   o Verify that no memory is allocated.  (C-2)
        //:   o Verify defensive checks are triggered for invalid values.
        //:     (C-4)
        //
        // Testing:
        //   void xorEqual(size_t index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING XOREQUAL(INDEX, VALUE) METHOD\n"
                               "=====================================\n";

        static const char *SPECS[] = {
            "0",      "01",     "011",    "0110",   "01100",
            "0110001",         "01100011",         "011000110",
            "011000110001100", "0110001100011000", "01100011000110001",
            "0000100011100001001100000100110",
            "00001000111000010011000001001101",
            "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
             0}; // Null string required as last element.

        static const int EXTEND[]   = {
            0, 1, 2, 4, 8, 16, 32, 63, 64, 65, 127, 128, 129
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        for (int tj = 0; tj < NUM_EXTEND; ++tj) {
            const int N = EXTEND[tj];

            bool flip = false;
            for (int ti = 0; true; ++ti) {
                if (! SPECS[ti]) {
                    if (flip) {
                        break;
                    }

                    flip = true;
                    ti   = 0;
                }
                const char *const SPEC = SPECS[ti];

                Obj mX;    const Obj& X = ggDispatch(&mX, SPEC);     // control
                if (flip) {
                    mX.toggleAll();
                }

                if (veryVerbose) {
                    P_(SPEC);    P_(flip);    P(X.length());
                }

                Obj mY(X, &testAllocator);    const Obj& Y = mY;
                mY.reserveCapacity(X.length() + N);
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 B = testAllocator.numBlocksInUse();

                size_t i;

                if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.xorEqual(i, false);
                    ASSERT(b == Y[i]);
                }
                LOOP_ASSERT(SPEC, X == Y);
                if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                mY = X;
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.xorEqual(i, -1);
                    ASSERT(!b == Y[i]);
                    mY.xorEqual(i, -1);
                }
                LOOP_ASSERT(SPEC, X == Y);

                mY = X;
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.xorEqual(i, 1);
                    ASSERT(!b == Y[i]);
                    mY.xorEqual(i, -1);
                }
                LOOP_ASSERT(SPEC, X == Y);

                mY = X;
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.xorEqual(i, 2);
                    ASSERT(!b == Y[i]);
                    mY.xorEqual(i, -1);
                }
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 A = testAllocator.numBlocksInUse();
                ASSERT(B == A);
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.xorEqual(      0, true));
            ASSERT_SAFE_PASS(mX.xorEqual(len - 1, true));
            ASSERT_SAFE_PASS(mX.xorEqual(len / 2, true));

            ASSERT_SAFE_FAIL(mX.xorEqual(    len, true));
            ASSERT_SAFE_FAIL(mX.xorEqual(len +99, true));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING OREQUAL(INDEX, VALUE) METHOD
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 No allocations are performed.
        //:
        //: 3 The method does not depend upon capacity.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of test vectors:
        //:   o Reserve a varying amount of padding to verify that this makes
        //:     no difference.  (C-3)
        //:   o Verify that the methods give the expected value.  (C-1)
        //:   o Verify that no memory is allocated.  (C-2)
        //:   o Verify defensive checks are triggered for invalid values.
        //:     (C-4)
        //
        // Testing:
        //   void orEqual(size_t index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING OREQUAL(INDEX, VALUE) METHOD\n"
                               "====================================\n";

        static const char *SPECS[] = {
           "0",      "01",     "011",    "0110",   "01100",
           "0110001",         "01100011",         "011000110",
           "011000110001100", "0110001100011000", "01100011000110001",
           "0000100011100001001100000100110",
           "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
           "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
            0}; // Null string required as last element.

        const int EXTEND[] = { 0, 1, 2, 3, 16, 32, 63, 64, 96, 127, 128, 129 };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        for (int tj = 0; tj < NUM_EXTEND; ++tj) {
            const int N = EXTEND[tj];

            bool flip = false;
            for (int ti = 0; true; ++ti) {
                if (! SPECS[ti]) {
                    if (flip) {
                        break;
                    }

                    flip = true;
                    ti   = 0;
                }
                const char *const SPEC = SPECS[ti];

                Obj mX;    const Obj& X = ggDispatch(&mX, SPEC);     // control
                if (flip) {
                    mX.toggleAll();
                }

                if (veryVerbose) {
                    P_(SPEC);    P_(flip);    P(X.length());
                }

                Obj mY(X, &testAllocator);    const Obj& Y = mY;
                mY.reserveCapacity(X.length() + N);
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 B = testAllocator.numBlocksInUse();

                size_t i;

                if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.orEqual(i, 0);
                    ASSERT(b == Y[i]);
                }
                LOOP_ASSERT(SPEC, X == Y);
                if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                mY = X;
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.orEqual(i, -1);
                    ASSERT(true == Y[i]);
                    mY.assign(i, b);
                }
                LOOP_ASSERT(SPEC, X == Y);

                mY = X;
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.orEqual(i, 1);
                    ASSERT(true == Y[i]);
                    mY.assign(i, b);
                }
                LOOP_ASSERT(SPEC, X == Y);

                mY = X;
                for (i = 0; i < X.length(); ++i) {
                    const bool b = X[i];
                    ASSERT(b == Y[i]);
                    mY.orEqual(i, 2);
                    ASSERT(true == Y[i]);
                    mY.assign(i, b);
                }
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 A = testAllocator.numBlocksInUse();
                ASSERT(B == A);
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.orEqual(      0, true));
            ASSERT_SAFE_PASS(mX.orEqual(len - 1, true));
            ASSERT_SAFE_PASS(mX.orEqual(len / 2, true));

            ASSERT_SAFE_FAIL(mX.orEqual(    len, true));
            ASSERT_SAFE_FAIL(mX.orEqual(len +99, true));
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING MINUSEQUAL(INDEX, VALUE) METHOD
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 No allocations are performed.
        //:
        //: 3 The method does not depend upon capacity.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of test vectors:
        //:   o Reserve a varying amount of padding to verify that this makes
        //:     no difference.  (C-3)
        //:   o Verify that the methods give the expected value.  (C-1)
        //:   o Verify that no memory is allocated.  (C-2)
        //:   o Verify defensive checks are triggered for invalid values.
        //:     (C-4)
        //
        // Testing:
        //   void minusEqual(size_t index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MINUSEQUAL(INDEX, VALUE) METHOD\n"
                               "=======================================\n";

        static const char *SPECS[] = {
            "0",      "01",     "011",    "0110",   "01100",
            "0110001",         "01100011",         "011000110",
            "011000110001100", "0110001100011000", "01100011000110001",
            "0000100011100001001100000100110",
            "00001000111000010011000001001101",
            "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
            "xqhah3q7", "xqw3h7hc",
             0}; // Null string required as last element.

        static const int EXTEND[] = { 0, 1, 2, 16, 32, 63, 64, 65, 127, 129 };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        for (int tj = 0; tj < NUM_EXTEND; ++tj) {
            const int N = EXTEND[tj];

            bool flip = false;
            for (int ti = 0; true; ++ti) {
                if (! SPECS[ti]) {
                    if (flip) {
                        break;
                    }

                    flip = true;
                    ti = 0;
                }
                const char *const SPEC = SPECS[ti];

                Obj mX;    const Obj& X = ggDispatch(&mX, SPEC);     // control
                if (flip) {
                    mX.toggleAll();
                }

                if (veryVerbose) {
                    P_(X.length());    P_(flip);    P(SPEC);
                }

                {
                    Obj mY(X, &testAllocator);    const Obj& Y = mY;
                    mY.reserveCapacity(X.length() + N);
                    ASSERT(X == Y);

                    const Int64 B = testAllocator.numBlocksInUse();

                    size_t i;

                    if (veryVerbose) { cout << "\t\t"; P(Y); }
                    for (i = 0; i < X.length(); ++i) {
                        const bool b = Y[i];
                        mY.minusEqual(i, 0);
                        ASSERT(Y[i] == b);
                    }
                    LOOP_ASSERT(SPEC, X == Y);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                    mY = X;
                    for (i = 0; i < X.length(); ++i) {
                        bool b = Y[i];
                        mY.minusEqual(i, -1);
                        ASSERT(0 == Y[i]);
                        mY.assign(i, b);
                    }
                    LOOP_ASSERT(SPEC, X == Y);

                    mY = X;
                    for (i = 0; i < X.length(); ++i) {
                        bool b = Y[i];
                        mY.minusEqual(i, 1);
                        ASSERT(0 == Y[i]);
                        mY.assign(i, b);
                    }
                    LOOP_ASSERT(SPEC, X == Y);

                    mY = X;
                    for (i = 0; i < X.length(); ++i) {
                        bool b = Y[i];
                        mY.minusEqual(i, 2);
                        ASSERT(0 == Y[i]);
                        mY.assign(i, b);
                    }
                    LOOP_ASSERT(SPEC, X == Y);

                    const Int64 A = testAllocator.numBlocksInUse();
                    ASSERT(B == A);
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.minusEqual(      0, true));
            ASSERT_SAFE_PASS(mX.minusEqual(len - 1, true));
            ASSERT_SAFE_PASS(mX.minusEqual(len / 2, true));

            ASSERT_SAFE_FAIL(mX.minusEqual(    len, true));
            ASSERT_SAFE_FAIL(mX.minusEqual(len +99, true));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING ANDEQUAL(INDEX, VALUE) METHOD
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 No allocations are performed.
        //:
        //: 3 The method does not depend upon capacity.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each of a sequence of test vectors:
        //:   o Reserve a varying amount of padding to verify that this makes
        //:     no difference.  (C-3)
        //:   o Verify that the methods give the expected value.  (C-1)
        //:   o Verify that no memory is allocated.  (C-2)
        //:   o Verify defensive checks are triggered for invalid values.
        //:     (C-4)
        //
        // Testing:
        //   void andEqual(size_t index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ANDEQUAL(INDEX, VALUE) METHOD\n"
                               "=====================================\n";

        static const char *SPECS[] = {
            "0",      "01",     "011",    "0110",   "01100",
            "0110001",         "01100011",         "011000110",
            "011000110001100", "0110001100011000", "01100011000110001",
            "0000100011100001001100000100110",
            "00001000111000010011000001001101",
            "xhaq5haq5w3", "xh4w7q9ha", "xqah5yca532wdwb", "xhqyabdc9hawwd",
             0}; // Null string required as last element.

        const int EXTEND[] = { 0, 1, 2, 3, 16, 32, 63, 64, 96, 127, 128, 129 };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        for (int tj = 0; tj < NUM_EXTEND; ++tj) {
            const int N = EXTEND[tj];

            bool flip = false;
            for (int ti = 0; true; ++ti) {
                if (!SPECS[ti]) {
                    if (flip) {
                        break;
                    }

                    flip = true;
                    ti = 0;
                }
                const char *const SPEC = SPECS[ti];

                Obj mX;    const Obj& X = ggDispatch(&mX, SPEC);     // control
                if (flip) {
                    mX.toggleAll();
                }

                if (veryVerbose) {
                    P_(X.length());    P_(SPEC);    P(flip);
                }

                Obj mY(X, &testAllocator);    const Obj& Y = mY;
                mY.reserveCapacity(X.length() + N);
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 B = testAllocator.numBlocksTotal();

                size_t i;

                if (veryVerbose) { cout << "\t\t"; P(Y); }
                for (i = 0; i < X.length(); ++i) {
                    const bool b = Y[i];
                    mY.andEqual(i, -1);
                    ASSERT(Y[i] == b);
                }
                if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                LOOP_ASSERT(SPEC, X == Y);

                for (i = 0; i < X.length(); ++i) {
                    const bool b = Y[i];
                    ASSERT(X[i] == b);
                    mY.andEqual(i, 1);
                    ASSERT(Y[i] == b);
                }
                LOOP_ASSERT(SPEC, X == Y);

                for (i = 0; i < X.length(); ++i) {
                    const bool b = Y[i];
                    mY.andEqual(i, 2);
                    ASSERT(Y[i] == b);
                }
                LOOP_ASSERT(SPEC, X == Y);

                for (i = 0; i < X.length(); ++i) {
                    const bool b = Y[i];
                    mY.andEqual(i, 0);
                    ASSERT(0 == Y[i]);
                    mY.assign(i, b);
                }
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 A = testAllocator.numBlocksTotal();
                ASSERT(B == A);
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.andEqual(      0, true));
            ASSERT_SAFE_PASS(mX.andEqual(len - 1, true));
            ASSERT_SAFE_PASS(mX.andEqual(len / 2, true));

            ASSERT_SAFE_FAIL(mX.andEqual(    len, true));
            ASSERT_SAFE_FAIL(mX.andEqual(len +99, true));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING SHIFT METHODS
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 the correct result is obtained
        //:
        //: 2 no allocations are performed from the object's allocator (for
        //:   the '<<' and '>>' methods, allocation from the default allocator
        //:   will happen for the temporary that is created).
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate through different specs indicating the initial state of a
        //:   bit array object 'X'.
        //:
        //: 2 In a nested loop, iterate through different numbers of bits to
        //:   shift.
        //:
        //: 3 Calculate through string operations the spec of the expected
        //:   result of a right shift on 'X', and create an object EXP from
        //:   that spec.
        //:
        //: 4 Copy-construct 'mY' from 'X'.
        //:
        //: 5 Apply the infix '>>' operator to 'Y' and keep a const ref 'Z' to
        //:   the result, and verify 'EXP == Z'.  (C-1)
        //:
        //: 6 Apply '>>=' to 'mY', and verify 'EXP == Y'.  (C-1)
        //:
        //: 7 Check 'Y', bit-by-bit, and compare with bits in 'X' that should
        //:   match.  (C-1)
        //:
        //: 8 Do 5, 6, 7, in a BEGIN - END block causing the allocator from
        //:   which 'Y' is constructed to throw, and verify that it never
        //:   threw.  (C-2)
        //:
        //: 9 Repeat steps 3-8, except doing '<<' and '<<=' instead of '>>' and
        //:   '>>='.  (C-1) (C-2)
        //:
        //: 10 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   BitArray& operator<<=(size_t numBits);
        //   BitArray& operator>>=(size_t numBits);
        //   BitArray operator<<(const BitArray& bitArray, size_t numBits);
        //   BitArray operator>>(const BitArray& bitArray, size_t numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING SHIFT METHODS\n"
                               "=====================\n";

        if (verbose) cout <<
       "\nTesting 'operator<<=', 'operator>>=', 'operator<<', and 'operator>>'"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",
                "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            "xqbhay9hew5h0", "xw9w5hb", "xwaw9h5w7",
            0}; // Null string required as last element.

            static const int ROTATE[]   = {
                0, 1, 2, 3, 5, 16, 31, 32, 63, 64, 65, 95, 96, 97, 127, 128,
                                                                            129
            };
            enum { NUM_ROTATE = sizeof ROTATE / sizeof *ROTATE };

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const Obj&        X    = gDispatch(SPEC);            // control

                if (veryVerbose) P(X);

                const bsl::string& bs = binSpec(X);

                for (int tj = 0; tj < NUM_ROTATE; ++tj) {
                    const size_t NUMBITS = ROTATE[tj];
                    if (NUMBITS > X.length()) {
                        break;
                    }

                    {
                        Obj mY(X, &testAllocator);    const Obj& Y = mY;
                        ASSERTV(SPEC, NUMBITS, X == Y);

                        bsl::string expSpec = bs.substr(NUMBITS);
                        expSpec.append(NUMBITS, '0');
                        const Obj& EXP = gDispatch(expSpec.c_str());

                        int count = 0;
                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            ++count;
                            const Obj& Z = Y >> NUMBITS;
                            ASSERT(EXP == Z);

                            Obj& YY = (mY >>= NUMBITS);
                            ASSERT(&YY == &Y);
                            if (veryVerbose) {
                                cout << "\tright: "; P_(NUMBITS); P(Y);
                            }

                            ASSERT(EXP == Y);

                            LOOP_ASSERT(SPEC, X.length() == Y.length());
                            size_t i;
                            for (i = 0; i < X.length() - NUMBITS; ++i) {
                                LOOP5_ASSERT(SPEC, i, NUMBITS, X[i + NUMBITS],
                                                 Y[i], X[i + NUMBITS] == Y[i]);
                            }
                            for (; i < X.length(); ++i) {
                                ASSERTV(SPEC, NUMBITS, 0 == Y[i]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        ASSERT(1 == count);
                    }
                    {
                        Obj mY(X, &testAllocator);    const Obj& Y = mY;
                        ASSERTV(SPEC, NUMBITS, X == Y);

                        bsl::string expSpec = bs;
                        expSpec.insert(expSpec.begin(),
                                       NUMBITS,
                                       '0');
                        expSpec.resize(bs.length());
                        const Obj& EXP = gDispatch(expSpec.c_str());

                        int count = 0;
                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            ++count;

                            const Obj& Z = Y << NUMBITS;
                            ASSERT(EXP == Z);

                            Obj& YY = (mY <<= NUMBITS);
                            ASSERT(&YY == &Y);
                            if (veryVerbose) {
                                cout << "\tleft: "; P_(NUMBITS); P(Y);
                            }

                            ASSERTV(SPEC, NUMBITS, EXP == Y);

                            LOOP_ASSERT(SPEC, X.length() == Y.length());
                            size_t i;
                            for (i = 0; i < NUMBITS; ++i) {
                                ASSERTV(SPEC, NUMBITS, 0 == Y[i]);
                            }
                            for (; i < X.length(); ++i) {
                                ASSERTV(SPEC, NUMBITS, X[i - NUMBITS] == Y[i]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        ASSERT(1 == count);
                    }
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_PASS(X << 0);
            ASSERT_PASS(X >> 0);
            ASSERT_PASS(X << len);
            ASSERT_PASS(X >> len);
            ASSERT_PASS(X << len / 2);
            ASSERT_PASS(X >> len / 2);

            ASSERT_PASS(mX <<= 0);
            ASSERT_PASS(mX >>= 0);
            ASSERT_PASS(mX <<= len);
            ASSERT_PASS(mX >>= len);
            ASSERT_PASS(mX <<= len / 2);
            ASSERT_PASS(mX >>= len / 2);

            ASSERT_SAFE_FAIL(X << (len + 1));
            ASSERT_SAFE_FAIL(X >> (len + 1));

            ASSERT_SAFE_FAIL(mX <<= (len + 1));
            ASSERT_SAFE_FAIL(mX >>= (len + 1));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING ROTATE METHODS
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained.
        //:
        //: 2 The operation is exception-neutral.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use specs from a table to drive a sequence of possible object
        //:   values.
        //:
        //: 2 Use 'binSpec' to produce 's', a binary spec of the object.
        //:
        //: 3 Iterate through a table of values specifying the amount to
        //:   rotate.
        //:
        //: 4 By string operations on 's', calculate the spec of the expected
        //:   value of the object after 'rotateRight'.  Create an object 'EXP'
        //:   from this spec.
        //:
        //: 5 Apply 'rotateRight' and compare the result to 'EXP'.  (C-1)
        //:
        //: 6 Steps 4 and 5 are enclosed in a 'BEGIN - END' block to ensure
        //:   that 'rotateRight' is exception-neutral.  (C-2)
        //:
        //: 7 Repeat steps 4-6, except for 'rotateLeft'.  (C-1) (C-2)
        //:
        //: 8 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   void rotateLeft(size_t numBits);
        //   void rotateRight(size_t numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ROTATE METHODS\n"
                               "======================\n";

        if (verbose) cout <<
            "\nTesting 'rotateLeft(numBits)' and 'rotateRight(numBits)'"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "0000100011100001001100000100110100001000111000010011000001001100"
                                                                           "1",
           "0000100011100001001100000100110100001000111000010011000001001100"
                                                                       "10101",
           "0000100011100001001100000100110100001000111000010011000001001100"
                                                                    "11101110",
           "0000100011100001001100000100110100001000111000010011000001001100"
                                                                  "1101101101",
           "0000100011100001001100000100110100001000111000010011000001001100"
                                                               "1000100010001",
            "xhaqdyccy7h9", "xq5h3haq0qfw2", "xww5w3wc",
            0}; // Null string required as last element.

            static const int ROTATE[]   = {
                0, 1, 2, 3, 5, 16, 31, 32, 60, 63, 64, 65, 68, 101, 151, 201
            };
            enum { NUM_ROTATE = sizeof ROTATE / sizeof *ROTATE };

            size_t oldLen = 0;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                size_t            curLen = strlen(SPEC);

                const Obj X = gDispatch(SPEC);                  // control
                LOOP_ASSERT(ti, 'x' == *SPEC ? X.length() > curLen
                                             : curLen ==X.length());
                                                                // same lengths
                const bsl::string& s = binSpec(X);
                if ('x' == *SPEC) {
                    curLen = static_cast<int>(s.length());
                }

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, !curLen || oldLen < curLen);
                                                         // strictly increasing
                oldLen = curLen;

                for (int tj = 0; tj < NUM_ROTATE; ++tj) {
                    const size_t NUMBITS = ROTATE[tj];

                    if (X.length() < NUMBITS) { break; }

                    Obj mY(X, &testAllocator);    const Obj& Y = mY;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERTV(SPEC, NUMBITS, X == Y);

                        const bsl::string expSpec =
                                      s.substr(NUMBITS) + s.substr(0, NUMBITS);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        if (veryVerbose) { cout << "\t\t"; P_(NUMBITS); P(Y); }
                        mY.rotateRight(NUMBITS);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                        LOOP_ASSERT(SPEC, EXP == Y);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    Obj mZ(X, &testAllocator);    const Obj& Z = mZ;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERTV(SPEC, NUMBITS, X == Z);

                        size_t            LEN     = X.length() - NUMBITS;
                        const bsl::string expSpec =
                                              s.substr(LEN) + s.substr(0, LEN);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        if (veryVerbose) { cout << "\t\t"; P_(NUMBITS); P(Z); }
                        mZ.rotateLeft(NUMBITS);
                        if (veryVerbose) { cout << "\t\t\t"; P(Z); }
                        LOOP_ASSERT(SPEC, EXP == Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_PASS(mX.rotateLeft (    0));
            ASSERT_PASS(mX.rotateRight(    0));
            ASSERT_PASS(mX.rotateLeft (  len));
            ASSERT_PASS(mX.rotateRight(  len));
            ASSERT_PASS(mX.rotateLeft (len/2));
            ASSERT_PASS(mX.rotateRight(len/2));

            ASSERT_FAIL(mX.rotateLeft (len+1));
            ASSERT_FAIL(mX.rotateRight(len+1));
            ASSERT_FAIL(mX.rotateLeft (len+99));
            ASSERT_FAIL(mX.rotateRight(len+99));
        }
      } break;
      case 18: {
        // -------------------------------------------------------------------
        // TESTING TOGGLE METHODS AND NOT OPERATOR
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 the correct result is obtained
        //:
        //: 2 no allocations are performed
        //:
        //: 3 does not depend upon internal representation
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 In the first loop, iterate through a sequence of specs, and
        //:   nested within that, iterate through amounts to extend by.
        //:
        //: 2 Create an object 'X', based on the spec.  Create an object 'mY'
        //:   stretch it by the amount to extend by, and initialize it
        //:   according to the spec.  (C-3)
        //:
        //: 3 Create an object 'Z' by applying the '~' operator to 'X'.
        //:
        //: 4 Apply 'toggleAll' to 'mY'.
        //:
        //: 5 Verify, one by one, that the bits in 'Y' are as expected.  (C-1)
        //:
        //: 6 Verify 'Z == Y'.  (C-1)
        //:
        //: 7 Verify that no memory allocation occurred in either toggle.
        //:   (C-2)
        //:
        //: 8 In the second loop, iterate through a sequence of specs, and
        //:   nested within that, iterate through amounts to extend by.
        //:
        //: 9 Create an object 'X' according to the spec, and an a possibly
        //:   stretched object 'Y' with an identical value.  (C-3)
        //:
        //: 10 Iterate 'idx' through all valid values for an index into 'X'.
        //:
        //: 11 Call the single-bit 'toggle' to toggle the bit at index in 'mY'.
        //:
        //: 12 Verify, one by one, that the bits in 'Y' are as expected.  (C-1)
        //:
        //: 13 Toggle the one bit back so 'X == Y' again.
        //:
        //: 14 Iterate 'nb' from 0 to 'X.length() - idx'.
        //:
        //: 15 Call 'toggle(idx, nb)' to toggle the 'nb' bits of 'mY' starting
        //:    at 'idx'.
        //:
        //: 16 Verify, one by one, that the bits in 'Y' are as expected.  (C-1)
        //:
        //: 17 Toggle the 'nb' bits back so 'X == Y' again.
        //:
        //: 18 Verify that no memory allocation occurred in the whole loop.
        //:    (C-3)
        //:
        //: 19 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void toggleAll();
        //   void toggle(size_t index);
        //   void toggle(size_t index, size_t numBits);
        //   operator~(const BitArray& bitArray);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TOGGLE METHODS AND NOT OPERATOR\n"
                               "=======================================\n";

        if (verbose) cout <<
            "\nTesting 'toggleAll' and 'operator~'" << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            static const int EXTEND[]   = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100,
                200, 500, 1000
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            size_t oldLen = 0;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const size_t      curLen = strlen(SPEC);

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, !curLen || oldLen < curLen);
                                                         // strictly increasing
                oldLen = curLen;

                const Obj& X = gDispatch(SPEC);         // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const size_t N = EXTEND[tj];
                        Obj          mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = gg(&mY, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);

                        const Int64 B = testAllocator.numBlocksTotal();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        // Note new object created uses default allocator.

                        const Obj& Z = ~X;
                        LOOP_ASSERT(ti, X.length() == Z.length());
                        ASSERT(Z.isEmpty() == (X == Z));

                        mY.toggleAll();
                        LOOP_ASSERT(ti, X.length() == Y.length());
                        ASSERT(Y.isEmpty() == (X == Y));
                        for (size_t i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(ti, X[i] != Y[i]);
                        }
                        LOOP_ASSERT(ti, Y == Z);

                        mY.toggleAll();
                        LOOP_ASSERT(ti, X == Y);

                        const Int64 A = testAllocator.numBlocksTotal();
                        ASSERT(B  == A);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'toggle(index)', and 'toggle(index, numBits)'"
                          << endl;
        {
            static const char *SPECS[] = {
          "",      "0",      "01",     "011",    "0110",   "01100",
           "0110001",
           "01100011",         "011000110",
           "00001000111000010011000001001101000010001110000100110000010011001",
            "xwawcq3y5", "xq2wawcq3y5", "xh7q3wawbqfy5",
            0}; // Null string required as last element.

            const int EXTENDS[] = { 0, 1, 3 };    const int NUM_EXTENDS = 3;

            size_t oldLen = 0;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const Obj&        X      = gDispatch(SPEC);          // control
                const size_t      curLen = X.length();

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, !curLen || oldLen < curLen);
                                                         // strictly increasing
                oldLen = curLen;

                for (int te = 0; te < NUM_EXTENDS; ++te) {
                    const int EXTEND = EXTENDS[te];

                    Obj mY(&testAllocator);    const Obj& Y = mY;
                    if (EXTEND > 0) {
                        stretchRemoveAll(&mY,
                                          curLen + EXTEND * k_BITS_PER_UINT64);
                    }
                    ggDispatch(&mY, SPEC);

                    CInt64 BB = testAllocator.numBlocksTotal();

                    for (size_t idx = 0; idx < X.length(); ++idx) {
                        ASSERT(X == Y);

                        {
                            mY.toggle(idx);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, X[idx] != Y[idx]);

                            size_t i;
                            for (i = 0; i < idx; ++i) {
                                ASSERT(X[i] == Y[i]);
                            }
                            for (i = idx + 1; i < X.length(); ++i) {
                                ASSERT(X[i] == Y[i]);
                            }

                            mY.toggle(idx);
                            LOOP_ASSERT(ti, X == Y);
                        }

                        const size_t maxNb = X.length() - idx;
                        for (size_t nb = 0; nb <= maxNb; ++nb) {
                            ASSERTV(SPEC, X == Y);

                            if (veryVerbose) {
                                cout << "\t\t"; P(X) P(Y);
                                P(idx) P(nb)
                            }

                            mY.toggle(idx, nb);

                            if (veryVerbose) {
                                cout << "\t\t"; P(Y);
                            }

                            size_t i;
                            for (i = 0; i < idx; ++i) {
                                ASSERT(X[i] == Y[i]);
                            }
                            for (; i < idx + nb; ++i) {
                                ASSERT(X[i] != Y[i]);
                            }
                            for (; i < X.length(); ++i) {
                                ASSERT(X[i] == Y[i]);
                            }

                            mY.toggle(idx, nb);
                            ASSERT(X == Y);
                        }
                    }

                    CInt64 AA = testAllocator.numBlocksTotal();
                    ASSERT(BB == AA);
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.toggle(0));
            ASSERT_SAFE_PASS(mX.toggle(len - 1));
            ASSERT_SAFE_FAIL(mX.toggle(-1));
            ASSERT_SAFE_FAIL(mX.toggle(-99));
            ASSERT_SAFE_FAIL(mX.toggle(len));

            ASSERT_SAFE_PASS(mX.toggle(0, 0));
            ASSERT_SAFE_PASS(mX.toggle(0, len));
            ASSERT_SAFE_FAIL(mX.toggle(0, len + 1));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING ASSIGN METHODS
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The correct result is obtained
        //:
        //: 2 No allocations are performed
        //:
        //: 3 Does not depend upon internal representation
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Iterate through a table of specs, creating an object 'X' from
        //:   the spec.
        //:
        //: 2 Iterate through a table of amounts to extend by, creating
        //:   stretched objects 'Y' and 'Y2' initialized to the spec from '1'.
        //:   (C-3)
        //:
        //: 3 Apply 'assignAll', 'assignAll1', and 'assignAll0' to 'Y' and
        //:   'Y2', using 'isAny*' to verify the result is as expected.  (C-1)
        //:
        //: 4 Start a new pair of loops, iterating through all specs and
        //:   extend amounts, creating a non-extended 'X' based on the spec.
        //:
        //: 5 Create an extended 'mY' whose value equals that of 'X'.  (C-3)
        //:
        //: 6 Iterate 'idx' through possible value indexes of 'X'.
        //:
        //: 7 Apply 'assign1(idx)' and 'assign0(idx)' to 'mY' and observe the
        //:   result.  (C-1)
        //:
        //: 8 Still in the same loop, iterate 'nb' from 0 to
        //:   'X.length() - idx'.
        //:
        //: 9 Apply 'assign1(idx, nb)' and 'assign0(idx, nb)', observing the
        //:   result is as expected.  (C-1)
        //:
        //: 10 Observe that none of that assignment methods did any memory
        //:    allocation.  (C-2)
        //:
        //: 11 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void assignAll(bool value);
        //   void assignAll0();
        //   void assignAll1();
        //   void assign0(size_t index);
        //   void assign0(size_t index, size_t numBits);
        //   void assign1(size_t index);
        //   void assign1(size_t index, size_t numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ASSIGN METHODS\n"
                               "======================\n";

        if (verbose) cout << "\nTesting 'assignAll0' and 'assignAll1'" << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            static const int EXTEND[]   = {
                0, 32, 65, 129, 256, 257
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            size_t oldLen = 0;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const size_t      curLen = static_cast<int>(strlen(SPEC));

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, !curLen || oldLen < curLen);
                                                         // strictly increasing
                oldLen = curLen;

                const Obj X = gDispatch(SPEC);          // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const size_t N = EXTEND[tj];

                        Obj mY( &testAllocator);    const Obj& Y  = mY;
                        Obj mY2(&testAllocator);    const Obj& Y2 = mY2;
                        stretchRemoveAll(&mY, N);
                        stretchRemoveAll(&mY, N);
                        gg(&mY, SPEC);   gg(&mY2, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        LOOP2_ASSERT(SPEC, N, X == Y2);
                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        mY.assignAll1();
                        LOOP_ASSERT(ti, X.length() == Y.length());
                        ASSERT(! Y.isAny0());
                        LOOP4_ASSERT(ti, X.length(), Y, Y.num1(),
                                     X.length() == Y.num1());

                        mY2.assignAll(true);
                        ASSERT(Y == Y2);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                    {
                        const int N = EXTEND[tj];

                        Obj mY( &testAllocator);    const Obj& Y  = mY;
                        Obj mY2(&testAllocator);    const Obj& Y2 = mY2;
                        stretchRemoveAll(&mY, N);
                        stretchRemoveAll(&mY, N);
                        gg(&mY, SPEC);   gg(&mY2, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        LOOP2_ASSERT(SPEC, N, X == Y2);
                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        mY.assignAll0();
                        LOOP_ASSERT(ti, X.length() == Y.length());
                        ASSERT(! Y.isAny1());
                        LOOP4_ASSERT(ti, X.length(), Y, Y.num0(),
                                     X.length() == Y.num0());

                        mY2.assignAll(false);
                        ASSERT(Y == Y2);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'assign?(index)', and 'assign?(index, numBits)'"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "00001000111000010011000001001101",
           "0000100011100001001100000100110100001000111000010011000001001100",
            "xwaqc", "xwah7", "xw5wc", "xwwaqb", "xwww3",
            0}; // Null string required as last element.

            static const int EXTEND[]   = {
                1, 2, 3
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            size_t oldLen = 0;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const Obj         X      = gDispatch(SPEC);          // control
                const size_t      curLen = X.length();

                ASSERT(0 < curLen);

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);      // strictly increasing
                oldLen = curLen;

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const size_t N = X.length() +
                                                EXTEND[tj] * k_BITS_PER_UINT64;
                        Obj          mY(&testAllocator);    const Obj&  Y = mY;
                        stretchRemoveAll(&mY, N);
                        mY.append(X, 0, X.length());
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        const Int64 BB = testAllocator.numBlocksTotal();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        const size_t maxIdx = X.length() - 1;
                        for (size_t idx = 0; idx <= maxIdx;
                                                       incSizeT(&idx,maxIdx)) {
                            const size_t val        = Y[idx];
                            const size_t num0ExcIdx = Y.num0() - (0 == val);
                            const size_t num1ExcIdx = Y.num1() - (1 == val);

                            mY.assign1(idx);
                            ASSERT(Y[idx]);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP5_ASSERT(ti, idx, num0ExcIdx, Y, Y.num0(),
                                         num0ExcIdx == Y.num0());
                            LOOP4_ASSERT(ti, 1 + num1ExcIdx, Y, Y.num1(),
                                         1 + num1ExcIdx == Y.num1());

                            mY.assign(idx, val);
                            ASSERT(X == Y);

                            mY.assign0(idx);
                            ASSERT(!Y[idx]);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            ASSERT(false == Y[idx]);
                            LOOP_ASSERT(ti, 1 + num0ExcIdx == Y.num0());
                            LOOP_ASSERT(ti,     num1ExcIdx == Y.num1());

                            mY.assign(idx, val);
                            ASSERT(X == Y);

                            const size_t maxNb = X.length() - idx;
                            for (size_t nb = 0; nb <= maxNb;
                                                         incSizeT(&nb,maxNb)) {
                                LOOP2_ASSERT(SPEC, N, X == Y);

                                if (veryVerbose) {
                                    cout << "\t\t"; P_(N); P(Y);
                                }

                                mY.assign0(idx, nb);
                                size_t i;
                                for (i = 0; i < idx; ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }
                                for (; i < idx + nb; ++i) {
                                    ASSERT(0 == Y[i]);
                                }
                                for (; i < X.length(); ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }

                                mY.assign(idx, X, idx, nb);

                                LOOP2_ASSERT(SPEC, N, X == Y);

                                if (veryVerbose) {
                                    cout << "\t\t"; P_(N); P(Y);
                                }

                                mY.assign1(idx, nb);
                                for (i = 0; i < idx; ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }
                                for (; i < idx + nb; ++i) {
                                    ASSERT(1 == Y[i]);
                                }
                                for (; i < X.length(); ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }

                                mY.assign(idx, X, idx, nb);
                            }
                        }

                        CInt64 AA = testAllocator.numBlocksTotal();
                        ASSERT(BB == AA);
                    }
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.assign0(0));
            ASSERT_SAFE_PASS(mX.assign1(0));
            ASSERT_SAFE_PASS(mX.assign0(len - 1));
            ASSERT_SAFE_PASS(mX.assign1(len - 1));
            ASSERT_SAFE_FAIL(mX.assign0(len));
            ASSERT_SAFE_FAIL(mX.assign1(len));

            ASSERT_SAFE_PASS(mX.assign0(0, 0));
            ASSERT_SAFE_PASS(mX.assign1(0, 0));
            ASSERT_SAFE_PASS(mX.assign0(0, len));
            ASSERT_SAFE_PASS(mX.assign1(0, len));
            ASSERT_SAFE_PASS(mX.assign0(len - 1, 1));
            ASSERT_SAFE_PASS(mX.assign1(len - 1, 1));
            ASSERT_SAFE_PASS(mX.assign0(len, 0));
            ASSERT_SAFE_PASS(mX.assign1(len, 0));
            ASSERT_SAFE_FAIL(mX.assign0(  0, len + 1));
            ASSERT_SAFE_FAIL(mX.assign1(  0, len + 1));
            ASSERT_SAFE_FAIL(mX.assign0(len, 1));
            ASSERT_SAFE_FAIL(mX.assign1(len, 1));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING RESERVECAPACITY
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The resulting value is correct (unchanged).
        //:
        //: 2 The resulting capacity is correct (not less than initial).
        //:
        //: 3 The method is exception neutral w.r.t. allocation.
        //:
        //: 4 The resulting value is unchanged in the event of
        //:   exceptions.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a table of initial object values and subsequent
        //:   capacities to reserve.
        //:
        //: 2 Construct each tabulated value.
        //:
        //: 3 Call 'reserveCapacity' with the tabulated number of elements.
        //:
        //: 4 Confirm that the test object has the same value as a separately
        //:   constructed control object.  (C-1)
        //:
        //: 5 Append as many values as required to bring the test object's
        //:   length to the specified number of elements, and use
        //:   'bslma::TestAllocator' to verify that no additional allocations
        //:   have occurred.  (C-2)
        //:
        //: 6 Perform each test in the standard 'bslma' exception-testing macro
        //:   block.  (C-3) (C-4)
        //:
        //: 7 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void reserveCapacity(size_t ne);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING RESERVECAPACITY\n"
                               "=======================\n";

        // Test 'reserveCapacity' methods.

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_x;        // initial value
            int         d_ne;       // total number of elements to reserve
        } DATA[] = {
                //line  initialValue   numElements
                //----  ------------   -----------
                { L_,   "",               0        },
                { L_,   "",               1        },
                { L_,   "",               2        },
                { L_,   "",               3        },
                { L_,   "",               4        },
                { L_,   "",               5        },
                { L_,   "",              15        },
                { L_,   "",              16        },
                { L_,   "",              17        },

                { L_,   "0",              0        },
                { L_,   "0",              1        },
                { L_,   "0",              2        },
                { L_,   "0",              3        },
                { L_,   "0",              4        },
                { L_,   "0",              5        },
                { L_,   "0",             15        },
                { L_,   "0",             16        },
                { L_,   "0",             17        },
                { L_,   "0",             32        },
                { L_,   "0",             33        },
                { L_,   "0",             64        },
                { L_,   "0",             65        },
                { L_,   "0",             96        },
                { L_,   "0",             97        },
                { L_,   "0",            128        },
                { L_,   "0",            129        },

                { L_,   "01",             0        },
                { L_,   "01",             1        },
                { L_,   "01",             2        },
                { L_,   "01",             3        },
                { L_,   "01",             4        },
                { L_,   "01",             5        },
                { L_,   "01",            15        },
                { L_,   "01",            16        },
                { L_,   "01",            17        },
                { L_,   "01",            32        },
                { L_,   "01",            33        },
                { L_,   "01",            64        },
                { L_,   "01",            65        },
                { L_,   "01",            96        },
                { L_,   "01",            97        },
                { L_,   "01",           128        },
                { L_,   "01",           129        },

                { L_,   "01100",          0        },
                { L_,   "01100",          1        },
                { L_,   "01100",          2        },
                { L_,   "01100",          3        },
                { L_,   "01100",          4        },
                { L_,   "01100",          5        },
                { L_,   "01100",         15        },
                { L_,   "01100",         16        },
                { L_,   "01100",         17        },
                { L_,   "01100",         32        },
                { L_,   "01100",         33        },
                { L_,   "01100",         64        },
                { L_,   "01100",         65        },
                { L_,   "01100",         96        },
                { L_,   "01100",         97        },
                { L_,   "01100",        128        },
                { L_,   "01100",        129        },

                { L_,   "xwwa",         275,       },
                { L_,   "xwwqha",       300,       },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose)
           cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int     LINE = DATA[ti].d_lineNum;
                const char   *SPEC = DATA[ti].d_x;
                const size_t  NE   = DATA[ti].d_ne;
                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P(NE);
                }

                const Obj W(gDispatch(SPEC), &testAllocator);
                Obj       mX(W, &testAllocator); const Obj &X = mX;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    ASSERT(X == W);

                    mX.reserveCapacity(NE);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, W == X);
                const Int64 NUM_BLOCKS = testAllocator.numBlocksTotal();
                const Int64 NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) P_(X);
                for (size_t i = X.length(); i < NE; ++i) {
                    mX.append(0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.reserveCapacity(0));
            ASSERT_SAFE_PASS(mX.reserveCapacity(len));
            ASSERT_SAFE_PASS(mX.reserveCapacity(4 * len));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING TWO-BIT SWAPBITS METHOD
        //
        // Concerns:
        //: 1 The method swaps the specified pair of values.
        //:
        //: 2 The method leaves all other values in the container untouched.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each length specified in a table, randomly populate a bit
        //:   array of that length.
        //:
        //: 2 For any possible pair of valid indexes into the bit array, call
        //:   'swapBits'.
        //:
        //: 3 Observe that the values at the two indexes are swapped.  (C-1)
        //:
        //: 4 Verify that no other values were changed.  (C-2)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   void swapBits(size_t index1, size_t index2);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TWO-BIT SWAPBITS METHOD\n"
                               "===============================\n";

        if (verbose) cout << "\nTesting swapBits(index1, index2)" << endl;

        const int CONTAINER_LENGTHS[] =
                               { 1, 2, 8, 63, 64, 65, 96, 127, 128, 129, 136 };
        enum { NUM_CONTAINER_LENGTHS = sizeof CONTAINER_LENGTHS /
                                                   sizeof *CONTAINER_LENGTHS };

        for (int ii = 0; ii < NUM_CONTAINER_LENGTHS; ++ii) {
            const int LENGTH = CONTAINER_LENGTHS[ii];

            Obj mX(&testAllocator);  const Obj& X = mX;  // control
            for (int jj = LENGTH; jj > 0; jj -= 10) {
                int r = bsl::rand();

                const int end = bsl::min(10, jj);
                for (int kk = 0; kk < end; ++kk) {
                    mX.append(r & (1 << kk));
                }
            }
            ASSERT(static_cast<int>(X.length()) == LENGTH);

            Obj mT(X, &testAllocator);    const Obj& T = mT;

            for (int pos1 = 0; pos1 < LENGTH; ++pos1) { // for each position
                for (int pos2 = 0; pos2 < LENGTH; ++pos2) {// for each position
                    const bool val1 = T[pos1];
                    const bool val2 = T[pos2];

                    mT.swapBits(pos1, pos2);

                    ASSERTV(val2, T[pos1], val2 == T[pos1]);
                    ASSERTV(val1, T[pos2], val1 == T[pos2]);

                    for (int i = 0; i < LENGTH; ++i) {
                        if ((i == pos1 || i == pos2) && (pos1 != pos2)) {
                            continue;
                        }
                        LOOP4_ASSERT(LENGTH, pos1, pos2, i,
                                     X[i] == T[i]);
                    }

                    mT.swapBits(pos1, pos2);

                    LOOP3_ASSERT(LENGTH, pos1, pos2, X == T);
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            size_t len = X.length();

            ASSERT_SAFE_PASS(mX.swapBits(0,     0));
            ASSERT_SAFE_PASS(mX.swapBits(0,     len-1));
            ASSERT_SAFE_PASS(mX.swapBits(len-1, 0));
            ASSERT_SAFE_FAIL(mX.swapBits(0,     len));
            ASSERT_SAFE_FAIL(mX.swapBits(len,   0));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ASSIGN
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The source is left unaffected (apart from aliasing).
        //:
        //: 2 The subsequent existence of the source has no effect on the
        //:   result object (apart from aliasing).
        //:
        //: 3 The function is alias-safe.
        //:
        //: 4 The function preserves object invariants.
        //:
        //: 5 Note that the 'assign' methods cannot allocate, but are tested
        //:   for exceptions anyway.
        //:
        //: 6 Test multi-word bit arrays.
        //:
        //: 7 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 In the "canonical state" (black-box) tests, we confirm that the
        //:   source is unmodified by the method call, and that its subsequent
        //:   destruction has no effect on the destination object.  (C-1) (C-2)
        //:
        //: 2 In all cases we want to make sure that after the application of
        //:   the operation, the object is allowed to go out of scope directly
        //:   to enable the destructor to assert object invariants.  (C-4)
        //:
        //: 3 Each object constructed should be wrapped in separate BSLMA test
        //:   assert macros and use gg as an optimization.  (C-5)
        //:
        //: 4 Generate random specs to drive the creation of dest and src bit
        //:   arrays, and do string operations to generate the spec of the
        //:   expected result, use that spec to create the expected result, and
        //:   compare with the result of the function.
        //:
        //: 5 Ensure that each function verifies all preconditions.  (C-7)
        //
        // Testing:
        //   void assign(size_t di, bool value);
        //   void assign(size_t di, bool value, size_t numBits);
        //   void assign(size_t di, const BitArray& sa, size_t si, size_t ne);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ASSIGN\n"
                               "==============\n";

        if (verbose) cout <<
            "\nTesting assign(di, sa, si, ne) et al.(no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to assign into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to assign from sa
                int         d_ne;       // number of elements to assign
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "0",     0,  "",      0,  0, "0"     },
                { L_,   "0",     1,  "",      0,  0, "0"     },

                { L_,   "",      0,  "1",     0,  0, ""      },
                { L_,   "",      0,  "1",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "01",    0,  "",      0,  0, "01"    },
                { L_,   "01",    1,  "",      0,  0, "01"    },
                { L_,   "01",    2,  "",      0,  0, "01"    },

                { L_,   "0",     0,  "1",     0,  0, "0"     },
                { L_,   "0",     0,  "1",     0,  1, "1"     },
                { L_,   "0",     0,  "1",     1,  0, "0"     },
                { L_,   "0",     1,  "1",     0,  0, "0"     },
                { L_,   "0",     1,  "1",     1,  0, "0"     },

                { L_,   "",      0,  "01",    0,  0, ""      },
                { L_,   "",      0,  "01",    1,  0, ""      },
                { L_,   "",      0,  "01",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "011",   0,  "",      0,  0, "011"   },
                { L_,   "011",   1,  "",      0,  0, "011"   },
                { L_,   "011",   2,  "",      0,  0, "011"   },
                { L_,   "011",   3,  "",      0,  0, "011"   },

                { L_,   "01",    0,  "1",     0,  0, "01"    },
                { L_,   "01",    0,  "1",     0,  1, "11"    },
                { L_,   "01",    0,  "1",     1,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  1, "01"    },
                { L_,   "01",    1,  "1",     1,  0, "01"    },
                { L_,   "01",    2,  "1",     0,  0, "01"    },
                { L_,   "01",    2,  "1",     1,  0, "01"    },

                { L_,   "0",     0,  "11",    0,  0, "0"     },
                { L_,   "0",     0,  "11",    0,  1, "1"     },
                { L_,   "0",     0,  "11",    1,  0, "0"     },
                { L_,   "0",     0,  "11",    1,  1, "1"     },
                { L_,   "0",     0,  "11",    2,  0, "0"     },
                { L_,   "0",     1,  "11",    0,  0, "0"     },
                { L_,   "0",     1,  "11",    1,  0, "0"     },
                { L_,   "0",     1,  "11",    2,  0, "0"     },

                { L_,   "",      0,  "011",   0,  0, ""      },
                { L_,   "",      0,  "011",   1,  0, ""      },
                { L_,   "",      0,  "011",   2,  0, ""      },
                { L_,   "",      0,  "011",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "0110",  0,  "",      0,  0, "0110"  },
                { L_,   "0110",  1,  "",      0,  0, "0110"  },
                { L_,   "0110",  2,  "",      0,  0, "0110"  },
                { L_,   "0110",  3,  "",      0,  0, "0110"  },
                { L_,   "0110",  4,  "",      0,  0, "0110"  },

                { L_,   "011",   0,  "0",     0,  0, "011"   },
                { L_,   "011",   0,  "0",     0,  1, "011"   },
                { L_,   "011",   0,  "0",     1,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  1, "001"   },
                { L_,   "011",   1,  "0",     1,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  1, "010"   },
                { L_,   "011",   2,  "0",     1,  0, "011"   },
                { L_,   "011",   3,  "0",     0,  0, "011"   },
                { L_,   "011",   3,  "0",     1,  0, "011"   },

                { L_,   "01",    0,  "10",    0,  0, "01"    },
                { L_,   "01",    0,  "10",    0,  1, "11"    },
                { L_,   "01",    0,  "10",    0,  2, "10"    },
                { L_,   "01",    0,  "10",    1,  0, "01"    },
                { L_,   "01",    0,  "10",    1,  1, "01"    },
                { L_,   "01",    0,  "10",    2,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  1, "01"    },
                { L_,   "01",    1,  "10",    1,  0, "01"    },
                { L_,   "01",    1,  "10",    1,  1, "00"    },
                { L_,   "01",    1,  "10",    2,  0, "01"    },
                { L_,   "01",    2,  "10",    0,  0, "01"    },
                { L_,   "01",    2,  "10",    1,  0, "01"    },
                { L_,   "01",    2,  "10",    2,  0, "01"    },

                { L_,   "0",     0,  "110",   0,  0, "0"     },
                { L_,   "0",     0,  "110",   0,  1, "1"     },
                { L_,   "0",     0,  "110",   1,  0, "0"     },
                { L_,   "0",     0,  "110",   1,  1, "1"     },
                { L_,   "0",     0,  "110",   2,  0, "0"     },
                { L_,   "0",     0,  "110",   2,  1, "0"     },
                { L_,   "0",     0,  "110",   3,  0, "0"     },
                { L_,   "0",     1,  "110",   0,  0, "0"     },
                { L_,   "0",     1,  "110",   1,  0, "0"     },
                { L_,   "0",     1,  "110",   2,  0, "0"     },
                { L_,   "0",     1,  "110",   3,  0, "0"     },

                { L_,   "",      0,  "0110",  0,  0, ""      },
                { L_,   "",      0,  "0110",  1,  0, ""      },
                { L_,   "",      0,  "0110",  2,  0, ""      },
                { L_,   "",      0,  "0110",  3,  0, ""      },
                { L_,   "",      0,  "0110",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "01100", 0,  "",      0,  0, "01100" },
                { L_,   "01100", 1,  "",      0,  0, "01100" },
                { L_,   "01100", 2,  "",      0,  0, "01100" },
                { L_,   "01100", 3,  "",      0,  0, "01100" },
                { L_,   "01100", 4,  "",      0,  0, "01100" },
                { L_,   "01100", 5,  "",      0,  0, "01100" },

                { L_,   "0110",  0,  "0",     0,  0, "0110"  },
                { L_,   "0110",  0,  "0",     0,  1, "0110"  },
                { L_,   "0110",  0,  "0",     1,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  1, "0010"  },
                { L_,   "0110",  1,  "0",     1,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  1, "0100"  },
                { L_,   "0110",  2,  "0",     1,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  1, "0110"  },
                { L_,   "0110",  3,  "0",     1,  0, "0110"  },
                { L_,   "0110",  4,  "0",     0,  0, "0110"  },
                { L_,   "0110",  4,  "0",     1,  0, "0110"  },

                { L_,   "011",   0,  "00",    0,  0, "011"   },
                { L_,   "011",   0,  "00",    0,  1, "011"   },
                { L_,   "011",   0,  "00",    0,  2, "001"   },
                { L_,   "011",   0,  "00",    1,  0, "011"   },
                { L_,   "011",   0,  "00",    1,  1, "011"   },
                { L_,   "011",   0,  "00",    2,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  1, "001"   },
                { L_,   "011",   1,  "00",    0,  2, "000"   },
                { L_,   "011",   1,  "00",    1,  0, "011"   },
                { L_,   "011",   1,  "00",    1,  1, "001"   },
                { L_,   "011",   1,  "00",    2,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  1, "010"   },
                { L_,   "011",   2,  "00",    1,  0, "011"   },
                { L_,   "011",   2,  "00",    1,  1, "010"   },
                { L_,   "011",   2,  "00",    2,  0, "011"   },
                { L_,   "011",   3,  "00",    0,  0, "011"   },
                { L_,   "011",   3,  "00",    1,  0, "011"   },
                { L_,   "011",   3,  "00",    2,  0, "011"   },

                { L_,   "01",    0,  "100",   0,  0, "01"    },
                { L_,   "01",    0,  "100",   0,  1, "11"    },
                { L_,   "01",    0,  "100",   0,  2, "10"    },
                { L_,   "01",    0,  "100",   1,  0, "01"    },
                { L_,   "01",    0,  "100",   1,  1, "01"    },
                { L_,   "01",    0,  "100",   2,  0, "01"    },
                { L_,   "01",    0,  "100",   2,  1, "01"    },
                { L_,   "01",    0,  "100",   3,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  1, "01"    },
                { L_,   "01",    1,  "100",   1,  0, "01"    },
                { L_,   "01",    1,  "100",   1,  1, "00"    },
                { L_,   "01",    1,  "100",   2,  0, "01"    },
                { L_,   "01",    1,  "100",   2,  1, "00"    },
                { L_,   "01",    1,  "100",   3,  0, "01"    },
                { L_,   "01",    2,  "100",   0,  0, "01"    },
                { L_,   "01",    2,  "100",   1,  0, "01"    },
                { L_,   "01",    2,  "100",   2,  0, "01"    },
                { L_,   "01",    2,  "100",   3,  0, "01"    },

                { L_,   "0",     0,  "1100",  0,  0, "0"     },
                { L_,   "0",     0,  "1100",  0,  1, "1"     },
                { L_,   "0",     0,  "1100",  1,  0, "0"     },
                { L_,   "0",     0,  "1100",  1,  1, "1"     },
                { L_,   "0",     0,  "1100",  2,  0, "0"     },
                { L_,   "0",     0,  "1100",  2,  1, "0"     },
                { L_,   "0",     0,  "1100",  3,  0, "0"     },
                { L_,   "0",     0,  "1100",  3,  1, "0"     },
                { L_,   "0",     0,  "1100",  4,  0, "0"     },
                { L_,   "0",     1,  "1100",  0,  0, "0"     },
                { L_,   "0",     1,  "1100",  1,  0, "0"     },
                { L_,   "0",     1,  "1100",  2,  0, "0"     },
                { L_,   "0",     1,  "1100",  3,  0, "0"     },
                { L_,   "0",     1,  "1100",  4,  0, "0"     },

                { L_,   "",      0,  "01100", 0,  0, ""      },
                { L_,   "",      0,  "01100", 1,  0, ""      },
                { L_,   "",      0,  "01100", 2,  0, ""      },
                { L_,   "",      0,  "01100", 3,  0, ""      },
                { L_,   "",      0,  "01100", 4,  0, ""      },
                { L_,   "",      0,  "01100", 5,  0, ""      },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = static_cast<int>(strlen(D_SPEC) +
                                                               strlen(S_SPEC));
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (veryVerbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(gDispatch(D_SPEC));      // control for destination
                Obj SS(gDispatch(S_SPEC));      // control for source
                Obj EE(gDispatch(E_SPEC));      // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\tassign(di, sa, si, ne)" << endl;
                {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    Obj s(SS, &testAllocator);  const Obj &S = s;

                    int count = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ++count;
                        ASSERT(X == DD);
                        ASSERT(S == SS);

                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.assign(DI, s, SI, NE);     // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);   // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERT(1 == count);
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                if (veryVerbose)
                               cout << "\t\tassign(di, value)" << endl;
                if (1 == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    Obj s(SS, &testAllocator);  const Obj &S = s;

                    int count = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ++count;
                        ASSERT(X == DD);
                        ASSERT(S == SS);

                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.assign(DI, s[SI]);           // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERT(1 == count);
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const Int64 NUM_BLOCKS = testAllocator.numBlocksTotal();
                const Int64 NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tassign(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.assign(DI, SS, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tassign(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.assign(DI, SS[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.assign(di, sa, si, ne) et al.(aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,  0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "0",     0,  0,  0,  "0"     },
                { L_,   "0",     0,  0,  1,  "0"     },
                { L_,   "0",     0,  1,  0,  "0"     },

                { L_,   "0",     1,  0,  0,  "0"     },
                { L_,   "0",     1,  1,  0,  "0"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "01",    0,  0,  0,  "01"    },
                { L_,   "01",    0,  0,  1,  "01"    },
                { L_,   "01",    0,  0,  2,  "01"    },
                { L_,   "01",    0,  1,  0,  "01"    },
                { L_,   "01",    0,  1,  1,  "11"    },
                { L_,   "01",    0,  2,  0,  "01"    },

                { L_,   "01",    1,  0,  0,  "01"    },
                { L_,   "01",    1,  0,  1,  "00"    },
                { L_,   "01",    1,  1,  0,  "01"    },
                { L_,   "01",    1,  1,  1,  "01"    },
                { L_,   "01",    1,  2,  0,  "01"    },

                { L_,   "01",    2,  0,  0,  "01"    },
                { L_,   "01",    2,  1,  0,  "01"    },
                { L_,   "01",    2,  2,  0,  "01"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "011",   0,  0,  0,  "011"   },
                { L_,   "011",   0,  0,  1,  "011"   },
                { L_,   "011",   0,  0,  2,  "011"   },
                { L_,   "011",   0,  0,  3,  "011"   },
                { L_,   "011",   0,  1,  0,  "011"   },
                { L_,   "011",   0,  1,  1,  "111"   },
                { L_,   "011",   0,  1,  2,  "111"   },
                { L_,   "011",   0,  2,  0,  "011"   },
                { L_,   "011",   0,  2,  1,  "111"   },
                { L_,   "011",   0,  3,  0,  "011"   },

                { L_,   "011",   1,  0,  0,  "011"   },
                { L_,   "011",   1,  0,  1,  "001"   },
                { L_,   "011",   1,  0,  2,  "001"   },
                { L_,   "011",   1,  1,  0,  "011"   },
                { L_,   "011",   1,  1,  1,  "011"   },
                { L_,   "011",   1,  1,  2,  "011"   },
                { L_,   "011",   1,  2,  0,  "011"   },
                { L_,   "011",   1,  2,  1,  "011"   },
                { L_,   "011",   1,  3,  0,  "011"   },

                { L_,   "011",   2,  0,  0,  "011"   },
                { L_,   "011",   2,  0,  1,  "010"   },
                { L_,   "011",   2,  1,  0,  "011"   },
                { L_,   "011",   2,  1,  1,  "011"   },
                { L_,   "011",   2,  2,  0,  "011"   },
                { L_,   "011",   2,  2,  1,  "011"   },
                { L_,   "011",   2,  3,  0,  "011"   },

                { L_,   "011",   3,  0,  0,  "011"   },
                { L_,   "011",   3,  1,  0,  "011"   },
                { L_,   "011",   3,  2,  0,  "011"   },
                { L_,   "011",   3,  3,  0,  "011"   },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "0110",  0,  0,  0,  "0110"  },
                { L_,   "0110",  0,  0,  1,  "0110"  },
                { L_,   "0110",  0,  0,  2,  "0110"  },
                { L_,   "0110",  0,  0,  3,  "0110"  },
                { L_,   "0110",  0,  0,  4,  "0110"  },
                { L_,   "0110",  0,  1,  0,  "0110"  },
                { L_,   "0110",  0,  1,  1,  "1110"  },
                { L_,   "0110",  0,  1,  2,  "1110"  },
                { L_,   "0110",  0,  1,  3,  "1100"  },
                { L_,   "0110",  0,  2,  0,  "0110"  },
                { L_,   "0110",  0,  2,  1,  "1110"  },
                { L_,   "0110",  0,  2,  2,  "1010"  },
                { L_,   "0110",  0,  3,  0,  "0110"  },
                { L_,   "0110",  0,  3,  1,  "0110"  },
                { L_,   "0110",  0,  4,  0,  "0110"  },

                { L_,   "0110",  1,  0,  0,  "0110"  },
                { L_,   "0110",  1,  0,  1,  "0010"  },
                { L_,   "0110",  1,  0,  2,  "0010"  },
                { L_,   "0110",  1,  0,  3,  "0011"  },
                { L_,   "0110",  1,  1,  0,  "0110"  },
                { L_,   "0110",  1,  1,  1,  "0110"  },
                { L_,   "0110",  1,  1,  2,  "0110"  },
                { L_,   "0110",  1,  1,  3,  "0110"  },
                { L_,   "0110",  1,  2,  0,  "0110"  },
                { L_,   "0110",  1,  2,  1,  "0110"  },
                { L_,   "0110",  1,  2,  2,  "0100"  },
                { L_,   "0110",  1,  3,  0,  "0110"  },
                { L_,   "0110",  1,  3,  1,  "0010"  },
                { L_,   "0110",  1,  4,  0,  "0110"  },

                { L_,   "0110",  2,  0,  0,  "0110"  },
                { L_,   "0110",  2,  0,  1,  "0100"  },
                { L_,   "0110",  2,  0,  2,  "0101"  },
                { L_,   "0110",  2,  1,  0,  "0110"  },
                { L_,   "0110",  2,  1,  1,  "0110"  },
                { L_,   "0110",  2,  1,  2,  "0111"  },
                { L_,   "0110",  2,  2,  0,  "0110"  },
                { L_,   "0110",  2,  2,  1,  "0110"  },
                { L_,   "0110",  2,  2,  2,  "0110"  },
                { L_,   "0110",  2,  3,  0,  "0110"  },
                { L_,   "0110",  2,  3,  1,  "0100"  },
                { L_,   "0110",  2,  4,  0,  "0110"  },

                { L_,   "0110",  3,  0,  0,  "0110"  },
                { L_,   "0110",  3,  0,  1,  "0110"  },
                { L_,   "0110",  3,  1,  0,  "0110"  },
                { L_,   "0110",  3,  1,  1,  "0111"  },
                { L_,   "0110",  3,  2,  0,  "0110"  },
                { L_,   "0110",  3,  2,  1,  "0111"  },
                { L_,   "0110",  3,  3,  0,  "0110"  },
                { L_,   "0110",  3,  3,  1,  "0110"  },
                { L_,   "0110",  3,  4,  0,  "0110"  },

                { L_,   "0110",  4,  0,  0,  "0110"  },
                { L_,   "0110",  4,  1,  0,  "0110"  },
                { L_,   "0110",  4,  2,  0,  "0110"  },
                { L_,   "0110",  4,  3,  0,  "0110"  },
                { L_,   "0110",  4,  4,  0,  "0110"  },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "01100", 0,  0,  0,  "01100" },
                { L_,   "01100", 0,  0,  1,  "01100" },
                { L_,   "01100", 0,  0,  2,  "01100" },
                { L_,   "01100", 0,  0,  3,  "01100" },
                { L_,   "01100", 0,  0,  4,  "01100" },
                { L_,   "01100", 0,  0,  5,  "01100" },
                { L_,   "01100", 0,  1,  0,  "01100" },
                { L_,   "01100", 0,  1,  1,  "11100" },
                { L_,   "01100", 0,  1,  2,  "11100" },
                { L_,   "01100", 0,  1,  3,  "11000" },
                { L_,   "01100", 0,  1,  4,  "11000" },
                { L_,   "01100", 0,  2,  0,  "01100" },
                { L_,   "01100", 0,  2,  1,  "11100" },
                { L_,   "01100", 0,  2,  2,  "10100" },
                { L_,   "01100", 0,  2,  3,  "10000" },
                { L_,   "01100", 0,  3,  0,  "01100" },
                { L_,   "01100", 0,  3,  1,  "01100" },
                { L_,   "01100", 0,  3,  2,  "00100" },
                { L_,   "01100", 0,  4,  0,  "01100" },
                { L_,   "01100", 0,  4,  1,  "01100" },
                { L_,   "01100", 0,  5,  0,  "01100" },

                { L_,   "01100", 1,  0,  0,  "01100" },
                { L_,   "01100", 1,  0,  1,  "00100" },
                { L_,   "01100", 1,  0,  2,  "00100" },
                { L_,   "01100", 1,  0,  3,  "00110" },
                { L_,   "01100", 1,  0,  4,  "00110" },
                { L_,   "01100", 1,  1,  0,  "01100" },
                { L_,   "01100", 1,  1,  1,  "01100" },
                { L_,   "01100", 1,  1,  2,  "01100" },
                { L_,   "01100", 1,  1,  3,  "01100" },
                { L_,   "01100", 1,  1,  4,  "01100" },
                { L_,   "01100", 1,  2,  0,  "01100" },
                { L_,   "01100", 1,  2,  1,  "01100" },
                { L_,   "01100", 1,  2,  2,  "01000" },
                { L_,   "01100", 1,  2,  3,  "01000" },
                { L_,   "01100", 1,  3,  0,  "01100" },
                { L_,   "01100", 1,  3,  1,  "00100" },
                { L_,   "01100", 1,  3,  2,  "00000" },
                { L_,   "01100", 1,  4,  0,  "01100" },
                { L_,   "01100", 1,  4,  1,  "00100" },
                { L_,   "01100", 1,  5,  0,  "01100" },

                { L_,   "01100", 2,  0,  0,  "01100" },
                { L_,   "01100", 2,  0,  1,  "01000" },
                { L_,   "01100", 2,  0,  2,  "01010" },
                { L_,   "01100", 2,  0,  3,  "01011" },
                { L_,   "01100", 2,  1,  0,  "01100" },
                { L_,   "01100", 2,  1,  1,  "01100" },
                { L_,   "01100", 2,  1,  2,  "01110" },
                { L_,   "01100", 2,  1,  3,  "01110" },
                { L_,   "01100", 2,  2,  0,  "01100" },
                { L_,   "01100", 2,  2,  1,  "01100" },
                { L_,   "01100", 2,  2,  2,  "01100" },
                { L_,   "01100", 2,  2,  3,  "01100" },
                { L_,   "01100", 2,  3,  0,  "01100" },
                { L_,   "01100", 2,  3,  1,  "01000" },
                { L_,   "01100", 2,  3,  2,  "01000" },
                { L_,   "01100", 2,  4,  0,  "01100" },
                { L_,   "01100", 2,  4,  1,  "01000" },
                { L_,   "01100", 2,  5,  0,  "01100" },

                { L_,   "01100", 3,  0,  0,  "01100" },
                { L_,   "01100", 3,  0,  1,  "01100" },
                { L_,   "01100", 3,  0,  2,  "01101" },
                { L_,   "01100", 3,  1,  0,  "01100" },
                { L_,   "01100", 3,  1,  1,  "01110" },
                { L_,   "01100", 3,  1,  2,  "01111" },
                { L_,   "01100", 3,  2,  0,  "01100" },
                { L_,   "01100", 3,  2,  1,  "01110" },
                { L_,   "01100", 3,  2,  2,  "01110" },
                { L_,   "01100", 3,  3,  0,  "01100" },
                { L_,   "01100", 3,  3,  1,  "01100" },
                { L_,   "01100", 3,  3,  2,  "01100" },
                { L_,   "01100", 3,  4,  0,  "01100" },
                { L_,   "01100", 3,  4,  1,  "01100" },
                { L_,   "01100", 3,  5,  0,  "01100" },

                { L_,   "01100", 4,  0,  0,  "01100" },
                { L_,   "01100", 4,  0,  1,  "01100" },
                { L_,   "01100", 4,  1,  0,  "01100" },
                { L_,   "01100", 4,  1,  1,  "01101" },
                { L_,   "01100", 4,  2,  0,  "01100" },
                { L_,   "01100", 4,  2,  1,  "01101" },
                { L_,   "01100", 4,  3,  0,  "01100" },
                { L_,   "01100", 4,  3,  1,  "01100" },
                { L_,   "01100", 4,  4,  0,  "01100" },
                { L_,   "01100", 4,  4,  1,  "01100" },
                { L_,   "01100", 4,  5,  0,  "01100" },

                { L_,   "01100", 5,  0,  0,  "01100" },
                { L_,   "01100", 5,  1,  0,  "01100" },
                { L_,   "01100", 5,  2,  0,  "01100" },
                { L_,   "01100", 5,  3,  0,  "01100" },
                { L_,   "01100", 5,  4,  0,  "01100" },
                { L_,   "01100", 5,  5,  0,  "01100" },

            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = static_cast<int>(strlen(X_SPEC));
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (veryVerbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(gDispatch(X_SPEC));      // control for destination
                Obj EE(gDispatch(E_SPEC));      // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Obj x(DD, &testAllocator);   const Obj &X = x;
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.assign(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\tassign(di, sa, si, ne)" << endl;
                {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.assign(DI, X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) cout << "\t\tassign(di, value)" << endl;
                if (1 == NE) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.assign(DI, X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                if (veryVerbose) cout << "\t\tassign(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.assign(DI, X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tassign(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.assign(DI, X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }
            }
        }

        if (verbose) cout << "Testing assigns with random specs\n";
        for (int ii = 0; ii < 1000; ++ii) {
            enum { MOD = 2 * CHAR_BIT * sizeof(uint64_t) };

            const int dstLen  = bsl::rand() % MOD + 1;
            const int srcLen  = bsl::rand() % (MOD + 1);

            ASSERT(dstLen >= 1);

            const bsl::string& dstSpec = randSpec(dstLen);
            const bsl::string& srcSpec = randSpec(srcLen);

            const Obj& DD = gDispatch(dstSpec.c_str());
            const Obj& SS = gDispatch(srcSpec.c_str());

            Obj mS(SS);  const Obj& S = mS;

            if (veryVerbose) {
                P_(ii); P_(dstLen); P(srcLen);
                P(DD); P(SS);
            }

            for (int jj = 0; jj < 10; ++jj) {
                const int dstIdx  = bsl::rand() % dstLen;
                int       srcIdx  = bsl::rand() % (srcLen + 1);
                int       numBits = bsl::rand() %
                              (bsl::min(srcLen - srcIdx, dstLen - dstIdx) + 1);

                if (veryVerbose) {
                    P_(ii); P_(jj); P_(dstIdx); P_(srcIdx); P(numBits);
                }

                if (veryVerbose) cout << "assign(" << dstIdx << ", bool);\n";
                for (int bit = 0; bit < 2; ++bit) {
                    Obj mD(DD);    const Obj& D = mD;

                    mD.assign(dstIdx, bit);

                    const bsl::string& expSpec = dstSpec.substr(0, dstIdx) +
                                                 (bit ? "1" : "0") +
                                                 dstSpec.substr(dstIdx + 1);
                    const Obj&         EXP     = gDispatch(expSpec.c_str());

                    ASSERT(EXP == D);
                }

                if (veryVerbose) cout << "assign(" << dstIdx << ", bool, " <<
                                                                 "numBits);\n";
                for (int bit = 0; bit < 2; ++bit) {
                    Obj mD(DD);    const Obj& D = mD;

                    mD.assign(dstIdx, bit, numBits);

                    const bsl::string  newBits(static_cast<size_t>(numBits),
                                               (bit ? '1' : '0'));
                    const bsl::string& expSpec =
                                              dstSpec.substr(0, dstIdx) +
                                              newBits +
                                              dstSpec.substr(dstIdx + numBits);
                    const Obj&         EXP     = gDispatch(expSpec.c_str());

                    ASSERT(EXP == D);
                }

                if (veryVerbose) cout << "assign(" << dstIdx << ", S, " <<
                                           srcIdx << ", " << numBits << ");\n";
                {
                    Obj mD(DD);    const Obj& D = mD;

                    mD.assign(dstIdx, S, srcIdx, numBits);

                    const bsl::string& expSpec =
                                              dstSpec.substr(0, dstIdx) +
                                              srcSpec.substr(srcIdx, numBits) +
                                              dstSpec.substr(dstIdx + numBits);
                    const Obj&         EXP     = gDispatch(expSpec.c_str());

                    ASSERT(EXP == D);
                    ASSERT(SS  == S);
                }

                if (veryVerbose) cout <<  "aliasing: mD.assign(" << dstIdx <<
                         ", mD, " << srcIdx << ", " << numBits << ");" << endl;
                {
                    srcIdx  = bsl::rand() % (dstLen + 1);
                    numBits = bsl::rand() %
                              (bsl::min(dstLen - dstIdx, dstLen - srcIdx) + 1);

                    Obj mD(DD);    const Obj& D = mD;

                    mD.assign(dstIdx, mD, srcIdx, numBits);

                    const bsl::string& expSpec =
                                             dstSpec.substr(0, dstIdx) +
                                             dstSpec.substr(srcIdx, numBits) +
                                             dstSpec.substr(dstIdx + numBits);
                    const Obj&         EXP     = gDispatch(expSpec.c_str());

                    ASSERTV(EXP, DD, D, dstIdx, srcIdx, numBits, EXP == D);
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            const size_t len = X.length();

            ASSERT_SAFE_PASS(mX.assign(0,     true));
            ASSERT_SAFE_PASS(mX.assign(len-1, true));
            ASSERT_SAFE_FAIL(mX.assign(-1,    true));
            ASSERT_SAFE_FAIL(mX.assign(len,   true));

            ASSERT_SAFE_PASS(mX.assign(  0, true,   0));
            ASSERT_SAFE_PASS(mX.assign(len-1, true,   1));
            ASSERT_SAFE_PASS(mX.assign(0, true,   len));
            ASSERT_SAFE_FAIL(mX.assign(len, true, 1));
            ASSERT_SAFE_FAIL(mX.assign(0, true, len + 1));

            ASSERT_SAFE_PASS(mX.assign(  0, X,   0,   0));
            ASSERT_SAFE_PASS(mX.assign(  0, X,   0, len));
            ASSERT_SAFE_PASS(mX.assign(  0, X, len,   0));
            ASSERT_SAFE_PASS(mX.assign(len, X, len,   0));
            ASSERT_SAFE_FAIL(mX.assign(  0, X, len,   1));
            ASSERT_SAFE_FAIL(mX.assign(  0, X,   0, len + 1));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING APPEND, INSERT, REMOVE
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The destination has the correct value.
        //:
        //: 2 The source is left unaffected (apart from aliasing).
        //:
        //: 3 The subsequent existing of the source has no effect on the
        //:   result object (apart from aliasing).
        //:
        //: 4 The function is alias-safe.
        //:
        //: 5 The function is exception neutral (w.r.t. allocation).
        //:
        //: 6 The function preserves object invariants.
        //:
        //: 7 QoI: asserted precondition violations are detected when enabled.
        //
        //    For the 'remove' methods, the concerns are simply to cover the
        //    full range of possible indices and numbers of elements.
        //
        // Plan:
        //: 1 Do the table-driven non-aliasing tests:
        //:   o Generate dest ('DD') src ('SS') and expected ('EE') values
        //:     from table.
        //:
        //:   o Copy construct objects 'X' and 'S' from 'DD' and 'SS'.
        //:
        //:   o Apply the operation between 'x' and 'S' and afterward compare
        //:     'X' with 'EE'.  (C-1)
        //:
        //:   o Enclose the operation in a 'BEGIN .. END' macro block to throw
        //:     exceptions when memory is allocated.  Catch the exceptions
        //:     before 'x' and 'S' are destroyed, and verify they still match
        //:     'DD' and 'SS'.  (C-5)
        //:
        //:   o Compare 'X' with 'EE' again after 'S' has gone out of scope,
        //:     to verify this had no influence on 'X'.  (C-3)
        //:
        //:   o Compare 'S' with 'SS'.  (C-2)
        //:
        //:   o In the case of append, only do the case where 'DI' (destination
        //:     index) equals the length of 'x', so the value of 'EE' for
        //:     'insert' will work.
        //:
        //:   o Exit the block where 'x' is created, so that its invariants are
        //:     checked by the d'tor.  (C-6)
        //:
        //: 2 Repeat '1', substituting 'X' for 'S', to test aliasing.  (C-4)
        //:
        //: 3 Do random testing to repeat the table-driven testing, but with
        //:   larger objects.
        //:   o Randomly choose the lengths 'dstLen' and 'srcLen' of the
        //:     destination and source.
        //:
        //:   o Use 'randSpec' to generate random binary specs of length
        //:     'dstLen' and 'srcLen'.
        //:
        //:   o Create an object 'S' from the source spec.
        //:
        //:   o In each block:
        //:     1 Create a destination object 'mD' from the destination spec.
        //:
        //:     2 Use string operations on the dest and src specs to generate a
        //:       binary spec of the result of the operation, create a bit
        //:       array 'EXP' from that spec.
        //:
        //:     3 Apply the operation and observe 'EXP == D'.  (C-1)
        //:
        //:     4 Do '3' in a 'BEGIN .. END' exception block, that beings after
        //:       'D' is created, and verify at the beginning of the block That
        //:       'D == DD'.  (C-5)
        //:
        //:     5 Compare 'S' with 'SS'.  (C-2)
        //:
        //:     6 Call 'removeAll' on 'S' and observe 'D' doesn't change.
        //:       (C-3)
        //:
        //:     7 Exit the block where 'x' is created, so that its invariants
        //:       are checked by the d'tor.  (C-6)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-7)
        //
        // Testing:
        //   void append(bool value);
        //   void append(bool value, size_t numBits);
        //   void append(const BitArray& sa);
        //   void append(const BitArray& sa, size_t si, size_t nb);
        //
        //   void insert(size_t di, bool value);
        //   void insert(size_t di, bool value, size_t numBits);
        //   void insert(size_t di, const BitArray& sa);
        //   void insert(size_t di, const BitArray& sa, size_t si, size_t ne);
        //
        //   void remove(size_t index);
        //   void remove(size_t index, size_t ne);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING APPEND, INSERT, REMOVE\n"
                               "==============================\n";

        testAllocator.setVerbose(veryVeryVeryVerbose);

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to insert into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to insert from sa
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "0",     0,  "",      0,  0, "0"     },
                { L_,   "0",     1,  "",      0,  0, "0"     },

                { L_,   "",      0,  "0",     0,  0, ""      },
                { L_,   "",      0,  "0",     0,  1, "0"     },
                { L_,   "",      0,  "0",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "01",    0,  "",      0,  0, "01"    },
                { L_,   "01",    1,  "",      0,  0, "01"    },
                { L_,   "01",    2,  "",      0,  0, "01"    },

                { L_,   "0",     0,  "1",     0,  0, "0"     },
                { L_,   "0",     0,  "1",     0,  1, "10"    },
                { L_,   "0",     0,  "1",     1,  0, "0"     },
                { L_,   "0",     1,  "1",     0,  0, "0"     },
                { L_,   "0",     1,  "1",     0,  1, "01"    },
                { L_,   "0",     1,  "1",     1,  0, "0"     },

                { L_,   "",      0,  "01",    0,  0, ""      },
                { L_,   "",      0,  "01",    0,  1, "0"     },
                { L_,   "",      0,  "01",    0,  2, "01"    },
                { L_,   "",      0,  "01",    1,  0, ""      },
                { L_,   "",      0,  "01",    1,  1, "1"     },
                { L_,   "",      0,  "01",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "011",   0,  "",      0,  0, "011"   },
                { L_,   "011",   1,  "",      0,  0, "011"   },
                { L_,   "011",   2,  "",      0,  0, "011"   },
                { L_,   "011",   3,  "",      0,  0, "011"   },

                { L_,   "01",    0,  "1",     0,  0, "01"    },
                { L_,   "01",    0,  "1",     0,  1, "101"   },
                { L_,   "01",    0,  "1",     1,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  1, "011"   },
                { L_,   "01",    1,  "1",     1,  0, "01"    },
                { L_,   "01",    2,  "1",     0,  0, "01"    },
                { L_,   "01",    2,  "1",     0,  1, "011"   },
                { L_,   "01",    2,  "1",     1,  0, "01"    },

                { L_,   "0",     0,  "11",    0,  0, "0"     },
                { L_,   "0",     0,  "11",    0,  1, "10"    },
                { L_,   "0",     0,  "11",    0,  2, "110"   },
                { L_,   "0",     0,  "11",    1,  0, "0"     },
                { L_,   "0",     0,  "11",    1,  1, "10"    },
                { L_,   "0",     0,  "11",    2,  0, "0"     },
                { L_,   "0",     1,  "11",    0,  0, "0"     },
                { L_,   "0",     1,  "11",    0,  1, "01"    },
                { L_,   "0",     1,  "11",    0,  2, "011"   },
                { L_,   "0",     1,  "11",    1,  0, "0"     },
                { L_,   "0",     1,  "11",    1,  1, "01"    },
                { L_,   "0",     1,  "11",    2,  0, "0"     },

                { L_,   "",      0,  "011",   0,  0, ""      },
                { L_,   "",      0,  "011",   0,  1, "0"     },
                { L_,   "",      0,  "011",   0,  2, "01"    },
                { L_,   "",      0,  "011",   0,  3, "011"   },
                { L_,   "",      0,  "011",   1,  0, ""      },
                { L_,   "",      0,  "011",   1,  1, "1"     },
                { L_,   "",      0,  "011",   1,  2, "11"    },
                { L_,   "",      0,  "011",   2,  0, ""      },
                { L_,   "",      0,  "011",   2,  1, "1"     },
                { L_,   "",      0,  "011",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "0110",  0,  "",      0,  0, "0110"  },
                { L_,   "0110",  1,  "",      0,  0, "0110"  },
                { L_,   "0110",  2,  "",      0,  0, "0110"  },
                { L_,   "0110",  3,  "",      0,  0, "0110"  },
                { L_,   "0110",  4,  "",      0,  0, "0110"  },

                { L_,   "011",   0,  "0",     0,  0, "011"   },
                { L_,   "011",   0,  "0",     0,  1, "0011"  },
                { L_,   "011",   0,  "0",     1,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  1, "0011"  },
                { L_,   "011",   1,  "0",     1,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  1, "0101"  },
                { L_,   "011",   2,  "0",     1,  0, "011"   },
                { L_,   "011",   3,  "0",     0,  0, "011"   },
                { L_,   "011",   3,  "0",     0,  1, "0110"  },
                { L_,   "011",   3,  "0",     1,  0, "011"   },

                { L_,   "01",    0,  "10",    0,  0, "01"    },
                { L_,   "01",    0,  "10",    0,  1, "101"   },
                { L_,   "01",    0,  "10",    0,  2, "1001"  },
                { L_,   "01",    0,  "10",    1,  0, "01"    },
                { L_,   "01",    0,  "10",    1,  1, "001"   },
                { L_,   "01",    0,  "10",    2,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  1, "011"   },
                { L_,   "01",    1,  "10",    0,  2, "0101"  },
                { L_,   "01",    1,  "10",    1,  0, "01"    },
                { L_,   "01",    1,  "10",    1,  1, "001"   },
                { L_,   "01",    1,  "10",    2,  0, "01"    },
                { L_,   "01",    2,  "10",    0,  0, "01"    },
                { L_,   "01",    2,  "10",    0,  1, "011"   },
                { L_,   "01",    2,  "10",    0,  2, "0110"  },
                { L_,   "01",    2,  "10",    1,  0, "01"    },
                { L_,   "01",    2,  "10",    1,  1, "010"   },
                { L_,   "01",    2,  "10",    2,  0, "01"    },

                { L_,   "0",     0,  "110",   0,  0, "0"     },
                { L_,   "0",     0,  "110",   0,  1, "10"    },
                { L_,   "0",     0,  "110",   0,  2, "110"   },
                { L_,   "0",     0,  "110",   0,  3, "1100"  },
                { L_,   "0",     0,  "110",   1,  0, "0"     },
                { L_,   "0",     0,  "110",   1,  1, "10"    },
                { L_,   "0",     0,  "110",   1,  2, "100"   },
                { L_,   "0",     0,  "110",   2,  0, "0"     },
                { L_,   "0",     0,  "110",   2,  1, "00"    },
                { L_,   "0",     0,  "110",   3,  0, "0"     },
                { L_,   "0",     1,  "110",   0,  0, "0"     },
                { L_,   "0",     1,  "110",   0,  1, "01"    },
                { L_,   "0",     1,  "110",   0,  2, "011"   },
                { L_,   "0",     1,  "110",   0,  3, "0110"  },
                { L_,   "0",     1,  "110",   1,  0, "0"     },
                { L_,   "0",     1,  "110",   1,  1, "01"    },
                { L_,   "0",     1,  "110",   1,  2, "010"   },
                { L_,   "0",     1,  "110",   2,  0, "0"     },
                { L_,   "0",     1,  "110",   2,  1, "00"    },
                { L_,   "0",     1,  "110",   3,  0, "0"     },

                { L_,   "",      0,  "0110",  0,  0, ""      },
                { L_,   "",      0,  "0110",  0,  1, "0"     },
                { L_,   "",      0,  "0110",  0,  2, "01"    },
                { L_,   "",      0,  "0110",  0,  3, "011"   },
                { L_,   "",      0,  "0110",  0,  4, "0110"  },
                { L_,   "",      0,  "0110",  1,  0, ""      },
                { L_,   "",      0,  "0110",  1,  1, "1"     },
                { L_,   "",      0,  "0110",  1,  2, "11"    },
                { L_,   "",      0,  "0110",  1,  3, "110"   },
                { L_,   "",      0,  "0110",  2,  0, ""      },
                { L_,   "",      0,  "0110",  2,  1, "1"     },
                { L_,   "",      0,  "0110",  2,  2, "10"    },
                { L_,   "",      0,  "0110",  3,  0, ""      },
                { L_,   "",      0,  "0110",  3,  1, "0"     },
                { L_,   "",      0,  "0110",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "01100", 0,  "",      0,  0, "01100" },
                { L_,   "01100", 1,  "",      0,  0, "01100" },
                { L_,   "01100", 2,  "",      0,  0, "01100" },
                { L_,   "01100", 3,  "",      0,  0, "01100" },
                { L_,   "01100", 4,  "",      0,  0, "01100" },
                { L_,   "01100", 5,  "",      0,  0, "01100" },

                { L_,   "0110",  0,  "0",     0,  0, "0110"  },
                { L_,   "0110",  0,  "0",     0,  1, "00110" },
                { L_,   "0110",  0,  "0",     1,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  1, "00110" },
                { L_,   "0110",  1,  "0",     1,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  1, "01010" },
                { L_,   "0110",  2,  "0",     1,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  1, "01100" },
                { L_,   "0110",  3,  "0",     1,  0, "0110"  },
                { L_,   "0110",  4,  "0",     0,  0, "0110"  },
                { L_,   "0110",  4,  "0",     0,  1, "01100" },
                { L_,   "0110",  4,  "0",     1,  0, "0110"  },

                { L_,   "011",   0,  "00",    0,  0, "011"   },
                { L_,   "011",   0,  "00",    0,  1, "0011"  },
                { L_,   "011",   0,  "00",    0,  2, "00011" },
                { L_,   "011",   0,  "00",    1,  0, "011"   },
                { L_,   "011",   0,  "00",    1,  1, "0011"  },
                { L_,   "011",   0,  "00",    2,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  1, "0011"  },
                { L_,   "011",   1,  "00",    0,  2, "00011" },
                { L_,   "011",   1,  "00",    1,  0, "011"   },
                { L_,   "011",   1,  "00",    1,  1, "0011"  },
                { L_,   "011",   1,  "00",    2,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  1, "0101"  },
                { L_,   "011",   2,  "00",    0,  2, "01001" },
                { L_,   "011",   2,  "00",    1,  0, "011"   },
                { L_,   "011",   2,  "00",    1,  1, "0101"  },
                { L_,   "011",   2,  "00",    2,  0, "011"   },
                { L_,   "011",   3,  "00",    0,  0, "011"   },
                { L_,   "011",   3,  "00",    0,  1, "0110"  },
                { L_,   "011",   3,  "00",    0,  2, "01100" },
                { L_,   "011",   3,  "00",    1,  0, "011"   },
                { L_,   "011",   3,  "00",    1,  1, "0110"  },
                { L_,   "011",   3,  "00",    2,  0, "011"   },

                { L_,   "01",    0,  "100",   0,  0, "01"    },
                { L_,   "01",    0,  "100",   0,  1, "101"   },
                { L_,   "01",    0,  "100",   0,  2, "1001"  },
                { L_,   "01",    0,  "100",   0,  3, "10001" },
                { L_,   "01",    0,  "100",   1,  0, "01"    },
                { L_,   "01",    0,  "100",   1,  1, "001"   },
                { L_,   "01",    0,  "100",   1,  2, "0001"  },
                { L_,   "01",    0,  "100",   2,  0, "01"    },
                { L_,   "01",    0,  "100",   2,  1, "001"   },
                { L_,   "01",    0,  "100",   3,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  1, "011"   },
                { L_,   "01",    1,  "100",   0,  2, "0101"  },
                { L_,   "01",    1,  "100",   0,  3, "01001" },
                { L_,   "01",    1,  "100",   1,  0, "01"    },
                { L_,   "01",    1,  "100",   1,  1, "001"   },
                { L_,   "01",    1,  "100",   1,  2, "0001"  },
                { L_,   "01",    1,  "100",   2,  0, "01"    },
                { L_,   "01",    1,  "100",   2,  1, "001"   },
                { L_,   "01",    1,  "100",   3,  0, "01"    },
                { L_,   "01",    2,  "100",   0,  0, "01"    },
                { L_,   "01",    2,  "100",   0,  1, "011"   },
                { L_,   "01",    2,  "100",   0,  2, "0110"  },
                { L_,   "01",    2,  "100",   0,  3, "01100" },
                { L_,   "01",    2,  "100",   1,  0, "01"    },
                { L_,   "01",    2,  "100",   1,  1, "010"   },
                { L_,   "01",    2,  "100",   1,  2, "0100"  },
                { L_,   "01",    2,  "100",   2,  0, "01"    },
                { L_,   "01",    2,  "100",   2,  1, "010"   },
                { L_,   "01",    2,  "100",   3,  0, "01"    },

                { L_,   "0",     0,  "1100",  0,  0, "0"     },
                { L_,   "0",     0,  "1100",  0,  1, "10"    },
                { L_,   "0",     0,  "1100",  0,  2, "110"   },
                { L_,   "0",     0,  "1100",  0,  3, "1100"  },
                { L_,   "0",     0,  "1100",  1,  0, "0"     },
                { L_,   "0",     0,  "1100",  1,  1, "10"    },
                { L_,   "0",     0,  "1100",  1,  2, "100"   },
                { L_,   "0",     0,  "1100",  2,  0, "0"     },
                { L_,   "0",     0,  "1100",  2,  1, "00"    },
                { L_,   "0",     0,  "1100",  3,  0, "0"     },
                { L_,   "0",     1,  "1100",  0,  0, "0"     },
                { L_,   "0",     1,  "1100",  0,  1, "01"    },
                { L_,   "0",     1,  "1100",  0,  2, "011"   },
                { L_,   "0",     1,  "1100",  0,  3, "0110"  },
                { L_,   "0",     1,  "1100",  1,  0, "0"     },
                { L_,   "0",     1,  "1100",  1,  1, "01"    },
                { L_,   "0",     1,  "1100",  1,  2, "010"   },
                { L_,   "0",     1,  "1100",  2,  0, "0"     },
                { L_,   "0",     1,  "1100",  2,  1, "00"    },
                { L_,   "0",     1,  "1100",  3,  0, "0"     },

                { L_,   "",      0,  "01100", 0,  0, ""      },
                { L_,   "",      0,  "01100", 0,  1, "0"     },
                { L_,   "",      0,  "01100", 0,  2, "01"    },
                { L_,   "",      0,  "01100", 0,  3, "011"   },
                { L_,   "",      0,  "01100", 0,  4, "0110"  },
                { L_,   "",      0,  "01100", 0,  5, "01100" },
                { L_,   "",      0,  "01100", 1,  0, ""      },
                { L_,   "",      0,  "01100", 1,  1, "1"     },
                { L_,   "",      0,  "01100", 1,  2, "11"    },
                { L_,   "",      0,  "01100", 1,  3, "110"   },
                { L_,   "",      0,  "01100", 1,  4, "1100"  },
                { L_,   "",      0,  "01100", 2,  0, ""      },
                { L_,   "",      0,  "01100", 2,  1, "1"     },
                { L_,   "",      0,  "01100", 2,  2, "10"    },
                { L_,   "",      0,  "01100", 2,  3, "100"   },
                { L_,   "",      0,  "01100", 3,  0, ""      },
                { L_,   "",      0,  "01100", 3,  1, "0"     },
                { L_,   "",      0,  "01100", 3,  2, "00"    },
                { L_,   "",      0,  "01100", 4,  0, ""      },
                { L_,   "",      0,  "01100", 4,  1, "0"     },
                { L_,   "",      0,  "01100", 5,  0, ""      },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = static_cast<int>(strlen(D_SPEC) +
                                                               strlen(S_SPEC));
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (veryVerbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(gDispatch(D_SPEC));      // control for destination
                Obj SS(gDispatch(S_SPEC));      // control for source
                Obj EE(gDispatch(E_SPEC));      // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insert(DI, s, SI, NE);        // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if (static_cast<int>(strlen(S_SPEC)) == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insert(DI, s);                // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insert(DI, s[SI]);            // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if (static_cast<int>(strlen(D_SPEC)) == DI) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(s, SI, NE);            // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if (static_cast<int>(strlen(D_SPEC)) == DI &&
                                      static_cast<int>(strlen(S_SPEC)) == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(s);                    // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if (static_cast<int>(strlen(D_SPEC)) == DI && 1 == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(s[SI]);                // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
                {
                    if (veryVerbose) { P_(DI) P(NE); }
                    // Note that specs are switched.

                    int count = 0;
                    Obj x(EE, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(EE == X);

                        ++count;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.remove(DI, NE);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP5_ASSERT(LINE, DD, X, DI, NE, DD == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERT(1 == count);
                }

                if (veryVerbose) cout << "\t\tremove(index)" << endl;
                if (1 == NE) {
                    // Note that specs are switched.

                    int count = 0;
                    Obj x(EE, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(EE == X);

                        ++count;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.remove(DI);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    LOOP4_ASSERT(LINE, DD, X, DI, DD == X);
                    ASSERT(1 == count);
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const Int64 NUM_BLOCKS = testAllocator.numBlocksTotal();
                const Int64 NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, SS, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if (static_cast<int>(strlen(S_SPEC)) == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, SS);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, SS[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if (static_cast<int>(strlen(D_SPEC)) == DI) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if (static_cast<int>(strlen(D_SPEC)) == DI &&
                                      static_cast<int>(strlen(S_SPEC)) == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if (static_cast<int>(strlen(D_SPEC)) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
                {
                    x.removeAll();  gg(&x, E_SPEC);  // Note: specs switched!
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.remove(DI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP5_ASSERT(LINE, DD, X, DI, NE, DD == X);
                }

                if (veryVerbose) cout << "\t\tremove(index)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, E_SPEC);  // Note: specs switched!
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.remove(DI);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP4_ASSERT(LINE, DD, X, DI, DD == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et al. (aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,   0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "0",     0,   0,  0,  "0"     },
                { L_,   "0",     0,   0,  1,  "00"    },
                { L_,   "0",     0,   1,  0,  "0"     },

                { L_,   "0",     1,   0,  0,  "0"     },
                { L_,   "0",     1,   0,  1,  "00"    },
                { L_,   "0",     1,   1,  0,  "0"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "01",    0,   0,  0,  "01"    },
                { L_,   "01",    0,   0,  1,  "001"   },
                { L_,   "01",    0,   0,  2,  "0101"  },
                { L_,   "01",    0,   1,  0,  "01"    },
                { L_,   "01",    0,   1,  1,  "101"   },
                { L_,   "01",    0,   2,  0,  "01"    },

                { L_,   "01",    1,   0,  0,  "01"    },
                { L_,   "01",    1,   0,  1,  "001"   },
                { L_,   "01",    1,   0,  2,  "0011"  },
                { L_,   "01",    1,   1,  0,  "01"    },
                { L_,   "01",    1,   1,  1,  "011"   },
                { L_,   "01",    1,   2,  0,  "01"    },

                { L_,   "01",    2,   0,  0,  "01"    },
                { L_,   "01",    2,   0,  1,  "010"   },
                { L_,   "01",    2,   0,  2,  "0101"  },
                { L_,   "01",    2,   1,  0,  "01"    },
                { L_,   "01",    2,   1,  1,  "011"   },
                { L_,   "01",    2,   2,  0,  "01"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "011",   0,   0,  0,  "011"     },
                { L_,   "011",   0,   0,  1,  "0011"    },
                { L_,   "011",   0,   0,  2,  "01011"   },
                { L_,   "011",   0,   0,  3,  "011011"  },
                { L_,   "011",   0,   1,  0,  "011"     },
                { L_,   "011",   0,   1,  1,  "1011"    },
                { L_,   "011",   0,   1,  2,  "11011"   },
                { L_,   "011",   0,   2,  0,  "011"     },
                { L_,   "011",   0,   2,  1,  "1011"    },
                { L_,   "011",   0,   3,  0,  "011"     },

                { L_,   "011",   1,   0,  0,  "011"     },
                { L_,   "011",   1,   0,  1,  "0011"    },
                { L_,   "011",   1,   0,  2,  "00111"   },
                { L_,   "011",   1,   0,  3,  "001111"  },
                { L_,   "011",   1,   1,  0,  "011"     },
                { L_,   "011",   1,   1,  1,  "0111"    },
                { L_,   "011",   1,   1,  2,  "01111"   },
                { L_,   "011",   1,   2,  0,  "011"     },
                { L_,   "011",   1,   2,  1,  "0111"    },
                { L_,   "011",   1,   3,  0,  "011"     },

                { L_,   "011",   2,   0,  0,  "011"     },
                { L_,   "011",   2,   0,  1,  "0101"    },
                { L_,   "011",   2,   0,  2,  "01011"   },
                { L_,   "011",   2,   0,  3,  "010111"  },
                { L_,   "011",   2,   1,  0,  "011"     },
                { L_,   "011",   2,   1,  1,  "0111"    },
                { L_,   "011",   2,   1,  2,  "01111"   },
                { L_,   "011",   2,   2,  0,  "011"     },
                { L_,   "011",   2,   2,  1,  "0111"    },
                { L_,   "011",   2,   3,  0,  "011"     },

                { L_,   "011",   3,   0,  0,  "011"     },
                { L_,   "011",   3,   0,  1,  "0110"    },
                { L_,   "011",   3,   0,  2,  "01101"   },
                { L_,   "011",   3,   0,  3,  "011011"  },
                { L_,   "011",   3,   1,  0,  "011"     },
                { L_,   "011",   3,   1,  1,  "0111"    },
                { L_,   "011",   3,   1,  2,  "01111"   },
                { L_,   "011",   3,   2,  0,  "011"     },
                { L_,   "011",   3,   2,  1,  "0111"    },
                { L_,   "011",   3,   3,  0,  "011"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "0110",  0,   0,  0,  "0110"      },
                { L_,   "0110",  0,   0,  1,  "00110"     },
                { L_,   "0110",  0,   0,  2,  "010110"    },
                { L_,   "0110",  0,   0,  3,  "0110110"   },
                { L_,   "0110",  0,   0,  4,  "01100110"  },
                { L_,   "0110",  0,   1,  0,  "0110"      },
                { L_,   "0110",  0,   1,  1,  "10110"     },
                { L_,   "0110",  0,   1,  2,  "110110"    },
                { L_,   "0110",  0,   1,  3,  "1100110"   },
                { L_,   "0110",  0,   2,  0,  "0110"      },
                { L_,   "0110",  0,   2,  1,  "10110"     },
                { L_,   "0110",  0,   2,  2,  "100110"    },
                { L_,   "0110",  0,   3,  0,  "0110"      },
                { L_,   "0110",  0,   3,  1,  "00110"     },
                { L_,   "0110",  0,   4,  0,  "0110"      },

                { L_,   "0110",  1,   0,  0,  "0110"      },
                { L_,   "0110",  1,   0,  1,  "00110"     },
                { L_,   "0110",  1,   0,  2,  "001110"    },
                { L_,   "0110",  1,   0,  3,  "0011110"   },
                { L_,   "0110",  1,   0,  4,  "00110110"  },
                { L_,   "0110",  1,   1,  0,  "0110"      },
                { L_,   "0110",  1,   1,  1,  "01110"     },
                { L_,   "0110",  1,   1,  2,  "011110"    },
                { L_,   "0110",  1,   1,  3,  "0110110"   },
                { L_,   "0110",  1,   2,  0,  "0110"      },
                { L_,   "0110",  1,   2,  1,  "01110"     },
                { L_,   "0110",  1,   2,  2,  "010110"    },
                { L_,   "0110",  1,   3,  0,  "0110"      },
                { L_,   "0110",  1,   3,  1,  "00110"     },
                { L_,   "0110",  1,   4,  0,  "0110"      },

                { L_,   "0110",  2,   0,  0,  "0110"      },
                { L_,   "0110",  2,   0,  1,  "01010"     },
                { L_,   "0110",  2,   0,  2,  "010110"    },
                { L_,   "0110",  2,   0,  3,  "0101110"   },
                { L_,   "0110",  2,   0,  4,  "01011010"  },
                { L_,   "0110",  2,   1,  0,  "0110"      },
                { L_,   "0110",  2,   1,  1,  "01110"     },
                { L_,   "0110",  2,   1,  2,  "011110"    },
                { L_,   "0110",  2,   1,  3,  "0111010"   },
                { L_,   "0110",  2,   2,  0,  "0110"      },
                { L_,   "0110",  2,   2,  1,  "01110"     },
                { L_,   "0110",  2,   2,  2,  "011010"    },
                { L_,   "0110",  2,   3,  0,  "0110"      },
                { L_,   "0110",  2,   3,  1,  "01010"     },
                { L_,   "0110",  2,   4,  0,  "0110"      },

                { L_,   "0110",  3,   0,  0,  "0110"      },
                { L_,   "0110",  3,   0,  1,  "01100"     },
                { L_,   "0110",  3,   0,  2,  "011010"    },
                { L_,   "0110",  3,   0,  3,  "0110110"   },
                { L_,   "0110",  3,   0,  4,  "01101100"  },
                { L_,   "0110",  3,   1,  0,  "0110"      },
                { L_,   "0110",  3,   1,  1,  "01110"     },
                { L_,   "0110",  3,   1,  2,  "011110"    },
                { L_,   "0110",  3,   1,  3,  "0111100"   },
                { L_,   "0110",  3,   2,  0,  "0110"      },
                { L_,   "0110",  3,   2,  1,  "01110"     },
                { L_,   "0110",  3,   2,  2,  "011100"    },
                { L_,   "0110",  3,   3,  0,  "0110"      },
                { L_,   "0110",  3,   3,  1,  "01100"     },
                { L_,   "0110",  3,   4,  0,  "0110"      },

                { L_,   "0110",  4,   0,  0,  "0110"      },
                { L_,   "0110",  4,   0,  1,  "01100"     },
                { L_,   "0110",  4,   0,  2,  "011001"    },
                { L_,   "0110",  4,   0,  3,  "0110011"   },
                { L_,   "0110",  4,   0,  4,  "01100110"  },
                { L_,   "0110",  4,   1,  0,  "0110"      },
                { L_,   "0110",  4,   1,  1,  "01101"     },
                { L_,   "0110",  4,   1,  2,  "011011"    },
                { L_,   "0110",  4,   1,  3,  "0110110"   },
                { L_,   "0110",  4,   2,  0,  "0110"      },
                { L_,   "0110",  4,   2,  1,  "01101"     },
                { L_,   "0110",  4,   2,  2,  "011010"    },
                { L_,   "0110",  4,   3,  0,  "0110"      },
                { L_,   "0110",  4,   3,  1,  "01100"     },
                { L_,   "0110",  4,   4,  0,  "0110"      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "01100", 0,   0,  0,  "01100"      },
                { L_,   "01100", 0,   0,  1,  "001100"     },
                { L_,   "01100", 0,   0,  2,  "0101100"    },
                { L_,   "01100", 0,   0,  3,  "01101100"   },
                { L_,   "01100", 0,   0,  4,  "011001100"  },
                { L_,   "01100", 0,   0,  5,  "0110001100" },
                { L_,   "01100", 0,   1,  0,  "01100"      },
                { L_,   "01100", 0,   1,  1,  "101100"     },
                { L_,   "01100", 0,   1,  2,  "1101100"    },
                { L_,   "01100", 0,   1,  3,  "11001100"   },
                { L_,   "01100", 0,   1,  4,  "110001100"  },
                { L_,   "01100", 0,   2,  0,  "01100"      },
                { L_,   "01100", 0,   2,  1,  "101100"     },
                { L_,   "01100", 0,   2,  2,  "1001100"    },
                { L_,   "01100", 0,   2,  3,  "10001100"   },
                { L_,   "01100", 0,   3,  0,  "01100"      },
                { L_,   "01100", 0,   3,  1,  "001100"     },
                { L_,   "01100", 0,   3,  2,  "0001100"    },
                { L_,   "01100", 0,   4,  0,  "01100"      },
                { L_,   "01100", 0,   4,  1,  "001100"     },
                { L_,   "01100", 0,   5,  0,  "01100"      },

                { L_,   "01100", 1,   0,  0,  "01100"      },
                { L_,   "01100", 1,   0,  1,  "001100"     },
                { L_,   "01100", 1,   0,  2,  "0011100"    },
                { L_,   "01100", 1,   0,  3,  "00111100"   },
                { L_,   "01100", 1,   0,  4,  "001101100"  },
                { L_,   "01100", 1,   0,  5,  "0011001100" },
                { L_,   "01100", 1,   1,  0,  "01100"      },
                { L_,   "01100", 1,   1,  1,  "011100"     },
                { L_,   "01100", 1,   1,  2,  "0111100"    },
                { L_,   "01100", 1,   1,  3,  "01101100"   },
                { L_,   "01100", 1,   1,  4,  "011001100"  },
                { L_,   "01100", 1,   2,  0,  "01100"      },
                { L_,   "01100", 1,   2,  1,  "011100"     },
                { L_,   "01100", 1,   2,  2,  "0101100"    },
                { L_,   "01100", 1,   2,  3,  "01001100"   },
                { L_,   "01100", 1,   3,  0,  "01100"      },
                { L_,   "01100", 1,   3,  1,  "001100"     },
                { L_,   "01100", 1,   3,  2,  "0001100"    },
                { L_,   "01100", 1,   4,  0,  "01100"      },
                { L_,   "01100", 1,   4,  1,  "001100"     },
                { L_,   "01100", 1,   5,  0,  "01100"      },

                { L_,   "01100", 2,   0,  0,  "01100"      },
                { L_,   "01100", 2,   0,  1,  "010100"     },
                { L_,   "01100", 2,   0,  2,  "0101100"    },
                { L_,   "01100", 2,   0,  3,  "01011100"   },
                { L_,   "01100", 2,   0,  4,  "010110100"  },
                { L_,   "01100", 2,   0,  5,  "0101100100" },
                { L_,   "01100", 2,   1,  0,  "01100"      },
                { L_,   "01100", 2,   1,  1,  "011100"     },
                { L_,   "01100", 2,   1,  2,  "0111100"    },
                { L_,   "01100", 2,   1,  3,  "01110100"   },
                { L_,   "01100", 2,   1,  4,  "011100100"  },
                { L_,   "01100", 2,   2,  0,  "01100"      },
                { L_,   "01100", 2,   2,  1,  "011100"     },
                { L_,   "01100", 2,   2,  2,  "0110100"    },
                { L_,   "01100", 2,   2,  3,  "01100100"   },
                { L_,   "01100", 2,   3,  0,  "01100"      },
                { L_,   "01100", 2,   3,  1,  "010100"     },
                { L_,   "01100", 2,   3,  2,  "0100100"    },
                { L_,   "01100", 2,   4,  0,  "01100"      },
                { L_,   "01100", 2,   4,  1,  "010100"     },
                { L_,   "01100", 2,   5,  0,  "01100"      },

                { L_,   "01100", 3,   0,  0,  "01100"      },
                { L_,   "01100", 3,   0,  1,  "011000"     },
                { L_,   "01100", 3,   0,  2,  "0110100"    },
                { L_,   "01100", 3,   0,  3,  "01101100"   },
                { L_,   "01100", 3,   0,  4,  "011011000"  },
                { L_,   "01100", 3,   0,  5,  "0110110000" },
                { L_,   "01100", 3,   1,  0,  "01100"      },
                { L_,   "01100", 3,   1,  1,  "011100"     },
                { L_,   "01100", 3,   1,  2,  "0111100"    },
                { L_,   "01100", 3,   1,  3,  "01111000"   },
                { L_,   "01100", 3,   1,  4,  "011110000"  },
                { L_,   "01100", 3,   2,  0,  "01100"      },
                { L_,   "01100", 3,   2,  1,  "011100"     },
                { L_,   "01100", 3,   2,  2,  "0111000"    },
                { L_,   "01100", 3,   2,  3,  "01110000"   },
                { L_,   "01100", 3,   3,  0,  "01100"      },
                { L_,   "01100", 3,   3,  1,  "011000"     },
                { L_,   "01100", 3,   3,  2,  "0110000"    },
                { L_,   "01100", 3,   4,  0,  "01100"      },
                { L_,   "01100", 3,   4,  1,  "011000"     },
                { L_,   "01100", 3,   5,  0,  "01100"      },

                { L_,   "01100", 4,   0,  0,  "01100"      },
                { L_,   "01100", 4,   0,  1,  "011000"     },
                { L_,   "01100", 4,   0,  2,  "0110010"    },
                { L_,   "01100", 4,   0,  3,  "01100110"   },
                { L_,   "01100", 4,   0,  4,  "011001100"  },
                { L_,   "01100", 4,   0,  5,  "0110011000" },
                { L_,   "01100", 4,   1,  0,  "01100"      },
                { L_,   "01100", 4,   1,  1,  "011010"     },
                { L_,   "01100", 4,   1,  2,  "0110110"    },
                { L_,   "01100", 4,   1,  3,  "01101100"   },
                { L_,   "01100", 4,   1,  4,  "011011000"  },
                { L_,   "01100", 4,   2,  0,  "01100"      },
                { L_,   "01100", 4,   2,  1,  "011010"     },
                { L_,   "01100", 4,   2,  2,  "0110100"    },
                { L_,   "01100", 4,   2,  3,  "01101000"   },
                { L_,   "01100", 4,   3,  0,  "01100"      },
                { L_,   "01100", 4,   3,  1,  "011000"     },
                { L_,   "01100", 4,   3,  2,  "0110000"    },
                { L_,   "01100", 4,   4,  0,  "01100"      },
                { L_,   "01100", 4,   4,  1,  "011000"     },
                { L_,   "01100", 4,   5,  0,  "01100"      },

                { L_,   "01100", 5,   0,  0,  "01100"      },
                { L_,   "01100", 5,   0,  1,  "011000"     },
                { L_,   "01100", 5,   0,  2,  "0110001"    },
                { L_,   "01100", 5,   0,  3,  "01100011"   },
                { L_,   "01100", 5,   0,  4,  "011000110"  },
                { L_,   "01100", 5,   0,  5,  "0110001100" },
                { L_,   "01100", 5,   1,  0,  "01100"      },
                { L_,   "01100", 5,   1,  1,  "011001"     },
                { L_,   "01100", 5,   1,  2,  "0110011"    },
                { L_,   "01100", 5,   1,  3,  "01100110"   },
                { L_,   "01100", 5,   1,  4,  "011001100"  },
                { L_,   "01100", 5,   2,  0,  "01100"      },
                { L_,   "01100", 5,   2,  1,  "011001"     },
                { L_,   "01100", 5,   2,  2,  "0110010"    },
                { L_,   "01100", 5,   2,  3,  "01100100"   },
                { L_,   "01100", 5,   3,  0,  "01100"      },
                { L_,   "01100", 5,   3,  1,  "011000"     },
                { L_,   "01100", 5,   3,  2,  "0110000"    },
                { L_,   "01100", 5,   4,  0,  "01100"      },
                { L_,   "01100", 5,   4,  1,  "011000"     },
                { L_,   "01100", 5,   5,  0,  "01100"      },

            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = static_cast<int>(strlen(X_SPEC));
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (veryVerbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(gDispatch(X_SPEC));      // control for destination
                Obj EE(gDispatch(E_SPEC));      // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Obj x(DD, &testAllocator);   const Obj &X = x;
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.insert(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insert(DI, X, SI, NE);
                        if (veryVerbose) {
                                       cout << "\t\t\t AFTER: "; P(X); P(EE); }
                        LOOP4_ASSERT(LINE, DI, SI, NE, EE == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insert(DI, X);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insert(DI, X[SI]);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == DI) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(X, SI, NE);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == DI &&
                                      static_cast<int>(strlen(X_SPEC)) == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(X);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == DI && 1 == NE) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == X);

                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(X[SI]);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const Int64 NUM_BLOCKS = testAllocator.numBlocksTotal();
                const Int64 NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == DI) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == DI &&
                                      static_cast<int>(strlen(X_SPEC)) == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if (static_cast<int>(strlen(X_SPEC)) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        if (verbose) cout << "Random generated arrays interacting\n";
        for (int ii = 0; ii < 1000; ++ii) {
            enum { MOD = 2 * CHAR_BIT * sizeof(uint64_t) + 1 };

            const int dstLen  = bsl::rand() % MOD;
            const int srcLen  = bsl::rand() % MOD;

            const bsl::string& dstSpec = randSpec(dstLen);
            const bsl::string& srcSpec = randSpec(srcLen);

            const Obj& DD = gDispatch(dstSpec.c_str());
            const Obj& SS = gDispatch(srcSpec.c_str());

            Obj mS(SS);  const Obj& S = mS;

            if (veryVeryVerbose) {
                P_(ii); P_(dstLen); P(srcLen);
                P(DD); P(SS);
            }

            if (veryVerbose) cout << "\tappend(bool);\n";
            {
                Obj mD(DD, &testAllocator); const Obj& D = mD;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    ASSERT(DD == D);

                    bool bit = srcLen & 1;
                    mD.append(bit);

                    const bsl::string& expSpec = dstSpec + (bit ? "1" : "0");
                    const Obj&         EXP     = gDispatch(expSpec.c_str());

                    ASSERT(EXP == D);
                    ASSERT(SS  == S);
                    mS.removeAll();
                    ASSERT(EXP == D);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                mS = SS;
            }

            for (int jj = 0; jj < 10; ++jj) {
                const int dstIdx  = bsl::rand() % (dstLen + 1);
                const int srcIdx  = bsl::rand() % (srcLen + 1);
                const int numBits = bsl::rand() % (srcLen - srcIdx + 1);

                if (veryVeryVerbose) {
                    P_(ii); P_(jj); P_(dstIdx); P_(srcIdx); P(numBits);
                }

                if (veryVerbose) cout << "\tinsert(" << dstIdx << ", SS, " <<
                                           srcIdx << ", " << numBits << ");\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        mD.insert(dstIdx, S, srcIdx, numBits);

                        const bsl::string& expSpec =
                                              dstSpec.substr(0, dstIdx) +
                                              srcSpec.substr(srcIdx, numBits) +
                                              dstSpec.substr(dstIdx);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                        ASSERT(SS  == S);
                        mS.removeAll();
                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    mS = SS;
                }

                if (veryVerbose) cout << "\tinsert(" << dstIdx << ", SS);\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        mD.insert(dstIdx, S);

                        const bsl::string& expSpec =
                                                    dstSpec.substr(0, dstIdx) +
                                                    srcSpec +
                                                    dstSpec.substr(dstIdx);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                        ASSERT(SS  == S);
                        mS.removeAll();
                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    mS = SS;
                }

                if (veryVerbose) cout << "\tinsert(" << dstIdx << ", bool);\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        bool bit = srcLen & 1;
                        mD.insert(dstIdx, bit);

                        const bsl::string& expSpec =
                                                    dstSpec.substr(0, dstIdx) +
                                                    (bit ? "1" : "0") +
                                                    dstSpec.substr(dstIdx);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                        ASSERT(SS  == S);
                        mS.removeAll();
                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    mS = SS;
                }

                if (veryVerbose) cout << "\tinsert(" << dstIdx << ", bool,"
                                                          << numBits << ");\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        bool bit = srcLen & 1;
                        mD.insert(dstIdx, bit, numBits);

                        const bsl::string  insertedBits(numBits,
                                                       (bit ? '1' : '0'));
                        const bsl::string& expSpec =
                                                    dstSpec.substr(0, dstIdx) +
                                                    insertedBits +
                                                    dstSpec.substr(dstIdx);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                        mS.removeAll();
                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    mS = SS;
                }

                if (veryVerbose) cout << "\tappend(S, " <<
                                           srcIdx << ", " << numBits << ");\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        mD.append(S, srcIdx, numBits);

                        const bsl::string& expSpec =
                                               dstSpec +
                                               srcSpec.substr(srcIdx, numBits);

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                        ASSERT(SS  == S);
                        mS.removeAll();
                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    mS = SS;
                }

                if (veryVerbose) cout << "\tappend(S);\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        mD.append(S);

                        const bsl::string& expSpec = dstSpec + srcSpec;

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                        ASSERT(SS  == S);
                        mS.removeAll();
                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    mS = SS;
                }

                if (veryVerbose) cout << "\tappend(bool," << numBits << ");\n";
                {
                    Obj mD(DD, &testAllocator); const Obj& D = mD;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ASSERT(DD == D);

                        bool bit = srcLen & 1;
                        mD.append(bit, numBits);

                        const bsl::string  insertedBits(numBits,
                                                            (bit ? '1' : '0'));
                        const bsl::string& expSpec = dstSpec + insertedBits;

                        const Obj& EXP = gDispatch(expSpec.c_str());

                        ASSERT(EXP == D);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(mX.remove(64));
            ASSERT_SAFE_FAIL(mX.remove(30, 35));

            ASSERT_SAFE_PASS(mX.remove(63));
            ASSERT_SAFE_PASS(mX.remove( 0));
            ASSERT_SAFE_PASS(mX.remove(32));
            ASSERT_SAFE_PASS(mX.remove(10,  0));
            ASSERT_SAFE_PASS(mX.remove(10, 10));
            ASSERT_SAFE_PASS(mX.remove(30, 21));    // up to the end
            ASSERT_SAFE_PASS(mX.remove( 0, 30));    // up to the end

            ASSERT(X.isEmpty());

            ASSERT_SAFE_PASS(mX.append(true,   0));
            ASSERT_SAFE_PASS(mX.append(true,   0));
            ASSERT_SAFE_PASS(mX.append(true, 100));

            ASSERT(100 == X.length());

            ASSERT_SAFE_PASS(mX.append(X));
            ASSERT_SAFE_PASS(mX.append(X,  0,  0));
            ASSERT_SAFE_PASS(mX.append(X,  0, 50));
            ASSERT_SAFE_PASS(mX.append(X, 50,  0));
            ASSERT_SAFE_PASS(mX.append(X, 50, 50));

            ASSERT(300 == X.length());

            ASSERT_PASS(mX.insert(  0, true));
            ASSERT_PASS(mX.insert(  0, true,   0));
            ASSERT_PASS(mX.insert(  0, true, 100));
            ASSERT_PASS(mX.insert(X.length(), true, 100));

            ASSERT_PASS(mX.insert(  0, X));
            ASSERT_PASS(mX.insert(X.length(), X));

            ASSERT_PASS(mX.insert(X.length(), X,   0,   0));
            ASSERT_PASS(mX.insert(X.length(), X,  10,  10));

            size_t len = X.length();
            ASSERT_PASS(mX.insert(len, X, len,  0));
            ASSERT_PASS(mX.insert(len, X, len - 10,  10));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING SET-LENGTH FUNCTION
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The resulting length is correct.
        //:
        //: 2 The resulting element values are correct when:
        //:   o new length <  initial length
        //:   o new length == initial length
        //:   o new length >  initial length
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set A of lengths.
        //:
        //: 2 For each a1 in A construct an object x of length a1 with each
        //:   element in x initialized to an arbitrary but known value V.
        //:
        //: 3 For each a2 in A use the 'setLength' method under test to set the
        //:   length of x and potentially remove or set element values as per
        //:   the method's contract.
        //:
        //: 4 Use the basic accessors to verify the length and element values
        //:   of the modified object x.  (C-1) (C-2)
        //:
        //: 5 Verify that 'setLength' is properly checking 'newLength'.  (C-3)
        //
        // Testing:
        //   void setLength(size_t newLength, bit iVal = false);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING SET-LENGTH FUNCTION\n"
                               "===========================\n";

        if (verbose) cout << "\nTesting 'setLength(int)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            enum { NUM_TESTS     = sizeof lengths / sizeof lengths[0] };

            const Element I_VALUE       = 1;
            const Element DEFAULT_VALUE = 0;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const size_t a1 = lengths[i];
                if (veryVerbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const size_t a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(a1, I_VALUE, &testAllocator);    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (size_t k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            ASSERTV(a1, a2, k, X[k], DEFAULT_VALUE == X[k]);
                        }
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, 0)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            enum { NUM_TESTS = sizeof lengths / sizeof lengths[0] };

            const Element I_VALUE   = 1;
            const Element F_VALUE   = 0;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const size_t a1 = lengths[i];
                if (veryVerbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const size_t a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(a1, I_VALUE, &testAllocator);    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2, F_VALUE);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (size_t k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, (F_VALUE != 0) == X[k]);
                        }
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, 1)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            enum { NUM_TESTS = sizeof lengths / sizeof lengths[0] };

            const Element I_VALUE   = 0;
            const Element F_VALUE   = 1;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const size_t a1 = lengths[i];
                if (veryVerbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const size_t a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(a1, I_VALUE, &testAllocator);    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2, F_VALUE);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (size_t k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, (F_VALUE!=0) == X[k]);
                        }
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, 2)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            enum { NUM_TESTS = sizeof lengths / sizeof lengths[0] };

            const Element I_VALUE   = 0;
            const Element F_VALUE   = 2;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const size_t a1 = lengths[i];
                if (veryVerbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const size_t a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(a1, I_VALUE, &testAllocator);    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2, F_VALUE);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (size_t k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, (F_VALUE!=0) == X[k]);
                        }
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, -1)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            enum { NUM_TESTS = sizeof lengths / sizeof lengths[0] };

            const Element I_VALUE   = 0;
            const Element F_VALUE   = -1;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const size_t a1 = lengths[i];
                if (veryVerbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const size_t a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(a1, I_VALUE, &testAllocator);    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2, F_VALUE);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (size_t k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, (F_VALUE!=0) == X[k]);
                        }
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }


        {
            Obj mX;

            bsls::AssertTestHandlerGuard guard;

            ASSERT_PASS(mX.setLength(  0));
            ASSERT_PASS(mX.setLength(100));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH CONSTRUCTORS
        //   Ensure the constructors have the desired effect.
        //
        // Concerns:
        //: 1 The initial value is correct.
        //:
        //: 2 The constructor is exception neutral w.r.t. memory allocation.
        //:
        //: 3 The internal memory management system is hooked up properly so
        //:   that *all* internally allocated memory draws from a user-supplied
        //:   allocator whenever one is specified.
        //
        // Plan:
        //: 1 For each constructor we will create objects
        //:   o With and without passing in an allocator.
        //:
        //:   o In the presence of exceptions during memory allocations using a
        //:     'bslma::TestAllocator' and varying its *allocation* *limit*.
        //:     (C-2)
        //:
        //:   o Where the object is constructed entirely in static memory
        //:     (using a 'bdlma::BufferedSequentialAllocator') and never
        //:     destroyed, so that any memory that doesn't come from the
        //:     specified allocator will be leaked and detected by purify.
        //:
        //: 2 Use basic accessors to verify (C-1)
        //:   o length
        //:   o element value at each index position { 0 .. length - 1 }.
        //:
        //: 3 Verify that the c'tors are checking their arguments.  (C-3)
        //
        // Testing:
        //   BitArray(size_t iLen, *ba = 0);
        //   BitArray(size_t iLen, bool iVal, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING INITIAL-LENGTH CONSTRUCTORS\n"
                               "===================================\n";

        if (verbose) cout << "\nTesting initial-length ctor with unspecified "
                             "(default) initial value." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const size_t  MAX_LENGTH    = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
                if (veryVerbose) P(length);
                Obj mX(length);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const size_t  MAX_LENGTH    = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
                if (veryVerbose) P(length);
                Obj mX(length, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const size_t  MAX_LENGTH    = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
              const Int64 numBytesInUse = testAllocator.numBytesInUse();
              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                ASSERT(testAllocator.numBytesInUse() == numBytesInUse);
                if (veryVerbose) P(length);
                Obj mX(length, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            const int                          memSize = 8192;
            char                               memory[memSize];
            bdlma::BufferedSequentialAllocator a(memory, memSize);
            const Element                      DEFAULT_VALUE = 0;  // ADJUST
            const size_t                       MAX_LENGTH    = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
                if (veryVerbose) P(length);
                size_t e           = length;
                Obj   *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(e, &a);
                Obj   &mX          = *doNotDelete;  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }

             // No destructor is called; will produce memory leak in purify if
             // internal allocators are not hooked up properly.
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting initial-length ctor with user-specified initial value."
                          << endl;
        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Element VALUE      = 1;  // ADJUST
            const size_t  MAX_LENGTH = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
                if (veryVerbose) P(length);
                Obj mX(length, VALUE);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Element VALUE      = 1;  // ADJUST
            const size_t  MAX_LENGTH = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
                if (veryVerbose) P(length);
                Obj mX(length, VALUE, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const Element VALUE      = 1;  // ADJUST
            const size_t  MAX_LENGTH = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
              const Int64 numBytesInUse = testAllocator.numBytesInUse();
              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                ASSERT(testAllocator.numBytesInUse() == numBytesInUse);
                if (veryVerbose) P(length);
                Obj mX(length, VALUE, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            const int memSize = 8192;
            char      memory[memSize];

            bdlma::BufferedSequentialAllocator a(memory, memSize);

            const Element VALUE      = 1;  // ADJUST
            const size_t  MAX_LENGTH = 100;
            for (size_t length = 0; length <= MAX_LENGTH; ++length) {
                if (veryVerbose) P(length);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(length, VALUE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (size_t i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_PASS(Obj(static_cast<size_t>(0)));
            ASSERT_SAFE_PASS(Obj(999));

            ASSERT_SAFE_PASS(Obj(  0, true));
            ASSERT_SAFE_PASS(Obj(999, false));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING FUNCTIONALITY
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' returns the correct
        //:   version to be used for the specified 'versionSelector'.
        //:
        //: 2 The 'bdexStreamOut' method is callable on a reference providing
        //:   only non-modifiable access.
        //:
        //: 3 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 4 For invalid streams, externalization leaves the stream invalid
        //:   and unexternalization does not alter the value of the object and
        //:   leaves the stream invalid.
        //:
        //: 5 Unexternalizing of incomplete, invalid, or corrupted data results
        //:   in a valid object of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //:
        //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference
        //:   to the provided stream in all situations.
        //:
        //: 9 The initial value of the object has no affect on
        //:   unexternalization.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 All calls to the 'bdexStreamOut' accessor will be done from a
        //:   'const' object or reference and all calls to the 'bdexStreamOut'
        //:   free function (provided by 'bslx') will be supplied a 'const'
        //:   object or reference.  (C-2)
        //:
        //: 3 Perform a direct test of the 'bdexStreamOut' and 'bdexStreamIn'
        //:   methods (the rest of the testing will use the free functions
        //:   'bslx::OutStreamFunctions::bdexStreamOut' and
        //:   'bslx::InStreamFunctions::bdexStreamIn').
        //:
        //: 4 Define a set 'S' of test values to be used throughout the test
        //:   case.
        //:
        //: 5 For all '(u, v)' in the cross product 'S X S', stream the value
        //:   of 'u' into (a temporary copy of) 'v', 'T', and assert 'T == u'.
        //:   (C-3, 9)
        //:
        //: 6 For all 'u' in 'S', create a copy of 'u' and attempt to stream
        //:   into it from an invalid stream.  Verify after each attempt that
        //:   the object is unchanged and that the stream is invalid.  (C-4)
        //:
        //: 7 Write 3 distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally, ensure that each object
        //:   streamed into is in some valid state.
        //:
        //: 8 Use the underlying stream package to simulate a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has become invalid.  (C-5)
        //:
        //: 9 Explicitly test the wire format.  (C-6)
        //:
        //:10 In all cases, confirm exception neutrality using the specially
        //:   instrumented 'bslx::TestInStream' and a pair of standard macros,
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //:   'bslx::TestInStream' object appropriately in a loop.  (C-7)
        //:
        //:11 In all cases, verify the return value of the tested method.
        //:   (C-8)
        //
        // Testing:
        //   int maxSupportedBdexVersion(int);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BDEX STREAMING FUNCTIONALITY\n"
                               "====================================\n";


        const int VERSION      = Obj::maxSupportedBdexVersion(0);
        const int VERSION_DATE = 20140601;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(gDispatch("011"), &testAllocator);
            if (veryVerbose) { cout << "\t   Value being streamed: "; P(X); }

            bslx::TestOutStream out(VERSION_DATE);
            bslx::OutStreamFunctions::bdexStreamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const size_t      LOD = out.length();

            bslx::TestInStream in(OD, LOD);
            ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(gDispatch("00"), &testAllocator);

            if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
            ASSERT(X != t);

            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) { cout << "\t  Value after overwrite: "; P(t); }
            ASSERT(X == t);

            // Corrupt the data, verify 'bdexStreamIn' detects it.

            bsl::vector<char> vc(OD, OD + LOD);
            vc.back() = static_cast<char>(0x7f);

            in.reset(vc.data(), out.length());

            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
        }

        if (verbose) cout <<
            "\nTesting stream operators ('<<' and '>>')." << endl;

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[]   = {
                0, 1, 3, 5, 32, 64, 100
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            size_t uOldLen = 0;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    cout << "\t\tFor source objects of length "
                         << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, !uLen || uOldLen < uLen);
                                                         // strictly increasing
                uOldLen = uLen;

                const Obj UU = gDispatch(U_SPEC);       // control
                LOOP_ASSERT(ui, uLen == UU.length());

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const size_t U_N = EXTEND[uj];

                    Obj        mU(&testAllocator);
                    stretchRemoveAll(&mU, U_N);
                    const Obj& U = gg(&mU, U_SPEC);

                    bslx::TestOutStream out(VERSION_DATE);

                    out.putVersion(VERSION);
                    bslx::OutStreamFunctions::bdexStreamOut(out, U, VERSION);
                    // testing stream-out operator here

                    const char *const OD  = out.data();
                    const size_t      LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bslx::TestInStream inStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, inStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !inStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const size_t      vLen   = strlen(V_SPEC);

                        const Obj VV = gDispatch(V_SPEC);       // control

                        if ((0 == uj && veryVerbose) || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                          BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(
                                                                inStream) {
                            inStream.reset();
                            const size_t V_N = EXTEND[vj];
                            const Int64  AL  = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, inStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !inStream.isEmpty());
            //--------------v
                            Obj        mV(&testAllocator);
                            stretchRemoveAll(&mV, V_N);
                            const Obj& V = gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose ||
                                               (veryVerbose && firstFew > 0)) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testAllocator.setAllocationLimit(AL);
                            int version;
                            inStream.getVersion(version);
                            ASSERT(VERSION == version);
                            bslx::InStreamFunctions::bdexStreamIn(inStream,
                                                                  mV,
                                                                  VERSION);
                            // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[]   = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            bslx::TestInStream inStream("", 0);
//          inStream.setSuppressVersionCheck(1);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = static_cast<int>(strlen(SPEC));

                if (veryVerbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == static_cast<int>(X.length()));

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(inStream) {
                    inStream.reset();

                    const int N = EXTEND[ei];

                    if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                    Obj t(&testAllocator);      gg(&t, SPEC);
                    stretchRemoveAll(&t, N);    gg(&t, SPEC);

                    // Ensure that reading from an empty or invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    LOOP2_ASSERT(ti, ei, inStream);
                    LOOP2_ASSERT(ti, ei, X == t);

                    bslx::InStreamFunctions::bdexStreamIn(inStream,
                                                      t,
                                                      VERSION);
                    LOOP2_ASSERT(ti, ei, !inStream);
                    LOOP2_ASSERT(ti, ei, X == t);

                    bslx::InStreamFunctions::bdexStreamIn(inStream,
                                                      t,
                                                      VERSION);
                    LOOP2_ASSERT(ti, ei, !inStream);
                    LOOP2_ASSERT(ti, ei, X == t);

                  } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj X1 = gDispatch("1010"),   Y1 = gDispatch("11"),
                                                       Z1 = gDispatch("01100");
            const Obj X2 = gDispatch("000"),    Y2 = gDispatch("10100"),
                                                       Z2 = gDispatch("1");
            const Obj X3 = gDispatch("00000"),  Y3 = gDispatch("1"),
                                                       Z3 = gDispatch("0100");

            bslx::TestOutStream out(VERSION_DATE);
            bslx::OutStreamFunctions::bdexStreamOut(out, Y1, VERSION);
            const size_t      LOD1 = out.length();
            bslx::OutStreamFunctions::bdexStreamOut(out, Y2, VERSION);
            const size_t      LOD2 = out.length();
            bslx::OutStreamFunctions::bdexStreamOut(out, Y3, VERSION);
            const size_t      LOD  = out.length();
            const char *const OD = out.data();

            for (unsigned i = 0; i < LOD; ++i) {
                bslx::TestInStream  inStream(OD, i);
                bslx::TestInStream& in = inStream;
//              in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { cout << "\t\t"; P(i); }

                Obj t1(X1, &testAllocator),
                    t2(X2, &testAllocator),
                    t3(X3, &testAllocator);

                if (i < LOD1) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(inStream) {
                    in.reset();
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    bslx::InStreamFunctions::bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (0 == i) LOOP_ASSERT(i, X1 == t1);
                    bslx::InStreamFunctions::bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    bslx::InStreamFunctions::bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                else if (i < LOD2) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(inStream) {
                    in.reset();
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    bslx::InStreamFunctions::bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP3_ASSERT(i, Y1, t1, Y1 == t1);
                    bslx::InStreamFunctions::bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    bslx::InStreamFunctions::bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                else {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(inStream) {
                    in.reset();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    bslx::InStreamFunctions::bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    bslx::InStreamFunctions::bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    bslx::InStreamFunctions::bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                  } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;                    // default value
        const Obj X = gDispatch("01100");       // original value
        const Obj Y = gDispatch("011");         // new value

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            int                 version = 1;
            const int           length  = 3;
            bsls::Types::Uint64 tmp     = 6;

            bslx::TestOutStream out(VERSION_DATE);
            out.putVersion(version);
            out.putLength(length);
            out.putArrayUint64(&tmp, 1);
            const char *const OD  = out.data();
            const size_t      LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

            bslx::TestInStream in(OD, LOD); ASSERT(in);
            in.getVersion(version);
            ASSERT(VERSION == version);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version); ASSERT(in);
            ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            char                version = 0; // too small
            const int           length  = 3;
            bsls::Types::Uint64 tmp     = 6;

            bslx::TestOutStream out(VERSION_DATE);
            out.putLength(length);
            out.putArrayUint64(&tmp, 1);
            const char *const OD  = out.data();
            const size_t      LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

            bslx::TestInStream in(OD, LOD);
//          in.setSuppressVersionCheck(1);
            ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        {
            const char          version = 2; // too large
            const int           length  = 3;
            bsls::Types::Uint64 tmp     = 6;

            bslx::TestOutStream out(VERSION_DATE);
            out.putLength(length);
            out.putArrayUint64(&tmp, 1);
            const char *const OD  = out.data();
            const size_t      LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

            bslx::TestInStream in(OD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tTruncated data." << endl;
        {
            const char          version = 1;
            const int           length  = 65;
            bsls::Types::Uint64 tmp[]   = { 6, 0 };

            bslx::TestOutStream out(VERSION_DATE);
            out.putLength(length);
            out.putArrayUint64(tmp + 0, 1);    // should be 2
            const char *const OD  = out.data();
            const size_t      LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

            bslx::TestInStream in(OD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W == t);    ASSERT(X != t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tData past length." << endl;
        {
            const char          version = 1;
            const int           length  = 3;
            bsls::Types::Uint64 tmp[]   = { 6, 0 };

            bslx::TestOutStream out(VERSION_DATE);
            out.putLength(length);
            out.putArrayUint64(tmp + 0, 2);    // should be 1
            const char *const OD  = out.data();
            const size_t      LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

            bslx::TestInStream in(OD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W == t);    ASSERT(X != t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion(0));
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec           ver  len  format
                //----  -------------  ---  ---  --------------------------
                { L_,   "",              0,   0, ""                         },
                { L_,   "0",             0,   0, ""                         },

                { L_,   "",              1,   1, "\x00"                     },

                { L_,   "0",             1,   9,
                                     "\x01\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,   "01",            1,   9,
                                     "\x02\x00\x00\x00\x00\x00\x00\x00\x02" },
                { L_,   "010",           1,   9,
                                     "\x03\x00\x00\x00\x00\x00\x00\x00\x02" },
                { L_,   "0101",          1,   9,
                                     "\x04\x00\x00\x00\x00\x00\x00\x00\x0a" },
                { L_,   "01010",         1,   9,
                                     "\x05\x00\x00\x00\x00\x00\x00\x00\x0a" },

                { L_,   "00001000111000010011000001001101",
                                         1,   9,
                                     "\x20\x00\x00\x00\x00\xb2\x0c\x87\x10" },
                { L_,   "000010001110000100110000010011011",
                                         1,   9,
                                     "\x21\x00\x00\x00\x01\xb2\x0c\x87\x10" },

                { L_,   "00001000111000010011000001001101"
                        "11000000001100001110000000000111",
                                         1,   9,
                                     "\x40\xe0\x07\x0c\x03\xb2\x0c\x87\x10" },
                { L_,   "00001000111000010011000001001101"
                        "110000000011000011100000000001111",
                                         1,  17,
                                     "\x41\xe0\x07\x0c\x03\xb2\x0c\x87\x10"
                                         "\x00\x00\x00\x00\x00\x00\x00\x01" },

                { L_,   "00001000111000010011000001001101"
                        "11000000001100001110000000000111"
                        "01001100011100001111000111001101",
                                         1,  17,
                                     "\x60\xe0\x07\x0c\x03\xb2\x0c\x87\x10"
                                         "\x00\x00\x00\x00\xb3\x8f\x0e\x32" },
                { L_,   "00001000111000010011000001001101"
                        "11000000001100001110000000000111"
                        "010011000111000011110001110011011",
                                         1,  17,
                                     "\x61\xe0\x07\x0c\x03\xb2\x0c\x87\x10"
                                         "\x00\x00\x00\x01\xb3\x8f\x0e\x32" },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC    = DATA[i].d_spec_p;
                const int         VERSION = DATA[i].d_version;
                const unsigned    LEN     = DATA[i].d_length;
                const char *const FMT     = DATA[i].d_fmt_p;

                Obj mX(&testAllocator);    const Obj& X = mX;
                mX = gDispatch(SPEC);
                bslx::ByteOutStream out(VERSION_DATE);
                X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (veryVerbose && LEN != out.length()) {
                    P_(LINE);
                    P(out.length());
                }
                if (veryVerbose && memcmp(out.data(), FMT, LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (unsigned j = 0; j < out.length(); ++j) {
                        unsigned char uc = *(out.data() + j);
                        cout << "\\x" << hex[uc >> 4] << hex[uc & 0xf];
                    }
                    cout << endl;
                }

                Obj mY(&testAllocator);  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bslx::ByteInStream in(out.data(),
                                          out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bslx::ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   Ensure the method has the desired effect.
        //
        // Concerns:
        //: 1 The value represented by any object can be assigned to any other
        //:   object regardless of how either value is represented internally.
        //:
        //: 2 The 'rhs' value must not be affected by the operation.
        //:
        //: 3 'rhs' going out of scope has no effect on the value of 'lhs'
        //:   after the assignment.
        //:
        //: 4 Aliasing (x = x): The assignment operator must always work --
        //:   even when the lhs and rhs are identically the same object.
        //:
        //: 5 The assignment operator must be neutral with respect to memory
        //:   allocation exceptions.
        //
        // Plan:
        //: 1 Specify a set S of unique object values with substantial and
        //:   varied differences, ordered by increasing length.  For each value
        //:   in S, construct an object x along with a sequence of similarly
        //:   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //:   aspect of white-box state by altering each xi in a unique way.
        //:   Let the union of all such objects be the set T.
        //:
        //: 2 Construct tests u = v for all (u, v) in T X T.  Using canonical
        //:   controls UU and VV, assert before the assignment that UU == u,
        //:   VV == v, and v == u if and only if VV == UU.  After the
        //:   assignment, assert that VV == u, VV == v, and, for grins, that
        //:   v == u.  Let v go out of scope and confirm that VV == u.  All of
        //:   these tests are performed within the 'bslma' exception testing
        //:   apparatus.  (C-1) (C-2) (C-5)
        //:
        //: 3 As a separate exercise, construct tests y = y for all y in T.
        //:   Using a canonical control X, verify that X == y before and after
        //:   the assignment, again within the 'bslma' exception testing
        //:   apparatus.  (C-4) (C-5)
        //:
        //: 4 After assignment, have the object on the rhs going out of scope
        //:   and verify this doesn't change the result.  (C-3)
        //
        // Testing:
        //   BitArray& operator=(const BitArray& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATOR\n"
                               "===========================\n";

        if (verbose) cout <<
             "\nAssign cross product of values with varied representations.\n";

        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[]   = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = static_cast<int>(strlen(U_SPEC));

                if (veryVerbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = gDispatch(U_SPEC);       // control
                LOOP_ASSERT(ui, uLen == static_cast<int>(UU.length()));

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int         vLen   =
                                              static_cast<int>(strlen(V_SPEC));

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = gDispatch(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mU(&testAllocator);
                            stretchRemoveAll(&mU, U_N);
                            const Obj& U = gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator);
                            stretchRemoveAll(&mV, V_N);
                            const Obj& V = gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose ||
                                               (veryVerbose && firstFew > 0)) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;

        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = static_cast<int>(strlen(SPEC));

                if (veryVerbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = gDispatch(SPEC);          // control
                LOOP_ASSERT(ti, curLen == static_cast<int>(X.length()));

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int  N = EXTEND[tj];
                    Obj        mY(&testAllocator);  stretchRemoveAll(&mY, N);
                    const Obj& Y = gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP MEMBER & FREE FUNCTIONS
        //   Ensure the methods have the desired effect.
        //
        // Concerns:
        //: 1 Swapping objects with different content should work: empty with
        //:   empty, empty with non-empty and non-empty with non-empty.
        //:
        //: 2 Swapping two objects should not involve memory allocation
        //:   (because it's no-throw).
        //:
        //: 3 Free function 'swap' and member function 'swap' must do the same
        //:   thing.
        //
        // Plan:
        //: 1 Define a set of specs that include specs for empty and non-empty
        //:   objects.  (C-1)
        //:
        //: 2 Iterate two nested loops through the set of specs, and create
        //:   create two objects, one from each of the specs selected.
        //:
        //: 3 Do a member swap and observe the results are as expected.  (C-1)
        //:
        //: 4 Do a free swap and observe the results are as expected.  (C-1)
        //:   (C-3)
        //:
        //: 5 Observe that no memory was allocated in either of the swaps in
        //:   steps 3 or 4.  (C-2)
        //
        // Testing:
        //   void swap(BitArray& lhs, BitArray& rhs);
        //   void swap(BitArray& other);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING SWAP MEMBER & FREE FUNCTIONS\n"
                               "====================================\n";

        if (verbose) {
            cout << "Testing 'swap' methods\n"
                 << "=====================\n";
        }

        if (verbose) cout << "Testing C-1, C-2, C-3\n";
        {
            const char *specs[] = { "", "1001", "xwwwwaw3", "xw4wwbwwc" };

            enum { specSize = sizeof(specs) / sizeof(*specs) };

            for (int i = 0; i < specSize; ++i) {
                for (int j = 0; j < specSize; ++j) {
                    for (int tf = 0; tf < 2; ++tf) {
                        Obj X1(gDispatch(specs[i])), X2(X1);
                        Obj Y1(gDispatch(specs[j])), Y2(Y1);

                        CInt64 totalBefore = testAllocator.numBlocksTotal();

                        if (tf) {
                            X1.swap(Y1);
                        }
                        else {
                            swap(X1, Y1);
                        }

                        ASSERT(testAllocator.numBlocksTotal() == totalBefore);

                        LOOP3_ASSERT(specs[i], i, j, X1 == Y2);
                        LOOP3_ASSERT(specs[j], i, j, X2 == Y1);
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Ensure the constructor has the desired effect.
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original object
        //:   (relying on the previously tested equality operators).
        //:
        //: 2 All internal representations of a given value can be used to
        //:   create a new object of equivalent value.
        //:
        //: 3 The value of the original object is left unaffected.
        //:
        //: 4 Subsequent changes in or destruction of the source object have no
        //:   effect on the copy-constructed object.
        //:
        //: 5 The function is exception neutral w.r.t. memory allocation.
        //:
        //: 6 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //
        // Plan:
        //: 1 Specify a set S of object values with
        //:   substantial and varied differences, ordered by increasing length.
        //:   For each value in S, initialize objects w and x, copy construct y
        //:   from x and use 'operator==' to verify that both x and y
        //:   subsequently have the same value as w.  Let x go out of scope and
        //:   again verify that w == x.  Repeat this test with x having the
        //:   same *logical* value, but perturbed so as to have potentially
        //:   different internal representations.  (C-1) (C-2) (C-3)
        //:
        //: 2 In one case, call 'removeAll' on the source object after the copy
        //:   construction and observe the copied object is unchanged.  (C-4)
        //:
        //: 3 Perform each of the above tests in the presence of exceptions
        //:   during memory allocations using a 'bslma::TestAllocator' and
        //:   varying its *allocation* *limit*.  (C-5)
        //:
        //: 4 For (C-6), repeat the above tests:
        //:   o When passing in no allocator.
        //:   o When passing in a null pointer: (bslma::Allocator *)0.
        //:   o When passing in a test allocator (see concern 5).
        //:   o Where the object is constructed entirely in static memory
        //:     (using a 'bdlma::BufferedSequentialAllocator') and never
        //:     destroyed.
        //:   o After the (dynamically allocated) source object is deleted and
        //:     its footprint erased (see concern 4).
        //
        // Testing:
        //   BitArray(const BitArray& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR\n"
                               "========================\n";

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            "xwwahb", "xwwawcw1", "xwwbh0ww2",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                if (veryVerbose) {
                    P(SPEC);
                }

                // Create control object w.
                Obj mW(&testAllocator);                const Obj& W = mW;
                ggDispatch(&mW, SPEC);
                if (veryVerbose) { cout << "\t"; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj &mX = *pX;
                    stretchRemoveAll(&mX, N);
                    const Obj& X = ggDispatch(&mX, SPEC);
                    if (veryVerbose) { cout << "\t\t"; P(X); }

                    {                                   // No allocator.
                        Obj mXX;    const Obj& XX = ggDispatch(&mXX, SPEC);
                        const Obj Y0(XX);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == XX);
                        mXX.removeAll();
                        LOOP2_ASSERT(SPEC, N, W == Y0);    // C-4
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bslma::Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    int count = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        ++count;
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(count > 1);
#endif

                    {                                   // Buffer Allocator.
                        const int memSize = 8 << 10;
                        char      memory[memSize];  // 8K

                        bdlma::BufferedSequentialAllocator a(memory, memSize);

                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);

                        // testAllocator will erase the footprint of pX
                        // preventing further reference to this object.

                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   Ensure the operators return the correct values.
        //
        // Concerns:
        //: 1 That '==' and '!=' correctly return a result based on the salient
        //:   attributes of 'lhs' and 'rhs'.
        //:
        //: 2 To test that no other internal state information is being
        //:   considered, we want also to verify that 'operator==' reports true
        //:   when applied to any two objects whose internal representations
        //:   may be different yet still represent the same (logical) value.
        //:
        //: 3 Note also that both equality operators must return either 1 or 0,
        //:   and neither 'lhs' nor 'rhs' value may be modified.
        //
        // Plan:
        //: 1 Specify a set S of unique object values, all different, some of
        //:   the differences subtle, ordered by non-decreasing length.
        //:
        //: 2 Iterate two nested loops, both iterating through the set 'S' of
        //:   specs, creating 'X' from one spec and 'V' from the other.  Verify
        //:   the correctness of 'operator==' and 'operator!=' (returning
        //:   either true or false) using all elements 'X' and 'V') of the
        //:   cross product S X S.  Verify that objects created from different
        //:   specs compare unequally, objects created from the same spec
        //:   compare equally.  (C-1)
        //:
        //: 3 Iterate four nested loops, 2 going through the values in S, and
        //:   the other going through different amounts to extend by.  Create
        //:   objects 'U' and 'V' from specs 'ti' and 'tj', extended by
        //:   amounts indexed by 'u' and 'v'.  Verify that objects created
        //:   from the same spec compare equally, regardless of their internal
        //:   representation.  (C-1) (C-2)
        //:
        //: 4 When comparing the result of '==' and '!=', compare them to the
        //:   results returned from '==' and '!=' applied to integers,
        //:   guaranteeing that they must equal 1 or 0.  (C-3)
        //
        // Testing:
        //   operator==(const BitArray&, const BitArray&);
        //   operator!=(const BitArray&, const BitArray&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY OPERATORS\n"
                               "==========================\n";

        static const char *SPECS[] = {
                "",
                "0",      "1",
                "00",     "01",     "11",     "10",
                "000",    "100",    "010",    "001",
                "0000",   "1000",   "0100",   "0010",   "0001",
                "00000",  "10000",  "01000",  "00100",  "00010",  "00001",
                "000000", "100000", "001000", "000100", "000001",
                "0000000",          "1000000",          "0000010",
                "00000000",         "01000000",         "00000100",
                "000000000",        "001000000",        "000001000",
                "0000000000",       "0001000000",       "0000010000",
                "0000000000000000000000000000000",
                "0000000000000000000000000000001",
                "00000000000000000000000000000000",
                "00000000000000000000000000000001",
                "00000100000000000000000000000000",
                "00000010000000000000000000000000",

           "000000000000000000000000000000000000000000000000000000000000000",
           "000000000000000000000000000000000000000000000000000000000000001",

           "0000100011100001001100000100110100001000111000010011000001001100",
           "0000100011100001001100000100110100000000111000010011000001001100",

           "0000000000000000000000000000000000000000000000000000000000000000",
           "0000000000000000000000000000000000000000000000000000000000000001",
           "0000010000000000000000000000000000000000000000000000000000000000",
           "0000001000000000000000000000000000000000000000000000000000000000",

           "0000100011100001001100000100110100001000111000010011000001001100"
           "0000100011100001001100000100110100001000111000010011000001001100",
           "0000100011100001001100000100110100001000111000010011000001001100"
           "0000100011100001001100000100110101001000111000010011000001001100",

            "xwwa", "xwwb", "xwawb", "xwahaqayaba", "xwahaqayaa8",
            "xhahbh8h7", "xhahbh8q7y767", "xhahbh8q767y7",

           "0000100011100001001100000100110100001000111000010011000001001100"
           "0000100011100001001100000100110101001000111000010011000001001100"
           "0000100011100001001100000100110101001000111000010011001001001100",
           "0000100011100001001100000100110100001000111000010011000001001100"
           "0000100011100001001100000100110101001000111000010011000001001100"
           "0000100011100001001100000100110101001000111000010011000001001100",
            0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            size_t oldLen = 0;
            bool matchedLen = false;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];

                Obj mU(&testAllocator);        const Obj& U = mU;
                ggDispatch(&mU, U_SPEC);
                size_t curLen = U.length();

                // Make sure there are at least 2 of each length.

                if (curLen == oldLen) {
                    matchedLen = true;
                }
                else {
                    ASSERTV(ti, oldLen, curLen, matchedLen);
                    oldLen = curLen;
                    matchedLen = false;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    Obj               mV(&testAllocator);    const Obj& V = mV;
                    ggDispatch(&mV, V_SPEC);

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const bool isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
            ASSERT(matchedLen);
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const int EXTEND[]   = {
                0, 57, 110, 194, 260, 350, 500
            };
            enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

            bool   matchedLen = true;
            size_t oldLen     = 0;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                Obj mX(&testAllocator);                const Obj& X = mX;
                ggDispatch(&mX, SPEC);
                const size_t curLen = X.length();

                if (curLen != oldLen) {
                    if (veryVerbose) cout <<"\tUsing objects having (logical) "
                                         "length " << curLen << '.' << endl;
                    ASSERTV(SPEC, oldLen, curLen, oldLen <= curLen);
                                                              // non-decreasing
                    oldLen = curLen;
                    ASSERTV(ti, matchedLen);
                    matchedLen = false;
                }
                else {
                    matchedLen = true;
                }

                LOOP_ASSERT(ti, curLen == X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X)}

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char * const SPECJ = SPECS[tj];

                    for (int u = 0; u < NUM_EXTEND; ++u) {
                        const int U_N = EXTEND[u];
                        Obj       mU(&testAllocator);        const Obj& U = mU;
                        stretchRemoveAll(&mU, U_N);
                        ggDispatch(&mU, SPECJ);

                        if (veryVerbose) { cout << "\t\t\t"; P_(U_N); P(U)}

                        // compare canonical representation with every
                        // variation

                        LOOP2_ASSERT(ti, tj, (ti == tj) == (U == X));
                        LOOP2_ASSERT(ti, tj, (ti == tj) == (X == U));
                        LOOP2_ASSERT(ti, tj, (ti != tj) == (U != X));
                        LOOP2_ASSERT(ti, tj, (ti != tj) == (X != U));

                        for (int v = 0; v < NUM_EXTEND; ++v) {
                            const int V_N = EXTEND[v];
                            Obj       mV(&testAllocator);    const Obj& V = mV;
                            stretchRemoveAll(&mV, V_N);
                            ggDispatch(&mV, SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose ||
                                               (veryVerbose && firstFew > 0)) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            // compare every variation with every other one

                            ASSERTV(SPEC, SPECJ, U_N, V_N,
                                                       (ti == tj) == (U == V));
                            ASSERTV(SPEC, SPECJ, U_N, V_N,
                                                       (ti == tj) == (V == U));
                            ASSERTV(SPEC, SPECJ, U_N, V_N,
                                                       (ti != tj) == (U != V));
                            ASSERTV(SPEC, SPECJ, U_N, V_N,
                                                       (ti != tj) == (V != U));
                        }
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        //   Ensure that the print and '<<' function produces the right output.
        //
        // Concerns:
        //: 1 Ensure that the method formats properly for empty and non-empty
        //:   values
        //:
        //: 2 Ensure that the method formats properly for negative, 0, and
        //:   positive levels.
        //:
        //: 3 Ensure that the method formats properly for 0 and non-zero spaces
        //:   per level.
        //
        // Plan:
        //: 1 Iterate through a table of specs, input values, and expected
        //:   output values:
        //:   o Create an object and initialize it to the spec using
        //:     'ggDispatch'.
        //:   o Call 'print' on the object with the 'level' and
        //:     'spacesPerLevel' from the table, capturing the output in a
        //:     string stream.
        //:   o Verify that the value of the string stream matches the expected
        //:     value from the table.  (C-1) (C-2) (C-3)
        //:   o Reset the string stream to empty.
        //:   o Call 'print' function using the string stream with 'level = 0'
        //:     and 'spacesPerLevel = -1'.
        //:   o Call '<<' using a second string stream on the same object.
        //:   o Verify the two string streams match.  (C-1) (C-2) (C-3)
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int spacesPerLevel);
        //   ostream& operator<<(ostream&, const BitArray&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRINT METHOD\n"
                               "====================\n";

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces/Tab  format        // ADJUST
                //----  ----    ----------  ----------  ------------------
                { L_,   "",        0,         0,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        0,        -1,        "[ ]"             },

                { L_,   "",        0,         2,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        1,         1,        " ["          NL
                                                        " ]"          NL  },

                { L_,   "",        1,         2,        "  ["         NL
                                                        "  ]"         NL  },

                { L_,   "",       -1,         2,        "["           NL
                                                        "  ]"         NL  },

                { L_,   "x0",      0,         0,        "["           NL
                                                        "0"           NL
                                                        "]"           NL  },

                { L_,   "x0",      0,        -1,        "[ 0 ]"           },

                { L_,   "x0",     -2,         1,        "["           NL
                                                        "   0"        NL
                                                        "  ]"         NL  },

                { L_,   "x7E",     1,         2,        "  ["         NL
                                                        "    7e"      NL
                                                        "  ]"         NL  },

                { L_,   "xa8",     0,        -1,        "[ a8 ]"          },

                { L_,   "x61",     2,         1,        "  ["         NL
                                                        "   61"       NL
                                                        "  ]"         NL  },

                { L_,   "x35862",  1,         3,        "   ["        NL
                                                        "      35862" NL
                                                        "   ]"        NL  },

                { L_,   "x10101",  0,        -1,        "[ 10101 ]"      },

                { L_,   "xabcdef0123456789 0123456789abcdef",
                                   1,         3,        "   ["        NL
                            "      abcdef0123456789 0123456789abcdef" NL
                                                        "   ]"        NL  },

                { L_,   "xabcdef0123456789 0123456789abcdef",
                                   0,        -1,
                            "[ abcdef0123456789 0123456789abcdef ]"       },

                { L_,                            "x0048732"
                        "abcdef0123456789 0123456789abcdef"
                        "2468013579abcdef fedcba0123456789",

                                   1,         3,
                                 "   ["        NL
                                 "                                       "
                                 "                           0048732" NL
                                 "      abcdef0123456789 0123456789abcdef"
                                        " 2468013579abcdef fedcba0123456789" NL
                                 "   ]"        NL  },

            };
#undef NL

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            size_t oldLen = 0;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const size_t      curLen = strlen(SPEC);
                const size_t      outLen = strlen(FMT);

                Obj mX(&testAllocator);  const Obj& X = ggDispatch(&mX, SPEC);

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton objects of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (veryVerbose) { cout << "\t\tSpec = \"" << SPEC << "\", ";
                                   P_(IND); P(SPL); }
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                bsl::ostringstream oss, ossB;
                X.print(oss, IND, SPL);
                if (veryVerbose) cout << "ACTUAL FORMAT:"   << endl <<
                                                             oss.str() << endl;

                ASSERTV(outLen, oss.str().length(),
                                                 outLen == oss.str().length());
                ASSERTV(oss.str(), FMT, oss.str() == FMT);

                oss.str("");
                X.print(oss, 0, -1);

                ossB << X;
                ASSERT(ossB.str() == oss.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Ensure the methods return the correct values.
        //
        // Concerns:
        //: 1 The accessors return results appropriate for the value of the
        //:   container.
        //:
        //: 2 Non-salient attributes do not affect the returned values.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set S of representative object values ordered by
        //:   non-decreasing length.
        //:
        //: 2 For each value w in S, initialize a newly constructed object x
        //:   with w using 'ggDispatch' and verify that 'length()',
        //:   'operator[]', and 'bits' return the expected result.  In the case
        //:   of 'operator[]' and 'bits', calculate the expected result by
        //:   operations on the binary spec of the object's value.
        //:
        //: 3 Reinitialize and repeat the same test on an existing object y
        //:   after perturbing y so as to achieve an internal state
        //:   representation of w that is potentially different from that of x.
        //:
        //: 4 Specify another table of specs of object values with the numbers
        //:   of set and clear bits specified in each spec.
        //:
        //: 5 Iterate through this second table:
        //:   o Initialize an object X according to the spec.
        //:
        //:   o Initialize a stretched object Y according to the spec.
        //:
        //:   o Verify the 'allocator()' method returns the allocator that was
        //:     supplied at construction.
        //:
        //:   o Verify that the sum of the number of 0's and 1's, from the
        //:     table, equal the returned 'length()'.
        //:
        //:   o Verify that 'isEmpty()' is true only when the object has a
        //:     length of 0.
        //:
        //:   o Verify for both X and Y that 'num1' and 'num0' match the
        //:     expected value.
        //:
        //:   o Verify that 'isAny1()' and 'isAny0()' return results consistent
        //:     with the numbers of 1's and 0's from the table.
        //:
        //: 6 At the end, verify that 'operator[]' and 'bits' are properly
        //:   checking their arguments.  (C-3)
        //
        // Testing:
        //   size_t length() const;
        //   bool operator[](size_t index) const;
        //   uint64_t bits(size_t index, size_t numBits) const;
        //   size_t num1() const;
        //   size_t num0() const;
        //   bool isAny1() const;
        //   bool isAny0() const;
        //   bslma::Allocator *allocator() const();
        //   bool isEmpty() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS\n"
                               "=======================\n";

        if (verbose) cout << "\nTesting 'length' & 'operator[]'" << endl;
        static const struct {
            int         d_lineNum;          // source line number
            const char *d_spec_p;           // specification string
            int         d_length;           // expected length
        } DATA_A[] = {
            { L_, "x", 0 },
            { L_, "x0", 4 },
            { L_, "xb", 4 },
            { L_, "x1234567890abcdef", 64 },
            { L_, "xw5", 64 },
            { L_, "xqbw5", 80 },
            { L_, "xhqya7h3", 92 },
            { L_, "x1234567890abcdef1234567890abcdef", 128 },
            { L_, "xhqya7h3hqya7h3", 184 },
            { L_, "xwwbhqya7h3", 220 },
            { L_, "xwwbhqya1234567890abcdef7h3", 284 },
        };
        enum { NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A };

        if (verbose) cout << "length, operator[], and bits\n";
        {
            Obj mY(&testAllocator);  // object with extended internal capacity

            const int EXTEND = 500; stretchRemoveAll(&mY, EXTEND);
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            size_t oldLen = 0;
            for (int ti = 0; ti < NUM_DATA_A ; ++ti) {
                const int         LINE   = DATA_A[ti].d_lineNum;
                const char *const SPEC   = DATA_A[ti].d_spec_p;
                const size_t      LENGTH = DATA_A[ti].d_length;

                Obj mX(&testAllocator);

                const Obj& X = ggDispatch(&mX, SPEC);
                const Obj& Y = ggDispatch(&mY, SPEC);

                const bsl::string& bs = binSpec(X);    // Binary Spec

                LOOP_ASSERT(ti, LENGTH == X.length()); // same lengths
                ASSERT(LENGTH == bs.length());

                if (LENGTH != oldLen) {
                    if (veryVerbose) cout << "\ton objects of length "
                                          << LENGTH << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < LENGTH);  // non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) {
                    cout << "\t\tSpec = \"" << SPEC << "\"\n"
                         << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                for (size_t ii = 0; ii < LENGTH; ++ii) {
                    ASSERT(('1' == bs.c_str()[ii]) == X[ii]);
                    ASSERT(('1' == bs.c_str()[ii]) == Y[ii]);
                }

                for (size_t idx = 0; idx < LENGTH; ++idx) {
                    size_t end = bsl::min(LENGTH - idx,
                                          sizeof(uint64_t) * CHAR_BIT);
                    for (int nb = 0; nb <= static_cast<int>(end); ++nb) {
                        // Set 'bitsSpec' to be the binary spec of what we
                        // expect from 'bits'.  Set 'x' to be the value.

                        const bsl::string& bitsSpec = bs.substr(idx, nb);
                        uint64_t           x = 0;
                        for (int shift = 0; shift < nb; ++shift) {
                            x |= static_cast<uint64_t>(
                                      '1' == bitsSpec.c_str()[shift]) << shift;
                        }

                        ASSERTV(LINE, idx, nb, X.bits(idx, nb) == x);
                    }
                }

                ASSERT(X == Y);
            }
        }

        if (verbose) cout << "\nTesting 'num?' and 'isAny?'" << endl;
        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_num0;          // expected number of 0 bits
                int         d_num1;          // expected number of 1 bits
            } DATA[] = {
                //line  spec            num0  num1
                //----  --------------  ----  ----
                { L_,   "",               0,    0  },

                { L_,   "0",              1,    0  },
                { L_,   "1",              0,    1  },

                { L_,   "00",             2,    0  },
                { L_,   "01",             1,    1  },
                { L_,   "11",             0,    2  },

                { L_,   "000",            3,    0  },
                { L_,   "110",            1,    2  },
                { L_,   "101",            1,    2  },
                { L_,   "111",            0,    3  },

                { L_,   "0000",           4,    0  },
                { L_,   "0011",           2,    2  },
                { L_,   "1001",           2,    2  },
                { L_,   "1111",           0,    4  },

                { L_,   "00000",          5,    0  },
                { L_,   "01100",          3,    2  },
                { L_,   "00110",          3,    2  },
                { L_,   "11111",          0,    5  },

                { L_,   "0000000",        7,    0  },
                { L_,   "0110001",        4,    3  },
                { L_,   "1111111",        0,    7  },

                { L_,   "00000000",       8,    0  },
                { L_,   "10100011",       4,    4  },
                { L_,   "11111111",       0,    8  },

                { L_,   "000000000",      9,    0  },
                { L_,   "110000110",      5,    4  },
                { L_,   "111111111",      0,    9  },

                { L_,   "xw0",           64,    0  },
                { L_,   "xwf",            0,   64  },
                { L_,   "xww0",         128,    0  },
                { L_,   "xwwf",           0,  128  },

                { L_,   "xhaq0h5qf",     48,   48  },
                { L_,   "xhaq0h5qfhaq0h5qf",
                                         96,   96  },

                { L_,   "xwwbw0",        96,   96  },

                { L_,   "xwwwwwwwwwwww5hf",
                                       384,   416  },

                { L_,
           "1100001100111001000101000110111011000011001110010001010001101110",
                                         34,   30  },
                { L_,
           "1111111111111111111111111111111111111111111111111111111111111111",
                                          0,   64  },

                { L_,
           "00000000000000000000000000000000000000000000000000000000000000000",
                                         65,    0  },
                { L_,
           "11000011001110010001010001101110110000110011100100010100011011101",
                                         34,   31  },
                { L_,
           "11111111111111111111111111111111111111111111111111111111111111111",
                                          0,   65  },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            Obj mY(&testAllocator);  // object with extended internal capacity

            const int EXTEND = 500; stretchRemoveAll(&mY, EXTEND);

            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE     = DATA[ti].d_lineNum;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const size_t      NUM0     = DATA[ti].d_num0;
                const size_t      NUM1     = DATA[ti].d_num1;
                const size_t      SPEC_LEN = strlen(SPEC);

                Obj mX(&testAllocator);

                const Obj& X = ggDispatch(&mX, SPEC); // canonical organization
                mY.removeAll();
                const Obj& Y = ggDispatch(&mY, SPEC); // has extended capacity

                if ('x' != *SPEC) {
                    ASSERT(SPEC_LEN == X.length()); // same lengths
                }

                ASSERT(&testAllocator == X.allocator());
                ASSERT(&testAllocator == Y.allocator());

                ASSERT(NUM0 + NUM1 == X.length());
                ASSERT(X.isEmpty() == (0 == X.length()));

                if (veryVerbose) {
                    cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                const size_t xNum0 = X.num0();
                const size_t yNum0 = Y.num0();
                ASSERT(xNum0 == yNum0);

                ASSERTV(LINE, NUM0, xNum0, NUM0 == xNum0);

                const size_t xNum1 = X.num1();
                const size_t yNum1 = Y.num1();
                ASSERT(xNum1 == yNum1);

                ASSERTV(LINE, NUM1, xNum1, NUM1 == xNum1);

                const bool xAny0 = X.isAny0();
                const bool yAny0 = Y.isAny0();
                ASSERT(xAny0 == yAny0);

                ASSERTV(LINE, NUM0, xAny0, (NUM0 > 0) == xAny0);

                const bool xAny1 = X.isAny1();
                const bool yAny1 = Y.isAny1();
                ASSERT(xAny1 == yAny1);

                ASSERTV(LINE, NUM1, xAny1, (NUM1 > 0) == xAny1);
            }
        }

        {
            Obj mX;    const Obj& X = ggDispatch(&mX, "xwa");

            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_PASS(X[  0]);
            ASSERT_SAFE_PASS(X[ 63]);
            ASSERT_SAFE_FAIL(X[ 64]);
            ASSERT_SAFE_FAIL(X[999]);

            ASSERT_SAFE_PASS(X.bits(  0,  0));
            ASSERT_SAFE_PASS(X.bits( 64,  0));
            ASSERT_SAFE_PASS(X.bits(  0, 64));
            ASSERT_SAFE_PASS(X.bits(  1, 63));
            ASSERT_SAFE_PASS(X.bits( 63,  1));
            ASSERT_SAFE_PASS(X.bits( 32, 32));
            ASSERT_SAFE_FAIL(X.bits(  0, 65));
            ASSERT_SAFE_FAIL(X.bits( 32, 33));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        //   Test the various helper functions.
        //
        // Concerns:
        //: 1 That 'ggg' and 'gggDispatch' function correctly on valid binary
        //:   specs.
        //:
        //: 2 That 'gggDispatch' correctly detect invalid specs.
        //:
        //: 3 That 'gg' and 'ggDispatch' function correctly on valid binary
        //:   and hex specs.
        //:
        //: 4 That 'gDispatch' returns an object with the same value as an
        //:   object initialized with 'gg' using the same spec.
        //:
        //: 5 'gDispatch' does not affect the test allocator.
        //:
        //: 6 'gDispatch' returns an object by value.
        //
        //: 7 That 'stretchRemoveAll' increases capacity appropriately.
        //:
        //: 8 That 'binSpec' correctly produces a binary spec correlating to
        //:   the state of the object passed to it.
        //:
        //: 9 That 'randSpec' correctly produces a random binary spec of the
        //:   length passed to it.
        //
        // Plan:
        //: 1 Iterate through a table of valid binary specs, which also has
        //:   expected values for the contents of the objects to be created.
        //:   Apply 'gg' and 'ggDispatch' and observe that the values created
        //:   are correct.  (C-1)
        //:
        //: 2 Iterate through a table of valid and invalid binary and hex
        //:   specs, parsing them with 'gggDispatch', and observe that the
        //:   value returned correctly reflects the correctness of the spec.
        //:   (C-2)
        //:
        //: 3 For each SPEC in a list of specifications, compare the object
        //:   returned (by value) from the generator function,
        //:   'gDispatch(SPEC)' with the value of a newly constructed OBJECT
        //:   configured using 'ggDispatch(&OBJECT, SPEC)'.  (C-4)
        //:
        //: 4 Verify that no memory was allocated from the test allocator
        //:   when 'gDispatch' was called.  (C-5)
        //:
        //: 5 Use 'sizeof' to verify the size of the return value of
        //:   'gDispatch' is the size of a bit array.  Take references to the
        //:   return value of multiple calls to 'ggDispatch' and 'gDispatch'
        //:   and verify that their addresses match and fail to match as would
        //:   be expected if 'ggDispatch' returns a reference to the object
        //:   passed to it and 'gDispatch' returns a newly created object by
        //:   value.  (C-6)
        //:
        //: 6 Iterate through a table of specs, with amounts to stretch and
        //:   expected increases in memory usage due to stretching, create
        //:   objects from the specs, stretch them, and observe the memory
        //:   usage is as expected.  (C-7)
        //:
        //: 7 Iterate through a table of pairs of specs, one simple binary spec
        //:   ('simple' meaning nothing but '0's and '1's) and one hex spec
        //:   that should give the same result.  Create objects with both
        //:   specs, using both 'ggg' and 'gggDispatch' in the case of the
        //:   binary spec, and using 'ggDispatch' in the case of the hex spec
        //:   and observe they match.  (C-3)
        //:
        //: 8 In the loop in plan part 4, call 'binSpec' on object created from
        //:   the specs and verify that it matches the simple binary spec from
        //:   the table.  (C-8)
        //:
        //: 9 Test 'randSpec' (C-9):
        //:   o Iterate through a range of lengths and call 'randSpec',
        //:     verifying that the string produced is a simple binary spec of
        //:     the length specified.
        //:
        //:   o Iterate many times, calling 'randSpec' with a fixed length,
        //:     and keep statistics on the nature of the strings produced to
        //:     to verify that they look random.
        //
        // Testing:
        //   BitArray& gg(BitArray* object, const char *spec);
        //   BitArray& ggDispatch(BitArray* object, const char *spec);
        //   int ggg(BitArray *object, const char *spec, int vF = 1);
        //   int gggDispatch(BitArray *object, const char *spec, int vF = 1);
        //   BitArray gDispatch(const char *spec);
        //   void stretchRemoveAll(Obj *object, size_t size);
        //   bsl::string binSpec(const BitArray& object);
        //   bsl::string randSpec(size_t length);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING HELPER FUNCTIONS\n"
                               "========================\n";

        if (verbose) cout << "\nTesting generator 'gg' on valid specs.\n";
        static const struct {
            int         d_lineNum;          // source line number
            const char *d_spec_p;           // specification string
            int         d_length;           // expected length
            Element     d_elements[10];     // expected element values
        } DATA_A[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",             0,      { 0 }                   },

            { L_,   "0",            1,      { 0 }                   },
            { L_,   "1",            1,      { 1 }                   },
            { L_,   "~",            0,      { 0 }                   },

            { L_,   "10",           2,      { 1, 0 }                },
            { L_,   "0~",           0,      { 0 }                   },
            { L_,   "~0",           1,      { 0 }                   },
            { L_,   "~~",           0,      { 0 }                   },

            { L_,   "011",          3,      { 0, 1, 1 }             },
            { L_,   "~11",          2,      { 1, 1 }                },
            { L_,   "0~1",          1,      { 1 }                   },
            { L_,   "01~",          0,      { 0 }                   },
            { L_,   "~~1",          1,      { 1 }                   },
            { L_,   "~1~",          0,      { 0 }                   },
            { L_,   "0~~",          0,      { 0 }                   },
            { L_,   "~~~",          0,      { 0 }                   },

            { L_,   "0110",         4,      { 0, 1, 1, 0 }          },
            { L_,   "~110",         3,      { 1, 1, 0 }             },
            { L_,   "0~10",         2,      { 1, 0 }                },
            { L_,   "01~0",         1,      { 0 }                   },
            { L_,   "011~",         0,      { 0 }                   },

            { L_,   "01100",        5,      { 0, 1, 1, 0, 0 }       },
            { L_,   "~1100",        4,      { 1, 1, 0, 0 }          },
            { L_,   "01~00",        2,      { 0, 0 }                },
            { L_,   "0110~",        0,      { 0 }                   },
            { L_,   "0~1~0",        1,      { 0 }                   },
            { L_,   "~1~0~",        0,      { 0 }                   },

            { L_,   "~110~~01100",  5,      { 0, 1, 1, 0, 0 }  },

            { L_,   "01100~1001~0", 1,      { 0 }                  },
        };
        enum { NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A };

        size_t oldLen = 0;
        for (int ti = 0; ti < NUM_DATA_A ; ++ti) {
            const int            LINE   = DATA_A[ti].d_lineNum;
            const char *const    SPEC   = DATA_A[ti].d_spec_p;
            const size_t         LENGTH = DATA_A[ti].d_length;
            const Element *const e      = DATA_A[ti].d_elements;
            const size_t         curLen = strlen(SPEC);

            Obj        mX(&testAllocator);
            const Obj& X = gg(&mX, SPEC);   // original spec
            Obj        mU(&testAllocator);
            const Obj& U = ggDispatch(&mU, SPEC);   // original spec

            static const char *const MORE_SPEC = "~011000110001100~";

            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj        mY(&testAllocator); const Obj& Y = gg(&mY, buf);
            Obj        mV(&testAllocator); const Obj& V = ggDispatch(&mV, buf);

            if (curLen != oldLen) {
                if (veryVerbose) cout << "\tof length "
                                  << curLen << ':' << endl;
                LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                cout << "\t\tBigSpec = \"" << buf  << '"' << endl;
                cout << "\t\t\t"; P(X);
                cout << "\t\t\t"; P(Y);
            }

            LOOP_ASSERT(LINE, LENGTH == X.length());
            LOOP_ASSERT(LINE, LENGTH == Y.length());
            for (size_t i = 0; i < LENGTH; ++i) {
                LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                LOOP2_ASSERT(LINE, i, e[i] == U[i]);
                LOOP2_ASSERT(LINE, i, e[i] == V[i]);
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA_B[] = {
            //line  spec            index
            //----  -------------   -----
            { L_,   "",             -1,     }, // control

            { L_,   "~",            -1,     }, // control
            { L_,   " ",             0,     },
            { L_,   ".",             0,     },
            { L_,   "2",             0,     },
            { L_,   "x.",            1,     },
            { L_,   "xz",            1,     },

            { L_,   "00",           -1,     }, // control
            { L_,   "30",            0,     },
            { L_,   "03",            1,     },
            { L_,   ".~",            0,     },
            { L_,   "~!",            1,     },
            { L_,   "  ",            0,     },
            { L_,   "x.~",           1,     },
            { L_,   "x~!",           1,     },

            { L_,   "011",          -1,     }, // control
            { L_,   " 11",           0,     },
            { L_,   "0 1",           1,     },
            { L_,   "01 ",           2,     },
            { L_,   "?#:",           0,     },
            { L_,   "   ",           0,     },
            { L_,   "x?#:",          1,     },

            { L_,   "01100",        -1,     }, // control
            { L_,   "41100",         0,     },
            { L_,   "01500",         2,     },
            { L_,   "01106",         4,     },
            { L_,   "04170",         1,     },
            { L_,   "xabcd-",        5,     },
            { L_,   "x41100",       -1,     },
            { L_,   "x01106",       -1,     },
            { L_,   "x0110q6",      -1,     },
            { L_,   "x  01500",     -1,     },
            { L_,   "x  04170",     -1,     },
            { L_,   "x  w1500",     -1,     },
            { L_,   "x  0w4170",    -1,     },
            { L_,   "xwhqy41100",   -1,     },

        };
        enum { NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B };

        oldLen = 0;
        for (int ti = 0; ti < NUM_DATA_B ; ++ti) {
            const int         LINE   = DATA_B[ti].d_lineNum;
            const char *const SPEC   = DATA_B[ti].d_spec_p;
            const int         INDEX  = DATA_B[ti].d_index;
            const size_t      curLen = strlen(SPEC);

            Obj mX(&testAllocator);

            if (curLen != oldLen) {
                if (veryVerbose) cout << "\tof length "
                                  << curLen << ':' << endl;
                LOOP_ASSERT(LINE, oldLen < curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) cout <<
                "\t\tSpec = \"" << SPEC << '"' << endl;

            int result = gggDispatch(&mX, SPEC);

            ASSERTV(LINE, INDEX, result, INDEX == result);
        }

        static const char *SPECS_G[] = {
            "", "~", "0", "1", "0~1~1~0~0", "01100", "011~00", "xwwhqa7",
            "xwwbw4h0y8",
        0}; // Null string required as last element.

        if (verbose) cout << "Compare values produced by 'gDispatch' and"
                                         " 'ggDispatch' for various inputs.\n";

        for (int ti = 0; SPECS_G[ti]; ++ti) {
            const char *spec = SPECS_G[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);  ggDispatch(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << gDispatch(spec) << endl;
                cout << "\tgg = " << X               << endl;
            }
            const Int64 TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const Int64 IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == gDispatch(spec));
            const Int64 TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const Int64 IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout <<
                          "Confirm address of return-by-value by gDispatch.\n";
        {
            const char *spec = "01100";

            ASSERT(sizeof(Obj) == sizeof gDispatch(spec));// compile-time fact

            Obj        x(&testAllocator);                    // runtime tests
            Obj&       r1 = gg(&x, spec);
            Obj&       r2 = gg(&x, spec);
            const Obj& r3 = gDispatch(spec);
            const Obj& r4 = gDispatch(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
        }

        if (verbose) cout <<
            "\nTesting 'stretchRemoveAll'." << endl;
        static const struct {
            int         d_lineNum;       // source line number
            const char *d_spec_p;        // specification string
            int         d_size;          // amount to grow (also length)
            int         d_firstResize;   // total blocks allocated
            int         d_secondResize;  // total blocks allocated

            // Note: total blocks (first/second Resize) and whether or not
            // 'removeAll' deallocates memory depends on 'Element' type.

        } DATA_C[] = {
            //line  spec            size    firstResize     secondResize
            //----  -------------   ----    -----------     ------------
            { L_,   "",             0,      0,              0       },

            { L_,   "",             1,      0,              0       },
            { L_,   "0",            0,      0,              0       },

            { L_,   "",             2,      0,              0       },
            { L_,   "0",            1,      0,              0       },
            { L_,   "01",           0,      0,              0       },

            { L_,   "",             3,      0,              0       },
            { L_,   "0",            2,      0,              0       },
            { L_,   "01",           1,      0,              0       },
            { L_,   "011",          0,      0,              0       },

            { L_,   "",             4,      0,              0       },
            { L_,   "0",            3,      0,              0       },
            { L_,   "01",           2,      0,              0       },
            { L_,   "011",          1,      0,              0       },
            { L_,   "0110",         0,      0,              0       },

            { L_,   "",             5,      0,              0       },
            { L_,   "0",            4,      0,              0       },
            { L_,   "01",           3,      0,              0       },
            { L_,   "011",          2,      0,              0       },
            { L_,   "0110",         1,      0,              0       },
            { L_,   "01100",        0,      0,              0       },

            { L_,   "010101010101010101",
                                    0,      0,              0       },
            { L_,   "010101010101010101",
                                   10,      0,              0       },
            { L_,   "010101010101010101",
                                   50,      0,              1       },

            { L_,   "0101010101010101010101010101010101010101",
                                   30,      0,              1       },
            { L_,   "0101010101010101010101010101010101010101",
                                   90,      0,              2       },
        };
        enum { NUM_DATA_C = sizeof DATA_C / sizeof *DATA_C };

        size_t oldDepth = 0;
        for (int ti = 0; ti < NUM_DATA_C ; ++ti) {
            const int         LINE         = DATA_C[ti].d_lineNum;
            const char *const SPEC         = DATA_C[ti].d_spec_p;
            const size_t      size         = DATA_C[ti].d_size;
            const int         firstResize  = DATA_C[ti].d_firstResize;
            const int         secondResize = DATA_C[ti].d_secondResize;
            const size_t      curLen       = strlen(SPEC);
            const size_t      curDepth     = curLen + size;

            Obj mX(&testAllocator);  const Obj& X = mX;
            Obj mY(&testAllocator);  const Obj& Y = mY;

            if (curDepth != oldDepth) {
                if (veryVerbose) cout << "\ton test vectors of depth "
                                  << curDepth << '.' << endl;
                LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                oldDepth = curDepth;
            }

            if (veryVerbose) {
                cout << "\t\t";  P_(SPEC); P(size);
                P_(firstResize); P_(secondResize);
                P_(curLen);      P(curDepth);
            }

            // Create identical objects using the gg function.
            {
                Int64 blocks1A = testAllocator.numBlocksTotal();
                Int64 bytes1A = testAllocator.numBytesInUse();

                gg(&mX, SPEC);

                Int64 blocks2A = testAllocator.numBlocksTotal();
                Int64 bytes2A = testAllocator.numBytesInUse();

                gg(&mY, SPEC);

                Int64 blocks3A = testAllocator.numBlocksTotal();
                Int64 bytes3A = testAllocator.numBytesInUse();

                Int64 blocks12A = blocks2A - blocks1A;
                Int64 bytes12A = bytes2A - bytes1A;

                Int64 blocks23A = blocks3A - blocks2A;
                Int64 bytes23A = bytes3A - bytes2A;

                if (veryVerbose) { P_(bytes12A);  P_(bytes23A);
                                   P_(blocks12A); P(blocks23A); }

                LOOP_ASSERT(LINE, curLen == X.length()); // same lengths
                LOOP_ASSERT(LINE, curLen == Y.length()); // same lengths

                LOOP_ASSERT(LINE, firstResize == blocks12A);

                LOOP_ASSERT(LINE, blocks12A == blocks23A);
                LOOP_ASSERT(LINE, bytes12A == bytes23A);

                ASSERT(binSpec(X) == SPEC);
            }

            // Apply both functions under test to the respective objects.
            {
                Int64 blocks1B  = testAllocator.numBlocksTotal();
                Int64 bytes1B   = testAllocator.numBytesInUse();

                for (size_t ii = 0; ii < size; ++ii) {
                    mX.append(0);
                }

                Int64 blocks2B  = testAllocator.numBlocksTotal();
                Int64 bytes2B   = testAllocator.numBytesInUse();

                const size_t len = Y.length();
                stretchRemoveAll(&mY, size);

                Int64 blocks3B  = testAllocator.numBlocksTotal();
                Int64 bytes3B   = testAllocator.numBytesInUse();

                for (size_t ii = 0; ii < len + size; ++ii) {
                    mY.append(0);
                }

                ASSERT(testAllocator.numBlocksTotal() == blocks3B);
                ASSERT(testAllocator.numBytesInUse()  == bytes3B);

                Int64 blocks12B = blocks2B - blocks1B;
                Int64 bytes12B  = bytes2B - bytes1B;

                Int64 blocks23B = blocks3B - blocks2B;
                Int64 bytes23B  = bytes3B - bytes2B;

                if (veryVerbose) { P_(bytes12B);  P_(bytes23B);
                                   P_(blocks12B); P(blocks23B); }

                LOOP_ASSERT(LINE, curDepth == X.length());
                LOOP_ASSERT(LINE, curDepth == Y.length());

                LOOP3_ASSERT(LINE, secondResize, blocks12B,
                                                    secondResize == blocks12B);

                LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                LOOP_ASSERT(LINE, bytes12B == bytes23B);   // True for POD;
            }                                              // else > or >=.
        }

        if (verbose) cout <<
            "\nTesting invalid hex parsed by 'gggDispatch'." << endl;
        static const struct {
            int         d_lineNum;
            const char *d_dispatchSpec;
            int         d_errorIdx;
        } DATA_D[] = {
            { L_, "x", -1 },

            { L_, "x.", 1 },
            { L_, "xz", 1 },
            { L_, "xy", 1 },
            { L_, "xq", 1 },
            { L_, "xh", 1 },
            { L_, "xw", 1 },
            { L_, "x .", 2 },
            { L_, "x z", 2 },

            { L_, "x00", -1 },
            { L_, "xh0", -1 },
            { L_, "xh0f", -1 },
            { L_, "xh0hq", 4 },
            { L_, "xyqhw0", -1 },
            { L_, "xyqhw0y", 6 },
            { L_, "xyqhw0f", -1 },
            { L_, "xyqhw0fh0", -1 },
        };
        enum { NUM_DATA_D = sizeof DATA_D / sizeof *DATA_D };

        for (int ti = 0; ti < NUM_DATA_D ; ++ti) {
            const int   LINE      = DATA_D[ti].d_lineNum;
            const char *SPEC      = DATA_D[ti].d_dispatchSpec;
            const int   ERROR_IDX = DATA_D[ti].d_errorIdx;

            Obj mX(&testAllocator); const Obj& X = mX;
            Obj mY(&testAllocator); const Obj& Y = mY;
            Obj mZ(&testAllocator); const Obj& Z = mZ;

            int rc = ggg(&mX, "010");    ASSERT(-1 == rc);
            rc     = ggg(&mY, "010");    ASSERT(-1 == rc);
            rc     = ggg(&mZ, "010");    ASSERT(-1 == rc);
            LOOP_ASSERT(LINE, X == Y);
            ASSERT(X == Z);
            ASSERT(Y == Z);

            rc = gggDispatch(&mX, SPEC);
            ASSERTV(LINE, ERROR_IDX == rc);

            if (-1 == rc) {
                // If 'gggDispatch' succeeded, we can only do multiples of
                // nibbles.

                ASSERT(0 == X.length() % 4);
            }
            else {
                // If 'gggDispatch' failed, 'X' should be unchanged.

                ASSERT(X == Z);
            }

            // Randomly insert white space throughout the string (but not
            // before the leading 'x' to show white space makes no difference.

            for (int repeat = 1; repeat <= 4; ++repeat) {
                mY = Z;

                bsl::string s(SPEC);
                for (size_t pos = s.length() - 1; pos >= 1; --pos) {
                    s.insert(pos, repeat, ' ');
                }

                const int expIdx = -1 == ERROR_IDX
                                  ? -1
                                  : ERROR_IDX * (repeat + 1);

                rc = gggDispatch(&mY, s.c_str());
                ASSERTV(s.c_str(), ERROR_IDX, expIdx, rc, expIdx == rc);
                ASSERT((-1 == ERROR_IDX) == (-1 == rc));

                ASSERT(X == Y);
            }
        }

        if (verbose) cout <<
            "\nComparing binary and hex specs, testing 'binSpec'." << endl;
        static const struct {
            int         d_lineNum;
            const char *d_binarySpec;
            const char *d_dispatchSpec;
        } DATA_E[] = {
            { L_, "", "x" },

            { L_, "0000", "x0" },
            { L_, "1111", "xf" },
            { L_, "1010", "x5" },
            { L_, "0101", "xa" },

            { L_, "01010101", "xya" },
            { L_, "10101111", "xf5" },
            { L_, "00000000", "xy0" },
            { L_, "00000000", "x00" },
            { L_, "11111111", "xyf" },
            { L_, "11111111", "xff" },

            { L_, "0000000000000000", "xq0" },
            { L_, "0000000000000000", "xy000" },
            { L_, "0000000000000000", "x0000" },
            { L_, "0000000000000000", "xyy0" },
            { L_, "1111111111111111", "xqf" },
            { L_, "1111111111111111", "xyfff" },
            { L_, "1111111111111111", "xffff" },
            { L_, "1111111111111111", "xyyf" },
            { L_, "1011101110111011", "xqd" },
            { L_, "1011101110111011", "xyddd" },
            { L_, "1011101110111011", "xdddd" },
            { L_, "1011101110111011", "xyyd" },
            { L_, "0000111100110011", "xycf 0" },
            { L_, "0000111100110011", "xccf 0" },

            { L_, "00000000000000000000000000000000", "xh0" },
            { L_, "00000000000000000000000000000000", "xqq0" },
            { L_, "00000000000000000000000000000000", "xyyyy0" },
            { L_, "00000000000000000000000000000000", "xqyy0" },
            { L_, "00000000000000000000000000000000", "xyqy0" },
            { L_, "00000000000000000000000000000000", "xy0 yq0" },
            { L_, "11111111111111111111111111111111", "xhf" },
            { L_, "11111111111111111111111111111111", "xqqf" },
            { L_, "11111111111111111111111111111111", "xyyyyf" },
            { L_, "11111111111111111111111111111111", "xqyyf" },
            { L_, "11111111111111111111111111111111", "xyqyf" },
            { L_, "11111111111111111111111111111111", "xyfyqf" },
            { L_, "11001100110011001100110011001100", "xh3" },
            { L_, "11001100110011001100110011001100", "xqq3" },
            { L_, "11001100110011001100110011001100", "xyyyy3" },
            { L_, "11001100110011001100110011001100", "xqyy3" },
            { L_, "11001100110011001100110011001100", "xyqy3" },
            { L_, "11001100110011001100110011001100", "xy3yq3" },

            { L_, "00000000000000000000000000000000"
                    "00000000000000000000000000000000", "xw0" },
            { L_, "00000000000000000000000000000000"
                    "00000000000000000000000000000000", "xhh0" },
            { L_, "00000000000000000000000000000000"
                    "00000000000000000000000000000000", "xqqqq0" },
            { L_, "00000000000000000000000000000000"
                    "00000000000000000000000000000000", "xyyyyyyyy0" },
            { L_, "00000000000000000000000000000000"
                    "00000000000000000000000000000000", "xh0q0y000" },
            { L_, "00000000000000000000000000000000"
                    "00000000000000000000000000000000", "xw0" },

            { L_, "10100000000000000000000000000000"
                    "00000000000000000000000000000101"
                  "01010101010111111111111111111110",   "x7qfqahqy05" },
            { L_, "10100000000000000000000000000000"
                    "00000000000000000000000000000101"
                  "01010101010111111111111111111110"
                    "00000000000000000000000000000000", "xh07qfqahqy05" },
            { L_, "01010101010101010101010101010101"
                    "01010101010101010101010101010101"
                  "10100000000000000000000000000000"
                    "00000000000000000000000000000101"
                  "01010101010111111111111111111110"
                    "00000000000000000000000000000000", "xh07qfqahqy05wa" },
        };
        enum { NUM_DATA_E = sizeof DATA_E / sizeof *DATA_E };

        for (int ti = 0; ti < NUM_DATA_E ; ++ti) {
            const int     LINE          = DATA_E[ti].d_lineNum;
            const char   *BINARY_SPEC   = DATA_E[ti].d_binarySpec;
            const char   *DISPATCH_SPEC = DATA_E[ti].d_dispatchSpec;
            const size_t  LENGTH        = strlen(BINARY_SPEC);

            ASSERTV(LINE, 0 == LENGTH % 4);   // Hex can only do whole nibbles.

            Obj mX(&testAllocator); const Obj& X = mX;
            Obj mY(&testAllocator); const Obj& Y = mY;
            Obj mZ(&testAllocator); const Obj& Z = mZ;

            int rc = gggDispatch(&mX, BINARY_SPEC);      ASSERT(-1 == rc);
            ASSERTV(LINE, LENGTH == X.length());

            for (size_t ii = 0; ii < LENGTH; ++ii) {
                ASSERT(X[ii] == (BINARY_SPEC[ii] == '1'));
            }

            {
                Obj mU;    const Obj& U = mU;

                ASSERT(-1 == ggg(&mU, BINARY_SPEC));
                ASSERTV(LINE, LENGTH == U.length());
                ASSERT(X == U);
            }

            // Randomly insert white space throughout the string (but not
            // before the leading 'x' to show white space makes no difference.

            for (bsl::size_t repeat = 0; repeat <= 4; ++repeat) {
                // Set 'mY' to a random state.  'gggDispatch' passed a valid
                // hex spec will 'removeAll' at the start.

                bsl::string s(DISPATCH_SPEC);
                for (bsl::size_t pos = s.length() - 1; pos >= 1; --pos) {
                    s.insert(pos, repeat, ' ');
                }

                rc = gggDispatch(&mY, s.c_str());                // hex spec
                LOOP_ASSERT(LINE, -1 == rc);

                ASSERTV(LINE, s.c_str(), X.length(), Y.length(), X == Y);
                ASSERT(X.length() == Y.length());
                ASSERT(LENGTH     == Y.length());

                // Convert the string to upper case to show that that makes no
                // difference.

                for (bsl::size_t pos = 0; pos < s.length(); ++pos) {
                    if (bsl::isalpha(static_cast<unsigned char>(s[pos]))) {
                        s[pos] = static_cast<char>(bsl::toupper(s[pos]));
                    }
                }

                rc = ggg(&mY, "~101");                ASSERT(-1 == rc);
                ASSERT(3 == Y.length());
                ASSERT(X != Y);

                // Observe that 'gggDispatch' empties the object before
                // initializing it, and use the same hex string we did last
                // time, only upper case.

                rc = gggDispatch(&mY, s.c_str());     ASSERT(-1 == rc);

                ASSERTV(X.length(), Y.length(), X == Y);

                rc = ggg(&mZ, "~101");                ASSERT(-1 == rc);
                ASSERT(3 == Z.length());
                ASSERT(X != Z);

                ASSERT(&Z == &ggDispatch(&mZ, s.c_str()));
                ASSERT(X == Z);

                // Observe that 'binSpec' matches the binary spec.

                ASSERT(BINARY_SPEC == binSpec(X));
            }
        }

        if (verbose) cout << "Check that 'randSpec' is providing reasonably"
                                                             " random specs\n";
        {
            for (unsigned len = 0; len < 320; ++len) {
                const bsl::string& rs = randSpec(len);
                ASSERT(rs.length() == len);
                ASSERT(bsl::string::npos == rs.find_first_not_of("01"));

                Obj mX;    const Obj& X = mX;
                ASSERT(-1 == ggg(&mX, rs.c_str()));    // 'rs' is valid
                ASSERT(static_cast<unsigned>(X.length()) == len);
            }

            enum { STRING_LEN = 64,
                   ITERATIONS = 1000 };

            int wasTrue[STRING_LEN] = { 0 };

            char prev = '0';
            for (int ti = 0; ti < ITERATIONS; ++ti) {
                const bsl::string& rs = randSpec(STRING_LEN);
                ASSERT(STRING_LEN == rs.length());
                ASSERT(bsl::string::npos == rs.find_first_not_of("01"));

                int count = 0, changes = 0, tj = 0;
                for (; tj < STRING_LEN; ++tj) {
                    if ('1' == rs[tj]) {
                        ++count;
                        ++wasTrue[tj];
                    }

                    changes += rs[tj] != prev;
                    prev = rs[tj];
                }
                ASSERT(0 == rs.c_str()[tj]);

                ASSERT(count > 10);
                ASSERT(count < 53);
                ASSERT(changes > 10);
                ASSERT(changes < 53);
            }

            // minTrue = INT_MAX, maxTrue = INT_MIN to start off with, then
            // accumulate min & max values, respectively, from there.

            int minTrue = static_cast<unsigned>(-1) >> 1, minTrueIdx = -1;
            int maxTrue = ~minTrue,                       maxTrueIdx = -1;
            for (int tj = 0; tj < STRING_LEN; ++tj) {
                if (wasTrue[tj] < minTrue) {
                    minTrue    = wasTrue[tj];
                    minTrueIdx = tj;
                }

                if (wasTrue[tj] > maxTrue) {
                    maxTrue    = wasTrue[tj];
                    maxTrueIdx = tj;
                }
            }

            ASSERT(minTrue > 0);
            ASSERT(maxTrue < ITERATIONS);

            if (verbose) {
                P_(minTrue); P_(minTrueIdx); P_(maxTrue); P(maxTrueIdx);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //: 1 The default Constructor
        //:   o creates the correct initial value.
        //:   o is exception neutral with respect to memory allocation.
        //:   o has the internal memory management system hooked up properly so
        //:     that *all* internally allocated memory draws from the same
        //:     user-supplied allocator whenever one is specified.
        //:
        //: 2 The destructor properly deallocates all allocated memory to
        //:   its corresponding allocator from any attainable state.
        //:
        //: 3 'append'
        //:   o produces the expected value.
        //:   o increases capacity as needed.
        //:   o maintains valid internal state.
        //:   o treats argument as boolean
        //:   o is exception neutral with respect to memory allocation.
        //:
        //: 4 'removeAll'
        //:   o produces the expected value (empty).
        //:   o properly destroys each contained element value.
        //:   o maintains valid internal state.
        //:   o does not allocate memory.
        //
        // Plan:
        //: 1 Create an object using the default constructor (C-1) (C-2):
        //:   o With and without passing in an allocator.
        //:   o In the presence of exceptions during memory allocations using a
        //:     'bslma::TestAllocator' and varying its *allocation* *limit*.
        //:   o Where the object is constructed entirely in static memory
        //:     (using a 'bdlma::BufferedSequentialAllocator') and never
        //:     destroyed.
        //:
        //: 2 (C-3) Construct a series of independent objects, ordered by
        //:   increasing length using the input values 0, 1, and 2.  In each
        //:   test, allow the object to leave scope without further
        //:   modification, so that the destructor asserts internal object
        //:   invariants appropriately.  After the final append operation in
        //:   each test, use the (untested) basic accessors to cross-check the
        //:   value of the object and the 'bslma::TestAllocator' to confirm
        //:   whether a resize has occurred.
        //:
        //: 3 (C-4) construct a similar test, replacing 'append' with
        //:   'removeAll'; this time, however, use the test allocator to record
        //:   *numBlocksInUse* rather than *numBlocksTotal*.
        //:
        //: 4 (Parts of C-2, C-3, and C-4) create a small "area" test that
        //:   exercises the construction and destruction of objects of various
        //:   lengths and capacities in the presence of memory allocation
        //:   exceptions.  Let S be the sequence of integers
        //:   '{ 0 .. N - 1 } mod 3'.  Two separate tests will be performed,
        //:   where The first test acts as a "control" in that 'removeAll' is
        //:   not called; if only the second test produces an error, we know
        //:   that 'removeAll' is to blame.  We will rely on
        //:   'bslma::TestAllocator' and purify to address concern 2, and on
        //:   the object invariant assertions in the destructor to address
        //:   concerns 3e and 4d.
        //:   o For each i in S, use the default constructor and 'append' to
        //:     create an object of length i, confirm its value (using basic
        //:     accessors), and let it leave scope.
        //:   o For each (i, j) in S X S, use 'append' to create an object of
        //:     length i, use 'removeAll' to erase its value and confirm (with
        //:     'length'), use append to set the object to a value of length j,
        //:     verify the value, and allow the object to leave scope.
        //
        // Testing:
        //   BitArray(bslma::Allocator *ba);
        //   ~BitArray();
        //   BOOTSTRAP: void append(bool value); // no aliasing
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS (BOOTSTRAP)\n"
                               "========================================\n";

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma::Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            int passCount = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                ++passCount;
                if (veryVerbose) cout <<
                        "\tTesting Exceptions In Default Ctor" << endl;
                const Obj X(&testAllocator);
                if (veryVerbose) { cout << "\t\t"; P(X); }
                ASSERT(0 == X.length());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#ifdef BDE_BUILD_TARGET_EXC
            ASSERT(2 == passCount);
#endif
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];

            bdlma::BufferedSequentialAllocator a(
                                              memory,
                                              static_cast<int>(sizeof memory));

            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'append' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(0);
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(1 == X.length());
            ASSERT(0 == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0);

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(1);
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(2 == X.length());
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1);

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(2);
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(3 == X.length());
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
            ASSERT(1 == X[2]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1); mX.append(2);

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(0);
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(4 == X.length());
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
            ASSERT(1 == X[2]);
            ASSERT(0 == X[3]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1); mX.append(2); mX.append(0);

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(1);
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(5 == X.length());
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
            ASSERT(1 == X[2]);
            ASSERT(0 == X[3]);
            ASSERT(1 == X[4]);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.length());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);   // always
            ASSERT(B - 0 == A); // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0);
            ASSERT(1 == X.length());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1);
            ASSERT(2 == X.length());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1); mX.append(2);
            ASSERT(3 == X.length());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1) create an object
            //    2) append { 0, 1, 2, 0, 1, 2, 0, ... }  up to length i
            //    3) verify initial length and contents
            //    4) allow the object to leave scope

            const size_t NUM_TRIALS = 100;
            for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (veryVerbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    size_t k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;           // 1.
                    for (k = 0; k < i; ++k) {                             // 2.
                        mX.append(k % 3);
                    }

                    LOOP_ASSERT(i, X.length() == i);                      // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP2_ASSERT(i, k, (k % 3 > 0) == X[k]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                  // 4.
            }
        }

        if (verbose) cout << "\tWith 'append' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1) create an object
            //    2) append 0 values up to a length of i
            //    3) verify initial length and contents
            //    4) removeAll contents from object
            //    5) verify length is 0
            //    6) append { 0, 1, 2, 0, 1, 2, 0, ... }  up to length j
            //    7) verify new length and contents
            //    8) allow the object to leave scope

            const size_t NUM_TRIALS = 100;
            for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (veryVerbose) cout <<
                     "\t\tOn an object of initial length " << i << '.' << endl;

                for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        size_t k; // loop index

                        Obj mX(&testAllocator);  const Obj& X = mX;       // 1.
                        for (k = 0; k < i; ++k) {                         // 2.
                            mX.append(0);
                        }

                        LOOP2_ASSERT(i, j, i == X.length());              // 3.
                        for (k = 0; k < i; ++k) {
                            LOOP3_ASSERT(i, j, k, 0 == X[k]);
                        }

                        mX.removeAll();                                   // 4.
                        LOOP2_ASSERT(i, j, 0 == X.length());              // 5.

                        for (k = 0; k < j; ++k) {                         // 6.
                            mX.append(k % 3);
                        }

                        LOOP2_ASSERT(i, j, j == X.length());              // 7.
                        for (k = 0; k < j; ++k) {
                            LOOP3_ASSERT(i, j, k, (k % 3 > 0) == X[k]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END              // 8.
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 This test exercises basic functionality, but tests nothing.
        //
        // Plan:
        //: 1 Create an object x1 (default ctor).
        //:
        //: 2 Create a second object x2 (copy from x1).
        //:
        //: 3 Append an element value 0 to x1).
        //:
        //: 4 Append the same element value 0 to x2).
        //:
        //: 5 Append another element value 1 to x2).
        //:
        //: 6 Remove all elements from x1.
        //:
        //: 7 Create a third object x3 (default ctor).
        //:
        //: 8 Create a fourth object x4 (copy of x2).
        //:
        //: 9 Assign x2 = x1 (non-empty becomes empty).
        //:
        //: 10 Assign x3 = x4 (empty becomes non-empty).
        //:
        //: 11 Assign x4 = x4 (aliasing).
        //:
        //: 12 Verify type traits.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1) Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2) Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3) Append an element value 0 to x1)."
                             "\t\t\t{ x1:0 x2: }" << endl;
        mX1.append(0);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(0 == X1[0]);

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4) Append the same element value 0 to x2)."
                             "\t\t{ x1:0 x2:0 }" << endl;
        mX2.append(0);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(0 == X2[0]);

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5) Append another element value 1 to x2)."
                             "\t\t{ x1:0 x2:01 }" << endl;
        mX2.append(1);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(0 == X2[0]);
        ASSERT(1 == X2[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 0);          ASSERT((X2 != X1) == 1);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6) Remove all elements from x1."
                             "\t\t\t{ x1: x2:01 }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7) Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:01 x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT((X3 == X1) == 1);          ASSERT((X3 != X1) == 0);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8) Create a fourth object x4 (copy of x2)."
                             "\t\t{ x1: x2:01 x3: x4:01 }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(0 == X4[0]);
        ASSERT(1 == X4[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 1);          ASSERT((X4 != X2) == 0);
        ASSERT((X4 == X3) == 0);          ASSERT((X4 != X3) == 1);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9) Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:01 }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);
        ASSERT((X2 == X3) == 1);          ASSERT((X2 != X3) == 0);
        ASSERT((X2 == X4) == 0);          ASSERT((X2 != X4) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10) Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(2 == X3.length());
        ASSERT(0 == X3[0]);
        ASSERT(1 == X3[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT((X3 == X1) == 0);          ASSERT((X3 != X1) == 1);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);
        ASSERT((X3 == X4) == 1);          ASSERT((X3 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11) Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(2 == X4.length());
        ASSERT(0 == X4[0]);
        ASSERT(1 == X4[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 0);          ASSERT((X4 != X2) == 1);
        ASSERT((X4 == X3) == 1);          ASSERT((X4 != X3) == 0);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                "\n 1) Create an object X5 (default ctor, default allocator)."
                                                       "\t\t\t{ X5: }" << endl;
        {
            bslma::TestAllocator         ga(veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&ga);
            Obj                          mX5;  const Obj& X5 = mX5;
            if (verbose) { cout << '\t';  P(X5); }

            if (verbose) cout << "\ta) Check initial state of X5." << endl;
            ASSERT(0 == X5.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X5 <op> X5." << endl;
            ASSERT((X5 == X5) == 1);          ASSERT((X5 != X5) == 0);

            mX5.append(0);
            if (verbose) { cout << '\t';  P(X5); }

            ASSERT(ga.numBytesInUse() > 0);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                "\n 1) Create an object X6 with ctor(int, bool, ...\n"
                                                       "\t\t\t{ X6: }" << endl;
        {
            Obj mX6(8, false, &testAllocator);  const Obj& X6 = mX6;
            if (verbose) { cout << '\t';  P(X6); }

            if (verbose) cout << "\ta) Check initial state of X6." << endl;
            ASSERT(8 == X6.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X6 <op> X6." << endl;
            ASSERT((X6 == X6) == 1);        ASSERT((X6 != X6) == 0);

            ASSERT(X6.length() == 8);
            ASSERT(false == X6[0]);         ASSERT(false == X6[4]);
            ASSERT(false == X6[5]);         ASSERT(false == X6[7]);
        }
        {
            bslma::TestAllocator         ga(veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&ga);
            // Obj mX6(8, 0);  const Obj& X6 = mX6; <-- note: causes error
            Obj mX6(8, false);  const Obj& X6 = mX6;
            if (verbose) { cout << '\t';  P(X6); }

            ASSERT(ga.numBytesInUse() > 0);

            if (verbose) cout << "\ta) Check initial state of X6." << endl;
            ASSERT(8 == X6.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X6 <op> X6." << endl;
            ASSERT((X6 == X6) == 1);        ASSERT((X6 != X6) == 0);

            ASSERT(X6.length() == 8);
            ASSERT(false == X6[0]);         ASSERT(false == X6[4]);
            ASSERT(false == X6[5]);         ASSERT(false == X6[7]);
        }

        if (verbose) cout << "Verify type traits.\n";
        {
            ASSERT(bslmf::IsBitwiseMoveable<Obj>::value);
            ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
            ASSERT(!bsl::is_trivially_copyable<Obj>::value);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == testAllocator.numBlocksInUse());

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
