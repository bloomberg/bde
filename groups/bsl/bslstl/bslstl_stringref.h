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
//@DESCRIPTION: This component defines classes that provide a reference to a
// sequence of non-modifiable characters, i.e., a 'const' string.  The type of
// characters in the 'const' string can be either 'char' (for
// 'bslstl_StringRef' class) or 'wchar_t' (for 'bslstl_StringRefWide' class).
// Many operations on 'bslstl_StringRef' apply to the referenced string (e.g.,
// 'operator==' compares referenced strings, not whether 'bslstl_StringRef'
// objects reference the same string).  However, since the referenced string is
// 'const', the copy assignment has the pointer semantics.
//
// The string that is referenced by a 'bslstl_StringRef' object need not be
// null-terminated.  Moreover, the string may contain embedded null ('\0')
// characters.  As such, the string referenced by 'bslstl_StringRef', in
// general, is not a C-style string.
//
// The address and extent of the string referenced by 'bslstl_StringRef' are
// indicated by the 'data' and 'length' accessors, respectively.  The
// referenced string is also indicated by the 'begin' and 'end' accessors that
// return STL-compatible iterators to the beginning of the string and one
// character past the end of the string, respectively.  An overloaded
// 'operator[]' is also provided for direct by-index access to individual
// characters in the string.
//
// Several free operators are provided for (1) lexicographical comparison of
// the strings referenced by two 'bslstl_StringRef' objects, (2) concatenation
// of two 'bslstl_StringRef' objects producing a 'bsl::string' containing the
// result, and (3) writing a 'bslstl_StringRef' object to a specified output
// stream.
//
// The 'bsl::hash' template class is specialized for 'bslstl_StringRef' to
// enable the use of 'bslstl_StringRef' with STL hash containers (e.g.,
// 'bsl::hash_set' and 'bsl::hash_map').
//
///Efficiency and Usage Considerations
///-----------------------------------
// Using 'bslstl_StringRef' to pass strings as function arguments can be
// considerably more efficient than passing 'bsl::string' objects by 'const'
// reference.  For example, consider the following hypothetical class method in
// which the parameter is a reference to a non-modifiable 'bsl::string':
//..
//  void MyClass::setLabel(const bsl::string& label)
//  {
//      d_label = label;  // 'MyClass::d_label' is of type 'bsl::string'
//  }
//..
// Then consider a typical call to this method:
//..
//  MyClass myClassObj;
//  myClassObj.setLabel("hello");
//..
// As a side-effect of this call, a temporary 'bsl::string' containing a *copy*
// of "hello" is created (using the default allocator), that value is copied to
// 'd_label', and the temporary is eventually destroyed.  The call thus
// requires two data copies, one allocation, and one deallocation.
//
// Next consider the same method taking a reference to a non-modifiable
// 'bslstl_StringRef':
//..
//  void MyClass::setLabel(const bslstl_StringRef& label)
//  {
//      d_label.assign(label.begin(), label.end());
//  }
//..
// Now this call:
//..
//  myClassObj.setLabel("hello");
//..
// has the side-effect of creating a temporary 'bslstl_StringRef' object
// which is likely to be more efficient than creating a temporary
// 'bsl::string'.  In this case, instead of copying the *contents* of "hello",
// the *address* of the literal string is copied.  In addition, 'bsl::strlen'
// is applied to the string in order to locate its end.  There are *no*
// allocations done on behalf of the temporary object.
//
///Caveats
///-------
// 1) The string referenced by 'bslstl_StringRef' need not be null-terminated,
// and, in fact, may *contain* embedded null ('\0') characters.  Thus, it is
// generally not valid to pass the address returned by the 'data'
// accessor to Standard C functions that expect a null-terminated string (e.g.,
// 'std::strlen', 'std::strcmp', etc.).
//
// 2) The string referenced by 'bslstl_StringRef' must remain valid for as long
// as the 'bslstl_StringRef' references that string.  Lifetime issues should be
// carefully considered when, for example, returning a 'bslstl_StringRef'
// object from a function or storing a 'bslstl_StringRef' object in a
// container.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Operations
///- - - - - - - - - - - - - -
// The following snippets of code illustrate basic and varied use of the
// 'bslstl_StringRef' class.
//
// First, we define a function, 'getNumBlanks', which returns the number of
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
// Then, make several calls to 'getNumBlanks' with strings of various forms to
// illustrate the benefit of having 'getNumBlanks' take a 'bslstl_StringRef'
// argument.
//
// We start by demonstrating the behavior of empty 'bslstl_String' objects,
// which are treated as if they reference an empty string:
//..
//  bslstl_StringRef emptyRef;
//  int numBlanks = getNumBlanks(emptyRef);
//  assert(0 == numBlanks);
//
//  assert(true  == (emptyRef   == ""));
//  assert(false == ("anything" <  emptyRef));
//..
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
// Next, we define a string literal, 'poem', which is a longer string that we
// make use of in the rest of this usage example:
//..
//  const char poem[] =                  // by William Butler Yeats (1865-1939)
//      "O love is the crooked thing,\n"          //  5 blanks
//      "There is nobody wise enough\n"           //  4   "
//      "To find out all that is in it,\n"        //  7   "
//      "For he would be thinking of love\n"      //  6   "
//      "Till the stars had run away\n"           //  5   "
//      "And the shadows eaten the moon.\n"       //  5   "
//      "Ah, penny, brown penny, brown penny,\n"  //  5   "
//      "One cannot begin it too soon.";          //  5   "
//                                                // ---------
//                                                // 42 blanks
//
//  numBlanks = getNumBlanks(poem);
//  assert(42 == numBlanks);
//..
// Then, we construct a 'bslstl_StringRef' object, 'line', that references only
// the first line of the 'poem':
//..
//  bslstl_StringRef line(poem, 29);
//  numBlanks = getNumBlanks(line);
//  assert( 5 == numBlanks);
//  assert(29 == line.length());
//  assert( 0 == std::strncmp(poem, line.data(), line.length()));
//..
// Next, we use the 'assign' method to make 'line' reference the second line of
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
// Now, we make a 'bslstl_StringRef' object that references a string with
// embedded null ('\0') characters.  We populate the 'poemWithNulls' array with
// the contents of 'poem':
//..
//  char poemWithNulls[512];
//  const int poemLength = std::strlen(poem);
//  std::memcpy(poemWithNulls, poem, poemLength + 1);
//  assert(0 == std::strcmp(poem, poemWithNulls));
//..
// Then, we replace each occurrence of '\n' in 'poemWithNulls' with '\0':
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
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>              // for 'std::size_t'
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>              // for 'std::strlen', 'std::memcmp'
#endif

