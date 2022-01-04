// bdlb_scopeexit.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_SCOPEEXIT
#define INCLUDED_BDLB_SCOPEEXIT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a general-purpose proctor object for scope-exit logic.
//
//@CLASSES:
//  bdlb::ScopeExit: executes a function upon destruction
//  bdlb::ScopeExitAny: an alias to 'ScopeExit<bsl::function<void()>>'
//  bdlb::ScopeExitUtil: Modern C++ factory method for creating guards
//
//@MACROS:
//  BDLB_SCOPEEXIT_GUARD: creates a scope guard using an exit function
//  BDLB_SCOPEEXIT_PROCTOR: creates a scope proctor from name and exit function
//
//@SEE_ALSO:
// P0052R6 - Generic Scope Guard and RAII Wrapper for the Standard Library
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0052r6.pdf
//
//@DESCRIPTION:
// This component provides a class template mechanism 'bdlb::ScopeExit', that
// will invoke a client supplied function upon its destruction.  The component
// has been inspired by P0052R6 a since not-adopted ISO C++ Library proposal,
// but has been extended to support C++03, BDE methodology, and attributes.
// 'bdlb::ScopeExit' is intended to facilitate creating scoped-proctors
// (similar to those found in {'bslma'}) that run a user-specified exit
// function upon their destruction, unless 'release()'-d.  The primary purpose
// of such a proctor is to execute a guaranteed undo of some operation in case
// the complete chain of operations was not successful (exception thrown or
// early return).  The proctor may also be used as a guard to unconditionally
// run its exit function upon exiting a scope, however for such guards
// dedicated classes are highly recommended.  See also {Guard vs Proctor}.  In
// case of Modern C++ platform (C++11 or later) this component also defines a
// utility 'bdlb::ScopeExitUtil' containing a factory method for creating
// 'bdlb::ScopeExit' objects ('makeScopeExit').  This component also defines a
// type alias 'bdlb::ScopeExitAny' that may be used in C++03 code to create a
// proctor where the type of the exit function is not known (for example it is
// the result of a bind expression). Finally, this component defines two
// macros that hide the differences in creating 'bdlb::ScopeExit' objects under
// C++03 and Modern C++.  The first, 'BDLB_SCOPEEXIT_PROCTOR' creates a scope
// proctor with a given variable name and exit function.  The second,
// 'BDLB_SCOPEEXIT_GUARD' creates a scope guard variable of unspecified name
// given only an exit function argument.  See also {Guard vs Proctor}, and
// {C++03 Restrictions When Exit Function Type is Unknown}.
//
///Guard vs Proctor
///----------------
// Guard and Proctor are terminology used by BDE methodology (see below).
// Because 'bdlb::ScopeExit' is so general (its exit function is provided by
// its user) it can be used both as a Guard and as a Proctor.  Below are the
// BDE definitions followed by an explanation of how they apply to this
// component.
//
///Definitions
///- - - - - -
// A (scoped) Guard is an object that maintains control over an associated
// resource (often acquired on construction) and releases that control when the
// Guard is destroyed (typically at scope exit -- either normally or because an
// exception was thrown).  The guard may provide an explicit release method,
// but -- unlike a Proctor -- such a release method is not commonly used.
//
// A Proctor is a special kind of guard intended to restore a valid state under
// abnormal circumstances (e.g., a thrown exception), until a valid state is
// restored normally, after which the Proctor's responsibility is explicitly
// released by its client.  The Proctor must provide a mechanism to release the
// resource from management, and -- unlike a standard Guard -- its management
// responsibility is typically released prior to its destruction.
//
///Further Explanation
///- - - - - - - - - -
// A Guard controls the lifetime a resource, it owns it during the resource's
// complete lifetime, and its purpose is to free up that resource once the work
// that needed it is done.  Guards may provide a 'release' method for "off
// label use" in case the responsibility for freeing up the resource is
// transferred to some other code, but it is not its common use.
//
// A Proctor is commonly responsible for undoing changes when a locally
// non-recoverable error occurs.  Proctors do not necessarily clean up
// resources (although they might, as part of restoring a valid state).  When
// all goes well, Proctors do nothing, because when all the work is
// successfully completed the user code "deactivates" the Proctor (or Proctors)
// to "commit" the work by calling the 'release' method.
//
///C++03 Restrictions When Exit Function Type Is Unknown
///-----------------------------------------------------
// There are two restrictions when using this component with C++03 and both
// are related to missing core language features.
//
// The first restriction is related to 'bdlb::ScopeExit' being a move-only
// type, and although this class tries its best to emulate move semantics,
// returning an instance of 'bdlb::ScopeExit' from a factory function is just
// not feasible under C++03.  It is technically possible, but would require a
// lot of boilerplate code on the user side, making the use of the component
// way too cumbersome to use under C++03.
//
// The second restriction is caused by the absence of the type-deducting 'auto'
// keyword in C++03.  Although we can deduce the type of the exit function in
// the factory method, without the type deduction capabilities provided by
// 'auto' we cannot turn that return type into the type of a variable.
//
// For the above reasons we do not generate or use a factory method in C++03
// but rely on 'bsl::function<void()>' (as described below) to define a type
// that is able to store and call any exit function.
//
// In case the type of the exit function is known to the programmer (see
// {Example 2: Using a Scope Exit Guard In C++03}) it is possible to simply
// provide that type as the template argument for 'bdlb::ScopeExit'.  But if
// our exit function has an unknown type (such as is the result of a 'bind'
// expression) we are facing the same above two restrictions (see also
// {Example 3: Unknown Exit Function Type In C++03}).
//
// Given the C+003 restrictions the following design decisions have been made:
//: 1. Do not provide the utility and factory function when compiling with
//:    C++03.
//:
//: 2. Provide a type 'bdlb::ScopeExitAny' that is an alias to
//:    'bdlb::ScopeExit<bsl::function<void()> >'.  An instance of
//:    'bdlb::ScopeExitAny' can be created without having to name the type of
//:    the exit function it is constructed from because 'bsl::function'
//:    provides type erasure.  The downside is the runtime performance cost of
//:    the type erasure (virtual function call that inhibits inlining) and
//:    potential memory allocation for stored arguments - all of which are the
//:    cost of using 'bsl::function'.
//:
//: 3. Provide 'BDLB_SCOPEEXIT_PROCTOR' and 'BDLB_SCOPEEXIT_GUARD' macro that
//:    hide the difference between using a legacy C++03 'bdlb::ScopeExitAny',
//:    or Modern C++ 'auto' type deduction from the factory function call.
//:    The macros thereby will select the most efficient 'bdlb::ScopeExit' type
//:    that is feasible on the given compiler platform without having to
//:    specify the exact type of the exit function (the template parameter to
//:    'bdlb::ScopeExit').
//
///Memory Allocation and Relationship with BDE Allocators
///------------------------------------------------------
// The exit function is created as a member of the guard object, so no memory
// is allocated for storing it.  However, if the exit function copy or move
// constructors require memory allocation, that memory is supplied by the
// currently installed default allocator.  Currently this component does not
// support custom allocators.  The main use-case of a guard object is to be
// created on the stack and automatic variables are supposed to use the
// default allocator, therefore no allocator support was added.
//
///Usage Examples
///--------------
// This section illustrates intended use of this component.
//
///Example 1: Using a Scope Exit Proctor in Modern C++
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we assume a Modern C++ compiler supporting C++11 or later.
// Suppose we are creating a simple database that stores names and their
// associated addresses and we store the names and addresses in two separate
// tables.  While adding data, these tables may fail the insertion, in which
// case we need to roll back the already inserted data, such as if we inserted
// the address first, we need to remove it if insertion of the associated name
// fails.
//
// First, we emulate our database access with the following simple functions:
//..
//  int removedAddressId = 0;
//  int insertAddress(const char *address)
//  {
//      (void)address;
//      return (0 == removedAddressId ? 2 : 3);
//  }
//
//  int insertCustomer(const char *name, int addressId)
//  {
//      (void)name;
//      (void)addressId;
//      if (0 == removedAddressId) throw 5; // Simulate failure once
//      return 7;
//  }
//
//  void removeAddress(int id)
//  {
//      removedAddressId = id;
//  }
//..
// Next, we draw up our complex, customer-creating function signature:
//..
//  int addCustomer11(const char *name, const char *address)
//  {
//..
// Then we implement it, starting by inserting the address:
//..
//      const int addressId = insertAddress(address);
//..
// Our dummy function returns 42, indicating successful address insertion.
//
// Next, we create a proctor to remove the already inserted address if the name
// insertion fails:
//..
//      auto addressProctor = bdlb::ScopeExitUtil::makeScopeExit(
//                                         [=](){ removeAddress(addressId); });
//..
// Then, we attempt to insert the name:
//..
//      const int custId = insertCustomer(name, addressId);
//..
// As our dummy 'insertCustomer' function will fail first time (when
// 'removedAddressId' is zero) with an exception, and we exist this function to
// the caller's error handling 'catch' clause.  While exiting the function via
// the exception the local stack is unwound.  All local variables having
// non-trivial destructors get destroyed by calling them.  On this stack there
// is only 'addressProctor' with a destructor, and it calls its exist function,
// which will save our non-zero 'addressId' value into the global
// 'removedAddressId' variable.
//
// On a second call to this function, because 'removedAddressId' is now
// non-zero, 'insertCustomer' will not fail, and we continue execution here.
//
// Next, if the insertion succeeded we are done, so we need to release the
// proctor to make the address permanent, after which we can return the ID:
//..
//      addressProctor.release();
//
//      return custId;                                                // RETURN
//  }
//..
// Now we can verify that a first attempt to add a customer fails with the
// "right" exception and that 'removedAddressId' is the expected value:
//..
// bool seenException = false;
//  try {
//      addCustomer11("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
//  }
//  catch (int exceptionValue) {
//      assert(5 == exceptionValue);
//      seenException = true;
//  }
//  assert(seenException);
//  assert(2 == removedAddressId);
//..
// Finally we verify that calling 'addCustomer11' again succeeds with the right
// identifier returned, and that 'removedAddressId' does not change:
//..
//  assert(7 == addCustomer11("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
//  assert(2 == removedAddressId);
//..
//
///Example 2: Using a Scope Exit Guard in C++03
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are in the same situation as in the Modern C++ example, but we
// have to create a solution that supports C++03 as well.
//
// First, we have to hand-craft a functor that calls 'removeAddress' with a
// given ID because Because C++03 does not support lambdas:
//..
//  class RemoveAddress {
//      int d_id;  // The identifier of the address (row) to remove
//
//    public:
//      explicit RemoveAddress(int id)
//      : d_id(id)
//      {
//      }
//
//      void operator()() const
//      {
//          removeAddress(d_id);
//      }
//  };
//..
// Then, we implement the add customer function for C++03:
//..
//  int addCustomer03(const char *name, const char *address)
//  {
//      const int addressId = insertAddress(address);
//..
// The code is almost the same code as was in 'addCustomer11' for Modern C++,
// except for the upcoming proctor variable definition.
//
// Next, we define the proctor variable with an explicitly spelled out type
// (that uses the functor type template argument), and a functor object
// initialized with the identifier of the address to remove:
//..
//      bdlb::ScopeExit<RemoveAddress> addrProctor((RemoveAddress(addressId)));
//..
// Notice the extra parenthesis we had to use to avoid "the most vexing parse"
// (https://en.wikipedia.org/wiki/Most_vexing_parse) issue.  Since we are in
// C++03, we cannot use (curly) brace initialization to avoid that issue.
//
// Now, we can complete the rest of the 'addCustomer03', which is exactly the
// same as the corresponding part of the modern 'addCustomer11' variation:
//..
//      const int custId = insertCustomer(name, addressId);
//      addrProctor.release();
//
//      return custId;                                                // RETURN
//  }
//..
// Finally, we can verify that both during the failing first attempt to add a
// customer to our imaginary database and the successful second attempt the
// 'RemoveAddress' functor based proctor works just as well as the lambda based
// modern variant did:
//..
//  removedAddressId = 0;
//  seenException = false;
//  try {
//      addCustomer03("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
//  }
//  catch (int exceptionValue) {
//      assert(5 == exceptionValue);
//      seenException = true;
//  }
//  assert(seenException);
//  assert(2 == removedAddressId);

