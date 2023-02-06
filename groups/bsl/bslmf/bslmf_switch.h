// bslmf_switch.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_SWITCH
#define INCLUDED_BSLMF_SWITCH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time 'switch' meta-function.
//
//@CLASSES:
//  bslmf::Switch:  'switch' meta-function (variable number of types)
//  bslmf::Switch2: 'switch' meta-function (among two types)
//  bslmf::Switch3: 'switch' meta-function (among three types)
//  bslmf::Switch4: 'switch' meta-function (among four types)
//  bslmf::Switch5: 'switch' meta-function (among five types)
//  bslmf::Switch6: 'switch' meta-function (among six types)
//  bslmf::Switch7: 'switch' meta-function (among seven types)
//  bslmf::Switch8: 'switch' meta-function (among eight types)
//  bslmf::Switch9: 'switch' meta-function (among nine types)
//
//@SEE_ALSO: bslmf_typelist
//
//@DESCRIPTION: This component provides a compile-time 'switch' meta-function.
// Its main class, 'bslmf::Switch', parameterized by an integral
// 't_SWITCH_SELECTOR' and a variable number 'N' of types, 't_T0' up to
// 'T{N - 1}', contains a single type named 'Type', which is the result of the
// meta-function and is an alias to 'T{t_SWITCH_SELECTOR}' or to 'bslmf::Nil'
// if 't_SWITCH_SELECTOR' is outside the range '[ 0 .. N - 1 ]'.  The analogy
// between the following "meta-code" and its valid C++ version using
// 'bslmf::Switch' may serve as a useful mental picture to understand and
// memorize the usage of this component.
//..
//  "Meta-code" (not C++)               Valid C++ using 'bslmf::Switch'
//  ---------------------               ------------------------------
//  typedef                             typedef typename
//    switch (t_SWITCH_SELECTOR) {             bslmf::Switch<t_SWITCH_SELECTOR,
//      case 0:     t_T0;                                      t_T0,
//      case 1:     t_T1;                                      t_T1,
//      // . . .                                             // . . .
//      case N - 1: T{N - 1};                                T{N - 1}
//      default:    bslmf::Nil;                              >
//    }                                                      ::
//                  Type;                                      Type;
//..
// Note the use of the keyword 'typename', necessary *only* if one or more of
// the 't_SWITCH_SELECTOR' or 't_T0' up to 'T{N - 1}' is dependent on a
// template parameter of the local context (i.e., that of the block using
// 'bslmf::Switch').  In particular, it should be omitted if the
// 'bslmf::Switch' is not used within a class or function template, as in the
// usage example below.
//
// For most situations, the number 'N' of template type arguments is known and
// the 'bslmf::SwitchN' meta-functions, which take exactly the indicated number
// of arguments, should be preferred.  Their usage leads to shorter mangled
// symbol names in object files (e.g., no extra defaulted template type
// arguments are included in the name), and shorter compilation times, as well.
//
///Usage
///-----
// Assume an external server API for storing and retrieving data:
//..
//  class data_Server {
//      // Dummy implementation of data server
//
//      int d_data;
//
//    public:
//       void store(char  data) { d_data = data | 0Xefface00; }
//       void store(short data) { d_data = data | 0Xdead0000; }
//       void store(int   data) { d_data = data; }
//
//       void retrieve(char  *data) {
//          *data = static_cast<char>(d_data & 0x000000ff);
//       }
//       void retrieve(short *data) {
//          *data = static_cast<short>(d_data & 0x0000ffff);
//       }
//       void retrieve(int   *data) { *data = d_data; }
//  };
//..
// In our application, we need some very small (1, 2, and 4-byte),
// special-purpose string types, so we create the following 'ShortString' class
// template:
//..
//  template <int LEN>
//  class ShortString {
//      // Store a short, fixed-length string.
//
//      char d_buffer[LEN];
//
//    public:
//      ShortString(const char *s = "") { std::strncpy(d_buffer, s, LEN); }
//          // Construct a 'ShortString' from a NTCS.
//
//      void retrieve(data_Server *server);
//          // Retrieve this string from a data server.
//
//      void store(data_Server *server) const;
//          // Store this string to a data server.
//
//      char operator[](int n) const { return d_buffer[n]; }
//          // Return the nth byte in this string.
//  };
//
//  template <int LEN>
//  bool operator==(const ShortString<LEN>& lhs, const ShortString<LEN>& rhs)
//      // Return true if a 'lhs' is equal to 'rhs'
//  {
//      return 0 == std::memcmp(&lhs, &rhs, LEN);
//  }
//
//  template <int LEN>
//  bool operator==(const ShortString<LEN>& lhs, const char *rhs)
//      // Return true if a 'ShortString' 'lhs' is equal to a NTCS 'rhs'.
//  {
//      int i;
//      for (i = 0; LEN > i && lhs[i]; ++i) {
//          if (lhs[i] != rhs[i]) {
//              return false;
//          }
//      }
//
//      return ('\0' == rhs[i]);
//  }
//..
// We would like to store our short strings in the data server, but the data
// server only handles 'char', 'short' and 'int' types.  Since our strings fit
// into these simple types, we can transform 'ShortString' into these integral
// types when calling 'store' and 'retrieve', using 'bslmf::Switch' to choose
// which integral type to use for each 'ShortString' type:
//..
//  template <int LEN>
//  void ShortString<LEN>::retrieve(data_Server *server)
//  {
//      // 'transferType will be 'char' if 'LEN' is 1, 'short' if 'LEN' is 2,
//      // and 'int' if 'LEN' 4.  Will choose 'void' and thus not compile if
//      // 'LEN' is 0 or 3.
//
//      typedef typename
//         bslmf::Switch<LEN, void, char, short, void, int>::Type transferType;
//
//      transferType x = 0;
//      server->retrieve(&x);
//      std::memcpy(d_buffer, &x, LEN);
//  }
//
//  template <int LEN>
//  void ShortString<LEN>::store(data_Server *server) const
//  {
//      // 'transferType will be 'char' if 'LEN' is 1, 'short' if 'LEN' is 2,
//      // and 'int' if 'LEN' 4.  Will choose 'void' and thus not compile if
//      // 'LEN' is 0 or 3.
//      typedef typename
//         bslmf::Switch<LEN, void, char, short, void, int>::Type transferType;
//
//      transferType x = 0;
//      std::memcpy(&x, d_buffer, LEN);
//      server->store(x);
//  }
//..
// In our main program, we first assert our basic assumptions, then we store
// and retrieve strings using our 'ShortString' template.
//..
//  int main()
//  {
//      assert(2 == sizeof(short));
//      assert(4 == sizeof(int));
//
//      data_Server server;
//
//      ShortString<1> a("A");
//      ShortString<1> b("B");
//      assert(a == "A");
//      assert(b == "B");
//      assert(! (a == b));
//
//      a.store(&server);
//      b.retrieve(&server);
//      assert(a == "A");
//      assert(b == "A");
//      assert(a == b);
//
//      ShortString<2> cd("CD");
//      ShortString<2> ef("EF");
//      assert(cd == "CD");
//      assert(ef == "EF");
//      assert(! (cd == ef));
//
//      cd.store(&server);
//      ef.retrieve(&server);
//      assert(cd == "CD");
//      assert(ef == "CD");
//      assert(cd == ef);
//
//      ShortString<4> ghij("GHIJ");
//      ShortString<4> klmn("KLMN");
//      assert(ghij == "GHIJ");
//      assert(klmn == "KLMN");
//      assert(! (ghij == klmn));
//
//      ghij.store(&server);
//      klmn.retrieve(&server);
//      assert(ghij == "GHIJ");
//      assert(klmn == "GHIJ");
//      assert(ghij == klmn);
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bslmf_nil.h>

