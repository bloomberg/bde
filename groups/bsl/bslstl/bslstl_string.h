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
//         bsl::string: typedef for bsl::basic_string<char>
//        bsl::wstring: typedef for bsl::basic_string<wchar>
//
//@SEE_ALSO: ISO C++ Standard, Section 23 [strings]
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<string>' and use 'bsl::string' instead.  This component implements a
// dynamic string class that supports the 'bslma_Allocator' model and is
// suitable for use as an implementation of the 'std::basic_string' class
// template.
//
///Lexicographical Comparisons
///---------------------------
// Two strings 'lhs' and 'rhs' are lexicographically compared by first
// determining 'N', the smaller of the lengths of 'lhs' and 'rhs', and
// comparing characters at each position between 0 and 'N - 1', using
// 'CHAR_TRAITS::lt' in lexicographical fashion.  If comparison can be
// determined (smaller or larger), then this is the result.  Otherwise, the
// lengths of the strings are compared and the shorter string is declared the
// smaller.  Lexicographical comparison returns equality only when both strings
// have the same length and the same character in each respective position.

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

#ifndef INCLUDED_BSLSTL_UTIL
#include <bslstl_util.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#include <bslalg_typetraitsgroupstlsequence.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
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

#ifndef INCLUDED_EXCEPTION
#include <exception>
#define INCLUDED_EXCEPTION
#endif

#ifndef INCLUDED_STRING
#include <string>  // for native_std::char_traits
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_STDEXCEPT
#include <stdexcept>
#define INCLUDED_STDEXCEPT
#endif

#endif

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

// For transitive includes.  This is not placed in bsl+stdhdrs because it
// causes a cycle within the native standard headers.
#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
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

  public:
    // CLASS METHODS
    template <class SIZE_TYPE>
    static const char_type *find(const char_type  *s,
                                 SIZE_TYPE         n,
                                 const char_type&  a);

};

template <>
class String_Traits<native_std::char_traits<char> > {
    // Sun implemented 'find' for 'char' properly, so this specialization
    // simply forwards the call to Sun.

  public:
    // CLASS METHODS
    template <class SIZE_TYPE>
    static const char *find(const char *s, SIZE_TYPE n, const char& a);
};

// CLASS METHODS
template <class ORIGINAL_TRAITS>
template <class SIZE_TYPE>
const typename ORIGINAL_TRAITS::char_type *
String_Traits<ORIGINAL_TRAITS>::find(const char_type  *s,
                                     SIZE_TYPE         n,
                                     const char_type&  a)
{
    while (n > 0 && !ORIGINAL_TRAITS::eq(*s, a)) {
        --n;
        ++s;
    }
    return n > 0 ? s : 0;
}

template <class SIZE_TYPE>
inline
const char *
String_Traits<native_std::char_traits<char> >::find(const char  *s,
                                                    SIZE_TYPE    n,
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
    // This component-private class describes the basic layout for a string
    // class, to be included into the 'basic_string' layout *before* the
    // allocator (provided by 'bslstl_ContainerBase') to take better advantage
    // of cache prefetching.  It is parameterized by 'CHAR_TYPE' and
    // 'SIZE_TYPE' only, and implements the portion of 'basic_string' that does
    // not need to know about its parameterized 'CHAR_TRAITS' or 'ALLOCATOR'.
    // In empty string representation, it points to a static buffer containing
    // a single null character, and stores a length of 0 and a capacity of 1.
    // In non-empty representation, it points to a dynamically allocated buffer
    // containing data of a certain length followed by a null-terminating
    // character, and whose length is the string capacity plus one (hence
    // capacity also does not account for the null-terminating character).
    // Note that the buffer is held by this representation, but owned by the
    // actual string class, which also controls the allocators.

    // CLASS DATA
    static CHAR_TYPE s_nullChar;
        // This character, default constructed, provides a buffer shared by all
        // empty strings of the same type.  It may *not* be modified by any
        // object.

    // DATA
    CHAR_TYPE *d_start_p;  // pointer to the data buffer (held)
    SIZE_TYPE  d_length;   // length, in either string representation
    SIZE_TYPE  d_capacity; // capacity, in either string representation

    // FRIENDS
    template <class CHAR_TYPE2, class CHAR_TRAITS2, class ALLOCATOR2>
    friend class bsl::basic_string;

  public:
    // TYPES
    enum ConfigurableParameters {
        // These parameters, configurable in this component, allow to change
        // the basic behavior in a centralized position.

        BASIC_STRING_DEALLOCATE_IN_CLEAR  = false
      , BASIC_STRING_HONOR_SHRINK_REQUEST = false
    };

    // CLASS METHODS
    static SIZE_TYPE computeNewCapacity(SIZE_TYPE newLength,
                                        SIZE_TYPE capacity,
                                        SIZE_TYPE maxSize);
        // Return a capacity at least the specified 'newLength' and at least
        // twice the specified 'capacity', not to exceed the specified
        // 'maxSize'.  The behavior is undefined unless 'newLength > capacity'.

    // CREATORS
    explicit
    String_Imp(CHAR_TYPE *buffer);
    String_Imp(CHAR_TYPE *buffer, SIZE_TYPE length, SIZE_TYPE capacity);
        // Create a string representation using the specified 'buffer', and
        // storing the optionally specified 'length' and 'capacity'.  If
        // 'length' is not specified, then both 'length' and 'capacity' are 0.
        // If 'length' is specified, then 'capacity' must be specified also.
        // Note that 'length' is not the length of the 'buffer' but the length
        // of the data contained in the buffer (not including the
        // null-terminating character), and that the length of the 'buffer' is
        // 'capacity + 1'.

    // MANIPULATORS
    void swap(String_Imp& other);
        // Swap the representation of this string with that of the specified
        // 'other' string.
};

                        // =======================
                        // class bsl::basic_string
                        // =======================