//  assert(7 == addCustomer03("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
//  assert(2 == removedAddressId);
//..
//
/// Example 3: Unknown Exit Function Type In C++03
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we decide not to write a functor class for removing an address
// but use the function itself directly with 'bdlf::BindUtil::bind' and that
// way keep the roll-back-code near the rest like lambdas allow us in
// Modern C++, albeit with a less fortunate syntax.
//
// First, we design our bind expression as
// 'bdlf::BindUtil::bind(&removeAddress, addressId)'.
//
// Then, we can even try it to see if it works as intended by calling the
// result of a bind expression using a constant for the address ID:
//..
//  removedAddressId = 0;
//  bdlf::BindUtil::bind(&removeAddress, 11)();
//  assert(11 == removedAddressId);
//..
// Notice the subtle '()' after the bind expression.  We immediately call it
// after creating it (then destroy it).  We have to do it this way.  We have no
// idea what its type is so we cannot make a variable for it.
//
// Next, we create yet another customer adding function that differs only in
// its proctor definition from the Modern C++ variant:
//..
//  int addCustomerAny(const char *name, const char *address)
//  {
//      const int addressId = insertAddress(address);
//..
// Because we do not know the type of our exit function (it is "some functor
// object of some type", created by 'bind') we have to use the 'bsl::function'
// based 'bdlb::ScopeExitAny':
//..
//      bdlb::ScopeExitAny addressProctor(bdlf::BindUtil::bind(&removeAddress,
//                                                             addressId));
//..
// Consult {C++03 Restrictions When Exit Function Type Is Unknown} to be aware
// what additional runtime costs this more compact code has compared to a
// "hand made" functor with a known type.
//
// Note that since we have to take the address of a function to create the
// 'bind'-expression-functor we cannot use this format with standard library
// functions (unless taking their address is explicitly allowed by the C++
// standard), and if 'removeAddress' were an overloaded function the code would
// not compile as the compiler would not know which address we want.
//
// The rest of the function is the same and omitted for brevity.
//
// Finally, we can verify that 'bind' and 'bdlb::ScopeExitAny' based proctor
// works just as well:
//..
//  removedAddressId = 0;
//  seenException = false;
//  try {
//      addCustomerAny("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
//  }
//  catch (int exceptionValue) {
//      assert(5 == exceptionValue);
//      seenException = true;
//  }
//  assert(seenException);
//  assert(2 == removedAddressId);

