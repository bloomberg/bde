// bslmf_matchinteger.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_MATCHINTEGER
#define INCLUDED_BSLMF_MATCHINTEGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class supporting "do the right thing" dispatch.
//
//@CLASSES:
//  bslmf::MatchInteger: implicit conversion of integral types
//
//@SEE_ALSO: bslstl_deque, bslstl_string, bslstl_vector
//
//@AUTHOR: Pablo Halpern (phalpern), Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component defines a class, 'bslmf::MatchInteger', to which
// any integral type can be implicitly converted.  A class with that conversion
// property is needed to meet the certain requirements of the sequential
// standard containers.
//
// Sequential containers have several overloaded method templates that accept a
// pair of input iterators (e.g., constructors, 'insert' and 'append' methods),
// but which must *not* accept integral types (e.g., 'char', 'short', 'int').
// See "ISO/IEC 14882:2011 Programming Language C++" (see
// 'http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3242.pdf'),
// "Section 23.2.3 [sequence.reqmts]", paragraphs 14-15.  This requirement is
// informally known as the "do the right thing" clause.  See
// 'http://gcc.gnu.org/onlinedocs/libstdc++/ext/lwg-defects.html#438'.
//
// The convertibility of arguments to 'bslmf::MatchInteger' is used to dispatch
// calls with integral arguments to the appropriate methods.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: "Do the Right Thing" Dispatch
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have a container with two constructors:
//
//: o one constructor providing initialization with multiple copies of a single
//:   value (a "duplicate value constructor"), and
//:
//: o the other providing initialization via a pair of iterators (a "range
//:   constructor").
//
// There are common usage situations in which arguments meaningful to the
// former constructor are provided but where the compiler resolves the overload
// to the latter constructor.
//
// For example, the 'MyProblematicContainer' class outlined below provides two
// such constructors.  Note that each is atypically embellished with a
// 'message' parameter, allowing us to trace the call flow.
//..
//                      // ============================
//                      // class MyProblematicContainer
//                      // ============================
//
//  template <class VALUE_TYPE>
//  class MyProblematicContainer {
//
//      // ...
//
//    public:
//      // CREATORS
//      MyProblematicContainer(std::size_t        numElements,
//                             const VALUE_TYPE&  value,
//                             const char        *message);
//          // Create a 'MyProblematicContainer' object containing the
//          // specified 'numElements' of the specified 'value', and write to
//          // standard output the specified 'message'.
//
//      template <class INPUT_ITER>
//      MyProblematicContainer(INPUT_ITER  first,
//                             INPUT_ITER  last,
//                             const char *message);
//          // Create a 'MyProblematicContainer' object containing the values
//          // in the range starting at the specified 'first' iterator and
//          // ending immediately before the specified 'last' iterator of the
//          // parameterized 'INPUT_ITER' type, and write to standard output
//          // the specified 'message'.
//
//      // ...
//
//  };
//
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//                      // ============================
//                      // class MyProblematicContainer
//                      // ============================
//
//  // CREATORS
//  template <class VALUE_TYPE>
//  MyProblematicContainer<VALUE_TYPE>::MyProblematicContainer(
//                                              std::size_t        numElements,
//                                              const VALUE_TYPE&  value,
//                                              const char        *message)
//  {
//      assert(message);
//
//      (void)numElements;
//      (void)value;
//
//      printf("CTOR: duplicate value: %s\n", message);
//  }
//
//  template <class VALUE_TYPE>
//  template <class INPUT_ITER>
//  MyProblematicContainer<VALUE_TYPE>::MyProblematicContainer(
//                                                         INPUT_ITER  first,
//                                                         INPUT_ITER  last,
//                                                         const char *message)
//  {
//      assert(message);
//
//      (void)first;
//      (void)last;
//
//      printf("CTOR: range          : %s\n", message);
//  }
//..
// The problem with the 'MyProblematicContainer' class becomes manifest when
// we create several objects:
//..
//      const char                   input[] = "How now brown cow?";
//      MyProblematicContainer<char> initFromPtrPair(
//                                                input,
//                                                input + sizeof(input),
//                                                "Called with pointer pair.");
//
//      MyProblematicContainer<char> initFromIntAndChar(
//                                            5,
//                                            'A',
//                                            "Called with 'int' and 'char'.");
//
//      MyProblematicContainer<char> initFromIntAndInt(
//                                            5,
//                                            65, // 'A'
//                                            "Called with 'int' and 'int'.");
//..
// The trace messages from the constructors show that 'initFromIntAndInt',
// the third object, is created using the range constructor.  At best, this
// will fail to compile!
//..
//  CTOR: range          : Called with pointer pair.
//  CTOR: duplicate value: Called with 'int' and 'char'.
//  CTOR: range          : Called with 'int' and 'int'.
//..
// The arguments provided for the creation of 'initFromIntAndInt' resolve to
// the range constructor because the template match of two arguments of the
// same type ('int') is stronger than that found for the duplicate value
// constructor, which requires conversions.
//
// In many other situations, overloading resolution issues can be avoided by
// function renaming; however, as these are constructors, we do not have
// that option.
//
// Instead, we redesign our class ('MyContainer' is the redesigned class)
// so that the calls to the range constructor with two 'int' arguments
// (or pairs of matching integer types) are routed to the duplicate value
// constructor.
//
// First, before we can perform a duplicate value construction from the context
// of the range constructor, we must isolate the essential actions of that
// these constructors into non-creator methods.  We define two 'privateInit'
// methods corresponding to the two constructors.
//..
//                      // =================
//                      // class MyContainer
//                      // =================
//
//  template <class VALUE_TYPE>
//  class MyContainer {
//
//      // ...
//
//    private:
//      // PRIVATE MANIPULATORS
//      void privateInit(std::size_t        numElements,
//                       const VALUE_TYPE&  value,
//                       const char        *message);
//          // Initialize a 'MyProblematic' object containing the specified
//          // 'numElements' of the specified 'value', and write to standard
//          // output the specified 'message'.
//
//      template <class INPUT_ITER>
//      void privateInit(INPUT_ITER  first,
//                       INPUT_ITER  last,
//                       const char *message);
//          // Initialize a 'MyProblematic' object containing the values in the
//          // range starting at the specified 'first' and ending immediately
//          // before the specified 'last' iterators of the parameterized
//          // 'INPUT_ITER' type, and write to standard output the specified
//          // 'message'.
//..
// The implementation of these private methods shows that we have preserved the
// essential behavior of the 'MyProblematicContainer' constructors; nothing is
// actually constructed but the call is easily traced.
//..
//  // PRIVATE MANIPULATORS
//  template <class VALUE_TYPE>
//  void MyContainer<VALUE_TYPE>::privateInit(std::size_t        numElements,
//                                            const VALUE_TYPE&  value,
//                                            const char        *message)
//  {
//      assert(message);
//      (void) numElements;
//      (void) value;
//
//      printf("INIT: duplicate value: %s\n", message);
//  }
//  template <class VALUE_TYPE>
//  template <class INPUT_ITER>
//  void MyContainer<VALUE_TYPE>::privateInit(INPUT_ITER  first,
//                                            INPUT_ITER  last,
//                                            const char *message)
//  {
//      assert(message);
//      (void)first;
//      (void)last;
//
//      printf("INIT: range          : %s\n", message);
//  }
//..
// Next, we define two overloaded 'privateInitDispatch' methods, each taking
// two parameters (the last two) which serve no purpose other than guiding the
// overload resolution.
//
// One overload accepts a wide range of pameter types.  Note that last
// two argument an match 'bslmf::AnyType'.
//..
//      template <class INPUT_ITER>
//      void privateInitDispatch(INPUT_ITER      first,
//                               INPUT_ITER      last,
//                               const char     *message,
//                               bslmf::AnyType  ,
//                               bslmf::AnyType  );
//          // Initialize a 'MyProblematic' object containing the values in the
//          // range starting at the specified 'first' and ending immediately
//          // before the specified 'last' iterators of the parameterized
//          // 'INPUT_ITER' type, and write to standard output the specified
//          // 'message'.  The last two arguments are used only for overload
//          // resolution.
//..
// The other overload is more specialized: The penultimate parameter
// is 'bslmf::MatchInteger' (basic integral types accepted here), and
// the last parameter is a simple 'int'.
//..
//      template <class INTEGER_TYPE>
//      void privateInitDispatch(INTEGER_TYPE         numElements,
//                               INTEGER_TYPE         value,
//                               const char          *message,
//                               bslmf::MatchInteger  ,
//                               int                  );
//          // Initialize a 'MyProblematic' object containing the specified
//          // 'numElements' of the specified 'value', and write to standard
//          // output the specified 'message'.  The last two arguments are used
//          // only for overload resolution.
//..
// The implementation of the more general overload invokes the private
// manipulator that handles the range construction.
//..
//  template <class VALUE_TYPE>
//  template <class INPUT_ITER>
//  void MyContainer<VALUE_TYPE>::privateInitDispatch(INPUT_ITER      first,
//                                                    INPUT_ITER      last,
//                                                    const char     *message,
//                                                    bslmf::AnyType  ,
//                                                    bslmf::AnyType  )
//  {
//      privateInit(first, last, message);
//  }
//..
// While the implementation of the more specialized overload invokes the
// private manipulator that handles the duplicate value construction.
//..
//  template <class VALUE_TYPE>
//  template <class INTEGER_TYPE>
//  void MyContainer<VALUE_TYPE>::privateInitDispatch(
//                                          INTEGER_TYPE        numElements,
//                                          INTEGER_TYPE        value,
//                                          const char         *message,
//                                          bslmf::MatchIntger  ,
//                                          int                 )
//  {
//      (void)message;
//
//      privateInit(static_cast<std::size_t>(numElements),
//                  static_cast<VALUE_TYPE>(value),
//                  "via 'privateInitDispatch'");
//  }
//..
// Notice that a distinctive message is supplied to mark our passage through
// this dispatch method.
//
// The range constructor (problematic in our earlier class) calls the
// overloaded 'privateInitDispatch' method.  The use of hardcoded '0' in the
// last argument is an exact match for method expecting an 'int' as its last
// argument.  Thus, the overload resolves to that method provided the type of
// the penultimate argument, 'first', can be converted to
// 'bslmf::MatchInteger' (i.e., 'first' is an integral type).
//..
//  template <class VALUE_TYPE>
//  template <class INPUT_ITER>
//  MyContainer<VALUE_TYPE>::MyContainer(INPUT_ITER  first,
//                                       INPUT_ITER  last,
//                                       const char *message)
//  {
//      privateInitDispatch(first, last, message, first, 0);
//  }
//..
// Notice that this solution excludes iterators that themselves happen to have
// a conversion to 'int'.  Such types would require two user-defined
// conversions (iterator-to-integral type, then integral-type to
// 'bslmf::MatchInteger') which are disallowed by the C++ compiler.
//
// The implementation of the duplicate value constructor simply calls the the
// corresponding private manipulator.
//..
//  // CREATORS
//  template <class VALUE_TYPE>
//  MyContainer<VALUE_TYPE>::MyContainer(std::size_t        numElements,
//                                       const VALUE_TYPE&  value,
//                                       const char        *message)
//  {
//      privateInit(numElements, value, message);
//  }
//..
// Finally, we create three objects of 'MyContainer', using the same arguments
// has we used for the three 'MyProblematicContainer' objects.
//..
//      const char        input[] = "How now brown cow?";
//      MyContainer<char> initFromPtrPair(input,
//                                        input + sizeof(input),
//                                        "Called with pointer pair.");
//
//      MyContainer<char> initFromIntAndChar(5,
//                                           'A',
//                                           "Called with 'int' and 'char'.");
//
//      MyContainer<char> initFromIntAndInt(5,
//                                          65, // 'A'
//                                          "Called with 'int' and 'int'.");
//
//..
//  INIT: range          : Called with pointer pair.
//  INIT: duplicate value: Called with 'int' and 'char'.
//  INIT: duplicate value: via 'privateInitDispatch'
//..
// Notice that the duplicate value 'privateInit' method is called directly for
// the second object, but called via 'privateInitDispatch' for the third
// object.

// Prevent 'bslmf' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslmf_matchinteger.h> header can't be included directly in \
        BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // ==================
                        // class MatchInteger
                        // ==================

struct MatchInteger {
    // Use as formal parameter for functions where an integral type can be
    // confused with an iterator type.  This class will match any integral
    // value.  If a type has a user-defined conversion to integral value, this
    // will *not* match because passing such an object would require two
    // user-defined conversions.

    // CREATORS
    MatchInteger(int);                                              // IMPLICIT
        // Conversion constructor.  Does nothing.
};


// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
MatchInteger::MatchInteger(int)
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
