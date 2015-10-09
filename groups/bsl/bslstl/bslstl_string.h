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
//  bsl::basic_string: C++ standard compliant 'basic_string' implementation
//  bsl::string: 'typedef' for 'bsl::basic_string<char>'
//  bsl::wstring: 'typedef' for 'bsl::basic_string<wchar_t>'
//
//@SEE_ALSO: ISO C++ Standard, Section 21 [strings]
//
//@DESCRIPTION: This component defines a single class template 'basic_string',
// implementing standard containers, 'std::string' and 'std::wstring', that
// hold a sequence of characters.
//
// An instantiation of 'basic_string' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of characters) and the
// sequence of characters that the string contains.  The 'basic_string' 'class'
// is parameterized by the character type, 'CHAR_TYPE', that character type's
// traits, 'CHAR_TRAITS', and an allocator, 'ALLOCATOR'.  The traits for each
// character type provide functions that assign, compare, and copy a sequence
// of those characters.
//
// A 'basic_string' meets the requirements of a sequential container with
// random access iterators as specified in the [basic.string] section of the
// C++ standard [21.4].  The 'basic_string' implemented here adheres to the
// C++11 standard, except that it does not have interfaces that take rvalue
// references or 'initializer_lists', the 'shrink_to_fit' method,and template
// specializations 'std::u16string' and 'std::u32string'.  Note that excluded
// C++11 features are those that require (or are greatly simplified by) C++11
// compiler support.
//
///Memory Allocation
///-----------------
// The type supplied as a 'basic_string's 'ALLOCATOR' template parameter
// determines how that 'basic_string' will allocate memory.  The 'basic_string'
// template supports allocators meeting the requirements of the C++11 standard,
// in addition it supports scoped-allocators derived from the
// 'bslma::Allocator' memory allocation protocol.  Clients intending to use
// 'bslma' style allocators should use the template's default 'ALLOCATOR' type:
// The default type for the 'ALLOCATOR' template parameter, 'bsl::allocator',
// provides a C++11 standard-compatible adapter for a 'bslma::Allocator'
// object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of an 'basic_string'
// instantiation is 'bsl::allocator', then objects of that 'basic_string' type
// will conform to the standard behavior of a 'bslma'-allocator-enabled type.
// Such a 'basic_string' accepts an optional 'bslma::Allocator' argument at
// construction.  If the address of a 'bslma::Allocator' object is explicitly
// supplied at construction, it is used to supply memory for the 'basic_string'
// throughout its lifetime; otherwise, the 'basic_string' will use the default
// allocator installed at the time of the 'basic_string''s construction (see
// 'bslma_default').
//
///Lexicographical Comparisons
///---------------------------
// Two 'basic_string's 'lhs' and 'rhs' are lexicographically compared by first
// determining 'N', the smaller of the lengths of 'lhs' and 'rhs', and
// comparing characters at each position between 0 and 'N - 1', using
// 'CHAR_TRAITS::lt' in lexicographical fashion.  If 'CHAR_TRAITS::lt'
// determines that strings are non-equal (smaller or larger), then this is the
// result.  Otherwise, the lengths of the strings are compared and the shorter
// string is declared the smaller.  Lexicographical comparison returns equality
// only when both strings have the same length and the same character value in
// each respective position.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances of
// 'basic_string':
//..
//  Legend
//  ------
//  'V'              - the 'CHAR_TYPE' template parameter type of the
//                     'basic_string'
//  'a', 'b'         - two distinct objects of type 'basic_string<V>'
//  'k'              - an integral number
//  'al'             - an STL-style memory allocator
//  'i1', 'i2'       - two iterators defining a sequence of 'CHAR_TYPE'
//                     characters
//  'v'              - an object of type 'V'
//  'p1', 'p2'       - two iterators belonging to 'a'
//  distance(i1,i2)  - the number of values in the range [i1, i2)
//
//  +-----------------------------------------+-------------------------------+
//  | Operation                               | Complexity                    |
//  |=========================================+===============================|
//  | basic_string<V> a (default construction)| O[1]                          |
//  | basic_string<V> a(al)                   |                               |
//  |-----------------------------------------+-------------------------------|
//  | basic_string<V> a(b) (copy construction)| O[n]                          |
//  | basic_string<V> a(b, al)                |                               |
//  |-----------------------------------------+-------------------------------|
//  | basic_string<V> a(k)                    | O[n]                          |
//  | basic_string<V> a(k, al)                |                               |
//  |-----------------------------------------+-------------------------------|
//  | basic_string<V> a(i1, i2)               | O[distance(i1,i2)]            |
//  | basic_string<V> a(i1, i2, al)           |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.~basic_string<V>()  (destruction)     | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.assign(k, v)                          | O[k]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.assign(i1, i2)                        | O[distance(i1,i2)]            |
//  |-----------------------------------------+-------------------------------|
//  | get_allocator()                         | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.begin(), a.end(),                     | O[1]                          |
//  | a.cbegin(), a.cend(),                   |                               |
//  | a.rbegin(), a.rend(),                   |                               |
//  | a.crbegin(), a.crend()                  |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.size()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.max_size()                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.resize(k)                             | O[k]                          |
//  | a.resize(k, v)                          |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.empty()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.reserve(k)                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a[k]                                    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.at(k)                                 | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.front()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.back()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.push_back()                           | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.pop_back()                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, v)                         | O[1 + distance(p1, a.end())]  |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, k, v)                      | O[k + distance(p1, a.end())]  |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, i1, i2)                    | O[distance(i1, i2)            |
//  |                                         |      + distance(p1, a.end())] |
//  |-----------------------------------------+-------------------------------|
//  | a.erase(p1)                             | O[1 + distance(p1, a.end())]  |
//  |-----------------------------------------+-------------------------------|
//  | a.erase(p1, p2)                         | O[1 + distance(p1, a.end())]  |
//  |-----------------------------------------+-------------------------------|
//  | a.swap(b), swap(a,b)                    | O[1] if 'a' and 'b' use the   |
//  |                                         | same allocator, O[n + m]      |
//  |                                         | otherwise                     |
//  |-----------------------------------------+-------------------------------|
//  | a.clear()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a = b;           (assignment)           | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a == b, a != b                          | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a < b, a <= b, a > b, a >= b            | O[n]                          |
//  +-----------------------------------------+-------------------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// In this example, we will show how to create and use the 'string' typedef.
//
// First, we will default-construct a 'string' object:
//..
//  bsl::string s;
//  assert(s.empty());
//  assert(0  == s.size());
//  assert("" == s);
//..
// Then, we will construct a 'string' object from a string literal:
//..
//  bsl::string t = "Hello World";
//  assert(!t.empty());
//  assert(11 == t.size());
//  assert("Hello World" == t);
//..
// Next, we will clear the contents of 't' and assign it a couple of values:
// first from a string literal; and second from another 'string' object:
//..
//  t.clear();
//  assert(t.empty());
//  assert("" == t);
//
//  t = "Good Morning";
//  assert(!t.empty());
//  assert("Good Morning" == t);
//
//  t = s;
//  assert(t.empty());
//  assert("" == t);
//  assert(t == s);
//..
// Then, we will create three 'string' objects: the first representing a street
// name, the second a state, and the third a zipcode.  We will then concatenate
// them into a single address 'string' and print the contents of that 'string'
// on standard output:
//..
//  const bsl::string street  = "731 Lexington Avenue";
//  const bsl::string state   = "NY";
//  const bsl::string zipcode = "10022";
//
//  const bsl::string fullAddress = street + " " + state + " " + zipcode;
//
//  bsl::cout << fullAddress << bsl::endl;
//..
// The above print statement should produce a single line of output:
//..
//  731 Lexington Avenue NY 10022
//..
// Then, we search the contents of 'address' (using the 'find' function) to
// determine if it lies on a specified street:
//..
//  const bsl::string streetName = "Lexington";
//
//  if (bsl::string::npos != fullAddress.find(streetName, 0)) {
//      bsl::cout << "The address " << fullAddress << " is located on "
//                << streetName << "." << bsl::endl;
//  }
//..
// Next, we show how to get a reference providing modifiable access to the
// null-terminated string literal stored by a 'string' object using the 'c_str'
// function.  Note that the returned string literal can be passed to various
// standard functions expecting a null-terminated string:
//..
//  const bsl::string  v = "Another string";
//  const char        *cs = v.c_str();
//  assert(bsl::strlen(cs) == v.size());
//..
// Then, we construct two 'string' objects, 'x' and 'y', using a user-specified
// allocator:
//..
//  bslma::TestAllocator allocator1, allocator2;
//
//  const char *SHORT_STRING = "A small string";
//  const char *LONG_STRING  = "This long string would definitely cause "
//                             "memory to be allocated on creation";
//
//  const bsl::string x(SHORT_STRING, &allocator1);
//  const bsl::string y(LONG_STRING,  &allocator2);
//
//  assert(SHORT_STRING == x);
//  assert(LONG_STRING  == y);
//..
// Notice that, no memory was allocated from the allocator for object 'x'
// because of the short-string optimization used in the 'string' type.
//
// Finally, we can track memory usage of 'x' and 'y' using 'allocator1' and
// 'allocator2' and check that memory was allocated only by 'allocator2':
//..
//  assert(0 == allocator1.numBlocksInUse());
//  assert(1 == allocator2.numBlocksInUse());
//..
//
///Example 2: 'string' as a Data Member
/// - - - - - - - - - - - - - - - - - -
// The most common use of 'string' objects are as data members in user-defined
// classes.  In this example, we will show how 'string' objects can be used as
// data members.
//
// First, we begin to define a 'class', 'Employee', that represents the data
// corresponding to an employee of a company:
//..
//  class Employee {
//      // This simply constrained (value-semantic) attribute class represents
//      // the information about an employee.  An employee's first and last
//      // name are represented as 'string' objects and their employee
//      // identification number is represented by an 'int'.  Note that the
//      // class invariants are identically the constraints on the individual
//      // attributes.
//      //
//      // This class:
//      //: o supports a complete set of *value-semantic* operations
//      //:   o except for 'bslx' serialization
//      //: o is *exception-neutral* (agnostic)
//      //: o is *alias-safe*
//      //: o is 'const' *thread-safe*
//
//      // DATA
//      bsl::string d_firstName;       // first name
//      bsl::string d_lastName;        // last name
//      int         d_id;              // identification number
//..
//  Next, we define the creators for this class:
//..
//    public:
//      // CREATORS
//      Employee(bslma::Allocator *basicAllocator = 0);
//          // Create a 'Employee' object having the (default) attribute
//          // values:
//          //..
//          //  firstName() == ""
//          //  lastName()  == ""
//          //  id()        == 0
//          //..
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default
//          // allocator is used.
//
//      Employee(const bslstl::StringRef&  firstName,
//               const bslstl::StringRef&  lastName,
//               int                       id,
//               bslma::Allocator         *basicAllocator = 0);
//          // Create a 'Employee' object having the specified 'firstName',
//          // 'lastName', and 'id'' attribute values.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      Employee(const Employee&   original,
//               bslma::Allocator *basicAllocator = 0);
//          // Create a 'Employee' object having the same value as the
//          // specified 'original' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      //! ~Employee() = default;
//          // Destroy this object.
//..
// Notice that all constructors of the 'Employee' class are optionally provided
// an allocator that is then passed through to the 'string' data members of
// 'Employee'.  This allows the user to control how memory is allocated by
// 'Employee' objects.  Also note that the type of the 'firstName' and
// 'lastName' arguments of the value constructor is 'bslstl::StringRef'.  The
// 'bslstl::StringRef' allows specifying a 'string' or a 'const char *' to
// represent a string value.  For the sake of brevity its implementation is
// not explored here.
//
// Then, declare the remaining methods of the class:
//..
//      // MANIPULATORS
//      Employee& operator=(const Employee& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
//      void setFirstName(const bslstl::StringRef& value);
//          // Set the 'firstName' attribute of this object to the specified
//          // 'value'.
//
//      void setLastName(const bslstl::StringRef& value);
//          // Set the 'lastName' attribute of this object to the specified
//          // 'value'.
//
//      void setId(int value);
//          // Set the 'id' attribute of this object to the specified 'value'.
//
//      // ACCESSORS
//      const bsl::string& firstName() const;
//          // Return a reference providing non-modifiable access to the
//          // 'firstName' attribute of this object.
//
//      const bsl::string& lastName() const;
//          // Return a reference providing non-modifiable access to the
//          // 'lastName' attribute of this object.
//
//      int id() const;
//          // Return the value of the 'id' attribute of this object.
//  };
//..
// Next, we declare the free operators for 'Employee':
//..
//  inline
//  bool operator==(const Employee& lhs, const Employee& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'Employee' objects have the
//      // same value if all of their corresponding values of their
//      // 'firstName', 'lastName', and 'id' attributes are the same.
//
//  inline
//  bool operator!=(const Employee& lhs, const Employee& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'Employee' objects do
//      // not have the same value if any of the corresponding values of their
//      // 'firstName', 'lastName', or 'id' attributes are not the same.
//..
// Then, we implement the various methods of the 'Employee' class:
//..
//  // CREATORS
//  inline
//  Employee::Employee(bslma::Allocator *basicAllocator)
//  : d_firstName(basicAllocator)
//  , d_lastName(basicAllocator)
//  , d_id(0)
//  {
//  }
//
//  inline
//  Employee::Employee(const bslstl::StringRef&  firstName,
//                     const bslstl::StringRef&  lastName,
//                     int                       id,
//                     bslma::Allocator         *basicAllocator)
//  : d_firstName(firstName.begin(), firstName.end(), basicAllocator)
//  , d_lastName(lastName.begin(), lastName.end(), basicAllocator)
//  , d_id(id)
//  {
//      BSLS_ASSERT_SAFE(!firstName.isEmpty());
//      BSLS_ASSERT_SAFE(!lastName.isEmpty());
//  }
//
//  inline
//  Employee::Employee(const Employee&   original,
//                     bslma::Allocator *basicAllocator)
//  : d_firstName(original.d_firstName, basicAllocator)
//  , d_lastName(original.d_lastName, basicAllocator)
//  , d_id(original.d_id)
//  {
//  }
//..
// Notice that the 'basicAllocator' parameter can simply be passed as an
// argument to the constructor of 'bsl::string'.
//
// Now, we implement the remaining manipulators of the 'Employee' class:
//..
//  // MANIPULATORS
//  inline
//  Employee& Employee::operator=(const Employee& rhs)
//  {
//      d_firstName = rhs.d_firstName;
//      d_lastName  = rhs.d_lastName;
//      d_id        = rhs.d_id;
//      return *this;
//  }
//
//  inline
//  void Employee::setFirstName(const bslstl::StringRef& value)
//  {
//      BSLS_ASSERT_SAFE(!value.isEmpty());
//
//      d_firstName.assign(value.begin(), value.end());
//  }
//
//  inline
//  void Employee::setLastName(const bslstl::StringRef& value)
//  {
//      BSLS_ASSERT_SAFE(!value.isEmpty());
//
//      d_lastName.assign(value.begin(), value.end());
//  }
//
//  inline
//  void Employee::setId(int value)
//  {
//      d_id = value;
//  }
//
//  // ACCESSORS
//  inline
//  const bsl::string& Employee::firstName() const
//  {
//      return d_firstName;
//  }
//
//  inline
//  const bsl::string& Employee::lastName() const
//  {
//      return d_lastName;
//  }
//
//  inline
//  int Employee::id() const
//  {
//      return d_id;
//  }
//..
// Finally, we implement the free operators for 'Employee' class:
//..
//  inline
//  bool operator==(const Employee& lhs, const Employee& rhs)
//  {
//      return lhs.firstName() == rhs.firstName()
//          && lhs.lastName()  == rhs.lastName()
//          && lhs.id()        == rhs.id();
//  }
//
//  inline
//  bool operator!=(const Employee& lhs, const Employee& rhs)
//  {
//      return lhs.firstName() != rhs.firstName()
//          || lhs.lastName()  != rhs.lastName()
//          || lhs.id()        != rhs.id();
//  }
//..
//
///Example 3: A Stream Text Replacement Filter
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize the 'string' type and its associated
// utility functions to define a function that reads data from an input stream,
// replaces all occurrences of a specified text fragment with another text
// fragment, and writes the resulting text to an output stream.
//
// First, we define the signature of the function, 'replace':
//..
//  void replace(bsl::ostream&      outputStream,
//               bsl::istream&      inputStream,
//               const bsl::string& oldString,
//               const bsl::string& newString)
//      // Read data from the specified 'inputStream' and replace all
//      // occurrences of the text contained in the specified 'oldString' in
//      // the stream with the text contained in the specified 'newString'.
//      // Write the modified data to the specified 'outputStream'.
//..
// Then, we provide the implementation for 'replace':
//..
//  {
//      const int   oldStringSize = oldString.size();
//      const int   newStringSize = newString.size();
//      bsl::string line;
//
//      bsl::getline(inputStream, line);
//..
// Notice that we can use the 'getline' free function defined in this component
// to read a single line of data from an input stream into a 'bsl::string'.
//..
//      if (!inputStream) {
//          return;                                                   // RETURN
//      }
//
//      do {
//..
// Next, we use the 'find' function to search the contents of 'line' for
// characters matching the contents of 'oldString':
//..
//          int pos = line.find(oldString);
//          while (bsl::string::npos != pos) {
//..
// Now, we use the 'replace' method to modify the contents of 'line' matching
// 'oldString' to 'newString':
//..
//              line.replace(pos, oldStringSize, newString);
//              pos = line.find(oldString, pos + newStringSize);
//..
// Notice that we provide 'find' with the starting position from which to start
// searching.
//..
//          }
//..
// Finally, we write the updated contents of 'line' to the output stream:
//..
//          outputStream << line;
//
//          bsl::getline(inputStream, line);
//      } while (inputStream);
//  }
//..

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

