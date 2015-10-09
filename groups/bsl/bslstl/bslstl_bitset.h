// bslstl_bitset.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_BITSET
#define INCLUDED_BSLSTL_BITSET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant bitset class.
//
//@CLASSES:
//  bsl::bitset: STL-compatible bitset template
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_bitset.h>' instead and use 'bsl::bitset' directly.  This component
// implements a static bitset class that is suitable for use as an
// implementation of the 'std::bitset' class template.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Determining if a Number is Prime (Sieve of Eratosthenes)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to write a function to determine whether or not a given
// number is prime.  One way to implement this function is by using what's
// called the Sieve of Eratosthenes.  The basic idea of this algorithm is to
// repeatedly walk the sequence of integer values and mark any numbers up to
// and including the particular value of interest that are integer multiples of
// first 2, then 3, then 5, etc. (skipping 4 because it was previously marked
// when we walked the sequence by 2's).  Once we have walked the sequence
// with all primes up to and including the square root of the number of
// interest, we check to see if that number has been marked: If it has, it's
// composite; otherwise it's prime.
//
// When implementing this classic algorithm, we need an efficient way of
// representing a flag for each potential prime number.  The following
// illustrates how we can use 'bsl::bitset' to accomplish this result, provided
// we know an upper bound on supplied candidate values at compile time.
//
// First, we begin to define a function template that will determine whether or
// not a given candidate value is prime:
//..
//  template <unsigned int MAX_VALUE>
//  bool isPrime(int candidate)
//      // Return 'true' if the specified 'candidate' value is a prime number,
//      // and 'false' otherwise.  The behavior is undefined unless
//      // '2 <= candidate <= MAX_VALUE'
//  {
//      BSLMF_ASSERT(2 <= MAX_VALUE);
//      BSLS_ASSERT(2 <= candidate); BSLS_ASSERT(candidate <= MAX_VALUE);
//..
// Then, we declare a 'bsl::bitset', 'compositeFlags', that will contain flags
// indicating whether a value corresponding to a given index is known to be
// composite ('true') or is still potentially prime ('false') up to and
// including the compile-time constant template parameter, 'MAX_VALUE'.
//..
//      // Candidate primes in the '[2 .. MAX_VALUE]' range.
//
//      bsl::bitset<MAX_VALUE + 1> compositeFlags;
//..
// Next, we observe that a default-constructed 'bsl::bitset' has no flags set,
// We can check this by asserting that the 'none' method returns true, by
// asserting that the 'any' method returns false, or by asserting that the
// 'count' of set bits is 0:
//..
//      assert(true  == compositeFlags.none());
//      assert(false == compositeFlags.any());
//      assert(0     == compositeFlags.count());
//..
// Then, we note that a 'bsl::bitset' has a fixed 'size' (the set can't be
// grown or shrunk) and verify that 'size' is the same as the template
// argument used to create the 'bsl::bitset':
//..
//      assert(MAX_VALUE + 1 == compositeFlags.size());
//..
// Next, we compute 'sqrt(candidate)', which is as far as we need to look:
//..
//      // We need to cast the 'sqrt' argument to avoid an overload ambiguity.
//      const int sqrtOfCandidate = std::sqrt(static_cast<double>(candidate))
//                                + 0.01;  // fudge factor
//..
// Now, we loop from 2 to 'sqrtOfCandidate', and use the sieve algorithm to
// eliminate non-primes:
//..
//      // Note that we treat 'false' values as potential primes,
//      // since that is how 'bsl::bitset' is default-initialized.
//
//      for (std::size_t i = 2; i <= sqrtOfCandidate; ++i) {
//          if (compositeFlags[i]) {
//              continue; // Skip this value: it's flagged as composite, so all
//                        // of its multiples are already flagged as composite
//                        // as well.
//          }
//
//          for (std::size_t flagValue = i;
//               flagValue <= candidate;
//               flagValue += i) {
//              compositeFlags[flagValue] = true;
//          }
//
//          if (true == compositeFlags[candidate]) {
//              return false;                                         // RETURN
//          }
//      }
//
//      BSLS_ASSERT(false == compositeFlags[candidate]);
//
//      return true;
//  }
//..
// Notice that if we don't return 'false' from the loop, none of the lower
// numbers evenly divided the candidate value; hence, it is a prime number.
//
// Finally, we can exercise our 'isPrime' function with an upper bound of
// 10,000:
//..
//  enum { UPPER_BOUND = 10000 };
//
//  assert(1 == isPrime<UPPER_BOUND>(2));
//  assert(1 == isPrime<UPPER_BOUND>(3));
//  assert(0 == isPrime<UPPER_BOUND>(4));
//  assert(1 == isPrime<UPPER_BOUND>(5));
//  assert(0 == isPrime<UPPER_BOUND>(6));
//  assert(1 == isPrime<UPPER_BOUND>(7));
//  assert(0 == isPrime<UPPER_BOUND>(8));
//  assert(0 == isPrime<UPPER_BOUND>(9));
//  assert(0 == isPrime<UPPER_BOUND>(10));
//  // ...
//  assert(1 == isPrime<UPPER_BOUND>(9973));
//  assert(0 == isPrime<UPPER_BOUND>(9975));
//  assert(0 == isPrime<UPPER_BOUND>(10000));
//..


// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_bitset.h> instead of <bslstl_bitset.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>    // 'min'
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_STDEXCEPT
#include <stdexcept>
#define INCLUDED_STDEXCEPT
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#endif

namespace bsl {

template <std::size_t N>
class bitset;

                        // =================
                        // class bsl::bitset
                        // =================

template <std::size_t N>
class bitset {
    // This class template provides an STL-compliant 'bitset'.  For the
    // requirements of a 'bitset' class, consult the second revision of the
    // ISO/IEC 14882 Programming Language c++ (2003).
    //
    // In addition to the methods defined in the standard, this class also
    // provides an extra constructor that takes a 'bsl::basic_string'.  This
    // extra constructor provides the capability to construct a 'bitset' from a
    // 'bsl::basic_string', in addition to a 'std::basic_string'.

    // PRIVATE TYPES
    enum {
        BYTESPERINT = sizeof(int),
        BITSPERINT  = 8 * sizeof(int),
        BITSETSIZE  = N ? (N - 1) / BITSPERINT + 1 : 1
    };

    // DATA
    unsigned int d_data[BITSETSIZE];  // storage for bitset, d_data[0] holds
                                      // the least significant bit.

    // FRIENDS
    friend class reference;

  public:
    // PUBLIC TYPES
    class reference {
        // This class represents a reference to a modifiable bit inside a
        // 'bsl::bitset'.

        // FRIENDS
        friend class bitset;

        // DATA
        unsigned int *d_int_p;   // pointer to the int inside the bitset.
        unsigned int  d_offset;  // bit offset to 'd_int'.

        // PRIVATE CREATORS
        reference(unsigned int *i, unsigned int offset);

      public:
        // MANIPULATORS
        reference& operator=(bool x);

        reference& operator=(const reference& x);

        reference& flip();

        // ACCESSORS
        operator bool() const;

        bool operator~() const;
    };

  private:
    // PRIVATE MANIPULATORS
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    void copyString(const native_std::basic_string<CHAR_TYPE,
                                                   TRAITS,
                                                   ALLOCATOR>&           str,
                    typename native_std::basic_string<CHAR_TYPE,
                                                   TRAITS,
                                                   ALLOCATOR>::size_type pos,
                    typename native_std::basic_string<CHAR_TYPE,
                                                   TRAITS,
                                                   ALLOCATOR>::size_type n);

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    void copyString(const     bsl::basic_string<CHAR_TYPE,
                                                   TRAITS,
                                                   ALLOCATOR>&           str,
                    typename  bsl::basic_string<CHAR_TYPE,
                                                   TRAITS,
                                                   ALLOCATOR>::size_type pos,
                    typename  bsl::basic_string<CHAR_TYPE,
                                                   TRAITS,
                                                   ALLOCATOR>::size_type n);

