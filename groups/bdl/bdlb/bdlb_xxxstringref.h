// bdlb_xxxstringref.h                                                -*-C++-*-
#ifndef INCLUDED_BDLB_XXXSTRINGREF
#define INCLUDED_BDLB_XXXSTRINGREF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reference to a 'const' string.
//
//@DEPRECATED: Use 'bslstl::StringRef' (via 'bsl_string.h') instead.
//
//@CLASSES:
//  bdlb::StringRef: reference wrapper for a 'const' string
//
//@AUTHOR: Vladimir Kliatchko (vkliatch), Anthony Comerico (acomeric)
//
//@DESCRIPTION: The 'bdlb::StringRef' class defined in this component, having a
// hybrid of reference and pointer semantics, provides a reference to a
// sequence of non-modifiable characters, i.e., a 'const' string.  Many
// operations on 'bdlb::StringRef' (e.g., 'operator==' and user-defined
// conversion to 'bsl::string') apply to the referenced (or bound) string
// without having to dereference the referent.  However, since the bound string
// is 'const', copy assignment favors a pointer-semantic rebinding.
// Furthermore, a 'bdlb::StringRef' also may be unbound, i.e., not bound to any
// string.
//
// The string that is bound to a 'bdlb::StringRef' need not be null-terminated.
// Moreover, the string may contain null ('\0') characters.  As such, the
// string bound to a 'bdlb::StringRef', in general, is not a C-style string.
// Note that the bound string is not managed in any way by a 'bdlb::StringRef'.
//
// The address and extent of the string bound to a 'bdlb::StringRef' are
// indicated by the 'data' and 'length' accessors, respectively.  The bound
// string is also indicated by the 'begin' and 'end' accessors that return
// STL-compliant random access iterators to the beginning of the string and one
// character past the end of the string, respectively.  An overloaded
// 'operator[]' is also provided for direct by-index access to individual
// characters in the bound string.
//
// Several constructors are provided for efficiently creating instances of
// 'bdlb::StringRef'.  Corresponding to the accessors just introduced, there is
// a constructor that takes a 'const char *' address and a length.  Similarly,
// there is a constructor that takes begin and end iterators of type
// 'bdlb::StringRef::const_iterator'.  Two conversion constructors also are
// provided that take 'const char *' and 'const bsl::string&' arguments.  These
// conveniently allow for automatic conversion to 'bdlb::StringRef' in contexts
// where required.  Note that the 'const char *' argument in this case *must*
// be null-terminated.  Lastly, there is a default constructor and a copy
// constructor.  Default-constructed instances of 'bdlb::StringRef' are
// unbound.
//
// Once constructed, a 'bdlb::StringRef' may be bound to another string through
// assignment or by calling the overloaded 'assign' methods.  Each of the
// constructors, except for the default constructor, has a corresponding
// 'assign' method that takes the same arguments as the constructor.  A
// 'bdlb::StringRef' also can be made unbound through copy assignment, or by
// invoking an 'assign' method.  In addition, the 'unbind' method may be called
// to affirmatively set a 'bdlb::StringRef' to be unbound.
//
// Several free operators are provided for (1) lexicographical comparison of
// the strings bound to two 'bdlb::StringRef' instances, (2) concatenation of
// the strings bound to two 'bdlb::StringRef' instances producing an
// 'bsl::string' containing the result, and (3) writing the string bound to a
// 'bdlb::StringRef' to a specified output stream.  All class methods and free
// operators are well-defined in the presence of unbound 'bdlb::StringRef'
// instances.  Refer to the function-level documentation for details.
//
// Lastly, the 'bsl::hash' template class is specialized for 'bdlb::StringRef'
// to enable use of 'bdlb::StringRef' within STL hash containers (e.g.,
// 'bsl::unordered_set<bdlb::StringRef>' and
// 'bsl::unordered_map<bdlb::StringRef, Type>' for some type 'Type').
//
///Efficiency and Usage Considerations
///-----------------------------------
// Passing strings by 'bdlb::StringRef' can be considerably more efficient even
// than passing by 'const' reference.  (See the "Caveats" section below.)  For
// example, consider the following hypothetical class method in which the
// parameter is a reference to a non-modifiable 'bsl::string':
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
// non-modifiable 'bdlb::StringRef':
//..
//  void MyClass::setLabel(const bdlb::StringRef& label)
//  {
//      d_label.assign(label.data(), label.length());
//  }
//..
// Now this call:
//..
//  myClassObj.setLabel("hello");
//..
// has the side-effect of creating a temporary 'bdlb::StringRef' object which
// is likely to be substantially more efficient than creating a temporary
// 'bsl::string'.  In this case, instead of copying the *contents* of "hello",
// the *address* of the literal string is copied.  In addition, 'bsl::strlen'
// is applied to the string in order to locate its end.  There are *no*
// allocations done on behalf of the temporary object.
//
// If the length of the string is known at the point of the call to 'setLabel'
// (or if the address one-past the last character is known), then the call can
// be made even more efficiently (while, of course, sacrificing the automatic
// conversion to 'bdlb::StringRef').  The next code snippet creates two string
// references bound to a string that expressly is *not* null-terminated:
//..
//  const char HELLO[]   = { 'h', 'e', 'l', 'l', 'o' };
//  const int  HELLO_LEN = 5;
//  myClassObj.setLabel(bdlb::StringRef(HELLO, HELLO_LEN));
//
//  myClassObj.setLabel(bdlb::StringRef(HELLO, HELLO + HELLO_LEN));
//..
// Another important feature of 'bdlb::StringRef' is that it is tolerant of
// null pointers.  In particular, a 'bdlb::StringRef' can be created from a
// null pointer; it is identical to one that is created using the default
// constructor.  Consider a 'process' class method that takes a 'command'
// argument of type 'const bdlb::StringRef&':
//..
//  void MyClass::process(const bdlb::StringRef& command)
//      // Process the specified 'command'.  Note that 'command' may be unbound
//      // indicating the null command.
//  {
//      bsl::cout << "command = `" << command << "'" << bsl::endl;
//
//      if (command.isBound()) {
//          // Process non-null command.
//      }
//      else {
//          // Handle null command.
//      }
//  }
//..
// Note that it is not necessary to check that 'command' is bound before
// printing it.  All class methods and free operators defined in this component
// are well-defined for unbound 'bdlb::StringRef' arguments.  Typically, an
// unbound 'bdlb::StringRef' is treated *as* *if* it were bound to the empty
// string.  The two exceptions are the 'isBound' and 'data' accessors which are
// available to clients that need to distinguish a 'bdlb::StringRef' that is
// bound to the empty string from one that is truly unbound.
//
///Caveats
///-------
// 1) The string bound to a 'bdlb::StringRef' need not be null-terminated, and,
// in fact, may *contain* null ('\0') characters.  Thus, it is generally not
// valid to pass the address returned by the 'data' accessor to Standard C
// functions that expect a null-terminated string (e.g., 'bsl::strlen',
// 'bsl::strcmp', etc.).
//
// 2) However, if a 'bdlb::StringRef' is bound to a string via the one-argument
// constructor or 'assign' method that takes a 'const char *' parameter, then
// the string *must* be null-terminated.  'bsl::strlen' is applied to the
// string in these two cases (only) in order to locate the end of the string.
//
// 3) The string bound to a 'bdlb::StringRef' must remain valid for as long as
// the 'bdlb::StringRef' is bound to that string.  Lifetime issues should be
// carefully considered when, for example, returning a 'bdlb::StringRef' from a
// function or storing a 'bdlb::StringRef' in a container.
//
///Thread-Safety
///-------------
// It is safe to access the same 'bdlb::StringRef' object from multiple
// threads.  It is safe to access and manipulate different 'bdlb::StringRef'
// objects that are bound to the same string from multiple threads.  It is
// unsafe to manipulate the same 'bdlb::StringRef' object from multiple
// threads.
//
///Usage
///-----
// The following snippets of code illustrate basic and varied use of the
// 'bdlb::StringRef' class.  The sample code uses the 'getNumBlanks' function
// defined below, which returns the number of blank (' ') characters contained
// in the string that is bound to a specified 'bdlb::StringRef'.  The function
// delegates the work to the 'bsl::count' STL algorithm.  This delegation is
// made possible by the STL-compliant random access iterators provided by
// 'bdlb::StringRef' via the 'begin' and 'end' accessors:
//..
//  #include <algorithm>
//
//  int getNumBlanks(const bdlb::StringRef& stringRef)
//      // Return the number of blank (' ') characters in the string bound to
//      // the specified 'stringRef', or 0 if 'stringRef' is unbound.
//  {
//      return bsl::count(stringRef.begin(), stringRef.end(), ' ');
//  }
//..
// We make several calls to 'getNumBlanks' with strings of various forms to
// illustrate the benefit of having 'getNumBlanks' take a 'bdlb::StringRef'
// argument.  First we verify tolerance of null pointers and 'bdlb::StringRef'
// objects that are unbound:
//..
//  int numBlanks = getNumBlanks((const char *)0);
//  assert(0 == numBlanks);
//
//  bdlb::StringRef unbound;
//  numBlanks = getNumBlanks(unbound);
//  assert(0 == numBlanks);
//..
// In most contexts, unbound 'bdlb::StringRef' objects are treated as if they
// are bound to the empty string:
//..
//  assert(true  == (unbound    == ""));
//  assert(false == ("anything" <  unbound));
//..
// Similarly, the 'operator bsl::string()' conversion operator returns an
// 'bsl::string' that is initialized to the empty string:
//..
//  bsl::string empty = unbound;
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
// 'bdlb::StringRef':
//..
//  bdlb::StringRef line(poem, 29);
//  numBlanks = getNumBlanks(line);
//  assert( 5 == numBlanks);
//  assert(29 == line.length());
//  assert( 0 == bsl::strncmp(poem, line.data(), line.length()));
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
//  assert(bdlb::StringRef(poemString) == poemString);
//  assert(bdlb::StringRef(poemString) == poemString.c_str());
//..
// Finally, we illustrate binding a 'bdlb::StringRef' to a string that contains
// null ('\0') characters.  First we populate the 'poemWithNulls' array with
// the contents of 'poem':
//..
//  char poemWithNulls[512];
//  const int poemLength = bsl::strlen(poem);
//  bsl::memcpy(poemWithNulls, poem, poemLength + 1);
//  assert(0 == bsl::strcmp(poem, poemWithNulls));
//..
// Next we replace each occurrence of '\n' in 'poemWithNulls' with '\0':
//..
//  bsl::replace(poemWithNulls, poemWithNulls + poemLength, '\n', '\0');
//  assert(0 != bsl::strcmp(poem, poemWithNulls));
//..
// We now observe that 'poemWithNulls' has the same number of blank characters
// as the original 'poem':
//..
//  numBlanks = getNumBlanks(bdlb::StringRef(poemWithNulls, poemLength));
//  assert(42 == numBlanks);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>         // for 'bsl::size_t'
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>         // for 'bsl::strlen', 'bsl::memcmp'
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_STRING
#include <string>                // for 'std::string'
#define INCLUDED_STRING
#endif

