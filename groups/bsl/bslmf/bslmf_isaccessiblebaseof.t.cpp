// bslmf_isaccessiblebaseof.t.cpp                                     -*-C++-*-

#include <bslmf_isaccessiblebaseof.h>

#include <bslmf_integralconstant.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stdio.h>   // `printf`, `puts`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines metafunction `bslmf::IsAccessibleBaseOf`
// that determines whether one class is an unambiguous public base class of
// another.  Tests need to ensure that all pairs that have the base and
// derived relationship return `true`, and all others return `false`.
// Interesting cases include transitive base and derived relationships,
// multiple inheritance, `private` and `protected` inheritance, virtual
// inheritance, `final` classes, cv-qualified types, incomplete types, unions,
// and enumerations.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] BloombergLP::bslmf::IsAccessibleBaseOf::value
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 2] CONCERN: incomplete type in an overload set
// [ 3] CONCERN: out-of-line member template definition
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                    GLOBAL CLASS DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------

#if BSLS_COMPILERFEATURES_FULL_CPP11
    // Access checks through ambiguous, protected, or private inheritance
    // participate in SFINAE only in C++11 and later.
# define BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED
#endif

class Base {
};

class Derived : public Base {
};

#ifdef BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED
class PrivatelyDerived : private Base {
  public:
    bool isBaseTest()
    {
        Base *base = this;
        (void)base;
        return bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value;
    }
    static bool isBaseStaticTest()
    {
        PrivatelyDerived  pd;
        PrivatelyDerived *pdPtr   = &pd;
        Base             *basePtr = pdPtr;
        (void)basePtr;
        return bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value;
    }
};

class ProtectedlyDerived : protected Base {
  public:
    bool isBaseTest()
    {
        Base *base = this;
        (void)base;
        return bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value;
    }
    static bool isBaseStaticTest()
    {
        ProtectedlyDerived  pd;
        ProtectedlyDerived *pdPtr   = &pd;
        Base               *basePtr = pdPtr;
        (void)basePtr;
        return bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value;
    }
};

class DerivedOfProtectedlyDerived : private ProtectedlyDerived {
  public:
    bool isBaseTest()
    {
        Base *base = this;
        (void)base;
        return bslmf::IsAccessibleBaseOf<Base,
                                         DerivedOfProtectedlyDerived>::value;
    }
    static bool isBaseStaticTest()
    {
        DerivedOfProtectedlyDerived  pd;
        DerivedOfProtectedlyDerived *pdPtr   = &pd;
        Base                        *basePtr = pdPtr;
        (void)basePtr;
        return bslmf::IsAccessibleBaseOf<Base,
                                         DerivedOfProtectedlyDerived>::value;
    }
};

#endif  // BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED

class TransitivelyDerived : public Derived {
};

class OtherBase {
};

class MultipleDerived : public Base, public OtherBase {
};

class VirtualDiamondLeft : virtual public Base {
};

class VirtualDiamondRight : virtual public Base {
};

class VirtualDiamondTop : public VirtualDiamondLeft,
                          public VirtualDiamondRight {
};

class AmbiguousLeft : public Base {
};

class AmbiguousRight : public Base {
};

class AmbiguousTop : public AmbiguousLeft, public AmbiguousRight {
};

#if BSLS_COMPILERFEATURES_FULL_CPP11
class Final final {
};

class FinalDerived final : public Base {
};
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

union Union {
    int d_value;
};

union OtherUnion {
    int d_value;
};

enum Enum {
    e_ENUM
};

#if BSLS_COMPILERFEATURES_FULL_CPP11
enum FixedEnum : int {
    e_FIXED_ENUM
};

enum class ScopedEnum : int {
    e_SCOPED_ENUM
};
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

class Incomplete;

/// This metafunction provides a nested `type` only if the specified `t_COND`
/// is `true`.
template <bool t_COND>
struct EnableIfInt {
};

template <>
struct EnableIfInt<true> {
    typedef int type;
};

/// This metafunction wraps `bslmf::IsAccessibleBaseOf` so that its evaluation
/// is not in the immediate context of a constrained function template.
template <class t_TYPE>
struct IsNotDerivedFromBase
: bsl::integral_constant<bool,
                         !bslmf::IsAccessibleBaseOf<Base, t_TYPE>::value> {
};

/// This class has a converting constructor constrained on
/// `IsNotDerivedFromBase`, which is therefore considered whenever a function
/// taking `const ConstrainedConverter&` is in an overload set.
struct ConstrainedConverter {
    template <class t_TYPE>
    ConstrainedConverter(
            const t_TYPE&,
            typename EnableIfInt<IsNotDerivedFromBase<t_TYPE>::value>::type =
                                                                            0);
};

