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
//  bslstl_StringRefImp: reference wrapper for a generic string
//  bslstl_StringRef: reference wrapper for a 'char' string
//  bslstl_StringRefWide: reference wrapper for a 'wchar_t' string
//
//@AUTHOR: Vladimir Kliatchko (vkliatch), Anthony Comerico (acomeric)
//
//@DESCRIPTION: This component defines two classes, 'bsls_StringRef' and
// 'bsls_StringRefWide', each providing a reference to a non-modifiable string
// value having an external representation.  The type of characters in the
// string value can be either 'char' (for 'bslstl_StringRef') or 'wchar_t' (for
// 'bslstl_StringRefWide').
//
// The invariant of 'bslstl_StringRef' is that it always has a valid
// non-modifiable 'std::string' value, where non-empty string values have an
// external representation.  Empty string values do not need to have an
// external representation.  Most operations on 'bslstl_StringRef' objects have
// reference semantics and apply to the string value: e.g., 'operator=='
// compares string values, not whether 'bslstl_StringRef' objects reference the
// same string object.
//
// The only operations that do not apply to the string value (i.e., that have
// pointer semantics) are copy construction and assignment.  These operations
// produce a 'bslstl_StringRef' object with the same external representation as
// the original 'bslstl_StringRef' object, which is a stronger post-condition
// than having 'operator==' return 'true' for two 'bslstl_StringRef' objects
// that have the same value.
//
// The standard notion of substitutability defined by the 'operator==' does not
// necessarily apply to 'bslstl_StringRef' since 'bslstl_StringRef' is not a
// value-semantic type (because of the external representation).  Therefore
// there can be a plausible sequence of operations applied to two "equal"
// 'bslstl_StringRef' objects that result in objects that don't compare equal.
//
// The string value that is represented by a 'bslstl_StringRef' object need not
// be null-terminated.  Moreover, the string may contain embedded null
// characters.  As such, the string referenced by 'bslstl_StringRef', in
// general, is not a C-style string.  Moreover, the notion of a null-string
// value is not supported.
//
// The address and extent of the string referenced by 'bslstl_StringRef' are
// indicated by the 'data' and 'length' accessors, respectively.  The
// referenced string is also indicated by the 'begin' and 'end' accessors that
// return STL-compatible iterators to the beginning of the string and one
// character past the end of the string, respectively.  An overloaded
// 'operator[]' is also provided for direct by-index access to individual
// characters in the string.
//
// Several overloaded free operators are provided for 'bslstl_StringRef'
// objects (as well as variants for all combinations involving
// 'bslstl_StringRef' and 'std::string', and 'bslst_StringRef' and 'char *')
// for (1) lexicographic comparison of values, and (2) concatenation of values
// (producing an 'std::string'); also provided is an overloaded free
// 'operator<<' for writting the value of a 'bslst_StringRef' object to a
// specified output stream.
//
// The 'bsl::hash' template class is specialized for 'bslstl_StringRef' to
// enable the use of 'bslstl_StringRef' with STL hash containers (e.g.,
// 'bsl::hash_set' and 'bsl::hash_map').
//
///Efficiency and Usage Considerations
///-----------------------------------
// Using 'bslstl_StringRef' to pass strings as function arguments can be
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
// 'bslstl_StringRef':
//..
//  void MyClass::setLabel(const bslstl_StringRef& label)
//  {
//      d_label.assign(label.begin(), label.end());
//  }
//..
// Now:
//..
//  myClassObj.setLabel("hello");
//..
// This call has the side-effect of creating a temporary 'bslstl_StringRef'
// object, which is likely to be more efficient than creating a temporary
// 'bsl::string' (even which is implemented using the short-string
// optimization).  In this case, instead of copying the *contents* of "hello",
// the *address* of the literal string is copied.  In addition, 'bsl::strlen'
// is applied to the string in order to locate its end.  There are *no*
// allocations done on behalf of the temporary object.
//
///Caveats
///-------
// 1) The string referenced by 'bslstl_StringRef' need not be null-terminated,
// and, in fact, may *contain* embedded null characters.  Thus, it is generally
// not valid to pass the address returned by the 'data' accessor to Standard C
// functions that expect a null-terminated string (e.g., 'std::strlen',
// 'std::strcmp', etc.).
//
// 2) The string referenced by 'bslstl_StringRef' must remain valid as long
// as the 'bslstl_StringRef' references that string.  Lifetime issues should be
// carefully considered when, for example, returning a 'bslstl_StringRef'
// object from a function or storing a 'bslstl_StringRef' object in a
// container.
//
// 3) Passing a null string to any function (e.g., 'operator==') without also
// passing a 0 length results in undefined behavior.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Basic Operations
///- - - - - - - - - - - - - -
// The following snippets of code illustrate basic and varied use of the
// 'bslstl_StringRef' class.
//
// First, we define a function, 'getNumBlanks', that returns the number of
// blank (' ') characters contained in the string referenced by a specified
// 'bslstl_StringRef':
//..
//  #include <algorithm>
//
//  int getNumBlanks(const bslstl_StringRef& stringRef)
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
// Then, call 'getNumBlanks' on a default constructed 'bslstl_StringRef':
//..
//  bslstl_StringRef emptyRef;
//  int numBlanks = getNumBlanks(emptyRef);
//  assert(0 == numBlanks);
//
//  assert(""         == emptyRef);
//  assert("anything" >= emptyRef);
//..
// Notice that the behavior a default constructed 'bslstl_StringRef' object
// behaves the same as if it referenced an empty string.
//
// Next, we (implicitly) construct a 'bsl::string' object from
// 'bslstl_StringRef':
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
//                                                  //  length  blanks
//      "O love is the crooked thing,\n"            //    29      5
//      "There is nobody wise enough\n"             //    27      4
//      "To find out all that is in it,\n"          //    31      7
//      "For he would be thinking of love\n"        //    33      6
//      "Till the stars had run away\n"             //    26      5
//      "And the shadows eaten the moon.\n"         //    32      5
//      "Ah, penny, brown penny, brown penny,\n"    //    37      5
//      "One cannot begin it too soon.";            //    29      5
//                                                  //          ----
//                                                  //           42
//
//  numBlanks = getNumBlanks(poem);
//  assert(42 == numBlanks);
//..
// Then, we construct a 'bslstl_StringRef' object, 'line', that refers to only
// the first line of the 'poem':
//..
//  bslstl_StringRef line(poem, 29);
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
//  assert(bslstl_StringRef(poemString) == poemString);
//  assert(bslstl_StringRef(poemString) == poemString.c_str());
//..
// Next, we make a 'bslstl_StringRef' object that refers to a string that will
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
//  numBlanks = getNumBlanks(bslstl_StringRef(poemWithNulls, poemLength));
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

