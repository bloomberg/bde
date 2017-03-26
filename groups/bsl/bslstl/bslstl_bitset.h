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
// when we walked the sequence by 2's).  Once we have walked the sequence with
// all primes up to and including the square root of the number of interest, we
// check to see if that number has been marked: If it has, it's composite;
// otherwise it's prime.
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
// grown or shrunk) and verify that 'size' is the same as the template argument
// used to create the 'bsl::bitset':
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

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#endif

#ifndef INCLUDED_BSLMA_STDALLOCATOR
#include <bslma_stdallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_CPP11
#include <bsls_cpp11.h>
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

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_LIMITS_H
#include <limits.h>
#define INCLUDED_LIMITS_H
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) ||                                        \
   (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 40400)
    // Last tested against MSVC 2013.  The Microsoft compiler cannot parse the
    // nested typenames for function parameters with default arguments, where
    // the function parameter type is a dependent type within a template, such
    // as 'typename std::basic_string<C,T,A>::size_type'.  The error message
    // complains about invalid identiefiers on the right of the '::', and this
    // feature macro is named accordingly.  Older version of g++ also have this
    // problem.
# define BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS
#endif

namespace bsl {

template <std::size_t N>
class bitset;

                          // ====================
                          // class Bitset_ImpUtil
                          // ====================
struct Bitset_ImpUtil {
    enum {
        k_BYTES_PER_INT = sizeof(int),
        k_BITS_PER_INT  = CHAR_BIT * sizeof(int),
        k_INTS_IN_LONG  = sizeof(long) / sizeof(int)
    };

    static void defaultInit(unsigned int  *data,
                            size_t         size,
                            unsigned long  val = 0);
        // Initialize the memory at the address specified by 'data' so that the
        // the first 'M' bit positions correspond to the bit values in the
        // specified 'val' where 'M' is the smaller of 'size * k_BITS_PER_INT'
        // and 'CHAR_BIT * sizeof(unsigned long)'.  The remaining bits are
        // initialized to zero '0'.
};

                          // ====================
                          // class Bitset_ImpBase
                          // ====================

template <std::size_t BITSETSIZE,
          std::size_t NUM_INIT =  (BITSETSIZE < sizeof(long) / sizeof(int))
                                 ? BITSETSIZE : sizeof(long) / sizeof(int)>
class Bitset_ImpBase;
    // This component private 'class' template describes the basic data and
    // initialization semantics needed in order to implement a C++11 'bitset'
    // class.  The 'BITSETSIZE' template parameter specifies the size of the
    // underlying data array, 'd_data'.  The 'NUM_INIT' template parameter
    // specifies the number of elements in 'd_data' needed to use when storing
    // a value of type 'unsigned long'.  Partial class template specializations
    // of 'Bitset_ImpBase' are provided for 'NUM_INIT == 1' and
    // 'NUM_INIT == 2'.  No other values of 'NUM_INIT' are supported.

#if __cplusplus >= 201103L
template <std::size_t BITSETSIZE, std::size_t NUM_INIT>
class Bitset_ImpBase {
  private:
    static_assert(NUM_INIT != NUM_INIT,
                  "bsl::bitset is not supported on this target because "
                  "sizeof(long) / sizeof(int) is neither 1 nor 2.");
};
#endif


template <std::size_t BITSETSIZE>
class Bitset_ImpBase<BITSETSIZE, 1> {
  public:
    // PUBLIC DATA
    unsigned int d_data[BITSETSIZE];

    // CREATORS
    BSLS_CPP11_CONSTEXPR Bitset_ImpBase();
        // Create a 'Bitset_ImpBase' with each bit in 'd_data' initialized to
        // zero.  In C++11 this constructor can be used in a constant
        // expression.

    BSLS_CPP11_CONSTEXPR Bitset_ImpBase(unsigned long val);
        // Create a 'Bitset_ImpBase' with the first 'N' bit positions of
        // 'd_data' corresponding to the first 'N' bit positions of the
        // specified 'val' after conversion to 'unsigned int' and the remaining
        // bits in 'd_data' initialized to zero, where 'N' is
        // 'CHAR_BIT * sizeof(int)'.  The behavior is undefined unless
        // 'BITSETSIZE == 1' or 'sizeof(long) / sizeof(int) == 1'.  In C++11
        // this constructor can be used in a constant expression.
};

template <std::size_t BITSETSIZE>
class Bitset_ImpBase<BITSETSIZE, 2> {
  public:
    // PUBLIC DATA
    unsigned int d_data[BITSETSIZE];

