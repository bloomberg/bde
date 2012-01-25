// bslstl_string.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRING
#define INCLUDED_BSLSTL_STRING

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant 'basic_string' class template.
//
//@CLASSES:
//   bsl::basic_string: C++ standard compliant 'basic_string' implementation
//         bsl::string: 'typedef' for 'bsl::basic_string<char>'
//        bsl::wstring: 'typedef' for 'bsl::basic_string<wchar>'
//
//@SEE_ALSO: ISO C++ Standard, Section 21 [strings]
//
//@AUTHOR: Herve Bronnimann (hbronnim), Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_string.h>' and use 'bsl::string' instead.  This component implements a
// dynamic string class that supports the 'bslma_Allocator' model and is
// suitable for use as an implementation of the 'std::basic_string' class
// template.
//
///Lexicographical Comparisons
///---------------------------
// Two strings 'lhs' and 'rhs' are lexicographically compared by first
// determining 'N', the smaller of the lengths of 'lhs' and 'rhs', and
// comparing characters at each position between 0 and 'N - 1', using
// 'CHAR_TRAITS::lt' in lexicographical fashion.  If 'CHAR_TRAITS::lt'
// determines that strings are non-equal (smaller or larger), then this is the
// result.  Otherwise, the lengths of the strings are compared and the shorter
// string is declared the smaller.  Lexicographical comparison returns equality
// only when both strings have the same length and the same character value in
// each respective position.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_string.h> instead of <bslstl_string.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_CONTAINERBASE
#include <bslstl_containerbase.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_STRINGREFDATA
#include <bslstl_stringrefdata.h>
#endif

#ifndef INCLUDED_BSLSTL_UTIL
#include <bslstl_util.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#include <bslalg_typetraitsgroupstlsequence.h>
#endif

#ifndef INCLUDED_BSLMF_ANYTYPE
#include <bslmf_anytype.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNEDBUFFER
#include <bsls_alignedbuffer.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTFROMTYPE
#include <bsls_alignmentfromtype.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
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

#ifndef INCLUDED_ISTREAM
#include <istream>  // for 'std::basic_istream', 'sentry'
#define INCLUDED_ISTREAM
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>  // for 'std::basic_ostream', 'sentry'
#define INCLUDED_OSTREAM
#endif

#ifndef INCLUDED_STRING
#include <string>  // for 'native_std::char_traits'
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_EXCEPTION
#include <exception>
#define INCLUDED_EXCEPTION
#endif

#ifndef INCLUDED_STDEXCEPT
#include <stdexcept>
#define INCLUDED_STDEXCEPT
#endif

#endif

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

// For transitive includes.  This is not placed in 'bsl+stdhdrs' because it
// causes a cycle within the native standard headers.
#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

#if defined(BDE_BUILD_TARGET_STLPORT)
// Code in Robo depends on these headers included transitively with <string>
// and it fails to build otherwise in the stlport4 mode on Sun.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#endif

namespace bsl {

// Forward declaration
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
class basic_string;

#if defined(BSLS_PLATFORM__CMP_SUN) || defined(BSLS_PLATFORM__CMP_HP)
template <class ORIGINAL_TRAITS>
class String_Traits {
    // Workaround for Sun's 'char_traits::find' returning incorrect results
    // for any character type that's not 'char' (such as, 'wchar').

    // PRIVATE TYPES
    typedef typename ORIGINAL_TRAITS::char_type char_type;
    typedef native_std::size_t                  size_type;

  public:
    // CLASS METHODS
    static const char_type *find(const char_type  *s,
                                 size_type         n,
                                 const char_type&  a);
};

template <>
class String_Traits<native_std::char_traits<char> > {
    // Sun implemented 'find' for 'char' properly, so this specialization
    // simply forwards the call to Sun.

    typedef native_std::size_t                  size_type;

  public:
    // CLASS METHODS
    static const char *find(const char *s, size_type n, const char& a);
};

// CLASS METHODS
template <class ORIGINAL_TRAITS>
const typename ORIGINAL_TRAITS::char_type *
String_Traits<ORIGINAL_TRAITS>::find(const char_type  *s,
                                     size_type         n,
                                     const char_type&  a)
{
    while (n > 0 && !ORIGINAL_TRAITS::eq(*s, a)) {
        --n;
        ++s;
    }
    return n > 0 ? s : 0;
}

inline
const char *
String_Traits<native_std::char_traits<char> >::find(const char  *s,
                                                    size_type    n,
                                                    const char&  a)
{
    return native_std::char_traits<char>::find(s, n, a);
}

#define BSLSTL_CHAR_TRAITS String_Traits<CHAR_TRAITS>

#else

#define BSLSTL_CHAR_TRAITS CHAR_TRAITS

#endif

                        // ================
                        // class String_Imp
                        // ================

template <typename CHAR_TYPE, typename SIZE_TYPE>
class String_Imp {
    // This component-private class describes the basic data layout for a
    // string class and provides methods to help encapsulate internal string
    // implementation details.  It is parameterized by 'CHAR_TYPE' and
    // 'SIZE_TYPE' only, and implements the portion of 'basic_string' that does
    // not need to know about its parameterized 'CHAR_TRAITS' or 'ALLOCATOR'.
    // It contains the following data fields: pointer to string, short string
    // buffer, length, and capacity.  The purpose of the short string buffer is
    // to implement a "short string optimization" the idea of which is to store
    // strings with lengths shorter than a certain number of characters
    // directly inside the string object (inside the short string buffer), and
    // avoid memory allocations/deallocations.

  public:
    // TYPES
    enum ShortBufferConstraints {
        // This enum contains values necessary to calculate the size of the
        // short string buffer.  The starting value is
        // 'SHORT_BUFFER_MIN_BYTES', which defines the minimal number of bytes
        // (or 'char' values) that the short string buffer should be able to
        // contain.  Then this value is aligned to a word boundary.  Then we
        // make sure that it fits at least one 'CHAR_TYPE' character (because
        // the default state of the string object requires that the first
        // character is initialized with a NULL-terminator).  The final output
        // of this enum used by 'String_Imp' is the 'SHORT_BUFFER_CAPACITY'
        // value.  It defines the capacity of the short string buffer and also
        // the capacity of the default-constructed empty string object.

        SHORT_BUFFER_MIN_BYTES  = 20 // minimum required size of the short
                                     // string buffer in bytes

      , SHORT_BUFFER_NEED_BYTES =
                               (SHORT_BUFFER_MIN_BYTES + sizeof(SIZE_TYPE) - 1)
                                                     & ~(sizeof(SIZE_TYPE) - 1)
                                    // round it to a word boundary

      , SHORT_BUFFER_BYTES      = sizeof(CHAR_TYPE) < SHORT_BUFFER_NEED_BYTES
                                  ? SHORT_BUFFER_NEED_BYTES
                                  : sizeof(CHAR_TYPE)
                                    // in case 'CHAR_TYPE' is very large

      , SHORT_BUFFER_LENGTH     = SHORT_BUFFER_BYTES / sizeof(CHAR_TYPE)

      , SHORT_BUFFER_CAPACITY   = SHORT_BUFFER_LENGTH - 1
                                    // short string buffer capacity (not
                                    // including the null-terminator)
    };

    // Make sure the buffer is large enough to fit a pointer.
    BSLMF_ASSERT(SHORT_BUFFER_BYTES >= sizeof(CHAR_TYPE *));

    enum ConfigurableParameters {
        // These configurable parameters define various aspects of the string
        // behavior when it's not strictly defined by the Standard.

        BASIC_STRING_DEALLOCATE_IN_CLEAR  = false
      , BASIC_STRING_HONOR_SHRINK_REQUEST = false
    };

    // DATA
    union {
        // This is the union of the string storage options: it can either be
        // stored inside the short string buffer, 'd_short', or in the
        // externally allocated memory, pointed to by 'd_start_p'.

        BloombergLP::bsls_AlignedBuffer<
                   SHORT_BUFFER_BYTES,
                   BloombergLP::bsls_AlignmentFromType<CHAR_TYPE>::VALUE>
                                 d_short;   // short string buffer
        CHAR_TYPE               *d_start_p; // pointer to the data on heap
    };

    SIZE_TYPE      d_length;    // length of the string
    SIZE_TYPE      d_capacity;  // capacity to which the string can grow
                                // without reallocation

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(String_Imp,
                                 BloombergLP::bslalg_TypeTraitBitwiseMoveable);
        // 'CHAR_TYPE' is required to be a POD as per the Standard, which makes
        // 'CHAR_TYPE' bitwise-moveable, so 'String_Imp' is also
        // bitwise-moveable.

    // CLASS METHODS
    static SIZE_TYPE computeNewCapacity(SIZE_TYPE newLength,
                                        SIZE_TYPE oldCapacity,
                                        SIZE_TYPE maxSize);
        // Compute and return a new capacity value required to fit a string
        // with length 'newLength' while using the specified 'oldCapacity' to
        // exercise an exponential capacity growth necessary to ensure the
        // amortized linear complexity of 'push_back' and other operations.
        // The return value can't be larger than the specified 'maxSize' to
        // prevent overflow.  Note that the behavior is undefined unless
        // 'newLength > oldCapacity' and both 'newLength' and 'oldCapacity' are
        // less than 'maxSize'.

    // CREATORS
    String_Imp();
        // Create a 'String_Imp' object in a default state.  All fields are
        // value-initialized and 'd_capacity' is initialized with the
        // 'SHORT_BUFFER_CAPACITY' value.

    String_Imp(SIZE_TYPE length, SIZE_TYPE capacity);
        // Create a 'String_Imp' object and initialize 'd_length' and
        // 'd_capacity' fields with the specified 'length' and 'capacity'
        // parameters.  If 'capacity' is less then 'SHORT_BUFFER_CAPACITY' then
        // d_capacity is set to 'SHORT_BUFFER_CAPACITY'.  The value of the
        // 'd_short' and 'd_start_p' fields are left uninitialized.
        // 'basic_string' is required to assign either d_short or d_start_p to
        // a proper value before using any methods of this class.

    //! String_Imp(const String_Imp&);
    //! ~String_Imp();
    //! String_Imp &operator=(const String_Imp&);
        // Compiler generated copy constructor, destructor, and copy-assignment
        // operator.  'String_Imp' does not own its resources, so trivial
        // implementations suffice.

    // MANIPULATORS
    void swap(String_Imp& other);
        // Swap the representation of this string with that of the specified
        // 'other' string.

    void resetFields();
        // Reset 'String_Imp' fields to their default state as if the object
        // were constructed with the default constructor.

    CHAR_TYPE       *dataPtr();
        // Return a pointer to the modifiable NULL-terminated C-string stored
        // in this string object.  The pointer can point to either the internal
        // short string buffer or the externally allocated memory depending on
        // the type of the string defined by the return value of
        // 'isShortString'.

    // ACCESSORS
    bool isShortString() const;
        // Return 'true' if this object contains a short string and the
        // string data is stored in the short string buffer, and 'false' if the
        // object contains a long string (and the short string buffer contains
        // a pointer to the string data allocated externally).

    const CHAR_TYPE *dataPtr() const;
        // Return a pointer to the non-modifiable NULL-terminated C-string
        // stored in this string object.  The pointer can point to either the
        // internal short string buffer or the externally allocated memory
        // depending on the type of the string defined by the return value of
        // 'isShortString'.
};