namespace BloombergLP {

                    // ====================================
                    // class bslstl_StringRefImp<CHAR_TYPE>
                    // ====================================

template <typename CHAR_TYPE>
class bslstl_StringRefImp : public bslstl_StringRefData<CHAR_TYPE> {
    // This class, having pointer copy semantics, provides a reference to a
    // sequence of non-modifiable characters, i.e., a 'const' string.  A
    // 'bslstl_StringRefImp' supports a hybrid of reference and pointer
    // semantics.  Many operations on a 'bslstl_StringRefImp' (e.g.,
    // 'operator==') apply to the referenced string without having to
    // dereference the referent.  However, since the referenced string is
    // 'const', the copy assignment has pointer semantics.
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
        // Create an empty string reference which does not reference any
        // string.  'begin' and 'end' accessors of the empty string reference
        // return 0.

    bslstl_StringRefImp(const CHAR_TYPE *data, int length);
        // Create a string reference to the string at the specified 'data'
        // address and extending for the specified 'length' characters.  The
        // string indicated by 'data' and 'length', if any, must remain valid
        // for as long as it is bound to this string reference.  The behavior
        // is undefined unless '0 <= length'.  Note that if 'data' is 0, then
        // 'length' also must be 0.  Also note that the string, if any, need
        // not be null-terminated and may contain null ('\0') characters.

    bslstl_StringRefImp(const_iterator begin, const_iterator end);
        // Create a string reference to the string at the specified 'begin'
        // iterator and extending to, but not including, the character at the
        // specified 'end' iterator.  The string indicated by 'begin' and
        // 'end', if any, must remain valid for as long as it is bound to this
        // string reference.  The behavior is undefined unless 'begin <= end'.
        // Note that if 'begin' is 0, than 'end' must also be 0.  Also note
        // that the string, if any, need not be null-terminated and may contain
        // null ('\0') characters.

    bslstl_StringRefImp(const CHAR_TYPE *data);
        // Create a string reference to the string at the specified 'data'
        // address and extending for 'std::strlen(data)' characters.
        // The string at the 'data' address, if any, must remain valid for as
        // long as it is bound to this string reference.  The behavior is
        // undefined unless 'data' is 0, or 'data' is null-terminated.

    bslstl_StringRefImp(const native_std::basic_string<CHAR_TYPE>& str);
    bslstl_StringRefImp(const bsl::basic_string<CHAR_TYPE>& str);
        // Create a string reference to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    bslstl_StringRefImp(const bslstl_StringRefImp& original);
        // Create a string reference to the same string as that of the
        // specified 'original' string reference.  The string bound to the
        // 'original' string reference, if any, must remain valid for as long
        // as it is bound to this string reference.

    // ~bslstl_StringRefImp() = default;
        // Destroy this string reference.  Note that this destructor is
        // compiler generated.

