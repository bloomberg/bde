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
//  bslstl_StringArgument: reference wrapper for a 'const' string
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

/*
#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSL_HASH_SET
#include <bsl_hash_set.h>
#endif

*/

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
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

/*
#ifndef INCLUDED_STRING
#include <string>               // for 'std::string'
#define INCLUDED_STRING
#endif
*/

namespace BloombergLP {

                        // ===========================
                        // class bslstl_StringArgument
                        // ===========================

class bslstl_StringArgument : public bslstl_StringArgument_Data
    // This class, having non-standard copy semantics, provides a reference to
    // a sequence of non-modifiable characters, i.e., a 'const' string.  A
    // 'bslstl_StringArgument' supports a hybrid of reference and pointer
    // semantics.  Many operations on a 'bslstl_StringArgument' (e.g.,
    // 'operator==') apply to the referenced (or bound) string without having
    // to dereference the referent.  However, since the bound string is
    // 'const', copy assignment favors a pointer- semantic rebind.
    //
    // The string bound to a 'bslstl_StringArgument' need not be
    // null-terminated, and may contain null ('\0') characters.  The bound
    // string, if any, must remain valid for as long as the
    // 'bslstl_StringArgument' is bound to that string.  After construction, a
    // 'bslstl_StringArgument' may be bound to another string (via 'operator='
    // or 'assign').
{
  public:
    // PUBLIC TYPES
    typedef const char *iterator;        // type of non-'const' iterator
    typedef const char *const_iterator;  // type of 'const' iterator
    typedef const char  value_type;      // type of objects iterated over
        // Iterator types ('iterator' and 'const_iterator') and value type
        // ('value_type') for STL-compatible iterators.

  public:
    // CREATORS
    bslstl_StringArgument();
        // Create an unbound string reference.  An unbound string reference is
        // not bound to any string.

    bslstl_StringArgument(const char *data, int length);
        // Create a string reference bound to the string at the specified
        // 'data' address and extending for the specified 'length' characters.
        // The string indicated by 'data' and 'length', if any, must remain
        // valid for as long as it is bound to this string reference.  The
        // behavior is undefined unless '0 <= length'.  Note that if 'data' is
        // 0, then 'length' also must be 0.  Also note that the bound string,
        // if any, need not be null-terminated and may contain null ('\0')
        // characters.

    bslstl_StringArgument(const_iterator begin, const_iterator end);
        // Create a string reference bound to the string at the specified
        // 'begin' iterator and extending to, but not including, the character
        // at the specified 'end' iterator.  The string indicated by 'begin'
        // and 'end', if any, must remain valid for as long as it is bound to
        // this string reference.  The behavior is undefined unless 'begin <=
        // end'.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    bslstl_StringArgument(const char *data);
        // Create a string reference bound to the string at the specified
        // 'data' address and extending for 'std::strlen(data)' characters.
        // The string at the 'data' address, if any, must remain valid for as
        // long as it is bound to this string reference.  The behavior is
        // undefined unless 'data' is 0, or 'data' is null-terminated.

    bslstl_StringArgument(const native_std::string& str);
    bslstl_StringArgument(const bsl::string& str);
        // Create a string reference bound to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    bslstl_StringArgument(const bslstl_StringArgument& original);
        // Create a string reference bound to the same string as that of the
        // specified 'original' string reference, or an unbound string
        // reference if 'original' is unbound.  The string bound to the
        // 'original' string reference, if any, must remain valid for as long
        // as it is bound to this string reference.

    // ~bslstl_StringArgument();
        // Destroy this string reference.  Note that the string bound to this
        // string reference, if any, is not affected.  Also note that this
        // trivial destructor is generated by the compiler.

    // MANIPULATORS
    bslstl_StringArgument& operator=(const bslstl_StringArgument& rhs);
        // Assign to this string reference the binding of the specified 'rhs'
        // string reference.  The string bound to 'rhs', if any, must remain
        // valid for as long as it is bound to this string reference.

    void assign(const char *data, int length);
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

    void assign(const char *data);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for 'std::strlen(data)' characters.  The
        // string at the 'data' address, if any, must remain valid for as long
        // as it is bound to this string reference.  The behavior is undefined
        // unless 'data' is not 0, or 'data' is null-terminated.

    void assign(const bsl::string& str);
        // Bind this string reference to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    void assign(const bslstl_StringArgument& stringArg);
        // Assign to this string reference the binding of the specified
        // 'stringArg'.  The string bound to 'stringArg', if any, must remain
        // valid for as long as it is bound to this string reference.

    void clear();
        // Make this string reference unbound.

    // ACCESSORS
    const char& operator[](int index) const;
        // Return a reference to the non-modifiable character at the specified
        // 'index' in the string bound to this string reference.  The reference
        // remains valid for as long as the string currently bound to this
        // string reference remains valid.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that this operator must not be used
        // on an unbound string reference.

    operator native_std::string() const;
        // Return an 'std::string' (synonymous with 'native_std::string')
        // having the value of the string bound to this string reference.

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

    const char *data() const;
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
bool operator==(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs);
bool operator==(const bsl::string& lhs, const bslstl_StringArgument& rhs);
bool operator==(const bslstl_StringArgument& lhs,
                const native_std::string& rhs);
bool operator==(const native_std::string& lhs,
                const bslstl_StringArgument& rhs);
bool operator==(const bslstl_StringArgument& lhs, const bsl::string& rhs);
bool operator==(const char *lhs, const bslstl_StringArgument& rhs);
bool operator==(const bslstl_StringArgument& lhs, const char *rhs);
    // Return 'true' if the strings indicated by the specified 'lhs' and 'rhs'
    // have the same lexicographic value, and 'false' otherwise.  Two strings
    // have the same lexicographic value if they are the same length and the
    // values at each respective character position are the same.  Null strings
    // are treated *as* *if* they were the empty string.

bool operator!=(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs);
bool operator!=(const bsl::string& lhs, const bslstl_StringArgument& rhs);
bool operator!=(const bslstl_StringArgument& lhs, const bsl::string& rhs);
bool operator!=(const bslstl_StringArgument& lhs,
                const native_std::string& rhs);
bool operator!=(const native_std::string& lhs,
                const bslstl_StringArgument& rhs);
bool operator!=(const char *lhs, const bslstl_StringArgument& rhs);
bool operator!=(const bslstl_StringArgument& lhs, const char *rhs);
    // Return 'true' if the strings indicated by the specified 'lhs' and 'rhs'
    // do not have the same lexicographic value, and 'false' otherwise.  Two
    // strings do not have the same lexicographic value if they differ in
    // length or differ in at least one respective character position.  Null
    // strings are treated *as* *if* they were the empty string.

bool operator<(const bslstl_StringArgument& lhs,
               const bslstl_StringArgument& rhs);
bool operator<(const bsl::string& lhs, const bslstl_StringArgument& rhs);
bool operator<(const bslstl_StringArgument& lhs, const bsl::string& rhs);
bool operator<(const bslstl_StringArgument& lhs,
               const native_std::string& rhs);
bool operator<(const native_std::string& lhs,
               const bslstl_StringArgument& rhs);
bool operator<(const char *lhs, const bslstl_StringArgument& rhs);
bool operator<(const bslstl_StringArgument& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically less than the string indicated by the specified 'rhs',
    // and 'false' otherwise.  Null strings are treated *as* *if* they were the
    // empty string.

bool operator>(const bslstl_StringArgument& lhs,
               const bslstl_StringArgument& rhs);
bool operator>(const bsl::string& lhs, const bslstl_StringArgument& rhs);
bool operator>(const bslstl_StringArgument& lhs, const bsl::string& rhs);
bool operator>(const bslstl_StringArgument& lhs,
               const native_std::string& rhs);
bool operator>(const native_std::string& lhs,
               const bslstl_StringArgument& rhs);
bool operator>(const char *lhs, const bslstl_StringArgument& rhs);
bool operator>(const bslstl_StringArgument& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically greater than the string indicated by the specified
    // 'rhs', and 'false' otherwise.  Null strings are treated *as* *if* they
    // were the empty string.

bool operator<=(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs);
bool operator<=(const bsl::string& lhs, const bslstl_StringArgument& rhs);
bool operator<=(const bslstl_StringArgument& lhs, const bsl::string& rhs);
bool operator<=(const bslstl_StringArgument& lhs,
                const native_std::string& rhs);
bool operator<=(const native_std::string& lhs,
                const bslstl_StringArgument& rhs);
bool operator<=(const char *lhs, const bslstl_StringArgument& rhs);
bool operator<=(const bslstl_StringArgument& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically less than or equal to the string indicated by the
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

bool operator>=(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs);
bool operator>=(const bsl::string& lhs, const bslstl_StringArgument& rhs);
bool operator>=(const bslstl_StringArgument& lhs, const bsl::string& rhs);
bool operator>=(const bslstl_StringArgument& lhs,
                const native_std::string& rhs);
bool operator>=(const native_std::string& lhs,
                const bslstl_StringArgument& rhs);
bool operator>=(const char *lhs, const bslstl_StringArgument& rhs);
bool operator>=(const bslstl_StringArgument& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically greater than or equal to the string indicated by the
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

bsl::string operator+(const bslstl_StringArgument& lhs,
                      const bslstl_StringArgument& rhs);
bsl::string operator+(const bsl::string& lhs,
                      const bslstl_StringArgument& rhs);
bsl::string operator+(const bslstl_StringArgument& lhs,
                      const bsl::string& rhs);
bsl::string operator+(const native_std::string& lhs,
                      const bslstl_StringArgument& rhs);
bsl::string operator+(const bslstl_StringArgument& lhs,
                      const native_std::string& rhs);
bsl::string operator+(const char *lhs, const bslstl_StringArgument& rhs);
bsl::string operator+(const bslstl_StringArgument& lhs, const char *rhs);
    // Return an 'bsl::string' having the value of the concatenation of the
    // strings indicated by the specified 'lhs' and rhs'.  Null strings are
    // treated *as* *if* they were the empty string.

std::ostream& operator<<(std::ostream&                stream,
                         const bslstl_StringArgument& stringArg);
    // Write the string bound to the specified 'stringArg' to the specified
    // output 'stream' and return a reference to the modifiable 'stream'.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------------------
                        // class bslstl_StringArgument
                        // ---------------------------

// CREATORS
inline
bslstl_StringArgument::bslstl_StringArgument()
: bslstl_StringArgument_Data(0, 0)
{
}

inline
bslstl_StringArgument::bslstl_StringArgument(const char *data, int length)
: bslstl_StringArgument_Data(data, data + length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);
}

inline
bslstl_StringArgument::bslstl_StringArgument(const_iterator begin,
                                             const_iterator end)
: bslstl_StringArgument_Data(begin, end)
{
}

inline
bslstl_StringArgument::bslstl_StringArgument(const char *data)
: bslstl_StringArgument_Data(data, data + std::strlen(data))
{
}

inline
bslstl_StringArgument::bslstl_StringArgument(const bsl::string& str)
: bslstl_StringArgument_Data(str.data(), str.data() + str.length())
{
}

inline
bslstl_StringArgument::bslstl_StringArgument(const native_std::string& str)
: bslstl_StringArgument_Data(str.data(), str.data() + str.length())
{
}

inline
bslstl_StringArgument::bslstl_StringArgument(
                                         const bslstl_StringArgument& original)
: bslstl_StringArgument_Data(original.d_begin, original.d_end)
{
}

// MANIPULATORS
inline
bslstl_StringArgument& bslstl_StringArgument::operator=(
                                              const bslstl_StringArgument& rhs)
{
    d_begin = rhs.d_begin;
    d_end   = rhs.d_end;
    return *this;
}

inline
void bslstl_StringArgument::assign(const char *data, int length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);

    d_begin = data;
    d_end   = data + length;
}

inline
void bslstl_StringArgument::assign(const_iterator begin, const_iterator end)
{
    d_begin = begin;
    d_end   = end;
}

inline
void bslstl_StringArgument::assign(const char *data)
{
    d_begin = data;
    d_end   = data ? data + std::strlen(data) : 0;
}

inline
void bslstl_StringArgument::assign(const bsl::string& str)
{
    d_begin = str.data();
    d_end   = str.data() + str.length();
}

inline
void bslstl_StringArgument::assign(const bslstl_StringArgument& stringArg)
{
    d_begin = stringArg.d_begin;
    d_end   = stringArg.d_end;
}

inline
void bslstl_StringArgument::clear()
{
    d_begin = 0;
    d_end   = 0;
}

// ACCESSORS
inline
const char& bslstl_StringArgument::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < static_cast<int>(d_end - d_begin));

    return d_begin[index];
}

inline
bslstl_StringArgument::operator native_std::string() const
{
    return native_std::string(d_begin, d_end);
}

inline
bslstl_StringArgument::const_iterator bslstl_StringArgument::begin() const
{
    return d_begin;
}

inline
bslstl_StringArgument::const_iterator bslstl_StringArgument::end() const
{
    return d_end;
}

inline
const char *bslstl_StringArgument::data() const
{
    return d_begin;
}

inline
bool bslstl_StringArgument::empty() const
{
    return d_begin == d_end;
}

inline
int bslstl_StringArgument::length() const
{
    return static_cast<int>(d_end - d_begin);
}

// FREE OPERATORS
inline
bool operator==(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs)
{
    const int len = lhs.length();

    if (len != rhs.length()) {
        return false;                                                 // RETURN
    }

    return 0 == len || 0 == std::memcmp(lhs.data(), rhs.data(), len);
}

inline
bool operator==(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) == rhs;
}

inline
bool operator==(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs == bslstl_StringArgument(rhs);
}

inline
bool operator==(const native_std::string& lhs,
                const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) == rhs;
}