    // CREATORS
    BSLS_CPP11_CONSTEXPR Bitset_ImpBase();
        // Create a Bitset_ImpBase with each bit in 'd_data' initialized to
        // zero.  In C++11 this constructor can be used in a constant
        // expression.

    BSLS_CPP11_CONSTEXPR Bitset_ImpBase(unsigned long);
        // Create a 'Bitset_ImpBase' with the first 'N' bit positions of
        // 'd_data' corresponding to the first 'N' bit positions of the
        // specified 'val' after conversion to 'unsigned int' and the remaining
        // bits in 'd_data' initialized to zero, where 'N' is
        // 'CHAR_BIT * sizeof(int) * 2'.  The behavior is undefined unless
        // 'sizeof(long) / sizeof(int) == 2'.  In C++11 this constructor can be
        // used in a constant expression.
};

                        // =================
                        // class bsl::bitset
                        // =================

template <std::size_t N>
class bitset :
       private Bitset_ImpBase<N ? (N - 1) / (CHAR_BIT * sizeof(int)) + 1 : 1> {
    // This class template provides an STL-compliant 'bitset'.  For the
    // requirements of a 'bitset' class, consult the second revision of the
    // ISO/IEC 14882 Programming Language c++ (2011).
    //
    // In addition to the methods defined in the standard, this class also
    // provides an extra constructor that takes a 'bsl::basic_string'.  This
    // extra constructor provides the capability to construct a 'bitset' from a
    // 'bsl::basic_string', in addition to a 'std::basic_string'.

    // PRIVATE TYPES
    enum {
        k_BYTES_PER_INT = sizeof(int),
        k_BITS_PER_INT  = CHAR_BIT * k_BYTES_PER_INT,
        k_BITSETSIZE    = N ? (N - 1) / k_BITS_PER_INT + 1 : 1
    };

    // 'static_cast' is needed here to avoid warning with '-Wextra' and 'gcc'.
    typedef Bitset_ImpBase<static_cast<std::size_t>(k_BITSETSIZE)> Base;

    using Base::d_data;

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
        unsigned int *d_int_p;   // pointer to the 'int' inside the 'bitset'.
        unsigned int  d_offset;  // bit offset to 'd_int'.

        // PRIVATE CREATORS
        reference(unsigned int *i, unsigned int offset);
            // Create a 'reference' object that refers to the bit at the
            // specified 'offset' of the 'int' pointed to by the specified 'i'.
            // The behavior is undefined unless 'i' points to an 'int' inside a
            // 'bsl::bitset'.

      public:
        // MANIPULATORS
        reference& operator=(bool x) BSLS_CPP11_NOEXCEPT;
            // Assign to the bit referenced by this object the specified value
            // 'x' and return a reference offering modifiable access to this
            // object.

        reference& operator=(const reference& x) BSLS_CPP11_NOEXCEPT;
            // Assign this object to refer to the same bit as the specified 'x'
            // and return a reference offering modifiable access to this
            // object.

        reference& flip() BSLS_CPP11_NOEXCEPT;
            // Invert the value of the bit referenced by this object and return
            // a reference offering modifiable access to this object.

        // ACCESSORS
        operator bool() const BSLS_CPP11_NOEXCEPT;
            //  Return the value of the bit referenced by this object.

        bool operator~() const BSLS_CPP11_NOEXCEPT;
            // Return the inverted value of the bit referenced by this object.
            // Note that the value of the referenced bit remains unchanged.
    };

  private:
    // PRIVATE MANIPULATORS
    void clearUnusedBits();
        // Clear the bits unused by the bitset in 'd_data', namely, bits
        // 'k_BITSETSIZE * k_BITS_PER_INT - 1' to 'N' (where the bit count
        // starts at 0).