                        // =======================
                        // class bsl::basic_string
                        // =======================

// Import 'char_traits' into the 'bsl' namespace so that 'basic_string' and
// 'char_traits' are always in the same namespace.
using native_std::char_traits;

template <typename CHAR_TYPE,
          typename CHAR_TRAITS = char_traits<CHAR_TYPE>,
          typename ALLOCATOR = allocator<CHAR_TYPE> >
class basic_string
    : private String_Imp<CHAR_TYPE, typename ALLOCATOR::size_type>
    , public BloombergLP::bslstl_ContainerBase<ALLOCATOR>
{
    // This class template provides an STL-compliant 'string' that conforms to
    // the 'bslma_Allocator' model.  For the requirements of a string class,
    // consult the second revision of the ISO/IEC 14882 Programming Language
    // C++ (2003).  Note that the parameterized 'CHAR_TYPE' must be *equal* to
    // 'ALLOCATOR::value_type'.  In addition, this implementation offers strong
    // exception guarantees (see below), with the general rules that:
    //..
    //   (1) any method that would result in a string of length larger than the
    //       size returned by 'max_size' throws 'std::length_error', and
    //   (2) any method that attempts to access a position outside the valid
    //       range of a string throws 'std::out_of_range'.
    //..
    // Circumstances where a method throws 'bsl::length_error' (1) are clear
    // and not repeated in the individual function-level documentations below.
    //
    // More generally, this class supports an almost complete set of *in-core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison (but excluding 'ostream' printing since this
    // component is below STL).  A precise operational definition of when two
    // objects have the same value can be found in the description of
    // 'operator==' for the class.  This class is *exception* *neutral* with
    // full guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left
    // unchanged.  In no event is memory leaked.
    //
    // Note that *aliasing* (e.g., using all or part of an object as both
    // source and destination) is supported in all cases in the public
    // interface of 'basic_string'.  However, the private interface ('...Raw'
    // methods) should be assumed to be not alias-safe unless specifically
    // noted otherwise.

  public:
    // PUBLIC TYPES
    typedef CHAR_TRAITS                            traits_type;
    typedef typename CHAR_TRAITS::char_type        value_type;
    typedef ALLOCATOR                              allocator_type;
    typedef typename ALLOCATOR::size_type          size_type;
    typedef typename ALLOCATOR::difference_type    difference_type;
    typedef typename ALLOCATOR::reference          reference;
    typedef typename ALLOCATOR::const_reference    const_reference;
    typedef typename ALLOCATOR::pointer            pointer;
    typedef typename ALLOCATOR::const_pointer      const_pointer;
    typedef CHAR_TYPE                             *iterator;
    typedef const CHAR_TYPE                       *const_iterator;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;
        // These types satisfy the 'ReversibleSequence' requirements.

  private:
    // PRIVATE TYPES
    typedef String_Imp<CHAR_TYPE, typename ALLOCATOR::size_type>  Imp;

    // PRIVATE MANIPULATORS

    // Note: '...Raw' functions are low level private manipulators and they do
    // not perform checks for exceptions.  '...Dispatch' functions perform
    // overload selection for iterator types in order to resolve ambiguities
    // between template and non-template method overloads.

    CHAR_TYPE *privateAllocate(size_type numChars);
        // Allocate and return a buffer capable of holding the specified
        // 'numChars' number of characters.

    void privateDeallocate();
        // Deallocate the internal string buffer, which was allocated with
        // 'privateAllocate' and stored in 'String_Imp::d_start_p' without
        // modifying any data members.

    void privateCopy(const basic_string& original);
        // Copy the 'original' string content into this string object, assuming
        // that the default copy constructor of the 'String_Imp' base class and
        // the appropriate copy constructor of the 'bslstl_ContainerBase' base
        // class have been just run.

    basic_string& privateAppendDispatch(iterator       begin,
                                        iterator       end);
    basic_string& privateAppendDispatch(const_iterator begin,
                                        const_iterator end);
    template <typename INPUT_ITER>
    basic_string& privateAppendDispatch(INPUT_ITER     begin,
                                        INPUT_ITER     end);
        // Match either 'iterator', 'const_iterator', or an arbitrary iterator
        // (which can also match an integral type).  In the first two cases,
        // use 'privateAppendRaw'.  In the last case, forward to
        // 'privateReplaceDispatch' to separate the integral type from iterator
        // types.

    basic_string& privateAppendRaw(const CHAR_TYPE *characterString,
                                   size_type        numChars);
        // Append characters from the specified 'characterString' array of
        // characters of the specified 'numChars' length to this string, and
        // return a reference to this modifiable string.  The behavior is
        // undefined unless 'numChars <= max_size() - length()', and the
        // 'characterString' array is at least 'numChars' long.

    basic_string& privateAppendRaw(size_type numChars,
                                   CHAR_TYPE character);
        // Append the specified 'numChars' copies of the specified 'character'
        // to this string.  Return a reference to this modifiable string.  The
        // behavior is undefined unless and
        // 'numChars <= max_size() - length()'.

    Imp& privateBase();
        // Return a reference to the modifiable base object of this string.

    void privateClear(bool deallocateBufferFlag);
        // Reset this string object to a default value optionally deallocating
        // the string buffer if the specified 'deallocateBufferFlag' is 'true'.

    void privateInitDispatch(iterator       begin,
                             iterator       end);
    void privateInitDispatch(const_iterator begin,
                             const_iterator end);
    template <typename INPUT_ITER>
    void privateInitDispatch(INPUT_ITER     begin,
                             INPUT_ITER     end);
        // Initialize this string object with a string defined by the specified
        // 'begin' and 'end' iterators.  Choose the correct overload depending
        // on the type of the 'begin' and 'end' parameters.  In case of
        // 'iterator' and 'const_iterator', use 'privateAppendRaw'.  In case of
        // a generic 'INPUT_ITERATOR' (which can also resolve to an integral
        // type), forward to 'privateReplaceDispatch' to separate the integral
        // type from iterator types.

    void privateInsertDispatch(const_iterator position,
                               iterator       first,
                               iterator       last);
    void privateInsertDispatch(const_iterator position,
                               const_iterator first,
                               const_iterator last);
    template <typename INPUT_ITER>
    void privateInsertDispatch(const_iterator position,
                               INPUT_ITER     first,
                               INPUT_ITER     last);
        // Insert a string defined by the specified 'first' and 'last'
        // iterators into this string object starting from the specified
        // 'position'.  Choose the correct overload depending on the type of
        // the 'first' and 'last' parameters.  In case of 'iterator' and
        // 'const_iterator', use 'privateInsertRaw'.  In case of a generic
        // 'INPUT_ITERATOR' (which can also resolve to an integral type),
        // forward to 'privateReplaceDispatch' to separate the integral type
        // from iterator types.

    basic_string& privateInsertRaw(size_type        outPosition,
                                   const CHAR_TYPE *characterString,
                                   size_type        numChars);
        // Insert characters from the specified 'characterString' array of
        // characters with the specified 'numChars' length, into this string
        // starting at the specified 'outPosition'.  The behavior is
        // undefined unless and 'numChars <= max_size() - length()' and
        // 'characterString' array is at least 'numChars' long.  Note that this
        // method is alias-safe, i.e., it works correctly even if
        // 'characterString' points into this string object.

    basic_string& privateReplaceRaw(size_type        outPosition,
                                    size_type        outNumChars,
                                    const CHAR_TYPE *characterString,
                                    size_type        numChars);
        // Replace the specified 'outNumChars' characters at the specified
        // 'outPosition' in this string, by the characters in the array of
        // length the specified 'numChars' at the specified 'characterString'
        // address, and return a reference to this modifiable string.  The
        // behavior is undefined unless 'outPosition <= length()',
        // 'outNumChars <= length()', 'outPosition <= length() - outNumChars',
        // 'numChars <= max_size()', and
        // 'length() - outNumChars <= max_size() - numChars'.  Note that this
        // method is alias-safe, i.e., it works correctly even if
        // 'characterString' points into this string object.

    basic_string& privateReplaceRaw(size_type outPosition,
                                    size_type outNumChars,
                                    size_type numChars,
                                    CHAR_TYPE character);
        // Replace the specified 'outNumChars' characters at the specified
        // 'outPosition' in this string, by the specified 'numChars' copies of
        // the specified 'character', and return a reference to this modifiable
        // string.  The behavior is undefined unless 'outPosition <= length()',
        // 'outNumChars <= length()', 'outPosition <= length() - outNumChars'
        // and 'length() <= max_size() - numChars'.

    template <typename INPUT_ITER>
    basic_string& privateReplaceDispatch(size_type  position,
                                         size_type  numChars,
                                         INPUT_ITER first,
                                         INPUT_ITER last,
                                         BloombergLP::bslstl_UtilIterator,
                                         int);
        // Match integral type for 'INPUT_ITER'.

    template <typename INPUT_ITER>
    basic_string& privateReplaceDispatch(size_type  position,
                                         size_type  numChars,
                                         INPUT_ITER first,
                                         INPUT_ITER last,
                                         BloombergLP::bslmf_AnyType,
                                         BloombergLP::bslmf_AnyType);
        // Match non-integral type for 'INPUT_ITER'.

    template <typename INPUT_ITER>
    basic_string& privateReplace(size_type  position,
                                 size_type  numChars,
                                 INPUT_ITER first,
                                 INPUT_ITER last,
                                 std::input_iterator_tag);
        // Specialized replacement for input iterators, using repeated
        // 'push_back' operations.

    template <typename INPUT_ITER>
    basic_string& privateReplace(size_type  position,
                                 size_type  numChars,
                                 INPUT_ITER first,
                                 INPUT_ITER last,
                                 std::forward_iterator_tag);
        // Specialized replacement for forward, bidirectional, and
        // random-access iterators.  Throw 'length_error' if
        // 'length() - numChars > max_size() - distance(first, last)'.

    basic_string& privateReplace(size_type      position,
                                 size_type      numChars,
                                 iterator       first,
                                 iterator       last,
                                 std::forward_iterator_tag);
    basic_string& privateReplace(size_type      position,
                                 size_type      numChars,
                                 const_iterator first,
                                 const_iterator last,
                                 std::forward_iterator_tag);
        // Replace the specified 'numChars' number of characters in this string
        // starting from the specified 'position' with the string defined by
        // the specified 'first' and 'last' iterators.

    void privateReserveRaw(size_type newCapacity);
        // Change the capacity of this string object to a value at least the
        // specified 'newCapacity'.  The behavior is undefined unless
        // 'newCapacity <= max_size()'.  Note that a null-terminating character
        // is not counted in 'newCapacity', and that this method has no effect
        // unless 'newCapacity > capacity()'.

    CHAR_TYPE *privateReserveRaw(size_type *storage,
                                 size_type  newCapacity,
                                 size_type  numChars);
        // Change the capacity of this string object, stored at the specified
        // 'storage' address, to a value at least the specified 'newCapacity'.
        // Upon reallocation, copy the first specified 'numChars' from the
        // previous buffer to the new buffer, and load 'storage' with the new
        // capacity.  If '*storage >= newCapacity', this method has no effect.
        // Return the new buffer if reallocation, and 0 otherwise.  The
        // behavior is undefined unless 'numChars <= length()' and
        // 'newCapacity <= max_size()'.  Note that a null-terminating character
        // is not counted in '*storage' nor 'newCapacity'.  Also note that the
        // previous buffer is *not* deallocated, nor is the string
        // representation changed (in case the previous buffer may contain data
        // that must be copied): it is the responsibility of the caller to do
        // so upon reallocation.

    basic_string& privateResizeRaw(size_type newLength, CHAR_TYPE character);
        // Change the length of this string to the specified 'newLength'.  If
        // 'newLength > length()', fill in the new positions by copies of the
        // specified 'character'.  Do not change the capacity unless
        // 'newLength' exceeds the current capacity.  The behavior is undefined
        // unless 'newLength <= max_size()'.

    // PRIVATE ACCESSORS
    int privateCompareRaw(size_type        lhsPosition,
                          size_type        lhsNumChars,
                          const CHAR_TYPE *other,
                          size_type        otherNumChars) const;
        // Lexicographically compare the substring of this string starting at
        // the specified 'lhsPosition' of length 'lhsNumChars' with the string
        // constructed from the specified 'numChars' characters in the array
        // starting at the specified 'characterString' address, and return a
        // negative value if this string is less than 'other', a positive value
        // if it is more than 'other', and 0 in case of equality.  The behavior
        // is undefined unless 'lhsPosition <= length()',
        // 'lhsNumChars <= length()', and
        // 'lhsPosition <= length() - lhsNumChars'.

    // INVARIANTS
    BSLMF_ASSERT((BloombergLP::bslmf_IsSame<CHAR_TYPE,
                                      typename ALLOCATOR::value_type>::VALUE));
        // This is required by the C++ standard (23.1, clause 1).

  public:
    // TRAITS
    typedef BloombergLP::bslalg_TypeTraitsGroupStlSequence<CHAR_TYPE,
                                                   ALLOCATOR> StringTypeTraits;

    BSLALG_DECLARE_NESTED_TRAITS(basic_string, StringTypeTraits);
        // Declare nested type traits for this class.  This class is bitwise
        // movable if the allocator is bitwise movable.  It uses 'bslma'
        // allocators if 'ALLOCATOR' is convertible from 'bslma_Allocator*'.

    // PUBLIC CLASS DATA
    static const size_type npos = ~size_type(0);
        // Value used to denote "not-a-position", guaranteed to be outside the
        // range '[ 0, max_size() ]'.

    // CREATORS

                   // *** 21.3.2 construct/copy/destroy: ***

    explicit
    basic_string(const ALLOCATOR& allocator = ALLOCATOR());
        // Create an empty string.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is not specified, a
        // default-constructed allocator is used.

    basic_string(const basic_string&       original);
    basic_string(const basic_string&       original,
                 const ALLOCATOR&          allocator);
        // Create a string that has the same value as the specified 'original'
        // string.  Optionally specify an 'allocator' used to supply memory.
        // If 'allocator' is not specified, then a default-constructed
        // allocator is used.  Note that it is important to have two copy
        // constructors instead of a single:
        //..
        //  basic_string(const basic_string& original,
        //               const ALLCOATOR&    allocator = ALLOCATOR());
        //..
        // When the copy constructor with the default allocator is used, xlC10
        // get confused and refuse to use the return value optimization, which
        // then causes extra allocations when returning by value in
        // 'operator+'.

    basic_string(const basic_string& original,
                 size_type           position,
                 size_type           numChars = npos,
                 const ALLOCATOR&    allocator = ALLOCATOR());
        // Create a string that has the same value as the substring of the
        // optionally specified 'numChars' length starting at the specified
        // 'position' in the specified 'original' string.  If 'numChars' equals
        // 'npos', then the remaining length of the string is used (i.e.,
        // 'numChars' is set to 'original.length() - position').  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // specified, a default-constructed allocator is used.  Throw
        // 'out_of_range' if 'position > original.length()' or
        // 'numChars != npos' and 'position + numChars < original.length()'.

    basic_string(const CHAR_TYPE  *characterString,
                 const ALLOCATOR&  allocator = ALLOCATOR());
    basic_string(const CHAR_TYPE  *characterString,
                 size_type         numChars,
                 const ALLOCATOR&  allocator = ALLOCATOR());
        // Create a string that has the same value as the substring of the
        // optionally specified 'numChars' length starting at the specified
        // 'position' in the specified 'characterString'.  If 'numChars' is not
        // specified, 'CHAR_TRAITS::length(characterString)' is used.
        // Optionally specify an 'allocator' used to supply memory.  If
        // 'allocator' is not specified, a default-constructed allocator is
        // used.  Throw 'out_of_range' if 'numChars >= npos'.

    basic_string(size_type        numChars,
                 CHAR_TYPE        character,
                 const ALLOCATOR& allocator = ALLOCATOR());
        // Create a string of the specified 'numChars' length whose every
        // position contains the specified 'character'.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not specified,
        // a default-constructed allocator is used.

    template <typename INPUT_ITER>
    basic_string(INPUT_ITER       first,
                 INPUT_ITER       last,
                 const ALLOCATOR& allocator = ALLOCATOR());
        // Create a string from the characters in the range starting at the
        // specified 'first' and ending at the specified 'last' iterators of
        // the parameterized 'INPUT_ITER' type.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not specified,
        // a default-constructed allocator is used.  The behavior is undefined
        // unless '[first, last)' is a valid iterator range.

    template <typename ALLOC2>
    basic_string(
        const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& original,
        const ALLOCATOR&                              allocator = ALLOCATOR());
        // Create a string that has the same value as the specified 'original'
        // string, where the type 'orig' is the string type native to the
        // compiler's library, instantiated with the same character type and
        // traits type, but not necessarily the same allocator type.  The
        // resulting string will contain the same sequence of characters as
        // 'original'.  Optionally specify an 'allocator' used to supply
        // memory.  If 'allocator' is not specified, then a default-constructed
        // allocator is used.

    basic_string(const BloombergLP::bslstl_StringRefData<CHAR_TYPE>& strRef,
                 const ALLOCATOR& allocator = ALLOCATOR());
        // Create a string that has the same value as the specified 'strRef'
        // string.  The resulting string will contain the same sequence of
        // characters as 'strRef'.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is not specified, then a
        // default-constructed allocator is used.

    ~basic_string();
        // Destroy this string object.

    // MANIPULATORS

                    // *** 21.3.2 construct/copy/destroy: ***

    basic_string& operator=(const basic_string& rhs);
        // Assign to this string the value of the specified 'rhs' string.

    basic_string& operator=(const CHAR_TYPE *rhs);
        // Assign to this string the value of the specified 'rhs' string.

    basic_string& operator=(CHAR_TYPE character);
        // Assign to this string the value of the string of length one
        // consisting of the specified 'character'.

                          // *** 21.3.4 capacity: ***

    void resize(size_type newLength, CHAR_TYPE character);
        // Change the length of this string to the specified 'newLength',
        // erasing characters at the end if 'newLength < length()' or appending
        // the appropriate number of copies of the specified 'character' at the
        // end if 'length() < newLength'.

    void resize(size_type newLength);
        // Change the length of this string to the specified 'newLength',
        // erasing characters at the end if 'newLength < length()' or appending
        // the appropriate number of copies of the specified 'character' at the
        // end if 'length() < newLength'.

    void reserve(size_type newCapacity = 0);
        // Change the capacity of this string to the specified 'newCapacity'.
        // Note that the capacity of a string is the maximum length it can
        // accommodate without reallocation.  The actual storage allocated may
        // be higher.

    void clear();
        // Reset this string to an empty value.  Note that the capacity may
        // change (or not if 'BASIC_STRING_DEALLOCATE_IN_CLEAR' is 'false').
        // Note that the Standard doesn't allow to reduce capacity on 'clear'.

                          // *** 21.3.3 iterators: ***

    iterator begin();
        // Return an iterator pointing the first character in this modifiable
        // string (or the past-the-end iterator if this string is empty).

    iterator end();
        // Return the past-the-end iterator for this modifiable string.

    reverse_iterator rbegin();
        // Return a reverse iterator pointing the last character in this
        // modifiable string (or the past-the-end reverse iterator if this
        // string is empty).

    reverse_iterator rend();
        // Return the past-the-end reverse iterator for this modifiable string.

                       // *** 21.3.5 element access: ***

    reference operator[](size_type position);
        // Return a reference to the modifiable character at the specified
        // 'position' in this string if 'position < length()', or the
        // *non-modifiable* null-terminating character if
        // 'position == length()'.  The behavior is undefined unless
        // 'position <= length()', and, in the case of 'position == length()',
        // the null-terminating character is not modified through the returned
        // reference.

    reference at(size_type position);
        // Return a reference to the modifiable character at the specified
        // 'position' in this string.  Throw 'out_of_range' if
        // 'position >= length()'.

    reference front();
        // Return a reference to the modifiable character at the first position
        // in this string.  The behavior is undefined if this string is empty.

    reference back();
        // Return a reference to the modifiable character at the last position
        // in this string.  The behavior is undefined if this string is empty.
        // Note that the last position is 'length() - 1'.

    template <typename ALLOC2>
    operator native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>() const;
        // Convert this object to a string type native to the compiler's
        // library, instantiated with the same character type and traits type,
        // but not necessarily the same allocator type.  The return string will
        // contain the same sequence of characters as 'orig' and will have a
        // default-constructed allocator.  Note that this conversion operator
        // can be invoked implicitly (e.g., during argument passing).

                         // *** 21.3.6 modifiers: ***

    basic_string& operator+=(const basic_string&  string);
    basic_string& operator+=(const CHAR_TYPE     *string);
        // Append the specified 'string' at the end of this string, and return
        // a reference to this modifiable string.

    basic_string& operator+=(CHAR_TYPE character);
        // Append the specified 'character' at the end of this string, and
        // return a reference to this modifiable string.

    basic_string& append(const basic_string& string);
    basic_string& append(const basic_string& string,
                         size_type           position,
                         size_type           numChars);
        // Append at the end of this string the first specified 'numChars'
        // characters starting at the specified 'position' in the specified
        // 'string', or the suffix of 'string' starting at 'position' if
        // 'position + numChars' is larger than the length of 'string'.  Return
        // a reference to this modifiable string.  Throw 'out_of_range' if
        // 'position > string.length()'.

    basic_string& append(const CHAR_TYPE *characterString,
                         size_type        numChars);
        // Append at the end of this string the specified 'numChars' characters
        // from the array starting at the specified 'characterString' address,
        // and return a reference to this modifiable string.

    basic_string& append(const CHAR_TYPE *characterString);
        // Append the specified 'characterString' (of length
        // 'CHAR_TRAITS::length(characterString)') at the end of this string,
        // and return a reference to this modifiable string.

    basic_string& append(size_type numChars,
                         CHAR_TYPE character);
        // Append a number equal to the specified 'numChars' of copies of the
        // specified 'character' at the end of this string, and return a
        // reference to this modifiable string.

    template <typename INPUT_ITER>
    basic_string& append(INPUT_ITER first, INPUT_ITER last);
        // Append to the end of this string a string built from the characters
        // in the range starting at the specified 'first' and ending before the
        // specified 'last' iterators of the 'iterator', 'const_iterator' or
        // parameterized 'INPUT_ITER' type, respectively.  Return a reference
        // to this modifiable string.  The behavior is undefined unless
        // '[first, last)' is a valid iterator range.

    void push_back(CHAR_TYPE   character);
        // Append the specified 'character' at the end of this string.

    basic_string& assign(const basic_string&       string);
        // Assign to this string the value of the specified 'string', and
        // return a reference to this modifiable string.

    basic_string& assign(const basic_string& string,
                         size_type           position,
                         size_type           numChars);
        // Assign to this string the value of the substring of the specified
        // 'numChars' length or 'string.length() - position', whichever is
        // smaller, starting at the specified 'position' in the specified
        // 'string', and return a reference to this modifiable string.  Throw
        // 'out_of_range' if 'position > string.length()'.

    basic_string& assign(const CHAR_TYPE *characterString);
        // Assign to this string the value of the specified null-terminated
        // 'characterString', and return a reference to this modifiable string.

    basic_string& assign(const CHAR_TYPE *characterString,
                          size_type       numChars);
        // Assign to this string the value of the string constructed from the
        // specified 'numChars' characters in the array starting at the
        // specified 'characterString' address, and return a reference to this
        // modifiable string.

    basic_string& assign(size_type numChars, CHAR_TYPE character);
        // Assign to this string the value of a string of the specified
        // 'numChars' length whose every characters equal the specified
        // 'character', and return a reference to this modifiable string.

    template <typename INPUT_ITER>
    basic_string& assign(INPUT_ITER first, INPUT_ITER last);
        // Assign to this string the value of a string built from the
        // characters in the range starting at the specified 'first' and ending
        // before the specified 'last' iterators of the 'iterator',
        // 'const_iterator' or parameterized 'INPUT_ITER' type, respectively.
        // Return a reference to this modifiable string.  The behavior is
        // undefined unless '[first, last)' is a valid iterator range.

    basic_string& insert(size_type position, const basic_string& string);
        // Insert at the specified 'position' in this string a copy of the
        // specified 'string', and return a reference to this modifiable
        // string.  Throw 'out_of_range' if 'position > length()'.

    basic_string& insert(size_type            outPosition,
                          const basic_string& string,
                          size_type           position,
                          size_type           numChars);
        // Insert at the specified 'outPosition' in this string a copy of the
        // substring of the specified 'numChars' length or
        // 'string.length() - position', whichever is smaller, starting at the
        // specified 'position' in the specified 'string', and return a
        // reference to this modifiable string.  Throw 'out_of_range' if
        // 'position > length()'.

    basic_string& insert(size_type        position,
                         const CHAR_TYPE *characterString,
                         size_type        numChars);
        // Insert at the specified 'position' in this string a copy of the
        // string constructed from the specified 'numChars' characters in the
        // array starting at the specified 'characterString' address, and
        // return a reference to this modifiable string.  Throw 'out_of_range'
        // if 'position > length()'.

    basic_string& insert(size_type        position,
                         const CHAR_TYPE *characterString);
        // Insert at the specified 'position' in this string a copy of the
        // string constructed from the specified 'characterString' (of length
        // 'CHAR_TRAITS::length(characterString)'), and return a reference to
        // this modifiable string.  Throw 'out_of_range' if
        // 'position > length()'.

    basic_string& insert(size_type position,
                         size_type numChars,
                         CHAR_TYPE character);
        // Insert at the specified 'position' in this string a number equal to
        // the specified 'numChars' of copies of the specified 'character', and
        // return a reference to this modifiable string.  Throw 'out_of_range'
        // if 'position > length()'.

    iterator insert(const_iterator position, CHAR_TYPE character);
        // Insert at the specified 'position' in this string a copy of the
        // specified 'character', and return an iterator which refers to the
        // copy of the inserted character.  The behavior is undefined unless
        // 'position' is a valid iterator on this string.

    iterator insert(const_iterator  position,
                    size_type       numChars,
                    CHAR_TYPE       character);
        // Insert at the specified 'position' in this string a specified
        // 'numChars' number of copies of the specified 'character', and return
        // an iterator which refers to the copy of the first inserted
        // character, or a non-const copy of the 'position' iterator, if
        // 'numChars == 0'.  The behavior is undefined unless 'position' is a
        // valid iterator on this string.

    template <typename INPUT_ITER>
    iterator insert(const_iterator  position,
                    INPUT_ITER      first,
                    INPUT_ITER      last);
        // Insert at the specified 'position' in this string a string built
        // from the characters in the range starting at the specified 'first'
        // and ending before the specified 'last' iterators, and return an
        // iterator which refers to the copy of the first inserted character,
        // or a non-const copy of the 'position' iterator, if 'first == last'.
        // The behavior is undefined unless 'position' is a valid iterator on
        // this string and '[first, last)' is a valid iterator range.

    basic_string& erase(size_type position = 0, size_type numChars = npos);
        // Erase from this string the substring of length the optionally
        // specified 'numChars' or 'original.length() - position', whichever is
        // smaller, starting at the optionally specified 'position'.  If
        // 'position' is not specified, the first position is used (i.e.,
        // 'position' is set to 0).  Return a reference to this modifiable
        // string.  Note that if 'numChars' equals 'npos', then the remaining
        // length of the string is erased (i.e., 'numChars' is set to
        // 'length() - position').  Throw 'out_of_range' if
        // 'position > length()'.

    iterator erase(const_iterator position);
        // Erase a character at the specified 'position' from this string, and
        // return an iterator pointing to the character which the 'position'
        // iterator was pointing to prior to erasing.  If no such character
        // exists, return 'end()'.  The behavior is undefined unless 'position'
        // belongs to the half-open range '[cbegin(), cend())'.

    iterator erase(const_iterator first, const_iterator last);
        // Erase from this string a substring defined by the pair of 'first'
        // and 'last' iterators within this string.  Return an iterator
        // pointing to the character which the 'last' iterator was pointing to
        // prior to erasing.  If no such character exists, return
        // 'end()'.  The behavior is undefined unless 'first' and 'last' both
        // belong to '[cbegin(), cend()]' and 'first <= last'.  Note that this
        // call invalidates existing iterators pointing to 'first' or a
        // subsequent position.

    void pop_back();
        // Erase the last character from this string.  The behavior is
        // undefined if this string is empty.

    basic_string& replace(size_type            outPosition,
                          size_type            outNumChars,
                          const basic_string&  string);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by the specified 'string', and return a
        // reference to this modifiable string.  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(size_type           outPosition,
                          size_type           outNumChars,
                          const basic_string& string,
                          size_type           position,
                          size_type           numChars);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by the substring of the specified 'numChars'
        // length or 'string.length() - position', whichever is smaller,
        // starting at the specified 'position' in the specified 'string'.
        // Return a reference to this modifiable string.  Throw 'out_of_range'
        // if 'outPosition > length()' or 'position > string.length()'.