inline
bool operator==(const bslstl_StringArgument& lhs,
                const native_std::string& rhs)
{
    return lhs == bslstl_StringArgument(rhs);
}

inline
bool operator==(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) == rhs;
}

inline
bool operator==(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs == bslstl_StringArgument(rhs);
}

inline
bool operator!=(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) != rhs;
}

inline
bool operator!=(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs != bslstl_StringArgument(rhs);
}

inline
bool operator!=(const native_std::string& lhs,
                const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) != rhs;
}

inline
bool operator!=(const bslstl_StringArgument& lhs,
                const native_std::string& rhs)
{
    return lhs != bslstl_StringArgument(rhs);
}

inline
bool operator!=(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) != rhs;
}

inline
bool operator!=(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs != bslstl_StringArgument(rhs);
}

inline
bool operator<(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) < rhs;
}

inline
bool operator<(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs < bslstl_StringArgument(rhs);
}

inline
bool operator<(const native_std::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) < rhs;
}

inline
bool operator<(const bslstl_StringArgument& lhs, const native_std::string& rhs)
{
    return lhs < bslstl_StringArgument(rhs);
}

inline
bool operator<(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) < rhs;
}

inline
bool operator<(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs < bslstl_StringArgument(rhs);
}

inline
bool operator>(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) > rhs;
}