/// Return a reference to an `Incomplete` object.  Note that this function is
/// declared but not defined, and is used only in unevaluated operands to
/// produce an lvalue of incomplete type.
const Incomplete& makeIncomplete();

/// Return a value whose size identifies the selected overload: `int` for a
/// `ConstrainedConverter`, `char` for an `Incomplete`, and `short` for any
/// other argument.  Note that these functions are declared but not defined,
/// and are used only in unevaluated operands to form an overload set in which
/// one candidate is constrained on `bslmf::IsAccessibleBaseOf` with an
/// incomplete derived type.  Also note that the ellipsis overload is the worst
/// match, so it is selected only when the constraint removes the
/// `ConstrainedConverter` overload.
int   overloaded(const ConstrainedConverter&);
char  overloaded(const Incomplete&);
short overloaded(...);

/// This class template has a member function template, defined out of line,
/// whose return type is constrained on a conjunction of
/// `bslmf::IsAccessibleBaseOf` results, as members of `bsl::basic_string` are.
template <class t_TYPE1, class t_TYPE2>
struct OutOfLineConstrained {
    /// Return 1.  Note that this function participates in overload resolution
    /// only if neither `t_TYPE1` nor `t_TYPE2` is an unambiguous public base
    /// of the specified `t_OTHER`.
    template <class t_OTHER>
    typename EnableIfInt<
        !bslmf::IsAccessibleBaseOf<t_TYPE1, t_OTHER>::value &&
        !bslmf::IsAccessibleBaseOf<t_TYPE2, t_OTHER>::value>::type
    notDerived(const t_OTHER&) const;
};

template <class t_TYPE1, class t_TYPE2>
template <class t_OTHER>
typename EnableIfInt<
    !bslmf::IsAccessibleBaseOf<t_TYPE1, t_OTHER>::value &&
    !bslmf::IsAccessibleBaseOf<t_TYPE2, t_OTHER>::value>::type
OutOfLineConstrained<t_TYPE1, t_TYPE2>::notDerived(const t_OTHER&) const
{
    return 1;
}

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
namespace Example1 {
///Example 1: Base And Derived Classes
///- - - - - - - - - - - - - - - - - -
// Define a base class, a class publicly derived from it, and a class privately
// derived from it.
// ```
struct Base {};
struct Derived : Base {};
struct PrivatelyDerived : private Base {};
// ```
// Evaluate `bslmf::IsAccessibleBaseOf::value`.  Note that `Base` is a private
// base class of `PrivatelyDerived`, so `std::is_base_of` would evaluate `true`
// but `bslmf::IsAccessibleBaseOf` evaluates `false` unless building without
// full C++11 support, in which case a private base would cause a compilation
// error instead.
// ```
void example1()
{
    ASSERT((true  == bslmf::IsAccessibleBaseOf<Base,    Derived>::value));
    ASSERT((true  == bslmf::IsAccessibleBaseOf<Base,    Base>   ::value));
    ASSERT((true  == bslmf::IsAccessibleBaseOf<Derived, Derived>::value));
    ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, Base>   ::value));
#if BSLS_COMPILERFEATURES_FULL_CPP11
    ASSERT((false ==
            bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value));
#endif
}
// ```
}  // close namespace Example1