//  assert(7 == addCustomerAny("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
//  assert(2 == removedAddressId);
//..
//
///Example 4: Using the Scope Exit Proctor Macro
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have to create portable code that will compile with C++03 as well
// as modern compilers.  We would like to keep things easy to remember, so we
// don't want to remember and write different, long macro names for the case
// when the type of the exit function is known, and when it is not.  But we
// also want our code to use the more efficient 'auto' and factory-method
// variation when compiled with modern C++, and only fall back to the slower
// 'bdlb::ScopeExitAny' solution on C++03 big iron compilers.
//
// We still need to use either functor ('RemoveAddress' in our examples) or a
// bind expression for the exit function because C++03 has no lambdas, so
// portable code cannot use them.  But we *can* chose the easy-to-use
// 'BDLB_SCOPEEXIT_PROCTOR' macro and not sprinkle the customer adder function
// with '#ifdef' to see which proctor definition to use.
//
// First, we start the add customer function as usual:
//..
//  int addCustomerMacro(const char *name, const char *address)
//  {
//      const int addressId = insertAddress(address);
//..
// Then, we define the proctor using a bind expression and the macro:
//..
//      BDLB_SCOPEEXIT_PROCTOR(proctor, bdlf::BindUtil::bind(&removeAddress,
//                                                           addressId));
//..
// Significantly less effort than creating code to chose between the faster
// running Modern C++ variation and the C++03 variation, and then also write
// both implementations at every proctor variable definition.
//
// Alternatively, we could have also written a functor and write the shorter
// 'BDLB_SCOPEEXIT_PROCTOR(proctor, RemoveAddress(addressId))' for the proctor.
//
// The rest of the function is the same and omitted for brevity.
//
// Finally, we can verify the easy proctor with the now customary code:
//..
//  removedAddressId = 0;
//  seenException = false;
//  try {
//      addCustomerMacro("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
//  }
//  catch (int exceptionValue) {
//      assert(5 == exceptionValue);
//      seenException = true;
//  }
//  assert(seenException);
//  assert(2 == removedAddressId);

