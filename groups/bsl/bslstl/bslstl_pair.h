// bslstl_pair.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_PAIR
#define INCLUDED_BSLSTL_PAIR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple 'struct' with two members that may use allocators.
//
//@CLASSES:
//  bsl::pair: pair of values, each of which may use a 'bslma::Allocator'
//
//@CANONICAL_HEADER: bsl_utility.h
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
// copy-constructs each data member, a constructor taking two arguments of type
// 'T1' and 'T2', which are used to copy-construct 'first' and 'second'
// respectively, and a conversion constructor template for converting from a
// 'bsl::pair' of different types, 'PARAM_1' and 'PARAM_2', provided 'PARAM_1'
// is convertible to 'T1' and 'PARAM_2' is convertible to 'T2'.  Each
// constructor also has an optional 'bslma::Allocator' pointer argument.  If
// neither 'T1' nor 'T2' use 'bslma::Allocator', this argument is ignored.
// Otherwise, either 'first' or 'second', or both, depending on whether each
// type uses 'bslma::Allocator', will be passed the 'bslma::Allocator*'
// argument during construction.  Whether or not a type uses 'bslma::Allocator'
// is determined by querying the 'bslma::UsesBslmaAllocator' trait for that
// type.  This component also defines a full set of equality and relational
// operators that can be instantiated if 'T1' and 'T2' both provide those
// operators.
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
///Enhancements Enabled by Modern Language Standards
///-------------------------------------------------
// Language standards after the first (C++98) and its corrigendum (C++03) have
// added code C++ language features, type traits as well as requirements that
// affect the exact interface 'pair' provides.  This section describes such
// enhancements as they become available.
//
///Conditional Default Constructor (C++11)
///- - - - - - - - - - - - - - - - - - - -
// C++11 has introduced type traits, many of which needs compiler backing to be
// (automatically implementable) and also introduced "defaulted" (special
// member) functions that generate code without making mistakes.  Before C++11
// it was not possible to determine if a type had a default constructor in a
// non-intrusive manner.  C++11 makes it possible using <type_traits> to
// determine that, and '= default' makes it possible to create special member
// functions that exists only when they can be implemented properly.
//
// Hence, when using compilers with a reasonably complete implementation of
// C++11 (notably MSVC 2013 is not one of those) we only implement the default
// constructor of pair if both types inside the pair type have a default
// constructor.  Note that it means that when using C++11 (except in compilers
// not implementing it properly) a 'bsl::pair' that stores a reference type
// (such as 'int&'), or any other type that cannot be default constructed using
// the syntax 'T v{};' will cause pair to neither declare nor define a default
// constructor.  So from C++11 onwards a type of 'bsl::pair<T1, T2>' will have
// a default constructor only if both types 'T1' and 'T2' are default
// constructible.  Otherwise, pair will have a default constructor that gives a
// compile-error (only) if called.
//
///Usage
///-----
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

#include <bslscm_version.h>

#include <bslstl_hash.h>

#include <bslalg_synththreewayutil.h>

#include <bslh_hash.h>

#include <bslma_allocator.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#include <bslma_managedptr_pairproxy.h>
#endif // BDE_OMIT_INTERNAL_DEPRECATED
#include <bslma_usesbslmaallocator.h>

#include <bslmf_addlvaluereference.h>
#include <bslmf_allocatorargt.h>
#include <bslmf_conditional.h>
#include <bslmf_integersequence.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_ispair.h>
#include <bslmf_isswappable.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_makeintegersequence.h>
#include <bslmf_movableref.h>
#include <bslmf_usesallocatorargt.h>
#include <bslmf_util.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <cstddef>  // 'std::size_t'

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
#  include <tuple>  // 'std::tuple'
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
#include <concepts>
#endif

#include <utility> // 'std::pair' and (in C++11 mode) 'std::swap'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

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


#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS) \
 || (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
# define BSLSTL_PAIR_NO_IMPLICIT_DELETED_FOR_MOVE_OPS 1
// In order to support the correct signature for copy constructor/assignment
// operators of members with non-'const' references for those operations, we
// must take the implicitly generated declarations.  However, the specification
// for assignment through references requires defining the assignment operator
// in those cases, and that will delete any (otherwise) implicitly-declared
// constructors, so they must be explicitly defaulted on platforms that support
// them.  However, Visual C++ 2013 refused to recognize these defaults as valid
// for move constructors, so a special exception must be made in this case.
#endif

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS) ||            \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS) ||          \
    (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
// MSVC 2013 implicitly declared and defines a default constructor, even for
// members that are not default constructible such as references.
#define BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR 1
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#if (BSLS_COMPILERFEATURES_CPLUSPLUS < 201703L ||                             \
     BSLS_PLATFORM_CMP_VERSION <= 1916)
// MSVC 2017 and earlier, as well as later versions of MSVC compiling against
// pre-C++17 standards, are unable to perform the required template argument
// deductions to enable the use of 'bsl::is_swappable' within the SFINAE test
// for 'swap()'.  In this case we must fall back on the previous, pre-C++17
// implementation which simply assumes swappability.
#define BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE 1
#endif
#endif

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
// When compiling without 'decltype' support, 'bsl::is_swappable' is not
// defined.  In this case we must fall back on the previous, pre-C++17
// implementation which simply assumes swappability.
#define BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE 1
#endif

namespace BloombergLP {
namespace bslstl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
template <std::size_t... INTS>
using Pair_IndexSequence = bslmf::IntegerSequence<std::size_t, INTS...>;
    // 'Pair_IndexSequence' is an alias to the 'bslmf::IntegerSequence' class
    // template specialized for the common case of the integer sequence were
    // the element type 'T' is 'std::size_t'.


template <std::size_t N>
using Pair_MakeIndexSequence = bslmf::MakeIntegerSequence<std::size_t, N>;
    // 'Pair_MakeIndexSequence' is an alias template to the
    // 'bslmf::MakeIntegerSequence' meta-function specialized for the common
    // case of the integer sequence were the element type 'T' is 'std::size_t'.
#endif

}  // close package namespace
}  // close enterprise namespace

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
    // This component-private meta-function determines whether the specified
    // 'TYPE' template parameter takes a 'bslma::Allocator*' constructor
    // argument and, if so, whether that argument is at the end of the argument
    // list or at the beginning of the argument list following an argument of
    // type 'bsl::allocator_arg_t'.  This type derived from
    // 'bsl::integral_constant<int, N>' where 'N' is the number of additional
    // parameters required to pass an allocator to a constructor using the
    // chosen idiom.
};

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template <class TYPE, class ... ARGS>
struct Pair_ConstructionParametersPackLength : integral_constant<int,
                               sizeof...(ARGS) + Pair_BslmaIdiom<TYPE>::value >
{
    // This component-private component-private meta-function determines the
    // number of elements in a tuple containing parameters for piecewise
    // construction of a 'bsl::pair' member having the specified 'TYPE'.
    // Result value depends on the 'TYPE', whether it takes a
    // 'bslma::Allocator*' constructor argument and, if so, whether that
    // argument follows an argument of type 'bsl::allocator_arg_t'.
};
#endif

                        // ===================
                        // struct Pair_ImpUtil
                        // ===================
struct Pair_ImpUtil {
     // This 'struct' provides a namespace for utility functions used to
     // creating a tuple, containing arguments for the constructor of pair
     // element in the process of 'bsl::pair' piecewise construction.

    // CLASS METHODS
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    template <class ... ARGS>
    static
    std::tuple<ARGS...>
    concatAllocator(
                 BSLS_COMPILERFEATURES_FORWARD_REF(std::tuple<ARGS...>)  tpl,
                 BloombergLP::bslma::Allocator                          *alloc,
                 bsl::Pair_BslmaIdiomNone);
        // Construct and return by value a tuple, containing arguments for the
        // corresponding  constructor of (template parameter) 'TYPE',
        // forwarding in order the elements in the specified 'tpl' and
        // discarding the specified 'alloc', because 'TYPE' does not support
        // 'bslma'-style allocators.  This method provides the no-throw
        // exception-safety guarantee.


    template <class ... ARGS>
    static
    std::tuple<ARGS..., BloombergLP::bslma::Allocator *>
    concatAllocator(
                 BSLS_COMPILERFEATURES_FORWARD_REF(std::tuple<ARGS...>)  tpl,
                 BloombergLP::bslma::Allocator                          *alloc,
                 bsl::Pair_BslmaIdiomAtEnd);
        // Construct and return by value a tuple, containing arguments for the
        // corresponding  constructor of (template parameter) 'TYPE',
        // forwarding in order the elements in the specified 'tpl' and
        // appending the specified 'alloc', because 'TYPE' takes a
        // 'bslma'-style allocator as the last constructor argument.  This
        // method provides the no-throw exception-safety guarantee.

