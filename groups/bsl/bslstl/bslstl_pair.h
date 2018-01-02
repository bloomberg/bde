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
//@DESCRIPTION: 'bsl::pair' is an allocator-aware version of 'std::pair'.
// The 'bsl::pair' class template is instantiated on two types, 'T1' and 'T2',
// and provides two public data members, 'first' and 'second' of types 'T1' and
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
// 'bsl::pair' has four constructors: a default constructor that
// default-constructs the two data members, a copy constructor that
// copy-constructs each data member, a constructor taking two arguments of
// type 'T1' and 'T2', which are used to copy-construct 'first' and 'second'
// respectively, and a conversion constructor template for converting from a
// 'bsl::pair' of different types, 'U1' and 'U2', provided 'U1' is convertible
// to 'T1' and 'U2' is convertible to 'T2'.  If and only if 'T1' and/or 'T2' is
// a type that uses 'bslma::Allocator' for memory allocation, then each
// constructor also has an optional 'bslma::Allocator' pointer argument.
// Whether or not a type uses 'bslma::Allocator' is determined by querying the
// 'bslma::UsesBslmaAllocator' trait for that type.  This component also
// defines a full set of equality and relational operators that can be
// instantiated if 'T1' and 'T2' both provide those operators.
//
// A 'bsl::pair' declares a set of associated type traits that are computed
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
//      // from the specified 'basicAllocator'
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
//      my_String(const char* s, bslma::Allocator *alloc = 0);      // IMPLICIT
//          // Construct a string with contents specified in 's' using the
//          // optionally-specified allocator 'basicAllocator'.
//
//      my_String(const my_String& rhs, bslma::Allocator *alloc = 0);
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

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif


#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#include <bslmf_addlvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ALLOCATORARGT
#include <bslmf_allocatorargt.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
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

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLMF_USESALLOCATORARGT
#include <bslmf_usesallocatorargt.h>
#endif

#ifndef INCLUDED_BSLMF_UTIL
#include <bslmf_util.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_CPP11
#include <bsls_cpp11.h>
#endif

#ifndef INCLUDED_BSLS_LIBRARYFEATURES
#include <bsls_libraryfeatures.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
# ifndef INCLUDED_TUPLE
#  include <tuple>  // 'std::tuple'
#  define INCLUDED_TUPLE
# endif
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

#if !defined(BSLS_PLATFORM_CMP_SUN) \
 || !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
namespace std {
template <class TYPE>
void swap(TYPE& a, TYPE& b);
}  // close namespace std
#endif

#endif // ! BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES  && ! CLANG

namespace tmp {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
template <int ...>
struct index_sequence { };

template <int N, int ...I>
struct make_index_sequence : make_index_sequence<N-1, N-1, I...> { };

template <int ...I>
struct make_index_sequence<0, I...>
{
    typedef index_sequence<I...> type;
};
#endif

}  // close namespace tmp

namespace bsl {

                        // =========================
                        // trait Pair_AllocatorIdiom
                        // =========================

typedef bsl::integral_constant<int, 0> Pair_BslmaIdiomNone;
    // Type tag for types that do not take a 'bslma::Allocator*' constructor
    // argument.

typedef bsl::integral_constant<int, 1> Pair_BslmaIdiomAtEnd;
    // Type tag for types that take a 'bslma::Allocator*' as the last argument
    // of their constructors.

typedef bsl::integral_constant<int, 2> Pair_BslmaIdiomAllocatorArgT;
    // Type tag for types that take a 'bslma::Allocator*' as the second
    // argument of their constructors, following an argument of type
    // 'bsl::allocator_arg_t'.

template <class TYPE>
struct Pair_BslmaIdiom : bsl::integral_constant<int,
        BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value
        ? (BloombergLP::bslmf::UsesAllocatorArgT<TYPE>::value
           ? Pair_BslmaIdiomAllocatorArgT::value
           : Pair_BslmaIdiomAtEnd::value)
        : Pair_BslmaIdiomNone::value>
{
    // Component-private meta-function to determine whether the specified
    // 'TYPE' template parameter takes a 'bslma::Allocator*' constructor
    // argument and, of so, whether that argument is at the end of the
    // argument list or at the begining of the argument list following an
    // argument of type 'bsl::allocator_arg_t'.  This type is derived from one
    // of 'Pair_BslmaIdiomNone', 'Pair_BslmaIdiomAtEnd', or
    // 'Pair_BslmaIdiomAllocatorArgT'.
};

                             // ====================
                             // struct Pair_MakeUtil
                             // ====================

struct Pair_MakeUtil {
    // This class provides a suite of utility functions for returning newly
    // constructed pair elements by value.
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    // CLASS METHODS
    template <class TYPE, class ...Args, int ...I>
    static TYPE make(
           BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
           tmp::index_sequence<I...>);
        // Construct and return by value an object of (template parameter) type
        // 'TYPE', forwarding in order the elements in the specified 'tuple' to
        // the corresponding constructor of 'TYPE'.