//  assert(7 == addCustomerMacro("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
//  assert(2 == removedAddressId);
//..
//
///Example 5: Using the Scope Exit Guard Macro
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are writing a printing system that is capable of printing
// out dynamic data structures that can contain numbers, strings, arrays, maps,
// etc.  When printing out data we often have to print delimiters and find
// that it is really easy to forget to print the closing the delimiter.  So we
// look for a simple way to automate them.  We decide we don't want to change
// the printing of the opening delimiters, just have a way to automate the
// printing of the close delimiters without worry about early returns or
// 'break', 'continue', or other control flow changes.
//
// First, we create a functor type that prints closing delimiters:
//..
//  class CloseDelimPrinter {
//      const char *d_closingChars; // Using string literals for brevity
//
//    public:
//      explicit CloseDelimPrinter(const char *s)
//      : d_closingChars(s)
//      {
//      }
//
//      void operator()() const
//      {
//          outStream << d_closingChars; // To a fixed stream for brevity
//      }
//  };
//..
// Then, we can use the above functor and a scope exit guard to automate
// closing of delimiters in the printing functions:
//..
//  void printText(const bsl::string_view& text)
//  {
//..
// Although this function is very simplistic it serves only as a teaching tool.
//
// Next, we can move the printing of the opening delimiter and the closing one
// near each other in code, so it is clearly visible if an opened delimiter is
// closed:
//..
//      outStream << '"';  BDLB_SCOPEEXIT_GUARD(CloseDelimPrinter("\""));
//..
// 'BDLB_SCOPEEXIT_GUARD' provides a very simple way of doing this.  Notice
// that we did not need to provide a name for the guard variable, the macro
// gave it a unique name.  We did not need to worry about "the most vexing
// parse", the macro takes care of adding the extra pair of parenthesis.  We do
// not need to suppress an unused variable compiler warning, the macro does
// that.  And of course C++03 compatibility is included as well.
//
// What is the name of the guard variable?  It is unspecified.  Since this is a
// guard, meaning we do not want to 'release()' it (like a proctor), we do not
// need to know the name.  If you need to call 'release()', use the other
// macro: 'BDLB_SCOPEEXIT_PROCTOR'.
//
// Now, we can just print what goes inside the delimiters, and we are done:
//..
//      bsl::string_view::size_type pos = 0;
//      for(;;) {
//          const bsl::string_view::size_type end = text.find('"', pos);
//          outStream << text.substr(pos,
//                                   end != bsl::string_view::npos ?
//                                   end - pos :  end);
//          if (end == bsl::string_view::npos) {
//              break;                                                 // BREAK
//          }
//          outStream << "\\\"";
//          pos = end + 1;
//      }
//  }
//..
// Finally, we can print text and verify that it is indeed delimited:
//..
//  printText("simple text");
//  assert(outStreamContent() == "\"simple text\"");
//
//  printText("\"simple\" \"text\"");
//  assert(outStreamContent() == "\"\\\"simple\\\" \\\"text\\\"\"");
//..