// Import char_traits into the bslstl namespace so that basic_string and
// char_traits are always in the same namespace.
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
    //       size returned by 'max_size' throws 'bsl::length_error', and
    //   (2) any method that attempts to access a position outside the valid
    //       range of a string throws 'bsl::out_of_range'.
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
    // unchanged.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

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

    // Note: none of the '...Raw' private manipulators perform checks for
    // exceptions.  The '...Dispatch' functions perform overload selection for
    // iterator types, but internally (in order to avoid ambiguities in user
    // code).

    CHAR_TYPE *privateAllocate(size_type numChars);
        // Return a buffer capable of holding (at least) the specified
        // 'numChars'.  Note that the buffer actual capacity is stored as a
        // prefix of the returned buffer, thus this buffer should not be
        // deallocated except by using 'privateDeallocate' below.

    void privateDeallocate(CHAR_TYPE *buffer);
        // Deallocate the specified 'buffer'.  The behavior is undefined unless
        // this buffer was obtained via 'privateAllocate'.  Note that the
        // actual address being deallocated is a prefix of 'buffer', not
        // 'buffer' itself.

    basic_string& privateAppendDispatch(iterator       begin,
                                        iterator       end);
    basic_string& privateAppendDispatch(const_iterator begin,
                                        const_iterator end);
    template <typename INPUT_ITER>
    basic_string& privateAppendDispatch(INPUT_ITER     begin,
                                        INPUT_ITER     end);
        // Match either 'iterator', 'const_iterator', or arbitrary iterator.
        // In the first two cases, use 'privateAppendRaw'.  In the last case,
        // forward to 'privateReplaceDispatch' to take care of integral
        // iterator, input iterator, or (at least) forward iterator.

    basic_string& privateAppendRaw(const CHAR_TYPE *characterString,
                                   size_type        numChars);
        // Append the characters in the array of length the specified
        // 'numChars' at the specified 'characterString' address to this
        // string, and return a reference to this modifiable string.  The
        // behavior is undefined unless 'length() + numChars <= max_size()'.

    basic_string& privateAppendRaw(size_type numChars,
                                   CHAR_TYPE character);
        // Append the specified 'numChars' copies of the specified 'character'
        // to this string.  Return a reference to this modifiable string.  The
        // behavior is undefined unless 'length() + numChars <= max_size()'.

    Imp& privateBase();
        // Return a reference to the modifiable base object of this string.

    void privateClear(bool deallocateBufferFlag);
        // Reset this string to an empty value, and reset to short string
        // representation (and deallocate buffer if currently in long string
        // representation) if the specified 'deallocateBuffer' is true.

    void privateInitDispatch(iterator       begin,
                             iterator       end);
    void privateInitDispatch(const_iterator begin,
                             const_iterator end);
    template <typename INPUT_ITER>
    void privateInitDispatch(INPUT_ITER     begin,
                             INPUT_ITER     end);
        // Match either 'iterator', 'const_iterator', or arbitrary iterator.
        // In the first two cases, use 'privateAppendRaw'.  In the last case,
        // forward to 'privateReplaceDispatch' to take care of integral
        // iterator, input iterator, or (at least) forward iterator.

    void privateInsertDispatch(iterator       position,
                               iterator       first,
                               iterator       last);
    void privateInsertDispatch(iterator       position,
                               const_iterator first,
                               const_iterator last);
    template <typename INPUT_ITER>
    void privateInsertDispatch(iterator       position,
                               INPUT_ITER     first,
                               INPUT_ITER     last);
        // Match either 'iterator', 'const_iterator', or arbitrary iterator.
        // In the first two cases, use 'privateInsertRaw'.  In the last case,
        // forward to 'privateReplaceDispatch' to take care of integral
        // iterator, input iterator, or (at least) forward iterator.

    basic_string& privateInsertRaw(size_type        outPosition,
                                   const CHAR_TYPE *characterString,
                                   size_type        numChars);
        // Insert the characters in the array of length the specified
        // 'numChars' at the specified 'characterString' address at the
        // specified 'outPosition' in this string, and return a reference to
        // this modifiable string.  The behavior is undefined unless
        // 'outPosition <= length()' and 'length() + numChars <= max_size()'.

    basic_string& privateReplaceRaw(size_type        outPosition,
                                    size_type        outNumChars,
                                    const CHAR_TYPE *characterString,
                                    size_type        numChars);
        // Replace the specified 'outNumChars' characters at the specified
        // 'outPosition' in this string, by the characters in the array of
        // length the specified 'numChars' at the specified 'characterString'
        // address, and return a reference to this modifiable string.  The
        // behavior is undefined unless 'outPosition + outNumChars <= length()'
        // and 'length() + numChars - outNumChars <= max_size()'.

    basic_string& privateReplaceRaw(size_type outPosition,
                                    size_type outNumChars,
                                    size_type numChars,
                                    CHAR_TYPE character);
        // Replace the specified 'outNumChars' characters at the specified
        // 'outPosition' in this string, by the specified 'numChars' copies of
        // the specified 'character', and return a reference to this modifiable
        // string.  The behavior is undefined unless
        // 'outPosition + outNumChars <= length()'.

    template <typename INPUT_ITER>
    basic_string& privateReplaceRaw(size_type  outPosition,
                                    size_type  outNumChars,
                                    INPUT_ITER first,
                                    size_type  numChars);
        // Replace the specified 'outNumChars' characters at the specified
        // 'outPosition' in this string, by the characters in the range of
        // length the specified 'numChars' starting at the specified 'first'
        // iterator of the parameterized 'FWD_ITER'.  The behavior is undefined
        // unless 'outPosition + outNumChars <= length()' and
        // 'length() + numChars - outNumChars <= max_size()'.

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
        // 'length() + distance(first, last) - numChars > max_size()'.

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

    basic_string& privateResizeRaw(size_type newLength,
                                   CHAR_TYPE character);
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
        // is undefined unless 'lhsPosition + lhsNumChars <= length()'.

    bool privateIsEmpty() const;
        // Return 'true' if this string has 0 capacity and 'false' otherwise.
        // Note that this is subtly different from testing whether 'd_length'
        // is 0, as it is possible for a string to have 0 length and non-zero
        // capacity.

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
        // a default-constructed allocator is used.

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

    ~basic_string();
        // Destroy this string object.

    // MANIPULATORS

                    // *** 21.3.2 construct/copy/destroy: ***

    basic_string& operator=(const basic_string& rhs);
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
        // 'position' in this string, or a null-terminating character if
        // 'position == length()'.  Throw 'out_of_range' if
        // 'position > length()'.

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
        // to this modifiable string.

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
        // modifiable string.  Note that 'CHAR_TRAITS::length(characterString)'
        // is not necessarily larger than or equal to 'numChars'.

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
        // Return a reference to this modifiable string.

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

    iterator insert(iterator position, CHAR_TYPE character);
        // Insert at the specified 'position' in this string a copy of the
        // specified 'character', and return a reference to this modifiable
        // string.  Throw 'out_of_range' if 'position > length()'.

    void insert(iterator  position,
                size_type numChars,
                CHAR_TYPE character);
        // Insert at the specified 'position' in this string a number equal to
        // the specified 'numChars' of copies of the specified 'character', and
        // return a reference to this modifiable string.  Throw 'out_of_range'
        // if 'position > length()'.

    template <typename INPUT_ITER>
    void insert(iterator position, INPUT_ITER first, INPUT_ITER last);
        // Insert at the specified 'position' in this string a string built
        // from the characters in the range starting at the specified 'first'
        // and ending before the specified 'last' iterators of the 'iterator',
        // 'const_iterator', or parameterized 'INPUT_ITER' type, respectively.
        // Throw 'out_of_range' if 'position > length()'.

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

    iterator erase(iterator position);
        // Erase from this string the character at the specified 'position',
        // and return an iterator pointing to the first non-erased character
        // following 'position' after this call, i.e., to 'begin() + n' if 'n'
        // equals 'position - begin()' prior to this call.  The behavior is
        // undefined unless 'position' belongs to '[ begin(), end() ]'.

    iterator erase(iterator first, iterator last);
        // Erase from this string the substring starting at the specified
        // 'first' position and ending before the specified 'last' position.
        // Return an iterator pointing to the first non-erased character
        // following 'first' after this call, i.e., to 'begin() + n' if 'n'
        // equals 'first - begin()' prior to this call.  The behavior is
        // undefined unless 'first' and 'last' both belong to
        // '[ begin(), end() ]' and 'first <= last'.  Note that this call
        // invalidates existing iterators pointing to 'first' or a subsequent
        // position.

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
        // whichever is smaller, by the null-terminated specified 'string' (of
        // length 'CHAR_TRAITS::length(characterString)').  Throw
        // 'out_of_range' if 'outPosition > length()'.

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

    basic_string& replace(iterator            first,
                          iterator            last,
                          const basic_string& string);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by
        // the specified 'string'.  Return a reference to this modifiable
        // string.  The behavior is undefined unless 'first' and 'last' both
        // belong to '[ begin(), end() ]' and 'first <= last'.

    basic_string& replace(iterator         first,
                          iterator         last,
                          const CHAR_TYPE *characterString,
                          size_type        numChars);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // copy of the string constructed from the specified 'numChars'
        // characters in the array starting at the specified 'characterString'
        // address.  Return a reference to this modifiable string.  The
        // behavior is undefined unless 'first' and 'last' both belong to
        // '[ begin(), end() ]' and 'first <= last'.

    basic_string& replace(iterator         first,
                          iterator         last,
                          const CHAR_TYPE *characterString);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by
        // the null-terminated specified 'characterString'.  Return a reference
        // to this modifiable string.  The behavior is undefined unless 'first'
        // and 'last' both belong to the range '[ begin(), end() ]' and
        // 'first <= last'.

    basic_string& replace(iterator  first,
                          iterator  last,
                          size_type numChars,
                          CHAR_TYPE character);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // number equal to the specified 'numChars' of copies of the specified
        // 'character'.  Return a reference to this modifiable string.  The
        // behavior is undefined unless 'first' and 'last' both belong to the
        // range '[ begin(), end() ]' and 'first <= last'.

    template <typename INPUT_ITER>
    basic_string& replace(iterator   first,
                          iterator   last,
                          INPUT_ITER stringFirst,
                          INPUT_ITER stringLast);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // string built from the characters in the range starting at the
        // specified 'stringFirst' and ending before the specified 'stringLast'
        // iterators of the 'iterator', 'const_iterator', or parameterized
        // 'INPUT_ITER' type, respectively.  Return a reference to this
        // modifiable string.  The behavior is undefined unless 'first' and
        // 'last' both belong to the range '[ begin(), end() ]' and
        // 'first <= last'.

    void swap(basic_string& other);
        // Exchange the value of this string with that of the specified
        // 'string', so that the value of this string upon return equals that
        // of 'string' prior to this call, and vice-versa.

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
        // 'position', or '\0' if 'position == length()'.  Throw 'out_of_range'
        // if 'position > length()'.

    const_reference at(size_type position) const;
        // Return a reference to the non-modifiable character at the specified
        // 'position'.  Throw 'out_of_range' if 'position >= length()'.

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
        // Return a pointer to a (not necessarily null-terminated) buffer of
        // characters of length equal to the value returned by the 'length'
        // method, whose contents are identical to the value of this string.
        // Note that any call to the string destructor or any of its
        // manipulators invalidates the returned pointer.

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
    // and 'rhs' arguments, i.e., 'basic_string(lhs).append(rhs)'.  Note that
    // 'lhs' and 'rhs' are not necessarily strings themselves.

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