    void clearUnusedBits();
        // Clear the bits unused by the bitset in 'd_data', namely, bits
        // 'BITSETSIZE * BITSPERINT - 1' to 'N' (where the bit count starts at
        // 0).

    void clearUnusedBits(bsl::false_type);
    void clearUnusedBits(bsl::true_type);
        // Implementations of 'clearUnusedBits', overloaded by whether there
        // are any unused bits.

    // PRIVATE ACCESSORS
    std::size_t numOneSet(unsigned int src) const;
        // Return the number of 1 bits in the specified 'src'.

  public:
    // CREATORS
    bitset();
        // Create a bitset with all bits initialized to 0.

    bitset(unsigned long val);
        // Create a bitset with its first 'M' bit positions correspond to bit
        // values in the specified 'val'.  'M' is the smaller of the
        // parameterized 'N' and '8 * sizeof(unsigned long)'.  If 'M < N', the
        // remaining bit positions are initialized to 0.

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    explicit
    bitset(const native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
           typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                      size_type pos = 0);
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    bitset(const native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
           typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                      size_type pos,
           typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                      size_type n);
        // Create a bitset with its first 'M' bit positions correspond to the
        // characters in the specified 'pos' of the specified 'str'.  'M' is
        // the smaller of the parameterized 'N' and 'str.length()'.  If
        // 'M < N', the remaining bit positions are initialized to 0.  The
        // behavior is undefined if the characters in the specified 'str' is
        // not '0' and not '1'.

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    explicit
    bitset(const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
           typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                      size_type pos = 0);
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    bitset(const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
           typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                      size_type pos,
           typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                      size_type n);
        // Create a bitset with its first 'M' bit positions correspond to the
        // characters in the specified 'pos' of the specified 'str'.  'M' is
        // the smaller of the parameterized 'N' and 'str.length()'.  If
        // 'M < N', the remaining bit positions are initialized to 0.  The
        // behavior is undefined if the characters in the specified 'str' is
        // not 0 and not 1.

    // MANIPULATORS
    bitset& operator&=(const bitset& rhs);
        // Clear each bit of this bitset for each corresponding bit that is 0
        // in the specified 'rhs', and leaves all other bits unchanged.  Return
        // a reference to this modifiable bitset.  Note that this is equivalent
        // to a bitwise OR.

    bitset& operator|=(const bitset& rhs);
        // Set each bit of this bitset for each corresponding bit that is 1 in
        // the specified 'rhs', and leaves all other bits unchanged.  Return a
        // reference to this modifiable bitset.  Note that this is equivalent
        // to a bitwise AND.

    bitset& operator^=(const bitset& rhs);
        // Toggle each bit of this bitset for each corresponding bit that is 1
        // in the specified 'rhs', and leaves all other bits unchanged.  Return
        // a reference to this modifiable bitset.  Note that this is equivalent
        // to a bitwise XOR.

    bitset& operator<<=(std::size_t pos);
        // Shift the bits of this bitset left (towards the most significant
        // bit) by the specified 'pos' and return a reference to this
        // modifiable bitset.  For all bits with position I where 'I <= pos',
        // the new value is 0.  The behavior is undefined unless 'pos <= N'.

    bitset& operator>>=(std::size_t pos);
        // Shift the bits of this bitset right (towards the least significant
        // bit) by the specified 'pos' and return a reference to this
        // modifiable bitset.  For all bits with position I where
        // 'I > N - pos', the new value is 0.  The behavior is undefined unless
        // 'pos <= N'.

    bitset& flip();
        // Toggle all bits of this bitset and return a reference to this
        // modifiable bitset.

    bitset& flip(std::size_t pos);
        // Toggle the bit at the specified 'pos' of this bitset and return a
        // reference to this modifiable bitset.