#include <bsls_compilerfeatures.h>

#include <stddef.h>


#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#define BSLMF_SWITCH_USING_VARIADIC_TEMPLATES
    // This macro indicates that we have all the needed features for an
    // implementation of the type-switch facility that is source-compatible
    // with the pre-existing C++03 facility.  The main change is that the
    // numbered classes Switch0-Switch9 are no longer distinct classes, but
    // aliases of specific instantiations of the primary Switch template.
    // Eventually these partial template specializations will be eliminated,
    // when the individually named members are no longer used throughout the
    // whole of the Bloomberg codebase.
#endif

namespace BloombergLP {

namespace bslmf {

#if defined(BSLMF_SWITCH_USING_VARIADIC_TEMPLATES)
template <size_t t_SWITCH_SELECTOR, class... t_TYPES>
struct Switch {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // types 't_T0' up to 't_T9', provides a single type alias, 'Type', which
    // resolves, through specialization for a particular value 'N' of
    // 't_SWITCH_SELECTOR', to the type 'TN', or to 'Nil' if
    // 't_SWITCH_SELECTOR' is negative or larger than the number of template
    // arguments provided for the types.

    typedef Nil Type;
        // This 'Type' is an alias to the parameterized 'TN', where 'N' is the
        // integral value of the parameterized 't_SWITCH_SELECTOR'.  Note that
        // the first type in the list corresponds to 't_T0', not 't_T1'.
};

// SPECIALIZATIONS
template <class t_T0, class... t_TYPES>
struct Switch<0u, t_T0, t_TYPES...> {
    typedef t_T0 Type;
};

template <class t_T0, class t_T1, class... t_TYPES>
struct Switch<1u, t_T0, t_T1, t_TYPES...> {
    typedef t_T1 Type;
};

template <class t_T0, class t_T1, class t_T2, class... t_TYPES>
struct Switch<2u, t_T0, t_T1, t_T2, t_TYPES...> {
    typedef t_T2 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3, class... t_TYPES>
struct Switch<3u, t_T0, t_T1, t_T2, t_T3, t_TYPES...> {
    typedef t_T3 Type;
};

template <class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class... t_TYPES>
struct Switch<4u, t_T0, t_T1, t_T2, t_T3, t_T4, t_TYPES...> {
    typedef t_T4 Type;
};

template <class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5,
          class... t_TYPES>
struct Switch<5u, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_TYPES...> {
    typedef t_T5 Type;
};

template <class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5,
          class    t_T6,
          class... t_TYPES>
struct Switch<6u, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_TYPES...> {
    typedef t_T6 Type;
};

template <class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5,
          class    t_T6,
          class    t_T7,
          class... t_TYPES>
struct Switch<7u, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_TYPES...> {
    typedef t_T7 Type;
};

template <class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5,
          class    t_T6,
          class    t_T7,
          class    t_T8,
          class... t_TYPES>
struct Switch<8u,
              t_T0,
              t_T1,
              t_T2,
              t_T3,
              t_T4,
              t_T5,
              t_T6,
              t_T7,
              t_T8,
              t_TYPES...> {
    typedef t_T8 Type;
};

template <class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5,
          class    t_T6,
          class    t_T7,
          class    t_T8,
          class    t_T9,
          class... t_TYPES>
struct Switch<9u,
              t_T0,
              t_T1,
              t_T2,
              t_T3,
              t_T4,
              t_T5,
              t_T6,
              t_T7,
              t_T8,
              t_T9,
              t_TYPES...> {
    typedef t_T9 Type;
};

template <size_t   t_SWITCH_SELECTOR,
          class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5,
          class    t_T6,
          class    t_T7,
          class    t_T8,
          class    t_T9,
          class    t_T10,
          class... t_TYPES>
struct Switch<t_SWITCH_SELECTOR,
              t_T0,
              t_T1,
              t_T2,
              t_T3,
              t_T4,
              t_T5,
              t_T6,
              t_T7,
              t_T8,
              t_T9,
              t_T10,
              t_TYPES...> {
    typedef
        typename Switch<t_SWITCH_SELECTOR - 10, t_T10, t_TYPES...>::Type Type;
};

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <size_t t_SWITCH_SELECTOR, class t_T0, class t_T1>
using Switch2 = Switch<t_SWITCH_SELECTOR, t_T0, t_T1>;

template <size_t t_SWITCH_SELECTOR, class t_T0, class t_T1, class t_T2>
using Switch3 = Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2>;

template <size_t   t_SWITCH_SELECTOR,
          class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3>
using Switch4 = Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3>;

template <size_t   t_SWITCH_SELECTOR,
          class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4>
using Switch5 = Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4>;

template <size_t   t_SWITCH_SELECTOR,
          class    t_T0,
          class    t_T1,
          class    t_T2,
          class    t_T3,
          class    t_T4,
          class    t_T5>
using Switch6 = Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5>;

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1,
          class  t_T2,
          class  t_T3,
          class  t_T4,
          class  t_T5,
          class  t_T6>
using Switch7 =
           Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6>;

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1,
          class  t_T2,
          class  t_T3,
          class  t_T4,
          class  t_T5,
          class  t_T6,
          class  t_T7>
using Switch8 =
     Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7>;

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1,
          class  t_T2,
          class  t_T3,
          class  t_T4,
          class  t_T5,
          class  t_T6,
          class  t_T7,
          class  t_T8>
using Switch9 = Switch<t_SWITCH_SELECTOR,
                       t_T0,
                       t_T1,
                       t_T2,
                       t_T3,
                       t_T4,
                       t_T5,
                       t_T6,
                       t_T7,
                       t_T8>;
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#else