// HASH SPECIALIZATION
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::size_t
hashBasicString(const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str)
    // Return a hash value using the specified 'str'.
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

std::size_t hashBasicString(const string& str);
    // Return a hash value using the specified 'str'.

std::size_t hashBasicString(const wstring& str);
    // Return a hash value using the specified 'str'.

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

template <typename CHAR_TYPE, typename SIZE_TYPE>
CHAR_TYPE
String_Imp<CHAR_TYPE, SIZE_TYPE>::s_nullChar = CHAR_TYPE();

// CLASS METHODS
template <typename CHAR_TYPE, typename SIZE_TYPE>
SIZE_TYPE
String_Imp<CHAR_TYPE, SIZE_TYPE>::computeNewCapacity(SIZE_TYPE newLength,
                                                     SIZE_TYPE capacity,
                                                     SIZE_TYPE maxSize)
{
    BSLS_ASSERT_SAFE(newLength >= capacity);

    // We must exercise a minimum acceptable growth as doubling, otherwise we
    // cannot guarantee amortized time for append, insert, push_back, replace,
    // etc.

    SIZE_TYPE newCapacity = capacity;
    newCapacity += capacity + 1;   // minimum acceptable growth

    if (newLength > newCapacity) {
        newCapacity = newLength;
    }

    if (newCapacity < capacity || newCapacity > maxSize) {  // overflow
        newCapacity = maxSize;
    }

    return newCapacity;
}