namespace BloombergLP {

namespace bdlb {
                        // =====================
                        // class StringRef
                        // =====================

class StringRef : public bslstl::StringRef {
    // This class, having non-standard copy semantics, provides a reference to
    // a sequence of non-modifiable characters, i.e., a 'const' string.  A
    // 'StringRef' supports a hybrid of reference and pointer semantics.
    // Many operations on a 'StringRef' (e.g., 'operator==' and
    // user-defined conversion to 'bsl::string') apply to the referenced (or
    // bound) string without having to dereference the referent.  However,
    // since the bound string is 'const', copy assignment favors a pointer-
    // semantic rebind.  In addition, a 'StringRef' also may be unbound,
    // i.e., not bound to any string.
    //
    // The string bound to a 'StringRef' need not be null-terminated, and
    // may contain null ('\0') characters.  The bound string, if any, must
    // remain valid for as long as the 'StringRef' is bound to that
    // string.  After construction, a 'StringRef' may be bound to another
    // string (via 'operator=' or 'assign'), or may be made unbound (via
    // 'unbind', 'operator=', or 'assign').
    //
    // Most of the behavior of the accesssors and free operators on
    // 'StringRef' are inherited from 'bslstl::StringRef'.  This class is
    // similar to 'bslstl::StringRef' except that it is easier to forward
    // declare; and it supports the 'clear' and 'unbind' manipulators; and its
    // 'length' accessor returns an 'int' rather than a 'bsl::size_t'.

