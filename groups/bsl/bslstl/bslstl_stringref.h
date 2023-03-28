// bslstl_stringref.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGREF
#define INCLUDED_BSLSTL_STRINGREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reference to a 'const' string.
//
//@CLASSES:
//   bslstl::StringRefImp: reference wrapper for a generic string
//      bslstl::StringRef: reference wrapper for a 'char' string
//  bslstl::StringRefWide: reference wrapper for a 'wchar_t' string
//
//@SEE_ALSO: bdlb_stringrefutil
//
//@DESCRIPTION: This component defines two classes, 'bslstl::StringRef' and
// 'bslstl::StringRefWide', each providing a reference to a non-modifiable
// string value having an external representation.  The type of characters in
// the string value can be either 'char' (for 'bslstl::StringRef') or 'wchar_t'
// (for 'bslstl::StringRefWide').
//
// The invariant of 'bslstl::StringRef' is that it always has a valid
// non-modifiable 'std::string' value, where non-empty string values have an
// external representation.  Empty string values do not need to have an
// external representation.  Most operations on 'bslstl::StringRef' objects
// have reference semantics and apply to the string value: e.g., 'operator=='
// compares string values, not whether 'bslstl::StringRef' objects reference
// the same string object.
//
// The only operations that do not apply to the string value (i.e., that have
// pointer semantics) are copy construction and assignment.  These operations
// produce a 'bslstl::StringRef' object with the same external representation
// as the original 'bslstl::StringRef' object, which is a stronger
// post-condition than having 'operator==' return 'true' for two
// 'bslstl::StringRef' objects that have the same value.
//
// The standard notion of substitutability defined by the 'operator==' does not
// necessarily apply to 'bslstl::StringRef' since 'bslstl::StringRef' is not a
// value-semantic type (because of the external representation).  Therefore
// there can be a plausible sequence of operations applied to two "equal"
// 'bslstl::StringRef' objects that result in objects that don't compare equal.
//
// The string value that is represented by a 'bslstl::StringRef' object need
// not be null-terminated.  Moreover, the string may contain embedded null
// characters.  As such, the string referenced by 'bslstl::StringRef', in
// general, is not a C-style string.  Moreover, the notion of a null-string
// value is not supported.
//
// The address of the string referenced by 'bslstl::StringRef' is indicated by
// the 'data' accessor.  Its extent is indicated by the 'length' and 'size'
// accessors.  The referenced string is also indicated by the 'begin' and 'end'
// accessors that return STL-compatible iterators to the beginning of the
// string and one character past the end of the string, respectively.  An
// overloaded 'operator[]' is also provided for direct by-index access to
// individual characters in the string.
//
// Several overloaded free operators are provided for 'bslstl::StringRef'
// objects (as well as variants for all combinations involving
// 'bslstl::StringRef' and 'std::string', and 'bslstl::StringRef' and 'char *')
// for (1) lexicographic comparison of values, and (2) concatenation of values
// (producing an 'std::string'); also provided is an overloaded free
// 'operator<<' for writing the value of a 'bslstl::StringRef' object to a
// specified output stream.
//
// The 'bsl::hash' template class is specialized for 'bslstl::StringRef' to
// enable the use of 'bslstl::StringRef' with STL hash containers (e.g.,
// 'bsl::unordered_set' and 'bsl::unordered_map').
//
///How to include 'bslstl::StringRef'
///----------------------------------
// To include 'bslstl::StringRef' use '#include <bsl_string.h>' (*not*
// '#include <bslstl_stringref.h>').
//
///Efficiency and Usage Considerations
///-----------------------------------
// Using 'bslstl::StringRef' to pass strings as function arguments can be
// considerably more efficient than passing 'bsl::string' objects by 'const'
// reference.  First, consider a hypothetical class method in which the
// parameter is a reference to a non-modifiable 'bsl::string':
//..
//  void MyClass::setLabel(const bsl::string& label)
//  {
//      d_label = label;  // 'MyClass::d_label' is of type 'bsl::string'
//  }
//..
// Then, consider a typical call to this method:
//..
//  MyClass myClassObj;
//  myClassObj.setLabel("hello");
//..
// As a side-effect of this call, a temporary 'bsl::string' containing a *copy*
// of "hello" is created (using the default allocator), that value is copied to
// 'd_label', and the temporary is eventually destroyed.  The call thus
// requires the string data to be copied twice (as well as a possible
// allocation and deallocation).
//
// Next, consider the same method taking a reference to a non-modifiable
// 'bslstl::StringRef':
//..
//  void MyClass::setLabel(const bslstl::StringRef& label)
//  {
//      d_label.assign(label.begin(), label.end());
//  }
//..
// Now:
//..
//  myClassObj.setLabel("hello");
//..
// This call has the side-effect of creating a temporary 'bslstl::StringRef'
// object, which is likely to be more efficient than creating a temporary
// 'bsl::string' (even when implemented using the short-string optimization).
// In this case, instead of copying the *contents* of "hello", the *address* of
// the literal string is copied.  In addition, 'bsl::strlen' is applied to the
// string in order to locate its end.  There are *no* allocations done on
// behalf of the temporary object.
//
///Caveats
///-------
// 1) The string referenced by 'bslstl::StringRef' need not be null-terminated,
// and, in fact, may *contain* embedded null characters.  Thus, it is generally
// not valid to pass the address returned by the 'data' accessor to Standard C
// functions that expect a null-terminated string (e.g., 'std::strlen',
// 'std::strcmp', etc.).
//
// 2) The string referenced by 'bslstl::StringRef' must remain valid as long as
// the 'bslstl::StringRef' references that string.  Lifetime issues should be
// carefully considered when, for example, returning a 'bslstl::StringRef'
// object from a function or storing a 'bslstl::StringRef' object in a
// container.
//
// 3) Passing a null string to any function (e.g., 'operator==') without also
// passing a 0 length results in undefined behavior.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Operations
///- - - - - - - - - - - - - -
// The following snippets of code illustrate basic and varied use of the
// 'bslstl::StringRef' class.
//
// First, we define a function, 'getNumBlanks', that returns the number of
// blank (' ') characters contained in the string referenced by a specified
// 'bslstl::StringRef':
//..
//  #include <algorithm>
//
//   bslstl::StringRef::size_type
//   getNumBlanks(const bslstl::StringRef& stringRef)
//      // Return the number of blank (' ') characters in the string referenced
//      // by the specified 'stringRef'.
//  {
//      return std::count(stringRef.begin(), stringRef.end(), ' ');
//  }
//..
// Notice that the function delegates the work to the 'std::count' STL
// algorithm.  This delegation is made possible by the STL-compatible iterators
// provided by the 'begin' and 'end' accessors.
//
// Then, call 'getNumBlanks' on a default constructed 'bslstl::StringRef':
//..
//  bslstl::StringRef            emptyRef;
//  bslstl::StringRef::size_type numBlanks = getNumBlanks(emptyRef);
//  assert(0 == numBlanks);
//
//  assert(""         == emptyRef);
//  assert("anything" >= emptyRef);
//..
// Notice that the behavior a default constructed 'bslstl::StringRef' object
// behaves the same as if it referenced an empty string.
//
// Next, we (implicitly) construct a 'bsl::string' object from
// 'bslstl::StringRef':
//..
//  bsl::string empty(emptyRef);
//  assert(0 == empty.size());
//..
// Then, we call 'getNumBlanks' on a string literal and assert that the number
// of blanks returned is as expected:
//..
//  numBlanks = getNumBlanks("Good things come to those who wait.");
//  assert(6 == numBlanks);
//..
// Next, we define a longer string literal, 'poem', that we will use in the
// rest of this usage example:
//..
//  const char poem[] =                  // by William Butler Yeats (1865-1939)
//      |....5....|....5....|....5....|....5....|   //  length  blanks
//                                                  //
//      "O love is the crooked thing,\n"            //    29      5
//      "There is nobody wise enough\n"             //    28      4
//      "To find out all that is in it,\n"          //    31      7
//      "For he would be thinking of love\n"        //    33      6
//      "Till the stars had run away\n"             //    28      5
//      "And the shadows eaten the moon.\n"         //    32      5
//      "Ah, penny, brown penny, brown penny,\n"    //    37      5
//      "One cannot begin it too soon.";            //    29      5
//                                                  //          ----
//                                                  //    total: 42
//
//  numBlanks = getNumBlanks(poem);
//  assert(42 == numBlanks);
//..
// Then, we construct a 'bslstl::StringRef' object, 'line', that refers to only
// the first line of the 'poem':
//..
//  bslstl::StringRef line(poem, 29);
//  numBlanks = getNumBlanks(line);
//
//  assert( 5 == numBlanks);
//  assert(29 == line.length());
//  assert( 0 == std::strncmp(poem, line.data(), line.length()));
//..
// Next, we use the 'assign' method to make 'line' refer to the second line of
// the 'poem':
//..
//  line.assign(poem + 29, poem + 57);
//  numBlanks = getNumBlanks(line);
//  assert(4 == numBlanks);
//  assert((57 - 29) == line.length());
//  assert("There is nobody wise enough\n" == line);
//..
// Then, we call 'getNumBlanks' with a 'bsl::string' initialized to the
// contents of the 'poem':
//..
//  const bsl::string poemString(poem);
//  numBlanks = getNumBlanks(poemString);
//  assert(42 == numBlanks);
//  assert(bslstl::StringRef(poemString) == poemString);
//  assert(bslstl::StringRef(poemString) == poemString.c_str());
//..
// Next, we make a 'bslstl::StringRef' object that refers to a string that will
// be able to hold embedded null characters:
//..
//  char poemWithNulls[512];
//  const bsl::size_t poemLength = std::strlen(poem);
//  assert(poemLength < 512);
//
//  std::memcpy(poemWithNulls, poem, poemLength + 1);
//  assert(0 == std::strcmp(poem, poemWithNulls));
//..
// Now, we replace each occurrence of a '\n' in 'poemWithNulls' with a yielding
// '\0':
//..
//  std::replace(poemWithNulls, poemWithNulls + poemLength, '\n', '\0');
//  assert(0 != std::strcmp(poem, poemWithNulls));
//..
// Finally, we observe that 'poemWithNulls' has the same number of blank
// characters as the original 'poem':
//..
//  numBlanks = getNumBlanks(bslstl::StringRef(poemWithNulls, poemLength));
//  assert(42 == numBlanks);
//..