    template <class ... ARGS>
    static
    std::tuple<bsl::allocator_arg_t,
               BloombergLP::bslma::Allocator *,
               ARGS...>
    concatAllocator(
                 BSLS_COMPILERFEATURES_FORWARD_REF(std::tuple<ARGS...>)  tpl,
                 BloombergLP::bslma::Allocator                          *alloc,
          bsl::Pair_BslmaIdiomAllocatorArgT);
        // Construct and return by value a tuple, containing arguments for the
        // corresponding constructor of (template parameter) 'TYPE', forwarding
        // in order the elements in the specified 'tpl' preceded by
        // 'bsl::allocator_arg' object and the specified 'alloc', because
        // 'TYPE' takes a 'bslma'-style allocator as the second constructor
        // argument preceded by 'bsl::allocator_arg'.  This method provides the
        // no-throw exception-safety guarantee.
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT) &&                         \
    defined(BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE)
    template <class TYPE1, class TYPE2>
    static constexpr bool hasNothrowSwap()
        // Utility function to determine whether 'swap()' is 'noexcept (true)'
        // when called with the specified (template) arguments 'TYPE1' and
        // 'TYPE2'.  This function is only defined on platforms where
        // 'noexcept' is supported but the newer, C++17 compatible trait
        // 'bsl::is_nothrow_swappable' is not available.
    {
        using std::swap;
        typedef BloombergLP::bslmf::Util U;
        return BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(U::declval<TYPE1&>(),
                                                   U::declval<TYPE1&>())) &&
               BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(U::declval<TYPE2&>(),
                                                   U::declval<TYPE2&>()));
    }
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
    typedef BloombergLP::bslmf::MovableRefUtil   MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // implementing movable references in C++03 and C++11 environments.

    typedef typename Pair_BslmaIdiom<TYPE>::type FirstBslmaIdiom;

  protected:
  public:
    // PUBLIC DATA
    TYPE first;

    // CREATORS