  public:
    // CREATORS
    StringRef();
        // Create an unbound string reference.  An unbound string reference is
        // not bound to any string.

    StringRef(const char *data, int length);
        // Create a string reference bound to the string at the specified
        // 'data' address and extending for the specified 'length' characters,
        // or create an unbound string reference if both 'data' and 'length'
        // are 0.  The string indicated by 'data' and 'length', if any, must
        // remain valid for as long as it is bound to this string reference.
        // The behavior is undefined unless '0 <= length'.  Note that if 'data'
        // is 0, then 'length' also must be 0.  Also note that the bound
        // string, if any, need not be null-terminated and may contain null
        // ('\0') characters.

    StringRef(const_iterator begin, const_iterator end);
        // Create a string reference bound to the string at the specified
        // 'begin' iterator and extending to, but not including, the character
        // at the specified 'end' iterator, or create an unbound string
        // reference if both 'begin' and 'end' are 0.  The string indicated by
        // 'begin' and 'end', if any, must remain valid for as long as it is
        // bound to this string reference.  The behavior is undefined unless
        // 'begin <= end'.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    StringRef(const bslstl::StringRef& otherStringRef);       // IMPLICIT
        // Create a string reference bound to the string bound to the specified
        // 'otherStringRef'.  The behavior is undefined unless the bound
        // string, if any, remains valid for the lifetime of this object and
        // 'otherStringRef.begin() <= otherStringRef.end()'.  Note that the
        // bound string, if any, need not be null-terminated and may contain
        // null ('\0') characters.