#include <bslscm_version.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringrefdata.h>

#include <bslmf_enableif.h>
#include <bslmf_isintegral.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_nil.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <algorithm>            // for 'std::min'
#include <cstddef>              // for 'std::size_t'
#include <cstring>
#include <iosfwd>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif

namespace BloombergLP {
namespace bslstl {

#if defined(BSLS_PLATFORM_OS_AIX)

// These 'using's are necessary for a compiler bug on Aix where sometimes when
// 'bslstl::StringRef's are compared, the ADL doesn't look in the namespace of
// the base class for candidates.

using BloombergLP::bslstl_stringview_relops::operator==;
using BloombergLP::bslstl_stringview_relops::operator!=;
using BloombergLP::bslstl_stringview_relops::operator<;
using BloombergLP::bslstl_stringview_relops::operator>;
using BloombergLP::bslstl_stringview_relops::operator<=;
using BloombergLP::bslstl_stringview_relops::operator>=;

#endif

                    // =============================
                    // class StringRefImp<CHAR_TYPE>
                    // =============================

template <class CHAR_TYPE>
class StringRefImp : public StringRefData<CHAR_TYPE> {
    // This class provides a reference-semantic-like (see below) mechanism that
    // allows 'const' 'std::string' values, which are represented externally as
    // either an 'std::string' or null-terminated c-style string (or parts
    // thereof), to be treated both uniformly and efficiently when passed as an
    // argument to a function in which the string's length will be needed.  The
    // interface of this class provides a subset of accessor methods found on
    // 'std::string' (but none of the manipulators) -- all of which apply to
    // the referenced string.  But, because only non-modifiable access is
    // afforded to the referenced string value, each of the manipulators on
    // this type -- assignment in particular -- apply to this string-reference
    // object itself (as if it had pointer semantics).  Hence, this class has a
    // hybrid of reference- and pointer-semantics.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  private:
    // PRIVATE TYPES
    typedef StringRefData<CHAR_TYPE> Base;