#ifndef INCLUDED_CSTDDEF
#include <cstddef>              // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>              // for 'std::strlen', 'std::memcmp'
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

                    // ====================================
                    // class bslstl_StringRefImp<CHAR_TYPE>
                    // ====================================

template <typename CHAR_TYPE>
class bslstl_StringRefImp : public bslstl_StringRefData<CHAR_TYPE> {
    // This class provides a reference-semantic-like (see below) mechanism that
    // allows 'const' 'std::string' values, which are represented externally as
    // either an 'std::string' or null-terminated c-style string (or parts
    // thereof), to be treated both uniformally and efficiently when passed as
    // an argument to a function in which the string's length will be needed.
    // The interface of this class provides a subset of accessor methods found
    // on 'std::string' (but none of the manipulators) -- all of which apply to
    // the referenced string.  But, because only non-modifiable access is
    // afforded to the referenced string value, each of the manipulators on
    // this type -- assignment in particular -- apply to this string-referrence
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
    typedef bslstl_StringRefData<CHAR_TYPE> Base;

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
    bslstl_StringRefImp();
        // Create an object representing an empty
        // 'std::string' value that is independent of any external
        // representation and with the following attribute values:
        //..
        //  begin() == end()
        //  isEmpty() == true
        //..

    bslstl_StringRefImp(const CHAR_TYPE *data, int length);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'data' address
        // and extends for the specified 'numCharacters.  The external
        // representation must remain valid as long as it is bound to this
        // string reference.   Passing 0 has the same effect as default
        // construction.  The behavior is undefined unless '0 <= length' and,
        // if '0 == data', then '0 == length'.  Note that, like an
        // 'std::string', the 'data' need not be null-terminated and may
        // contain embedded null characters.

    bslstl_StringRefImp(const_iterator begin, const_iterator end);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'begin'
        // iterator and extends up to, but not including, the specified 'end'
        // iterator.  The external representation must remain valid as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'begin <= end'.  Note that, like an 'std::string', the string
        // need not be null-terminated and may contain embedded null
        // characters.

    bslstl_StringRefImp(const CHAR_TYPE *data);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation begins at the specified 'data' address
        // and extends for 'std::strlen(data)' characters.  The external
        // representation must remain valid as long as it is bound to this
        // string reference.  The behavior is undefined unless 'data' is
        // null-terminated.