    void clearUnusedBits(bsl::false_type);
    void clearUnusedBits(bsl::true_type);
        // Implementations of 'clearUnusedBits', overloaded by whether there
        // are any unused bits.

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    void copyString(const   native_std::basic_string<CHAR_TYPE,
                                                     TRAITS,
                                                     ALLOCATOR>&           str,
                   typename native_std::basic_string<CHAR_TYPE,
                                                     TRAITS,
                                                     ALLOCATOR>::size_type pos,
                   typename native_std::basic_string<CHAR_TYPE,
                                                     TRAITS,
                                                     ALLOCATOR>::size_type n,
                    CHAR_TYPE zeroChar,
                    CHAR_TYPE oneChar);
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    void copyString(
      const     bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&           str,
      typename  bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type pos,
      typename  bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type n,
      CHAR_TYPE zeroChar,
      CHAR_TYPE oneChar);
        // Assign to the first 'M' bit of this object a value corresponding to
        // the characters in the specified 'pos' of the specified 'str'.  'M'
        // is the smaller of the specified 'N' and 'str.length()'.  If 'M < N'
        // the remaining bit positions are left unchanged.  Characters with the
        // value 'zeroChar' correspond to an unset bit and characters with the
        // value 'oneChar' correspond to a set bit.  The behavior is undefined
        // if any characters in 'str' is neither the specified 'zeroChar' nor
        // the specified 'oneChar'.

    // PRIVATE ACCESSORS
    std::size_t numOneSet(unsigned int src) const;
        // Return the number of 1 bits in the specified 'src'.

  public:
    // CREATORS
    BSLS_CPP11_CONSTEXPR bitset() BSLS_CPP11_NOEXCEPT;
        // Create a 'bitset' with all bits initialized to '0'.

    BSLS_CPP11_CONSTEXPR
    bitset(unsigned long val) BSLS_CPP11_NOEXCEPT;                  // IMPLICIT
        // Create a bitset with its first 'M' bit positions correspond to bit
        // values in the specified 'val'.  'M' is the smaller of the
        // parameterized 'N' and '8 * sizeof(unsigned long)'.  If 'M < N', the
        // remaining bit positions are initialized to 0.

#if !defined(BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS)
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    explicit bitset(
     const native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&     str,
     typename
     native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type pos = 0,
     typename
     native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type n   =
                  native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::npos,
     CHAR_TYPE zeroChar = CHAR_TYPE('0'),
     CHAR_TYPE oneChar  = CHAR_TYPE('1'));
#else
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    explicit
    bitset(const native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
           bsl::string::size_type pos      = 0,
           bsl::string::size_type n        = bsl::string::npos,
           CHAR_TYPE              zeroChar = CHAR_TYPE('0'),
           CHAR_TYPE              oneChar  = CHAR_TYPE('1'));
#endif
        // Create a bitset with its first 'M' bit positions corresponding to
        // the characters in the specified 'pos' of the specified 'str'.  'M'
        // is the smaller of the parameterized 'N' and 'str.length()'.  If
        // 'M < N', the remaining bit positions are initialized to 0.
        // Characters with the value 'zeroChar' correspond to an unset bit and
        // characters with the value 'oneChar' correspond to a set bit.  The
        // behavior is undefined if any characters in 'str' is neither the
        // specified 'zeroChar' nor the specified 'oneChar'.

#if !defined(BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS)
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    explicit bitset(
      const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&              str,
      typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type pos =
                                                                             0,
      typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type n   =
                         bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::npos,
      CHAR_TYPE zeroChar = CHAR_TYPE('0'),
      CHAR_TYPE oneChar  = CHAR_TYPE('1'));
#else
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    explicit bitset(const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
                    bsl::string::size_type pos      = 0,
                    bsl::string::size_type n        = bsl::string::npos,
                    CHAR_TYPE              zeroChar = CHAR_TYPE('0'),
                    CHAR_TYPE              oneChar  = CHAR_TYPE('1'));
#endif
        // Create a bitset with its first 'M' bit positions corresponding to 0
        // the characters in the specified 'pos' of the specified 'str'.  'M'
        // is the smaller of the parameterized 'N' and 'str.length()'.  If
        // 'M < N', the remaining bit positions are initialized to 0.
        // Characters with the value 'zeroChar' correspond to an unset bit and
        // characters with the value 'oneChar' correspond to a set bit.  The
        // behavior is undefined if the characters in the specified 'str' is
        // not the specified 'zeroChar' and not the specified 'oneChar'