    basic_string& replace(size_type        outPosition,
                          size_type        outNumChars,
                          const CHAR_TYPE *characterString,
                          size_type        numChars);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by a copy of the string constructed from the
        // specified 'numChars' characters in the array starting at the
        // specified 'characterString' address.  Return a reference to this
        // modifiable string.  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(size_type        outPosition,
                          size_type        outNumChars,
                          const CHAR_TYPE *characterString);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by the null-terminated specified
        // 'characterString' (of length
        // 'CHAR_TRAITS::length(characterString)').  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(size_type outPosition,
                          size_type outNumChars,
                          size_type numChars,
                          CHAR_TYPE character);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by a number equal to the specified 'numChars'
        // of copies of the specified 'character'.  Return a reference to this
        // modifiable string.  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(const_iterator      first,
                          const_iterator      last,
                          const basic_string& string);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by
        // the specified 'string'.  Return a reference to this modifiable
        // string.  The behavior is undefined unless 'first' and 'last' both
        // belong to '[cbegin(), cend()]' and 'first <= last'.

    basic_string& replace(const_iterator   first,
                          const_iterator   last,
                          const CHAR_TYPE *characterString,
                          size_type        numChars);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // copy of the string constructed from the specified 'numChars'
        // characters in the array starting at the specified 'characterString'
        // address.  Return a reference to this modifiable string.  The
        // behavior is undefined unless 'first' and 'last' both belong to
        // '[cbegin(), cend()]' and 'first <= last'.

    basic_string& replace(const_iterator   first,
                          const_iterator   last,
                          const CHAR_TYPE *characterString);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by
        // the null-terminated specified 'characterString'.  Return a reference
        // to this modifiable string.  The behavior is undefined unless 'first'
        // and 'last' both belong to the range '[cbegin(), cend()]'
        // and 'first <= last'.

    basic_string& replace(const_iterator first,
                          const_iterator last,
                          size_type      numChars,
                          CHAR_TYPE      character);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // number equal to the specified 'numChars' of copies of the specified
        // 'character'.  Return a reference to this modifiable string.  The
        // behavior is undefined unless 'first' and 'last' both belong to the
        // range '[cbegin(), cend()]' and 'first <= last'.

    template <typename INPUT_ITER>
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          INPUT_ITER     stringFirst,
                          INPUT_ITER     stringLast);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // string built from the characters in the range starting at the
        // specified 'stringFirst' and ending before the specified 'stringLast'
        // iterators of the 'iterator', 'const_iterator', or parameterized
        // 'INPUT_ITER' type, respectively.  Return a reference to this
        // modifiable string.  The behavior is undefined unless 'first' and
        // 'last' both belong to the range '[cbegin(), cend()]',
        // 'first <= last' and '[stringFirst, stringLast)' is a valid iterator
        // range.

    void swap(basic_string& other);
        // Exchange the value of this string with that of the specified
        // 'string', so that the value of this string upon return equals that
        // of 'other' prior to this call, and vice-versa.

    // ACCESSORS

                     // *** 21.3.3 iterators: ***

    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator pointing the first character in this
        // non-modifiable string (or the past-the-end iterator if this string
        // is empty).

    const_iterator end() const;
    const_iterator cend() const;
        // Return the past-the-end iterator for this non-modifiable string.

    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
        // Return a reverse iterator pointing the last character in this
        // non-modifiable string (or the past-the-end reverse iterator if this
        // string is empty).

    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
        // Return the past-the-end reverse iterator for this modifiable string.

                          // *** 21.3.4 capacity: ***

    size_type length() const;
        // Return the length of this string.  Note that this number may differ
        // from 'CHAR_TRAITS::length(c_str())' in case the string contains null
        // characters.  Also note that a null-terminating character added by
        // the 'c_str' method is *not* counted in this length.

    size_type size() const;
        // Return the length of this string.  Note that this number may differ
        // from 'CHAR_TRAITS::length(c_str())' in case the string contains null
        // characters.  Also note that a null-terminating character added by
        // the 'c_str' method is *not* counted in this length.

    size_type max_size() const;
        // Return the maximal possible length of this string.  Note that
        // requests to create a string longer than this number of characters is
        // guaranteed to raise a 'length_error' exception.

    size_type capacity() const;
        // Return the capacity of this string, i.e., the maximum length for
        // which resizing is guaranteed not to trigger a reallocation.

    bool empty() const;
        // Return 'true' if this string has length 0, and 'false' otherwise.

                       // *** 21.3.5 element access: ***

    const_reference operator[](size_type position) const;
        // Return a reference to the non-modifiable character at the specified
        // 'position' in this string.  The behavior is undefined unless
        // 'position <= length()'.  Note that if 'position == length()', a
        // reference to the null-terminating character is returned.

    const_reference at(size_type position) const;
        // Return a reference to the non-modifiable character at the specified
        // 'position' in this string.  Throw 'out_of_range' if
        // 'position >= length()'.

    const_reference front() const;
        // Return a reference to the non-modifiable character at the first
        // position in this string.  The behavior is undefined if this string
        // is empty.

    const_reference back() const;
        // Return a reference to the non-modifiable character at the last
        // position in this string.  The behavior is undefined if this string
        // is empty.  Note that the last position is 'length() - 1'.

    size_type copy(CHAR_TYPE *characterString,
                   size_type  numChars,
                   size_type  position = 0) const;
        // Copy the specified 'numChars' or 'length() - position', whichever is
        // smaller, characters from this string into the specified
        // 'characterString' buffer, and return the number of characters
        // copied.  Throw 'out_of_range' if 'position > length()'.  Note that
        // the output 'characterString' is *not* null-terminated.

                     // *** 21.3.7 string operations: ***

    const_pointer c_str() const;
        // Return a pointer to a null-terminated buffer of characters of length
        // equal to 'length() + 1' whose contents are identical to the value of
        // this string.  Note that any call to the string destructor or any of
        // its manipulators invalidates the returned pointer.

    const_pointer data() const;
        // Return a pointer to a null-terminated buffer of characters of length
        // equal to the value returned by the 'length' method, whose contents
        // are identical to the value of this string.  Note that any call to
        // the string destructor or any of its manipulators invalidates the
        // returned pointer.

    allocator_type get_allocator() const;
        // Return the allocator used by this string to supply memory.

    size_type find(const basic_string& string,
                   size_type           position = 0) const;
        // Return the starting position of the *first* occurrence of a
        // substring whose value equals that of the specified 'string', if such
        // a substring can be found in this string (on or *after* the
        // optionally specified 'position' if such a 'position' is specified),
        // and return 'npos' otherwise.

    size_type find(const CHAR_TYPE *string,
                   size_type        position,
                   size_type        numChars) const;
    size_type find(const CHAR_TYPE *string,
                   size_type        position = 0) const;
        // Return the starting position of the *first* occurrence of a
        // substring whose value equals that of the specified 'characterString'
        // of the optionally specified 'numChars' length, if such a substring
        // can be found in this string (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.  If 'numChars' is not specified,
        // 'CHAR_TRAITS::length(characterString)' is used.

    size_type find(CHAR_TYPE character, size_type position = 0) const;
        // Return the position of the *first* occurrence of the specified
        // 'character', if such an occurrence can be found in this string (on
        // or *after* the optionally specified 'position' if such a 'position'
        // is specified), and return 'npos' otherwise.

    size_type rfind(const basic_string& string,
                    size_type           position = npos) const;
        // Return the starting position of the *last* occurrence of a substring
        // whose value equals that of the specified 'string', if such a
        // substring can be found in this string (on or *before* the optionally
        // specified 'position' if such a 'position' is specified), and return
        // 'npos' otherwise.

    size_type rfind(const CHAR_TYPE *characterString,
                    size_type        position,
                    size_type        numChars) const;
    size_type rfind(const CHAR_TYPE *characterString,
                    size_type        position = npos) const;
        // Return the starting position of the *last* occurrence of a substring
        // whose value equals that of the specified 'characterString' of the
        // optionally specified 'numChars' length, if such a substring can be
        // found in this string (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.  If 'numChars' is not specified,
        // 'CHAR_TRAITS::length(characterString)' is used.

    size_type rfind(CHAR_TYPE character, size_type position = npos) const;
        // Return the position of the *last* occurrence of the specified
        // 'character', if such an occurrence can be found in this string (on
        // or *before* the optionally specified 'position' if such a 'position'
        // is specified), and return 'npos' otherwise.

    size_type find_first_of(const basic_string& string,
                            size_type           position = 0) const;
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'string', if such an occurrence can be
        // found in this string (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    size_type find_first_of(const CHAR_TYPE *characterString,
                            size_type        position,
                            size_type        numChars) const;
    size_type find_first_of(const CHAR_TYPE *characterString,
                            size_type        position = 0) const;
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'characterString' of the optionally
        // specified 'numChars' length, if such an occurrence can be found in
        // this string (on or *after* the optionally specified 'position' if
        // such a 'position' is specified), and return 'npos' otherwise.  If
        // 'numChars' is not specified, 'CHAR_TRAITS::length(characterString)'
        // is used.

    size_type find_first_of(CHAR_TYPE character,
                            size_type position = 0) const;
        // Return the position of the *first* occurrence of the specified
        // 'character', if such an occurrence can be found in this string (on
        // or *after* the optionally specified 'position' if such a 'position'
        // is specified), and return 'npos' otherwise.

    size_type find_last_of(const basic_string& string,
                           size_type           position = npos) const;
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'string', if such an occurrence can be
        // found in this string (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    size_type find_last_of(const CHAR_TYPE *characterString,
                           size_type        position,
                           size_type        numChars) const;
    size_type find_last_of(const CHAR_TYPE *characterString,
                           size_type        position = npos) const;
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'characterString' of the optionally
        // specified 'numChars' length, if such an occurrence can be found in
        // this string (on or *before* the optionally specified 'position' if
        // such a 'position' is specified), and return 'npos' otherwise.  If
        // 'numChars' is not specified, 'CHAR_TRAITS::length(characterString)'
        // is used.

    size_type find_last_of(CHAR_TYPE character,
                           size_type position = npos) const;
        // Return the position of the *last* occurrence of the specified
        // 'character', if such an occurrence can be found in this string (on
        // or *before* the optionally specified 'position' if such a 'position'
        // is specified), and return 'npos' otherwise.

    size_type find_first_not_of(const basic_string& string,
                                size_type            position = 0) const;
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'string', if such an occurrence can be
        // found in this string (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    size_type find_first_not_of(const CHAR_TYPE *characterString,
                                size_type        position,
                                size_type        numChars) const;
    size_type find_first_not_of(const CHAR_TYPE *characterString,
                                size_type        position = 0) const;
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'characterString' of the optionally
        // specified 'numChars' length, if such an occurrence can be found in
        // this string (on or *after* the optionally specified 'position' if
        // such a 'position' is specified), and return 'npos' otherwise.  If
        // 'numChars' is not specified, 'CHAR_TRAITS::length(characterString)'
        // is used.

    size_type find_first_not_of(CHAR_TYPE character,
                                size_type position = 0) const;
        // Return the position of the *first* occurrence of a character
        // *different* from the specified 'character', if such an occurrence
        // can be found in this string (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    size_type find_last_not_of(const basic_string& string,
                               size_type           position = npos) const;
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'string', if such an occurrence can be
        // found in this string (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    size_type find_last_not_of(const CHAR_TYPE *characterString,
                               size_type        position,
                               size_type        numChars) const;
    size_type find_last_not_of(const CHAR_TYPE *characterString,
                               size_type        position = npos) const;
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'characterString' of the optionally
        // specified 'numChars' length, if such an occurrence can be found in
        // this string (on or *before* the optionally specified 'position' if
        // such a 'position' is specified), and return 'npos' otherwise.  If
        // 'numChars' is not specified, 'CHAR_TRAITS::length(characterString)'
        // is used.

    size_type find_last_not_of(CHAR_TYPE character,
                               size_type position = npos) const;
        // Return the position of the *last* occurrence of a character
        // *different* from the specified 'character', if such an occurrence
        // can be found in this string (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    basic_string substr(size_type position = 0,
                        size_type numChars = npos) const;
        // Return a string whose value is the substring of length the
        // optionally specified 'numChars' or 'length() - position', whichever
        // is smaller, starting at the optionally specified 'position' in this
        // string.  If 'position' is not specified, 0 is used (i.e., the
        // substring is from the beginning of this string).  If 'numChars' is
        // not specified, 'npos' is used (i.e., the entire suffix from
        // 'position' to the end of the string is returned).

    int compare(const basic_string& other) const;
        // Lexicographically compare this string with the specified 'other'
        // string (using 'CHAR_TRAITS::lt' to compare characters), and return a
        // negative value if this string is less than 'other', a positive value
        // if it is more than 'other', and 0 in case of equality.  See
        // "Lexicographical Comparisons" for definitions.

    int compare(size_type           position,
                size_type           numChars,
                const basic_string& other) const;
        // Lexicographically compare the substring of this string starting at
        // the specified 'lhsPosition' of length 'lhsNumChars' or
        // 'length() - lhsPosition', whichever is smaller, with the specified
        // 'other' string (using 'CHAR_TRAITS::lt' to compare characters), and
        // return a negative value if this string is less than 'other', a
        // positive value if it is more than 'other', and 0 in case of
        // equality.  See "Lexicographical Comparisons" for definitions.

    int compare(size_type           lhsPosition,
                size_type           lhsNumChars,
                const basic_string& other,
                size_type           otherPosition,
                size_type           otherNumChars) const;
        // Lexicographically compare the substring of this string starting at
        // the specified 'lhsPosition' of length 'lhsNumChars' or
        // 'length() - lhsPosition', whichever is smaller, with the substring
        // of the specified 'other' string starting at the specified
        // 'otherPosition' of length 'otherNumChars' or
        // 'length() - otherPosition', whichever is smaller, (using
        // 'CHAR_TRAITS::lt' to compare characters), and return a negative
        // value if this string is less than 'other', a positive value if it is
        // more than 'other', and 0 in case of equality.  Throw 'out_of_range'
        // if 'lhsPosition > length()' or 'otherPosition > other.length()'.
        // See "Lexicographical Comparisons" for definitions.

    int compare(const CHAR_TYPE *other) const;
        // Lexicographically compare this string with the null-terminated
        // specified 'other' string (of length 'CHAR_TRAITS::length(other)',
        // using 'CHAR_TRAITS::lt' to compare characters), and return a
        // negative value if this string is less than 'other', a positive value
        // if it is more than 'other', and 0 in case of equality.  See
        // "Lexicographical Comparisons" for definitions.

    int compare(size_type        lhsPosition,
                size_type        lhsNumChars,
                const CHAR_TYPE *other,
                size_type        otherNumChars) const;
        // Lexicographically compare the substring of this string starting at
        // the specified 'lhsPosition' of length 'lhsNumChars' or
        // 'length() - lhsPosition', whichever is smaller, with the string
        // constructed from the specified 'numChars' characters in the array
        // starting at the specified 'characterString' address, and return a
        // negative value if this string is less than 'other', a positive value
        // if it is more than 'other', and 0 in case of equality.  Throw
        // 'out_of_range' if 'lhsPosition > length()'.  See "Lexicographical
        // Comparisons" for definitions.

    int compare(size_type        lhsPosition,
                size_type        lhsNumChars,
                const CHAR_TYPE *other) const;
        // Lexicographically compare the substring of this string starting at
        // the specified 'lhsPosition' of length 'lhsNumChars' or
        // 'length() - lhsPosition', whichever is smaller, with the
        // null-terminated specified 'other' string (of length
        // 'CHAR_TRAITS::length(other)', using 'CHAR_TRAITS::lt' to compare
        // characters), and return a negative value if this string is less than
        // 'other', a positive value if it is more than 'other', and 0 in case
        // of equality.  Throw 'out_of_range' if 'lhsPosition > length()'.  See
        // "Lexicographical Comparisons" for definitions.
};