    StringRef(const char *data);                              // IMPLICIT
        // Create a string reference bound to the string at the specified
        // 'data' address and extending for 'bsl::strlen(data)' characters, or
        // create an unbound string reference if 'data' is 0.  The string at
        // the 'data' address, if any, must remain valid for as long as it is
        // bound to this string reference.  The behavior is undefined unless
        // 'data' is 0, or 'data' is null-terminated.

    StringRef(const native_std::string& string);              // IMPLICIT
    StringRef(const bsl::string& string);                     // IMPLICIT
        // Create a string reference bound to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    StringRef(const StringRef& original);
        // Create a string reference bound to the same string as that of the
        // specified 'original' string reference, or an unbound string
        // reference if 'original' is unbound.  The string bound to the
        // 'original' string reference, if any, must remain valid for as long
        // as it is bound to this string reference.

    // ~StringRef() = default;
        // Destroy this string reference.  Note that the string bound to this
        // string reference, if any, is not affected.  Also note that this
        // trivial destructor is generated by the compiler.

    // MANIPULATORS
    StringRef& operator=(const StringRef& rhs);
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
        // specified 'end' iterator, or make this string reference unbound if
        // both 'begin' and 'end' are 0.  The string indicated by 'begin' and
        // 'end', if any, must remain valid for as long as it is bound to this
        // string reference.  The behavior is undefined unless 'begin <= end'.
        // Note that the bound string, if any, need not be null-terminated and
        // may contain null ('\0') characters.

    void assign(const char *data);
        // Bind this string reference to the string at the specified 'data'
        // address and extending for 'bsl::strlen(data)' characters, or make
        // this string reference unbound if 'data' is 0.  The string at the
        // 'data' address, if any, must remain valid for as long as it is bound
        // to this string reference.  The behavior is undefined unless 'data'
        // is 0, or 'data' is null-terminated.