    // MANIPULATORS
    bitset& operator&=(const bitset& rhs) BSLS_CPP11_NOEXCEPT;
        // Clear each bit of this bitset for each corresponding bit that is 0
        // in the specified 'rhs', and leaves all other bits unchanged.  Return
        // a reference to this modifiable bitset.  Note that this is equivalent
        // to a bitwise OR.

    bitset& operator|=(const bitset& rhs) BSLS_CPP11_NOEXCEPT;
        // Set each bit of this bitset for each corresponding bit that is 1 in
        // the specified 'rhs', and leaves all other bits unchanged.  Return a
        // reference to this modifiable bitset.  Note that this is equivalent
        // to a bitwise AND.

    bitset& operator^=(const bitset& rhs) BSLS_CPP11_NOEXCEPT;
        // Toggle each bit of this bitset for each corresponding bit that is 1
        // in the specified 'rhs', and leaves all other bits unchanged.  Return
        // a reference to this modifiable bitset.  Note that this is equivalent
        // to a bitwise XOR.

    bitset& operator<<=(std::size_t pos) BSLS_CPP11_NOEXCEPT;
        // Shift the bits of this bitset left (towards the most significant
        // bit) by the specified 'pos' and return a reference to this
        // modifiable bitset.  For all bits with position I where 'I <= pos',
        // the new value is 0.  The behavior is undefined unless 'pos <= N'.

    bitset& operator>>=(std::size_t pos) BSLS_CPP11_NOEXCEPT;
        // Shift the bits of this bitset right (towards the least significant
        // bit) by the specified 'pos' and return a reference to this
        // modifiable bitset.  For all bits with position I where
        // 'I > N - pos', the new value is 0.  The behavior is undefined unless
        // 'pos <= N'.

    bitset& flip() BSLS_CPP11_NOEXCEPT;
        // Toggle all bits of this bitset and return a reference to this
        // modifiable bitset.

    bitset& flip(std::size_t pos);
        // Toggle the bit at the specified 'pos' of this bitset and return a
        // reference to this modifiable bitset.

    bitset& reset() BSLS_CPP11_NOEXCEPT;
        // Set all bits of this bitset to 0 and return a reference to this
        // modifiable bitset.

    bitset& reset(std::size_t pos);
        // Set the bit at the specified 'pos' of this bitset to 0 and return a
        // reference to this modifiable bitset.

    bitset& set() BSLS_CPP11_NOEXCEPT;
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
    bitset operator<<(std::size_t pos) const BSLS_CPP11_NOEXCEPT;
        // Return a bitset constructed from shifting this bitset left by the
        // specified 'pos'.

    bitset operator>>(std::size_t pos) const BSLS_CPP11_NOEXCEPT;
        // Return a bitset constructed from shifting this bitset right by the
        // specified 'pos'.

    bitset operator~() const BSLS_CPP11_NOEXCEPT;
        // Toggle all bits of this bitset and return a reference to this
        // modifiable bitset.

#if __cplusplus >= 201103L
    template <class CHAR_TYPE = char,
              class TRAITS = char_traits<CHAR_TYPE>,
              class ALLOCATOR = allocator<CHAR_TYPE> >
#else
    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
#endif
    basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> to_string(
                                        CHAR_TYPE zero = CHAR_TYPE('0'),
                                        CHAR_TYPE one  = CHAR_TYPE('1')) const;
        // Return a 'basic_string' representation of this bitset, where the
        // zero-bits are represented by the specified 'zero' character and the
        // one-bits are represented by the specified 'one' character.  The
        // most-significant bit is placed at the beginning of the string, and
        // the least-significant bit is placed at the end of the string.

    BSLS_CPP11_CONSTEXPR bool operator[](std::size_t pos) const;
        // Return the value of the bit position at the specified 'pos'.