#ifndef INCLUDED_BSLALG_CONTAINERBASE
#include <bslalg_containerbase.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHARITHMETICTYPE
#include <bslmf_matcharithmetictype.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
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

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_ISTREAM
#include <istream>  // for 'std::basic_istream', 'sentry'
#define INCLUDED_ISTREAM
#endif

#ifndef INCLUDED_LIMITS
#include <limits>
#define INCLUDED_LIMITS
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>  // for 'std::basic_ostream', 'sentry'
#define INCLUDED_OSTREAM
#endif

#ifndef INCLUDED_STRING
#include <string>  // for 'native_std::char_traits'
#define INCLUDED_STRING
#endif


namespace bsl {

// Import 'char_traits' into the 'bsl' namespace so that 'basic_string' and
// 'char_traits' are always in the same namespace.
using native_std::char_traits;

template <class CHAR_TYPE,
          class CHAR_TRAITS = char_traits<CHAR_TYPE>,
          class ALLOCATOR = allocator<CHAR_TYPE> >
class basic_string;

// TYPEDEFS
typedef basic_string<char>    string;
typedef basic_string<wchar_t> wstring;

#if defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_CMP_HP)
template <class ORIGINAL_TRAITS>
class String_Traits {
    // This 'class' provides an implementation of the 'find' function for the
    // parameterized 'ORIGINAL_TRAITS' type.  This is an alternate
    // representation for Sun's 'char_traits::find' that returns an incorrect
    // result for character types other than 'char' (such as 'wchar').

    // PRIVATE TYPES
    typedef typename ORIGINAL_TRAITS::char_type char_type;
    typedef native_std::size_t                  size_type;

  public:
    // CLASS METHODS
    static const char_type *find(const char_type  *s,
                                 size_type         n,
                                 const char_type&  a);
        // Return an address providing non-modifiable access to the first
        // character that matches the specified character 'a' in the specified
        // 'n' characters of the specified 's' string.  The behavior is
        // undefined unless 's' holds at least 'n' characters.
};

template <>
class String_Traits<native_std::char_traits<char> > {
    // Sun implemented 'find' for 'char' properly, so this specialization
    // simply forwards the call to Sun.

    // PRIVATE TYPES
    typedef native_std::size_t size_type;

  public:
    // CLASS METHODS
    static const char *find(const char *s, size_type n, const char& a);
        // Return an address providing non-modifiable access to the first
        // character that matches the specified character 'a' in the specified
        // 'n' characters of the specified 's' string.  The behavior is
        // undefined unless 's' holds at least 'n' characters.
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

template <class CHAR_TYPE, class SIZE_TYPE>
class String_Imp {
    // This component private 'class' describes the basic data layout for a
    // string class and provides methods to help encapsulate internal string
    // implementation details.  It is parameterized by 'CHAR_TYPE' and
    // 'SIZE_TYPE' only, and implements the portion of 'basic_string' that does
    // not need to know about its parameterized 'CHAR_TRAITS' or 'ALLOCATOR'.
    // It contains the following data fields: pointer to string, short string
    // buffer, length, and capacity.  The purpose of the short string buffer is
    // to implement a "short string optimization" such that strings with
    // lengths shorter than a certain number of characters are stored directly
    // inside the string object (inside the short string buffer), and thereby
    // avoid memory allocations/deallocations.

  public:
    // TYPES
    enum ShortBufferConstraints {
        // This 'enum' contains values necessary to calculate the size of the
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

        SHORT_BUFFER_MIN_BYTES  = 20, // minimum required size of the short
                                      // string buffer in bytes

        SHORT_BUFFER_NEED_BYTES =
                              (SHORT_BUFFER_MIN_BYTES + sizeof(SIZE_TYPE) - 1)
                                                    & ~(sizeof(SIZE_TYPE) - 1),
                                    // round it to a word boundary

        SHORT_BUFFER_BYTES      = sizeof(CHAR_TYPE) < SHORT_BUFFER_NEED_BYTES
                                  ? SHORT_BUFFER_NEED_BYTES
                                  : sizeof(CHAR_TYPE),
                                    // in case 'CHAR_TYPE' is very large

        SHORT_BUFFER_LENGTH     = SHORT_BUFFER_BYTES / sizeof(CHAR_TYPE),

        SHORT_BUFFER_CAPACITY   = SHORT_BUFFER_LENGTH - 1
                                    // short string buffer capacity (not
                                    // including the null-terminator)
    };

    // Make sure the buffer is large enough to fit a pointer.
    BSLMF_ASSERT(SHORT_BUFFER_BYTES >= sizeof(CHAR_TYPE *));

    enum ConfigurableParameters {
        // These configurable parameters define various aspects of the string
        // behavior when it's not strictly defined by the Standard.

        BASIC_STRING_DEALLOCATE_IN_CLEAR  = false,
        BASIC_STRING_HONOR_SHRINK_REQUEST = false
    };

    // DATA
    union {
        // This is the union of the string storage options: it can either be
        // stored inside the short string buffer, 'd_short', or in the
        // externally allocated memory, pointed to by 'd_start_p'.

        BloombergLP::bsls::AlignedBuffer<
                        SHORT_BUFFER_BYTES,
                        BloombergLP::bsls::AlignmentFromType<CHAR_TYPE>::VALUE>
                   d_short;     // short string buffer
        CHAR_TYPE *d_start_p;   // pointer to the data on heap
    };

    SIZE_TYPE      d_length;    // length of the string
    SIZE_TYPE      d_capacity;  // capacity to which the string can grow
                                // without reallocation

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(String_Imp,
                                   BloombergLP::bslmf::IsBitwiseMoveable);
        // 'CHAR_TYPE' is required to be a POD as per the Standard, which makes
        // 'CHAR_TYPE' bitwise-moveable, so 'String_Imp' is also
        // bitwise-moveable.

    // CLASS METHODS
    static SIZE_TYPE computeNewCapacity(SIZE_TYPE newLength,
                                        SIZE_TYPE oldCapacity,
                                        SIZE_TYPE maxSize);
        // Compute and return the capacity required for a string having the
        // specified 'newLength' and using the specified 'oldCapacity' to
        // exercise an exponential capacity growth necessary to ensure the
        // amortized linear complexity of 'push_back' and other operations and
        // ensuring that the new capacity does not exceed the specified
        // 'maxSize'.  Note that the behavior is undefined unless
        // 'newLength > oldCapacity', 'newLength < maxSize', and
        // 'oldCapacity < maxSize'.