inline
bool operator>(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs > bslstl_StringArgument(rhs);
}

inline
bool operator>(const native_std::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) > rhs;
}

inline
bool operator>(const bslstl_StringArgument& lhs, const native_std::string& rhs)
{
    return lhs > bslstl_StringArgument(rhs);
}

inline
bool operator>(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) > rhs;
}

inline
bool operator>(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs > bslstl_StringArgument(rhs);
}

inline
bool operator<=(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs)
{
    return !(lhs > rhs);
}

inline
bool operator<=(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) <= rhs;
}

inline
bool operator<=(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs <= bslstl_StringArgument(rhs);
}

inline
bool operator<=(const native_std::string& lhs,
                const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) <= rhs;
}

inline
bool operator<=(const bslstl_StringArgument& lhs,
                const native_std::string& rhs)
{
    return lhs <= bslstl_StringArgument(rhs);
}

inline
bool operator<=(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) <= rhs;
}

inline
bool operator<=(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs <= bslstl_StringArgument(rhs);
}

inline
bool operator>=(const bslstl_StringArgument& lhs,
                const bslstl_StringArgument& rhs)
{
    return !(lhs < rhs);
}

inline
bool operator>=(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) >= rhs;
}

inline
bool operator>=(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs >= bslstl_StringArgument(rhs);
}