    bool operator==(const bitset& rhs) const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if the specified 'rhs' has the same value as this
        // bitset and 'false' otherwise.  Two bitsets have the same value when
        // the sequence and value of bits they hold are the same.

    bool operator!=(const bitset& rhs) const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if the specified 'rhs' do not have the same value as
        // this bitset and 'false' otherwise.  Two bitset do not have the same
        // value when either the sequence or the value of bits they hold are
        // not the same.

    bool all() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if all of the bits in this bitset have the value of 1
        // and 'false' otherwise.  Note that 'all()' and 'none()' are both true
        // for bitsets of size 0.

    bool any() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if one or more of the bits in this bitset has the
        // value of 1 and 'false' otherwise.

    bool none() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if all the bits in this bitset has the value of 0 and
        // 'false' otherwise.  Note that 'all()' and 'none()' are both true
        // for bitsets of size 0.

    BSLS_CPP11_CONSTEXPR std::size_t size() const BSLS_CPP11_NOEXCEPT;
        // Return the number of bits this bitset holds.

    std::size_t count() const BSLS_CPP11_NOEXCEPT;
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
bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs)
                                                           BSLS_CPP11_NOEXCEPT;
    // Return a 'bitset' that results from a bitwise AND of the specified 'lhs'
    // and 'rhs'.

template <std::size_t N>
bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs)
                                                           BSLS_CPP11_NOEXCEPT;
    // Return a 'bitset' that results from a bitwise OR of the specified 'lhs'
    // and 'rhs'.

template <std::size_t N>
bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs)
                                                           BSLS_CPP11_NOEXCEPT;
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


                        // --------------------------
                        // class Bitset_ImpBase<N, 1>
                        // --------------------------

template <std::size_t BITSETSIZE>
inline BSLS_CPP11_CONSTEXPR
Bitset_ImpBase<BITSETSIZE, 1>::Bitset_ImpBase()
: d_data()
{
}

template <std::size_t BITSETSIZE>
inline BSLS_CPP11_CONSTEXPR
Bitset_ImpBase<BITSETSIZE, 1>::Bitset_ImpBase(unsigned long val)
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) \
 &&!(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER < 1900)
  : d_data{static_cast<unsigned int>(val)}
{
}
#else
{
    Bitset_ImpUtil::defaultInit(d_data, BITSETSIZE, val);
}
#endif

                        // --------------------------
                        // class Bitset_ImpBase<N, 2>
                        // --------------------------

template <std::size_t BITSETSIZE>
inline BSLS_CPP11_CONSTEXPR
Bitset_ImpBase<BITSETSIZE, 2>::Bitset_ImpBase()
: d_data()
{
}

  template <std::size_t BITSETSIZE>
inline BSLS_CPP11_CONSTEXPR
Bitset_ImpBase<BITSETSIZE, 2>::Bitset_ImpBase(unsigned long val)
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS) \
 &&!(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER < 1900)
  : d_data{static_cast<unsigned int>(val),
           static_cast<unsigned int>(val >> (sizeof(int) * CHAR_BIT))}
{
}
#else
{
    Bitset_ImpUtil::defaultInit(d_data, BITSETSIZE, val);
}
#endif

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
bitset<N>::reference::operator=(bool x) BSLS_CPP11_NOEXCEPT
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
bitset<N>::reference::operator=(const reference& x) BSLS_CPP11_NOEXCEPT
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
bitset<N>::reference::flip() BSLS_CPP11_NOEXCEPT
{
    *d_int_p ^= (1 << d_offset);
    return *this;
}

// ACCESSORS
template <std::size_t N>
inline
bitset<N>::reference::operator bool() const BSLS_CPP11_NOEXCEPT
{
    return ((*d_int_p & (1 << d_offset)) != 0);
}

template <std::size_t N>
inline
bool bitset<N>::reference::operator~() const BSLS_CPP11_NOEXCEPT
{
    return ((*d_int_p & (1 << d_offset)) == 0);
}

                        // ------------
                        // class bitset
                        // ------------