                               // =============
                               // struct Switch
                               // =============

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1 = Nil,
          class  t_T2 = Nil,
          class  t_T3 = Nil,
          class  t_T4 = Nil,
          class  t_T5 = Nil,
          class  t_T6 = Nil,
          class  t_T7 = Nil,
          class  t_T8 = Nil,
          class  t_T9 = Nil>
struct Switch {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // types 't_T0' up to 't_T9', provides a single type alias, 'Type', which
    // resolves, through specialization for a particular value 'N' of
    // 't_SWITCH_SELECTOR', to the type 'TN', or to 'Nil' if
    // 't_SWITCH_SELECTOR' is negative or larger than the number of template
    // arguments provided for the types.

    typedef Nil Type;
        // This 'Type' is an alias to the parameterized 'TN', where 'N' is the
        // integral value of the parameterized 't_SWITCH_SELECTOR'.  Note that
        // the first type in the list corresponds to 't_T0', not 't_T1'.
};

// SPECIALIZATIONS
template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<0, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<1, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<2, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<3, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<4, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 4 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T4' as 'Type'.

    typedef t_T4 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<5, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 5 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T5' as 'Type'.

    typedef t_T5 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<6, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 6 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T6' as 'Type'.

    typedef t_T6 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<7, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 7 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T7' as 'Type'.