    void assign(const bsl::string& string);
        // Bind this string reference to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    void assign(const native_std::string& string);
        // Bind this string reference to the string at the specified
        // 'string.data()' address and extending for 'string.size()'
        // characters.  The string indicated by 'string.data()' and
        // 'string.size()' must remain valid for as long as it is bound to this
        // string reference.  Note that the bound string, if any, need not be
        // null-terminated and may contain null ('\0') characters.

    void assign(const StringRef& stringRef);
        // Assign to this string reference the binding of the specified
        // 'stringRef'.  The string bound to 'stringRef', if any, must remain
        // valid for as long as it is bound to this string reference.

    void assign(const bslstl::StringRef& stringRef);
        // Assign to this string reference the binding of the specified
        // 'stringRef'.  The string bound to 'stringRef', if any, must remain
        // valid for as long as it is bound to this string reference.

    void clear();
        // Make this string reference unbound.
        //
        // DEPRECATED: use 'unbind' instead.

    void unbind();
        // Make this string reference unbound.

    // ACCESSORS
    const char& operator[](int index) const;
        // Return a reference to the non-modifiable character at the specified
        // 'index' in the string bound to this string reference.  The reference
        // remains valid for as long as the string currently bound to this
        // string reference remains valid.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that this operator must not be used
        // on an unbound string reference.

    const char *data() const;
        // Return the address of the first character of the string bound to
        // this string reference such that '[data(), data() + length())' is a
        // valid half-open range of characters.  Return 0 for an empty string
        // reference object.  Note that the range of characters may not be
        // null-terminated and may contain embedded null characters.

    bool isBound() const;
        // Return 'true' if this string reference is bound, and 'false'
        // otherwise.

    bool isEmpty() const;
        // Return 'true' if this string reference is bound, and 'false'
        // otherwise.

    int length() const;
        // Return the length of the string bound to this string reference, and
        // 0 if this string reference is unbound.

                                // Iterators
    const_iterator begin() const;
        // Return a STL-conforming random access iterator to the first
        // character of the string bound to this string reference or 'end()' if
        // the string reference is empty.  The iterator remains valid as long
        // as this object is valid and is bound to the same string.

