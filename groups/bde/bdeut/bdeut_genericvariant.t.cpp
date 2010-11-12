// bdeut_genericvariant.t.cpp    -*-C++-*-

#include <bdeut_genericvariant.h>

#include <bdema_testallocator.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace BloombergLP;
using std::cout;
using std::cerr;
using std::flush;
using std::endl;
using std::atoi;
using std::stringstream;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a generic value-semantic scalar whose state is
// identically its value.  The value of the generic object consists of the
// embedded object's type and value.  Note that the generic object may also
// contain no value, in which case its type is 'void'.
//
// Most of the test cases are pretty straightforward because the value-semantic
// operations are simply forwarded to the value-semantic operations for the
// contained type (which should be tested in their respective components).  We
// are only concerned that the arguments are passed correctly and the special
// cases (as documented in the component-level documentation) work as expected.
//
// Memory allocation must be tested for exception neutrality via the
// 'bdema_testallocator' component.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdeut_GenericVariant' object is created with a default 'void' type.  An
//   object's white-box state can be modified using the primary manipulators
//   'assign'.  The default constructor, in conjunction with the primary
//   manipulator, is sufficient to attain any achievable white-box state.
//
//       o bdeut_GenericVariant(bdema_Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   The 'assign' methods represent the minimal set of manipulators that can
//   attain any achievable white-box state.
//
//       o bdeut_GenericVariant& assign<TYPE>();
//       o bdeut_GenericVariant& assign<TYPE>(const TYPE& value);
//
// Basic Accessors:
//   The 'is', 'the', and 'typeInfo' methods represent the maximal set of
//   accessors that return the black-box representation of the object.
//
//       o bool is<TYPE>() const;
//       o const TYPE& the<TYPE>() const;
//       o const std::type_info& typeInfo() const;
//
// The following abbreviations are used throughout this documentation:
//
//   o b_A  -> bdema_Allocator
//   o b_GV -> bdeut_GenericVariant
//   o c    -> const
//
//-----------------------------------------------------------------------------
// CREATORS
// [  ] bdeut_GenericVariant(b_A *ba = 0);
// [  ] bdeut_GenericVariant(c b_GV& original, b_A *ba = 0);
// [  ] ~bdeut_GenericVariant();
//
// MANIPULATORS
// [  ] bdeut_GenericVariant& operator=(c b_GV& rhs);
// [  ] bdeut_GenericVariant& assign<TYPE>();
// [  ] bdeut_GenericVariant& assign<TYPE>(c TYPE& value);
// [  ] STREAM& bdexStreamIn<STREAM>(STREAM& stream, int version);
// [  ] bdeut_GenericVariant& reset();
// [  ] TYPE& the<TYPE>();
//
// ACCESSORS
// [  ] STREAM& bdexStreamOut<STREAM>(STREAM& stream, int version) const;
// [  ] bool is<TYPE>() const;
// [  ] int maxSupportedBdexVersion() const;
// [  ] std::ostream& print(std::ostream& stream, int, int) const;
// [  ] const TYPE& the<TYPE>() const;
// [  ] const std::type_info& typeInfo() const;
//
// FREE OPERATORS
// [  ] int operator==(const b_GV& lhs, const b_GV& rhs);
// [  ] int operator!=(const b_GV& lhs, const b_GV& rhs);
// [  ] std::ostream& operator<<(std::ostream& stream, const b_GV& rhs);
//
// TRAITS
// [  ] bdealg_TypeTraits<bdeut_GenericVariant>
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
// [ 3] int ggg(b_GV *object, const char *spec, int vF = 1);
// [ 3] b_GV& gg(b_GV *object, const char *spec);
// [ 8] b_GV g(const char *spec);
// ----------------------------------------------------------------------------

//=============================================================================
//                            SUN 5.2 WORKAROUNDS
//-----------------------------------------------------------------------------

#ifndef TEMPLATE_MEMBER_BUG

#if defined BDES_PLATFORM__CMP_SUN && BDES_PLATFORM__CMP_VERSION < 0x550
// This is a workaround for the bug in Sun compilers before version 5.5 where
// the compiler does not recognise template methods that do not use any of the
// template arguments in the method's argument list.  This bug only manifests
// itself when the object for which the templated method is invoked is accessed
// through a reference instead of accessed directly.  E.g., the following
// works:
//..
//    bdeut_GenericVariant obj;
//
//    // ... initialize 'obj' to an 'int' value ...
//
//    obj.the<int>();
//..
// However, the following will fail:
//..
//    bdeut_GenericVariant& OBJ = obj;
//
//    OBJ.the<int>();
//..
// The compiler treats data members of a class as references inside the methods
// of the class, so this bug also applies to data members of a class.  The
// 'TEMPLATE_MEMBER_BUG' macro below is used to work around this bug.  The
// workaround is to add 'template ' before the name of the templated function,
// like this:
//..
//    OBJ.template the<int>();
//..
#   define TEMPLATE_MEMBER_BUG template
#else
#   define TEMPLATE_MEMBER_BUG
#endif

#endif  // #ifndef TEMPLATE_MEMBER_BUG



//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int g_objectCount = 0;  // Global counter for number of objects
                               // currently constructed.  If this value is not
                               // zero when the test driver exits, that means
                               // the variant object did not construct or
                               // destroy the objects correctly.

                    // ===================================
                    // class TestType_PrintMethod_Template
                    // ===================================

template <typename TYPE>
class TestType_PrintMethod_Template {
    // This template is used to generate the 'TestType_PrintMethod' type, which
    // simulates a type that declares the 'bdeu_TypeTraitHasPrintMethod' trait.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_GenericVariant' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

    // PRIVATE DATA MEMBERS
    TYPE                  d_value;           // value that will be printed out
    mutable std::ostream *d_stream;          // 'stream' that was passed to the
                                             // 'print' method
    mutable int           d_level;           // 'level' that was passed to the
                                             // 'print' method
    mutable int           d_spacesPerLevel;  // 'spacesPerLevel' that was
                                             // passed to the 'print' method

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(TestType_PrintMethod_Template,
                                 bdeu_TypeTraitHasPrintMethod);

    // CREATORS
    TestType_PrintMethod_Template()
    : d_stream(0)
    , d_level(0)
    , d_spacesPerLevel(0)
    {
        ++g_objectCount;
    }

    TestType_PrintMethod_Template(
                                 const TestType_PrintMethod_Template& original)
    : d_value(original.d_value)
    , d_stream(original.d_stream)
    , d_level(original.d_level)
    , d_spacesPerLevel(original.d_spacesPerLevel)
    {
        ++g_objectCount;
    }

    TestType_PrintMethod_Template(const TYPE& value)
    : d_value(value)
    , d_stream(0)
    , d_level(0)
    , d_spacesPerLevel(0)
    {
        ++g_objectCount;
    }

    ~TestType_PrintMethod_Template()
    {
        --g_objectCount;
    }

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
                        int           level,
                        int           spacesPerLevel) const
    {
        d_stream         = &stream;
        d_level          = level;
        d_spacesPerLevel = spacesPerLevel;

        return stream;
    }

    const TYPE& value() const
    {
        return d_value;
    }

    std::ostream* stream() const
    {
        return d_stream;
    }

    int level() const
    {
        return d_level;
    }

    int spacesPerLevel() const
    {
        return d_spacesPerLevel;
    }
};

template <typename TYPE>
int operator==(const TestType_PrintMethod_Template<TYPE>& lhs,
               const TestType_PrintMethod_Template<TYPE>& rhs)
{
    return lhs.value() == rhs.value();
}

                  // =======================================
                  // class TestType_OStreamOperator_Template
                  // =======================================

template <typename TYPE>
class TestType_OStreamOperator_Template {
    // This template is used to generate the 'TestType_OStreamOperator' type,
    // which simulates a type that has 'operator<<' declared.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_GenericVariant' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

    // PRIVATE DATA MEMBERS
    TYPE d_value;  // value that will be printed out