    bslstl_StringRefImp(const native_std::basic_string<CHAR_TYPE>& str);
    bslstl_StringRefImp(const bsl::basic_string<CHAR_TYPE>& str);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation is defined by the specified 'str'
        // object.  The external representation must remain valid as long as it
        // is bound to this string reference.

    bslstl_StringRefImp(const bslstl_StringRefImp& original);
        // Create a string-reference object having a valid 'std::string' value,
        // whose external representation is defined by the specified 'original'
        // object.  The external representation must remain valid as long as it
        // is bound to this string reference.

    // ~bslstl_StringRefImp() = default;
        // Destroy this object.

    // MANIPULATORS
    bslstl_StringRefImp& operator=(const bslstl_StringRefImp& rhs);
        // Modify this string reference to refer to the same string as the
        // specified 'rhs' string reference and return a reference providing
        // modifiable access to this object.  The assigned object is guaranteed
        // to have values of attributes 'begin' and 'end' equal to the 'rhs'
        // object's attributes.

    void assign(const CHAR_TYPE *data, int length);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for the specified 'length' characters.  The
        // string indicated by 'data' and 'length' must remain valid as
        // long as it is bound to this object.  The behavior is undefined
        // unless '0 <= length' or '0 == data && 0 == length'.  Note that the
        // string need not be null-terminated and may contain embedded null
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
        // address and extending for 'std::strlen(data)' characters.  The
        // string at the 'data' address must remain valid as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'data' is null-terminated.

    void assign(const bsl::basic_string<CHAR_TYPE>& str);
        // Bind this string reference to the specified 'str' string.  The
        // string indicated by 'str' must remain valid as long as it is
        // bound to this object.

    void assign(const bslstl_StringRefImp<CHAR_TYPE>& stringRef);
        // Modify this string reference to refer to the same string as the
        // specified 'stringRef'.  Note, that the string bound to 'stringRef'
        // must remain valid as long as it is bound to this object.

    void reset();
        // Reset this string reference to the default-constructed state
        // having an empty 'std::string' value and the following attribute
        // values:
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
        // this string reference such that '[data(), data()+length())' is a
        // valid half-open range of characters.  Return 0 for an empty string
        // reference object.  Note that the range of characters may not be
        // null-terminated and may contain embedded null characters.

    bool isEmpty() const;
        // Return 'true' if this object represents an empty string value, and
        // 'false' otherwise.  This object represents an empty string value if
        // 'begin() == end()'.

    size_type length() const;
        // Return the length of the string refered to by this object.  Note
        // that this call is equivalent to 'end() - begin()'.
};