// TYPEDEFS
typedef basic_string<char>    string;
typedef basic_string<wchar_t> wstring;

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator==(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator==(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator==(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs);
    // Return 'true' if the specified 'lhs' string has the same value as the
    // specified 'rhs' string, and 'false' otherwise.  Two strings have the
    // same value if they have the same length, and the characters at each
    // respective position have the same value according to 'CHAR_TRAITS::eq'.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator!=(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator!=(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator!=(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs);
    // Return 'true' if the specified 'lhs' string has a different value from
    // the specified 'rhs' string, and 'false' otherwise.  Two strings have the
    // same value if they have the same length, and the characters at each
    // respective position have the same value according to 'CHAR_TRAITS::eq'.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator<(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
          const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator<(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
          const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const CHAR_TYPE                                  *lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const CHAR_TYPE                                  *rhs);
    // Return 'true' if the specified 'lhs' string has a lexicographically
    // smaller value than the specified 'rhs' string, and 'false' otherwise.
    // See "Lexicographical Comparisons" for definitions.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator>(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
          const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator>(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
          const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator>(const CHAR_TYPE                                  *lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const CHAR_TYPE                                  *rhs);
    // Return 'true' if the specified 'lhs' string has a lexicographically
    // larger value than the specified 'rhs' string, and 'false' otherwise.
    // See "Lexicographical Comparisons" for definitions.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator<=(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator<=(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<=(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs);
    // Return 'true' if the specified 'lhs' string has a value
    // lexicographically smaller than or or equal to the specified 'rhs'
    // string, and 'false' otherwise.  See "Lexicographical Comparisons" for
    // definitions.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator>=(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator>=(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator>=(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs);
    // Return 'true' if the specified 'lhs' string has a value
    // lexicographically larger than or equal to the specified 'rhs' string,
    // and 'false' otherwise.  See "Lexicographical Comparisons" for
    // definitions.

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>
operator+(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
          const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>
operator+(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
          const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs);
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const CHAR_TYPE                                      *lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  rhs);
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(CHAR_TYPE                                             lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  rhs);
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  lhs,
          const CHAR_TYPE                                      *rhs);
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  lhs,
          CHAR_TYPE                                             rhs);
    // Return the concatenation of strings constructed from the specified 'lhs'
    // and 'rhs' arguments, i.e., 'basic_string(lhs).append(rhs)'.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&          os,
           const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str);
    // Write the string specified by 'str' into the output stream specified by
    // 'os' and return 'os'.  If the string is shorter than 'os.width()', then
    // it is padded to 'os.width()' with the current 'os.fill()' character.
    // The padding, if any, is output after the string (on the right) if
    // 'os.flags() | ios::left' is non-zero and before the string otherwise.
    // This function will do nothing unless 'os.good()' is true on entry.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&
