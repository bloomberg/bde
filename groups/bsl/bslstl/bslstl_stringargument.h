// bslstl_stringargument.h                                            -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGARGUMENT
#define INCLUDED_BSLSTL_STRINGARGUMENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reference to a 'const' string.
//
//@CLASSES:
//  bslstl_StringArgument_Base: reference wrapper for a 'const' basic_string
//  bslstl_StringArgument: typedef for the 'char' character type
//  bslstl_WStringArgument: typedef for the 'wchar_t' character type
//
//@AUTHOR: Vladimir Kliatchko (vkliatch), Anthony Comerico (acomeric)
//
//@DESCRIPTION: The 'bslstl_StringArgument' class defined in this component,
// having a hybrid of reference and pointer semantics, provides a reference to
// a sequence of non-modifiable characters, i.e., a 'const' string.  Many
// operations on 'bslstl_StringArgument' (e.g., 'operator==') apply to the
// referenced string without having to dereference the referent.
// However, since the bound string is 'const', copy assignment favors a
// pointer-semantic rebinding.
//
// The string that is bound to a 'bslstl_StringArgument' need not be
// null-terminated.  Moreover, the string may contain null ('\0') characters.
// As such, the string bound to a 'bslstl_StringArgument', in general, is not a
// C-style string.  Note that the bound string is not managed in any way by a
// 'bslstl_StringArgument'.
//
// The address and extent of the string bound to a 'bslstl_StringArgument' are
// indicated by the 'data' and 'length' accessors, respectively.  The bound
// string is also indicated by the 'begin' and 'end' accessors that return
// STL-compatible iterators to the beginning of the string and one character
// past the end of the string, respectively.  An overloaded 'operator[]' is
// also provided for direct by-index access to individual characters in the
// bound string.
//
// Several constructors are provided for efficiently creating instances of
// 'bslstl_StringArgument'.  Corresponding to the accessors just introduced,
// there is a constructor that takes a 'const char *' address and a length.
// Similarly, there is a constructor that takes begin and end iterators of type
// 'bslstl_StringArgument::const_iterator'.  Two conversion constructors also
// are provided that take 'const char *' and 'const bsl::string&' arguments.
// These conveniently allow for automatic conversion to 'bslstl_StringArgument'
// in contexts where required.  Note that the 'const char *' argument in this
// case *must* be null-terminated.  Lastly, there is a default constructor and
// a copy constructor.
//
// Once constructed, a 'bslstl_StringArgument' may be bound to another string
// through assignment or by calling the overloaded 'assign' methods.  Each of
// the constructors, except for the default constructor, has a corresponding
// 'assign' method that takes the same arguments as the constructor.
//
// Several free operators are provided for (1) lexicographical comparison of
// the strings bound to two 'bslstl_StringArgument' instances, (2)
// concatenation of the strings bound to two 'bslstl_StringArgument' instances
// producing an 'bsl::string' containing the result, and (3) writing the string
// bound to a 'bslstl_StringArgument' to a specified output stream.
//
// Lastly, the 'bsl::hash' template class is specialized for
// 'bslstl_StringArgument' to enable use of 'bslstl_StringArgument' within STL
// hash containers (e.g., 'bsl::hash_set<bslstl_StringArgument>' and
// 'bsl::hash_map<bslstl_StringArgument, Type>' for some type 'Type').
//
///Efficiency and Usage Considerations
///-----------------------------------
// Passing strings by 'bslstl_StringArgument' can be considerably more
// efficient even than passing by 'const' reference.  (See the "Caveats"
// section below.)  For example, consider the following hypothetical class
// method in which the parameter is a reference to a non-modifiable
// 'bsl::string':
//..
//  void MyClass::setLabel(const bsl::string& label)
//  {
//      d_label = label;  // 'MyClass::d_label' is of type 'bsl::string'
//  }
//..
// and consider a typical call to this method:
//..
//  MyClass myClassObj;
//  myClassObj.setLabel("hello");
//..
// As a side-effect of this call, a temporary 'bsl::string' containing a *copy*
// of "hello" is created (using the default allocator), that value is copied to
// 'd_label', and the temporary is eventually destroyed.  The call thus
// requires two data copies, one allocation, and one deallocation.  Note that
// an additional allocation (and possibly a deallocation) would be needed if
// the capacity of 'd_label' is not sufficient to store "hello".
//
// Next consider the same method redefined to take a reference to a
// non-modifiable 'bslstl_StringArgument':
//..
//  void MyClass::setLabel(const bslstl_StringArgument& label)
//  {
//      d_label.assign(label.data(), label.length());
//  }
//..
// Now this call:
//..
//  myClassObj.setLabel("hello");
//..
// has the side-effect of creating a temporary 'bslstl_StringArgument' object
// which is likely to be substantially more efficient than creating a temporary
// 'bsl::string'.  In this case, instead of copying the *contents* of "hello",
// the *address* of the literal string is copied.  In addition, 'bsl::strlen'
// is applied to the string in order to locate its end.  There are *no*
// allocations done on behalf of the temporary object.
//
// If the length of the string is known at the point of the call to 'setLabel'
// (or if the address one-past the last character is known), then the call can
// be made even more efficiently (while, of course, sacrificing the automatic
// conversion to 'bslstl_StringArgument').  The next code snippet creates two
// string references bound to a string that expressly is *not* null-terminated:
//..
//  const char HELLO[]   = { 'h', 'e', 'l', 'l', 'o' };
//  const int  HELLO_LEN = sizeof HELLO;
//  myClassObj.setLabel(bslstl_StringArgument(HELLO, HELLO_LEN));
//
//  myClassObj.setLabel(bslstl_StringArgument(HELLO, HELLO + HELLO_LEN));
//..
//
///Caveats
///-------
// 1) The string bound to a 'bslstl_StringArgument' need not be
// null-terminated, and, in fact, may *contain* null ('\0') characters.  Thus,
// it is generally not valid to pass the address returned by the 'data'
// accessor to Standard C functions that expect a null-terminated string (e.g.,
// 'std::strlen', 'std::strcmp', etc.).
//
// 2) However, if a 'bslstl_StringArgument' is bound to a string via the
// one-argument constructor or 'assign' method that takes a 'const char *'
// parameter, then the string *must* be null-terminated.  'std::strlen' is
// applied to the string in these two cases (only) in order to locate the end
// of the string.
//
// 3) The string bound to a 'bslstl_StringArgument' must remain valid for as
// long as the 'bslstl_StringArgument' is bound to that string.  Lifetime
// issues should be carefully considered when, for example, returning a
// 'bslstl_StringArgument' from a function or storing a 'bslstl_StringArgument'
// in a container.
//
///Thread-Safety
///-------------
// It is safe to access the same 'bslstl_StringArgument' object from multiple
// threads.  It is safe to access and manipulate different
// 'bslstl_StringArgument' objects that are bound to the same string from
// multiple threads.  It is unsafe to manipulate the same
// 'bslstl_StringArgument' object from multiple threads.
//
///Usage
///-----
// The following snippets of code illustrate basic and varied use of the
// 'bslstl_StringArgument' class.  The sample code uses the 'getNumBlanks'
// function defined below, which returns the number of blank (' ') characters
// contained in the string that is bound to a specified
// 'bslstl_StringArgument'.  The function delegates the work to the
// 'std::count' STL algorithm.  This delegation is made possible by the
// STL-compatible iterators provided by 'bslstl_StringArgument' via the 'begin'
// and 'end' accessors:
//..
//  #include <algorithm>
//
//  int getNumBlanks(const bslstl_StringArgument& stringArg)
//      // Return the number of blank (' ') characters in the string bound to
//      // the specified 'stringArg'.
//  {
//      return std::count(stringArg.begin(), stringArg.end(), ' ');
//  }
//..
// We make several calls to 'getNumBlanks' with strings of various forms to
// illustrate the benefit of having 'getNumBlanks' take a
// 'bslstl_StringArgument' argument.  First we verify tolerance of
// 'bslstl_StringArgument' objects that are unbound:
//..
//  bslstl_StringArgument unbound;
//  numBlanks = getNumBlanks(unbound);
//  assert(0 == numBlanks);
//..
// In most contexts, unbound 'bslstl_StringArgument' objects are treated as if
// they are bound to the empty string:
//..
//  assert(true  == (unbound    == ""));
//  assert(false == ("anything" <  unbound));
//..
// Similarly, the a 'bsl::string' object can be (implicitly) constructed from
// 'bslstl_StringArgument':
//..
//  bsl::string empty(unbound);
//  assert(0 == empty.size());
//..
// Next we call 'getNumBlanks' on a string literal and assert that the number
// of blanks returned is as expected:
//..
//  numBlanks = getNumBlanks("Good things come to those who wait.");
//  assert(6 == numBlanks);
//..
// The following 'poem' is a longer string that we make use of in the rest of
// this usage example:
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
// To begin, we bind just the first line of 'poem' to the 'line' object of type
// 'bslstl_StringArgument':
//..
//  bslstl_StringArgument line(poem, 29);
//  numBlanks = getNumBlanks(line);
//  assert( 5 == numBlanks);
//  assert(29 == line.length());
//  assert( 0 == std::strncmp(poem, line.data(), line.length()));
//..
// Next, we use the 'assign' method to rebind 'line' to the second line of the
// 'poem':
//..
//  line.assign(poem + 29, poem + 57);
//  numBlanks = getNumBlanks(line);
//  assert(4 == numBlanks);
//  assert((57 - 29) == line.length());
//  assert("There is nobody wise enough\n" == line);
//..
// Now we call 'getNumBlanks' with an 'bsl::string' initialized to the contents
// of the 'poem':
//..
//  const bsl::string poemString(poem);
//  numBlanks = getNumBlanks(poemString);
//  assert(42 == numBlanks);
//  assert(bslstl_StringArgument(poemString) == poemString);
//  assert(bslstl_StringArgument(poemString) == poemString.c_str());
//..
// Finally, we illustrate binding a 'bslstl_StringArgument' to a string that
// contains null ('\0') characters.  First we populate the 'poemWithNulls'
// array with the contents of 'poem':
//..
//  char poemWithNulls[512];
//  const int poemLength = std::strlen(poem);
//  std::memcpy(poemWithNulls, poem, poemLength + 1);
//  assert(0 == std::strcmp(poem, poemWithNulls));
//..
// Next we replace each occurrence of '\n' in 'poemWithNulls' with '\0':
//..
//  std::replace(poemWithNulls, poemWithNulls + poemLength, '\n', '\0');
//  assert(0 != std::strcmp(poem, poemWithNulls));
//..
// We now observe that 'poemWithNulls' has the same number of blank characters
// as the original 'poem':
//..
//  numBlanks = getNumBlanks(bslstl_StringArgument(poemWithNulls, poemLength));
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