#include <bdlscm_version.h>

#include <bsla_maybeunused.h>
#include <bsla_nodiscard.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_decay.h>
#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfunction.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_removepointer.h>
#include <bslmf_util.h>

#include <bsl_functional.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <bsl_type_traits.h>
#endif

#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

// ============================================================================
//                                 MACROS
// ============================================================================

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// 'BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L' is implied by
// 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'.
#define BDLB_SCOPEEXIT_USES_MODERN_CPP
#endif
    // This macro controls if C++03 form, or Modern C++ (C++11 or later) form.
    // When the macro is not defined (C++03 form) no utility 'struct'
    // (therefore no factory method) will be defined by this component and the
    // variable definition macros will use the type 'bdlb::ScopeExitAny'.  When
    // This macro is defined (Modern C++ form) the factory method and its
    // utility 'struct' will be defined and the variable definition macros will
    // use 'auto' to determine the type of the variable.  This macro is only
    // for internal use in this component header only and therefore it is
    // undefined at the end of the file.

#ifdef BDE_BUILD_TARGET_EXC
#define BDLB_SCOPEEXIT_NOEXCEPT_SPEC BSLS_KEYWORD_NOEXCEPT_SPECIFICATION
#else
#define BDLB_SCOPEEXIT_NOEXCEPT_SPEC(...)
#endif
    // This macro is for internal use only and is undefined at the end of the
    // file.

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
#define BDLB_SCOPEEXIT_PROCTOR(NAME, EXIT_FUNC) \
    auto NAME{BloombergLP::bdlb::ScopeExitUtil::makeScopeExit(EXIT_FUNC)}