    template <class TYPE, class ...Args, int ...I>
    static TYPE make(
           BloombergLP::bslma::Allocator                         *,
           BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
           tmp::index_sequence<I...>,
           Pair_BslmaIdiomNone);
    template <class TYPE, class ...Args, int ...I>
    static TYPE make(
           BloombergLP::bslma::Allocator                         *allocator,
           BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
           tmp::index_sequence<I...>,
           Pair_BslmaIdiomAtEnd);
    template <class TYPE, class ...Args, int ...I>
    static TYPE make(
           BloombergLP::bslma::Allocator                         *allocator,
           BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
           tmp::index_sequence<I...>,
           Pair_BslmaIdiomAllocatorArgT);
        // Construct and return by value an object of (template parameter) type
        // 'TYPE', forwarding in order the elements in the specified 'tuple' to
        // the corresponding constructor of 'TYPE', using the specified
        // 'basicAllocator' to supply memory.  Note that exactly one of these
        // three constructors is enabled at compile-time for 'TYPE' based on
        // the following respective criteria: 1) 'TYPE' does not support
        // 'bslma'-style allocators, 2) 'TYPE' takes a 'bslma'-style allocator
        // as the last constructor argument, and 3) 'TYPE' takes a 'bslma'-
        // style allocator as the second constructor argument preceded by
        // 'bsl::allocator_arg'.
#endif
};

                             // =================
                             // struct Pair_First
                             // =================

template <class TYPE>
struct Pair_First {
    // This component-private 'class' holds the 'first' data member of a 'pair'
    // and constructs it appropriately.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                 MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // implementing movable references in C++03 and C++11 environments.

    typedef typename Pair_BslmaIdiom<TYPE>::type               FirstBslmaIdiom;

  protected:
    // PUBLIC DATA
    TYPE first;

    // CREATORS
    Pair_First();
        // Construct the 'first' member of a 'pair' using the default
        // constructor for (template parameter) 'TYPE'.