inline
bool operator>=(const native_std::string& lhs,
                const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) >= rhs;
}

inline
bool operator>=(const bslstl_StringArgument& lhs,
                const native_std::string& rhs)
{
    return lhs >= bslstl_StringArgument(rhs);
}

inline
bool operator>=(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) >= rhs;
}

inline
bool operator>=(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs >= bslstl_StringArgument(rhs);
}

inline
bsl::string operator+(const bsl::string& lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) + rhs;
}

inline
bsl::string operator+(const bslstl_StringArgument& lhs, const bsl::string& rhs)
{
    return lhs + bslstl_StringArgument(rhs);
}

inline
bsl::string operator+(const native_std::string& lhs,
                      const bslstl_StringArgument&    rhs)
{
    return bslstl_StringArgument(lhs) + rhs;
}

inline
bsl::string operator+(const bslstl_StringArgument&    lhs,
                      const native_std::string& rhs)
{
    return lhs + bslstl_StringArgument(rhs);
}

inline
bsl::string operator+(const char *lhs, const bslstl_StringArgument& rhs)
{
    return bslstl_StringArgument(lhs) + rhs;
}

inline
bsl::string operator+(const bslstl_StringArgument& lhs, const char *rhs)
{
    return lhs + bslstl_StringArgument(rhs);
}

}  // close namespace BloombergLP

                        // ==================================
                        // struct hash<bslstl_StringArgument>
                        // ==================================

namespace bsl {

template <>
struct hash<BloombergLP::bslstl_StringArgument> {
    // This template specialization enables use of 'bslstl_StringArgument'
    // within STL hash containers, for example,
    // 'bsl::hash_set<bslstl_StringArgument>' and
    // 'bsl::hash_map<bslstl_StringArgument, Type>' for some type 'Type'.

    // ACCESSORS
    std::size_t
    operator()(const BloombergLP::bslstl_StringArgument& stringArg) const;
        // Return a hash corresponding to the string bound to the specified
        // 'stringArg'.
};

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