// FREE OPERATORS
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator==(const bsl::basic_string<CHAR_TYPE>&   lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator==(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bool operator==(const CHAR_TYPE                      *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE                      *rhs);
    // Return 'true' if the strings refered to by the specified 'lhs' and 'rhs'
    // have the same lexicographic value, and 'false' otherwise.  Two strings
    // have the same lexicographic value if they have the same length, and the
    // respective values at each character position are the same.

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator!=(const bsl::basic_string<CHAR_TYPE>&   lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator!=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bool operator!=(const CHAR_TYPE                      *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE                      *rhs);
    // Return 'true' if the strings refered to by the specified 'lhs' and 'rhs'
    // do not have the same lexicographic value, and 'false' otherwise.  Two
    // strings do not have the same lexicographic value if they do not have the
    // same length, or respective values at any character position are not the
    // same.

template <typename CHAR_TYPE>
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<(const bsl::basic_string<CHAR_TYPE>&   lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bool operator<(const CHAR_TYPE                      *lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const CHAR_TYPE                      *rhs);
    // Return 'true' if the string refered to by the specified 'lhs' is
    // lexicographically less than the string refered to by the specified
    // 'rhs', and 'false' otherwise.

template <typename CHAR_TYPE>
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>(const bsl::basic_string<CHAR_TYPE>&   lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bool operator>(const CHAR_TYPE                      *lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const CHAR_TYPE                      *rhs);
    // Return 'true' if the string refered to by the specified 'lhs' is
    // lexicographically greater than the string refered to by the specified
    // 'rhs', and 'false' otherwise.

template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<=(const bsl::basic_string<CHAR_TYPE>&   lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bool operator<=(const CHAR_TYPE                      *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE                      *rhs);
    // Return 'true' if the string refered to by the specified 'lhs' is
    // lexicographically less than or equal to the string refered to by the
    // specified 'rhs', and 'false' otherwise.

template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>=(const bsl::basic_string<CHAR_TYPE>&   lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bool operator>=(const CHAR_TYPE                      *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE                      *rhs);
    // Return 'true' if the string refered to by the specified 'lhs' is
    // lexicographically greater than or equal to the string refered to by the
    // specified 'rhs', and 'false' otherwise.

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
              const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bsl::basic_string<CHAR_TYPE>&   lhs,
              const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
              const bsl::basic_string<CHAR_TYPE>&   rhs);
template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
              const native_std::basic_string<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const native_std::basic_string<CHAR_TYPE>& lhs,
              const bslstl_StringRefImp<CHAR_TYPE>&      rhs);
template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const CHAR_TYPE                      *lhs,
              const bslstl_StringRefImp<CHAR_TYPE>& rhs);
template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
              const CHAR_TYPE                      *rhs);
    // Return a 'bsl::string' having the value of the concatenation of the
    // strings refered to by the specified 'lhs' and rhs' values.

template <typename CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
    operator<<(std::basic_ostream<CHAR_TYPE>&           stream,
               const bslstl_StringRefImp<CHAR_TYPE>&    stringRef);
    // Write the value of the string bound to the specified 'stringRef' to the
    // specified output 'stream' and return a reference to the modifiable
    // 'stream'.

// ===========================================================================
//                                  TYPEDEFS
// ===========================================================================

typedef bslstl_StringRefImp<char>       bslstl_StringRef;
typedef bslstl_StringRefImp<wchar_t>    bslstl_StringRefWide;

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // -------------------------
                          // class bslstl_StringRefImp
                          // -------------------------

// CREATORS
template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp()
: Base(0, 0)
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp(const CHAR_TYPE *data,
                                                    int              length)
: Base(data, data + length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp(const_iterator begin,
                                                    const_iterator end)
: Base(begin, end)
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp(const CHAR_TYPE *data)
: Base(data, data + std::strlen(data))
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp(
                                       const bsl::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp(
                                const native_std::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::bslstl_StringRefImp(
                         const bslstl_StringRefImp<CHAR_TYPE>& original)
: Base(original.begin(), original.end())
{
}

// MANIPULATORS
template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>&
    bslstl_StringRefImp<CHAR_TYPE>::operator=(const bslstl_StringRefImp& rhs)
{
    Base::operator=(rhs);
    return *this;
}

template <typename CHAR_TYPE>
inline
void bslstl_StringRefImp<CHAR_TYPE>::assign(const CHAR_TYPE *data,
                                            int              length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);

    *this = bslstl_StringRef(data, data + length);
}

template <typename CHAR_TYPE>
inline
void bslstl_StringRefImp<CHAR_TYPE>::assign(const_iterator begin,
                                            const_iterator end)
{
    *this = bslstl_StringRef(begin, end);
}

template <typename CHAR_TYPE>
inline
void bslstl_StringRefImp<CHAR_TYPE>::assign(const CHAR_TYPE *data)
{
    BSLS_ASSERT_SAFE(data);

    *this = bslstl_StringRef(data, data + std::strlen(data));
}

template <typename CHAR_TYPE>
inline
void bslstl_StringRefImp<CHAR_TYPE>::assign(
                                       const bsl::basic_string<CHAR_TYPE>& str)
{
    *this = bslstl_StringRef(str.data(), str.data() + str.length());
}

template <typename CHAR_TYPE>
inline
void bslstl_StringRefImp<CHAR_TYPE>::assign(
                               const bslstl_StringRefImp<CHAR_TYPE>& stringRef)
{
    *this = stringRef;
}

template <typename CHAR_TYPE>
inline
void bslstl_StringRefImp<CHAR_TYPE>::reset()
{
    *this = bslstl_StringRef(0, 0);
}

// ACCESSORS
template <typename CHAR_TYPE>
inline
typename bslstl_StringRefImp<CHAR_TYPE>::const_reference
    bslstl_StringRefImp<CHAR_TYPE>::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < static_cast<int>(end() - begin()));

    return begin()[index];
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefImp<CHAR_TYPE>::
                           operator native_std::basic_string<CHAR_TYPE>() const
{
    return native_std::basic_string<CHAR_TYPE>(begin(), end());
}

template <typename CHAR_TYPE>
inline
typename bslstl_StringRefImp<CHAR_TYPE>::const_iterator
    bslstl_StringRefImp<CHAR_TYPE>::begin() const
{
    return Base::begin();
}

template <typename CHAR_TYPE>
inline
typename bslstl_StringRefImp<CHAR_TYPE>::const_iterator
    bslstl_StringRefImp<CHAR_TYPE>::end() const
{
    return Base::end();
}

template <typename CHAR_TYPE>
inline
const CHAR_TYPE *bslstl_StringRefImp<CHAR_TYPE>::data() const
{
    return begin();
}

template <typename CHAR_TYPE>
inline
bool bslstl_StringRefImp<CHAR_TYPE>::isEmpty() const
{
    return begin() == end();
}

template <typename CHAR_TYPE>
inline
typename bslstl_StringRefImp<CHAR_TYPE>::size_type
    bslstl_StringRefImp<CHAR_TYPE>::length() const
{
    return static_cast<size_type>(end() - begin());
}

// FREE OPERATORS
template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    std::size_t len = lhs.length();

    if (len != rhs.length()) {
        return false;                                                 // RETURN
    }

    return 0 == len || 0 == std::memcmp(lhs.data(), rhs.data(), len);
}

template <typename CHAR_TYPE>
inline
bool operator==(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) == rhs;
}

template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs == bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator==(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) == rhs;
}

template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs == bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator==(const CHAR_TYPE *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) == rhs;
}

