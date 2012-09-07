// bsltf_templatetestfacility.h                                       -*-C++-*-
#ifndef INCLUDED_BSLTF_TEMPLATETESTFACILITY
#define INCLUDED_BSLTF_TEMPLATETESTFACILITY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to help with testing templates.
//
//@CLASSES:
//  bsltf::TemplateTestFacility: namespace for template-testing utilities
//
//@SEE_ALSO: bsltf_simpletesttype, bslstl_map
//
//@AUTHOR: Chen He (che2)
//
//@DESCRIPTION: When testing a container template having a type parameter, we
// need to ensure that the template supports its contractually specified
// categories of parameter types.  The 'bsltf' package provides a
// representative set of types intended for testing that can be used as
// template parameters for doing this kind of verification.
//
// Creating a separate test for each category of types supported by a template
// would be cumbersome.  Instead, writing a single templatized test is usually
// preferable.  Unfortunately, different types often require different syntaxes
// for constructing an object and getting an object's value.  This
// inconsistency makes writing generic code rather difficult.
//
// This component provides a solution with a utility 'struct',
// 'TemplateTestFacility', that defines two class method templates, 'create'
// and 'getIdentifier', that respectively have consistent syntaxes for creating
// objects and getting a integer value representing the state of objects of a
// parameterized type.
//
// This component also provides a macro,
// 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE', that serves as a convenient way
// to instantiate and invoke a template (for testing) having a type parameter
// for a specified list of types.  In addition, this component provides a set
// of macros referring to commonly useful lists of types intended for testing
// that can be used as arguments to 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.
//
// The utilities and macros provided by this component, along with the types
// defined in the 'bsltf' package, are explained in more detail in the
// following sections.
//
///'TemplateTestFacility'
///----------------------
// The 'TemplateTestFacility' 'struct' provides the following static (class)
// method templates to construct objects and get the states of objects of a
// supported parameterized type (supported types are those types intended for
// testing defined in the 'bsltf' package):
//
//: o 'create':    Return an object of the parameterized 'TYPE' whose value is
//:                uniquely associated with a specified integer identifier.
//
//: o 'getIdentifier': Return the integer identifier used to create a specified
//:                    object of the parameterized 'TYPE'.
//
///Macros and Test Types
///---------------------
// The 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro instantiates a
// specified class template for a specified list of types and call a specified
// class method of each instantiation.  The macro takes in arguments in the
// following order:
//
//: o The name of the class template to be instantiated
//:
//: o The name of the class method to be invoked
//:
//: o The names of the types for which the class template will be instantiated
//:   (up 20)
//
// This component also defines a set of macros, each providing a list of types,
// that can be used as the last argument to
// 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'.  The following is a brief
// synopsis of these macros (note that all macros names have the
// 'BSLTF_TEMPLATETESTFACILITY_' prefix, which is omitted for layout
// efficiency):
//
//: o 'TEST_TYPES_PRIMITIVE':     list of primitive types
//: o 'TEST_TYPES_USER_DEFINED':  list of user-defined types
//: o 'TEST_TYPES_REGULAR':       list of typically used types
//: o 'TEST_TYPES_AWKWARD':       list of types with odd behaviors
//: o 'TEST_TYPES_ALL':           list of all of the types
//
// The 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE' macro refers to a
// representative set of primivite types that are useful for testing:
//..
//  Type                                Description
//  ----                                -----------
//  signed char                         signed character
//
//  size_t                              signed integral type
//
//  TemplateTestFacility::ObjectPtr     pointer to an object
//
//  TemplateTestFacility::FunctionPtr   pointer to a function
//
//  TemplateTestFacility::MethodPtr     pointer to a method
//..
// The 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED' macro refers to a
// representative set of user-defined types that are useful for testing (note
// that all types described here belong to the 'bsltf' namespace, which is not
// explicitly qualified for layout efficiency):
//..
//  Type                                Description
//  ----                                -----------
//  EnumeratedTestType::Enum            an enumeration
//
//  UnionTestType                       a union
//
//  SimpleTestType                      class with no special traits defined
//
//  AllocTestType                       class that allocates memory, defines
//                                      the
//                                      'bslalg::TypeTraitUsesBslmaAllocator'
//                                      trait, and ensures it is not bitwise
//                                      moved
//
//  BitwiseMoveableTestType             class that is bitwise-moveable and
//                                      defines the
//                                      'bslalg::TypeTraitBitwiseMoveable'
//                                      trait
//
//  AllocatingBitwiseMoveableTestType   class that allocates memory, is
//                                      bitwisemoveable, and defines the
//                                      'bslalg::TypeTraitUsesBslmaAllocator'
//                                      and 'bslalg::TypeTraitBitwiseMoveable'
//                                      traits
//
//  NonTypicalOverloadsTestType         class that defines and assert on
//                                      invocation of certain
//                                      non-typically-overloaded operators
//                                      ('operator new', 'operator delete',
//                                      'operator&') to ensure that they are
//                                      not called
//..
//
// The 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR' macro refers to the
// union of the types provided by
// 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE' and
// 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED'.  These types are
// designed to work within the regular operating conditions of a typical
// template.  Typically, a test driver for a template instantiates its tests
// (using the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro) for all of the
// types refered by 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR'.
//
// The 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD' macro refers to a set of
// types that have certain attributes that make them unlikely to work for all
// of the operations of a template.  Typically, not all methods of a template
// are instantiable with these types, so these types are most often used
// independently in tests explicitly designed for a (single) type.
//..
//  Type                                Description
//  ----                                -----------
//  NonAssignableTestType               class having no assignment operator
//
//  NonCopyConstructibleTestType        class having no copy constructor (Note
//                                      that this class can not be created with
//                                      'TemplateTestFacility::create' because
//                                      the class method returns the newly
//                                      constructed object by value.)
//
//  NonDefaultConstructibleTestType     class having no default constructor
//
//  NonEqualComparableTestType          class having no equality comparison
//                                      operators
//..
// The 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL' refers to all the test types
// provided in the 'bsltf' package.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' Macro
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use
// 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' to call a class method of a
// template for a list of types.
//
// First, we define a 'struct' template 'TestTemplate' taking in a
// parameterized 'TYPE' that has a class method, 'printTypeName':
//..
//  template <class TYPE>
//  struct TestTemplate {
//      // This 'struct' provides a namespace for a simple test method.
//
//      // CLASS METHODS
//      static void printTypeName();
//          // Prints the name of the parameterized 'TYPE' to the console.
//  };
//
//  template <>
//  void TestTemplate<int>::printTypeName()
//  {
//      printf("int\n");
//  }
//
//  template <>
//  void TestTemplate<char>::printTypeName()
//  {
//      printf("char\n");
//  }
//
//  template <>
//  void TestTemplate<double>::printTypeName()
//  {
//      printf("double\n");
//  }
//..
// Now, we can instantiate the 'TestTemplate' class for each of the types
// 'int', 'char', and 'double', and call the 'printTypeName' class method of
// each instantiation using the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE'
// macro:
//..
//  BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestTemplate,
//                                           printTypeName,
//                                           int, char, double);
//..
// Finally, we observe the console output:
//..
//  int
//  char
//  double
//..
//
///Example 2: Writing a Type Independent Test Driver
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate using the 'TemplateTestFacility' 'struct'
// and the macros provided by this component to test the default constructor
// and primary manipulator of a class template in the context of a typical
// BDE-style test driver.  Note that a goal of the demonstrated test is to
// validate the class template with a broad range of types emulating those with
// which the template might be instantiated.
//
// First, we define a simple class template, 'MyNullableValue', that we will
// later need to test:
//..
//  template <class TYPE>
//  class MyNullableValue {
//      // This (value-semantic) class template extends the parameterized
//      // 'TYPE' to include the notion of a "null" value.
//
//      // DATA
//      TYPE d_value;     // non-null value
//      bool d_nullFlag;  // flag to indicate if the value is null
//
//    public:
//      MyNullableValue()
//          // Create a 'MyNullableValue' that initially has a value of null.
//      : d_nullFlag(true)
//      {
//      }
//
//      bool isNull() const
//          // Return 'true' if this object is null, and 'false' otherwise.
//      {
//          return d_nullFlag;
//      }
//
//      void makeNull()
//          // Set this object to the null value.
//      {
//          d_nullFlag = true;
//      }
//
//      const TYPE& value() const {
//          // Return a reference providing non-modifiable access to the
//          // underlying object of the parameterized 'TYPE'.  The behavior is
//          // undefined if the object is null.
//          return d_value;
//      }
//
//      void makeValue(const TYPE& value)
//          // Set the value of this object to be that of the specified 'value'
//          // of the parameterized 'TYPE'.
//      {
//          d_nullFlag = false;
//          d_value = value;
//      }
//  };
//..
// Then, we define some aliases for the micros that will be used by the test
// driver:
//..
//  #define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
//  #define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
//..
// Next, we define a 'struct' template, 'MyTestDriver', that provides a
// namespace containing the test cases (here, only 'testCase2' is defined for
// brevity) of the test driver:
//..
//  template <class TYPE>
//  struct MyTestDriver {
//      // This 'struct' provides a namespace for the class methods used to
//      // implement the test driver.
//
//      // TYPES
//      typedef MyNullableValue<TYPE> Obj;
//          // This 'typedef' provides an alias to the type under testing.
//
//      static void testCase2();
//          // Test primary manipulators.
//  };
//..
// Now, we define the implementation of 'MyTestDriver::testCase2':
//..
//  template <class TYPE>
//  void MyTestDriver<TYPE>::testCase2()
//  {
//      // --------------------------------------------------------------------
//      // DEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR
//      //   Ensure that we can use the default constructor to create an
//      //   object (having the default-constructed value), use the primary
//      //   manipulators to put that object into any state relevant for
//      //   thorough testing, and use the destructor to destroy it safely.
//      //
//      // Concerns:
//      //: 1 An object created using the default constructor (with or without
//      //:   a supplied allocator) has the contractually specified value.
//      //:
//      //: 2 The 'makeValue' method sets the value of a object to any
//      //:   specified value.
//      //:
//      //: 3 The 'makeNull' method set the value of a object to null.
//      //:
//      //: 4 Objects of different values can coexist.
//      //:
//      //: 5 The destructor does not modify other objects.
//      //
//      // Plan:
//      //: 1 Default-construct an object and use the (as yet unproven) salient
//      //:   attribute accessors to verify that the value of the object is the
//      //:   null value.  (C-1)
//      //:
//      //: 2 Default-construct another object, and use the 'makeValue' method,
//      //:   to set the value of the object to a non-null value.  Use the (as
//      //:   yet unproven) salient attribute accessors to verify that the new
//      //:   object has the expected value and the object created in P-1 still
//      //:   has the same value.  (C-2, 4)
//      //:
//      //: 3 Using the loop-based approach, for each identifier in a range of
//      //:   integer identifieres:
//      //:
//      //:   1 Default-construct a modifiable object, 'mL', and use the (as
//      //:     yet unproven) salient attribute accessors to verify the value
//      //:     of the default constructed object is the null value.  (C-1)
//      //:
//      //:   2 Create an object of the parameterized 'TYPE', 'LV', using the
//      //:     'TemplateTestFacility::create' class method template,
//      //:     specifying the integer loop identifier.
//      //:
//      //:   3 Use the 'makeValue' method to set the value of 'mL' to 'LV'.
//      //:     Use the (as yet unproven) salient attribute accessors to verify
//      //:     'mL' has the expected value.  (C-2)
//      //:
//      //:   4 Invoke the 'makeNull' method of 'mL'.  Use the attribute
//      //:     accessors to verify the value of the object is now null.  (C-3)
//      //:
//      //: 4 Create an object in a nested block.  Below the block, verify the
//      //:   objects created in P-1 and P-2 still have the same value.  (C-5)
//      //
//      // Testing:
//      //   MyNullableValue();
//      //   ~MyNullableValue();
//      //   void makeNull();
//      //   void MakeValue(const TYPE& value);
//      // --------------------------------------------------------------------
//
//      if (verbose)
//                  printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, AND DTOR"
//                         "\n============================================\n");
//
//
//      if (verbose) printf("\nTesting default constructor.\n");
//
//      Obj mW; const Obj& W = mW;
//      ASSERT(true == W.isNull());
//
//      Obj mX; const Obj& X = mX;
//      const TYPE XV = TemplateTestFacility::create<TYPE>(1);
//      mX.makeValue(XV);
//      ASSERT(1 == TemplateTestFacility::getIdentifier<TYPE>(X.value()));
//
//      if (verbose) printf("\nTesting primary manipulators.\n");
//
//      for (size_t ti = 0; ti < 10; ++ti) {
//
//          if (veryVerbose) { T_ P(ti) }
//
//          Obj mL; const Obj& L = mL;
//          ASSERT(true == L.isNull());
//
//          const TYPE LV = TemplateTestFacility::create<TYPE>(ti);
//
//          mL.makeValue(LV);
//          ASSERT(false == L.isNull());
//          ASSERT(LV == L.value());
//
//          mL.makeNull();
//          ASSERT(true == L.isNull());
//      }
//
//      if (verbose) printf("\nTesting destructor.\n");
//      {
//          Obj Z;
//      }
//
//      ASSERT(true == W.isNull());
//      ASSERT(XV == X.value());
//  }
//..
// Notice that, we create objects of the parameterized 'TYPE' using the
// 'TemplateTestFacility::create' class method template specifying an integer
// identifier; the created object has a value that is uniquely associated with
// the integer identifier.
//
// Also notice that we verified that an object of the parameterized 'TYPE' has
// the expected value in two ways:
//
//: 1 By equal comparing (1) the integer identifier returned from calling the
//:   'TemplateTestFacility::getIdentifier' class method template (specifying
//:   the object), and (2) the integer identifier uniquely associated with the
//:   expected state of the object.
//:
//: 2 By directly using the equality comparison operator for the parameterized
//:   'TYPE'.  Note that the equality comparison operator is defined for all
//:   types intended for testing in the 'bsltf' package except for
//:   'bsltf::NonEqualComparableTestType'.
//
// Finally, we use the 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro to
// instantiate 'MyTestDriver' for each of the types listed in
// 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR' and invoke the 'testCase2'
// class method of each instantiation:
//..
//  case 2: {
//    // ----------------------------------------------------------------------
//    // DEFAULT CTOR & PRIMARY MANIPULATORS
//    // ----------------------------------------------------------------------
//
//    if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
//                        "\n===================================\n");
//
//    RUN_EACH_TYPE(MyTestDriver, testCase2, TEST_TYPES_REGULAR);
//  } break;
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_ALLOCBITWISEMOVEABLETESTTYPE
#include <bsltf_allocbitwisemoveabletesttype.h>
#endif