    const_iterator end() const;
        // Return a STL-conforming random access iterator one past the last
        // character of the string bound to this string reference or 'begin()'
        // if the string reference is empty.  The iterator remains valid as
        // long as this object is valid and is bound to the same string.
};

// ============================================================================
//                            FREE OPERATORS
// ============================================================================

// FREE OPERATORS
bool operator==(const StringRef& lhs, const StringRef& rhs);
bool operator==(const bsl::string& lhs, const StringRef& rhs);
bool operator==(const StringRef& lhs, const native_std::string& rhs);
bool operator==(const native_std::string& lhs, const StringRef& rhs);
bool operator==(const StringRef& lhs, const bsl::string& rhs);
bool operator==(const char *lhs, const StringRef& rhs);
bool operator==(const StringRef& lhs, const char *rhs);
    // Return 'true' if the strings indicated by the specified 'lhs' and 'rhs'
    // have the same lexicographic value, and 'false' otherwise.  Two strings
    // have the same lexicographic value if they are the same length and the
    // values at each respective character position are the same.  Null strings
    // are treated *as* *if* they were the empty string.

bool operator!=(const StringRef& lhs, const StringRef& rhs);
bool operator!=(const bsl::string& lhs, const StringRef& rhs);
bool operator!=(const StringRef& lhs, const bsl::string& rhs);
bool operator!=(const StringRef& lhs, const native_std::string& rhs);
bool operator!=(const native_std::string& lhs, const StringRef& rhs);
bool operator!=(const char *lhs, const StringRef& rhs);
bool operator!=(const StringRef& lhs, const char *rhs);
    // Return 'true' if the strings indicated by the specified 'lhs' and 'rhs'
    // do not have the same lexicographic value, and 'false' otherwise.  Two
    // strings do not have the same lexicographic value if they differ in
    // length or differ in at least one respective character position.  Null
    // strings are treated *as* *if* they were the empty string.

bool operator<(const StringRef& lhs, const StringRef& rhs);
bool operator<(const bsl::string& lhs, const StringRef& rhs);
bool operator<(const StringRef& lhs, const bsl::string& rhs);
bool operator<(const StringRef& lhs, const native_std::string& rhs);
bool operator<(const native_std::string& lhs, const StringRef& rhs);
bool operator<(const char *lhs, const StringRef& rhs);
bool operator<(const StringRef& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically less than the string indicated by the specified 'rhs',
    // and 'false' otherwise.  Null strings are treated *as* *if* they were the
    // empty string.

bool operator>(const StringRef& lhs, const StringRef& rhs);
bool operator>(const bsl::string& lhs, const StringRef& rhs);
bool operator>(const StringRef& lhs, const bsl::string& rhs);
bool operator>(const StringRef& lhs, const native_std::string& rhs);
bool operator>(const native_std::string& lhs, const StringRef& rhs);
bool operator>(const char *lhs, const StringRef& rhs);
bool operator>(const StringRef& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically greater than the string indicated by the specified
    // 'rhs', and 'false' otherwise.  Null strings are treated *as* *if* they
    // were the empty string.

bool operator<=(const StringRef& lhs, const StringRef& rhs);
bool operator<=(const bsl::string& lhs, const StringRef& rhs);
bool operator<=(const StringRef& lhs, const bsl::string& rhs);
bool operator<=(const StringRef& lhs, const native_std::string& rhs);
bool operator<=(const native_std::string& lhs, const StringRef& rhs);
bool operator<=(const char *lhs, const StringRef& rhs);
bool operator<=(const StringRef& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically less than or equal to the string indicated by the
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

bool operator>=(const StringRef& lhs, const StringRef& rhs);
bool operator>=(const bsl::string& lhs, const StringRef& rhs);
bool operator>=(const StringRef& lhs, const bsl::string& rhs);
bool operator>=(const StringRef& lhs, const native_std::string& rhs);
bool operator>=(const native_std::string& lhs, const StringRef& rhs);
bool operator>=(const char *lhs, const StringRef& rhs);
bool operator>=(const StringRef& lhs, const char *rhs);
    // Return 'true' if the string indicated by the specified 'lhs' is
    // lexicographically greater than or equal to the string indicated by the
    // specified 'rhs', and 'false' otherwise.  Null strings are treated *as*
    // *if* they were the empty string.

bsl::string operator+(const StringRef& lhs, const StringRef& rhs);
bsl::string operator+(const bsl::string& lhs, const StringRef& rhs);
bsl::string operator+(const StringRef& lhs, const bsl::string& rhs);
bsl::string operator+(const native_std::string& lhs,
                      const StringRef& rhs);
bsl::string operator+(const StringRef& lhs,
                      const native_std::string& rhs);
bsl::string operator+(const char *lhs, const StringRef& rhs);
bsl::string operator+(const StringRef& lhs, const char *rhs);
    // Return an 'bsl::string' having the value of the concatenation of the
    // strings indicated by the specified 'lhs' and rhs'.  Null strings are
    // treated *as* *if* they were the empty string.

bsl::ostream& operator<<(bsl::ostream&          stream,
                         const StringRef& stringRef);
    // Write the string bound to the specified 'stringRef' to the specified
    // output 'stream' and return a reference to the modifiable 'stream'.  This
    // method has no effect if 'stringRef' is unbound.

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class StringRef
                        // ---------------------

// CREATORS
inline
StringRef::StringRef()
: bslstl::StringRef()
{
}

inline
StringRef::StringRef(const char *data, int length)
: bslstl::StringRef(data, length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);
}

inline
StringRef::StringRef(const_iterator begin, const_iterator end)
: bslstl::StringRef(begin, end)
{
}

inline
StringRef::StringRef(const bslstl::StringRef& otherStringRef)
: bslstl::StringRef(otherStringRef)
{
}

inline
StringRef::StringRef(const char *data)
: bslstl::StringRef()
{
    if (data) {
        bslstl::StringRef::assign(data);
    }
}

inline
StringRef::StringRef(const bsl::string& string)
: bslstl::StringRef(string)
{
}

inline
StringRef::StringRef(const native_std::string& string)
: bslstl::StringRef(string)
{
}

inline
StringRef::StringRef(const StringRef& original)
: bslstl::StringRef(original.begin(), original.end())
{
}

// MANIPULATORS
inline
StringRef& StringRef::operator=(const StringRef& rhs)
{
    bslstl::StringRef::assign(rhs.begin(), rhs.end());
    return *this;
}

inline
void StringRef::assign(const char *data, int length)
{
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(data || 0 == length);

    bslstl::StringRef::assign(data, length);
}

inline
void StringRef::assign(const_iterator begin, const_iterator end)
{
    bslstl::StringRef::assign(begin, end);
}

inline
void StringRef::assign(const char *data)
{
    if (data) {
        bslstl::StringRef::assign(data);
    }
    else {
        unbind();
    }
}

inline
void StringRef::assign(const bsl::string& string)
{
    bslstl::StringRef::assign(string);
}

inline
void StringRef::assign(const native_std::string& string)
{
    // Note there is no 'bslstl::StringRef::assign(const native_std::string&)',
    // so 'assign(string)' become ambiguous between 'string' being converted to
    // a 'bsl::string' or converrted to a 'StringRef'.  Also, on some
    // platforms, 'native_std::string::{begin,end}' aren't of type
    // 'const char *' so 'assign(string.begin(), string.end())' doesn't work.

    bslstl::StringRef::assign(string.data(),
                              static_cast<int>(string.length()));
}

inline
void StringRef::assign(const StringRef& stringRef)
{
    bslstl::StringRef::assign(stringRef.begin(), stringRef.end());
}

inline
void StringRef::assign(const bslstl::StringRef& stringRef)
{
    bslstl::StringRef::assign(stringRef.begin(), stringRef.end());
}

inline
void StringRef::clear()
{
    bslstl::StringRef::assign((const char *) 0, (const char *) 0);
}

inline
void StringRef::unbind()
{
    bslstl::StringRef::assign((const char *) 0, (const char *) 0);
}

// ACCESSORS
inline
const char& StringRef::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < static_cast<int>(end() - begin()));

    return begin()[index];
}



inline
const char *StringRef::data() const
{
    return bslstl::StringRef::begin();
}

inline
bool StringRef::isBound() const
{
    return bslstl::StringRef::begin() != 0;
}

inline
bool StringRef::isEmpty() const
{
    return bslstl::StringRef::begin() == bslstl::StringRef::end();
}

inline
int StringRef::length() const
{
    return static_cast<int>(bslstl::StringRef::length());
}