  public:
    // PUBLIC TYPES
    typedef const CHAR_TYPE                        value_type;
    typedef const CHAR_TYPE&                       reference;
    typedef const CHAR_TYPE&                       const_reference;
    typedef const CHAR_TYPE                       *iterator;
    typedef const CHAR_TYPE                       *const_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;
    typedef std::ptrdiff_t                         difference_type;
    typedef std::size_t                            size_type;
        // Standard Library general container requirements.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StringRefImp, bsl::is_trivially_copyable);

  private:
    // PRIVATE ACCESSORS
    void write(std::basic_ostream<CHAR_TYPE>& stream) const;
        // Write the value of this string reference to the specified output
        // 'stream' in the unformatted way.

  public:
    // CREATORS
    StringRefImp();
        // Create an object representing an empty 'std::string' value that is
        // independent of any external representation and with the following
        // attribute values:
        //..
        //  begin() == end()
        //  isEmpty() == true
        //..

    template <class INT_TYPE>
    StringRefImp(const CHAR_TYPE *data,
                 INT_TYPE         length,
                 typename bsl::enable_if<bsl::is_integral<INT_TYPE>::value,
                                         bslmf::Nil>::type = bslmf::Nil());
    StringRefImp(const CHAR_TYPE *data, size_type length);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'data' address
        // and extends for the specified 'length'.  The external representation
        // must remain valid as long as it is bound to this string reference.
        // Passing 0 has the same effect as default construction.  The behavior
        // is undefined unless '0 <= length' and, if '0 == data', then
        // '0 == length'.  Note that, like an 'std::string', the 'data' need
        // not be null-terminated and may contain embedded null characters.
        // Note that the template and non-template versions combine to allow
        // various integral and enumeration types to be used for length while
        // preventing '(char *, 0)' initializer arguments from matching the
        // two-iterator constructor below.