  public:
    // CREATORS
    TestType_OStreamOperator_Template()
    {
        ++g_objectCount;
    }

    TestType_OStreamOperator_Template(
                             const TestType_OStreamOperator_Template& original)
    : d_value(original.d_value)
    {
        ++g_objectCount;
    }

    TestType_OStreamOperator_Template(const TYPE& value)
    : d_value(value)
    {
        ++g_objectCount;
    }

    ~TestType_OStreamOperator_Template()
    {
        --g_objectCount;
    }

    // ACCESSORS
    const TYPE& value() const
    {
        return d_value;
    }
};

template <typename TYPE>
int operator==(const TestType_OStreamOperator_Template<TYPE>& lhs,
               const TestType_OStreamOperator_Template<TYPE>& rhs)
{
    return lhs.value() == rhs.value();
}

template <typename CHAR_T, typename TRAITS, typename TYPE>
std::basic_ostream<CHAR_T, TRAITS>&
operator<<(std::basic_ostream<CHAR_T, TRAITS>&            stream,
           const TestType_OStreamOperator_Template<TYPE>& rhs)
{
    return stream << rhs.value();
}

                        // ============================
                        // class TestType_Bdex_Template
                        // ============================

template <typename TYPE>
class TestType_Bdex_Template {
    // This template is used to generate the 'TestType_Bdex' type, which
    // simulates a type that declares the 'bdex_TypeTraitStreamable' trait.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_GenericVariant' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

    // PRIVATE DATA MEMBERS
    TYPE        d_value;    // value that will be streamed
    mutable int d_version;  // 'version' that was passed to 'bdex' functions

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(TestType_Bdex_Template,
                                 bdex_TypeTraitStreamable);

    // CLASS METHODS
    static int maxSupportedBdexVersion()
    {
        return 123;  // some arbitrary non-zero value
    }

    // CREATORS
    TestType_Bdex_Template()
    : d_version(0)
    {
        ++g_objectCount;
    }

    TestType_Bdex_Template(const TestType_Bdex_Template& original)
    : d_value(original.d_value)
    , d_version(0)
    {
        ++g_objectCount;
    }

    TestType_Bdex_Template(const TYPE& value)
    : d_value(value)
    , d_version(0)
    {
        ++g_objectCount;
    }

    ~TestType_Bdex_Template()
    {
        --g_objectCount;
    }

    // MANIPULATORS
    template <typename STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version)
    {
        d_version = version;
        return stream >> d_value;
    }

    // ACCESSORS
    template <typename STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const
    {
        d_version = version;
        return stream << d_value;
    }

    const TYPE& value() const
    {
        return d_value;
    }

    int version() const
    {
        return d_version;
    }
};

template <typename TYPE>
int operator==(const TestType_Bdex_Template<TYPE>& lhs,
               const TestType_Bdex_Template<TYPE>& rhs)
{
    return lhs.value() == rhs.value();
}

                      // ================================
                      // class TestType_NoTraits_Template
                      // ================================

template <typename TYPE>
class TestType_NoTraits_Template {
    // This type is used to generate the 'TestType_NoTraits' type, which
    // simulates a type that does not have any traits and no 'operator<<'.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_GenericVariant' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

    // PRIVATE DATA MEMBERS
    TYPE d_value;  // value contained by this object

  public:
    // CREATORS
    TestType_NoTraits_Template()
    {
        ++g_objectCount;
    }

    TestType_NoTraits_Template(const TestType_NoTraits_Template& original)
    : d_value(original.d_value)
    {
        ++g_objectCount;
    }

    TestType_NoTraits_Template(const TYPE& value)
    : d_value(value)
    {
        ++g_objectCount;
    }

    ~TestType_NoTraits_Template()
    {
        --g_objectCount;
    }

    // ACCESSORS
    const TYPE& value() const
    {
        return d_value;
    }
};

template <typename TYPE>
int operator==(const TestType_NoTraits_Template<TYPE>& lhs,
               const TestType_NoTraits_Template<TYPE>& rhs)
{
    return lhs.value() == rhs.value();
}

                              // ===============
                              // class ValueType
                              // ===============

class ValueType {
    // This value type is used as the template argument 'TYPE' for the class
    // templates above.  It is basically a wrapper around 'std::string' with a
    // default value.

    // PRIVATE DATA MEMBERS
    std::string d_value;  // the value

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(ValueType,
                                 bdex_TypeTraitStreamable);

    // CLASS METHODS
    static int maxSupportedBdexVersion()
    {
        return 1;
    }

    // CREATORS
    ValueType()
    : d_value("(* DEFAULT *)")
    {
    }

    ValueType(const ValueType& original)
    : d_value(original.d_value)
    {
    }

    explicit ValueType(const std::string value)
    : d_value(value)
    {
    }

    ~ValueType()
    {
    }

    // MANIPULATORS
    template <typename STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version)
    {
        return stream >> d_value;
    }

    // ACCESSORS
    template <typename STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const
    {
        return stream << d_value;
    }

    const std::string& value() const
    {
        return d_value;
    }
};

int operator==(const ValueType& lhs, const ValueType& rhs)
{
    return lhs.value() == rhs.value();
}

int operator!=(const ValueType& lhs, const ValueType& rhs)
{
    return lhs.value() != rhs.value();
}

std::ostream& operator<<(std::ostream& stream, const ValueType& rhs)
{
    return stream << rhs.value();
}