    typedef t_T7 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<8, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 8 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T8' as 'Type'.

    typedef t_T8 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8,
          class t_T9>
struct Switch<9, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8, t_T9> {
    // This specialization of 'Switch' for a value of 9 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T9' as 'Type'.

    typedef t_T9 Type;
};

                        // ==============
                        // struct Switch2
                        // ==============

template <size_t t_SWITCH_SELECTOR, class t_T0, class t_T1>
struct Switch2 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly two types 't_T0' and 't_T1', offers functionality identical to
    // 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0, class t_T1>
struct Switch2<0, t_T0, t_T1> {
    // This specialization of 'Switch2' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0, class t_T1>
struct Switch2<1, t_T0, t_T1> {
    // This specialization of 'Switch2' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

                        // ==============
                        // struct Switch3
                        // ==============

template <size_t t_SWITCH_SELECTOR, class t_T0, class t_T1, class t_T2>
struct Switch3 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly three types 't_T0' up to 't_T2', offers functionality identical
    // to 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0, class t_T1, class t_T2>
struct Switch3<0, t_T0, t_T1, t_T2> {
    // This specialization of 'Switch3' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0, class t_T1, class t_T2>
struct Switch3<1, t_T0, t_T1, t_T2> {
    // This specialization of 'Switch3' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0, class t_T1, class t_T2>
struct Switch3<2, t_T0, t_T1, t_T2> {
    // This specialization of 'Switch3' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

                               // ==============
                               // struct Switch4
                               // ==============

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1,
          class  t_T2,
          class  t_T3>
struct Switch4 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly four types 't_T0' up to 't_T3', offers functionality identical
    // to 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0, class t_T1, class t_T2, class t_T3>
struct Switch4<0, t_T0, t_T1, t_T2, t_T3> {
    // This specialization of 'Switch4' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3>
struct Switch4<1, t_T0, t_T1, t_T2, t_T3> {
    // This specialization of 'Switch4' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3>
struct Switch4<2, t_T0, t_T1, t_T2, t_T3> {
    // This specialization of 'Switch4' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3>
struct Switch4<3, t_T0, t_T1, t_T2, t_T3> {
    // This specialization of 'Switch4' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

                               // ==============
                               // struct Switch5
                               // ==============

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1,
          class  t_T2,
          class  t_T3,
          class  t_T4>
struct Switch5 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly five types 't_T0' up to 't_T4', offers functionality identical
    // to 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0, class t_T1, class t_T2, class t_T3, class t_T4>
struct Switch5<0, t_T0, t_T1, t_T2, t_T3, t_T4> {
    // This specialization of 'Switch5' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3, class t_T4>
struct Switch5<1, t_T0, t_T1, t_T2, t_T3, t_T4> {
    // This specialization of 'Switch5' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3, class t_T4>
struct Switch5<2, t_T0, t_T1, t_T2, t_T3, t_T4> {
    // This specialization of 'Switch5' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3, class t_T4>
struct Switch5<3, t_T0, t_T1, t_T2, t_T3, t_T4> {
    // This specialization of 'Switch5' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

template <class t_T0, class t_T1, class t_T2, class t_T3, class t_T4>
struct Switch5<4, t_T0, t_T1, t_T2, t_T3, t_T4> {
    // This specialization of 'Switch5' for a value of 4 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T4' as 'Type'.

    typedef t_T4 Type;
};

