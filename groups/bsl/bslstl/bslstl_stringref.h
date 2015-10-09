// bslstl_stringref.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGREF
#define INCLUDED_BSLSTL_STRINGREF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reference to a 'const' string.
//
//@CLASSES:
//   bslstl::StringRefImp: reference wrapper for a generic string
//      bslstl::StringRef: reference wrapper for a 'char' string
//  bslstl::StringRefWide: reference wrapper for a 'wchar_t' string
//
//@DESCRIPTION: This component defines two classes, 'bsls::StringRef' and
// 'bsls::StringRefWide', each providing a reference to a non-modifiable string
// value having an external representation.  The type of characters in the
// string value can be either 'char' (for 'bslstl::StringRef') or 'wchar_t'
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
// The address and extent of the string referenced by 'bslstl::StringRef' are
// indicated by the 'data' and 'length' accessors, respectively.  The
// referenced string is also indicated by the 'begin' and 'end' accessors that
// return STL-compatible iterators to the beginning of the string and one
// character past the end of the string, respectively.  An overloaded
// 'operator[]' is also provided for direct by-index access to individual
// characters in the string.
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
// 'bsl::string' (even which is implemented using the short-string
// optimization).  In this case, instead of copying the *contents* of "hello",
// the *address* of the literal string is copied.  In addition, 'bsl::strlen'
// is applied to the string in order to locate its end.  There are *no*
// allocations done on behalf of the temporary object.
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
//  int getNumBlanks(const bslstl::StringRef& stringRef)
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
//  bslstl::StringRef emptyRef;
//  int numBlanks = getNumBlanks(emptyRef);
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
//  const int poemLength = std::strlen(poem);
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#endif