// CREATORS
template <typename CHAR_TYPE, typename SIZE_TYPE>
String_Imp<CHAR_TYPE, SIZE_TYPE>::String_Imp(CHAR_TYPE *buffer)
: d_start_p(buffer)
, d_length(0)
, d_capacity(0)
{
}

template <typename CHAR_TYPE, typename SIZE_TYPE>
String_Imp<CHAR_TYPE, SIZE_TYPE>::String_Imp(CHAR_TYPE *buffer,
                                             SIZE_TYPE  length,
                                             SIZE_TYPE  capacity)
: d_start_p(buffer)
, d_length(length)
, d_capacity(capacity)
{
}

// MANIPULATORS
template <typename CHAR_TYPE, typename SIZE_TYPE>
void String_Imp<CHAR_TYPE, SIZE_TYPE>::swap(String_Imp& other)
{
    CHAR_TYPE *startPtr = other.d_start_p;
    SIZE_TYPE  length   = other.d_length;
    SIZE_TYPE  capacity = other.d_capacity;
    other.d_start_p  = d_start_p;
    other.d_length   = d_length;
    other.d_capacity = d_capacity;
    d_start_p  = startPtr;
    d_length   = length;
    d_capacity = capacity;
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
    return this->allocateN((CHAR_TYPE *)0, ++numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateDeallocate(
                                                             CHAR_TYPE *buffer)
{
    this->deallocateN(buffer, this->d_capacity + 1);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendDispatch(
                                                                iterator begin,
                                                                iterator end)
{
    return privateAppendDispatch(const_iterator(begin), const_iterator(end));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendDispatch(
                                                          const_iterator begin,
                                                          const_iterator end)
{
    size_type numChars = end - begin;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                            "string<...>::append<Iter>(i,j): string too long");
    }
    return privateAppendRaw(begin, numChars);
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
    BSLS_ASSERT_SAFE(length() + numChars <= max_size());

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

        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
    else {
        CHAR_TRAITS::move(end(), characterString, numChars);
        CHAR_TRAITS::assign(*(begin() + newLength), CHAR_TYPE());
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
    BSLS_ASSERT_SAFE(length() + numChars <= max_size());

    size_type newLength = this->d_length + numChars;
    privateReserveRaw(newLength);
    CHAR_TRAITS::assign(begin() + this->d_length, numChars, character);
    this->d_length = newLength;
    CHAR_TRAITS::assign(*(begin() + newLength), CHAR_TYPE());
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
        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }
        this->d_start_p  = &Imp::s_nullChar;
        this->d_capacity = 0;
    }
    else {
        CHAR_TRAITS::assign(*begin(), CHAR_TYPE());
    }
    this->d_length = 0;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                                iterator begin,
                                                                iterator end)
{
    privateInitDispatch((const_iterator)begin, (const_iterator)end);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                        const_iterator   begin,
                                                        const_iterator   end)
{
    size_type numChars = end - begin;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                          "string<...>(i,j): string too long");
    }
    privateAppendRaw(begin, numChars);
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
                                                            iterator position,
                                                            iterator first,
                                                            iterator last)
{
    privateInsertDispatch(position,
                          const_iterator(first),
                          const_iterator(last));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                       iterator       position,
                                                       const_iterator first,
                                                       const_iterator last)
{
    size_type pos = position - begin();
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
    privateInsertRaw(pos, first, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                           iterator   position,
                                                           INPUT_ITER first,
                                                           INPUT_ITER last)
{
    size_type pos = position - begin();
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
    BSLS_ASSERT_SAFE(length() + numChars <= max_size());

    size_type  newLength  = this->d_length + numChars;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    CHAR_TYPE *tail    = begin() + outPosition;
    size_type  tailLen = this->d_length - outPosition;

    if (newBuffer) {
        // Source and destination cannot overlap, order of next two copies is
        // arbitrary.  Do it left to right to maintain cache consistency.

        CHAR_TRAITS::copy(newBuffer + outPosition, characterString, numChars);
        CHAR_TRAITS::copy(newBuffer + outPosition + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }

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
        const CHAR_TYPE *shifted = (tail < first && last <= tail + tailLen)
                                 ? last    // 'first' shifted by 'numChars'
                                 : first;  // 'no shift

        CHAR_TRAITS::move(tail + numChars, tail, tailLen);
        CHAR_TRAITS::move(tail, shifted, numChars);
        CHAR_TRAITS::assign(*(begin() + newLength), CHAR_TYPE());
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
    BSLS_ASSERT_SAFE(outPosition + outNumChars <= length());
    BSLS_ASSERT_SAFE(length() + numChars - outNumChars <= max_size());

    const difference_type displacement =
                          static_cast<difference_type>(numChars - outNumChars);

    size_type  newLength  = this->d_length + displacement;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    CHAR_TYPE *tail    = begin() + outPosition + outNumChars;
    size_type  tailLen = this->d_length - outPosition - outNumChars;

    if (newBuffer) {
        // Source and destination cannot overlap, order of next two copies is
        // arbitrary.  Do it left to right to maintain cache consistency.

        CHAR_TRAITS::copy(newBuffer + outPosition, characterString, numChars);
        CHAR_TRAITS::copy(newBuffer + outPosition + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
        this->d_length = newLength;
        return *this;                                                 // RETURN
    }

    // Because of possible aliasing, we have to be very careful in which order
    // to move blocks.  There are up to three blocks if 'characterString'
    // overlaps with the tail.

    CHAR_TYPE       *dest  = begin() + outPosition;
    CHAR_TYPE const *first = characterString;
    CHAR_TYPE const *last  = characterString + numChars;

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
    CHAR_TRAITS::assign(*(begin() + newLength), CHAR_TYPE());
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
    BSLS_ASSERT_SAFE(outPosition + outNumChars <= length());

    size_type  newLength  = this->d_length - outNumChars + numChars;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    CHAR_TYPE *tail    = begin() + outPosition + outNumChars;
    size_type  tailLen = this->d_length - outPosition - outNumChars;

    if (newBuffer) {
        CHAR_TYPE *dest = newBuffer + outPosition;

        CHAR_TRAITS::assign(dest, numChars, character);
        CHAR_TRAITS::copy(dest + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
    else {
        CHAR_TYPE *dest = begin() + outPosition;

        CHAR_TRAITS::move(dest + numChars, tail, tailLen);
        CHAR_TRAITS::assign(dest, numChars, character);
        CHAR_TRAITS::assign(*(begin() + newLength), CHAR_TYPE());
    }

    this->d_length = newLength;
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceRaw(
                                                    size_type  outPosition,
                                                    size_type  outNumChars,
                                                    INPUT_ITER first,
                                                    size_type  numChars)
{
    BSLS_ASSERT_SAFE(outPosition + outNumChars <= length());
    BSLS_ASSERT_SAFE(length() + numChars - outNumChars <= max_size());

    difference_type displacement =
                          static_cast<difference_type>(numChars - outNumChars);

    size_type  newLength  = this->d_length + displacement;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    CHAR_TYPE *tail    = begin() + outPosition + outNumChars;
    size_type  tailLen = this->d_length - outPosition - outNumChars;

    if (newBuffer) {
        CHAR_TYPE *dest = newBuffer + outPosition;

        for (size_type i = 0; i != numChars; ++first, ++i) {
            CHAR_TRAITS::assign(*(dest + i), *first);
        }
        CHAR_TRAITS::copy(dest + numChars, tail, tailLen);
        CHAR_TRAITS::assign(*(newBuffer + newLength), CHAR_TYPE());

        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }

        this->d_start_p  = newBuffer;
        this->d_capacity = newStorage;
    }
    else {
        CHAR_TYPE *dest = begin() + outPosition;

        CHAR_TRAITS::move(dest + numChars, tail, tailLen);
        for (size_type i = 0; i != numChars; ++first, ++i) {
            CHAR_TRAITS::assign(*(dest + i), *first);
        }
        CHAR_TRAITS::assign(*(begin() + newLength), CHAR_TYPE());
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
inline
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
    return privateReplaceRaw(outPosition, outNumChars, first, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReserveRaw(
                                                         size_type newCapacity)
{
    BSLS_ASSERT_SAFE(newCapacity <= max_size());

    if (this->d_capacity < newCapacity) {
        // Note: make sure doubling kicks in to provide amortized constant time
        // for 'push_back'.

        size_type newStorage = computeNewCapacity(newCapacity,
                                                  this->d_capacity,
                                                  max_size());
        CHAR_TYPE *newBuffer = privateAllocate(newStorage);

        CHAR_TRAITS::copy(newBuffer, begin(), this->d_length + 1);

        if (!privateIsEmpty()) {
            privateDeallocate(begin());
        }

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

    if (*storage < newCapacity) {
        // Note: make sure doubling kicks in to provide amortized constant time
        // for 'push_back'.

        *storage = computeNewCapacity(newCapacity,
                                      *storage,
                                      max_size());

        CHAR_TYPE *newBuffer = privateAllocate(*storage);

        CHAR_TRAITS::copy(newBuffer, begin(), numChars);
        return newBuffer;                                             // RETURN
    }
    return 0;
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
        CHAR_TRAITS::assign(begin() + this->d_length,
                            newLength - this->d_length,
                            character);
    }
    this->d_length = newLength;
    CHAR_TRAITS::assign(*(begin() + this->d_length), CHAR_TYPE());
    return *this;
}

// PRIVATE ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateCompareRaw(
                                          size_type        lhsPosition,
                                          size_type        lhsNumChars,
                                          const CHAR_TYPE *other,
                                          size_type        otherNumChars) const
{
    BSLS_ASSERT_SAFE(lhsPosition + lhsNumChars <= length());

    size_type numChars = lhsNumChars < otherNumChars ? lhsNumChars
                                                     : otherNumChars;
    int cmpResult = CHAR_TRAITS::compare(begin() + lhsPosition,
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

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
bool
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateIsEmpty() const
{
    return 0 == this->d_capacity;
}

// CREATORS

                // *** 21.3.2 construct/copy/destroy: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                    const ALLOCATOR& allocator)
: Imp(&Imp::s_nullChar)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                 const basic_string& original)
: Imp(&Imp::s_nullChar, original.length(), original.length())
, BloombergLP::bslstl_ContainerBase<allocator_type>(ALLOCATOR())
{
    if (0 < this->d_length) {
        CHAR_TYPE *newBuffer = privateAllocate(this->d_capacity);

        CHAR_TRAITS::copy(newBuffer, original.data(), this->d_length);
        CHAR_TRAITS::assign(*(newBuffer + this->d_length), CHAR_TYPE());

        this->d_start_p  = newBuffer;
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                 const basic_string& original,
                                                 const ALLOCATOR&    allocator)
: Imp(&Imp::s_nullChar, original.length(), original.length())
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    if (0 < this->d_length) {
        CHAR_TYPE *newBuffer = privateAllocate(this->d_capacity);

        CHAR_TRAITS::copy(newBuffer, original.data(), this->d_length);
        CHAR_TRAITS::assign(*(newBuffer + this->d_length), CHAR_TYPE());

        this->d_start_p  = newBuffer;
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                 const basic_string& original,
                                                 size_type           position,
                                                 size_type           numChars,
                                                 const ALLOCATOR&    allocator)
: Imp(&Imp::s_nullChar)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    assign(original, position, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                             const CHAR_TYPE  *characterString,
                                             const ALLOCATOR&  allocator)
: Imp(&Imp::s_nullChar)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    assign(characterString);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                             const CHAR_TYPE  *characterString,
                                             size_type         numChars,
                                             const ALLOCATOR&  allocator)
: Imp(&Imp::s_nullChar)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    assign(characterString, numChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                   size_type         numChars,
                                                   CHAR_TYPE         character,
                                                   const ALLOCATOR&  allocator)
: Imp(&Imp::s_nullChar)
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
: Imp(&Imp::s_nullChar)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    privateInitDispatch(first, last);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename ALLOC2>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
    const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& original,
    const ALLOCATOR&                                              allocator)
: Imp(&Imp::s_nullChar)
, BloombergLP::bslstl_ContainerBase<allocator_type>(allocator)
{
    this->assign(original.data(), original.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::~basic_string()
{
    if (!privateIsEmpty()) {
        privateDeallocate(begin());
    }
    this->d_length = (size_type)-1;  // invalid length
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
    return this->d_start_p;
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
    return assign(characterString, CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
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
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(iterator  position,
                                                      CHAR_TYPE character)
{
    size_type pos = position - begin();
    insert(pos, size_type(1), character);
    return begin() + pos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(iterator  position,
                                                           size_type numChars,
                                                           CHAR_TYPE character)
{
    size_type pos = position - begin();
    insert(pos, numChars, character);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
template <typename INPUT_ITER>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(iterator   position,
                                                           INPUT_ITER first,
                                                           INPUT_ITER last)
{
    privateInsertDispatch(position, first, last);
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
        CHAR_TRAITS::move(begin() + position,
                          begin() + position + numChars,
                          this->d_length - position);
        CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
    }
    return *this;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(iterator position)
{
    BSLS_ASSERT_SAFE(position >= begin());
    BSLS_ASSERT_SAFE(position < end());

    iterator postPosition = position; ++postPosition;
    --this->d_length;

    // Note that 'move' below is 'end() - position' instead of
    // 'end() - postPosition' because 'end()' already incorporated the change
    // in 'd_length'.

    CHAR_TRAITS::move(position, postPosition, end() - position);
    CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
    return position;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(iterator first,
                                                     iterator last)
{
    if (first != last) {
        this->d_length -= last - first;

        // Note that 'move' below is 'end() - first' instead of 'end() - last'
        // because 'end()' already incorporated the change in 'd_length'.

        CHAR_TRAITS::move(first, last, end() - first);
        CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
    }
    return first;
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
                             "string<...>::insert(char*...): string too long");
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
                                                    iterator            first,
                                                    iterator            last,
                                                    const basic_string& string)
{
    size_type outPosition = first - begin();
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
                                              iterator         first,
                                              iterator         last,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(first >= begin());
    BSLS_ASSERT_SAFE(first <= end());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= end());

    size_type outPosition = first - begin();
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
                                              iterator         first,
                                              iterator         last,
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(first >= begin());
    BSLS_ASSERT_SAFE(first <= end());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= end());

    return replace(first,
                   last,
                   characterString,
                   CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(iterator  first,
                                                       iterator  last,
                                                       size_type numChars,
                                                       CHAR_TYPE character)
{
    BSLS_ASSERT_SAFE(first >= begin());
    BSLS_ASSERT_SAFE(first <= end());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= end());

    size_type outPosition = first - begin();
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
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(iterator   first,
                                                       iterator   last,
                                                       INPUT_ITER stringFirst,
                                                       INPUT_ITER stringLast)
{
    BSLS_ASSERT_SAFE(first >= begin());
    BSLS_ASSERT_SAFE(first <= end());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= end());

    size_type outPosition = first - begin();
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
    return this->d_start_p;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::cbegin() const
{
    return this->d_start_p;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::end() const
{
    return this->d_start_p + this->d_length;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::cend() const
{
    return this->d_start_p + this->d_length;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rbegin() const
{
    return const_reverse_iterator(this->d_start_p + this->d_length);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::crbegin() const
{
    return const_reverse_iterator(this->d_start_p + this->d_length);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rend() const
{
    return const_reverse_iterator(this->d_start_p);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::crend() const
{
    return const_reverse_iterator(this->d_start_p);
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
    size_type allocMaxSize = get_allocator().max_size() - 1;
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
    return 0 == this->d_length;
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
    return *begin();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::back() const
{
    return *(end() - 1);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::copy(CHAR_TYPE *characterString,
                                                    size_type  numChars,
                                                    size_type  position) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                       "const string<...>::copy(str,pos,n): invalid position");
    }
    if (numChars > length() - position) {
        numChars = length() - position;
    }
    CHAR_TRAITS::move(characterString, begin() + position, numChars);
    return numChars;
}

                 // *** 21.3.7 string operations: ***

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_pointer
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::c_str() const
{
    return begin();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_pointer
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::data() const
{
    return begin();
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
    size_type remChars = length() - position;
    if (position > length() || numChars > remChars) {
        return npos;                                                  // RETURN
    }
    if (0 == numChars) {
        return position;                                              // RETURN
    }
    const CHAR_TYPE *thisString = begin() + position;
    const CHAR_TYPE *nextString;
    for (remChars -= numChars - 1;
         0 != (nextString = BSLSTL_CHAR_TRAITS::find(thisString,
                                                     remChars,
                                                     *string));
         remChars -= ++nextString - thisString, thisString = nextString)
    {
        if (0 == CHAR_TRAITS::compare(nextString, string, numChars)) {
            return nextString - begin();                              // RETURN
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find (
                                               const CHAR_TYPE *string,
                                               size_type        position) const
{
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
    const CHAR_TYPE *result = BSLSTL_CHAR_TRAITS::find(begin() + position,
                                                       length() - position,
                                                       character);
    return result ? result - begin() : npos;
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
    if (0 == numChars) {
        return position > length() ? length() : position;             // RETURN
    }
    if (numChars <= length()) {
        if (position > length() - numChars) {
            position = length() - numChars;
        }
        const CHAR_TYPE *thisString = begin() + position;
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
    if (0 < numChars && position < length()) {
        for (const CHAR_TYPE *current = begin() + position; current != end();
                                                                   ++current) {
            if (BSLSTL_CHAR_TRAITS::find(characterString, numChars, *current)
                != 0) {
                return current - begin();                             // RETURN
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
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of (
                                            const basic_string& string,
                                            size_type           position) const
{
    return find_last_of(string.data(), position, string.length());
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of (
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    if (0 < numChars && 0 < length()) {
        size_type remChars = position < length() ? position : length() - 1;
        for (const CHAR_TYPE *current = begin() + remChars; ; --current) {
            if (BSLSTL_CHAR_TRAITS::find(
                                        characterString, numChars, *current)) {
                return current - begin();                             // RETURN
            }
            if (current == begin()) {
                break;
            }
        }
    }
    return npos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of (
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    return find_last_of(characterString,
                        position,
                        CHAR_TRAITS::length(characterString));
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of (
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
    if (position < length()) {
        const CHAR_TYPE *last = end();
        for (const CHAR_TYPE *current = begin() + position; current != last;
                                                                   ++current) {
            if (!BSLSTL_CHAR_TRAITS::find(
                                        characterString, numChars, *current)) {
                return current - begin();                             // RETURN
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
    if (0 < length()) {
        size_type remChars = position < length() ? position : length() - 1;
        for (const CHAR_TYPE *current = begin() + remChars; remChars != npos;
                                                       --current, --remChars) {
            if (!BSLSTL_CHAR_TRAITS::find(
                                        characterString, numChars, *current)) {
                return current - begin();                             // RETURN
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
                             other.begin() + otherPosition,
                             otherNumChars);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                                  const CHAR_TYPE *other) const
{
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
    std::size_t len = CHAR_TRAITS::length(lhs);
    return len == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs, rhs.data(), len);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                const CHAR_TYPE*                                 rhs)
{
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
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    const std::size_t minLen = lhs.length() < rhs.length()
                             ? lhs.length() : rhs.length();
    int ret = CHAR_TRAITS::compare(lhs.begin(), rhs.begin(), minLen);
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
    int ret = CHAR_TRAITS::compare(lhs.c_str(), rhs.begin(), minLen);
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
    int ret = CHAR_TRAITS::compare(lhs.begin(), rhs.c_str(), minLen);
    if (0 == ret) {
        return lhs.length() < rhs.length();                           // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const CHAR_TYPE                                  *lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    const std::size_t lhsLen = CHAR_TRAITS::length(lhs);
    const std::size_t minLen = lhsLen < rhs.length() ? lhsLen : rhs.length();
    int ret = CHAR_TRAITS::compare(lhs, rhs.begin(), minLen);
    if (0 == ret) {
        return lhsLen < rhs.length();                                 // RETURN
    }
    return ret < 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const CHAR_TYPE                                  *rhs)
{
    const std::size_t rhsLen = CHAR_TRAITS::length(rhs);
    const std::size_t minLen = rhsLen < lhs.length() ? rhsLen : lhs.length();
    int ret = CHAR_TRAITS::compare(lhs.begin(), rhs, minLen);
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
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
               const CHAR_TYPE                                  *rhs)
{
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
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs)
{
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
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                const CHAR_TYPE                                  *rhs)
{
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
                        std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>* buf,
                        size_t n)
    // Do not use, for internal use by 'operator<<' only.
{
    CHAR_TYPE __f = os.fill();
    size_t __i;
    bool ok = true;

    for (__i = 0; __i < n; ++__i) {
        ok = ok && !CHAR_TRAITS::eq_int_type(buf->sputc(__f),
                                                 CHAR_TRAITS::eof());
    }

    return ok;
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