    StringRefImp(const_iterator begin, const_iterator end);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'begin'
        // iterator and extends up to, but not including, the specified 'end'
        // iterator.  The external representation must remain valid as long as
        // it is bound to this string reference.  The behavior is undefined
        // unless 'begin <= end'.  Note that, like an 'std::string', the string
        // need not be null-terminated and may contain embedded null
        // characters.

    StringRefImp(const CHAR_TYPE *data);                            // IMPLICIT
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'data' address
        // and extends for 'std::char_traits<CHAR_TYPE>::length(data)'
        // characters.  The external representation must remain valid as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'data' is null-terminated.

    StringRefImp(const bsl::basic_string_view<CHAR_TYPE>& str);     // IMPLICIT
    StringRefImp(const std::basic_string<CHAR_TYPE>& str);          // IMPLICIT
    StringRefImp(const bsl::basic_string<CHAR_TYPE>& str);          // IMPLICIT
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    StringRefImp(const std::pmr::basic_string<CHAR_TYPE>& str);     // IMPLICIT
#endif
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation is defined by the specified 'str'
        // object.  The external representation must remain valid as long as it
        // is bound to this string reference.

//! StringRefImp(const StringRefImp& original) = default;
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation is defined by the specified 'original'
        // object.  The external representation must remain valid as long as it
        // is bound to this string reference.  Note that this trivial copy
        // constructor's definition is compiler generated.

    StringRefImp(const StringRefImp& original,
                 size_type           startIndex,
                 size_type           numCharacters);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'startIndex'
        // in the specified 'original' string reference, and extends either the
        // specified 'numCharacters' or until the end of the 'original' string
        // reference, whichever comes first.  The external representation must
        // remain valid as long as it is bound to this string reference.  The
        // behavior is undefined unless 'startIndex <= original.length()'.
        // Note that if 'startIndex' is 'original.length()' an empty string
        // reference is returned.

//! ~StringRefImp() = default;
        // Destroy this object.

    // MANIPULATORS
//! StringRefImp& operator=(const StringRefImp& rhs) = default;
        // Modify this string reference to refer to the same string as the
        // specified 'rhs' string reference and return a reference providing
        // modifiable access to this object.  The assigned object is guaranteed
        // to have values of attributes 'begin' and 'end' equal to the 'rhs'
        // object's attributes.

    template <class INT_TYPE>
    void assign(const CHAR_TYPE *data,
                INT_TYPE         length,
                typename bsl::enable_if<bsl::is_integral<INT_TYPE>::value,
                                             bslmf::Nil>::type = bslmf::Nil());
    void assign(const CHAR_TYPE *data, size_type length);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for the specified 'length' characters.  The
        // string indicated by 'data' and 'length' must remain valid as long as
        // it is bound to this object.  The behavior is undefined unless
        // '0 <= length' or '0 == data && 0 == length'.  Note that the string
        // need not be null-terminated and may contain embedded null
        // characters.  Note that the template and non-template versions
        // combine to allow various integral and enumeration types to be used
        // for length while preventing '(char *, 0)' initializer arguments from
        // matching the two-iterator overload of 'assign' below.

