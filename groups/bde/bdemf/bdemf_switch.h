// bdemf_switch.h                  -*-C++-*-
#ifndef INCLUDED_BDEMF_SWITCH
#define INCLUDED_BDEMF_SWITCH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time 'switch' meta-function.
//
//@DEPRECATED: Use 'bslmf_switch' instead.
//
//@CLASSES:
//  bdemf_Switch:  'switch' meta-function (variable number of types)
//  bdemf_Switch2: 'switch' meta-function (among two types)
//  bdemf_Switch3: 'switch' meta-function (among three types)
//  bdemf_Switch4: 'switch' meta-function (among four types)
//  bdemf_Switch5: 'switch' meta-function (among five types)
//  bdemf_Switch6: 'switch' meta-function (among six types)
//  bdemf_Switch7: 'switch' meta-function (among seven types)
//  bdemf_Switch8: 'switch' meta-function (among eight types)
//  bdemf_Switch9: 'switch' meta-function (among nine types)
//
//@SEE_ALSO: bdemf_typelist
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a compile-time 'switch' meta-function.
// Its main class, 'bdemf_Switch', parameterized by an integral 'SELECTOR' and
// a variable number 'N' of types, 'T0' up to 'T{N - 1}', contains a single
// type named 'Type', which is the result of the meta-function and is an alias
// to 'T{SELECTOR}' or to 'bdemf_Nil' if 'SELECTOR' is outside the range
// '[ 0 .. N - 1 ]'.  The analogy between the following "meta-code" and its
// valid C++ version using 'bdemf_Switch' may serve as a useful mental picture
// to understand and memorize the usage of this component.
//..
//    "Meta-code" (not C++)               Valid C++ using 'bdemf_Switch'
//    ---------------------               ------------------------------
//    typedef                             typedef typename
//      switch (SELECTOR) {                       bdemf_Switch<SELECTOR,
//        case 0:     T0;                                      T0,
//        case 1:     T1;                                      T1,
//        // . . .                                             // . . .
//        case N - 1: T{N - 1};                                T{N - 1}
//        default:    bdemf_Nil;                              >
//      }                                                      ::
//                    Type;                                      Type;
//..
// Note the use of the keyword 'typename', necessary *only* if one or more of
// the 'SELECTOR' or 'T0' up to 'T{N - 1}' is dependent on a template parameter
// of the local context (i.e., that of the block using 'bdemf_Switch').  In
// particular, it should be omitted if the 'bdemf_Switch' is not used within a
// class or function template, as in the usage example below.
//
// For most situations, the number 'N' of template type arguments is known and
// the 'bdemf_SwitchN' meta-functions, which take exactly the indicated number
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
//       void retrieve(char  *data) { *data = d_data & 0x000000ff; }
//       void retrieve(short *data) { *data = d_data & 0x0000ffff; }
//       void retrieve(int   *data) { *data = d_data; }
//  };
//..
// In our application, we need some very small (1, 2, and 4-byte),
// special-purpose string types, so we create the following 'ShortString'
// class template:
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
// types when calling 'store' and 'retrieve', using 'bdemf_Switch5' to choose
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
//         bdemf_Switch5<LEN, void, char, short, void, int>::Type transferType;
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
//         bdemf_Switch5<LEN, void, char, short, void, int>::Type transferType;
//
//      transferType x = 0;
//      std::memcpy(&x, d_buffer, LEN);
//      server->store(x);
//  }
//..
// In our main program, we first assert our basic assumptions, then we
// store and retrieve strings using our 'ShortString' template.
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_SWITCH
#include <bslmf_switch.h>
#endif

#ifndef bdemf_Switch
#define bdemf_Switch   bslmf_Switch
    // This meta-function, parameterized by an integral 'SELECTOR' and
    // types 'T0' up to 'T9', provides a single type alias, 'Type', which
    // resolves, through specialization for a particular value 'N' of
    // 'SELECTOR', to the type 'TN', or to 'bslmf_Nil' if 'SELECTOR' is
    // negative or larger than the number of template arguments provided for
    // the types.
#endif

#ifndef bdemf_Switch1
#define bdemf_Switch1  bslmf_Switch1
#endif

#ifndef bdemf_Switch2
#define bdemf_Switch2  bslmf_Switch2
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // two types 'T0' and 'T1', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1>'.
#endif

#ifndef bdemf_Switch3
#define bdemf_Switch3  bslmf_Switch3
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // three types 'T0' up to 'T2', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2>'.
#endif

#ifndef bdemf_Switch4
#define bdemf_Switch4  bslmf_Switch4
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // four types 'T0' up to 'T3', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2, T3>'.
#endif

#ifndef bdemf_Switch5
#define bdemf_Switch5  bslmf_Switch5
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // five types 'T0' up to 'T4', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2, T3, T4>'.
#endif

#ifndef bdemf_Switch6
#define bdemf_Switch6  bslmf_Switch6
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // six types 'T0' up to 'T5', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2, T3, T4, T5>'.
#endif

#ifndef bdemf_Switch7
#define bdemf_Switch7  bslmf_Switch7
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // seven types 'T0' up to 'T6', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2, T3, T4, T5, T6>'.
#endif

#ifndef bdemf_Switch8
#define bdemf_Switch8  bslmf_Switch8
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // eight types 'T0' up to 'T7', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2, T3, T4, T5, T6, T7>'.
#endif

#ifndef bdemf_Switch9
#define bdemf_Switch9  bslmf_Switch9
    // This meta-function, parameterized by an integral 'SELECTOR' and exactly
    // nine types 'T0' up to 'T8', offers functionality identical to
    // 'bslmf_Switch<SELECTOR, T0, T1, T2, T3, T4, T5, T6, T7, T8>'.
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