#if !defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)
    template <
        class BSLSTL_DUMMY = TYPE,
        typename
            std::enable_if<std::is_default_constructible<BSLSTL_DUMMY>::value,
                           int>::type = 0>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First() : first()
    {
        // This constructor template must be defined inline inside the
        // class definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }
#else
    BSLS_KEYWORD_CONSTEXPR
    Pair_First();
#endif
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

    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(
                   typename bsl::add_lvalue_reference<const TYPE>::type value);
        // Construct the 'first' member from the specified non-modifiable
        // 'value', without specifying an allocator.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(PARAM&& value);
#else
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(const PARAM& value);
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(PARAM& value);
#endif
        // TBD: improve comment.
        // Construct the 'first' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    Pair_First(PARAM&&                        value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class PARAM>
    Pair_First(PARAM&&                        value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    template <class PARAM>
    Pair_First(PARAM&&                        value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAllocatorArgT);
#else
    template <class PARAM>
    Pair_First(const PARAM&                   value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class PARAM>
    Pair_First(PARAM&                         value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class PARAM>
    Pair_First(const PARAM&                   value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    template <class PARAM>
    Pair_First(PARAM&                         value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAtEnd);
    template <class PARAM>
    Pair_First(const PARAM&                   value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomAllocatorArgT);
    template <class PARAM>
    Pair_First(PARAM&                         value,
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    template <class ...ARGS, size_t ...I>
    Pair_First(std::tuple<ARGS...>&& argsPack,
               BloombergLP::bslstl::Pair_IndexSequence<I...>);
        // Construct the 'first' member of a 'pair', forwarding in order the
        // elements in the specified 'argsPack' to the corresponding
        // constructor of (template parameter) 'TYPE'.  The length of the
        // 'argsPack' is equal to the lenght of the specified (template
        // parameter pack) 'I...' and passed to the constructor via the
        // 'Pair_IndexSequence' object.
#endif

    //! Pair_First(const Pair_First&) = default;
    //! Pair_First(Pair_First&&) = default;
    //! Pair_First& operator=(Pair_First&&) = default;
    //! ~Pair_First() = default;
};

template <class TYPE>
struct Pair_First<TYPE&> {
    // This component-private 'class' holds the 'first' data member of a 'pair'
    // and constructs it appropriately.

  protected:
    // PROTECTED DATA
    TYPE& first;

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(TYPE& value);
        // Bind the specified 'value' into the 'first' reference-member.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(PARAM&& value);
#else
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(const PARAM& value);   // for derived-to-const-base-ref
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(PARAM& value);
#endif
        // Bind the specified 'value' into the 'first' reference-member.
        // TBD: Consider SFINAE-ing these constructors

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First(PARAM&&                        value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
#else
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First(const PARAM&                   value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First(PARAM&                         value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
#endif
        // Bind the specified 'value' into the 'first' reference-member.  The
        // specified 'basicAllocator' is not used.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    template <class ARG>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First(std::tuple<ARG>&& arg,
               BloombergLP::bslstl::Pair_IndexSequence<0u>);
        // Construct the 'first' member of a 'pair', forwarding in order the
        // elements in the specified 'argsPack' to the corresponding
        // constructor of (template parameter) 'TYPE'.  The length of the
        // 'argsPack' is equal to the lenght of the specified (template
        // parameter pack) 'I...' and passed to the constructor via the
        // 'Pair_IndexSequence' object.
#endif

    //! ~Pair_First() = default;

    // MANIPULATORS
#if !defined(BSLSTL_PAIR_NO_IMPLICIT_DELETED_FOR_MOVE_OPS)
    Pair_First(const Pair_First&) = default;
    Pair_First(Pair_First&&) = default;
#endif
    Pair_First& operator=(const Pair_First& rhs) BSLS_KEYWORD_NOEXCEPT;
};


#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct Pair_First<TYPE&&> {
    // This component-private 'class' holds the 'first' data member of a 'pair'
    // and constructs it appropriately.

  protected:
    // PROTECTED DATA
    TYPE&& first;

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(TYPE&& value);
        // Construct the 'first' member from the specified non-modifiable
        // 'value', without specifying an allocator.

    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_First(PARAM&& value);
        // TBD: improve comment.
        // Construct the 'first' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.
        // TBD: Consider SFINAE-ing this constructor, but maybe better handled
        // at the 'pair' level?

    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First(PARAM&&                        value,
               BloombergLP::bslma::Allocator *basicAllocator,
               Pair_BslmaIdiomNone);
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    template <class ARG>
    BSLS_KEYWORD_CONSTEXPR
    Pair_First(std::tuple<ARG>&& arg,
               BloombergLP::bslstl::Pair_IndexSequence<0u>);
        // Construct the 'first' member of a 'pair', forwarding in order the
        // elements in the specified 'argsPack' to the corresponding
        // constructor of (template parameter) 'TYPE'.  The length of the
        // 'argsPack' is equal to the lenght of the specified (template
        // parameter pack) 'I...' and passed to the constructor via the
        // 'Pair_IndexSequence' object.
#endif

    //! ~Pair_First() = default;

    // MANIPULATORS
#if !defined(BSLSTL_PAIR_NO_IMPLICIT_DELETED_FOR_MOVE_OPS)
    Pair_First(const Pair_First&) = default;
    Pair_First(Pair_First&&) = default;
#endif
    Pair_First& operator=(const Pair_First& rhs) BSLS_KEYWORD_NOEXCEPT;
};
#endif


                             // ==================
                             // struct Pair_Second
                             // ==================

template <class TYPE>
struct Pair_Second {
    // This component-private 'class' holds the 'second' data member of a
    // 'pair' and constructs it appropriately.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil   MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // implementing movable references in C++03 and C++11 environments.

    typedef typename Pair_BslmaIdiom<TYPE>::type SecondBslmaIdiom;

  protected:
    // PROTECTED DATA
    TYPE second;

    // CREATORS
#if !defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)
    template <
        class BSLSTL_DUMMY = TYPE,
        typename
            std::enable_if<std::is_default_constructible<BSLSTL_DUMMY>::value,
                           int>::type = 0>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second() : second()
    {
        // This constructor template must be defined inline inside the
        // class definition, as Microsoft Visual C++ does not recognize the
        // definition as matching this signature when placed out-of-line.
    }
#else
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second();
#endif
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

    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(
                   typename bsl::add_lvalue_reference<const TYPE>::type value);
        // Construct the 'second' member from the specified non-modifiable
        // 'value', without specifying an allocator.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(PARAM&& value);
#else
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(const PARAM& value);
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(PARAM& value);
#endif
        // Construct the 'second' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    Pair_Second(PARAM&&                        value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class PARAM>
    Pair_Second(PARAM&&                        value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    template <class PARAM>
    Pair_Second(PARAM&&                        value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAllocatorArgT);
#else
    template <class PARAM>
    Pair_Second(const PARAM&                   value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class PARAM>
    Pair_Second(PARAM&                         value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class PARAM>
    Pair_Second(const PARAM&                   value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    template <class PARAM>
    Pair_Second(PARAM&                         value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAtEnd);
    template <class PARAM>
    Pair_Second(const PARAM&                   value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomAllocatorArgT);
    template <class PARAM>
    Pair_Second(PARAM&                         value,
                BloombergLP::bslma::Allocator *basicAllocator,
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    template <class ...ARGS, size_t ...I>
    Pair_Second(std::tuple<ARGS...>&& argsPack,
                BloombergLP::bslstl::Pair_IndexSequence<I...>);
        // Construct the 'second' member of a 'pair', forwarding in order the
        // elements in the specified 'argsPack' to the corresponding
        // constructor of (template parameter) 'TYPE'.  The length of the
        // 'argsPack' is equal to the lenght of the specified (template
        // parameter pack) 'I...' and passed to the constructor via the
        // 'Pair_IndexSequence' object.
#endif

    //! Pair_Second(const Pair_Second&) = default;
    //! Pair_Second(Pair_Second&&) = default;
    //! Pair_Second& operator=(Pair_Second&&) = default;
    //! ~Pair_Second() = default;
};

template <class TYPE>
struct Pair_Second<TYPE&> {
    // This component-private 'class' holds the 'second' data member of a
    // 'pair' and constructs it appropriately.

  protected:
    // PUBLIC DATA
    TYPE& second;

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(TYPE& value);
        // Construct the 'second' member from the specified non-modifiable
        // 'value', without specifying an allocator.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(PARAM&& value);
#else
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(const PARAM& value);
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(PARAM& value);
#endif
        // Construct the 'second' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second(PARAM&&                        value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
#else
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second(const PARAM&                   value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second(PARAM&                         value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    template <class ARG>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second(std::tuple<ARG>&& arg,
                BloombergLP::bslstl::Pair_IndexSequence<0u>);
        // Construct the 'second' member of a 'pair', forwarding in order the
        // elements in the specified 'argsPack' to the corresponding
        // constructor of (template parameter) 'TYPE'.  The length of the
        // 'argsPack' is equal to the lenght of the specified (template
        // parameter pack) 'I...' and passed to the constructor via the
        // 'Pair_IndexSequence' object.
#endif

    //! ~Pair_Second() = default;

    // MANIPULATORS
#if !defined(BSLSTL_PAIR_NO_IMPLICIT_DELETED_FOR_MOVE_OPS)
    Pair_Second(const Pair_Second&) = default;
    Pair_Second(Pair_Second&&) = default;
#endif
    Pair_Second& operator=(const Pair_Second& rhs) BSLS_KEYWORD_NOEXCEPT;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct Pair_Second<TYPE&&> {
    // This component-private 'class' holds the 'second' data member of a
    // 'pair' and constructs it appropriately.

  protected:
    // PROTECTED DATA
    TYPE&& second;

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(TYPE&& value);
        // Construct the 'second' member from the specified non-modifiable
        // 'value', without specifying an allocator.

    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    explicit Pair_Second(PARAM&& value);
        // Construct the 'second' member from the specified 'value', without
        // specifying an allocator.  This function (perfectly) forwards 'value'
        // to the constructor of (template parameter) 'TYPE'.

    template <class PARAM>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second(PARAM&&                            value,
                BloombergLP::bslma::Allocator *basicAllocator,
                Pair_BslmaIdiomNone);
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    template <class ARG>
    BSLS_KEYWORD_CONSTEXPR
    Pair_Second(std::tuple<ARG>&& arg,
                BloombergLP::bslstl::Pair_IndexSequence<0u>);
        // Construct the 'second' member of a 'pair', forwarding in order the
        // elements in the specified 'argsPack' to the corresponding
        // constructor of (template parameter) 'TYPE'.  The length of the
        // 'argsPack' is equal to the lenght of the specified (template
        // parameter pack) 'I...' and passed to the constructor via the
        // 'Pair_IndexSequence' object.
#endif

    //! ~Pair_Second() = default;

    // MANIPULATORS
#if !defined(BSLSTL_PAIR_NO_IMPLICIT_DELETED_FOR_MOVE_OPS)
    Pair_Second(const Pair_Second&) = default;
    Pair_Second(Pair_Second&&) = default;
#endif
    Pair_Second& operator=(const Pair_Second& rhs) BSLS_KEYWORD_NOEXCEPT;
};
#endif

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
#if !defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)
    BSLS_KEYWORD_CONSTEXPR
    pair() = default;
#else
    BSLS_KEYWORD_CONSTEXPR
    pair();
#endif
    explicit pair(BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a 'pair' with the 'first' and 'second' members initialized
        // to default values.  Optionally specify a 'basicAllocator', used to
        // supply memory for each of 'first' and 'second' when its type
        // (template parameter 'T1' or 'T2', respectively) uses 'bslma'-style
        // allocators.  This method requires that 'T1' and 'T2' be
        // default-constructible.

    pair(const pair& original, BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a 'pair' having the same value as that of the specified
        // 'original' pair.  Optionally specify a 'basicAllocator', used to
        // supply memory for each of 'first' and 'second' when its type
        // (template parameter 'T1' or 'T2', respectively) uses 'bslma'-style
        // allocators.  Note that the copy constructor is implicitly declared
        // (if 'T1' and 'T2' are both 'copy-constructible') by compilers that
        // do not support defaulted declarations.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    //! pair(pair&& original);  // Allow move ctor to implicitly default/delete
    pair(pair&& original, BloombergLP::bslma::Allocator *basicAllocator);
#else
    BSLS_KEYWORD_CONSTEXPR
    pair(BloombergLP::bslmf::MovableRef<pair>  original);
    pair(BloombergLP::bslmf::MovableRef<pair>  original,
         BloombergLP::bslma::Allocator        *basicAllocator);
        // Construct a pair having the same value as that of the specified
        // 'original' before the call to the move constructor.  Optionally
        // specify a 'basicAllocator', used to supply memory for each of
        // 'first' and 'second' when its type (template parameter 'T1' or 'T2',
        // respectively) uses 'bslma'-style allocators.  Note that 'original'
        // is left in a valid but unspecified state.  Also note that this
        // method requires that 'T1' and 'T2' be move-constructible.  Note that
        // the move constructor is implicitly declared (if 'T1' and 'T2' are
        // both move-constructible) by compilers that do not support defaulted
        // declarations, but do support rvalue references.
#endif

    BSLS_KEYWORD_CONSTEXPR
    pair(typename bsl::add_lvalue_reference<const T1>::type  a,
         typename bsl::add_lvalue_reference<const T2>::type  b);
    pair(typename bsl::add_lvalue_reference<const T1>::type  a,
         typename bsl::add_lvalue_reference<const T2>::type  b,
         BloombergLP::bslma::Allocator                      *basicAllocator);
        // Construct a 'pair' with the 'first' member initialized to the
        // specified 'a' value and the 'second' member initialized to the
        // specified 'b' value.  Optionally specify a 'basicAllocator', used to
        // supply memory for each of 'first' and 'second' when its type
        // (template parameter 'T1' or 'T2', respectively) uses 'bslma'-style
        // allocators.  This method requires that 'T1' and 'T2' be
        // 'copy-constructible'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(PARAM_1&& a,
         PARAM_2&& b,
         typename bsl::enable_if<std::is_constructible<T1, PARAM_1>::value
                              && std::is_constructible<T2, PARAM_2>::value
                              && !(bsl::is_pointer<typename
                                   bsl::remove_reference<PARAM_2>::type>::value
                                && bsl::is_convertible<PARAM_2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(std::forward<PARAM_1>(a))
    , SecondBase(std::forward<PARAM_2>(b))
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }
    template <class PARAM_1, class PARAM_2>
    pair(PARAM_1&&                      a,
         PARAM_2&&                      b,
         BloombergLP::bslma::Allocator *basicAllocator);
#else
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const PARAM_1& a,
         const PARAM_2& b,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value
                              && !(bsl::is_pointer<typename
                                   bsl::remove_reference<PARAM_2>::type>::value
                                && bsl::is_convertible<PARAM_2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }
    template <class PARAM_1, class PARAM_2>
    pair(const PARAM_1&                 a,
         const PARAM_2&                 b,
         BloombergLP::bslma::Allocator *basicAllocator);

    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(PARAM_1&       a,
         const PARAM_2& b,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value
                              && !(bsl::is_pointer<typename
                                   bsl::remove_reference<PARAM_2>::type>::value
                                && bsl::is_convertible<PARAM_2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }
    template <class PARAM_1, class PARAM_2>
    pair(PARAM_1&                       a,
         const PARAM_2&                 b,
         BloombergLP::bslma::Allocator *basicAllocator);

    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const PARAM_1& a,
         PARAM_2&       b,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value
                              && !(bsl::is_pointer<typename
                                   bsl::remove_reference<PARAM_2>::type>::value
                                && bsl::is_convertible<PARAM_2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }
    template <class PARAM_1, class PARAM_2>
    pair(const PARAM_1&                 a,
         PARAM_2&                       b,
         BloombergLP::bslma::Allocator *basicAllocator);

    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(PARAM_1& a,
         PARAM_2& b,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value
                              && !(bsl::is_pointer<typename
                                   bsl::remove_reference<PARAM_2>::type>::value
                                && bsl::is_convertible<PARAM_2,
                                     BloombergLP::bslma::Allocator *>::value),
                                 void *>::type = 0)
    : FirstBase(a)
    , SecondBase(b)
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (MSVC 2010) bug where the definition
        // cannot be matched to the declaration when an 'enable_if' is used.
    }
    template <class PARAM_1, class PARAM_2>
    pair(PARAM_1&                       a,
         PARAM_2&                       b,
         BloombergLP::bslma::Allocator *basicAllocator);
#endif
    // Construct a pair with the 'first' member initialized to the specified
    // 'a' value of (template parameter) type 'PARAM_1' and the 'second' member
    // initialized to the specified 'b' value of (template parameter) type
    // 'PARAM_2'.  Optionally specify a 'basicAllocator', used to supply memory
    // for each of 'first' and 'second' when its type (template parameter 'T1'
    // or 'T2', respectively) uses 'bslma'-style allocators.  This method
    // requires that 'T1' and 'T2' be convertible from 'PARAM_1' and 'PARAM_2',
    // respectively.

#if defined(BSLSTL_PAIR_ENABLE_ALL_CONVERTIBILITY_CHECKS)
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const pair<PARAM_1, PARAM_2>& other,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value,
                                 void *>::type = 0);
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const std::pair<PARAM_1, PARAM_2>& other,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value,
                                 void *>::type = 0);
#else
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const pair<PARAM_1, PARAM_2>& other);
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const std::pair<PARAM_1, PARAM_2>& other);
#endif
    template <class PARAM_1, class PARAM_2>
    pair(const pair<PARAM_1, PARAM_2>&       other,
         BloombergLP::bslma::Allocator      *basicAllocator);
    template <class PARAM_1, class PARAM_2>
    pair(const std::pair<PARAM_1, PARAM_2>&  other,
         BloombergLP::bslma::Allocator      *basicAllocator);
        // Construct a 'pair' from the specified 'other' pair, holding 'first'
        // and 'second' values of (template parameter) type 'PARAM_1' and
        // 'PARAM_2' respectively.  Optionally specify a 'basicAllocator', used
        // to supply memory for each of 'first' and 'second' when its type
        // (template parameter 'T1' or 'T2', respectively) uses 'bslma'-style
        // allocators.  This method requires that 'T1' and 'T2' be convertible
        // from 'PARAM_1' and 'PARAM_2', respectively.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(pair<PARAM_1, PARAM_2>&& other,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value,
                                 void *>::type = 0);
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(std::pair<PARAM_1, PARAM_2>&& other,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value,
                                 void *>::type = 0);
    template <class PARAM_1, class PARAM_2>
    pair(pair<PARAM_1, PARAM_2>&&       other,
         BloombergLP::bslma::Allocator *basicAllocator);
    template <class PARAM_1, class PARAM_2>
    pair(std::pair<PARAM_1, PARAM_2>&&  other,
         BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a 'pair' from the specified 'other' pair, holding 'first'
        // and 'second' values of (template parameter) type 'PARAM_1' and
        // 'PARAM_2' respectively.  Optionally specify a 'basicAllocator', used
        // to supply memory for each of 'first' and 'second' when its type
        // (template parameter 'T1' or 'T2', respectively) uses 'bslma'-style
        // allocators.  This method requires that 'T1' and 'T2' be convertible
        // from 'PARAM_1' and 'PARAM_2', respectively.
#else
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(BloombergLP::bslmf::MovableRef<pair<PARAM_1, PARAM_2> > other,
         typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                              && bsl::is_convertible<PARAM_2, T2>::value,
                                 void *>::type = 0)
    : FirstBase(MovUtil::move(MovUtil::access(other).first))
    , SecondBase(MovUtil::move(MovUtil::access(other).second))
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }

    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(
     BloombergLP::bslmf::MovableRef<std::pair<PARAM_1, PARAM_2> > other,
     typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                                    && bsl::is_convertible<PARAM_2, T2>::value,
                             void *>::type = 0)
    : FirstBase(MovUtil::move(MovUtil::access(other).first))
    , SecondBase(MovUtil::move(MovUtil::access(other).second))
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
    }

    template <class PARAM_1, class PARAM_2>
    pair(
      BloombergLP::bslmf::MovableRef<pair<PARAM_1, PARAM_2> >  other,
      BloombergLP::bslma::Allocator                           *basicAllocator);
    template <class PARAM_1, class PARAM_2>
    pair(BloombergLP::bslmf::MovableRef<std::pair<PARAM_1, PARAM_2> > other,
         BloombergLP::bslma::Allocator *basicAllocator);
        // Construct a 'pair' from the specified 'other' pair, holding 'first'
        // and 'second' values of (template parameter) type 'PARAM_1' and
        // 'PARAM_2' respectively.  Optionally specify a 'basicAllocator', used
        // to supply memory for each of 'first' and 'second' when its type
        // (template parameter 'T1' or 'T2', respectively) uses 'bslma'-style
        // allocators.  This method requires that 'T1' and 'T2' be convertible
        // from 'PARAM_1' and 'PARAM_2', respectively.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    template<class ...ARGS_1, class ...ARGS_2>
    pair(std::piecewise_construct_t,
         std::tuple<ARGS_1...>          first_args,
         std::tuple<ARGS_2...>          second_args);
    template<class ...ARGS_1, class ...ARGS_2>
    pair(std::piecewise_construct_t,
         std::tuple<ARGS_1...>          first_args,
         std::tuple<ARGS_2...>          second_args,
         BloombergLP::bslma::Allocator *basicAllocator);
        // Create a 'pair' from piece-wise construction of 'first' and 'second'
        // values by forwarding in order the elements in the specified
        // 'first_args' and 'second_args' tuples to the corresponding
        // constructor of (template parameter) types 'T1' and 'T2',
        // respectively.  Optionally specify a 'basicAllocator', used to supply
        // memory for each of 'first' and 'second' when its type (template
        // parameter 'T1' or 'T2', respectively) uses 'bslma'-style allocators.
        // Allocators can also be passed as tuple members straight to 'T1' or
        // 'T2' (or both) constructors using the first version (but use of the
        // second version for this approach will result in a compile-time
        // error).  This method requires that 'T1' and 'T2' be constructible
        // from (the variable number of template parameters) 'ARGS_1' and
        // 'ARGS_2' respectively.
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    template <class PARAM_1, class PARAM_2>
    BSLS_KEYWORD_CONSTEXPR
    pair(const BloombergLP::bslma::ManagedPtr_PairProxy<PARAM_1, PARAM_2>&
                                                              rhs); // IMPLICIT
        // Create a 'pair' that has the same value as the specified 'rhs' pair
        // proxy.  The behavior is undefined unless 'T1' is constructible from
        // 'PARAM_1' and 'T2' is constructible from from 'PARAM_2'.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    //! ~pair() = default;
        // Destroy this object.  Call destructors on 'first' and 'second'.

    // MANIPULATORS
    template <class PARAM_1, class PARAM_2>
    pair& operator=(const pair<PARAM_1, PARAM_2>& rhs);
        // Assign to this 'pair' from the specified 'rhs' pair, holding the
        // parameterized types 'PARAM_1' and 'PARAM_2', and return a reference
        // offering modifiable access to this object.  Assign 'first' the value
        // in 'rhs.first' and 'second' the value in 'rhs.second'.  Attempted
        // use of this assignment operator will not compile unless both 'T1'
        // and 'T2' supply assignment operators, and 'T1' is assignable from
        // 'PARAM_1' and 'T2' is assignable from 'PARAM_2'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class PARAM_1, class PARAM_2>
    pair& operator=(pair<PARAM_1, PARAM_2>&& rhs);
        // Assign to this 'pair' the value of the specified 'rhs' pair, holding
        // 'first' and 'second' values of (template parameter) type 'PARAM_1'
        // and 'PARAM_2' respectively, and return a reference providing
        // modifiable access to this object.  This method requires that 'T1' be
        // assignable from 'PARAM_1' and 'T2' be assignable from 'PARAM_2'.
#else
    pair& operator=(BloombergLP::bslmf::MovableRef<pair> rhs)
                                    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false);
        // Assign to this 'pair' the value of the specified 'rhs' pair (before
        // the call to the assignment), and return a reference providing
        // modifiable access to this object.  Note that 'rhs' is left in a
        // valid but unspecified state.  This method requires that (template
        // parameter) types 'T1' and 'T2' be move-assignable.

    template <class PARAM_1, class PARAM_2>
    pair& operator=(
                  BloombergLP::bslmf::MovableRef<pair<PARAM_1, PARAM_2> > rhs);
        // Assign to this 'pair' the value of the specified 'rhs' pair, holding
        // 'first' and 'second' values of (template parameter) types 'PARAM_1'
        // and 'PARAM_2' respectively, and return a reference providing
        // modifiable access to this object.  This method requires that 'T1' be
        // assignable from 'PARAM_1' and 'T2' be assignable from 'PARAM_2'.
#endif

    template <class PARAM_1, class PARAM_2>
    pair& operator=(const std::pair<PARAM_1, PARAM_2>& rhs);
        // Assign to this 'pair' from the specified 'rhs' pair, where the type
        // 'rhs' is the pair type native to the compiler's library, holding the
        // parameterized types 'PARAM_1' and 'PARAM_2', and return a reference
        // offering modifiable access to this object.  Assign 'first' the value
        // in 'rhs.first' and 'second' the value in 'rhs.second'.  Attempted
        // use of this assignment operator will not compile unless both 'T1'
        // and 'T2' supply assignment operators, and 'T1' is assignable from
        // 'PARAM_1' and 'T2' is assignable from 'PARAM_2'.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    template <class PARAM_1, class PARAM_2,
              typename bsl::enable_if<bsl::is_convertible<T1, PARAM_1>::value
                                   && bsl::is_convertible<T2, PARAM_2>::value,
                            bool>::type = 0>
    operator std::tuple<PARAM_1&, PARAM_2&>() BSLS_KEYWORD_NOEXCEPT;
        // Return an 'std::tuple' object, holding references that provide
        // modifiable access to the members of this object.

    template <class PARAM_1,
              typename bsl::enable_if<bsl::is_convertible<T1, PARAM_1>::value,
                            bool>::type = 0>
    operator std::tuple<PARAM_1&, decltype(std::ignore)&>()
                                                         BSLS_KEYWORD_NOEXCEPT;
    // This partial specialization of 'template <class PARAM_1, class PARAM_2>
    // operator std::tuple<PARAM_1&, PARAM_2&>()', for when the (template
    // parameter) 'PARAM_2' (second element's type) is the type of
    // 'std::ignore'.

    template <class PARAM_2,
              typename bsl::enable_if<bsl::is_convertible<T2, PARAM_2>::value,
                            bool>::type = 0>
    operator std::tuple<decltype(std::ignore)&, PARAM_2&>()
                                                         BSLS_KEYWORD_NOEXCEPT;
    // This partial specialization of 'template <class PARAM_1, class PARAM_2>
    // operator std::tuple<PARAM_1&, PARAM_2&>()', for when the (template
    // parameter) 'PARAM_1' (first element's type) is the type of
    // 'std::ignore'.

    operator std::tuple<decltype(std::ignore)&,
                        decltype(std::ignore)&>() BSLS_KEYWORD_NOEXCEPT
    // This partial specialization of 'template <class PARAM_1, class PARAM_2>
    // operator std::tuple<PARAM_1&, PARAM_2&>()', for when the (template
    // parameters) 'PARAM_1' (first element's type) and 'PARAM_2' (second
    // element's type) are the type of 'std::ignore'.  Note that this method is
    // defined within the class body intentionally to avoid build failure on
    // MSVC 2015.
    {
        return std::tuple<decltype(std::ignore)&,
                          decltype(std::ignore)&>(std::ignore,
                                                  std::ignore);
    }
#endif

#if defined(BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE)
    void swap(pair& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                       Pair_ImpUtil::hasNothrowSwap<T1, T2>());
#else
    void swap(pair& other)
     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(bsl::is_nothrow_swappable<T1>::value
                                      && bsl::is_nothrow_swappable<T2>::value);
#endif
        // Swap the value of this pair with the value of the specified 'other'
        // pair by applying 'swap' to each of the 'first' and 'second' pair
        // fields.  Note that this method is no-throw only if 'swap' on each
        // field is no-throw.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

// When we are deducing from a 'pair<T1, T2>' (std or bsl) and an
// 'Allocator *', we want to deduce a 'bsl::pair<T1, T2>', not a
// bsl::pair<pair<T1, T2>, Allocator *>.
//
// Note that order is important; if given an 'Allocator *' and a
// 'pair<T1, T2>' we deduce 'bsl::pair<Allocator *, pair<T1, T2>>'.

template <class T1, class T2>
pair(T1, T2) -> pair<T1, T2>;
    // Deduce the specified types 'T1' and 'T2' from the corresponding types
    // supplied to the constructor of 'pair'.

template <
    class T1,
    class T2,
    class ALLOC,
    class = typename bsl::enable_if_t<
         bsl::is_convertible_v<ALLOC *, BloombergLP::bslma::Allocator *>>
    >
pair(T1, T2, ALLOC *) -> pair<T1, T2>;
    // Deduce the specified types 'T1' and 'T2' from the corresponding types
    // supplied to the constructor of 'pair'.  This guide does not participate
    // in deduction unless the specified 'ALLOC' inherits from
    // 'bslma::Allocator'.

template <
    class T1,
    class T2,
    class ALLOC,
    class = typename bsl::enable_if_t<
         bsl::is_convertible_v<ALLOC *, BloombergLP::bslma::Allocator *>>
    >
pair(pair<T1, T2>, ALLOC *) -> pair<T1, T2>;
    // Deduce the specified types 'T1' and 'T2' from the corresponding template
    // parameters of the 'bsl::pair' supplied to the constructor of 'pair'.
    // This guide does not participate in deduction unless the specified
    // 'ALLOC' inherits from 'bslma::Allocator'.

template <class T1, class T2>
pair(std::pair<T1, T2>) -> pair<T1, T2>;
    // Deduce the specified types 'T1' and 'T2' from the corresponding template
    // parameters of the 'std::pair' supplied to the constructor of 'pair'.

template <
    class T1,
    class T2,
    class ALLOC,
    class = typename bsl::enable_if_t<
         bsl::is_convertible_v<ALLOC *, BloombergLP::bslma::Allocator *>>
    >
pair(std::pair<T1, T2>, ALLOC *) -> pair<T1, T2>;
    // Deduce the specified types 'T1' and 'T2' from the corresponding template
    // parameters of the 'std::pair' supplied to the constructor of 'pair'.
    // This guide does not participate in deduction unless the specified
    // 'ALLOC' inherits from 'bslma::Allocator'.
#endif

// FREE OPERATORS
template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' and 'rhs' pair objects have the same
    // value and false otherwise.  'lhs' has the same value as 'rhs' if
    // 'lhs.first == rhs.first' and 'lhs.second == rhs.second'.  A call to this
    // operator will not compile unless both 'T1' and 'T2' supply 'operator=='.

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' and 'rhs' pair objects do not have
    // the same value and false otherwise.  'lhs' does not have the same value
    // as 'rhs' if 'lhs == rhs' would return false.  A call to this operator
    // will not compile unless a call to 'lhs == rhs' would compile.
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class T1, class T2>
BSLS_KEYWORD_CONSTEXPR
std::common_comparison_category_t<
    BloombergLP::bslalg::SynthThreeWayUtil::Result<T1>,
    BloombergLP::bslalg::SynthThreeWayUtil::Result<T2>
> operator<=>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' pairs by using the comparison operators of 'T1' and
    // 'T2'; return the result of that comparison.

#else

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
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
BSLS_KEYWORD_CONSTEXPR
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value greater than the
    // specified 'rhs' and false otherwise.  'lhs' has a value greater than
    // 'rhs' if 'rhs' < 'lhs' would return true.  A call to this operator will
    // not compile unless a call to 'lhs < rhs' would compile.

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value less than or equal to the
    // specified 'rhs' and false otherwise.  'lhs' has a value less than or
    // equal to 'rhs' if 'rhs' < 'lhs' would return false.  A call to this
    // operator will not compile unless a call to 'lhs < rhs' would compile.

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs);
    // Return true if the specified 'lhs' has a value greater than or equal to
    // the specified 'rhs' and false otherwise.  'lhs' has a value greater than
    // or equal to 'rhs' if 'lhs' < 'rhs' would return false.  A call to this
    // operator will not compile unless a call to 'lhs < rhs' would compile.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class T1, class T2>
#if defined(BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE)
void
#else
typename bsl::enable_if<bsl::is_swappable<T1>::value
                     && bsl::is_swappable<T2>::value>::type
#endif
swap(pair<T1, T2>& a, pair<T1, T2>& b)
BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
    // Swap the values of the specified 'a' and 'b' pairs by applying 'swap' to
    // each of the 'first' and 'second' pair fields.  Note that this method is
    // no-throw only if 'swap' on each field is no-throw.

// HASH SPECIALIZATIONS
template <class HASHALG, class T1, class T2>
void hashAppend(HASHALG& hashAlg, const pair<T1, T2>&  input);
    // Pass the specified 'input' to the specified 'hashAlg'

}  // close namespace bsl

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)

namespace std {

#if defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

                             // ====================
                             // struct tuple_element
                             // ====================
template<class T1, class T2>
struct tuple_element<0, bsl::pair<T1, T2> >
    // This partial specialization of 'tuple_element' provides compile-time
    // access to the type of the pair's first element.
{
    // TYPES
    typedef T1 type;
};

template<class T1, class T2>
struct tuple_element<1, bsl::pair<T1, T2> >
    // This partial specialization of 'tuple_element' provides compile-time
    // access to the type of the pair's second element.
{
    // TYPES
    typedef T2 type;
};

                              // =================
                              // struct tuple_size
                              // =================

template<class T1, class T2>
struct tuple_size<bsl::pair<T1, T2> > : integral_constant<size_t, 2>
    // This meta-function provides a compile-time way to obtain the number of
    // elements in a pair, which is always 2.
{};

#if defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic pop
#endif

}  // close namespace std

namespace BloombergLP {
namespace bslstl {

                          // =====================
                          // class Pair_GetImpUtil
                          // =====================

template <std::size_t INDEX, class T1, class T2>
struct Pair_GetImpUtil
    // This utility class template provides functions for selecting the element
    // of pair, returned from 'bsl::get(bsl::pair<T1, T2>)', by its index.
{
    BSLMF_ASSERT(INDEX < 2);
};

template <class T1, class T2>
struct Pair_GetImpUtil<0, T1, T2>
    // This partial specialization of 'Pair_GetImpUtil', for when the
    // (template parameter) 'INDEX'(element's index) is equal to 0.
{
  private:
    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // implementing movable references in C++03 and C++11 environments.

  public:
    // CLASS METHODS
    static T1& getPairElement(bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing modifiable access to the first element
        // of the specified 'p'.

    static
    const T1& getPairElement(const bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing non-modifiable access to the first
        // element of the specified 'p'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    static T1&&
    getPairElement(bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a rvalue reference providing modifiable access to the first
        // element of the specified 'p'

    static const T1&&
    getPairElement(const bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a rvalue reference providing non-modifiable access to the
        // first element of the specified 'p'

#endif
};

template <class T1, class T2>
struct Pair_GetImpUtil<1u, T1, T2>
    // This partial specialization of 'Pair_GetImpUtil', for when the
    // (template parameter) 'INDEX'(element's index) is equal to 1.
{
  private:
    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MovUtil;
        // This typedef is a convenient alias for the utility associated with
        // implementing movable references in C++03 and C++11 environments.

  public:
    // CLASS METHODS
    static T2& getPairElement(bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing modifiable access to the second element
        // of the specified 'p'.

    static
    const T2& getPairElement(const bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing non-modifiable access to the second
        // element of the specified 'p'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    static T2&&
    getPairElement(bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a rvalue reference providing modifiable access to the second
        // element of the specified 'p'

    static const T2&&
    getPairElement(const bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
        // Return a rvalue reference providing non-modifiable access to the
        // second element of the specified 'p'
#endif
};

template <class T, class T1, class T2>
struct Pair_IndexOfType
    // This meta-function provides a compile-time way to obtain the index of
    // 'bsl::pair' element, having the (template parameter) type 'T'.  If
    // neither type of pair's element is equal to 'T' or both pair's elements
    // have the same type, code is not compiled.
{};

template <class T1, class T2>
struct Pair_IndexOfType<T1, T1, T2> : bsl::integral_constant<size_t, 0>
    // This partial specialization of 'Pair_IndexOfType' returns the index of
    // first element of pair.
{};

template <class T1, class T2>
struct Pair_IndexOfType<T2, T1, T2> : bsl::integral_constant<size_t, 1u>
    // This partial specialization of 'Pair_IndexOfType' returns the index of
    // second element of pair.
{};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

// FREE FUNCTIONS
template<std::size_t INDEX, class T1, class T2>
typename std::tuple_element<INDEX, bsl::pair<T1, T2> >::type&
get(bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a reference providing modifiable access to the element of the
    // specified 'p', having the ordinal number specified by the (template
    // parameter) 'INDEX'.  This function will not compile unless the 'INDEX'
    // is either 0 or 1.

template<std::size_t INDEX, class T1, class T2>
const typename std::tuple_element<INDEX, bsl::pair<T1, T2> >::type&
get(const bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a reference providing non-modifiable access to the element of the
    // specified 'p', having the ordinal number specified by the (template
    // parameter) 'INDEX'.  This function will not compile unless the 'INDEX'
    // is either 0 or 1.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template<std::size_t INDEX, class T1, class T2>
typename std::tuple_element<INDEX, bsl::pair<T1, T2> >::type&&
get(bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a rvalue reference providing modifiable access to the element of
    // the specified 'p', having the ordinal number specified by the (template
    // parameter) 'INDEX'.  This function will not compile unless the 'INDEX'
    // is either 0 or 1.
#endif

template<class TYPE, class T1, class T2>
TYPE& get(bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a reference providing modifiable access to the element of the
    // specified 'p', having the (template parameter) 'TYPE'.  This function
    // will not compile unless the types 'T1' and 'T2' are different and the
    // 'TYPE' is the same as either 'T1' or 'T2'.

template<class TYPE, class T1, class T2>
const TYPE& get(const bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a reference providing non-modifiable access to the element of the
    // specified 'p', having the (template parameter) 'TYPE'.  This function
    // will not compile unless the types 'T1' and 'T2' are different and the
    // 'TYPE' is the same as either 'T1' or 'T2'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template<class TYPE, class T1, class T2>
TYPE&& get(bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a rvalue reference providing modifiable access to the element of
    // the specified 'p', having the (template parameter) 'TYPE'.  This
    // function will not compile unless the types 'T1' and 'T2' are different
    // and the 'TYPE' is the same as either 'T1' or 'T2'.

template<class TYPE, class T1, class T2>
const TYPE&& get(const bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT;
    // Return a rvalue reference providing non-modifiable access to the element
    // of the specified 'p', having the (template parameter) 'TYPE'.  This
    // function will not compile unless the types 'T1' and 'T2' are different
    // and the 'TYPE' is the same as either 'T1' or 'T2'.
#endif

}  // close bsl namespace

#endif


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
// See IMPLEMENTATION NOTES in the .cpp before modifying anything below.

namespace bsl {
                        // -------------------
                        // struct Pair_ImpUtil
                        // -------------------
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template <class ... ARGS>
inline
std::tuple<ARGS...>
Pair_ImpUtil::concatAllocator(
          BSLS_COMPILERFEATURES_FORWARD_REF(std::tuple<ARGS...>)  tpl,
          BloombergLP::bslma::Allocator                          *,
          bsl::Pair_BslmaIdiomNone)
{
    return BloombergLP::bslmf::MovableRefUtil::move(tpl);
}

template <class ... ARGS>
inline
std::tuple<ARGS..., BloombergLP::bslma::Allocator *>
Pair_ImpUtil::concatAllocator(
                 BSLS_COMPILERFEATURES_FORWARD_REF(std::tuple<ARGS...>)  tpl,
                 BloombergLP::bslma::Allocator                          *alloc,
                 bsl::Pair_BslmaIdiomAtEnd)
{
    return std::tuple_cat(BloombergLP::bslmf::MovableRefUtil::move(tpl),
                          std::tie(alloc));
}

template <class ... ARGS>
inline
std::tuple<bsl::allocator_arg_t,
           BloombergLP::bslma::Allocator *,
           ARGS...>
Pair_ImpUtil::concatAllocator(
                 BSLS_COMPILERFEATURES_FORWARD_REF(std::tuple<ARGS...>)  tpl,
                 BloombergLP::bslma::Allocator                          *alloc,
                 bsl::Pair_BslmaIdiomAllocatorArgT)
{
    return std::tuple_cat(std::tie(bsl::allocator_arg, alloc),
                          BloombergLP::bslmf::MovableRefUtil::move(tpl));
}
#endif

                             // -----------------
                             // struct Pair_First
                             // -----------------

// CREATORS
#if defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_First<TYPE>::Pair_First()
: first()
{
}
#endif

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
BSLS_KEYWORD_CONSTEXPR
Pair_First<TYPE>::Pair_First(
                    typename bsl::add_lvalue_reference<const TYPE>::type value)
: first(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_First<TYPE>::Pair_First(PARAM&& value)
: first(BSLS_COMPILERFEATURES_FORWARD(PARAM, value))
{
}
#else
template <class TYPE>
template <class PARAM>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_First<TYPE>::Pair_First(const PARAM& value)
: first(value)
{
}

template <class TYPE>
template <class PARAM>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_First<TYPE>::Pair_First(PARAM& value)
: first(value)
{
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(PARAM&&                        value,
                             BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first(BSLS_COMPILERFEATURES_FORWARD(PARAM,value))
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(PARAM&&                        value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(BSLS_COMPILERFEATURES_FORWARD(PARAM, value), basicAllocator)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(PARAM&&                        value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg,
        basicAllocator,
        BSLS_COMPILERFEATURES_FORWARD(PARAM, value))
{
}
#else
template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(const PARAM&                   value,
                             BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first(value)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(PARAM&                         value,
                             BloombergLP::bslma::Allocator *,
                             Pair_BslmaIdiomNone)
: first(value)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(const PARAM&                   value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(value, basicAllocator)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(PARAM&                         value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAtEnd)
: first(value, basicAllocator)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(const PARAM&                   value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg, basicAllocator, value)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_First<TYPE>::Pair_First(PARAM&                         value,
                             BloombergLP::bslma::Allocator *basicAllocator,
                             Pair_BslmaIdiomAllocatorArgT)
: first(bsl::allocator_arg, basicAllocator, value)
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class TYPE>
template <class ...ARGS, size_t ...I>
inline

Pair_First<TYPE>::Pair_First(std::tuple<ARGS...>&& argsPack,
                             BloombergLP::bslstl::Pair_IndexSequence<I...>)
: first(std::get<I>(std::move(argsPack))...)
{
}
#endif

                             // ------------------------
                             // struct Pair_First<TYPE&>
                             // ------------------------

// CREATORS

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(TYPE& value)
: first(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(PARAM&& value)
: first(std::forward<PARAM>(value))
{
}
#else
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(const PARAM& value)
: first(value)
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(PARAM& value)
: first(value)
{
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(PARAM&&                        value,
                              BloombergLP::bslma::Allocator *,
                              Pair_BslmaIdiomNone                 )
: first(std::forward<PARAM>(value))
{
}
#else
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(const PARAM&                   value,
                              BloombergLP::bslma::Allocator *,
                              Pair_BslmaIdiomNone                 )
: first(value)
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(PARAM&                         value,
                              BloombergLP::bslma::Allocator *,
                              Pair_BslmaIdiomNone                 )
: first(value)
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class TYPE>
template <class ARG>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&>::Pair_First(std::tuple<ARG>&& arg,
                              BloombergLP::bslstl::Pair_IndexSequence<0u>)
: first(std::get<0u>(arg))
{
}
#endif

// MANIPULATORS

template <class TYPE>
inline
Pair_First<TYPE&>& Pair_First<TYPE&>::operator=(const Pair_First& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    first = rhs.first;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
                             // -------------------------
                             // struct Pair_First<TYPE&&>
                             // -------------------------

// CREATORS

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&&>::Pair_First(TYPE&& value)
: first(std::move(value))
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&&>::Pair_First(PARAM&& value)
: first(std::forward<PARAM>(value))
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&&>::Pair_First(PARAM&&                        value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone                 )
: first(std::forward<PARAM>(value))
{
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class TYPE>
template <class ARG>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_First<TYPE&&>::Pair_First(std::tuple<ARG>&& arg,
                               BloombergLP::bslstl::Pair_IndexSequence<0u>)
: first(std::get<0u>(arg))
{
}
#endif

// MANIPULATORS
template <class TYPE>
inline
Pair_First<TYPE&&>& Pair_First<TYPE&&>::operator=(const Pair_First& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    first = rhs.first;
    return *this;
}
#endif

                             // ------------------
                             // struct Pair_Second
                             // ------------------

// CREATORS
#if defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_Second<TYPE>::Pair_Second()
: second()
{
}
#endif

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second()
{
}

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(basicAllocator)
{
}

template <class TYPE>
inline
Pair_Second<TYPE>::Pair_Second(BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg, basicAllocator)
{
}

template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_Second<TYPE>::Pair_Second(
                    typename bsl::add_lvalue_reference<const TYPE>::type value)
: second(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_Second<TYPE>::Pair_Second(PARAM&& value)
: second(BSLS_COMPILERFEATURES_FORWARD(PARAM, value))
{
}
#else
template <class TYPE>
template <class PARAM>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_Second<TYPE>::Pair_Second(const PARAM& value)
: second(value)
{
}
template <class TYPE>
template <class PARAM>
inline
BSLS_KEYWORD_CONSTEXPR
Pair_Second<TYPE>::Pair_Second(PARAM& value)
: second(value)
{
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(PARAM&&                        value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second(BSLS_COMPILERFEATURES_FORWARD(PARAM, value))
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(PARAM&&                        value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(BSLS_COMPILERFEATURES_FORWARD(PARAM, value), basicAllocator)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(PARAM&&                        value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg,
         basicAllocator,
         BSLS_COMPILERFEATURES_FORWARD(PARAM, value))
{
}
#else
template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(const PARAM&                   value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second(value)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(PARAM&                         value,
                               BloombergLP::bslma::Allocator *,
                               Pair_BslmaIdiomNone)
: second(value)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(const PARAM&                   value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(value, basicAllocator)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(PARAM&                         value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAtEnd)
: second(value, basicAllocator)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(const PARAM&                   value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg, basicAllocator, value)
{
}

template <class TYPE>
template <class PARAM>
inline
Pair_Second<TYPE>::Pair_Second(PARAM&                         value,
                               BloombergLP::bslma::Allocator *basicAllocator,
                               Pair_BslmaIdiomAllocatorArgT)
: second(bsl::allocator_arg, basicAllocator, value)
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class TYPE>
template <class ...ARGS, size_t ...I>
inline
Pair_Second<TYPE>::Pair_Second(std::tuple<ARGS...>&& argsPack,
                               BloombergLP::bslstl::Pair_IndexSequence<I...>)
: second(std::get<I>(std::move(argsPack))...)
{
}
#endif

                             // -------------------------
                             // struct Pair_Second<TYPE&>
                             // -------------------------

    // CREATORS
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(TYPE& value)
: second(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(PARAM&& value)
: second(std::forward<PARAM>(value))
{
}
#else
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(const PARAM& value)
: second(value)
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(PARAM& value)
: second(value)
{
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(PARAM&&                        value,
                                BloombergLP::bslma::Allocator *,
                                Pair_BslmaIdiomNone                 )
: second(std::forward<PARAM>(value))
{
}
#else
template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(const PARAM&                   value,
                                BloombergLP::bslma::Allocator *,
                                Pair_BslmaIdiomNone                 )
: second(value)
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(PARAM&                         value,
                                BloombergLP::bslma::Allocator *,
                                Pair_BslmaIdiomNone                 )
: second(value)
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class TYPE>
template <class ARG>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&>::Pair_Second(std::tuple<ARG>&& arg,
                                BloombergLP::bslstl::Pair_IndexSequence<0u>)
: second(std::get<0u>(arg))
{
}
#endif

// MANIPULATORS
template <class TYPE>
inline
Pair_Second<TYPE&>& Pair_Second<TYPE&>::operator=(const Pair_Second& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    second = rhs.second;
    return *this;
}


#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
                             // --------------------------
                             // struct Pair_Second<TYPE&&>
                             // --------------------------

// CREATORS
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
Pair_Second<TYPE&&>::Pair_Second(TYPE&& value)
: second(std::move(value))
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&&>::Pair_Second(PARAM&& value)
: second(std::forward<PARAM>(value))
{
}

template <class TYPE>
template <class PARAM>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&&>::Pair_Second(PARAM&&                        value,
                                 BloombergLP::bslma::Allocator *,
                                 Pair_BslmaIdiomNone            )
: second(std::forward<PARAM>(value))
{
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)                            \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class TYPE>
template <class ARG>
BSLS_KEYWORD_CONSTEXPR
inline
Pair_Second<TYPE&&>::Pair_Second(std::tuple<ARG>&& arg,
                                 BloombergLP::bslstl::Pair_IndexSequence<0u>)
: second(std::get<0u>(arg))
{
}
#endif

// MANIPULATORS
template <class TYPE>
inline
Pair_Second<TYPE&&>& Pair_Second<TYPE&&>::operator=(const Pair_Second& rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    second = rhs.second;
    return *this;
}
#endif
                                 // ----------
                                 // class pair
                                 // ----------

// CREATORS
#if defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)
template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair()
: FirstBase()
, SecondBase()
{
}
#endif

template <class T1, class T2>
inline
pair<T1, T2>::pair(BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(basicAllocator, FirstBslmaIdiom())
, SecondBase(basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
BSLS_KEYWORD_CONSTEXPR
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
            typename bsl::add_lvalue_reference<const T1>::type  a,
            typename bsl::add_lvalue_reference<const T2>::type  b,
            BloombergLP::bslma::Allocator                      *basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(PARAM_1&&                      a,
                   PARAM_2&&                      b,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(BSLS_COMPILERFEATURES_FORWARD(PARAM_1, a),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(BSLS_COMPILERFEATURES_FORWARD(PARAM_2, b),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#else
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(const PARAM_1&                 a,
                   const PARAM_2&                 b,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(PARAM_1&                       a,
                   const PARAM_2&                 b,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(const PARAM_1&                 a,
                   PARAM_2&                       b,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(PARAM_1&                       a,
                   PARAM_2&                       b,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(a, basicAllocator, FirstBslmaIdiom())
, SecondBase(b, basicAllocator, SecondBslmaIdiom())
{
}
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)        \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
template <class T1, class T2>
template<class ...ARGS_1, class ...ARGS_2>
inline
pair<T1, T2>::pair(std::piecewise_construct_t,
                   std::tuple<ARGS_1...>      first_args,
                   std::tuple<ARGS_2...>      second_args)
: FirstBase(std::move(first_args),
            typename BloombergLP::bslstl::Pair_MakeIndexSequence<
                   std::tuple_size<std::tuple<ARGS_1...> >::value
                                              >())
, SecondBase(std::move(second_args),
            typename BloombergLP::bslstl::Pair_MakeIndexSequence<
                   std::tuple_size<std::tuple<ARGS_2...> >::value
                                              >())
{
}

template <class T1, class T2>
template<class ...ARGS_1, class ...ARGS_2>
inline
pair<T1, T2>::pair(std::piecewise_construct_t,
                   std::tuple<ARGS_1...>          first_args,
                   std::tuple<ARGS_2...>          second_args,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(Pair_ImpUtil::concatAllocator(std::move(first_args),
                                          basicAllocator,
                                          FirstBslmaIdiom()),
            typename BloombergLP::bslstl::Pair_MakeIndexSequence<
                Pair_ConstructionParametersPackLength<T1, ARGS_1...>::value>())
, SecondBase(Pair_ImpUtil::concatAllocator(std::move(second_args),
                                           basicAllocator,
                                           SecondBslmaIdiom()),
             typename BloombergLP::bslstl::Pair_MakeIndexSequence<
                Pair_ConstructionParametersPackLength<T2, ARGS_2...>::value>())
{
}
#endif

template <class T1, class T2>
inline
pair<T1, T2>::pair(const pair&                    original,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(original.first, basicAllocator, FirstBslmaIdiom())
, SecondBase(original.second, basicAllocator, SecondBslmaIdiom())
{
}

#if defined (BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
inline
pair<T1, T2>::pair(pair&&                         original,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(BSLS_COMPILERFEATURES_FORWARD(T1, original.first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(BSLS_COMPILERFEATURES_FORWARD(T2, original.second),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#else
template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(BloombergLP::bslmf::MovableRef<pair> original)
: FirstBase(MovUtil::move(MovUtil::access(original).first))
, SecondBase(MovUtil::move(MovUtil::access(original).second))
{
}

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(BloombergLP::bslmf::MovableRef<pair>  original,
                   BloombergLP::bslma::Allocator        *basicAllocator)
: FirstBase(MovUtil::move(MovUtil::access(original).first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(MovUtil::move(MovUtil::access(original).second),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#endif

#if defined(BSLSTL_PAIR_ENABLE_ALL_CONVERTIBILITY_CHECKS)
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(
               const pair<PARAM_1, PARAM_2>& other,
               typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                                    && bsl::is_convertible<PARAM_2, T2>::value,
                        void *>::type)
: FirstBase(other.first)
, SecondBase(other.second)
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(
               const std::pair<PARAM_1, PARAM_2>& other,
               typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                                    && bsl::is_convertible<PARAM_2, T2>::value,
                        void *>::type)
: FirstBase(other.first)
, SecondBase(other.second)
{
}
#else
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(const pair<PARAM_1, PARAM_2>& other)
: FirstBase(other.first)
, SecondBase(other.second)
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(const std::pair<PARAM_1, PARAM_2>& other)
: FirstBase(other.first)
, SecondBase(other.second)
{
}
#endif

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(const pair<PARAM_1, PARAM_2>&  other,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(other.first, basicAllocator, FirstBslmaIdiom())
, SecondBase(other.second, basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>::pair(const std::pair<PARAM_1, PARAM_2>&  other,
                   BloombergLP::bslma::Allocator      *basicAllocator)
: FirstBase(other.first, basicAllocator, FirstBslmaIdiom())
, SecondBase(other.second, basicAllocator, SecondBslmaIdiom())
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(
               pair<PARAM_1, PARAM_2>&& other,
               typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                                    && bsl::is_convertible<PARAM_2, T2>::value,
                        void *>::type)
: FirstBase(MovUtil::move(other.first))
, SecondBase(MovUtil::move(other.second))
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(
               std::pair<PARAM_1, PARAM_2>&& other,
               typename bsl::enable_if<bsl::is_convertible<PARAM_1, T1>::value
                                    && bsl::is_convertible<PARAM_2, T2>::value,
                        void *>::type)
: FirstBase(MovUtil::move(other.first))
, SecondBase(MovUtil::move(other.second))
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
pair<T1, T2>::pair(pair<PARAM_1, PARAM_2>&&       other,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(MovUtil::move(other.first), basicAllocator, FirstBslmaIdiom())
, SecondBase(MovUtil::move(other.second), basicAllocator, SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
pair<T1, T2>::pair(std::pair<PARAM_1, PARAM_2>&&  other,
                   BloombergLP::bslma::Allocator *basicAllocator)
: FirstBase(MovUtil::move(other.first), basicAllocator, FirstBslmaIdiom())
, SecondBase(MovUtil::move(other.second), basicAllocator, SecondBslmaIdiom())
{
}
#else
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
pair<T1, T2>::pair(
    BloombergLP::bslmf::MovableRef<pair<PARAM_1, PARAM_2> > other,
    BloombergLP::bslma::Allocator                          *basicAllocator)
: FirstBase(MovUtil::move(MovUtil::access(other).first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(MovUtil::move(MovUtil::access(other).second),
             basicAllocator,
             SecondBslmaIdiom())
{
}

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
pair<T1, T2>::pair(
   BloombergLP::bslmf::MovableRef<std::pair<PARAM_1, PARAM_2> > other,
   BloombergLP::bslma::Allocator                               *basicAllocator)
: FirstBase(MovUtil::move(MovUtil::access(other).first),
            basicAllocator,
            FirstBslmaIdiom())
, SecondBase(MovUtil::move(MovUtil::access(other).second),
             basicAllocator,
             SecondBslmaIdiom())
{
}
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
BSLS_KEYWORD_CONSTEXPR
pair<T1, T2>::pair(
         const BloombergLP::bslma::ManagedPtr_PairProxy<PARAM_1, PARAM_2>& rhs)
: FirstBase(rhs.first)
, SecondBase(rhs.second)
{
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

// MANIPULATORS
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
pair<T1, T2>& pair<T1, T2>::operator=(BloombergLP::bslmf::MovableRef<pair> rhs)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
{
    pair& lvalue = rhs;
    first = MovUtil::move(lvalue.first);
    second = MovUtil::move(lvalue.second);
    return *this;
}
#endif

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>& pair<T1, T2>::operator=(const pair<PARAM_1, PARAM_2>& rhs)
{
    first = rhs.first;
    second = rhs.second;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
pair<T1, T2>& pair<T1, T2>::operator=(pair<PARAM_1, PARAM_2>&& rhs)
{
    first = MovUtil::move(rhs.first);
    second = MovUtil::move(rhs.second);
    return *this;
}
#else
template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
pair<T1, T2>& pair<T1, T2>::operator=(
                   BloombergLP::bslmf::MovableRef<pair<PARAM_1, PARAM_2> > rhs)
{
    pair<PARAM_1, PARAM_2>& lvalue = rhs;
    first = MovUtil::move(lvalue.first);
    second = MovUtil::move(lvalue.second);
    return *this;
}
#endif

template <class T1, class T2>
template <class PARAM_1, class PARAM_2>
inline
pair<T1, T2>&
pair<T1, T2>::operator=(const std::pair<PARAM_1, PARAM_2>& rhs)
{
    first = rhs.first;
    second = rhs.second;
    return *this;
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template <class T1, class T2>
template <class PARAM_1, class PARAM_2,
          typename bsl::enable_if<bsl::is_convertible<T1, PARAM_1>::value
                               && bsl::is_convertible<T2, PARAM_2>::value,
                        bool>::type>
inline
pair<T1, T2>::operator std::tuple<PARAM_1&, PARAM_2&>() BSLS_KEYWORD_NOEXCEPT
{
    return std::tuple<PARAM_1&, PARAM_2&>(first, second);
}

template <class T1, class T2>
template <class PARAM_1,
          typename bsl::enable_if<bsl::is_convertible<T1, PARAM_1>::value,
                        bool>::type>
inline
pair<T1, T2>::operator std::tuple<PARAM_1&, decltype(std::ignore)&>()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::tuple<PARAM_1&, decltype(std::ignore)&>(first, std::ignore);
}

template <class T1, class T2>
template <class PARAM_2,
          typename bsl::enable_if<bsl::is_convertible<T2, PARAM_2>::value,
                        bool>::type>
inline
pair<T1, T2>::operator std::tuple<decltype(std::ignore)&, PARAM_2&>()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::tuple<decltype(std::ignore)&, PARAM_2&>(std::ignore, second);
}
#endif


template <class T1, class T2>
inline
#if defined(BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE)
void pair<T1, T2>::swap(pair& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(Pair_ImpUtil::hasNothrowSwap<T1, T2>())
#else
void pair<T1, T2>::swap(pair& other)
      BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(bsl::is_nothrow_swappable<T1>::value
                                       && bsl::is_nothrow_swappable<T2>::value)
#endif
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
BSLS_KEYWORD_CONSTEXPR
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return ! (lhs == rhs);
}
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class T1, class T2>
BSLS_KEYWORD_CONSTEXPR
std::common_comparison_category_t<
    BloombergLP::bslalg::SynthThreeWayUtil::Result<T1>,
    BloombergLP::bslalg::SynthThreeWayUtil::Result<T2>
> operator<=>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    using BloombergLP::bslalg::SynthThreeWayUtil;
    auto result = SynthThreeWayUtil::compare(lhs.first, rhs.first);
    return result == 0 ? SynthThreeWayUtil::compare(lhs.second, rhs.second)
                       : result;
}

#else

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return (lhs.first < rhs.first ? true  :
            rhs.first < lhs.first ? false :
            lhs.second < rhs.second);
}

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return rhs < lhs;
}

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return ! (rhs < lhs);
}

template <class T1, class T2>
inline
BSLS_KEYWORD_CONSTEXPR
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{
    return ! (lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class T1, class T2>
inline
#if defined(BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE)
void
#else
typename bsl::enable_if<bsl::is_swappable<T1>::value
                     && bsl::is_swappable<T2>::value>::type
#endif
swap(pair<T1, T2>& a, pair<T1, T2>& b)
                       BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(noexcept(a.swap(b)))
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
namespace BloombergLP {
namespace bslstl {

                         // ---------------------
                         // class Pair_GetImpUtil
                         // ---------------------

// CLASS METHODS
template <class T1, class T2>
inline
T1& Pair_GetImpUtil<0, T1, T2>::getPairElement(bsl::pair<T1, T2>& p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return p.first;
}

template <class T1, class T2>
inline
const T1&
Pair_GetImpUtil<0, T1, T2>::getPairElement(const bsl::pair<T1, T2>& p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return p.first;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
inline
T1&& Pair_GetImpUtil<0, T1, T2>::getPairElement(bsl::pair<T1, T2>&&  p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::move(p.first);
}

template <class T1, class T2>
inline
const T1&&
Pair_GetImpUtil<0, T1, T2>::getPairElement(const bsl::pair<T1, T2>&&  p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::move(p.first);
}
#endif

template <class T1, class T2>
inline
T2& Pair_GetImpUtil<1u, T1, T2>::getPairElement(bsl::pair<T1, T2>& p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return p.second;
}

template <class T1, class T2>
inline
const T2&
Pair_GetImpUtil<1u, T1, T2>::getPairElement(const bsl::pair<T1, T2>& p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return p.second;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class T1, class T2>
inline
T2&& Pair_GetImpUtil<1u, T1, T2>::getPairElement(bsl::pair<T1, T2>&&  p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::move(p.second);
}

template <class T1, class T2>
inline
const T2&&
Pair_GetImpUtil<1u, T1, T2>::getPairElement(const bsl::pair<T1, T2>&&  p)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return std::move(p.second);
}

#endif

}  // close package namespace
}  // close enterprise namespace

// FREE FUNCTIONS
template<std::size_t INDEX, class T1, class T2>
inline
typename std::tuple_element<INDEX, bsl::pair<T1, T2> >::type&
bsl::get(bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<INDEX, T1, T2>::getPairElement(
                                                                            p);
}

template<std::size_t INDEX, class T1, class T2>
inline
const typename std::tuple_element<INDEX, bsl::pair<T1, T2> >::type&
bsl::get(const bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<INDEX, T1, T2>::getPairElement(
                                                                            p);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template<std::size_t INDEX, class T1, class T2>
inline
typename std::tuple_element<INDEX, bsl::pair<T1, T2> >::type&&
bsl::get(bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<INDEX, T1, T2>::getPairElement(
                                                                 std::move(p));
}
#endif

template<class TYPE, class T1, class T2>
inline
TYPE& bsl::get(bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<
        BloombergLP::bslstl::Pair_IndexOfType<TYPE, T1, T2>::value, T1, T2>
                                                           ::getPairElement(p);
}

template<class TYPE, class T1, class T2>
inline
const TYPE& bsl::get(const bsl::pair<T1, T2>& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<
        BloombergLP::bslstl::Pair_IndexOfType<TYPE, T1, T2>::value, T1, T2>
                                                           ::getPairElement(p);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template<class TYPE, class T1, class T2>
inline
TYPE&& bsl::get(bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<
        BloombergLP::bslstl::Pair_IndexOfType<TYPE, T1, T2>::value, T1, T2>
            ::getPairElement(std::move(p));
}

template<class TYPE, class T1, class T2>
inline
const TYPE&& bsl::get(const bsl::pair<T1, T2>&& p) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslstl::Pair_GetImpUtil<
        BloombergLP::bslstl::Pair_IndexOfType<TYPE, T1, T2>::value, T1, T2>
            ::getPairElement(std::move(p));
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

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

#ifdef BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE
#undef BSLSTL_PAIR_DO_NOT_SFINAE_TEST_IS_SWAPPABLE
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