    void assign(const_iterator begin, const_iterator end);
        // Bind this string reference to the string at the specified 'begin'
        // iterator, extending up to, but not including, the character at the
        // specified 'end' iterator.  The string indicated by 'begin' and 'end'
        // must remain valid as long as it is bound to this object.  The
        // behavior is undefined unless 'begin <= end'.  Note that the string
        // need not be null-terminated and may contain embedded null
        // characters.

    void assign(const CHAR_TYPE *data);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for
        // 'std::char_traits<CHAR_TYPE>::length(data)' characters.  The string
        // at the 'data' address must remain valid as long as it is bound to
        // this string reference.  The behavior is undefined unless 'data' is
        // null-terminated.

    void assign(const bsl::basic_string<CHAR_TYPE>& str);
        // Bind this string reference to the specified 'str' string.  The
        // string indicated by 'str' must remain valid as long as it is bound
        // to this object.

    void assign(const StringRefImp<CHAR_TYPE>& stringRef);
        // Modify this string reference to refer to the same string as the
        // specified 'stringRef'.  Note, that the string bound to 'stringRef'
        // must remain valid as long as it is bound to this object.

    void reset();
        // Reset this string reference to the default-constructed state having
        // an empty 'std::string' value and the following attribute values:
        //..
        //  begin() == end()
        //  isEmpty() == true
        //..

    // ACCESSORS
    const_reference operator[](size_type index) const;
        // Return a reference providing a non-modifiable access to the
        // character at the specified 'index' in the string bound to this
        // reference.  This reference remains valid as long as the string
        // currently bound to this object remains valid.  The behavior is
        // undefined unless '0 <= index < length()'.

    operator std::basic_string<CHAR_TYPE>() const;
        // Return an 'std::basic_string' (synonymous with
        // 'std::basic_string') having the value of the string bound to
        // this string reference.

    const_iterator begin() const;
        // Return an STL-compatible iterator to the first character of the
        // string bound to this string reference or 'end()' if the string
        // reference is empty.  The iterator remains valid as long as this
        // object is valid and is bound to the same string.

    const_iterator end() const;
        // Return an STL-compatible iterator one-past-the-last character of the
        // string bound to this string reference or 'begin()' if the string
        // reference is empty.  The iterator remains valid as long as this
        // object is valid and is bound to the same string.

    const_reverse_iterator rbegin() const;
        // Return an STL-compatible reverse iterator to the last character of
        // the string bound to this string reference or 'rend()' if the string
        // reference is empty.  The iterator remains valid as long as this
        // object is valid and is bound to the same string.

    const_reverse_iterator rend() const;
        // Return an STL-compatible reverse iterator to the
        // prior-to-the-beginning character of the string bound to this string
        // reference or 'rbegin()' if the string reference is empty.  The
        // iterator remains valid as long as this object is valid and is bound
        // to the same string.

    const CHAR_TYPE *data() const;
        // Return the address of the first character of the string bound to
        // this string reference such that '[data() .. data()+length())' is a
        // valid half-open range of characters.  Note that the range of
        // characters might not be null-terminated and may contain embedded
        // null characters.

    bool empty() const;
        // Return 'true' if this object represents an empty string value, and
        // 'false' otherwise.  This object represents an empty string value if
        // 'begin() == end()'.  Note that this method is functionally identical
        // with the 'isEmpty' method and allows developers to avoid distracting
        // syntax differences when 'StringRef' appears in juxtaposition with
        // 'string', which defines 'empty' but not 'isEmpty'.

    bool isEmpty() const;
        // Return 'true' if this object represents an empty string value, and
        // 'false' otherwise.  This object represents an empty string value if
        // 'begin() == end()'.

    size_type length() const;
        // Return the length of the string referred to by this object.  Note
        // that this call is equivalent to 'end() - begin()'.

    size_type size() const;
        // Return the number of characters in the string referred to by this
        // object.  Note that this call is equivalent to 'end() - begin()'.