#ifndef INCLUDED_BSLSTL_STRINGARGUMENTDATA
#include <bslstl_stringargumentdata.h>
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

                      // ================================
                      // class bslstl_StringArgument_Base
                      // ================================

template <typename CHAR_TYPE>
class bslstl_StringArgument_Base
    : public bslstl_StringArgumentData<CHAR_TYPE>
    // This class, having non-standard copy semantics, provides a reference to
    // a sequence of non-modifiable characters, i.e., a 'const' string.  A
    // 'bslstl_StringArgument_Base' supports a hybrid of reference and pointer
    // semantics.  Many operations on a 'bslstl_StringArgument_Base' (e.g.,
    // 'operator==') apply to the referenced (or bound) string without having
    // to dereference the referent.  However, since the bound string is
    // 'const', copy assignment favors a pointer- semantic rebind.
    //
    // The string bound to a 'bslstl_StringArgument_Base' need not be
    // null-terminated, and may contain null ('\0') characters.  The bound
    // string, if any, must remain valid for as long as the
    // 'bslstl_StringArgument_Base' is bound to that string.  After
    // construction, a 'bslstl_StringArgument_Base' may be bound to another
    // string (via 'operator=' or 'assign').
{
  private:
    typedef bslstl_StringArgumentData<CHAR_TYPE> Base;

  public:
    // PUBLIC TYPES
    typedef const CHAR_TYPE *iterator;        // type of non-'const' iterator
    typedef const CHAR_TYPE *const_iterator;  // type of 'const' iterator
    typedef const CHAR_TYPE  value_type;      // type of objects iterated over
        // Iterator types ('iterator' and 'const_iterator') and value type
        // ('value_type') for STL-compatible iterators.

  public:
    // CREATORS
    bslstl_StringArgument_Base();
        // Create an unbound string reference.  An unbound string reference is
        // not bound to any string.

    bslstl_StringArgument_Base(const CHAR_TYPE *data, int length);
        // Create a string reference bound to the string at the specified
        // 'data' address and extending for the specified 'length' characters.
        // The string indicated by 'data' and 'length', if any, must remain
        // valid for as long as it is bound to this string reference.  The
        // behavior is undefined unless '0 <= length'.  Note that if 'data' is
        // 0, then 'length' also must be 0.  Also note that the bound string,
        // if any, need not be null-terminated and may contain null ('\0')
        // characters.

    bslstl_StringArgument_Base(const_iterator begin, const_iterator end);
        // Create a string reference bound to the string at the specified
        // 'begin' iterator and extending to, but not including, the character
        // at the specified 'end' iterator.  The string indicated by 'begin'
        // and 'end', if any, must remain valid for as long as it is bound to
        // this string reference.  The behavior is undefined unless 'begin <=
        // end'.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    bslstl_StringArgument_Base(const CHAR_TYPE *data);
        // Create a string reference bound to the string at the specified
        // 'data' address and extending for 'std::strlen(data)' characters.
        // The string at the 'data' address, if any, must remain valid for as
        // long as it is bound to this string reference.  The behavior is
        // undefined unless 'data' is 0, or 'data' is null-terminated.

    bslstl_StringArgument_Base(const native_std::basic_string<CHAR_TYPE>& str);
    bslstl_StringArgument_Base(const bsl::basic_string<CHAR_TYPE>& str);
        // Create a string reference bound to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    bslstl_StringArgument_Base(const bslstl_StringArgument_Base& original);
        // Create a string reference bound to the same string as that of the
        // specified 'original' string reference, or an unbound string
        // reference if 'original' is unbound.  The string bound to the
        // 'original' string reference, if any, must remain valid for as long
        // as it is bound to this string reference.

    // ~bslstl_StringArgument_Base();
        // Destroy this string reference.  Note that the string bound to this
        // string reference, if any, is not affected.  Also note that this
        // trivial destructor is generated by the compiler.

    // MANIPULATORS
    bslstl_StringArgument_Base& operator=(const bslstl_StringArgument_Base& rhs);
        // Assign to this string reference the binding of the specified 'rhs'
        // string reference.  The string bound to 'rhs', if any, must remain
        // valid for as long as it is bound to this string reference.

    void assign(const CHAR_TYPE *data, int length);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for the specified 'length' characters, or make
        // this string reference unbound if both 'data' and 'length' are 0.
        // The string indicated by 'data' and 'length', if any, must remain
        // valid for as long as it is bound to this string reference.  The
        // behavior is undefined unless '0 <= length'.  Note that if 'data' is
        // 0, then 'length' also must be 0.  Also note that the bound string,
        // if any, need not be null-terminated and may contain null ('\0')
        // characters.

    void assign(const_iterator begin, const_iterator end);
        // Bind this string reference to the string at the specified 'begin'
        // iterator and extending to, but not including, the character at the
        // specified 'end' iterator.  The string indicated by 'begin' and
        // 'end', if any, must remain valid for as long as it is bound to this
        // string reference.  The behavior is undefined unless 'begin <= end'.
        // Note that the bound string, if any, need not be null-terminated and
        // may contain null ('\0') characters.

    void assign(const CHAR_TYPE *data);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for 'std::strlen(data)' characters.  The
        // string at the 'data' address, if any, must remain valid for as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'data' is not 0, or 'data' is null-terminated.

    void assign(const bsl::basic_string<CHAR_TYPE>& str);
        // Bind this string reference to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    void assign(const bslstl_StringArgument_Base<CHAR_TYPE>& stringArg);
        // Assign to this string reference the binding of the specified
        // 'stringArg'.  The string bound to 'stringArg', if any, must remain
        // valid for as long as it is bound to this string reference.

    void clear();
        // Make this string reference unbound.

    // ACCESSORS
    const CHAR_TYPE& operator[](int index) const;
        // Return a reference to the non-modifiable character at the specified
        // 'index' in the string bound to this string reference.  The reference
        // remains valid for as long as the string currently bound to this
        // string reference remains valid.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that this operator must not be used
        // on an unbound string reference.

    operator native_std::basic_string<CHAR_TYPE>() const;
        // Return an 'std::basic_string' (synonymous with
        // 'native_std::basic_string') having the value of the string bound to
        // this string reference.

    const_iterator begin() const;
        // Return an (STL-compatible) iterator to the non-modifiable first
        // character of the string bound to this string reference, or 0 if this
        // string reference is unbound.  The iterator, if non-null, remains
        // valid for as long as the string currently bound to this string
        // reference remains valid.  Note that 'begin() == end()' if the string
        // bound to this string reference is the empty string, or if this
        // string reference is unbound.

    const_iterator end() const;
        // Return an (STL-compatible) iterator one past the non-modifiable last
        // character of the string bound to this string reference, or 0 if this
        // string reference is unbound.  The iterator, if non-null, remains
        // valid for as long as the string currently bound to this string
        // reference remains valid.  Note that 'begin() == end()' if the string
        // bound to this string reference is the empty string, or if this
        // string reference is unbound.

    const CHAR_TYPE *data() const;
        // Return the address of the non-modifiable leading character (if any)
        // of the string bound to this string reference, or 0 if this string
        // reference is unbound.  The address, if non-null, remains valid for
        // as long as the string currently bound to this string reference
        // remains valid.  Note that this method is equivalent to 'begin'.
        // Also note that the bound string, if any, need not be null-terminated
        // and may contain null ('\0') characters.

    bool empty() const;
        // Return 'true' if this string reference is bound to a string of
        // length 0 or if it is unbound, and 'false' otherwise.

    int length() const;
        // Return the length of the string bound to this string reference, and
        // 0 if this string reference is unbound.
};