operator>>(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&     is,
           basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& str);
    // Replace the contents of the specified 'str' string with a word read from
    // the specified 'is' input stream and return 'is'.  The word begins at the
    // first non-whitespace character on the input stream and ends when another
    // whitespace character (or eof) is found.  The trailing whitespace
    // character is left on the input stream.  If 'is.good()' is not true on
    // entry or if eof is found before any non-whitespace characters, then
    // 'str' is unchanged and 'is.fail()' is becomes true.  If eof is detected
    // after some characters have been read into 'str', then 'is.eof()' becomes
    // true, but 'is.fail()' does not.

// FREE FUNCTIONS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void swap(basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& lhs,
          basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& rhs);
    // Exchange the values of the specified 'lhs' and 'rhs' strings in constant
    // time without throwing an exception.  The behavior is undefined unless
    // 'lhs.get_allocator() == rhs.get_allocator()'.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&
getline(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&     is,
        basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& str,
        CHAR_TYPE                                       delim);
    // Replace the contents of the specified 'str' string by extracting
    // characters from the specified 'is' stream until the specified 'delim'
    // character is extracted and return 'is'.  The 'delim' character is
    // removed from the input stream but is not appended to 'str'.  If an 'eof'
    // is detected before 'delim', then the characters up to the 'eof' are put
    // into 'str' and 'is.eof()' becomes true.  If 'is.good()' is false on
    // entry, then do nothing, otherwise if no characters are extracted (e.g.,
    // because because the stream is at eof), 'str' will become empty and
    // 'is.fail()' will become true.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&
getline(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&     is,
        basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& str);
    // Replace the contents of the specified 'str' string by extracting
    // characters from the specified 'is' stream until a newline character
    // (is.widen('\n') is extracted and return 'is'.  The newline character is
    // removed from the input stream but is not appended to 'str'.  If an 'eof'
    // is detected before the newline, then the characters up to the 'eof' are
    // put into 'str' and 'is.eof()' becomes true.  If 'is.good()' is false on
    // entry, then do nothing, otherwise if no characters are extracted (e.g.,
    // because because the stream is at eof), 'str' will become empty and
    // 'is.fail()' will become true.

// HASH SPECIALIZATIONS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::size_t
hashBasicString(const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str);
    // Return a hash value for the specified 'str'.

std::size_t hashBasicString(const string& str);
    // Return a hash value for the specified 'str'.

std::size_t hashBasicString(const wstring& str);
    // Return a hash value for the specified 'str'.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
struct hash<basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> >
    // Specialization of 'hash' for 'basic_string'.
{
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(hash,
                                 BloombergLP::bslalg_TypeTraitBitwiseCopyable);

    std::size_t operator()(const basic_string<CHAR_TYPE,
                                            CHAR_TRAITS, ALLOCATOR>& str) const
        // Return a hash value computed using the specified 'str' value.
    {
        return hashBasicString(str);
    }
};

// ==========================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================
// See IMPLEMENTATION NOTES in the '.cpp' before modifying anything below.

                          // ----------------
                          // class String_Imp
                          // ----------------

// CLASS METHODS
template <typename CHAR_TYPE, typename SIZE_TYPE>
SIZE_TYPE
String_Imp<CHAR_TYPE, SIZE_TYPE>::computeNewCapacity(SIZE_TYPE newLength,
                                                     SIZE_TYPE oldCapacity,
                                                     SIZE_TYPE maxSize)
{
    BSLS_ASSERT_SAFE(newLength >= oldCapacity);

    SIZE_TYPE newCapacity = oldCapacity + (oldCapacity >> 1);
        // We must exercise an exponential growth, otherwise we cannot
        // guarantee amortized time for 'append', 'insert', 'push_back',
        // 'replace', etc.  1.5 growth factor helps to reuse previously
        // allocated and freed memory blocks on frequent re-allocations due to
        // a continuous string growth (for example, when calling 'push_back' in
        // a loop).
        //
        // TBD: consider bounding the exponential growth when 'newCapacity' is
        // about several megabytes.

    if (newLength > newCapacity) {
        newCapacity = newLength;
    }

    if (newCapacity < oldCapacity || newCapacity > maxSize) {  // overflow
        newCapacity = maxSize;
    }

    return newCapacity;
}

// CREATORS
template <typename CHAR_TYPE, typename SIZE_TYPE>
String_Imp<CHAR_TYPE, SIZE_TYPE>::String_Imp()
: d_start_p(0)
, d_length(0)
, d_capacity(SHORT_BUFFER_CAPACITY)
{
}

template <typename CHAR_TYPE, typename SIZE_TYPE>
String_Imp<CHAR_TYPE, SIZE_TYPE>::String_Imp(SIZE_TYPE length,
                                             SIZE_TYPE capacity)
: d_start_p(0)
, d_length(length)
, d_capacity(capacity <= static_cast<SIZE_TYPE>(SHORT_BUFFER_CAPACITY)
                      ?  static_cast<SIZE_TYPE>(SHORT_BUFFER_CAPACITY)
                      :  capacity)
{
}

// MANIPULATORS
template <typename CHAR_TYPE, typename SIZE_TYPE>
void String_Imp<CHAR_TYPE, SIZE_TYPE>::swap(String_Imp& other)
{
    if (!isShortString() && !other.isShortString()) {
        // If both strings are long, swap the individual fields.
        std::swap(d_length,   other.d_length);
        std::swap(d_capacity, other.d_capacity);
        std::swap(d_start_p,  other.d_start_p);
    }
    else {
        // Otherwise bitwise-swap the whole objects (relies on the
        // BitwiseMoveable type trait).
        BloombergLP::bslalg_ScalarPrimitives::swap(*this, other);
    }
}

// PRIVATE MANIPULATORS
template <typename CHAR_TYPE, typename SIZE_TYPE>
inline
void String_Imp<CHAR_TYPE, SIZE_TYPE>::resetFields()
{
    d_start_p  = 0;
    d_length   = 0;
    d_capacity = SHORT_BUFFER_CAPACITY;
}

template <typename CHAR_TYPE, typename SIZE_TYPE>
inline
CHAR_TYPE *String_Imp<CHAR_TYPE, SIZE_TYPE>::dataPtr()
{
    return isShortString()
           ? reinterpret_cast<CHAR_TYPE *>((void *)d_short.buffer())
           : d_start_p;
}

// PRIVATE ACCESSORS
template <typename CHAR_TYPE, typename SIZE_TYPE>
inline
bool String_Imp<CHAR_TYPE, SIZE_TYPE>::isShortString() const
{
    return d_capacity == SHORT_BUFFER_CAPACITY;
}

template <typename CHAR_TYPE, typename SIZE_TYPE>
inline
const CHAR_TYPE *String_Imp<CHAR_TYPE, SIZE_TYPE>::dataPtr() const
{
    return isShortString()
          ? reinterpret_cast<const CHAR_TYPE *>((const void *)d_short.buffer())
          : d_start_p;
}

                        // -----------------------
                        // class bsl::basic_string
                        // -----------------------

// CLASS DATA
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
const typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::npos;