    // CREATORS
    String_Imp();
        // Create a 'String_Imp' object having (default) attribute values
        // except that the 'd_capacity' attribute is initialized with
        // 'SHORT_BUFFER_CAPACITY'.

    String_Imp(SIZE_TYPE length, SIZE_TYPE capacity);
        // Create a 'String_Imp' object and initialize the 'd_length' and
        // 'd_capacity' attributes with the specified 'length' and specified
        // 'capacity', respectively.  If 'capacity' is less than
        // 'SHORT_BUFFER_CAPACITY', then d_capacity is set to
        // 'SHORT_BUFFER_CAPACITY'.  The value of the 'd_short' and 'd_start_p'
        // fields are left uninitialized.  'basic_string' is required to assign
        // either d_short or d_start_p to a proper value before using any
        // methods of this class.

    //! String_Imp(const String_Imp& original) = default;
        // Create a 'String_Imp' object having the same value as the specified
        // 'original' object.  Note that this copy constructor is generated by
        // the compiler.

    //! ~String_Imp() = default;
        // Destroy this object.  Note that this destructor is generated by the
        // compiler.

    //! String_Imp& operator=(const String_Imp& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that this assignment operator is generated by the compiler.

    // MANIPULATORS
    void swap(String_Imp& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    void resetFields();
        // Reset all fields of this object to their default-constructed state.

    CHAR_TYPE *dataPtr();
        // Return an address providing modifiable access to the NULL-terminated
        // C-string stored by this string object.  Note that the returned
        // address can point to either the internal short string buffer or the
        // externally allocated memory depending on the type of the string
        // defined by the return value of 'isShortString'.

    // ACCESSORS
    bool isShortString() const;
        // Return 'true' if this object contains a short string and the string
        // data is stored in the short string buffer, and 'false' if the object
        // contains a long string (and the short string buffer contains a
        // pointer to the string data allocated externally).

    const CHAR_TYPE *dataPtr() const;
        // Return an address providing non-modifiable access to the
        // NULL-terminated C-string stored by this string object.  Note that
        // the returned address can point to either the internal short string
        // buffer or the externally allocated memory depending on the type of
        // the string defined by the return value of 'isShortString'.
};