                        // Iterators
inline
StringRef::const_iterator StringRef::begin() const
{
    return bslstl::StringRef::begin();
}

inline
StringRef::const_iterator StringRef::end() const
{
    return bslstl::StringRef::end();
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const StringRef& lhs, const StringRef& rhs)
{
    const int len = lhs.length();

    if (len != rhs.length()) {
        return false;                                                 // RETURN
    }

    return 0 == len || 0 == bsl::memcmp(lhs.data(), rhs.data(), len);
}

inline
bool bdlb::operator==(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) == rhs;
}

inline
bool bdlb::operator==(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs == StringRef(rhs);
}

inline
bool bdlb::operator==(const native_std::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) == rhs;
}

inline
bool bdlb::operator==(const StringRef& lhs, const native_std::string& rhs)
{
    return lhs == StringRef(rhs);
}

inline
bool bdlb::operator==(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) == rhs;
}

inline
bool bdlb::operator==(const StringRef& lhs, const char *rhs)
{
    return lhs == StringRef(rhs);
}

inline
bool bdlb::operator!=(const StringRef& lhs, const StringRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdlb::operator!=(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) != rhs;
}

inline
bool bdlb::operator!=(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs != StringRef(rhs);
}

inline
bool bdlb::operator!=(const native_std::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) != rhs;
}

inline
bool bdlb::operator!=(const StringRef& lhs, const native_std::string& rhs)
{
    return lhs != StringRef(rhs);
}

inline
bool bdlb::operator!=(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) != rhs;
}

inline
bool bdlb::operator!=(const StringRef& lhs, const char *rhs)
{
    return lhs != StringRef(rhs);
}

