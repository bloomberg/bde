// bslstl_pair.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_PAIR
#define INCLUDED_BSLSTL_PAIR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple 'struct' with two members that may use allocators.
//
//@CLASSES:
//  bsl::pair: pair of values, each of which may use a 'bslma::Allocator'
//
//@DESCRIPTION: *This class is for internal use only.  Do not use.  Use
// '<bsl_utility.h>' and 'bsl::pair' directly instead.*
//
// The 'bsl::pair' class template is instantiated on two types, 'T1' and 'T2',
// and provides two public data members, 'first' and 'second' of type 'T1' and
// 'T2', respectively.  Each data member might or might not allocate memory
// using 'bslma::Allocator'.  Its interface is identical to 'std::pair' except
// that it has constructors that take optional allocator arguments to correctly
// construct the member variables.  For example, a
// 'bsl::pair<std::string, int>' has member 'first' of type 'std::string' and
// 'second' of type 'int'.  A client can pass a 'bslma::Allocator' pointer to
// the pair constructor and the constructor will pass it through to the 'first'
// member.  Similarly, the copy constructor takes an optional
// 'bslma::Allocator' pointer and copy-constructs the 'first' member using that
// allocator.
//
// 'bsl::pair' is unusual in that its data members, 'first' and 'second', are
// public.  Once constructed, a client program accesses these members directly.
// This part of the interface is identical to 'std::pair', for which
// 'bsl::pair' is intended to be a drop-in replacement.
//
// 'bsl::pair' has four constructors: a default constructor which
// default-constructs the two data members, a copy constructor which
// copy-constructs each data member, a constructor taking two arguments of type
// 'T1' and 'T2', which are used to copy-construct 'first' and 'second'
// respectively, and a conversion constructor template for converting from a
// 'bsl::pair' of different types, 'U1' and 'U2', provided 'U1' is convertible
// to 'T1' and 'U2' is convertible to 'T2'.  If and only if 'T1' and/or 'T2' is
// a type that uses 'bslma::Allocator' for memory allocation, then each
// constructor also has an optional 'bslma::Allocator' pointer argument.
// Whether or not a type uses 'bslma::Allocator' is determined by querying the
// 'bslma::UsesBslmaAllocator' trait for that type.  This component also
// defines a full set of equality and relational operators which can be
// instantiated if 'T1' and 'T2' both provide those operators.
//
// A 'bsl::pair' declares a set of associated type traits which are computed
// from the type traits of 'T1' and 'T2'.  For each supported type trait, a
// given specialization of 'bsl::pair' has that trait if and only if *both*
// 'T1' and 'T2' have that trait.  Supported traits are:
//..
//  bslmf::IsBitwiseMoveable
//  bslmf::IsBitwiseEqualityComparable
//  bsl::is_trivially_copyable
//  bsl::is_trivially_default_constructible
//..
// In addition, a 'bsl::pair' specialization has the
// 'bslma::UsesBslmaAllocator' trait if *either* 'T1' or 'T2' have that trait,
// or both.
//
///Usage
///-----
// *This class is for internal use only, do not use.* This usage example is
// here to illustrate a typical usage of this class for BDE only.
//
// A 'bsl::pair' is a very simple object when used without allocators.  Our
// usage example concentrates on the use of allocators with 'bsl::pair'.
// First, we create a utility function that copies a null-terminated string
// into memory allocated from a supplied allocator:
//..
//  char *myStrDup(const char *s, bslma::Allocator *basicAllocator)
//      // Copy the specified null-terminated string 's' into memory allocated
//      // from 'basicAllocator'
//  {
//      char *result = (char*) basicAllocator->allocate(std::strlen(s) + 1);
//      return std::strcpy(result, s);
//  }
//..
// We create a simple string class that holds strings allocated from a supplied
// allocator.  It uses 'myStrDup' (above) in its implementation:
//..
//  class my_String {
//      // Simple string class that uses a 'bslma::Allocator' allocator.
//
//      bslma::Allocator *d_allocator_p;
//      char             *d_data;
//
//    public:
//      BSLMF_NESTED_TRAIT_DECLARATION(my_String, bslma::UsesBslmaAllocator);
//
//      explicit my_String(bslma::Allocator *basicAllocator = 0);
//          // Construct an empty string using the optionally-specified
//          // allocator 'basicAllocator'.
//
//      my_String(const char* s, bslma::Allocator *basicAllocator = 0);
//          // Construct a string with contents specified in 's' using the
//          // optionally-specified allocator 'basicAllocator'.
//
//      my_String(const my_String& rhs, bslma::Allocator *basicAllocator = 0);
//          // Construct a copy of the specified 'rhs' string using the
//          // optionally-specified allocator 'basicAllocator'.
//
//      ~my_String();
//          // Destroy this string.
//
//      my_String& operator=(const my_String& rhs);
//          // Copy specified 'rhs' string value to this string.
//
//      const char* c_str() const;
//          // Return the null-terminated character array for this string.
//
//      bslma::Allocator *allocator() const;
//          // Return the allocator used to construct this string or, if no
//          // allocator was specified at construction, the default allocator
//          // at the time of construction.
//  };
//
//  bool operator==(const my_String& str1, const my_String& str2)
//  {
//      return 0 == std::strcmp(str1.c_str(), str2.c_str());
//  }
//
//  bool operator==(const my_String& str, const char *p)
//  {
//      return 0 == std::strcmp(p, str.c_str());
//  }
//
//  bool operator==(const char *p, const my_String& str)
//  {
//      return str == p;
//  }
//
//  bool operator!=(const my_String& str1, const my_String& str2)
//  {
//      return ! (str1 == str2);
//  }
//
//  bool operator!=(const my_String& str, const char *p)
//  {
//      return ! (str == p);
//  }
//
//  bool operator!=(const char *p, const my_String& str)
//  {
//      return ! (str == p);
//  }
//
//  bool operator<(const my_String& str1, const my_String& str2)
//  {
//      return std::strcmp(str1.c_str(), str2.c_str()) < 0;
//  }
//
//  my_String::my_String(bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator)), d_data(0)
//  {
//      d_data = myStrDup("", d_allocator_p);
//  }
//
//  my_String::my_String(const char *s, bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator)), d_data(0)
//  {
//      d_data = myStrDup(s, d_allocator_p);
//  }
//
//  my_String::my_String(const my_String&  rhs,
//                       bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator)), d_data(0)
//  {
//      d_data = myStrDup(rhs.d_data, d_allocator_p);
//  }
//
//  my_String::~my_String()
//  {
//      d_allocator_p->deallocate(d_data);
//  }
//
//  my_String& my_String::operator=(const my_String& rhs)
//  {
//      if (this != &rhs) {
//          d_allocator_p->deallocate(d_data);
//          d_data = myStrDup(rhs.d_data, d_allocator_p);
//      }
//      return *this;
//  }
//
//  const char *my_String::c_str() const
//  {
//      return d_data;
//  }
//
//  bslma::Allocator *my_String::allocator() const
//  {
//      return d_allocator_p;
//  }
//..
// Our main program creates a mapping from strings to integers.  Each node of
// the mapping consists of a 'bsl::pair<my_String, int>'.  The program
// allocates memory from a test allocator in order to ensure that there are no
// leaks:
//..
//  int main()
//  {
//      typedef bsl::pair<my_String, int> Node;
//
//      Node *mapping[3];
//      bslma::TestAllocator alloc;
//..
// When constructing a 'Node', an allocator is supplied in addition to
// parameters for the 'first' and 'second' data members.
//..
//  {
//      mapping[0] = new(basicAllocator) Node("One", 1, &basicAllocator);
//      mapping[1] = new(basicAllocator) Node("Three", 3, &basicAllocator);
//      mapping[2] = new(basicAllocator) Node("Two", 2, &basicAllocator);
//      // Temporaries get destroyed here, even on broken compilers.
//  }
//
//  assert("One" == mapping[0]->first);
//  assert(1 == mapping[0]->second);
//  assert("Three" == mapping[1]->first);
//  assert(3 == mapping[1]->second);
//  assert("Two" == mapping[2]->first);
//  assert(2 == mapping[2]->second);
//
//  assert(6 == alloc.numBlocksInUse());
//..
// Clean up at end.
//..
//      alloc.deleteObjectRaw(mapping[0]);
//      alloc.deleteObjectRaw(mapping[1]);
//      alloc.deleteObjectRaw(mapping[2]);
//
//      assert(0 == alloc.numBlocksInUse());
//
//      return 0;
//  }
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_utility.h> instead of <bslstl_pair.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif


#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISPAIR
#include <bslmf_ispair.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
#endif

#ifndef INCLUDED_UTILITY
#include <utility> // 'std::pair' and (in C++11 mode) 'std::swap'
#define INCLUDED_UTILITY
#endif

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && !defined(BSLS_PLATFORM_CMP_CLANG)

// If the compiler supports rvalue references, then we have C++11 'std::swap',
// which has a complicated SFINAE clause.  Fortunately, it is defined in
// <utility>, which is included.
//
// However, if the compiler does not support rvalue references, then we have
// C++03 'std::swap', which has a trivial signature.  We forward-declare it
// here because otherwise we'd have to '#include <algorithm>', which causes
// difficult-to-fix cyclic dependencies between the native library and bsl.
#ifdef std
#   error This header should not be #included with 'std' being a macro
#endif
namespace std {
template <class TYPE> void swap(TYPE& a, TYPE& b);
}

#endif // ! BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES  && ! CLANG

namespace bsl {

                        // ===============
                        // struct Pair_Imp
                        // ===============

template <class T1, class T2, int T1_USES_BSLMA, int T2_USES_BSLMA>
struct Pair_Imp {
    // Private class to implement Pair constructor.  This general template is
    // for types that don't use allocators.  It is partially specialized for
    // types that do take allocators.