    bitset& reset();
        // Set all bits of this bitset to 0 and return a reference to this
        // modifiable bitset.

    bitset& reset(std::size_t pos);
        // Set the bit at the specified 'pos' of this bitset to 0 and return a
        // reference to this modifiable bitset.

    bitset& set();
        // Set all bits of this bitset to 1 and return a reference to this
        // modifiable bitset.

    bitset& set(std::size_t pos, int val = true);
        // Set the bit at the specified 'pos' of this bitset to 1 and return a
        // reference to this modifiable bitset.  Optionally specify 'val' as
        // the value to set the bit.  If 'val' is non-zero, the bit is set to
        // 1, otherwise the bit is set to 0.

    reference operator[](std::size_t pos);
        // Return a 'reference' to the modifiable bit position at the specified
        // 'pos'.

    // ACCESSORS
    bitset operator<<(std::size_t pos) const;
        // Return a bitset constructed from shifting this bitset left by the
        // specified 'pos'.

    bitset operator>>(std::size_t pos) const;
        // Return a bitset constructed from shifting this bitset right by the
        // specified 'pos'.

    bitset operator~() const;
        // Toggle all bits of this bitset and return a reference to this
        // modifiable bitset.

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> to_string() const;
        // Return a 'basic_string' representation of this bitset, where bit
        // value 1 is represented with '1' and bit value 0 is represented with
        // '0'.  The most-significant bit is placed at the beginning of the
        // string, and the least-significant bit is placed at the end of the
        // string.

    bool operator[](std::size_t pos) const;
        // Return the value of the bit position at the specified 'pos'.

    bool operator==(const bitset& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // bitset and 'false' otherwise.  Two bitsets have the same value when
        // the sequence and value of bits they hold are the same.

    bool operator!=(const bitset& rhs) const;
        // Return 'true' if the specified 'rhs' do not have the same value as
        // this bitset and 'false' otherwise.  Two bitset do not have the same
        // value when either the sequence or the value of bits they hold are
        // not the same.

    bool any() const;
        // Return 'true' if any of the bits in this bitset has the value of 1
        // and 'false' otherwise.

    bool none() const;
        // Return 'true' if all the bits in this bitset has the value of 0 and
        // 'false' otherwise.

    std::size_t size() const;
        // Return the number of bits this bitset holds.

    std::size_t count() const;
        // Return the number of bits in this bitset that have the value of 1.

    bool test(size_t pos) const;
        // Return 'true' if the bit at the specified 'pos' has the value of 1
        // and 'false' otherwise.

    unsigned long to_ulong() const;
        // Return an 'unsigned' 'long' value that has the same bit value as the
        // bitset.  Note that the behavior is undefined if the bitset cannot be
        // represented as an 'unsigned' 'long'.
};

// FREE OPERATORS
template <std::size_t N>
bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs);
    // Return a 'bitset' that results from a bitwise AND of the specified 'lhs'
    // and 'rhs'.

template <std::size_t N>
bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs);
    // Return a 'bitset' that results from a bitwise OR of the specified 'lhs'
    // and 'rhs'.

template <std::size_t N>
bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs);
    // Return a 'bitset' that results from a bitwise XOR of the specified 'lhs'
    // and 'rhs'.

template <class CHAR_TYPE, class TRAITS, std::size_t N>
std::basic_istream<CHAR_TYPE, TRAITS>&
operator>>(std::basic_istream<CHAR_TYPE, TRAITS>& is, bitset<N>& x);

template <class CHAR_TYPE, class TRAITS, std::size_t N>
std::basic_ostream<CHAR_TYPE, TRAITS>&
operator<<(std::basic_ostream<CHAR_TYPE, TRAITS>& os, const bitset<N>& x);

// ============================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class bitset::reference
                        // -----------------------

// PRIVATE CREATORS
template <std::size_t N>
inline
bitset<N>::reference::reference(unsigned int *i, unsigned int offset)
: d_int_p(i)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(d_int_p);
}