    // MANIPULATORS
    bslstl_StringRefImp& operator=(const bslstl_StringRefImp& rhs);
        // Modify this string reference to refer to the same string as the
        // specified 'rhs' string reference.  REturn a reference providing
        // modifiable access to this object.  Note, that the string bound to
        // 'rhs', if any, must remain valid for as long as it is bound to this
        // string reference.

    void assign(const CHAR_TYPE *data, int length);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for the specified 'length' characters.
        // The string indicated by 'data' and 'length', if any, must remain
        // valid for as long as it is bound to this string reference.  The
        // behavior is undefined unless '0 <= length'.  Note that if 'data' is
        // 0, then 'length' also must be 0.  Also note that the string, if any,
        // need not be null-terminated and may contain null ('\0') characters.

    void assign(const_iterator begin, const_iterator end);
        // Bind this string reference to the string at the specified 'begin'
        // iterator and extending to, but not including, the character at the
        // specified 'end' iterator.  The string indicated by 'begin' and
        // 'end', if any, must remain valid for as long as it is bound to this
        // string reference.  The behavior is undefined unless 'begin <= end'.
        // Note that if 'begin' is 0, than 'end' must also be 0.  Also note
        // that the string, if any, need not be null-terminated and may contain
        // null ('\0') characters.

    void assign(const CHAR_TYPE *data);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for 'std::strlen(data)' characters.  The
        // string at the 'data' address, if any, must remain valid for as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'data' is not 0, or 'data' is null-terminated.

    void assign(const bsl::basic_string<CHAR_TYPE>& str);
        // Bind this string reference to the specified 'str' string.  The
        // string indicated by 'str' must remain valid for as long as it is
        // bound to this string reference.

    void assign(const bslstl_StringRefImp<CHAR_TYPE>& stringRef);
        // Modify this string reference to refer to the same string as the
        // specified 'stringRef'.  Note, that the string bound to 'stringRef',
        // if any, must remain valid for as long as it is bound to this string
        // reference.

    void reset();
        // Resets this string reference to a default state.  After this method
        // call 'begin', 'end' and length() accessors will return 0, and
        // 'empty' will returns 'true' .

    // ACCESSORS
    const_reference operator[](int index) const;
        // Return a reference to the non-modifiable character at the specified
        // 'index' in the string bound to this string reference.  The reference
        // remains valid for as long as the string currently bound to this
        // string reference remains valid.  The behavior is undefined unless
        // '0 <= index < length()'.

    operator native_std::basic_string<CHAR_TYPE>() const;
        // Return an 'std::basic_string' (synonymous with
        // 'native_std::basic_string') having the value of the string bound to
        // this string reference.

    const_iterator begin() const;
        // Return an (STL-compatible) iterator to the non-modifiable first
        // character of the string bound to this string reference or 'end()' if
        // the string reference is empty.  The iterator remains valid as long
        // as this object is valid and is bound to the same string.

    const_iterator end() const;
        // Return an (STL-compatible) iterator one past the non-modifiable last
        // character of the string bound to this string reference or 'begin()'
        // if the string reference is empty.  The iterator remains valid as
        // long as this object is valid and is bound to the same string.

    const CHAR_TYPE *data() const;
        // Return the address of the non-modifiable first character of the
        // string bound to this string reference such that
        // '[data(), data()+length())' is a valid half-open range of
        // characters.  Return 0 for an empty string reference object.  Note
        // that the range of characters may not be null-terminated and may
        // contain null ('\0') characters.

    bool empty() const;
        // Return 'true' if this object references an empty string
        // ('begin()==end()') and 'false' otherwise.

    size_type length() const;
        // Return the length of the string referenced by this object.  Note
        // that this is equivalent to 'end()-begin()'.
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
    // have the same lexicographic value if they are the same length and the
    // values at each respective character position are the same.  Null strings
    // are treated *as* *if* they were the empty string.

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
    // strings do not have the same lexicographic value if they differ in
    // length or differ in at least one respective character position.  Null
    // strings are treated *as* *if* they were the empty string.

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
    // 'rhs', and 'false' otherwise.  Null strings are treated *as* *if* they
    // were the empty string.

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
    // 'rhs', and 'false' otherwise.  Null strings are treated *as* *if* they
    // were the empty string.

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
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

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
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

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
    // Return an 'bsl::string' having the value of the concatenation of the
    // strings refered to by the specified 'lhs' and rhs'.  Null strings are
    // treated *as* *if* they were the empty string.

template <typename CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
    operator<<(std::basic_ostream<CHAR_TYPE>&           stream,
               const bslstl_StringRefImp<CHAR_TYPE>&  stringRef);
    // Write the string bound to the specified 'stringRef' to the specified
    // output 'stream' and return a reference to the modifiable 'stream'.

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
    *this = bslstl_StringRef(data, data ? data + std::strlen(data) : 0);
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
bool bslstl_StringRefImp<CHAR_TYPE>::empty() const
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