#ifndef INCLUDED_BSLSTL_STRINGREFDATA
#include <bslstl_stringrefdata.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>              // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslstl {

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
    typedef StringRefData<CHAR_TYPE> Base;

    // PRIVATE ACCESSORS
    void write(std::basic_ostream<CHAR_TYPE>& stream) const;
        // Write the value of this string reference to the specified output
        // 'stream' in the unformatted way.

    template <class OTHER_CHAR_TYPE>
    friend
    std::basic_ostream<OTHER_CHAR_TYPE>& operator<<(
                       std::basic_ostream<OTHER_CHAR_TYPE>&         stream,
                       const bslstl::StringRefImp<OTHER_CHAR_TYPE>& stringRef);

  public:
    // PUBLIC TYPES
    typedef const CHAR_TYPE     value_type;
    typedef const CHAR_TYPE&    reference;
    typedef const CHAR_TYPE&    const_reference;
    typedef const CHAR_TYPE    *iterator;
    typedef const CHAR_TYPE    *const_iterator;
    typedef std::ptrdiff_t      difference_type;
    typedef std::size_t         size_type;
        // Standard Library general container requirements.

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

    StringRefImp(const CHAR_TYPE *data, int length);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'data' address
        // and extends for the specified 'numCharacters.  The external
        // representation must remain valid as long as it is bound to this
        // string reference.  Passing 0 has the same effect as default
        // construction.  The behavior is undefined unless '0 <= length' and,
        // if '0 == data', then '0 == length'.  Note that, like an
        // 'std::string', the 'data' need not be null-terminated and may
        // contain embedded null characters.

    StringRefImp(const_iterator begin, const_iterator end);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'begin'
        // iterator and extends up to, but not including, the specified 'end'
        // iterator.  The external representation must remain valid as long as
        // it is bound to this string reference.  The behavior is undefined
        // unless 'begin <= end'.  Note that, like an 'std::string', the string
        // need not be null-terminated and may contain embedded null
        // characters.

    StringRefImp(const CHAR_TYPE *data);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'data' address
        // and extends for 'std::char_traits<CHAR_TYPE>::length(data)'
        // characters.  The external representation must remain valid as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'data' is null-terminated.

    StringRefImp(const native_std::basic_string<CHAR_TYPE>& str);
    StringRefImp(const bsl::basic_string<CHAR_TYPE>& str);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation is defined by the specified 'str'
        // object.  The external representation must remain valid as long as it
        // is bound to this string reference.

    StringRefImp(const StringRefImp& original);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation is defined by the specified 'original'
        // object.  The external representation must remain valid as long as it
        // is bound to this string reference.

    StringRefImp(const StringRefImp& original,
                 int                 startIndex,
                 int                 numCharacters);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'startIndex'
        // in the specified 'original' string reference, and extends either
        // the specified 'numCharacters' or until the end of the 'original'
        // string reference, whichever comes first.  The external
        // representation must remain valid as long as it is bound to this
        // string reference.  The behavior is undefined unless
        // '0 <= startIndex <= original.length()' and '0 <= numCharacters'.
        // Note that if 'startIndex' is 'original.length()' an empty string
        // reference is returned.

    // ~StringRefImp() = default;
        // Destroy this object.

    // MANIPULATORS
    StringRefImp& operator=(const StringRefImp& rhs);
        // Modify this string reference to refer to the same string as the
        // specified 'rhs' string reference and return a reference providing
        // modifiable access to this object.  The assigned object is guaranteed
        // to have values of attributes 'begin' and 'end' equal to the 'rhs'
        // object's attributes.

    void assign(const CHAR_TYPE *data, int length);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for the specified 'length' characters.  The
        // string indicated by 'data' and 'length' must remain valid as long as
        // it is bound to this object.  The behavior is undefined unless
        // '0 <= length' or '0 == data && 0 == length'.  Note that the string
        // need not be null-terminated and may contain embedded null
        // characters.

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
    const_reference operator[](int index) const;
        // Return a reference providing a non-modifiable access to the
        // character at the specified 'index' in the string bound to this
        // reference.  This reference remains valid as long as the string
        // currently bound to this object remains valid.  The behavior is
        // undefined unless '0 <= index < length()'.

    operator native_std::basic_string<CHAR_TYPE>() const;
        // Return an 'std::basic_string' (synonymous with
        // 'native_std::basic_string') having the value of the string bound to
        // this string reference.

    const_iterator begin() const;
        // Return an STL-compatible iterator to the first character of the
        // string bound to this string reference or 'end()' if the string
        // reference is empty.  The iterator remains valid as long as this
        // object is valid and is bound to the same string.

    const_iterator end() const;
        // Return an STL-compatible iterator one past the last character of the
        // string bound to this string reference or 'begin()' if the string
        // reference is empty.  The iterator remains valid as long as this
        // object is valid and is bound to the same string.

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

    int compare(const StringRefImp& other) const;
        // Compare this and 'other' string objects using a lexicographical
        // comparison and return a negative value if this string is less than
        // 'other' string, a positive value if this string is greater than
        // 'other' string, and 0 if this string is equal to 'other' string.
};