#ifndef INCLUDED_BSLTF_ALLOCTESTTYPE
#include <bsltf_alloctesttype.h>
#endif

#ifndef INCLUDED_BSLTF_BITWISEMOVEABLETESTTYPE
#include <bsltf_bitwisemoveabletesttype.h>
#endif

#ifndef INCLUDED_BSLTF_ENUMERATEDTESTTYPE
#include <bsltf_enumeratedtesttype.h>
#endif

#ifndef INCLUDED_BSLTF_NONASSIGNABLETESTTYPE
#include <bsltf_nonassignabletesttype.h>
#endif

#ifndef INCLUDED_BSLTF_NONDEFAULTCONSTRUCTIBLETESTTYPE
#include <bsltf_nondefaultconstructibletesttype.h>
#endif

#ifndef INCLUDED_BSLTF_NONEQUALCOMPARABLETESTTYPE
#include <bsltf_nonequalcomparabletesttype.h>
#endif

#ifndef INCLUDED_BSLTF_NONTYPICALOVERLOADSTESTTYPE
#include <bsltf_nontypicaloverloadstesttype.h>
#endif

#ifndef INCLUDED_BSLTF_SIMPLETESTTYPE
#include <bsltf_simpletesttype.h>
#endif

#ifndef INCLUDED_BSLTF_UNIONTESTTYPE
#include <bsltf_uniontesttype.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <stddef.h>  // for 'size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTDIO
#include <stdio.h>  // for 'printf'
#define INCLUDED_CSTDIO
#endif