    int compare(const StringRefImp& other) const;
        // Compare this and the specified 'other' string objects using a
        // lexicographical comparison and return a negative value if this
        // string is less than 'other' string, a positive value if this string
        // is greater than 'other' string, and 0 if this string is equal to
        // 'other' string.
};

                        // ===============================
                        // struct StringRefImp_CompareUtil
                        // ===============================

struct StringRefImp_CompareUtil {
    // [!PRIVATE!] This class provides a namespace for private comparison
    // implementation functions.

    // CLASS METHODS
    template <class CHAR_TYPE>
    static
    int compare(const StringRefImp<CHAR_TYPE>&  a,
                const CHAR_TYPE                *b);
        // Compare the specified string object 'a' with the specified
        // null-terminated C-string 'b' using a lexicographical comparison and
        // return a negative value if 'a' is less than 'b', a positive value if
        // 'a' is greater than 'b', and 0 if 'a' is equal to 'b'.

    template <class CHAR_TYPE>
    static
    bool compareEqual(const StringRefImp<CHAR_TYPE>& a,
                      const StringRefImp<CHAR_TYPE>& b);
        // Return 'true' if the specified 'a' is equal to 'b' and 'false'
        // otherwise.  Note that this function is more efficient than 'compare'
        // for non-lexicographical equality comparisons.