    Pair_First(BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    Pair_First(BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    Pair_First(BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAllocatorArgT);
        // Construct the 'first' member of a 'pair', using the specified
        // 'basicAllocator' to supply memory.  Note that exactly one of these
        // three constructors is enabled at compile-time for (template
        // parameter) type 'TYPE' based on the following respective criteria:
        // 1) 'TYPE' does not support 'bslma'-style allocators, 2) 'TYPE'
        // takes a 'bslma'-style allocator as the last constructor argument,
        // and 3) 'TYPE' takes a 'bslma'-style allocator as the second
        // constructor argument preceded by 'bsl::allocator_arg'.

    explicit Pair_First(
                   typename bsl::add_lvalue_reference<const TYPE>::type value);
        // Construct the 'first' member from the specified non-modifiable
        // 'value', without specifying an allocator.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U>
    explicit Pair_First(U&& value);
#else
    template <class U>
    explicit Pair_First(const U& value);
    template <class U>
    explicit Pair_First(U& value);
#endif
        // TBD: improve comment.
        // Construct the 'first' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U>
    Pair_First(U&&                            value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class U>
    Pair_First(U&&                            value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    template <class U>
    Pair_First(U&&                            value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAllocatorArgT);
#else
    template <class U>
    Pair_First(const U&                       value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class U>
    Pair_First(U&                             value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class U>
    Pair_First(const U&                       value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    template <class U>
    Pair_First(U&                             value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    template <class U>
    Pair_First(const U&                       value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAllocatorArgT);
    template <class U>
    Pair_First(U&                             value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAllocatorArgT);
#endif
        // Construct the 'first' member of a 'pair' from the specified 'value',
        // using the specified 'basicAllocator' to supply memory.  This
        // function (perfectly) forwards 'value' to the constructor of
        // (template parameter) 'TYPE'.  Note that exactly one of these three
        // constructors is enabled at compile-time for (template parameter)
        // type 'TYPE' based on the following respective criteria: 1) 'TYPE'
        // does not support 'bslma'-style allocators, 2) 'TYPE' takes a
        // 'bslma'-style allocator as the last constructor argument, and 3)
        // 'TYPE' takes a 'bslma'-style allocator as the second constructor
        // argument preceded by 'bsl::allocator_arg'.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    template<class ...Args>
    Pair_First(native_std::tuple<Args...> tuple);
    template<class ...Args>
    Pair_First(native_std::tuple<Args...> tuple,
               BloombergLP::bslma::Allocator *basicAllocator);
        // Construct the 'first' member of a 'pair', forwarding in order the
        // elements in the specified 'tuple' to the corresponding constructor
        // of (template parameter) type 'TYPE'.  Optionally specify a
        // 'basicAllocator' used to supply memory.
#endif

    //! Pair_First(const Pair_First&) = default;
    //! Pair_First(Pair_First&&) = default;
    //! Pair_First& operator=(Pair_First&&) = default;
    //! ~Pair_First() = default;
};

                             // ==================
                             // struct Pair_Second
                             // ==================

template <class TYPE>
struct Pair_Second {
    // This component-private 'class' holds the 'second' data member of a
    // 'pair' and constructs it appropriately.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // implementing movable references in C++03 and C++11 environments.

    typedef typename Pair_BslmaIdiom<TYPE>::type              SecondBslmaIdiom;

  protected:
    // PUBLIC DATA
    TYPE second;

    // CREATORS
    Pair_Second();
        // Construct the 'second' member of a 'pair' using the default
        // constructor for (template parameter) type 'TYPE'.

    Pair_Second(BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
    Pair_Second(BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    Pair_Second(BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAllocatorArgT);
        // Construct the 'second' member of a 'pair', using the specified
        // 'basicAllocator' to supply memory.  Note that exactly one of these
        // three constructors is enabled at compile-time for (template
        // parameter) type 'TYPE' based on the following respective criteria:
        // 1) 'TYPE' does not support 'bslma'-style allocators, 2) 'TYPE'
        // takes a 'bslma'-style allocator as the last constructor argument,
        // and 3) 'TYPE' takes a 'bslma'-style allocator as the second
        // constructor argument preceded by 'bsl::allocator_arg'.

    explicit Pair_Second(
                   typename bsl::add_lvalue_reference<const TYPE>::type value);
        // Construct the 'second' member from the specified non-modifiable
        // 'value', without specifying an allocator.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U>
    explicit Pair_Second(U&& value);
#else
    template <class U>
    explicit Pair_Second(const U& value);
    template <class U>
    explicit Pair_Second(U& value);
#endif
        // Construct the 'second' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U>
    Pair_Second(BSLS_COMPILERFEATURES_FORWARD_REF(U) value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class U>
    Pair_Second(BSLS_COMPILERFEATURES_FORWARD_REF(U) value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    template <class U>
    Pair_Second(BSLS_COMPILERFEATURES_FORWARD_REF(U) value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomAllocatorArgT);
#else
    template <class U>
    Pair_Second(const U& value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class U>
    Pair_Second(U& value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class U>
    Pair_Second(const U& value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    template <class U>
    Pair_Second(U& value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    template <class U>
    Pair_Second(const U& value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomAllocatorArgT);
    template <class U>
    Pair_Second(U& value,
                BloombergLP::bslma::Allocator       *basicAllocator,
                Pair_BslmaIdiomAllocatorArgT);
#endif
        // Construct the 'second' member of a 'pair' from the specified
        // forwarding reference 'value', using the specified 'basicAllocator'
        // to supply memory.  This function (perfectly) forwards 'value' to the
        // constructor of (template parameter) 'TYPE'.  Note that exactly one
        // of these three constructors is enabled at compile-time for (template
        // parameter) type 'TYPE' based on the following respective criteria:
        // 1) 'TYPE' does not support 'bslma'-style allocators, 2) 'TYPE' takes
        // a 'bslma'-style allocator as the last constructor argument, and 3)
        // 'TYPE' takes a 'bslma'-style allocator as the second constructor
        // argument preceded by 'bsl::allocator_arg'.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    template<class ...Args>
    Pair_Second(native_std::tuple<Args...> tuple);
    template<class ...Args>
    Pair_Second(native_std::tuple<Args...> tuple,
                BloombergLP::bslma::Allocator *basicAllocator);
        // Construct the 'second' member of a 'pair', forwarding in order the
        // elements in the specified 'tuple' to the corresponding constructor
        // of (template parameter) 'TYPE'.  Optionally specify a
        // 'basicAllocator' used to supply memory if (template parameter) type
        // 'TYPE' accepts an allocator upon construction.
#endif

    //! Pair_Second(const Pair_Second&) = default;
    //! Pair_Second(Pair_Second&&) = default;
    //! Pair_Second& operator=(Pair_Second&&) = default;
    //! ~Pair_Second() = default;
};


                                // ==========
                                // class pair
                                // ==========

template <class T1, class T2>
class pair : public Pair_First<T1>, public Pair_Second<T2> {
    // The class template 'pair' provides a pair of public data members,
    // 'first' and 'second', of type 'T1' and 'T2' respectively.  If either
    // 'T1' or 'T2' uses 'bslma::Allocator' for memory management, then provide
    // an optional 'bslma::Allocator' argument for each constructor, to be
    // passed through to the constructors of 'first' and/or 'second' as
    // appropriate.  The interface to this class is identical to the standard
    // 'std::pair' except for the addition of the allocators.  Note that the
    // implementation of this class provides 'first' and 'second' through
    // multiple base classes in order to simplify construction of each member
    // when allowing for the various rules for passing allocators in C++11.

    // PRIVATE TYPES
    typedef Pair_First<T1>                                 FirstBase;
    typedef Pair_Second<T2>                                SecondBase;
        // 'ThirdBase' and 'HomePlate' left as an exercise for the reader.

    typedef typename Pair_BslmaIdiom<T1>::type             FirstBslmaIdiom;
    typedef typename Pair_BslmaIdiom<T2>::type             SecondBslmaIdiom;

    // The constructors that take an argument of type 'bslma::Allocator *'
    // (allocator-aware constructors) should not be callable unless either
    // 'T1' or 'T2' uses a bslma allocator.  The typedef 'AllocatorPtr' is
    // used as the allocator argument in these constructors and is computed to
    // be 'bslma::Allocator*' when allocator-aware constructors are valid and
    // 'bslmf::Nil' when they are not valid, thus preventing the invalid
    // constructors from participating in overload resolution.  This technique
    // is used instead of 'enable_if' because the allocator type is not a
    // template parameter and cannot, therefore, be used for SFINAE
    // metaprogramming.

    typedef typename bsl::conditional<
        (BloombergLP::bslma::UsesBslmaAllocator<T1>::value ||
         BloombergLP::bslma::UsesBslmaAllocator<T2>::value),
        BloombergLP::bslma::Allocator *,
        BloombergLP::bslmf::Nil>::type                     AllocatorPtr;

    typedef BloombergLP::bslmf::MovableRefUtil             MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

  public:
    // PUBLIC TYPES
    typedef T1 first_type;
    typedef T2 second_type;

    // PUBLIC DATA
    using FirstBase::first;
    using SecondBase::second;

    // CREATORS
    pair();
    explicit pair(AllocatorPtr basicAllocator);
        // Construct a 'pair' with the 'first' and 'second' members initialized
        // to default values.  Optionally specify a 'basicAllocator' used to
        // supply memory if either (template parameter) type 'T1' or 'T2' (or
        // both) use 'bslma'-style allocators; using the second version if
        // neither 'T1' nor 'T2' use 'bslma'-style allocators will result in a
        // compile-time error.  This method requires that 'T1' and 'T2' be
        // default-constructible.

    pair(typename bsl::add_lvalue_reference<const T1>::type a,
         typename bsl::add_lvalue_reference<const T2>::type b);
    pair(typename bsl::add_lvalue_reference<const T1>::type a,
         typename bsl::add_lvalue_reference<const T2>::type b,
         AllocatorPtr                                       basicAllocator);
        // Construct a 'pair' with the 'first' member initialized to the
        // specified 'a' value and the 'second' member initialized to the
        // specified 'b' value.  Optionally specify a 'basicAllocator' used to
        // supply memory if either (template parameter) type 'T1' or 'T2' (or
        // both) use 'bslma'-style allocators; using the second version if
        // neither 'T1' nor 'T2' use 'bslma'-style allocators will result in a
        // compile-time error.  This method requires that 'T1' and 'T2' be
        // copy-constructible.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U1, class U2>
    pair(U1&& a,
         U2&& b,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value
                              && !(bsl::is_pointer<typename
                                        bsl::remove_reference<U2>::type>::value
                                && bsl::is_convertible<U2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(BSLS_COMPILERFEATURES_FORWARD(U1, a))
    , SecondBase(BSLS_COMPILERFEATURES_FORWARD(U2, b))
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }
    template <class U1, class U2>
    pair(U1&& a, U2&& b, AllocatorPtr basicAllocator);
#else
    template <class U1, class U2>
    pair(const U1& a,
         const U2& b,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value
                              && !(bsl::is_pointer<typename
                                        bsl::remove_reference<U2>::type>::value
                                && bsl::is_convertible<U2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }
    template <class U1, class U2>
    pair(const U1& a, const U2& b, AllocatorPtr basicAllocator);

    template <class U1, class U2>
    pair(U1&       a,
         const U2& b,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value
                              && !(bsl::is_pointer<typename
                                        bsl::remove_reference<U2>::type>::value
                                && bsl::is_convertible<U2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }
    template <class U1, class U2>
    pair(U1& a, const U2& b, AllocatorPtr basicAllocator);

    template <class U1, class U2>
    pair(const U1& a,
         U2&       b,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value
                              && !(bsl::is_pointer<typename
                                        bsl::remove_reference<U2>::type>::value
                                && bsl::is_convertible<U2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }
    template <class U1, class U2>
    pair(const U1& a, U2& b, AllocatorPtr basicAllocator);

    template <class U1, class U2>
    pair(U1& a,
         U2& b,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value
                              && !(bsl::is_pointer<typename
                                        bsl::remove_reference<U2>::type>::value
                                && bsl::is_convertible<U2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }
    template <class U1, class U2>
    pair(U1& a, U2& b, AllocatorPtr basicAllocator);
#endif
        // Construct a pair with the 'first' member initialized to the
        // specified 'a' value of (template parameter) type 'U1' and the
        // 'second' member initialized to the specified 'b' value of (template
        // parameter) type 'U2'.  Optionally specify a 'basicAllocator' used to
        // supply memory if either (template parameter) type 'T1' or 'T2' (or
        // both) use 'bslma'-style allocators; using the second version
        // if neither 'T1' nor 'T2' use 'bslma'-style allocators will result in
        // a compile-time error.  This method requires that 'T1' and 'T2' be
        // convertible from 'U1' and 'U2', respectively.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    template<class ...Args1, class ...Args2>
    pair(native_std::piecewise_construct_t,
         native_std::tuple<Args1...> first_args,
         native_std::tuple<Args2...> second_args);
    template<class ...Args1, class ...Args2>
    pair(native_std::piecewise_construct_t,
         native_std::tuple<Args1...> first_args,
         native_std::tuple<Args2...> second_args,
         AllocatorPtr           basicAllocator);
        // Create a 'pair' from piece-wise construction of 'first' and 'second'
        // values by forwarding in order the elements in the specified
        // 'first_args' and 'second_args' tuples to the corresponding
        // constructor of (template parameter) types 'T1' and 'T2',
        // respectively.  Optionally specify a 'basicAllocator' used to supply
        // memory if either (template parameter) type 'T1' or 'T2' (or both)
        // use 'bslma'-style allocators; using the second version if neither
        // 'T1' nor 'T2' use 'bslma'-style allocators will result in a
        // compile-time error.  This method requires that 'T1' and 'T2' be
        // constructible from (the variable number of template paramters)
        // 'Args1' and 'Args2' respectively.
#endif

    pair(const pair& original);
    pair(const pair& original, AllocatorPtr basicAllocator);
        // Construct a 'pair' having the same value as that of the specified
        // 'original' pair.  Optionally specify a 'basicAllocator' used to
        // supply memory if either (template parameter) type 'T1' or 'T2' (or
        // both) use 'bslma'-style allocators; using the second version if
        // neither 'T1' nor 'T2' use 'bslma'-style allocators will result in a
        // compile-time error.  This method requires that 'T1' and 'T2' be
        // copy-constructible.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    pair(pair&& original);
    pair(pair&& original, AllocatorPtr basicAllocator);
#else
    pair(BloombergLP::bslmf::MovableRef<pair> original);
    pair(BloombergLP::bslmf::MovableRef<pair> original,
         AllocatorPtr                         basicAllocator);
        // Construct a pair having the same value as that of the specified
        // 'original' before the call to the move constructor.  Optionally
        // specify a 'basicAllocator' used to supply memory if either (template
        // parameter) 'T1' or 'T2' (or both) use 'bslma'-style allocators;
        // using the second version if neither 'T1' nor 'T2' use 'bslma'-style
        // allocators will result in a compile-time error.  Note that
        // 'original' is left in a valid but unspecified state.  Also note that
        // this method requires that (template parameter) types 'T1' and 'T2'
        // be move-constructible.
#endif

    template <class U1, class U2>
#if defined(BSLSTL_PAIR_ENABLE_ALL_CONVERTIBILITY_CHECKS)
    pair(const pair<U1, U2>& other,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value,
                                 void *>::type = 0);
#else
    pair(const pair<U1, U2>& other);
#endif
    template <class U1, class U2>
    pair(const pair<U1, U2>& other, AllocatorPtr basicAllocator);
        // Construct a 'pair' from the specified 'other' pair, holding 'first'
        // and 'second' values of (template parameter) type 'U1' and 'U2'
        // respectively.  Optionally specify a 'basicAllocator' used to supply
        // memory if either (template parameter) type 'T1' or 'T2' (or both)
        // use 'bslma'-style allocataors; using the second version if neither
        // 'T1' nor 'T2' use 'bslma'-style allocators will result in a compile-
        // time error.  This method requires that 'T1' and 'T2' be convertible
        // from 'U1' and 'U2', respectively.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U1, class U2>
    pair(pair<U1, U2>&& other,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value,
                                 void *>::type = 0);
    template <class U1, class U2>
    pair(pair<U1, U2>&& other, AllocatorPtr basicAllocator);
        // Construct a 'pair' from the specified 'other' pair, holding 'first'
        // and 'second' values of (template parameter) type 'U1' and 'U2'
        // respectively.  Optionally specify a 'basicAllocator' used to supply
        // memory if either (template parameter) type 'T1' or 'T2' (or both)
        // use 'bslma'-style allocators; using the second version if neither
        // 'T1' nor 'T2' use 'bslma'-style allocators will result in compile-
        // time error.  This method requires that 'T1' and 'T2' be convertible
        // from 'U1' and 'U2', respectively.
#else
    template <class U1, class U2>
    pair(BloombergLP::bslmf::MovableRef<pair<U1, U2> > other,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value,
                                 void *>::type = 0)
    : FirstBase(MovUtil::move(MovUtil::access(other).first))
    , SecondBase(MovUtil::move(MovUtil::access(other).second))
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }
    template <class U1, class U2>
    pair(BloombergLP::bslmf::MovableRef<pair<U1, U2> > other,
         AllocatorPtr basicAllocator);
        // Construct a 'pair' from the specified 'other' pair, holding 'first'
        // and 'second' values of (template parameter) type 'U1' and 'U2'
        // respectively.  Optionally specify a 'basicAllocator' used to supply
        // memory if either (template parameter) type 'T1' or 'T2' (or both)
        // use 'bslma'-style allocators; using the second version if neither
        // 'T1' nor 'T2' use 'bslma'-style allocators will result in a compile-
        // time error.  This method requires that 'T1' and 'T2' be convertible
        // from 'U1' and 'U2', respectively.
#endif

    template <class U1, class U2>
#if defined(BSLSTL_PAIR_ENABLE_ALL_CONVERTIBILITY_CHECKS)
    pair(const native_std::pair<U1, U2>&  rhs,
         typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                              && bsl::is_convertible<U2, T2>::value,
                                 void *>::type = 0);                // IMPLICIT
#else
    pair(const native_std::pair<U1, U2>&  rhs);                     // IMPLICIT
#endif
    template <class U1, class U2>
    pair(const native_std::pair<U1, U2>&  rhs,
         BloombergLP::bslma::Allocator   *basicAllocator);
        // Create a 'pair' that has the same value as the specified 'rhs' pair,
        // where the type 'rhs' is the pair type native to the compiler's
        // library, holding the parameterized types 'U1' and 'U2'.  Uses the
        // default allocator to provide memory.  Optionally specify a
        // 'basicAllocator' used to supply memory for the constructor(s) of
        // which ever data member(s) accept an allocator.  The behavior is
        // undefined unless 'T1' is constructible from 'U1' and 'T2' is
        // constructible from from 'U2'.


    //! ~pair() = default;
        // Destroy this object.  Call destructors on 'first' and 'second'.

    // MANIPULATORS
    pair& operator=(const pair& rhs);
        // Assign to this 'pair' the value of the specified 'rhs' pair and
        // return a reference providing modifiable access to this object.
        // This method requires that (template parameter) types 'T1' and 'T2'
        // be copy-assignable.

    pair& operator=(BloombergLP::bslmf::MovableRef<pair> rhs)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Assign to this 'pair' the value of the specified 'rhs' pair (before
        // the call to the assignment), and return a reference providing
        // modifiable access to this object.  Note that 'rhs' is left in a
        // valid but unspecified state.  This method requires that (template
        // parameter) types 'T1' and 'T2' be move-assignable.

    template <class U1, class U2>
    pair& operator=(const pair<U1, U2>& rhs);
        // Assign to this 'pair' from the specified 'rhs' pair, holding the
        // parameterized types 'U1' and 'U2', and return a reference offering
        // modifiable access to this object.  Assign 'first' the value in
        // 'rhs.first' and 'second' the value in 'rhs.second'.  Attempted use
        // of this assignment operator will not compile unless both 'T1' and
        // 'T2' supply assignment operators, and 'T1' is assignable from 'U1'
        // and 'T2' is assignable from 'U2'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class U1, class U2>
    pair& operator=(pair<U1, U2>&& rhs);
        // Assign to this 'pair' the value of the specified 'rhs' pair, holding
        // 'first' and 'second' values of (template parameter) type 'U1' and
        // 'U2' respectively, and return a reference providing modifiable
        // access to this object.  This method requires that 'T1' be assignable
        // from 'U1' and 'T2' be assignable from 'U2'.
#else
    template <class U1, class U2>
    pair& operator=(BloombergLP::bslmf::MovableRef<pair<U1, U2> > rhs);
        // Assign to this 'pair' the value of the specified 'rhs' pair, holding
        // 'first' and 'second' values of (template parameter) types 'U1' and
        // 'U2' respectively, and return a reference providing modifiable
        // access to this object.  This method requires that 'T1' be assignable
        // from 'U1' and 'T2' be assignable from 'U2'.
#endif

    template <class U1, class U2>
    pair& operator=(const native_std::pair<U1, U2>& rhs);
        // Assign to this 'pair' from the specified 'rhs' pair, where the type
        // 'rhs' is the pair type native to the compiler's library, holding the
        // parameterized types 'U1' and 'U2', and return a reference offering
        // modifiable access to this object.  Assign 'first' the value in
        // 'rhs.first' and 'second' the value in 'rhs.second'.  Attempted use
        // of this assignment operator will not compile unless both 'T1' and
        // 'T2' supply assignment operators, and 'T1' is assignable from 'U1'
        // and 'T2' is assignable from 'U2'.

    void swap(pair& other)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Swap the value of this pair with the value of the specified 'other'
        // pair by applying 'swap' to each of the 'first' and 'second' pair
        // fields.  Note that this method is no-throw only if 'swap' on each
        // field is no-throw.
};

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
void swap(pair<T1, T2>& a, pair<T1, T2>& b)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
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

                             // --------------------
                             // struct Pair_MakeUtil
                             // --------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template <class TYPE, class ...Args, int ...I>
inline
TYPE Pair_MakeUtil::make(
           BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
           tmp::index_sequence<I...>)
{
        return TYPE(native_std::get<I>(BSLS_COMPILERFEATURES_FORWARD(
                                        native_std::tuple<Args...>,tuple))...);
}

template <class TYPE, class ...Args, int ...I>
inline
TYPE Pair_MakeUtil::make(
           BloombergLP::bslma::Allocator                                *,
           BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
           tmp::index_sequence<I...>,
           Pair_BslmaIdiomNone)
{
    return TYPE(native_std::get<I>(BSLS_COMPILERFEATURES_FORWARD(
                                        native_std::tuple<Args...>,tuple))...);
}

template <class TYPE, class ...Args, int ...I>
inline
TYPE Pair_MakeUtil::make(
       BloombergLP::bslma::Allocator                                *allocator,
       BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
       tmp::index_sequence<I...>,
       Pair_BslmaIdiomAtEnd)
{
    return TYPE(native_std::get<I>(BSLS_COMPILERFEATURES_FORWARD(
                                         native_std::tuple<Args...>,tuple))...,
                allocator);
}

template <class TYPE, class ...Args, int ...I>
inline
TYPE Pair_MakeUtil::make(
       BloombergLP::bslma::Allocator                                *allocator,
       BSLS_COMPILERFEATURES_FORWARD_REF(native_std::tuple<Args...>) tuple,
       tmp::index_sequence<I...>,
       Pair_BslmaIdiomAllocatorArgT)
{
    return TYPE(bsl::allocator_arg,
                allocator,
                native_std::get<I>(BSLS_COMPILERFEATURES_FORWARD(
                                        native_std::tuple<Args...>,tuple))...);
}
#endif

                             // -----------------
                             // struct Pair_First
                             // -----------------

// CREATORS
template <class TYPE>
inline
Pair_First<TYPE>::Pair_First()
: first()
{
}

template <class TYPE>
inline
Pair_First<TYPE>::Pair_First(BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first()
{
}

template <class TYPE>
inline
Pair_First<TYPE>::Pair_First(BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(basicAllocator)
{
}

template <class TYPE>
inline
Pair_First<TYPE>::Pair_First(BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg, basicAllocator)
{
}

template <class TYPE>
inline
Pair_First<TYPE>::Pair_First(
                    typename bsl::add_lvalue_reference<const TYPE>::type value)
: first(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U&& value)
: first(BSLS_COMPILERFEATURES_FORWARD(U,value))
{
}
#else
template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(const U& value)
: first(value)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U& value)
: first(value)
{
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U&& value,
                             BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first(BSLS_COMPILERFEATURES_FORWARD(U,value))
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U&&                            value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(BSLS_COMPILERFEATURES_FORWARD(U, value), basicAllocator)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U&&                            value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg,
        basicAllocator,
        BSLS_COMPILERFEATURES_FORWARD(U, value))
{
}
#else
template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(const U& value,
                             BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first(value)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U& value,
                             BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first(value)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(const U& value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(value, basicAllocator)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U& value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(value, basicAllocator)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(const U& value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg, basicAllocator, value)
{
}

template <class TYPE>
template <class U>
inline
Pair_First<TYPE>::Pair_First(U& value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg, basicAllocator, value)
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template <class TYPE>
template <class ...Args>
inline
Pair_First<TYPE>::Pair_First(native_std::tuple<Args...> tuple)
: first(Pair_MakeUtil::make<TYPE>(
              BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args...>, tuple),
              typename tmp::make_index_sequence<
                     native_std::tuple_size<native_std::tuple<Args...> >::value
                                               >::type()))
{
}

template <class TYPE>
template <class ...Args>
inline
Pair_First<TYPE>::Pair_First(native_std::tuple<Args...> tuple,
                             BloombergLP::bslma::Allocator *allocator)
: first(Pair_MakeUtil::make<TYPE>(
              allocator,
              BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args...>, tuple),
              typename tmp::make_index_sequence<
                     native_std::tuple_size<native_std::tuple<Args...> >::value
                                               >::type(),
              FirstBslmaIdiom()))
{
}
#endif

                             // ------------------
                             // struct Pair_Second
                             // ------------------

// CREATORS
template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second()
: second()
{
}

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second()
{
}

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(BloombergLP::bslma::Allocator *alloc,
                               Pair_BslmaIdiomAtEnd)
: second(alloc)
{
}

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(BloombergLP::bslma::Allocator *alloc,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg, alloc)
{
}

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(
    typename bsl::add_lvalue_reference<const TYPE>::type value)
: second(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U&& value)
: second(BSLS_COMPILERFEATURES_FORWARD(U, value))
{
}
#else
template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(const U& value)
: second(value)
{
}
template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U& value)
: second(value)
{
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U&& value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second(BSLS_COMPILERFEATURES_FORWARD(U, value))
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U&&                            value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(BSLS_COMPILERFEATURES_FORWARD(U, value), basicAllocator)
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U&&                            value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg,
         basicAllocator,
         BSLS_COMPILERFEATURES_FORWARD(U, value))
{
}
#else
template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(const U& value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second(value)
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U& value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second(value)
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(const U&                       value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(value, basicAllocator)
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U&                             value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(value, basicAllocator)
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(const U&                       value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg, basicAllocator, value)
{
}

template <class TYPE>
template <class U>
inline
Pair_Second<TYPE>::Pair_Second(U&                             value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg, basicAllocator, value)
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template <class TYPE>
template <class ...Args>
inline
Pair_Second<TYPE>::Pair_Second(native_std::tuple<Args...> tuple)
: second(Pair_MakeUtil::make<TYPE>(
              BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args...>, tuple),
              typename tmp::make_index_sequence<
                     native_std::tuple_size<native_std::tuple<Args...> >::value
                                               >::type()))
{
}

template <class TYPE>
template <class ...Args>
inline
Pair_Second<TYPE>::Pair_Second(native_std::tuple<Args...> tuple,
                               BloombergLP::bslma::Allocator *allocator)
: second(Pair_MakeUtil::make<TYPE>(
              allocator,
              BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args...>, tuple),
              typename tmp::make_index_sequence<
                     native_std::tuple_size<native_std::tuple<Args...> >::value
                                               >::type(),
              SecondBslmaIdiom()))
{
}
#endif

                                 // ----------
                                 // class pair
                                 // ----------

// CREATORS
template <class T1, class T2>
inline
pair<T1, T2>::pair()
: FirstBase()
, SecondBase()
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(AllocatorPtr basicAllocator)
: FirstBase (basicAllocator, FirstBslmaIdiom() )
, SecondBase(basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(typename bsl::add_lvalue_reference<const T1>::type a,
                   typename bsl::add_lvalue_reference<const T2>::type b)
: FirstBase(a)
, SecondBase(b)
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(
             typename bsl::add_lvalue_reference<const T1>::type a,
             typename bsl::add_lvalue_reference<const T2>::type b,
             AllocatorPtr                                       basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(U1&& a, U2&& b, AllocatorPtr basicAllocator)
: FirstBase(BSLS_COMPILERFEATURES_FORWARD(U1,a),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(BSLS_COMPILERFEATURES_FORWARD(U2,b),
            basicAllocator,
            SecondBslmaIdiom())
{
}
#else
template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(const U1& a, const U2& b, AllocatorPtr basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(U1& a, const U2& b, AllocatorPtr basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(const U1& a, U2& b, AllocatorPtr basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(U1& a, U2& b, AllocatorPtr basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
template <class T1, class T2>
template<class ...Args1, class ...Args2>
inline
pair<T1, T2>::pair(native_std::piecewise_construct_t,
                   native_std::tuple<Args1...> first_args,
                   native_std::tuple<Args2...> second_args)
: FirstBase(BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args1...>,
                                          first_args))
, SecondBase(BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args2...>,
                                           second_args))
{
}

template <class T1, class T2>
template<class ...Args1, class ...Args2>
inline
pair<T1, T2>::pair(native_std::piecewise_construct_t,
                   native_std::tuple<Args1...> first_args,
                   native_std::tuple<Args2...> second_args,
                   AllocatorPtr           basicAllocator)
: FirstBase(
        BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args1...>, first_args),
        basicAllocator)
, SecondBase(
       BSLS_COMPILERFEATURES_FORWARD(native_std::tuple<Args2...>, second_args),
       basicAllocator)
{
}
#endif

template <class T1, class T2>
inline
pair<T1, T2>::pair(const pair& original)
: FirstBase(original.first)
, SecondBase(original.second)
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(const pair& original, AllocatorPtr basicAllocator)
: FirstBase(original.first, basicAllocator, FirstBslmaIdiom())
, SecondBase(original.second, basicAllocator, SecondBslmaIdiom())
{
}

#if defined (BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
inline
pair<T1, T2>::pair(pair&& original)
: FirstBase(BSLS_COMPILERFEATURES_FORWARD(T1,original.first))
, SecondBase(BSLS_COMPILERFEATURES_FORWARD(T2,original.second))
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(pair&& original, AllocatorPtr basicAllocator)
: FirstBase(BSLS_COMPILERFEATURES_FORWARD(T1,original.first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(BSLS_COMPILERFEATURES_FORWARD(T2,original.second),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#else
template <class T1, class T2>
inline
pair<T1, T2>::pair(BloombergLP::bslmf::MovableRef<pair> original)
: FirstBase(MovUtil::move(MovUtil::access(original).first))
, SecondBase(MovUtil::move(MovUtil::access(original).second))
{
}

template <class T1, class T2>
inline
pair<T1, T2>::pair(BloombergLP::bslmf::MovableRef<pair> original,
                   AllocatorPtr basicAllocator)
: FirstBase(MovUtil::move(MovUtil::access(original).first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(MovUtil::move(MovUtil::access(original).second),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#endif

template <class T1, class T2>
template <class U1, class U2>
inline
#if defined(BSLSTL_PAIR_ENABLE_ALL_CONVERTIBILITY_CHECKS)
pair<T1, T2>::pair(const pair<U1, U2>& other,
                   typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                                        && bsl::is_convertible<U2, T2>::value,
                                           void *>::type)
#else
pair<T1, T2>::pair(const pair<U1, U2>& other)
#endif
: FirstBase(other.first)
, SecondBase(other.second)
{
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>::pair(const pair<U1, U2>& other, AllocatorPtr basicAllocator)
: FirstBase(other.first, basicAllocator, FirstBslmaIdiom())
, SecondBase(other.second, basicAllocator, SecondBslmaIdiom())
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>::pair(pair<U1, U2>&& other,
                   typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                                        && bsl::is_convertible<U2, T2>::value,
                                           void *>::type)
: FirstBase(MovUtil::move(other.first))
, SecondBase(MovUtil::move(other.second))
{
}

template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>::pair(pair<U1, U2>&& other, AllocatorPtr basicAllocator)
: FirstBase(MovUtil::move(other.first), basicAllocator, FirstBslmaIdiom())
, SecondBase(MovUtil::move(other.second), basicAllocator, SecondBslmaIdiom())
{
}
#else
template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>::pair(BloombergLP::bslmf::MovableRef<pair<U1, U2> > other,
                   AllocatorPtr basicAllocator)
: FirstBase(MovUtil::move(MovUtil::access(other).first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(MovUtil::move(MovUtil::access(other).second),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#endif

template <class T1, class T2>
template <class U1, class U2>
#if defined(BSLSTL_PAIR_ENABLE_ALL_CONVERTIBILITY_CHECKS)
pair<T1, T2>::pair(const native_std::pair<U1, U2>& rhs,
                   typename bsl::enable_if<bsl::is_convertible<U1, T1>::value
                                        && bsl::is_convertible<U2, T2>::value,
                                           void *>::type)
#else
pair<T1, T2>::pair(const native_std::pair<U1, U2>& rhs)
#endif
: FirstBase(rhs.first)
, SecondBase(rhs.second)
{
}

template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>::pair(const native_std::pair<U1, U2>&  rhs,
                   BloombergLP::bslma::Allocator   *basicAllocator)
: FirstBase(rhs.first, basicAllocator, FirstBslmaIdiom())
, SecondBase(rhs.second, basicAllocator, SecondBslmaIdiom())
{
}


// MANIPULATORS
template <class T1, class T2>
inline
pair<T1, T2>& pair<T1, T2>::operator=(const pair& rhs)
{
    first = rhs.first;
    second = rhs.second;
    return *this;
}

template <class T1, class T2>
inline
pair<T1, T2>& pair<T1, T2>::operator=(BloombergLP::bslmf::MovableRef<pair> rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    pair& lvalue = rhs;
    first = MovUtil::move(lvalue.FirstBase::first);
    second = MovUtil::move(lvalue.SecondBase::second);
    return *this;
}

template <class T1, class T2>
template <class U1, class U2>
inline
pair<T1, T2>& pair<T1, T2>::operator=(const pair<U1, U2>& rhs)
{
    first = rhs.first;
    second = rhs.second;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>& pair<T1, T2>::operator=(pair<U1, U2>&& rhs)
{
    first = MovUtil::move(rhs.first);
    second = MovUtil::move(rhs.second);
    return *this;
}
#else
template <class T1, class T2>
template <class U1, class U2>
pair<T1, T2>& pair<T1, T2>::operator=(
                            BloombergLP::bslmf::MovableRef<pair<U1, U2> > rhs)
{
    pair<U1, U2>& lvalue = rhs;
    first = MovUtil::move(lvalue.first);
    second = MovUtil::move(lvalue.second);
    return *this;
}
#endif

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
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
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
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
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
                                           sizeof(bsl::pair<T1, T2>)>
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