// MANIPULATORS
template <std::size_t N>
inline
typename bitset<N>::reference&
bitset<N>::reference::operator=(bool x)
{
    if (x) {
        *d_int_p |= (1 << d_offset);
    }
    else {
        *d_int_p &= ~(1 << d_offset);
    }
    return *this;
}

template <std::size_t N>
inline
typename bitset<N>::reference&
bitset<N>::reference::operator=(const reference& x)
{
    if (x) {
        *d_int_p |= (1 << d_offset);
    }
    else {
        *d_int_p &= ~(1 << d_offset);
    }
    return *this;
}

template <std::size_t N>
inline
typename bitset<N>::reference&
bitset<N>::reference::flip()
{
    *d_int_p ^= (1 << d_offset);
    return *this;
}

// ACCESSORS
template <std::size_t N>
inline
bitset<N>::reference::operator bool() const
{
    return ((*d_int_p & (1 << d_offset)) != 0);
}

template <std::size_t N>
inline
bool bitset<N>::reference::operator~() const
{
    return ((*d_int_p & (1 << d_offset)) == 0);
}

                        // ------------
                        // class bitset
                        // ------------

// PRIVATE MANIPULATORS
template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
void bitset<N>::copyString(const native_std::basic_string<CHAR_TYPE,
                                               TRAITS,
                                               ALLOCATOR>&           str,
                typename native_std::basic_string<CHAR_TYPE,
                                               TRAITS,
                                               ALLOCATOR>::size_type pos,
                typename native_std::basic_string<CHAR_TYPE,
                                               TRAITS,
                                               ALLOCATOR>::size_type n)
{
    typedef typename native_std::basic_string<CHAR_TYPE,
                                               TRAITS,
                                               ALLOCATOR>::size_type size_type;
    n = std::min(N, std::min(n, str.size() - pos));
    for (size_type i = 0; i < n; ++i) {
        typename TRAITS::int_type bit = TRAITS::to_int_type(
                                                        str[pos + n  - i - 1]);

        if (bit == '1') {
            set(i);
        }
        else if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(bit != '0')) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument(
                                             "string for bitset constructor "
                                             "must be '0' or '1'");
        }
    }
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
void bitset<N>::copyString(const bsl::basic_string<CHAR_TYPE,
                                               TRAITS,
                                               ALLOCATOR>&           str,
                typename bsl::basic_string<CHAR_TYPE,
                                              TRAITS,
                                              ALLOCATOR>::size_type pos,
                typename bsl::basic_string<CHAR_TYPE,
                                              TRAITS,
                                              ALLOCATOR>::size_type n)
{
    typedef typename bsl::basic_string<CHAR_TYPE,
                                          TRAITS,
                                          ALLOCATOR>::size_type size_type;
    n = std::min(N, std::min(n, str.size() - pos));
    for (size_type i = 0; i < n; ++i) {
        typename TRAITS::int_type bit = TRAITS::to_int_type(
                                                        str[pos + n  - i - 1]);

        if (bit == '1') {
            set(i);
        }
        else if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(bit != '0')) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument(
                                             "string for bitset constructor "
                                             "must be '0' or '1'");
        }
    }
}

template <std::size_t N>
inline
void bitset<N>::clearUnusedBits()
{
    enum { VALUE = N % BITSPERINT ? 1 : 0 };

    clearUnusedBits(bsl::integral_constant<bool, VALUE>());
}

template <std::size_t N>
inline
void bitset<N>::clearUnusedBits(bsl::false_type)
{
}

template <std::size_t N>
inline
void bitset<N>::clearUnusedBits(bsl::true_type)
{
    const unsigned int offset = N % BITSPERINT;  // never 0

    d_data[BITSETSIZE - 1] &= ~(~((unsigned int)0) << offset);
}