#else
#define BDLB_SCOPEEXIT_PROCTOR(NAME, EXIT_FUNC) \
    BloombergLP::bdlb::ScopeExitAny NAME((EXIT_FUNC))
#endif
    // Create a local variable with the specified 'NAME' with a type that is an
    // instantiation of 'bdlb::ScopeExit' initialized with the specified
    // 'EXIT_FUNC'.  Note that the specific type of 'bdlb::ScopeExit' used will
    // depend on available language features.


#define BDLB_SCOPEEXIT_CAT(X, Y) BDLB_SCOPEEXIT_CAT_IMP(X, Y)
#define BDLB_SCOPEEXIT_CAT_IMP(X, Y) X##Y
    // Second layer needed to ensure that arguments are expanded before
    // concatenation.  This macro is for use by this component only.

#if defined(BSLS_PLATFORM_CMP_MSVC) || defined(BSLS_PLATFCORM_CMP_GNU) ||     \
    defined(BSLS_PLATFCORM_CMP_CLANG)
#define BDLB_SCOPEEXIT_UNIQNUM __COUNTER__
    // MSVC: The '__LINE__' macro breaks when '/ZI' is used (see Q199057 or
    // KB199057).  Fortunately the '__COUNTER__' extension provided by MSVC
    // is even better.  Since '__COUNTER__' is also available on our other
    // modern C++ platforms (GNU g++ and clang) we use it there as well.
#else
#define BDLB_SCOPEEXIT_UNIQNUM __LINE__
#endif
    // This macro is for use by 'BDLB_SCOPEEXIT_GUARD' only.

#define BDLB_SCOPEEXIT_GUARD(EXIT_FUNC)                                       \
    BSLA_MAYBE_UNUSED                                                         \
    const BDLB_SCOPEEXIT_PROCTOR(                                             \
        BDLB_SCOPEEXIT_CAT(bdlbScopeExitGuard_, BDLB_SCOPEEXIT_UNIQNUM),      \
                           EXIT_FUNC)
    // Create a local variable with a generated unique name and with a type
    // that is an instantiation of 'bdlb::ScopeExit' initialized with the
    // specified 'EXIT_FUNC'.  Note that the specific type of 'bdlb::ScopeExit'
    // used will depend on available language features.  The behavior is
    // undefined if this macro is used (expanded) more than once on the same
    // source line.  Note that using this macro in another macro more than once
    // is equivalent to using it more than once on the same source line.

// ============================================================================
//                                CLASS TYPES
// ============================================================================