                               // ==============
                               // struct Switch6
                               // ==============

template <size_t t_SWITCH_SELECTOR,
          class  t_T0,
          class  t_T1,
          class  t_T2,
          class  t_T3,
          class  t_T4,
          class  t_T5>
struct Switch6 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly six types 't_T0' up to 't_T5', offers functionality identical to
    // 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5>
struct Switch6<0, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5> {
    // This specialization of 'Switch6' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5>
struct Switch6<1, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5> {
    // This specialization of 'Switch6' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5>
struct Switch6<2, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5> {
    // This specialization of 'Switch6' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5>
struct Switch6<3, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5> {
    // This specialization of 'Switch6' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5>
struct Switch6<4, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5> {
    // This specialization of 'Switch6' for a value of 4 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T4' as 'Type'.

    typedef t_T4 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5>
struct Switch6<5, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5> {
    // This specialization of 'Switch6' for a value of 5 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T5' as 'Type'.

    typedef t_T5 Type;
};

                               // ==============
                               // struct Switch7
                               // ==============

template <size_t t_SWITCH_SELECTOR,
          class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly seven types 't_T0' up to 't_T6', offers functionality identical
    // to
    // 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<0, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<1, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<2, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<3, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<4, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 4 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T4' as 'Type'.

    typedef t_T4 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<5, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 5 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T5' as 'Type'.

    typedef t_T5 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6>
struct Switch7<6, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6> {
    // This specialization of 'Switch7' for a value of 6 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T6' as 'Type'.

    typedef t_T6 Type;
};

                               // ==============
                               // struct Switch8
                               // ==============

template <size_t t_SWITCH_SELECTOR,
          class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly eight types 't_T0' up to 't_T7', offers functionality identical
    // to
    // 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4,
    //                            t_T5, t_T6, t_T7>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<0, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<1, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<2, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<3, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<4, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 4 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T4' as 'Type'.

    typedef t_T4 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<5, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 5 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T5' as 'Type'.

    typedef t_T5 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<6, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 6 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T6' as 'Type'.

    typedef t_T6 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7>
struct Switch8<7, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7> {
    // This specialization of 'Switch8' for a value of 7 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T7' as 'Type'.

    typedef t_T7 Type;
};

                               // ==============
                               // struct Switch9
                               // ==============

template <size_t t_SWITCH_SELECTOR,
          class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9 {
    // This meta-function, parameterized by an integral 't_SWITCH_SELECTOR' and
    // exactly nine types 't_T0' up to 't_T8', offers functionality identical
    // to
    // 'Switch<t_SWITCH_SELECTOR, t_T0, t_T1, t_T2, t_T3, t_T4,
    //                            t_T5, t_T6, t_T7, t_T8>'.

    typedef Nil Type;
};

// SPECIALIZATIONS
template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<0, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 0 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T0' as 'Type'.

    typedef t_T0 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<1, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 1 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T1' as 'Type'.

    typedef t_T1 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<2, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 2 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T2' as 'Type'.

    typedef t_T2 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<3, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 3 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T3' as 'Type'.

    typedef t_T3 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<4, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 4 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T4' as 'Type'.

    typedef t_T4 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<5, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 5 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T5' as 'Type'.

    typedef t_T5 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<6, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 6 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T6' as 'Type'.

    typedef t_T6 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<7, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 7 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T7' as 'Type'.

    typedef t_T7 Type;
};

template <class t_T0,
          class t_T1,
          class t_T2,
          class t_T3,
          class t_T4,
          class t_T5,
          class t_T6,
          class t_T7,
          class t_T8>
struct Switch9<8, t_T0, t_T1, t_T2, t_T3, t_T4, t_T5, t_T6, t_T7, t_T8> {
    // This specialization of 'Switch9' for a value of 8 of the parameterized
    // 't_SWITCH_SELECTOR' selects the parameterized 't_T8' as 'Type'.

    typedef t_T8 Type;
};
#endif

}  // close package namespace


#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY

// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_Switch
#undef bslmf_Switch
#endif
#define bslmf_Switch bslmf::Switch
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch2
#undef bslmf_Switch2
#endif
#define bslmf_Switch2 bslmf::Switch2
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch3
#undef bslmf_Switch3
#endif
#define bslmf_Switch3 bslmf::Switch3
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch4
#undef bslmf_Switch4
#endif
#define bslmf_Switch4 bslmf::Switch4
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch5
#undef bslmf_Switch5
#endif
#define bslmf_Switch5 bslmf::Switch5
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch6
#undef bslmf_Switch6
#endif
#define bslmf_Switch6 bslmf::Switch6
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch7
#undef bslmf_Switch7
#endif
#define bslmf_Switch7 bslmf::Switch7
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch8
#undef bslmf_Switch8
#endif
#define bslmf_Switch8 bslmf::Switch8
    // This alias is defined for backward compatibility.

#ifdef bslmf_Switch9
#undef bslmf_Switch9
#endif
#define bslmf_Switch9 bslmf::Switch9
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