namespace Example2 {
///Example 2: Unrelated Classes
/// - - - - - - - - - - - - - -
// Define two classes that are not related to each other.
// ```
class Unrelated  {};
class Unrelated2 {};
// ```
// Evaluate `bslmf::IsAccessibleBaseOf::value`.
// ```
void example2()
{
    ASSERT((false ==
            bslmf::IsAccessibleBaseOf<Unrelated, Unrelated2>::value));
}
// ```
}  // close namespace Example2

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)verbose;              // suppress warning
    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);  // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");
        Example1::example1();
        Example2::example2();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCERN: OUT-OF-LINE MEMBER TEMPLATE DEFINITION
        //   Ensure that a member function template whose signature names
        //   `bslmf::IsAccessibleBaseOf` can be defined out of line.  Note that
        //   some compilers, notably Sun CC, fail to match such a definition
        //   when the trait has a dependent default template argument.
        //
        // Concerns:
        // 1. An out-of-line definition of a member function template whose
        //    return type is constrained on a conjunction of
        //    `bslmf::IsAccessibleBaseOf` results matches its declaration.
        //
        // 2. The constrained member function can be called.
        //
        // Plan:
        // 1. Define `OutOfLineConstrained::notDerived` out of line, and call
        //    it with arguments of types derived from neither `Base` nor
        //    `Derived`.  (C-1..2)
        //
        // Testing:
        //   CONCERN: out-of-line member template definition
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONCERN: OUT-OF-LINE MEMBER TEMPLATE DEFINITION"
                          "\n===============================================");

        OutOfLineConstrained<Base, Derived>        mX;
        const OutOfLineConstrained<Base, Derived>& X = mX;

        ASSERT(1 == X.notDerived(0));
        ASSERT(1 == X.notDerived(OtherBase()));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONCERN: INCOMPLETE TYPE IN AN OVERLOAD SET
        //   Ensure that an overload set remains valid when one candidate is
        //   constrained on `bslmf::IsAccessibleBaseOf` with an incomplete
        //   derived type and another candidate is the best match.
        //
        // Concerns:
        // 1. A function template constrained, through an intermediate trait,
        //    on `IsAccessibleBaseOf<Base, Incomplete>` is discarded or
        //    considered without a compilation error.
        //
        // 2. The constrained candidate is selected for complete arguments
        //    that satisfy its constraint.
        //
        // 3. The constrained candidate is discarded for an argument whose
        //    type is derived from `Base`.
        //
        // Plan:
        // 1. Call a function overloaded for `const ConstrainedConverter&`,
        //    `const Incomplete&`, and an ellipsis with an `Incomplete`
        //    lvalue, and verify by the size of the return type that the
        //    `const Incomplete&` overload is selected.  (C-1)
        //
        // 2. Call the same function with an `int` and with an `OtherBase`
        //    object, and verify by the size of the return type that the
        //    `const ConstrainedConverter&` overload is selected.  (C-2)
        //
        // 3. Call the same function with a `Base` object and with a `Derived`
        //    object, and verify by the size of the return type that the
        //    ellipsis overload is selected.  (C-3)
        //
        // Testing:
        //   CONCERN: incomplete type in an overload set
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONCERN: INCOMPLETE TYPE IN AN OVERLOAD SET"
                          "\n===========================================");

        ASSERT(sizeof(char)  == sizeof(overloaded(makeIncomplete())));

        ASSERT(sizeof(int)   == sizeof(overloaded(0)));
        ASSERT(sizeof(int)   == sizeof(overloaded(OtherBase())));

        ASSERT(sizeof(short) == sizeof(overloaded(Base())));
        ASSERT(sizeof(short) == sizeof(overloaded(Derived())));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING `bslmf::IsAccessibleBaseOf::value`
        //   Ensure that `IsAccessibleBaseOf::value` correctly identifies the
        //   base-derived relationship between classes which inherit publicly
        //   and not ambiguously.
        //
        // Concerns:
        // 1. `value` is `true` for base-derived related classes that inherit
        //    publicly, including transitive relationships, multiple
        //    inheritance relationships, virtual relationships, and `final`
        //    derived classes, and for a class and itself even if that class
        //    is incomplete.
        //
        // 2. `value` is `false` for unrelated classes and for a derived class
        //    tested as the base of its own base class.  This includes `final`
        //    classes.
        //
        // 3. With full C++11 support `value` is `false` for classes having
        //    ambiguous, private, or protected inheritance, even where that
        //    base is accessible.
        //
        // 4. Cv-qualifiers on either type are ignored.
        //
        // 5. `value` is `false` if either type is not a class type, including
        //    fundamental types, `void`, pointers, references, arrays, function
        //    types, and enumerations (including, in C++11 and later, scoped
        //    enumerations and enumerations having a fixed underlying type,
        //    paired with that underlying type).
        //
        // 6. `value` is `false` for an incomplete derived class that is not
        //    the same type as the base class ignoring cv-qualifiers, rather
        //    than causing a compilation error.
        //
        // 7. A union is treated as a class type, as for `bsl::is_class`, so
        //    `value` is `true` for a union and itself ignoring cv-qualifiers,
        //    unlike `bsl::is_base_of`, and `false` for distinct unions, or a
        //    union paired with a class or non-class type.
        //
        // 8. The metafunction derives from `bsl::true_type` if `value` is
        //    `true`, and from `bsl::false_type` otherwise.
        //
        // Plan:
        // 1. Assert the expected value of `IsAccessibleBaseOf::value` in all
        //    relevant configurations of types.  (C-1..7)
        //
        // 2. Assert the expected value of `IsAccessibleBaseOf::value` as
        //    returned from a member function and from a static member
        //    function of a class whose protected or private base is
        //    accessible there.  (C-3)
        //
        // 3. Initialize `bsl::true_type` and `bsl::false_type` objects from
        //    `IsAccessibleBaseOf` objects.  (C-8)
        //
        // Testing:
        //   BloombergLP::bslmf::IsAccessibleBaseOf::value
        // --------------------------------------------------------------------
        if (verbose) puts("\nTESTING `bslmf::IsAccessibleBaseOf::value`"
                          "\n==========================================");
        ASSERT((true == bslmf::IsAccessibleBaseOf<Base, Derived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, TransitivelyDerived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, MultipleDerived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<OtherBase, MultipleDerived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, VirtualDiamondLeft>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, VirtualDiamondRight>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, VirtualDiamondTop>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, AmbiguousLeft>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, AmbiguousRight>::value));

        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, Base>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Derived, Derived>::value));

        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Incomplete, Incomplete>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<const Incomplete,
                                          Incomplete>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Incomplete,
                                          volatile Incomplete>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<const Incomplete,
                                          const volatile Incomplete>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Incomplete, Base>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, Incomplete>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, const Incomplete>::value));