// PRIVATE MANIPULATORS
template <std::size_t N>
inline
void bitset<N>::clearUnusedBits()
{
    enum { k_VALUE = N % k_BITS_PER_INT ? 1 : 0 };

    clearUnusedBits(bsl::integral_constant<bool, k_VALUE>());
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
    const unsigned int offset = N % k_BITS_PER_INT;  // never 0

    d_data[k_BITSETSIZE - 1] &= ~(~((unsigned int)0) << offset);
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

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
void bitset<N>::copyString(
const    native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&           str,
typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type pos,
typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type n,
CHAR_TYPE zeroChar,
CHAR_TYPE oneChar)
{
    typedef typename native_std::basic_string<CHAR_TYPE,
                                               TRAITS,
                                               ALLOCATOR>::size_type size_type;
    n = std::min(N, std::min(n, str.size() - pos));
    for (size_type i = 0; i < n; ++i) {
        typename TRAITS::int_type bit = TRAITS::to_int_type(
                                                        str[pos + n  - i - 1]);

        if (bit == oneChar) {
            set(i);
        }
        else if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(bit != zeroChar)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument(
                                             "string for bitset constructor "
                                             "must be '0' or '1'");
        }
    }
}

template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
void bitset<N>::copyString(
       const    bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&           str,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type pos,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type n,
       CHAR_TYPE zeroChar,
       CHAR_TYPE oneChar)
{
    typedef typename bsl::basic_string<CHAR_TYPE,
                                          TRAITS,
                                          ALLOCATOR>::size_type size_type;
    n = std::min(N, std::min(n, str.size() - pos));
    for (size_type i = 0; i < n; ++i) {
        typename TRAITS::int_type bit = TRAITS::to_int_type(
                                                        str[pos + n  - i - 1]);

        if (bit == oneChar) {
            set(i);
        }
        else if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(bit != zeroChar)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument(
                                             "string for bitset constructor "
                                             "must be '0' or '1'");
        }
    }
}

// CREATORS
template <std::size_t N>
inline BSLS_CPP11_CONSTEXPR
bitset<N>::bitset() BSLS_CPP11_NOEXCEPT : Base()
{
}

template <std::size_t N>
BSLS_CPP11_CONSTEXPR
bitset<N>::bitset(unsigned long val) BSLS_CPP11_NOEXCEPT : Base(val)
{
}

#if !defined(BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS)
template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const    native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&    str,
       typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                                 size_type pos,
       typename native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::
                                                                 size_type n,
      CHAR_TYPE zeroChar,
      CHAR_TYPE oneChar)
#else
template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const native_std::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
       bsl::string::size_type pos,
       bsl::string::size_type n,
       CHAR_TYPE              zeroChar,
       CHAR_TYPE              oneChar)
#endif
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > str.size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                  "'pos > str.size()' for bitset constructor");
    }
    memset(d_data, 0, k_BITSETSIZE * k_BYTES_PER_INT);
    copyString(str, pos, n, zeroChar, oneChar);
}


#if !defined(BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS)
template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const    bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>&           str,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type pos,
       typename bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>::size_type n,
       CHAR_TYPE zeroChar,
       CHAR_TYPE oneChar)
#else
template <std::size_t N>
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
bitset<N>::
bitset(const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& str,
       bsl::string::size_type pos,
       bsl::string::size_type n,
       CHAR_TYPE              zeroChar,
       CHAR_TYPE              oneChar)
#endif
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > str.size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                  "'pos > str.size()' for bitset constructor");
    }
    memset(d_data, 0, k_BITSETSIZE * k_BYTES_PER_INT);
    copyString(str, pos, n, zeroChar, oneChar);
}