// FREE OPERATORS
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator==(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator==(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator==(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs);
    // Return 'true' if the strings indicated by the specified 'lhs' and 'rhs'
    // have the same lexicographic value, and 'false' otherwise.  Two strings
    // have the same lexicographic value if they are the same length and the
    // values at each respective character position are the same.  Null strings
    // are treated *as* *if* they were the empty string.

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator!=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator!=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator!=(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs);
    // Return 'true' if the strings indicated by the specified 'lhs' and 'rhs'
    // do not have the same lexicographic value, and 'false' otherwise.  Two
    // strings do not have the same lexicographic value if they differ in
    // length or differ in at least one respective character position.  Null
    // strings are treated *as* *if* they were the empty string.

template <typename CHAR_TYPE>
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<(const bsl::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<(const CHAR_TYPE *lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const CHAR_TYPE *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically less than the string indicated by the specified 'rhs',
    // and 'false' otherwise.  Null strings are treated *as* *if* they were the
    // empty string.

template <typename CHAR_TYPE>
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>(const bsl::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>(const CHAR_TYPE *lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const CHAR_TYPE *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically greater than the string indicated by the specified
    // 'rhs', and 'false' otherwise.  Null strings are treated *as* *if* they
    // were the empty string.

template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<=(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically less than or equal to the string indicated by the
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>=(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically greater than or equal to the string indicated by the
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bsl::basic_string<CHAR_TYPE>& lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const bsl::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const native_std::basic_string<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const native_std::basic_string<CHAR_TYPE>& lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const CHAR_TYPE *lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs);

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const CHAR_TYPE *rhs);
    // Return an 'bsl::string' having the value of the concatenation of the
    // strings indicated by the specified 'lhs' and rhs'.  Null strings are
    // treated *as* *if* they were the empty string.

template <typename CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
    operator<<(std::basic_ostream<CHAR_TYPE>&           stream,
               const bslstl_StringArgument_Base<CHAR_TYPE>&  stringArg);
    // Write the string bound to the specified 'stringArg' to the specified
    // output 'stream' and return a reference to the modifiable 'stream'.

// ===========================================================================
//                                TYPEDEFS
// ===========================================================================

typedef bslstl_StringArgument_Base<char> bslstl_StringArgument;
typedef bslstl_StringArgument_Base<wchar_t> bslstl_WStringArgument;

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // --------------------------------
                      // class bslstl_StringArgument_Base
                      // --------------------------------

// CREATORS
template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base()
: Base(0, 0)
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base(
        const CHAR_TYPE *data,
        int length)
: Base(data, data + length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base(
        const_iterator begin,
        const_iterator end)
: Base(begin, end)
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base(
                                                         const CHAR_TYPE *data)
: Base(data, data + std::strlen(data))
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base(
                                       const bsl::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base(
                                const native_std::basic_string<CHAR_TYPE>& str)
: Base(str.data(), str.data() + str.length())
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::bslstl_StringArgument_Base(
                         const bslstl_StringArgument_Base<CHAR_TYPE>& original)
: Base(original.begin(), original.end())
{
}

// MANIPULATORS
template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>&
    bslstl_StringArgument_Base<CHAR_TYPE>::operator=(
                                         const bslstl_StringArgument_Base& rhs)
{
    Base::operator=(rhs);
    return *this;
}

template <typename CHAR_TYPE>
inline
void bslstl_StringArgument_Base<CHAR_TYPE>::assign(const CHAR_TYPE *data,
                                                   int length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);

    *this = bslstl_StringArgument(data, data + length);
}

template <typename CHAR_TYPE>
inline
void bslstl_StringArgument_Base<CHAR_TYPE>::assign(const_iterator begin,
                                                  const_iterator end)
{
    *this = bslstl_StringArgument(begin, end);
}

template <typename CHAR_TYPE>
inline
void bslstl_StringArgument_Base<CHAR_TYPE>::assign(const CHAR_TYPE *data)
{
    *this = bslstl_StringArgument(data, data ? data + std::strlen(data) : 0);
}

template <typename CHAR_TYPE>
inline
void bslstl_StringArgument_Base<CHAR_TYPE>::assign(
                                       const bsl::basic_string<CHAR_TYPE>& str)
{
    *this = bslstl_StringArgument(str.data(), str.data() + str.length());
}

template <typename CHAR_TYPE>
inline
void bslstl_StringArgument_Base<CHAR_TYPE>::assign(
                        const bslstl_StringArgument_Base<CHAR_TYPE>& stringArg)
{
    *this = stringArg;
}

template <typename CHAR_TYPE>
inline
void bslstl_StringArgument_Base<CHAR_TYPE>::clear()
{
    *this = bslstl_StringArgument(0, 0);
}

// ACCESSORS
template <typename CHAR_TYPE>
inline
const CHAR_TYPE& bslstl_StringArgument_Base<CHAR_TYPE>::
    operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < static_cast<int>(end() - begin()));

    return begin()[index];
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgument_Base<CHAR_TYPE>::
                           operator native_std::basic_string<CHAR_TYPE>() const
{
    return native_std::basic_string<CHAR_TYPE>(begin(), end());
}

template <typename CHAR_TYPE>
inline
typename bslstl_StringArgument_Base<CHAR_TYPE>::const_iterator
    bslstl_StringArgument_Base<CHAR_TYPE>::begin() const
{
    return Base::begin();
}

template <typename CHAR_TYPE>
inline
typename bslstl_StringArgument_Base<CHAR_TYPE>::const_iterator
    bslstl_StringArgument_Base<CHAR_TYPE>::end() const
{
    return Base::end();
}

template <typename CHAR_TYPE>
inline
const CHAR_TYPE *bslstl_StringArgument_Base<CHAR_TYPE>::data() const
{
    return begin();
}

template <typename CHAR_TYPE>
inline
bool bslstl_StringArgument_Base<CHAR_TYPE>::empty() const
{
    return begin() == end();
}

template <typename CHAR_TYPE>
inline
int bslstl_StringArgument_Base<CHAR_TYPE>::length() const
{
    return static_cast<int>(end() - begin());
}

// FREE OPERATORS
template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    const int len = lhs.length();

    if (len != rhs.length()) {
        return false;                                                 // RETURN
    }

    return 0 == len || 0 == std::memcmp(lhs.data(), rhs.data(), len);
}

template <typename CHAR_TYPE>
inline
bool operator==(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) == rhs;
}

template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs == bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator==(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) == rhs;
}

template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs == bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator==(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) == rhs;
}

template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs == bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) != rhs;
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs != bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) != rhs;
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs != bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator!=(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) != rhs;
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs != bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename CHAR_TYPE>
inline
bool operator<(const bsl::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) < rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs < bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) < rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs < bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<(const CHAR_TYPE *lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) < rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs < bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                        lhs.begin(), lhs.end());
}