  private:
    // NOT IMPLEMENTED
    Pair_Imp(const Pair_Imp&);
    Pair_Imp& operator=(const Pair_Imp&);

  public:
    // PUBLIC DATA
    T1 first;
    T2 second;

    // CREATORS
    Pair_Imp();
        // Construct a 'Pair_Imp' object from using default constructor for
        // 'first' and 'second'.

    Pair_Imp(const T1& a, const T2& b);
    template <class U1, class U2>
    Pair_Imp(const U1& a, const U2& b);
        // Construct a 'Pair_Imp' object from specified values 'a' and 'b'.

    ~Pair_Imp();
        // Destroy this object.
};

template <class T1, class T2>
struct Pair_Imp<T1, T2, 1, 0> {
    // Private class to implement Pair constructor.  This partial
    // specialization is for 'T1' that uses allocators.

  private:
    // NOT IMPLEMENTED
    Pair_Imp(const Pair_Imp&);
    Pair_Imp& operator=(const Pair_Imp&);

  public:
    // PUBLIC DATA
    T1 first;
    T2 second;

    // CREATORS
    explicit Pair_Imp(BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Construct a 'Pair_Imp' object.  Optionally specify a
        // 'basicAllocator' used to supply memory for 'first'.  Note that this
        // specialization of 'Pair_Imp' guarantees that 'second' does not need
        // an allocator.

    Pair_Imp(const T1& a, const T2& b,
             BloombergLP::bslma::Allocator *basicAllocator = 0);
    template <class U1, class U2>
    Pair_Imp(const U1& a, const U2& b,
             BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Construct a 'Pair_Imp' object from specified values 'a' and 'b'.
        // Optionally specify a 'basicAllocator' used to supply memory for
        // 'first'.  Note that this specialization of 'Pair_Imp' guarantees
        // that 'second' does not need an allocator.

    ~Pair_Imp();
        // Destroy this object.
};

template <class T1, class T2>
struct Pair_Imp<T1, T2, 0, 1> {
    // Private class to implement Pair constructor.  This partial
    // specialization is for 'T2' that uses allocators.

  private:
    // NOT IMPLEMENTED
    Pair_Imp(const Pair_Imp&);
    Pair_Imp& operator=(const Pair_Imp&);

  public:
    // PUBLIC DATA
    T1 first;
    T2 second;

    // CREATORS
    explicit Pair_Imp(BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Construct a 'Pair_Imp' object.  Optionally specify a
        // 'basicAllocator' used to supply memory for 'second'.  Note that this
        // specialization of 'Pair_Imp' guarantees that 'first' does not need
        // an allocator.

    Pair_Imp(const T1& a, const T2& b,
             BloombergLP::bslma::Allocator *basicAllocator = 0);
    template <class U1, class U2>
    Pair_Imp(const U1& a, const U2& b,
             BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Construct a 'Pair_Imp' object from specified values 'a' and 'b'.
        // Optionally specify a 'basicAllocator' used to supply memory for
        // 'second'.  Note that this specialization of 'Pair_Imp' guarantees
        // that 'second' does not need an allocator.

    ~Pair_Imp();
        // Destroy this object.
};

template <class T1, class T2>
struct Pair_Imp<T1, T2, 1, 1> {
    // Private class to implement Pair constructor.  This partial
    // specialization is for 'T1' and 'T2' that uses allocators.

  private:
    // NOT IMPLEMENTED
    Pair_Imp(const Pair_Imp&);
    Pair_Imp& operator=(const Pair_Imp&);

  public:
    // PUBLIC DATA
    T1 first;
    T2 second;

    // CREATORS
    explicit Pair_Imp(BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Construct a 'Pair_Imp' object.  Optionally specify a
        // 'basicAllocator' used to supply memory for both 'first' and
        // 'second'.

    Pair_Imp(const T1& a, const T2& b,
             BloombergLP::bslma::Allocator *basicAllocator = 0);
    template <class U1, class U2>
    Pair_Imp(const U1& a, const U2& b,
             BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Construct a 'Pair_Imp' object from specified values 'a' and 'b'.
        // Optionally specify a 'basicAllocator' used to supply memory for both
        // 'first' and 'second'.

    ~Pair_Imp();
        // Destroy this object.
};

                        // ==========
                        // class pair
                        // ==========

template <class T1, class T2>
class pair : public Pair_Imp<T1, T2,
                             BloombergLP::bslma::UsesBslmaAllocator<T1>::value,
                             BloombergLP::bslma::UsesBslmaAllocator<T2>::value>
{
    // Provide a pair of public data members, 'first' and 'second', of type
    // 'T1' and 'T2' respectively.  If either 'T1' or 'T2' uses
    // 'bslma::Allocator' for memory management, then provide an optional
    // 'bslma::Allocator' argument for each constructor, to be passed through
    // to the constructors of 'first' and/or 'second' as appropriate.  The
    // interface to this class is identical to the standard 'std::pair' except
    // for the addition of the allocators.

    // PRIVATE TYPES
    typedef Pair_Imp<T1, T2,
                     BloombergLP::bslma::UsesBslmaAllocator<T1>::value,
                     BloombergLP::bslma::UsesBslmaAllocator<T2>::value> Base;

  public:
    // PUBLIC TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // PUBLIC DATA
    using Base::first;
    using Base::second;

    // CREATORS
    pair();
    explicit pair(BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a pair.  Default construct for 'first' and 'second'.
        // Optionally specify a 'basicAllocator' used to supply memory for the
        // constructor(s) of which ever data member(s) accept an allocator.
        // Attempted use of this constructor will not compile unless 'T1' and
        // 'T2' both supply default constructors.  Attempted use of either
        // version of the allocator version will not compile unless one or both
        // of 'T1' and 'T2' accept an allocator.

    pair(const T1& a, const T2& b);
    pair(const T1& a, const T2& b,
         BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a pair from the specified values 'a' and 'b'.
        // Copy-construct 'first' from 'a' and 'second' from 'b'.  Optionally
        // specify a 'basicAllocator' used to supply memory for the
        // constructor(s) of which ever data member(s) accept an allocator.
        // Attempted use of of either version of this constructor will not
        // compile unless 'T1' and 'T2' both supply copy constructors.
        // Attempted use of the allocator version will not compile unless one
        // or both of 'T1' and 'T2' accept an allocator.

    pair(const pair& original);
    pair(const pair& original, BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a pair from the specified 'original' value.
        // Copy-construct 'first' from 'rhs.first' and 'second' from
        // 'rhs.second'.  Optionally specify a 'basicAllocator' used to supply
        // memory for the constructor(s) of which ever data member(s) accept an
        // allocator.  Attempted use of of either version of this constructor
        // will not compile unless 'T1' and 'T2' both supply copy constructors.
        // Attempted use of the allocator version will not compile unless one
        // or both of 'T1' and 'T2' accept an allocator.

    template <class U1, class U2>
    pair(const pair<U1, U2>& rhs);
    template <class U1, class U2>
    pair(const pair<U1, U2>&           rhs,
         BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a pair from the specified 'rhs' value.  Convert-construct
        // 'first' from 'rhs.first' and 'second' from 'rhs.second'.  Optionally
        // specify a 'basicAllocator' used to supply memory for the
        // constructor(s) of which ever data member(s) accept an allocator.
        // Attempted use of of either version of this constructor will not
        // compile unless 'T1' is constructible from 'U1' and 'T2' is
        // constructible from 'U2'.  Attempted use of the allocator version
        // will not compile unless one or both of 'T1' and 'T2' accept an
        // allocator.

    template <class U1, class U2>
    pair(const native_std::pair<U1, U2>&  rhs);                     // IMPLICIT
    template <class U1, class U2>
    pair(const native_std::pair<U1, U2>&  rhs,
         BloombergLP::bslma::Allocator   *basicAllocator);
        // Create a pair that has the same value as the specified 'rhs' pair,
        // where the type 'rhs' is the pair type native to the compiler's
        // library, holding the parameterized types 'U1' and 'U2'.  Uses the
        // default allocator to provide memory.  Optionally specify a
        // 'basicAllocator' used to supply memory for the constructor(s) of
        // which ever data member(s) accept an allocator.  The behavior is
        // undefined unless 'T1' is constructible from 'U1' and 'T2' is
        // constructible from from 'U2'.


    ~pair();
        // Destroy this object.  Call destructors on 'first' and 'second'.

    // MANIPULATORS
    pair& operator=(const pair& rhs);
        // Assign to this pair from the specified 'rhs' value and return a
        // modifiable reference to this object.  Assign 'first' the value in
        // 'rhs.first' and 'second' the value in 'rhs.second'.  Attempted use
        // of this assignment operator will not compile unless both 'T1' and
        // 'T2' supply assignment operators.

    template <class U1, class U2>
    pair& operator=(const native_std::pair<U1, U2>& rhs);
        // Assign to this pair from the specified 'rhs' pair, where the type
        // 'rhs' is the pair type native to the compiler's library, holding the
        // parameterized types 'U1' and 'U2', and return a modifiable reference
        // to this object.  Assign 'first' the value in 'rhs.first' and
        // 'second' the value in 'rhs.second'.  Attempted use of this
        // assignment operator will not compile unless both 'T1' and 'T2'
        // supply assignment operators, and 'T1' is assignable from 'U1' and
        // 'T2' is assignable from 'U2'.

    void swap(pair& other);
        // Swap the value of this pair with the value of the specified 'other'
        // pair by applying 'swap' to each of the 'first' and 'second' pair
        // fields.  Note that this method is no-throw only if 'swap' on each
        // field is no-throw.
};

}  // close namespace bsl

namespace bsl {

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' and 'rhs' pair objects have the same
    // value and false otherwise.  'lhs' has the same value as 'rhs' if
    // 'lhs.first == rhs.first' and 'lhs.second == rhs.second'.  A call to this
    // operator will not compile unless both 'T1' and 'T2' supply 'operator=='.

template <class T1, class T2>
inline
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' and 'rhs' pair objects do not have
    // the same value and false otherwise.  'lhs' does not have the same value
    // as 'rhs' if 'lhs == rhs' would return false.  A call to this operator
    // will not compile unless a call to 'lhs == rhs' would compile.

template <class T1, class T2>
inline
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value less than the specified
    // 'rhs' and false otherwise.  Whether or not 'lhs' is less than 'rhs' is
    // determined by a lexicographical comparison of the 'first' and 'second'
    // data members of 'lhs' and 'rhs'.  In other words: return true if
    // 'lhs.first < rhs.first' and false if 'rhs.first < lhs.first', otherwise
    // return 'lhs.second < rhs.second'.  A call to this operator will not
    // compile unless both 'T1' and 'T2' supply 'operator<'.

template <class T1, class T2>
inline
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value greater than the
    // specified 'rhs' and false otherwise.  'lhs' has a value greater than
    // 'rhs' if 'rhs' < 'lhs' would return true.  A call to this operator will
    // not compile unless a call to 'lhs < rhs' would compile.

template <class T1, class T2>
inline
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value less than or equal to the
    // specified 'rhs' and false otherwise.  'lhs' has a value less than or
    // equal to 'rhs' if 'rhs' < 'lhs' would return false.  A call to this
    // operator will not compile unless a call to 'lhs < rhs' would compile.

template <class T1, class T2>
inline
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value greater than or equal to
    // the specified 'rhs' and false otherwise.  'lhs' has a value greater than
    // or equal to 'rhs' if 'lhs' < 'rhs' would return false.  A call to this
    // operator will not compile unless a call to 'lhs < rhs' would compile.

// FREE FUNCTIONS
template <class T1, class T2>
void swap(pair<T1, T2>& a, pair<T1, T2>& b);
    // Swap the values of the specified 'a' and 'b' pairs by applying 'swap' to
    // each of the 'first' and 'second' pair fields.  Note that this method is
    // no-throw only if 'swap' on each field is no-throw.

// HASH SPECIALIZATIONS
template <class HASHALG, class T1, class T2>
void hashAppend(HASHALG& hashAlg, const pair<T1, T2>&  input);
    // Pass the specified 'input' to the specified 'hashAlg'

}  // close namespace bsl

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// See IMPLEMENTATION NOTES in the .cpp before modifying anything below.

namespace bsl {

                           // ---------------
                           // struct Pair_Imp
                           // ---------------

// CREATORS
template <class T1, class T2, int T1_USES_BSLMA, int T2_USES_BSLMA>
inline
Pair_Imp<T1, T2, T1_USES_BSLMA, T2_USES_BSLMA>::
Pair_Imp()
: first(), second()
{
}

template <class T1, class T2, int T1_USES_BSLMA, int T2_USES_BSLMA>
inline
Pair_Imp<T1, T2, T1_USES_BSLMA, T2_USES_BSLMA>::
Pair_Imp(const T1& a, const T2& b)
: first(a), second(b)
{
}

template <class T1, class T2, int T1_USES_BSLMA, int T2_USES_BSLMA>
template <class U1, class U2>
inline
Pair_Imp<T1, T2, T1_USES_BSLMA, T2_USES_BSLMA>::
Pair_Imp(const U1& a, const U2& b)
: first(a), second(b)
{
}

template <class T1, class T2, int T1_USES_BSLMA, int T2_USES_BSLMA>
inline
Pair_Imp<T1, T2, T1_USES_BSLMA, T2_USES_BSLMA>::~Pair_Imp()
{
}

                        // -----------------------------
                        // struct Pair_Imp<T1, T2, 1, 0>
                        // -----------------------------

// CREATORS
template <class T1, class T2>
inline
Pair_Imp<T1, T2, 1, 0>::
Pair_Imp(BloombergLP::bslma::Allocator *basicAllocator)
: first(basicAllocator), second()
{
}

template <class T1, class T2>
inline
Pair_Imp<T1, T2, 1, 0>::
Pair_Imp(const T1& a, const T2& b,
         BloombergLP::bslma::Allocator *basicAllocator)
: first(a, basicAllocator), second(b)
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
Pair_Imp<T1, T2, 1, 0>::
Pair_Imp(const U1& a, const U2& b,
         BloombergLP::bslma::Allocator *basicAllocator)
: first(a, basicAllocator), second(b)
{
}

template <class T1, class T2>
inline
Pair_Imp<T1, T2, 1, 0>::~Pair_Imp()
{
}

                        // -----------------------------
                        // struct Pair_Imp<T1, T2, 0, 1>
                        // -----------------------------

// CREATORS
template <class T1, class T2>
inline
Pair_Imp<T1, T2, 0, 1>::
Pair_Imp(BloombergLP::bslma::Allocator *basicAllocator)
: first(), second(basicAllocator)
{
}

template <class T1, class T2>
inline
Pair_Imp<T1, T2, 0, 1>::
Pair_Imp(const T1& a, const T2& b,
         BloombergLP::bslma::Allocator *basicAllocator)
: first(a), second(b, basicAllocator)
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
Pair_Imp<T1, T2, 0, 1>::
Pair_Imp(const U1& a, const U2& b,
         BloombergLP::bslma::Allocator *basicAllocator)
: first(a), second(b, basicAllocator)
{
}

template <class T1, class T2>
inline
Pair_Imp<T1, T2, 0, 1>::~Pair_Imp()
{
}

                        // -----------------------------
                        // struct Pair_Imp<T1, T2, 1, 1>
                        // -----------------------------

// CREATORS
template <class T1, class T2>
inline
Pair_Imp<T1, T2, 1, 1>::
Pair_Imp(BloombergLP::bslma::Allocator *basicAllocator)
: first(basicAllocator), second(basicAllocator)
{
}

template <class T1, class T2>
inline
Pair_Imp<T1, T2, 1, 1>::
Pair_Imp(const T1& a, const T2& b,
         BloombergLP::bslma::Allocator *basicAllocator)
: first(a, basicAllocator), second(b, basicAllocator)
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
Pair_Imp<T1, T2, 1, 1>::
Pair_Imp(const U1& a, const U2& b,
         BloombergLP::bslma::Allocator *basicAllocator)
: first(a, basicAllocator), second(b, basicAllocator)
{
}

template <class T1, class T2>
inline
Pair_Imp<T1, T2, 1, 1>::~Pair_Imp()
{
}

                                 // ----------
                                 // class pair
                                 // ----------

// CREATORS
template <class T1, class T2>
inline
pair<T1, T2>::pair()
: Base()
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(BloombergLP::bslma::Allocator *basicAllocator)
: Base(basicAllocator)
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(const T1& a, const T2& b)
: Base(a, b)
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(const T1&                      a,
                   const T2&                      b,
                   BloombergLP::bslma::Allocator *basicAllocator)
: Base(a, b, basicAllocator)
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(const pair<T1, T2>& original)
: Base(original.first, original.second)
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(const pair<T1, T2>&            original,
                   BloombergLP::bslma::Allocator *basicAllocator)
: Base(original.first, original.second, basicAllocator)
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(const pair<U1, U2>& rhs)
: Base(rhs.first, rhs.second)
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(const pair<U1, U2>&            rhs,
                   BloombergLP::bslma::Allocator *basicAllocator)
: Base(rhs.first, rhs.second, basicAllocator)
{
}

template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>::pair(const native_std::pair<U1, U2>& rhs)
: Base(rhs.first, rhs.second)
{
}

template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>::pair(const native_std::pair<U1, U2>&  rhs,
                   BloombergLP::bslma::Allocator   *basicAllocator)
: Base(rhs.first, rhs.second, basicAllocator)
{
}


template <class T1, class T2>
inline
pair<T1, T2>::~pair()
{
}

// MANIPULATORS
template <class T1, class T2>
inline
pair<T1, T2>&
pair<T1, T2>::operator=(const pair<T1, T2>& rhs)
{
    first = rhs.first;
    second = rhs.second;
    return *this;
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>&
pair<T1, T2>::operator=(const native_std::pair<U1, U2>& rhs)
{
    first = rhs.first;
    second = rhs.second;
    return *this;
}

template <class T1, class T2>
inline
void pair<T1, T2>::swap(pair& other)
{
    // Find either 'std::swap' or a specialized 'swap' for 'T1' and 'T2' via
    // ADL.

    using std::swap;

    swap(first, other.first);
    swap(second, other.second);
}

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class T1, class T2>
inline
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return ! (lhs == rhs);
}

template <class T1, class T2>
inline
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return (lhs.first < rhs.first ? true  :
            rhs.first < lhs.first ? false :
            lhs.second < rhs.second);
}

template <class T1, class T2>
inline
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return rhs < lhs;
}

template <class T1, class T2>
inline
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return ! (rhs < lhs);
}

template <class T1, class T2>
inline
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return ! (lhs < rhs);
}

// FREE FUNCTIONS
template <class T1, class T2>
inline
void swap(pair<T1, T2>& a, pair<T1, T2>& b)
{
    a.swap(b);
}

// HASH SPECIALIZATIONS
template <class HASHALG, class T1, class T2>
void hashAppend(HASHALG& hashAlg, const pair<T1, T2>&  input)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, input.first);
    hashAppend(hashAlg, input.second);
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bsl {

template <class T1, class T2>
struct is_trivially_copyable<pair<T1, T2> >
    : bsl::integral_constant<bool, is_trivially_copyable<T1>::value
                                  && is_trivially_copyable<T2>::value>
{};

template <class T1, class T2>
struct is_trivially_default_constructible<bsl::pair<T1, T2> >
: bsl::integral_constant<bool, is_trivially_default_constructible<T1>::value
                            && is_trivially_default_constructible<T2>::value>
{};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

template <class T1, class T2>
struct IsPair<bsl::pair<T1, T2> > : bsl::true_type
{};

// Note that we must explicitly declare bitwise moveable sine 'T1' or 'T2' may
// be bitwise moveable and not bitwise copyable.

template <class T1, class T2>
struct IsBitwiseMoveable<bsl::pair<T1, T2> >
    : bsl::integral_constant<bool, bslmf::IsBitwiseMoveable<T1>::value
                                  && bslmf::IsBitwiseMoveable<T2>::value>
{};

template <class T1, class T2>
struct IsBitwiseEqualityComparable<bsl::pair<T1, T2> >
: bsl::integral_constant<bool, bslmf::IsBitwiseEqualityComparable<T1>::value
                            && bslmf::IsBitwiseEqualityComparable<T2>::value
                            && sizeof(T1) + sizeof(T2) ==
                                           sizeof(bsl::Pair_Imp<T1, T2, 0, 0>)>
{};

}  // close namespace bslmf

namespace bslma {

template <class T1, class T2>
struct UsesBslmaAllocator<bsl::pair<T1, T2> >
    : bsl::integral_constant<bool, UsesBslmaAllocator<T1>::value
                                   || UsesBslmaAllocator<T2>::value>
{};

}  // close namespace bslma

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