inline
bool bdlb::operator<(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) < rhs;
}

inline
bool bdlb::operator<(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs < StringRef(rhs);
}

inline
bool bdlb::operator<(const native_std::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) < rhs;
}

inline
bool bdlb::operator<(const StringRef& lhs, const native_std::string& rhs)
{
    return lhs < StringRef(rhs);
}

inline
bool bdlb::operator<(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) < rhs;
}

inline
bool bdlb::operator<(const StringRef& lhs, const char *rhs)
{
    return lhs < StringRef(rhs);
}

inline
bool bdlb::operator>(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) > rhs;
}

inline
bool bdlb::operator>(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs > StringRef(rhs);
}

inline
bool bdlb::operator>(const native_std::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) > rhs;
}

inline
bool bdlb::operator>(const StringRef& lhs, const native_std::string& rhs)
{
    return lhs > StringRef(rhs);
}

inline
bool bdlb::operator>(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) > rhs;
}

inline
bool bdlb::operator>(const StringRef& lhs, const char *rhs)
{
    return lhs > StringRef(rhs);
}

inline
bool bdlb::operator<=(const StringRef& lhs, const StringRef& rhs)
{
    return !(lhs > rhs);
}

inline
bool bdlb::operator<=(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) <= rhs;
}

inline
bool bdlb::operator<=(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs <= StringRef(rhs);
}

inline
bool bdlb::operator<=(const native_std::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) <= rhs;
}

inline
bool bdlb::operator<=(const StringRef& lhs, const native_std::string& rhs)
{
    return lhs <= StringRef(rhs);
}

inline
bool bdlb::operator<=(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) <= rhs;
}

inline
bool bdlb::operator<=(const StringRef& lhs, const char *rhs)
{
    return lhs <= StringRef(rhs);
}

inline
bool bdlb::operator>=(const StringRef& lhs, const StringRef& rhs)
{
    return !(lhs < rhs);
}

inline
bool bdlb::operator>=(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) >= rhs;
}

inline
bool bdlb::operator>=(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs >= StringRef(rhs);
}

inline
bool bdlb::operator>=(const native_std::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) >= rhs;
}

inline
bool bdlb::operator>=(const StringRef& lhs, const native_std::string& rhs)
{
    return lhs >= StringRef(rhs);
}

inline
bool bdlb::operator>=(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) >= rhs;
}

inline
bool bdlb::operator>=(const StringRef& lhs, const char *rhs)
{
    return lhs >= StringRef(rhs);
}

inline
bsl::string bdlb::operator+(const bsl::string& lhs, const StringRef& rhs)
{
    return StringRef(lhs) + rhs;
}

inline
bsl::string bdlb::operator+(const StringRef& lhs, const bsl::string& rhs)
{
    return lhs + StringRef(rhs);
}

inline
bsl::string bdlb::operator+(const native_std::string& lhs,
                      const StringRef&    rhs)
{
    return StringRef(lhs) + rhs;
}

inline
bsl::string bdlb::operator+(const StringRef&    lhs,
                      const native_std::string& rhs)
{
    return lhs + StringRef(rhs);
}

inline
bsl::string bdlb::operator+(const char *lhs, const StringRef& rhs)
{
    return StringRef(lhs) + rhs;
}

inline
bsl::string bdlb::operator+(const StringRef& lhs, const char *rhs)
{
    return lhs + StringRef(rhs);
}

}  // close enterprise namespace

                        // ============================
                        // struct hash<bdlb::StringRef>
                        // ============================

namespace bsl {

template <>
struct hash<BloombergLP::bdlb::StringRef> {
    // This template specialization enables use of 'bdlb::StringRef' within STL
    // hash containers, for example, 'bsl::unordered_set<bdlb::StringRef>' and
    // 'bsl::unordered_map<bdlb::StringRef, Type>' for some type 'Type'.

    // ACCESSORS
    bsl::size_t
    operator()(const BloombergLP::bdlb::StringRef& stringRef) const;
        // Return a hash corresponding to the string bound to the specified
        // 'stringRef'.  Note that an unbound string reference and one bound to
        // the empty string will produce the same hash.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