template <typename CHAR_TYPE>
inline
bool operator>(const bsl::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) > rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs > bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>(const native_std::basic_string<CHAR_TYPE>& lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) > rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs > bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>(const CHAR_TYPE *lhs,
               const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) > rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
               const CHAR_TYPE *rhs)
{
    return lhs > bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return !(lhs > rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) <= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs <= bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) <= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs <= bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator<=(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) <= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator<=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs <= bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bsl::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) >= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs >= bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const native_std::basic_string<CHAR_TYPE>& lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) >= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs >= bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bool operator>=(const CHAR_TYPE *lhs,
                const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) >= rhs;
}

template <typename CHAR_TYPE>
inline
bool operator>=(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
                const CHAR_TYPE *rhs)
{
    return lhs >= bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
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
    operator+(const bsl::basic_string<CHAR_TYPE>& lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) + rhs;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const bsl::basic_string<CHAR_TYPE>& rhs)
{
    return lhs + bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const native_std::basic_string<CHAR_TYPE>& lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) + rhs;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const native_std::basic_string<CHAR_TYPE>& rhs)
{
    return lhs + bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const CHAR_TYPE *lhs,
              const bslstl_StringArgument_Base<CHAR_TYPE>& rhs)
{
    return bslstl_StringArgument_Base<CHAR_TYPE>(lhs) + rhs;
}