template <std::size_t N>
std::size_t bitset<N>::numOneSet(unsigned int src) const
{
    // The following code was taken from 'bdes_bitutil'.
    unsigned input = src;

    // First we use a tricky way of getting every 2-bit half-nibble to
    // represent the number of bits that were set in those two bits.

    input -= (input >> 1) & 0x55555555;

    // Henceforth, we just accumulate the sum down into lower and lower bits.

    {
        const int mask = 0x33333333;
        input = ((input >> 2) & mask) + (input & mask);
    }

    // Any 4-bit nibble is now guaranteed to be less than or equal to 8, so we
    // do not have to mask both sides of the addition.  We must mask after the
    // addition, so 8-bit bytes are the sum of bits in those 8 bits.

    input = ((input >> 4) + input) & 0x0f0f0f0f;

    // It is no longer necessary to mask the additions, because it is
    // impossible for any bit groups to add up to more than 256 and carry, thus
    // interfering with adjacent groups.  Each 8-bit byte is independent from
    // now on.

    input = (input >>  8) + input;
    input = (input >> 16) + input;

    return input & 0x000000ff;
}

// CREATORS
template <std::size_t N>
inline
bitset<N>::bitset()
{
    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);
}

template <std::size_t N>
bitset<N>::bitset(unsigned long val)
{
    enum {
        BSLSTL_INTS_IN_LONG = sizeof(unsigned long) / sizeof(int)
    };

    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);

    if (1 == BSLSTL_INTS_IN_LONG) {
        d_data[0] = static_cast<unsigned int>(val);
    }
    else {
        const unsigned int numInts = (unsigned int) BSLSTL_INTS_IN_LONG
                                                    < (unsigned int) BITSETSIZE
                                     ? (unsigned int) BSLSTL_INTS_IN_LONG
                                     : (unsigned int) BITSETSIZE;

        for (unsigned int i = 0; i < numInts; ++i) {
            d_data[i] = static_cast<unsigned int>(val >> (BITSPERINT * i));
        }
    }
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const    native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
       typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                              size_type pos)
{
    typedef native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> StringType;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > str.size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                               "'pos > str.size()' for bitset "
                                               "constructor");
    }
    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);
    copyString(str, pos, StringType::npos);
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const    native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
       typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                              size_type pos,
       typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                                size_type n)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > str.size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                               "'pos > str.size()' for bitset "
                                               "constructor");
    }
    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);
    copyString(str, pos, n);
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const    bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                                 size_type pos)
{
    typedef bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> StringType;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > str.size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                               "'pos > str.size()' for bitset "
                                               "constructor");
    }
    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);
    copyString(str, pos, StringType::npos);
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const    bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                                 size_type pos,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                                   size_type n)

{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > str.size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                               "'pos > str.size()' for bitset "
                                               "constructor");
    }
    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);
    copyString(str, pos, n);
}