namespace BloombergLP {
namespace bsltf {

                        // ====================================
                        // class TemplateTestFacility_StubClass
                        // ====================================

class TemplateTestFacility_StubClass {
    // This class provides a single method template, 'method', that simply
    // returns the parameterized integer 'IDENTIFIER'.  Note that the purpose
    // of this class is to serve as a stub class and method to be referred by
    // 'TemplateTestFacility::ObjectPtr' and 'TemplateTestFacility::MethodPtr'
    // respectively.

  public:
    template <int IDENTIFIER>
    int method();
        // Return the parameterized 'IDENTIFIER'.
};

                        // ===========================
                        // struct TemplateTestFacility
                        // ===========================

struct TemplateTestFacility {
    // This 'struct' provides a namespace that contains three aliases for types
    // intended to be used as template parameters for testing templates.  The
    // namespace also contain two class method templates, 'create' and
    // 'getIdentifier', that respectively provides a consistent interface to
    // (1) create a specified object of a parameterized type from an integer
    // identifier and (2) get the identifier value of a specified object.  The
    // identifier value returned from the 'getIdentifier' method is undefined
    // unless the specified object was originally created with the 'create'
    // class method template.

    // PUBLIC TYPES
    typedef TemplateTestFacility_StubClass *ObjectPtr;
        // This 'typedef' is an alias for a pointer to a
        // 'TemplateTestFacility_StubClass' object.