namespace BloombergLP {
namespace bdlb {
                             // ===============
                             // class ScopeExit
                             // ===============

template <class EXIT_FUNC>
class BSLA_NODISCARD ScopeExit
    // 'ScopeExit' is a general-purpose scope guard class template that is
    // intended to use an an automatic (stack) variable and it calls an exit
    // function upon its destruction (when its scope is exited).
    //
    // The template argument 'EXIT_FUNC' shall be a function object type, or a
    // pointer to a function.  If 'EXIT_FUNC' is an object type, it shall
    // satisfy the requirements of *Destructible*, *Callable*, and
    // *MoveConstructible* as specified by the ISO C++ standard.  Note that to
    // fulfill the *MoveConstructible* constraint a type does not have to
    // implement a move constructor.  If it has a copy constructor, that will
    // work fine as long at the move constructor is not deleted (or in case
    // of C++03 emulated moves 'private').  The behavior is undefined if
    // calling (the member instance of) 'EXIT_FUNC' throws an exception (as it
    // will be called from the destructor).
{
  private:
    // TEMPLATE PARAMETER CONTRACT
    BSLMF_ASSERT(
        !bsl::is_pointer<EXIT_FUNC>::value
        || bsl::is_function<
                        typename bsl::remove_pointer<EXIT_FUNC>::type>::value);
        // Only function pointers are allowed, no other pointers.

    BSLMF_ASSERT(!bsl::is_reference<EXIT_FUNC>::value);
        // Reference types are not allowed, only objects.

    // We could check for for more, but 'is_destructible' needs full C++1
    // '<type_traits>' support, and 'is_invocable' is C++17 or later.  So we
    // skip checking these to avoid major conditional compilation clutter.  The
    // chance of passing non-destructible or non-callable 'EXIT_FUNC' argument
    // is low, and they will result in a reasonable error message.

    // PRIVATE DATA
    EXIT_FUNC d_exitFunction;          // A function pointer or functor to call
    bool      d_executeOnDestruction;  // 'false' if 'release' was called

  private:
    // NOT IMPLEMENTED
    ScopeExit(const ScopeExit&)            BSLS_KEYWORD_DELETED;  // No copying
    ScopeExit& operator=(const ScopeExit&) BSLS_KEYWORD_DELETED;  // No
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES            // assignment
    ScopeExit& operator=(ScopeExit&&)      BSLS_KEYWORD_DELETED;  // at all.
#endif

  public:
    // CREATORS
    template <class EXIT_FUNC_PARAM>
    explicit ScopeExit(
        BSLS_COMPILERFEATURES_FORWARD_REF(EXIT_FUNC_PARAM) function,
        typename bsl::enable_if<
            !bsl::is_same<ScopeExit<EXIT_FUNC>,
                          typename bsl::decay<EXIT_FUNC_PARAM>::type>::value &&
            bsl::is_convertible<EXIT_FUNC_PARAM,
                                EXIT_FUNC>::value>::type * = 0);
        // Create a 'ScopeExit' object, which, upon its destruction will invoke
        // the specified 'function' (or functor) unless its 'release' method
        // was called.  If 'function' is copied into the 'EXIT_FUNC' member,
        // and that copy throws an exception, invoke 'function' and rethrow the
        // exception.  If 'EXIT_FUNC_PARAM' cannot be move converted to
        // 'EXIT_FUNC' via no-throw means (either because such conversion does
        // not exist or it it not marked as non-throwing), 'function' will
        // always be copied into the member.  The behavior is undefined if
        // 'function', when called, throws an exception.  The behavior is also
        // undefined if calling the resulting member instance of 'EXIT_FUNC'
        // throws an exception.

    ScopeExit(bslmf::MovableRef<ScopeExit> original)
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_move_constructible<EXIT_FUNC>::value
                      || bsl::is_nothrow_copy_constructible<EXIT_FUNC>::value);
#else
       BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_move_constructible<EXIT_FUNC>::value);
#endif
        // If 'bsl::is_nothrow_move_constructible<EXIT_FUNC>::value' is 'true'
        // or 'EXIT_FUNC' is a move-only type, move construct, otherwise, copy
        // construct the exit function from the specified 'original'.  If
        // construction succeeds, call 'release()' on 'original'.

    ~ScopeExit() BSLS_KEYWORD_NOEXCEPT;
        // Destroy this object.  Execute the exit function unless 'release()'
        // has been called on this object.

  public:
    // MANIPULATORS
    void release();
        // Turn off the execution of the exit function of this object on
        // destruction.  The exact semantic meaning of releasing a
        // 'bdlb::ScopeExit' object depends on what its exit function does.
};

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

#ifndef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
// In case there is no reliable '<type_traits>' header we use 'bsl' a trait to
// mark our class no-throw-move-constructible, if the single member is.

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
template <class EXIT_FUNC>
struct is_nothrow_move_constructible<BloombergLP::bdlb::ScopeExit<EXIT_FUNC> >
: is_nothrow_move_constructible<EXIT_FUNC>
{};
}  // close 'bsl' namespace

// Reopen enterprise and package namespace
namespace BloombergLP {
namespace bdlb {

#endif

typedef ScopeExit<bsl::function<void()> > ScopeExitAny;
    // 'ScopeExitAny' is an alias to 'ScopeExit<bsl::function<void()> >',
    // effectively making it a polymorphic scope exit type.

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP

                             // ===================
                             // class ScopeExitUtil
                             // ===================

struct ScopeExitUtil {
    // A utility that provides a factory function for Modern C++ scope guards.
    // Notice that the utility type does not exist on C++03 systems.