// FREE OPERATORS
template <class CHAR_TYPE>
bool operator==(const StringRefImp<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator==(const bsl::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bool operator==(const StringRefImp<CHAR_TYPE>&             lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator==(const native_std::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bool operator==(const StringRefImp<CHAR_TYPE>&      lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator==(const CHAR_TYPE                *lhs,
                const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bool operator==(const StringRefImp<CHAR_TYPE>&  lhs,
                const CHAR_TYPE                *rhs);
    // Return 'true' if the strings referred to by the specified 'lhs' and
    // 'rhs' have the same lexicographic value, and 'false' otherwise.  Two
    // strings have the same lexicographic value if they have the same length,
    // and the respective values at each character position are the same.

template <class CHAR_TYPE>
bool operator!=(const StringRefImp<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator!=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bool operator!=(const StringRefImp<CHAR_TYPE>&      lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator!=(const StringRefImp<CHAR_TYPE>&             lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator!=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bool operator!=(const CHAR_TYPE                *lhs,
                const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bool operator!=(const StringRefImp<CHAR_TYPE>&  lhs,
                const CHAR_TYPE                *rhs);
    // Return 'true' if the strings referred to by the specified 'lhs' and
    // 'rhs' do not have the same lexicographic value, and 'false' otherwise.
    // Two strings do not have the same lexicographic value if they do not have
    // the same length, or respective values at any character position are not
    // the same.

template <class CHAR_TYPE>
bool operator<(const StringRefImp<CHAR_TYPE>& lhs,
               const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator<(const bsl::basic_string<CHAR_TYPE>& lhs,
               const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bool operator<(const StringRefImp<CHAR_TYPE>&      lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator<(const StringRefImp<CHAR_TYPE>&             lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator<(const native_std::basic_string<CHAR_TYPE>& lhs,
               const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bool operator<(const CHAR_TYPE                *lhs,
               const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bool operator<(const StringRefImp<CHAR_TYPE>&  lhs,
               const CHAR_TYPE                *rhs);
    // Return 'true' if the string referred to by the specified 'lhs' is
    // lexicographically less than the string referred to by the specified
    // 'rhs', and 'false' otherwise.

template <class CHAR_TYPE>
bool operator>(const StringRefImp<CHAR_TYPE>& lhs,
               const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator>(const bsl::basic_string<CHAR_TYPE>& lhs,
               const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bool operator>(const StringRefImp<CHAR_TYPE>&      lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator>(const StringRefImp<CHAR_TYPE>&             lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator>(const native_std::basic_string<CHAR_TYPE>& lhs,
               const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bool operator>(const CHAR_TYPE                *lhs,
               const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bool operator>(const StringRefImp<CHAR_TYPE>&  lhs,
               const CHAR_TYPE                *rhs);
    // Return 'true' if the string referred to by the specified 'lhs' is
    // lexicographically greater than the string referred to by the specified
    // 'rhs', and 'false' otherwise.

template <class CHAR_TYPE>
bool operator<=(const StringRefImp<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator<=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bool operator<=(const StringRefImp<CHAR_TYPE>&      lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator<=(const StringRefImp<CHAR_TYPE>&             lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator<=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bool operator<=(const CHAR_TYPE                *lhs,
                const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bool operator<=(const StringRefImp<CHAR_TYPE>&  lhs,
                const CHAR_TYPE                *rhs);
    // Return 'true' if the string referred to by the specified 'lhs' is
    // lexicographically less than or equal to the string referred to by the
    // specified 'rhs', and 'false' otherwise.

template <class CHAR_TYPE>
bool operator>=(const StringRefImp<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator>=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&      rhs);
template <class CHAR_TYPE>
bool operator>=(const StringRefImp<CHAR_TYPE>&      lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator>=(const StringRefImp<CHAR_TYPE>&             lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bool operator>=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bool operator>=(const CHAR_TYPE                *lhs,
                const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bool operator>=(const StringRefImp<CHAR_TYPE>&  lhs,
                const CHAR_TYPE                *rhs);
    // Return 'true' if the string referred to by the specified 'lhs' is
    // lexicographically greater than or equal to the string referred to by the
    // specified 'rhs', and 'false' otherwise.

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
    operator+(const StringRefImp<CHAR_TYPE>&             lhs,
              const native_std::basic_string<CHAR_TYPE>& rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const native_std::basic_string<CHAR_TYPE>& lhs,
              const StringRefImp<CHAR_TYPE>&             rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const CHAR_TYPE                *lhs,
              const StringRefImp<CHAR_TYPE>&  rhs);
template <class CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const StringRefImp<CHAR_TYPE>&  lhs,
              const CHAR_TYPE                *rhs);
    // Return a 'bsl::string' having the value of the concatenation of the
    // strings referred to by the specified 'lhs' and rhs' values.

template <class CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
    operator<<(std::basic_ostream<CHAR_TYPE>& stream,
               const StringRefImp<CHAR_TYPE>& stringRef);
    // Write the value of the string bound to the specified 'stringRef' to the
    // specified output 'stream' and return a reference to the modifiable
    // 'stream'.

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

// PRIVATE ACCESSORS
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
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const CHAR_TYPE *data,
                                      int              length)
: Base(data, data + length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const_iterator begin,
                                      const_iterator end)
: Base(begin, end)
{
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const CHAR_TYPE *data)
: Base(data, data + native_std::char_traits<CHAR_TYPE>::length(data))
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
StringRefImp<CHAR_TYPE>::StringRefImp(
                                const native_std::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(const StringRefImp<CHAR_TYPE>& original)
: Base(original.begin(), original.end())
{
}

template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>::StringRefImp(
                                  const StringRefImp<CHAR_TYPE>& original,
                                  int                            startIndex,
                                  int                            numCharacters)
: Base(original.begin() + startIndex,
       original.begin() + startIndex +
         native_std::min<native_std::size_t>(numCharacters,
                                             original.length() - startIndex))
{
    BSLS_ASSERT_SAFE(0 <= startIndex);
    BSLS_ASSERT_SAFE(0 <= numCharacters);
    BSLS_ASSERT_SAFE(static_cast<native_std::size_t>(startIndex)
                                                        <= original.length());
}


// MANIPULATORS
template <class CHAR_TYPE>
inline
StringRefImp<CHAR_TYPE>&
    StringRefImp<CHAR_TYPE>::operator=(const StringRefImp& rhs)
{
    Base::operator=(rhs);
    return *this;
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const CHAR_TYPE *data,
                                     int              length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);

    *this = StringRefImp(data, data + length);
}

template <class CHAR_TYPE>
inline
void StringRefImp<CHAR_TYPE>::assign(const_iterator begin,
                                     const_iterator end)
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
                data + native_std::char_traits<CHAR_TYPE>::length(data));
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
    *this = StringRefImp(0, 0);
}

// ACCESSORS
template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_reference
    StringRefImp<CHAR_TYPE>::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < static_cast<int>(end() - begin()));

    return begin()[index];
}

}  // close package namespace

template <class CHAR_TYPE>
inline
bslstl::StringRefImp<CHAR_TYPE>::
                           operator native_std::basic_string<CHAR_TYPE>() const
{
    return native_std::basic_string<CHAR_TYPE>(begin(), end());
}

namespace bslstl {

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_iterator
    StringRefImp<CHAR_TYPE>::begin() const
{
    return Base::begin();
}

template <class CHAR_TYPE>
inline
typename StringRefImp<CHAR_TYPE>::const_iterator
    StringRefImp<CHAR_TYPE>::end() const
{
    return Base::end();
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
    return static_cast<size_type>(end() - begin());
}

template <class CHAR_TYPE>
inline
int StringRefImp<CHAR_TYPE>::compare(
        const StringRefImp<CHAR_TYPE>& other) const
{
    int result = native_std::char_traits<CHAR_TYPE>::compare(
                    this->data(),
                    other.data(),
                    native_std::min(this->length(), other.length()));

    return result != 0 ? result : this->length() - other.length();
}

}  // close package namespace

// FREE OPERATORS
template <class CHAR_TYPE>
inline
bool bslstl::operator==(const StringRefImp<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>& rhs)
{
    return lhs.compare(rhs) == 0;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator==(const bsl::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) == rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator==(const StringRefImp<CHAR_TYPE>&      lhs,
                        const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs == StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator==(const native_std::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) == rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator==(const StringRefImp<CHAR_TYPE>&             lhs,
                        const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs == StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator==(const CHAR_TYPE                *lhs,
                        const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) == rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator==(const StringRefImp<CHAR_TYPE>&  lhs,
                        const CHAR_TYPE                *rhs)
{
    return lhs == StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const StringRefImp<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const bsl::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) != rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const StringRefImp<CHAR_TYPE>&      lhs,
                        const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs != StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const native_std::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) != rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const StringRefImp<CHAR_TYPE>&             lhs,
                        const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs != StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const CHAR_TYPE                *lhs,
                        const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) != rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator!=(const StringRefImp<CHAR_TYPE>&  lhs,
                        const CHAR_TYPE                *rhs)
{
    return lhs != StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const StringRefImp<CHAR_TYPE>& lhs,
                       const StringRefImp<CHAR_TYPE>& rhs)
{
    return lhs.compare(rhs) < 0;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const bsl::basic_string<CHAR_TYPE>& lhs,
                       const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) < rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const StringRefImp<CHAR_TYPE>&      lhs,
                       const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs < StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const native_std::basic_string<CHAR_TYPE>& lhs,
                       const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) < rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const StringRefImp<CHAR_TYPE>&             lhs,
                       const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs < StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const CHAR_TYPE                *lhs,
                       const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) < rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<(const StringRefImp<CHAR_TYPE>&  lhs,
                       const CHAR_TYPE                *rhs)
{
    return lhs < StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const StringRefImp<CHAR_TYPE>& lhs,
                       const StringRefImp<CHAR_TYPE>& rhs)
{
    return lhs.compare(rhs) > 0;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const bsl::basic_string<CHAR_TYPE>& lhs,
                       const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) > rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const StringRefImp<CHAR_TYPE>&      lhs,
                       const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs > StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const native_std::basic_string<CHAR_TYPE>& lhs,
                       const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) > rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const StringRefImp<CHAR_TYPE>&             lhs,
                       const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs > StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const CHAR_TYPE                *lhs,
                       const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) > rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>(const StringRefImp<CHAR_TYPE>&  lhs,
                       const CHAR_TYPE                *rhs)
{
    return lhs > StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const StringRefImp<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>& rhs)
{
    return !(lhs > rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const bsl::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) <= rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const StringRefImp<CHAR_TYPE>&      lhs,
                        const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs <= StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const native_std::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) <= rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const StringRefImp<CHAR_TYPE>&             lhs,
                        const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs <= StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const CHAR_TYPE                *lhs,
                        const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) <= rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator<=(const StringRefImp<CHAR_TYPE>&  lhs,
                        const CHAR_TYPE                *rhs)
{
    return lhs <= StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const StringRefImp<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const bsl::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&      rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) >= rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const StringRefImp<CHAR_TYPE>&      lhs,
                        const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs >= StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const native_std::basic_string<CHAR_TYPE>& lhs,
                        const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) >= rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const StringRefImp<CHAR_TYPE>&             lhs,
                        const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs >= StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const CHAR_TYPE                *lhs,
                        const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) >= rhs;
}

template <class CHAR_TYPE>
inline
bool bslstl::operator>=(const StringRefImp<CHAR_TYPE>&  lhs,
                        const CHAR_TYPE                *rhs)
{
    return lhs >= StringRefImp<CHAR_TYPE>(rhs);
}

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
    bslstl::operator+(const native_std::basic_string<CHAR_TYPE>& lhs,
                      const StringRefImp<CHAR_TYPE>&             rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    bslstl::operator+(const StringRefImp<CHAR_TYPE>&             lhs,
                      const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs + StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    bslstl::operator+(const CHAR_TYPE                *lhs,
                      const StringRefImp<CHAR_TYPE>&  rhs)
{
    return StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <class CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    bslstl::operator+(const StringRefImp<CHAR_TYPE>&  lhs,
                      const CHAR_TYPE                *rhs)
{
    return lhs + StringRefImp<CHAR_TYPE>(rhs);
}

template <class CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
bslstl::operator<<(std::basic_ostream<CHAR_TYPE>& stream,
                   const StringRefImp<CHAR_TYPE>& stringRef)
{
    typedef CHAR_TYPE                                           char_type;
    typedef typename std::basic_ostream<char_type>::ios_base    ios_base;
    typedef typename bslstl::StringRefImp<char_type>::size_type size_type;

    size_type width = static_cast<size_type>(stream.width());
    size_type len = stringRef.length();

    if (len < width) {
        bool leftAdjusted
            = (stream.flags() & ios_base::adjustfield) == ios_base::left;
        char_type fillChar = stream.fill();

        if (leftAdjusted) {
            stringRef.write(stream);
        }

        for (size_type n = 0; n != width - len; ++n) {
            stream.put(fillChar);
        }

        if (!leftAdjusted) {
            stringRef.write(stream);
        }
    }
    else {
        stringRef.write(stream);
    }

    stream.width(0);

    return stream;
}

template <class CHAR_TYPE, class HASHALG>
inline
void bslstl::hashAppend(HASHALG& hashAlg,
                        const StringRefImp<CHAR_TYPE>&  input)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAlg(input.data(), sizeof(CHAR_TYPE)*input.length());
    hashAppend(hashAlg, input.length());
}

}  // close enterprise namespace

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