    typedef void (*FunctionPtr) ();
        // This 'typedef' is an alias for a pointer to a function pointer.

    typedef int (TemplateTestFacility_StubClass::*MethodPtr) ();
        // This 'typedef' is an alias for a pointer to a method of
        // 'TemplateTestFacility_StubClass' taking no arguments and returning
        // an 'int'.

    // CLASS METHODS
    template <class TYPE>
    static TYPE create(int identifier);
        // Return a new object of the parameterized 'TYPE' whose state is
        // unique for the specified 'identifier'.  The behavior is undefined
        // unless '0 <= value < 128' and 'TYPE' is contained in the macro
        // 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL'.

    template <class TYPE>
    static int getIdentifier(const TYPE& object);
        // Return the integer identifier that uniquely identifies the specified
        // 'object'.  The behavior is undefined unless 'object' could be
        // created from the 'TemplateTestFacility::create' class method
        // template.
};

// FREE FUNCTIONS
void debugprint(const EnumeratedTestType::Enum& obj);
void debugprint(const UnionTestType& obj);
void debugprint(const SimpleTestType& obj);
void debugprint(const AllocTestType& obj);
void debugprint(const BitwiseMoveableTestType& obj);
void debugprint(const AllocBitwiseMoveableTestType& obj);
void debugprint(const NonTypicalOverloadsTestType& obj);
void debugprint(const NonDefaultConstructibleTestType& obj);
    // Print the value of the specified 'obj' to the console.  Note that this
    // free function is provided to allow 'bsls_bsltestutil' to support these
    // types intended for testing.  See the component-level documentation for
    // 'bsls_bsltestutil' for more details.