typedef bdeut_GenericVariant                         Obj;
typedef TestType_PrintMethod_Template<ValueType>     TestType_PrintMethod;
typedef TestType_OStreamOperator_Template<ValueType> TestType_OStreamOperator;
typedef TestType_Bdex_Template<ValueType>            TestType_Bdex;
typedef TestType_NoTraits_Template<ValueType>        TestType_NoTraits;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//            GENERATOR FUNCTIONS 'g', 'gg' and 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' to configure the object
// according to a custom language.  The given 'spec' contains the information
// needed to assign a type and optional value to a generic variant object.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>       ::= <EMPTY> | <DATA>
//
// <EMPTY>      ::=
//
// <DATA>       ::= <ASSIGN> | <ASSIGN> <DATA>
//
// <ASSIGN>     ::= 'V'              # assign<void>()
//                  | 'P'            # assign<TestType_PrintMethod>()
//                  | 'P' <VALUE>    # assign<TestType_PrintMethod>(VALUE)
//                  | 'O'            # assign<TestType_OStreamOperator>()
//                  | 'O' <VALUE>    # assign<TestType_OStreamOperator>(VALUE)
//                  | 'B'            # assign<TestType_Bdex>()
//                  | 'B' <VALUE>    # assign<TestType_Bdex>(VALUE)
//                  | 'N'            # assign<TestType_NoTraits>()
//                  | 'N' <VALUE>    # assign<TestType_NoTraits>(VALUE)
//
// <VALUE>      ::= ( ^[ '\x00' | 'A'-'Z' ] )+  # one or more of any character
//                                              # except null characters &
//                                              # capital letters
//
// Spec String       Description
// ----------------- ----------------------------------------------------------
// ""                Has no effect.
// "V"               Calls 'assign<void>()'.
// "VV"              Calls 'assign<void>()' then calls 'assign<void>()' again.
// "VP"              Calls 'assign<void>()' then calls
//                   'assign<TestType_PrintMethod>()'.
// "Pabc"            Calls 'assign<TestType_PrintMethod>(ValueType("abc"))'.
// "VPabc"           Calls 'assign<void>()' then calls
//                   'assign<TestType_PrintMethod>(ValueType("abc"))'.
// "BdefV"           Calls 'assign<TestType_Bdex>(ValueType("def"))' then calls
//                   'assign<void>()'.
// "BdefOghi"        Calls 'assign<TestType_Bdex>(ValueType("def"))' then calls
//                   'assign<TestType_OStreamOperator>(ValueType("ghi"))'.
// ----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, int vF = 1)
    // Configure the specified 'object' according to the specified 'spec' using
    // the primary manipulator function 'assign'.  Optionally specify a zero
    // 'vF' to suppress 'spec' syntax error messages.  Return the index of the
    // first invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('V' == spec[i]) {
            object->TEMPLATE_MEMBER_BUG assign<void>();
        }
        else if ('P' == spec[i]) {
            const char *p = &spec[i+1];
            int         j = 0;

            for (; (0 != p[j]) && (p[j] < 'A' || 'Z' < p[j]); ++j);

            if (j) {
                std::string value(p, j);
                object->TEMPLATE_MEMBER_BUG assign<TestType_PrintMethod>(
                                                             ValueType(value));
                i += j;
            }
            else {
                object->TEMPLATE_MEMBER_BUG assign<TestType_PrintMethod>();
            }
        }
        else if ('O' == spec[i]) {
            const char *p = &spec[i+1];
            int         j = 0;

            for (; (0 != p[j]) && (p[j] < 'A' || 'Z' < p[j]); ++j);

            if (j) {
                std::string value(p, j);
                object->TEMPLATE_MEMBER_BUG assign<TestType_OStreamOperator>(
                                                             ValueType(value));
                i += j;
            }
            else {
                object->TEMPLATE_MEMBER_BUG assign<TestType_OStreamOperator>();
            }
        }
        else if ('B' == spec[i]) {
            const char *p = &spec[i+1];
            int         j = 0;

            for (; (0 != p[j]) && (p[j] < 'A' || 'Z' < p[j]); ++j);

            if (j) {
                std::string value(p, j);
                object->TEMPLATE_MEMBER_BUG assign<TestType_Bdex>(
                                                             ValueType(value));
                i += j;
            }
            else {
                object->TEMPLATE_MEMBER_BUG assign<TestType_Bdex>();
            }
        }
        else if ('N' == spec[i]) {
            const char *p = &spec[i+1];
            int         j = 0;

            for (; (0 != p[j]) && (p[j] < 'A' || 'Z' < p[j]); ++j);

            if (j) {
                std::string value(p, j);
                object->TEMPLATE_MEMBER_BUG assign<TestType_NoTraits>(
                                                             ValueType(value));
                i += j;
            }
            else {
                object->TEMPLATE_MEMBER_BUG assign<TestType_NoTraits>();
            }
        }
        else {
            // syntax error, print an error message if vF != 0
            if (vF) {
                cout << "Error, bad character ('" << spec[i] << "') at "
                     << "position " << i << "." << endl;
                P(spec);
            }
            return i;  // Discontinue processing this spec.
        }
    }

    return SUCCESS;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object;
    return gg(&object, spec);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator  testAllocator;
    bdema_TestAllocator *ALLOC = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   We need to test the assignment operator ('operator=').
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S X S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdeut_GenericVariant& operator=(c b_GV& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // spec string
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   ""      },

            { L_,   "V"     },
            { L_,   "P"     },
            { L_,   "O"     },
            { L_,   "B"     },
            { L_,   "N"     },

            { L_,   "Pabc"  },
            { L_,   "Oabc"  },
            { L_,   "Babc"  },
            { L_,   "Nabc"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        for (int i = 0; i < NUM_DATA; ++i)
        {
            const int   V_LINE = DATA[i].d_lineNum;
            const char *V_SPEC = DATA[i].d_spec;
            if (veryVerbose) {
                T_ P(V_SPEC)
            }

            Obj        mV(ALLOC);
            const Obj& V = gg(&mV, V_SPEC);

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   U_LINE = DATA[j].d_lineNum;
                const char *U_SPEC = DATA[j].d_spec;

                if (veryVeryVerbose) {
                    T_ T_ P(U_SPEC)
                }

                Obj        mU(ALLOC);
                const Obj& U = gg(&mU, U_SPEC);

                if (veryVeryVerbose) { T_ T_ P_(V) P_(U) }

                Obj mW(V, ALLOC);  const Obj& W = mW;  // control

                mU = V;
                if (veryVeryVerbose) P(U);
                LOOP2_ASSERT(V_LINE, U_LINE, W == U);
                LOOP2_ASSERT(V_LINE, U_LINE, W == V);
            }
        }

        if (verbose) cout << "\nTesting assignment u = u (Aliasing)." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_lineNum;
            const char *SPEC = DATA[i].d_spec;

            Obj        mU(ALLOC);
            const Obj& U = gg(&mU, SPEC);

            Obj mW(U, ALLOC);  const Obj& W = mW;  // control

            mU = U;
            if (veryVerbose) { T_ P_(U) P(W) }
            LOOP_ASSERT(LINE, W == U);
        }

        if (verbose) cout << "\nEnd of Assignment Operator Test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //   This will test the 'g' generator function.
        //
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need only
        //   to verify that the arguments are properly forwarded and that 'g'
        //   returns an object by value.
        //
        // Plan:
        //   For each spec in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed object 'mX' configured using
        //   'gg(&mX, SPEC)'.  The test also ensures that 'g' returns a
        //   distinct object by comparing the memory addresses.
        //
        // Testing:
        //   b_GV g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Generator Function 'g'" << endl
                          << "\n==============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // spec string
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   ""      },

            { L_,   "V"     },
            { L_,   "P"     },
            { L_,   "O"     },
            { L_,   "B"     },
            { L_,   "N"     },

            { L_,   "Pabc"  },
            { L_,   "Oabc"  },
            { L_,   "Babc"  },
            { L_,   "Nabc"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare values produced by 'g' and 'gg' "
                          << "for various inputs." << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_lineNum;
            const char *SPEC = DATA[i].d_spec;

            if (veryVerbose) {
                T_ P_(i) P(SPEC)
            }

            Obj        mX(ALLOC);
            const Obj& X = gg(&mX, SPEC);

            if (veryVeryVerbose) {
                cout << "\t\t g = " << g(SPEC) << endl;
                cout << "\t\tgg = " << X       << endl;
            }

            LOOP_ASSERT(LINE, X == g(SPEC));
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *SPEC = "Pabc";

            ASSERT(sizeof(Obj) == sizeof g(SPEC));  // compile-time fact

            Obj mX;                                 // runtime tests
            Obj& r1 = gg(&mX, SPEC);
            Obj& r2 = gg(&mX, SPEC);
            const Obj& r3 = g(SPEC);
            const Obj& r4 = g(SPEC);
            ASSERT(&r2 == &r1);
            ASSERT(&mX == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&mX != &r3);
        }

        if (verbose) cout << "\nEnd of Generator Function 'g' Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   This will test the copy constructor.
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically-values objects W and X using tested methods
        //   (in this case, the 'gg' function).  Then copy construct an object
        //   Y from X, and use the equality operator to assert that both X and
        //   Y have the same value as W.
        //
        // Testing:
        //   bdeut_GenericVariant(c b_GV& original, b_A *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;


        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // spec string
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   ""      },

            { L_,   "V"     },
            { L_,   "P"     },
            { L_,   "O"     },
            { L_,   "B"     },
            { L_,   "N"     },

            { L_,   "Pabc"  },
            { L_,   "Oabc"  },
            { L_,   "Babc"  },
            { L_,   "Nabc"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_lineNum;
            const char *SPEC = DATA[i].d_spec;

            Obj        mW(ALLOC);
            const Obj& W = gg(&mW, SPEC);

            Obj        mX(ALLOC);
            const Obj& X = gg(&mX, SPEC);

            Obj        mY(X, ALLOC);
            const Obj& Y = mY;

            if (veryVerbose) {
                T_ P_(W) P_(X) P_(Y) P(SPEC)
            }

            LOOP3_ASSERT(LINE, X, W, X == W);
            LOOP3_ASSERT(LINE, Y, W, Y == W);
        }

        if (verbose) cout << "\nEnd of Copy Constructor Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   This will test the equality operations ('operator==' and
        //   'operator!=').
        //
        // Concerns:
        //   We want to make sure that 'operator==' returns 0 for objects that
        //   are very similar but still different, but returns 1 for objects
        //   that are exactly the same.  Likewise, we want to make sure that
        //   'operator!=' returns 1 for objects that are very similar but still
        //   different, but returns 0 for objects that are exactly the same.
        //
        // Plan:
        //   Construct a set of specs containing similar but different data
        //   values.  Then loop through the cross product of the test data.
        //   For each tuple, generate two objects 'U' and 'V' using the
        //   previously tested 'gg' function.  Use the '==' and '!=' operators
        //   and check their return value for correctness.
        //
        // Testing:
        //   int operator==(const b_GV& lhs, const b_GV& rhs);
        //   int operator!=(const b_GV& lhs, const b_GV& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nCompare each pair of similar values (u, v) in "
                          << "S X S." << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // spec string
        } DATA[] = {
            //line  spec
            //----  ----
            { L_,   ""      },

            { L_,   "P"     },
            { L_,   "O"     },
            { L_,   "B"     },
            { L_,   "N"     },

            { L_,   "Pabc"  },
            { L_,   "Oabc"  },
            { L_,   "Babc"  },
            { L_,   "Nabc"  },
        };
        int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   U_LINE = DATA[i].d_lineNum;
            const char *U_SPEC = DATA[i].d_spec;

            Obj        mU(ALLOC);
            const Obj& U = gg(&mU, U_SPEC);

            if (veryVerbose) {
                T_ P_(i) P_(U) P(U_SPEC)
            }

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   V_LINE = DATA[j].d_lineNum;
                const char *V_SPEC = DATA[j].d_spec;

                Obj        mV(ALLOC);
                const Obj& V = gg(&mV, V_SPEC);

                if (veryVeryVerbose) {
                    T_ T_ P_(j) P_(V) P(V_SPEC)
                }

                const int isSame = (i == j);
                LOOP2_ASSERT(U_LINE, V_LINE,  isSame == (U == V));
                LOOP2_ASSERT(U_LINE, V_LINE, !isSame == (U != V));
            }
        }

        if (verbose) cout << "\nEnd of Equality Operators Test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR AND PRINT METHOD
        //   This will test the '<<' operator and the print method.
        //
        // Concerns:
        //   We want to make sure that the object is written to the stream
        //   correctly and in the expected format (with correct indentation).
        //
        // Plan:
        //   The '<<' operator depends on the 'print' method.  So, we need to
        //   test 'print' before testing 'operator<<'.  This test is broken up
        //   into two parts:
        //
        //       1. Testing of 'print'.
        //       2. Testing of 'operator<<'.
        //
        //   Each datum in DATA contains a specification string, level, spaces
        //   per level and the expected output format for 'print' and
        //   'operator<<'.  For each datum, construct an independent object
        //   'mX' and call 'gg' with the specification string.  Create a
        //   'stringstream' object and use the 'print' method to print 'mX'.
        //   Compare the contents of the stream object with the expected output
        //   string.
        //
        //   To test the '<<' operator, repeat the test but use 'operator<<'
        //   instead of the 'print' method.  Compare the contents of the stream
        //   object with the expected output string.
        //
        // Testing:
        //   std::ostream& print(std::ostream& stream, int, int) const;
        //   std::ostream& operator<<(std::ostream& stream, const b_GV& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Output (<<) Operator And Print Method"
                          << "\n============================================="
                          << endl;

        static const struct {
            int         d_lineNum;         // source line number
            const char *d_spec;            // spec string
            int         d_level;           // level
            int         d_spacesPerLevel;  // spaces per level
            const char *d_printOutput;     // 'print' output
            const char *d_operatorOutput;  // 'operator<<' output
        } DATA[] = {
            //line   spec      lvl   spcs  printOutput       opOutput
            //----   ----      ---   ----  -----------       --------
            { L_,    "",       -2,   -2,   "(* NULL *)",     "(* NULL *)"    },
            { L_,    "",       -2,   -1,   "(* NULL *)",     "(* NULL *)"    },
            { L_,    "",       -2,   0,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       -2,   1,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       -2,   2,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       -1,   -2,   "(* NULL *)",     "(* NULL *)"    },
            { L_,    "",       -1,   -1,   "(* NULL *)",     "(* NULL *)"    },
            { L_,    "",       -1,   0,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       -1,   1,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       -1,   2,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       0,    -2,   "(* NULL *)",     "(* NULL *)"    },
            { L_,    "",       0,    -1,   "(* NULL *)",     "(* NULL *)"    },
            { L_,    "",       0,    0,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       0,    1,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       0,    2,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       1,    -2,   "  (* NULL *)",   "(* NULL *)"    },
            { L_,    "",       1,    -1,   " (* NULL *)",    "(* NULL *)"    },
            { L_,    "",       1,    0,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       1,    1,    " (* NULL *)\n",  "(* NULL *)"    },
            { L_,    "",       1,    2,    "  (* NULL *)\n", "(* NULL *)"    },
            { L_,    "",       2,    -2,   "    (* NULL *)", "(* NULL *)"    },
            { L_,    "",       2,    -1,   "  (* NULL *)",   "(* NULL *)"    },
            { L_,    "",       2,    0,    "(* NULL *)\n",   "(* NULL *)"    },
            { L_,    "",       2,    1,    "  (* NULL *)\n", "(* NULL *)"    },
            { L_,    "",       2,    2,    "    (* NULL *)\n", "(* NULL *)"  },

            { L_,    "P",      -2,   -2,   "",               ""              },
            { L_,    "P",      -2,   -1,   "",               ""              },
            { L_,    "P",      -2,   0,    "",               ""              },
            { L_,    "P",      -2,   1,    "",               ""              },
            { L_,    "P",      -2,   2,    "",               ""              },
            { L_,    "P",      -1,   -2,   "",               ""              },
            { L_,    "P",      -1,   -1,   "",               ""              },
            { L_,    "P",      -1,   0,    "",               ""              },
            { L_,    "P",      -1,   1,    "",               ""              },
            { L_,    "P",      -1,   2,    "",               ""              },
            { L_,    "P",      0,    -2,   "",               ""              },
            { L_,    "P",      0,    -1,   "",               ""              },
            { L_,    "P",      0,    0,    "",               ""              },
            { L_,    "P",      0,    1,    "",               ""              },
            { L_,    "P",      0,    2,    "",               ""              },
            { L_,    "P",      1,    -2,   "",               ""              },
            { L_,    "P",      1,    -1,   "",               ""              },
            { L_,    "P",      1,    0,    "",               ""              },
            { L_,    "P",      1,    1,    "",               ""              },
            { L_,    "P",      1,    2,    "",               ""              },
            { L_,    "P",      2,    -2,   "",               ""              },
            { L_,    "P",      2,    -1,   "",               ""              },
            { L_,    "P",      2,    0,    "",               ""              },
            { L_,    "P",      2,    1,    "",               ""              },
            { L_,    "P",      2,    2,    "",               ""              },

            { L_,    "Oabc",   -2,   -2,   "abc",            "abc"           },
            { L_,    "Oabc",   -2,   -1,   "abc",            "abc"           },
            { L_,    "Oabc",   -2,   0,    "abc\n",          "abc"           },
            { L_,    "Oabc",   -2,   1,    "abc\n",          "abc"           },
            { L_,    "Oabc",   -2,   2,    "abc\n",          "abc"           },
            { L_,    "Oabc",   -1,   -2,   "abc",            "abc"           },
            { L_,    "Oabc",   -1,   -1,   "abc",            "abc"           },
            { L_,    "Oabc",   -1,   0,    "abc\n",          "abc"           },
            { L_,    "Oabc",   -1,   1,    "abc\n",          "abc"           },
            { L_,    "Oabc",   -1,   2,    "abc\n",          "abc"           },
            { L_,    "Oabc",   0,    -2,   "abc",            "abc"           },
            { L_,    "Oabc",   0,    -1,   "abc",            "abc"           },
            { L_,    "Oabc",   0,    0,    "abc\n",          "abc"           },
            { L_,    "Oabc",   0,    1,    "abc\n",          "abc"           },
            { L_,    "Oabc",   0,    2,    "abc\n",          "abc"           },
            { L_,    "Oabc",   1,    -2,   "  abc",          "abc"           },
            { L_,    "Oabc",   1,    -1,   " abc",           "abc"           },
            { L_,    "Oabc",   1,    0,    "abc\n",          "abc"           },
            { L_,    "Oabc",   1,    1,    " abc\n",         "abc"           },
            { L_,    "Oabc",   1,    2,    "  abc\n",        "abc"           },
            { L_,    "Oabc",   2,    -2,   "    abc",        "abc"           },
            { L_,    "Oabc",   2,    -1,   "  abc",          "abc"           },
            { L_,    "Oabc",   2,    0,    "abc\n",          "abc"           },
            { L_,    "Oabc",   2,    1,    "  abc\n",        "abc"           },
            { L_,    "Oabc",   2,    2,    "    abc\n",      "abc"           },

            { L_,    "Nabc",   -2,   -2,   "(* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -2,   -1,   "(* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -2,   0,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -2,   1,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -2,   2,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -1,   -2,   "(* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -1,   -1,   "(* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -1,   0,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -1,   1,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   -1,   2,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   0,    -2,   "(* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   0,    -1,   "(* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   0,    0,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   0,    1,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   0,    2,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   1,    -2,   "  (* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   1,    -1,   " (* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   1,    0,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   1,    1,    " (* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   1,    2,    "  (* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   2,    -2,   "    (* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   2,    -1,   "  (* UNPRINTABLE *)",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   2,    0,    "(* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   2,    1,    "  (* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
            { L_,    "Nabc",   2,    2,    "    (* UNPRINTABLE *)\n",
                                                         "(* UNPRINTABLE *)" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\n1. Testing 'print'." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE             = DATA[i].d_lineNum;
            const char *SPEC             = DATA[i].d_spec;
            const int   LEVEL            = DATA[i].d_level;
            const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
            const char *EXPECTED_OUTPUT  = DATA[i].d_printOutput;

            Obj        mX(ALLOC);
            const Obj& X = gg(&mX, SPEC);

            stringstream ss;
            X.print(ss, LEVEL, SPACES_PER_LEVEL);

            if (veryVerbose) { T_ P_(X) P_(EXPECTED_OUTPUT) P(ss.str()) }
            LOOP3_ASSERT(LINE, EXPECTED_OUTPUT,   ss.str(),
                               EXPECTED_OUTPUT == ss.str());

            if ("P" == SPEC) {
                // Check that level & spaces per level was passed correctly for
                // types that have a print method.

                typedef TestType_PrintMethod Type;

                LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());

                const Type& obj = X.TEMPLATE_MEMBER_BUG the<Type>();

                LOOP_ASSERT(LINE, &ss == obj.stream());
                LOOP3_ASSERT(LINE, LEVEL,   obj.level(),
                                   LEVEL == obj.level());
                LOOP3_ASSERT(LINE, SPACES_PER_LEVEL,   obj.spacesPerLevel(),
                                   SPACES_PER_LEVEL == obj.spacesPerLevel());
            }
        }

        if (verbose) cout << "\n2. Testing 'operator<<'." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE            = DATA[i].d_lineNum;
            const char *SPEC            = DATA[i].d_spec;
            const char *EXPECTED_OUTPUT = DATA[i].d_operatorOutput;

            Obj        mX(ALLOC);
            const Obj& X = gg(&mX, SPEC);

            stringstream ss;
            ss << X;

            if (veryVerbose) { T_ P_(X) P_(EXPECTED_OUTPUT) P(ss.str()) }
            LOOP3_ASSERT(LINE, EXPECTED_OUTPUT,   ss.str(),
                               EXPECTED_OUTPUT == ss.str());
        }

        if (verbose) cout << "\nEnd of Output (<<) Operator And Print Method "
                          << "Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   In this test case, we will test the basic accessors.
        //
        // Concerns:
        //   We want to verify that, for each unique object value, the basic
        //   accessors return the correct values.
        //
        // Plan:
        //   For each test vector in DATA, construct an object 'mX' using the
        //   default constructor.  Then call 'gg' to set the type and value of
        //   'mX'.  Ensure that the basic accessors return the expected
        //   results.
        //
        // Testing:
        //   bool is<TYPE>() const;
        //   const TYPE& the<TYPE>() const;
        //   const std::type_info& typeInfo() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;

        typedef ValueType VT;  // shorthand for table entries

        static const struct {
            int         d_lineNum;        // source line number
            const char *d_spec;           // spec string
            char        d_expectedType;   // expected final type of value
            ValueType   d_expectedValue;  // expected final value
        } DATA[] = {
            //line   spec                       expectedType   expectedValue
            //----   ----                       ------------   -------------
            { L_,    "",                        'V'            /* NULL */    },
            { L_,    "V",                       'V'            /* NULL */    },
            { L_,    "P",                       'P',           VT()          },
            { L_,    "Pa",                      'P',           VT("a")       },
            { L_,    "Pab",                     'P',           VT("ab")      },
            { L_,    "Pabc",                    'P',           VT("abc")     },
            { L_,    "Pabc@[",                  'P',           VT("abc@[")   },
            { L_,    "O",                       'O',           VT()          },
            { L_,    "Oa",                      'O',           VT("a")       },
            { L_,    "Oab",                     'O',           VT("ab")      },
            { L_,    "Oabc",                    'O',           VT("abc")     },
            { L_,    "Oabc@[",                  'O',           VT("abc@[")   },
            { L_,    "B",                       'B',           VT()          },
            { L_,    "Ba",                      'B',           VT("a")       },
            { L_,    "Bab",                     'B',           VT("ab")      },
            { L_,    "Babc",                    'B',           VT("abc")     },
            { L_,    "Babc@[",                  'B',           VT("abc@[")   },
            { L_,    "N",                       'N',           VT()          },
            { L_,    "Na",                      'N',           VT("a")       },
            { L_,    "Nab",                     'N',           VT("ab")      },
            { L_,    "Nabc",                    'N',           VT("abc")     },
            { L_,    "Nabc@[",                  'N',           VT("abc@[")   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE           = DATA[i].d_lineNum;
            const char       *SPEC           = DATA[i].d_spec;
            const char        EXPECTED_TYPE  = DATA[i].d_expectedType;
            const ValueType&  EXPECTED_VALUE = DATA[i].d_expectedValue;

            Obj        mX(ALLOC);
            const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) {
                T_ P(X)
            }

            switch (EXPECTED_TYPE) {
              case 'V': {   // void
                typedef void Type;
                LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                LOOP_ASSERT(LINE, typeid(Type) == X.typeInfo());
              } break;
              case 'P': {   // TestType_PrintMethod
                typedef TestType_PrintMethod Type;

                const ValueType& value
                               = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                LOOP_ASSERT(LINE, typeid(Type) == X.typeInfo());
                LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                   EXPECTED_VALUE == value);
              } break;
              case 'O': {   // TestType_OStreamOperator
                typedef TestType_OStreamOperator Type;

                const ValueType& value
                               = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                LOOP_ASSERT(LINE, typeid(Type) == X.typeInfo());
                LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                   EXPECTED_VALUE == value);
              } break;
              case 'B': {   // TestType_Bdex
                typedef TestType_Bdex Type;

                const ValueType& value
                               = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                LOOP_ASSERT(LINE, typeid(Type) == X.typeInfo());
                LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                   EXPECTED_VALUE == value);
              } break;
              case 'N': {   // TestType_NoTraits
                typedef TestType_NoTraits Type;

                const ValueType& value
                               = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                LOOP_ASSERT(LINE, typeid(Type) == X.typeInfo());
                LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                   EXPECTED_VALUE == value);
              } break;
              default: {
                ASSERT(0);
              } break;
            }
        }

        if (verbose) cout << "\nEnd of Basic Accessors Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION 'gg'
        //   This will test the 'gg' generator function.
        //
        // Concerns:
        //   We want to verify that (1) valid generator syntax produces
        //   expected results, and (2) invalid syntax is detected and reported.
        //
        // Plan:
        //   For each enumerated sequence of 'spec' values, use the primitive
        //   generator function 'gg' to set the state of a newly created
        //   object.  Verify that 'gg' returns a valid reference to the
        //   modified object and, using basic accessors, that the value of the
        //   object is as expected.  Note that we are testing the parser only;
        //   the primary manipulators are already assumed to work.
        //
        //   This test case also tests the 'ggg' function using invalid 'spec'
        //   values, to ensure that error messages are caught and reported
        //   correctly.
        //
        // Testing:
        //   int ggg(b_GV *object, const char *spec, int vF = 1);
        //   b_GV& gg(b_GV *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primitive Generator Function 'gg'"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            typedef ValueType VT;  // shorthand for table entries

            static const struct {
                int         d_lineNum;        // source line number
                const char *d_spec;           // spec string
                char        d_expectedType;   // expected final type of value
                ValueType   d_expectedValue;  // expected final value
            } DATA[] = {
                //line   spec                   expectedType   expectedValue
                //----   ----                   ------------   -------------
                { L_,    "",                    'V'            /* NULL */    },
                { L_,    "V",                   'V'            /* NULL */    },
                { L_,    "P",                   'P',           VT()          },
                { L_,    "Pa",                  'P',           VT("a")       },
                { L_,    "Pab",                 'P',           VT("ab")      },
                { L_,    "Pabc",                'P',           VT("abc")     },
                { L_,    "Pabc@[",              'P',           VT("abc@[")   },
                { L_,    "O",                   'O',           VT()          },
                { L_,    "Oa",                  'O',           VT("a")       },
                { L_,    "Oab",                 'O',           VT("ab")      },
                { L_,    "Oabc",                'O',           VT("abc")     },
                { L_,    "Oabc@[",              'O',           VT("abc@[")   },
                { L_,    "B",                   'B',           VT()          },
                { L_,    "Ba",                  'B',           VT("a")       },
                { L_,    "Bab",                 'B',           VT("ab")      },
                { L_,    "Babc",                'B',           VT("abc")     },
                { L_,    "Babc@[",              'B',           VT("abc@[")   },
                { L_,    "N",                   'N',           VT()          },
                { L_,    "Na",                  'N',           VT("a")       },
                { L_,    "Nab",                 'N',           VT("ab")      },
                { L_,    "Nabc",                'N',           VT("abc")     },
                { L_,    "Nabc@[",              'N',           VT("abc@[")   },

                { L_,    "VV",                  'V'            /* NULL */    },
                { L_,    "PV",                  'V'            /* NULL */    },
                { L_,    "OV",                  'V'            /* NULL */    },
                { L_,    "BV",                  'V'            /* NULL */    },
                { L_,    "NV",                  'V'            /* NULL */    },
                { L_,    "PaV",                 'V'            /* NULL */    },
                { L_,    "OaV",                 'V'            /* NULL */    },
                { L_,    "BaV",                 'V'            /* NULL */    },
                { L_,    "NaV",                 'V'            /* NULL */    },
                { L_,    "PabV",                'V'            /* NULL */    },
                { L_,    "OabV",                'V'            /* NULL */    },
                { L_,    "BabV",                'V'            /* NULL */    },
                { L_,    "NabV",                'V'            /* NULL */    },
                { L_,    "PabcV",               'V'            /* NULL */    },
                { L_,    "OabcV",               'V'            /* NULL */    },
                { L_,    "BabcV",               'V'            /* NULL */    },
                { L_,    "NabcV",               'V'            /* NULL */    },

                { L_,    "VP",                  'P',           VT()          },
                { L_,    "PP",                  'P',           VT()          },
                { L_,    "OP",                  'P',           VT()          },
                { L_,    "BP",                  'P',           VT()          },
                { L_,    "NP",                  'P',           VT()          },
                { L_,    "PaP",                 'P',           VT()          },
                { L_,    "OaP",                 'P',           VT()          },
                { L_,    "BaP",                 'P',           VT()          },
                { L_,    "NaP",                 'P',           VT()          },
                { L_,    "PabP",                'P',           VT()          },
                { L_,    "OabP",                'P',           VT()          },
                { L_,    "BabP",                'P',           VT()          },
                { L_,    "NabP",                'P',           VT()          },
                { L_,    "PabcP",               'P',           VT()          },
                { L_,    "OabcP",               'P',           VT()          },
                { L_,    "BabcP",               'P',           VT()          },
                { L_,    "NabcP",               'P',           VT()          },
                { L_,    "PabcPd",              'P',           VT("d")       },
                { L_,    "OabcPd",              'P',           VT("d")       },
                { L_,    "BabcPd",              'P',           VT("d")       },
                { L_,    "NabcPd",              'P',           VT("d")       },
                { L_,    "PabcPde",             'P',           VT("de")      },
                { L_,    "OabcPde",             'P',           VT("de")      },
                { L_,    "BabcPde",             'P',           VT("de")      },
                { L_,    "NabcPde",             'P',           VT("de")      },
                { L_,    "PabcPdef",            'P',           VT("def")     },
                { L_,    "OabcPdef",            'P',           VT("def")     },
                { L_,    "BabcPdef",            'P',           VT("def")     },
                { L_,    "NabcPdef",            'P',           VT("def")     },

                { L_,    "VO",                  'O',           VT()          },
                { L_,    "PO",                  'O',           VT()          },
                { L_,    "OO",                  'O',           VT()          },
                { L_,    "BO",                  'O',           VT()          },
                { L_,    "NO",                  'O',           VT()          },
                { L_,    "PaO",                 'O',           VT()          },
                { L_,    "OaO",                 'O',           VT()          },
                { L_,    "BaO",                 'O',           VT()          },
                { L_,    "NaO",                 'O',           VT()          },
                { L_,    "PabO",                'O',           VT()          },
                { L_,    "OabO",                'O',           VT()          },
                { L_,    "BabO",                'O',           VT()          },
                { L_,    "NabO",                'O',           VT()          },
                { L_,    "PabcO",               'O',           VT()          },
                { L_,    "OabcO",               'O',           VT()          },
                { L_,    "BabcO",               'O',           VT()          },
                { L_,    "NabcO",               'O',           VT()          },
                { L_,    "PabcOd",              'O',           VT("d")       },
                { L_,    "OabcOd",              'O',           VT("d")       },
                { L_,    "BabcOd",              'O',           VT("d")       },
                { L_,    "NabcOd",              'O',           VT("d")       },
                { L_,    "PabcOde",             'O',           VT("de")      },
                { L_,    "OabcOde",             'O',           VT("de")      },
                { L_,    "BabcOde",             'O',           VT("de")      },
                { L_,    "NabcOde",             'O',           VT("de")      },
                { L_,    "PabcOdef",            'O',           VT("def")     },
                { L_,    "OabcOdef",            'O',           VT("def")     },
                { L_,    "BabcOdef",            'O',           VT("def")     },
                { L_,    "NabcOdef",            'O',           VT("def")     },

                { L_,    "VB",                  'B',           VT()          },
                { L_,    "PB",                  'B',           VT()          },
                { L_,    "OB",                  'B',           VT()          },
                { L_,    "BB",                  'B',           VT()          },
                { L_,    "NB",                  'B',           VT()          },
                { L_,    "PaB",                 'B',           VT()          },
                { L_,    "OaB",                 'B',           VT()          },
                { L_,    "BaB",                 'B',           VT()          },
                { L_,    "NaB",                 'B',           VT()          },
                { L_,    "PabB",                'B',           VT()          },
                { L_,    "OabB",                'B',           VT()          },
                { L_,    "BabB",                'B',           VT()          },
                { L_,    "NabB",                'B',           VT()          },
                { L_,    "PabcB",               'B',           VT()          },
                { L_,    "OabcB",               'B',           VT()          },
                { L_,    "BabcB",               'B',           VT()          },
                { L_,    "NabcB",               'B',           VT()          },
                { L_,    "PabcBd",              'B',           VT("d")       },
                { L_,    "OabcBd",              'B',           VT("d")       },
                { L_,    "BabcBd",              'B',           VT("d")       },
                { L_,    "NabcBd",              'B',           VT("d")       },
                { L_,    "PabcBde",             'B',           VT("de")      },
                { L_,    "OabcBde",             'B',           VT("de")      },
                { L_,    "BabcBde",             'B',           VT("de")      },
                { L_,    "NabcBde",             'B',           VT("de")      },
                { L_,    "PabcBdef",            'B',           VT("def")     },
                { L_,    "OabcBdef",            'B',           VT("def")     },
                { L_,    "BabcBdef",            'B',           VT("def")     },
                { L_,    "NabcBdef",            'B',           VT("def")     },

                { L_,    "VN",                  'N',           VT()          },
                { L_,    "PN",                  'N',           VT()          },
                { L_,    "ON",                  'N',           VT()          },
                { L_,    "BN",                  'N',           VT()          },
                { L_,    "NN",                  'N',           VT()          },
                { L_,    "PaN",                 'N',           VT()          },
                { L_,    "OaN",                 'N',           VT()          },
                { L_,    "BaN",                 'N',           VT()          },
                { L_,    "NaN",                 'N',           VT()          },
                { L_,    "PabN",                'N',           VT()          },
                { L_,    "OabN",                'N',           VT()          },
                { L_,    "BabN",                'N',           VT()          },
                { L_,    "NabN",                'N',           VT()          },
                { L_,    "PabcN",               'N',           VT()          },
                { L_,    "OabcN",               'N',           VT()          },
                { L_,    "BabcN",               'N',           VT()          },
                { L_,    "NabcN",               'N',           VT()          },
                { L_,    "PabcNd",              'N',           VT("d")       },
                { L_,    "OabcNd",              'N',           VT("d")       },
                { L_,    "BabcNd",              'N',           VT("d")       },
                { L_,    "NabcNd",              'N',           VT("d")       },
                { L_,    "PabcNde",             'N',           VT("de")      },
                { L_,    "OabcNde",             'N',           VT("de")      },
                { L_,    "BabcNde",             'N',           VT("de")      },
                { L_,    "NabcNde",             'N',           VT("de")      },
                { L_,    "PabcNdef",            'N',           VT("def")     },
                { L_,    "OabcNdef",            'N',           VT("def")     },
                { L_,    "BabcNdef",            'N',           VT("def")     },
                { L_,    "NabcNdef",            'N',           VT("def")     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE           = DATA[i].d_lineNum;
                const char       *SPEC           = DATA[i].d_spec;
                const char        EXPECTED_TYPE  = DATA[i].d_expectedType;
                const ValueType&  EXPECTED_VALUE = DATA[i].d_expectedValue;

                Obj        mX(ALLOC);
                const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) {
                    T_ P(SPEC)
                }

                if (veryVeryVerbose) {
                    T_ T_ P(X)
                }

                switch (EXPECTED_TYPE) {
                  case 'V': {   // void
                    typedef void Type;
                    LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                  } break;
                  case 'P': {   // TestType_PrintMethod
                    typedef TestType_PrintMethod Type;

                    const ValueType& value
                                   = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                    LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                    LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                       EXPECTED_VALUE == value);
                  } break;
                  case 'O': {   // TestType_OStreamOperator
                    typedef TestType_OStreamOperator Type;

                    const ValueType& value
                                   = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                    LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                    LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                       EXPECTED_VALUE == value);
                  } break;
                  case 'B': {   // TestType_Bdex
                    typedef TestType_Bdex Type;

                    const ValueType& value
                                   = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                    LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                    LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                       EXPECTED_VALUE == value);
                  } break;
                  case 'N': {   // TestType_NoTraits
                    typedef TestType_NoTraits Type;

                    const ValueType& value
                                   = X.TEMPLATE_MEMBER_BUG the<Type>().value();

                    LOOP_ASSERT(LINE, X.TEMPLATE_MEMBER_BUG is<Type>());
                    LOOP3_ASSERT(LINE, EXPECTED_VALUE,   value,
                                       EXPECTED_VALUE == value);
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // spec string
                int         d_index;    // offending character index
            } DATA[] = {
                //line   spec                    index
                //----   ----                    -----
                { L_,    "a",                    0     },
                { L_,    "@",                    0     },
                { L_,    "[",                    0     },
                { L_,    "U",                    0     },
                { L_,    "W",                    0     },
                { L_,    "M",                    0     },
                { L_,    "Q",                    0     },
                { L_,    "A",                    0     },
                { L_,    "C",                    0     },

                { L_,    "Va",                   1     },
                { L_,    "V@",                   1     },
                { L_,    "V[",                   1     },
                { L_,    "VU",                   1     },
                { L_,    "VW",                   1     },
                { L_,    "VM",                   1     },
                { L_,    "VQ",                   1     },
                { L_,    "VA",                   1     },
                { L_,    "VC",                   1     },

                { L_,    "PU",                   1     },
                { L_,    "PW",                   1     },
                { L_,    "PM",                   1     },
                { L_,    "PQ",                   1     },
                { L_,    "PA",                   1     },
                { L_,    "PC",                   1     },
                { L_,    "PaU",                  2     },
                { L_,    "PaW",                  2     },
                { L_,    "PaM",                  2     },
                { L_,    "PaQ",                  2     },
                { L_,    "PaA",                  2     },
                { L_,    "PaC",                  2     },
                { L_,    "PabU",                 3     },
                { L_,    "PabW",                 3     },
                { L_,    "PabM",                 3     },
                { L_,    "PabQ",                 3     },
                { L_,    "PabA",                 3     },
                { L_,    "PabC",                 3     },
                { L_,    "PabcU",                4     },
                { L_,    "PabcW",                4     },
                { L_,    "PabcM",                4     },
                { L_,    "PabcQ",                4     },
                { L_,    "PabcA",                4     },
                { L_,    "PabcC",                4     },

                { L_,    "OU",                   1     },
                { L_,    "OW",                   1     },
                { L_,    "OM",                   1     },
                { L_,    "OQ",                   1     },
                { L_,    "OA",                   1     },
                { L_,    "OC",                   1     },
                { L_,    "OaU",                  2     },
                { L_,    "OaW",                  2     },
                { L_,    "OaM",                  2     },
                { L_,    "OaQ",                  2     },
                { L_,    "OaA",                  2     },
                { L_,    "OaC",                  2     },
                { L_,    "OabU",                 3     },
                { L_,    "OabW",                 3     },
                { L_,    "OabM",                 3     },
                { L_,    "OabQ",                 3     },
                { L_,    "OabA",                 3     },
                { L_,    "OabC",                 3     },
                { L_,    "OabcU",                4     },
                { L_,    "OabcW",                4     },
                { L_,    "OabcM",                4     },
                { L_,    "OabcQ",                4     },
                { L_,    "OabcA",                4     },
                { L_,    "OabcC",                4     },

                { L_,    "BU",                   1     },
                { L_,    "BW",                   1     },
                { L_,    "BM",                   1     },
                { L_,    "BQ",                   1     },
                { L_,    "BA",                   1     },
                { L_,    "BC",                   1     },
                { L_,    "BaU",                  2     },
                { L_,    "BaW",                  2     },
                { L_,    "BaM",                  2     },
                { L_,    "BaQ",                  2     },
                { L_,    "BaA",                  2     },
                { L_,    "BaC",                  2     },
                { L_,    "BabU",                 3     },
                { L_,    "BabW",                 3     },
                { L_,    "BabM",                 3     },
                { L_,    "BabQ",                 3     },
                { L_,    "BabA",                 3     },
                { L_,    "BabC",                 3     },
                { L_,    "BabcU",                4     },
                { L_,    "BabcW",                4     },
                { L_,    "BabcM",                4     },
                { L_,    "BabcQ",                4     },
                { L_,    "BabcA",                4     },
                { L_,    "BabcC",                4     },

                { L_,    "NU",                   1     },
                { L_,    "NW",                   1     },
                { L_,    "NM",                   1     },
                { L_,    "NQ",                   1     },
                { L_,    "NA",                   1     },
                { L_,    "NC",                   1     },
                { L_,    "NaU",                  2     },
                { L_,    "NaW",                  2     },
                { L_,    "NaM",                  2     },
                { L_,    "NaQ",                  2     },
                { L_,    "NaA",                  2     },
                { L_,    "NaC",                  2     },
                { L_,    "NabU",                 3     },
                { L_,    "NabW",                 3     },
                { L_,    "NabM",                 3     },
                { L_,    "NabQ",                 3     },
                { L_,    "NabA",                 3     },
                { L_,    "NabC",                 3     },
                { L_,    "NabcU",                4     },
                { L_,    "NabcW",                4     },
                { L_,    "NabcM",                4     },
                { L_,    "NabcQ",                4     },
                { L_,    "NabcA",                4     },
                { L_,    "NabcC",                4     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_lineNum;
                const char *SPEC  = DATA[i].d_spec;
                const int   INDEX = DATA[i].d_index;

                Obj mX(ALLOC);

                if (veryVerbose) {
                    T_ P_(INDEX); P(SPEC);
                }

                int result = ggg(&mX, SPEC, veryVerbose);
                LOOP2_ASSERT(LINE, result, INDEX == result);
            }
        }

        if (verbose) cout << "\nEnd of Primitive Generator Function 'gg' Test."
                          << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //   This test will verify that the primary manipulators ar working as
        //   expected.
        //
        // Concerns:
        //   When a value is passed to the object, the object must morph to the
        //   type of the value passed.  Also, the default constructor should
        //   create an object that does not hold a value value.  We also need
        //   to make sure that the destructor works.
        //
        // Plan:
        //   First, verify the default constructor by testing that the
        //   resulting object does not hold any value.
        //
        //   Next, verify that the 'assign' methods work by constructing a
        //   series of independent objects using the default constructor and
        //   running 'assign' using various value types.  Verify the type and
        //   value held by the object using the basic accessors.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdeut_GenericVariant(b_A *ba = 0);
        //   ~bdeut_GenericVariant();
        //   BOOTSTRAP: bdeut_GenericVariant& assign<TYPE>();
        //   BOOTSTRAP: bdeut_GenericVariant& assign<TYPE>(c TYPE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators (Bootstrap)"
                          << "\n========================================"
                          << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            Obj mX(ALLOC);  const Obj& X = mX;
            if (veryVerbose) { T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<void>());
        }

        if (verbose) cout << "\nTesting 'assign<TYPE>()'." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'void'." << endl;
            typedef void Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'int'." << endl;
            typedef int Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(Type() == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'std::string'." << endl;
            typedef std::string Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(Type() == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_PrintMethod'." << endl;
            typedef TestType_PrintMethod Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(Type() == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_OStreamOperator'."
                                  << endl;
            typedef TestType_OStreamOperator Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(Type() == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_Bdex'." << endl;
            typedef TestType_Bdex Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(Type() == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_NoTraits'." << endl;
            typedef TestType_NoTraits Type;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign<Type>();
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(Type() == X.TEMPLATE_MEMBER_BUG the<Type>());
        }

        if (verbose) cout << "\nTesting 'assign<TYPE>(const TYPE& value)'."
                          << endl;
        {
            if (veryVerbose) cout << "\tUsing 'int'." << endl;
            typedef int Type;

            const Type VALUE = 123;

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign(VALUE);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(VALUE == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'std::string'." << endl;
            typedef std::string Type;

            const Type VALUE = "Hello World";

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign(VALUE);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(VALUE == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_PrintMethod'." << endl;
            typedef TestType_PrintMethod Type;

            const Type VALUE = ValueType("Hello World");

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign(VALUE);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(VALUE == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_OStreamOperator'."
                                  << endl;
            typedef TestType_OStreamOperator Type;

            const Type VALUE = ValueType("Hello World");

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign(VALUE);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(VALUE == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_Bdex'." << endl;
            typedef TestType_Bdex Type;

            const Type VALUE = ValueType("Hello World");

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign(VALUE);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(VALUE == X.TEMPLATE_MEMBER_BUG the<Type>());
        }
        {
            if (veryVerbose) cout << "\tUsing 'TestType_NoTraits'." << endl;
            typedef TestType_NoTraits Type;

            const Type VALUE = ValueType("Hello World");

            Obj mX(ALLOC);  const Obj& X = mX;
            mX.assign(VALUE);
            if (veryVeryVerbose) { T_ T_ P(X); }
            ASSERT(X.TEMPLATE_MEMBER_BUG is<Type>());
            ASSERT(VALUE == X.TEMPLATE_MEMBER_BUG the<Type>());
        }

        if (verbose) cout << "\nEnd of Primary Manipulators (Bootstrap) Test."
                          << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'update' and 'reset'.
        //     - basic accessors: 'view'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //    1. Create an object x1 (init. to VA)  { x1:VA                  }
        //    2. Create an object x2 (copy of x1)   { x1:VA x2:VA            }
        //    3. Create an object x3 (default ctor) { x1:VA x2:VA x3:U       }
        //    4. Create an object x4 (copy of x3)   { x1:VA x2:VA x3:U  x4:U }
        //    5. Set x3 using 'assign' (set to VB)  { x1:VA x2:VA x3:VB x4:U }
        //    6. Change x1 using 'reset'            { x1:U  x2:VA x3:VB x4:U }
        //    7. Change x1 ('assign', set to VC)    { x1:VC x2:VA x3:VB x4:U }
        //    8. Assign x2 = x1                     { x1:VC x2:VC x3:VB x4:U }
        //    9. Assign x2 = x3                     { x1:VC x2:VB x3:VB x4:U }
        //   10. Assign x1 = x1 (aliasing)          { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_length;   // length of source input
        } DATA[] = {
            //line  source                   length
            //----  ------                   ------
            { L_,   "",                      0        },
            { L_,   "\x00\x01\x02\x03\x04",  5        },
            { L_,   "\x01\x02\x03\x04\x05",  5        },
            { L_,   "\x02\x03\x04\x05\x06",  5        },
        };

        // possible types
        typedef int         TA;
        typedef double      TB;
        typedef std::string TC;
        typedef void        TU;

        // possible values
        const TA VA = 123;
        const TB VB = 38.23;
        const TC VC = "Hello World";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(ALLOC);  mX1.assign(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(X1.TEMPLATE_MEMBER_BUG is<TA>());
        ASSERT(VA == X1.TEMPLATE_MEMBER_BUG the<TA>());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1, ALLOC);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(X2.TEMPLATE_MEMBER_BUG is<TA>());
        ASSERT(VA == X2.TEMPLATE_MEMBER_BUG the<TA>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3(ALLOC);  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.TEMPLATE_MEMBER_BUG is<TU>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3, ALLOC);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.TEMPLATE_MEMBER_BUG is<TU>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using 'assign' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.assign(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(X3.TEMPLATE_MEMBER_BUG is<TB>());
        ASSERT(VB == X3.TEMPLATE_MEMBER_BUG the<TB>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.TEMPLATE_MEMBER_BUG is<TU>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('assign', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.assign(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.TEMPLATE_MEMBER_BUG is<TC>());
        ASSERT(VC == X1.TEMPLATE_MEMBER_BUG the<TC>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(X2.TEMPLATE_MEMBER_BUG is<TC>());
        ASSERT(VC == X2.TEMPLATE_MEMBER_BUG the<TC>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(X2.TEMPLATE_MEMBER_BUG is<TB>());
        ASSERT(VB == X2.TEMPLATE_MEMBER_BUG the<TB>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.TEMPLATE_MEMBER_BUG is<TC>());
        ASSERT(VC == X1.TEMPLATE_MEMBER_BUG the<TC>());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        if (verbose) cout << "\nEnd of Breathing Test." << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    LOOP2_ASSERT(test, g_objectCount, 0 == g_objectCount);

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