    template <class CHAR_TYPE>
    static
    bool compareEqual(const StringRefImp<CHAR_TYPE>&  a,
                      const CHAR_TYPE                *b);
        // Return 'true' if the specified 'a' is equal to the specified
        // null-terminated C-string 'b' and 'false' otherwise.  Note that this
        // function is more efficient than 'compare' for non-lexicographical
        // equality comparisons.
};

template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const StringRefImp<CHAR_TYPE>& lhs,
          const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const bsl::basic_string<CHAR_TYPE>& lhs,
          const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const StringRefImp<CHAR_TYPE>&      lhs,
          const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const StringRefImp<CHAR_TYPE>&      lhs,
          const std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const std::basic_string<CHAR_TYPE>& lhs,
          const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const CHAR_TYPE                *lhs,
          const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
operator+(const StringRefImp<CHAR_TYPE>&  lhs,
          const CHAR_TYPE                *rhs);
    // Return a 'bsl::string' having the value of the concatenation of the
    // strings referred to by the specified 'lhs' and 'rhs' values.

// FREE FUNCTIONS
template <class CHAR_TYPE, class HASHALG>
void hashAppend(HASHALG& hashAlg, const StringRefImp<CHAR_TYPE>&  input);
    // Pass the specified 'input' to the specified 'hashAlg'

// ============================================================================
//                                  TYPEDEFS
// ============================================================================

typedef StringRefImp<char>       StringRef;
typedef StringRefImp<wchar_t>    StringRefWide;

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ------------------
                          // class StringRefImp
                          // ------------------

// PRIVATE ACCESSOR
template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::write(
                                   std::basic_ostream<CHAR_TYPE>& stream) const
{
    if (data()) {
        stream.write(data(), length());
    }
    else {
        BSLS_ASSERT_SAFE(length() == 0);
    }
}

// CREATORS
template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp()
: Base(0, 0)
{
}

template <class CHAR_TYPE>
template <class INT_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(
                     const CHAR_TYPE *data,
                     INT_TYPE         length,
                     typename bsl::enable_if<bsl::is_integral<INT_TYPE>::value,
                                             bslmf::Nil>::type)
: Base(data, data + length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const CHAR_TYPE *data, size_type length)
: Base(data, data + length)
{
    BSLS_ASSERT_SAFE(data || 0 == length);
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const_iterator begin, const_iterator end)
: Base(begin, end)
{
    BSLS_ASSERT_SAFE((begin == 0) == (end == 0));
    BSLS_ASSERT_SAFE(begin <= end);
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const CHAR_TYPE *data)
: Base(data, data + Base::cStringLength(data))
{
    BSLS_ASSERT_SAFE(data);
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(
                                  const bsl::basic_string_view<CHAR_TYPE>& str)
: Base(str)
{
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const bsl::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const std::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(
                                  const std::pmr::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}
#endif

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(
                                  const StringRefImp<CHAR_TYPE>& original,
                                  size_type                      startIndex,
                                  size_type                      numCharacters)
: Base(original.begin() + startIndex,
       original.begin() + startIndex +
                std::min(numCharacters, original.length() - startIndex))
{
    BSLS_ASSERT_SAFE(startIndex <= original.length());
}


// MANIPULATORS
template <class CHAR_TYPE>
template <class INT_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(
                     const CHAR_TYPE *data,
                     INT_TYPE         length,
                     typename bsl::enable_if<bsl::is_integral<INT_TYPE>::value,
                                         bslmf::Nil>::type)
{
    BSLS_ASSERT_SAFE(data || 0 == length);

    *this = StringRefImp(data, data + length);
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const CHAR_TYPE *data, size_type length)
{
    BSLS_ASSERT_SAFE(data || 0 == length);

    *this = StringRefImp(data, data + length);
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const_iterator begin, const_iterator end)
{
    *this = StringRefImp(begin, end);
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const CHAR_TYPE *data)
{
    BSLS_ASSERT_SAFE(data);

    *this = StringRefImp(
                      data,
                      data + std::char_traits<CHAR_TYPE>::length(data));
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const bsl::basic_string<CHAR_TYPE>& str)
{
    *this = StringRefImp(str.data(), str.data() + str.length());
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const StringRefImp<CHAR_TYPE>& stringRef)
{
    *this = stringRef;
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::reset()
{
    *this = StringRefImp();
}

// ACCESSORS
template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_reference
StringRefImp<CHAR_TYPE>::operator[](size_type index) const
{
    BSLS_ASSERT_SAFE(index < length());

    return begin()[index];
}

}  // close package namespace

template <class CHAR_TYPE>
inline
bslstl::StringRefImp<CHAR_TYPE>::operator std::basic_string<CHAR_TYPE>() const
{
    return std::basic_string<CHAR_TYPE>(begin(), end());
}

namespace bslstl {

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_iterator
    StringRefImp<CHAR_TYPE>::begin() const
{
    return Base::data();
}

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_iterator
    StringRefImp<CHAR_TYPE>::end() const
{
    return Base::data() + Base::size();
}

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_reverse_iterator
    StringRefImp<CHAR_TYPE>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_reverse_iterator
    StringRefImp<CHAR_TYPE>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class CHAR_TYPE>
inline
const CHAR_TYPE *StringRefImp<CHAR_TYPE>::data() const
{
    return begin();
}

template <class CHAR_TYPE>
inline
bool StringRefImp<CHAR_TYPE>::empty() const
{
    return begin() == end();
}

template <class CHAR_TYPE>
inline
bool StringRefImp<CHAR_TYPE>::isEmpty() const
{
    return begin() == end();
}

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::size_type
    StringRefImp<CHAR_TYPE>::length() const
{
    return end() - begin();
}

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::size_type
    StringRefImp<CHAR_TYPE>::size() const
{
    return end() - begin();
}

template <class CHAR_TYPE>
inline
int StringRefImp<CHAR_TYPE>::compare(
                                    const StringRefImp<CHAR_TYPE>& other) const
{
    // Note that, on some platforms but not others, if 'CHAR_TYPE' is signed,
    // char_traits<CHAR_TYPE>::compare' casts the chars to their equivalent
    // sized unsigned type before comparing them.

    int result = std::char_traits<CHAR_TYPE>::compare(
                    this->data(),
                    other.data(),
                    std::min(this->length(), other.length()));

    if (result == 0 && this->length() != other.length()) {
        result = this->length() < other.length() ? -1 : 1;
    }
    return result;
}

                          // ------------------------------
                          // class StringRefImp_CompareUtil
                          // ------------------------------

template <class CHAR_TYPE>
int StringRefImp_CompareUtil::compare(const StringRefImp<CHAR_TYPE>&  a,
                                      const CHAR_TYPE                *b)
{
    // Not inline.

    typedef typename StringRefImp<CHAR_TYPE>::const_iterator const_iterator;

    // Imitate the behavior of the other 'StringRefImp::compare' and
    // 'basic_string::privateCompareRaw' -- if one string is shorter, but they
    // match up to that point, the longer string is always greater, even if the
    // next character of the longer string has a negative value.

    const const_iterator end = a.end();
    for (const_iterator pc = a.begin(); pc < end; ++pc, ++b) {
        if (0 == *b) {
            return +1;                                                // RETURN
        }

        if (*pc != *b) {
            // 'std::char_traits::compare' is a mess, usually
            // implemented with specialized templates, with behavior that
            // varies tremendously depending upon the platform, the compiler,
            // and 'CHAR_TYPE'.  In theory, it should compare individual
            // characters with 'std::char_traits::lt', but in practice
            // that's very often not the case.  Attempting to exactly
            // anticipate its behavior under all circumstances quickly turned
            // into a hopeless, brittle horror show of '#ifdef's and template
            // programming.  So we delegate directly to
            // 'std::char_traits::compare' to compare individual
            // characters known to differ, guaranteeing that compares between
            // 'basic_string's, 'StringRefImp's, and null-terminated 'const
            // CHAR_TYPE *'s all yield matching results.

            return std::char_traits<CHAR_TYPE>::compare(pc,
                                                        b,
                                                        1);           // RETURN
        }
    }

    return *b ? -1 : 0;
}

template <class CHAR_TYPE>
inline
bool StringRefImp_CompareUtil::compareEqual(const StringRefImp<CHAR_TYPE>& a,
                                            const StringRefImp<CHAR_TYPE>& b)
{
    return a.length() == b.length() &&
           (0 == a.length() ||
            0 == std::memcmp(
                     a.data(), b.data(), a.length() * sizeof(CHAR_TYPE)));
}

template <class CHAR_TYPE>
bool StringRefImp_CompareUtil::compareEqual(const StringRefImp<CHAR_TYPE>&  a,
                                            const CHAR_TYPE                *b)
{
    // Not inline.

    typedef typename StringRefImp<CHAR_TYPE>::const_iterator const_iterator;

    const const_iterator end = a.end();
    CHAR_TYPE            c   = *b;
    for (const_iterator pc = a.begin(); pc < end; ++pc, c = *++b) {
        if (0 == c || *pc != c) {
            return false;                                             // RETURN
        }
    }

    return 0 == c;
}

}  // close package namespace

template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const StringRefImp<CHAR_TYPE>& lhs,
                  const StringRefImp<CHAR_TYPE>& rhs)
{
    bsl::basic_string<CHAR_TYPE> result;

    result.reserve(lhs.length() + rhs.length());
    result.assign(lhs.begin(), lhs.end());
    result.append(rhs.begin(), rhs.end());

    return result;
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const bsl::basic_string<CHAR_TYPE>& lhs,
                  const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const StringRefImp<CHAR_TYPE>&      lhs,
                  const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs + StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const std::basic_string<CHAR_TYPE>& lhs,
                  const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const StringRefImp<CHAR_TYPE>&      lhs,
                  const std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs + StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const CHAR_TYPE                *lhs,
                  const StringRefImp<CHAR_TYPE>&  rhs)
{
    // We have to traverse 'lhs' to know how much space to allocate in the
    // result anyway, so best to build a 'StringRefImp' from it.

    return StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
bslstl::operator+(const StringRefImp<CHAR_TYPE>&  lhs,
                  const CHAR_TYPE                *rhs)
{
    // We have to traverse 'rhs' to know how much space to allocate in the
    // result anyway, so best to build a 'StringRefImp' from it.

    return lhs + StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE, class HASHALG>
inline
void bslstl::hashAppend(HASHALG&                       hashAlg,
                        const StringRefImp<CHAR_TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAlg(input.data(), sizeof(CHAR_TYPE)*input.length());
    hashAppend(hashAlg, input.length());
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE)
namespace bslstl {
extern template class bslstl::StringRefImp<char>;
extern template class bslstl::StringRefImp<wchar_t>;

extern template
bsl::basic_string<char>
operator+(const StringRefImp<char>& lhs, const StringRefImp<char>& rhs);

extern template
bsl::basic_string<wchar_t>
operator+(const StringRefImp<wchar_t>& lhs, const StringRefImp<wchar_t>& rhs);

}  // close package namespace
#endif
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslstl_StringRefImp
#undef bslstl_StringRefImp
#endif
#define bslstl_StringRefImp bslstl::StringRefImp
    // This alias is defined for backward compatibility.

#ifdef bslstl_StringRefWide
#undef bslstl_StringRefWide
#endif
#define bslstl_StringRefWide bslstl::StringRefWide
    // This alias is defined for backward compatibility.

#ifdef bslstl_StringRef
#undef bslstl_StringRef
#endif
#define bslstl_StringRef bslstl::StringRef
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY


#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