                       // =================
                       // Macro Definitions
                       // =================

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE                       \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr,                             \
        bsltf::TemplateTestFacility::MethodPtr
    // This macro refers to all of the primitive test types defined in this
    // package.  Note that it can be used as the last argument to the
    // 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro.

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED                    \
    bsltf::EnumeratedTestType::Enum,                                          \
    bsltf::UnionTestType,                                                     \
    bsltf::SimpleTestType,                                                    \
    bsltf::AllocTestType,                                                     \
    bsltf::BitwiseMoveableTestType,                                           \
    bsltf::AllocBitwiseMoveableTestType,                                      \
    bsltf::NonTypicalOverloadsTestType
    // This macro refers to all of the user-defined test types defined in this
    // package.  Note that the macro can be used as the last argument to the
    // 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro.

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR                         \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,                      \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED
    // This macro refers to all of the regular test types defined in this
    // package.  Note that the macro can be used as the last argument to the
    // 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro.

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD                         \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType,                               \
        bsltf::NonEqualComparableTestType
    // This macro refers to all of the awkward test types defined in this
    // package.  Note that the macro can be used as the last argument to the
    // 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro.

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL                             \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD
    // This macro refers to all of the test types defined in this package.
    // Note that the macro can be used as the last argument to the
    // 'BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE' macro.


#define BSLTF_TEMPLATETESTFACILITY_NUM_ARGS_IMPL(X20, X19, X18, X17, X16,     \
                                                 X15, X14, X13, X12, X11,     \
                                                 X10,  X9,  X8,  X7,  X6,     \
                                                  X5,  X4,  X3,  X2,  X1,     \
                                                   N, ...)                    \
        N

// The 'BSLTF_TEMPLATETESTFACILITY_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of some macros.
#define BSLTF_TEMPLATETESTFACILITY_EXPAND(X)                                  \
        X

#define BSLTF_TEMPLATETESTFACILITY_NUM_ARGS(...)                              \
        BSLTF_TEMPLATETESTFACILITY_EXPAND(                                    \
        BSLTF_TEMPLATETESTFACILITY_NUM_ARGS_IMPL(__VA_ARGS__,                 \
                                                 20, 19, 18, 17, 16, 15, 14,  \
                                                 13, 12, 11, 10,  9,  8,  7,  \
                                                 6, 5, 4, 3, 2, 1, 0))

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL1(C, M, T1)              \
        C<T1>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL2(C, M, T1, T2)          \
        C<T1>::M(); C<T2>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL3(C, M, T1, T2, T3)      \
        C<T1>::M(); C<T2>::M(); C<T3>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL4(C, M, T1, T2, T3, T4)  \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL5(C, M, T1, T2, T3, T4,  \
                                                       T5)                    \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL6(C, M, T1, T2, T3, T4,  \
                                                       T5, T6)                \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL7(C, M, T1, T2, T3, T4,  \
                                                       T5, T6, T7)            \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL8(C, M, T1, T2, T3, T4,  \
                                                       T5, T6, T7, T8)        \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL9(C, M, T1, T2, T3, T4,  \
                                                       T5, T6, T7, T8, T9)    \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL10(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10)                  \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL11(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11)             \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL12(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12)        \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL13(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13)   \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL14(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14)                  \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL15(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14, T15)             \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M(); C<T15>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL16(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14, T15, T16)        \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M(); C<T15>::M();      \
        C<T16>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL17(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14, T15, T16, T17)   \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M(); C<T15>::M();      \
        C<T16>::M(); C<T17>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL18(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14, T15, T16, T17,   \
                                                        T18)                  \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M(); C<T15>::M();      \
        C<T16>::M(); C<T17>::M(); C<T18>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL19(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14, T15, T16, T17,   \
                                                        T18, T19)             \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M(); C<T15>::M();      \
        C<T16>::M(); C<T17>::M(); C<T18>::M(); C<T19>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL20(C, M, T1, T2, T3, T4, \
                                                        T5, T6, T7, T8, T9,   \
                                                        T10, T11, T12, T13,   \
                                                        T14, T15, T16, T17,   \
                                                        T18, T19, T20)        \
        C<T1>::M(); C<T2>::M(); C<T3>::M(); C<T4>::M(); C<T5>::M();           \
        C<T6>::M(); C<T7>::M(); C<T8>::M(); C<T9>::M(); C<T10>::M();          \
        C<T11>::M(); C<T12>::M(); C<T13>::M(); C<T14>::M(); C<T15>::M();      \
        C<T16>::M(); C<T17>::M(); C<T18>::M(); C<T19>::M(); C<T20>::M();

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL(C, M, N, ...)           \
        BSLTF_TEMPLATETESTFACILITY_EXPAND(                                    \
         BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL ## N(C, M, __VA_ARGS__))

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_(C, M, N, ...)               \
        BSLTF_TEMPLATETESTFACILITY_EXPAND(                                    \
           BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_IMPL(C, M, N, __VA_ARGS__))

#define BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(CLASS, METHOD, ...)          \
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE_(                            \
                            CLASS,                                            \
                            METHOD,                                           \
                            BSLTF_TEMPLATETESTFACILITY_NUM_ARGS(__VA_ARGS__), \
                            __VA_ARGS__)                                      \
        // Instantiates the specified 'CLASS' for each of the types specified
        // in the third to last arguments of this macro.  Call the specified
        // class 'METHOD' of each instantiation.  The compilation will fail if
        // the number of specified types is more than 20.

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(TYPE)          \
        inline void dbg_print(const TYPE& val)                                \
       { printf("%d", bsltf::TemplateTestFacility::getIdentifier<TYPE>(val)); }

#define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINTS()             \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                             bsltf::EnumeratedTestType::Enum) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                                        bsltf::UnionTestType) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                                       bsltf::SimpleTestType) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                                        bsltf::AllocTestType) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                              bsltf::BitwiseMoveableTestType) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                         bsltf::AllocBitwiseMoveableTestType) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                          bsltf::NonTypicalOverloadsTestType) \
                                                                              \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_DEFINE_DBG_PRINT(               \
                                      bsltf::NonDefaultConstructibleTestType)
    // Defines a list of 'dbg_print' overloads for use in the test driver.
    // FIXME: Change this to integrate with Alisdair's test driver print
    // facility once its ready.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------------------
                        // class TemplateTestFacility_StubClass
                        // ------------------------------------

template <int IDENTIFIER>
int TemplateTestFacility_StubClass::method()
{
    return IDENTIFIER;
}

                        // ---------------------------
                        // struct TemplateTestFacility
                        // ---------------------------

// CLASS METHODS
template <class TYPE>
inline
TYPE TemplateTestFacility::create(int identifier)
{
    return TYPE(identifier);
}

template <>
inline
EnumeratedTestType::Enum TemplateTestFacility::create<
                                      EnumeratedTestType::Enum>(int identifier)
{
    return static_cast<EnumeratedTestType::Enum>(identifier);
}

template <>
inline
TemplateTestFacility::MethodPtr TemplateTestFacility::create<
                               TemplateTestFacility::MethodPtr>(int identifier)
{
    BSLS_ASSERT(0 <= identifier);  BSLS_ASSERT(identifier < 128);

    switch (identifier) {
      case 0:   return &TemplateTestFacility_StubClass::method<0>;
      case 1:   return &TemplateTestFacility_StubClass::method<1>;
      case 2:   return &TemplateTestFacility_StubClass::method<2>;
      case 3:   return &TemplateTestFacility_StubClass::method<3>;
      case 4:   return &TemplateTestFacility_StubClass::method<4>;
      case 5:   return &TemplateTestFacility_StubClass::method<5>;
      case 6:   return &TemplateTestFacility_StubClass::method<6>;
      case 7:   return &TemplateTestFacility_StubClass::method<7>;
      case 8:   return &TemplateTestFacility_StubClass::method<8>;
      case 9:   return &TemplateTestFacility_StubClass::method<9>;
      case 10:  return &TemplateTestFacility_StubClass::method<10>;
      case 11:  return &TemplateTestFacility_StubClass::method<11>;
      case 12:  return &TemplateTestFacility_StubClass::method<12>;
      case 13:  return &TemplateTestFacility_StubClass::method<13>;
      case 14:  return &TemplateTestFacility_StubClass::method<14>;
      case 15:  return &TemplateTestFacility_StubClass::method<15>;
      case 16:  return &TemplateTestFacility_StubClass::method<16>;
      case 17:  return &TemplateTestFacility_StubClass::method<17>;
      case 18:  return &TemplateTestFacility_StubClass::method<18>;
      case 19:  return &TemplateTestFacility_StubClass::method<19>;
      case 20:  return &TemplateTestFacility_StubClass::method<20>;
      case 21:  return &TemplateTestFacility_StubClass::method<21>;
      case 22:  return &TemplateTestFacility_StubClass::method<22>;
      case 23:  return &TemplateTestFacility_StubClass::method<23>;
      case 24:  return &TemplateTestFacility_StubClass::method<24>;
      case 25:  return &TemplateTestFacility_StubClass::method<25>;
      case 26:  return &TemplateTestFacility_StubClass::method<26>;
      case 27:  return &TemplateTestFacility_StubClass::method<27>;
      case 28:  return &TemplateTestFacility_StubClass::method<28>;
      case 29:  return &TemplateTestFacility_StubClass::method<29>;
      case 30:  return &TemplateTestFacility_StubClass::method<30>;
      case 31:  return &TemplateTestFacility_StubClass::method<31>;
      case 32:  return &TemplateTestFacility_StubClass::method<32>;
      case 33:  return &TemplateTestFacility_StubClass::method<33>;
      case 34:  return &TemplateTestFacility_StubClass::method<34>;
      case 35:  return &TemplateTestFacility_StubClass::method<35>;
      case 36:  return &TemplateTestFacility_StubClass::method<36>;
      case 37:  return &TemplateTestFacility_StubClass::method<37>;
      case 38:  return &TemplateTestFacility_StubClass::method<38>;
      case 39:  return &TemplateTestFacility_StubClass::method<39>;
      case 40:  return &TemplateTestFacility_StubClass::method<40>;
      case 41:  return &TemplateTestFacility_StubClass::method<41>;
      case 42:  return &TemplateTestFacility_StubClass::method<42>;
      case 43:  return &TemplateTestFacility_StubClass::method<43>;
      case 44:  return &TemplateTestFacility_StubClass::method<44>;
      case 45:  return &TemplateTestFacility_StubClass::method<45>;
      case 46:  return &TemplateTestFacility_StubClass::method<46>;
      case 47:  return &TemplateTestFacility_StubClass::method<47>;
      case 48:  return &TemplateTestFacility_StubClass::method<48>;
      case 49:  return &TemplateTestFacility_StubClass::method<49>;
      case 50:  return &TemplateTestFacility_StubClass::method<50>;
      case 51:  return &TemplateTestFacility_StubClass::method<51>;
      case 52:  return &TemplateTestFacility_StubClass::method<52>;
      case 53:  return &TemplateTestFacility_StubClass::method<53>;
      case 54:  return &TemplateTestFacility_StubClass::method<54>;
      case 55:  return &TemplateTestFacility_StubClass::method<55>;
      case 56:  return &TemplateTestFacility_StubClass::method<56>;
      case 57:  return &TemplateTestFacility_StubClass::method<57>;
      case 58:  return &TemplateTestFacility_StubClass::method<58>;
      case 59:  return &TemplateTestFacility_StubClass::method<59>;
      case 60:  return &TemplateTestFacility_StubClass::method<60>;
      case 61:  return &TemplateTestFacility_StubClass::method<61>;
      case 62:  return &TemplateTestFacility_StubClass::method<62>;
      case 63:  return &TemplateTestFacility_StubClass::method<63>;
      case 64:  return &TemplateTestFacility_StubClass::method<64>;
      case 65:  return &TemplateTestFacility_StubClass::method<65>;
      case 66:  return &TemplateTestFacility_StubClass::method<66>;
      case 67:  return &TemplateTestFacility_StubClass::method<67>;
      case 68:  return &TemplateTestFacility_StubClass::method<68>;
      case 69:  return &TemplateTestFacility_StubClass::method<69>;
      case 70:  return &TemplateTestFacility_StubClass::method<70>;
      case 71:  return &TemplateTestFacility_StubClass::method<71>;
      case 72:  return &TemplateTestFacility_StubClass::method<72>;
      case 73:  return &TemplateTestFacility_StubClass::method<73>;
      case 74:  return &TemplateTestFacility_StubClass::method<74>;
      case 75:  return &TemplateTestFacility_StubClass::method<75>;
      case 76:  return &TemplateTestFacility_StubClass::method<76>;
      case 77:  return &TemplateTestFacility_StubClass::method<77>;
      case 78:  return &TemplateTestFacility_StubClass::method<78>;
      case 79:  return &TemplateTestFacility_StubClass::method<79>;
      case 80:  return &TemplateTestFacility_StubClass::method<80>;
      case 81:  return &TemplateTestFacility_StubClass::method<81>;
      case 82:  return &TemplateTestFacility_StubClass::method<82>;
      case 83:  return &TemplateTestFacility_StubClass::method<83>;
      case 84:  return &TemplateTestFacility_StubClass::method<84>;
      case 85:  return &TemplateTestFacility_StubClass::method<85>;
      case 86:  return &TemplateTestFacility_StubClass::method<86>;
      case 87:  return &TemplateTestFacility_StubClass::method<87>;
      case 88:  return &TemplateTestFacility_StubClass::method<88>;
      case 89:  return &TemplateTestFacility_StubClass::method<89>;
      case 90:  return &TemplateTestFacility_StubClass::method<90>;
      case 91:  return &TemplateTestFacility_StubClass::method<91>;
      case 92:  return &TemplateTestFacility_StubClass::method<92>;
      case 93:  return &TemplateTestFacility_StubClass::method<93>;
      case 94:  return &TemplateTestFacility_StubClass::method<94>;
      case 95:  return &TemplateTestFacility_StubClass::method<95>;
      case 96:  return &TemplateTestFacility_StubClass::method<96>;
      case 97:  return &TemplateTestFacility_StubClass::method<97>;
      case 98:  return &TemplateTestFacility_StubClass::method<98>;
      case 99:  return &TemplateTestFacility_StubClass::method<99>;
      case 100: return &TemplateTestFacility_StubClass::method<100>;
      case 101: return &TemplateTestFacility_StubClass::method<101>;
      case 102: return &TemplateTestFacility_StubClass::method<102>;
      case 103: return &TemplateTestFacility_StubClass::method<103>;
      case 104: return &TemplateTestFacility_StubClass::method<104>;
      case 105: return &TemplateTestFacility_StubClass::method<105>;
      case 106: return &TemplateTestFacility_StubClass::method<106>;
      case 107: return &TemplateTestFacility_StubClass::method<107>;
      case 108: return &TemplateTestFacility_StubClass::method<108>;
      case 109: return &TemplateTestFacility_StubClass::method<109>;
      case 110: return &TemplateTestFacility_StubClass::method<110>;
      case 111: return &TemplateTestFacility_StubClass::method<111>;
      case 112: return &TemplateTestFacility_StubClass::method<112>;
      case 113: return &TemplateTestFacility_StubClass::method<113>;
      case 114: return &TemplateTestFacility_StubClass::method<114>;
      case 115: return &TemplateTestFacility_StubClass::method<115>;
      case 116: return &TemplateTestFacility_StubClass::method<116>;
      case 117: return &TemplateTestFacility_StubClass::method<117>;
      case 118: return &TemplateTestFacility_StubClass::method<118>;
      case 119: return &TemplateTestFacility_StubClass::method<119>;
      case 120: return &TemplateTestFacility_StubClass::method<120>;
      case 121: return &TemplateTestFacility_StubClass::method<121>;
      case 122: return &TemplateTestFacility_StubClass::method<122>;
      case 123: return &TemplateTestFacility_StubClass::method<123>;
      case 124: return &TemplateTestFacility_StubClass::method<124>;
      case 125: return &TemplateTestFacility_StubClass::method<125>;
      case 126: return &TemplateTestFacility_StubClass::method<126>;
      case 127: return &TemplateTestFacility_StubClass::method<127>;
    }
    return 0;
}

template <class TYPE>
inline
int TemplateTestFacility::getIdentifier(const TYPE& object)
{
    return int(object);
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::EnumeratedTestType::Enum>(
                                 const bsltf::EnumeratedTestType::Enum& object)
{
    return static_cast<int>(object);
}

template <>
inline
int TemplateTestFacility::getIdentifier<
    bsltf::TemplateTestFacility::ObjectPtr>(
                             const bsltf::TemplateTestFacility::ObjectPtr& ptr)
{

    bsls::Types::IntPtr value = reinterpret_cast<bsls::Types::IntPtr>(ptr);
    return static_cast<int>(value);
}

template <>
inline
int TemplateTestFacility::getIdentifier<
    bsltf::TemplateTestFacility::FunctionPtr>(
                           const bsltf::TemplateTestFacility::FunctionPtr& ptr)
{
    bsls::Types::IntPtr value = reinterpret_cast<bsls::Types::IntPtr>(ptr);
    return static_cast<int>(value);
}

template <>
inline
int TemplateTestFacility::getIdentifier<
    bsltf::TemplateTestFacility::MethodPtr>(
                             const bsltf::TemplateTestFacility::MethodPtr& ptr)
{
    TemplateTestFacility_StubClass object;
    return (object.*ptr)();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::UnionTestType>(
                                            const bsltf::UnionTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::SimpleTestType>(
                                           const bsltf::SimpleTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::AllocTestType>(
                                            const bsltf::AllocTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::BitwiseMoveableTestType>(
                                  const bsltf::BitwiseMoveableTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::AllocBitwiseMoveableTestType>(
                             const bsltf::AllocBitwiseMoveableTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::NonTypicalOverloadsTestType>(
                              const bsltf::NonTypicalOverloadsTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::NonAssignableTestType>(
                             const bsltf::NonAssignableTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<
    bsltf::NonDefaultConstructibleTestType>(
                          const bsltf::NonDefaultConstructibleTestType& object)
{
    return object.data();
}

template <>
inline
int TemplateTestFacility::getIdentifier<bsltf::NonEqualComparableTestType>(
                               const bsltf::NonEqualComparableTestType& object)
{
    return object.data();
}

// FREE FUNCTIONS
inline
void debugprint(const EnumeratedTestType::Enum& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const UnionTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const SimpleTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const AllocTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const BitwiseMoveableTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const AllocBitwiseMoveableTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const NonTypicalOverloadsTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
}

inline
void debugprint(const NonDefaultConstructibleTestType& obj)
{
    printf("%d", bsltf::TemplateTestFacility::getIdentifier(obj));
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