// MANIPULATORS
template <std::size_t N>
bitset<N>& bitset<N>::operator&=(const bitset<N>& rhs)
{
    for (std::size_t i = 0; i < BITSETSIZE; ++i) {
        d_data[i] &= rhs.d_data[i];
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator|=(const bitset<N>& rhs)
{
    for (std::size_t i = 0; i < BITSETSIZE; ++i) {
        d_data[i] |= rhs.d_data[i];
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator^=(const bitset<N>& rhs)
{
    for (std::size_t i = 0; i < BITSETSIZE; ++i) {
        d_data[i] ^= rhs.d_data[i];
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator<<=(std::size_t pos)
{
    BSLS_ASSERT_SAFE(pos <= N);

    if (pos) {
        const std::size_t shift  = pos / BITSPERINT;
        const std::size_t offset = pos % BITSPERINT;

        if (shift) {
            std::memmove(d_data + shift,
                         d_data,
                         (BITSETSIZE - shift) * BYTESPERINT);
            std::memset(d_data, 0, shift * BYTESPERINT);
        }

        if (offset) {
            for (std::size_t i = BITSETSIZE - 1; i > shift; --i) {
                d_data[i] = (d_data[i] << offset) |
                                        (d_data[i-1] >> (BITSPERINT - offset));
            }
            d_data[shift] <<= offset;
        }

        clearUnusedBits();
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator>>=(std::size_t pos)
{
    BSLS_ASSERT_SAFE(pos <= N);

    if (pos) {
        const std::size_t shift  = pos / BITSPERINT;
        const std::size_t offset = pos % BITSPERINT;

        if (shift) {
            std::memmove(d_data,
                         d_data + shift,
                         (BITSETSIZE - shift) * BYTESPERINT);
            std::memset(d_data + BITSETSIZE - shift, 0, shift * BYTESPERINT);
        }

        if (offset) {
            for (std::size_t i = 0; i < BITSETSIZE - shift - 1; ++i) {
                d_data[i] = (d_data[i] >> offset) |
                                        (d_data[i+1] << (BITSPERINT - offset));
            }
            d_data[BITSETSIZE - shift - 1] >>= offset;
        }

        clearUnusedBits();
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::flip()
{
    for (std::size_t i = 0; i < BITSETSIZE; ++i) {
        d_data[i] = ~d_data[i];
    }
    clearUnusedBits();
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::flip(std::size_t pos)
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / BITSPERINT;
    const std::size_t offset = pos % BITSPERINT;
    d_data[shift] ^= (1 << offset);
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::reset()
{
    std::memset(d_data, 0, BITSETSIZE * BYTESPERINT);
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::reset(std::size_t pos)
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / BITSPERINT;
    const std::size_t offset = pos % BITSPERINT;
    d_data[shift] &= ~(1 << offset);
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::set()
{
    std::memset(d_data, 0xFF, BITSETSIZE * BYTESPERINT);
    clearUnusedBits();
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::set(std::size_t pos, int val)
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / BITSPERINT;
    const std::size_t offset = pos % BITSPERINT;
    if (val) {
        d_data[shift] |= (1 << offset);
    }
    else {
        d_data[shift] &= ~(1 << offset);
    }
    return *this;
}

template <std::size_t N>
inline
typename bitset<N>::reference bitset<N>::operator[](std::size_t pos)
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / BITSPERINT;
    const std::size_t offset = pos % BITSPERINT;
    return typename bitset<N>::reference(&d_data[shift],
                                         static_cast<unsigned int>(offset));
}

// ACCESSORS
template <std::size_t N>
inline
bitset<N> bitset<N>::operator<<(std::size_t pos) const
{
    BSLS_ASSERT_SAFE(pos <= N);

    bitset<N> tmp(*this);
    return tmp <<= pos;
}

template <std::size_t N>
inline
bitset<N> bitset<N>::operator>>(std::size_t pos) const
{
    BSLS_ASSERT_SAFE(pos <= N);

    bitset<N> tmp(*this);
    return tmp >>= pos;
}

template <std::size_t N>
inline
bitset<N> bitset<N>::operator~() const
{
    bitset<N> tmp(*this);
    return tmp.flip();
}

template <std::size_t N>
inline
bool bitset<N>::operator[](std::size_t pos) const
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / BITSPERINT;
    const std::size_t offset = pos % BITSPERINT;
    return ((d_data[shift] & (1 << offset)) != 0);
}

template <std::size_t N>
inline
bool bitset<N>::operator==(const bitset& rhs) const
{
    return std::memcmp(d_data, rhs.d_data, BITSETSIZE * BYTESPERINT) == 0;
}

template <std::size_t N>
inline
bool bitset<N>::operator!=(const bitset& rhs) const
{
    return !operator==(rhs);
}

template <std::size_t N>
bool bitset<N>::any() const
{
    for (std::size_t i = 0; i < BITSETSIZE; ++i) {
        if (d_data[i] != 0) {
            return true;                                              // RETURN
        }
    }
    return false;
}

template <std::size_t N>
inline
bool bitset<N>::none() const
{
    return !any();
}

template <std::size_t N>
inline
std::size_t bitset<N>::size() const
{
    return N;
}

template <std::size_t N>
std::size_t bitset<N>::count() const
{
    std::size_t sum = 0;
    for (std::size_t i = 0; i < BITSETSIZE; ++i) {
        sum += numOneSet(d_data[i]);
    }
    return sum;
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> bitset<N>::to_string() const
{
    basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> str(N,
                                                   TRAITS::to_char_type('0'));
    for (std::size_t i = 0; i < N; ++i) {
        if (this->operator[](i)) {
            str[N - i - 1] = TRAITS::to_char_type('1');
        }
    }
    return str;
}

template <std::size_t N>
inline
bool bitset<N>::test(size_t pos) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos >= N)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                        "out_of_range in bsl::bitset<>::test");
    }
    return operator[](pos);
}

template <std::size_t N>
unsigned long bitset<N>::to_ulong() const
{
    enum {
        BSLSTL_INTS_IN_LONG = sizeof(unsigned long) / sizeof(int)
    };

    for (std::size_t i = BSLSTL_INTS_IN_LONG; i < BITSETSIZE; ++i) {
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_data[i])) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwOverflowError(
                                        "overflow in bsl::bitset<>::to_ulong");
        }
    }

    unsigned long      value   = 0;
    const unsigned int numInts = (unsigned int) BSLSTL_INTS_IN_LONG
                                                    < (unsigned int) BITSETSIZE
                               ? (unsigned int) BSLSTL_INTS_IN_LONG
                               : (unsigned int) BITSETSIZE;

    for (unsigned int i = 0; i < numInts; ++i) {
        value |= (unsigned long) d_data[i] << (BITSPERINT * i);
    }
    return value;
}

// FREE OPERATORS
template <std::size_t N>
bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs)
{
    bitset<N> tmp(lhs);
    return tmp &= rhs;
}

template <std::size_t N>
bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs)
{
    bitset<N> tmp(lhs);
    return tmp |= rhs;
}

template <std::size_t N>
bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs)
{
    bitset<N> tmp(lhs);
    return tmp ^= rhs;
}

template <class CHAR_TYPE, class TRAITS, std::size_t N>
std::basic_istream<CHAR_TYPE, TRAITS>&
operator>>(std::basic_istream<CHAR_TYPE, TRAITS>& is, bitset<N>& x)
{
    typedef typename TRAITS::int_type int_type;

    basic_string<CHAR_TYPE, TRAITS, allocator<CHAR_TYPE> > tmp;
    tmp.reserve(N);

    typename std::basic_istream<CHAR_TYPE, TRAITS>::sentry sen(is);
    if (sen) {
        std::basic_streambuf<CHAR_TYPE, TRAITS> *buffer = is.rdbuf();
        for (std::size_t i = 0; i < N; ++i) {
            static int_type eof = TRAITS::eof();
            int_type cint = buffer->sbumpc();
            if (TRAITS::eq_int_type(cint, eof)) {
                is.setstate(std::ios_base::eofbit);
                break;
            }
            else {
                CHAR_TYPE cchar = TRAITS::to_char_type(cint);
                char      c     = is.narrow(cchar, '*');

                if (c == '0' || c == '1') {
                    tmp.push_back(c);
                }
                else if (TRAITS::eq_int_type(buffer->sputbackc(cchar), eof)) {
                    is.setstate(std::ios_base::failbit);
                    break;
                }
            }
        }

        if (tmp.empty()) {
            is.setstate(std::ios_base::failbit);
        }
        else {
            x = bitset<N>(tmp);
        }
    }
    return is;
}

template <class CHAR_TYPE, class TRAITS, std::size_t N>
inline
std::basic_ostream<CHAR_TYPE, TRAITS>&
operator<<(std::basic_ostream<CHAR_TYPE, TRAITS>& os, const bitset<N>& x)
{
    basic_string<CHAR_TYPE, TRAITS, allocator<CHAR_TYPE> > tmp (
             x.template to_string<CHAR_TYPE, TRAITS, allocator<CHAR_TYPE> >());
    return os << tmp;
}

}  // close namespace bsl

#endif

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
