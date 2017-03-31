// bslstl_string.t.cpp                                                -*-C++-*-
#include <bslstl_string.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_stringrefdata.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>
#include <bslmf_assert.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_issame.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_macrorepeat.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsltf_stdstatefulallocator.h>

#include <algorithm>    // 'adjacent_find', 'sort'
#include <iomanip>
#include <iostream>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include <limits.h>     // 'CHAR_MAX'
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if defined(std)
// This is a workaround for the way test drivers are built in an IDE-friendly
// manner in Visual Studio.  A "normal" test driver built from the command line
// will not have 'std' defined as a macro, and so will not need this
// workaround.  This workaround simply undoes the illusion that namespace 'std'
// is namespace 'bsl', so returning this test driver to its expected view.
#undef std
#endif

using namespace BloombergLP;
using bsls::NameOf;
using bsls::nameOfType;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The object under testing is a container whose interface and contract is
// dictated by the C++ standard.  In particular, the standard mandates "strong"
// exception safety (with full guarantee of rollback) along with throwing
// 'std::length_error' if about to request memory for more than 'max_size()'
// elements.  (Note: 'max_size' depends on the parameterized 'VALUE_TYPE'.)
// The general concerns are compliance, exception safety, and proper
// dispatching (for member function templates such as assign and insert).  In
// addition, it is a value-semantic type whose salient attributes are size and
// value of each element in sequence.  This container is implemented in the
// form of a class template, and thus its proper instantiation for several
// types is a concern.  Regarding the allocator template argument, we use
// mostly a 'bsl::allocator' together with a 'bslma::TestAllocator' mechanism,
// but we also verify the C++ standard.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' and 'clear' methods to be used by the generator functions 'g'
// and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that some
// manipulators must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bslma_testallocator'
// component.  After the mandatory sequence of cases (1--10) for value-semantic
// types (cases 5 and 10 are not implemented, as there is not output or
// streaming below bslstl), we test each individual constructor, manipulator,
// and accessor in subsequent cases.
//
// Regarding the allocation model, we attempt to write a general test driver
// that will work regardless of the allocation strategy chosen (short-string
// optimization, static null string, etc.)  However, some testing is necessary
// to make sure we don't overallocate capacity, and for this, knowledge of the
// internal allocation model of the implementation under test is necessary, of
// course.  This is done by the 'DEFAULT_CAPACITY' and
// 'INITIAL_CAPACITY_FOR_NON_EMPTY_OBJECT' constants, as well as the
// 'NUM_ALLOCS' array of constants, which would need to be adjusted if the
// allocation strategy were changed.  Since this version provides access to the
// new capacity computation (in the 'Imp' class method 'computeNewCapacity'),
// we reuse that but abstract it in the test facility 'computeNewCapacity'.
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//..
//     C               VALUE_TYPE (template argument, no default)
//     A               ALLOC (template argument, dflt: bsl::allocator<T>)
//     basic_string    basic_string<C,CT,A> if template arguments not specified
//     string<C,CT,A>  basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
//..
// In the signatures, to keep one-liners, the arguments 'pos', 'pos1', pos'2',
// 'n', 'n1', and 'n2', are always of 'size_type', and 'a' is always of type
// 'const A&'.
//-----------------------------------------------------------------------------
// class string<C,CT,A> (string)
// =============================
// [11] TRAITS
//
// CREATORS:
// [ 2] basic_string(const ALLOC& a = ALLOC());
// [ 7] basic_string(const basic_string& original);
// [ 7] basic_string(const basic_string& original, basicAllocator);
// [ 7] basic_string(MovableRef<basic_string> original);
// [ 7] basic_string(MovableRef<basic_string> original, basicAllocator);
// [12] basic_string(const basic_string& str, pos, n = npos, a = A());
// [12] basic_string(const CHAR_TYPE *s, a = A());
// [12] basic_string(const CHAR_TYPE *s, size_type n, a = A());
// [12] basic_string(size_type n, CHAR_TYPE c = CHAR_TYPE(), a = A());
// [12] template<class Iter> basic_string(Iter first, Iter last, a = A());
// [26] basic_string(const native_std::basic_string<CHAR, TRAITS, A2>&);
// [  ] basic_string(const StringRefData& strRefData, a = A());
// [33] basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
// [ 2] ~basic_string();
//
/// MANIPULATORS:
// [ 9] basic_string& operator=(const basic_string& rhs);
// [ 9] basic_string& operator=(const StringRefData& strRefData);
// [ 9] basic_string& operator=(MovableRef<basic_string> rhs);
// [  ] basic_string& operator=(const CHAR_TYPE *s);
// [  ] basic_string& operator=(CHAR_TYPE c);
// [33] basic_string& operator=(initializer_list<CHAR_TYPE> values);
// [  ] basic_string& operator+=(const basic_string& rhs);
// [  ] basic_string& operator+=(const CHAR_TYPE *s);
// [17] basic_string& operator+=(CHAR_TYPE c);
// [17] basic_string& operator+=(const StringRefData& strRefData);
// [16] iterator begin();
// [16] iterator end();
// [16] reverse_iterator rbegin();
// [16] reverse_iterator rend();
// [14] void resize(size_type n);
// [14] void resize(size_type n, CHAR_TYPE c);
// [14] void reserve(size_type n);
// [ 2] void clear();
// [15] reference operator[](size_type pos);
// [15] reference at(size_type pos);
// [15] reference front();
// [15] reference back();
// [13] basic_string& assign(const basic_string& str);
// [13] basic_string& assign(bslmf::MovableRef<basic_string> str);
// [13] basic_string& assign(const basic_string& str, pos, n);
// [13] basic_string& assign(const CHAR_TYPE *s, size_type n);
// [13] basic_string& assign(const CHAR_TYPE *s);
// [  ] basic_string& assign(const bslstl::StringRefData<CHAR_TYPE>& strRef);
// [13] basic_string& assign(size_type n, CHAR_TYPE c);
// [13] template <class Iter> basic_string& assign(Iter first, Iter last);
// [33] basic_string& assign(initializer_list<CHAR_TYPE> values);
// [17] basic_string& append(const basic_string& str);
// [17] basic_string& append(const basic_string& str, pos, n);
// [17] basic_string& append(const CHAR_TYPE *s, size_type n);
// [17] basic_string& append(const CHAR_TYPE *s);
// [17] basic_string& append(size_type n, CHAR_TYPE c);
// [17] template <class Iter> basic_string& append(Iter first, Iter last);
// [ 2] void push_back(CHAR_TYPE c);
// [  ] basic_string& insert(size_type pos1, const string& str);
// [  ] basic_string& insert(size_type pos1, const string& str, pos2, n);
// [  ] basic_string& insert(size_type pos, const CHAR_TYPE *s, n2);
// [  ] basic_string& insert(size_type pos, const CHAR_TYPE *s);
// [18] basic_string& insert(size_type pos, size_type n, CHAR_TYPE c);
// [18] iterator insert(const_iterator pos, CHAR_TYPE value);
// [18] iterator insert(const_iterator pos, size_type n, CHAR_TYPE value);
// [18] template <class Iter> iterator insert(const_iterator, Iter, Iter);
// [33] iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
// [19] void pop_back();
// [19] iterator erase(size_type pos = 0, size_type n = npos);
// [19] iterator erase(const_iterator position);
// [19] iterator erase(const_iterator first, const_iterator last);
// [20] basic_string& replace(pos1, n1, const string& str);
// [20] basic_string& replace(pos1, n1, const string& str, pos2, n2);
// [20] basic_string& replace(pos1, n1, const C *s, n2);
// [20] basic_string& replace(pos1, n1, const C *s);
// [20] basic_string& replace(pos1, n1, size_type n2, C c);
// [20] basic_string& replace(const_iterator p, q, const string& str);
// [20] basic_string& replace(const_iterator p, q, const C *s, n2);
// [20] basic_string& replace(const_iterator p, q, const C *s);
// [20] basic_string& replace(const_iterator p, q, size_type n2, C c);
// [20] template <It> basic_string& replace(const_iterator p, q, It f, l);
// [21] void swap(basic_string& other);
//
// ACCESSORS:
// [26] operator native_std::basic_string<CHAR, TRAITS, A2>() const;
// [ 4] const_reference operator[](size_type pos) const;
// [ 4] const_reference at(size_type pos) const;
// [15] const_reference front() const;
// [15] const_reference back() const;
// [  ] size_type length() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [14] size_type capacity() const;
// [14] bool empty() const;
// [16] const_iterator begin() const;
// [16] const_iterator end() const;
// [16] const_reverse_iterator rbegin() const;
// [16] const_reverse_iterator rend() const;
// [  ] const_iterator cbegin() const;
// [  ] const_iterator cend() const;
// [  ] const_reverse_iterator crbegin() const;
// [  ] const_reverse_iterator crend() const;
// [  ] const C *c_str() const;
// [  ] const C *data() const;
// [  ] allocator_type get_allocator() const;
// [22] size_type find(const string& str, pos = 0) const;
// [22] size_type find(const C *s, pos, n) const;
// [22] size_type find(const C *s, pos = 0) const;
// [22] size_type find(C c, pos = 0) const;
// [22] size_type rfind(const string& str, pos = 0) const;
// [22] size_type rfind(const C *s, pos, n) const;
// [22] size_type rfind(const C *s, pos = 0) const;
// [22] size_type rfind(C c, pos = 0) const;
// [22] size_type find_first_of(const string& str, pos = 0) const;
// [22] size_type find_first_of(const C *s, pos, n) const;
// [22] size_type find_first_of(const C *s, pos = 0) const;
// [22] size_type find_first_of(C c, pos = 0) const;
// [22] size_type find_last_of(const string& str, pos = 0) const;
// [22] size_type find_last_of(const C *s, pos, n) const;
// [22] size_type find_last_of(const C *s, pos = 0) const;
// [22] size_type find_last_of(C c, pos = 0) const;
// [22] size_type find_first_not_of(const string& str, pos = 0) const;
// [22] size_type find_first_not_of(const C *s, pos, n) const;
// [22] size_type find_first_not_of(const C *s, pos = 0) const;
// [22] size_type find_first_not_of(C c, pos = 0) const;
// [22] size_type find_last_not_of(const string& str, pos = 0) const;
// [22] size_type find_last_not_of(const C *s, pos, n) const;
// [22] size_type find_last_not_of(const C *s, pos = 0) const;
// [22] size_type find_last_not_of(C c, pos = 0) const;
// [23] string substr(pos, n) const;
// [23] size_type copy(char *s, n, pos = 0) const;
// [24] int compare(const string& str) const;
// [24] int compare(pos1, n1, const string& str) const;
// [24] int compare(pos1, n1, const string& str, pos2, n2) const;
// [24] int compare(const C *s) const;
// [24] int compare(pos1, n1, const C *s) const;
// [24] int compare(pos1, n1, const C *s, n2) const;
//
// FREE OPERATORS:
// [ 6] bool operator==(const string<C,CT,A>&, const string<C,CT,A>&);
// [ 6] bool operator==(const C *, const string<C,CT,A>&);
// [ 6] bool operator==(const string<C,CT,A>&, const C *);
// [ 6] bool operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
// [ 6] bool operator!=(const C *, const string<C,CT,A>&);
// [ 6] bool operator!=(const string<C,CT,A>&, const C *);
// [24] bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator<(const C *, const string<C,CT,A>&);
// [24] bool operator<(const string<C,CT,A>&, const C *);
// [24] bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator>(const C *, const string<C,CT,A>&);
// [24] bool operator>(const string<C,CT,A>&, const C *);
// [24] bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator<=(const C *, const string<C,CT,A>&);
// [24] bool operator<=(const string<C,CT,A>&, const C *);
// [24] bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator>=(const C *, const string<C,CT,A>&);
// [24] bool operator>=(const string<C,CT,A>&, const C *);
// [21] void swap(basic_string& lhs, basic_string& rhs);
// [30] int stoi(const string& str, std::size_t *pos = 0, int base = 10);
// [30] int stoi(const wstring& str, std::size_t *pos = 0, int base = 10);
// [30] long stol(const string& str, std::size_t *pos, int base);
// [30] long stol(const wstring& str, std::size_t *pos, int base);
// [30] unsigned long stoul(const string& s, std::size_t *pos, int base);
// [30] unsigned long stoul(const wstring& s, std::size_t *pos, int base);
// [30] long long stoll(const string& str, std::size_t *pos, int base);
// [30] long long stoll(const wstring& str, std::size_t *pos, int base);
// [30] unsigned long long stoull(const string&, std::size_t *, int);
// [30] unsigned long long stoull(const wstring&, std::size_t *, int);
// [31] float stof(const string& str, std::size_t *pos =0);
// [31] float stof(const wstring& str, std::size_t *pos =0);
// [31] double stod(const string& str, std::size_t *pos =0);
// [31] double stod(const wstring& str, std::size_t *pos =0);
// [31] long double stold(const string& str, std::size_t *pos =0);
// [31] long double stold(const wstring& str, std::size_t *pos =0);
// [32] string to_string(int value);
// [32] string to_string(long value);
// [32] string to_string(long long value);
// [32] string to_string(unsigned value);
// [32] string to_string(unsigned long value);
// [32] string to_string(unsigned long long value);
// [32] string to_string(float value);
// [32] string to_string(double value);
// [32] string to_string(long double value);
// [32] wstring to_wstring(int value);
// [32] wstring to_wstring(long value);
// [32] wstring to_wstring(long long value);
// [32] wstring to_wstring(unsigned value);
// [32] wstring to_wstring(unsigned long value);
// [32] wstring to_wstring(unsigned long long value);
// [32] wstring to_wstring(float value);
// [32] wstring to_wstring(double value);
// [32] wstring to_wstring(long double value);
// [  ] basic_ostream& operator<<(basic_ostream& stream, const string& str);
// [  ] basic_istream& operator>>(basic_istream& stream, string& str);
// [29] hashAppend(HASHALG& hashAlg, const basic_string& str);
// [29] hashAppend(HASHALG& hashAlg, const native_std::basic_string& str);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [34] USAGE EXAMPLE
// [11] CONCERN: The object has the necessary type traits
// [26] 'npos' VALUE
// [25] CONCERN: 'std::length_error' is used properly
// [27] DRQS 16870796
// [ 9] basic_string& operator=(const CHAR_TYPE *s); [NEGATIVE ONLY]
// [35] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int TestDriver:ggg(Obj *object, const char *spec, int vF = 1);
// [ 3] Obj& TestDriver:gg(Obj *object, const char *spec);
// [ 8] Obj TestDriver::g(const char *spec);
// [ 8] Obj TestDriver::g(size_t length, TYPE seed);

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

#define MACROREPEAT_COMMA BSLS_MACROREPEAT_COMMA

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;

// TEST OBJECT (unless o/w specified)
typedef char                                             Element;
typedef bsl::basic_string<char, bsl::char_traits<char> > Obj;
typedef bsl::String_Imp<char, size_t>                    Imp;

typedef bslma::TestAllocatorMonitor                      Tam;

// CONSTANTS
const int MAX_ALIGN      = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
const int MAX_ALIGN_MASK = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;

const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';
const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
const char VF = 'F';
const char VG = 'G';
const char VH = 'H';
const char VI = 'I';
const char VJ = 'J';
const char VK = 'K';
const char VL = 'L';
    // All test types have character value type.

const int  LARGE_SIZE_VALUE = 2 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
    // Declare a large value for insertions into the string.  Note this value
    // will cause multiple resizes during insertion into the string.

const size_t SHORT_STRING_BUFFER_BYTES
                           = (20 + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
    // The size of the short string buffer, according to our implementation (20
    // bytes rounded to the word boundary - 1).  Appending one more than this
    // number of characters to a default object causes a reallocation.

const size_t INITIAL_CAPACITY_FOR_NON_EMPTY_OBJECT = 1;
                                // bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    // The capacity of a default constructed object after the first
    // 'push_back', according to our implementation.

const int NUM_ALLOCS[] = {
    // Number of allocations (blocks) to create a string of the following size
    // by using 'push_back' repeatedly (without initial reserve):
#if BSLS_PLATFORM_CPU_64_BIT
    // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,

    // 17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4
#else
    // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,

    // 17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3
#endif
};

// Provide expected values for short string buffer capacity.  Note that we
// clearly call out the expected values on Windows and Unix (2/4-btye 'wchar_t'
// representations) as these are the overwhelmingly common cases, and for a
// test driver the clarity of seeing exact numbers is more important than the
// redundancy involved in the manual evaluation of the formula below.

#if defined(BSLS_PLATFORM_CPU_32_BIT)
const size_t k_SHORT_BUFFER_CAPACITY_CHAR    = 19;
const size_t k_SHORT_BUFFER_CAPACITY_WCHAR_T = 2 == sizeof(wchar_t) ? 9
                                             : 4 == sizeof(wchar_t) ? 4
                                             : 20 / sizeof(wchar_t) - 1;
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
const size_t k_SHORT_BUFFER_CAPACITY_CHAR    = 23;
const size_t k_SHORT_BUFFER_CAPACITY_WCHAR_T = 2 == sizeof(wchar_t) ? 11
                                             : 4 == sizeof(wchar_t) ? 5
                                             : 24 / sizeof(wchar_t) - 1;
#else
// Unknown platform configuration is likely to error when trying to use either
// of the two constants not defined in this block.
#endif

template <class CHAR_TYPE>
struct ExpectedShortBufferCapacity;

template <>
struct ExpectedShortBufferCapacity<char>
    : bsl::integral_constant<size_t, k_SHORT_BUFFER_CAPACITY_CHAR> {};

template <>
struct ExpectedShortBufferCapacity<wchar_t>
    : bsl::integral_constant<size_t, k_SHORT_BUFFER_CAPACITY_WCHAR_T> {};

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Support function overloads for printing debug info, discovered via ADL.
namespace bsl {

template <class TRAITS, class ALLOC>
void debugprint(const bsl::basic_string<char, TRAITS, ALLOC>& v)
    // Print the contents of the specified string 'v' to 'stdout', then flush.
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        for (size_t i = 0; i < v.size(); ++i) {
            printf("%c", v[i]);
        }
    }
    fflush(stdout);
}

template <class TRAITS, class ALLOC>
void debugprint(const bsl::basic_string<wchar_t, TRAITS, ALLOC>& v)
    // Print the contents of the specified string 'v' to 'stdout', then flush.
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        for (size_t i = 0; i < v.size(); ++i) {
            printf("%lc", wint_t(v[i]));
        }
    }
    fflush(stdout);
}
}  // close namespace bsl

// Legacy debug print support.
inline
void dbg_print(const char *s) { printf("\"%s\"", s); }
void dbg_print(const wchar_t *s)
{
    putchar('"');
    while (*s) {
        printf("%lc", wint_t(*s));
        ++s;
    }
    putchar('"');
}

// Generic debug print function (3-arguments).
template <class TYPE>
void dbg_print(const char *s, const TYPE& val, const char *nl)
{
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

// String utilities
inline
size_t max(size_t lhs, size_t rhs)
    // Return the larger of the specified 'lhs' and 'rhs'.
{
    return lhs < rhs ? rhs : lhs;
}

inline
size_t min(size_t lhs, size_t rhs)
    // Return the smaller of the specified 'lhs' and 'rhs'.
{
    return lhs < rhs ? lhs : rhs;
}

size_t computeNewCapacity(size_t newLength,
                          size_t /* initLength */,
                          size_t capacity,
                          size_t maxSize)
    // Compute the expected capacity of a string constructed with the specified
    // 'newLength' and 'capacity' as in by:
    //..
    //  Obj mX(newLength, ' ', allocator);
    //  mX.reserve(capacity);
    //..
    // and later modified to the 'newLength' by inserting characters (either by
    // 'insert' or 'push_back', it should not matter), assignment (using
    // 'assign), or replacement (using 'replace').  We assume that none of the
    // intermediate quantities can overflow the range of 'size_t', although we
    // accept the specified 'maxSize' as a ceiling for the new capacity.  Note
    // that 'newLength' is not necessarily larger than 'initLength'.
{
    // This implementation conforms to the one in the header, and is verified
    // to provide constant amortized time per character for all manipulators
    // (w.r.t. memory allocation).

    if (newLength <= capacity) {
        return capacity;                                              // RETURN
    }

    // adjust capacity the same way as it's done by the string implementation
    capacity += (capacity >> 1);
    if (newLength > capacity) {
        capacity = newLength;
    }

    return capacity > maxSize ? maxSize : capacity;                   // RETURN
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
namespace std {
void debugprint(const std::initializer_list<char>& v)
{
    if (0 == v.size()) {
        printf("<empty>");
    }
    else {
        for (std::initializer_list<char>::iterator itr  = v.begin(),
                                                   end  = v.end();
                                                   end != itr;
                                                   ++itr) {
            printf("'%c'", *itr);
        }
    }
    fflush(stdout);
}

void debugprint(const std::initializer_list<wchar_t>& v)
{
    if (0 == v.size()) {
        printf("<empty>");
    }
    else {
        for (std::initializer_list<wchar_t>::iterator itr  = v.begin(),
                                                      end  = v.end();
                                                      end != itr;
                                                      ++itr) {
            printf("'l%c'", *itr);
        }
    }
    fflush(stdout);
}
}  // close namespace std

#endif

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// STATIC DATA
static bool verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;

static bslma::TestAllocator *globalAllocator_p,
                            *defaultAllocator_p,
                            *objectAllocator_p;

static int numCopyCtorCalls    = 0;
static int numDestructorCalls  = 0;

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class VALUE_TYPE>
struct ExceptionGuard {
    // This scoped guard helps to verify the full guarantee of rollback in
    // exception-throwing code.

    // DATA
    int               d_lineNum;
    VALUE_TYPE        d_value;
    const VALUE_TYPE *d_object_p;

  public:
    // CREATORS
    ExceptionGuard(const VALUE_TYPE &object, int line)
    : d_lineNum(line)
    , d_value(object)
    , d_object_p(&object)
    {}

    ~ExceptionGuard() {
        if (d_object_p) {
            const int LINE = d_lineNum;
            LOOP3_ASSERT(LINE, d_value, *d_object_p, d_value == *d_object_p);
        }
    }

    // MANIPULATORS
    void release() {
        d_object_p = 0;
    }

    void resetValue(const VALUE_TYPE& value, int line) {
        d_lineNum = line;
        d_value = value;
    }
};

                               // ==============
                               // class CharList
                               // ==============

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE>,
          class ALLOC = bsl::allocator<TYPE> >
class CharList {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the parameterized
    // 'TYPE'.

    typedef bsl::basic_string<TYPE,TRAITS,ALLOC> Obj;

    // DATA
    Obj  d_value;

  public:
    // TYPES
    typedef bslstl::ForwardIterator<const TYPE, const TYPE*> const_iterator;
        // Input iterator.

    // CREATORS
    CharList() {}
    explicit CharList(const Obj& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE, class TRAITS, class ALLOC>
CharList<TYPE,TRAITS,ALLOC>::CharList(const Obj& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE, class TRAITS, class ALLOC>
const TYPE& CharList<TYPE,TRAITS,ALLOC>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharList<TYPE,TRAITS,ALLOC>::const_iterator
CharList<TYPE,TRAITS,ALLOC>::begin() const {
    return const_iterator(&*d_value.begin());
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharList<TYPE,TRAITS,ALLOC>::const_iterator
CharList<TYPE,TRAITS,ALLOC>::end() const {
    return const_iterator(&*d_value.end());
}

                              // ===============
                              // class CharArray
                              // ===============

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE>,
          class ALLOC = bsl::allocator<TYPE> >
class CharArray {
    // This array class is a simple wrapper on a string offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically a *random-access* iterator and its value type is the
    // parameterized 'TYPE'.

    typedef bsl::basic_string<TYPE,TRAITS,ALLOC> Obj;

    // DATA
    Obj  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    CharArray() {}
    explicit CharArray(const Obj& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE, class TRAITS, class ALLOC>
CharArray<TYPE,TRAITS,ALLOC>::CharArray(const Obj& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE, class TRAITS, class ALLOC>
const TYPE& CharArray<TYPE,TRAITS,ALLOC>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharArray<TYPE,TRAITS,ALLOC>::const_iterator
CharArray<TYPE,TRAITS,ALLOC>::begin() const {
    return const_iterator(&*d_value.begin());
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharArray<TYPE,TRAITS,ALLOC>::const_iterator
CharArray<TYPE,TRAITS,ALLOC>::end() const {
    return const_iterator(&*d_value.end());
}

                                 // ==============
                                 // class UserChar
                                 // ==============

template <int SIZE>
class UserChar {
    // This class is a simulation of a user-defined char type.  It has a
    // variable object size to test that the string works with chars larger
    // than 'char' and 'wchar_t'.
  private:
    // DATA
    union {
        size_t d_words[SIZE];
        char   d_char;
    };

  public:
    // CREATORS
    explicit
    UserChar(char c = 10);

    // ACCESSORS
    bool operator==(const UserChar& rhs) const;
    bool operator!=(const UserChar& rhs) const;
};

template <int SIZE>
inline
UserChar<SIZE>::UserChar(char c)
: d_char(c)
{}

template <int SIZE>
inline
bool UserChar<SIZE>::operator==(const UserChar& rhs) const {
    return d_char == rhs.d_char;
}

template <int SIZE>
inline
bool UserChar<SIZE>::operator!=(const UserChar& rhs) const {
    return !(*this == rhs);
}

                              // ====================
                              // class LimitAllocator
                              // ====================

template <class ALLOC>
class LimitAllocator : public ALLOC {

  public:
    // TYPES
    typedef typename ALLOC::value_type        value_type;
    typedef typename ALLOC::pointer           pointer;
    typedef typename ALLOC::const_pointer     const_pointer;
    typedef typename ALLOC::reference         reference;
    typedef typename ALLOC::const_reference   const_reference;
    typedef typename ALLOC::size_type         size_type;
    typedef typename ALLOC::difference_type   difference_type;

    template <class OTHER_TYPE> struct rebind {
        // It is better not to inherit the 'rebind' template, or else
        // 'rebind<X>::other' would be 'ALLOC::rebind<OTHER_TYPE>::other'
        // instead of 'LimitAlloc<X>'.

        typedef LimitAllocator<typename ALLOC::template
                                             rebind<OTHER_TYPE>::other > other;
    };

  private:
    // PRIVATE TYPES
    typedef ALLOC AllocBase;

    // DATA
    size_type d_limit;

  public:
    // CREATORS
    LimitAllocator()
    : d_limit(-1) {}

    explicit LimitAllocator(bslma::Allocator *mechanism)
    : AllocBase(mechanism), d_limit(-1) { }

    explicit LimitAllocator(const ALLOC& rhs)
    : AllocBase((const AllocBase&) rhs), d_limit(-1) { }

    ~LimitAllocator() { }

    // MANIPULATORS
    void setMaxSize(size_type maxSize) { d_limit = maxSize; }

    // ACCESSORS
    size_type max_size() const { return d_limit; }
};

template <class TYPE, class TRAITS, class ALLOC>
inline
bool isNativeString(const bsl::basic_string<TYPE,TRAITS,ALLOC>&)
    { return false; }

template <class TYPE, class TRAITS, class ALLOC>
inline
bool isNativeString(const native_std::basic_string<TYPE,TRAITS,ALLOC>&)
    { return true; }

namespace BloombergLP {
namespace bslma {
// Specialize trait to clarify for bde_verify that 'LimitAllocator' does not
// require satisfy the 'UsesBslmaAllocator' trait.
template <class ALLOC>
struct UsesBslmaAllocator<LimitAllocator<ALLOC> > : bsl::false_type {};

}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE>,
          class ALLOC  = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..E] correspond to arbitrary (but unique) 'char'
    // values to be appended to the 'string' object.  A tilde ('~') indicates
    // that the logical (but not necessarily physical) state of the object is
    // to be set to its initial, empty state (via the 'clear' method).
    //..
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'H'
    //                                      // unique but otherwise arbitrary
    // <CLEAR>      ::= '~'
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Append the value corresponding to A.
    // "AA"         Append two values both corresponding to A.
    // "ABC"        Append three values corresponding to A, B and C.
    // "ABC~"       Append three values corresponding to A, B and C and then
    //              remove all the elements (set array length to 0).  Note that
    //              this spec yields an object that is logically equivalent
    //              (but not necessarily identical internally) to one
    //              yielded by ("").
    // "ABC~DE"     Append three values corresponding to A, B, and C; empty
    //              the object; and append values corresponding to D and E.
    //-------------------------------------------------------------------------
    //..

    // CONSTANTS
    enum {
        DEFAULT_CAPACITY = SHORT_STRING_BUFFER_BYTES / sizeof(TYPE) > 0
                                ? SHORT_STRING_BUFFER_BYTES / sizeof(TYPE) - 1
                                : 0
    };

    // TYPES
    typedef bsl::basic_string<TYPE, TRAITS, ALLOC> Obj;
        // Type under testing.

    typedef typename Obj::allocator_type           Allocator;
        // And its allocator (may be different from 'ALLOC' via rebind).

    typedef ALLOC AllocType;
        // Utility typedef for xlC10 silliness.

    typedef typename Obj::iterator                 iterator;
    typedef typename Obj::const_iterator           const_iterator;
    typedef typename Obj::reverse_iterator         reverse_iterator;
    typedef typename Obj::const_reverse_iterator   const_reverse_iterator;

    typedef bslmf::MovableRefUtil                  MoveUtil;

    struct InputIterator {
        // Input iterator type to test functions that take input iterators.

        const TYPE *d_data;

        typedef bsl::input_iterator_tag  iterator_category;
        typedef TYPE                     value_type;
        typedef bsls::Types::IntPtr      difference_type;
        typedef TYPE                    *pointer;
        typedef TYPE&                    reference;

        explicit InputIterator(const TYPE *initVal)
        : d_data(initVal)
            // Create an input iterator pointing to the specified '*initVal'
            // (or after 'initVal[-1]').
        {}

        // MANIPULATORS
        InputIterator& operator++()
            // Increment this iterator and return the new value.
        {
            ++d_data;

            return *this;
        }

        // ACCESSORS
        const TYPE& operator*() const
            // Dereference this iterator and return a reference to the object
            // it refers to.
        {
            return *d_data;
        }

        bool operator==(const InputIterator& rhs) const
            // Return 'true' if this iterator is equal to the specified 'rhs'
            // and 'false' otherwise.
        {
            return d_data == rhs.d_data;
        }

        bool operator!=(const InputIterator& rhs) const
            // Return 'true' if this iterator is not equal to the specified
            // 'rhs' and 'false' otherwise.
        {
            return d_data != rhs.d_data;
        }
    };

    // TEST APPARATUS
    static int getValues(const TYPE **values);
        // Load the specified 'values' with the address of an array containing
        // initialized values of the parameterized 'TYPE' and return the length
        // of that array.

    static int ggg(Obj *object, const char *spec, int verboseFlag = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push_back' and
        // white-box manipulator 'clear'.  Optionally specify a zero
        // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
        // index of the first invalid character, and a negative value
        // otherwise.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static Obj g(size_t length, TYPE seed);
        // Return, by value, a new string object with the specified 'length'
        // and the specified 'seed' character.  The actual content of the
        // string is not important, only the string length and the fact that
        // two different 'seeds' produce two different results.

    static void stretch(Obj *object, size_t size, const TYPE& value = TYPE());
        // Using only primary manipulators, extend the length of the specified
        // 'object' by the specified 'size' by adding copies of the optionally
        // specified 'value', or with the null character for the (template
        // parameter) 'TYPE' if 'value' is not specified.  The resulting value
        // is not specified.

    static void stretchRemoveAll(Obj         *object,
                                 size_t       size,
                                 const TYPE&  value = TYPE());
        // Using only primary manipulators, extend the capacity of the
        // specified 'object' to (at least) the specified 'size' by adding
        // copies of the optionally specified 'value' or with the null
        // character for the (template parameter) 'TYPE' if 'value' is not
        // specified; then remove all elements leaving 'object' empty.

    static void checkCompare(const Obj& X, const Obj& Y, int result);
        // Compare the specified 'X' and 'Y' strings according to the
        // specifications, and check that the specified 'result' agrees.

    // TEST CASES
    static void testCase35();
        // Test 'noexcept' specifications

    static void testCase33();
        // Test the 'initializer_list' functions where supported.

    static void testCase29();
        // Test the hash append specialization.

    static void testCase28();
        // Test the short string optimization.

    static void testCase26();
        // Test conversions to/from native strings.

    static void testCase25();
        // Test proper use of 'std::length_error'.

    static void testCase24();
        // Test comparison free operators.

    static void testCase24Negative();
        // Negative test 'compare' and comparison free operators.

    static void testCase23();
        // Test 'copy' and 'substr' methods.

    static void testCase23Negative();
        // Negative test for 'copy'.

    static void testCase22();
        // Test 'find...' methods.

    static void testCase22Negative();
        // Negative test for 'find...' methods.

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
              bool OTHER_FLAGS>
    static void testCase21_propagate_on_container_swap_dispatch();
    static void testCase21_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase21();
        // Test 'swap' member.

    static void testCase20();
        // Test 'replace'.

    static void testCase20MatchTypes();
        // Test 'replace' where the types of 'NUM_ELEMENTS' and 'VALUE' match.

    template <class CONTAINER>
    static void testCase20Range(const CONTAINER&);
        // Test 'replace' member template.

    static void testCase20Negative();
        // Negative test for 'replace'.

    static void testCase19();
        // Test 'erase' and 'pop_back'.

    static void testCase19Negative();
        // Negative test for 'erase' and 'pop_back'.

    static void testCase18();
        // Test 'insert' and move 'push_back'.

    template <class CONTAINER>
    static void testCase18Range(const CONTAINER&);
        // Test 'insert' member template.

    static void testCase18Negative();
        // Negative test for 'insert'.

    static void testCase17();
        // Test 'append'.

    template <class CONTAINER>
    static void testCase17Range(const CONTAINER&);
        // Test 'append' member template.

    static void testCase17Negative();
        // Negative test for 'append'.

    static void testCase16();
        // Test iterators.

    static void testCase15();
        // Test element access.

    static void testCase15Negative();
        // Negative test for element access.

    static void testCase14();
        // Test 'reserve' and capacity-related methods.

    static void testCase13();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testCase13Range(const CONTAINER&);
        // Test 'assign' member template.

    static void testCase13StrRefData();
        // Test 'assign' member template from 'StrRefData' type.

    static void testCase13InputIterator();
        // Test 'assign' member template from a pair of input iterators.

    static void testCase13Negative();
        // Negative test for 'assign' members.

    static void testCase12();
        // Test user-supplied constructors.

    static void testCase12Negative();
        // Negative test for user-supplied constructors.

    template <class CONTAINER>
    static void testCase12Range(const CONTAINER&);
        // Test user-supplied constructor templates.

    static void testCase10();
        // Test streaming functionality.  This test case tests nothing.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_move_assignment_dispatch();
    static void testCase9_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase9Move();
        // Test move assignment operator ('operator=').

    static void testCase9Negative();
        // Negative test for assignment operator ('operator=').

    static void testCase8();
        // Test generator function 'g'.

    static void testCase7();
        // Test copy constructors.

    static void testCase7Move();
        // Test move constructor.

    static void testCase6();
        // Test equality operators ('operator==/!=').

    static void testCase6Negative();
        // Negative test for equality operators.

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators ('push_back' and 'clear').

    static void testCase1();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void testCaseM1(const int NITER, const int RANDOM_SEED);
        // Performance regression test, performing the specified 'NITER'
        // iterations, and using the specified 'RANDOM_SEED' to vary the test
        // conditions.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::checkCompare(const Obj& X,
                                                 const Obj& Y,
                                                 int        result)
{
    // As per C++ standard, chapter 21, clause 21.3.7.9.

    typename Obj::size_type rlen = min(X.length(), Y.length());
    int ret = TRAITS::compare(X.data(), Y.data(), rlen);
    if (ret) {
        ASSERT(ret == result);
        return;                                                       // RETURN
    }
    if (X.size() > Y.size()) {
        ASSERT(result > 0);
        return;                                                       // RETURN
    }
    if (X.size() < Y.size()) {
        ASSERT(result < 0);
        return;                                                       // RETURN
    }
    if (X.size() == Y.size()) {
        ASSERT(result == 0);
        return;                                                       // RETURN
    }
    ASSERT(0);
}

template <class TYPE, class TRAITS, class ALLOC>
int TestDriver<TYPE,TRAITS,ALLOC>::getValues(const TYPE **values)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    const int NUM_VALUES = 12;
    static const TYPE initValues[NUM_VALUES] = { // avoid 'DEFAULT_VALUE' and
        TYPE(VA),                                // 'UNINITIALIZED_VALUE'.
        TYPE(VB),
        TYPE(VC),
        TYPE(VD),
        TYPE(VE),
        TYPE(VF),
        TYPE(VG),
        TYPE(VH),
        TYPE(VI),
        TYPE(VJ),
        TYPE(VK),
        TYPE(VL)
    };

    *values = initValues;
    return NUM_VALUES;
}

template <class TYPE, class TRAITS, class ALLOC>
int TestDriver<TYPE,TRAITS,ALLOC>::ggg(Obj        *object,
                                       const char *spec,
                                       int         verboseFlag)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'L') {
            object->push_back(VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->clear();
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.           // RETURN
        }
   }
   return SUCCESS;
}

template <class TYPE, class TRAITS, class ALLOC>
bsl::basic_string<TYPE,TRAITS,ALLOC>& TestDriver<TYPE,TRAITS,ALLOC>::gg(
                                                            Obj        *object,
                                                            const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class TRAITS, class ALLOC>
bsl::basic_string<TYPE,TRAITS,ALLOC>
TestDriver<TYPE,TRAITS,ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class TYPE, class TRAITS, class ALLOC>
bsl::basic_string<TYPE,TRAITS,ALLOC>
TestDriver<TYPE,TRAITS,ALLOC>::g(size_t length, TYPE seed)
{
    Obj object(length, TYPE());

    for (size_t i = 0; i < length; ++i) {
        object[i] = TYPE(i + seed);
    }

    return object;
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::stretch(Obj         *object,
                                            size_t       size,
                                            const TYPE&  value)
{
    ASSERT(object);
    for (size_t i = 0; i < size; ++i) {
        object->push_back(value);
    }
    ASSERT(object->size() >= size);
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::stretchRemoveAll(Obj         *object,
                                                     size_t       size,
                                                     const TYPE&  value)
{
    ASSERT(object);
    stretch(object, size, value);
    object->clear();
    ASSERT(0 == object->size());
}

                                // ----------
                                // TEST CASES
                                // ----------

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE, TRAITS, ALLOC>::testCase35()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the uniary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<TYPE>())
        P(bsls::NameOf<TRAITS>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: page 699: String classes

    // page 704
    //..
    //  // 21.3.1.2, construct/copy/destroy:
    //  basic_string() noexcept(noexcept(Allocator())) :
    //                      basic_string(Allocator()) { }
    //  explicit basic_string(const Allocator& a) noexcept;
    //  basic_string(basic_string&& str) noexcept;
    //  basic_string& operator=(basic_string&& str) noexcept(
    //      allocator_traits<Allocator>::
    //                         propagate_on_container_move_assignment::value ||
    //      allocator_traits<Allocator>::is_always_equal::value);
    //..

    {
        // Not implemented: 'basic_string()'

        ALLOC a;
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(Obj(a)));

        Obj str;
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(Obj(MoveUtil::move(str))));

        Obj s;
        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(s = MoveUtil::move(str)));
    }

    // page 704
    //..
    //  // 21.3.1.3, iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  reverse_iterator rbegin() noexcept;
    //  const_reverse_iterator rbegin() const noexcept;
    //  reverse_iterator rend() noexcept;
    //  const_reverse_iterator rend() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //  const_reverse_iterator crbegin() const noexcept;
    //  const_reverse_iterator crend() const noexcept;
    //..

    {
        Obj s;  const Obj& S = s;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(s.begin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.begin()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(s.end()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.end()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.rbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.rbegin()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(s.rend()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.rend()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.cbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.cend()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.crbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.crend()));
    }

    // page 704 - 705
    //..
    //  // 21.3.1.4, capacity:
    //  size_type size() const noexcept;
    //  size_type length() const noexcept;
    //  size_type max_size() const noexcept;
    //  size_type capacity() const noexcept;
    //  void clear() noexcept;
    //  bool empty() const noexcept;
    //..

    {
        Obj s;  const Obj& S = s;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.size()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.length()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.max_size()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.capacity()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(s.clear()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.empty()));
    }

    // page 705 - 706
    //..
    //  // 21.3.1.6, modifiers:
    //  basic_string& assign(basic_string&& str) noexcept(
    //      allocator_traits<Allocator>::
    //                         propagate_on_container_move_assignment::value ||
    //      allocator_traits<Allocator>::is_always_equal::value);
    //  void swap(basic_string& str) noexcept(
    //       allocator_traits<Allocator>::propagate_on_container_swap::value ||
    //       allocator_traits<Allocator>::is_always_equal::value);
    //..

    // page 706 - 707
    //..
    //  // 21.3.1.7, string operations:
    //  const charT* c_str() const noexcept;
    //  const charT* data() const noexcept;
    //  charT* data() noexcept;
    //  allocator_type get_allocator() const noexcept;
    //
    //  size_type find (const basic_string& str,
    //                  size_type pos = 0) const noexcept;
    //  size_type rfind(const basic_string& str,
    //                  size_type pos = npos) const noexcept;
    //  size_type find_first_of    (const basic_string& str,
    //                              size_type pos = 0) const noexcept;
    //  size_type find_last_of     (const basic_string& str,
    //                              size_type pos = npos) const noexcept;
    //  size_type find_first_not_of(const basic_string& str,
    //                              size_type pos = 0) const noexcept;
    //  size_type find_last_not_of (const basic_string& str,
    //                              size_type pos = npos) const noexcept;
    //  int compare(const basic_string& str) const noexcept;
    //..

    {
        Obj s;  const Obj& S = s;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.c_str()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.data()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(s.data()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.get_allocator()));

        Obj    str;
        size_t pos;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.find             (str, pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.rfind            (str, pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.find_first_of    (str, pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.find_last_of     (str, pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.find_first_not_of(str, pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.find_last_not_of (str, pos)));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(S.compare(str)));
    }

    // page 725
    //..
    //  21.3.2.2 operator== [string::operator==]
    //  template<class charT, class traits, class Allocator>
    //  bool operator==(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.3 operator!= [string::op!=]
    //  template<class charT, class traits, class Allocator>
    //  bool operator!=(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.4 operator< [string::op<]
    //  template<class charT, class traits, class Allocator>
    //  bool operator< (const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.5 operator> [string::op>]
    //  template<class charT, class traits, class Allocator>
    //  bool operator> (const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.6 operator<= [string::op<=]
    //  template<class charT, class traits, class Allocator>
    //  bool operator<=(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.7 operator>= [string::op>=]
    //  template<class charT, class traits, class Allocator>
    //  bool operator>=(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.8 swap [string.special]
    //  template<class charT, class traits, class Allocator>
    //  void swap(basic_string<charT,traits,Allocator>& lhs,
    //            basic_string<charT,traits,Allocator>& rhs)
    //      noexcept(noexcept(lhs.swap(rhs)));

    {
        Obj lhs;
        Obj rhs;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs == rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs != rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs <  rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs >  rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs <= rhs));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(lhs >= rhs));

        using bsl::swap;
        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(swap(lhs, rhs)));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase33()
{
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    // --------------------------------------------------------------------
    // TESTING 'initializer_list' FUNCTIONS
    //   Throughout this test, there are three categories of string lengths
    //   to be considered:
    //
    //: o Zero length
    //: o A length below the maximum length for short string optimization
    //: o A length above the short-string optimization limit (i.e., a string
    //:   that requires allocated memory)
    //
    // Concerns:
    //
    //: 1 Construction
    //:
    //:   1 The created string has the specified value and uses the specified
    //:     allocator.
    //:
    //:   2 There is no allocation when the initial string is less than or
    //:     equal to the short-string optimization limit.
    //:
    //:   3 The created strings can be of any of the three length categories
    //:
    //:   4 The constructor is exception-safe.
    //:
    //: 2 Assignment
    //:
    //:   1 The 'assign' and 'operator=' methods behave identically.
    //:
    //:   2 The 'lhs' value is set to the 'rhs' value.
    //:
    //:   3 The 'lhs' and 'rhs' can each be in any of the three string length
    //:     categories.
    //:
    //:   4 The assignment methods are exception-safe.
    //:
    //: 3 Insertion
    //:
    //:   1 The insertion point can be at the beginning, middle, or end of the
    //:     target string.
    //:
    //:   2 The insertion argument must be 'const'.
    //:
    //:   3 The target string and source initialization list can be of any of
    //:     the three string length categories.
    //:
    //:   4 The target string is set to the expected value.
    //:
    //:   5 The returned iterator has the expected value.
    //:
    //:   6 The returned iterator provided modifiable access to the string.
    //:
    //:   7 The insertion method is exception safe.
    //
    // Plan:
    //: 1 Construction (C-1.1..4)
    //:
    //:   1 For an initialization list at the boundaries between the three
    //:     string length categories, create a test object (string) that uses a
    //:     test allocator.
    //:
    //:   2 Confirm that the created object has the expected value.
    //:
    //:   3 Confirm that the object's expected allocator is reported the
    //:     accessor and shows allocation when expected and the default
    //:     allocator shows no use.
    //:
    //:   4 Wrap the construction with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that so that each allocation experiences an injected
    //:     exception at some point.
    //:
    //:   5 Repeat P-1.1 to P-1.4 for an object created to use the currently
    //:     installed default constructor and tests adjusted to expect use of
    //:     the default constructor.
    //:
    //: 2 Assignment (C-2.1..4)
    //:
    //:   1 Using a table-drive approach create a lhs object (a string) and a
    //:     rhs object (an initialization list) so that we at least one data
    //:     point in the cross product of the boundaries between the three
    //:     string length categories.
    //:
    //:   2 Confirm that the 'lhs' is set to the expected value.
    //:
    //:   3 Confirm that the 'lhs' allocator shows use when expected.
    //:
    //:   4 Wrap the assignments with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that so that each allocation experiences an injected
    //:     exception at some point.
    //:
    //:   5 For each step P-2.1 to P-2.4, shadow each use of the 'assign'
    //:     method with an analogous use of 'operator=' on an equivalent 'lhs'
    //:     object.
    //:
    //: 3 Insertion (C-3.1..7)
    //:
    //:   1 Using a table-drive approach create a target object (a string) and
    //:     a source object (an initialization list) so that we at least one
    //:     data point in the cross product of of the boundaries between the
    //:     three string length categories.  The table also defines insertion
    //:     points corresponding the beginning, middle, and end of the target
    //:     string.
    //:
    //:   2 Supply the position argument to the 'insert' method as a
    //:     'const_iterator' object.
    //:
    //:   2 Confirm that the target string is set to the expected value.
    //:
    //:   3 Confirm that the allocator of the target string shows use when
    //:     expected.
    //:
    //:   4 Confirm that the return value can be assigned to a (non-'const')
    //:     'iterator' object.
    //:
    //:   4 Wrap the assignments with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that so that each allocation experiences an injected
    //:     exception at some point.
    //
    // Testing:
    //   basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
    //   basic_string& operator=(initializer_list<CHAR_TYPE> values);
    //   basic_string& assign(initializer_list<CHAR_TYPE> values);
    //   iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
    // --------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting parameters: TYPE = %s.\n", NameOf<TYPE>().name());
        P(sizeof(TYPE))
    }

    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<TYPE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    // Define values that require allocations.

    typedef std::initializer_list<TYPE> IList;

#define LET_A0
#define LET_A1  'A'
#define LET_A2  'A', 'A'
#define LET_A3  'A', 'A', 'A'
#define LET_A4  'A', 'A', 'A', 'A'
#define LET_A5  'A', 'A', 'A', 'A', 'A'
#define LET_A6  'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A7  'A', 'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A8  'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A9  'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'

#define LET_A10 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A20 LET_A10, LET_A10
#define LET_A30 LET_A10, LET_A10, LET_A10
#define LET_A40 LET_A10, LET_A10, LET_A10, LET_A10
#define LET_A50 LET_A10, LET_A10, LET_A10, LET_A10, LET_A10

    const IList iListBySize[] = {
        {          LET_A0 }
      , {          LET_A1 }
      , {          LET_A2 }
      , {          LET_A3 }
      , {          LET_A4 }
      , {          LET_A5 }
      , {          LET_A6 }
      , {          LET_A7 }
      , {          LET_A8 }
      , {          LET_A9 }

      , { LET_A10, LET_A0 }
      , { LET_A10, LET_A1 }
      , { LET_A10, LET_A2 }
      , { LET_A10, LET_A3 }
      , { LET_A10, LET_A4 }
      , { LET_A10, LET_A5 }
      , { LET_A10, LET_A6 }
      , { LET_A10, LET_A7 }
      , { LET_A10, LET_A8 }
      , { LET_A10, LET_A9 }

      , { LET_A20, LET_A0 }
      , { LET_A20, LET_A1 }
      , { LET_A20, LET_A2 }
      , { LET_A20, LET_A3 }
      , { LET_A20, LET_A4 }
      , { LET_A20, LET_A5 }
      , { LET_A20, LET_A6 }
      , { LET_A20, LET_A7 }
      , { LET_A20, LET_A8 }
      , { LET_A20, LET_A9 }

      , { LET_A30, LET_A0 }
      , { LET_A30, LET_A1 }
      , { LET_A30, LET_A2 }
      , { LET_A30, LET_A3 }
      , { LET_A30, LET_A4 }
      , { LET_A30, LET_A5 }
      , { LET_A30, LET_A6 }
      , { LET_A30, LET_A7 }
      , { LET_A30, LET_A8 }
      , { LET_A30, LET_A9 }

      , { LET_A40, LET_A0 }
      , { LET_A40, LET_A1 }
      , { LET_A40, LET_A2 }
      , { LET_A40, LET_A3 }
      , { LET_A40, LET_A4 }
      , { LET_A40, LET_A5 }
      , { LET_A40, LET_A6 }
      , { LET_A40, LET_A7 }
      , { LET_A40, LET_A8 }
      , { LET_A40, LET_A9 }

      , { LET_A50, LET_A0 }
    };
    const size_t NUM_iListBySize = sizeof iListBySize / sizeof *iListBySize;

    // Check table correctness

    for (size_t i = 0; i < NUM_iListBySize; ++i) {
        IList LIST = iListBySize[i];
        LOOP_ASSERT(i, i == LIST.size());
        for (typename IList::const_iterator cur  = LIST.begin(),
                                            end  = LIST.end();
                                            end != cur;
                                            ++cur) {
            LOOP_ASSERT(i, 'A' == *cur);
        }
    }

#define STR_A0  ""
#define STR_A1  "A"
#define STR_A2  "A"  "A"
#define STR_A3  "A"  "A"  "A"
#define STR_A4  "A"  "A"  "A"  "A"
#define STR_A5  "A"  "A"  "A"  "A"  "A"
#define STR_A6  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A7  "A"  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A8  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A9  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"

#define STR_A10 "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A20 STR_A10  STR_A10
#define STR_A30 STR_A10  STR_A10  STR_A10
#define STR_A40 STR_A10  STR_A10  STR_A10  STR_A10
#define STR_A50 STR_A10  STR_A10  STR_A10  STR_A10  STR_A10

    const char *iSpecBySize[] = {
                   STR_A0
      ,            STR_A1
      ,            STR_A2
      ,            STR_A3
      ,            STR_A4
      ,            STR_A5
      ,            STR_A6
      ,            STR_A7
      ,            STR_A8
      ,            STR_A9

      ,   STR_A10  STR_A0
      ,   STR_A10  STR_A1
      ,   STR_A10  STR_A2
      ,   STR_A10  STR_A3
      ,   STR_A10  STR_A4
      ,   STR_A10  STR_A5
      ,   STR_A10  STR_A6
      ,   STR_A10  STR_A7
      ,   STR_A10  STR_A8
      ,   STR_A10  STR_A9

      ,   STR_A20  STR_A0
      ,   STR_A20  STR_A1
      ,   STR_A20  STR_A2
      ,   STR_A20  STR_A3
      ,   STR_A20  STR_A4
      ,   STR_A20  STR_A5
      ,   STR_A20  STR_A6
      ,   STR_A20  STR_A7
      ,   STR_A20  STR_A8
      ,   STR_A20  STR_A9

      ,   STR_A30  STR_A0
      ,   STR_A30  STR_A1
      ,   STR_A30  STR_A2
      ,   STR_A30  STR_A3
      ,   STR_A30  STR_A4
      ,   STR_A30  STR_A5
      ,   STR_A30  STR_A6
      ,   STR_A30  STR_A7
      ,   STR_A30  STR_A8
      ,   STR_A30  STR_A9

      ,   STR_A40  STR_A0
      ,   STR_A40  STR_A1
      ,   STR_A40  STR_A2
      ,   STR_A40  STR_A3
      ,   STR_A40  STR_A4
      ,   STR_A40  STR_A5
      ,   STR_A40  STR_A6
      ,   STR_A40  STR_A7
      ,   STR_A40  STR_A8
      ,   STR_A40  STR_A9

      ,   STR_A50  STR_A0
    };
    const size_t NUM_iSpecBySize = sizeof iSpecBySize / sizeof *iSpecBySize;

    // Check table correctness

    ASSERT(NUM_iListBySize == NUM_iSpecBySize);

    for (size_t i = 0; i < NUM_iSpecBySize; ++i) {
        const char * const SPEC = iSpecBySize[i];

        LOOP_ASSERT(i, i == strlen(SPEC));

        for (const char *       cur  = SPEC,
                        * const end  = SPEC + i;
                                end != cur;
                                ++cur) {
            LOOP_ASSERT(i, 'A' == *cur);
        }
    }

    const size_t k_SHORT_BUFFER_CAPACITY = ExpectedShortBufferCapacity<TYPE>
                                                                       ::value;

    const size_t     maxShortStrLen   = k_SHORT_BUFFER_CAPACITY;
    const size_t exceedsShortStrLen   = maxShortStrLen + 1;

    ASSERT(2 * exceedsShortStrLen < NUM_iListBySize);

    IList                        emptyList = iListBySize[0];
    const char * const           emptySpec = iSpecBySize[0];

    IList                  maxShortStrList = iListBySize[    maxShortStrLen];
    const char * const     maxShortStrSpec = iSpecBySize[    maxShortStrLen];

    IList              exceedsShortStrList = iListBySize[exceedsShortStrLen];
    const char * const exceedsShortStrSpec = iSpecBySize[exceedsShortStrLen];

    if (verbose) printf(
  "\nTesting constructor: initializer lists and specified object allocator\n");
    {
        const struct {
            int                         d_line;       // source line number
            std::initializer_list<TYPE> d_list;       // source list
            const char                 *d_specResult; // expected result
        } DATA[] = {
                //line  list                   result
                //----  -------------------    -------------------
                { L_,     {               },                    "" },
                { L_,     { 'A'           },                   "A" },
                { L_,     { 'A', 'A'      },                  "AA" },
                { L_,     { 'B', 'A'      },                  "BA" },
                { L_,     { 'A', 'B', 'C' },                 "ABC" },
                { L_,     { 'A', 'B', 'A' },                 "ABA" },

                { L_,             emptyList,             emptySpec },
                { L_,       maxShortStrList,       maxShortStrSpec },
                { L_,   exceedsShortStrList,   exceedsShortStrSpec }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE        = DATA[ti].d_line;
            IList              LIST        = DATA[ti].d_list;
            const char * const SPEC_RESULT = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE) P_(LIST) P(SPEC_RESULT) }

            if (veryVeryVerbose) { P_(Obj().capacity()) P(LIST.size()) }

            ASSERT(strlen(SPEC_RESULT) == LIST.size()); // table sanity check

            Tam oam(objectAllocator_p);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Obj mX(LIST, objectAllocator_p); const Obj& X = mX;

            ASSERT(Y                 == X);
            ASSERT(objectAllocator_p == X.get_allocator());
            ASSERT(&scratch          == Y.get_allocator());

            if (Obj().capacity() < LIST.size()) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }

        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf(
           "\nTesting constructor: initializer lists and default allocator\n");
    {
        const struct {
            int                         d_line;       // source line number
            std::initializer_list<TYPE> d_list;       // source list
            const char                 *d_specResult; // expected result
        } DATA[] = {
                //line  list                  result
                //----  -------------------   -------------------
                { L_,     {               },                   "" },
                { L_,     { 'A'           },                  "A" },
                { L_,     { 'A', 'A'      },                 "AA" },
                { L_,     { 'B', 'A'      },                 "BA" },
                { L_,     { 'A', 'B', 'C' },                "ABC" },
                { L_,     { 'A', 'B', 'A' },                "ABA" },

                { L_,             emptyList,            emptySpec },
                { L_,       maxShortStrList,      maxShortStrSpec },
                { L_,   exceedsShortStrList,  exceedsShortStrSpec }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE        = DATA[ti].d_line;
            IList              LIST        = DATA[ti].d_list;
            const char * const SPEC_RESULT = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE) P_(LIST) P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_RESULT) == LIST.size()); // table sanity check

            Tam dam(defaultAllocator_p);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Obj mX(LIST); const Obj& X = mX;

            ASSERT(Y                  == X);
            ASSERT(defaultAllocator_p == X.get_allocator());
            ASSERT(&scratch           == Y.get_allocator());

            if (Obj().capacity() < LIST.size()) {
                ASSERT(dam.isTotalUp());
            }
            else {
                ASSERT(dam.isTotalSame());
            }
        }
    }

    if (verbose) printf("\nTesting constructor: exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t length = 0; length < NUM_LENGTHS; ++length) {

            if (veryVerbose) { T_ P(length) }

            LOOP_ASSERT(length, length < NUM_iListBySize);
            IList              LIST = iListBySize[length];
            const char * const SPEC = iSpecBySize[length];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            int        exceptionLoopCount         = 0;
            const bool allocationExpected         = k_SHORT_BUFFER_CAPACITY
                                                    < length;
            const int  expectedExceptionLoopCount = allocationExpected
                                                    ? 2
                                                    : 1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                ++exceptionLoopCount;

                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                             P(exceptionLoopCount)
                                     }
                Tam oem(objectAllocator_p);

                Obj mX(LIST, objectAllocator_p); const Obj& X = mX;

                if (allocationExpected) {
                    ASSERT(oem.isTotalUp());
                }
                else {
                    ASSERT(oem.isTotalSame());
                }

                ASSERTV(length, Y                 == X);
                ASSERTV(length, objectAllocator_p == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(length,
                    expectedExceptionLoopCount == exceptionLoopCount);
        }
        ASSERT(oam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting 'assign' and 'operator=' with initializer lists\n");
    {
        const struct {
            int                         d_line;        // source line number
            const char                 *d_specLhs;     // target string
            std::initializer_list<TYPE> d_list;        // source list
            const char                 *d_specResult;  // expected result
        } DATA[] = {
       //-------^
       //line  lhs                  list                 result
       //----  -------------------- -------------------- -------------------
       { L_,                    "",   {               },                  "" },
       { L_,                    "",   { 'A'           },                 "A" },
       { L_,                   "A",   {               },                 ""  },
       { L_,                   "A",   { 'B'           },                "B"  },
       { L_,                   "A",   { 'A', 'B'      },               "AB"  },
       { L_,                   "A",   { 'B', 'C'      },               "BC"  },
       { L_,                  "AB",   {               },                 ""  },
       { L_,                  "AB",   { 'A', 'B', 'C' },              "ABC"  },
       { L_,                  "AB",   { 'C', 'D', 'E' },              "CDE"  },

       { L_,             emptySpec,           emptyList,           emptySpec },
       { L_,             emptySpec,     maxShortStrList,     maxShortStrSpec },
       { L_,             emptySpec, exceedsShortStrList, exceedsShortStrSpec },

       { L_,       maxShortStrSpec,           emptyList,           emptySpec },
       { L_,       maxShortStrSpec,     maxShortStrList,     maxShortStrSpec },
       { L_,       maxShortStrSpec, exceedsShortStrList, exceedsShortStrSpec },

       { L_,   exceedsShortStrSpec,           emptyList,           emptySpec },
       { L_,   exceedsShortStrSpec,     maxShortStrList,     maxShortStrSpec },
       { L_,   exceedsShortStrSpec, exceedsShortStrList, exceedsShortStrSpec }
       //-------V
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE        = DATA[ti].d_line;
            const char * const SPEC_LHS    = DATA[ti].d_specLhs;
            IList              LIST        = DATA[ti].d_list;
            const char * const SPEC_RESULT = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE)
                               P_(LIST)
                               P_(SPEC_LHS)
                               P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_RESULT) == LIST.size()); // table sanity check

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, SPEC_LHS);
            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC_LHS);
            Obj mZ(&scratch);          const Obj& Z = gg(&mZ, SPEC_RESULT);

            Tam          oam(objectAllocator_p);
            const size_t capacity = X.capacity();

            mX.assign(LIST);
            ASSERTV(Z, X, Z == X);

            mY = LIST;
            ASSERTV(Z, Y, Z == Y);

            if (capacity < LIST.size()) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting 'assign' and 'operator=': exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen,
                                   exceedsShortStrLen + 1
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t dstLength = 0; dstLength < NUM_LENGTHS; ++dstLength) {

            if (veryVerbose) { T_ P(dstLength) }

            LOOP_ASSERT(dstLength, dstLength < NUM_iSpecBySize);

            const char * const SPEC = iSpecBySize[dstLength];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            for (size_t srcLength = 0; srcLength < NUM_LENGTHS; ++srcLength) {
                if (veryVerbose) { T_ T_ P(srcLength) }

                LOOP_ASSERT(srcLength, srcLength < NUM_iListBySize);

                IList LIST  = iListBySize[srcLength];

                const bool allocationExpected = max(Y.capacity(),
                                                    k_SHORT_BUFFER_CAPACITY)
                                              < srcLength;

                int                   allocationCount = 0;
                const int     expectedAllocationCount = allocationExpected
                                                        ? 2  // two methods
                                                        : 0;
                int                exceptionLoopCount = 0;
                const int  expectedExceptionLoopCount = expectedAllocationCount
                                                      + 1;

                Obj        mX(LIST, objectAllocator_p);
                const Obj&  X = mX;

                Obj        mYmethod(Y, objectAllocator_p);
                const Obj&  Ymethod = mYmethod;

                Obj        mYoperator(Y, objectAllocator_p);
                const Obj&  Yoperator = mYoperator;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                    ++exceptionLoopCount;

                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                                 P(exceptionLoopCount)
                                         }
                    Tam oem(objectAllocator_p);

                    mYmethod.assign(LIST);  // test 'assign'    method
                    if (allocationExpected) ++allocationCount;

                    mYoperator = LIST;      // test 'operator=' method
                    if (allocationExpected) ++allocationCount;

                    ASSERTV(X == Ymethod);
                    ASSERTV(X == Yoperator);

                    if (allocationExpected) {
                        ASSERT(oem.isTotalUp());
                    }
                    else {
                        ASSERT(oem.isTotalSame());
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(srcLength,
                         expectedExceptionLoopCount ==  exceptionLoopCount);
                ASSERTV(srcLength,
                            expectedAllocationCount ==     allocationCount);
            }
        }
        ASSERT(oam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'insert' with initializer lists\n");
    {
        const char * const            maxShortStrSpec2 = iSpecBySize[
                                                           2 * maxShortStrLen];
        const char * const        exceedsShortStrSpec2 = iSpecBySize[
                                                       2 * exceedsShortStrLen];
        const char * const maxPlusExceedsShortStrSpec = iSpecBySize[
                                          maxShortStrLen + exceedsShortStrLen];

        const size_t M = maxShortStrLen / 2;
        const size_t E = maxShortStrLen;

        const struct {
            int                         d_line;        // source line number
            const char                 *d_specInitial; // initial state
            int                         d_position;    // position to insert
            std::initializer_list<TYPE> d_list;        // source list
            const char                 *d_specResult;  // expected result
        } DATA[] = {
   //-----------^
   //line  source              pos  list                result
   //----  ------------------  ---  ------------------  -------------------
   { L_,                   "", -1,   {                },                  "" },
   { L_,                   "", 99,   { 'A'            },                 "A" },
   { L_,                  "A",  0,   {                },                 "A" },
   { L_,                  "A",  0,   { 'B'            },                "BA" },
   { L_,                  "A",  1,   { 'B'            },                "AB" },
   { L_,                 "AB",  0,   {                },                "AB" },
   { L_,                 "AB",  0,   { 'A', 'B'       },              "ABAB" },
   { L_,                 "AB",  1,   { 'B', 'C'       },              "ABCB" },
   { L_,                 "AB",  2,   { 'A', 'B', 'C'  },             "ABABC" },
   { L_,                "ABC",  0,   { 'D'            },              "DABC" },

   { L_,            emptySpec,  0,           emptyList,           emptySpec  },
   { L_,            emptySpec,  0,     maxShortStrList,     maxShortStrSpec  },
   { L_,            emptySpec,  0, exceedsShortStrList, exceedsShortStrSpec  },

   { L_,      maxShortStrSpec,  0,           emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,  0,     maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  0, exceedsShortStrList,
                                                 maxPlusExceedsShortStrSpec  },

   { L_,      maxShortStrSpec,  M,           emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,  M,     maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  M, exceedsShortStrList,
                                                 maxPlusExceedsShortStrSpec  },

   { L_,      maxShortStrSpec,  E,           emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,  E,     maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  E, exceedsShortStrList,
                                                 maxPlusExceedsShortStrSpec  },

   { L_,  exceedsShortStrSpec,  0,           emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  0,     maxShortStrList,
                                                 maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  0, exceedsShortStrList, exceedsShortStrSpec2 },

   { L_,  exceedsShortStrSpec,  M,           emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  M,     maxShortStrList,
                                                 maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  M, exceedsShortStrList, exceedsShortStrSpec2 },

   { L_,  exceedsShortStrSpec,  E,           emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  E,     maxShortStrList,
                                                 maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  E, exceedsShortStrList, exceedsShortStrSpec2 }
   //-----------V
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE         = DATA[ti].d_line;
            const char * const SPEC_INITIAL = DATA[ti].d_specInitial;
            const int          POSITION     = DATA[ti].d_position;
            IList              LIST         = DATA[ti].d_list;
            const char * const SPEC_RESULT  = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE)
                               P_(LIST)
                               P_(SPEC_INITIAL)
                               P_(POSITION)
                               P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_INITIAL) + LIST.size()
                == strlen(SPEC_RESULT));  // table sanity check

            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, SPEC_INITIAL);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Tam oam(objectAllocator_p);

            size_t index = POSITION == -1 ? 0
                         : POSITION == 99 ? X.size()
                         : /* else */       POSITION;

            bool  expectAllocation = X.capacity() - X.length() < LIST.size();

            const_iterator position =  X.begin() + index;
            iterator       result   = mX.insert(position, LIST);

            ASSERTV(LINE,       result == X.begin() + index);
            ASSERTV(LINE, X, Y, X      == Y);

            if (expectAllocation) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'insert': exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen,
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t dstLength = 0; dstLength < NUM_LENGTHS; ++dstLength) {

            if (veryVerbose) { T_ P(dstLength) }

            LOOP_ASSERT(dstLength, dstLength < NUM_iSpecBySize);

            const char * const SPEC = iSpecBySize[dstLength];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            const size_t srcLength = Y.capacity() - Y.length() + 1;
            ASSERTV(dstLength, srcLength, srcLength < NUM_iListBySize);

            IList LIST = iListBySize[srcLength];

            const size_t expectedLength = Y.length() + srcLength;
            ASSERTV(dstLength, srcLength, expectedLength < NUM_iSpecBySize);

            const char * const expectedSPEC = iSpecBySize[expectedLength];

            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, expectedSPEC);

            int                exceptionLoopCount = 0;
            const int  expectedExceptionLoopCount = 2;
            int                   allocationCount = 0;
            const int     expectedAllocationCount = 1;

            Tam oem(objectAllocator_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                ++exceptionLoopCount;

                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                             P(exceptionLoopCount) }

                mY.insert(Y.size(), LIST);
                ++allocationCount;  // 'LIST' calculated to allocate.

                ASSERTV(srcLength, X == Y);
                ASSERTV(srcLength, oem.isInUseUp());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(srcLength,
                     expectedExceptionLoopCount ==  exceptionLoopCount);
            ASSERTV(srcLength,
                        expectedAllocationCount ==     allocationCount);
        }
        ASSERT(oam.isInUseSame());
    }
#else
    // Empty test.
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase29()
{
    // --------------------------------------------------------------------
    // TESTING 'hashAppend'
    //   Verify that the hashAppend function works properly and is picked
    //   up by 'bslh::Hash'
    //
    // Concerns:
    //: 1 'bslh::Hash' picks up 'hashAppend(string)' and can hash strings
    //:
    //: 2 'hashAppend' hashes the entire string, regardless of 'char' or
    //:   'wchar'
    //:
    //: 3 Empty strings can be hashed
    //:
    //: 4 Hash is not computed on data beyond the end of very short strings
    //
    // Plan:
    //: 1 Use 'bslh::Hash' to hash a few values of strings with each char type.
    //:   (C-1,2)
    //:
    //: 2 Hash an empty string. (C-3)
    //:
    //: 3 Hash two very short strings with the same value and assert that they
    //:   produce equal hashes. (C-4)
    //
    // Testing:
    //   hashAppend(HASHALG& hashAlg, const basic_string& str);
    //   hashAppend(HASHALG& hashAlg, const native_std::basic_string& str);
    // --------------------------------------------------------------------
    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef typename Hasher::result_type HashType;
    typedef native_std::basic_string<TYPE,TRAITS,ALLOC> NativeObj;

    const int PRIME = 100003; // Arbitrary large prime to be used in hash-table
                              // like testing

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif

    int       collisions[PRIME]       = {};
    int       nativeCollisions[PRIME] = {};

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

    Hasher    hasher;
    size_t    prevHash                = 0;
    HashType  hash                    = 0;

    if (verbose) printf("Use 'bslh::Hash' to hash a few values of strings with"
                        " each char type. (C-1,2)\n");
    {
        for (int i = 0; i != PRIME; ++i) {
            Obj num;
            if (i > 66000){
                //Make sure we're testing long strings
                for (int j = 0; j < 40; ++j) {
                    num.push_back(TYPE('A'));
                }
            }
            else if (i > 33000) {
                //Make sure we're testing with null characters in the strings
                for (int j = 0; j < 5; ++j) {
                    num.push_back(TYPE('A'));
                    num.push_back(TYPE('\0'));
                }
            }
            num.push_back( TYPE('0' + (i/1000000)     ));
            num.push_back( TYPE('0' + (i/100000)  %10 ));
            num.push_back( TYPE('0' + (i/10000)   %10 ));
            num.push_back( TYPE('0' + (i/1000)    %10 ));
            num.push_back( TYPE('0' + (i/100)     %10 ));
            num.push_back( TYPE('0' + (i/10)      %10 ));
            num.push_back( TYPE('0' + (i)         %10 ));

            if (veryVerbose) dbg_print("Testing hash of ", num.data(), "\n");

            prevHash = hash;
            hash     = hasher(num);

            // Check consecutive values are not hashing to the same hash
            ASSERT(prevHash != hash);

            // Check that minimal collisions are happening
            ASSERT(++collisions[hash % PRIME] <= 11);  // Choose 11 as a max
                                                   // number collisions

            Obj numCopy = num;

            // Verify same hash is produced for the same value
            ASSERT(num == numCopy);
            ASSERT(hash == hasher(numCopy));
        }
    }

    if (verbose) printf("Use 'bslh::Hash' to hash a few values of 'native_std'"
                        " strings with each char type. (C-1,2)\n");
    {
        for (int i = 0; i != PRIME; ++i) {
            NativeObj num;
            if (i > 66000){
                //Make sure we're testing long strings
                for (int j = 0; j < 40; ++j) {
                    num.push_back(TYPE('A'));
                }
            }
            else if (i > 33000) {
                //Make sure we're testing with null characters in the strings
                for (int j = 0; j < 5; ++j) {
                    num.push_back(TYPE('A'));
                    num.push_back(TYPE('\0'));
                }
            }
            num.push_back( TYPE('0' + (i/1000000)     ));
            num.push_back( TYPE('0' + (i/100000)  %10 ));
            num.push_back( TYPE('0' + (i/10000)   %10 ));
            num.push_back( TYPE('0' + (i/1000)    %10 ));
            num.push_back( TYPE('0' + (i/100)     %10 ));
            num.push_back( TYPE('0' + (i/10)      %10 ));
            num.push_back( TYPE('0' + (i)         %10 ));

            if (veryVerbose) dbg_print("Testing hash of ", num.data(), "\n");

            prevHash = hash;
            hash     = hasher(num);

            // Check consecutive values are not hashing to the same hash
            ASSERT(prevHash != hash);

            // Check that minimal collisions are happening
            ASSERT(++nativeCollisions[hash % PRIME] <= 11);
                                                         // Choose 11 as a max
                                                         // number collisions

            NativeObj numCopy = num;

            // Verify same hash is produced for the same value
            ASSERT(num == numCopy);
            ASSERT(hash == hasher(numCopy));
        }
    }

    if (verbose) printf("Hash an empty string. (C-3)\n");
    {
        Obj empty;
        ASSERT(hasher(empty));
    }

    if (verbose) printf("Hash two very short strings with the same value and"
                        " assert that they produce equal hashes. (C-4)\n");
    {
        Obj small1;
        Obj small2;
        small1.push_back( TYPE('0') );
        small2.push_back( TYPE('0') );
        ASSERT(hasher(small1) == hasher(small2));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase28()
{
    // --------------------------------------------------------------------
    // TESTING THE SHORT STRING OPTIMIZATION
    //
    // Concerns:
    //  1) String should have an initial non-zero capacity (short string
    //     buffer).
    //  2) It should not allocate up to that capacity.
    //  3) It should work with the char_type larger than the short string
    //     buffer.
    //  4) A Long string with length smaller than the size of the short string
    //     buffer copied to a new string should produce a short new string.
    //  5) It should work with the NULL-terminator different from '\0' to make
    //     sure that the implementation always uses char_type() default
    //     constructor to terminate the string rather than a null literal.
    //
    // Plan:
    //  1) Construct an empty string and check its capacity.
    //  2) Construct strings with lengths from 0 to N (where N > initial
    //     capacity) and verify that the string class allocates when the string
    //     length becomes larger than the short string buffer.
    //  3) Instantiate the string class with 'UserChar' char type and run it
    //     through this test.  'UserChar' is parameterized with size from 1 to
    //     8 words.
    //  4) Construct a long string.  Erase some characters from it, so the
    //     length becomes smaller than the size of the short string buffer.
    //     Then make a copy of this string using the test allocator and verify
    //     that the new copy did not need any new memory.
    //  5) Make 'UserChar' default value something other than '\0'.  Make sure
    //     that strings of different lengths terminate with 'UserChar()' value
    //     rather than '\0'.
    // ------------------------------------------------------------------------

    if (verbose) printf("\nString has a non-zero initial capacity.\n");

    {
        Obj emptyStr;
        ASSERT(emptyStr.capacity() == DEFAULT_CAPACITY);
    }

    if (verbose) printf("\nString doesn't allocate while it uses the short "
                        "string buffer.\n");

    {
        // make some reasonable number of test iterations
        const size_t specSize = 2 * DEFAULT_CAPACITY > 10
                                     ? 2 * DEFAULT_CAPACITY
                                     : 10;
        char spec[specSize];

        for (size_t size = 1; size < specSize; ++size) {

            // construct the spec string
            for (size_t i = 0; i < size; ++i) {
                spec[i] = (i & 1) ? 'A' : 'B';
            }

            spec[size] = '\0';

            // check if the string allocates
            bslma::TestAllocator testAllocator(veryVeryVerbose);
            Obj str(g(spec), &testAllocator);

            // allocates only if larger than the short string buffer
            ASSERT((size <= DEFAULT_CAPACITY) ==
                   (testAllocator.numBytesInUse() == 0));
            ASSERT((size <= DEFAULT_CAPACITY) ==
                   (testAllocator.numBlocksTotal() == 0));

            // check if copy-constructor allocates
            Obj strCpy(str, &testAllocator);

            ASSERT((size <= DEFAULT_CAPACITY)
                    == (testAllocator.numBytesInUse() == 0));
            ASSERT((size <= DEFAULT_CAPACITY)
                    == (testAllocator.numBlocksTotal() == 0));

            // check that copying a long string with a short length results in
            // a short string
            if (str.size() > DEFAULT_CAPACITY) {
                Obj strLong(str);
                size_t oldCapacity = strLong.capacity();

                // remove some characters from the string to shorten it
                strLong.erase(0, strLong.size() - DEFAULT_CAPACITY);

                // check that 'erase' did not change the capacity
                ASSERT(strLong.capacity() == oldCapacity);

                // check that copying produces a short string now and that it
                // doesn't allocate
                bslma::TestAllocator testAllocatorShort(veryVeryVerbose);
                Obj strShort(strLong, &testAllocatorShort);

                ASSERT(testAllocatorShort.numBytesInUse() == 0);
                ASSERT(testAllocatorShort.numBlocksTotal() == 0);
            }

            // check that the string is terminated properly with 'TYPE()' value
            // rather than just '\0'
            ASSERT(*(str.c_str() + str.size()) == TYPE());
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase26()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSIONS WITH NATIVE STRINGS
    //
    // Testing:
    //   CONCERNS:
    //    - A bsl::basic_string is implicitly convertible to a
    //      native_std::basic_string with the same CHAR_TYPE and
    //      CHAR_TRAITS.
    //    - A native_std::basic_string is explicitly convertible to a
    //      bsl::basic_string with the same CHAR_TYPE and
    //      CHAR_TRAITS.
    //    - A bsl::basic_string and a native_std::basic_string with the
    //      same template parameters will have the same npos value.
    //
    // Plan:
    //   For a variety of strings of different sizes and different values, test
    //   that the string is implicitly convertible to native_std::string and
    //   that the conversion yields the same value.  Test that one can
    //   construct a bsl::string from the native_std::string.  Test that
    //   bslstl::string::npos compares equal to native_std::string::npos.
    //
    // Testing:
    //   npos
    //   operator native_std::basic_string<CHAR, CHAR_TRAITS, ALLOC2>() const;
    //   basic_string(const native_std::basic_string<CHAR,
    //                                               CHAR_TRAITS,
    //                                               ALLOC2>&);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "ABA",
        "ABB",
        "AAAAABAAAAAAAAA",
        0  // null string required as last element
    };

    typedef LimitAllocator<ALLOC> AltAlloc;
    typedef native_std::basic_string<TYPE, TRAITS, ALLOC> NativeObj;
    typedef native_std::basic_string<TYPE, TRAITS, AltAlloc> NativeObjAlt;

    if (verbose) printf("\tTesting npos\n");

    LOOP2_ASSERT(Obj::npos, NativeObj::npos, Obj::npos == NativeObj::npos);

    if (verbose) printf("\tTesting conversion to native string\n");

    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            const Obj U(g(U_SPEC));

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            NativeObjAlt v;
            const NativeObjAlt& V = v;

            ASSERT(! isNativeString(U));
            ASSERT(  isNativeString(V));

            v = U;  // Assignment requires implicit conversion
            const NativeObjAlt V2(U);   // implicit Conversion
            const NativeObjAlt V3 = U;  // implicit Conversion
            ASSERT(Obj(V.c_str()) == Obj(U.c_str()));

            const Obj U2(V);  // Explicit conversion construction
            ASSERT(Obj(V.c_str()) == Obj(U2.c_str()));
            ASSERT(U2 == U);

            // 'operator=='
            ASSERT(V == U);
            ASSERT(U == V);

            // 'operator!='
            ASSERT(!(V != U));
            ASSERT(!(U != V));

            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const X_SPEC = SPECS[sj];
                const Obj X(g(X_SPEC));

                if (veryVerbose) {
                    T_; T_; T_; P_(X_SPEC); P(X);
                }

                // Since free operators for the case when both arguments are
                // 'bsl::string' is thoroughly tested already, we can compare
                // the results of 'std::string < bsl::string' against
                // 'bsl::string < bsl::string', etc...

                ASSERT((U < X) == (V < X));
                ASSERT((X < U) == (X < V));
                ASSERT((U > X) == (V > X));
                ASSERT((X > U) == (X > V));

                ASSERT((U <= X) == (V <= X));
                ASSERT((X <= U) == (X <= V));
                ASSERT((U >= X) == (V >= X));
                ASSERT((X >= U) == (X >= V));

                ASSERT((U + X) == (V + X));
                ASSERT((X + U) == (X + V));
                ASSERT((U + X) == (V + X));
                ASSERT((X + U) == (X + V));
            }
        }
    }

    if (verbose) printf("\tTesting conversion from native string\n");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            const Obj U(g(U_SPEC));
            NativeObjAlt v;         const NativeObjAlt& V = v;
            v = U;

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            ASSERT(!isNativeString(U));
            ASSERT( isNativeString(V));

            Obj bslString;
            bslString = V;  // Assignment requires implicit conversion

            ASSERT(Obj(bslString.c_str()) == Obj(V.c_str()));

            const Obj U2(V);  // Explicit conversion construction
            ASSERT(Obj(V.c_str()) == Obj(U2.c_str()));
            ASSERT(U2 == U);

            // 'operator=='
            ASSERT(V == U);
            ASSERT(U == V);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase25()
{
    // --------------------------------------------------------------------
    // TESTING 'std::length_error'
    //
    // Concerns:
    //   1) That any call to a constructor, 'append', 'assign', 'insert',
    //      'operator+=', or 'replace' which would result in a value exceeding
    //      'max_size()' throws 'std::length_error'.
    //   2) That the 'max_size()' taken into consideration is that of the
    //      allocator, and not an absolute constant.
    //   3) That the value of the string is unchanged if an exception is
    //      thrown.
    //   4) That integer overflows are correctly handled when length_error
    //      exceeds 'Obj::max_size()' (which is the largest representable
    //      size_type).
    //
    // Plan:
    //   For concern 2, we use an allocator wrapper that provides the same
    //   functionality as 'ALLOC' but changes the return value of 'max_size()'
    //   to a 'limit' value settable at runtime.  Note that the allocator
    //   'max_size()' includes the null-terminating char, and so the string
    //   'max_size()' is one less than the allocator; in other words, the
    //   operations throw unless 'length <= limit - 1', i.e., they throw if
    //   'limit <= length'.
    //
    //   Construct objects with value large enough that the constructor throws.
    //   For 'append', 'assign', 'insert', 'operator+=', or 'replace', we
    //   construct a small (non-empty) object, and use the corresponding
    //   function to request an increase in size that is guaranteed to result
    //   in a value exceeding 'max_size()'.
    //
    // Testing:
    //   Proper use of 'std::length_error'
    // ------------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    LimitAllocator<ALLOC> a(&testAllocator);
    a.setMaxSize((size_t)-1);

    const size_t LENGTH = 32;
    typedef bsl::basic_string<TYPE,TRAITS,LimitAllocator<ALLOC> > LimitObj;

    LimitObj mY(LENGTH, DEFAULT_VALUE);  // does not throw
    const LimitObj& Y = mY;

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nConstructor 'string(str, pos, n, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y, 0, LENGTH, a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        } catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        } catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string(C *s, n, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y.c_str(), LENGTH, a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string(C *s, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y.c_str(), a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string(n, c, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string<Iter>(f, l, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y.begin(), Y.end(), a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'resize'.\n");
    {
        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                mX.resize(LENGTH, DEFAULT_VALUE);
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'assign'.\n");

    for (int assignMethod = 0; assignMethod <= 5; ++assignMethod) {

        if (veryVerbose) {
            switch (assignMethod) {
                case 0: printf("\tWith assign(str).\n");          break;
                case 1: printf("\tWith assign(str, pos, n).\n");  break;
                case 2: printf("\tWith assign(C *s, n).n");       break;
                case 3: printf("\tWith assign(C *s).\n");         break;
                case 4: printf("\tWith assign(n, c).n");          break;
                case 5: printf("\tWith assign<Iter>(f, l).\n");   break;
                default: ASSERT(0);
            };
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (assignMethod) {
                  case 0: mX.assign(Y);                   break;
                  case 1: mX.assign(Y, 0, LENGTH);        break;
                  case 2: mX.assign(Y.c_str(), LENGTH);   break;
                  case 3: mX.assign(Y.c_str());           break;
                  case 4: mX.assign(LENGTH, Y[0]);        break;
                  case 5: mX.assign(Y.begin(), Y.end());  break;
                  default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'operator+='.\n");

    for (int operatorMethod = 0; operatorMethod <= 2; ++operatorMethod) {

        if (veryVerbose) {
            switch (operatorMethod) {
                case 0: printf("\toperator+=(str).\n");   break;
                case 1: printf("\toperator+=(C *s).\n");  break;
                case 2: printf("\toperator+=(C c).\n");   break;
                default: ASSERT(0);
            };
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (operatorMethod) {
                  case 0: {
                    mX += Y;
                  } break;
                  case 1: {
                    mX += Y.c_str();
                  } break;
                  case 2: {
                    for (size_t i = 0; i < Y.size(); ++i) {
                        mX += Y[i];
                    }
                  } break;
                  default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'append'.\n");

    for (int appendMethod = 0; appendMethod <= 5; ++appendMethod) {

        if (verbose) {
            switch (appendMethod) {
              case 0: printf("\tWith append(str).\n");         break;
              case 1: printf("\tWith append(str, pos, n).\n"); break;
              case 2: printf("\tWith append(C *s, n).\n");     break;
              case 3: printf("\tWith append(C *s).\n");        break;
              case 4: printf("\tWith append(n, c).\n");        break;
              case 5: printf("\tWith append<Iter>(f, l).\n");  break;
              default: ASSERT(0);
            };
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (appendMethod) {
                  case 0: {
                    mX.append(Y);
                  } break;
                  case 1: {
                    mX.append(Y, 0, LENGTH);
                  } break;
                  case 2: {
                    mX.append(Y.c_str(), LENGTH);
                  } break;
                  case 3: {
                    mX.append(Y.c_str());
                  } break;
                  case 4: {
                    mX.append(LENGTH, DEFAULT_VALUE);
                  } break;
                  case 5: {
                    mX.append(Y.begin(), Y.end());
                  } break;
                  default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'insert'.\n");

    for (int insertMethod = 0; insertMethod <= 8; ++insertMethod) {

        if (verbose) {
            switch (insertMethod) {
              case 0: printf("\tWith push_back(c).\n");               break;
              case 1: printf("\tWith insert(pos, str).\n");           break;
              case 2: printf("\tWith insert(pos, str, pos2, n).\n");  break;
              case 3: printf("\tWith insert(pos, C *s, n).\n");       break;
              case 4: printf("\tWith insert(pos, C *s).\n");          break;
              case 5: printf("\tWith insert(pos, n, C c).\n");        break;
              case 6: printf("\tWith insert(p, C c).\n");             break;
              case 7: printf("\tWith insert(p, n, C c).\n");          break;
              case 8: printf("\tWith insert<Iter>(p, f, l).\n");      break;
              default: ASSERT(0);
            };
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (insertMethod) {
                  case 0: {
                    for (size_t i = 0; i < LENGTH; ++i) {
                        mX.push_back(Y[i]);
                    }
                  } break;
                  case 1: {
                    mX.insert(0, Y);
                  } break;
                  case 2: {
                    mX.insert(0, Y, 0, LENGTH);
                  } break;
                  case 3: {
                    mX.insert(0, Y.c_str(), LENGTH);
                  } break;
                  case 4: {
                    mX.insert(0, Y.c_str());
                  } break;
                  case 5: {
                    mX.insert((size_t)0, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 6: {
                    for (size_t i = 0; i < LENGTH; ++i) {
                        mX.insert(mX.begin(), DEFAULT_VALUE);
                    }
                  } break;
                  case 7: {
                    mX.insert(mX.begin(), LENGTH, DEFAULT_VALUE);
                  } break;
                  case 8: {
                    mX.insert(mX.cbegin(), Y.begin(), Y.end());
                  } break;
                  default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'replace'.\n");

    for (int replaceMethod = 1; replaceMethod < 8; ++replaceMethod) {

        if (verbose) {
            switch (replaceMethod) {
              case 1: printf("\tWith replace(pos1, n1, str).\n");
                      break;
              case 2: printf("\tWith replace(pos1, n1, str, pos2, n2).\n");
                      break;
              case 3: printf("\tWith replace(pos1, n1, C *s, n2).\n");
                      break;
              case 4: printf("\tWith replace(pos1, n1, C *s).\n");
                      break;
              case 5: printf("\tWith replace(pos1, n1, n2, C c).\n");
                      break;
              case 6: printf("\tWith replace(f, l, const C *s, n2).\n");
                      break;
              case 7: printf("\tWith replace(f, l, n, const C *s).\n");
                      break;
              case 8: printf("\treplace(f, l, n2, C c).\n");
                      break;
              default: ASSERT(0);
            };
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (replaceMethod) {
                  case 1: {
                    mX.replace(0, 1, Y);
                  } break;
                  case 2: {
                    mX.replace(0, 1, Y, 0, LENGTH);
                  } break;
                  case 3: {
                    mX.replace(0, 1, Y.c_str(), LENGTH);
                  } break;
                  case 4: {
                    mX.replace(0, 1, Y.c_str());
                  } break;
                  case 5: {
                    mX.replace(0, 1, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 6: {
                    mX.replace(mX.begin(), mX.end(), Y.c_str(), LENGTH);
                  } break;
                  case 7: {
                    mX.replace(mX.begin(), mX.end(), Y.c_str());
                  } break;
                  case 8: {
                    mX.replace(mX.begin(), mX.end(), LENGTH, DEFAULT_VALUE);
                  } break;
                  default: ASSERT(0);
                };
            } catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            } catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t \tCaught std::exception(%s).\n", e.what());
                }
            } catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    const int PADDING = 16;
    size_t expMaxSize = -1;
    const size_t& EXP_MAX_SIZE = expMaxSize;
    {
        const Obj X;
        expMaxSize = X.max_size();
    }
    ASSERTV(EXP_MAX_SIZE, (size_t)-1 > EXP_MAX_SIZE);

    if (EXP_MAX_SIZE >= (size_t)-2) {
        printf("\n\nERROR: Cannot continue this test case without attempting\n"
               "to allocate huge amounts of memory.  *** Aborting. ***\n\n");
        return;                                                       // RETURN
    }

    const size_t DATA[] = {
        EXP_MAX_SIZE + 1,
        EXP_MAX_SIZE + 2,
        (EXP_MAX_SIZE + 1) / 2 + (size_t)-1 / 2,
        (size_t)-2,
        (size_t)-1,
        0  // must be the last value
    };

    if (verbose) printf("\nConstructor 'string(n, c, a = A())'"
                        " and 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int i = 0; DATA[i]; ++i)
    {
        bool exceptionCaught = false;

        if (veryVerbose) printf("\tWith 'n' = " ZU "\n", DATA[i]);

        try {
            Obj mX(DATA[i], DEFAULT_VALUE);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'reserve/resize' and"
                        " 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int capacityMethod = 0; capacityMethod < 3; ++capacityMethod)
    {
        if (verbose) {
            switch (capacityMethod) {
              case 0: printf("\tWith reserve(n).\n");        break;
              case 1: printf("\tWith resize(n).\n");         break;
              case 2: printf("\tWith resize(n, C c).\n");    break;
              default: ASSERT(0);
            };
        }

        for (int i = 0; DATA[i]; ++i)
        {
            bool exceptionCaught = false;

            if (veryVerbose) printf("\t\tWith 'n' = " ZU "\n", DATA[i]);

            try {
                Obj mX;

                switch (capacityMethod) {
                  case 0:  mX.reserve(DATA[i]);                  break;
                  case 1:  mX.resize(DATA[i]);                   break;
                  case 2:  mX.resize(DATA[i], DEFAULT_VALUE);    break;
                  default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'append' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int appendMethod = 4; appendMethod <= 4; ++appendMethod) {

        if (verbose) {
            switch (appendMethod) {
              case 4: printf("\tWith append(n, c).\n");        break;
              default: ASSERT(0);
            };
        }

        for (int i = 0; DATA[i]; ++i) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length " ZU ".\n", DATA[i]);

            try {
                Obj mX(PADDING, DEFAULT_VALUE, a);

                mX.append(DATA[i] - PADDING, DEFAULT_VALUE);
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'insert' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int insertMethod = 5; insertMethod <= 7; insertMethod += 2) {

        if (verbose) {
            switch (insertMethod) {
              case 5: printf("\tWith insert(pos, n, C c).\n");        break;
              case 7: printf("\tWith insert(p, n, C c).\n");          break;
              default: ASSERT(0);
            };
        }

        for (int i = 0; DATA[i]; ++i) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length " ZU ".\n", DATA[i]);

            try {
                Obj mX(PADDING, DEFAULT_VALUE, a);

                const size_t LENGTH = DATA[i] - PADDING;
                switch (insertMethod) {
                  case 5: {
                    mX.insert((size_t)0, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 7: {
                    mX.insert(mX.begin(), LENGTH, DEFAULT_VALUE);
                  } break;
                  default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'replace' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int replaceMethod = 5; replaceMethod <= 8; replaceMethod += 3) {

        if (verbose) {
            switch (replaceMethod) {
              case 5: printf("\tWith replace(pos1, n1, n2, C c).\n");  break;
              case 8: printf("\tWith replace(f, l, n2, C c).\n");      break;
              default: ASSERT(0);
            };
        }

        for (int i = 0; DATA[i]; ++i) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length " ZU ".\n", DATA[i]);

            try {
                Obj mX(3 * PADDING, DEFAULT_VALUE);

                const size_t LENGTH = DATA[i] - PADDING;
                switch (replaceMethod) {
                  case 5: {
                    mX.replace(PADDING, PADDING, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 8: {
                    mX.replace(mX.begin() + PADDING,
                               mX.begin() + 2 * PADDING,
                               LENGTH,
                               DEFAULT_VALUE);
                  } break;
                  default: ASSERT(0);
                };
            } catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            } catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t \tCaught std::exception(%s).\n", e.what());
                }
            } catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase24()
{
    // --------------------------------------------------------------------
    // TESTING COMPARISONS
    //
    // Concerns:
    //   1) 'operator<' returns the lexicographic comparison on two arrays.
    //   2) 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //      'operator<'.
    //   3) 'compare' returns the correct result.
    //   4) That traits get selected properly.
    //
    // Plan:
    //   For a variety of strings of different sizes and different values, test
    //   that the comparison returns as expected.  Note that capacity is not of
    //   concern here, the implementation specifically uses only 'begin()',
    //   'end()', and 'size()'.  For concern 4, use 'TRAITS::compare()'
    //   explicitly in the 'check_compare' helper function, and check that the
    //   return value has not only the correct sign, but the same value as
    //   well.
    //
    // Testing:
    //   int  compare(const string& str) const;
    //   int  compare(pos1, n1, const string& str) const;
    //   int  compare(pos1, n1, const string& str, pos2, n2) const;
    //   int  compare(const C* s) const;
    //   int  compare(pos1, n1, const C* s) const;
    //   int  compare(pos1, n1, const C* s, n2) const;
    //   bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator<(const C *, const string<C,CT,A>&);
    //   bool operator<(const string<C,CT,A>&, const C *);
    //   bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator>(const C *, const string<C,CT,A>&);
    //   bool operator>(const string<C,CT,A>&, const C *);
    //   bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator<=(const C *, const string<C,CT,A>&);
    //   bool operator<=(const string<C,CT,A>&, const C *);
    //   bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator>=(const C *, const string<C,CT,A>&);
    //   bool operator>=(const string<C,CT,A>&, const C *);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "AAA",
        "AAAA",
        "AAAAA",
        "AAAAAA",
        "AAAAAAA",
        "AAAAAAAA",
        "AAAAAAAAA",
        "AAAAAAAAAA",
        "AAAAAAAAAAA",
        "AAAAAAAAAAAA",
        "AAAAAAAAAAAAA",
        "AAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAA",
        "AAAAAB",
        "AAAAABA",
        "AAAAABAA",
        "AAAAABAAA",
        "AAAAABAAAA",
        "AAAAABAAAAA",
        "AAAAABAAAAAA",
        "AAAAABAAAAAAA",
        "AAAAABAAAAAAAA",
        "AAAAABAAAAAAAAA",
        "AAAAB",
        "AAAABAAAAAA",
        "AAAABAAAAAAA",
        "AAAABAAAAAAAA",
        "AAAABAAAAAAAAA",
        "AAAABAAAAAAAAAA",
        "AAAB",
        "AAABA",
        "AAABAA",
        "AAABAAAAAA",
        "AAB",
        "AABA",
        "AABAA",
        "AABAAA",
        "AABAAAAAA",
        "AB",
        "ABA",
        "ABAA",
        "ABAAA",
        "ABAAAAAA",
        "B",
        "BA",
        "BAA",
        "BAAA",
        "BAAAA",
        "BAAAAA",
        "BAAAAAA",
        "BB",
        0  // null string required as last element
    };

    if (verbose) printf("\tTesting free operators <, >, <=, >=.\n");

    if (veryVerbose) printf("\tCompare each pair of similar and different"
                            " values (u, v) in S X S.\n");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            const Obj U(g(U_SPEC));

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                const Obj V(g(V_SPEC));

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                // First comparisons with 'string' objects
                const bool isLess = si < sj;
                const bool isLessEq = !(sj < si);
                LOOP2_ASSERT(si, sj,  isLess   == (U < V));
                LOOP2_ASSERT(si, sj, !isLessEq == (U > V));
                LOOP2_ASSERT(si, sj,  isLessEq == (U <= V));
                LOOP2_ASSERT(si, sj, !isLess   == (U >= V));

                // Then test comparisons with C-strings
                LOOP2_ASSERT(si, sj,  isLess   == (U.c_str() < V));
                LOOP2_ASSERT(si, sj, !isLessEq == (U.c_str() > V));
                LOOP2_ASSERT(si, sj,  isLessEq == (U.c_str() <= V));
                LOOP2_ASSERT(si, sj, !isLess   == (U.c_str() >= V));

                LOOP2_ASSERT(si, sj,  isLess   == (U < V.c_str()));
                LOOP2_ASSERT(si, sj, !isLessEq == (U > V.c_str()));
                LOOP2_ASSERT(si, sj,  isLessEq == (U <= V.c_str()));
                LOOP2_ASSERT(si, sj, !isLess   == (U >= V.c_str()));
            }
        }
    }

    if (verbose) printf("\tTesting 'compare'.\n");

    if (veryVerbose) printf("\tCompare each substring previous.\n");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];
            const size_t      U_LEN  = strlen(U_SPEC);

            const Obj U(g(U_SPEC));

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const V_SPEC = SPECS[sj];
                const size_t      V_LEN  = strlen(V_SPEC);

                const Obj V(g(V_SPEC));

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                checkCompare(U, V, U.compare(V));
                checkCompare(U, V, U.compare(V.c_str()));

                for (size_t i = 0; i <= U_LEN; ++i) {
                    for (size_t j = 0; j <= V_LEN; ++j) {
                        const Obj U1(U, i, 1);
                        const Obj UN(U, i, U_LEN);
                        const Obj V1(V, j, 1);
                        const Obj VN(V, j, V_LEN);

                        checkCompare(U1, V,  U.compare(i, 1, V));
                        checkCompare(U1, V1, U.compare(i, 1, V, j, 1));
                        checkCompare(U1, VN, U.compare(i, 1, V, j, V_LEN));

                        checkCompare(U1, V,  U.compare(i, 1, V));
                        checkCompare(U1, V1, U.compare(i, 1, V, j, 1));
                        checkCompare(U1, VN, U.compare(i, 1, V, j, V_LEN));

                        checkCompare(UN, V,  U.compare(i, U_LEN, V));
                        checkCompare(UN, V1, U.compare(i, U_LEN, V, j, 1));
                        checkCompare(UN, VN, U.compare(i, U_LEN, V, j, V_LEN));

                        checkCompare(UN, V,  U.compare(i, U_LEN, V));
                        checkCompare(UN, V1, U.compare(i, U_LEN, V, j, 1));
                        checkCompare(UN, VN, U.compare(i, U_LEN, V, j, V_LEN));
                    }
                }
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase24Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING COMPARISONS
    //
    // Concerns:
    //   1 'compare' asserts on undefined behavior when it's passed a NULL
    //   C-string pointer.
    //   2 comparison free operator overloads assert on undefined behavior when
    //   they are passed a NULL C-string pointer.
    //
    // Plan:
    //   For each 'compare' method overload, create a non-empty string and test
    //   'compare' with a NULL C-string pointer parameter.
    //
    //   For each comparison free-function overload, do the same as for
    //   'compare' method.
    //
    // Testing:
    //   int  compare(const C* s) const;
    //   int  compare(pos1, n1, const C* s) const;
    //   int  compare(pos1, n1, const C* s, n2) const;
    //   bool operator<(const C *s, const string<C,CT,A>& str);
    //   bool operator<(const string<C,CT,A>& str, const C *s);
    //   bool operator>(const C *s, const string<C,CT,A>& str);
    //   bool operator>(const string<C,CT,A>& str, const C *s);
    //   bool operator<=(const C *s, const string<C,CT,A>& str);
    //   bool operator<=(const string<C,CT,A>& str, const C *s);
    //   bool operator>=(const C *s, const string<C,CT,A>& str);
    //   bool operator>=(const string<C,CT,A>& str, const C *s);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj mX(g("ABCDE"));
    const Obj& X = mX;

    const TYPE *nullStr = NULL;
    // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
    (void) nullStr;
#endif

    if (veryVerbose) printf("\tcompare(s)\n");

    {
        ASSERT_SAFE_FAIL(X.compare(nullStr));
        ASSERT_SAFE_PASS(X.compare(X.c_str()));
    }

    if (veryVerbose) printf("\tcompare(pos1, n1, s)\n");

    {
        ASSERT_SAFE_FAIL(X.compare(0, X.size(), nullStr));
        ASSERT_SAFE_PASS(X.compare(0, X.size(), X.c_str()));
    }

    if (veryVerbose) printf("\tcompare(pos1, n1, s, n2)\n");

    {
        ASSERT_SAFE_FAIL(X.compare(0, X.size(), nullStr, X.size()));
        ASSERT_SAFE_PASS(X.compare(0, X.size(), X.c_str(), X.size()));
    }

    if (veryVerbose) printf("\toperator<\n");

    {
        ASSERT_SAFE_FAIL(if(X < nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr < X){});
        ASSERT_SAFE_PASS(if(X < X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() < X){});
    }

    if (veryVerbose) printf("\toperator>\n");

    {
        ASSERT_SAFE_FAIL(if(X > nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr > X){});
        ASSERT_SAFE_PASS(if(X > X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() > X){});
    }

    if (veryVerbose) printf("\toperator<=\n");

    {
        ASSERT_SAFE_FAIL(if(X <= nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr >= X){});
        ASSERT_SAFE_PASS(if(X <= X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() <= X){});
    }

    if (veryVerbose) printf("\toperator>=\n");

    {
        ASSERT_SAFE_FAIL(if(X >= nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr >= X){});
        ASSERT_SAFE_PASS(if(X >= X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() >= X){});
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase23()
{
    // --------------------------------------------------------------------
    // TESTING SUBSTRING
    //
    // Concerns:
    //   1) That the 'substr' and 'copy' operations have the correct behavior
    //      and return value, cases where 'n' is smaller than, equal to, or
    //      larger than 'length() - pos'.
    //   2. That 'substr' and 'copy' throw 'std::out_of_range' when passed an
    //      out-of-bound position.
    //   3) That 'copy' does not overwrite beyond the buffer boundaries.
    //
    // Plan:
    //   For a set of string values, create the substring using the already
    //   tested constructors, and compare the results to those constructed
    //   substrings, with 'n' being either 0, 1 (smaller than 'length() - pos'
    //   whenever there are remaining characters), 'length() - pos' exactly,
    //   and 'length() + 1' and 'npos'.  Leave padding on both ends for the
    //   'copy' buffer and verify that padding has not been written into.
    //
    // Testing:
    //   string substr(pos, n) const;
    //   size_type copy(char *s, n, pos = 0) const;
    // ------------------------------------------------------------------------

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const int MAX_LEN = 128;
    TYPE buffer[MAX_LEN];

    const size_t npos = Obj::npos;  // note: NPOS is a system macro

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // specifications
    } DATA[] = {
        //line  spec                                       length
        //----  ----                                       ------
        { L_,   ""                                      }, //   0
        { L_,   "A"                                     }, //   1
        { L_,   "AB"                                    }, //   2
        { L_,   "ABC"                                   }, //   3
        { L_,   "ABCD"                                  }, //   4
        { L_,   "ABCDE"                                 }, //   5
        { L_,   "ABCDEA"                                }, //   6
        { L_,   "ABCDEAB"                               }, //   7
        { L_,   "ABCDEABC"                              }, //   8
        { L_,   "ABCDEABCD"                             }, //   9
        { L_,   "ABCDEABCDEA"                           }, //  11
        { L_,   "ABCDEABCDEAB"                          }, //  12
        { L_,   "ABCDEABCDEABCD"                        }, //  14
        { L_,   "ABCDEABCDEABCDE"                       }, //  15
        { L_,   "ABCDEABCDEABCDEA"                      }, //  16
        { L_,   "ABCDEABCDEABCDEAB"                     }, //  17
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEA"       }, //  31
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"      }, //  32
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEABC"     }, //  33
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                      "ABCDEABCDEABCDEABCDEABCDEABCDEA" }, //  63
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEAB"  }, //  64
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEABC" }  //  65
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting substr(pos, n).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));

            if (veryVerbose) {
                printf("\tOn a string of length " ZU ":\t", LENGTH); P(SPEC);
            }

            for (size_t i = 0; i <= LENGTH; ++i) {
                const Obj EXP1 = Obj(X, i, 1);
                const Obj EXPN = Obj(X, i, LENGTH);

                const Obj Y0 = X.substr(i, 0);
                LOOP2_ASSERT(LINE, i, Y0.empty());

                const Obj Y1 = X.substr(i, 1);
                LOOP2_ASSERT(LINE, i, EXP1 == Y1);

                const Obj YM = X.substr(i, LENGTH - i);
                LOOP2_ASSERT(LINE, i, EXPN == YM);

                const Obj YL = X.substr(i, LENGTH + 1);
                LOOP2_ASSERT(LINE, i, EXPN == YL);

                const Obj YN = X.substr(i, npos);
                LOOP2_ASSERT(LINE, i, EXPN == YN);
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\t\tWith exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));

            bool outOfRangeCaught = false;
            try {
                const Obj Y = X.substr(LENGTH + 1, 0);
                ASSERT(0);
            }
            catch (std::out_of_range) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif

    if (verbose) printf("\tTesting copy(s, n, pos).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));
            ASSERT(LENGTH < MAX_LEN - 2);

            if (veryVerbose) {
                printf("\tOn a string of length " ZU ":\t", LENGTH); P(SPEC);
            }

            for (size_t i = 0; i <= LENGTH; ++i) {
                for (size_t j = 0; j <= LENGTH + 1; ++j) {
                    for (int k = 0; k < MAX_LEN; ++k) {
                        buffer[k] = DEFAULT_VALUE;
                    }

                    size_t ret = X.copy(buffer + 1, j, i);

                    size_t m = j < LENGTH - i ? j : LENGTH - i;
                    LOOP3_ASSERT(LINE, i, j, m == ret);

                    LOOP3_ASSERT(LINE, i, j, DEFAULT_VALUE == buffer[0]);
                    for (m = 0; m < j && m < LENGTH - i; ++m) {
                        LOOP4_ASSERT(LINE, i, j, m, buffer[1 + m] == X[i + m]);
                    }
                    LOOP3_ASSERT(LINE, i, j, ret == m);
                    LOOP3_ASSERT(LINE, i, j, DEFAULT_VALUE == buffer[1 + m]);
                }
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\t\tWith exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));

            bool outOfRangeCaught = false;
            try {
                (void) X.copy(buffer + 1, MAX_LEN - 2, LENGTH + 1);
                ASSERT(0);
            }
            catch (std::out_of_range) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase23Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING COPY:
    //
    // Concerns:
    //   1 'copy' asserts on undefined behavior when it's passed a NULL
    //   C-string pointer.
    //
    // Plan:
    //   Create a non-empty string and test 'copy' with a NULL C-string pointer
    //   parameter.
    //
    // Testing:
    //   size_type copy(char *s, n, pos = 0) const;
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tcopy(s, n, pos)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
        (void) nullStr;
#endif

        TYPE dest[10];
        ASSERT(sizeof dest / sizeof *dest > X.size());

        // characterString == NULL
        ASSERT_SAFE_FAIL(X.copy(nullStr, X.size(), 0));

        // pass
        ASSERT_SAFE_PASS(X.copy(dest, X.size(), 0));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase22()
{
    // --------------------------------------------------------------------
    // TESTING FIND VARIANTS
    //
    // Concerns:
    //   1) That the return value is correct, even in the presence of no or
    //      multiple occurrences of the search pattern.
    //   2) That passing a 'pos' argument that is out-of-bounds is not an
    //      error, and does not throw.
    //
    // Plan:
    //   For a set of carefully selected set of string and search pattern
    //   values, compare the value of 'find' and 'rfind' against expected
    //   return values.  For each string, exercise the special case of an empty
    //   pattern (the find operations always return the current position,
    //   except for 'find' when out-of-bounds: npos is returned instead).  Also
    //   exercise the special case of a single character (different signature),
    //   and verify that return value equals that of a brute-force
    //   computation.
    //
    //   For 'find_first_...' and 'find_last_...', use the Cartesian product of
    //   a set of strings and a set of search patterns, and compare the results
    //   to those of a brute-force computation.  Also exercise the special
    //   cases of a single-character pattern (different signature) and of an
    //   empty pattern (although redundant since the set of search patterns
    //   includes an empty string, this also enables us to test the correctness
    //   of our brute-force implementation in the boundary cases).
    //
    // Testing:
    //   size_type find(const string& str, pos = 0) const;
    //   size_type find(const C *s, pos, n) const;
    //   size_type find(const C *s, pos = 0) const;
    //   size_type find(C c, pos = 0) const;
    //   size_type rfind(const string& str, pos = 0) const;
    //   size_type rfind(const C *s, pos, n) const;
    //   size_type rfind(const C *s, pos = 0) const;
    //   size_type rfind(C c, pos = 0) const;
    //   size_type find_first_of(const string& str, pos = 0) const;
    //   size_type find_first_of(const C *s, pos, n) const;
    //   size_type find_first_of(const C *s, pos = 0) const;
    //   size_type find_first_of(C c, pos = 0) const;
    //   size_type find_last_of(const string& str, pos = 0) const;
    //   size_type find_last_of(const C *s, pos, n) const;
    //   size_type find_last_of(const C *s, pos = 0) const;
    //   size_type find_last_of(C c, pos = 0) const;
    //   size_type find_first_not_of(const string& str, pos = 0) const;
    //   size_type find_first_not_of(const C *s, pos, n) const;
    //   size_type find_first_not_of(const C *s, pos = 0) const;
    //   size_type find_first_not_of(C c, pos = 0) const;
    //   size_type find_last_not_of(const string& str, pos = 0) const;
    //   size_type find_last_not_of(const C *s, pos, n) const;
    //   size_type find_last_not_of(const C *s, pos = 0) const;
    //   size_type find_last_not_of(C c, pos = 0) const;
    // --------------------------------------------------------------------

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const size_t npos = Obj::npos;  // note: NPOS is a system macro

    if (verbose) printf("\nTesting 'find' and 'rfind'.\n");
    {
        static const struct {
            int         d_lineNum;
            const char *d_spec;
            const char *d_pattern;
            size_t      d_exp;
            size_t      d_rexp;
        } DATA[] = {
            //line spec         pattern                   exp           rexp
            //---- ----         -------                   ---           ----
            { L_,  "",          "A",              (size_t)-1,   (size_t)-1,  },
            { L_,  "A",         "A",              (size_t) 0,   (size_t) 0,  },
            { L_,  "A",         "B",              (size_t)-1,   (size_t)-1,  },
            { L_,  "AABAA",     "B",              (size_t) 2,   (size_t) 2,  },
            { L_,  "ABABA",     "B",              (size_t) 1,   (size_t) 3,  },
            { L_,  "BAAAB",     "B",              (size_t) 0,   (size_t) 4,  },
            { L_,  "ABCDE",     "BCD",            (size_t) 1,   (size_t) 1,  },
            { L_,  "ABABA",     "ABA",            (size_t) 0,   (size_t) 2,  },
            { L_,  "ABACABA",   "ABA",            (size_t) 0,   (size_t) 4,  },
            { L_,  "ABABABAB",  "BABAB",          (size_t) 1,   (size_t) 3,  },
            { L_,  "ABABABAB",  "C",              (size_t)-1,   (size_t)-1,  },
            { L_,  "ABABABAB",  "ABABABAC",       (size_t)-1,   (size_t)-1,  },
            { L_,  "A",         "ABABA",          (size_t)-1,   (size_t)-1,  },
            { L_,  "AABAA",     "CDCDC",          (size_t)-1,   (size_t)-1,  },

            // Add further tests below, but note that test will fail if the
            // spec has the pattern in more than two occurrences.
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE    = DATA[i].d_lineNum;
            const char* const SPEC    = DATA[i].d_spec;
            const size_t      LENGTH  = strlen(SPEC);
            const char* const PATTERN = DATA[i].d_pattern;
            const size_t      N       = strlen(PATTERN);
            const size_t      EXP     = DATA[i].d_exp;
            const size_t      REXP    = DATA[i].d_rexp;

            const Obj X(g(SPEC));

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU
                       " and empty pattern.\n",
                       SPEC,
                       LENGTH);
                printf("\t\tExpecting 'find' and 'rfind' at each position.\n");
            }

            const Obj Z;

            LOOP2_ASSERT(LINE, SPEC, 0 == X.find(Z));
            LOOP2_ASSERT(LINE, SPEC, 0 == X.find(Z.c_str()));

            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str()));

            for (size_t j = 0; j <= LENGTH; ++j) {
                LOOP3_ASSERT(LINE, SPEC, j, j == X.find(Z, j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.find(Z.c_str(), j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.find(Z.c_str(), j, 0));

                LOOP3_ASSERT(LINE, SPEC, j, j == X.rfind(Z, j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.rfind(Z.c_str(), j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.rfind(Z.c_str(), j, 0));
            }

            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z, LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z, npos));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), npos));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), LENGTH + 1, 0));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z, LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z, npos));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), npos));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), LENGTH + 1,
                                                       0));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), npos, 0));

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU
                       " and every 'char'.\n",
                       SPEC,
                       LENGTH);
                printf("\t\tComparing with values computed ad hoc.\n");
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const TYPE C = VALUES[i];

                size_t *exp  = new size_t[LENGTH + 1];
                const size_t *EXP  = exp;

                size_t *rExp = new size_t[LENGTH + 1];
                const size_t *REXP = rExp;

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : LENGTH - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH; ++k) {
                        if (TRAITS::eq(C, X[k])) {
                            exp[j] = k; break;
                        }
                    }
                    ASSERT(npos == EXP[j] || (EXP[j] < LENGTH &&
                                              C == X[EXP[j]]));
                    for (int k = static_cast<int>(lastJ); k >= 0; --k) {
                        if (TRAITS::eq(C, X[k])) {
                            rExp[j] = k; break;
                        }
                    }
                    ASSERT(npos == REXP[j] || (REXP[j] <= lastJ &&
                                               C == X[REXP[j]]));
                }

                LOOP2_ASSERT(LINE, SPEC, EXP[0]       == X.find(C));
                LOOP2_ASSERT(LINE, SPEC, REXP[LENGTH] == X.rfind(C));

                for (size_t j = 0; j <= LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, SPEC, j, EXP[j]  == X.find(C, j));
                        LOOP3_ASSERT(LINE, SPEC, j, REXP[j] == X.rfind(C, j));
                }

                LOOP2_ASSERT(LINE, SPEC, npos == X.find(C, LENGTH + 1));
                LOOP2_ASSERT(LINE, SPEC, npos == X.find(C, npos));

                LOOP2_ASSERT(LINE, SPEC, REXP[LENGTH] == X.rfind(C, LENGTH +
                                                                           1));
                LOOP2_ASSERT(LINE, SPEC, REXP[LENGTH] == X.rfind(C, npos));

                delete[] exp;
                delete[] rExp;
            }

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU
                       " and pattern \"%s\".\n",
                       SPEC,
                       LENGTH,
                       PATTERN);
                printf("\t\tExpecting 'find' at " ZU
                       " and 'rfind' at " ZU ".\n",
                       EXP,
                       REXP);
            }

            const Obj Y(g(PATTERN));

            LOOP4_ASSERT(LINE, SPEC, PATTERN, EXP,  EXP  == X.find(Y));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, EXP,  EXP  == X.find(Y.c_str()));

            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP, REXP == X.rfind(Y));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y.c_str()));

            if (EXP == npos) {
                ASSERT(EXP == REXP);
                for (size_t j = 0; j <= LENGTH + 2; ++j) {
                    LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                                 EXP == X.find(Y, j));
                    LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                                 EXP == X.find(Y.c_str(), j, N));
                }
                continue;
            }

            for (size_t j = 0; j < EXP; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.find(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.find(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.find(Y.c_str(), j, N));

                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             npos == X.rfind(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             npos == X.rfind(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             npos == X.rfind(Y.c_str(), j, N));
            }

            LOOP4_ASSERT(LINE, SPEC, PATTERN, EXP,
                         EXP  == X.find(Y, EXP));

            for (size_t j = EXP + 1; j <= REXP; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.find(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.find(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.find(Y.c_str(), j, N));
            }
            for (size_t j = EXP; j < REXP; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.rfind(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.rfind(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.rfind(Y.c_str(), j, N));
            }

            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP, REXP == X.rfind(Y, REXP));

            for (size_t j = REXP + 1; j < LENGTH + 2; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             npos == X.find(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             npos == X.find(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             npos == X.find(Y.c_str(), j, N));

                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.rfind(Y));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.rfind(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.rfind(Y.c_str(), j, N));
            }
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         npos == X.find(Y, npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         npos == X.find(Y.c_str(), npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         npos == X.find(Y.c_str(), npos, N));

            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y, npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y.c_str(), npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y.c_str(), npos, N));

        }
    }

    if (verbose) printf("\nTesting 'find_first_...' and 'find_last_...'.\n");
    {
        static const struct {
            int         d_lineNum;
            const char *d_spec;
        } DATA[] = {
            //line spec
            //---- ----
            { L_,  ""                         },
            { L_,  "A"                        },
            { L_,  "B"                        },
            { L_,  "AB"                       },
            { L_,  "ABABABABAB"               },
            { L_,  "AAAAAABBBB"               },
            { L_,  "ABCDABCD"                 },
            { L_,  "ABCDEABCDE"               },
            { L_,  "AAAABBBBCCCCDDDDEEEE"     }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct {
            int         d_lineNum;
            const char *d_pattern;
        } PATTERNS[] = {
            //line pattern
            //---- -------
            { L_,  "",            },
            { L_,  "A",           },
            { L_,  "AAA",         },
            { L_,  "AB",          },
            { L_,  "ABC",         },
            { L_,  "ABCDE",       },
            { L_,  "B",           },
            { L_,  "E",           },
            { L_,  "F",           },
            { L_,  "FGHIJKL",     }
        };
        enum { NUM_PATTERNS = sizeof PATTERNS / sizeof *PATTERNS };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE    = DATA[i].d_lineNum;
            const char* const SPEC    = DATA[i].d_spec;
            const size_t      LENGTH  = strlen(SPEC);

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU ".\n",
                       SPEC,
                       LENGTH);
            }

            const Obj X(g(SPEC));

            if (veryVerbose) {
                printf("\t\tWith empty pattern.\n");
                printf("\t\t\tExpecting 'find_.._not_of' at each position.\n");
            }

            const Obj Z;

            LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_of(Z));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_of(Z.c_str()));

            LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_of(Z));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_of(Z.c_str()));

            if (LENGTH) {
                LOOP2_ASSERT(LINE, SPEC, 0 == X.find_first_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, 0 == X.find_first_not_of(Z.c_str()));

                LOOP2_ASSERT(LINE, SPEC, LENGTH - 1 == X.find_last_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, LENGTH - 1 == X.find_last_not_of(
                                                                   Z.c_str()));
            }
            else {
                LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_not_of(
                                                                   Z.c_str()));

                LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_not_of(
                                                                   Z.c_str()));
            }

            for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_first_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_first_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_first_of(Z.c_str(), j, 0));

                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_last_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_last_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_last_of(Z.c_str(), j, 0));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_first_not_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_first_not_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_first_not_of(Z.c_str(), j, 0));

                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_last_not_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_last_not_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_last_not_of(Z.c_str(), j, 0));
            }

            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_of(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_last_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_last_of(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_not_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_not_of(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC,
                         LENGTH - 1 == X.find_last_not_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         LENGTH - 1 == X.find_last_not_of(Z.c_str(), npos, 0));

            if (veryVerbose) {
                printf("\t\tWith 'char' pattern.\n");
                printf("\t\t\tComparing with values computed ad hoc.\n");
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const TYPE C = VALUES[i];

                size_t *exp  = new size_t[LENGTH + 1];
                const size_t *EXP  = exp;

                size_t *rExp = new size_t[LENGTH + 1];
                const size_t *REXP = rExp;

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : j - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH; ++k) {
                        if (!TRAITS::eq(C, X[k])) {
                            exp[j] = k; break;
                        }
                    }
                    ASSERT(npos == EXP[j] || (j <= EXP[j] &&
                                              EXP[j] < LENGTH &&
                                              C != X[EXP[j]]));
                    for (int k = static_cast<int>(lastJ); k >= 0; --k) {
                        if (!TRAITS::eq(C, X[k])) {
                            rExp[j] = k; break;
                        }
                    }
                    ASSERT(npos == REXP[j] || (REXP[j] <= lastJ &&
                                               C != X[REXP[j]]));
                }

                LOOP2_ASSERT(LINE, SPEC, X.find(C)  == X.find_first_of(C));
                LOOP2_ASSERT(LINE, SPEC, X.rfind(C) == X.find_last_of(C));

                LOOP2_ASSERT(LINE, SPEC, EXP[0] == X.find_first_not_of(C));
                LOOP2_ASSERT(LINE, SPEC,
                             REXP[LENGTH] == X.find_last_not_of(C));

                for (size_t j = 0; j <= LENGTH + 2; ++j) {
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 X.find(C, j)  == X.find_first_of(C, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 X.rfind(C, j) == X.find_last_of(C, j));
                }

                for (size_t j = 0; j <= LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 EXP[j]  == X.find_first_not_of(C, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 REXP[j] == X.find_last_not_of(C, j));
                }

                LOOP2_ASSERT(LINE, SPEC,
                             npos == X.find_first_not_of(C, LENGTH + 1));
                LOOP2_ASSERT(LINE, SPEC,
                             REXP[LENGTH] == X.find_last_not_of(C,
                                                                LENGTH + 1));

                delete[] exp;
                delete[] rExp;
            }

            for (int k = 0; k < NUM_PATTERNS; ++k) {
                const int         PLINE   = PATTERNS[k].d_lineNum;
                const char* const PATTERN = PATTERNS[k].d_pattern;
                const size_t      N       = strlen(PATTERN);
                const Obj Y(g(PATTERN));

                if (veryVerbose) {
                    printf("\t\tWith pattern \"%s\".\n", PATTERN);
                    printf("\t\t\tComparing with values computed ad hoc.\n");
                    printf("\t\t\tFor 'find_{first,last}_of'.\n");
                }

                size_t *exp  = new size_t[LENGTH + 1];
                const size_t *EXP  = exp;

                size_t *rExp = new size_t[LENGTH + 1];
                const size_t *REXP = rExp;

                size_t *expN  = new size_t[LENGTH + 1];
                const size_t *EXP_N  = expN;

                size_t *rExpN = new size_t[LENGTH + 1];
                const size_t *REXP_N = rExpN;

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : j - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH && EXP[j] == npos; ++k) {
                        for (size_t m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                exp[j] = k; expN[j] = m; break;
                            }
                        }
                    }
                    ASSERT(npos == EXP[j] || (j <= EXP[j] && EXP[j] < LENGTH &&
                                                    Y[EXP_N[j]] == X[EXP[j]]));
                    for (int k = static_cast<int>(lastJ);
                         k >= 0 && REXP[j] == npos;
                         --k) {
                        for (size_t m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                rExp[j] = k; rExpN[j] = m; break;
                            }
                        }
                    }
                    ASSERT(npos == REXP[j] || (REXP[j] <= lastJ &&
                                                  Y[REXP_N[j]] == X[REXP[j]]));
                }

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_of(Y.c_str()));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_of(Y.c_str()));

                for (size_t j = 0; j <= LENGTH; ++j) {
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP[j] == X.find_first_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP [j]== X.find_first_of(Y.c_str(), j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP [j]== X.find_first_of(Y.c_str(), j, N));

                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_of(Y.c_str(), j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_of(Y.c_str(), j, N));
                }

                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_of(Y, npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_of(Y.c_str(), npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_of(Y.c_str(), npos, N));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_of(Y, npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_of(Y.c_str(), npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_of(Y.c_str(),
                                                            npos,
                                                            N));

                if (veryVerbose)
                    printf("\t\t\tFor 'find_{first,last}_not_of'.\n");

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : j - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH; ++k) {
                        size_t m;
                        for (m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                break;
                            }
                        }
                        if (m == N) {
                            exp[j] = k; expN[j] = m; break;
                        }
                    }
                    ASSERT(npos == EXP[j] || (j <= EXP[j] && EXP[j] < LENGTH));
                    for (int k = static_cast<int>(lastJ); k >= 0; --k) {
                        size_t m;
                        for (m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                break;
                            }
                        }
                        if (m == N) {
                            rExp[j] = k; rExpN[j] = m; break;
                        }
                    }
                    ASSERT(npos == REXP[j] || REXP[j] <= lastJ);
                }

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_not_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_not_of(Y.c_str()));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_not_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_not_of(Y.c_str()));

                for (size_t j = 0; j <= LENGTH; ++j) {
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP[j] == X.find_first_not_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP [j]== X.find_first_not_of(Y.c_str(), j));
                    LOOP6_ASSERT(
                               LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                               EXP [j]== X.find_first_not_of(Y.c_str(), j, N));

                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_not_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_not_of(Y.c_str(), j));
                    LOOP6_ASSERT(
                               LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                               REXP[j] == X.find_last_not_of(Y.c_str(), j, N));
                }

                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_not_of(Y, npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_not_of(Y.c_str(), npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_not_of(Y.c_str(), npos, N));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_not_of(Y, npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_not_of(Y.c_str(),
                                                                npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_not_of(Y.c_str(),
                                                                npos,
                                                                N));

                delete[] exp;
                delete[] expN;
                delete[] rExp;
                delete[] rExpN;
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase22Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING COPY:
    //
    // Concerns:
    //   'find' asserts on undefined behavior when it's passed a NULL C-string
    //   pointer.
    //
    // Plan:
    //   For each variant of the 'find...' method, create a non-empty string
    //   and test the 'find' method with a NULL C-string pointer parameter.
    //
    // Testing:
    //   size_type find(const C *s, pos, n) const;
    //   size_type find(const C *s, pos = 0) const;
    //   size_type rfind(const C *s, pos, n) const;
    //   size_type rfind(const C *s, pos = 0) const;
    //   size_type find_first_of(const C *s, pos, n) const;
    //   size_type find_first_of(const C *s, pos = 0) const;
    //   size_type find_last_of(const C *s, pos, n) const;
    //   size_type find_last_of(const C *s, pos = 0) const;
    //   size_type find_first_not_of(const C *s, pos, n) const;
    //   size_type find_first_not_of(const C *s, pos = 0) const;
    //   size_type find_last_not_of(const C *s, pos, n) const;
    //   size_type find_last_not_of(const C *s, pos = 0) const;
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj mX(g("ABCDE"));
    const Obj& X = mX;

    const TYPE *nullStr = NULL;
    // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
    (void) nullStr;
#endif

    if (veryVerbose) printf("\tfind(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find(nullStr, 0));
        ASSERT_SAFE_PASS(X.find(X.c_str(), 0));
    }

    if (veryVerbose) printf("\trfind(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.rfind(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.rfind(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\trfind(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.rfind(nullStr, 0));
        ASSERT_SAFE_PASS(X.rfind(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_first_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_first_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_first_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_first_of(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_last_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_last_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_last_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_last_of(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_first_not_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_not_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_first_not_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_first_not_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_not_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_first_not_of(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_last_not_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_not_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_last_not_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_last_not_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_not_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_last_not_of(X.c_str(), 0));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, TRAITS, ALLOC>::
                              testCase21_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                   OTHER_FLAGS>  StdAlloc;

    typedef bsl::basic_string<TYPE, TRAITS, StdAlloc>  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "This is very long string which must allocate",
        "This is another very long string which must allocate"
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC = SPECS[ti];

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj mW(ISPEC, scratch);  const Obj& W = mW;  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            const Obj mZ(JSPEC, scratch);  const Obj& Z = mZ;  // control

            for (int member = 0; member < 2; ++member) {
                Obj mY(ISPEC, mas);  const Obj& Y = mY;

                Obj mX(JSPEC, mat);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(JSPEC) P_(Y) P_(X) P(W) }

                ASSERTV(ISPEC, JSPEC, Z, X, Z == X);
                ASSERTV(ISPEC, JSPEC, W, Y, W == Y);

                if (member) {
                    mX.swap(mY);
                }
                else {
                    swap(mX, mY);
                }

                ASSERTV(ISPEC, JSPEC, W, X, W == X);
                ASSERTV(ISPEC, JSPEC, Z, Y, Z == Y);

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mas == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mat == Y.get_allocator());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mat == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mas == Y.get_allocator());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE, TRAITS, ALLOC>::testCase21_propagate_on_container_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_swap' trait is 'false', the
    //:   allocators used by the source and target objects remain unchanged
    //:   (i.e., the allocators are *not* exchanged).
    //:
    //: 2 If the 'propagate_on_container_swap' trait is 'true', the
    //:   allocator used by the target (source) object is updated to be a copy
    //:   of that used by the source (target) object (i.e., the allocators
    //:   *are* exchanged).
    //:
    //: 3 If the allocators are propagated (i.e., exchanged), there is no
    //:   additional allocation from any allocator.
    //:
    //: 4 The effect of the 'propagate_on_container_swap' trait is independent
    //:   of the other three allocator propagation traits.
    //:
    //: 5 Following the swap operation, neither object holds on to memory
    //:   allocated from the other object's allocator.
    //
    // Plan:
    //: 1 Specify a set S of object values with distinct values, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_swap' property configured to 'false'.  In two
    //:   successive iterations of P-3, first configure the three properties
    //:   not under test to be 'false', then configure them all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize two objects from 'x', a control object 'Z' using a
    //:     scratch allocator and an object 'X' using one of the allocators
    //:     from P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Using both member 'swap' and free function 'swap', swap 'X' with
    //:     'Y' and use 'operator==' to verify that 'X' and 'Y' have the
    //:     expected values.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocators of 'X'
    //:     and 'Y' are *not* exchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocators of 'X' and 'Y'
    //:   *are* exchanged.  Also verify that there is no additional allocation
    //:   from any allocator.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_swap
    // ------------------------------------------------------------------------

    if (verbose) printf("\t'propagate_on_container_swap::value == false'\n");

    testCase21_propagate_on_container_swap_dispatch<false, false>();
    testCase21_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\t'propagate_on_container_swap::value == true'\n");

    testCase21_propagate_on_container_swap_dispatch<true, false>();
    testCase21_propagate_on_container_swap_dispatch<true, true>();
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase21()
{
    // --------------------------------------------------------------------
    // TESTING SWAP
    //
    // Concerns:
    //   1) Swapping containers does not swap allocators.
    //   2) Swapping containers with same allocator results in no allocation
    //      or deallocation operations.
    //   3) Swapping containers with different allocators does result in
    //      allocation and deallocation operations.
    //   4) Swap free function works the same way as the 'swap' method.
    //   5) Swap works correctly for the short string optimization (swapping
    //      short and long strings).
    //
    // Plan:
    //   Construct 'str1' and 'str2' with different test allocators.
    //   Add data to 'str1'.  Remember allocation statistics.
    //   Swap 'str1' and 'str2'.
    //   Verify that contents were swapped.
    //   Verify that allocators for each are unchanged.
    //   Verify that allocation statistics changed for each test allocator.
    //   Create a 'str3' with same allocator as 'str2'.
    //   Swap 'str2' and 'str3'
    //   Verify that contents were swapped.
    //   Verify that allocation statistics did not change.
    //   Let 'str3' got out of scope.
    //   Verify that memory was returned to allocator.
    //   Construct two strings, apply a free function swap to them and verify
    //   the result.
    //   Construct short and long strings, swap them with each other and verify
    //   the result.
    //
    // Testing:
    //   swap(string<C,CT,A>& rhs);                       // method
    //   swap(string<C,CT,A>& lhs, string<C,CT,A>& rhs);  // free function
    // ------------------------------------------------------------------------

    const size_t LENGTH = DEFAULT_CAPACITY * 2;

    {
        bslma::TestAllocator testAlloc2(veryVeryVerbose);
        ASSERT(0 == testAlloc2.numBytesInUse());

        Obj str1(g(LENGTH, TYPE('0')));
        Obj str1cpy(str1);
        Obj str2(&testAlloc2);

        if (verbose) printf("Swap strings with unequal allocators.\n");

        str1.swap(str2);

        ASSERT(0  == str1.size());
        ASSERT(LENGTH == str2.size());
        ASSERT(str1cpy == str2);

        ASSERT(bslma::Default::defaultAllocator() == str1.get_allocator());
        ASSERT(&testAlloc2 == str2.get_allocator());

        const Int64 numAlloc2 = testAlloc2.numAllocations();
        const Int64 numDealloc2 = testAlloc2.numDeallocations();
        const Int64 inUse2 = testAlloc2.numBytesInUse();

        if (verbose) printf("Swap strings with equal allocators.\n");

        {
            Obj str3(&testAlloc2);
            ASSERT(testAlloc2.numBytesInUse() == inUse2);

            str3.swap(str2);
            ASSERT(str2.empty());
            ASSERT(LENGTH == str3.size());
            ASSERT(str1cpy == str3);

            ASSERT(numAlloc2 == testAlloc2.numAllocations());
            ASSERT(numDealloc2 == testAlloc2.numDeallocations());
            ASSERT(inUse2 == testAlloc2.numBytesInUse());
        }

        // Destructor for str3 should have freed memory
        ASSERT(0 == testAlloc2.numBytesInUse());
    }

    if (verbose) printf("Swap free function.\n");

    {
        Obj str1(g(LENGTH, TYPE('0')));
        Obj str1cpy(str1);

        Obj str2(g(LENGTH, TYPE('9')));
        Obj str2cpy(str2);

        using bsl::swap;
        swap(str1, str2);

        ASSERT(str1 == str2cpy);
        ASSERT(str2 == str1cpy);
    }

    if (verbose) printf("Swap and short string optimization.\n");

    {
        if (veryVerbose) printf("    short <-> short\n");

        {
            Obj shortStr1(g(DEFAULT_CAPACITY, TYPE('0')));
            Obj shortStr1Cpy(shortStr1);

            Obj shortStr2(g(DEFAULT_CAPACITY, TYPE('9')));
            Obj shortStr2Cpy(shortStr2);

            shortStr1.swap(shortStr2);

            ASSERT(shortStr1 == shortStr2Cpy);
            ASSERT(shortStr2 == shortStr1Cpy);
        }

        if (veryVerbose) printf("    short <-> long\n");

        {
            Obj shortStr(g(DEFAULT_CAPACITY, TYPE('0')));
            Obj shortStrCpy(shortStr);

            Obj longStr(g(DEFAULT_CAPACITY * 2, TYPE('9')));
            Obj longStrCpy(longStr);

            shortStr.swap(longStr);

            ASSERT(shortStr == longStrCpy);
            ASSERT(longStr == shortStrCpy);
        }

        if (veryVerbose) printf("     long <-> short\n");

        {
            Obj longStr(g(DEFAULT_CAPACITY * 2, TYPE('0')));
            Obj longStrCpy(longStr);

            Obj shortStr(g(DEFAULT_CAPACITY, TYPE('9')));
            Obj shortStrCpy(shortStr);

            longStr.swap(shortStr);

            ASSERT(longStr == shortStrCpy);
            ASSERT(shortStr == longStrCpy);
        }

        if (veryVerbose) printf("     long <-> long\n");

        {
            Obj longStr1(g(DEFAULT_CAPACITY * 2, TYPE('0')));
            Obj longStr1Cpy(longStr1);

            Obj longStr2(g(DEFAULT_CAPACITY * 2, TYPE('9')));
            Obj longStr2Cpy(longStr2);

            longStr1.swap(longStr2);

            ASSERT(longStr1 == longStr2Cpy);
            ASSERT(longStr2 == longStr1Cpy);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20()
{
    // --------------------------------------------------------------------
    // TESTING REPLACE:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the 'replace' return value is a reference to self.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved via Traits::move.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   The plan is similar to 'insert' (case 17) with two nested loops for
    //   the beginning and end of the replace range (instead of only one for
    //   the insert position).  Since both 'erase' and 'insert' have been
    //   tested, and conceptually replace is equivalent to 'erase' followed by
    //   'insert', is suffices to perform 'replace' using this alternate method
    //   and compare the resulting strings.
    //
    // Testing:
    //   string& replace(pos1, n1, size_type n2, C c);
    //   replace(const_iterator first, const_iterator last, size_type n2, C c);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        REPLACE_CHAR_MODE_FIRST    = 0,
        REPLACE_CHAR_N_AT_INDEX    = 0,
        REPLACE_CHAR_N_AT_ITERATOR = 1,
        REPLACE_CHAR_MODE_LAST     = 1
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,                  0     },
        { L_,                  1     },
        { L_,                  2     },
        { L_,                  3     },
        { L_,                  4     },
        { L_,                  5     },
        { L_,                  9     },
        { L_,   DEFAULT_CAPACITY - 1 },  // May result in duplicate test values
        { L_,   DEFAULT_CAPACITY,    },  // for wide strings.
        { L_,   DEFAULT_CAPACITY + 1 },
        { L_,   DEFAULT_CAPACITY * 5 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'replace'.\n");

    for (int replaceMode  = REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\tUsing 'n' copies of 'value', replaceMode = %d.\n",
                   replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);

                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               ALLOC(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace "); P_(SIZE);
                            printf("elements between "); P_(BEGIN); P_(END);
                            printf("using "); P_(NUM_ELEMENTS); P(VALUE);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (replaceMode) {
                          case REPLACE_CHAR_N_AT_ITERATOR: {
                            //   void replace(iterator p, iterator q,
                            //                size_type n, C c);
                            Obj& result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          case REPLACE_CHAR_N_AT_INDEX: {
                            // string& replace(pos1, n1, n2, C c);
                            Obj &result = mX.replace(BEGIN,
                                                     SIZE,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          default:
                            printf("***UNKNOWN REPLACE MODE***\n");
                            ASSERT(0);
                        };

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     LENGTH == X.size());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     CAP == X.capacity());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     BB + REALLOC == AA);
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     B + A_ALLOC ==  A);
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int replaceMode  =  REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   ALLOC(&testAllocator));  const Obj& X = mX;
                            mX.reserve(INIT_RES);

                            Obj mExp(X);  const Obj& EXP = mExp;
                            mExp.erase(mExp.begin() + BEGIN,
                                       mExp.begin() + END);
                            mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            bool checkResultFlag = false;
                            switch (replaceMode) {
                              case REPLACE_CHAR_N_AT_INDEX: {
                                // string& replace(pos1, n1, n2, C c);
                                Obj &result = mX.replace(BEGIN, SIZE,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, SIZE,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              case REPLACE_CHAR_N_AT_ITERATOR: {
                                // void replace(iterator p, iterator q,
                                //              size_type n2, C c);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, END,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              default:
                                printf("***UNKNOWN REPLACE MODE***\n");
                                ASSERT(0);
                            };
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            if (checkResultFlag) {
                                LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                             EXP == X);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20MatchTypes()
{
    // --------------------------------------------------------------------
    // TESTING REPLACE:
    // We have the following concerns:
    //   1) That the resulting string value is correct.
    //   2) That the 'replace' return value is a reference to self.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved via Traits::move.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   The plan is similar to 'insert' (case 17) with two nested loops
    //   for the beginning and end of the replace range (instead of only one
    //   for the insert position).  Since both 'erase' and 'insert' have been
    //   tested, and conceptually replace is equivalent to 'erase' followed by
    //   'insert', is suffices to perform 'replace' using this alternate method
    //   and compare the resulting strings.
    //
    // Testing:
    //   string& replace(pos1, n1, size_type n2, C c);
    //   replace(const_iterator first, const_iterator last, size_type n2, C c);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        REPLACE_CHAR_MODE_FIRST    = 0,
        REPLACE_CHAR_N_AT_INDEX    = 0,
        REPLACE_CHAR_N_AT_ITERATOR = 1,
        REPLACE_CHAR_MODE_LAST     = 1
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       18   }
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       32   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'replace'.\n");

    for (int replaceMode  = REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\tUsing 'n' copies of 'value', replaceMode = %d.\n",
                   replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = std::min(b + s, INIT_LENGTH);

                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               ALLOC(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace "); P_(SIZE);
                            printf("elements between "); P_(BEGIN); P_(END);
                            printf("using "); P_(NUM_ELEMENTS); P(VALUE);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (replaceMode) {
                          case REPLACE_CHAR_N_AT_ITERATOR: {
                            //   void replace(iterator p, iterator q,
                            //                size_type n, C c);
                            Obj& result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          case REPLACE_CHAR_N_AT_INDEX: {
                            // string& replace(pos1, n1, n2, C c);
                            Obj &result = mX.replace(BEGIN,
                                                     SIZE,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          default:
                            printf("***UNKNOWN REPLACE MODE***\n");
                            ASSERT(0);
                        };

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     LENGTH == X.size());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     CAP == X.capacity());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     BB + REALLOC == AA);
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     B + A_ALLOC ==  A);
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int replaceMode  =  REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   ALLOC(&testAllocator));  const Obj& X = mX;
                            mX.reserve(INIT_RES);

                            Obj mExp(X);  const Obj& EXP = mExp;
                            mExp.erase(mExp.begin() + BEGIN,
                                       mExp.begin() + END);
                            mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                            testAllocator.setAllocationLimit(AL);

                            bool checkResultFlag = false;
                            switch (replaceMode) {
                              case REPLACE_CHAR_N_AT_INDEX: {
                                // string& replace(pos1, n1, n2, C c);
                                Obj &result = mX.replace(BEGIN, SIZE,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, SIZE,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              case REPLACE_CHAR_N_AT_ITERATOR: {
                                // void replace(iterator p, iterator q,
                                //              size_type n2, C c);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, END,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              default:
                                printf("***UNKNOWN REPLACE MODE***\n");
                                ASSERT(0);
                            };

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            if (checkResultFlag) {
                                LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                             EXP == X);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING REPLACE:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the return value is a reference to self.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly so that
    //      *all* internally allocated memory draws from a user-supplied
    //      allocator whenever one is specified.
    //
    // Plan:
    //   See 'testCase20'.
    //
    // Testing:
    //   string& replace(pos1, n1, const string& str);
    //   string& replace(pos1, n1, const string& str, pos2, n2);
    //   string& replace(pos1, n1, const C *s, n2);
    //   string& replace(pos1, n1, const C *s);
    //   replace(const_iterator first, const_iterator last, const C *s, n2);
    //   replace(const_iterator first, const_iterator last, const C *s);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    enum {
        REPLACE_STRING_MODE_FIRST        = 0,
        REPLACE_SUBSTRING_AT_INDEX       = 0,
        REPLACE_STRING_AT_INDEX          = 1,
        REPLACE_CSTRING_N_AT_INDEX       = 2,
        REPLACE_CSTRING_AT_INDEX         = 3,
        REPLACE_STRING_AT_ITERATOR       = 4,
        REPLACE_CONST_STRING_AT_ITERATOR = 5,
        REPLACE_CSTRING_N_AT_ITERATOR    = 6,
        REPLACE_CSTRING_AT_ITERATOR      = 7,
        REPLACE_STRING_MODE_LAST         = 7
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   }
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int replaceMode  = REPLACE_STRING_AT_INDEX;
             replaceMode <= REPLACE_STRING_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace "); P_(NUM_ELEMENTS);
                            printf("between "); P_(BEGIN); P_(END);
                            printf("using "); P(SPEC);
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, Y);

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch(replaceMode) {
                          case REPLACE_STRING_AT_INDEX: {
                            // string& replace(pos1, n1, const string& str);
                            Obj &result = mX.replace(BEGIN, SIZE, Y);
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_N_AT_INDEX: {
                            // string& replace(pos1, n1, const C *s, n2);
                            Obj &result = mX.replace(BEGIN,
                                                     SIZE,
                                                     Y.data(),
                                                     NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_AT_INDEX: {
                            // string& replace(pos1, n1, const C *s);
                            Obj &result = mX.replace(BEGIN, SIZE, Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_N_AT_ITERATOR: {
                            // string& replace(iterator p, q, const C *s);
                            Obj &result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     Y.data(),
                                                     NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_AT_ITERATOR: {
                            // string& replace(iterator p, q, const C *s);
                            Obj &result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            //   void replace(iterator p, iterator q,
                            //                InputIter first, last);
                            mX.replace(mX.begin() + BEGIN,
                                       mX.begin() + END,
                                       mU.begin(),
                                       mU.end());
                          } break;
                          case REPLACE_CONST_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            //   void replace(iterator p, iterator q,
                            //                InputIter first, last);
                            mX.replace(mX.begin() + BEGIN,
                                       mX.begin() + END,
                                       U.begin(),
                                       U.end());
                          } break;
                          default:
                            printf("***UNKNOWN REPLACE MODE***\n");
                            ASSERT(0);
                        };

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                         CAP == X.capacity());
                        }
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, BEGIN, END, SIZE,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, ti,
                                     B + A_ALLOC <=  A);
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with replaceMode = %d.\n",
                        REPLACE_SUBSTRING_AT_INDEX);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);
                        const size_t POS2 = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS2;
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS_INS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, Y, POS2, NUM_ELEMENTS);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace"); P_(NUM_ELEMENTS_INS);
                            printf("between "); P_(BEGIN); P_(END);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS2);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& replace(pos1, n1, const string& str,
                        //                 pos2, n2);
                        Obj &result = mX.replace(BEGIN, SIZE,
                                                 Y, POS2, NUM_ELEMENTS);
                        ASSERT(&result == &mX);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, POS2,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, POS2,
                                         CAP == X.capacity());
                        }
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                     BEGIN, END, BB + REALLOC <= AA);
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                     BEGIN, END,  B + A_ALLOC <=  A);
                    }
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int replaceMode  = REPLACE_STRING_MODE_FIRST;
             replaceMode <= REPLACE_STRING_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN  = b;
                        const size_t SIZE   = s;
                        const size_t END    = min(b + s, INIT_LENGTH);

                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);

                            Obj mExp(X);  const Obj& EXP = mExp;
                            mExp.erase(mExp.begin() + BEGIN,
                                       mExp.begin() + END);
                            mExp.insert(BEGIN, Y);

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            switch(replaceMode) {
                              case REPLACE_STRING_AT_INDEX: {
                                // string& replace(pos1, n1,
                                //                 const string& str);
                                Obj &result = mX.replace(BEGIN, SIZE, Y);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_SUBSTRING_AT_INDEX: {
                                // string& replace(pos1, n1, const string& str,
                                //                 pos2, n2);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y,
                                                         0,
                                                         NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_N_AT_INDEX: {
                                // string& replace(pos1, n1, const C *s, n2);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y.data(),
                                                         NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_AT_INDEX: {
                                // string& replace(pos1, n1, const C *s);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y.c_str());
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_N_AT_ITERATOR: {
                                // replace(const_iterator p, q, const C *s);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         Y.data(),
                                                         NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_AT_ITERATOR: {
                                // replace(const_iterator p, q, const C *s);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         Y.c_str());
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_STRING_AT_ITERATOR: {
                                // template <class InputIter>
                                // replace(const_iterator p, const_iterator q,
                                //         InputIter first, last);
                                mX.replace(mX.begin() + BEGIN,
                                           mX.begin() + END,
                                           mU.begin(),
                                           mU.end());
                              } break;
                              case REPLACE_CONST_STRING_AT_ITERATOR: {
                                // template <class InputIter>
                                // replace(const_iterator p, const_iterator q,
                                //         InputIter first, last);
                                mX.replace(mX.begin() + BEGIN,
                                           mX.begin() + END,
                                           U.begin(),
                                           U.end());
                              } break;
                              default:
                                printf("***UNKNOWN REPLACE MODE***\n");
                                ASSERT(0);
                            };
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                         LENGTH == X.size());
                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                         EXP == X);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int replaceMode  = REPLACE_STRING_MODE_FIRST;
             replaceMode <= REPLACE_STRING_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                    const size_t BEGIN = b;
                    const size_t SIZE  = s;
                    const size_t END   = min(b + s, INIT_LENGTH);

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    for (size_t k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] = VALUES[k % NUM_VALUES];
                    }

                    Obj mY(X); const Obj& Y = mY;  // control

                    if (veryVerbose) {
                        printf("\t\t\tReplace with "); P_(Y);
                        printf(" between "); P_(BEGIN); P_(END);
                    }

                    switch(replaceMode) {
                      case REPLACE_STRING_AT_INDEX: {
                        // string& replace(pos1, n1, const string& str);
                        mX.replace(BEGIN, SIZE, Y);
                        mY.replace(BEGIN, SIZE, Y);
                      } break;
                      case REPLACE_CSTRING_N_AT_INDEX: {
                        // string& replace(pos1, n1, const C *s, n);
                        mX.replace(BEGIN, SIZE, Y.data(), INIT_LENGTH);
                        mY.replace(BEGIN, SIZE, Y.data(), INIT_LENGTH);
                      } break;
                      case REPLACE_CSTRING_AT_INDEX: {
                        // string& replace(pos1, n1, const C *s);
                        mX.replace(BEGIN, SIZE, Y.c_str());
                        mY.replace(BEGIN, SIZE, Y.c_str());
                      } break;
                      case REPLACE_SUBSTRING_AT_INDEX: {
                        // string& replace(pos1, n1, const string& str,
                        //                 pos2, n);
                        mX.replace(BEGIN, SIZE, Y, 0, INIT_LENGTH);
                        mY.replace(BEGIN, SIZE, Y, 0, INIT_LENGTH);
                      } break;
                      case REPLACE_STRING_AT_ITERATOR: {
                        // replace(const_iterator p, q, InputIter first, last);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   mY.begin(), mY.end());
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   mY.begin(), mY.end());
                      } break;
                      case REPLACE_CONST_STRING_AT_ITERATOR: {
                        // replace(const_iterator p, q, InputIter first, last);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   Y.begin(), Y.end());
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   Y.begin(), Y.end());
                      } break;
                      case REPLACE_CSTRING_N_AT_ITERATOR: {
                        // replace(const_iterator p, q, const C *s, n);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   Y.data(), INIT_LENGTH);
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   Y.data(), INIT_LENGTH);
                      } break;
                      case REPLACE_CSTRING_AT_ITERATOR: {
                        // string& replace(pos, const C *s);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   Y.c_str());
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   Y.c_str());
                      } break;
                      default:
                        printf("***UNKNOWN REPLACE MODE***\n");
                        ASSERT(0);
                    };

                    if (veryVerbose) {
                        T_; T_; T_; T_; P(X);
                        T_; T_; T_; T_; P(Y);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_CAP, BEGIN, SIZE, X == Y);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d (complete).\n",
                   REPLACE_SUBSTRING_AT_INDEX);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                    const size_t BEGIN = b;
                    const size_t SIZE  = s;
                    const size_t END   = min(b + s, INIT_LENGTH);

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        for (size_t m = 0; m < INIT_LENGTH; ++m) {
                            const size_t INDEX        = h;
                            const size_t NUM_ELEMENTS = m;

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            for (size_t k = 0; k < INIT_LENGTH; ++k) {
                                mX[k] = VALUES[k % NUM_VALUES];
                            }

                            Obj mY(X); const Obj& Y = mY;  // control

                            if (veryVerbose) {
                                printf("\t\t\tInsert substring of itself");
                                printf(" with "); P_(INDEX); P(NUM_ELEMENTS);
                                printf("between "); P_(BEGIN); P(END);
                            }

                            mX.replace(BEGIN, SIZE, Y, INDEX, NUM_ELEMENTS);
                            mY.replace(BEGIN, SIZE, Y, INDEX, NUM_ELEMENTS);

                            if (veryVerbose) {
                                T_; T_; T_; T_; P(X);
                                T_; T_; T_; T_; P(Y);
                            }

                            LOOP6_ASSERT(INIT_LINE, INIT_CAP, BEGIN, SIZE,
                                         INDEX, NUM_ELEMENTS, X == Y);
                        }
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING REPLACE:
    //
    // Concerns:
    //   1 'replace' asserts on undefined behavior when it's passed either a
    //   NULL C-string pointer, invalid iterators, or invalid iterator ranges.
    //
    // Plan:
    //   For each 'replace' overload create a non-empty string and test
    //   'replace' with different combinations of invalid parameters.
    //
    // Testing:
    //   replace(const_iterator first, const_iterator last, size_type n2, C c);
    //   replace(const_iterator first, const_iterator last, const string& str);
    //   replace(const_iterator first, const_iterator last, InputIter f, l);
    //   replace(pos1, n1, const C *s);
    //   replace(pos1, n1, const C *s, n2);
    //   replace(const_iterator first, const_iterator last, const C *s);
    //   replace(const_iterator first, const_iterator last, const C *s, n2);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\treplase(first, last, n, c)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(), 1, X[0]));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(), 1, X[0]));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1, 1, X[0]));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1, 1, X[0]));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(), 1, X[0]));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), 1, X[0]));
    }

    if (veryVerbose) printf("\treplace(first, last, str)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        Obj mY(g("AB"));    // replacement

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(), mY));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(), mY));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1, mY));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1, mY));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(), mY));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), mY));
    }

    if (veryVerbose) printf("\treplace(first, last, f, l)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        Obj mY(g("AB"));    // replacement
        const Obj& Y = mY;

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(),
                                    mY.begin(), mY.end()));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(),
                                    mY.begin(), mY.end()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1,
                                    mY.begin(), mY.end()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1,
                                    mY.begin(), mY.end()));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(),
                                    mY.begin(), mY.end()));

        // stringFirst > stringLast (non-const iterators)
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(),
                                    mY.end(), mY.begin()));

        // stringFirst > stringLast (const iterators)
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(),
                                    Y.end(), Y.begin()));

        // pass (non-const iterators)
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(),
                                    mY.begin(), mY.end()));

        // pass (const iterators)
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(),
                                    Y.begin(), Y.end()));
    }

    if (veryVerbose) printf("\treplace(pos1, n1, s)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(0, X.size(), NULL));

        // pass
        ASSERT_SAFE_PASS(mX.replace(0, X.size(), X.c_str()));
    }

    if (veryVerbose) printf("\treplace(pos1, n1, s, n2)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        const TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
        (void) nullStr;
#endif

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(0, X.size(), nullStr, 10));

        // pass
        ASSERT_SAFE_PASS(mX.replace(0, X.size(), X.c_str(), X.size()));
    }

    if (veryVerbose) printf("\treplace(first, last, s)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        const TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
       (void) nullStr;
#endif

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(), X.c_str()));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(), X.c_str()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1, X.c_str()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1, X.c_str()));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(), X.c_str()));

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(), nullStr));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), X.c_str()));
    }

    if (veryVerbose) printf("\treplace(first, last, s, n2)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        const TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
        (void) nullStr;
#endif

        // first < begin()
        ASSERT_SAFE_FAIL(
                      mX.replace(X.begin() - 1, X.end(), X.c_str(), X.size()));

        // first > end()
        ASSERT_SAFE_FAIL(
                        mX.replace(X.end() + 1, X.end(), X.c_str(), X.size()));

        // last < begin()
        ASSERT_SAFE_FAIL(
                    mX.replace(X.begin(), X.begin() - 1, X.c_str(), X.size()));

        // last > end()
        ASSERT_SAFE_FAIL(
                      mX.replace(X.begin(), X.end() + 1, X.c_str(), X.size()));

        // first > last
        ASSERT_SAFE_FAIL(
                    mX.replace(X.begin() + 1, X.begin(), X.c_str(), X.size()));

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(), nullStr, X.size()));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), X.c_str(), X.size()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase19()
{
    // --------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concerns:
    //   1) That the resulting value is correct.
    //   2) That erasing a suffix of the array never allocates, and thus never
    //      throws.  In particular, 'pop_back()' and 'erase(..., X.end())' do
    //      not throw.
    //   3) That erasing is exception neutral w.r.t. memory allocation.
    //   4) That erasing does not modify the capacity (i.e., shrink).
    //   5) That no memory is leaked.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a string
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but not computing the number of allocations or checking on the
    //        value in case an exception is thrown (it is enough to verify that
    //        all the elements have been destroyed indirectly by making sure
    //        that there are no memory leaks).
    //   For concern 2, we verify that the number of allocations is as
    //   expected:
    //      - length of the tail (last element erased to last element) if the
    //        type uses a 'bslma' allocator, and is not moveable.
    //      - 0 otherwise.
    //
    // Testing:
    //   void pop_back();
    //   string& erase(size_type pos, size_type n);
    //   iterator erase(const_iterator p);
    //   iterator erase(const_iterator first, iterator last);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'pop_back' on non-empty strings.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH       = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                const Obj& X = mX;
                mX.reserve(INIT_RES);
                const size_t INIT_CAP = X.capacity();

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] =  VALUES[k % NUM_VALUES];
                }

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBefore: "); P_(BB); P(B);
                }

                mX.pop_back();

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAfter : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());
                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             INIT_CAP == X.capacity());

                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, k,
                                 VALUES[k % NUM_VALUES] == X[k]);
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BB == AA);
                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,  B ==  A);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const size_t LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t CAP = DATA[l].d_length;
                ASSERT(LENGTH <= CAP);

                Obj mX(LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                mX.reserve(CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(LENGTH); P(CAP);
                }

                bool exceptionCaught = false;
                try {
                    mX.pop_back();
                }
                catch (...) {
                    exceptionCaught = true;
                }
                LOOP_ASSERT(LINE, !exceptionCaught);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
#endif

    if (verbose) printf("\nTesting 'erase(pos, n)'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <=  INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH + 2; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k >= INIT_LENGTH
                                              ? INIT_LENGTH : k;
                    const size_t NUM_ELEMENTS = k - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH -
                                                         (END_POS - BEGIN_POS);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const size_t CAPACITY = X.capacity();

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    Obj *result = &mX.erase(BEGIN_POS, NUM_ELEMENTS);
                                                             // test erase here

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BEGIN_POS,
                                 NUM_ELEMENTS, result == &mX);
                    LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BEGIN_POS,
                                 NUM_ELEMENTS, LENGTH == X.size());
                    LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BEGIN_POS,
                                 NUM_ELEMENTS, CAPACITY == X.capacity());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 BB + 0 == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 B + 0 ==  A);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k >= INIT_LENGTH
                                              ? INIT_LENGTH : k;
                    const size_t NUM_ELEMENTS = k - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH -
                                                         (END_POS - BEGIN_POS);

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        Obj *result = &mX.erase(BEGIN_POS, NUM_ELEMENTS);
                                                             // test erase here
                        guard.release();
                        ASSERT(&X == result);

                        for (m = 0; m < BEGIN_POS; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                                               VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
    if (verbose) printf("\nTesting 'erase(pos)' on non-empty strings.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase one element at "); P(POS);
                    }

                    const size_t CAPACITY = X.capacity();

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore: "); P_(BB); P(B);
                    }

                    typename Obj::iterator result = mX.erase(mX.begin() + POS);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tAfter : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 result == mX.begin() + POS);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 CAPACITY == X.capacity());

                    for (m = 0; m < POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[(m + 1) % NUM_VALUES] == X[m]);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 BB + 0 == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 B + 0 ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        mX.erase(mX.begin() + POS);  // test erase here
                        guard.release();

                        for (m = 0; m < POS; ++m) {
                            LOOP5_ASSERT(
                                       INIT_LINE, INIT_LENGTH, INIT_CAP,
                                       POS, m, VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(
                                 INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 POS, m, VALUES[(m + 1) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const size_t NUM_ELEMENTS = END_POS - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const size_t CAPACITY = X.capacity();

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    typename Obj::iterator result = mX.erase(
                                                        mX.begin() + BEGIN_POS,
                                                        mX.begin() + END_POS);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(
                               INIT_LINE, INIT_LENGTH, INIT_CAP,
                               NUM_ELEMENTS, result == mX.begin() + BEGIN_POS);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, CAPACITY == X.capacity());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 BB + 0 == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 B + 0 ==  A);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const size_t NUM_ELEMENTS = END_POS - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        mX.erase(mX.begin() + BEGIN_POS, mX.begin() + END_POS);
                                                             // test erase here
                        guard.release();

                        for (m = 0; m < BEGIN_POS; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                                               VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase19Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ERASE
    //
    // Concerns:
    //   1 'pop_back' asserts on undefined behavior when the string is empty,
    //   2 'erase' asserts on undefined behavior when iterators are not valid
    //   on the string being tested or they don't make a valid range.
    //
    // Plan:
    //   For concern (1), create an empty string and call 'pop_back' which
    //   should assert.  For concern (2), create a non-empty string and test
    //   'erase' with different combinations of invalid iterators and iterator
    //   ranges.
    //
    // Testing:
    //   void pop_back();
    //   iterator erase(const_iterator p);
    //   iterator erase(const_iterator first, iterator last);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tnegative testing pop_back\n");

    {
        Obj mX;

        // pop_back on empty string
        ASSERT_SAFE_FAIL(mX.pop_back());
    }

    if (veryVerbose) printf("\tnegative testing erase(iterator)\n");

    {
        Obj mX(g("ABCDE"));

        // position < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() - 1));

        // position >= end()
        ASSERT_SAFE_FAIL(mX.erase(mX.end()));
        ASSERT_SAFE_FAIL(mX.erase(mX.end() + 1));
    }

    if (veryVerbose) printf("\tnegative testing erase(iterator, iterator)\n");

    {
        Obj mX(g("ABCDE"));

        // first < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() - 1, mX.end()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin(), mX.end() + 1));

        // first > last
        ASSERT_SAFE_FAIL(mX.erase(mX.end(), mX.begin()));
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() + 1, mX.begin()));
        ASSERT_SAFE_FAIL(mX.erase(mX.end(), mX.end() - 1));

        // first > end()
        ASSERT_SAFE_FAIL(mX.erase(mX.end() + 1, mX.end()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin(), mX.begin() - 1));
    }

    if (veryVerbose) {
        printf("\tnow try some valid parameters for pop_back/erase\n");
    }

    {
        Obj mX(g("ABCDE"));
        ASSERT_SAFE_PASS(mX.pop_back());
        ASSERT_SAFE_PASS(mX.erase(mX.begin()));
        ASSERT_SAFE_PASS(mX.erase(mX.begin(), mX.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase18()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the 'insert' return (if any) value is a reference to self,
    //      or a valid iterator, even when the string underwent a reallocation.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved via Traits::move.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes and capacities
    //   containing default values, and insert a distinct 'value' at various
    //   positions, or a variable number of copies of this value.  Perform the
    //   above tests:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   basic_string& insert(size_type pos, size_type n, C c);
    //   iterator insert(const_iterator p, size_type n, C c);
    //   iterator insert(const_iterator p, C c);
    //   // string& insert(size_type pos, const C *s, n2);
    //   // string& insert(size_type pos, const C *s);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        INSERT_CHAR_MODE_FIRST    = 0,
        INSERT_CHAR_N_AT_INDEX    = 0,
        INSERT_CHAR_N_AT_ITERATOR = 1,
        INSERT_CHAR_AT_ITERATOR   = 2,
        INSERT_CHAR_MODE_LAST     = 2
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   }
#else
        // This portion of the test would be appropriate if we performed
        // short-string optimization, for which the boundary sizes would differ
        // in 32 and 64-bit modes, but we do not in this version.

        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'insert'.\n");

    if (verbose) printf("\tUsing a single 'value'.\n");

    for (int insertMode  = INSERT_CHAR_AT_ITERATOR;
             insertMode <= INSERT_CHAR_MODE_LAST;
             ++insertMode)
    {
        if (verbose)
            printf("\t\tUsing string with insertMode = %d.\n", insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    switch (insertMode) {
                      case INSERT_CHAR_AT_ITERATOR: {
                        //   iterator insert(iterator p, C c);
                        iterator result = mX.insert(mX.begin() + POS, VALUE);
                        LOOP3_ASSERT(INIT_LINE, i, j,
                                     X.begin() + POS == result);
                      } break;
                      default:
                        printf("***UNKNOWN INSERT MODE***\n");
                        ASSERT(0);
                    };

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
                    LOOP3_ASSERT(INIT_LINE, i, j, CAP == X.capacity());

                    for (k = 0; k < POS; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    LOOP3_ASSERT(INIT_LINE, LENGTH, POS, VALUE == X[POS]);
                    for (++k; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[(k - 1) % NUM_VALUES] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                 BB + REALLOC == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                 B + A_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    for (int insertMode = INSERT_CHAR_MODE_FIRST;
             insertMode < INSERT_CHAR_AT_ITERATOR;
         ++insertMode)
    {
        if (verbose)
            printf("\tUsing 'n' copies of 'value', insertMode = %d.\n",
                   insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(VALUE);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (insertMode) {
                          case INSERT_CHAR_N_AT_ITERATOR: {
                            // iterator insert(const_iterator, size_type, C);
                            iterator result = mX.insert(X.begin() + POS,
                                                        NUM_ELEMENTS,
                                                        VALUE);
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         X.begin() + POS == result);
                          } break;
                          case INSERT_CHAR_N_AT_INDEX: {
                            // string& insert(pos, n, C c);
                            Obj &result = mX.insert(POS, NUM_ELEMENTS, VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, j, &X == &result);
                          } break;
                          default:
                            printf("***UNKNOWN INSERT MODE***\n");
                            ASSERT(0);
                        };

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     CAP == X.capacity());

                        size_t m = 0;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (; k < POS + NUM_ELEMENTS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUE == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + REALLOC == AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     B + A_ALLOC ==  A);
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int insertMode  = INSERT_CHAR_MODE_FIRST;
             insertMode <= INSERT_CHAR_MODE_LAST;
         ++insertMode)
    {
        if (verbose)
            printf("\t\tUsing string with insertMode = %d.\n", insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            const size_t CAP = computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            bool checkResultFlag = false;
                            switch (insertMode) {
                              case INSERT_CHAR_N_AT_INDEX: {
                                // string& insert(pos, n, C c);
                                Obj &result = mX.insert(POS,
                                                        NUM_ELEMENTS,
                                                        VALUE);
                                LOOP3_ASSERT(INIT_LINE, i, j, &X == &result);
                                checkResultFlag = true;
                              } break;
                              case INSERT_CHAR_N_AT_ITERATOR: {
                                // iterator insert(const_iterator p,
                                //                 size_type n, C c);
                                mX.insert(mX.begin() + POS,
                                          NUM_ELEMENTS,
                                          VALUE);
                                checkResultFlag = true;
                              } break;
                              case INSERT_CHAR_AT_ITERATOR: {
                                if (NUM_ELEMENTS != 1) {
                                    break;
                                }
                                //   iterator insert(iterator p, C c);
                                iterator result =
                                            mX.insert(mX.begin() + POS, VALUE);
                                LOOP3_ASSERT(INIT_LINE, i, j,
                                             X.begin() + POS == result);
                                checkResultFlag = true;
                              } break;
                              default:
                                printf("***UNKNOWN INSERT MODE***\n");
                                ASSERT(0);
                            };
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            if (checkResultFlag) {
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             LENGTH == X.size());
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             CAP == X.capacity());

                                size_t k;
                                for (k = 0; k < POS; ++k) {
                                    LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                                 DEFAULT_VALUE == X[k]);
                                }
                                for (; k < POS + NUM_ELEMENTS; ++k) {
                                    LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                                 VALUE == X[k]);
                                }
                                for (; k < LENGTH; ++k) {
                                    LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                                 DEFAULT_VALUE == X[k]);
                                }
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase18Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the initial range is correctly imported and then moved if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes with different
    //   'value' as argument.  Perform the above tests:
    //      - From the parameterized 'CONTAINER::const_iterator'.
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there is a change in capacity, 0 otherwise
    //      - 1 if the  type uses an allocator and the value is an alias.
    //      -
    //   For concern 4, we test with a bitwise-moveable type that the only
    //   reallocations are for the new elements plus one if the string
    //   undergoes a reallocation (capacity changes).
    //
    // Testing:
    //   template <class InputIter>
    //   iterator insert(const_iterator p, InputIter first, InputIter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                     typename bsl::iterator_traits<
                      typename CONTAINER::const_iterator>::iterator_category
                   >::value;

    enum {
        INSERT_STRING_MODE_FIRST        = 0,
        INSERT_SUBSTRING_AT_INDEX       = 0,
        INSERT_STRING_AT_INDEX          = 1,
        INSERT_CSTRING_N_AT_INDEX       = 2,
        INSERT_CSTRING_AT_INDEX         = 3,
        INSERT_STRING_AT_ITERATOR       = 4,
        INSERT_STRING_AT_CONST_ITERATOR = 5,
        INSERT_STRING_MODE_LAST         = 5
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int insertMode  = INSERT_STRING_AT_INDEX;
             insertMode <= INSERT_STRING_MODE_LAST;
             ++insertMode)
    {
        if (verbose)
            printf("\tUsing string with insertMode = %d.\n", insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch(insertMode) {
                          case INSERT_STRING_AT_INDEX: {
                            // string& insert(pos1, const string<C,CT,A>& str);
                            Obj &result = mX.insert(POS, Y);
                            ASSERT(&result == &mX);
                          } break;
                          case INSERT_CSTRING_N_AT_INDEX: {
                            // string& insert(pos, const C *s, n);
                            Obj &result = mX.insert(POS,
                                                    Y.data(),
                                                    NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case INSERT_CSTRING_AT_INDEX: {
                            // string& insert(pos, const C *s);
                            Obj &result = mX.insert(POS, Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case INSERT_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                            mX.insert(mX.cbegin() + POS, mU.begin(), mU.end());
                          } break;
                          case INSERT_STRING_AT_CONST_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                            mX.insert(mX.cbegin() + POS, U.begin(), U.end());
                          } break;
                          default:
                            printf("***UNKNOWN INSERT MODE***\n");
                            ASSERT(0);
                        };

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         CAP == X.capacity());
                        }

                        size_t m = 0;
                        for (k = 0; k < POS; ++k, ++m) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }
                        for (m = 0; k < POS + NUM_ELEMENTS; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         Y[m] == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     B + A_ALLOC <=  A);
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with insertMode = %d.\n",
                        INSERT_SUBSTRING_AT_INDEX);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t POS  = j;
                        const size_t POS2 = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS2;
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS_INS;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS_INS);
                            printf("at "); P_(POS);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS2);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& insert(pos1, const string<C,CT,A>& str,
                        //                pos2, n);
                        Obj &result = mX.insert(POS, Y, POS2, NUM_ELEMENTS);
                        ASSERT(&result == &mX);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         CAP == X.capacity());
                        }

                        size_t m;
                        for (n = 0; n < POS; ++n) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, n,
                                         VALUES[n % NUM_VALUES] == X[n]);
                        }
                        for (m = 0; m < NUM_ELEMENTS_INS; ++m, ++n) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, m, n,
                                         Y[POS2 + m] == X[n]);
                        }
                        for (m = POS; n < LENGTH; ++m, ++n) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, m, n,
                                         VALUES[m % NUM_VALUES] == X[n]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     B + A_ALLOC <=  A);
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int insertMode  = INSERT_STRING_MODE_FIRST;
             insertMode <= INSERT_STRING_MODE_LAST;
             ++insertMode)
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            const size_t CAP = computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            switch(insertMode) {
                              case INSERT_STRING_AT_INDEX: {
                            // string& insert(pos1, const string<C,CT,A>& str);
                                Obj &result = mX.insert(POS, Y);
                                ASSERT(&result == &mX);
                              } break;
                              case INSERT_CSTRING_N_AT_INDEX: {
                            // string& insert(pos, const C *s, n);
                                Obj &result = mX.insert(POS,
                                                        Y.data(),
                                                        NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case INSERT_CSTRING_AT_INDEX: {
                            // string& insert(pos, const C *s);
                                Obj &result = mX.insert(POS, Y.c_str());
                                ASSERT(&result == &mX);
                              } break;
                              case INSERT_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                                mX.insert(mX.cbegin() + POS,
                                          mU.begin(),
                                          mU.end());
                              } break;
                              case INSERT_STRING_AT_CONST_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                                mX.insert(mX.cbegin() + POS,
                                          U.begin(),
                                          U.end());
                              } break;
                              case INSERT_SUBSTRING_AT_INDEX: {
                            // string& insert(pos1, const string<C,CT,A>& str,
                            //                pos2, n);
                                mX.insert(POS, Y, 0, NUM_ELEMENTS);
                              } break;
                              default:
                                printf("***UNKNOWN INSERT MODE***\n");
                                ASSERT(0);
                            };
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());
                            if (!INPUT_ITERATOR_TAG) {
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             CAP == X.capacity());
                            }

                            size_t k, m;
                            for (k = 0; k < POS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                            for (m = 0; m < NUM_ELEMENTS; ++k, ++m) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             Y[m] == X[k]);
                            }
                            for (m = POS; k < LENGTH; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int insertMode  = INSERT_STRING_MODE_FIRST;
             insertMode <= INSERT_STRING_MODE_LAST;
             ++insertMode)
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    for (size_t k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] = VALUES[k % NUM_VALUES];
                    }

                    Obj mY(X); const Obj& Y = mY;  // control

                    if (veryVerbose) {
                        printf("\t\t\tInsert itself");
                        printf(" at "); P(POS);
                    }

                    switch(insertMode) {
                      case INSERT_STRING_AT_INDEX: {
                    // string& insert(pos1, const string<C,CT,A>& str);
                        mX.insert(POS, Y);
                        mY.insert(POS, Y);
                      } break;
                      case INSERT_CSTRING_N_AT_INDEX: {
                    // string& insert(pos, const C *s, n);
                        mX.insert(POS, Y.data(), INIT_LENGTH);
                        mY.insert(POS, Y.data(), INIT_LENGTH);
                      } break;
                      case INSERT_CSTRING_AT_INDEX: {
                    // string& insert(pos, const C *s);
                        mX.insert(POS, Y.c_str());
                        mY.insert(POS, Y.c_str());
                      } break;
                      case INSERT_SUBSTRING_AT_INDEX: {
                    // string& insert(pos1, const string<C,CT,A>& str, pos2, n)
                        mX.insert(POS, Y, 0, INIT_LENGTH);
                        mY.insert(POS, Y, 0, INIT_LENGTH);
                      } break;
                      case INSERT_STRING_AT_ITERATOR: {
                    // insert(const_iterator p, InputIter first, last);
                        mX.insert(mX.cbegin() + POS, Y.begin(), Y.end());
                        mY.insert(mY.cbegin() + POS, Y.begin(), Y.end());
                      } break;
                      case INSERT_STRING_AT_CONST_ITERATOR: {
                    // insert(const_iterator p, InputIter first, last);
                        mX.insert(mX.cbegin() + POS, mY.begin(), mY.end());
                        mY.insert(mY.cbegin() + POS, mY.begin(), mY.end());
                      } break;
                      default:
                        printf("***UNKNOWN INSERT MODE***\n");
                        ASSERT(0);
                    };

                    if (veryVerbose) {
                        T_; T_; T_; T_; P(X);
                        T_; T_; T_; T_; P(Y);
                    }

                    LOOP4_ASSERT(INIT_LINE, i, INIT_CAP, POS, X == Y);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        for (size_t m = 0; m < INIT_LENGTH; ++m) {
                            const size_t INDEX        = h;
                            const size_t NUM_ELEMENTS = m;

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            for (size_t k = 0; k < INIT_LENGTH; ++k) {
                                mX[k] = VALUES[k % NUM_VALUES];
                            }

                            Obj mY(X); const Obj& Y = mY;  // control

                            if (veryVerbose) {
                                printf("\t\t\tInsert substring of itself");
                                printf(" with "); P_(INDEX); P(NUM_ELEMENTS);
                                printf(" at "); P_(POS);
                            }

                            mX.insert(POS, Y, INDEX, NUM_ELEMENTS);
                            mY.insert(POS, Y, INDEX, NUM_ELEMENTS);

                            if (veryVerbose) {
                                T_; T_; T_; T_; P(X);
                                T_; T_; T_; T_; P(Y);
                            }

                            LOOP5_ASSERT(INIT_LINE, i, INIT_CAP, POS, INDEX,
                                         X == Y);
                        }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase18Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING INSERTION:
    //
    // Concerns:
    //   1 'insert' methods assert on undefined behavior when either a NULL
    //     C-string pointer is passed or invalid iterators are passed.  Other
    //     valid parameters do not change the ability of 'insert' to assert on
    //     invalid parameters.
    //
    // Plan:
    //   Construct a string object with some string data, call 'insert' with a
    //   NULL C-string pointer and verify that it asserts.  Then call 'insert'
    //   with invalid iterators and verify that it asserts.
    //
    // Testing:
    //   string& insert(size_type pos, const C *s);
    //   string& insert(size_type pos, const C *s, n2);
    //   iterator insert(const_iterator p, C c);
    //   iterator insert(const_iterator p, size_type n, C c);
    //   template <class InputIter>
    //     iterator insert(const_iterator p, InputIter first, InputIter last);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tnegative testing insert(pos, s)\n");

    {
        Obj mX(g("ABCDE"));
        const TYPE *nullStr = 0;
        // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
        (void) nullStr;
#endif

        ASSERT_SAFE_FAIL(mX.insert(1, nullStr));
        ASSERT_SAFE_FAIL(mX.insert(mX.length() + 1, nullStr));

        ASSERT_SAFE_PASS(mX.insert(1, mX.c_str()));
    }

    if (veryVerbose) printf("\tnegative testing insert(pos, s, n)\n");

    {
        Obj mX(g("ABCDE"));
        const TYPE *nullStr = 0;
        // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
        (void) nullStr;
#endif

        ASSERT_SAFE_PASS(mX.insert(1, nullStr, 0));
        ASSERT_SAFE_FAIL(mX.insert(mX.length() + 1, nullStr, 10));

        ASSERT_SAFE_PASS(mX.insert(1, mX.c_str(), mX.length()));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, c)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0]));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0]));

        // begin() <= position < end()
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0]));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, n, c)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0], 0));
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0], 2));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0], 0));
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0], 2));

        // begin() <= position <= end()
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0], 0));
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0], 2));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, first, last)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        Obj mY(g("ABE"));
        const Obj& Y = mY;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, Y.begin(), Y.end()));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, Y.begin(), Y.end()));

        // first > last
        ASSERT_SAFE_FAIL(mX.insert(X.begin(), Y.end(), Y.begin()));

        // begin() <= position <= end() && first <= last
        ASSERT_SAFE_PASS(mX.insert(X.begin(), Y.begin(), Y.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase17()
{
    // --------------------------------------------------------------------
    // TESTING 'append'
    //
    // Plan:
    //   For appending, we will create objects of varying sizes containing
    //   default values for type T, and then append different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the 'append'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   basic_string& operator+=(CHAR_TYPE c);
    //   basic_string& append(size_type n, CHAR_TYPE c);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
        { L_,       17   },
        { L_,       19   },
        { L_,       21   },
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   },
        { L_,       35   },
        { L_,       40   },
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'append'.\n");

    if (verbose) printf("\tUsing multiple copies of 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tAppend "); P_(NUM_ELEMENTS);
                        printf("using "); P(VALUE);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    // string& operator+=(C c);
                    for (int j = 0; j != NUM_ELEMENTS; ++j) {
                        Obj &result = mX += VALUE;
                        LOOP2_ASSERT(INIT_LINE, i, &X == &result);
                    }

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    // TBD: Accurately determine the capacity/allocator
                    //      behavior of repeated '+=' operators with a single
                    //      character, equivalent to repeated 'push_back' calls

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
//                    LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());

                    for (k = 0; k < INIT_LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    for (; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUE == X[k]);
                    }

//                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

//                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
//                                 BB + REALLOC == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 B + A_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing multiple copies of 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tAppend "); P_(NUM_ELEMENTS);
                        printf("using "); P(VALUE);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    // string& append(n, C c);
                    Obj &result = mX.append(NUM_ELEMENTS, VALUE);
                    LOOP2_ASSERT(INIT_LINE, i, &X == &result);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                    LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());

                    for (k = 0; k < INIT_LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    for (; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUE == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 BB + REALLOC == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 B + A_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              X.length(),
                                                              X.capacity(),
                                                              X.max_size());

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        // void append(size_type n, C c);
                        mX.append(NUM_ELEMENTS, VALUE);
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                        LOOP2_ASSERT(INIT_LINE, LINE, CAP    == X.capacity());

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         DEFAULT_VALUE == X[k]);
                        }
                        for (; k < LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         VALUE == X[k]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase17Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING 'append'
    //
    // Plan:
    //   For appending we will create objects of varying sizes containing
    //   default values for type T, and then append different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   basic_string& operator+=(const StringRefData& strRefData);
    //   basic_string& append(const basic_string& str);
    //   basic_string& append(const basic_string& str, pos, n);
    //   basic_string& append(const CHAR_TYPE *s, size_type n);
    //   basic_string& append(const CHAR_TYPE *s);
    //   template <class Iter> basic_string& append(Iter first, Iter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                     >::value;

    enum {
        APPEND_STRING_MODE_FIRST  = 0,
        APPEND_SUBSTRING          = 0,
        APPEND_STRING             = 1,
        APPEND_CSTRING_N          = 2,
        APPEND_CSTRING_NULL_0     = 3,
        APPEND_CSTRING            = 4,
        APPEND_RANGE              = 5,
        APPEND_CONST_RANGE        = 6,
        APPEND_STRINGREFDATA      = 7,
        APPEND_STRING_MODE_LAST   = 7
    };

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   }
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int appendMode = APPEND_STRING;
             appendMode <= APPEND_STRING_MODE_LAST;
             ++appendMode)
    {
        if (verbose)
            printf("\tUsing string with appendMode = %d.\n", appendMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS =
                      (APPEND_CSTRING_NULL_0 == appendMode) ? 0 : strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    bslstl::StringRefData<TYPE> mV(&*Y.begin(),
                                                   &*Y.end());
                    const bslstl::StringRefData<TYPE> V = mV;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                        printf("using "); P(SPEC);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    switch(appendMode) {
                      case APPEND_STRING: {
                        // string& append(const string<C,CT,A>& str);
                        Obj &result = mX.append(Y);
                        ASSERT(&result == &mX);
                      } break;
                      case APPEND_CSTRING_N: {
                        // string& append(pos, const C *s, n);
                        Obj &result = mX.append(Y.data(), NUM_ELEMENTS);
                        ASSERT(&result == &mX);
                      } break;
                      case APPEND_CSTRING_NULL_0: {
                        // string& append(pos, const C *s, n);
                        Obj &result = mX.append(0, NUM_ELEMENTS);
                        ASSERT(&result == &mX);
                      } break;
                      case APPEND_CSTRING: {
                        // string& append(const C *s);
                        Obj &result = mX.append(Y.c_str());
                        ASSERT(&result == &mX);
                      } break;
                      case APPEND_RANGE: {
                        // void append(InputIter first, last);
                        Obj &result = mX.append(mU.begin(), mU.end());
                        ASSERT(&result == &mX);
                      } break;
                      case APPEND_CONST_RANGE: {
                        // void append(InputIter first, last);
                        Obj &result = mX.append(U.begin(), U.end());
                        ASSERT(&result == &mX);
                      } break;
                      case APPEND_STRINGREFDATA: {
                        //operator+=(const StringRefData& strRefData);
                        Obj &result = mX += V;
                        ASSERT(&result == &mX);
                      } break;
                      default:
                        printf("***UNKNOWN APPEND MODE***\n");
                        ASSERT(0);
                    };

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                    if (!INPUT_ITERATOR_TAG) {
                        LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                    }

                    size_t m = 0;
                    for (k = 0; k < INIT_LENGTH; ++k, ++m) {
                        LOOP3_ASSERT(INIT_LINE, LINE, k,
                                     VALUES[m % NUM_VALUES] == X[k]);
                    }
                    for (m = 0; k < LENGTH; ++k, ++m) {
                        LOOP4_ASSERT(INIT_LINE, LINE, k, m,
                                     Y[m] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 BB + REALLOC <= AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 B + A_ALLOC <=  A);
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with appendMode = %d.\n",
                        APPEND_SUBSTRING);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t POS2 = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS2;
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS_INS;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tAppend"); P_(NUM_ELEMENTS_INS);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS2);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& append(const string<C,CT,A>& str,
                        //                pos2, n);
                        Obj &result = mX.append(Y, POS2, NUM_ELEMENTS);
                        ASSERT(&result == &mX);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                        }

                        size_t m;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            LOOP3_ASSERT(INIT_LINE, LINE, n,
                                         VALUES[n % NUM_VALUES] == X[n]);
                        }
                        for (m = 0; m < NUM_ELEMENTS_INS; ++m, ++n) {
                            LOOP4_ASSERT(INIT_LINE, LINE, m, n,
                                         Y[POS2 + m] == X[n]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                     B + A_ALLOC <=  A);
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int appendMode = APPEND_STRING_MODE_FIRST;
             appendMode <= APPEND_STRING_MODE_LAST;
             ++appendMode)
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS =
                      (APPEND_CSTRING_NULL_0 == appendMode) ? 0 : strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    bslstl::StringRefData<TYPE> mV(&*Y.begin(),
                                                   &*Y.end());
                    const bslstl::StringRefData<TYPE> V = mV;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        switch(appendMode) {
                          case APPEND_STRING: {
                        // string& append(const string<C,CT,A>& str);
                            Obj &result = mX.append(Y);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING_N: {
                        // string& append(const C *s, n);
                            Obj &result = mX.append(Y.data(), NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING_NULL_0: {
                        // string& append(const C *s, n); 's = 0';
                            Obj &result = mX.append(0, NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING: {
                        // string& append(const C *s);
                            Obj &result = mX.append(Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_RANGE: {
                        // string& append(InputIter first, last);
                            Obj &result = mX.append(mU.begin(), mU.end());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CONST_RANGE: {
                        // string& append(InputIter first, last);
                            Obj &result = mX.append(U.begin(), U.end());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_SUBSTRING: {
                        // string& append(const string<C,CT,A>& str, pos2, n);
                            Obj &result = mX.append(Y, 0, NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_STRINGREFDATA: {
                        // string& operator+=(const StringRefData& strRefData);
                            Obj &result = mX += V;
                            ASSERT(&result == &mX);
                          } break;
                          default:
                            printf("***UNKNOWN APPEND MODE***\n");
                            ASSERT(0);
                        };

                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                        }

                        size_t k, m;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         DEFAULT_VALUE == X[k]);
                        }
                        for (m = 0; m < NUM_ELEMENTS; ++k, ++m) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         Y[m] == X[k]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int appendMode  = APPEND_STRING_MODE_FIRST;
             appendMode <= APPEND_STRING_MODE_LAST;
             ++appendMode)
    {
        if (verbose)
            printf("\tUsing string with appendMode = %d.\n", appendMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                Obj mX(INIT_LENGTH,
                       DEFAULT_VALUE,
                       AllocType(&testAllocator));  const Obj& X = mX;
                mX.reserve(INIT_RES);

                for (size_t k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] = VALUES[k % NUM_VALUES];
                }

                Obj mY(X); const Obj& Y = mY;  // control

                bslstl::StringRefData<TYPE> mV(&*Y.begin(),
                                                   &*Y.end());
                const bslstl::StringRefData<TYPE> V = mV;

                if (veryVerbose) {
                    printf("\t\t\tAppend itself.\n");
                }

                switch(appendMode) {
                  case APPEND_STRING: {
                // string& append(const string<C,CT,A>& str);
                    mX.append(Y);
                    mY.append(Y);
                  } break;
                  case APPEND_CSTRING_N: {
                // string& append(pos, const C *s, n);
                    mX.append(Y.data(), INIT_LENGTH);
                    mY.append(Y.data(), INIT_LENGTH);
                  } break;
                  case APPEND_CSTRING_NULL_0: {
                // string& append(pos, const C *s, n);
                    mX.append(0, 0);
                    mY.append(0, 0);
                  } break;
                  case APPEND_CSTRING: {
                // string& append(const C *s);
                    mX.append(Y.c_str());
                    mY.append(Y.c_str());
                  } break;
                  case APPEND_SUBSTRING: {
                // string& append(const string<C,CT,A>& str, pos2, n);
                    mX.append(Y, 0, INIT_LENGTH);
                    mY.append(Y, 0, INIT_LENGTH);
                  } break;
                  case APPEND_RANGE: {
                // void append(InputIter first, last);
                    mX.append(mY.begin(), mY.end());
                    mY.append(mY.begin(), mY.end());
                  } break;
                  case APPEND_CONST_RANGE: {
                // void append(InputIter first, last);
                    mX.append(Y.begin(), Y.end());
                    mY.append(Y.begin(), Y.end());
                  } break;
                  case APPEND_STRINGREFDATA: {
                //operator+=(const StringRefData& strRefData);
                    mX += V;
                    mY += V;
                  } break;
                  default:
                    printf("***UNKNOWN APPEND MODE***\n");
                    ASSERT(0);
                };

                if (veryVerbose) {
                    T_; T_; T_; T_; P(X);
                    T_; T_; T_; T_; P(Y);
                }

                LOOP5_ASSERT(INIT_LINE, appendMode, INIT_RES, X, Y, X == Y);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    {
        if (verbose) printf("\tUsing string with appendMode = %d (complete)."
                            "\n",
                            APPEND_SUBSTRING);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t h = 0; h < INIT_LENGTH; ++h) {
                    for (size_t m = 0; m < INIT_LENGTH; ++m) {
                        const size_t INDEX        = h;
                        const size_t NUM_ELEMENTS = m;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X); const Obj& Y = mY;  // control

                        if (veryVerbose) {
                            printf("\t\t\tAppend substring of itself");
                            printf(" with "); P_(INDEX); P(NUM_ELEMENTS);
                        }

                        // string& append(const string<C,CT,A>& str, pos2, n);
                        mX.append(Y, INDEX, NUM_ELEMENTS);
                        mY.append(Y, INDEX, NUM_ELEMENTS);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        LOOP5_ASSERT(INIT_LINE, INIT_RES, INDEX, X, Y, X == Y);
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase17Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING 'append'
    //
    // Concerns:
    //   'append', 'operator+=', and 'operator+' assert on undefined behavior:
    //   1 when a character string pointer parameter is NULL,
    //   2 when the 'first'/'last' parameters do not specify a valid iterator
    //     range
    //
    // Plan:
    //   1 Create a string object and then test the following things that
    //     should produce an assert for undefined behavior:
    //     o call 'append' with a NULL pointer,
    //     o call 'operator+=' with a NULL pointer,
    //     o call 'operator+' with a NULL pointer,
    //     o call 'append' with various invalid iterator ranges.
    //   2 After that, call those methods with valid parameters and verify that
    //     they don't assert.
    //
    // Testing:
    //   string& append(const C *s, size_type n);
    //   string& append(const C *s);
    //   template <class InputIter> append(InputIter first, InputIter last);
    //   operator+=(const string& rhs);
    //   operator+(const string& lhs, const CHAR_TYPE *rhs);
    //   operator+(const CHAR_TYPE *lhs, const string& rhs);
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tappend/operator+/+= with NULL\n");

    {
        Obj mX;
        const TYPE *nullStr = 0;

        ASSERT_SAFE_FAIL(mX.append(nullStr));
        ASSERT_SAFE_PASS(mX.append(nullStr, 0));
        ASSERT_SAFE_FAIL(mX.append(nullStr, 10));
        ASSERT_SAFE_FAIL(mX += nullStr);
        ASSERT_SAFE_FAIL(mX + nullStr);
        ASSERT_SAFE_FAIL(nullStr + mX);

        if (veryVerbose) printf("\tappend/operator+/+= with valid C-string\n");

        Obj nonNull(g("ABCDE"));

        ASSERT_SAFE_PASS(mX.append(nonNull.c_str()));
        ASSERT_SAFE_PASS(mX.append(nonNull.c_str(), 0));
        ASSERT_SAFE_PASS(mX.append(nonNull.c_str(), nonNull.length()));
        ASSERT_SAFE_PASS(mX += nonNull.c_str());
        ASSERT_SAFE_PASS(mX + nonNull.c_str());
        ASSERT_SAFE_PASS(nonNull.c_str() + mX);
    }

    if (veryVerbose) printf("\t'append' with invalid range\n");

    {
        Obj mX;
        Obj mY(g("ABCDE"));
        const Obj& Y = mY;
        (void) Y; // to disable "unused variable" warning

        ASSERT_SAFE_FAIL(mX.append(mY.end(), mY.begin()));
        ASSERT_SAFE_FAIL(mX.append(Y.end(), Y.begin()));
    }

    if (veryVerbose) printf("\t'append' with valid range\n");

    {
        Obj mX;
        Obj mY(g("ABCDE"));
        const Obj& Y = mY;

        ASSERT_SAFE_PASS(mX.append(mY.begin(), mY.end()));
        ASSERT_SAFE_PASS(mX.append(Y.begin(), Y.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase16()
{
    // --------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //   1) That 'begin' and 'end' return mutable iterators for a
    //      reference to a modifiable string, and non-mutable iterators
    //      otherwise.
    //   2) That the range '[begin(), end())' equals the value of the string.
    //   3) Same concerns with 'rbegin' and 'rend'.
    // In addition:
    //   4) That 'iterator' is a pointer to 'TYPE'.
    //   5) That 'const_iterator' is a pointer to 'const TYPE'.
    //   6) That 'reverse_iterator' and 'const_reverse_iterator' are
    //      implemented by the (fully-tested) 'bslstl::ReverseIterator' over a
    //      pointer to 'TYPE' or 'const TYPE'.
    //   6. That 'reverse_iterator' and 'const_reverse_iterator' are
    //      implemented by the (fully-tested) 'bsl::reverse_iterator' over a
    //      pointer to 'TYPE' or 'const TYPE'.
    //
    // Plan:
    //   For 1--3, for each value given by variety of specifications of
    //   different lengths, create a test string with this value, and access
    //   each element in sequence and in reverse sequence, both as a reference
    //   offering modifiable access (setting it to a default value, then back
    //   to its original value, and as a reference offering non-modifiable
    //   access.
    //
    // For 4--6, use 'bsl::is_same' to assert the identity of iterator types.
    // Note that these concerns let us get away with other concerns such as
    // testing that 'iter[i]' and 'iter + i' advance 'iter' by the correct
    // number 'i' of positions, and other concern about traits, because
    // 'bsl::iterator_traits' and 'bsl::reverse_iterator' have already been
    // fully tested in the 'bslstl_iterator' component.
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin();
    //   const_iterator end();
    //   const_reverse_iterator rbegin();
    //   const_reverse_iterator rend();
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("Testing 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            size_t i = 0;
            for (iterator iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                LOOP_ASSERT(LINE, Y[i] == *iter);
                *iter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *iter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, LENGTH == i);

            LOOP_ASSERT(LINE, Y == X);

            i = 0;
            for (const_iterator iter = X.begin(); iter != X.end();
                                                                 ++iter, ++i) {
                LOOP2_ASSERT(LINE, i, Y[i] == *iter);
            }
            LOOP_ASSERT(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("Testing 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;
            const int   LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            int i = LENGTH - 1;
            for (reverse_iterator riter = mX.rbegin(); riter != mX.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
                *riter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *riter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, -1 == i);

            LOOP_ASSERT(LINE, Y == X);

            i = LENGTH - 1;
            for (const_reverse_iterator riter = X.rbegin(); riter != X.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
            }
            LOOP_ASSERT(LINE, -1 == i);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase15()
{
    // --------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   1) That 'v[x]', as well as 'v.front()' and 'v.back()', allow to modify
    //      its indexed element when 'v' is an lvalue, but must not modify its
    //      indexed element when it is an rvalue.
    //   2) That 'v.at(pos)' returns 'v[x]' or throws if 'pos == v.size())'.
    //   3) That 'v.front()' is identical to 'v[0]' and 'v.back()' the same as
    //      'v[v.size() - 1]'.
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test string with this value, and access each element
    //   (front, back, at each position) both as a reference offering
    //   modifiable access (setting it to a default value, then back to its
    //   original value, and as a reference offering non-modifiable access.
    //   Verify that 'at' throws 'std::out_of_range' when accessing the
    //   past-the-end element.
    //
    // Testing:
    //   reference operator[](size_type position);
    //   reference at(size_type n);
    //   reference front();
    //   reference back();
    //   const_reference front() const;
    //   const_reference back() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            if (veryVerbose) { T_; P_(LINE); P(SPEC); }

            LOOP_ASSERT(LINE, Y == X);

            // Test front/back.
            if (!mX.empty()) {
                LOOP_ASSERT(LINE, mX.front() == mX[0]);
                LOOP_ASSERT(LINE, mX.back() == mX[mX.size() - 1]);
            }

            // Test operator[].
            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X[j]);
                mX[j] = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X[j]);
                mX.at(j) = Y[j];
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X.at(j));
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            bool outOfRangeCaught = false;
            try {
                mX.at(LENGTH) = DEFAULT_VALUE;
            }
            catch (std::out_of_range) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, Y == X);
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase15Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   For a string 's', the following const and non-const operations assert
    //   on undefined behavior:
    //   1 s[x] - when the index 'x' is out of range
    //   2 s.front() - when 's' is empty
    //   3 s.back() - when 's' is empty
    //
    // Plan:
    //   To test concerns (2) and (3), create an empty string and verify that
    //   'front'/'back' methods assert correctly.  Then insert a single
    //   character into the string and verify that the methods don't assert any
    //   more.  Then remove the character to make the string empty again, and
    //   verify that the methods start asserting again.
    //
    //   To test concern (1), create a string using a variety of specifications
    //   of different lengths, then scan the range of negative and positive
    //   indices for 'operator[]' and verify that 'operator[]' asserts when the
    //   index is out of range.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   const T& operator[](size_type position) const;
    //   T& front();
    //   T& back();
    //   const T& front() const;
    //   const T& back() const;
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEABCDEABCDE"               }, // 20
#ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }, // 15
        { L_,   "ABCDEABCDEABCDEABCDE"               }, // 20
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }, // 30
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEABCDEABCDE" }, // 40
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (veryVerbose) printf("\toperator[]\n");

    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            (void) LINE;
            (void) LENGTH;

            Obj mX(g(SPEC));
            const Obj& X = mX;

            const int numChars = static_cast<int>(X.size());
            for (int i = -numChars - 1; i < numChars * 2 + 2; ++i) {
                if (i >= 0 && i <= numChars) {
                    ASSERT_SAFE_PASS(X[i]);
                    ASSERT_SAFE_PASS(mX[i]);
                }
                else {
                    ASSERT_SAFE_FAIL(X[i]);
                    ASSERT_SAFE_FAIL(mX[i]);
                }
            }
        }
    }

    if (veryVerbose) printf("\tfront/back\n");

    {
        Obj mX;
        const Obj& X = mX;
        ASSERT_SAFE_FAIL(X.front());
        ASSERT_SAFE_FAIL(mX.front());
        ASSERT_SAFE_FAIL(X.back());
        ASSERT_SAFE_FAIL(mX.back());

        mX.push_back(TYPE('A'));
        ASSERT_SAFE_PASS(X.front());
        ASSERT_SAFE_PASS(mX.front());
        ASSERT_SAFE_PASS(X.back());
        ASSERT_SAFE_PASS(mX.back());

        mX.pop_back();
        ASSERT_SAFE_FAIL(X.front());
        ASSERT_SAFE_FAIL(mX.front());
        ASSERT_SAFE_FAIL(X.back());
        ASSERT_SAFE_FAIL(mX.back());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase14()
{
    // --------------------------------------------------------------------
    // TESTING CAPACITY
    //
    // Concerns:
    //   1) That 'v.reserve(n)' reserves sufficient capacity in 'v' to hold
    //      'n' elements without reallocation, but does not change value.
    //      In addition, if 'v.reserve(n)' allocates, it must allocate for a
    //      capacity of exactly 'n' bytes.
    //   2) That 'v.resize(n, val)' brings the new size to 'n', adding elements
    //      equal to 'val' if 'n' is larger than the current size.
    //   3) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   4) That 'reserve' and 'resize' are exception-neutral with full
    //      guarantee of rollback.
    //   5) That the accessors such as 'capacity', 'empty', return the correct
    //      value.
    //
    // Plan:
    //   For string 'v' having various initial capacities, call 'v.reserve(n)'
    //   for various values of 'n'.  Verify that sufficient capacity is
    //   allocated by filling 'v' with 'n' elements.  Perform each test in the
    //   standard 'bslma' exception-testing macro block.
    //
    // Testing:
    //   void reserve(size_type n);
    //   void resize(size_type n);
    //   void resize(size_type n, CHAR_TYPE c);
    //   size_type max_size() const;
    //   size_type capacity() const;
    //   bool empty() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;
    ASSERT(0 == testAllocator.numBytesInUse());

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const size_t EXTEND[] = {
        0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
    };
    enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

    static const size_t DATA[] = {
        0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.  Any reasonable value must be 2^31 -1 at least.

        Obj X;
        ASSERT(~(size_t)0 / sizeof(TYPE)  - 1 >= X.max_size());
        ASSERT(~(unsigned)0 / sizeof(TYPE) / 2  - 1 <= X.max_size());
    }

    if (verbose) printf("\tTesting 'reserve', 'capacity' and 'empty'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);

              const Int64  NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY         = X.capacity();
              {
                  ExceptionGuard<Obj> guard(X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, CAP == X.size());
                  LOOP_ASSERT(ti, CAPACITY >= NE || NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, CAP + DELTA == X.size());
              LOOP_ASSERT(ti,
                          NUM_ALLOC_AFTER == testAllocator.numAllocations());

              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP = EXTEND[ei];

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              testAllocator.setAllocationLimit(AL);
              const Int64 NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY        = X.capacity();
              {
                  ExceptionGuard<Obj> guard(X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, 0  == X.size());
                  LOOP_ASSERT(ti, NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, NE);
              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti,
                          NUM_ALLOC_AFTER == testAllocator.numAllocations());

              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    if (verbose) printf("\tTesting 'resize'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);
              const Int64 NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY        = X.capacity();

              ExceptionGuard<Obj> guard(X, L_);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = CAP; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              const Int64 NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY        = X.capacity();

              ExceptionGuard<Obj> guard(X, L_);

              testAllocator.setAllocationLimit(AL);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = 0; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13()
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   The concerns are similar to those of the constructor with the same
    //   signature (case 12), except that the implementation is different, and
    //   in addition the previous value of the string being assigned to must be
    //   freed properly.  There is a further concern about aliasing (although
    //   assigning a portion of oneself is not subject to aliasing in most
    //   implementations), and an additional concern that allocators are not
    //   propagated (until we support the C++11 allocator propagation traits).
    //   Finally, all 'assign' functions must return a reference to the string
    //   that has just been assigned to, just like the assignment operators.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the assignment.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'copy' followed by 'erase + append', and append will
    //   be tested more completely in test case 25.
    //
    // Testing:
    // * basic_string& assign(const basic_string& str);
    //   basic_string& assign(bslmf::MovableRef<basic_string> str);
    // * basic_string& assign(basic_string& str, pos, n);
    // * basic_string& assign(const CHAR_TYPE *s, size_type n);
    // * basic_string& assign(const CHAR_TYPE *s);
    //   basic_string& assign(size_type n, CHAR_TYPE c);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        ASSIGN_STRING_MODE_FIRST  = 0,
        ASSIGN_SUBSTRING_AT_INDEX = 0,
        ASSIGN_STRING_AT_INDEX    = 1,
        ASSIGN_CSTRING_N_AT_INDEX = 2,
        ASSIGN_CSTRING_AT_INDEX   = 3,
        ASSIGN_STRING_AT_ITERATOR = 4,
        ASSIGN_STRING_MODE_LAST   = 5
    };

    if (verbose) printf("\nTesting initial-length assignment.\n");
    {
        static const struct {
            int         d_lineNum;          // source line number
            int         d_length;           // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0   },
            { L_,        1   },
            { L_,        2   },
            { L_,        3   },
            { L_,        4   },
            { L_,        5   },
            { L_,        9   },

            { L_,       11   },
            { L_,       12   },
            { L_,       13   },
            { L_,       15   },
            { L_,       23   },
            { L_,       24   },
            { L_,       25   },
            { L_,       30   }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    Obj& result = mX.assign(LENGTH, VALUE);        // test here
                    ASSERT(&result == &mX);

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 X.capacity() >= X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, VALUE == X[j]);
                    }
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf("\tWith exceptions.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;
                        ExceptionGuard<Obj> guard(X, L_);

                        testAllocator.setAllocationLimit(AL);

                        Obj& result = mX.assign(LENGTH, VALUE);    // test here
                        ASSERT(&result == &mX);
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == testAllocator.numMismatches());
                    ASSERT(0 == testAllocator.numBlocksInUse());
                }
            }
        }

        if (verbose) printf("\nTesting move assign.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                Obj src(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                Obj dst(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    Obj& result1 = src.assign(LENGTH, VALUE);
                    ASSERT(&result1 == &src);
                    const Obj src_copy(src);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    Obj& result = dst.assign(MoveUtil::move(src));
                    ASSERT(&result == &dst);                    // ^--test here

                    LOOP2_ASSERT(INIT_LINE, LINE, dst == src_copy);
                    LOOP2_ASSERT(INIT_LINE, LINE,
                                         BB == testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(INIT_LINE, LINE,
                                          B >= testAllocator.numBlocksInUse());
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf(
            "\nTesting move assign with different allocators.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    Obj src(INIT_LENGTH, DEFAULT_VALUE);
                    Obj dst(INIT_LENGTH, DEFAULT_VALUE,
                            AllocType(&testAllocator));
                    ASSERT(src.get_allocator() != dst.get_allocator());

                    Obj& result1 = src.assign(LENGTH, VALUE);
                    ASSERT(&result1 == &src);
                    const Obj src_copy(src);

                    const bool noAlloc = dst.capacity() >= src.size();
                    const bool reAlloc = dst.capacity() > DEFAULT_CAPACITY &&
                                         !noAlloc;
                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    Obj& result = dst.assign(MoveUtil::move(src));
                    ASSERT(&result == &dst);                       // test here

                    LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE, dst == src);
                    LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                   src.get_allocator() != dst.get_allocator());

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();
                    if (noAlloc) {
                        LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                     BB + 0 == AA);
                        LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                     B  + 0 ==  A);
                    }
                    else {
                        LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                     BB + 1 == AA);
                        if (!reAlloc) {
                            LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                         B  + 1 ==  A);
                        }
                        else {
                            LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                         B + 0 == A);
                        }
                    }
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf("\nTesting exception safety of move assignment"
                            " with string having different allocators.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;

                        Obj mY(LENGTH, VALUE);
                        const Obj& Y = mY;
                        ExceptionGuard<Obj> guard(X, L_);

                        testAllocator.setAllocationLimit(AL);

                        Obj& result = mX.assign(MoveUtil::move(Y));  // test
                        ASSERT(&result == &mX);
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == testAllocator.numMismatches());
                    ASSERT(0 == testAllocator.numBlocksInUse());
                }
            }
        }
    }

#ifndef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nSkip testing assign() exception-safety.\n");
#else
    if (verbose) printf("\nTesting assign() exception-safety.\n");

    {
        size_t defaultCapacity = Obj().capacity();

        Obj src(defaultCapacity + 1, '1', &testAllocator);

        Obj dst(defaultCapacity, '2', &testAllocator);
        Obj dstCopy(dst, &testAllocator);

        bsls::Types::Int64 oldLimit = testAllocator.allocationLimit();

        {
            // make the allocator throw on the next allocation
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                // the assignment will require to allocate more memory
                dst.assign(src);
            }
            catch (bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src, 0, Obj::npos);
            }
            catch (bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.c_str());
            }
            catch (bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.c_str(), src.size());
            }
            catch (bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.size(), 'c');
            }
            catch (bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.begin(), src.end());
            }
            catch (bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        // restore the allocator state
        testAllocator.setAllocationLimit(oldLimit);
    }
#endif

}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13StrRefData()
{
    // --------------------------------------------------------------------
    // TESTING 'assign' FROM STRING REF
    // The concerns are the same as for the constructor with the same
    // signature (case 12), except that the implementation is different,
    // and in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the 'bslstl::StringRefData' object.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;  // source line number
        unsigned    d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    {
        const Obj dummy;
        ASSERT(dummy.capacity() < DATA[NUM_DATA - 1].d_length);
    }

    if (verbose) printf("\tUsing 'bslstl::StringRefData'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                Obj mY(g(SPEC)); const Obj& Y = mY;

                const bslstl::StringRefData<TYPE> strRef(Y.begin(), Y.end());

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES],
                       AllocType(&testAllocator));
                const Obj& X = mX;

                const size_t preCapacity = X.capacity();

                const bsls::Types::Int64 numAllocs =
                                                testAllocator.numAllocations();

                mX.assign(strRef);

                if (LENGTH <= preCapacity) {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                                  testAllocator.numAllocations() == numAllocs);
                }
                else {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                              testAllocator.numAllocations() == numAllocs + 1);
                }

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP6_ASSERT(INIT_LINE, LINE, i, ti, LENGTH, X.size(),
                                                           LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13InputIterator()
{
    // --------------------------------------------------------------------
    // TESTING 'assign' FROM STRING REF
    // The concerns are the same as for the constructor with the same
    // signature (case 12), except that the implementation is different,
    // and in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the 'bslstl::StringRefData' object.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    {
        const Obj dummy;
        ASSERT(dummy.capacity() < (size_t)DATA[NUM_DATA - 1].d_length);
    }

    if (verbose) printf("\tUsing 'bslstl::StringRefData'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                Obj mY(g(SPEC)); const Obj& Y = mY;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES],
                       AllocType(&testAllocator));
                const Obj& X = mX;

                mX.assign(InputIterator(Y.data()),
                          InputIterator(Y.data() + LENGTH));

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP6_ASSERT(INIT_LINE, LINE, i, ti, LENGTH, X.size(),
                                                           LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   The concerns are the same as for the constructor with the same
    //   signature (case 12), except that the implementation is different, and
    //   in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class Iter> basic_string& assign(Iter first, Iter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                     >::value;

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }, // 15
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    {
        // Ensure at least the longest test sequence will exceed the short
        // string capacity.

        const Obj dummy;
        ASSERT(dummy.capacity() < (size_t)(DATA[NUM_DATA - 1].d_length));
    }

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES],
                       AllocType(&testAllocator));
                const Obj& X = mX;

                const size_t preCapacity = X.capacity();

                const bsls::Types::Int64 numAllocs =
                                                testAllocator.numAllocations();

                mX.assign(U.begin(), U.end());

                if (LENGTH <= preCapacity) {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                                  testAllocator.numAllocations() == numAllocs);
                }
                else {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                              testAllocator.numAllocations() == numAllocs + 1);
                }

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP6_ASSERT(INIT_LINE, LINE, i, ti, LENGTH, X.size(),
                                                           LENGTH == X.size());
                if (!INPUT_ITERATOR_TAG) {
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 X.capacity() >= X.size());
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;
                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;  // control

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);  const Obj& X = mX;
                    ExceptionGuard<Obj> guard(X, L_);

                    testAllocator.setAllocationLimit(AL);

                    Obj& result = mX.assign(U.begin(), U.end());  // test here
                    ASSERT(&result == &mX);
                    guard.release();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(testAllocator.numMismatches(),
                            0 == testAllocator.numMismatches());
                LOOP_ASSERT(testAllocator.numBlocksInUse(),
                            0 == testAllocator.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING 'assign'
    //
    // Concerns:
    //   'assign' methods asserts on undefined behavior when:
    //   1 C-string pointer parameter is NULL
    //   2 'first' and 'last' iterators do not make a valid range
    //      (i.e., first > last).
    //
    // Plan:
    //   For concern (1), create a string and call 'assign' methods with NULL
    //   pointer.
    //   For concern (2), create a string and call 'assign' with two iterators
    //   that do not make a valid range.
    //
    // Testing:
    //   void assign(const C *s, size_type n);
    //   void assign(const C *s);
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tnegative test assign with NULL C-string\n");

    {
        Obj mX;
        ASSERT_SAFE_FAIL(mX.assign(0));
        ASSERT_SAFE_PASS(mX.assign(0, size_t(0)));
        ASSERT_SAFE_FAIL(mX.assign(0, size_t(10)));

        Obj mY(g("ABCD"));
        ASSERT_SAFE_PASS(mX.assign(mY.c_str()));
        ASSERT_SAFE_PASS(mX.assign(mY.c_str(), mY.length()));
    }

    if (veryVerbose) {
        printf("\tnegative test assign with invalid iterator range\n");
    }

    {
        Obj mX;
        Obj mY(g("ABCD"));

        // first > last
        ASSERT_SAFE_FAIL(mX.assign(mY.end(), mY.begin()));
        ASSERT_SAFE_FAIL(mX.assign(mY.begin() + 1, mY.begin()));

        // first <= last
        ASSERT_SAFE_PASS(mX.assign(mY.begin(), mY.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase12()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTORS:
    //
    // Concerns:
    //  1) The initial value is correct.
    //  2) The initial capacity is correctly set up.
    //  3) The constructor is exception neutral w.r.t. memory allocation.
    //  4) The internal memory management system is hooked up properly so that
    //     *all* internally allocated memory draws from a  user-supplied
    //     allocator whenever one is specified.
    //  5) The move constructor moves value, capacity, and allocator correctly,
    //     and without performing any allocation.
    //
    // Plan:
    //   For the constructor we will create objects of varying sizes with
    //   different 'value' as argument.  Test first with the default value
    //   for type T, and then test with different values.  Perform the above
    //   tests:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   As for concern 5, we simply move-construct each value into a new
    //   string and check that the value, capacity, and allocator are as
    //   expected, and that no allocation was performed.
    //
    // Testing:
    //   basic_string(const basic_string& str, pos, n = npos, a = A());
    //   basic_string(const CHAR_TYPE *s, a = A());
    //   basic_string(const CHAR_TYPE *s, size_type n, a = A());
    //   basic_string(size_type n, CHAR_TYPE c = CHAR_TYPE(), a = A());
    //   basic_string(const basic_string& original, a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE           *values     = 0;
    const TYPE *const&    VALUES     = values;
    const int             NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // specifications
    } DATA[] = {
        //line  spec                                       length
        //----  ----                                       ------
        { L_,   ""                                      }, //   0
        { L_,   "A"                                     }, //   1
        { L_,   "AB"                                    }, //   2
        { L_,   "ABC"                                   }, //   3
        { L_,   "ABCD"                                  }, //   4
        { L_,   "ABCDE"                                 }, //   5
        { L_,   "ABCDEA"                                }, //   6
        { L_,   "ABCDEAB"                               }, //   7
        { L_,   "ABCDEABC"                              }, //   8
        { L_,   "ABCDEABCD"                             }, //   9
        { L_,   "ABCDEABCDEA"                           }, //  11
        { L_,   "ABCDEABCDEAB"                          }, //  12
        { L_,   "ABCDEABCDEABC"                         }, //  13
        { L_,   "ABCDEABCDEABCDE"                       }, //  15
        { L_,   "ABCDEABCDEABCDEA"                      }, //  16
        { L_,   "ABCDEABCDEABCDEAB"                     }, //  17
        { L_,   "ABCDEABCDEABCDEABCDEABC"               }, //  23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"              }, //  24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"             }, //  25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEA"       }, //  31
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"      }, //  32
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEABC"     }, //  33
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                      "ABCDEABCDEABCDEABCDEABCDEABCDEA" }, //  63
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEAB"  }, //  64
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEABC" }  //  65
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting string(n, c, a = A()).\n");
    {
        if (verbose) printf("\t\tWithout passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const char  *SPEC   = DATA[ti].d_spec;
                const size_t LENGTH = strlen(SPEC);
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            //  LOOP2_ASSERT(LINE, ti,
            //                        LENGTH | MAX_ALIGN_MASK == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\t\tWith passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const char  *SPEC   = DATA[ti].d_spec;
                const size_t LENGTH = strlen(SPEC);
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                Obj mX(LENGTH, VALUE, AllocType(&testAllocator));
                const Obj& X = mX;

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }

                if (LENGTH <= DEFAULT_CAPACITY)
                {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
                }
            }
        }
    }

    if (verbose) printf("\tTesting string(const C *s, n, a = A())\n"
                        "\t    and string(const C *s, a = A()).\n");
    {
        if (verbose) printf("\t\tWithout passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const char  *SPEC   = DATA[ti].d_spec;
                const size_t LENGTH = strlen(SPEC);

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj mY(g(SPEC));  const Obj& Y = mY;  // get a valid const C*

                {
                    Obj mX(&Y[0], LENGTH);  const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                    LOOP2_ASSERT(LINE, ti, Y == X);
                }
                {
                    Obj mX(&Y[0]);  const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                    LOOP2_ASSERT(LINE, ti, Y == X);
                }
            }
        }

        if (verbose) printf("\t\tWith passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const char  *SPEC   = DATA[ti].d_spec;
                const size_t LENGTH = strlen(SPEC);

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj mY(g(SPEC));  const Obj& Y = mY;  // get a valid const C*

                {
                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    Obj mX(&Y[0], LENGTH, AllocType(&testAllocator));
                    const Obj& X = mX;

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                    LOOP2_ASSERT(LINE, ti, Y == X);

                    if (LENGTH <= DEFAULT_CAPACITY)
                    {
                        LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
                    }
                }
                {
                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    Obj mX(&Y[0], AllocType(&testAllocator));
                    const Obj& X = mX;

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                    LOOP2_ASSERT(LINE, ti, Y == X);

                    if (LENGTH <= DEFAULT_CAPACITY)
                    {
                        LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
                    }
                }
            }
        }
    }

    if (verbose) printf("\tTesting string(str, pos, n = npos, a = A()).\n");
    {
        if (verbose) printf("\t\tWithout passing in an allocator, nor n.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const char  *SPEC   = DATA[ti].d_spec;
                const size_t LENGTH = strlen(SPEC);

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj mY(g(SPEC));  const Obj& Y = mY;

                for (size_t i = 0; i < LENGTH; ++i) {
                    const size_t POS = i;

                    Obj mU(g(SPEC + POS));  const Obj& U = mU;

                    if (veryVerbose) {
                        printf("\t\t\t\tFrom "); P_(Y); P(POS);
                    }

                    Obj mX(Y, POS);  const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH - POS == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH - POS <= X.capacity());
                    LOOP2_ASSERT(LINE, ti, U == X);
                }
            }
        }

        if (verbose) printf("\t\tWithout passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE       = DATA[ti].d_lineNum;
                const char  *SPEC       = DATA[ti].d_spec;
                const size_t LENGTH     = strlen(SPEC);

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj mY(g(SPEC));  const Obj& Y = mY;

                for (size_t i = 0; i <= LENGTH; ++i) {
                    const size_t POS = i;

                    Obj mU(g(SPEC + POS));  const Obj& U = mU;

                    if (veryVerbose) {
                        printf("\t\t\t\tFrom "); P_(Y); P_(POS);
                        printf("with " ZU " chars.\n", LENGTH - POS);
                    }

                    {
                        Obj mX(Y, POS, Obj::npos);  const Obj& X = mX;

                        if (veryVerbose) {
                            T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(LINE, ti, LENGTH - POS == X.size());
                        LOOP2_ASSERT(LINE, ti, LENGTH - POS <= X.capacity());
                        LOOP2_ASSERT(LINE, ti, U == X);
                    }
                    {
                        Obj mX(Y, POS, LENGTH);  const Obj& X = mX;

                        if (veryVerbose) {
                            T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(LINE, ti, LENGTH - POS == X.size());
                        LOOP2_ASSERT(LINE, ti, LENGTH - POS <= X.capacity());
                        LOOP2_ASSERT(LINE, ti, U == X);
                    }
                    {
                        Obj mX(Y, POS, LENGTH - POS);  const Obj& X = mX;

                        if (veryVerbose) {
                            T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(LINE, ti, LENGTH - POS == X.size());
                        LOOP2_ASSERT(LINE, ti, LENGTH - POS <= X.capacity());
                        LOOP2_ASSERT(LINE, ti, U == X);
                    }

                    if (veryVerbose) {
                        printf("\t\t\t\tFrom "); P_(Y); P_(POS);
                        printf("but at most one char.");
                    }

                    {
                        Obj mX(Y, POS, 1);  const Obj& X = mX;

                        if (veryVerbose) {
                            T_; T_; P_(X); P(X.capacity());
                        }

                        if (POS < LENGTH) {
                            LOOP2_ASSERT(LINE, ti, 1 == X.size());
                            LOOP2_ASSERT(LINE, ti, 1 <= X.capacity());
                            LOOP2_ASSERT(LINE, ti, Y[POS] == X[0]);
                        }
                        else {
                            LOOP2_ASSERT(LINE, ti, 0 == X.size());
                            LOOP2_ASSERT(LINE, ti, TYPE() == X[0]);
                        }
                    }
                }
            }
        }

        if (verbose) printf("\t\tWith passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE       = DATA[ti].d_lineNum;
                const char  *SPEC       = DATA[ti].d_spec;
                const size_t LENGTH     = strlen(SPEC);

                if (veryVerbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj mY(g(SPEC));  const Obj& Y = mY;

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                Obj mX(Y, 0, LENGTH, AllocType(&testAllocator));
                const Obj& X = mX;

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

                if (LENGTH <= DEFAULT_CAPACITY)
                {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
                }
            }
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);
            const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

            if (veryVerbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(VALUE);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(LENGTH, VALUE, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            if (veryVerbose) {
                printf("\t\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj mY(g(SPEC));  const Obj& Y = mY;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(&Y[0], AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(&Y[0], LENGTH, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(Y, 0, LENGTH, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }

    if (verbose) printf("\tAllocators hooked up properly.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);
            const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

            if (veryVerbose) {
                printf("\t\tCreating object of "); P(LENGTH);
            }

            Obj mY(g(SPEC));  const Obj& Y = mY;

            for (int i = 0; i < 4; ++i) {
                const Int64 TB = defaultAllocator_p->numBytesInUse();
                ASSERTV(LINE, 0  == globalAllocator_p->numBytesInUse());
                ASSERTV(LINE, TB == defaultAllocator_p->numBytesInUse());
                ASSERTV(LINE, 0  == objectAllocator_p->numBytesInUse());

                switch (i) {
                  case 0: {
                    Obj x(LENGTH, VALUE, objectAllocator_p);

                    ASSERTV(LINE, 0  == globalAllocator_p->numBytesInUse());
                    ASSERTV(LINE, TB == defaultAllocator_p->numBytesInUse());
                  } break;
                  case 1: {
                    Obj x(&Y[0], objectAllocator_p);

                    ASSERTV(LINE, 0  == globalAllocator_p->numBytesInUse());
                    ASSERTV(LINE, TB == defaultAllocator_p->numBytesInUse());
                  } break;
                  case 2: {
                    Obj x(&Y[0], LENGTH, objectAllocator_p);

                    ASSERTV(LINE, 0  == globalAllocator_p->numBytesInUse());
                    ASSERTV(LINE, TB == defaultAllocator_p->numBytesInUse());
                  } break;
                  case 3: {
                    Obj x(Y, 0, LENGTH, objectAllocator_p);

                    ASSERTV(LINE, 0  == globalAllocator_p->numBytesInUse());
                    ASSERTV(LINE, TB == defaultAllocator_p->numBytesInUse());
                  } break;
                };

                ASSERTV(LINE, 0 == globalAllocator_p->numBytesInUse());
                ASSERTV(LINE, 0 == objectAllocator_p->numBytesInUse());
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase12Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING RANGE (TEMPLATE) CONSTRUCTORS:
    //
    //   Concerns:
    //    1) That the initial value is correct.
    //    2) That the initial range is correctly imported and then moved if the
    //       initial 'FWD_ITER' is an input iterator.
    //    3) That the initial capacity is correctly set up if the initial
    //       'FWD_ITER' is a random-access iterator.
    //    4) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    5) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   We will create objects of varying sizes and capacities containing
    //   default values, and insert a range containing distinct values as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   template<class InputIter>
    //     string(InputIter first, InputIter last, a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const int INPUT_ITERATOR_TAG =
         bsl::is_same<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            Obj mY(g(SPEC));  const Obj& Y = mY;
            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) { printf("\t\tCreating object "); P(SPEC); }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));     const Obj& Y = mY;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64  B = testAllocator.numBlocksInUse();

            Obj mX(U.begin(), U.end(), AllocType(&testAllocator));
            const Obj& X = mX;

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }

            if (LENGTH <= DEFAULT_CAPACITY) {
                LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
            }
            else if (INPUT_ITERATOR_TAG) {
             // LOOP2_ASSERT(LINE, ti, BB + 1 + NUM_ALLOCS[LENGTH] == AA);
                LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
            } else {
             // LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
            }
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));        const Obj& Y = mY;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64  B = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(U.begin(), U.end(), AllocType(&testAllocator));

                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            if (LENGTH <= DEFAULT_CAPACITY) {
                LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
            }
            else {
                if (INPUT_ITERATOR_TAG) {
                    LOOP2_ASSERT(LINE, ti, BB + 1 <= AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
            }

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase12Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING FOR CONSTRUCTORS:
    //
    // Concerns:
    //   1 Constructing a string with a NULL pointer, or integral NULL value
    //   including 'false' is undefined behavior and it asserts.
    //   2 Constructing a string with an invalid iterator range asserts on
    //   undefined behavior.
    //
    // Plan:
    //   For (1), call the proper constructor with a NULL value and verify that
    //   it asserts.  Then call the same constructor with a valid C-string
    //   pointer and verify that it doesn't assert.
    //   For (2), construct a string with the constructor taking an invalid
    //   iterator range ('first > last') and verify that it asserts.
    //
    // Testing:
    //   string<C,CT,A>(const C *s, n, a = A());
    //   string<C,CT,A>(const C *s, a = A());
    //   template<class InputIter>
    //     string(InputIter first, InputIter last, a = A());
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    ASSERT_SAFE_FAIL_RAW(Obj(0));

    ASSERT_SAFE_PASS_RAW(Obj(0, size_t(0)));
    ASSERT_SAFE_FAIL_RAW(Obj(0, size_t(10)));

    Obj mY(g("ABCDE"));
    ASSERT_SAFE_FAIL_RAW(Obj(mY.end(), mY.begin()));

    Obj mX(g("ABCDE"));
    ASSERT_SAFE_PASS_RAW(Obj(mX.c_str()));
    ASSERT_SAFE_PASS_RAW(Obj(mX.c_str(), 0));
    ASSERT_SAFE_PASS_RAW(Obj(mX.c_str(), mX.length()));
    ASSERT_SAFE_PASS_RAW(Obj(mY.begin(), mY.end()));
}

template <class TYPE, class TRAITS, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, TRAITS, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>  StdAlloc;

    typedef bsl::basic_string<TYPE, TRAITS, StdAlloc>  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "This is very long string which must allocate",
        "This is another very long string which must allocate"
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC = SPECS[ti];

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj mW(ISPEC, scratch);  const Obj& W = mW;  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            {
                Obj mY(ISPEC, mas);  const Obj& Y = mY;

                Obj mX(JSPEC, mat);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(JSPEC) P_(Y) P_(X) P(W) }

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC,  W,   Y,  W == Y);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::
                             testCase9_propagate_on_container_copy_assignment()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_copy_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_copy_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_copy_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_copy_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Copy-assign 'Y' to 'X' and use 'operator==' to verify that both
    //:     'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_copy_assignment
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == false'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class TYPE, class TRAITS, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, TRAITS, ALLOC>::
                    testCase9_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                       StdAlloc;

    typedef bsl::basic_string<TYPE, TRAITS, StdAlloc>  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "This is very long string which must allocate",
        "This is another very long string which must allocate"
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC = SPECS[ti];

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj mW(ISPEC, scratch);  const Obj& W = mW;  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC = SPECS[tj];

            {
                Obj mY(ISPEC, mas);  const Obj& Y = mY;

                Obj mX(JSPEC, mat);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(JSPEC) P_(Y) P_(X) P(W) }

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = MoveUtil::move(mY));

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::
                             testCase9_propagate_on_container_move_assignment()
{
    // ------------------------------------------------------------------------
    // MOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_move_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_move_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_move_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_move_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Move-assign 'y' to 'X' and use 'operator==' to verify that both
    //:     'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_move_assignment
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\t'propagate_on_container_move_assignment::value == false'\n");

    testCase9_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_move_assignment::value == true'\n");

    testCase9_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase9()
{
    // --------------------------------------------------------------------
    // TESTING ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1) The value represented by any instance can be assigned to any
    //      other instance regardless of how either value is represented
    //      internally.
    //   2) The 'rhs' value must not be affected by the operation.
    //   3) 'rhs' going out of scope has no effect on the value of 'lhs'
    //      after the assignment.
    //   4) Aliasing (x = x): The assignment operator must always work --
    //      even when the lhs and rhs are identically the same object.
    //   5) The assignment operator must be neutral with respect to memory
    //      allocation exceptions.
    //   6) The copy constructor's internal functionality varies
    //      according to which bitwise copy/move trait is applied.
    //   7) The assignment can be made from a 'StringRefData' without creating
    //      temporary string and without using the default allocator.
    //
    // Plan:
    //   Specify a set S of unique object values with substantial and
    //   varied differences, ordered by increasing length.  The set S must
    //   include objects with the following states:
    //   I) empty
    //   II) short string
    //   III) long string
    //   IV) dynamic string with short-string length
    //
    //   For each value in S, construct an object x along with a sequence of
    //   similarly constructed duplicates x1, x2, ..., xN.  Attempt to affect
    //   every aspect of white-box state by altering each xi in a unique way.
    //   Let the union of all such objects be the set T.
    //
    //   To address concerns 1, 2, and 5, construct tests u = v for all
    //   (u, v) in T X T.  Using canonical controls UU and VV, assert
    //   before the assignment that UU == u, VV == v, and v == u if and only if
    //   VV == UU.  After the assignment, assert that VV == u,
    //   UU == v if the allocators match or VV == v otherwise and, for grins,
    //   that v == u.  Let v go out of scope and confirm that VV == u.
    //   All of these tests are performed within the 'bslma' exception testing
    //   apparatus.  Since the execution time is lengthy with exceptions,
    //   every permutation is not performed when exceptions are tested.
    //   Every permutation is also tested separately without exceptions.
    //
    //   As a separate exercise, we address 4 and 5 by constructing tests
    //   y = y for all y in T.  Using a canonical control X, we will verify
    //   that X == y before and after the assignment, again within
    //   the bslma exception testing apparatus.
    //
    //   To address concern 6, all these tests are performed on user
    //   defined types:
    //          With allocator, copyable
    //          With allocator, moveable
    //          With allocator, not moveable
    //
    // Testing:
    //   basic_string& operator=(const basic_string& rhs);
    //   basic_string& operator=(const StringRefData& strRefData);
    //   basic_string& operator=(const basic_string& rhs);
    //   basic_string& operator=(const CHAR_TYPE *s);
    //   basic_string& operator=(CHAR_TYPE c);
    // --------------------------------------------------------------------

    bslma::TestAllocator         testAllocator(veryVeryVerbose);
    Allocator                    Z(&testAllocator);
    bslma::TestAllocator         defaultAllocator(veryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    (void) NUM_VALUES;

    // --------------------------------------------------------------------

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "Without Exceptions\n");
    {
        static const char *SPECS[] = {                      // length
            "",                                             // 0
            "A",                                            // 1
            "BC",                                           // 2
            "CDE",                                          // 3
            "DEAB",                                         // 4
            "DEABC",                                        // 5
            "CBAEDCBA",                                     // 8
            "EDCBAEDCB",                                    // 9
            "EDCBAEDCBAE",                                  // 11
            "EDCBAEDCBAED",                                 // 12
            "EDCBAEDCBAEDC",                                // 13
            "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
            "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
            "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        for (int fromRef = 0; fromRef < 2; ++fromRef) {
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
                uOldLen = static_cast<int>(uLen);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC); // control

                    const bool EQUAL = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            Obj mU(Z);  const Obj& U = mU;
                            stretchRemoveAll(&mU, U_N, VALUES[0]);
                            gg(&mU, U_SPEC);
                            {
                    // v--------
                    Obj mV(Z);  const Obj& V = mV;
                    stretchRemoveAll(&mV, V_N, VALUES[0]);
                    gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, EQUAL == (V == U));

                    const Int64 TDA = defaultAllocator.numAllocations();
                    const int NUM_CTOR = numCopyCtorCalls;
                    const int NUM_DTOR = numDestructorCalls;
                    const size_t OLD_LENGTH = U.size();

                    if (!fromRef) {
                        mU = V; // test assignment here
                    }
                    else {
                        bslstl::StringRefData<TYPE> srd(
                                              V.data(), V.data() + V.length());
                        mU = srd; // test assignment here
                    }

                    const Int64 TDB = defaultAllocator.numAllocations();

                    ASSERTV(fromRef, U_SPEC, V_SPEC, TDB - TDA, TDB == TDA);
                    ASSERT((numCopyCtorCalls - NUM_CTOR) <= (int)V.size());
                    ASSERT((numDestructorCalls - NUM_DTOR) <=
                                                 (int)(V.size() + OLD_LENGTH));

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    // ---------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "With Exceptions\n");
    {
        static const char *SPECS[] = {
            // spec                              length
            // ----                              ------
            "",                                   // 0
            "A",                                 // 1
            "AB",                                // 2
            "ABC",                               // 3
            "ABCD",                              // 4
            "ABCDEABC",                          // 8
            "ABCDEABCD",                         // 9
            "ABCDEABCDEA",                       // 11
            "ABCDEABCDEAB",                      // 12
            "ABCDEABCDEABC",                     // 13
            "ABCDEABCDEABCDE",                   // 15
            "ABCDEABCDEABCDEABCDEABC",           // 23
            "ABCDEABCDEABCDEABCDEABCD",          // 24
            "ABCDEABCDEABCDEABCDEABCDE",         // 25
            "ABCDEABCDEABCDEABCDEABCDEABCDE",    // 30
            0
        }; // Null string required as last element.

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 9, 11, 12, 13,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int iterationModulus = 1;
        int iteration = 0;
        for (int fromRef = 0; fromRef < 2; ++fromRef) {
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      U_LEN  = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", U_LEN);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)U_LEN);
                uOldLen = static_cast<int>(U_LEN);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, U_LEN == UU.size()); // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      V_LEN   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               V_LEN);
                        P(V_SPEC);
                    }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                    //--------------^
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(Z);  const Obj& U = mU;
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    gg(&mU, U_SPEC);
                    {
                        Obj mV(Z);  const Obj& V = mV;
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ExceptionGuard<Obj> guard(U, L_);
                        testAllocator.setAllocationLimit(AL);

                        const Int64 TDA = defaultAllocator.numAllocations();

                        if (!fromRef) {
                            mU = V; // test assignment here
                        }
                        else {
                            bslstl::StringRefData<TYPE> srd(
                                              V.data(), V.data() + V.length());
                            mU = srd; // test assignment here
                        }
                        guard.release();

                        const Int64 TDB = defaultAllocator.numAllocations();

                        ASSERTV(fromRef, U_SPEC, V_SPEC, TDB - TDA,
                                                                   TDB == TDA);

                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    //--------------v
                                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting self assignment (Aliasing).\n");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int curLen = (int) strlen(SPEC);

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }
            LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
            oldLen = curLen;

            // control
            const Obj X = g(SPEC);
            LOOP_ASSERT(ti, curLen == (int)X.size());  // same lengths

            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(Z);  const Obj& Y = mY;
                    stretchRemoveAll(&mY, N, VALUES[0]);
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    {
                        ExceptionGuard<Obj> guard(Y, L_);
                        mY = Y; // test assignment here
                    }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }

#ifndef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nSkip testing assignment exception-safety.\n");
#else
    if (verbose) printf("\nTesting assignment exception-safety.\n");

    {
        size_t defaultCapacity = Obj().capacity();

        Obj src(defaultCapacity + 1, '1', &testAllocator);

        Obj dst(defaultCapacity, '2', &testAllocator);
        Obj dstCopy(dst, &testAllocator);

        // make the allocator throw on the next allocation
        bsls::Types::Int64 oldLimit = testAllocator.allocationLimit();
        testAllocator.setAllocationLimit(0);

        bool exceptionCaught = false;

        try
        {
            // the assignment will require to allocate more memory
            dst = src;
        }
        catch (bslma::TestAllocatorException &)
        {
            exceptionCaught = true;
        }
        catch (...)
        {
            exceptionCaught = true;
            ASSERT(0 && "Wrong exception caught");
        }

        // restore the allocator state
        testAllocator.setAllocationLimit(oldLimit);

        ASSERT(exceptionCaught);
        ASSERT(dst == dstCopy);
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase9Move()
{
    // --------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1) The value represented by any object can be assigned to any other
    //      object regardless of how either value is represented internally.
    //   2) The 'rhs' value is left usable after the move.
    //   3) 'rhs' going out of scope has no effect on the value of 'lhs' after
    //      the assignment.
    //   4) Aliasing ('x = x'): The assignment operator must always work --
    //      even when the lhs and rhs are identically the same object.
    //   5) The assignment operator must be neutral with respect to memory
    //      allocation exceptions.
    //   6) The move constructor's internal functionality varies according to
    //      which bitwise copy/move trait is applied.
    //
    // Plan:
    //   Specify a set S of unique object values with substantial and varied
    //   differences, ordered by increasing length.  For each value in S,
    //   construct an object x along with a sequence of similarly constructed
    //   duplicates x1, x2, ..., xN.  Attempt to affect every aspect of
    //   white-box state by altering each xi in a unique way.  Let the union of
    //   all such objects be the set T.
    //
    //   To address concerns 1, 2, and 5, construct tests u = v for all (u, v)
    //   in T X T.  Using canonical controls UU and VV, assert before the
    //   assignment that UU == u, VV == v, and v == u if and only if VV == UU.
    //   After the assignment, assert that VV == u, VV == v, and, for grins,
    //   that v == u.  Let v go out of scope and confirm that VV == u.  All of
    //   these tests are performed within the 'bslma' exception testing
    //   apparatus.  Since the execution time is lengthy with exceptions, every
    //   permutation is not performed when exceptions are tested.  Every
    //   permutation is also tested separately without exceptions.
    //
    //   As a separate exercise, we address 4 and 5 by constructing tests y = y
    //   for all y in T.  Using a canonical control X, we will verify that
    //   X == y before and after the assignment, again within the bslma
    //   exception testing apparatus.
    //
    //   To address concern 6, all these tests are performed on user defined
    //   types:
    //          With allocator, copyable
    //          With allocator, moveable
    //          With allocator, not moveable
    //
    // Testing:
    //   basic_string& operator=(MovableRef<basic_string> rhs);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    (void) NUM_VALUES;

    // --------------------------------------------------------------------

    static const char *SPECS[] = {                      // length
        "",                                             // 0
        "A",                                            // 1
        "BC",                                           // 2
        "CDE",                                          // 3
        "DEAB",                                         // 4
        "DEABC",                                        // 5
        "CBAEDCBA",                                     // 8
        "EDCBAEDCB",                                    // 9
        "EDCBAEDCBAE",                                  // 11
        "EDCBAEDCBAED",                                 // 12
        "EDCBAEDCBAEDC",                                // 13
        "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
        "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
        "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
        0 // null string required as last element
    };

    static const int EXTEND[] = {
        0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
    };
    enum { NUM_EXTEND = sizeof(EXTEND) / sizeof(*EXTEND) };

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "Without Exceptions\n");
    { // Testing Move with same allocators
        if (verbose) printf("\twith the same allocators.\n");
        int uOldLen = -1;
        for (int ui = 0; SPECS[ui]; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = strlen(U_SPEC);

            if (veryVerbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
            uOldLen = static_cast<int>(uLen);

            const Obj UU = g(U_SPEC);  // control
            LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

            for (int vi = 0; SPECS[vi]; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = strlen(V_SPEC);

                if (veryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                const Obj VV = g(V_SPEC); // control

                const bool EQUAL = ui == vi; // flag indicating same values

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];
                    for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                        const int V_N = EXTEND[vj];

                        Obj mU(Z);  const Obj& U = mU;
                        stretchRemoveAll(&mU, U_N, VALUES[0]);
                        gg(&mU, U_SPEC);
                        {
                    // v--------
                    Obj mV(Z);  const Obj& V = mV;
                    stretchRemoveAll(&mV, V_N, VALUES[0]);
                    gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, EQUAL == (V == U));

                    const int NUM_CTOR = numCopyCtorCalls;
                    const int NUM_DTOR = numDestructorCalls;

                    Obj& result = (mU = MoveUtil::move(mV));
                        // test move assignment here

                    ASSERT(&result == &U);

                    ASSERT((numCopyCtorCalls - NUM_CTOR) == 0);
                    ASSERT((numDestructorCalls - NUM_DTOR) == 0);

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,
                                 V.empty() || V == VV);
                    // ---------v
                        }
                        // 'mV' (and therefore 'V') now out of scope
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    }
                }
            }
        }
    }
    { // Testing Move with different allocators
        if (verbose) printf("\twith different allocators.\n");
        int uOldLen = -1;
        for (int ui = 0; SPECS[ui]; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = strlen(U_SPEC);

            if (veryVerbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
            uOldLen = static_cast<int>(uLen);

            const Obj UU = g(U_SPEC);  // control
            LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

            for (int vi = 0; SPECS[vi]; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = strlen(V_SPEC);

                if (veryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                const Obj VV = g(V_SPEC); // control

                const bool EQUAL = ui == vi; // flag indicating same values

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];
                    for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                        const int V_N = EXTEND[vj];

                        Obj mU; const Obj& U = mU;
                        stretchRemoveAll(&mU, U_N, VALUES[0]);
                        gg(&mU, U_SPEC);
                        {
                    // v--------
                    Obj mV(Z); const Obj& V = mV;
                    stretchRemoveAll(&mV, V_N, VALUES[0]);
                    gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, EQUAL == (V == U));

                    const int NUM_CTOR = numCopyCtorCalls;
                    const int NUM_DTOR = numDestructorCalls;

                    // test move assignment here
                    mU = MoveUtil::move(mV);

                    ASSERT((numCopyCtorCalls - NUM_CTOR) == 0);
                    ASSERT((numDestructorCalls - NUM_DTOR) == 0);

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    // ---------v
                        }
                        // 'mV' (and therefore 'V') now out of scope
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    }
                }
            }
        }
    }

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "With Exceptions\n");
    {
        static const char *SPECS[] = {
            // spec                              length
            // ----                              ------
            "",                                   // 0
            "A",                                 // 1
            "AB",                                // 2
            "ABC",                               // 3
            "ABCD",                              // 4
            "ABCDEABC",                          // 8
            "ABCDEABCD",                         // 9
            "ABCDEABCDEA",                       // 11
            "ABCDEABCDEAB",                      // 12
            "ABCDEABCDEABC",                     // 13
            "ABCDEABCDEABCDE",                   // 15
            "ABCDEABCDEABCDEABCDEABC",           // 23
            "ABCDEABCDEABCDEABCDEABCD",          // 24
            "ABCDEABCDEABCDEABCDEABCDE",         // 25
            "ABCDEABCDEABCDEABCDEABCDEABCDE",    // 30
            0
        }; // Null string required as last element.

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 9, 11, 12, 13,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int iterationModulus = 1;
        int iteration = 0;
        {
            if (verbose) printf("\tWith the same allocators.\n");
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
                uOldLen = static_cast<int>(uLen);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size()); // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",vLen);
                            P(V_SPEC);
                        }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                //--------------^
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(Z);  const Obj& U = mU;
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    gg(&mU, U_SPEC);
                    {
                        Obj mV(Z); const Obj& V = mV;
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ExceptionGuard<Obj> guard(U, L_);
                        testAllocator.setAllocationLimit(AL);
                        // test move assignment here
                        mU = MoveUtil::move(mV);
                        guard.release();

                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,
                                     V.empty() || VV == V);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                //--------------v
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }

        iterationModulus = 1;
        iteration = 0;
        {
            if (verbose) printf("\tWith different allocators.\n");
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
                uOldLen = static_cast<int>(uLen);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size()); // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",vLen);
                            P(V_SPEC);
                        }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                //--------------^
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(Z);  const Obj& U = mU;
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    gg(&mU, U_SPEC);
                    {
                        Obj mV; const Obj& V = mV;
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ExceptionGuard<Obj> guard(U, L_);
                        testAllocator.setAllocationLimit(AL);
                        // test move assignment here
                        mU = MoveUtil::move(mV);
                        guard.release();

                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                //--------------v
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting self assignment (Aliasing).\n");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int curLen = (int) strlen(SPEC);

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }
            LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
            oldLen = curLen;

            // control
            const Obj X = g(SPEC);
            LOOP_ASSERT(ti, curLen == (int)X.size());  // same lengths

            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(Z);  const Obj& Y = mY;
                    stretchRemoveAll(&mY, N, VALUES[0]);
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    {  // test move assignment here
                        ExceptionGuard<Obj> guard(Y, L_);
                        mY = MoveUtil::move(mY);
                    }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
#ifndef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nSkip testing assignment exception-safety.\n");
#else
    if (verbose) printf("\nTesting assignment exception-safety.\n");

    {
        size_t defaultCapacity = Obj().capacity();

        // 'src' and 'dst' must have different allocators for the exception to
        // be thrown.
        Obj src(defaultCapacity + 1, '1');

        Obj dst(defaultCapacity, '2', &testAllocator);
        Obj dstCopy(dst, &testAllocator);

        // Make the allocator throw on the next allocation:
        bsls::Types::Int64 oldLimit = testAllocator.allocationLimit();
        testAllocator.setAllocationLimit(0);

        bool exceptionCaught = false;

        try
        {
            // the assignment will require to allocate more memory
            dst = MoveUtil::move(src);
        }
        catch (bslma::TestAllocatorException &)
        {
            exceptionCaught = true;
        }
        catch (...)
        {
            exceptionCaught = true;
            ASSERT(0 && "Wrong exception caught");
        }

        // Restore the allocator state.
        testAllocator.setAllocationLimit(oldLimit);

        ASSERT(exceptionCaught);
        ASSERT(dst == dstCopy);
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase9Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1 Assigning a NULL C-string to a string object asserts.
    //
    // Plan:
    //   Construct a string object, then assign a NULL C-string to it and
    //   verify that it asserts.  After that assign a valid C-string and verify
    //   that it succeeds.
    //
    // Testing:
    //   basic_string& operator=(const CHAR_TYPE *);
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj X;

    if (verbose) printf("\tassigning a NULL C-string\n");

    {
        const TYPE *s = 0;
        (void) s; // to disable "unused variable" warning
        ASSERT_SAFE_FAIL(X = s);
    }

    if (verbose) printf("\tassigning a valid C-string\n");

    {
        Obj Y(g("ABC"));
        ASSERT_SAFE_PASS(X = Y.c_str());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase8()
{
    // --------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g:
    //
    // Concerns:
    //   Since 'g' is implemented almost entirely using 'gg', we need to verify
    //   only that the arguments are properly forwarded, that 'g' does not
    //   affect the test allocator, and that 'g' returns an object by value.
    //
    // Plan:
    //   For each SPEC in a short list of specifications, compare the object
    //   returned (by value) from the generator function, 'g(SPEC)' with the
    //   value of a newly constructed OBJECT configured using 'gg(&OBJECT,
    //   SPEC)'.  Compare the results of calling the allocator's
    //   'numBlocksTotal' and 'numBytesInUse' methods before and after calling
    //   'g' in order to demonstrate that 'g' has no effect on the test
    //   allocator.  Finally, use 'sizeof' to confirm that the (temporary)
    //   returned by 'g' differs in size from that returned by 'gg'.
    //
    // Testing:
    //   string g(const char *spec);
    //   string g(size_t len, TYPE seed);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    static const char *SPECS[] = {
        "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0  // null string required as last element
    };

    if (verbose)
        printf("\nCompare values produced by 'g' and 'gg' "
               "for various inputs.\n");

    for (int ti = 0; SPECS[ti]; ++ti) {
        const char *SPEC = SPECS[ti];
        if (veryVerbose) { P_(ti);  P(SPEC); }

        Obj mX(Z);
        gg(&mX, SPEC);  const Obj& X = mX;

        if (veryVerbose) {
            printf("\t g = "); debugprint(g(SPEC)); printf("\n");
            printf("\tgg = "); debugprint(X); printf("\n");
        }
        const Int64 TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
        const Int64 IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, X == g(SPEC));
        const Int64 TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
        const Int64 IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");

    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

              Obj  x(Z);
              Obj& r1 = gg(&x, SPEC);
              Obj& r2 = gg(&x, SPEC);
        const Obj& r3 = g(SPEC);
        const Obj& r4 = g(SPEC);
        ASSERT(&r2 == &r1);
        ASSERT(&x  == &r1);
        ASSERT(&r4 != &r3);
        ASSERT(&x  != &r3);
    }

    if (verbose) printf("\nVerify generator g(size_t len, TYPE seed)\n");

    {
        if (veryVerbose) printf("  Returned string length is correct.\n");

        for (size_t i = 0; i < 200; ++i) {
            ASSERT(g(i, TYPE()).size() == i);
        }

        if (veryVerbose) printf("  Seed produces unique strings.\n");

        // scan the char range, wchar_t can be too wide (32bit)
        Obj  values[CHAR_MAX];
        Obj *values_end = values + CHAR_MAX;

        for (char s = 0; s != CHAR_MAX; ++s) {
            values[s] = g(10, TYPE(s));
        }

        // all values are unique
        std::sort(values, values_end);
        ASSERT(std::adjacent_find(values, values_end) == values_end);
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase7()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //
    // Concerns:
    //   1) copy constructor
    //       1a) The new object's value is the same as that of the original
    //           object (relying on the equality operator) and created with
    //           the correct capacity.
    //       1b) All internal representations of a given value can be used to
    //            create a new object of equivalent value.
    //       1c) The value of the original object is left unaffected.
    //       1d) Subsequent changes in or destruction of the source object have
    //            no effect on the copy-constructed object.
    //       1e) Subsequent changes ('push_back's) on the created object have
    //            no effect on the original and change the capacity of the new
    //            object correctly.
    //       1f) The object has its internal memory management system hooked up
    //            properly so that *all* internally allocated memory draws
    //            from a user-supplied allocator whenever one is specified.
    //       1g) The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //   Specify a set S of object values with substantial and varied
    //   differences, ordered by increasing length, to be used in the following
    //   tests.
    //
    //   For concerns a - d, for each value in S, initialize objects w and x,
    //   copy construct y from x and use 'operator==' to verify that
    //   both x and y subsequently have the same value as w or, in the case
    //   that x is moved-from, x is empty. Let x go out of scope and again
    //   verify that w == y.
    //
    //   For concern e, for each value in S initialize objects w and x, and
    //   copy construct y from x.  Change the state of y, by using the
    //   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //   that y differs from x and w, and verify that the capacity of y changes
    //   correctly.
    //
    //   To address concern f, we will perform tests performed for concern 1:
    //     - While passing a testAllocator as a parameter to the new object
    //       and ascertaining that the new object gets its memory from the
    //       provided testAllocator.  Also perform test for concerns 2 and 5.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concern g, perform tests for concern 1 performed in the
    //   presence of exceptions during memory allocations using a
    //   'bslma::TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   string(const string<C,CT,A>& original);
    //   string(const string<C,CT,A>& original, const A& basicAllocator);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    {
        static const char *SPECS[] = {
            "",                                             // 0
            "A",                                            // 1
            "BC",                                           // 2
            "CDE",                                          // 3
            "DEAB",                                         // 4
            "CBAEDCBA",                                     // 8
            "EDCBAEDCB",                                    // 9
            "EDCBAEDCBAE",                                  // 11
            "EDCBAEDCBAED",                                 // 12
            "EDCBAEDCBAEDC",                                // 13
            "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
            "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
            "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
            0  // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };

        enum { NUM_EXTEND = sizeof(EXTEND) / sizeof(*EXTEND) };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\tFor an object of length " ZU ":\t", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = static_cast<int>(LENGTH);

            // Create control object w.
            Obj mW; const Obj& W = mW;
            gg(&mW, SPEC);

            LOOP_ASSERT(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Stretch capacity of x object by different amounts.

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                const int N = EXTEND[ei];
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                Obj *pX = new Obj(Z);
                Obj& mX = *pX;

                stretchRemoveAll(&mX, N, VALUES[0]);
                const Obj& X = mX;  gg(&mX, SPEC);

                if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

                {   // Testing concern 1a.

                    if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                    const Obj Y0(X);

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, W == X);
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                    LOOP2_ASSERT(SPEC, N, LENGTH <= Y0.capacity());
                }
                {   // Testing concern 1e.

                    if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N + 1; ++i) {
                        const size_t oldCap = Y1.capacity();
                        const size_t remSlots = Y1.capacity() - Y1.size();

                        size_t newCap = 0 != remSlots
                                      ? -1
                                      : Imp::computeNewCapacity(LENGTH + i,
                                                                oldCap,
                                                                Y1.max_size());

                        stretch(&Y1, 1, VALUES[i % NUM_VALUES]);

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y1.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X != Y1);

                        if (oldCap == 0) {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() ==
                                        INITIAL_CAPACITY_FOR_NON_EMPTY_OBJECT);
                        }
                        else if (remSlots == 0) {
                            LOOP5_ASSERT(SPEC, N, i, Y1.capacity(), newCap,
                                         Y1.capacity() == newCap);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == oldCap);
                        }
                    }
                }
                {   // Testing concern 1e with test allocator.

                    if (veryVerbose)
                        printf("\t\t\tInsert into created obj, "
                                "with test allocator:\n");

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(X, AllocType(&testAllocator));  // testing this

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    if (LENGTH <= DEFAULT_CAPACITY) {
                        // Should not allocate when the new string is short,
                        // even when the original string has allocated a buffer
                        // due to previous extension.
                        ASSERTV(SPEC, N, BB, AA, BB + 0 == AA);
                        ASSERTV(SPEC, N,  B,  A,  B + 0 ==  A);
                    }
                    else {
                        // Perform no more than one allocation for a long
                        // string.
                        ASSERTV(SPEC, N, BB, AA, BB + 1 == AA);
                        ASSERTV(SPEC, N,  B,  A,  B + 1 ==  A);
                    }

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y11.capacity();

                        const Int64 CC = testAllocator.numBlocksTotal();
                        const Int64  C = testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const Int64 DD = testAllocator.numBlocksTotal();
                        const Int64  D = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore Append: ");  P_(DD); P(D);
                            printf("\t\t\t\tAfter Append : "); P_(CC); P(C);
                            T_ T_ T_ T_ T_ P_(i); P_(Y11.capacity()); P(Y11);
                        }

                        // Blocks allocated should increase only when trying to
                        // add more than capacity.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        if (oldCap < Y11.capacity()) {
                            if (oldCap == DEFAULT_CAPACITY) {
                                LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 1  == D);
                            }
                            else {
                                LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 0  == D);
                            }
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X != Y11);
                        LOOP3_ASSERT(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                }
                {   // Exception checking.

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(X, AllocType(&testAllocator));
                        if (veryVerbose) {
                            printf("\t\t\tException Case  :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                        LOOP2_ASSERT(SPEC, N,
                                     Y2.get_allocator() == X.get_allocator());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    const size_t initCap = DEFAULT_CAPACITY;

                    if (initCap < LENGTH) {
                        LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    }
                    LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                }
                {                            // with 'original' destroyed
                    Obj Y5(X);
                    if (veryVerbose) {
                        printf("\t\t\tWith Original deleted: \n");
                        printf("\t\t\t\tBefore Delete : "); P(Y5);
                    }

                    delete pX;

                    LOOP2_ASSERT(SPEC, N, W == Y5);

                    for (int i = 1; i < N+1; ++i) {
                        stretch(&Y5, 1, VALUES[i % NUM_VALUES]);
                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append to new obj : ");
                            P_(i);P(Y5);
                        }
                        LOOP3_ASSERT(SPEC, N, i, W != Y5);
                    }
                }
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase7Move()
{
    // --------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTORS:
    //
    // Concerns:
    //   1) move constructor
    //       1a) The new object's value is the same as that of the original
    //           object (relying on the equality operator) and created with
    //           the correct capacity.
    //       1b) All internal representations of a given value can be used to
    //            create a new object of equivalent value.
    //       1c) The value of the original object remains usable.
    //       1d) Subsequent changes in or destruction of the source object have
    //            no effect on the move-constructed object.
    //       1e) Subsequent changes ('push_back's) on the created object have
    //            no effect on the original and change the capacity of the new
    //            object correctly.
    //       1f) The object has its internal memory management system hooked up
    //            properly so that *all* internally allocated memory draws
    //            from a user-supplied allocator whenever one is specified.
    //       1g) The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //   Specify a set S of object values with substantial and varied
    //   differences, ordered by increasing length, to be used in the
    //   following tests.
    //
    //   For concerns a - d, for each value in S, initialize objects w and
    //   x, move construct y from x and use 'operator==' to verify that
    //   both x and y subsequently have the same value as w or, in the case
    //   that x is moved-from, x is empty. Let x go out of scope and again
    //   verify that w == y.
    //
    //   For concern e, for each value in S initialize objects w and x,
    //   and move construct y from x.  Change the state of y, by using the
    //   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //   that y differs from x and w, and verify that the capacity of y
    //   changes correctly.
    //
    //   To address concern f, we will perform tests performed for concern 1:
    //     - While passing a testAllocator as a parameter to the new object
    //       and ascertaining that the new object gets its memory from the
    //       provided testAllocator.  Also perform test for concerns 'a' and
    //       'e'.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concern g, perform tests for concern 1 performed
    //   in the presence of exceptions during memory allocations using a
    //   'bslma::TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   string(MovableRef<string> original);
    //   string(MovableRef<string> original, ALLOC basicAllocator);
    // --------------------------------------------------------------------
    if (verbose) {
        printf("Testing Move Constructor\n");
    }

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    {
        static const char *SPECS[] = {
            "",                                             // 0
            "A",                                            // 1
            "BC",                                           // 2
            "CDE",                                          // 3
            "DEAB",                                         // 4
            "CBAEDCBA",                                     // 8
            "EDCBAEDCB",                                    // 9
            "EDCBAEDCBAE",                                  // 11
            "EDCBAEDCBAED",                                 // 12
            "EDCBAEDCBAEDC",                                // 13
            "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
            "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
            "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
            0  // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\tFor an object of length " ZU ":\t", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = static_cast<int>(LENGTH);

            // Create control object w.
            Obj mW; gg(&mW, SPEC);
            const Obj& W = mW;

            LOOP_ASSERT(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Stretch capacity of x object by different amounts.

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                const int N = EXTEND[ei];
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                {   // Testing concern 1a with implicit allocator.
                    Obj *pX = new Obj(Z);
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    const bool XAllocated = X.capacity() != DEFAULT_CAPACITY;

                    if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

                    if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                    const Int64 B  = testAllocator.numBlocksTotal();
                    const Int64 BB = testAllocator.numBlocksInUse();

                    // Note that 'Obj Y0 = bslmf::...' will not compile on a
                    // strict C++03 compiler due to an ambiguity.  We do not
                    // add a separate C++11 only test for that syntax, as that
                    // would be testing the compiler, rather than the library.
                    Obj Y0(MoveUtil::move(mX));

                    const Int64 A  = testAllocator.numBlocksTotal();
                    const Int64 AA = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, LENGTH <= Y0.capacity());
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                                            X.get_allocator());
                    // If 'X' had a dynamically allocated string, check that
                    // we stole the string and left 'X' empty.  Otherwise,
                    // check that 'X' is unchanged.
                    if (XAllocated) {
                        LOOP2_ASSERT(SPEC, N, X.empty());
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, X == W);
                    }

                    ASSERTV(A,   B,  A == B);
                    ASSERTV(AA, BB, AA == BB);

                    // Testing concern 1d by modifying X.
                    stretch(&mX, 1, VALUES[ei % NUM_VALUES]);
                    LOOP2_ASSERT(SPEC, N, W == Y0);

                    delete pX;

                    // Testing concern 1d after deleting X.
                    LOOP2_ASSERT(SPEC, N, W == Y0);
                }
                {   // Testing concern 1a with specified different allocator.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    bool isShort = X.size() <= DEFAULT_CAPACITY;

                    if (veryVerbose) { printf("\t\t\tRegular Case With "
                                                     "Different Allocators :");
                    }

                    const Int64 B  = testAllocator.numBlocksTotal();
                    const Int64 BB = testAllocator.numBlocksInUse();

                    Obj Y0(MoveUtil::move(mX), &testAllocator);

                    const Int64 A  = testAllocator.numBlocksTotal();
                    const Int64 AA = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N,
                                      Y0.get_allocator() != X.get_allocator());
                    LOOP2_ASSERT(SPEC, N,
                                         Y0.get_allocator() == &testAllocator);

                    // Check that 'X' is left unchanged because it does not
                    // have the same allocator as 'Y0'.
                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, W == X);
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                                               &testAllocator);
                    LOOP2_ASSERT(SPEC, N, LENGTH <= Y0.capacity());

                    if (!isShort) {
                        LOOP2_ASSERT(SPEC, N,  A == B  + 1);
                        LOOP2_ASSERT(SPEC, N, AA == BB + 1);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N,  A == B);
                        LOOP2_ASSERT(SPEC, N, AA == BB);
                    }

                    // Testing concern 1d by modifying X.
                    stretch(&mX, 1, VALUES[ei % NUM_VALUES]);
                    LOOP2_ASSERT(SPEC, N, W == Y0);

                    delete pX;

                    // Testing concern 1d after deleting X.
                    LOOP2_ASSERT(SPEC, N, W == Y0);
                }
                {   // Testing concern 1e.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    if (veryVerbose) printf("\t\t\tMove into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(MoveUtil::move(mX));
                    // Control object for X.
                    Obj X_ctrl(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N + 1; ++i) {
                        const size_t oldCap = Y1.capacity();
                        const size_t remSlots = Y1.capacity() - Y1.size();

                        size_t newCap = 0 != remSlots
                                      ? -1
                                      : Imp::computeNewCapacity(LENGTH + i,
                                                                oldCap,
                                                                Y1.max_size());

                        stretch(&Y1, 1, VALUES[i % NUM_VALUES]);

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y1.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X == X_ctrl);
                        if (remSlots == 0) {
                            LOOP5_ASSERT(SPEC, N, i, Y1.capacity(), newCap,
                                         Y1.capacity() == newCap);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == oldCap);
                        }
                    }

                    delete pX;
                }
                {   // Testing concern 1e with test allocator.
                    Obj *pX = new Obj(Z);
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    if (veryVerbose)
                        printf("\t\t\tMove into created obj, "
                                                     "with test allocator:\n");

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(MoveUtil::move(mX),
                            AllocType(&testAllocator));
                    // Control object for X.
                    Obj X_ctrl(X);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y11.capacity();
                        const size_t initCap = DEFAULT_CAPACITY;

                        const Int64 CC = testAllocator.numBlocksTotal();
                        const Int64  C = testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const Int64 DD = testAllocator.numBlocksTotal();
                        const Int64  D = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore Append: ");  P_(DD); P(D);
                            printf("\t\t\t\tAfter Append : "); P_(CC); P(C);
                            T_ T_ T_ T_ T_ P_(i); P_(Y11.capacity()); P(Y11);
                        }

                        // Blocks allocated should increase only when trying to
                        // add more than capacity.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        if (oldCap < Y11.capacity() && oldCap == initCap) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 1  == D);
                        }
                        else if (oldCap < Y11.capacity()) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0  == D);
                        } else {
                            LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X == X_ctrl);
                        LOOP3_ASSERT(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                    delete pX;
                }
                {   // Testing concern 1e with different allocator.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    bool isShort = X.size() <= DEFAULT_CAPACITY;

                    if (veryVerbose)
                        printf("\t\t\t\tMove into created obj, "
                                "with test allocator:\n");

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(MoveUtil::move(mX), AllocType(&testAllocator));

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    if (isShort) {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 1 ==  A);
                    }

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y11.capacity();
                        const size_t initCap = DEFAULT_CAPACITY;

                        const Int64 CC = testAllocator.numBlocksTotal();
                        const Int64  C = testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const Int64 DD = testAllocator.numBlocksTotal();
                        const Int64  D = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore Append: ");  P_(DD); P(D);
                            printf("\t\t\t\tAfter Append : "); P_(CC); P(C);
                            T_ T_ T_ T_ T_ P_(i); P_(Y11.capacity()); P(Y11);
                        }

                        // Blocks allocated should increase only when trying to
                        // add more than capacity.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        if (oldCap < Y11.capacity() && oldCap == initCap) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 1  == D);
                        }
                        else if (oldCap < Y11.capacity()) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0  == D);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X == W);
                        LOOP3_ASSERT(SPEC, N, i,
                                         Y11.get_allocator() ==&testAllocator);
                    }
                    delete pX;
                }
                {   // Exception checking with same allocators.
                    Obj *pX = new Obj(Z);
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    bool XAllocated = X.capacity() != DEFAULT_CAPACITY;

                    AllocType testA = &testAllocator;
                    ASSERT(X.get_allocator() == testA);
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(MoveUtil::move(mX),
                                     AllocType(&testAllocator));
                        if (veryVerbose) {
                            printf("\t\t\tException Case with same "
                                                             "allocators :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N,
                                     Y2.get_allocator() == X.get_allocator());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    if (XAllocated) {
                        LOOP2_ASSERT(SPEC, N,  B - 1 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                    }

                    delete pX;
                }
                {   // Exception checking with different allocators.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    bool isShort = X.size() <= DEFAULT_CAPACITY;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(MoveUtil::move(mX),
                                     AllocType(&testAllocator));
                        if (veryVerbose) {
                            printf("\t\t\tException Case with "
                                                   "different allocators :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N,
                                          Y2.get_allocator() ==&testAllocator);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    if (isShort) {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                    }
                    LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);

                    delete pX;
                }
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    //
    // Concerns:
    //   1 Objects constructed with the same values are returned as equal.
    //   2 Objects constructed such that they have same (logical) value but
    //     different internal representation (due to the lack or presence
    //     of an allocator, and/or different capacities) are always returned
    //     as equal.
    //   3 Unequal objects are always returned as unequal.
    //   4 Equality comparisons with 'const CHAR_TYPE *' yield the same results
    //     as equality comparisons with 'string' objects.
    //   5 Correctly selects the 'bitwiseEqualityComparable' traits.
    //
    // Plan:
    //   For concerns 1 and 3, Specify a set A of unique allocators including
    //   no allocator.  Specify a set S of unique object values having various
    //   minor or subtle differences, ordered by non-decreasing length.
    //   Verify the correctness of 'operator==' and 'operator!=' (returning
    //   either true or false) using all elements (u, ua, v, va) of the
    //   cross product S X A X S X A.
    //
    //   For concern 2 create two objects using all elements in S one at a
    //   time.  For the second object change its internal representation by
    //   extending it by different amounts in the set E, followed by erasing
    //   its contents using 'clear'.  Then recreate the original value and
    //   verify that the second object still return equal to the first.
    //
    //   For concern 4, test equality operators taking 'const CHAR_TYPE *'
    //   parameters right after equality comparisons of 'string' objects have
    //   been verified to perform correctly.
    //
    //   For concern 5, we instantiate this test driver on a test type having
    //   allocators or not, and possessing the bitwise-equality-comparable
    //   trait or not.
    //
    // Testing:
    //   operator==(const string<C,CT,A>&, const string<C,CT,A>&);
    //   operator==(const C *, const string<C,CT,A>&);
    //   operator==(const string<C,CT,A>&, const C *);
    //   operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
    //   operator!=(const C *, const string<C,CT,A>&);
    //   operator!=(const string<C,CT,A>&, const C *);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    Allocator *AllocType[] = {
        new Allocator(&testAllocator1),
        new Allocator(&testAllocator2)
    };

    const int NUM_AllocType = sizeof AllocType / sizeof *AllocType;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const char *SPECS[] = {
        "",
        "A",      "B",
        "AA",     "AB",     "BB",     "BA",
        "AAA",    "BAA",    "ABA",    "AAB",
        "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
        "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
        "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
        "AAAAAAA",          "BAAAAAA",          "AAAAABA",
        "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
        "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
        "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
        "AAAAAAAAAAA",      "AAAABAAAAAA",      "AAAAABAAAAA",
        "AAAAAAAAAAAA",     "AAAABAAAAAAA",     "AAAAABAAAAAA",
        "AAAAAAAAAAAAA",    "AAAABAAAAAAAA",    "AAAAABAAAAAAA",
        "AAAAAAAAAAAAAA",   "AAAABAAAAAAAAA",   "AAAAABAAAAAAAA",
        "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA",
        0  // null string required as last element
    };

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {

        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_AllocType; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(*AllocType[ai]); const Obj& U = gg(&mU, U_SPEC);
                LOOP2_ASSERT(si, ai, LENGTH == static_cast<int>(U.size()));
                                                                // same lengths

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n", LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= LENGTH);//non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) { T_; T_;
                    P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_AllocType; ++aj) {

                        const char *const V_SPEC = SPECS[sj];
                        Obj mV(*AllocType[aj]);
                        const Obj& V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            T_; T_; P_(sj); P_(V_SPEC); P(V);
                        }

                        // First test comparisons with 'string'
                        const bool isSame = si == sj;
                        LOOP2_ASSERT(si, sj,  isSame == (U == V));
                        LOOP2_ASSERT(si, sj, !isSame == (U != V));

                        // And then with 'const CHAR_TYPE *'
                        LOOP2_ASSERT(si, sj,  isSame == (U == V.c_str()));
                        LOOP2_ASSERT(si, sj, !isSame == (U != V.c_str()));

                        LOOP2_ASSERT(si, sj,  isSame == (U.c_str() == V));
                        LOOP2_ASSERT(si, sj, !isSame == (U.c_str() != V));
                    }
                }
            }
        }
    }

    if (verbose) printf("\nCompare each pair of similar values (u, ua, v, va)"
                        " in S X A X S X A after perturbing.\n");
    {
        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_AllocType; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const size_t    LENGTH = strlen(U_SPEC);

                Obj mU(*AllocType[ai]); const Obj& U = mU;
                gg(&mU, U_SPEC);
                LOOP_ASSERT(si, LENGTH == U.size());  // same lengths

                if ((int)LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length " ZU ".\n",
                                LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= (int)LENGTH);
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_AllocType; ++aj) {
                        //Perform perturbation
                        for (int e = 0; e < NUM_EXTEND; ++e) {

                            const char *const V_SPEC = SPECS[sj];
                            Obj mV(*AllocType[aj]); const Obj& V = mV;
                            gg(&mV, V_SPEC);

                            stretchRemoveAll(&mV, EXTEND[e],
                                             VALUES[e % NUM_VALUES]);
                            gg(&mV, V_SPEC);

                            if (veryVerbose) {
                                T_; T_; P_(sj); P_(V_SPEC); P(V);
                            }

                            // First test comparisons with 'string'
                            const bool isSame = si == sj;
                            LOOP2_ASSERT(si, sj,  isSame == (U == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V));

                            // And then with 'const CHAR_TYPE *'
                            LOOP2_ASSERT(si, sj,  isSame == (U == V.c_str()));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V.c_str()));

                            LOOP2_ASSERT(si, sj,  isSame == (U.c_str() == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U.c_str() != V));
                        }
                    }
                }
            }
        }
    }

    delete AllocType[0];
    delete AllocType[1];
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase6Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING EQUALITY OPERATORS
    //
    // Concerns:
    //   Equality comparison operators taking C-string pointer parameters
    //   assert on undefined behavior when the pointer is NULL.
    //
    // Plan:
    //   For each equality comparison operator, create a non-empty string and
    //   use it to test equality comparison operators by passing NULL to
    //   C-string pointer parameters.
    //
    // Testing:
    //   operator==(const C *s, const string<C,CT,A>& str);
    //   operator==(const string<C,CT,A>& str, const C *s);
    //   operator!=(const C *s, const string<C,CT,A>& str);
    //   operator!=(const string<C,CT,A>& str, const C *s);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj mX(g("ABCDE"));
    const Obj& X = mX;

    TYPE *nullStr = NULL;
    // disable "unused variable" warning in non-safe mode:
#if !defined BSLS_ASSERT_SAFE_IS_ACTIVE
    (void) nullStr;
#endif

    if (veryVerbose) printf("\toperator==(s, str)\n");

    {
        ASSERT_SAFE_FAIL(if(nullStr == X){});
        ASSERT_SAFE_PASS(if(X.c_str() == X){});
    }

    if (veryVerbose) printf("\toperator==(str, s)\n");

    {
        ASSERT_SAFE_FAIL(if(X == nullStr){});
        ASSERT_SAFE_PASS(if(X == X.c_str()){});
    }

    if (veryVerbose) printf("\toperator!=(s, str)\n");

    {
        ASSERT_SAFE_FAIL(if(nullStr != X){});
        ASSERT_SAFE_PASS(if(X.c_str() != X){});
    }

    if (veryVerbose) printf("\toperator==(str, s)\n");

    {
        ASSERT_SAFE_FAIL(if(X != nullStr){});
        ASSERT_SAFE_PASS(if(X != X.c_str()){});
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase4()
{
    // --------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    //
    // Concerns:
    //   1) The returned value for operator[] and function at() is correct
    //      as long as pos < size(), and 'operator[] const' returns C() for
    //      'pos == size()'.
    //   2) The at() function throws out_of_range exception if
    //      pos >= size().
    //   3) The elements stored at indices 0 up to size() - 1 are stored
    //      contiguously.
    //   4) Changing the internal representation to get the same (logical)
    //      final value, should not change the result of the element
    //      accessor functions.
    //   5) The internal memory management is correctly hooked up so that
    //      changes made to the state of the object via these accessors
    //      do change the state of the object.
    //
    // Plan:
    //   For 1 and 4 do the following:
    //   Specify a set S of representative object values ordered by
    //   increasing length.  For each value w in S, initialize a newly
    //   constructed object x with w using 'gg' and verify that each basic
    //   accessor returns the expected result.  Reinitialize and repeat
    //   the same test on an existing object y after perturbing y so as to
    //   achieve an internal state representation of w that is potentially
    //   different from that of x.

    //   For 3, using the object values of the previous experiment, verify that
    //   the array of 'size()' beginning at '&X[0]' is identical in contents to
    //   the object value.
    //
    //   For 2, check that function at() throws a out_of_range exception
    //   when pos >= size().
    //
    //   For 5, For each value w in S, create a object x with w using
    //   'gg'.  Create another empty object y and make it 'resize' capacity
    //   equal to the size of x.  Now using the element accessor functions
    //   recreate the value of x in y.  Verify that x == y.
    //   Note - Using untested resize(int).
    //
    // Testing:
    //   size_type size() const;
    //   reference operator[](size_type pos);
    //   const_reference operator[](size_type pos) const;
    //   reference at(size_type pos);
    //   const_reference at(size_type pos) const;
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    Allocator *AllocType[] = {
        new Allocator(&testAllocator),
        new Allocator(&testAllocator1),
        new Allocator(&testAllocator2)
    };

    enum { NUM_AllocType = sizeof AllocType / sizeof *AllocType };

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const size_t MAX_LENGTH = 32;

    static const struct {
        int         d_lineNum;                   // source line number
        const char *d_spec_p;                    // specification string
        int         d_length;                    // expected length
        char        d_elements[MAX_LENGTH + 1];  // expected element values
    } DATA[] = {
        //line  spec            length  elements
        //----  --------------  ------  ------------------------
        { L_,   "",                  0, { }                     },
        { L_,   "A",                 1, { VA }                  },
        { L_,   "B",                 1, { VB }                  },
        { L_,   "AB",                2, { VA, VB }              },
        { L_,   "BC",                2, { VB, VC }              },
        { L_,   "BCA",               3, { VB, VC, VA }          },
        { L_,   "CAB",               3, { VC, VA, VB }          },
        { L_,   "CDAB",              4, { VC, VD, VA, VB }      },
        { L_,   "DABC",              4, { VD, VA, VB, VC }      },
        { L_,   "ABCDE",             5, { VA, VB, VC, VD, VE }  },
        { L_,   "EDCBA",             5, { VE, VD, VC, VB, VA }  },
        { L_,   "ABCDEA",            6, { VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "ABCDEAB",           7, { VA, VB, VC, VD, VE,
                                          VA, VB }              },
        { L_,   "BACDEABC",          8, { VB, VA, VC, VD, VE,
                                          VA, VB, VC }          },
        { L_,   "CBADEABCD",         9, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD }      },
        { L_,   "CBADEABCDAB",      11, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABC",     12, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC }              },
        { L_,   "CBADEABCDABCDE",   14, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE }      },
        { L_,   "CBADEABCDABCDEA",  15, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA }  },
        { L_,   "CBADEABCDABCDEAB", 16, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA", 31,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEAB", 32,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA, VB }              }
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting operator[] and function at(),"
                        " where pos < size().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;

            Obj mExp;
            const Obj& EXP = gg(&mExp, e);   // expected spec

            ASSERT(LENGTH <= MAX_LENGTH);

            for (int ai = 0; ai < NUM_AllocType; ++ai) {
                Obj mX(*AllocType[ai]);

                const Obj& X = gg(&mX, SPEC);    // canonical organization

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf( "\ton objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                size_t i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX.at(i));
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X.at(i));

                    LOOP3_ASSERT(LINE, ai, i, &mX[0] + i == &mX[i]);
                    LOOP3_ASSERT(LINE, ai, i, &X[0] + i == &X[i]);
                }
                LOOP2_ASSERT(LINE, ai, &mX[0] == &X[0]);
                LOOP2_ASSERT(LINE, ai, TYPE() == X[LENGTH]);

                for (; i < MAX_LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, 0 == e[i]);
                }

                // Check for perturbation.
                static const int EXTEND[] = {
                    0, 1, 2, 3, 4, 5, 7, 8, 9,
                    DEFAULT_CAPACITY - 1,
                    DEFAULT_CAPACITY,
                    DEFAULT_CAPACITY + 1,
                    DEFAULT_CAPACITY * 5
                };

                enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

                Obj mY(*AllocType[ai]);

                const Obj& Y = gg(&mY, SPEC);

                {                             // Perform the perturbation
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                        stretchRemoveAll(&mY, EXTEND[ei],
                                         VALUES[ei % NUM_VALUES]);
                        gg(&mY, SPEC);

                        if (veryVerbose) { T_; T_; T_; P(Y); }

                        size_t j;
                        for (j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == mY[j]);
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == Y[j]);
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == mY.at(j));
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == Y.at(j));
                        }

                        for (; j < MAX_LENGTH; ++j) {
                            LOOP4_ASSERT(LINE, ai, j, ei, 0 == e[j]);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting non-const operator[] and "
                        "function at() modify state of object correctly.\n");
    {

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;

            for (int ai = 0; ai < NUM_AllocType; ++ai) {
                Obj mX(*AllocType[ai]);

                const Obj& X = gg(&mX, SPEC);

                LOOP2_ASSERT(LINE, ai, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf("\tOn objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                Obj mY(*AllocType[ai]); const Obj& Y = mY;
                Obj mZ(*AllocType[ai]); const Obj& Z = mZ;

                mY.resize(LENGTH);
                mZ.resize(LENGTH);

                // When '0 == LENGTH', all have the default value.  Otherwise,
                // we should observe a change of value after the assignments.

                LOOP2_ASSERT(LINE, ai, !LENGTH || Y != X);
                LOOP2_ASSERT(LINE, ai, !LENGTH || Z != X);

                // Change state of Y and Z so its same as X

                for (size_t j = 0; j < LENGTH; j++) {
                    mY[j]    = TYPE(e[j]);
                    mZ.at(j) = TYPE(e[j]);
                }

                if (veryVerbose) {
                    printf("\t\tNew object1: "); P(Y);
                    printf("\t\tNew object2: "); P(Z);
                }

                LOOP2_ASSERT(LINE, ai, Y == X);
                LOOP2_ASSERT(LINE, ai, Z == X);
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tTesting for out_of_range exceptions thrown"
                        " by at() when pos >= size().\n");
    {

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE    = DATA[ti].d_lineNum;
            const char *const SPEC    = DATA[ti].d_spec_p;
            const size_t      LENGTH  = DATA[ti].d_length;

            for (int ai = 0; ai < NUM_AllocType; ++ai) {
                int exceptions, trials;

                const int NUM_TRIALS = 2;

                // Check exception behavior for non-const version of at()
                // Checking the behavior for 'pos == size()' and
                // 'pos > size()'.

                for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                               ; ++trials) {
                    try {
                        Obj mX(*AllocType[ai]);
                        gg(&mX, SPEC);
                        mX.at(LENGTH + trials);
                    } catch (std::out_of_range) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("In out_of_range exception.\n");
                            P_(LINE); P(trials);
                        }
                        continue;
                    }
                }

                ASSERT(exceptions == trials);

                // Check exception behavior for const version of at()
                for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                               ; ++trials) {

                    try {
                        Obj mX(*AllocType[ai]);
                        const Obj& X = gg(&mX, SPEC);
                        X.at(LENGTH + trials);
                    } catch (std::out_of_range) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("In out_of_range exception." );
                            P_(LINE); P(trials);
                        }
                        continue;
                    }
                }

                ASSERT(exceptions == trials);
            }
        }
    }
#endif // BDE_BUILD_TARGET_EXC

    delete AllocType[0];
    delete AllocType[1];
    delete AllocType[2];
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase3()
{
    // --------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg':
    //
    // Concerns:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions, we want to verify (1) that valid
    //   generator syntax produces expected results and (2) that invalid
    //   syntax is detected and reported.
    //
    // Plan:
    //   For each of an enumerated sequence of 'spec' values, ordered by
    //   increasing 'spec' length, use the primitive generator function
    //   'gg' to set the state of a newly created object.  Verify that 'gg'
    //   returns a valid reference to the modified argument object and,
    //   using basic accessors, that the value of the object is as
    //   expected.  Repeat the test for a longer 'spec' generated by
    //   prepending a string ending in a '~' character (denoting
    //   'clear').  Note that we are testing the parser only; the
    //   primary manipulators are already assumed to work.
    //
    //   For each of an enumerated sequence of 'spec' values, ordered by
    //   increasing 'spec' length, use the primitive generator function
    //   'ggg' to set the state of a newly created object.  Verify that
    //   'ggg' returns the expected value corresponding to the location of
    //   the first invalid value of the 'spec'.  Repeat the test for a
    //   longer 'spec' generated by prepending a string ending in a '~'
    //   character (denoting 'clear').  Note that we are testing the
    //   parser only; the primary manipulators are already assumed to work.
    //
    // Testing:
    //   string<C,CT,A>& gg(string<C,CT,A> *object, const char *spec);
    //   int ggg(string<C,CT,A> *object, const char *spec, int vF = 1);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int MAX_LENGTH = 10;
        static const struct {
            int         d_lineNum;               // source line number
            const char *d_spec_p;                // specification string
            int         d_length;                // expected length
            char        d_elements[MAX_LENGTH];  // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",             0,      { 0 }                   },

            { L_,   "A",            1,      { VA }                  },
            { L_,   "B",            1,      { VB }                  },
            { L_,   "~",            0,      { 0 }                   },

            { L_,   "CD",           2,      { VC, VD }              },
            { L_,   "E~",           0,      { 0 }                   },
            { L_,   "~E",           1,      { VE }                  },
            { L_,   "~~",           0,      { 0 }                   },

            { L_,   "ABC",          3,      { VA, VB, VC }          },
            { L_,   "~BC",          2,      { VB, VC }              },
            { L_,   "A~C",          1,      { VC }                  },
            { L_,   "AB~",          0,      { 0 }                   },
            { L_,   "~~C",          1,      { VC }                  },
            { L_,   "~B~",          0,      { 0 }                   },
            { L_,   "A~~",          0,      { 0 }                   },
            { L_,   "~~~",          0,      { 0 }                   },

            { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
            { L_,   "~BCD",         3,      { VB, VC, VD }          },
            { L_,   "A~CD",         2,      { VC, VD }              },
            { L_,   "AB~D",         1,      { VD }                  },
            { L_,   "ABC~",         0,      { 0 }                   },

            { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
            { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
            { L_,   "AB~DE",        2,      { VD, VE }              },
            { L_,   "ABCD~",        0,      { 0 }                   },
            { L_,   "A~C~E",        1,      { VE }                  },
            { L_,   "~B~D~",        0,      { 0 }                   },

            { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

            { L_,   "ABCDE~CDEC~E", 1,      { VE }                  }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t    LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;
            const int         curLen = (int)strlen(SPEC);

            Obj mX(Z);
            const Obj& X = gg(&mX, SPEC);   // original spec

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj mY(Z);
            const Obj& Y = gg(&mY, buf);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                printf("\t\tBigSpec = \"%s\"\n", buf);
                T_; T_; T_; P(X);
                T_; T_; T_; P(Y);
            }

            LOOP_ASSERT(LINE, LENGTH == X.size());
            LOOP_ASSERT(LINE, LENGTH == Y.size());
            for (size_t i = 0; i < LENGTH; ++i) {
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == X[i]);
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == Y[i]);
            }

        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec            index
            //----  -------------   -----
            { L_,   "",             -1,     }, // control

            { L_,   "~",            -1,     }, // control
            { L_,   " ",             0,     },
            { L_,   ".",             0,     },
            { L_,   "L",             -1,    }, // control
            { L_,   "M",             0,     },
            { L_,   "Z",             0,     },

            { L_,   "AE",           -1,     }, // control
            { L_,   "aE",            0,     },
            { L_,   "Ae",            1,     },
            { L_,   ".~",            0,     },
            { L_,   "~!",            1,     },
            { L_,   "  ",            0,     },

            { L_,   "ABC",          -1,     }, // control
            { L_,   " BC",           0,     },
            { L_,   "A C",           1,     },
            { L_,   "AB ",           2,     },
            { L_,   "?#:",           0,     },
            { L_,   "   ",           0,     },

            { L_,   "ABCDE",        -1,     }, // control
            { L_,   "aBCDE",         0,     },
            { L_,   "ABcDE",         2,     },
            { L_,   "ABCDe",         4,     },
            { L_,   "AbCdE",         1,     }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(Z);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                // LOOP_ASSERT(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = static_cast<int>(LENGTH);
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            LOOP_ASSERT(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase2()
{
    // --------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //
    // Concerns:
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - push_back             (black-box)
    //      - clear                 (white-box)
    //   operate as expected.  We have the following specific concerns:
    //    1) The default constructor
    //        1a) creates the correct initial value.
    //        1b) does *not* allocate memory.
    //        1c) has the internal memory management system hooked up
    //              properly so that *all* internally allocated memory
    //              draws from the same user-supplied allocator whenever
    //              one is specified.
    //    2) The destructor properly deallocates all allocated memory to
    //         its corresponding allocator from any attainable state.
    //    3) 'push_back'
    //        3a) produces the expected value.
    //        3b) increases capacity as needed.
    //        3c) maintains valid internal state.
    //        3d) is exception neutral with respect to memory allocation.
    //    4) 'clear'
    //        4a) produces the expected value (empty).
    //        4b) properly destroys each contained element value.
    //        4c) maintains valid internal state.
    //        4d) does not allocate memory.
    //    5) The size based parameters of the class reflect the platform.
    //
    // Plan:
    //   To address concerns 1a - 1c, create an object using the default
    //   constructor:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concerns 3a - 3c, construct a series of independent
    //   objects, ordered by increasing length.  In each test, allow the
    //   object to leave scope without further modification, so that the
    //   destructor asserts internal object invariants appropriately.
    //   After the final insert operation in each test, use the (untested)
    //   basic accessors to cross-check the value of the object
    //   and the 'bslma::TestAllocator' to confirm whether a resize has
    //   occurred.
    //
    //   To address concerns 4a-4c, construct a similar test, replacing
    //   'push_back' with 'clear'; this time, however, use the test
    //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //
    //   To address concerns 2, 3d, 4d, create a small "area" test that
    //   exercises the construction and destruction of objects of various
    //   lengths and capacities in the presence of memory allocation
    //   exceptions.  Two separate tests will be performed.
    //
    //   Let S be the sequence of integers { 0 .. N - 1 }.
    //      (1) for each i in S, use the default constructor and 'push_back'
    //          to create an instance of length i, confirm its value (using
    //           basic accessors), and let it leave scope.
    //      (2) for each (i, j) in S X S, use 'push_back' to create an
    //          instance of length i, use 'clear' to clear its value
    //          and confirm (with 'length'), use insert to set the instance
    //          to a value of length j, verify the value, and allow the
    //          instance to leave scope.
    //
    //   The first test acts as a "control" in that 'clear' is not
    //   called; if only the second test produces an error, we know that
    //   'clear' is to blame.  We will rely on 'bslma::TestAllocator'
    //   and purify to address concern 2, and on the object invariant
    //   assertions in the destructor to address concerns 3d and 4d.
    //
    //   To address concern 5, the values will be explicitly compared to
    //   the expected values.  This will be done first so as to ensure all
    //   other tests are reliable and may depend upon the class's
    //   constants.
    //
    // Testing:
    //   basic_string(const ALLOC& a = ALLOC());
    //   ~basic_string();
    //   void push_back(CHAR_TYPE c);
    //   void clear();
    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting default ctor (thoroughly).\n");

    if (verbose) printf("\t\tWithout passing in an allocator.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const Obj X;
        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        if (1 == sizeof(TYPE)) {
            ASSERTV(k_SHORT_BUFFER_CAPACITY_CHAR,
                    X.capacity(),
                    k_SHORT_BUFFER_CAPACITY_CHAR == X.capacity());
        }
        else if (sizeof(wchar_t) == sizeof(TYPE)) {
            ASSERTV(k_SHORT_BUFFER_CAPACITY_WCHAR_T,
                    X.capacity(),
                    k_SHORT_BUFFER_CAPACITY_WCHAR_T == X.capacity());
        }

        // Verify no allocation from default allocators
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    if (verbose) printf("\t\tPassing in an allocator.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("object",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Allocator Z(&oa);

        const Obj X(Z);

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        // Verify no allocation from default and object allocators.
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(oa.numBlocksTotal(), 0 == oa.numBlocksTotal());
        ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
    }

    if (verbose) printf("\t\tIn place using a test allocator.\n");
    {
        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());

        Obj x(objectAllocator_p);

        // Verify no allocation from global, default and object allocators.
        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());
    }
    ASSERT(0 == globalAllocator_p->numBytesInUse());
    ASSERT(0 == defaultAllocator_p->numBytesInUse());
    ASSERT(0 == objectAllocator_p->numBytesInUse());

    // --------------------------------------------------------------------
    bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tBEFORE: "); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            if(veryVerbose){
                printf("\t\t\tAFTER: "); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) printf("\t\tOn an object of initial length " ZU ".\n",
                                li);

            Obj mX(Z);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            // LOOP_ASSERT(li, BB + NUM_ALLOCS[li + 1] - NUM_ALLOCS[li] == AA);
            // LOOP_ASSERT(li, B + 1 == A);

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }
            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) printf("\t\tOn an object of initial length " ZU ".\n",
                                li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            if(veryVerbose){
                printf("\t\t\tBEFORE ");
                P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            mX.clear();

            if(veryVerbose){
                printf("\t\t\tAFTER ");
                P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT ");
                P_(X.capacity()); P(X);
            }
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) printf("\t\tOn an object of initial length " ZU ".\n",
                                li);

            Obj mX(Z);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B);
                typename Obj::size_type Cap = X.capacity();P_(Cap);P(X);
            }

            mX.clear();

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 CC = testAllocator.numBlocksTotal();
            const Int64 C  = testAllocator.numBlocksInUse();

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            LOOP_ASSERT(li, BB == AA);
            LOOP_ASSERT(li, BB == CC);

            LOOP_ASSERT(li, B  == A);
            LOOP_ASSERT(li, B  == C);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'push_back' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an instance
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the instance to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose) printf("\t\t\tOn an object of length " ZU ".\n", li);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

            Obj mX(Z);  const Obj& X = mX;                           // 1.
            for (size_t i = 0; i < li; ++i) {                        // 2.
                ExceptionGuard<Obj> guard(X, L_);
                mX.push_back(VALUES[i % NUM_VALUES]);
                guard.release();
            }

            LOOP_ASSERT(li, li == X.size());                         // 3.
            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                   // 4.
          LOOP_ASSERT(li, 0 == testAllocator.numBlocksInUse());      // 5.
        }
    }

    if (verbose) printf("\t\tWith 'push_back' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1) create an instance
        //    2) insert V0 values up to a length of i
        //    3) verify initial length and contents
        //    4) clear contents from instance
        //    5) verify length is 0
        //    6) insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7) verify new length and contents
        //    8) allow the instance to leave scope
        //    9) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
            if (verbose)
                printf("\t\t\tOn an object of initial length " ZU ".\n", i);

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVerbose)
                    printf("\t\t\t\tAnd with final length " ZU ".\n", j);

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                size_t k; // loop index

                Obj mX(Z);  const Obj& X = mX;         // 1.
                for (k = 0; k < i; ++k) {                           // 2.
                    ExceptionGuard<Obj> guard(X, L_);
                    mX.push_back(VALUES[0]);
                    guard.release();
                }

                LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                for (k = 0; k < i; ++k) {
                    LOOP3_ASSERT(i, j, k, VALUES[0] == X[k]);
                }

                mX.clear();                                         // 4.
                LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                for (k = 0; k < j; ++k) {                           // 6.
                    ExceptionGuard<Obj> guard(X, L_);
                    mX.push_back(VALUES[k % NUM_VALUES]);
                    guard.release();
                }

                LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                for (k = 0; k < j; ++k) {
                    LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END              // 8.
              LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());  // 9.
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //
    // Concerns:
    //   We want to exercise basic value-semantic functionality.  In
    //   particular we want to demonstrate a base-line level of correct
    //   operation of the following methods and operators:
    //      - default and copy constructors (and also the destructor)
    //      - the assignment operator (including aliasing)
    //      - equality operators: 'operator==' and 'operator!='
    //      - primary manipulators: 'push_back' and 'clear' methods
    //      - basic accessors: 'size' and 'operator[]'
    //   In addition we would like to exercise objects with potentially
    //   different internal organizations representing the same value.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators, basic
    //   accessors, equality operators, and the assignment operator.
    //   Invoke the primary manipulator [1&5], copy constructor [2&8], and
    //   assignment operator [9&10] in situations where the internal data
    //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
    //   assignment for a non-empty instance [11] and allow the result to
    //   leave scope, enabling the destructor to assert internal object
    //   invariants.  Display object values frequently in verbose mode:
    //
    // 1) Create an object x1 (default ctor).         { x1: }
    // 2) Create a second object x2 (copy from x1).   { x1: x2: }
    // 3) Append an element value A to x1).           { x1:A x2: }
    // 4) Append the same element value A to x2).     { x1:A x2:A }
    // 5) Append another element value B to x2).      { x1:A x2:AB }
    // 6) Remove all elements from x1.                { x1: x2:AB }
    // 7) Create a third object x3 (default ctor).    { x1: x2:AB x3: }
    // 8) Create a forth object x4 (copy of x2).      { x1: x2:AB x3: x4:AB }
    // 9) Assign x2 = x1 (non-empty becomes empty).   { x1: x2: x3: x4:AB }
    // 10) Assign x3 = x4 (empty becomes non-empty).  { x1: x2: x3:AB x4:AB }
    // 11) Assign x4 = x4 (aliasing).                 { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    (void) NUM_VALUES;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(Z);  const Obj& X1 = mX1;
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

    if(veryVerbose){
        typename Obj::size_type capacity = X1.capacity();
        T_; T_;
        P(capacity);
    }

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");
    Obj mX2(X1, AllocType(&testAllocator));  const Obj& X2 = mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check the initial state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 3) Append an element value A to x1)."
                        "\t\t\t{ x1:A x2: }\n");
    mX1.push_back(VALUES[0]);
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(1 == X1.size());
    ASSERT(VALUES[0] == X1[0]);

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 4) Append the same element value A to x2)."
                         "\t\t{ x1:A x2:A }\n");
    mX2.push_back(VALUES[0]);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(1 == X2.size());
    ASSERT(VALUES[0] == X2[0]);

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5) Append another element value B to x2)."
                         "\t\t{ x1:A x2:AB }\n");
    mX2.push_back(VALUES[1]);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(2 == X2.size());
    ASSERT(VALUES[0] == X2[0]);
    ASSERT(VALUES[1] == X2[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(!(X2 == X1));          ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 6) Remove all elements from x1."
                         "\t\t\t{ x1: x2:AB }\n");
    mX1.clear();
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(0 == X1.size());

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 7) Create a third object x3 (default ctor)."
                         "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3(Z);  const Obj& X3 = mX3;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(0 == X3.size());

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3.\n");
    ASSERT(  X3 == X1 );          ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                         "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2, AllocType(&testAllocator));  const Obj& X4 = mX4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");

    ASSERT(2 == X4.size());
    ASSERT(VALUES[0] == X4[0]);
    ASSERT(VALUES[1] == X4[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );          ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));          ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2, x3, x4.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );          ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));          ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(2 == X3.size());
    ASSERT(VALUES[0] == X3[0]);
    ASSERT(VALUES[1] == X3[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X3 == X1));          ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );          ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n11) Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");
    ASSERT(2 == X4.size());
    ASSERT(VALUES[0] == X4[0]);
    ASSERT(VALUES[1] == X4[1]);

    if (verbose)
        printf("\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCaseM1(const int /* NITER */,
                                               const int /* RANDOM_SEED */)
{
    // --------------------------------------------------------------------
    // PERFORMANCE TEST
    //
    // Concerns:
    //   1) That performance does not regress between versions.
    //   2) That no surprising performance (both extremely fast or slow) is
    //      detected, which might be indicating missed optimizations or
    //      inadvertent loss of performance (e.g., by wrongly setting the
    //      capacity and triggering too frequent reallocations).
    //   3) That small "improvements" can be tested w.r.t. to performance, in a
    //      uniform benchmark (e.g., measuring the overhead of allocating for
    //      empty strings).
    //
    // Plan:
    //   We follow a simple benchmark that performs the operation under a
    //   timing test in a loop.  Specifically, we wish to measure the time
    //   taken by:
    //     C1) The various constructors.
    //     C2) The copy constructor.
    //     A1) The copy assignment.
    //     A2) The 'assign' operations.
    //     P1) The 'append' operation in its various forms (including
    //         'push_back').
    //     I1) The 'insert' operation in its various forms, at the end (should
    //         be compared to the corresponding 'append' sequence).
    //     B1) The 'insert' operation in its various forms.
    //     M3) The 'replace' operation in its various forms.
    //     S1) The 'swap' operation in its various forms.
    //     F1) The 'find' and 'rfind' operations.
    //     F2) The 'find_first_of' and 'find_last_of' operations.
    //     F3) The 'find_first_not_of' and 'find_last_not_of' operations.
    //   Also we wish to record the size of the various string
    //   instantiations.
    //
    //   We create two tables, one containing long strings, and another
    //   containing short strings.  All strings are preallocated so that we do
    //   not measure the performance of the random generator.  In order not to
    //   measure the time overhead of the test allocator, we use the default
    //   allocator throughout.  As for choosing the overloads to use, we rely
    //   on the implementation and avoid those that are simple inline
    //   forwarding calls (this might need to be adjusted for different
    //   implementations, however most implementations rely on a single
    //   implementation and the other calls can reduce to it).

    //   Note: This is a *synthetic* benchmark.  It does not replace measuring
    //   real benchmarks (e.g., for strings, ADSP) whose conclusions may differ
    //   due to different memory allocation and access patterns, function call
    //   frequencies, etc.  Its main use is for comparing two implementations
    //   (versions) against the same benchmark, and to test that one
    //   improvement in one function does not translate into a slow-down in
    //   another function.
    //
    //   Also note, that this is spaghetti code but we make no attempt at
    //   shortening it with helper functions or macros, since we feel that
    //
    // Testing:
    //   This "test" measures performance of basic operations, for performance
    //   regression.
    //
    //
    // RESULTS: Native SunProSTL on sundev13 as of Tue Jun 24 16:48:36 EDT 2008
    // ------------------------------------------------------------------------
    //
    //  String size:            4 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.003849s       0.000098s       0.003947s
    //      C2  From 'char*':   0.01829s        0.00043s        0.018722s
    //      C3  Copy ctor:      0.02700s        0.00044s        0.027436s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.02696s        0.00051s        0.027473s
    //      A2  Assign string:  0.10399s        0.00984s        0.113824s
    //      A3  Assign range:   0.11826s        0.01286s        0.131116s
    //  Append (with reallocations):
    //      P1  Append chars:   0.58007s        2.54784s        3.127911s
    //      P2  Append string:  0.87475s        3.07695s        3.951698s
    //          (without reallocation):
    //      P3  Append string:  0.13705s        0.02840s        0.165452s
    //      P4  Append range:   0.15452s        0.07269s        0.227208s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.22898s        2.03761s        2.266580s
    //      I2  Insert string:  0.54691s        2.39825s        2.945158s
    //          at end (without reallocation):
    //      I3  Insert string:  0.15257s        0.06108s        0.213649s
    //      I4  Insert range:   0.17466s        0.07843s        0.253086s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.23185s        2.17408s        2.405936s
    //      B2  Insert string:  0.24647s        2.07533s        2.321807s
    //          (without reallocation):
    //      B3  Insert string:  0.14609s        0.22978s        0.375868s
    //      B4  Insert range:   0.17793s        0.20753s        0.385453s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.16203s        0.06911s        0.231147s
    //      R2  Replace at begin: 0.15793s      1.08946s        1.247386s
    //  Misc.:
    //      M1  Swap reverse:   0.07511s        0.06057s        0.135686s
    //      M2  Substring:      5.60723s        14.51155s       20.118778s
    //
    // RESULTS: bslstl 1.11 on sundev13 as of Mon Jun 30 14:23:52 EDT 2008
    // --------------------------------------------------------------------
    //
    //  String size:            16 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //  Total amount of work should be about same in both
    //  long and short columns (one long string vs. many
    //  short strings), except for reallocations.
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.007775s       0.000244s       0.008019s
    //      C2  From 'char*':   0.06390s        0.00924s        0.073137s
    //      C3  Copy ctor:      0.06638s        0.00931s        0.075684s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.02828s        0.01366s        0.041943s
    //      A2  Assign string:  0.02685s        0.01311s        0.039953s
    //      A3  Assign range:   0.02439s        0.01429s        0.038676s
    //  Append (with reallocations):
    //      P1  Append chars:   0.56922s        0.52341s        1.092630s
    //      P2  Append string:  0.69453s        0.60602s        1.300548s
    //          (without reallocation):
    //      P3  Append string:  0.13577s        0.02976s        0.165528s
    //      P4  Append range:   0.13689s        0.06424s        0.201128s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.23625s        0.19838s        0.434627s
    //      I2  Insert string:  0.22206s        0.16017s        0.382224s
    //          at end (without reallocation):
    //      I3  Insert string:  0.13643s        0.06546s        0.201888s
    //      I4  Insert range:   0.14023s        0.06459s        0.204820s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.22982s        1.27716s        1.506978s
    //      B2  Insert string:  0.22201s        0.38553s        0.607539s
    //          (without reallocation):
    //      B3  Insert string:  0.14811s        0.49707s        0.645182s
    //      B4  Insert range:   0.14965s        0.50169s        0.651337s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.16184s        0.07699s        0.238831s
    //      R2  Replace at begin: 0.16303s      1.23734s        1.400366s
    //  Misc.:
    //      M1  Swap reverse:   0.23449s        0.09505s        0.329545s
    //      M2  Substring:      5.18101s        31.76464s       36.945650s
    //
    // RESULTS: Native gcc STL on sundev13 as of Tue Jun 24 16:48:36 EDT 2008
    // ----------------------------------------------------------------------
    //
    //  String size:            4 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.001821s       0.000066s       0.001887s
    //      C2  From 'char*':   0.01383s        0.00048s        0.014304s
    //      C3  Copy ctor:      0.01726s        0.00019s        0.017452s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.00372s        0.00007s        0.003786s
    //      A2  Assign string:  0.00128s        0.00003s        0.001310s
    //      A3  Assign range:   0.11978s        0.01919s        0.138965s
    //  Append (with reallocations):
    //      P1  Append chars:   0.61198s        0.48359s        1.095569s
    //      P2  Append string:  0.85236s        0.66129s        1.513646s
    //          (without reallocation):
    //      P3  Append string:  0.15236s        0.02859s        0.180949s
    //      P4  Append range:   0.16615s        0.05822s        0.224371s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.19453s        0.15631s        0.350844s
    //      I2  Insert string:  0.19757s        0.14761s        0.345184s
    //          at end (without reallocation):
    //      I3  Insert string:  0.14629s        0.05944s        0.205725s
    //      I4  Insert range:   0.14613s        0.06080s        0.206930s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.21156s        0.22866s        0.440219s
    //      B2  Insert string:  0.24475s        0.23070s        0.475451s
    //          (without reallocation):
    //      B3  Insert string:  0.15124s        0.19940s        0.350643s
    //      B4  Insert range:   0.18475s        0.21291s        0.397659s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.16595s        0.07109s        0.237031s
    //      R2  Replace at begin: 0.22566s      1.30403s        1.529692s
    //  Misc.:
    //      M1  Swap reverse:   1.30312s        0.39883s        1.701954s
    //      M2  Substring:      4.79876s        11.50113s       16.299887s
    //
    // RESULTS: Native STL on ibm1 as of  Tue Jun 24 16:48:36 EDT 2008
    // ------------------------------------------------------------------
    //
    //  String size:            16 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.001741s       0.000031s       0.001772s
    //      C2  From 'char*':   0.02315s        0.00278s        0.025921s
    //      C3  Copy ctor:      0.02373s        0.00277s        0.026501s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.00253s        0.00071s        0.003236s
    //      A2  Assign string:  0.00672s        0.00098s        0.007693s
    //      A3  Assign range:   0.00788s        0.00042s        0.008292s
    //  Append (with reallocations):
    //      P1  Append chars:   0.15841s        0.17874s        0.337155s
    //      P2  Append string:  0.17893s        0.18019s        0.359119s
    //          (without reallocation):
    //      P3  Append string:  0.06268s        0.01879s        0.081468s
    //      P4  Append range:   0.04941s        0.03089s        0.080301s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.06832s        0.06723s        0.135552s
    //      I2  Insert string:  0.07338s        0.06499s        0.138365s
    //          at end (without reallocation):
    //      I3  Insert string:  0.04835s        0.02264s        0.070989s
    //      I4  Insert range:   0.05462s        0.02913s        0.083743s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.06699s        0.07817s        0.145161s
    //      B2  Insert string:  0.07192s        0.07696s        0.148877s
    //          (without reallocation):
    //      B3  Insert string:  0.03464s        0.03325s        0.067894s
    //      B4  Insert range:   0.04500s        0.04113s        0.086138s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.05194s        0.02473s        0.076671s
    //      R2  Replace at begin: 0.04611s      0.29988s        0.345990s
    //  Misc.:
    //      M1  Swap reverse:   0.01834s        0.01893s        0.037267s
    //      M2  Substring:      2.23025s        6.25982s        8.490068s
    //
    // RESULTS: bslstl 1.11 on ibm1 as of Mon Jun 30 14:24:46 EDT 2008
    // --------------------------------------------------------------------
    //
    // --------------------------------------------------------------------

    bsls::Stopwatch t;

    printf("\n\tString size:\t\t" ZU " bytes.\n", sizeof(Obj));

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    // DATA  INITIALIZATION (NOT TIMED)
    const int SHORT_LENGTH = 16;     // length of short words
    const int SHORT_SIGMA  = 8;      // max deviation for short words
    const int NSHORT       = 50000;  // number of short words

    const int LONG_LENGTH  = 800;    // length of long words
    const int LONG_SIGMA   = 400;    // max deviation for long words
    const int NLONG        = 1000;   // number of long words

    ASSERT(NSHORT * (SHORT_LENGTH + SHORT_SIGMA / 2) ==
                                       NLONG * (LONG_LENGTH + LONG_SIGMA / 2));

    const int SL_RATIO     = NSHORT / NLONG;
    ASSERT(NLONG < NSHORT && 0 < SL_RATIO );

    Obj *shortWords = new Obj[NSHORT];
    size_t totalShortLength = 0;

    Obj *longWords  = new Obj[NLONG];
    size_t totalLongLength = 0;

    for (int i = 0; i < NSHORT; ++i) {
        const size_t LENGTH = SHORT_LENGTH + (rand() % (SHORT_SIGMA + 1));
        shortWords[i].assign(LENGTH, DEFAULT_VALUE);
        totalShortLength += LENGTH;
    }

    for (int i = 0; i < NLONG; ++i) {
        const size_t LENGTH = LONG_LENGTH + (rand() % (LONG_SIGMA + 1));
        longWords[i].assign(LENGTH, DEFAULT_VALUE);
        totalLongLength += LENGTH;
    }

    printf("\tUsing %d short words and %d long words.\n"
           "\tTotal length about same: " ZU " (short) and " ZU " (long).\n\n",
           NSHORT, NLONG, totalShortLength, totalLongLength);

    printf("\tTotal amount of work should be about same in both\n"
           "\tlong and short columns (one long string vs. many\n"
           "\tshort strings), except for reallocations.\n\n");

    printf("\t\t\t\tShort\t\tLong\t\tTotal\n");
    printf("\t\t\t\t-----\t\t----\t\t-----\n");

    printf("\tConstructors:\n");
    // {
        // C1) CONSTRUCTORS
        double timeC1 = 0., timeC1short = 0., timeC1long = 0.;

        t.reset(); t.start();
        Obj *shortStrings = new Obj[NSHORT];
        timeC1short = t.elapsedTime();
        size_t *shortStringLength = new size_t[NSHORT];

        t.reset(); t.start();
        Obj *longStrings  = new Obj[NLONG];
        timeC1long = t.elapsedTime();
        size_t *longStringLength = new size_t[NLONG];

        printf("\t    C1\tDefault ctor:\t%1.6fs\t%1.6fs\t%1.6fs\n",
               timeC1short, timeC1long, timeC1 = timeC1short + timeC1long);

        // C2) CONSTRUCTORS
        double timeC2 = 0., timeC2short = 0., timeC2long = 0.;

        for (int i = 0; i < NSHORT; ++i) {
            (shortStrings + i)->~Obj();
        }
        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            new(shortStrings + i) Obj(shortWords[i]);
        }
        timeC2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringLength[i] = shortStrings[i].length();
        }

        for (int i = 0; i < NLONG; ++i) {
            (longStrings + i)->~Obj();
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            new(longStrings + i) Obj(longWords[i]);
        }
        timeC2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) {
            longStringLength[i] = longStrings[i].length();
        }

        printf("\t    C2\tFrom 'char*':\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeC2short, timeC2long, timeC2 = timeC2short + timeC2long);

        // C3) COPY CONSTRUCTOR (with allocations)
        double timeC3 = 0., timeC3short = 0., timeC3long = 0.;
        void *shortStringBufCopy = new bsls::ObjectBuffer<Obj>[NSHORT];
        void *longStringBufCopy  = new bsls::ObjectBuffer<Obj>[NLONG];
        Obj *shortStringCopy = (Obj *)shortStringBufCopy;
        Obj *longStringCopy  = (Obj *)longStringBufCopy;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            new(shortStringCopy + i) Obj(shortStrings[i]);
        }
        timeC3short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            new(longStringCopy + i) Obj(longStrings[i]);
        }
        timeC3long = t.elapsedTime();

        printf("\t    C3\tCopy ctor:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeC3short, timeC3long, timeC3 = timeC3short + timeC3long);
    // }

    printf("\tAssignment (no reallocation):\n");
    // {
        // A1) COPY ASSIGNMENT (without allocations)
        double timeA1 = 0., timeA1short = 0., timeA1long = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringCopy[i] = shortStrings[i];
        }
        timeA1short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            longStringCopy[i] = longStrings[i];
        }
        timeA1long = t.elapsedTime();

        printf("\t    A1\t'operator=':\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeA1short, timeA1long, timeA1 = timeA1short + timeA1long);

        // A2) ASSIGN OPERATIONS (again, no allocation)
        double timeA2 = 0., timeA2short = 0., timeA2long = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringCopy[i].assign(shortStrings[i]);
        }
        timeA2short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            longStringCopy[i].assign(longStrings[i]);
        }
        timeA2long = t.elapsedTime();

        printf("\t    A2\tAssign string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeA2short, timeA2long, timeA2 = timeA2short + timeA2long);

        // A3.
        double timeA3 = 0., timeA3short = 0., timeA3long = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringCopy[i].assign(shortStrings[i].begin(),
                                      shortStrings[i].end());
        }
        timeA3short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            longStringCopy[i].assign(longStrings[i].begin(),
                                     longStrings[i].end());
        }
        timeA3long = t.elapsedTime();

        printf("\t    A3\tAssign range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeA3short, timeA3long, timeA3 = timeA3short + timeA3long);
    // }

#define COMPARE_WITH_NATIVE_SUNPRO_STL 1
    // When comparing performance with the native Sunpro STL (based on Rogue
    // Wave), a bug causes the native STL to reallocate every time after
    // push_back, instead of doubling the capacity.  This prohibits the testing
    // of P1, I1, and B1.

    printf("\tAppend (with reallocations):\n");
    // {
        // P1) PUSH_BACK OPERATION (with reallocations, i.e., changes capacity)
        // Pushing individual chars into either short/long strings, should take
        // slightly shorter for long strings since there will be fewer
        // reallocations.

        double timeP1 = 0., timeP1short = 0., timeP1long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[ilong]; ++j) {
                shortStrings[i].push_back(longStringCopy[ilong][j]);
            }
#else
            shortStrings[i].append(longStringCopy[ilong]);
#endif
        }
        timeP1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStrings[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[i]; ++j) {
                for (int k = 0; k < SL_RATIO; ++k) {
                    longStrings[i].push_back(longStringCopy[i][j]);
                }
            }
#else
            for (int k = 0; k < SL_RATIO; ++k) {
                longStrings[i].append(longStringCopy[i]);
            }
#endif
        }
        timeP1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStrings[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
        }

        printf("\t    P1\tAppend chars:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP1short, timeP1long, timeP1 = timeP1short + timeP1long);

        // P2) APPEND OPERATION (with reallocations, i.e., changes capacity)
        // Unlike P1, pushing whole string at once will be more efficient since
        // only one allocation for short strings (final length is known), or
        // only a few reallocations for long strings.

        double timeP2 = 0., timeP2short = 0., timeP2long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].append(longStrings[ilong]);
        }
        timeP2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].append(longStrings[i]);
            }
        }
        timeP2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    P2\tAppend string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP2short, timeP2long, timeP2 = timeP2short + timeP2long);
    // }

    printf("\t\t(without reallocation):\n");
    // {
        // P3) AGAIN (this time, without reallocation)
        double timeP3 = 0., timeP3short = 0., timeP3long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].append(longStrings[ilong]);
        }
        timeP3short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i, ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].append(longStrings[i]);
            }
        }
        timeP3long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    P3\tAppend string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP3short, timeP3long, timeP3 = timeP3short + timeP3long);

        // P4) APPEND RANGE
        double timeP4 = 0., timeP4short = 0., timeP4long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].append(longStrings[ilong].begin(),
                                      longStrings[ilong].end());
        }
        timeP4short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].append(longStrings[i].begin(),
                                         longStrings[i].end());
            }
        }
        timeP4long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    P4\tAppend range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP4short, timeP4long, timeP4 = timeP4short + timeP4long);
    // }

    printf("\tInsertions at end (with reallocations):\n");
    // {
        // I1) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeI1 = 0., timeI1short = 0., timeI1long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStrings[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
            // ASSERT(shortStringLength[i] == shortStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[ilong]; ++j) {
                shortStrings[i].insert(shortStrings[i].end(),
                                       longStrings[ilong][j]);
            }
#else
            shortStrings[i].insert(shortStrings[i].length(),
                                   longStrings[ilong]);
#endif
        }
        timeI1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) {  // restore length but not capacity
            shortStrings[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStrings[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
            // ASSERT(longStringLength[i] == longStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[i]; ++j) {
                for (int k = 0; k < SL_RATIO; ++k) {
                    longStrings[i].insert(longStrings[i].end(),
                                          longStringCopy[i][j]);
                }
            }
#else
            for (int k = 0; k < SL_RATIO; ++k) {
                longStrings[i].insert(longStrings[i].length(),
                                      longStringCopy[i]);
            }
#endif
        }
        timeI1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStrings[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
        }

        printf("\t    I1\tInsert chars:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI1short, timeI1long, timeI1 = timeI1short + timeI1long);

        // I2) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeI2 = 0., timeI2short = 0., timeI2long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStringCopy[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
            // ASSERT(shortStringLength[i] == shortStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].length(),
                                      longStrings[ilong]);
        }
        timeI2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStringCopy[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
            // ASSERT(longStringLength[i] == longStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].insert(longStringCopy[i].length(),
                                         longStrings[i]);
            }
        }
        timeI2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    I2\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI2short, timeI2long, timeI2 = timeI2short + timeI2long);
    // }

    printf("\t\tat end (without reallocation):\n");
    // {
        // I3) AGAIN (this time, without reallocation)
        double timeI3 = 0., timeI3short = 0., timeI3long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].length(),
                                      longStrings[ilong]);
        }
        timeI3short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].insert(longStringCopy[i].length(),
                                         longStrings[i]);
            }
        }
        timeI3long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    I3\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI3short, timeI3long, timeI3 = timeI3short + timeI3long);

        // I4) INSERT RANGE
        double timeI4 = 0., timeI4short = 0., timeI4long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].end(),
                                      longStrings[ilong].begin(),
                                      longStrings[ilong].end());
        }
        timeI4short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].insert(longStringCopy[i].end(),
                                         longStrings[i].begin(),
                                         longStrings[i].end());
            }
        }
        timeI4long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    I4\tInsert range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI4short, timeI4long, timeI4 = timeI4short + timeI4long);
    // }

    printf("\tInsertions at beginning (with reallocations):\n");
    // {
        // B1) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeB1 = 0., timeB1short = 0., timeB1long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStrings[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
            // ASSERT(shortStringLength[i] == shortStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (size_t j = 0; j < longStringLength[ilong]; ++j) {
                shortStrings[i].insert(j, 1, longStringCopy[ilong][j]);
            }
#else
            shortStrings[i].insert((size_t)0, longStringCopy[ilong]);
#endif
        }
        timeB1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) {  // restore length but not capacity
            shortStrings[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStrings[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
            // ASSERT(longStringLength[i] == longStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            size_t pos = 0;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (size_t j = 0; j < longStringLength[i]; ++j) {
                for (int k = 0; k < SL_RATIO; ++k, ++pos) {
                    longStrings[i].insert(pos, 1, longStringCopy[i][j]);
                }
            }
#else
                for (int k = 0; k < SL_RATIO; ++k, ++pos) {
                    longStrings[i].insert(pos, longStringCopy[i]);
                }
#endif
        }
        timeB1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStrings[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
        }

        printf("\t    B1\tInsert chars:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB1short, timeB1long, timeB1 = timeB1short + timeB1long);

        // B2) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeB2 = 0., timeB2short = 0., timeB2long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStringCopy[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
            // ASSERT(shortStringLength[i] == shortStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert((size_t)0, longStrings[ilong]);
        }
        timeB2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStringCopy[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
            // ASSERT(longStringLength[i] == longStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            size_t pos = 0;
            for (int k = 0; k < SL_RATIO; ++k, pos += longStringLength[i]) {
                longStringCopy[i].insert(pos, longStrings[i]);
            }
        }
        timeB2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    B2\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB2short, timeB2long, timeB2 = timeB2short + timeB2long);
    // }

    printf("\t\t(without reallocation):\n");
    // {
        // B3) AGAIN (this time, without reallocation)
        double timeB3 = 0., timeB3short = 0., timeB3long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert((size_t)0, longStrings[ilong]);
        }
        timeB3short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            size_t pos = 0;
            for (int k = 0; k < SL_RATIO; ++k, pos += longStringLength[i]) {
                longStringCopy[i].insert(pos, longStrings[i]);
            }
        }
        timeB3long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    B3\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB3short, timeB3long, timeB3 = timeB3short + timeB3long);

        // B4) INSERT RANGE
        double timeB4 = 0., timeB4short = 0., timeB4long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].begin(),
                                      longStrings[ilong].begin(),
                                      longStrings[ilong].end());
        }
        timeB4short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            typename Obj::iterator pos = longStringCopy[i].begin();
            for (int k = 0; k < SL_RATIO; ++k, pos += longStringLength[i]) {
                longStringCopy[i].insert(pos,
                                         longStrings[i].begin(),
                                         longStrings[i].end());
            }
        }
        timeB4long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    B4\tInsert range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB4short, timeB4long, timeB4 = timeB4short + timeB4long);
    // }

    printf("\tReplacements (without reallocation):\n");
    // {
        // R1) REPLACE AT END
        double timeR1 = 0., timeR1short = 0., timeR1long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].replace(shortStringCopy[i].end() - 1,
                                       shortStringCopy[i].end(),
                                       longStrings[ilong]);
        }
        timeR1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].replace(longStringCopy[i].end() - 1,
                                          longStringCopy[i].end(),
                                          longStrings[i]);
            }
        }
        timeR1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    R1\tReplace at end:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeR1short, timeR1long, timeR1 = timeR1short + timeR1long);

        // R2) REPLACE AT BEGINNING
        double timeR2 = 0., timeR2short = 0., timeR2long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].replace(shortStringCopy[i].begin(),
                                       shortStringCopy[i].begin() + 1,
                                       longStrings[ilong]);
        }
        timeR2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].replace(longStringCopy[i].begin(),
                                          longStringCopy[i].begin() + 1,
                                          longStrings[i]);
            }
        }
        timeR2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    R2\tReplace at begin: %1.5fs\t%1.5fs\t%1.6fs\n",
               timeR2short, timeR2long, timeR2 = timeR2short + timeR2long);
    // }

    printf("\tMisc.:\n");
    // {
        // M1) SWAP OPERATION
        double timeM1 = 0., timeM1short = 0., timeM1long = 0.;

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO; ++k) {
            for (int i = 0; i < NSHORT; ++i) {
                shortStrings[i].swap(shortStrings[NSHORT - 1 - i]);
            }
        }
        timeM1short = t.elapsedTime();

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO * SL_RATIO; ++k) {
            for (int i = 0; i < NLONG; ++i) {
                longStrings[i].swap(longStrings[NLONG - 1 - i]);
            }
        }
        timeM1long = t.elapsedTime();

        printf("\t    M1\tSwap reverse:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeM1short, timeM1long, timeM1 = timeM1short + timeM1long);

        // M2) SUBSTRING (plus assignment)
        double timeM2 = 0., timeM2short = 0., timeM2long = 0.;

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO; ++k) {
            for (int i = 0; i < NSHORT; ++i) {
                shortStringCopy[i] = shortStrings[i].substr(
                                                      0, shortStringLength[i]);
            }
        }
        timeM2short = t.elapsedTime();

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO * SL_RATIO; ++k) {
            for (int i = 0; i < NLONG; ++i) {
                longStringCopy[i] = longStrings[i].substr(0,
                                                          longStringLength[i]);
            }
        }
        timeM2long = t.elapsedTime();

        printf("\t    M2\tSubstring:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeM2short, timeM2long, timeM2 = timeM2short + timeM2long);
    // }

    // F1) FIND AND RFIND OPERATIONS

    // F2) FIND_FIRST_OF AND FIND_LAST_OF OPERATIONS

    // F3) FIND_FIRST_NOT_OF AND FIND_LAST_NOT_OF OPERATIONS
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// BDE_VERIFY pragma: push  // usage example relaxes rules for clear exposition
//
// BDE_VERIFY pragma: -FD01 // Contracts are often expository
// BDE_VERIFY pragma: -FD02 // Contracts are often expository
// BDE_VERIFY pragma: -FD03 // Contracts are often expository

namespace BloombergLP {
namespace bslstl {

class StringRef {
    // This 'class' provides a dummy implementation for use with the usage
    // example.   The interface is minimal and only supports functions needed
    // for testing.

    // DATA
    const char *d_begin_p;
    const char *d_end_p;

  public:
    // CREATORS
    StringRef(const char *begin, const char *end)
    : d_begin_p(begin)
    , d_end_p(end)
    {
    }

    // ACCESSORS
    const char *begin() const { return d_begin_p; }
    const char *end() const   { return d_end_p;   }

    bool isEmpty() const { return d_begin_p == d_end_p; }
};

}  // close package namespace
}  // close enterprise namespace

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 2: 'string' as a data member
///- - - - - - - - - - - - - - - - - -
// The most common use of 'string' objects are as data members in user-defined
// classes.  In this example, we will show how 'string' objects can be used as
// data members.
//
// First, we begin to define a 'class', 'Employee', that represents the data
// corresponding to an employee of a company:
//..
    class Employee {
        // This simply constrained (value-semantic) attribute class represents
        // the information about an employee.  An employee's first and last
        // name are represented as 'string' objects and their employee
        // identification number is represented by an 'int'.  Note that the
        // class invariants are identically the constraints on the individual
        // attributes.
        //
        // This class:
        //: o supports a complete set of *value-semantic* operations
        //:   o except for 'bslx' serialization
        //: o is *exception-neutral* (agnostic)
        //: o is *alias-safe*
        //: o is 'const' *thread-safe*

        // DATA
        bsl::string d_firstName;       // first name
        bsl::string d_lastName;        // last name
        int         d_id;              // identification number
//..
//  Next, we define the creators for this class:
//..
      public:
        // CREATORS
        explicit Employee(bslma::Allocator *basicAllocator = 0);
            // Create a 'Employee' object having the (default) attribute
            // values:
            //..
            //  firstName() == ""
            //  lastName()  == ""
            //  id()        == 0
            //..
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default
            // allocator is used.

        Employee(const bslstl::StringRef&  firstName,
                 const bslstl::StringRef&  lastName,
                 int                       id,
                 bslma::Allocator         *basicAllocator = 0);
            // Create a 'Employee' object having the specified 'firstName',
            // 'lastName', and 'id' attribute values.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        Employee(const Employee&   original,
                 bslma::Allocator *basicAllocator = 0);
            // Create a 'Employee' object having the same value as the
            // specified 'original' object.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        //! ~Employee() = default;
            // Destroy this object.
//..
// Notice that all constructors of the 'Employee' class are optionally provided
// an allocator that is then passed through to the 'string' data members of
// 'Employee'.  This allows the user to control how memory is allocated by
// 'Employee' objects.  Also note that the type of the 'firstName' and
// 'lastName' arguments of the value constructor is 'bslstl::StringRef'.  The
// 'bslstl::StringRef' allows specifying a 'string' or a 'const char *' to
// represent a string value.  For the sake of brevity its implementation is
// not explored here.
//
// Then, declare the remaining methods of the class:
//..
        // MANIPULATORS
        Employee& operator=(const Employee& rhs);
            // Assign to this object the value of the specified 'rhs' object,
            // and return a reference providing modifiable access to this
            // object.

        void setFirstName(const bslstl::StringRef& value);
            // Set the 'firstName' attribute of this object to the specified
            // 'value'.

        void setLastName(const bslstl::StringRef& value);
            // Set the 'lastName' attribute of this object to the specified
            // 'value'.

        void setId(int value);
            // Set the 'id' attribute of this object to the specified 'value'.

        // ACCESSORS
        const bsl::string& firstName() const;
            // Return a reference providing non-modifiable access to the
            // 'firstName' attribute of this object.

        const bsl::string& lastName() const;
            // Return a reference providing non-modifiable access to the
            // 'lastName' attribute of this object.

        int id() const;
            // Return the value of the 'id' attribute of this object.
    };
//..
// Next, we declare the free operators for 'Employee':
//..
    inline
    bool operator==(const Employee& lhs, const Employee& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two 'Employee' objects have the same
        // value if all of their corresponding values of their 'firstName',
        // 'lastName', and 'id' attributes are the same.

    inline
    bool operator!=(const Employee& lhs, const Employee& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same value, and 'false' otherwise.  Two 'Employee' objects do
        // not have the same value if any of the corresponding values of their
        // 'firstName', 'lastName', or 'id' attributes are not the same.
//..
// Then, we implement the various methods of the 'Employee' class:
//..
    // CREATORS
    inline
    Employee::Employee(bslma::Allocator *basicAllocator)
    : d_firstName(basicAllocator)
    , d_lastName(basicAllocator)
    , d_id(0)
    {
    }

    inline
    Employee::Employee(const bslstl::StringRef&  firstName,
                       const bslstl::StringRef&  lastName,
                       int                       id,
                       bslma::Allocator         *basicAllocator)
    : d_firstName(firstName.begin(), firstName.end(), basicAllocator)
    , d_lastName(lastName.begin(), lastName.end(), basicAllocator)
    , d_id(id)
    {
        BSLS_ASSERT_SAFE(!firstName.isEmpty());
        BSLS_ASSERT_SAFE(!lastName.isEmpty());
    }

    inline
    Employee::Employee(const Employee&   original,
                       bslma::Allocator *basicAllocator)
    : d_firstName(original.d_firstName, basicAllocator)
    , d_lastName(original.d_lastName, basicAllocator)
    , d_id(original.d_id)
    {
    }
//..
// Notice that the 'basicAllocator' parameter can simply be passed as an
// argument to the constructor of 'bsl::string'.
//
// Now, we implement the remaining manipulators of the 'Employee' class:
//..
    // MANIPULATORS
    inline
    Employee& Employee::operator=(const Employee& rhs)
    {
        d_firstName = rhs.d_firstName;
        d_lastName  = rhs.d_lastName;
        d_id        = rhs.d_id;
        return *this;
    }

    inline
    void Employee::setFirstName(const bslstl::StringRef& value)
    {
        BSLS_ASSERT_SAFE(!value.isEmpty());

        d_firstName.assign(value.begin(), value.end());
    }

    inline
    void Employee::setLastName(const bslstl::StringRef& value)
    {
        BSLS_ASSERT_SAFE(!value.isEmpty());

        d_lastName.assign(value.begin(), value.end());
    }

    inline
    void Employee::setId(int value)
    {
        d_id = value;
    }

    // ACCESSORS
    inline
    const bsl::string& Employee::firstName() const
    {
        return d_firstName;
    }

    inline
    const bsl::string& Employee::lastName() const
    {
        return d_lastName;
    }

    inline
    int Employee::id() const
    {
        return d_id;
    }
//..
// Finally, we implement the free operators for 'Employee' class:
//..
    inline
    bool operator==(const Employee& lhs, const Employee& rhs)
    {
        return lhs.firstName() == rhs.firstName()
            && lhs.lastName()  == rhs.lastName()
            && lhs.id()        == rhs.id();
    }

    inline
    bool operator!=(const Employee& lhs, const Employee& rhs)
    {
        return lhs.firstName() != rhs.firstName()
            || lhs.lastName()  != rhs.lastName()
            || lhs.id()        != rhs.id();
    }
//..
//
///Example 3: A stream text replacement filter
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize the 'string' type and its associated
// utility functions to define a function that reads data from an input stream,
// replaces all occurrences of a specified text fragment with another text
// fragment, and writes the resulting text to an output stream.
//
// First, we define the signature of the function, 'replace':
//..
    void replace(std::ostream&      outputStream,
                 std::istream&      inputStream,
                 const bsl::string& oldString,
                 const bsl::string& newString)
        // Read data from the specified 'inputStream' and replace all
        // occurrences of the text contained in the specified 'oldString' in
        // the stream with the text contained in the specified 'newString'.
        // Write the modified data to the specified 'outputStream'.
//..
// Then, we provide the implementation for 'replace':
//..
    {
        const bsl::string::size_type oldStringSize = oldString.size();
        const bsl::string::size_type newStringSize = newString.size();
        bsl::string                  line;

        bsl::getline(inputStream, line);
//..
// Notice that we can use the 'getline' free function defined in this component
// to read a single line of data from an input stream into a 'bsl::string'.
//..
        if (!inputStream) {
            return;                                                   // RETURN
        }

        do {
//..
// Next, we use the 'find' function to search the contents of 'line' for
// characters matching the contents of 'oldString':
//..
            bsl::string::size_type pos = line.find(oldString);
            while (bsl::string::npos != pos) {
//..
// Now, we use the 'replace' method to modify the contents of 'line' matching
// 'oldString' to 'newString':
//..
                line.replace(pos, oldStringSize, newString);
                pos = line.find(oldString, pos + newStringSize);
//..
// Notice that we provide 'find' with the starting position from which to start
// searching.
//..
            }
//..
// Finally, we write the updated contents of 'line' to the output stream:
//..
            outputStream << line;

            bsl::getline(inputStream, line);
        } while (inputStream);
    }
//..

}  // close namespace UsageExample

namespace BloombergLP {
namespace bslma {
// Specialize trait to clarify for bde_verify that 'LimitAllocator' does not
// require satisfy the 'UsesBslmaAllocator' trait.
template <>
struct UsesBslmaAllocator<UsageExample::Employee> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

// BDE_VERIFY pragma: pop  // end of usage example

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                          veryVeryVeryVerbose);
    bslma::Allocator *originalGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    // Confirm no static initialization locked the global allocator
    ASSERT(globalAllocator_p == bslma::Default::globalAllocator());

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    defaultAllocator_p = &defaultAllocator;
    bslma::Default::setDefaultAllocator(defaultAllocator_p);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);
    objectAllocator_p = &objectAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 35: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase35();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase35();

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        {
            using namespace UsageExample;
            bslma::TestAllocator defaultAllocator("defaultAllocator");
            bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

            bslma::TestAllocator objectAllocator("objectAllocator");
            bslma::TestAllocator scratch("scratch");

///Example 1: Basic Syntax
///- - - - - - - - - - - -
// In this example, we will show how to create and use the 'string' typedef.
//
// First, we will default-construct a 'string' object:
//..
    bsl::string s;
    ASSERT(s.empty());
    ASSERT(0  == s.size());
    ASSERT("" == s);
//..
// Then, we will construct a 'string' object from a string literal:
//..
    bsl::string t = "Hello World";
    ASSERT(!t.empty());
    ASSERT(11 == t.size());
    ASSERT("Hello World" == t);
//..
// Next, we will clear the contents of 't' and assign it a couple of values:
// first from a string literal; and second from another 'string' object:
//..
    t.clear();
    ASSERT(t.empty());
    ASSERT("" == t);
//
    t = "Good Morning";
    ASSERT(!t.empty());
    ASSERT("Good Morning" == t);
//
    t = s;
    ASSERT(t.empty());
    ASSERT("" == t);
    ASSERT(t == s);
//..
// Then, we will create three 'string' objects: the first representing a street
// name, the second a state, and the third a zipcode.  We will then concatenate
// them into a single address 'string' and print the contents of that 'string'
// on standard output:
//..
    const bsl::string street  = "731 Lexington Avenue";
    const bsl::string state   = "NY";
    const bsl::string zipcode = "10022";
//
    const bsl::string fullAddress = street + " " + state + " " + zipcode;
//
  if (veryVerbose) {
      debugprint(fullAddress);
  }
//..
// The above print statement should produce a single line of output:
//..
//  731 Lexington Avenue NY 10022
//..
// Then, we search the contents of 'address' (using the 'find' function) to
// determine if it lies on a specified street:
//..
    const bsl::string streetName = "Lexington";
//
    if (bsl::string::npos != fullAddress.find(streetName, 0)) {
      if (veryVerbose) {
          debugprint("The address " + fullAddress + " is located on "
                     + streetName + ".");
      }
    }
//..
// Next, we show how to get a reference providing modifiable access to the
// null-terminated string literal stored by a 'string' object using the 'c_str'
// function.  Note that the returned string literal can be passed to various
// standard functions expecting a null-terminated string:
//..
    const bsl::string  v = "Another string";
    const char        *cs = v.c_str();
    ASSERT(strlen(cs) == v.size());
//..
// Then, we construct two 'string' objects, 'x' and 'y', using a user-specified
// allocator:
//..
    bslma::TestAllocator allocator1, allocator2;
//
    const char *SHORT_STRING = "A small string";
    const char *LONG_STRING  = "This long string would definitely cause "
                               "memory to be allocated on creation";
//
    const bsl::string x(SHORT_STRING, &allocator1);
    const bsl::string y(LONG_STRING,  &allocator2);
//
    ASSERT(SHORT_STRING == x);
    ASSERT(LONG_STRING  == y);
//..
// Notice that, no memory was allocated from the allocator for object 'x'
// because of the short-string optimization used in the 'string' type.
//
// Finally, we can track memory usage of 'x' and 'y' using 'allocator1' and
// 'allocator2' and check that memory was allocated only by 'allocator2':
//..
    ASSERT(0 == allocator1.numBlocksInUse());
    ASSERT(1 == allocator2.numBlocksInUse());
//..
//
        }

///Example 2: 'string' as a data member
///- - - - - - - - - - - - - - - - - -
        {
            using namespace UsageExample;

            // Default ctor
            Employee e1;  const Employee& E1 = e1;
            ASSERT("" == E1.firstName());
            ASSERT("" == E1.lastName());
            ASSERT(0  == E1.id());

            // Value ctor
            bsl::string       FIRST_NAME = "Joe";
            bsl::string       LAST_NAME  = "Smith";
            bslstl::StringRef FIRST(FIRST_NAME.begin(), FIRST_NAME.end());
            bslstl::StringRef LAST(LAST_NAME.begin(), LAST_NAME.end());
            int               ID         = 1;

            Employee e2(FIRST, LAST, ID);  const Employee& E2 = e2;
            ASSERT(FIRST_NAME == E2.firstName());
            ASSERT(LAST_NAME  == E2.lastName());
            ASSERT(ID         == E2.id());

            // Equality operators
            ASSERT(! (e1 == e2));
            ASSERT(   e1 != e2);

            // Manipulators and accessors
            e1.setFirstName(FIRST);
            ASSERT(FIRST_NAME == e1.firstName());

            e1.setLastName(LAST);
            ASSERT(LAST_NAME == e1.lastName());

            e1.setId(ID);
            ASSERT(ID == e1.id());

            ASSERT(   e1 == e2);
            ASSERT(! (e1 != e2));

            // Copy constructor
            Employee e3(e1);  const Employee& E3 = e3;
            (void) E3;

            ASSERT(   e1 == e3);
            ASSERT(! (e1 != e3));
        }

///Example 3: A 'string' replace function
///- - - - - - - - - - - - - - - - - - -
        {
            using namespace UsageExample;

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_old_p;           // old string to replace
                const char *d_new_p;           // new string to replace with
                const char *d_orig_p;          // original result
                const char *d_exp_p;           // expected result
            } DATA[] = {
            //line  old           new           orig          exp
            //----  ----          ----          ----          ----
            { L_,  "a",          "b",          "abcdeabc",   "bbcdebbc"    },
            { L_,  "b",          "a",          "abcdeabc",   "aacdeaac"    },
            { L_,  "ab",         "xy",         "ababefgh",   "xyxyefgh"    },
            { L_,  "abc",        "xyz",        "abcdefgh",   "xyzdefgh"    },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const bsl::string OLD   = DATA[ti].d_old_p;
                const bsl::string NEW   = DATA[ti].d_new_p;
                const bsl::string ORIG  = DATA[ti].d_orig_p;
                const bsl::string EXP   = DATA[ti].d_exp_p;

                std::istringstream is(ORIG);
                std::ostringstream os;
                replace(os, is, OLD, NEW);
                LOOP_ASSERT(LINE, EXP == os.str());
            }
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING 'initializer_list' FUNCTIONS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase33' for details.
        //
        // Testing:
        //   basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
        //   basic_string& operator=(initializer_list<CHAR_TYPE> values);
        //   basic_string& assign(initializer_list<CHAR_TYPE> values);
        //   iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'initializer_list' FUNCTIONS"
                            "\n====================================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        TestDriver<char   >::testCase33();
        TestDriver<wchar_t>::testCase33();
#else
        if (verbose) printf("\nSkip %d: Requires C++11 Initializers\n", test);
#endif
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING 'to_string' AND 'to_wstring'
        //
        // Concerns:
        //: 1  The 'to_string' function has sufficient overloads to create a
        //:    'string' for any arithmetic type.
        //: 2  The 'to_wstring' function has sufficient overloads to create a
        //:    'wstring' for any arithmetic type.
        //: 3  The numeric -> string conversion functions return an
        //:    appropriately formatted (short) string containing the correct
        //:    value.
        //
        // Plan:
        //: 1 use 'sprintf' and 'swprintf' with an arbitrarily large buffer,
        //:   (in this test case the buffer size will be 384) and compare it to
        //:   the output of 'to_string' and 'to_wstring'.  The buffer must be
        //:   at least as large as the largest floating point value that might
        //:   print, which is roughly 308 decimal digits.
        //:
        //: 2 Using table-driven testing, test a suitable range of numbers that
        //:   are, each in turn, cast to every integer type that can hold each
        //:   value in order to check every overload is called and handled
        //:   correctly.
        //
        // Testing:
        //   string to_string(int value);
        //   string to_string(long value);
        //   string to_string(long long value);
        //   string to_string(unsigned value);
        //   string to_string(unsigned long value);
        //   string to_string(unsigned long long value);
        //   string to_string(float value);
        //   string to_string(double value);
        //   string to_string(long double value);
        //   wstring to_wstring(int value);
        //   wstring to_wstring(long value);
        //   wstring to_wstring(long long value);
        //   wstring to_wstring(unsigned value);
        //   wstring to_wstring(unsigned long value);
        //   wstring to_wstring(unsigned long long value);
        //   wstring to_wstring(float value);
        //   wstring to_wstring(double value);
        //   wstring to_wstring(long double value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'to_string' AND 'to_wstring'"
                            "\n====================================\n");

        static const size_t BUF_LEN = 384;
        char    tempBuf[BUF_LEN];  // 'char' buffer for largest number
        wchar_t wTempBuf[BUF_LEN]; // 'wchar_t' buffer for largest number

        static const struct {
            int         d_lineNum;
            long long   d_value;
            bool        d_isShortChar;
            bool        d_isShortWchar;
        } DATA[] = {
          //                             string is short:
          // LINE                VALUE   'char' 'wchar_t'
            { L_,                    0,   true,  true                },
            { L_,                    1,   true,  true                },
            { L_,                   -1,   true,  true                },
            { L_,                10101,   true,  sizeof(wchar_t) < 4 },
            { L_,               -10101,   true,  sizeof(wchar_t) < 4 },
            { L_,                32767,   true,  sizeof(wchar_t) < 4 },
            { L_,               -32767,   true,  sizeof(wchar_t) < 4 },
            { L_,             11001100,   true,  sizeof(wchar_t) < 4 },
            { L_,            -11001100,   true,  sizeof(wchar_t) < 4 },
            { L_,           2147483647,   true,  false               },
            { L_,          -2147483647,   true,  false               },
            { L_,  9223372036854775807LL, true,  false               },
            { L_, -9223372036854775807LL, false, false               }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct {
            int         d_lineNum;
            double      d_value;
        } DOUBLE_DATA[] = {
            //   value
            {L_, 1.0},
            {L_, 1.01},
            {L_, 1.001},
            {L_, 1.0101},
            {L_, 1.01001},
            {L_, 1.010101},
            {L_, 1.01010101},
            {L_, 1.0101019},
            {L_, 3.1415926},
            {L_, 5.156},
            {L_, 24.0},
            {L_, 24.1111111111111111111},
            {L_, 12345.12345678},
            {L_, 123456789.123456789},
            {L_, 123456789012345.123456},
            {L_, 1234567890123456789.123456789},
            {L_, std::numeric_limits<float>::max()},
            {L_, std::numeric_limits<float>::min()},
            {L_, 1.79769e+308},
            {L_,-1.79769e+308},
        };
        enum { NUM_DOUBLE_DATA = sizeof DOUBLE_DATA / sizeof *DOUBLE_DATA };

        if (verbose) printf("\nTesting 'to_string' with integrals.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE           = DATA[ti].d_lineNum;
            const long long          VALUE          = DATA[ti].d_value;
//          const bool               IS_SHORT_CHAR  = DATA[ti].d_isShortChar;
//          const bool               IS_SHORT_WCHAR = DATA[ti].d_isShortWchar;
            const unsigned long long U_VALUE        =
                                        static_cast<unsigned long long>(VALUE);

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'string'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            // Set up the oracle results for signed integers, using 'long long'
            // versions of the corresponding C library 'sprintf' functions.

            sprintf(tempBuf, "%lld", VALUE);

            if (VALUE <= std::numeric_limits<int>::max() &&
                VALUE >= std::numeric_limits<int>::min()) {
                const int TEST_VALUE = static_cast<int>(VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str);
            }

            if (VALUE <= std::numeric_limits<long>::max() &&
                VALUE >= std::numeric_limits<long>::min()) {
                const long TEST_VALUE = static_cast<long>(VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str);
            }

            {
                bsl::string str = bsl::to_string(VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            const Int64 MM = defaultAllocator.numBlocksTotal();
            const Int64  M = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tMiddle: ");P_(MM);P(M);
            }

            // Set up the oracle results for unsigned integers, using the
            // 'unsigned long long' versions of the corresponding C library
            // 'sprintf' functions.

            sprintf(tempBuf, "%llu", U_VALUE);

            if (U_VALUE <= std::numeric_limits<unsigned int>::max()) {
                const unsigned int TEST_VALUE =
                                            static_cast<unsigned int>(U_VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str);
            }

            if (U_VALUE <= std::numeric_limits<unsigned long>::max()) {
                const unsigned long TEST_VALUE =
                                           static_cast<unsigned long>(U_VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            {
                bsl::string str = bsl::to_string(U_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());

        if (verbose) printf("\nTesting 'to_wstring' with integrals.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE    = DATA[ti].d_lineNum;
            const long long          VALUE   = DATA[ti].d_value;
            const unsigned long long U_VALUE =
                                        static_cast<unsigned long long>(VALUE);

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'wstring'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            // Set up the oracle results for signed integers, using 'long long'
            // versions of the corresponding C library 'sprintf' functions.

            swprintf(wTempBuf, BUF_LEN, L"%lld", VALUE);

            if (VALUE <= std::numeric_limits<int>::max() &&
                VALUE >= std::numeric_limits<int>::min()) {
                const int TEST_VALUE = static_cast<int>(VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr);
            }

            if (VALUE <= std::numeric_limits<long>::max() &&
                VALUE >= std::numeric_limits<long>::min()) {
                const long TEST_VALUE = static_cast<long>(VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            {
                bsl::wstring wstr = bsl::to_wstring(VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            // Set up the oracle results for unsigned integers, using the
            // 'unsigned long long' versions of the corresponding C library
            // 'sprintf' functions.

            sprintf(tempBuf, "%llu", U_VALUE);
            swprintf(wTempBuf, BUF_LEN, L"%llu", U_VALUE);

            if (U_VALUE <= std::numeric_limits<unsigned int>::max()) {
                const unsigned int TEST_VALUE =
                                            static_cast<unsigned int>(U_VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr);
            }

            if (U_VALUE <= std::numeric_limits<unsigned long>::max()) {
                const unsigned long TEST_VALUE =
                                           static_cast<unsigned long>(U_VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            {
                bsl::wstring wstr = bsl::to_wstring(U_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());

        if (verbose) printf("\nTesting extreme integer values.\n");
        {
            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }

        if (verbose) printf("\nTesting 'to_string' with floating points.\n");

        for (int ti = 0; ti < NUM_DOUBLE_DATA; ++ti){
            const int                LINE  = DOUBLE_DATA[ti].d_lineNum;
            const double             VALUE = DOUBLE_DATA[ti].d_value;

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'string'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                sprintf(tempBuf, "%f", static_cast<float>(VALUE));
                bsl::string str = bsl::to_string(static_cast<float>(VALUE));
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            if (VALUE <= std::numeric_limits<double>::max()){
                sprintf(tempBuf, "%f", static_cast<double>(VALUE));
                bsl::string str = bsl::to_string(static_cast<double>(VALUE));
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                sprintf(tempBuf, "%Lf", static_cast<long double>(VALUE));
                bsl::string str =
                               bsl::to_string(static_cast<long double>(VALUE));
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());

        if (verbose) printf("\nTesting 'to_wstring' with floating points.\n");

        for (int ti = 0; ti < NUM_DOUBLE_DATA; ++ti){
            const int    LINE  = DOUBLE_DATA[ti].d_lineNum;
            const double VALUE = DOUBLE_DATA[ti].d_value;

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'wstring'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                swprintf(wTempBuf,
                         sizeof wTempBuf / sizeof *wTempBuf,
                         L"%f",
                         static_cast<float>(VALUE));
                bsl::wstring wstr = bsl::to_wstring(static_cast<float>(VALUE));
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            if (VALUE <= std::numeric_limits<double>::max()){
                swprintf(wTempBuf,
                         sizeof wTempBuf / sizeof *wTempBuf,
                         L"%f",
                         static_cast<double>(VALUE));
                bsl::wstring wstr =
                                   bsl::to_wstring(static_cast<double>(VALUE));
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                swprintf(wTempBuf,
                         sizeof wTempBuf / sizeof *wTempBuf,
                         L"%Lf",
                         static_cast<long double>(VALUE));
                bsl::wstring wstr =
                              bsl::to_wstring(static_cast<long double>(VALUE));
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING STRING TO FLOATING POINT NUMBER CONVERSIONS
        //
        // Concerns:
        //: 1 'stof', 'stod', and 'stold' parse the string correctly into a
        //:   floating point number of the requested size.
        //:
        //: 2 The methods discard leading white space characters and create a
        //:   valid floating point number by consuming the longest sequence of
        //:   characters that can represent a floating point number..
        //:
        //: 3 Detects the correct base with leading 0X or 0x.
        //:
        //: 4 The methods detect exponents correctly.
        //:
        //: 5 The methods correctly identify INF/INFINITY as appropriate.
        //:
        //: 6 Malformed strings report errors correctly.
        //:
        //: 7 The functions are overloaded to convert from 'bsl::string' and
        //:   'bsl::wstring' objects.
        //
        // Plan:
        //: 1 Use 'stof', 'stod', and 'stold' on a variety of valid values to
        //:   ensure that the methods parse correctly (C-1)
        //:
        //: 2 Try to convert partially valid strings, ie strings that contain
        //:   characters that are not valid in the base of the number.
        //:
        //: 3 Test a variety of numbers in base 0 to check if they detect the
        //:   correct base
        //
        // Testing:
        //   float stof(const string& str, std::size_t *pos =0);
        //   float stof(const wstring& str, std::size_t *pos =0);
        //   double stod(const string& str, std::size_t *pos =0);
        //   double stod(const wstring& str, std::size_t *pos =0);
        //   long double stold(const string& str, std::size_t *pos =0);
        //   long double stold(const wstring& str, std::size_t *pos =0);
        // --------------------------------------------------------------------

        if (verbose) printf(
                  "\nTESTING STRING TO FLOATING POINT NUMBER CONVERSIONS"
                  "\n===================================================\n");

        enum { k_DEFAULT_POSITION = 54321 };

        if (verbose) printf("Testing stof, stod, and stold with strings.\n");
        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_input;            // input
                size_t      d_pos;              // position of character after
                                                // the numeric value
                double      d_spec;             // specifications
            } DATA[] = {
                //line  input               pos  spec
                //----  -----               ---  ----
                { L_,   "0",                1,    0     },
                { L_,   "-0",               2,    0     },
                { L_,   "+.5",              3,    0.5   },
                { L_,   "3.145gg",          5,    3.145 },
                { L_,   "    -5.9991",      11,  -5.9991},
                { L_,   "10e",              2,    10    },
                { L_,   "10e1",             4,    1e2   },
                { L_,   "10p2",             2,    10    },
#if !(defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS) || \
     (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR <=1800))
                { L_,   "0xf.f",            5,    15.937500},
#endif
#if __cplusplus >= 201103L
                { L_,   "inF",              3,    std::numeric_limits
                                                        <double>::infinity() },
                { L_,   "-inFinITY",        9,   -std::numeric_limits
                                                        <double>::infinity() },
                { L_,   "+InF",             4,    std::numeric_limits
                                                        <double>::infinity() },
#endif
                // Awkward - fails only for 'float'
                { L_,   "   9e99",          7,    9e99 },

                // Range errors
                { L_,   "9e9999",           0,   -1 },
                { L_,   "-9e9999",          0,   -1 },
                { L_,   "+9e9999",          0,   -1 },

                // Invalid buffers
                { L_,   "",                 0,   -2 },
                { L_,   ".",                0,   -2 },
                { L_,   "-",                0,   -2 },
                { L_,   "e",                0,   -2 },
                { L_,   ".e",               0,   -2 },
                { L_,   "+e",               0,   -2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE   = DATA[ti].d_lineNum;
                const char   *INPUT  = DATA[ti].d_input;
                const size_t  POS    = DATA[ti].d_pos;
                double        SPEC   = DATA[ti].d_spec;

                const bsl::string      inV(INPUT);
                bsl::string::size_type position = k_DEFAULT_POSITION;

                if (POS && POS != 7)  // 7 uniquely denotes the awkward row
                {
                    float value = bsl::stof(inV, NULL);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        float value = bsl::stof(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(SPEC, POS, -1 == SPEC || 7 == POS);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(SPEC, -2 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (POS)
                {
                    double value = bsl::stod(inV, NULL);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(LINE, POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(LINE, POS, position,
                            POS == position);

                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        double value = bsl::stod(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -1 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -2 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (POS)
                {
                    long double value = bsl::stold(inV, NULL);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(LINE, POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(LINE, POS, position,
                            POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        long double value = bsl::stold(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -1 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -2 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
#endif
                }
            }
        }

        if (verbose) printf("Testing stof, stod and stold with wstrings.\n");
        {
            static const struct {
                int            d_lineNum;          // source line number
                const wchar_t *d_input;            // input
                size_t         d_pos;              // position of character
                                                   // after the numeric value
                long double    d_spec;             // specifications
            } DATA[] = {
                //line  input               pos  spec
                //----  -----               ---  ----
                { L_,   L"0",               1,    0     },
                { L_,   L"-0",              2,    0     },
                { L_,   L"+.5",             3,    0.5   },
                { L_,   L"3.125gg",         5,    3.125 },
                { L_,   L"    -4.6875",     11,  -4.6875},
                { L_,   L"10e",             2,    10    },
                { L_,   L"10e1",            4,    1e2   },
                { L_,   L"10p2",            2,    10    },
#if !(defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS) || \
     (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR <=1800))
                { L_,   L"0xf.f",           5,    15.937500},
#endif
#if __cplusplus >= 201103L
                { L_,   L"inF",             3,    std::numeric_limits
                                                        <double>::infinity() },
                { L_,   L"-inFinITY",       9,   -std::numeric_limits
                                                        <double>::infinity() },
                { L_,   L"+InF",            4,    std::numeric_limits
                                                        <double>::infinity() },
#endif
                // Awkward - fails only for 'float'
                { L_,   L" 8.5e99",         7,    8.5e99 },

                // Range errors
                { L_,   L"9e9999",          0,   -1 },
                { L_,   L"-9e9999",         0,   -1 },
                { L_,   L"+9e9999",         0,   -1 },

                // Invalid buffers
                { L_,   L"",                0,   -2 },
                { L_,   L".",               0,   -2 },
                { L_,   L"-",               0,   -2 },
                { L_,   L"e",               0,   -2 },
                { L_,   L".e",              0,   -2 },
                { L_,   L"+e",              0,   -2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_lineNum;
                const wchar_t *INPUT  = DATA[ti].d_input;
                const size_t   POS    = DATA[ti].d_pos;
                long double    SPEC   = DATA[ti].d_spec;

                const bsl::wstring      inV(INPUT);
                bsl::wstring::size_type position = k_DEFAULT_POSITION;

                if (POS && POS != 7)  // 7 uniquely denotes the awkward row
                {
                    float value = bsl::stof(inV, NULL);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        float value = bsl::stof(inV, NULL);
                        ASSERTV(LINE, inV, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, POS, -1 == SPEC || 7 == POS);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, -2 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (POS)
                {
                    double value = bsl::stod(inV, NULL);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);

                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        double value = bsl::stod(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, -1 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, -2 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (POS)
                {
                    long double value = bsl::stold(inV, NULL);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        long double value = bsl::stold(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(SPEC, -1 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(SPEC, -2 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
#endif
                }
            }
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING STRING TO INTEGER CONVERSIONS
        //
        // Concerns:
        //: 1 'stoi', 'stol', and 'stoll' parse the string properly into the
        //:   correct signed integer value.
        //:
        //: 2 'stoul', and 'stoull' parse the string properly into the
        //:   correct unsigned integer value, even when the input is a string
        //:   denoting a negative number.
        //:
        //: 3 The methods discard leading white space characters and create
        //:   largest valid integral number.
        //:
        //: 4 The correct base if deduced if '0 == base'.
        //:
        //: 5 The 'stoX' functions handle null pointers to 'pos' correctly.
        //
        // Plan:
        //: 1 Use 'stoi', 'stol', 'stoll', 'stoul', and 'stoull' on a variety
        //:   of valid values to ensure that the methods parse correctly (C-1)
        //:
        //: 2 Try to convert partially valid strings, ie strings that contain
        //:   characters that are not valid in the base of the number.
        //:
        //: 3 Test a variety of numbers in base 0 to check if they detect the
        //:   correct base.
        //
        // Testing:
        //   int stoi(const string& str, std::size_t *pos = 0, int base = 10);
        //   int stoi(const wstring& str, std::size_t *pos = 0, int base = 10);
        //   long stol(const string& str, std::size_t *pos, int base);
        //   long stol(const wstring& str, std::size_t *pos, int base);
        //   unsigned long stoul(const string& s, std::size_t *pos, int base);
        //   unsigned long stoul(const wstring& s, std::size_t *pos, int base);
        //   long long stoll(const string& str, std::size_t *pos, int base);
        //   long long stoll(const wstring& str, std::size_t *pos, int base);
        //   unsigned long long stoull(const string&, std::size_t *, int);
        //   unsigned long long stoull(const wstring&, std::size_t *, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STRING TO INTEGER CONVERSIONS"
                            "\n=====================================\n");

        enum { k_DEFAULT_POSITION = 54321 };

        enum IntType {
            e_INT,
            e_LONG_INT,
            e_UNSIGNED_LONG_INT,
            e_LONG_LONG_INT,
            e_UNSIGNEDLONG_LONG_INT,
            e_NOT_INT
        };

        enum ErrType {
            e_PASS,
            e_INVALID,
            e_RANGE
        };

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_input;            // input
            int         d_base;             // base of input
            size_t      d_pos;              // position of character after the
                                            // numeric value
            long long   d_spec;             // specifications
        } DATA[] = {
            //line input                   base  pos  spec
            //---- -----                   ----  ---  ----
            { L_,  "0",                    10,   1,   0    },
            { L_,  "0w",                   10,   1,   0    },
            { L_,  "0x",                   10,   1,   0    },
            { L_,  "-0",                   10,   2,   0    },
            { L_,  "10101",                10,   5,   10101},
            { L_,  "-10101",               10,   6,  -10101},
            { L_,  "32767",                10,   5,   32767},
            { L_,  "-32767",               10,   6,  -32767},
            { L_,  "000032767",            10,   9,   32767},
            { L_,  "2147483647",           10,   10,  2147483647LL},
            { L_,  "2147483648",           10,   10,  2147483648LL},
            { L_,  "-2147483647",          10,   11, -2147483647LL},
            { L_,  "-2147483648",          10,   11, -2147483648LL},
            { L_,  " -2147483649",         10,   12, -2147483649LL},
            { L_,  "4294967295",           10,   10,  4294967295LL},
            { L_,  "9223372036854775807",  10,   19,  9223372036854775807LL  },
            { L_,  "-9223372036854775807", 10,   20, -9223372036854775807LL  },
            { L_,  "-9223372036854775808", 10,   20, -9223372036854775807LL-1},
            { L_,  "-9223372036854775809", 10,   20,  9223372036854775807LL  },
            { L_,  "19223372036854775808", 10,   0,   0},

            //test usage of spaces, and non valid characters with in the string
            { L_,  "  515",                10,   5,   515},
            { L_,  "  515  505050",        10,   5,   515},
            { L_,  " 99abc99",             10,   3,   99 },
            { L_,  " 3.14159",             10,   2,   3  },
            { L_,  "0x555",                10,   1,   0  },

            //test different bases
            { L_,  "111",                  2,    3,   7      },
            { L_,  "101",                  2,    3,   5      },
            { L_,  "100",                  2,    3,   4      },
            { L_,  "101010101010 ",        2,    12,  2730   },
            { L_,  "1010101010102 ",       2,    12,  2730   },
            { L_,  "111111111111111",      2,    15,  32767  },
            { L_,  "-111111111111111",     2,    16, -32767  },
            { L_,  "77777",                8,    5,   32767  },
            { L_,  "-77777",               8,    6,  -32767  },
            { L_,  "7FFF",                 16,   4,   32767  },
            { L_,  "0x7FfF",               16,   6,   32767  },
            { L_,  "-00000x7FFf",          16,   6,  -0      },
            { L_,  "ZZZZ",                 36,   4,   1679615},

            // base zero
            { L_,  "79FFZZZf",             0,    2,   79   },
            { L_,  "0xFfAb",               0,    6,   65451},
            { L_,  "05471",                0,    5,   2873 },
            { L_,  "0X5471",               0,    6,   21617},
            { L_,  "5471",                 0,    4,   5471 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("Testing stoi, stol, stoll, stoul and stoull with"
                            " 'char' strings.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_lineNum;
            const char        *INPUT  = DATA[ti].d_input;
            const int          BASE   = DATA[ti].d_base;
            const size_t       POS    = DATA[ti].d_pos;
            const long long    SPEC   = DATA[ti].d_spec;

            if (veryVerbose) {
                P_(INPUT) P_(BASE) P_(POS) P(SPEC);
            }

            const bsl::string      STRING_VALUE(INPUT);
            bsl::string::size_type position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<int>::max()
             && SPEC >= std::numeric_limits<int>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<long>::max()
             && SPEC >= std::numeric_limits<long>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                long value = bsl::stol(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long value = bsl::stol(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (((sizeof(long) == sizeof(long long)
                 || SPEC <= std::numeric_limits<unsigned long>::max())
                 && SPEC >= std::numeric_limits<long>::min()
                 && POS)
             || POS == 12) {
                const unsigned long EXPECT = static_cast<unsigned long>(SPEC);

                unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (POS && (POS < 20 || SPEC < 0)) {
                long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;

            if (POS) {
                const unsigned long long EXPECT = SPEC;

                unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;
        }

        static const struct {
            int            d_lineNum;      // source line number
            const wchar_t *d_input;        // input
            int            d_base;         // base of input
            size_t         d_pos;          // position of character after the
                                           // numeric value
            long long      d_spec;         // specifications
        } WDATA[] = {
            //line input                   base  pos  spec
            //---- -----                   ----  ---  ----
            { L_,  L"0",                    10,  1,   0    },
            { L_,  L"0w",                   10,  1,   0    },
            { L_,  L"0x",                   10,  1,   0    },
            { L_,  L"-0",                   10,  2,   0    },
            { L_,  L"10101",                10,  5,   10101},
            { L_,  L"-10101",               10,  6,  -10101},
            { L_,  L"32767",                10,  5,   32767},
            { L_,  L"-32767",               10,  6,  -32767},
            { L_,  L"000032767",            10,  9,   32767},
            { L_,  L"2147483647",           10,  10,  2147483647LL},
            { L_,  L"2147483648",           10,  10,  2147483648LL},
            { L_,  L"-2147483647",          10,  11, -2147483647LL},
            { L_,  L"-2147483648",          10,  11, -2147483648LL},
            { L_,  L" -2147483649",         10,  12, -2147483649LL},
            { L_,  L"4294967295",           10,  10,  4294967295LL},
            { L_,  L"9223372036854775807",  10,  19,  9223372036854775807LL  },
            { L_,  L"-9223372036854775807", 10,  20, -9223372036854775807LL  },
            { L_,  L"-9223372036854775808", 10,  20, -9223372036854775807LL-1},
            { L_,  L"-9223372036854775809", 10,  20,  9223372036854775807LL  },
            { L_,  L"19223372036854775808", 10,  0,   0},

            //test usage of spaces, and non valid characters with in the string
            { L_,  L"  515",                10,  5,   515},
            { L_,  L"  515  505050",        10,  5,   515},
            { L_,  L" 99abc99",             10,  3,   99 },
            { L_,  L" 3.14159",             10,  2,   3  },
            { L_,  L"0x555",                10,  1,   0  },

            //test different bases
            { L_,  L"111",                  2,   3,   7      },
            { L_,  L"101",                  2,   3,   5      },
            { L_,  L"100",                  2,   3,   4      },
            { L_,  L"101010101010 ",        2,   12,  2730   },
            { L_,  L"1010101010102 ",       2,   12,  2730   },
            { L_,  L"111111111111111",      2,   15,  32767  },
            { L_,  L"-111111111111111",     2,   16, -32767  },
            { L_,  L"77777",                8,   5,   32767  },
            { L_,  L"-77777",               8,   6,  -32767  },
            { L_,  L"7FFF",                 16,  4,   32767  },
            { L_,  L"0x7FfF",               16,  6,   32767  },
            { L_,  L"-00000x7FFf",          16,  6,  -0      },
            { L_,  L"ZZZZ",                 36,  4,   1679615},

            // base zero
            { L_,  L"79FFZZZf",             0,   2,   79   },
            { L_,  L"0xFfAb",               0,   6,   65451},
            { L_,  L"05471",                0,   5,   2873 },
            { L_,  L"0X5471",               0,   6,   21617},
            { L_,  L"5471",                 0,   4,   5471 },
        };
        const int NUM_WDATA = sizeof WDATA / sizeof *WDATA;

        if (verbose) printf("Testing stoi, stol, stoll, stoul and stoull with"
                            " 'wstring'.\n");

        for (int ti = 0; ti < NUM_WDATA; ++ti) {
            const int           LINE   = WDATA[ti].d_lineNum;
            const wchar_t      *INPUT  = WDATA[ti].d_input;
            const int           BASE   = WDATA[ti].d_base;
            const size_t        POS    = WDATA[ti].d_pos;
            const long long     SPEC   = WDATA[ti].d_spec;

            if (veryVerbose) {
                P_(INPUT) P_(BASE) P_(POS) P(SPEC);
            }

            const bsl::wstring      STRING_VALUE(INPUT);
            bsl::wstring::size_type position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<int>::max()
             && SPEC >= std::numeric_limits<int>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<long>::max()
             && SPEC >= std::numeric_limits<long>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                long value = bsl::stol(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long value = bsl::stol(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (((sizeof(long) == sizeof(long long) ||
                   SPEC <= std::numeric_limits<unsigned long>::max())
                && SPEC >= std::numeric_limits<long>::min()
                && POS)
             || POS == 12) {
                const unsigned long EXPECT = static_cast<unsigned long>(SPEC);

                unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (POS && (POS < 20 || SPEC < 0)) {
                long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;

            if (POS) {
                const unsigned long long EXPECT = SPEC;

                unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;
        }

#if defined(BDE_BUILD_TARGET_EXC)
        // Testing error handling requires catching exceptions thrown from the
        // 'bsl' function as the standard-mandated error-reporting technique.

        if (verbose) printf("Testing error handling for stoi, stol, stoll,"
                            "  stoul and stoull with 'char' strings.\n");
        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_input;            // input
                int         d_base;             // base of input
                ErrType     d_error;            // expected exception type
            } DATA[] = {
                //line  input                   base   error
                //----  -----                   ----   -----
                { L_,   "0",                    10,    e_PASS },
                { L_,   "-0",                   10,    e_PASS },
                { L_,   "--0",                  10,    e_INVALID },
                { L_,   "-+0",                  10,    e_INVALID },
                { L_,   "+0",                   10,    e_PASS },
                { L_,   "+-0",                  10,    e_INVALID },
                { L_,   "++0",                  10,    e_INVALID },
                { L_,   "",                     10,    e_INVALID },
                { L_,   "-",                    10,    e_INVALID },
                { L_,   "+",                    10,    e_INVALID },

                { L_,   " 0",                   10,    e_PASS },
                { L_,   " -0",                  10,    e_PASS },
                { L_,   " --0",                 10,    e_INVALID },
                { L_,   " -+0",                 10,    e_INVALID },
                { L_,   " +0",                  10,    e_PASS },
                { L_,   " +-0",                 10,    e_INVALID },
                { L_,   " ++0",                 10,    e_INVALID },
                { L_,   " ",                    10,    e_INVALID },
                { L_,   " -",                   10,    e_INVALID },
                { L_,   " +",                   10,    e_INVALID },

                { L_,   "0",                    10,    e_PASS },
                { L_,   "0",                    36,    e_PASS },

                { L_,   "-0x",                  10,    e_PASS },
                { L_,   "--0x",                 10,    e_INVALID },
                { L_,   "-+0x",                 10,    e_INVALID },
                { L_,   "+0x",                  10,    e_PASS },
                { L_,   "+-0x",                 10,    e_INVALID },
                { L_,   "++0x",                 10,    e_INVALID },

                { L_,   "-0x",                  36,    e_PASS },
                { L_,   "--0x",                 36,    e_INVALID },
                { L_,   "-+0x",                 36,    e_INVALID },
                { L_,   "+0x",                  36,    e_PASS },
                { L_,   "+-0x",                 36,    e_INVALID },
                { L_,   "++0x",                 36,    e_INVALID },
                { L_,   "",                     36,    e_INVALID },
                { L_,   "-",                    36,    e_INVALID },
                { L_,   "+",                    36,    e_INVALID },

                { L_,   "0x",                   0,     e_PASS },
                { L_,   "-0x",                  0,     e_PASS },
                { L_,   "--0x",                 0,     e_INVALID },
                { L_,   "-+0x",                 0,     e_INVALID },
                { L_,   "+0x",                  0,     e_PASS },
                { L_,   "+-0x",                 0,     e_INVALID },
                { L_,   "++0x",                 0,     e_INVALID },
                { L_,   "",                     0,     e_INVALID },
                { L_,   "-",                    0,     e_INVALID },
                { L_,   "+",                    0,     e_INVALID },

                { L_,   "0x",                   16,    e_PASS },
                { L_,   "-0x",                  16,    e_PASS },
                { L_,   "--0x",                 16,    e_INVALID },
                { L_,   "-+0x",                 16,    e_INVALID },
                { L_,   "+0x",                  16,    e_PASS },
                { L_,   "+-0x",                 16,    e_INVALID },
                { L_,   "++0x",                 16,    e_INVALID },

                { L_,   "+Z",                   35,    e_INVALID },
                { L_,   "+z",                   36,    e_PASS },
                { L_,   "ZzZzzZZzzZZzzzz",      36,    e_RANGE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int intType = e_INT; intType != e_NOT_INT; ++intType) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int          LINE   = DATA[ti].d_lineNum;
                    const char        *INPUT  = DATA[ti].d_input;
                    const int          BASE   = DATA[ti].d_base;
                    const ErrType      ERROR  = DATA[ti].d_error;

                    const bsl::string  STRING_VALUE(INPUT);

                    if (veryVerbose) {
                        T_ P_(INPUT) P_(BASE) P((int)ERROR);
                    }

                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                          bsl::stoul(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_LONG_LONG_INT : {
                            long long value =
                                          bsl::stoll(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                    }

                    size_t next = 0;
                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                         bsl::stoul(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_LONG_LONG_INT : {
                            long long value =
                                         bsl::stoll(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                        bsl::stoull(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                        ASSERTV(next, !next);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                        ASSERTV(next, !next);
                    }
                }

                if (veryVerbose) printf("negative testing");
                {
                    bsls::AssertTestHandlerGuard guard;

                    size_t next = 0;
                    for (int i = -3; i != 43; ++i) {
                        if (!i || (2 <= i && i <= 36)) {
                            ASSERT_PASS(bsl::stoi("0", NULL, i));
                            ASSERT_PASS(bsl::stoi("-1", &next, i));
                        }
                        else {
                            ASSERT_FAIL(bsl::stoi("0", NULL, i));
                            ASSERT_FAIL(bsl::stoi("-1", &next, i));
                        }
                    }
                }
            }
        }

        if (verbose) printf("Testing error handling for stoi, stol, stoll,"
                            "  stoul and stoull with 'wchar_t' strings.\n");
        {
            static const struct {
                int            d_lineNum;          // source line number
                const wchar_t *d_input;            // input
                int            d_base;             // base of input
                ErrType        d_error;            // expected exception type
            } DATA[] = {
                //line  input                   base   error
                //----  -----                   ----   -----
                { L_,   L"0",                    10,    e_PASS },
                { L_,   L"-0",                   10,    e_PASS },
                { L_,   L"--0",                  10,    e_INVALID },
                { L_,   L"-+0",                  10,    e_INVALID },
                { L_,   L"+0",                   10,    e_PASS },
                { L_,   L"+-0",                  10,    e_INVALID },
                { L_,   L"++0",                  10,    e_INVALID },
                { L_,   L"",                     10,    e_INVALID },
                { L_,   L"-",                    10,    e_INVALID },
                { L_,   L"+",                    10,    e_INVALID },

                { L_,   L" 0",                   10,    e_PASS },
                { L_,   L" -0",                  10,    e_PASS },
                { L_,   L" --0",                 10,    e_INVALID },
                { L_,   L" -+0",                 10,    e_INVALID },
                { L_,   L" +0",                  10,    e_PASS },
                { L_,   L" +-0",                 10,    e_INVALID },
                { L_,   L" ++0",                 10,    e_INVALID },
                { L_,   L" ",                    10,    e_INVALID },
                { L_,   L" -",                   10,    e_INVALID },
                { L_,   L" +",                   10,    e_INVALID },

                { L_,   L"0",                    10,    e_PASS },
                { L_,   L"0",                    36,    e_PASS },

                { L_,   L"-0x",                  10,    e_PASS },
                { L_,   L"--0x",                 10,    e_INVALID },
                { L_,   L"-+0x",                 10,    e_INVALID },
                { L_,   L"+0x",                  10,    e_PASS },
                { L_,   L"+-0x",                 10,    e_INVALID },
                { L_,   L"++0x",                 10,    e_INVALID },

                { L_,   L"-0x",                  36,    e_PASS },
                { L_,   L"--0x",                 36,    e_INVALID },
                { L_,   L"-+0x",                 36,    e_INVALID },
                { L_,   L"+0x",                  36,    e_PASS },
                { L_,   L"+-0x",                 36,    e_INVALID },
                { L_,   L"++0x",                 36,    e_INVALID },
                { L_,   L"",                     36,    e_INVALID },
                { L_,   L"-",                    36,    e_INVALID },
                { L_,   L"+",                    36,    e_INVALID },

                { L_,   L"0x",                   0,     e_PASS },
                { L_,   L"-0x",                  0,     e_PASS },
                { L_,   L"--0x",                 0,     e_INVALID },
                { L_,   L"-+0x",                 0,     e_INVALID },
                { L_,   L"+0x",                  0,     e_PASS },
                { L_,   L"+-0x",                 0,     e_INVALID },
                { L_,   L"++0x",                 0,     e_INVALID },
                { L_,   L"",                     0,     e_INVALID },
                { L_,   L"-",                    0,     e_INVALID },
                { L_,   L"+",                    0,     e_INVALID },

                { L_,   L"0x",                   16,    e_PASS },
                { L_,   L"-0x",                  16,    e_PASS },
                { L_,   L"--0x",                 16,    e_INVALID },
                { L_,   L"-+0x",                 16,    e_INVALID },
                { L_,   L"+0x",                  16,    e_PASS },
                { L_,   L"+-0x",                 16,    e_INVALID },
                { L_,   L"++0x",                 16,    e_INVALID },

                { L_,   L"+Z",                   35,    e_INVALID },
                { L_,   L"+z",                   36,    e_PASS },
                { L_,   L"ZzZzzZZzzZZzzzz",      36,    e_RANGE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int intType = e_INT; intType != e_NOT_INT; ++intType) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int          LINE   = DATA[ti].d_lineNum;
                    const wchar_t     *INPUT  = DATA[ti].d_input;
                    const int          BASE   = DATA[ti].d_base;
                    const ErrType      ERROR  = DATA[ti].d_error;

                    const bsl::wstring  STRING_VALUE(INPUT);

                    if (veryVerbose) {
                        T_ P_(INPUT) P_(BASE) P((int)ERROR);
                    }

                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, INPUT, intType, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, INPUT, intType, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                          bsl::stoul(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, INPUT, intType, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_LONG_LONG_INT : {
                            long long value =
                                          bsl::stoll(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, intType, INPUT, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          }
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                    }

                    size_t next = 0;
                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                         bsl::stoul(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_LONG_LONG_INT : {
                            long long value =
                                         bsl::stoll(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                        bsl::stoull(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          }
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                        ASSERTV(next, !next);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                        ASSERTV(next, !next);
                    }
                }

                if (veryVerbose) printf("negative testing");
                {
                    bsls::AssertTestHandlerGuard guard;

                    size_t next = 0;
                    for (int i = -3; i != 43; ++i) {
                        if (!i || (2 <= i && i <= 36)) {
                            ASSERT_PASS(bsl::stoi("0", NULL, i));
                            ASSERT_PASS(bsl::stoi("-1", &next, i));
                        }
                        else {
                            ASSERT_FAIL(bsl::stoi("0", NULL, i));
                            ASSERT_FAIL(bsl::stoi("-1", &next, i));
                        }
                    }
                }
            }
        }
#endif // BDE_BUILD_TARGET_EXC
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Verify that the hashAppend function works properly and is picked
        //   up by 'bslh::Hash'
        //
        // Concerns:
        //: 1 'bslh::Hash' picks up 'hashAppend(string)' and can hash strings
        //: 2 'hashAppend' hashes the entire string, regardless of 'char' or
        //:   'wchar'
        //
        // Plan:
        //: 1 Use 'bslh::Hash' to hash a few values of strings with each char
        //:   type. (C-1,2)
        //
        // Testing:
        //   hashAppend(HASHALG& hashAlg, const basic_string& str);
        //   hashAppend(HASHALG& hashAlg, const native_std::basic_string& str);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase29();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase29();

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING THE SHORT STRING OPTIMIZATION
        //
        // Concerns:
        //  - String should have an initial non-zero capacity (short string
        //    buffer).
        //  - It shouldn't allocate up to that capacity.
        //  - It should work with the char_type larger than the short string
        //    buffer.
        //  - It should work with the NULL-terminator different from '\0' to
        //    make sure that the implementation always uses char_type() default
        //    constructor to terminate the string rather than a null literal.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase28' for details.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING THE SHORT STRING OPTIMIZATION"
                            "\n=====================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase28();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase28();

        if (verbose)
            printf("\n... with 'UserChar' that can be pretty large.\n");
        TestDriver<UserChar<1> >::testCase28();
        TestDriver<UserChar<2> >::testCase28();
        TestDriver<UserChar<3> >::testCase28();
        TestDriver<UserChar<4> >::testCase28();
        TestDriver<UserChar<5> >::testCase28();
        TestDriver<UserChar<6> >::testCase28();
        TestDriver<UserChar<7> >::testCase28();
        TestDriver<UserChar<8> >::testCase28();
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // REPRODUCING KNOWN BUG CAUSING SEGFAULT IN 'find'
        //   This is a problem with the native library, being pursued in DRQS
        //   16870796.  This test will do nothing unless run in verbose mode.
        //
        // Concerns:
        //   That a known bug in string::find on Sun cc is reproduced in this
        //   test suite.
        //
        // Plan:
        //
        // Testing:
        //   DRQS 16870796
        // --------------------------------------------------------------------

        if (verbose)
                printf("\nREPRODUCING KNOWN BUG CAUSING SEGFAULT IN 'find'"
                       "\n================================================\n");

        if (verbose) printf("\nReproducing known segfault in string::find"
                            "\n==========================================\n");

        if (verbose) {
            const char *pc = std::char_traits<char>::find("bcabcd", 2, 'a');
            P((const void *) pc);
            ASSERT(0 == pc);

            const wchar_t *pw =
                           std::char_traits<wchar_t>::find(L"bcabcd", 2, L'a');
            P((const void *) pw);
            ASSERT(0 == pw);

            bsl::basic_string<wchar_t, std::char_traits<wchar_t> > s =
                                                                 L"aababcabcd";
            s.find(L"abcde", 0, 5);    // segfaults
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING CONVERSIONS WITH NATIVE STRINGS
        //
        // Concerns:
        //: 1 A bsl::basic_string is implicitly convertible to a
        //:     native_std::basic_string with the same CHAR_TYPE and
        //: 2   CHAR_TRAITS.
        //:     A native_std::basic_string is implicitly convertible to a
        //:     bsl::basic_string with the same CHAR_TYPE and
        //: 3   CHAR_TRAITS.
        //:     A bsl::basic_string and a native_std::basic_string with the
        //:     same template parameters will have the same npos value.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase26' for details.
        //
        // Testing:
        //   'npos' VALUE
        //   operator native_std::basic_string<CHAR, TRAITS, A2>() const;
        //   basic_string(const native_std::basic_string<CHAR, TRAITS, A2>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSIONS WITH NATIVE STRINGS"
                            "\n=======================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase26();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase26();

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTIONS
        //
        // Concerns:
        //: 1 The expect 'logic_error' derived exceptions are thrown where
        //:   mandated by the C++ standard.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase25' for details.
        //
        // Testing:
        //   CONCERN: 'std::length_error' is used properly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EXCEPTIONS"
                            "\n==================\n");

        if (verbose) printf("\nTesting use of 'std::length_error'"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase25();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase25();

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING COMPARISONS
        //
        // Concerns:
        //: 1 The 'compare' member function and comparison operators return the
        //:   canonical ordering of two string values.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase24' for details.
        //
        // Testing:
        //   int compare(const string& str) const;
        //   int compare(pos1, n1, const string& str) const;
        //   int compare(pos1, n1, const string& str, pos2, n2) const;
        //   int compare(const C *s) const;
        //   int compare(pos1, n1, const C *s) const;
        //   int compare(pos1, n1, const C *s, n2) const;
        //   bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator<(const C *, const string<C,CT,A>&);
        //   bool operator<(const string<C,CT,A>&, const C *);
        //   bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator>(const C *, const string<C,CT,A>&);
        //   bool operator>(const string<C,CT,A>&, const C *);
        //   bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator<=(const C *, const string<C,CT,A>&);
        //   bool operator<=(const string<C,CT,A>&, const C *);
        //   bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator>=(const C *, const string<C,CT,A>&);
        //   bool operator>=(const string<C,CT,A>&, const C *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISONS"
                            "\n===================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase24();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase24();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative testing comparisons"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase24Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase24Negative();
#endif

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING SUBSTRING OPERATIONS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase23' for details.
        //
        // Testing:
        //   string substr(pos, n) const;
        //   size_type copy(char *s, n, pos = 0) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SUBSTRING OPERATIONS"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase23();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase23();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing 'copy'"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase23Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase23Negative();
#endif

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'find...' VARIANTS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase22' for details.
        //
        // Testing:
        //   size_type find(const string& str, pos = 0) const;
        //   size_type find(const C *s, pos, n) const;
        //   size_type find(const C *s, pos = 0) const;
        //   size_type find(C c, pos = 0) const;
        //   size_type rfind(const string& str, pos = 0) const;
        //   size_type rfind(const C *s, pos, n) const;
        //   size_type rfind(const C *s, pos = 0) const;
        //   size_type rfind(C c, pos = 0) const;
        //   size_type find_first_of(const string& str, pos = 0) const;
        //   size_type find_first_of(const C *s, pos, n) const;
        //   size_type find_first_of(const C *s, pos = 0) const;
        //   size_type find_first_of(C c, pos = 0) const;
        //   size_type find_last_of(const string& str, pos = 0) const;
        //   size_type find_last_of(const C *s, pos, n) const;
        //   size_type find_last_of(const C *s, pos = 0) const;
        //   size_type find_last_of(C c, pos = 0) const;
        //   size_type find_first_not_of(const string& str, pos = 0) const;
        //   size_type find_first_not_of(const C *s, pos, n) const;
        //   size_type find_first_not_of(const C *s, pos = 0) const;
        //   size_type find_first_not_of(C c, pos = 0) const;
        //   size_type find_last_not_of(const string& str, pos = 0) const;
        //   size_type find_last_not_of(const C *s, pos, n) const;
        //   size_type find_last_not_of(const C *s, pos = 0) const;
        //   size_type find_last_not_of(C c, pos = 0) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'find...' VARIANTS"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase22();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase22();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative testing 'find...' methods."
                            "\n===================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase22Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase22Negative();
#endif

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase21' for details.
        //
        // Testing:
        //   void swap(basic_string& other);
        //   void swap(basic_string& lhs, basic_string& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase21();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase21();

        if (verbose) printf("\nTesting Allocator Propagation on Swap"
                            "\n=====================================\n");
        TestDriver<char>::testCase21_propagate_on_container_swap();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'replace'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase20' for details.
        //
        // Testing:
        //   basic_string& replace(pos1, n1, const string& str);
        //   basic_string& replace(pos1, n1, const string& str, pos2, n2);
        //   basic_string& replace(pos1, n1, const C *s, n2);
        //   basic_string& replace(pos1, n1, const C *s);
        //   basic_string& replace(pos1, n1, size_type n2, C c);
        //   basic_string& replace(const_iterator p, q, const string& str);
        //   basic_string& replace(const_iterator p, q, const C *s, n2);
        //   basic_string& replace(const_iterator p, q, const C *s);
        //   basic_string& replace(const_iterator p, q, size_type n2, C c);
        //   template <It> basic_string& replace(const_iterator p, q, It f, l);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'replace'"
                            "\n=================\n");

        if (verbose) printf("\nTesting 'replace' with value"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase20();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase20();

        if (verbose) printf("\n... with 'char' & matching integral types.\n");
        TestDriver<char>::testCase20MatchTypes();

        if (verbose) printf("\n... with 'wchar_t' & matching integ types.\n");
        TestDriver<wchar_t>::testCase20MatchTypes();

        if (verbose) printf("\nTesting 'replace' with range"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase20Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase20Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase20Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase20Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing 'replace'"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase20Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase20Negative();
#endif

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'erase' AND 'pop_back'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase19' for details.
        //
        // Testing:
        //   iterator erase(size_type pos = 0, size_type n = npos);
        //   iterator erase(const_iterator position);
        //   iterator erase(const_iterator first, const_iterator last);
        //   void pop_back();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'erase' AND 'pop_back'"
                            "\n==============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase19();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase19();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing for 'erase' and 'pop_back'"
                            "\n===========================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase19Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase19Negative();
#endif

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase18' for details.
        //
        // Testing:
        //   basic_string& insert(size_type pos, size_type n, CHAR_TYPE c);
        //   iterator insert(const_iterator pos, CHAR_TYPE value);
        //   iterator insert(const_iterator pos, size_type n, CHAR_TYPE value);
        //   template <class Iter> iterator insert(const_iterator, Iter, Iter);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INSERTION"
                            "\n=================\n");

        if (verbose) printf("\nTesting Value Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase18();

        if (verbose) printf("\nTesting Range Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase18Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase18Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase18Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase18Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Insertion"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase18Negative();
#endif

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'append'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase17' for details.
        //
        // Testing:
        //   basic_string& operator+=(CHAR_TYPE c);
        //   basic_string& operator+=(const StringRefData& strRefData);
        //   basic_string& append(const basic_string& str);
        //   basic_string& append(const basic_string& str, pos, n);
        //   basic_string& append(const CHAR_TYPE *s, size_type n);
        //   basic_string& append(const CHAR_TYPE *s);
        //   basic_string& append(size_type n, CHAR_TYPE c);
        //   template <class Iter> basic_string& append(Iter first, Iter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'append'"
                            "\n================\n");

        if (verbose) printf("\nTesting Value Append"
                            "\n====================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase17();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase17();

        if (verbose) printf("\nTesting Range Append"
                            "\n====================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase17Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase17Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase17Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase17Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Range Append"
                            "\n=============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase17Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase17Negative();
#endif

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase16' for details.
        //
        // Testing:
        //   iterator begin();
        //   iterator end();
        //   reverse_iterator rbegin();
        //   reverse_iterator rend();
        //   const_iterator begin() const;
        //   const_iterator end() const;
        //   const_reverse_iterator rbegin() const;
        //   const_reverse_iterator rend() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ITERATORS"
                            "\n=================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase16();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase16();

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase15' for details.
        //
        // Testing:
        //   reference operator[](size_type pos);
        //   reference at(size_type pos);
        //   reference front();
        //   reference back();
        //   const_reference front() const;
        //   const_reference back() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ELEMENT ACCESS"
                            "\n======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase15();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Element Access"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase15Negative();
#endif

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'reserve' AND 'capacity'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase14' for details.
        //
        // Testing:
        //   void reserve(size_type n);
        //   void resize(size_type n);
        //   void resize(size_type n, CHAR_TYPE c);
        //   size_type max_size() const;
        //   size_type capacity() const;
        //   bool empty() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'reserve' AND 'capacity'"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase14();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase14();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase13' for details.
        //
        // Testing:
        //   basic_string& assign(const basic_string& str);
        //   basic_string& assign(bslmf::MovableRef<basic_string> str);
        //   basic_string& assign(const basic_string& str, pos, n);
        //   basic_string& assign(const CHAR_TYPE *s, size_type n);
        //   basic_string& assign(const CHAR_TYPE *s);
        //   basic_string& assign(size_type n, CHAR_TYPE c);
        //   template <class Iter> basic_string& assign(Iter first, Iter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT"
                            "\n==================\n");

        if (verbose) printf("\nTesting Initial-Length Assignment"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase13();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase13();

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase13InputIterator();

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary forward iterator.\n");
        TestDriver<char>::testCase13Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase13Range(CharArray<char>());

        if (verbose) printf("\n... with 'char' "
                            "and StringRefData.\n");
        TestDriver<char>::testCase13StrRefData();

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase13InputIterator();

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary forward iterator.\n");
        TestDriver<wchar_t>::testCase13Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase13Range(CharArray<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and StringRefData.\n");
        TestDriver<wchar_t>::testCase13StrRefData();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Assignment"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase13Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase13Negative();
#endif

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase12' for details.
        //
        // Testing:
        //   basic_string(const basic_string& str, pos, n = npos, a = A());
        //   basic_string(const CHAR_TYPE *s, a = A());
        //   basic_string(const CHAR_TYPE *s, size_type n, a = A());
        //   basic_string(size_type n, CHAR_TYPE c = CHAR_TYPE(), a = A());
        //   template<class Iter> basic_string(Iter first, Iter last, a = A());
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        if (verbose) printf("\nTesting Initial-Length Constructor"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase12();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase12();

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase12Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase12Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase12Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase12Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative testing of Constructors"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase12Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase12Negative();
#endif

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST TYPE TRAITS
        //   In this test we verify that the following set of the traits have
        //   expected values:
        //   - 'bslma::UsesBslmaAllocator'
        //   - 'bslmf::IsBitwiseMoveable'
        //   - 'bslalg::HasStlIterators'
        //   - 'bsl::is_nothrow_move_constructible'
        //
        // Concerns:
        //: 1 'bslma::UsesBslmaAllocator' trait is set for string types that
        //:    use 'bslma' allocators and NOT set otherwise.
        //:
        //: 2 'bsl::basic_string' should be bitwise movable if the allocator
        //:   type is bitwise movable.
        //:
        //: 3 'bslalg::HasStlIterators' trait is set for 'bsl::basic_string'
        //:
        //: 4 'bsl::is_nothrow_move_constructible' trait is set when compiled
        //:   in C++11 mode.
        //
        // Plan:
        //: 1 Use 'ASSERT' to verify the type traits.  (C-1..4)
        //
        // Testing:
        //   CONCERN: The object has the necessary type traits
        // --------------------------------------------------------------------

        if (veryVerbose) printf("\nTESTING TYPE TRAITS"
                                "\n===================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
        const bool EXP_NOTHROW = true;
#else
        const bool EXP_NOTHROW = false;
#endif

        if (veryVerbose) printf("\tTesting 'bsl::string'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::string>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::string>::value);
        ASSERT(bslalg::HasStlIterators<bsl::string>::value);
        ASSERT(EXP_NOTHROW ==
                       bsl::is_nothrow_move_constructible<bsl::string>::value);


        if (veryVerbose) printf("\tTesting 'bsl::wstring'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::wstring>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::wstring>::value);
        ASSERT(bslalg::HasStlIterators<bsl::wstring>::value);
        ASSERT(EXP_NOTHROW ==
                      bsl::is_nothrow_move_constructible<bsl::wstring>::value);

        if (veryVerbose) printf("\tTesting 'native_std::string'.\n");

        ASSERT(!bslma::UsesBslmaAllocator<native_std::string>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<native_std::string>::value);

        // SUN for some reasons declares this trait for native strings.
#if !defined(BSLS_PLATFORM_CMP_SUN)
        ASSERT(!bslalg::HasStlIterators<native_std::string>::value);
#endif

        // MSVC has unconditional 'noexcept' specifications for move
        // constructor in C++11 mode.  Fixed in MSVC 2015.
#if !defined(BSLS_PLATFORM_CMP_MSVC) || \
    (defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900))
        ASSERT(EXP_NOTHROW ==
                bsl::is_nothrow_move_constructible<native_std::string>::value);
#endif

        if (veryVerbose) printf("\tTesting 'native_std::wstring'.\n");

        ASSERT(!bslma::UsesBslmaAllocator<native_std::wstring>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<native_std::wstring>::value);

#if !defined(BSLS_PLATFORM_CMP_SUN)
        ASSERT(!bslalg::HasStlIterators<native_std::wstring>::value);
#endif

#if !defined(BSLS_PLATFORM_CMP_MSVC) || \
    (defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900))
        ASSERT(EXP_NOTHROW ==
               bsl::is_nothrow_move_constructible<native_std::wstring>::value);
#endif

        if (veryVerbose)
            printf("\tTesting string with bitwise movable allocator.\n");

        typedef bslma::StdTestAllocator<char>  BitwiseMoveableAllocator;
        typedef bsl::basic_string<char,
                                  bsl::char_traits<char>,
                                  BitwiseMoveableAllocator> StringBM;

        ASSERT(bslma::UsesBslmaAllocator<StringBM>::value);
        ASSERT(bslmf::IsBitwiseMoveable<BitwiseMoveableAllocator>::value);
        ASSERT(bslmf::IsBitwiseMoveable<StringBM>::value);
        ASSERT(bslalg::HasStlIterators<StringBM>::value);
        ASSERT(EXP_NOTHROW ==
                          bsl::is_nothrow_move_constructible<StringBM>::value);

        if (veryVerbose)
            printf("\tTesting string with not bitwise movable allocator.\n");

        typedef LimitAllocator<bsl::allocator<char> >
                                                   NotBitwiseMoveableAllocator;
        typedef bsl::basic_string<char,
                                  bsl::char_traits<char>,
                                  NotBitwiseMoveableAllocator> StringNBM;

        ASSERT(!bslma::UsesBslmaAllocator<StringNBM>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<NotBitwiseMoveableAllocator>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<StringNBM>::value);
        ASSERT(bslalg::HasStlIterators<StringNBM>::value);
        ASSERT(EXP_NOTHROW ==
                         bsl::is_nothrow_move_constructible<StringNBM>::value);

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STREAMING FUNCTIONALITY"
                            "\n===============================\n");

        if (verbose)
            printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //   Now that we can generate many values for our test objects, and
        //   compare results of assignments, we can test the assignment
        //   operator.    This is achieved by the 'testCase9' class method of
        //   the test driver template, instantiated for the basic test type.
        //   See that function for a list of concerns and a test plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase9' for details.
        //
        // Testing:
        //   basic_string& operator=(const basic_string& rhs);
        //   basic_string& operator=(MovableRef<basic_string> rhs);
        //   basic_string& operator=(const CHAR_TYPE *s); [NEGATIVE ONLY]
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATORS"
                            "\n===========================\n");

        if (verbose) printf("\nTesting Copy Assignment Operator"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase9();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Assignment Operator"
                            "\n====================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase9Negative();
#endif

        if (verbose) printf("\nTesting Move Assignment Operator"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9Move();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase9Move();

        if (verbose) printf("\nTesting Allocator Propagation on Copy"
                            "\n=====================================\n");
        TestDriver<char>::testCase9_propagate_on_container_copy_assignment();

        if (verbose) printf("\nTesting Allocator Propagation on Move"
                            "\n=====================================\n");
        TestDriver<char>::testCase9_propagate_on_container_move_assignment();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.  Because the generator is used for various types
        //   in higher numbered test cases, we need to test it on all test
        //   types.  This is achieved by the 'testCase8' class method of the
        //   test driver template, instantiated for the basic test type.
        //
        // Concerns:
        //: See 'TestDriver<CHAR_TYPE>::testCase8' for details.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase8' for details.
        //
        // Testing:
        //   Obj TestDriver::g(const char *spec);
        //   Obj TestDriver::g(size_t length, TYPE seed);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTION 'g'"
                            "\n==============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase8();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase8();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY AND MOVE CONSTRUCTORS
        //   Having now full confidence in 'operator==', we can use it
        //   to test that copy constructors preserve the notion of
        //   value.  This is achieved by the 'testCase7' class method of the
        //   test driver template, instantiated for the basic test type.  See
        //   that function for a list of concerns and a test plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase7' for details.
        //
        // Testing:
        //   basic_string(const basic_string& original);
        //   basic_string(const basic_string& original, basicAllocator);
        //   basic_string(MovableRef<basic_string> original);
        //   basic_string(MovableRef<basic_string> original, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY AND MOVE CONSTRUCTORS"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase7();
        TestDriver<char>::testCase7Move();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase7();
        TestDriver<wchar_t>::testCase7Move();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value.  This is achieved by the 'testCase6' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase6' for details.
        //
        // Testing:
        //   bool operator==(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator==(const C *, const string<C,CT,A>&);
        //   bool operator==(const string<C,CT,A>&, const C *);
        //   bool operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator!=(const C *, const string<C,CT,A>&);
        //   bool operator!=(const string<C,CT,A>&, const C *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISON OPERATORS"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase6();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase6();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Equality Operators"
                            "\n===================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase6Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase6Negative();
#endif

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OUTPUT (<<) OPERATOR"
                            "\n============================\n");

        if (verbose)
            printf("There is no output operator for this component.\n");

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Having implemented an effective generation mechanism, we now would
        //   like to test thoroughly the basic accessor functions
        //     - size() const
        //     - operator[](int) const
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.  This
        //   is achieved by the 'testCase4' class method of the test driver
        //   template, instantiated for the basic test type.  See that function
        //   for a list of concerns and a test plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase4' for details.
        //
        // Testing:
        //   size_type size() const;
        //   const_reference operator[](size_type pos) const;
        //   const_reference at(size_type pos) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase4();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase4();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //   This is achieved by the 'testCase3' class method of the test
        //   driver template, instantiated for the basic test type.  See that
        //   function for a list of concerns and a test plan.
        //
        // Concerns:
        //: See 'TestDriver<CHAR_TYPE>::testCase3' for details.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase3' for details.
        //
        // Testing:
        //   int TestDriver:ggg(Obj *object, const char *spec, int vF = 1);
        //   Obj& TestDriver:gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTIONS"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase3();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase3();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   We want to ensure that the primary manipulators
        //      - push_back             (black-box)
        //      - clear                 (white-box)
        //   operate as expected.  This is achieved by the 'testCase2' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase2' for details.
        //
        // Testing:
        //   basic_string(const ALLOC& a = ALLOC());
        //   ~basic_string();
        //   void push_back(CHAR_TYPE c);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS (BOOTSTRAP)"
                            "\n========================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase2();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase2();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //   We want to exercise basic value-semantic functionality.  This is
        //   achieved by the 'testCase1' class method of the test driver
        //   template, instantiated for a few basic test types.  See that
        //   function for a list of concerns and a test plan.  In addition, we
        //   want to make sure that we can use any standard-compliant
        //   allocator, including not necessarily rebound to the same type as
        //   the contained element, and that various manipulators and accessors
        //   work as expected in normal operation.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //: 2 That 'bsls::NameOf' can properly represent the 'bsl::string' as
        //:   "bsl::string".
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase1' for details.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nStandard value-semantic test.\n");

        if (verbose) printf("\n\t... with 'char' type.\n");
        TestDriver<char>::testCase1();

        if (verbose) printf("\n\t... with 'wchar_t' type.\n");
        TestDriver<wchar_t>::testCase1();

        if (verbose) printf("\nAdditional tests: allocators.\n");

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        bsl::allocator<int> zza(&testAllocator);
        bsl::basic_string<char,
                          bsl::char_traits<char>,
                          bsl::allocator<char> > zz1, zz2(zza);

        if (verbose) printf("\nAdditional tests: misc.\n");

        bsl::basic_string<char, bsl::char_traits<char> > myStr(5, 'a');
        bsl::basic_string<char, bsl::char_traits<char> >::const_iterator citer;
        ASSERT(5 == myStr.size());
        for (citer = myStr.begin(); citer != myStr.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myStr);

        myStr.insert(myStr.begin(), 'z');
        ASSERT(6 == myStr.size());
        ASSERT('z' == myStr[0]);
        for (citer = myStr.begin() + 1; citer != myStr.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myStr);

        myStr.erase(myStr.begin() + 2, myStr.begin() + 4);
        ASSERT(4 == myStr.size());
        ASSERT('z' == myStr[0]);
        for (citer = myStr.begin() + 1; citer != myStr.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myStr);

        if (verbose) printf("\nAdditional tests: traits.\n");

        ASSERT((bslmf::IsBitwiseMoveable<bsl::basic_string<char,
                                         bsl::char_traits<char> > >::value));
        ASSERT((bslmf::IsBitwiseMoveable<bsl::basic_string<wchar_t,
                                        bsl::char_traits<wchar_t> > >::value));

        if (verbose) printf("\nStreaming test.\n");

        std::ostringstream ostrm;
        myStr.assign("hello world");
        ostrm << myStr << '\0';
        ASSERT(ostrm.good());
        LOOP_ASSERT(ostrm.str().c_str(),
                    0 == strcmp(ostrm.str().c_str(), "hello world"));

        // can operator<< handle negative width?
        ostrm.str("");
        ostrm << std::setw(-10) << myStr;
        ASSERT(ostrm.str() == myStr);

        char inbuf[] = " goodbye world\n";
        std::istringstream istrm(inbuf);
        istrm >> myStr;
        ASSERT(istrm.good());
        LOOP_ASSERT(myStr.c_str(), "goodbye" == myStr);
        getline(istrm, myStr);
        ASSERT(! istrm.fail());
        LOOP_ASSERT(myStr.c_str(), " world" == myStr);
        getline(istrm, myStr);
        ASSERT(istrm.fail());
        myStr = "done";
        getline(istrm, myStr);
        LOOP_ASSERT(myStr.c_str(), "done" == myStr);

        // can operator>> handle negative width?
        istrm.str("setw");
        istrm.clear();
        istrm >> std::setw(-10) >> myStr;
        LOOP_ASSERT(myStr.c_str(), myStr == "setw");

        // Does 'bsls::NameOf' succeed in shortening the string name from the
        // 'bsl::basic_string<...' monstrosity?

        bsl::string strName = NameOf<bsl::string>().name();
        if ("unknown_type" != strName) {
            ASSERTV(strName, "bsl::string" == strName);
            ASSERTV(nameOfType(strName), strName,
                                               nameOfType(strName) == strName);

            strName = NameOf<std::string>();
            ASSERTV(strName, "std::string" == strName);
            ASSERTV(nameOfType(strName), strName,
                                               nameOfType(strName) != strName);
            ASSERT(nameOfType(std::string("woof")) == strName);
        }
#if !defined(BSLS_PLATFORM_CMP_SUN) || BSLS_PLATFORM_CMP_VERSION >= 20768
        else {
            ASSERT(0 && "'NameOf' should work everywhere but old Sun CC");
        }
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            ASSERT((""    == []() -> bsl::string { return {}; }()));
            ASSERT(("a"   == []() -> bsl::string { return {'a'}; }()));
            ASSERT(("aba" == []() -> bsl::string {
                return {'a', 'b', 'a'};
            }()));
        }
#endif

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   1) That performance does not regress between versions.
        //   2) That small "improvements" can be tested w.r.t. to performance,
        //      in a uniform benchmark.
        //
        // Plan:
        //   We follow a simple benchmark that copies strings into a table.  We
        //   create two tables, one containing long strings, and another
        //   containing short strings.  All strings are preallocated so that we
        //   do not measure the performance of the random generator.
        //   Specifically, we wish to measure the time taken by
        //     C1) The various constructors.
        //     C2) The copy constructor.
        //     A1) The copy assignment.
        //     A2) The 'assign' operations.
        //     M1) The 'append' operation in its various forms (including
        //         'push_back').
        //     M2) The 'insert' operation in its various forms.
        //     M3) The 'replace' operation in its various forms.
        //     S1) The 'swap' operation in its various forms.
        //     F1) The 'find' and 'rfind' operations.
        //     F2) The 'find_first_of' and 'find_last_of' operations.
        //     F3) The 'find_first_not_of' and 'find_last_not_of' operations.
        //   Also we wish to record the size of the various string
        //   instantiations.
        //
        //   Note: This is a *synthetic* benchmark.  It does not replace
        //   measuring real benchmarks (e.g., for strings, ADSP) whose
        //   conclusions may differ due to different memory allocation and
        //   access patterns, function call frequencies, etc.
        //
        // Testing:
        //   This "test" measures performance of basic operations, for
        //   performance regression.
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

        const int NITER = (argc < 3) ? 1 : atoi(argv[2]);

        if (verbose)
          printf("\tUsing %d repetitions of the tests (default 1).\n", NITER);

        const int RANDOM_SEED = (argc < 4) ? 0x12345678 : atoi(argv[3]);

        if (veryVerbose)
          printf("\tUsing %d random seed (of the tests (default 1).\n", NITER);

        srand(RANDOM_SEED);

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCaseM1(NITER, RANDOM_SEED);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    bslma::Default::setGlobalAllocator(originalGlobalAllocator);

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