// PRIVATE MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
CHAR_TYPE *
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAllocate(
                                                            size_type numChars)
{
    return this->allocateN((CHAR_TYPE *)0, numChars + 1);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateDeallocate()
{
    if (!this->isShortString()) {
        this->deallocateN(this->d_start_p, this->d_capacity + 1);
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateCopy(
                                                  const basic_string& original)
{
    // Reinitialize String_Imp in case we're going from long to short.
    static_cast<Imp &>(*this) = Imp(original.length(), original.length());

    if (!this->isShortString()) {
        this->d_start_p = privateAllocate(this->d_capacity);
    }

    CHAR_TRAITS::copy(this->dataPtr(), original.data(), this->d_length + 1);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendDispatch(
                                                                iterator begin,
                                                                iterator end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    return privateAppendDispatch(const_iterator(begin), const_iterator(end));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendDispatch(
                                                          const_iterator begin,
                                                          const_iterator end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    size_type numChars = end - begin;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                            "string<...>::append<Iter>(i,j): string too long");
    }
    return privateAppendRaw(&*begin, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendDispatch(
                                                              INPUT_ITER begin,
                                                              INPUT_ITER end)
{
    return privateReplaceDispatch(length(),
                                  size_type(0),
                                  begin,
                                  end,
                                  begin,
                                  end);  // disambiguate overloads
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendRaw(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(length() <= max_size() - numChars);

    size_type  newLength  = this->d_length + numChars;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              this->d_length);

    if (newBuffer) {
        CHAR_TRAITS::copy(newBuffer + this->d_length,
                          characterString,
                          numChars);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        privateDeallocate();

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
    else {
        CHAR_TRAITS::move(this->dataPtr() + length(),
                          characterString,
                          numChars);
        CHAR_TRAITS::assign(*(this->dataPtr() + newLength), CHAR_TYPE());
    }

    this->d_length = newLength;
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendRaw(
                                                           size_type numChars,
                                                           CHAR_TYPE character)
{
    BSLS_ASSERT_SAFE(length() <= max_size() - numChars);

    size_type newLength = this->d_length + numChars;
    privateReserveRaw(newLength);
    CHAR_TRAITS::assign(this->dataPtr() + this->d_length, numChars, character);
    this->d_length = newLength;
    CHAR_TRAITS::assign(*(this->dataPtr() + newLength), CHAR_TYPE());
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::Imp&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateBase()
{
    return *static_cast<Imp *>(this);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateClear(
                                                     bool deallocateBufferFlag)
{
    if (deallocateBufferFlag) {
        privateDeallocate();
        this->resetFields();
    }
    else {
        this->d_length = 0;
    }

    CHAR_TRAITS::assign(*begin(), CHAR_TYPE());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                                iterator begin,
                                                                iterator end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    privateInitDispatch((const_iterator)begin, (const_iterator)end);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                        const_iterator   begin,
                                                        const_iterator   end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    size_type numChars = end - begin;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                          "string<...>(i,j): string too long");
    }
    privateAppendRaw(&*begin, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                              INPUT_ITER begin,
                                                              INPUT_ITER end)
{
    privateReplaceDispatch((size_type)0,
                           (size_type)0,
                           begin,
                           end,
                           begin,
                           end);  // disambiguate overloads
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                       const_iterator position,
                                                       iterator       first,
                                                       iterator       last)
{
    BSLS_ASSERT_SAFE(first <= last);

    privateInsertDispatch(position,
                          const_iterator(first),
                          const_iterator(last));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                       const_iterator position,
                                                       const_iterator first,
                                                       const_iterator last)
{
    BSLS_ASSERT_SAFE(first <= last);

    size_type pos = position - cbegin();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                       "string<...>::insert<Iter>(pos,i,j): invalid position");
    }
    size_type numChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                        "string<...>::insert<Iter>(pos,i,j): string too long");
    }
    privateInsertRaw(pos, &*first, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                       const_iterator position,
                                                       INPUT_ITER     first,
                                                       INPUT_ITER     last)
{
    size_type pos = position - cbegin();
    privateReplaceDispatch(pos, size_type(0), first, last, first, last);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertRaw(
                                              size_type        outPosition,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(outPosition <= length());
    BSLS_ASSERT_SAFE(numChars <= max_size() - length());
    BSLS_ASSERT_SAFE(characterString);

    size_type  newLength  = this->d_length + numChars;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    if (newBuffer) {
        // Source and destination cannot overlap, order of next two copies is
        // arbitrary.  Do it left to right to maintain cache consistency.

        const CHAR_TYPE *tail    = this->dataPtr() + outPosition;
        size_type        tailLen = this->d_length - outPosition;

        CHAR_TRAITS::copy(newBuffer + outPosition, characterString, numChars);
        CHAR_TRAITS::copy(newBuffer + outPosition + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        privateDeallocate();

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
    else {
        // Because of possible aliasing, we have to be very careful in which
        // order to move blocks.  If 'characterString' overlaps with tail, or
        // is entirely contained: in the former case, 'characterString' is
        // shifted by 'numChars' (takes 'first' onto 'last'); in the latter,
        // the tail moves in by 'numChars', so cannot overwrite
        // 'characterString'!

        const CHAR_TYPE *first   = characterString;
        const CHAR_TYPE *last    = characterString + numChars;
        CHAR_TYPE       *tail    = this->dataPtr() + outPosition;
        size_type        tailLen = this->d_length - outPosition;
        const CHAR_TYPE *shifted = (tail < first && last <= tail + tailLen)
                                 ? last    // 'first' shifted by 'numChars'
                                 : first;  // 'no shift

        CHAR_TRAITS::move(tail + numChars, tail, tailLen);
        CHAR_TRAITS::move(tail, shifted, numChars);
        CHAR_TRAITS::assign(*(this->dataPtr() + newLength), CHAR_TYPE());
    }

    this->d_length = newLength;
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceRaw(
                                              size_type        outPosition,
                                              size_type        outNumChars,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(outPosition <= length());
    BSLS_ASSERT_SAFE(outNumChars <= length());
    BSLS_ASSERT_SAFE(outPosition <= length() - outNumChars);
    BSLS_ASSERT_SAFE(length() - outNumChars <= max_size() - numChars);
    BSLS_ASSERT_SAFE(characterString);

    const difference_type displacement =
                          static_cast<difference_type>(numChars - outNumChars);

    size_type  newLength  = this->d_length + displacement;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    const CHAR_TYPE *tail = this->dataPtr() + outPosition + outNumChars;
    size_type tailLen = this->d_length - outPosition - outNumChars;

    if (newBuffer) {
        // Source and destination cannot overlap, order of next two copies is
        // arbitrary.  Do it left to right to maintain cache consistency.

        CHAR_TRAITS::copy(newBuffer + outPosition, characterString, numChars);
        CHAR_TRAITS::copy(newBuffer + outPosition + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        privateDeallocate();

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
        this->d_length = newLength;
        return *this;                                                 // RETURN
    }

    // Because of possible aliasing, we have to be very careful in which order
    // to move blocks.  There are up to three blocks if 'characterString'
    // overlaps with the tail.

    CHAR_TYPE       *dest  = this->dataPtr() + outPosition;
    const CHAR_TYPE *first = characterString;
    const CHAR_TYPE *last  = characterString + numChars;

    if (tail < last && last <= tail + tailLen) {
        // Either 'characterString' overlaps with tail, or is entirely
        // contained.

        if (first < tail) {
            // Not entirely contained: break '[first, last)' at 'tail', and
            // move it in two steps, the second shifted but not the first.

            size_type prefix = tail - first, suffix = last - tail;
            if (outNumChars < numChars) {
                CHAR_TRAITS::move(dest + numChars, tail, tailLen);
                CHAR_TRAITS::move(dest, first, prefix);
            }
            else {
                CHAR_TRAITS::move(dest, first, prefix);
                CHAR_TRAITS::move(dest + numChars, tail, tailLen);
            }
            CHAR_TRAITS::move(dest + prefix,
                              last - suffix + displacement,
                              suffix);
        }
        else {
            // Entirely contained: copy 'tail' first, and copy '[first, last)'
            // shifted by 'displacement'.

            CHAR_TRAITS::move(dest + numChars, tail, tailLen);
            CHAR_TRAITS::copy(dest, first + displacement, numChars);
        }
    }
    else {
        // Note: no aliasing in tail.

        if (outNumChars < numChars) {
            CHAR_TRAITS::move(dest + numChars, tail, tailLen);
            CHAR_TRAITS::move(dest, characterString, numChars);
        }
        else {
            CHAR_TRAITS::move(dest, characterString, numChars);
            CHAR_TRAITS::move(dest + numChars, tail, tailLen);
        }
    }
    CHAR_TRAITS::assign(*(this->dataPtr() + newLength), CHAR_TYPE());
    this->d_length = newLength;
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceRaw(
                                                         size_type outPosition,
                                                         size_type outNumChars,
                                                         size_type numChars,
                                                         CHAR_TYPE character)
{
    BSLS_ASSERT_SAFE(outPosition <= length());
    BSLS_ASSERT_SAFE(outNumChars <= length());
    BSLS_ASSERT_SAFE(outPosition <= length() - outNumChars);
    BSLS_ASSERT_SAFE(length() <= max_size() - numChars);

    size_type  newLength  = this->d_length - outNumChars + numChars;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    const CHAR_TYPE *tail = this->dataPtr() + outPosition + outNumChars;
    size_type tailLen = this->d_length - outPosition - outNumChars;

    if (newBuffer) {
        CHAR_TYPE *dest = newBuffer + outPosition;

        CHAR_TRAITS::assign(dest, numChars, character);
        CHAR_TRAITS::copy(dest + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        privateDeallocate();

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
    else {
        CHAR_TYPE *dest = this->dataPtr() + outPosition;

        CHAR_TRAITS::move(dest + numChars, tail, tailLen);
        CHAR_TRAITS::assign(dest, numChars, character);
        CHAR_TRAITS::assign(*(this->dataPtr() + newLength), CHAR_TYPE());
    }

    this->d_length = newLength;
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceDispatch(
                                         size_type                position,
                                         size_type                numChars,
                                         INPUT_ITER               first,
                                         INPUT_ITER               last,
                                         BloombergLP::bslstl_UtilIterator,
                                         int)
{
    return replace(position, numChars, (size_type)first, (CHAR_TYPE)last);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceDispatch(
                                                  size_type  position,
                                                  size_type  numChars,
                                                  INPUT_ITER first,
                                                  INPUT_ITER last,
                                                  BloombergLP::bslmf_AnyType,
                                                  BloombergLP::bslmf_AnyType)
{
    typename iterator_traits<INPUT_ITER>::iterator_category tag;
    return privateReplace(position, numChars, first, last, tag);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplace(
                                                   size_type  outPosition,
                                                   size_type  outNumChars,
                                                   INPUT_ITER first,
                                                   INPUT_ITER last,
                                                   std::input_iterator_tag)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < outPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                 "string<...>::replace<InputIter>(pos,i,j): invalid position");
    }
    basic_string temp(this->get_allocator());
    for (; first != last; ++first) {
        temp.push_back(*first);
    }
    if (outPosition == 0 && length() <= outNumChars) {
        // Note: can potentially shrink the capacity, hence the reserve.

        temp.privateReserveRaw(capacity());
        privateBase().swap(temp.privateBase());
        return *this;                                                 // RETURN
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             temp.data(),
                             temp.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplace(
                                                 size_type  outPosition,
                                                 size_type  outNumChars,
                                                 INPUT_ITER first,
                                                 INPUT_ITER last,
                                                 std::forward_iterator_tag)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < outPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                      "string<...>::replace<Iter>(pos,i,j): invalid position");
    }
    size_type numChars = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                           max_size() - (length() - outPosition) < numChars)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                     "string<...>::replace<Iter>(pos,n,i,j): string too long");
    }

    // Create a temp string because the 'first'/'last' iterator pair can alias
    // the current string; not using ctor with two iterators because it
    // recurses back here.
    basic_string temp(numChars, CHAR_TYPE());

    for (size_type pos = 0; pos != numChars; ++first, ++pos) {
        temp[pos] = *first;
    }

    return privateReplaceRaw(outPosition,
                             outNumChars,
                             temp.data(),
                             temp.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplace(
                                                 size_type      outPosition,
                                                 size_type      outNumChars,
                                                 const_iterator first,
                                                 const_iterator last,
                                                 std::forward_iterator_tag)
{
    BSLS_ASSERT_SAFE(first <= last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < outPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                      "string<...>::replace<Iter>(pos,i,j): invalid position");
    }
    size_type numChars = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                           max_size() - (length() - outPosition) < numChars)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                     "string<...>::replace<Iter>(pos,n,i,j): string too long");
    }

    return privateReplaceRaw(outPosition, outNumChars, &*first, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplace(
                                                 size_type      outPosition,
                                                 size_type      outNumChars,
                                                 iterator       first,
                                                 iterator       last,
                                                 std::forward_iterator_tag)
{
    BSLS_ASSERT_SAFE(first <= last);

    return privateReplace(outPosition,
                          outNumChars,
                          const_iterator(first),
                          const_iterator(last),
                          std::forward_iterator_tag());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReserveRaw(
                                                         size_type newCapacity)
{
    BSLS_ASSERT_SAFE(newCapacity <= max_size());

    if (this->d_capacity < newCapacity) {
        size_type newStorage = this->computeNewCapacity(newCapacity,
                                                        this->d_capacity,
                                                        max_size());
        CHAR_TYPE *newBuffer = privateAllocate(newStorage);

        CHAR_TRAITS::copy(newBuffer, this->dataPtr(), this->d_length + 1);

        privateDeallocate();

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
CHAR_TYPE *
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReserveRaw(
                                                        size_type *storage,
                                                        size_type  newCapacity,
                                                        size_type  numChars)
{
    BSLS_ASSERT_SAFE(numChars <= length());
    BSLS_ASSERT_SAFE(newCapacity <= max_size());
    BSLS_ASSERT_SAFE(storage != 0);

    if (*storage >= newCapacity) {
        return 0;                                                     // RETURN
    }

    *storage = this->computeNewCapacity(newCapacity,
                                        *storage,
                                        max_size());

    CHAR_TYPE *newBuffer = privateAllocate(*storage);

    CHAR_TRAITS::copy(newBuffer, this->dataPtr(), numChars);
    return newBuffer;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateResizeRaw(
                                                           size_type newLength,
                                                           CHAR_TYPE character)
{
    BSLS_ASSERT_SAFE(newLength <= max_size());

    privateReserveRaw(newLength);

    if (newLength > this->d_length) {
        CHAR_TRAITS::assign(this->dataPtr() + this->d_length,
                            newLength - this->d_length,
                            character);
    }
    this->d_length = newLength;
    CHAR_TRAITS::assign(*(this->dataPtr() + this->d_length), CHAR_TYPE());
    return *this;
}

// PRIVATE ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateCompareRaw(
                                          size_type        lhsPosition,
                                          size_type        lhsNumChars,
                                          const CHAR_TYPE *other,
                                          size_type        otherNumChars) const
{
    BSLS_ASSERT_SAFE(lhsPosition <= length());
    BSLS_ASSERT_SAFE(lhsNumChars <= length());
    BSLS_ASSERT_SAFE(lhsPosition <= length() - lhsNumChars);
    BSLS_ASSERT_SAFE(other);

    size_type numChars = lhsNumChars < otherNumChars ? lhsNumChars
                                                     : otherNumChars;
    int cmpResult = CHAR_TRAITS::compare(this->dataPtr() + lhsPosition,
                                         other,
                                         numChars);
    if (cmpResult) {
        return cmpResult;                                             // RETURN
    }
    if (lhsNumChars < otherNumChars) {
        return -1;                                                    // RETURN
    }
    if (lhsNumChars > otherNumChars) {
        return 1;                                                     // RETURN
    }
    return 0;
}

// CREATORS

                // *** 21.3.2 construct/copy/destroy: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                    const ALLOCATOR& allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    CHAR_TRAITS::assign(*begin(), CHAR_TYPE());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                 const basic_string& original)
: Imp(original)
, BloombergLP::bslstl_ContainerBase<allocator_type>(ALLOCATOR())
{
    if (!this->isShortString()) {
        // Copy long string to either short or long.
        privateCopy(original);
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                 const basic_string& original,
                                                 const ALLOCATOR&    allocator)
: Imp(original)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    if (!this->isShortString()) {
        // Copy long string to either short or long.
        privateCopy(original);
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                 const basic_string& original,
                                                 size_type           position,
                                                 size_type           numChars,
                                                 const ALLOCATOR&    allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    assign(original, position, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                             const CHAR_TYPE  *characterString,
                                             const ALLOCATOR&  allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    BSLS_ASSERT_SAFE(characterString);

    assign(characterString);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                             const CHAR_TYPE  *characterString,
                                             size_type         numChars,
                                             const ALLOCATOR&  allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    BSLS_ASSERT_SAFE(characterString);

    assign(characterString, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                   size_type         numChars,
                                                   CHAR_TYPE         character,
                                                   const ALLOCATOR&  allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    assign(numChars, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                    INPUT_ITER       first,
                                                    INPUT_ITER       last,
                                                    const ALLOCATOR& allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    privateInitDispatch(first, last);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename ALLOC2>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
    const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& original,
    const ALLOCATOR&                                              allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    this->assign(original.data(), original.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
    const BloombergLP::bslstl_StringRefData<CHAR_TYPE>& strRef,
    const ALLOCATOR&                                    allocator)
: Imp()
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    assign(strRef.begin(), strRef.end());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::~basic_string()
{
    privateDeallocate();
    this->d_length = npos;  // invalid length
}

// MANIPULATORS

                // *** 21.3.2 construct/copy/destroy: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator=(
                                                      const basic_string& rhs)
{
    return assign(rhs, size_type(0), npos);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator=(const CHAR_TYPE *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return assign(rhs);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator=(CHAR_TYPE character)
{
    return assign(1, character);
}

                      // *** 21.3.4 capacity: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::resize(size_type newLength,
                                                           CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newLength > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                  "string<...>::resize(n,c): string too long");
    }
    privateResizeRaw(newLength, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::resize(size_type newLength)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newLength > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                    "string<...>::resize(n): string too long");
    }
    privateResizeRaw(newLength, CHAR_TYPE());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reserve(
                                                         size_type newCapacity)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newCapacity > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                   "string<...>::reserve(n): string too long");
    }
    privateReserveRaw(newCapacity);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::clear()
{
    // Note: Stlport and Dinkumware do not deallocate the allocated buffer in
    // long string representation, ApacheSTL does.

    privateClear(Imp::BASIC_STRING_DEALLOCATE_IN_CLEAR);
}

                      // *** 21.3.3 iterators: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::begin()
{
    return this->dataPtr();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::end()
{
    return begin() + this->d_length;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rend()
{
    return reverse_iterator(begin());
}

                   // *** 21.3.5 element access: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator[](size_type position)
{
    BSLS_ASSERT_SAFE(position <= length());

    return *(begin() + position);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::at(size_type position)
{
    // Note: deliberately not inline, because 1) this is not a very widely used
    // function, and 2) it is very convenient to have at least one non-inlined
    // element accessor for debugging.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                                       "string<...>::at(n): invalid position");
    }
    return *(begin() + position);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::front()
{
    BSLS_ASSERT_SAFE(!empty());

    return *begin();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::back()
{
    BSLS_ASSERT_SAFE(!empty());

    return *(begin() + length() - 1);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename ALLOC2>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::
    operator native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>() const
{
    native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2> result;
    result.assign(data(), length());
    return result;
}

                     // *** 21.3.6 modifiers: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(
                                                   const basic_string& string)
{
    return append(string);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(
                                                       const CHAR_TYPE *string)
{
    BSLS_ASSERT_SAFE(string);

    return append(string);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(CHAR_TYPE character)
{
    push_back(character);
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                                   const basic_string&  string)
{
    return append(string, size_type(0), npos);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                                  const basic_string& string,
                                                  size_type           position,
                                                  size_type           numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > string.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                 "string<...>::append(string const&,pos,n): invalid position");
    }
    if (numChars > string.length() - position) {
        numChars = string.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                     "string<...>::append(string const&...): string too long");
    }
    return privateAppendRaw(string.data() + position, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                             "string<...>::append(char*...): string too long");
    }
    return privateAppendRaw(characterString, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    return append(characterString, CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(size_type numChars,
                                                      CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                     numChars > max_size() - this->d_length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                  "string<...>::append(n,c): string too long");
    }
    return privateResizeRaw(length() + numChars, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(INPUT_ITER first,
                                                      INPUT_ITER last)
{
    return privateAppendDispatch(first, last);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::push_back(
                                                           CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() >= max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                              "string<...>::push_back(char): string too long");
    }
    if (length() + 1 > capacity()) {
        privateReserveRaw(length() + 1);
    }
    CHAR_TRAITS::assign(*(begin() + length()), character);
    ++this->d_length;
    CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                                   const basic_string& string)
{
    return assign(string, size_type(0), npos);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                                  const basic_string& string,
                                                  size_type           position,
                                                  size_type           numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > string.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                 "string<...>::assign(string const&,pos,n): invalid position");
    }
    if (numChars > string.length() - position) {
        numChars = string.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                     "string<...>::assign(string const&...): string too long");
    }
    this->d_length = 0;
    return privateAppendRaw(string.data() + position, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    return assign(characterString, CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                             "string<...>::assign(char*...): string too long");
    }
    this->d_length = 0;
    return privateAppendRaw(characterString, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(size_type numChars,
                                                      CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                  "string<...>::assign(n,c): string too long");
    }
    this->d_length = 0;
    return privateAppendRaw(numChars, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(INPUT_ITER first,
                                                      INPUT_ITER last)
{
    this->d_length = 0;
    return privateAppendDispatch(first, last);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                                  size_type           position,
                                                  const basic_string& string)
{
    return insert(position, string, size_type(0), npos);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                              size_type            outPosition,
                                              const basic_string&  string,
                                              size_type            position,
                                              size_type            numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())
     || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > string.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                "string<...>::insert(pos,string const&...): invalid position");
    }
    if (numChars > string.length() - position) {
        numChars = string.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                     "string<...>::insert(string const&...): string too long");
    }
    return privateInsertRaw(outPosition, string.data() + position, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                              size_type        position,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                        "string<...>::insert(pos,char*...): invalid position");
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                             "string<...>::insert(char*...): string too long");
    }
    return privateInsertRaw(position, characterString, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                              size_type        position,
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    return insert(position,
                  characterString,
                  CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(size_type position,
                                                      size_type numChars,
                                                      CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                             "string<...>::insert(pos,n,c): invalid position");
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                              "string<...>::insert(pos,n,v): string too long");
    }
    return privateReplaceRaw(position, size_type(0), numChars, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(const_iterator position,
                                                      CHAR_TYPE      character)
{
    BSLS_ASSERT_SAFE(position >= cbegin());
    BSLS_ASSERT_SAFE(position <= cend());

    size_type pos = position - cbegin();
    insert(pos, size_type(1), character);
    return begin() + pos;
}

#if defined(BSLS_PLATFORM__CMP_SUN)
    // Sun CC compiler doesn't like that 'iterator' return type of 'insert'
    // method with an additional 'INPUT_ITER' template parameter depends on
    // template parameters of the primary template class 'basic_string'.
    // However it happily accepts 'CHAR_TYPE *', which is how 'iterator' is
    // currently defined.  It will also accept an inline definition of this
    // method (this workaround should be used when 'iterator' becomes a real
    // class and the current workaround stops working).
#   define BSLSTL_INSERT_RETURN_TYPE CHAR_TYPE *
#else
#   define BSLSTL_INSERT_RETURN_TYPE \
    typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
#endif

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
BSLSTL_INSERT_RETURN_TYPE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(const_iterator position,
                                                      INPUT_ITER     first,
                                                      INPUT_ITER     last)
{
    BSLS_ASSERT_SAFE(position >= cbegin());
    BSLS_ASSERT_SAFE(position <= cend());

    size_type pos = position - cbegin();
    privateInsertDispatch(position, first, last);
    return begin() + pos;
}

#undef BSLSTL_INSERT_RETURN_TYPE

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(const_iterator position,
                                                      size_type      numChars,
                                                      CHAR_TYPE      character)
{
    BSLS_ASSERT_SAFE(position >= cbegin());
    BSLS_ASSERT_SAFE(position <= cend());

    size_type pos = position - cbegin();
    insert(pos, numChars, character);
    return begin() + pos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(size_type position,
                                                     size_type numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                                "string<...>::erase(pos,n): invalid position");
    }
    if (numChars > length() - position) {
        numChars = length() - position;
    }
    if (numChars) {
        this->d_length -= numChars;
        CHAR_TRAITS::move(this->dataPtr() + position,
                          this->dataPtr() + position + numChars,
                          this->d_length - position);
        CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
    }
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position >= cbegin());
    BSLS_ASSERT_SAFE(position < cend());

    const_iterator postPosition = position;
    iterator dstPosition = begin() + (position - cbegin());

    ++postPosition;
    CHAR_TRAITS::move(&*dstPosition, &*postPosition, cend() - postPosition);

    --this->d_length;
    CHAR_TRAITS::assign(*(this->dataPtr() + length()), CHAR_TYPE());

    return dstPosition;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(const_iterator first,
                                                     const_iterator last)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(last >= cbegin());
    BSLS_ASSERT_SAFE(last <= cend());
    BSLS_ASSERT_SAFE(last - first >= 0);

    iterator dstFirst = begin() + (first - cbegin());

    if (first != last) {
        CHAR_TRAITS::move(&*dstFirst, &*last, cend() - last);

        this->d_length -= last - first;
        CHAR_TRAITS::assign(*(this->dataPtr() + length()), CHAR_TYPE());
    }

    return dstFirst;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(!empty());

    --this->d_length;
    CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                               size_type           outPosition,
                                               size_type           outNumChars,
                                               const basic_string& string)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < outPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
               "string<...>::replace(pos,string const&...): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(string.length() > outNumChars
                   && string.length() - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                "string<...>::replace(pos,string const&...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             string.data(),
                             string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                               size_type           outPosition,
                                               size_type           outNumChars,
                                               const basic_string& string,
                                               size_type           position,
                                               size_type           numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
               "string<...>::replace(pos,string const&...): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > string.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
               "string<...>::replace(pos,string const&...): invalid position");
    }
    if (numChars > string.length() - position) {
        numChars = string.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                    "string<...>::replace(string const&...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             string.data() + position,
                             numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                              size_type        outPosition,
                                              size_type        outNumChars,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                       "string<...>::replace(pos,char*...): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                            "string<...>::replace(char*...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             characterString,
                             numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                              size_type        outPosition,
                                              size_type        outNumChars,
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    return replace(outPosition,
                   outNumChars,
                   characterString,
                   CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(size_type outPosition,
                                                       size_type outNumChars,
                                                       size_type numChars,
                                                       CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                            "string<...>::replace(pos,n,c): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                             "string<...>::replace(pos,n,v): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             numChars,
                             character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                                    const_iterator      first,
                                                    const_iterator      last,
                                                    const basic_string& string)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= cend());

    size_type outPosition = first - cbegin();
    size_type outNumChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(string.length() > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                      string.length() - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                    "string<...>::replace(string const&...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             string.data(),
                             string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                              const_iterator   first,
                                              const_iterator   last,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= cend());
    BSLS_ASSERT_SAFE(characterString);

    size_type outPosition = first - cbegin();
    size_type outNumChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                            "string<...>::replace(char*...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             characterString,
                             numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                              const_iterator   first,
                                              const_iterator   last,
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= cend());
    BSLS_ASSERT_SAFE(characterString);

    return replace(first,
                   last,
                   characterString,
                   CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                                      const_iterator first,
                                                      const_iterator last,
                                                      size_type      numChars,
                                                      CHAR_TYPE      character)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= cend());

    size_type outPosition = first - cbegin();
    size_type outNumChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                            "string<...>::replace(char*...): string too long");
    }
    return privateReplaceRaw(outPosition, outNumChars, numChars, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                                    const_iterator first,
                                                    const_iterator last,
                                                    INPUT_ITER     stringFirst,
                                                    INPUT_ITER     stringLast)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= cend());

    size_type outPosition = first - cbegin();
    size_type outNumChars = last - first;
    return privateReplaceDispatch(outPosition,
                                  outNumChars,
                                  stringFirst,
                                  stringLast,
                                  stringFirst,
                                  stringLast);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::swap(basic_string& other)
{
    if (get_allocator() == other.get_allocator()) {
        privateBase().swap(other.privateBase());
    }
    else {
        basic_string s1(other, this->get_allocator());
        basic_string s2(*this, other.get_allocator());

        s1.privateBase().swap(this->privateBase());
        s2.privateBase().swap(other.privateBase());
    }
}

// ACCESSORS

                     // *** 21.3.3 iterators: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::begin() const
{
    return this->dataPtr();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::cbegin() const
{
    return begin();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::end() const
{
    return begin() + this->d_length;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::cend() const
{
    return end();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rend() const
{
    return const_reverse_iterator(begin());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::crend() const
{
    return const_reverse_iterator(begin());
}

                      // *** 21.3.4 capacity: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size() const
{
    return this->d_length;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::length() const
{
    return this->d_length;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::max_size() const
{
    // Must take into account the null-terminating character.

    size_type stringMaxSize = ~size_type(0) / sizeof(CHAR_TYPE) - 1;
    size_type allocMaxSize  = get_allocator().max_size() - 1;
    return allocMaxSize < stringMaxSize ? allocMaxSize : stringMaxSize;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::capacity() const
{
    return this->d_capacity;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
bool basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::empty() const
{
    return this->d_length == 0;
}

                   // *** 21.3.5 element access: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator[](
                                                      size_type position) const
{
    BSLS_ASSERT_SAFE(position <= length());

    return *(begin() + position);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::at(size_type position) const
{
    // Note: deliberately not inlined (see comment in non-const version).

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                                 "const string<...>::at(n): invalid position");
    }
    return *(begin() + position);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *begin();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *(end() - 1);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::copy(CHAR_TYPE *characterString,
                                                    size_type  numChars,
                                                    size_type  position) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                       "const string<...>::copy(str,pos,n): invalid position");
    }
    if (numChars > length() - position) {
        numChars = length() - position;
    }
    CHAR_TRAITS::move(characterString, this->dataPtr() + position, numChars);
    return numChars;
}

                 // *** 21.3.7 string operations: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_pointer
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::c_str() const
{
    return this->dataPtr();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_pointer
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::data() const
{
    return this->dataPtr();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::allocator_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::get_allocator() const
{
    return BloombergLP::bslstl_ContainerBase<allocator_type>::allocator();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(
                                           const basic_string&  string,
                                           size_type            position) const
{
    return find(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(
                                               const CHAR_TYPE *string,
                                               size_type        position,
                                               size_type        numChars) const
{
    BSLS_ASSERT_SAFE(string);

    size_type remChars = length() - position;
    if (position > length() || numChars > remChars) {
        return npos;                                                  // RETURN
    }
    if (0 == numChars) {
        return position;                                              // RETURN
    }
    const CHAR_TYPE *thisString = this->dataPtr() + position;
    const CHAR_TYPE *nextString;
    for (remChars -= numChars - 1;
         0 != (nextString = BSLSTL_CHAR_TRAITS::find(thisString,
                                                     remChars,
                                                     *string));
         remChars -= ++nextString - thisString, thisString = nextString)
    {
        if (0 == CHAR_TRAITS::compare(nextString, string, numChars)) {
            return nextString - this->dataPtr();                      // RETURN
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(
                                               const CHAR_TYPE *string,
                                               size_type        position) const
{
    BSLS_ASSERT_SAFE(string);

    return find(string, position, CHAR_TRAITS::length(string));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(CHAR_TYPE character,
                                                    size_type position) const
{
    if (position >= length()) {
        return npos;                                                  // RETURN
    }
    const CHAR_TYPE *result =
        BSLSTL_CHAR_TRAITS::find(this->dataPtr() + position,
                                 length() - position,
                                 character);
    return result ? result - this->dataPtr() : npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(
                                            const basic_string& string,
                                            size_type           position) const
{
    return rfind(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (0 == numChars) {
        return position > length() ? length() : position;             // RETURN
    }
    if (numChars <= length()) {
        if (position > length() - numChars) {
            position = length() - numChars;
        }
        const CHAR_TYPE *thisString = this->dataPtr() + position;
        for (; position != npos; --thisString, --position) {
            if (0 == CHAR_TRAITS::compare(thisString,
                                          characterString,
                                          numChars)) {
                return position;                                      // RETURN
            }
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return rfind(characterString,
                 position,
                 CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(CHAR_TYPE character,
                                                     size_type position) const
{
    return rfind(&character, position, size_type(1));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                           const basic_string& string,
                                           size_type           position) const
{
    return find_first_of(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (0 < numChars && position < length()) {
        for (const CHAR_TYPE *current = this->dataPtr() + position;
             current != this->dataPtr() + length();
             ++current)
        {
            if (BSLSTL_CHAR_TRAITS::find(characterString, numChars, *current)
                != 0) {
                return current - this->dataPtr();                     // RETURN
            }
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_first_of(characterString,
                         position,
                         CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_first_of(&character, position, size_type(1));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                            const basic_string& string,
                                            size_type           position) const
{
    return find_last_of(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (0 < numChars && 0 < length()) {
        size_type remChars = position < length() ? position : length() - 1;
        for (const CHAR_TYPE *current = this->dataPtr() + remChars;
             ;
             --current)
        {
            if (BSLSTL_CHAR_TRAITS::find(
                                        characterString, numChars, *current)) {
                return current - this->dataPtr();                     // RETURN
            }
            if (current == this->dataPtr()) {
                break;
            }
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_last_of(characterString,
                        position,
                        CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_last_of(&character, position, size_type(1));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                            const basic_string& string,
                                            size_type           position) const
{
    return find_first_not_of(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (position < length()) {
        const CHAR_TYPE *last = this->dataPtr() + length();
        for (const CHAR_TYPE *current = this->dataPtr() + position;
             current != last;
             ++current)
        {
            if (!BSLSTL_CHAR_TRAITS::find(
                                        characterString, numChars, *current)) {
                return current - this->dataPtr();                     // RETURN
            }
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_first_not_of(characterString,
                             position,
                             CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_first_not_of(&character, position, size_type(1));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                            const basic_string& string,
                                            size_type           position) const
{
    return find_last_not_of(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (0 < length()) {
        size_type remChars = position < length() ? position : length() - 1;
        for (const CHAR_TYPE *current = this->dataPtr() + remChars;
             remChars != npos;
             --current, --remChars)
        {
            if (!BSLSTL_CHAR_TRAITS::find(
                                        characterString, numChars, *current)) {
                return current - this->dataPtr();                     // RETURN
            }
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_last_not_of(characterString,
                            position,
                            CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_last_not_of(&character, position, size_type(1));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::substr(size_type position,
                                                      size_type numChars) const
{
    return basic_string<CHAR_TYPE,
                        CHAR_TRAITS,
                        ALLOCATOR>(*this, position, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                               const basic_string& other) const
{
    return privateCompareRaw(size_type(0),
                             length(),
                             other.data(),
                             other.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                              size_type            position,
                                              size_type            numChars,
                                              const basic_string&  other) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                    "const string<...>::compare(pos,n,...): invalid position");
    }
    if (numChars > length() - position) {
        numChars = length() - position;
    }
    return privateCompareRaw(position, numChars, other.data(), other.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                       size_type           lhsPosition,
                                       size_type           lhsNumChars,
                                       const basic_string& other,
                                       size_type           otherPosition,
                                       size_type           otherNumChars) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                    "const string<...>::compare(pos,n,...): invalid position");
    }
    if (lhsNumChars > length() - lhsPosition) {
        lhsNumChars = length() - lhsPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(other.length() <
                                                              otherPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                    "const string<...>::compare(pos,n,...): invalid position");
    }
    if (otherNumChars > other.length() - otherPosition) {
        otherNumChars = other.length() - otherPosition;
    }
    return privateCompareRaw(lhsPosition,
                             lhsNumChars,
                             other.dataPtr() + otherPosition,
                             otherNumChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                                  const CHAR_TYPE *other) const
{
    BSLS_ASSERT_SAFE(other);

    return privateCompareRaw(size_type(0),
                             length(),
                             other,
                             CHAR_TRAITS::length(other));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                          size_type        lhsPosition,
                                          size_type        lhsNumChars,
                                          const CHAR_TYPE *other,
                                          size_type        otherNumChars) const
{
    BSLS_ASSERT_SAFE(other);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                    "const string<...>::compare(pos,n,...): invalid position");
    }
    if (lhsNumChars > length() - lhsPosition) {
        lhsNumChars = length() - lhsPosition;
    }
    return privateCompareRaw(lhsPosition,
                             lhsNumChars,
                             other,
                             otherNumChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                                  size_type        lhsPosition,
                                                  size_type        lhsNumChars,
                                                  const CHAR_TYPE *other) const
{
    BSLS_ASSERT_SAFE(other);

    return compare(lhsPosition,
                   lhsNumChars,
                   other,
                   CHAR_TRAITS::length(other));
}

// FREE FUNCTIONS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void swap(basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& lhs,
          basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.get_allocator() == rhs.get_allocator());

    lhs.swap(rhs);
}

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator==(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator==(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator==(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    std::size_t len = CHAR_TRAITS::length(lhs);
    return len == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs, rhs.data(), len);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                const CHAR_TYPE*                                 rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    std::size_t len = CHAR_TRAITS::length(rhs);
    return lhs.size() == len
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs, len);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator!=(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator!=(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator!=(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    const std::size_t minLen = lhs.length() < rhs.length()
                             ? lhs.length() : rhs.length();
    int ret = CHAR_TRAITS::compare(lhs.data(), rhs.data(), minLen);
    if (0 == ret) {
        return lhs.length() < rhs.length();                           // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator<(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
          const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    const std::size_t minLen = lhs.length() < rhs.length()
                             ? lhs.length() : rhs.length();
    int ret = CHAR_TRAITS::compare(lhs.data(), rhs.data(), minLen);
    if (0 == ret) {
        return lhs.length() < rhs.length();                           // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bool
operator<(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
          const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    const std::size_t minLen = lhs.length() < rhs.length()
                             ? lhs.length() : rhs.length();
    int ret = CHAR_TRAITS::compare(lhs.data(), rhs.data(), minLen);
    if (0 == ret) {
        return lhs.length() < rhs.length();                           // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const CHAR_TYPE                                  *lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    const std::size_t lhsLen = CHAR_TRAITS::length(lhs);
    const std::size_t minLen = lhsLen < rhs.length() ? lhsLen : rhs.length();
    int ret = CHAR_TRAITS::compare(lhs, rhs.data(), minLen);
    if (0 == ret) {
        return lhsLen < rhs.length();                                 // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    const std::size_t rhsLen = CHAR_TRAITS::length(rhs);
    const std::size_t minLen = rhsLen < lhs.length() ? rhsLen : lhs.length();
    int ret = CHAR_TRAITS::compare(lhs.data(), rhs, minLen);
    if (0 == ret) {
        return lhs.length() < rhsLen;                                 // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs) {
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator>(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
          const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator>(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
          const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>(const CHAR_TYPE                                  *lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs) {
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator<=(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator<=(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator<=(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs) {
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator>=(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
           const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
operator>=(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
           const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>=(const CHAR_TYPE                                  *lhs,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return !(lhs < rhs);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& rhs)
{
    basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> result;
    result.reserve(lhs.length() + rhs.length());
    result += lhs;
    result += rhs;
    return result;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>
operator+(const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
          const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2> result;
    result.reserve(lhs.length() + rhs.length());
    result.append(lhs.c_str(), lhs.length());
    result += rhs;
    return result;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>
operator+(const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
          const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1> result;
    result.reserve(lhs.length() + rhs.length());
    result += lhs;
    result.append(rhs.c_str(), rhs.length());
    return result;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const CHAR_TYPE                                      *lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
                                          lhsLength = CHAR_TRAITS::length(lhs);
    basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> result;
    result.reserve(lhsLength + rhs.length());
    result.append(lhs, lhsLength);
    result += rhs;
    return result;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(CHAR_TYPE                                            lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& rhs)
{
    basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> result;
    result.reserve(1 + rhs.length());
    result.push_back(lhs);
    result += rhs;
    return result;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  lhs,
          const CHAR_TYPE                                      *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
                                          rhsLength = CHAR_TRAITS::length(rhs);
    basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> result;
    result.reserve(lhs.length() + rhsLength);
    result += lhs;
    result.append(rhs, rhsLength);
    return result;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& lhs,
          CHAR_TYPE                                            rhs)
{
    basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> result;
    result.reserve(lhs.length() + 1);
    result += lhs;
    result.push_back(rhs);
    return result;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
bool bslstl_string_fill(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&   os,
                        std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS> *buf,
                        size_t n)
    // Do not use, for internal use by 'operator<<' only.
{
    BSLS_ASSERT_SAFE(buf);

    CHAR_TYPE fillChar = os.fill();

    for (size_t i = 0; i < n; ++i) {
        if (CHAR_TRAITS::eq_int_type(buf->sputc(fillChar), CHAR_TRAITS::eof()))
        {
            return false;                                             // RETURN
        }
    }

    return true;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&          os,
           const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str)
{
    typedef std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> Ostrm;
    typename Ostrm::sentry sentry(os);
    bool ok = false;

    if (sentry) {
        ok = true;
        size_t n = str.size();
        size_t padLen = 0;
        const bool left = (os.flags() & Ostrm::left) != 0;
        const size_t w = os.width(0);
        std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>* buf = os.rdbuf();

        if (n < w) {
            padLen = w - n;
        }

        if (!left) {
            ok = bslstl_string_fill(os, buf, padLen);
        }

        ok = ok && (buf->sputn(str.data(), std::streamsize(n)) ==
                        std::streamsize(n));

        if (left) {
            ok = ok && bslstl_string_fill(os, buf, padLen);
        }
    }

    if (!ok) {
        os.setstate(Ostrm::failbit);
    }

    return os;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&
operator>>(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&     is,
           basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& str)
{
    typedef std::basic_istream<CHAR_TYPE, CHAR_TRAITS> Istrm;
    typename Istrm::sentry sentry(is);

    if (sentry) {
        std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>* buf = is.rdbuf();
        typedef std::ctype<CHAR_TYPE> _C_type;

        const std::locale& loc = is.getloc();
        const _C_type& ctype = std::use_facet<_C_type>(loc);

        str.clear();
        size_t n = is.width(0);
        if (n == 0) {
            n = static_cast<size_t>(-1);
        }
        else {
            str.reserve(n);
        }

        while (n-- > 0) {
            typename CHAR_TRAITS::int_type c1 = buf->sbumpc();
            if (CHAR_TRAITS::eq_int_type(c1, CHAR_TRAITS::eof())) {
                is.setstate(Istrm::eofbit);
                break;
            }
            else {
                CHAR_TYPE c = CHAR_TRAITS::to_char_type(c1);

                if (ctype.is(_C_type::space, c)) {
                    if (CHAR_TRAITS::eq_int_type(buf->sputbackc(c),
                                                 CHAR_TRAITS::eof())) {
                        is.setstate(Istrm::failbit);
                    }
                    break;
                }
                else {
                    str.push_back(c);
                }
            }
        }

        // If we have read no characters, then set failbit.

        if (str.size() == 0) {
            is.setstate(Istrm::failbit);
        }
    }
    else {
        is.setstate(Istrm::failbit);
    }

    return is;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&
getline(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&    is,
        basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str,
        CHAR_TYPE                                      delim)
{
    typedef std::basic_istream<CHAR_TYPE, CHAR_TRAITS> Istrm;
    size_t nread = 0;
    typename Istrm::sentry sentry(is, true);
    if (sentry) {
        std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>* buf = is.rdbuf();
        str.clear();

        while (nread < str.max_size()) {
            int c1 = buf->sbumpc();
            if (CHAR_TRAITS::eq_int_type(c1, CHAR_TRAITS::eof())) {
                is.setstate(Istrm::eofbit);
                break;
            }

            ++nread;
            CHAR_TYPE c = CHAR_TRAITS::to_char_type(c1);
            if (!CHAR_TRAITS::eq(c, delim)) {
                str.push_back(c);
            }
            else {
                break;  // character is extracted but not appended
            }
        }
    }
    if (nread == 0 || nread >= str.max_size()) {
        is.setstate(Istrm::failbit);
    }

    return is;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&
getline(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&    is,
        basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str)
{
    return getline(is, str, is.widen('\n'));
}

// HASH SPECIALIZATIONS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::size_t
hashBasicString(const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str)
{
    unsigned long hashValue = 0;
    typedef typename basic_string<CHAR_TYPE,
                              CHAR_TRAITS, ALLOCATOR>::const_pointer const_ptr;

    std::size_t  len  = str.size();
    const_ptr    data = str.data();

    for (std::size_t i = 0; i < len; ++i) {
        hashValue = 5 * hashValue + data[i];
    }

    return std::size_t(hashValue);
}

}  // close namespace bsl

#undef BSLSTL_CHAR_TRAITS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
extern template class bsl::String_Imp<char, bsl::string::size_type>;
extern template class bsl::String_Imp<wchar_t, bsl::wstring::size_type>;
extern template class bsl::basic_string<char>;
extern template class bsl::basic_string<wchar_t>;
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