template <typename CHAR_TYPE>
inline
bsl::basic_string<CHAR_TYPE>
    operator+(const bslstl_StringArgument_Base<CHAR_TYPE>& lhs,
              const CHAR_TYPE *rhs)
{
    return lhs + bslstl_StringArgument_Base<CHAR_TYPE>(rhs);
}

template <typename CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>&
    operator<<(std::basic_ostream<CHAR_TYPE>&           stream,
               const bslstl_StringArgument_Base<CHAR_TYPE>&  stringArg)
{
    return stream.write(stringArg.data(), stringArg.length());
}

}  // close namespace BloombergLP

                        // ==================================
                        // struct hash<bslstl_StringArgument_Base>
                        // ==================================

namespace bsl {

template <typename CHAR_TYPE>
struct hash<BloombergLP::bslstl_StringArgument_Base<CHAR_TYPE> > {
    // This template specialization enables use of 'bslstl_StringArgument_Base'
    // within STL hash containers, for example,
    // 'bsl::hash_set<bslstl_StringArgument_Base>' and
    // 'bsl::hash_map<bslstl_StringArgument_Base, Type>' for some type 'Type'.

    // ACCESSORS
    std::size_t
    operator()(const BloombergLP::bslstl_StringArgument_Base<CHAR_TYPE>&
                                                              stringArg) const;
        // Return a hash corresponding to the string bound to the specified
        // 'stringArg'.
};

// ACCESSORS
template <typename CHAR_TYPE>
std::size_t hash<BloombergLP::bslstl_StringArgument_Base<CHAR_TYPE> >::
operator()(
     const BloombergLP::bslstl_StringArgument_Base<CHAR_TYPE>& stringArg) const
{
    const CHAR_TYPE *string  = stringArg.data();
    std::size_t stringLength = stringArg.length();

    // The following implementation was cloned from bdeu_hashutil.cpp, but
    // without the unneeded modulo operation.

    const unsigned int ADDEND       = 1013904223U;
    const unsigned int MULTIPLICAND =    1664525U;
    const unsigned int MASK         = 4294967295U;

    const CHAR_TYPE *end = string + stringLength;
    std::size_t r   = 0;

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