template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs == bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) != rhs;
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs != bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) != rhs;
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs != bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const CHAR_TYPE *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) != rhs;
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs != bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename CHAR_TYPE>
inline
bool operator<(const bsl::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) < rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs < bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) < rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs < bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<(const CHAR_TYPE *lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) < rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs < bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                        lhs.begin(), lhs.end());
}

template <typename CHAR_TYPE>
inline
bool operator>(const bsl::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) > rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs > bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) > rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs > bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>(const CHAR_TYPE *lhs,
               const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) > rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
               const CHAR_TYPE *rhs)
{
    return lhs > bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return !(lhs > rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) <= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs <= bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) <= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs <= bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const CHAR_TYPE *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) <= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs <= bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) >= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs >= bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) >= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs >= bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const CHAR_TYPE *lhs,
                const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) >= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs >= bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
              const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    bsl::basic_string<CHAR_TYPE> result;

    result.reserve(lhs.length() + rhs.length());
    result.assign(lhs.begin(), lhs.end());
    result.append(rhs.begin(), rhs.end());

    return result;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bsl::basic_string<CHAR_TYPE>&   lhs,
              const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
              const bsl::basic_string<CHAR_TYPE>&   rhs)
{
    return lhs + bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const native_std::basic_string<CHAR_TYPE>& lhs,
              const bslstl_StringRefImp<CHAR_TYPE>&      rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>&      lhs,
              const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs + bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const CHAR_TYPE                      *lhs,
              const bslstl_StringRefImp<CHAR_TYPE>& rhs)
{
    return bslstl_StringRefImp<CHAR_TYPE>(lhs) + rhs;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringRefImp<CHAR_TYPE>& lhs,
              const CHAR_TYPE                      *rhs)
{
    return lhs + bslstl_StringRefImp<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
    operator<<(std::basic_ostream<CHAR_TYPE>&        stream,
               const bslstl_StringRefImp<CHAR_TYPE>& stringRef)
{
    return stream.write(stringRef.data(), stringRef.length());
}

}  // close namespace BloombergLP

                      // ================================
                      // struct hash<bslstl_StringRefImp>
                      // ================================

namespace bsl {

template <typename CHAR_TYPE>
struct hash<BloombergLP::bslstl_StringRefImp<CHAR_TYPE> > {
    // This template specialization enables use of 'bslstl_StringRefImp'
    // within STL hash containers, for example,
    // 'bsl::hash_set<bslstl_StringRefImp>' and
    // 'bsl::hash_map<bslstl_StringRefImp, Type>' for some type 'Type'.

    // ACCESSORS
    std::size_t
    operator()(const BloombergLP::bslstl_StringRefImp<CHAR_TYPE>&
                                                              stringRef) const;
        // Return a hash corresponding to the string bound to the specified
        // 'stringRef'.
};

// ACCESSORS
template <typename CHAR_TYPE>
std::size_t hash<BloombergLP::bslstl_StringRefImp<CHAR_TYPE> >::
operator()(const BloombergLP::bslstl_StringRefImp<CHAR_TYPE>& stringRef) const
{
    const CHAR_TYPE *string = stringRef.begin();
    const CHAR_TYPE *end    = stringRef.end();

    const unsigned int ADDEND       = 1013904223U;
    const unsigned int MULTIPLICAND =    1664525U;
    const unsigned int MASK         = 4294967295U;

    std::size_t r = 0;

    if (4 == sizeof(int)) {
        while (string != end) {
            r ^= *string++;
            r = r * MULTIPLICAND + ADDEND;
        }
    }
    else {
        while (string != end) {
            r ^= *string++;
            r = (r * MULTIPLICAND + ADDEND) & MASK;
        }
    }

    return r;
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