                        // =======================
                        // class bsl::basic_string
                        // =======================

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_string
    : private String_Imp<CHAR_TYPE, typename ALLOCATOR::size_type>
    , public BloombergLP::bslalg::ContainerBase<ALLOCATOR>
{
    // This class template provides an STL-compliant 'string' that conforms to
    // the 'bslma::Allocator' model.  For the requirements of a string class,
    // consult the second revision of the ISO/IEC 14882 Programming Language
    // C++ (2003).  Note that the parameterized 'CHAR_TYPE' must be *equal* to
    // 'ALLOCATOR::value_type'.  In addition, this implementation offers strong
    // exception guarantees (see below), with the general rules that:
    //
    //: 1 any method that would result in a string of length larger than the
    //:   size returned by 'max_size' throws 'std::length_error', and
    //:
    //: 2 any method that attempts to access a position outside the valid range
    //:   of a string throws 'std::out_of_range'.
    //
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

    // 'to_string' functions are made friends to allow access to the internal
    // short string buffer.
    friend string to_string(int);
    friend string to_string(long);
    friend string to_string(long long);
    friend string to_string(unsigned);
    friend string to_string(unsigned long);
    friend string to_string(unsigned long long);

  private:
    // PRIVATE TYPES
    typedef String_Imp<CHAR_TYPE, typename ALLOCATOR::size_type> Imp;

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
        // Copy the specified 'original' string content into this string
        // object, assuming that the default copy constructor of the
        // 'String_Imp' base class and the appropriate copy constructor of the
        // 'bslstl::ContainerBase' base class have just been run.

    basic_string& privateAppendDispatch(iterator begin,
                                        iterator end);
    basic_string& privateAppendDispatch(const_iterator begin,
                                        const_iterator end);
    template <class INPUT_ITER>
    basic_string& privateAppendDispatch(INPUT_ITER begin,
                                        INPUT_ITER end);
        // Match either 'iterator', 'const_iterator', or an arbitrary iterator
        // (which can also match an integral type).  In the first two cases,
        // use 'privateAppendRaw'.  In the last case, forward to
        // 'privateReplaceDispatch' to separate the integral type from iterator
        // types.  The behavior is undefined unless the specified 'begin' and
        // 'end' refer to a sequence of valid values where 'begin' is at a
        // position at or before 'end'.

    basic_string& privateAssign(const CHAR_TYPE *characterString,
                                size_type        numChars);
        // Assign characters from the specified 'characterString' array of
        // characters of the specified 'numChars' length to this string
        // discarding the old content of the string, and return a reference
        // providing modifiable access to this string.  The behavior is
        // undefined unless 'numChars <= max_size()', and the 'characterString'
        // array is at least 'numChars' long.

    basic_string& privateAssign(size_type numChars,
                                CHAR_TYPE character);
        // Assign the specified 'numChars' copies of the specified 'character'
        // to this string discarding the old content of the string, and return
        // a reference providing modifiable access to this string.  The
        // behavior is undefined unless 'numChars <= max_size()'.

    basic_string& privateAppendRaw(const CHAR_TYPE *characterString,
                                   size_type        numChars);
        // Append characters from the specified 'characterString' array of
        // characters of the specified 'numChars' length to this string, and
        // return a reference providing modifiable access to this string.  The
        // behavior is undefined unless 'numChars <= max_size() - length()',
        // and the 'characterString' array is at least 'numChars' long.

    basic_string& privateAppendRaw(size_type numChars,
                                   CHAR_TYPE character);
        // Append the specified 'numChars' copies of the specified 'character'
        // to this string.  Return a reference providing modifiable access to
        // this string.  The behavior is undefined unless and
        // 'numChars <= max_size() - length()'.

    Imp& privateBase();
        // Return a reference providing modifiable access to the base object
        // of this string.

    void privateClear(bool deallocateBufferFlag);
        // Reset this string object to its default-constructed value and
        // deallocate its string buffer if the specified 'deallocateBufferFlag'
        // is 'true'.

    void privateInitDispatch(iterator begin,
                             iterator end);
    void privateInitDispatch(const_iterator begin,
                             const_iterator end);
        // Initialize this object with a string represented by the specified
        // 'begin' and 'end' iterators using the 'privateAppendRaw' method for
        // the initialization.  The behavior is undefined unless 'begin' and
        // 'end' refer to a sequence of valid values where 'begin' is at a
        // position at or before 'end'.

    template <class INPUT_ITER>
    void privateInitDispatch(INPUT_ITER begin,
                             INPUT_ITER end);
        // Initialize this object with a string represented by the specified
        // 'begin' and 'end' iterators.  The behavior is undefined unless
        // 'begin' and 'end' refer to a sequence of valid values where 'begin'
        // is at a position at or before 'end'.  Note that since the
        // parameterized 'INPUT_ITER' type can also resolve to an integral
        // type, use the 'privateReplaceDispatch' to disambiguate between the
        // integral type and iterator types.

    void privateInsertDispatch(const_iterator position,
                               iterator       first,
                               iterator       last);
    void privateInsertDispatch(const_iterator position,
                               const_iterator first,
                               const_iterator last);
        // Insert into this object at the specified 'position' a string
        // represented by the specified 'first' and 'last' iterators using the
        // 'privateInsertRaw' method for insertion.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.

    template <class INPUT_ITER>
    void privateInsertDispatch(const_iterator position,
                               INPUT_ITER     first,
                               INPUT_ITER     last);
        // Insert into this object at the specified 'position' a string
        // represented by the specified 'first' and 'last' iterators.  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.
        // Note that since the parameterized 'INPUT_ITER' type can also resolve
        // to an integral type, use the 'privateReplaceDispatch' to
        // disambiguate between the integral type and iterator types.

    basic_string& privateInsertRaw(size_type        outPosition,
                                   const CHAR_TYPE *characterString,
                                   size_type        numChars);
        // Insert into this object at the specified 'outPosition' the specified
        // 'numChars' starting at the specified 'characterString'.  The
        // behavior is undefined unless and 'numChars <= max_size() - length()'
        // and 'characterString' array is at least 'numChars' long.  Note that
        // this method is alias-safe, i.e., it works correctly even if
        // 'characterString' points into this string object.

    basic_string& privateReplaceRaw(size_type        outPosition,
                                    size_type        outNumChars,
                                    const CHAR_TYPE *characterString,
                                    size_type        numChars);
        // Replace the specified 'outNumChars' characters of this object
        // starting at the specified 'outPosition' by the specified 'numChars'
        // starting at the specified 'characterString', and return a reference
        // providing modifiable access to this string.  The behavior is
        // undefined unless 'outPosition <= length()',
        // 'outNumChars <= length()', 'outPosition <= length() - outNumChars',
        // 'numChars <= max_size()', and
        // 'length() - outNumChars <= max_size() - numChars'.  Note that this
        // method is alias-safe, i.e., it works correctly even if
        // 'characterString' points into this string object.

    basic_string& privateReplaceRaw(size_type outPosition,
                                    size_type outNumChars,
                                    size_type numChars,
                                    CHAR_TYPE character);
        // Replace the specified 'outNumChars' characters of this string
        // starting at the specified 'outPosition' by the specified 'numChars'
        // copies of the specified 'character', and return a reference
        // providing modifiable access to this string.  The behavior is
        // undefined unless 'outPosition <= length()',
        // 'outNumChars <= length()', 'outPosition <= length() - outNumChars'
        // and 'length() <= max_size() - numChars'.

    template <class INPUT_ITER>
    basic_string& privateReplaceDispatch(
                              size_type                               position,
                              size_type                               numChars,
                              INPUT_ITER                              first,
                              INPUT_ITER                              last,
                              BloombergLP::bslmf::MatchArithmeticType ,
                              BloombergLP::bslmf::Nil                 );
        // Match integral type for 'INPUT_ITER'.

    template <class INPUT_ITER>
    basic_string& privateReplaceDispatch(
                                     size_type                        position,
                                     size_type                        numChars,
                                     INPUT_ITER                       first,
                                     INPUT_ITER                       last,
                                     BloombergLP::bslmf::MatchAnyType ,
                                     BloombergLP::bslmf::MatchAnyType );
        // Match non-integral type for 'INPUT_ITER'.

    template <class INPUT_ITER>
    basic_string& privateReplace(size_type  position,
                                 size_type  numChars,
                                 INPUT_ITER first,
                                 INPUT_ITER last,
                                 std::input_iterator_tag);
        // Specialized replacement for input iterators, using repeated
        // 'push_back' operations.

    template <class INPUT_ITER>
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
        // Replace the specified 'numChars' characters of this object starting
        // at the specified 'position' with the string represented by the
        // specified 'first' and 'last' iterators.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.

    void privateReserveRaw(size_type newCapacity);
        // Update the capacity of this object to be a value greater than or
        // equal to the specified 'newCapacity'.  The behavior is undefined
        // unless 'newCapacity <= max_size()'.  Note that a null-terminating
        // character is not counted in 'newCapacity', and that this method has
        // no effect unless 'newCapacity > capacity()'.

    CHAR_TYPE *privateReserveRaw(size_type *storage,
                                 size_type  newCapacity,
                                 size_type  numChars);
        // Update the capacity of this object and load into the specified
        // 'storage' to be a value greater than or equal to the specified
        // 'newCapacity'.  Upon reallocation, copy the first specified
        // 'numChars' from the previous buffer to the new buffer, and load
        // 'storage' with the new capacity.  If '*storage >= newCapacity', this
        // method has no effect.  Return the new buffer if reallocation, and 0
        // otherwise.  The behavior is undefined unless 'numChars <= length()'
        // and 'newCapacity <= max_size()'.  Note that a null-terminating
        // character is not counted in '*storage' nor 'newCapacity'.  Also note
        // that the previous buffer is *not* deallocated, nor is the string
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
        // starting at the specified 'other' address, and return a negative
        // value if this string is less than 'other', a positive value if it is
        // more than 'other', and 0 in case of equality.  The behavior is
        // undefined unless 'lhsPosition <= length()',
        // 'lhsNumChars <= length()', and
        // 'lhsPosition <= length() - lhsNumChars'.

    // INVARIANTS
    BSLMF_ASSERT((bsl::is_same<CHAR_TYPE,
                               typename ALLOCATOR::value_type>::value));
        // This is required by the C++ standard (23.1, clause 1).

  public:
    // PUBLIC CLASS DATA
    static const size_type npos = ~size_type(0);
        // Value used to denote "not-a-position", guaranteed to be outside the
        // range '[0 .. max_size()]'.

    // CREATORS

                   // *** 21.3.2 construct/copy/destroy: ***

    explicit
    basic_string(const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create an empty string.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not specified, a
        // default-constructed allocator is used.

    basic_string(const basic_string& original);
    basic_string(const basic_string& original,
                 const ALLOCATOR&    basicAllocator);
        // Create a string that has the same value as the specified 'original'
        // string.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, then a
        // default-constructed allocator is used.  Note that it is important to
        // have two copy constructors instead of a single:
        //..
        //  basic_string(const basic_string& original,
        //               const ALLCOATOR&    basicAllocator = ALLOCATOR());
        //..
        // When the copy constructor with the default allocator is used, xlC10
        // get confused and refuses to use the return value optimization, which
        // then causes extra allocations when returning by value in
        // 'operator+'.

    basic_string(const basic_string& original,
                 size_type           position,
                 size_type           numChars = npos,
                 const ALLOCATOR&    basicAllocator = ALLOCATOR());
        // Create a string that has the same value as the substring of the
        // optionally specified 'numChars' length starting at the specified
        // 'position' in the specified 'original' string.  If 'numChars' equals
        // 'npos', then the remaining length of the string is used (i.e.,
        // 'numChars' is set to 'original.length() - position').  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not specified, a default-constructed allocator
        // is used.  Throw 'out_of_range' if 'position > original.length()' or
        // 'numChars != npos' and 'position + numChars < original.length()'.

    basic_string(const CHAR_TYPE  *characterString,
                 const ALLOCATOR&  basicAllocator = ALLOCATOR());   // IMPLICIT
    basic_string(const CHAR_TYPE  *characterString,
                 size_type         numChars,
                 const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a string that has the same value as the substring of the
        // optionally specified 'numChars' length starting at the specified
        // 'position' in the specified 'characterString'.  If 'numChars' is not
        // specified, 'CHAR_TRAITS::length(characterString)' is used.
        // Optionally specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not specified, a default-constructed allocator
        // is used.  Throw 'out_of_range' if 'numChars >= npos'.

    basic_string(size_type        numChars,
                 CHAR_TYPE        character,
                 const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a string of the specified 'numChars' length whose every
        // position contains the specified 'character'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed allocator is used.

    template <class INPUT_ITER>
    basic_string(INPUT_ITER       first,
                 INPUT_ITER       last,
                 const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a string from the characters in the range starting at the
        // specified 'first' and ending at the specified 'last' iterators of
        // the parameterized 'INPUT_ITER' type.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed allocator is used.  The behavior is
        // undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.

    template <class ALLOC2>
    basic_string(
        const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& original,
        const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a string that has the same value as the specified 'original'
        // string, where the type 'original' is the string type native to the
        // compiler's library, instantiated with the same character type and
        // traits type, but not necessarily the same allocator type.  The
        // resulting string will contain the same sequence of characters as
        // 'original'.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, then a
        // default-constructed allocator is used.

    basic_string(const BloombergLP::bslstl::StringRefData<CHAR_TYPE>& strRef,
                 const ALLOCATOR& basicAllocator = ALLOCATOR());    // IMPLICIT
        // Create a string that has the same value as the specified 'strRef'
        // string.  The resulting string will contain the same sequence of
        // characters as 'strRef'.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not specified, then a
        // default-constructed allocator is used.

    ~basic_string();
        // Destroy this string object.

    // MANIPULATORS

                    // *** 21.3.2 construct/copy/destroy: ***

    basic_string& operator=(const basic_string& rhs);
        // Assign to this string the value of the specified 'rhs' string, and
        // return a reference providing modifiable access to this object.

    basic_string& operator=(const CHAR_TYPE *rhs);
        // Assign to this string the value of the specified 'rhs' string, and
        // return a reference providing modifiable access to this object.

    basic_string& operator=(CHAR_TYPE character);
        // Assign to this string the value of the string of length one
        // consisting of the specified 'character', and return a reference
        // providing modifiable access to this object.

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
        // Return an iterator referring to the first character in this
        // modifiable string (or the past-the-end iterator if this string is
        // empty).

    iterator end();
        // Return the past-the-end iterator for this modifiable string.

    reverse_iterator rbegin();
        // Return a reverse iterator referring to the last character in this
        // modifiable string (or the past-the-end reverse iterator if this
        // string is empty).

    reverse_iterator rend();
        // Return the past-the-end reverse iterator for this modifiable string.

                       // *** 21.3.5 element access: ***

    reference operator[](size_type position);
        // Return a reference providing modifiable access to the character at
        // the specified 'position' in this string if 'position < length()', or
        // a reference providing non-modifiable access to the null-terminating
        // character if 'position == length()'.  The behavior is undefined
        // unless 'position <= length()', and, in the case of
        // 'position == length()', the null-terminating character is not
        // modified through the returned reference.

    reference at(size_type position);
        // Return a reference providing modifiable access to the character at
        // the specified 'position' in this string.  Throw 'out_of_range' if
        // 'position >= length()'.

    reference front();
        // Return a reference providing modifiable access to the character at
        // the first position in this string.  The behavior is undefined if
        // this string is empty.

    reference back();
        // Return a reference providing modifiable access to the character at
        // the last position in this string.  The behavior is undefined if this
        // string is empty.  Note that the last position is 'length() - 1'.

    template <class ALLOC2>
    operator native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>() const;
        // Convert this object to a string type native to the compiler's
        // library, instantiated with the same character type and traits type,
        // but not necessarily the same allocator type.  The return string will
        // contain the same sequence of characters as 'orig' and will have a
        // default-constructed allocator.  Note that this conversion operator
        // can be invoked implicitly (e.g., during argument passing).

                         // *** 21.3.6 modifiers: ***

    basic_string& operator+=(const basic_string&  rhs);
    basic_string& operator+=(const CHAR_TYPE     *rhs);
        // Append the specified 'rhs' at the end of this string, and return a
        // reference providing modifiable access to this string.

    basic_string& operator+=(CHAR_TYPE character);
        // Append the specified 'character' at the end of this string, and
        // return a reference providing modifiable access to this string.

    basic_string& operator+=(
                  const BloombergLP::bslstl::StringRefData<CHAR_TYPE>& strRef);
        // Append the specified 'strRef' at the end of this string.  Return a
        // reference providing modifiable access to this string.

    basic_string& append(const basic_string& suffix);
    basic_string& append(const basic_string& suffix,
                         size_type           position,
                         size_type           numChars);
        // Append at the end of this string the first specified 'numChars'
        // characters starting at the specified 'position' in the specified
        // 'suffix', or the tail of 'suffix' starting at 'position' if
        // 'position + numChars' is larger than the length of 'suffix'.  Return
        // a reference providing modifiable access to this string.  Throw
        // 'out_of_range' if 'position > suffix.length()'.

    basic_string& append(const CHAR_TYPE *characterString,
                         size_type        numChars);
        // Append at the end of this string the specified 'numChars' characters
        // from the array starting at the specified 'characterString' address,
        // and return a reference to this modifiable string.

    basic_string& append(const CHAR_TYPE *characterString);
        // Append the specified 'characterString' (of length
        // 'CHAR_TRAITS::length(characterString)') at the end of this string,
        // and return a reference providing modifiable access to this string.

    basic_string& append(size_type numChars,
                         CHAR_TYPE character);
        // Append a number equal to the specified 'numChars' of copies of the
        // specified 'character' at the end of this string, and return a
        // reference providing modifiable access to this string.

    template <class INPUT_ITER>
    basic_string& append(INPUT_ITER first, INPUT_ITER last);
        // Append to the end of this string a string built from the characters
        // in the range starting at the specified 'first' and ending before the
        // specified 'last' iterators of the 'iterator', 'const_iterator' or
        // parameterized 'INPUT_ITER' type, respectively.  Return a reference
        // providing modifiable access to this string.  The behavior is
        // undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.

    void push_back(CHAR_TYPE   character);
        // Append the specified 'character' at the end of this string.

    basic_string& assign(const basic_string& replacement);
        // Assign to this string the value of the specified 'replacement'
        // string, and return a reference providing modifiable access to this
        // string.

    basic_string& assign(const basic_string& replacement,
                         size_type           position,
                         size_type           numChars);
        // Assign to this string the value of the substring of the specified
        // 'numChars' length or 'replacement.length() - position', whichever is
        // smaller, starting at the specified 'position' in the specified
        // 'replacement' string, and return a reference providing modifiable
        // access to this string.  Throw 'out_of_range' if
        // 'position > other.length()'.

    basic_string& assign(const CHAR_TYPE *characterString);
        // Assign to this string the value of the specified null-terminated
        // 'characterString', and return a reference providing modifiable
        // access to this string.

    basic_string& assign(const CHAR_TYPE *characterString,
                         size_type        numChars);
        // Assign to this string the value of the string constructed from the
        // specified 'numChars' characters in the array starting at the
        // specified 'characterString' address, and return a reference
        // providing modifiable access to this string.

    basic_string& assign(
                  const BloombergLP::bslstl::StringRefData<CHAR_TYPE>& strRef);
        // Assign to this string the value of the specified 'strRef' string,
        // and return a reference providing modifiable access to this string.

    basic_string& assign(size_type numChars, CHAR_TYPE character);
        // Assign to this string the value of a string of the specified
        // 'numChars' length whose every characters equal the specified
        // 'character', and return a reference providing modifiable access to
        // this string.

    template <class INPUT_ITER>
    basic_string& assign(INPUT_ITER first, INPUT_ITER last);
        // Assign to this string the value of a string built from the
        // characters in the range starting at the specified 'first' and ending
        // before the specified 'last' iterators of the 'iterator',
        // 'const_iterator' or parameterized 'INPUT_ITER' type, respectively.
        // Return a reference providing modifiable access to this string.  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.

    basic_string& insert(size_type position, const basic_string& other);
        // Insert at the specified 'position' in this string a copy of the
        // specified 'other' string, and return a reference providing
        // modifiable access to this string.  Throw 'out_of_range' if
        // 'position > length()'.

    basic_string& insert(size_type           outPosition,
                         const basic_string& other,
                         size_type           position,
                         size_type           numChars);
        // Insert at the specified 'outPosition' in this string a copy of the
        // substring of the specified 'numChars' length or
        // 'other.length() - position', whichever is smaller, starting at the
        // specified 'position' in the specified 'other' string, and return a
        // reference providing modifiable access to this string.  Throw
        // 'out_of_range' if 'position > length()'.

    basic_string& insert(size_type        position,
                         const CHAR_TYPE *characterString,
                         size_type        numChars);
        // Insert at the specified 'position' in this string a copy of the
        // string constructed from the specified 'numChars' characters in the
        // array starting at the specified 'characterString' address, and
        // return a reference providing modifiable access to this string.
        // Throw 'out_of_range' if 'position > length()'.

    basic_string& insert(size_type        position,
                         const CHAR_TYPE *characterString);
        // Insert at the specified 'position' in this string a copy of the
        // string constructed from the specified 'characterString' (of length
        // 'CHAR_TRAITS::length(characterString)'), and return a reference
        // providing modifiable access to this string.  Throw 'out_of_range' if
        // 'position > length()'.

    basic_string& insert(size_type position,
                         size_type numChars,
                         CHAR_TYPE character);
        // Insert at the specified 'position' in this string a number equal to
        // the specified 'numChars' of copies of the specified 'character', and
        // return a reference providing modifiable access to this string.
        // Throw 'out_of_range' if 'position > length()'.

    iterator insert(const_iterator position, CHAR_TYPE character);
        // Insert at the specified 'position' in this string a copy of the
        // specified 'character', and return an iterator providing modifiable
        // access to the inserted character.  The behavior is undefined unless
        // 'position' is a valid iterator on this string.

    iterator insert(const_iterator position,
                    size_type      numChars,
                    CHAR_TYPE      character);
        // Insert at the specified 'position' in this string a specified
        // 'numChars' number of copies of the specified 'character', and return
        // an iterator providing modifiable access to the first inserted
        // character, or a non-const copy of the 'position' iterator, if
        // 'numChars == 0'.  The behavior is undefined unless 'position' is a
        // valid iterator on this string.

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last);
        // Insert at the specified 'position' in this string a string built
        // from the characters in the range starting at the specified 'first'
        // and ending before the specified 'last' iterators, and return an
        // iterator providing modifiable access to the first inserted
        // character, or a non-const copy of the 'position' iterator, if
        // 'first == last'.  The behavior is undefined unless 'position' is a
        // valid iterator on this string, and 'first' and 'last' refer to a
        // sequence of valid values where 'first' is at a position at or before
        // 'last'.

    basic_string& erase(size_type position = 0, size_type numChars = npos);
        // Erase from this string the substring of length the optionally
        // specified 'numChars' or 'original.length() - position', whichever is
        // smaller, starting at the optionally specified 'position'.  If
        // 'position' is not specified, the first position is used (i.e.,
        // 'position' is set to 0).  Return a reference providing modifiable
        // access to this string.  If 'numChars' equals 'npos', then the
        // remaining length of the string is erased (i.e., 'numChars' is set to
        // 'length() - position').  Throw 'out_of_range' if
        // 'position > length()'.

    iterator erase(const_iterator position);
        // Erase a character at the specified 'position' from this string, and
        // return an iterator providing modifiable access to the character at
        // 'position' prior to erasing.  If no such character exists, return
        // 'end()'.  The behavior is undefined unless 'position' is within the
        // half-open range '[cbegin() .. cend())'.

    iterator erase(const_iterator first, const_iterator last);
        // Erase from this string a substring defined by the pair of 'first'
        // and 'last' iterators within this string.  Return an iterator
        // providing modifiable access to the the character at the 'last'
        // position prior to erasing.  If no such character exists, return
        // 'end()'.  This method invalidates existing iterators pointing to
        // 'first' or a subsequent position.  The behavior is undefined unless
        // 'first' and 'last' are both within the range '[cbegin() .. cend()]'
        // and 'first <= last'.

    void pop_back();
        // Erase the last character from this string.  The behavior is
        // undefined if this string is empty.

    basic_string& replace(size_type           outPosition,
                          size_type           outNumChars,
                          const basic_string& replacement);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by the specified 'replacement', and return a
        // reference providing modifiable access to this string.  Throw
        // 'out_of_range' if 'outPosition > length()'.

    basic_string& replace(size_type           outPosition,
                          size_type           outNumChars,
                          const basic_string& replacement,
                          size_type           position,
                          size_type           numChars);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by the substring of the specified 'numChars'
        // length or 'replacement.length() - position', whichever is smaller,
        // starting at the specified 'position' in the specified 'replacement'.
        // Return a reference providing modifiable access to this string.
        // Throw 'out_of_range' if 'outPosition > length()' or
        // 'position > string.length()'.

    basic_string& replace(size_type        outPosition,
                          size_type        outNumChars,
                          const CHAR_TYPE *characterString,
                          size_type        numChars);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by a copy of the string constructed from the
        // specified 'numChars' characters in the array starting at the
        // specified 'characterString' address.  Return a reference providing
        // modifiable access to this string.  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(size_type        outPosition,
                          size_type        outNumChars,
                          const CHAR_TYPE *characterString);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by the null-terminated specified
        // 'characterString' (of length
        // 'CHAR_TRAITS::length(characterString)').  Return a reference
        // providing modifiable access to this string.  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(size_type outPosition,
                          size_type outNumChars,
                          size_type numChars,
                          CHAR_TYPE character);
        // Replace the substring of this string starting at the specified
        // 'outPosition' of length 'outNumChars' or 'length() - outPosition',
        // whichever is smaller, by a number equal to the specified 'numChars'
        // of copies of the specified 'character'.  Return a reference
        // providing modifiable access to this string.  Throw 'out_of_range' if
        // 'outPosition > length()'.

    basic_string& replace(const_iterator      first,
                          const_iterator      last,
                          const basic_string& replacement);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by
        // the specified 'replacement'.  Return a reference providing
        // modifiable access to this string.  The behavior is undefined unless
        // 'first' and 'last' are both within the range '[cbegin() .. cend()]'
        // and 'first <= last'.

    basic_string& replace(const_iterator   first,
                          const_iterator   last,
                          const CHAR_TYPE *characterString,
                          size_type        numChars);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // copy of the string constructed from the specified 'numChars'
        // characters in the array starting at the specified 'characterString'
        // address.  Return a reference providing modifiable access to this
        // string.  The behavior is undefined unless 'first' and 'last' are
        // both within the range '[cbegin() .. cend()]' and 'first <= last'.

    basic_string& replace(const_iterator   first,
                          const_iterator   last,
                          const CHAR_TYPE *characterString);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by
        // the null-terminated specified 'characterString'.  Return a reference
        // providing modifiable access to this string.  The behavior is
        // undefined unless 'first' and 'last' are both within the range
        // '[cbegin() .. cend()]' and 'first <= last'.

    basic_string& replace(const_iterator first,
                          const_iterator last,
                          size_type      numChars,
                          CHAR_TYPE      character);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // number equal to the specified 'numChars' of copies of the specified
        // 'character'.  Return a reference providing modifiable access to this
        // string.  The behavior is undefined unless 'first' and 'last' are
        // both within the range '[cbegin() .. cend()]' and 'first <= last'.

    template <class INPUT_ITER>
    basic_string& replace(const_iterator first,
                          const_iterator last,
                          INPUT_ITER     stringFirst,
                          INPUT_ITER     stringLast);
        // Replace the substring in the range starting at the specified 'first'
        // position and ending right before the specified 'last' position, by a
        // string built from the characters in the range starting at the
        // specified 'stringFirst' and ending before the specified 'stringLast'
        // iterators of the 'iterator', 'const_iterator', or parameterized
        // 'INPUT_ITER' type, respectively.  Return a reference providing
        // modifiable access to this string.  The behavior is undefined unless
        // 'first' and 'last' are both within the range '[cbegin() .. cend()]',
        // 'first <= last', and '[stringFirst .. stringLast)' is a valid
        // iterator range.

    void swap(basic_string& other);
        // Exchange the value of this string with that of the specified
        // 'string', so that the value of this string upon return equals that
        // of 'other' prior to this call, and vice-versa.

    // ACCESSORS

                     // *** 21.3.3 iterators: ***

    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // character of this string (or the past-the-end iterator if this
        // string is empty).

    const_iterator end() const;
    const_iterator cend() const;
        // Return the past-the-end iterator for this string.

    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last character of this string (or the past-the-end reverse iterator
        // if this string is empty).

    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
        // Return the past-the-end reverse iterator for this string.

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
        // requests to create a string longer than this number of characters
        // are guaranteed to raise a 'length_error' exception.

    size_type capacity() const;
        // Return the capacity of this string, i.e., the maximum length for
        // which resizing is guaranteed not to trigger a reallocation.

    bool empty() const;
        // Return 'true' if this string has length 0, and 'false' otherwise.

                       // *** 21.3.5 element access: ***

    const_reference operator[](size_type position) const;
        // Return a reference providing non-modifiable access to the character
        // at the specified 'position' in this string.  The behavior is
        // undefined unless 'position <= length()'.  Note that if
        // 'position == length()', a reference to the null-terminating
        // character is returned.

    const_reference at(size_type position) const;
        // Return a reference providing non-modifiable access to the character
        // at the specified 'position' in this string.  Throw 'out_of_range' if
        // 'position >= length()'.

    const_reference front() const;
        // Return a reference providing non-modifiable access to the character
        // at the first position in this string.  The behavior is undefined if
        // this string is empty.

    const_reference back() const;
        // Return a reference providing non-modifiable access to the character
        // at the last position in this string.  The behavior is undefined if
        // this string is empty.  Note that the last position is
        // 'length() - 1'.

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
        // Return an address providing non-modifiable access to the
        // null-terminated buffer of 'length() + 1' characters whose contents
        // are identical to the value of this string.  Note that any call to
        // the string destructor or any of its manipulators invalidates the
        // returned pointer.

    const_pointer data() const;
        // Return an address providing non-modifiable access to the
        // null-terminated buffer of 'length()' characters whose contents are
        // identical to the value of this string.  Note that any call to the
        // string destructor or any of its manipulators invalidates the
        // returned pointer.

    allocator_type get_allocator() const;
        // Return the allocator used by this string to supply memory.

    size_type find(const basic_string& substring,
                   size_type           position = 0) const;
        // Return the starting position of the *first* occurrence of the
        // specified 'substring', if such a substring can be found in this
        // string (on or *after* the optionally specified 'position' if such a
        // 'position' is specified) using 'CHAR_TRAITS::eq' to compare
        // characters, and return 'npos' otherwise.

    size_type find(const CHAR_TYPE *substring,
                   size_type        position,
                   size_type        numChars) const;
    size_type find(const CHAR_TYPE *substring,
                   size_type        position = 0) const;
        // Return the starting position of the *first* occurrence of the
        // specified 'substring' of the optionally specified 'numChars' length,
        // if such a substring can be found in this string (on or *after* the
        // optionally specified 'position' if such a 'position' is specified)
        // using 'CHAR_TRAITS::eq' to compare characters, and return 'npos'
        // otherwise.  If 'numChars' is not specified,
        // 'CHAR_TRAITS::length(substring)' is used.

    size_type find(CHAR_TYPE character, size_type position = 0) const;
        // Return the position of the *first* occurrence of the specified
        // 'character', if such an occurrence can be found in this string (on
        // or *after* the optionally specified 'position' if such a 'position'
        // is specified), and return 'npos' otherwise.

    size_type rfind(const basic_string& substring,
                    size_type           position = npos) const;
        // Return the starting position of the *last* occurrence of the
        // specified 'substring' within this string, if such a sequence can be
        // found in this string (on or *before* the optionally specified
        // 'position' if such a 'position' is specified) using
        // 'CHAR_TRAITS::eq' to compare characters, and return 'npos'
        // otherwise.

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

    size_type find_first_of(const basic_string& characterString,
                            size_type           position = 0) const;
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this string (on or *after* the optionally specified
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

    size_type find_last_of(const basic_string& characterString,
                           size_type           position = npos) const;
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this string (on or *before* the optionally specified
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

    size_type find_first_not_of(const basic_string& characterString,
                                size_type           position = 0) const;
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this string (on or *after* the optionally specified
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

    size_type find_last_not_of(const basic_string& characterString,
                               size_type           position = npos) const;
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this string (on or *before* the optionally specified
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
        // Return a string whose value is the substring  starting at the
        // optionally specified 'position' in this string, of length the
        // optionally specified 'numChars' or 'length() - position', whichever
        // is smaller.  If 'position' is not specified, 0 is used (i.e., the
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
        // the specified 'lhsPosition' of length 'lhsNumChars' or 'length() -
        // lhsPosition', whichever is smaller, with the specified 'other'
        // string (using 'CHAR_TRAITS::lt' to compare characters), and return a
        // negative value if this string is less than 'other', a positive value
        // if it is more than 'other', and 0 in case of equality.  See
        // "Lexicographical Comparisons" for definitions.

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
        // starting at the specified 'other' address, and return a negative
        // value if this string is less than 'other', a positive value if it is
        // more than 'other', and 0 in case of equality.  Throw 'out_of_range'
        // if 'lhsPosition > length()'.  See "Lexicographical Comparisons" for
        // definitions.

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const CHAR_TYPE                                      *lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(CHAR_TYPE                                             lhs,
          const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  lhs,
          const CHAR_TYPE                                      *rhs);
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

int  stoi(const string&  str, std::size_t* pos = 0, int base = 10);
int  stoi(const wstring& str, std::size_t* pos = 0, int base = 10);
long stol(const string&  str, std::size_t* pos = 0, int base = 10);
long stol(const wstring& str, std::size_t* pos = 0, int base = 10);
unsigned long stoul(const string&  str, std::size_t* pos = 0, int base = 10);
unsigned long stoul(const wstring& str, std::size_t* pos = 0, int base = 10);

#if !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR < 1800)
long long stoll(const string&  str, std::size_t* pos = 0, int base = 10);
long long stoll(const wstring& str, std::size_t* pos = 0, int base = 10);
unsigned long long stoull(const string&   str,
                          std::size_t    *pos = 0,
                          int             base = 10);
unsigned long long stoull(const wstring&  str,
                          std::size_t    *pos = 0,
                          int             base = 10);
#endif
    // Parses 'str' interpreting its content as an integral number. Optionally
    // specify 'pos' whose value is set to the position of the next character
    // after the numerical value.  Optionally specify 'base' used to change the
    // interpretation of 'str' to a integral number written in the specified
    // 'base'.  Valid bases are in the range of [0,35] where base 0
    // automatically determines the base of the string; The base will be 16 if
    // the number is prefixed with '0x' or '0X', base 8 if the number is
    // prefixed with a '0' and base 10 otherwise.  The function ignores leading
    // white space characters and interprets as many characters possible to
    // form a valid base n integral number.  If no conversion could be
    // performed, then an invalid_argument exception is thrown.  If the value
    // read is out of range of the return type, then an out_of_range exception
    // is thrown.

float  stof(const string&  str, std::size_t* pos =0);
float  stof(const wstring& str, std::size_t* pos =0);
double stod(const string&  str, std::size_t* pos =0);
double stod(const wstring& str, std::size_t* pos =0);

#if !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR < 1800)
long double stold(const string&  str, std::size_t* pos =0);
long double stold(const wstring& str, std::size_t* pos =0);
#endif
    // Parses 'str' interpreting its contents as a floating point number. In
    // C++11 if the number in 'str' is prefixed with '0x' or '0X' the string
    // will be interpreted as a hex number.  If there is no leading 0x or 0X
    // the string will be interpreted as a decimal number.  Optionally specify
    // 'pos' whose value is set to the position of the next character after the
    // numerical value.  The function ignores leading white space characters
    // and interprets as many characters possible to form a valid floating
    // point number.  If no conversion could be performed, then an
    // 'invalid_argument' exception is thrown.  If the value read is out of
    // range of the return type, then an out_of_range exception is thrown.

string to_string(int value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::sprintf(buf,"%d", value)' would produce with a sufficiently large
    // buffer.
string to_string(long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::sprintf(buf,"%ld", value)' would produce with a sufficiently large
    // buffer.
string to_string(long long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::sprintf(buf,"%lld", value)' would produce with a sufficiently
    // large buffer.
string to_string(unsigned value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::sprintf(buf,"%u", value)' would produce with a sufficiently large
    // buffer.
string to_string(unsigned long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::sprintf(buf,"%lu", value)' would produce with a sufficiently large
    // buffer.
string to_string(unsigned long long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::sprintf(buf,"%llu", value)' would produce with a sufficiently
    // large buffer.
string to_string(float value);
string to_string(double value);
    // converts a floating point value to a string with the same contents as
    // what 'std::sprintf(buf, "%f", value)' would produce for a suficiently
    // large buffer.
string to_string(long double value);
    // converts a floating point value to a string with the same contents as
    // what 'std::sprintf(buf, "%Lf", value)' would produce for a suficiently
    // large buffer.

wstring to_wstring(int value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::swprintf(buf,L"%d", value)' would produce with a sufficiently
    // large buffer.
wstring to_wstring(long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::swprintf(buf,L"%ld", value)' would produce with a sufficiently
    // large buffer.
wstring to_wstring(long long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::swprintf(buf,L"%lld", value)' would produce with a sufficiently
    // large buffer.
wstring to_wstring(unsigned value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::swprintf(buf,L"%u", value)' would produce with a sufficiently
    // large buffer.
wstring to_wstring(unsigned long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::swprintf(buf,L"%lu", value)' would produce with a sufficiently
    // large buffer.
wstring to_wstring(unsigned long long value);
    // Constructs a string with contents equal to the specified 'value'. The
    // contents of the string will be the same as what
    // 'std::swprintf(buf,L"%llu", value)' would produce with a sufficiently
    // large buffer.
wstring to_wstring(float value);
wstring to_wstring(double value);
    // converts a floating point value to a string with the same contents as
    // what 'std::sprintf(buf, sz, L"%f", value)' would produce for a
    // suficiently large buffer.
wstring to_wstring(long double value);
    // converts a floating point value to a string with the same contents as
    // 'what std::sprintf(buf, sz, L"%Lf", value)' would produce for a
    // suficiently large buffer.

enum MaxDecimalStringLengths{
    // This 'enum' give upper bounds on the maximum string lengths storing each
    // scalar numerical type.  It is safe to use stack-allocated buffers of
    // these sizes for generating decimal representations of the corresponding
    // type, including sign and terminating null character, using the default
    // precision of 6 significant digits for floating point types.

    e_MAX_SHORT_STRLEN10      = 2 + sizeof(short) * 3,
    e_MAX_INT_STRLEN10        = 2 + sizeof(int) * 3,
    e_MAX_INT64_STRLEN10      = 26,
    e_MAX_FLOAT_STRLEN10      = 48,
    e_MAX_DOUBLE_STRLEN10     = 318,
    e_MAX_LONGDOUBLE_STRLEN10 = 318,
    e_MAX_SCALAR_STRLEN10     = e_MAX_INT64_STRLEN10
};

// HASH SPECIALIZATIONS
template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void hashAppend(HASHALG& hashAlg,
                const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>&  input);
    // Pass the specified 'input' string to the specified 'hashAlg' hashing
    // algorithm of (template parameter) type 'HASHALG'.

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::size_t
hashBasicString(const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str);
    // Return a hash value for the specified 'str'.

std::size_t hashBasicString(const string& str);
    // Return a hash value for the specified 'str'.

std::size_t hashBasicString(const wstring& str);
    // Return a hash value for the specified 'str'.

}  // close namespace bsl

namespace BloombergLP {
namespace bslh {

template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void hashAppend(
    HASHALG&                                                           hashAlg,
    const native_std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& input);
    // Pass the specified 'input' string to the specified 'hashAlg' hashing
    // algorithm of (template parameter) type 'HASHALG'.  Note that this
    // function violates the BDE coding standard, adding a function for a
    // namespace for a different package, and none of the function parameters
    // are from this package either.  This is necessary in order to provide an
    // implementation of 'bslh::hashAppend' for the (native) standard library
    // 'string' type as we are not allowed to add overloads directly into
    // namespace 'std', and this component essentially provides the interface
    // between 'bsl' and 'std' string types.

}  // close namespace bslh
}  // close enterprise namespace

// ============================================================================
//                       FUNCTION TEMPLATE DEFINITIONS
// ============================================================================
// See IMPLEMENTATION NOTES in the '.cpp' before modifying anything below.

namespace bsl {
                          // ----------------
                          // class String_Imp
                          // ----------------

// CLASS METHODS
template <class CHAR_TYPE, class SIZE_TYPE>
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
template <class CHAR_TYPE, class SIZE_TYPE>
BSLS_PLATFORM_AGGRESSIVE_INLINE
String_Imp<CHAR_TYPE, SIZE_TYPE>::String_Imp()
: d_start_p(0)
, d_length(0)
, d_capacity(SHORT_BUFFER_CAPACITY)
{
}

template <class CHAR_TYPE, class SIZE_TYPE>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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
template <class CHAR_TYPE, class SIZE_TYPE>
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
        BloombergLP::bslalg::ScalarPrimitives::swap(*this, other);
    }
}

// PRIVATE MANIPULATORS
template <class CHAR_TYPE, class SIZE_TYPE>
inline
void String_Imp<CHAR_TYPE, SIZE_TYPE>::resetFields()
{
    d_start_p  = 0;
    d_length   = 0;
    d_capacity = SHORT_BUFFER_CAPACITY;
}

template <class CHAR_TYPE, class SIZE_TYPE>
inline
CHAR_TYPE *String_Imp<CHAR_TYPE, SIZE_TYPE>::dataPtr()
{
    return isShortString()
           ? reinterpret_cast<CHAR_TYPE *>((void *)d_short.buffer())
           : d_start_p;
}

// PRIVATE ACCESSORS
template <class CHAR_TYPE, class SIZE_TYPE>
inline
bool String_Imp<CHAR_TYPE, SIZE_TYPE>::isShortString() const
{
    return d_capacity == SHORT_BUFFER_CAPACITY;
}

template <class CHAR_TYPE, class SIZE_TYPE>
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
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
const typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::npos;

// PRIVATE MANIPULATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
CHAR_TYPE *
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAllocate(
                                                            size_type numChars)
{
    return this->allocateN((CHAR_TYPE *)0, numChars + 1);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateDeallocate()
{
    if (!this->isShortString()) {
        this->deallocateN(this->d_start_p, this->d_capacity + 1);
    }
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAppendDispatch(
                                                                iterator begin,
                                                                iterator end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    return privateAppendDispatch(const_iterator(begin), const_iterator(end));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                            "string<...>::append<Iter>(i,j): string too long");
    }
    return privateAppendRaw(&*begin, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
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
                                  BloombergLP::bslmf::Nil()); // disambiguate
                                                              // overloads
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAssign(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(numChars <= max_size());

    if (numChars <= capacity()) {
        // no reallocation required, perform assignment in-place

        this->d_length = 0;
        return privateAppendRaw(characterString, numChars);           // RETURN
    }
    else {
        // reallocation required, ensure strong exception-safety

        basic_string cpy(get_allocator());
        cpy.privateAppendRaw(characterString, numChars);
        cpy.swap(*this);
        return *this;                                                 // RETURN
    }
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateAssign(
                                                           size_type numChars,
                                                           CHAR_TYPE character)
{
    BSLS_ASSERT_SAFE(numChars <= max_size());

    if (numChars <= capacity()) {
        // no reallocation required, perform assignment in-place

        this->d_length = 0;
        return privateAppendRaw(numChars, character);                 // RETURN
    }
    else {
        // reallocation required, ensure strong exception-safety

        basic_string cpy(get_allocator());
        cpy.privateAppendRaw(numChars, character);
        cpy.swap(*this);
        return *this;                                                 // RETURN
    }
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::Imp&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateBase()
{
    return *static_cast<Imp *>(this);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                                iterator begin,
                                                                iterator end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    privateInitDispatch((const_iterator)begin, (const_iterator)end);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInitDispatch(
                                                        const_iterator   begin,
                                                        const_iterator   end)
{
    BSLS_ASSERT_SAFE(begin <= end);

    size_type numChars = end - begin;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                          "string<...>(i,j): string too long");
    }
    privateAppendRaw(&*begin, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
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
                           BloombergLP::bslmf::Nil());  // disambiguate
                                                        // overloads
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                       const_iterator position,
                                                       const_iterator first,
                                                       const_iterator last)
{
    BSLS_ASSERT_SAFE(first <= last);

    size_type pos = position - cbegin();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pos > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                       "string<...>::insert<Iter>(pos,i,j): invalid position");
    }
    size_type numChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                        "string<...>::insert<Iter>(pos,i,j): string too long");
    }
    privateInsertRaw(pos, &*first, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertDispatch(
                                                       const_iterator position,
                                                       INPUT_ITER     first,
                                                       INPUT_ITER     last)
{
    size_type pos = position - cbegin();
    privateReplaceDispatch(pos,
                           size_type(0),
                           first,
                           last,
                           first,
                           BloombergLP::bslmf::Nil());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateInsertRaw(
                                              size_type        outPosition,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(outPosition <= length());
    BSLS_ASSERT_SAFE(numChars <= max_size() - length());
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    const difference_type displacement =
                          static_cast<difference_type>(numChars - outNumChars);

    size_type  newLength  = this->d_length + displacement;
    size_type  newStorage = this->d_capacity;
    CHAR_TYPE *newBuffer  = privateReserveRaw(&newStorage,
                                              newLength,
                                              outPosition);

    const CHAR_TYPE *tail    = this->dataPtr() + outPosition + outNumChars;
    size_type        tailLen = this->d_length  - outPosition - outNumChars;

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
            // Not entirely contained: break '[first .. last)' at 'tail', and
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
            // Entirely contained: copy 'tail' first, and copy
            // '[first .. last)' shifted by 'displacement'.

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceDispatch(
                              size_type                               position,
                              size_type                               numChars,
                              INPUT_ITER                              first,
                              INPUT_ITER                              last,
                              BloombergLP::bslmf::MatchArithmeticType ,
                              BloombergLP::bslmf::Nil                 )
{
    return replace(position, numChars, (size_type)first, (CHAR_TYPE)last);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplaceDispatch(
                                     size_type                        position,
                                     size_type                        numChars,
                                     INPUT_ITER                       first,
                                     INPUT_ITER                       last,
                                     BloombergLP::bslmf::MatchAnyType ,
                                     BloombergLP::bslmf::MatchAnyType )
{
    typename iterator_traits<INPUT_ITER>::iterator_category tag;
    return privateReplace(position, numChars, first, last, tag);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
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
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
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
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                      "string<...>::replace<Iter>(pos,i,j): invalid position");
    }
    size_type numChars = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                           max_size() - (length() - outPosition) < numChars)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplace(
                                                     size_type      position,
                                                     size_type      numChars,
                                                     const_iterator first,
                                                     const_iterator last,
                                                     std::forward_iterator_tag)
{
    BSLS_ASSERT_SAFE(first <= last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                      "string<...>::replace<Iter>(pos,i,j): invalid position");
    }
    size_type numNewChars = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                           max_size() - (length() - position) < numNewChars)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                     "string<...>::replace<Iter>(pos,n,i,j): string too long");
    }

    return privateReplaceRaw(position, numChars, &*first, numNewChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::privateReplace(
                                                     size_type      position,
                                                     size_type      numChars,
                                                     iterator       first,
                                                     iterator       last,
                                                     std::forward_iterator_tag)
{
    BSLS_ASSERT_SAFE(first <= last);

    return privateReplace(position,
                          numChars,
                          const_iterator(first),
                          const_iterator(last),
                          std::forward_iterator_tag());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                               const ALLOCATOR& basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    CHAR_TRAITS::assign(*begin(), CHAR_TYPE());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                                  const basic_string& original)
: Imp(original)
, BloombergLP::bslalg::ContainerBase<allocator_type>(ALLOCATOR())
{
    if (!this->isShortString()) {
        // Copy long string to either short or long.
        privateCopy(original);
    }
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                            const basic_string& original,
                                            const ALLOCATOR&    basicAllocator)
: Imp(original)
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    if (!this->isShortString()) {
        // Copy long string to either short or long.
        privateCopy(original);
    }
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                            const basic_string& original,
                                            size_type           position,
                                            size_type           numChars,
                                            const ALLOCATOR&    basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    assign(original, position, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                             const CHAR_TYPE  *characterString,
                                             const ALLOCATOR&  basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    BSLS_ASSERT_SAFE(characterString);

    assign(characterString);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                             const CHAR_TYPE  *characterString,
                                             size_type         numChars,
                                             const ALLOCATOR&  basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    assign(characterString, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                              size_type         numChars,
                                              CHAR_TYPE         character,
                                              const ALLOCATOR&  basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    assign(numChars, character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
                                               INPUT_ITER       first,
                                               INPUT_ITER       last,
                                               const ALLOCATOR& basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    privateInitDispatch(first, last);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class ALLOC2>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
  const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& original,
  const ALLOCATOR&                                              basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    this->assign(original.data(), original.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::basic_string(
           const BloombergLP::bslstl::StringRefData<CHAR_TYPE>& strRef,
           const ALLOCATOR&                                     basicAllocator)
: Imp()
, BloombergLP::bslalg::ContainerBase<allocator_type>(basicAllocator)
{
    assign(strRef.begin(), strRef.end());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::~basic_string()
{
    // perform a validity check
    BSLS_ASSERT_SAFE((*this)[this->d_length] == CHAR_TYPE());
    BSLS_ASSERT_SAFE(capacity() >= length());

    privateDeallocate();
    this->d_length = npos;  // invalid length
}

// MANIPULATORS

                // *** 21.3.2 construct/copy/destroy: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator=(
                                                       const basic_string& rhs)
{
    return assign(rhs, size_type(0), npos);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator=(const CHAR_TYPE *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return assign(rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator=(CHAR_TYPE character)
{
    return assign(1, character);
}

                      // *** 21.3.4 capacity: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::resize(size_type newLength,
                                                           CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newLength > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                  "string<...>::resize(n,c): string too long");
    }
    privateResizeRaw(newLength, character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::resize(size_type newLength)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newLength > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                    "string<...>::resize(n): string too long");
    }
    privateResizeRaw(newLength, CHAR_TYPE());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reserve(
                                                         size_type newCapacity)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newCapacity > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                   "string<...>::reserve(n): string too long");
    }
    privateReserveRaw(newCapacity);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::clear()
{
    // Note: Stlport and Dinkumware do not deallocate the allocated buffer in
    // long string representation, ApacheSTL does.

    privateClear(Imp::BASIC_STRING_DEALLOCATE_IN_CLEAR);
}

                      // *** 21.3.3 iterators: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::begin()
{
    return this->dataPtr();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::end()
{
    return begin() + this->d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rend()
{
    return reverse_iterator(begin());
}

                   // *** 21.3.5 element access: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator[](size_type position)
{
    BSLS_ASSERT_SAFE(position <= length());

    return *(begin() + position);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::at(size_type position)
{
    // Note: deliberately not inline, because 1) this is not a very widely used
    // function, and 2) it is very convenient to have at least one non-inlined
    // element accessor for debugging.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                       "string<...>::at(n): invalid position");
    }
    return *(begin() + position);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::front()
{
    BSLS_ASSERT_SAFE(!empty());

    return *begin();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::back()
{
    BSLS_ASSERT_SAFE(!empty());

    return *(begin() + length() - 1);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class ALLOC2>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::
    operator native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>() const
{
    native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2> result;
    result.assign(data(), length());
    return result;
}

                     // *** 21.3.6 modifiers: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(
                                                       const basic_string& rhs)
{
    return append(rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(
                                                          const CHAR_TYPE *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return append(rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(CHAR_TYPE character)
{
    push_back(character);
    return *this;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator+=(
                   const BloombergLP::bslstl::StringRefData<CHAR_TYPE>& strRef)
{
    return append(strRef.begin(),strRef.end());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                                   const basic_string&  suffix)
{
    return append(suffix, size_type(0), npos);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                                  const basic_string& suffix,
                                                  size_type           position,
                                                  size_type           numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > suffix.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                 "string<...>::append(string const&,pos,n): invalid position");
    }
    if (numChars > suffix.length() - position) {
        numChars = suffix.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                     "string<...>::append(string const&...): string too long");
    }
    return privateAppendRaw(suffix.data() + position, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                             "string<...>::append(char*...): string too long");
    }
    return privateAppendRaw(characterString, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    return append(characterString, CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(size_type numChars,
                                                      CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                     numChars > max_size() - this->d_length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                  "string<...>::append(n,c): string too long");
    }
    return privateResizeRaw(length() + numChars, character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::append(INPUT_ITER first,
                                                      INPUT_ITER last)
{
    return privateAppendDispatch(first, last);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::push_back(
                                                           CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() >= max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                              "string<...>::push_back(char): string too long");
    }
    if (length() + 1 > capacity()) {
        privateReserveRaw(length() + 1);
    }
    CHAR_TRAITS::assign(*(begin() + length()), character);
    ++this->d_length;
    CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                               const basic_string& replacement)
{
    return assign(replacement, size_type(0), npos);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                               const basic_string& replacement,
                                               size_type           position,
                                               size_type           numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                            position > replacement.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                 "string<...>::assign(string const&,pos,n): invalid position");
    }
    if (numChars > replacement.length() - position) {
        numChars = replacement.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                     "string<...>::assign(string const&...): string too long");
    }

    return privateAssign(replacement.data() + position, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    return assign(characterString, CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                             "string<...>::assign(char*...): string too long");
    }

    return privateAssign(characterString, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(
                   const BloombergLP::bslstl::StringRefData<CHAR_TYPE>& strRef)
{
    return assign(strRef.begin(), strRef.end());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(size_type numChars,
                                                      CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                  "string<...>::assign(n,c): string too long");
    }

    return privateAssign(numChars, character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
inline
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::assign(INPUT_ITER first,
                                                      INPUT_ITER last)
{
    basic_string(first, last, get_allocator()).swap(*this);
    return *this;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                                  size_type           position,
                                                  const basic_string& other)
{
    return insert(position, other, size_type(0), npos);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                              size_type            outPosition,
                                              const basic_string&  other,
                                              size_type            position,
                                              size_type            numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())
     || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > other.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                "string<...>::insert(pos,string const&...): invalid position");
    }
    if (numChars > other.length() - position) {
        numChars = other.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                     "string<...>::insert(string const&...): string too long");
    }
    return privateInsertRaw(outPosition, other.data() + position, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(
                                              size_type        position,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                        "string<...>::insert(pos,char*...): invalid position");
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                             "string<...>::insert(char*...): string too long");
    }
    return privateInsertRaw(position, characterString, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::insert(size_type position,
                                                      size_type numChars,
                                                      CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                             "string<...>::insert(pos,n,c): invalid position");
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                           numChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                              "string<...>::insert(pos,n,v): string too long");
    }
    return privateReplaceRaw(position, size_type(0), numChars, character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

#if defined(BSLS_PLATFORM_CMP_SUN)
    // Sun CC compiler doesn't like that 'iterator' return type of 'insert'
    // method with an additional 'INPUT_ITER' template parameter depends on
    // template parameters of the primary template class 'basic_string'.
    // However, it happily accepts 'CHAR_TYPE *', which is how 'iterator' is
    // currently defined.  It will also accept an inline definition of this
    // method (this workaround should be used when 'iterator' becomes a real
    // class and the current workaround stops working).
#   define BSLSTL_INSERT_RETURN_TYPE CHAR_TYPE *
#else
#   define BSLSTL_INSERT_RETURN_TYPE \
    typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
#endif

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(size_type position,
                                                     size_type numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position >= cbegin());
    BSLS_ASSERT_SAFE(position < cend());

    const_iterator postPosition = position;
    iterator       dstPosition  = begin() + (position - cbegin());

    ++postPosition;
    CHAR_TRAITS::move(&*dstPosition, &*postPosition, cend() - postPosition);

    --this->d_length;
    CHAR_TRAITS::assign(*(this->dataPtr() + length()), CHAR_TYPE());

    return dstPosition;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(!empty());

    --this->d_length;
    CHAR_TRAITS::assign(*(begin() + length()), CHAR_TYPE());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                               size_type           outPosition,
                                               size_type           outNumChars,
                                               const basic_string& replacement)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < outPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
               "string<...>::replace(pos,string const&...): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                 replacement.length()               > outNumChars
              && replacement.length() - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "string<...>::replace(pos,string const&...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             replacement.data(),
                             replacement.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                               size_type           outPosition,
                                               size_type           outNumChars,
                                               const basic_string& replacement,
                                               size_type           position,
                                               size_type           numChars)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
               "string<...>::replace(pos,string const&...): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                            position > replacement.length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
               "string<...>::replace(pos,string const&...): invalid position");
    }
    if (numChars > replacement.length() - position) {
        numChars = replacement.length() - position;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                    "string<...>::replace(string const&...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             replacement.data() + position,
                             numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                              size_type        outPosition,
                                              size_type        outNumChars,
                                              const CHAR_TYPE *characterString,
                                              size_type        numChars)
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                       "string<...>::replace(pos,char*...): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                            "string<...>::replace(char*...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             characterString,
                             numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(size_type outPosition,
                                                       size_type outNumChars,
                                                       size_type numChars,
                                                       CHAR_TYPE character)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(outPosition > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                            "string<...>::replace(pos,n,c): invalid position");
    }
    if (outNumChars > length() - outPosition) {
        outNumChars = length() - outPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                             "string<...>::replace(pos,n,v): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             numChars,
                             character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::replace(
                                               const_iterator      first,
                                               const_iterator      last,
                                               const basic_string& replacement)
{
    BSLS_ASSERT_SAFE(first >= cbegin());
    BSLS_ASSERT_SAFE(first <= cend());
    BSLS_ASSERT_SAFE(first <= last);
    BSLS_ASSERT_SAFE(last <= cend());

    size_type outPosition = first - cbegin();
    size_type outNumChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                            replacement.length() > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                 replacement.length() - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                    "string<...>::replace(string const&...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             replacement.data(),
                             replacement.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    size_type outPosition = first - cbegin();
    size_type outNumChars = last - first;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(numChars > outNumChars)
     && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                             numChars - outNumChars > max_size() - length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                            "string<...>::replace(char*...): string too long");
    }
    return privateReplaceRaw(outPosition,
                             outNumChars,
                             characterString,
                             numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                            "string<...>::replace(char*...): string too long");
    }
    return privateReplaceRaw(outPosition, outNumChars, numChars, character);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
template <class INPUT_ITER>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::begin() const
{
    return this->dataPtr();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::cbegin() const
{
    return begin();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::end() const
{
    return begin() + this->d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::cend() const
{
    return end();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reverse_iterator
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::crend() const
{
    return const_reverse_iterator(begin());
}

                      // *** 21.3.4 capacity: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size() const
{
    return this->d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::length() const
{
    return this->d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::max_size() const
{
    // Must take into account the null-terminating character.

    size_type stringMaxSize = ~size_type(0) / sizeof(CHAR_TYPE) - 1;
    size_type allocMaxSize  = get_allocator().max_size() - 1;
    return allocMaxSize < stringMaxSize ? allocMaxSize : stringMaxSize;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::capacity() const
{
    return this->d_capacity;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
bool basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::empty() const
{
    return this->d_length == 0;
}

                   // *** 21.3.5 element access: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::operator[](
                                                      size_type position) const
{
    BSLS_ASSERT_SAFE(position <= length());

    return *(begin() + position);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::at(size_type position) const
{
    // Note: deliberately not inlined (see comment in non-const version).

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                 "const string<...>::at(n): invalid position");
    }
    return *(begin() + position);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *begin();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_reference
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *(end() - 1);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::copy(CHAR_TYPE *characterString,
                                                    size_type  numChars,
                                                    size_type  position) const
{
    BSLS_ASSERT_SAFE(characterString);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                       "const string<...>::copy(str,pos,n): invalid position");
    }
    if (numChars > length() - position) {
        numChars = length() - position;
    }
    CHAR_TRAITS::move(characterString, this->dataPtr() + position, numChars);
    return numChars;
}

                 // *** 21.3.7 string operations: ***

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_pointer
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::c_str() const
{
    return this->dataPtr();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::const_pointer
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::data() const
{
    return this->dataPtr();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::allocator_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::get_allocator() const
{
    return BloombergLP::bslalg::ContainerBase<allocator_type>::allocator();
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(
                                           const basic_string&  substring,
                                           size_type            position) const
{
    return find(substring.data(), position, substring.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(
                                               const CHAR_TYPE *substring,
                                               size_type        position,
                                               size_type        numChars) const
{
    BSLS_ASSERT_SAFE(substring);

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
                                                     *substring));
         remChars -= ++nextString - thisString, thisString = nextString)
    {
        if (0 == CHAR_TRAITS::compare(nextString, substring, numChars)) {
            return nextString - this->dataPtr();                      // RETURN
        }
    }
    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find(
                                               const CHAR_TYPE *substring,
                                               size_type        position) const
{
    BSLS_ASSERT_SAFE(substring);

    return find(substring, position, CHAR_TRAITS::length(substring));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(
                                            const basic_string& substring,
                                            size_type           position) const
{
    return rfind(substring.data(), position, substring.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::rfind(CHAR_TYPE character,
                                                     size_type position) const
{
    return rfind(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                           const basic_string& characterString,
                                           size_type           position) const
{
    return find_first_of(characterString.data(),
                         position,
                         characterString.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_of(
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_first_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                           const basic_string& characterString,
                                           size_type           position) const
{
    return find_last_of(characterString.data(),
                        position,
                        characterString.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_of(
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_last_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                           const basic_string& characterString,
                                           size_type           position) const
{
    return find_first_not_of(characterString.data(),
                             position,
                             characterString.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_first_not_of(
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_first_not_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                           const basic_string& characterString,
                                           size_type           position) const
{
    return find_last_not_of(characterString.data(),
                            position,
                            characterString.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::size_type
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::find_last_not_of (
                                                      CHAR_TYPE character,
                                                      size_type position) const
{
    return find_last_not_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::substr(size_type position,
                                                      size_type numChars) const
{
    return basic_string<CHAR_TYPE,
                        CHAR_TRAITS,
                        ALLOCATOR>(*this, position, numChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                               const basic_string& other) const
{
    return privateCompareRaw(size_type(0),
                             length(),
                             other.data(),
                             other.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                              size_type            position,
                                              size_type            numChars,
                                              const basic_string&  other) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                    "const string<...>::compare(pos,n,...): invalid position");
    }
    if (numChars > length() - position) {
        numChars = length() - position;
    }
    return privateCompareRaw(position, numChars, other.data(), other.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                       size_type           lhsPosition,
                                       size_type           lhsNumChars,
                                       const basic_string& other,
                                       size_type           otherPosition,
                                       size_type           otherNumChars) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                    "const string<...>::compare(pos,n,...): invalid position");
    }
    if (lhsNumChars > length() - lhsPosition) {
        lhsNumChars = length() - lhsPosition;
    }
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(other.length() <
                                                              otherPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                                  const CHAR_TYPE *other) const
{
    BSLS_ASSERT_SAFE(other);

    return privateCompareRaw(size_type(0),
                             length(),
                             other,
                             CHAR_TRAITS::length(other));
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
int basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>::compare(
                                          size_type        lhsPosition,
                                          size_type        lhsNumChars,
                                          const CHAR_TYPE *other,
                                          size_type        otherNumChars) const
{
    BSLS_ASSERT_SAFE(other);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
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

}  // close namespace bsl

// FREE FUNCTIONS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void bsl::swap(basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& lhs,
               basic_string<CHAR_TYPE,CHAR_TRAITS, ALLOCATOR>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.get_allocator() == rhs.get_allocator());

    lhs.swap(rhs);
}

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator==(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator==(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator==(const CHAR_TYPE                                  *lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    std::size_t len = CHAR_TRAITS::length(lhs);
    return len == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs, rhs.data(), len);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator==(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                     const CHAR_TYPE*                                 rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    std::size_t len = CHAR_TRAITS::length(rhs);
    return lhs.size() == len
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs, len);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator!=(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator!=(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator!=(const CHAR_TYPE                                  *lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator!=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                     const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return !(lhs == rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
bool bsl::operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
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
bsl::operator<(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
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
bsl::operator<(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
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
bool bsl::operator<(const CHAR_TYPE                                  *lhs,
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
bool bsl::operator<(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
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
bool bsl::operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                    const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator>(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator>(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator>(const CHAR_TYPE                                  *lhs,
                    const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator>(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                    const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return rhs < lhs;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator<=(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator<=(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator<=(const CHAR_TYPE                                  *lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator<=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                     const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return !(rhs < lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>& rhs)
{
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator>=(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>&        rhs)
{
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC1, class ALLOC2>
inline
bool
bsl::operator>=(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator>=(const CHAR_TYPE                                  *lhs,
                     const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  rhs)
{
    BSLS_ASSERT_SAFE(lhs);

    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
inline
bool bsl::operator>=(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC>&  lhs,
                     const CHAR_TYPE                                  *rhs)
{
    BSLS_ASSERT_SAFE(rhs);

    return !(lhs < rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
bsl::operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& lhs,
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
bsl::operator+(
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>& lhs,
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
bsl::operator+(
             const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1>&        lhs,
             const native_std::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC2>& rhs)
{
    bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOC1> result;
    result.reserve(lhs.length() + rhs.length());
    result += lhs;
    result.append(rhs.c_str(), rhs.length());
    return result;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
bsl::operator+(const CHAR_TYPE                                      *lhs,
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
bsl::operator+(CHAR_TYPE                                            lhs,
               const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& rhs)
{
    basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR> result;
    result.reserve(1 + rhs.length());
    result.push_back(lhs);
    result += rhs;
    return result;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
bsl::operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  lhs,
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

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
bsl::operator+(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& lhs,
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
                        std::size_t                                   n)
    // Do not use, for internal use by 'operator<<' only.
{
    BSLS_ASSERT_SAFE(buf);

    CHAR_TYPE fillChar = os.fill();

    for (std::size_t i = 0; i < n; ++i) {
        if (CHAR_TRAITS::eq_int_type(buf->sputc(fillChar), CHAR_TRAITS::eof()))
        {
            return false;                                             // RETURN
        }
    }

    return true;
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&
bsl::operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>&          os,
                const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str)
{
    typedef std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> Ostrm;
    typename Ostrm::sentry sentry(os);
    bool ok = false;

    if (sentry) {
        ok = true;
        std::size_t n = str.size();
        std::size_t padLen = 0;
        bool left = (os.flags() & Ostrm::left) != 0;
        std::streamsize w = os.width(0);
        std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>* buf = os.rdbuf();

        if (w > 0 && std::size_t(w) > n) {
            padLen = std::size_t(w) - n;
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
bsl::operator>>(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&     is,
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
        std::streamsize n = is.width(0);
        if (n <= 0) {
            n = std::numeric_limits<std::streamsize>::max();
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
bsl::getline(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&    is,
             basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str,
             CHAR_TYPE                                      delim)
{
    typedef std::basic_istream<CHAR_TYPE, CHAR_TRAITS> Istrm;
    std::size_t nread = 0;
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
bsl::getline(std::basic_istream<CHAR_TYPE, CHAR_TRAITS>&    is,
             basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>& str)
{
    return getline(is, str, is.widen('\n'));
}

// HASH SPECIALIZATIONS
template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
inline
void bsl::hashAppend(
                HASHALG&                                               hashAlg,
                const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& input)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAlg(input.data(), sizeof(CHAR_TYPE)*input.size());
    hashAppend(hashAlg, input.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
std::size_t bsl::hashBasicString(
                    const basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str)
{
    return ::BloombergLP::bslh::Hash<>()(str);
}

namespace BloombergLP {

template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void bslh::hashAppend(
    HASHALG&                                                           hashAlg,
    const native_std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& input)
{
    hashAlg(input.data(), sizeof(CHAR_TYPE)*input.size());
    hashAppend(hashAlg, input.size());
}

}  // close enterprise namespace
// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *sequence* containers:
//: o A sequence container defines STL iterators.
//: o A sequence container is bitwise moveable if the allocator is bitwise
//:     moveable.
//: o A sequence container uses 'bslma' allocators if the parameterized
//:     'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
struct HasStlIterators<bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOC> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslmf {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
struct IsBitwiseMoveable<bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOC> >
    : IsBitwiseMoveable<ALLOC>
{};

}  // close namespace bslmf

namespace bslma {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOC>
struct UsesBslmaAllocator<bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOC> >
    : bsl::is_convertible<Allocator *, ALLOC>
{};

}  // close namespace bslma

}  // close enterprise namespace

#undef BSLSTL_CHAR_TRAITS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
extern template class bsl::String_Imp<char, bsl::string::size_type>;
extern template class bsl::String_Imp<wchar_t, bsl::wstring::size_type>;
extern template class bsl::basic_string<char>;
extern template class bsl::basic_string<wchar_t>;
#endif

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
