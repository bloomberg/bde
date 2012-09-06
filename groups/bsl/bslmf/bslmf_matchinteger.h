// bslmf_matchinteger.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_MATCHINTEGER
#define INCLUDED_BSLMF_MATCHINTEGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class supporting "do-the-right-thing clause" dispatch.
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
// property is useful for meeting the certain requirements of the standard
// sequential containers (e.g., 'bsl::vector', 'bsl::deque', 'bsl::string').
//
// Sequential containers have several overloaded method templates that accept a
// pair of input iterators (e.g., constructors, 'insert' and 'append' methods),
// but which must *not* accept integral types (e.g., 'char', 'short', 'int').
// See "ISO/IEC 14882:2011 Programming Language C++" (see
// 'http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3242.pdf'),
// "Section 23.2.3 [sequence.reqmts]", paragraphs 14-15.  This requirement is
// informally known as the "do-the-right-thing clause".  See
// 'http://gcc.gnu.org/onlinedocs/libstdc++/ext/lwg-defects.html#438'.
//
// The convertibility of arguments to 'bslmf::MatchInteger' is used to dispatch
// calls with integral arguments to the appropriate methods.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: "Do-the-Right-Thing" Clause Dispatch
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a container with two constructors:
//
//: o one constructor providing initialization with multiple copies of a single
//:   value (a "repeated value constructor"), and
//:
//: o the other providing initialization from a sequence of values delimited by
//:   a pair of iterators (a "range constructor").
//
// A naive implementation can result in common usage situations in which
// arguments meaningful to the former constructor are provided but where the
// compiler resolves the overload to the latter constructor.
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
//          // 'INPUT_ITER' type, and write to standard output the specified
//          // 'message'.
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
//      printf("CTOR: repeated value: %s\n", message);
//      // ...
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
//      printf("CTOR: range         : %s\n", message);
//      // ...
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
// Standard output shows:
//..
//  CTOR: range         : Called with pointer pair.
//  CTOR: repeated value: Called with 'int' and 'char'.
//  CTOR: range         : Called with 'int' and 'int'.
//..
// Notice that the range constructor, not the repeated value constructor, is
// invoked for the creation of 'initFromIntAndInt', the third object.
//
// The range constructor is chosen to resolve that overload because its match
// of two arguments of the same type ('int' in this usage) without conversion
// is better than that provided by the repeated value constructor, which
// requires conversions of two different arguments.
//
// If we are fortunate, range constructor code will fail to compile; otherwise,
// dereferencing integer values (i.e., using them as pointers) leads to
// undefined behavior.
//
// Note that, in many other situations, overloading resolution issues can be
// avoided by function renaming; however, as these are constructors, we do not
// have that option.
//
// Instead, we redesign our class ('MyContainer' is the redesigned class) so
// that the calls to the range constructor with two 'int' arguments (or pairs
// of the same integer types) are routed to the repeated value constructor.
// The 'bslmf::MatchInteger' class is used to distinguish between integer types
// and other types.
//
// First, we define the 'MyContainer' class to have constructors taking
// the same arguments as the constructors of 'MyProblematicContainer':
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
//    public:
//      // CREATORS
//      MyContainer(std::size_t        numElements,
//                  const VALUE_TYPE&  value,
//                  const char        *message);
//          // Create a 'MyProblematicContainer' object containing the
//          // specified 'numElements' of the specified 'value', and write to
//          // standard output the specified 'message'.
//
//      template <class INPUT_ITER>
//      MyContainer(INPUT_ITER  first, INPUT_ITER  last, const char *message);
//          // Create a 'MyProblematicContainer' object containing the values
//          // in the range starting at the specified 'first' and ending
//          // immediately before the specified 'last' iterators of the type
//          // 'INPUT_ITER', and write to standard output the specified
//          // 'message'.
//
//      // ...
//
// };
//..
// Then, we isolate the essential actions of our two constructors into two
// private, non-creator methods.  This allows us to achieve the results of
// either constructor, as appropriate, from the context of the range
// constructor.  The two 'privateInit*' methods are:
//..
//    private:
//      // PRIVATE MANIPULATORS
//      void privateInit(std::size_t        numElements,
//                       const VALUE_TYPE&  value,
//                       const char        *message);
//          // Initialize a 'MyContainer' object containing the specified
//          // 'numElements' of the specified 'value', and write to standard
//          // output the specified 'message'.
//
//      template <class INPUT_ITER>
//      void privateInit(INPUT_ITER  first,
//                       INPUT_ITER  last,
//                       const char *message);
//          // Initialize a 'MyContainer' object containing the values in the
//          // range starting at the specified 'first' and ending immediately
//          // before the specified 'last' iterators of the type 'INPUT_ITER',
//          // and write to standard output the specified 'message'.
//..
// Note that, as in the constructors of the 'MyProblematic' class, the
// 'privateInit*' methods provide display a message so we can trace the call
// path.
//..
//  // PRIVATE MANIPULATORS
//  template <class VALUE_TYPE>
//  void MyContainer<VALUE_TYPE>::privateInit(std::size_t        numElements,
//                                            const VALUE_TYPE&  value,
//                                            const char        *message)
//  {
//      assert(message);
//      printf("INIT: repeated value: %s\n", message);
//      // ...
//  }
//
//  template <class VALUE_TYPE>
//  template <class INPUT_ITER>
//  void MyContainer<VALUE_TYPE>::privateInit(INPUT_ITER  first,
//                                            INPUT_ITER  last,
//                                            const char *message)
//  {
//      assert(message);
//      printf("INIT: range         : %s\n", message);
//      // ...
//  }
//..
// Now, we define two overloaded 'privateInitDispatch' methods, each taking
// two parameters (the last two) which serve no run-time purpose.  As we shall
// see, they exist only to guide overload resolution at compile-time.
//..
//      template <class INTEGER_TYPE>
//      void privateInitDispatch(INTEGER_TYPE         numElements,
//                               INTEGER_TYPE         value,
//                               const char          *message,
//                               bslmf::MatchInteger  ,
//                               bslmf::Nil           );
//          // Initialize a 'MyContainer' object containing the specified
//          // 'numElements' of the specified 'value', and write to standard
//          // output the specified 'message'.  The last two arguments are used
//          // only for overload resolution.
//
//      template <class INPUT_ITER>
//      void privateInitDispatch(INPUT_ITER                   first,
//                               INPUT_ITER                   last,
//                               const char                  *message,
//                               BloombergLP::bslmf::AnyType  ,
//                               BloombergLP::bslmf::AnyType  );
//          // Initialize a 'MyContainer' object containing the values in the
//          // range starting at the specified 'first' and ending immediately
//          // before the specified 'last' iterators of the type 'INPUT_ITER',
//          // and write to standard output the specified 'message'.  The last
//          // two arguments are used only for overload resolution.
//..
// Notice that the first overload has strict requirements on the last two
// arguments, but the second overload (accepting 'bslmf::AnyType' in those
// positions) will match all contexts in which the first fails to match.
//
// Then, we implement the two 'privateInitDispatch' overloads so that
// each invokes a different overload of the 'privateInit' methods:
//
//: o The 'privateInit' corresponding to repeated value constructor is invoked
//:   from the "strict" overload of 'privateInitDispatch'.
//:
//: o The 'privateInit' for range construction is invoked from the other
//:   'privateInitDispatch' overload.
//..
//  template <class VALUE_TYPE>
//  template <class INTEGER_TYPE>
//  void MyContainer<VALUE_TYPE>::privateInitDispatch(
//                                          INTEGER_TYPE         numElements,
//                                          INTEGER_TYPE         value,
//                                          const char          *message,
//                                          bslmf::MatchInteger  ,
//                                          bslmf::Nil           )
//  {
//      (void)message;
//
//      privateInit(static_cast<std::size_t>(numElements),
//                  static_cast<VALUE_TYPE>(value),
//                  "Called via 'privateInitDispatch'.");
//  }
//
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
// Next, we use overloaded 'privateInitDispatch' method in the range
// constructor of 'MyContainer'.  Note that we always supply a 'bslmf::Nil'
// object (an exact type match) as the final argument, the choice of overload
// will be govered according to the type of 'first'.  Consequently, if 'first'
// is implicitly convertible to 'bslmf::MatchInteger', then the overload
// leading to repeated value construction is used; otherwise, the overload
// leading to range construction is used.
//..
//  template <class VALUE_TYPE>
//  template <class INPUT_ITER>
//  MyContainer<VALUE_TYPE>::MyContainer(INPUT_ITER  first,
//                                       INPUT_ITER  last,
//                                       const char *message)
//  {
//      privateInitDispatch(first, last, message, first, bslmf::Nil());
//  }
//..
// Notice that this design is safe for iterators that themselves happen to have
// a conversion to 'int'.  Such types would require two user-defined
// conversions, which are disallowed by the C++ compiler, to match the
// 'bslmf::MatchInteger' parameter of the "strict" 'privateInitDispatch'
// overload.
//
// Then, we implement the repeated value constructor using a direct call
// to the repeated value private initializer:
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
// as we used for the three 'MyProblematicContainer' objects.
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
//..
// Standard output shows:
//..
//  INIT: range         : Called with pointer pair.
//  INIT: repeated value: Called with 'int' and 'char'.
//  INIT: repeated value: Called via 'privateInitDispatch'.
//..
// Notice that the repeated value 'privateInit' method is called directly
// for the second object, but called via 'privateInitDispatch' for the
// third object.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // ==================
                        // class MatchInteger
                        // ==================
struct MatchInteger {
    // This copy-constructible mechanism can be used as a formal parameter for
    // functions where an integral type can be confused with an iterator type.
    // A copy-constructbile mechanism is needed so that such objects can be
    // used as function arguments.
    //
    // Note that if a type has a user-defined conversion to integral value,
    // this will *not* match because passing such an object would require two
    // user-defined conversions.

    // CREATORS
    MatchInteger(int);                                              // IMPLICIT
        // Conversion constructor.  Does nothing.

    //! MatchInteger(const MatchInteger&) = default
        // Create a 'MatchInteger' object.  Note that as 'MatchInteger' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~MatchInteger() = default;
        // Destroy this object.
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