// MANIPULATORS
template <std::size_t N>
bitset<N>& bitset<N>::operator&=(const bitset& rhs) BSLS_CPP11_NOEXCEPT
{
    for (std::size_t i = 0; i < k_BITSETSIZE; ++i) {
        d_data[i] &= rhs.d_data[i];
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator|=(const bitset& rhs) BSLS_CPP11_NOEXCEPT
{
    for (std::size_t i = 0; i < k_BITSETSIZE; ++i) {
        d_data[i] |= rhs.d_data[i];
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator^=(const bitset& rhs) BSLS_CPP11_NOEXCEPT
{
    for (std::size_t i = 0; i < k_BITSETSIZE; ++i) {
        d_data[i] ^= rhs.d_data[i];
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator<<=(std::size_t pos) BSLS_CPP11_NOEXCEPT
{
    BSLS_ASSERT_SAFE(pos <= N);

    if (pos) {
        const std::size_t shift  = pos / k_BITS_PER_INT;
        const std::size_t offset = pos % k_BITS_PER_INT;

        if (shift) {
            memmove(d_data + shift,
                    d_data,
                    (k_BITSETSIZE - shift) * k_BYTES_PER_INT);
            memset(d_data, 0, shift * k_BYTES_PER_INT);
        }

        if (offset) {
            for (std::size_t i = k_BITSETSIZE - 1; i > shift; --i) {
                d_data[i] = (d_data[i] << offset)
                          | (d_data[i-1] >> (k_BITS_PER_INT - offset));
            }
            d_data[shift] <<= offset;
        }

        clearUnusedBits();
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::operator>>=(std::size_t pos) BSLS_CPP11_NOEXCEPT
{
    BSLS_ASSERT_SAFE(pos <= N);

    if (pos) {
        const std::size_t shift  = pos / k_BITS_PER_INT;
        const std::size_t offset = pos % k_BITS_PER_INT;

        if (shift) {
            memmove(d_data,
                    d_data + shift,
                    (k_BITSETSIZE - shift) * k_BYTES_PER_INT);
            memset(d_data + k_BITSETSIZE - shift, 0, shift * k_BYTES_PER_INT);
        }

        if (offset) {
            for (std::size_t i = 0; i < k_BITSETSIZE - shift - 1; ++i) {
                d_data[i] = (d_data[i] >> offset)
                          | (d_data[i+1] << (k_BITS_PER_INT - offset));
            }
            d_data[k_BITSETSIZE - shift - 1] >>= offset;
        }

        clearUnusedBits();
    }
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::flip() BSLS_CPP11_NOEXCEPT
{
    for (std::size_t i = 0; i < k_BITSETSIZE; ++i) {
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

    const std::size_t shift  = pos / k_BITS_PER_INT;
    const std::size_t offset = pos % k_BITS_PER_INT;
    d_data[shift] ^= (1 << offset);
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::reset() BSLS_CPP11_NOEXCEPT
{
    memset(d_data, 0, k_BITSETSIZE * k_BYTES_PER_INT);
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::reset(std::size_t pos)
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / k_BITS_PER_INT;
    const std::size_t offset = pos % k_BITS_PER_INT;
    d_data[shift] &= ~(1 << offset);
    return *this;
}

template <std::size_t N>
inline
bitset<N>& bitset<N>::set() BSLS_CPP11_NOEXCEPT
{
    memset(d_data, 0xFF, k_BITSETSIZE * k_BYTES_PER_INT);
    clearUnusedBits();
    return *this;
}

template <std::size_t N>
bitset<N>& bitset<N>::set(std::size_t pos, int val)
{
    BSLS_ASSERT_SAFE(pos < N);

    const std::size_t shift  = pos / k_BITS_PER_INT;
    const std::size_t offset = pos % k_BITS_PER_INT;
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

    const std::size_t shift  = pos / k_BITS_PER_INT;
    const std::size_t offset = pos % k_BITS_PER_INT;
    return typename bitset<N>::reference(&d_data[shift],
                                         static_cast<unsigned int>(offset));
}

// ACCESSORS
template <std::size_t N>
inline
bitset<N> bitset<N>::operator<<(std::size_t pos) const BSLS_CPP11_NOEXCEPT
{
    BSLS_ASSERT_SAFE(pos <= N);

    bitset<N> tmp(*this);
    return tmp <<= pos;
}

template <std::size_t N>
inline
bitset<N> bitset<N>::operator>>(std::size_t pos) const BSLS_CPP11_NOEXCEPT
{
    BSLS_ASSERT_SAFE(pos <= N);

    bitset<N> tmp(*this);
    return tmp >>= pos;
}

template <std::size_t N>
inline
bitset<N> bitset<N>::operator~() const BSLS_CPP11_NOEXCEPT
{
    bitset<N> tmp(*this);
    return tmp.flip();
}

template <std::size_t N>
inline BSLS_CPP11_CONSTEXPR
bool bitset<N>::operator[](std::size_t pos) const
{
#if defined(BSLSTL_BITSET_ALLOW_ASSERT_IN_CONSTEXPR)
    BSLS_ASSERT_SAFE(pos < N);
#endif

    return 0 != (d_data[pos / k_BITS_PER_INT] & (1 << (pos % k_BITS_PER_INT)));
}

template <std::size_t N>
inline
bool bitset<N>::operator==(const bitset& rhs) const BSLS_CPP11_NOEXCEPT
{
    return memcmp(d_data, rhs.d_data, k_BITSETSIZE * k_BYTES_PER_INT) == 0;
}

template <std::size_t N>
inline
bool bitset<N>::operator!=(const bitset& rhs) const BSLS_CPP11_NOEXCEPT
{
    return !operator==(rhs);
}

template <std::size_t N>
bool bitset<N>::all() const BSLS_CPP11_NOEXCEPT
{
    for (std::size_t i = 0; i < N / k_BITS_PER_INT; ++i) {
        if (d_data[i] != ~0u) {
            return false;
        }
    }

    const std::size_t modulo = N % k_BITS_PER_INT;

    if (modulo) {
        const std::size_t mask = ((1u << modulo) - 1);
        return d_data[k_BITSETSIZE - 1] == mask;
    }

    return true;
}

template <std::size_t N>
bool bitset<N>::any() const BSLS_CPP11_NOEXCEPT
{
    for (std::size_t i = 0; i < k_BITSETSIZE; ++i) {
        if (d_data[i] != 0) {
            return true;                                              // RETURN
        }
    }
    return false;
}

template <std::size_t N>
std::size_t bitset<N>::count() const BSLS_CPP11_NOEXCEPT
{
    std::size_t sum = 0;
    for (std::size_t i = 0; i < k_BITSETSIZE; ++i) {
        sum += numOneSet(d_data[i]);
    }
    return sum;
}

template <std::size_t N>
inline
bool bitset<N>::none() const BSLS_CPP11_NOEXCEPT
{
    return !any();
}

template <std::size_t N>
inline
BSLS_CPP11_CONSTEXPR std::size_t bitset<N>::size() const BSLS_CPP11_NOEXCEPT
{
    return N;
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
template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> bitset<N>::to_string(
                                                           CHAR_TYPE zero,
                                                           CHAR_TYPE one) const
{
    basic_string<CHAR_TYPE, TRAITS, ALLOCATOR> str(N, zero);
    for (std::size_t i = 0; i < N; ++i) {
        if (this->operator[](i)) {
            str[N - i - 1] = one;
        }
    }
    return str;
}

template <std::size_t N>
unsigned long bitset<N>::to_ulong() const
{
    enum {
        k_INTS_IN_LONG = sizeof(unsigned long) / sizeof(int)
    };

    for (std::size_t i = k_INTS_IN_LONG; i < k_BITSETSIZE; ++i) {
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_data[i])) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwOverflowError(
                                        "overflow in bsl::bitset<>::to_ulong");
        }
    }

    unsigned long      value   = 0;
    const unsigned int numInts = (unsigned int) k_INTS_IN_LONG
                                                  < (unsigned int) k_BITSETSIZE
                               ? (unsigned int) k_INTS_IN_LONG
                               : (unsigned int) k_BITSETSIZE;

    for (unsigned int i = 0; i < numInts; ++i) {
        value |= (unsigned long) d_data[i] << (k_BITS_PER_INT * i);
    }
    return value;
}

// FREE OPERATORS
template <std::size_t N>
bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    bitset<N> tmp(lhs);
    return tmp &= rhs;
}

template <std::size_t N>
bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    bitset<N> tmp(lhs);
    return tmp |= rhs;
}

template <std::size_t N>
bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs)
                                                            BSLS_CPP11_NOEXCEPT
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

#if defined(BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS)
# undef BSLSTL_BITSET_MSVC_CANNOT_PARSE_DEFAULTS_WITH_COLONS
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