    // CLASS METHODS
    template <class EXIT_FUNC>
    static
    ScopeExit<typename bsl::decay<EXIT_FUNC>::type>
    makeScopeExit(EXIT_FUNC&& function);
        // Return a 'ScopeExit' guard that has the specified 'function' as its
        // exit function.
};
#endif // BDLB_SCOPEEXIT_USES_MODERN_CPP

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class ScopeExit
                             // ---------------

// CREATORS
template <class EXIT_FUNC>
template <class EXIT_FUNC_PARAM>
inline
ScopeExit<EXIT_FUNC>::ScopeExit(
    BSLS_COMPILERFEATURES_FORWARD_REF(EXIT_FUNC_PARAM) function,
    typename bsl::enable_if<
        !bsl::is_same<ScopeExit<EXIT_FUNC>,
                      typename bsl::decay<EXIT_FUNC_PARAM>::type>::value &&
        bsl::is_convertible<EXIT_FUNC_PARAM, EXIT_FUNC>::value>::type *)
#ifdef BDE_BUILD_TARGET_EXC
try
#endif
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
: d_exitFunction(
      bslmf::Util::forward<
          typename bsl::conditional<
              bsl::is_nothrow_constructible<EXIT_FUNC,
                                            EXIT_FUNC_PARAM>::value ||
              !bsl::is_constructible<
                  EXIT_FUNC,
                  const typename bslmf::MovableRefUtil::RemoveReference<
                      EXIT_FUNC_PARAM>::type&
              >::value,
              EXIT_FUNC_PARAM,
              const typename bslmf::MovableRefUtil::RemoveReference<
                  EXIT_FUNC_PARAM>::type&
      >::type>(function))
#else  // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
: d_exitFunction(BSLS_COMPILERFEATURES_FORWARD(EXIT_FUNC_PARAM, function))
    // When we are unable to determine if it's safe to move the 'function'
    // parameter or not (based on its type, using type traits), we forward the
    // parameter in the usual manner, with the BDE-provided macro that picks
    // the best forwarding possible on the given compiler.  This prevents
    // compilation errors in case the functor is a move-only object.
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
, d_executeOnDestruction(true)
{
}
#ifdef BDE_BUILD_TARGET_EXC
catch (...)
{
    bslmf::MovableRefUtil::access(function)();
}
#endif

template <class EXIT_FUNC>
inline
ScopeExit<EXIT_FUNC>::ScopeExit(bslmf::MovableRef<ScopeExit> original)
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                          bsl::is_nothrow_move_constructible<EXIT_FUNC>::value
                       || bsl::is_nothrow_copy_constructible<EXIT_FUNC>::value)
#else
       BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                          bsl::is_nothrow_move_constructible<EXIT_FUNC>::value)
#endif
: d_exitFunction(bslmf::MovableRefUtil::move_if_noexcept(
                       bslmf::MovableRefUtil::access(original).d_exitFunction))
, d_executeOnDestruction(true)
{
    bslmf::MovableRefUtil::access(original).release();
}

template <class EXIT_FUNC>
inline
ScopeExit<EXIT_FUNC>::~ScopeExit() BSLS_KEYWORD_NOEXCEPT
{
    if (d_executeOnDestruction) {
        d_exitFunction();
    }
}

// MANIPULATORS
template <class EXIT_FUNC>
inline
void ScopeExit<EXIT_FUNC>::release()
{
    d_executeOnDestruction = false;
}

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP

                         // -------------------
                         // class ScopeExitUtil
                         // -------------------

// CLASS METHODS
template <class EXIT_FUNC>
inline
ScopeExit<typename bsl::decay<EXIT_FUNC>::type>
ScopeExitUtil::makeScopeExit(EXIT_FUNC&& function)
{
    return ScopeExit<typename bsl::decay<EXIT_FUNC>::type>(
                                    bslmf::Util::forward<EXIT_FUNC>(function));
}

#endif // BDLB_SCOPEEXIT_USES_MODERN_CPP

}  // close package namespace
}  // close enterprise namespace

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
#undef BDLB_SCOPEEXIT_USES_MODERN_CPP
#endif

#ifdef BDLB_SCOPEEXIT_NOEXCEPT_SPEC
#undef BDLB_SCOPEEXIT_NOEXCEPT_SPEC
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