#if BSLS_COMPILERFEATURES_FULL_CPP11
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, FinalDerived>::value));
        ASSERT((true == bslmf::IsAccessibleBaseOf<Final, Final>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Final>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Final, Base>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<FinalDerived, Base>::value));
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

#ifdef BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value));
        ASSERT(
              (false ==
               bslmf::IsAccessibleBaseOf<Base,
                                         DerivedOfProtectedlyDerived>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, AmbiguousTop>::value));
#endif  // BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED

        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, OtherBase>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, Base>::value));

        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<const Base, Derived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, volatile Derived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<volatile Base,
                                          const volatile Derived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<const Base, const Base>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<const Derived, Base>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<int, Base>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, Derived>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, Incomplete>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Incomplete, int>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<void, Base>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, Derived>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, Incomplete>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Incomplete, void>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<Base *, Derived *>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Derived *>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base&, Derived&>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Derived&>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base&, Derived>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Derived[2]>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base[2], Derived>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Derived()>::value));
#if BSLS_COMPILERFEATURES_FULL_CPP11
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Derived&&>::value));
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

        ASSERT((false == bslmf::IsAccessibleBaseOf<Enum, Enum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int,  Enum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Enum, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, Enum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Enum, Base>::value));
#if BSLS_COMPILERFEATURES_FULL_CPP11
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<FixedEnum, FixedEnum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, FixedEnum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<FixedEnum, int>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<ScopedEnum, ScopedEnum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, ScopedEnum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<ScopedEnum, int>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<FixedEnum, ScopedEnum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, ScopedEnum>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<ScopedEnum, Base>::value));
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Union, OtherUnion>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base,  Union>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Union, Base>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int,   Union>::value));
        ASSERT((true  == bslmf::IsAccessibleBaseOf<Union, Union>::value));
        ASSERT((true  ==
                bslmf::IsAccessibleBaseOf<const Union, Union>::value));
        ASSERT((true  ==
                bslmf::IsAccessibleBaseOf<Union, volatile Union>::value));

        const bsl::true_type  t1 = bslmf::IsAccessibleBaseOf<Base, Derived>();
        const bsl::true_type  t2 = bslmf::IsAccessibleBaseOf<Base, Base>();
        const bsl::true_type  t3 =
                             bslmf::IsAccessibleBaseOf<const Base, Derived>();
        const bsl::false_type f1 = bslmf::IsAccessibleBaseOf<Derived, Base>();
        const bsl::false_type f2 = bslmf::IsAccessibleBaseOf<int, int>();
        (void)t1;
        (void)t2;
        (void)t3;
        (void)f1;
        (void)f2;

#ifdef BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED
        ASSERT(false == PrivatelyDerived().isBaseTest());
        ASSERT(false == ProtectedlyDerived().isBaseTest());
        ASSERT(false == DerivedOfProtectedlyDerived().isBaseTest());

        ASSERT(false == PrivatelyDerived::isBaseStaticTest());
        ASSERT(false == ProtectedlyDerived::isBaseStaticTest());
        ASSERT(false == DerivedOfProtectedlyDerived::isBaseStaticTest());
#endif  // BSLMF_ISACCESSIBLEBASEOF_PRIVATE_AND_AMBIGUOUS_BASE_SFINAE_SUPPORTED
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    if (testStatus > 0)
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
