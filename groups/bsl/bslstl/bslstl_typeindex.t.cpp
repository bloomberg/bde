// bslstl_typeindex.t.cpp                                             -*-C++-*-
#include <bslstl_typeindex.h>

#include <bslstl_function.h>
#include <bslstl_sharedptr.h>
#include <bslstl_unorderedmap.h>

#include <bslh_defaulthashalgorithm.h>
#include <bslh_hash.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_outputredirector.h>
#include <bsls_platform.h>

#include <algorithm>    // 'swap' prior to C++11
#include <utility>      // 'swap' in C++11 or later

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
# include <unordered_set>
#endif

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp'

using namespace BloombergLP;

// ============================================================================
//                      WORKAROUND MACROS FOR COMPILER BUGS
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
# if defined(BSLS_PLATFORM_CMP_GNU)
    // gcc does not correctly encode the 'noexcept' part of a function type
    // into its 'typeid', so disable testing of that feature.  Last tested with
    // gcc 9.1.0.  Bug report filed:
    //..
    //  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83534
    //..
#   undef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
# endif
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
# if defined(BSLS_PLATFORM_CMP_MSVC)
    // MSVC does not correctly parse pointer-to-rvalue-ref-qualified-member
    // function types inside operators such as 'sizeof' and 'typeid'.  It has
    // no issues with these types outside of such operators, nor using a
    // typedef to such a type with the operator expression.  Likewise it
    // correctly parses pointer-to-lvalue-ref-qualified-member functions in all
    // contexts.
#   define BSLSTL_TYPEINDEX_MSVC_RVALUE_QUALIFIER_BUG   1
# endif
#endif

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bsl::type_index' is an in-core value-semantic attribute type.  As such, the
// test driver follows a pre-set formula consisting of a breathing test, then
// tests of the value constructor (acting as basic manipulator), test
// machinery, basic accessors (which double up for the comparison operators),
// then printing, copy construction, swap, and assignment, followed by any
// remaining accessors, manipulators, and global test concerns, and finally a
// real-world usage example.
//
// Primary Manipulators:
//: o value constructor 'type_index(const std::type_index &target)'
//
// Basic Accessors:
//: o 'operator=='
//: o 'operator!='
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//
// As all of the contracts in this component are wide, there are no negative
// testing concerns.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] type_index(const std::type_info& target) noexcept;
// [ 7] type_index(const type_index& original) noexcept;
// [ 7] type_index(type_index&& original) noexcept;
// [ 2] ~type_index();
//
// MANIPULATORS
// [ 9] type_index& operator=(const type_index& rhs) noexcept;
// [ 9] type_index& operator=(type_index&& rhs) noexcept;
//
// ACCESSORS
// [ 4] bool operator==(const type_index& other) const noexcept;
// [ 4] bool operator!=(const type_index& other) const noexcept;
// [10] bool operator<(const type_index& other) const noexcept;
// [10] bool operator<=(const type_index& other) const noexcept;
// [10] bool operator>(const type_index& other) const noexcept;
// [10] bool operator>=(const type_index& other) const noexcept;
// [10] auto operator<=>(const type_index& other) const noexcept;
// [11] size_t hash_code() const noexcept;
// [11] const char* name() const noexcept;
//
// FREE FUNCTIONS
// [12] void hashAppend(HASHALG& alg, const type_index& object);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 3] CONCERN: test machinery functions as expected
// [ 5] void debugprint(const type_info& object);
// [ *] CONCERN: in no case does memory come from the global allocator.
// [ *] CONCERN: in no case does memory come from the default allocator.
// [ 6] REDUNDANT: test case for equality comparison
// [ 8] CONCERN: supports standard use of 'swap'
// [13] CONCERN: type detects as trivial for all relevant traits
// [14] CONCERN: works with 'std::hash'

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define ZU           BSLS_BSLTESTUTIL_FORMAT_ZU  // 'printf' flag for 'size_t'

// ============================================================================
//               CUSTOM TEST DRIVER MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
# define ASSERT_NOEXCEPT(...)   ASSERT(noexcept(__VA_ARGS__))
#else
# define ASSERT_NOEXCEPT(...)
#endif

namespace usage {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bsl::type_index'
///- - - - - - - - - - - - - - - - - - - - -
// Assume you are implementing a graphics library, and need to represent a
// variety of shapes.  You might have a simple hierarchy, such as:
//..
//  +-----------------------------------------------------------------+
//  |                                                                 |
//  |                           .---------.                           |
//  |                          (   Shape   )                          |
//  |                           `---------'                           |
//  |                           ^    ^    ^                           |
//  |                         /      |      \                         |
//  |                        /       |       \                        |
//  |                       /        |        \                       |
//  |              ,------.      ,--------.     ,-------.             |
//  |             ( Circle )    ( Triangle )   ( Polygon )            |
//  |              `------'      `--------'     `-------'             |
//  |                                                                 |
//  +-----------------------------------------------------------------+
//..
// In order to manage the creation of objects in our hierarchy, we might deploy
// the Abstract Factory pattern:
//     https://en.wikipedia.org/wiki/Abstract_factory_pattern
// using objects of type 'bsl::function<shared_ptr<Shape> >' as factories.
//
// First, we define our basic class hierarchy.
//..
    class Shape {
      public:
        virtual ~Shape() = 0;
            // Destroy this object.

        // Further details elided from example.
    };
//..
// Then, we create a utility class containing a registry of factory functions
// indexed by their corresponding 'std::type_info', using 'bsl::type_index' to
// provide the value-semantic wrapper needed for the key used in the container.
// This registry will enable us to abstract away different constructors of the
// concrete object types.
//..
    struct ShapeUtil {
        // This 'struct' provides a namespace for utilities to manage the
        // creation of objects implementing the 'Shape' protocol.

        // PUBLIC TYPES

        typedef bsl::function<bsl::shared_ptr<Shape>(int, int)> CreateFunction;
        typedef bsl::unordered_map<bsl::type_index, CreateFunction>
                                                               AbstractFactory;
        // CLASS METHODS

        template <class SHAPE_TYPE>
        static bsl::shared_ptr<Shape> make(int x, int y);
            // Return a 'shared_ptr' owning a newly created object of (template
            // parameter) 'SHAPE_TYPE' at the specified position '(x, y)' if
            // 'SHAPE_TYPE' has been registered with this utility, and an empty
            // 'shared_ptr' otherwise.

        template <class SHAPE_TYPE, class FACTORY>
        static bool registerFactory(FACTORY factory);
            // Register the specified 'factory' creating objects of (template
            // parameter) 'SHAPE_TYPE'; return 'true' if this is the first
            // successful attempt to register such a factory function, and
            // 'false' otherwise.

      private:
        static AbstractFactory s_registry;      // registry for factories
    };
//..
// Now, we can implement the register and make functions, using the standard
// 'typeid' operator to create the key values as needed.
//..
    template <class SHAPE_TYPE>
    bsl::shared_ptr<Shape> ShapeUtil::make(int x, int y) {
        AbstractFactory::iterator it = s_registry.find(typeid(SHAPE_TYPE));
        if (s_registry.end() == it) {
            return 0;                                                 // RETURN
        }

        return it->second(x, y);
    }

    template <class SHAPE_TYPE, class FACTORY>
    bool ShapeUtil::registerFactory(FACTORY factory) {
        return s_registry.emplace(typeid(SHAPE_TYPE), factory).second;
    }
//..
// Next, we provide several concrete implementations of our 'Shape' class, to
// demonstrate use of this hierarchy.
//..
    class Circle : public Shape {
        // This class represents a circle, described by a position and radius.

      public:
        // CREATORS

        Circle(int x, int y, int radius);
            // Create a 'Triangle' having the it center at the specified
            // position '(x, y)', and having the specified 'radius'.

        ~Circle();
            // Destroy this object.

        // Further details elided from example.
    };

    class Triangle : public Shape {
        // This class represents a triangle.

      public:
        // CREATORS

        Triangle(int x1, int y1, int x2, int y2, int x3, int y3);
            // Create a 'Triangle' having the specified vertices, '(x1, y1)',
            // '(x2, y2)', and '(x3, y3)'.

        ~Triangle();
            // Destroy this object.

        // Further details elided from example.
    };

    class Polygon : public Shape {
        // This class represents a polygon having an arbitrary number of
        // vertices.

      public:
        // CREATORS

        template <class ITERATOR>
        Polygon(ITERATOR firstPoint, ITERATOR endPoint);
            // Create a Polygon having vertices given by the specified range
            // '[firstPoint, endPoint)'.

        ~Polygon();
            // Destroy this object.

        // Further details elided from example.
    };
//..
// Then, we provide some simple factory functions to create some shapes at the
// specified coordinates.
//..
   bsl::shared_ptr<Shape> makeCircle(int x, int y)
       // Return a 'Circle' at the specified position '(x, y)'.
   {
       return bsl::make_shared<Circle>(x, y, 5);
   }

   bsl::shared_ptr<Shape> makeTriangle(int x, int y)
       // Return a 'Triangle' with its lower left vertex at the specified
       // position '(x, y)'.
   {
       return bsl::make_shared<Triangle>(x, y, x+3, y+4, x+6, y);
   }
//..
// Finally, we can exercise the whole system in a simple test driver.  Note
// that as we do not register a factory function for the 'Polygon' class, the
// attempt to create a 'Polygon' will fail.
//..
    int main()
        // Simulated test driver.
    {
        // Install a test allocator to confirm there are no memory leaks.
        bslma::TestAllocator         ta("Usage example default allocator");
        bslma::DefaultAllocatorGuard guard(&ta);

        bool registeredCircle = ShapeUtil::registerFactory<Circle>(makeCircle);
        ASSERT(registeredCircle);

        bool registeredTriangle =
                            ShapeUtil::registerFactory<Triangle>(makeTriangle);
        ASSERT(registeredTriangle);

        bsl::shared_ptr<Shape> example = ShapeUtil::make<Circle>(10, 10);
        ASSERT(0 != bsl::dynamic_pointer_cast<Circle>(example));

        example = ShapeUtil::make<Triangle>(10, 10);
        ASSERT(0 != bsl::dynamic_pointer_cast<Triangle>(example));

        example = ShapeUtil::make<Polygon>(10, 10);
        ASSERT(0 == example);

        return 0;
    }
//..

// ============================================================================
//              IMPLEMENTATION DETAILS ELIDED TO SIMPLIFY THE EXAMPLE
// ----------------------------------------------------------------------------

                                // -----------
                                // class Shape
                                // -----------

ShapeUtil::AbstractFactory ShapeUtil::s_registry(
                                      bslma::NewDeleteAllocator::allocator(0));

                                // -----------
                                // class Shape
                                // -----------

// CREATORS

inline
Shape::~Shape()
{
}

                                // ------------
                                // class Circle
                                // ------------

// CREATORS

inline
Circle::Circle(int, int, int)
{
    // Constructor argument are ignored for the purposes of this example.
}

inline
Circle::~Circle()
{
}

                                // --------------
                                // class Triangle
                                // --------------

// CREATORS

inline
Triangle::Triangle(int, int, int, int, int, int)
{
    // Constructor argument are ignored for the purposes of this example.
}

inline
Triangle::~Triangle()
{
}

                                // -------------
                                // class Polygon
                                // -------------

// CREATORS

inline
Polygon::~Polygon()
{
}

}  // close namespace usage

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsl::type_index Obj;

// ============================================================================
//                     GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace bsl {

void debugprint(const type_index& object)
    // Print a human-readable representation of the specified 'object' to the
    // console, suitable to support debugging.
{
    using bsls::debugprint;

    printf("type_index{");
    debugprint(object.name());
    printf("}");
}

}  // close namespace bsl

namespace {

void sink(...) {}
    // This function swallows any scalar value without issuing a compiler
    // warning.  It is intended to support testing that there is only one
    // overload of a given function name within a class.

}  // close unnamed namespace

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT_DATA for test cases that stress a broad range of types across
// the type system, looking for QoI concerns if type information collides for
// some types on a specific platform.  The type system is rich in variations of
// function types, function pointer types, and pointer-to-member types, so
// exhaustively covering that space as most likely to uncover a collision.
// There is no need to test cv-qualified types or reference types, as top-level
// cv-qualifiers and references are stripped by the 'typeid' operator.
// Likewise, abominable function types are not supported.  However,
// pointers-to-cv-qualified types should be distinct, and tested, as should
// pointers-to-cv-qualified-member-function types, and types with non-throwing
// exception specifications as of C++17.

namespace {

class Host; // incomplete type used to form pointer-to-member types for testing

const std::type_info *const DEFAULT_DATA[] = {
      &typeid(void)
    , &typeid(void *)
    , &typeid(volatile void *)
    , &typeid(void())
    , &typeid(int)
    , &typeid(int *)
    , &typeid(const int *)
    , &typeid(int [])
    , &typeid(int [2])
    , &typeid(int [2][4])
    , &typeid(int *[])
    , &typeid(int *[2])
    , &typeid(int *[2][4])
    , &typeid(int (*)[])
    , &typeid(int (*)[2])
    , &typeid(int (*)[2][4])
    , &typeid(int())
    , &typeid(int(...))
    , &typeid(int(int))
    , &typeid(int(int...))
    , &typeid(int(int, int))
    , &typeid(int(int, int...))
    , &typeid(int * ())
    , &typeid(int * (...))
    , &typeid(int(*)())
    , &typeid(int(*)(...))
    , &typeid(int(*)(int))
    , &typeid(int(*)(int...))
    , &typeid(int(*)(int, int))
    , &typeid(int(*)(int, int...))
    , &typeid(void(Host::*)())
    , &typeid(void(Host::*)(...))
    , &typeid(int Host::*)
    , &typeid(const int Host::*)
    , &typeid(int(Host::*)())
    , &typeid(int(Host::*)(...))
    , &typeid(int(Host::*)(int))
    , &typeid(int(Host::*)(int...))
    , &typeid(int(Host::*)(int, int))
    , &typeid(int(Host::*)(int, int...))
    , &typeid(int(Host::*)() const)
    , &typeid(int(Host::*)(...) const)
    , &typeid(int(Host::*)(int) const)
    , &typeid(int(Host::*)(int...) const)
    , &typeid(int(Host::*)(int, int) const)
    , &typeid(int(Host::*)(int, int...) const)
    , &typeid(int(Host::*)() volatile)
    , &typeid(int(Host::*)(...) volatile)
    , &typeid(int(Host::*)(int) volatile)
    , &typeid(int(Host::*)(int...) volatile)
    , &typeid(int(Host::*)(int, int) volatile)
    , &typeid(int(Host::*)(int, int...) volatile)
    , &typeid(int(Host::*)() const volatile)
    , &typeid(int(Host::*)(...) const volatile)
    , &typeid(int(Host::*)(int) const volatile)
    , &typeid(int(Host::*)(int...) const volatile)
    , &typeid(int(Host::*)(int, int) const volatile)
    , &typeid(int(Host::*)(int, int...) const volatile)
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    , &typeid(int(Host::*)() &)
    , &typeid(int(Host::*)(...) &)
    , &typeid(int(Host::*)(int) &)
    , &typeid(int(Host::*)(int...) &)
    , &typeid(int(Host::*)(int, int) &)
    , &typeid(int(Host::*)(int, int...) &)
    , &typeid(int(Host::*)() const &)
    , &typeid(int(Host::*)(...) const &)
    , &typeid(int(Host::*)(int) const &)
    , &typeid(int(Host::*)(int...) const &)
    , &typeid(int(Host::*)(int, int) const &)
    , &typeid(int(Host::*)(int, int...) const &)
    , &typeid(int(Host::*)() volatile &)
    , &typeid(int(Host::*)(...) volatile &)
    , &typeid(int(Host::*)(int) volatile &)
    , &typeid(int(Host::*)(int...) volatile &)
    , &typeid(int(Host::*)(int, int) volatile &)
    , &typeid(int(Host::*)(int, int...) volatile &)
    , &typeid(int(Host::*)() const volatile &)
    , &typeid(int(Host::*)(...) const volatile &)
    , &typeid(int(Host::*)(int) const volatile &)
    , &typeid(int(Host::*)(int...) const volatile &)
    , &typeid(int(Host::*)(int, int) const volatile &)
    , &typeid(int(Host::*)(int, int...) const volatile &)
# if !defined(BSLSTL_TYPEINDEX_MSVC_RVALUE_QUALIFIER_BUG)
    , &typeid(int(Host::*)() &&)
    , &typeid(int(Host::*)(...) &&)
    , &typeid(int(Host::*)(int) &&)
    , &typeid(int(Host::*)(int...) &&)
    , &typeid(int(Host::*)(int, int) &&)
    , &typeid(int(Host::*)(int, int...) &&)
    , &typeid(int(Host::*)() const &&)
    , &typeid(int(Host::*)(...) const &&)
    , &typeid(int(Host::*)(int) const &&)
    , &typeid(int(Host::*)(int...) const &&)
    , &typeid(int(Host::*)(int, int) const &&)
    , &typeid(int(Host::*)(int, int...) const &&)
    , &typeid(int(Host::*)() volatile &&)
    , &typeid(int(Host::*)(...) volatile &&)
    , &typeid(int(Host::*)(int) volatile &&)
    , &typeid(int(Host::*)(int...) volatile &&)
    , &typeid(int(Host::*)(int, int) volatile &&)
    , &typeid(int(Host::*)(int, int...) volatile &&)
    , &typeid(int(Host::*)() const volatile &&)
    , &typeid(int(Host::*)(...) const volatile &&)
    , &typeid(int(Host::*)(int) const volatile &&)
    , &typeid(int(Host::*)(int...) const volatile &&)
    , &typeid(int(Host::*)(int, int) const volatile &&)
    , &typeid(int(Host::*)(int, int...) const volatile &&)
# endif
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
    , &typeid(int() noexcept)
    , &typeid(int(...) noexcept)
    , &typeid(int(int) noexcept)
    , &typeid(int(int...) noexcept)
    , &typeid(int(int, int) noexcept)
    , &typeid(int(int, int...) noexcept)
    , &typeid(int(*)() noexcept)
    , &typeid(int(*)(...) noexcept)
    , &typeid(int(*)(int) noexcept)
    , &typeid(int(*)(int...) noexcept)
    , &typeid(int(*)(int, int) noexcept)
    , &typeid(int(*)(int, int...) noexcept)
    , &typeid(int(Host::*)() noexcept)
    , &typeid(int(Host::*)(...) noexcept)
    , &typeid(int(Host::*)(int) noexcept)
    , &typeid(int(Host::*)(int...) noexcept)
    , &typeid(int(Host::*)(int, int) noexcept)
    , &typeid(int(Host::*)(int, int...) noexcept)
    , &typeid(int(Host::*)() const noexcept)
    , &typeid(int(Host::*)(...) const noexcept)
    , &typeid(int(Host::*)(int) const noexcept)
    , &typeid(int(Host::*)(int...) const noexcept)
    , &typeid(int(Host::*)(int, int) const noexcept)
    , &typeid(int(Host::*)(int, int...) const noexcept)
    , &typeid(int(Host::*)() volatile noexcept)
    , &typeid(int(Host::*)(...) volatile noexcept)
    , &typeid(int(Host::*)(int) volatile noexcept)
    , &typeid(int(Host::*)(int...) volatile noexcept)
    , &typeid(int(Host::*)(int, int) volatile noexcept)
    , &typeid(int(Host::*)(int, int...) volatile noexcept)
    , &typeid(int(Host::*)() const volatile noexcept)
    , &typeid(int(Host::*)(...) const volatile noexcept)
    , &typeid(int(Host::*)(int) const volatile noexcept)
    , &typeid(int(Host::*)(int...) const volatile noexcept)
    , &typeid(int(Host::*)(int, int) const volatile noexcept)
    , &typeid(int(Host::*)(int, int...) const volatile noexcept)
      // noexcept type support implies support for ref qualifiers
    , &typeid(int(Host::*)() & noexcept)
    , &typeid(int(Host::*)(...) & noexcept)
    , &typeid(int(Host::*)(int) & noexcept)
    , &typeid(int(Host::*)(int...) & noexcept)
    , &typeid(int(Host::*)(int, int) & noexcept)
    , &typeid(int(Host::*)(int, int...) & noexcept)
    , &typeid(int(Host::*)() const & noexcept)
    , &typeid(int(Host::*)(...) const & noexcept)
    , &typeid(int(Host::*)(int) const & noexcept)
    , &typeid(int(Host::*)(int...) const & noexcept)
    , &typeid(int(Host::*)(int, int) const & noexcept)
    , &typeid(int(Host::*)(int, int...) const & noexcept)
    , &typeid(int(Host::*)() volatile & noexcept)
    , &typeid(int(Host::*)(...) volatile & noexcept)
    , &typeid(int(Host::*)(int) volatile & noexcept)
    , &typeid(int(Host::*)(int...) volatile & noexcept)
    , &typeid(int(Host::*)(int, int) volatile & noexcept)
    , &typeid(int(Host::*)(int, int...) volatile & noexcept)
    , &typeid(int(Host::*)() const volatile & noexcept)
    , &typeid(int(Host::*)(...) const volatile & noexcept)
    , &typeid(int(Host::*)(int) const volatile & noexcept)
    , &typeid(int(Host::*)(int...) const volatile & noexcept)
    , &typeid(int(Host::*)(int, int) const volatile & noexcept)
    , &typeid(int(Host::*)(int, int...) const volatile & noexcept)
# if !defined(BSLSTL_TYPEINDEX_MSVC_RVALUE_QUALIFIER_BUG)
    , &typeid(int(Host::*)() && noexcept)
    , &typeid(int(Host::*)(...) && noexcept)
    , &typeid(int(Host::*)(int) && noexcept)
    , &typeid(int(Host::*)(int...) && noexcept)
    , &typeid(int(Host::*)(int, int) && noexcept)
    , &typeid(int(Host::*)(int, int...) && noexcept)
    , &typeid(int(Host::*)() const && noexcept)
    , &typeid(int(Host::*)(...) const && noexcept)
    , &typeid(int(Host::*)(int) const && noexcept)
    , &typeid(int(Host::*)(int...) const && noexcept)
    , &typeid(int(Host::*)(int, int) const && noexcept)
    , &typeid(int(Host::*)(int, int...) const && noexcept)
    , &typeid(int(Host::*)() volatile && noexcept)
    , &typeid(int(Host::*)(...) volatile && noexcept)
    , &typeid(int(Host::*)(int) volatile && noexcept)
    , &typeid(int(Host::*)(int...) volatile && noexcept)
    , &typeid(int(Host::*)(int, int) volatile && noexcept)
    , &typeid(int(Host::*)(int, int...) volatile && noexcept)
    , &typeid(int(Host::*)() const volatile && noexcept)
    , &typeid(int(Host::*)(...) const volatile && noexcept)
    , &typeid(int(Host::*)(int) const volatile && noexcept)
    , &typeid(int(Host::*)(int...) const volatile && noexcept)
    , &typeid(int(Host::*)(int, int) const volatile && noexcept)
    , &typeid(int(Host::*)(int, int...) const volatile && noexcept)
# endif
#endif
    };

const size_t DEFAULT_NUM_DATA =
               static_cast<size_t>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);
}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;       // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    ASSERTV(bslma::Default::allocator(0) == &defaultAllocator);

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usage::main();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'std::hash'
        //   Ensure that 'bsl::type_index' can be stored in native unordered
        //   containers on platforms that support hashing.
        //
        // Concerns:
        //: 1 The native standard library containers can use 'bsl::type_index'
        //:   as their key type.
        //
        // Plan:
        //: 1 Create an object of type 'std::set<bsl::type_index>'.
        //:
        //: 2 For each value in the global table of test values:
        //:
        //:   1 Create a 'const' object of type 'bsl::type_index' having that
        //:     value.
        //:
        //:   2 Verify 'X' can be inserted into the 'unordered_set'. (C-1)
        //
        // Testing:
        //   CONCERN: works with 'std::hash'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'std::hash'"
                            "\n==================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        if (verbose) printf("\nTesting 'std::hash'\n");
        {
            std::unordered_set<bsl::type_index> container;

            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                const Obj X = *DEFAULT_DATA[i];

                bool inserted = container.insert(X).second;
                ASSERTV(i, X, inserted, inserted);
            }
        }
#else
        if (verbose) printf("'std::hash' is not supported on this platform\n");
#endif

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   Several type traits are used in dispatching to optimal algorithm
        //   implementations and data structures, so verify that the subset of
        //   interesting traits have the expected values.
        //
        // Concerns:
        //: 1 'type_index' is trivially copyable.
        //:
        //: 2 'type_index' is no-throw move constructible.
        //:
        //: 3 'type_index' is bitwise movable.
        //:
        //: 4 'type_index' is NOT bitwise equality comparable.
        //:
        //: 5 'type_index' does NOT use 'bslma' allocators.
        //
        // Plan:
        //: 1 Directly test each trait for the expected value. (C-1..5)
        //
        // Testing:
        //   CONCERN: type detects as trivial for all relevant traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        ASSERT( bsl::is_trivially_copyable<Obj>::value);
        ASSERT( bsl::is_nothrow_move_constructible<Obj>::value);

        ASSERT( bslmf::IsBitwiseMoveable<Obj>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<Obj>::value);

        ASSERT(!bslma::UsesBslmaAllocator<Obj>::value);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   ADL-discoverd 'hashAppend' is the key extension point for the
        //   'bslh' hashing framework, so we will use 'bslh::Hash<>' as a proxy
        //   for testing ADL-discoverability of the 'hashAppend' function.
        //
        // Concerns:
        //: 1 'hashAppend' produces the same result when given the same input
        //:   values.
        //:
        //: 2 'hashAppend' produces different results when given differing
        //:   input values.
        //:
        //: 3 'hashAppend' combines the hash value into the accumulated state,
        //:   i.e., does not simply replace the accumulated state.
        //:
        //: 4 Works for 'const' and non-'const' type-indices.
        //:
        //: 5 'hashAppend' for 'bsl::type_index' objects can be found via
        //:   argument dependant lookup.
        //
        // Plan:
        //: 1 Create a 'bslh::Hash' object to provide hashing functionality
        //:   that relies on ADL discovery.
        //:
        //: 2 For each combination of values obtained by a nested iteration of
        //:   the global test table, confirm that for any two distinct values,
        //:   'hashAppend' will produce distinct values (via the 'hasher'
        //:   proxy) (C-1,2,5):
        //:
        //:   1 On each iteration of the outer loop, create an object 'x', a
        //:     'const' reference to that object, 'X', and an immutable object
        //:     'V', all having the value corresponding to the same index in
        //:     the global table of test values.
        //:
        //:   2 Verify that 'hasher(x)' and 'hasher(V)' produce the same value.
        //:
        //:   3 Starting a nested loop at the next index, create a 'const'
        //:     object 'Y' having the value corresponding to the same row of
        //:     the global table of test values.
        //:
        //:   4 Verify that 'hasher(X)' and 'hasher(Y)' produce different
        //:     values.
        //:
        //: 3 For each combination of values obtained by a nested iteration of
        //:   the global test table, confirm that for any two distinct values,
        //:   'hashAppend' will produce distinct values (C-3,4):
        //:
        //:   1 In the outer loop, create reference objects 'X' and 'V' having
        //:     the value corresponding to the same index in the global table
        //:     of test values, and where lower-case 'x' is a modifiable
        //:     lvalue.
        //:
        //:   2 For each object, 'x' and 'V', create a separate hasher object
        //:     to accumulate state.
        //:
        //:   3 Verify that calling 'hashAppend' for each object with its own
        //:     hash algorithm object produces that same state.
        //:
        //:   4 Verify that the modifiable object 'x' still has the same value
        //:     as 'V'. (C-4)
        //:
        //:   5 Create another more hash algorithm object, and verify that
        //:     combining the hash of 'X' twice produces a distinct value to
        //:     hashing 'X' just the once.
        //:
        //:   6 Starting a nested loop at the next index, create a 'const'
        //:     object 'Y' having the value corresponding to the same row of
        //:     the global table of test values.
        //:
        //:   7 Create another hash algorithm object for 'Y', and verify that
        //:     it produces a different state than hashing 'X' or 'V'.
        //:
        //:   8 Create two more hash algorithm objects, and verify that
        //:     combining the hashes of 'X' and 'Y' produces a distinct third
        //:     hash value.
        //:
        //:   9 Create one more hash algorithm objects, and verify that
        //:     combining the hashes of 'X' and 'Y' in the other order produces
        //:     a distinct fourth hash value.
        //
        // Testing:
        //   void hashAppend(HASHALG& alg, const type_index& object);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        if (verbose) printf("\nTesting via 'bslh::Hash' as a proxy\n");
        {
            typedef ::BloombergLP::bslh::Hash<> Hasher;

            const Hasher hasher = {};

            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                Obj       x = *DEFAULT_DATA[i]; const Obj& X = x;
                const Obj V = X;

                ASSERTV(i, x, V, hasher(x) == hasher(V));
                ASSERTV(i, X, V, V == X);

                for (size_t j = i + 1; j != DEFAULT_NUM_DATA; ++j) {
                    const Obj Y = *DEFAULT_DATA[j];

                    ASSERTV(i, j, X, Y, hasher(X) != hasher(Y));
                }
            }
        }

        if (verbose) printf("\nTesting 'hashAppend' directly\n");
        {
            typedef bslh::DefaultHashAlgorithm Hasher;
            typedef Hasher::result_type        HashValue;

            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                Obj       x = *DEFAULT_DATA[i]; const Obj& X = x;
                const Obj V = X;

                Hasher hashModifiable;
                hashAppend(hashModifiable, x);

                HashValue hX = hashModifiable.computeHash();

                Hasher hashV;
                hashAppend(hashV, V);

                HashValue hV = hashV.computeHash();

                ASSERTV(i, X, V,          V ==  X);
                ASSERTV(i, X, V, hV, hX, hV == hX);

                Hasher hashDuplicate;
                hashAppend(hashDuplicate, X);
                hashAppend(hashDuplicate, X);

                HashValue hDuplicate = hashDuplicate.computeHash();

                ASSERTV(i, X, hDuplicate, hX, hDuplicate != hX);

                for (size_t j = i + 1; j != DEFAULT_NUM_DATA; ++j) {
                    const Obj Y = *DEFAULT_DATA[j];

                    Hasher hashY;
                    hashAppend(hashY, Y);

                    HashValue hY = hashY.computeHash();

                    ASSERTV(i, j, X, Y, hX, hY, hX != hY);

                    Hasher hashCombined;
                    hashAppend(hashCombined, X);
                    hashAppend(hashCombined, Y);

                    HashValue hCombined = hashCombined.computeHash();

                    ASSERTV(i, j, X, Y, hCombined, hX, hCombined != hX);
                    ASSERTV(i, j, X, Y, hCombined, hY, hCombined != hY);

                    Hasher hashRepeat;
                    hashAppend(hashRepeat, X);
                    hashAppend(hashRepeat, Y);

                    HashValue hRepeat = hashRepeat.computeHash();

                    ASSERTV(i, j, X, Y, hCombined,   hRepeat,
                                        hCombined == hRepeat);

                    Hasher hashReorder;
                    hashAppend(hashReorder, Y);
                    hashAppend(hashReorder, X);

                    HashValue hReorder = hashReorder.computeHash();

                    ASSERTV(i, j, X, Y, hReorder, hX, hReorder != hX);
                    ASSERTV(i, j, X, Y, hReorder, hY, hReorder != hY);
                    ASSERTV(i, j, X, Y, hReorder,   hRepeat,
                                        hReorder != hRepeat);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL ACCESSORS
        //   Note that it is the responsibility of the underlying platform to
        //   ensure the QoI concerns that each test value has distinct results
        //   for each of these methods.  We test this QoI to be aware of any
        //   platforms that do not give this level of support.
        //
        // Concerns:
        //: 1 QoI: Each distinct value has a unique 'name'.
        //:
        //: 2 QoI: Each distinct value has a unique 'hash_code'.
        //:
        //: 3 Two 'type_index' objects that have the same value return the same
        //:   value for each accessor.
        //:
        //: 4 Accessors are declared 'const'.
        //:
        //: 5 There are no non-'const' overloads of any accessors.
        //:
        //: 6 Each accessor has a non-throwing exception specification on
        //:   implementations that support the 'noexcept' operator.
        //
        // Plan:
        //: 1 For each combination of values obtained by a nested iteration of
        //:   the global test table, confirm that for any two distinct values,
        //:   the 'hash_code' and 'name' accessors produce distinct values:
        //:
        //:   1 In the outer loop, create reference objects 'X' and 'V' having
        //:     the value corresponding to the same index in the global table
        //:     of test values.
        //:
        //:   2 Verify that 'X.name()' produces the exact same string pointer
        //:     as the corresponding 'std::type_info' object. (QoI: C-3)
        //:
        //:   3 Verify that 'X.hash_code()' produces the exact same hash value
        //:     as the corresponding 'std::type_info' object on C++11, or that
        //:     two 'type_index' objects having the same value produce the same
        //:     hash value prior to C++11. (C-3)
        //:
        //:   4 Starting a nested loop at the next index, create a 'const'
        //:     object 'Y' having the value corresponding to the same row of
        //:     the global table of test values.
        //:
        //:   5 Compare the string values of the 'name' function of both 'X'
        //:     and 'Y' to confirm that the strings have different values.
        //:     (QoI: C-1) (C-4)
        //:
        //:   6 Compare the hash code values of both 'X' and 'Y' to confirm
        //:     that they do not collide. (QoI: C-2) (C-4)
        //:
        //: 2 Using the 'sink' function, verify there are no additional
        //:   overloads for each accessor, ensuring that 'const' and
        //:   non-'const' lvalues call the same function. (C-5)
        //:
        //: 3 Using the 'ASSERT_NOEXCEPT' macro, verify each accessor has a
        //:   non-throwing exception specification. (C-6)
        //
        // Testing:
        //   size_t hash_code() const noexcept;
        //   const char* name() const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ADDITIONAL ACCESSORS"
                            "\n============================\n");

        if (verbose) printf("\nTesting QoI: accessors have distinct values\n");
        {
            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                Obj       x = *DEFAULT_DATA[i]; const Obj& X = x;
                const Obj V = X;

                // QoI check: same pointer value is faster than 'strcmp'!
                ASSERTV(i, x, x.name() == DEFAULT_DATA[i]->name());
                ASSERTV(i, X, V, V == X);

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
                ASSERTV(i, x, x.hash_code() == DEFAULT_DATA[i]->hash_code());
#else
                ASSERTV(i, x, V, x.hash_code() == V.hash_code());
#endif
                ASSERTV(i, X, V, V == X);

                for (size_t j = i + 1; j != DEFAULT_NUM_DATA; ++j) {
                    const Obj Y = *DEFAULT_DATA[j];

                    ASSERTV(i, j, X, Y, 0 != strcmp(X.name(), Y.name()));
                    ASSERTV(i, j, X, Y, X.hash_code() != Y.hash_code());
                }
            }
        }

        if (verbose) printf("\nVerify there are no non-'const' overloads\n");
        {
            sink(&bsl::type_index::hash_code);
            sink(&bsl::type_index::name);
        }

        if (verbose) printf("\nTesting exception specifications\n");
        {
            const Obj X(typeid(int));
            ASSERT_NOEXCEPT(X.hash_code());
            ASSERT_NOEXCEPT(X.name());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=, <=>)
        //   Ensure that each operator defines the correct relationship between
        //   any two 'type_index' values.
        //
        // Concerns:
        //:  1 An object 'X' is in relation to an object 'Y' according to an
        //:    implementation specific ordering defined the member function
        //:    'type_info::before'.
        //:
        //:  2 'false == (X <  X)' (i.e., irreflexivity).
        //:
        //:  3 'true  == (X <= X)' (i.e., reflexivity).
        //:
        //:  4 'false == (X >  X)' (i.e., irreflexivity).
        //:
        //:  5 'true  == (X >= X)' (i.e., reflexivity).
        //:
        //:  6 If 'X < Y', then '!(Y < X)' (i.e., asymmetry).
        //:
        //:  7 'X <= Y' if and only if 'X < Y' exclusive-or 'X == Y'.
        //:
        //:  8 If 'X > Y', then '!(Y > X)' (i.e., asymmetry).
        //:
        //:  9 'X >= Y' if and only if 'X > Y' exclusive-or 'X == Y'.
        //:
        //: 10 'operator<=>' is consistent with '<', '>', '<=', '>='.
        //:
        //: 11 Non-modifiable objects can be compared (i.e., objects or
        //:    references providing only non-modifiable access).
        //:
        //: 12 Non-modifiable and modifiable objects produce the same result
        //:    when compared.
        //:
        //: 13 All comparisons have non-throwing exception specifications on
        //:    implementations that support the 'noexcept' operator.
        //:
        //
        // Plan:
        //: 1 For each combination of values obtained by a nested iteration of
        //:   the global test table, confirm that the 4 relational operators
        //:   report a consistent ordering that agrees with the ordering given
        //:   by 'std::type_info':
        //:
        //:   1 Iterate over the whole test table.  On each iteration, create
        //:     a constant reference object, 'X'.
        //:
        //:   2 Compare 'X' to itself using each of the 4 relational operators,
        //:     and verify the expected value for each self-comparison.
        //:     (C-2..5)
        //:
        //:   3 In a nested loop, iterate the whole table of test values again,
        //:     creating another constand reference object, 'Y', having the
        //:     corresponding value for each iteration.
        //:
        //:   4 Store a 'bool' value, 'XbeforeY', that reports the relative
        //:     ordering of the corresponding 'std::type_info' objects using
        //:     the 'type_info::before' member function.
        //:
        //:   5 Verify each of the 4 relational operators on object 'X' and 'Y'
        //:     have the correct value according to the oracle, 'XbeforeY'.
        //:     Note that both orderings are covered by performing a full
        //:     iteration in both loops, rather than using a reduced set on
        //:     each subsequent iteration. (C-6..11).
        //:
        //: 2 Using the 'sink' function, verify there are no additional member
        //:   overloads for each relational operator, ensuring that 'const' and
        //:   non-'const' lvalues call the same function. (C-12)
        //:
        //: 3 Using the 'ASSERT_NOEXCEPT' macro, verify each relational
        //:   operator has a non-throwing exception specification. (C-13)
        //
        // Testing:
        //   bool operator<(const type_index& other) const noexcept;
        //   bool operator<=(const type_index& other) const noexcept;
        //   bool operator>(const type_index& other) const noexcept;
        //   bool operator>=(const type_index& other) const noexcept;
        //   auto operator<=>(const type_index& other) const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf(
            "\nTESTING RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=, <=>)"
            "\n===========================================================\n");

        if (verbose) printf("\nTesting results of comparison operators\n");
        {
            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                const Obj X = *DEFAULT_DATA[i];

                // Directly (ultimately redundantly) test concerns 2-5

                ASSERTV( i, X, (X <  X) == false );
                ASSERTV( i, X, (X <= X) == true  );
                ASSERTV( i, X, (X >  X) == false );
                ASSERTV( i, X, (X >= X) == true  );

                // Test all other values

                for (size_t j = 0; j != DEFAULT_NUM_DATA; ++j) {
                    const Obj Y = *DEFAULT_DATA[j];

                    bool XbeforeY = DEFAULT_DATA[i]->before(*DEFAULT_DATA[j]);

                    ASSERTV( i, j, X, Y, (X <  Y) ==   XbeforeY              );
                    ASSERTV( i, j, X, Y, (X <= Y) == ( XbeforeY || (i == j)) );
                    ASSERTV( i, j, X, Y, (X >  Y) == (!XbeforeY && (i != j)) );
                    ASSERTV( i, j, X, Y, (X >= Y) ==  !XbeforeY              );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
                    const bsl::strong_ordering C = X <=> Y;
                    ASSERTV( i, j, X, Y, (C <  0) ==   XbeforeY              );
                    ASSERTV( i, j, X, Y, (C <= 0) == ( XbeforeY || (i == j)) );
                    ASSERTV( i, j, X, Y, (C >  0) == (!XbeforeY && (i != j)) );
                    ASSERTV( i, j, X, Y, (C >= 0) ==  !XbeforeY              );
#endif
                }
            }
        }

        if (verbose) printf("\nVerify there are no non-'const' overloads\n");
        {
            sink(&bsl::type_index::operator<);
            sink(&bsl::type_index::operator<=);
            sink(&bsl::type_index::operator>);
            sink(&bsl::type_index::operator>=);
        }

        if (verbose) printf("\nTesting exception specifications\n");
        {
            const Obj X(typeid(int));

            ASSERT_NOEXCEPT(X < X);
            ASSERT_NOEXCEPT(X <= X);
            ASSERT_NOEXCEPT(X > X);
            ASSERT_NOEXCEPT(X >= X);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
            ASSERT_NOEXCEPT(X <=> X);
#endif
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value, and the original is unchanged.  As the
        //   operations are both trivial, check that the move assignment
        //   operator has the same behavior.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself has no observable effect.
        //:   (alias-safety).
        //:
        //: 6 The move-assignment operator behaves identically to the copy-
        //:   assignment operator.
        //:
        //: 7 Both assignment operators have non-throwing exception
        //:   specifications on implementations that support 'noexcept'.
        //
        // Plan:
        //: 1 Assign the address of each assignment operator to a
        //:   pointer-to-member having the expected signature, including the
        //:   presence of a 'noexcept' specification in C++17.  Note that the
        //:   move-assignment operator will identically be the copy-assignment
        //:   operator on C++03 and earlier. (C-2)
        //:
        //: 2 For each combination of values obtained by a nested iteration of
        //:   the global test table, confirm that assigning a value to an
        //:   existing object has the expected behavior:
        //:
        //:   1 Iterate over the whole test table.  On each iteration, create
        //:     a constant reference object, 'X', and a modifiable object 'y'
        //:     having the same value.
        //:
        //:   2 Assign 'y' to itself, and verify that the value is unchanged by
        //:     comparing to 'X'. (c-5)
        //:
        //:   3 In a nested loop, iterate the whole table of test values again,
        //:     creating a non-'const' object 'z' having the corresponding
        //:     value for each iteration.
        //:
        //:   4 Assign 'y' to 'z', and verify that 'z' now has the same value
        //:     as 'y'. (C-1)
        //:
        //:   5 Verify that the value of 'y' is unchanged, and has the same
        //:     value as 'X'. (C-4)
        //:
        //:   6 Verify that the address of the returned reference is the same
        //:     as the address of 'z'. (C-3)
        //:
        //: 3 Repeat the above steps for the move-assignment operator by using
        //:   'bslmf::MovableRefUtil::move' to move from 'y'.  Note that the
        //:   moved-from state of a 'type_index' object is unchanged.  (C-6)
        //:
        //: 4 Using the 'ASSERT_NOEXCEPT' macro, verify assignment to an lvalue
        //:   from both an lvalue and rvalue has a non-throwing exception
        //:   specification. (C-7)
        //
        // Testing:
        //   type_index& operator=(const type_index& rhs) noexcept;
        //   type_index& operator=(type_index&& rhs) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY-ASSIGNMENT OPERATOR"
                            "\n================================\n");

        if (verbose) printf(
                      "\nAssign the address of the operator to a variable.\n");
        {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
# define LOCAL_NOEXCEPT noexcept
#else
# define LOCAL_NOEXCEPT
#endif

            typedef Obj& (Obj::*CopyOperatorPtr)(const Obj&) LOCAL_NOEXCEPT;

            // Verify that the signature and return type are standard.

            CopyOperatorPtr operatorCopyAssignment = &Obj::operator=;

            (void)operatorCopyAssignment;  // quash potential compiler warning

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)             \
 &&(!defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1800)
            // Note that MSVC does not implement implicit move-assignment
            // operator prior of MSVC 2015

            typedef Obj& (Obj::*MoveOperatorPtr)(Obj&&) LOCAL_NOEXCEPT;

            // Verify that the signature and return type are standard.

            MoveOperatorPtr operatorMoveAssignment = &Obj::operator=;

            (void)operatorMoveAssignment;  // quash potential compiler warning
#endif

#undef LOCAL_NOEXCEPT
        }

        for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
            const Obj X = *DEFAULT_DATA[i];
            Obj       y = *DEFAULT_DATA[i]; const Obj& Y = y;

            ASSERTV(i, X, Y, X == Y);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign"
#endif
            y = y;  // self-assignment
#ifdef __clang__
#pragma clang diagnostic pop
#endif

            ASSERTV(i, X, Y, X == Y);

            for (size_t j = 0; j != DEFAULT_NUM_DATA; ++j) {
                Obj z = *DEFAULT_DATA[j];       const Obj& Z = z;

                ASSERTV(i, j, Z, Y, (Z == Y) == (i == j));

                Obj& R = (z = y);

                ASSERTV(i, j, Z, Y, Z == Y);
                ASSERTV(i, j, &R, &Z, &R == &Z);

                ASSERTV(i, j, X, Y, X == Y);
            }
        }

        for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
            const Obj X = *DEFAULT_DATA[i];
            Obj       y = *DEFAULT_DATA[i]; const Obj& Y = y;

            ASSERTV(i, X, Y, X == Y);

            y = bslmf::MovableRefUtil::move(y);  // self-assignment

            ASSERTV(i, X, Y, X == Y);

            for (size_t j = 0; j != DEFAULT_NUM_DATA; ++j) {
                Obj z = *DEFAULT_DATA[j];       const Obj& Z = z;

                ASSERTV(i, j, Z, Y, (Z == Y) == (i == j));

                Obj& R = (z = bslmf::MovableRefUtil::move(y));

                ASSERTV(i, j, Z, Y, Z == Y);
                ASSERTV(i, j, &R, &Z, &R == &Z);

                ASSERTV(i, j, X, Y, X == Y);
            }
        }

        if (verbose) printf("\nTesting exception specifications\n");
        {
            Obj x(typeid(int));

            ASSERT_NOEXCEPT(x = x);
            ASSERT_NOEXCEPT(x = bslmf::MovableRefUtil::move(x));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //   'type_index' does not implement an ADL-discoverable 'swap', so
        //   verify the regular 'std::swap' function has the expected behavior.
        //
        // Concerns:
        //: 1 Calling 'swap' unqualified with 'std::swap' available through
        //:   ordinary name lookup exchanges the values of two 'type_index'
        //:   objects.
        //:
        //: 2 The 'swap' function has a non-throwing exception specification on
        //:   implementations that support 'noexcept'.
        //
        // Plan:
        //: 1 Using nested 'for' loops, verify that calling 'swap' for two
        //:   'type_index' lvalues having all possible combinations of value
        //:   from the global table of test values, exchanges those values.
        //:   (C-1)
        //:
        //: 2 Using the 'ASSERT_NOEXCEPT' macro, verify the 'swap' function
        //:   found by name-lookup has a non-throwing exception specification.
        //:   (C-2)
        //
        // Testing:
        //   CONCERN: supports standard use of 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        using std::swap;

        for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
            for (size_t j = 0; j != DEFAULT_NUM_DATA; ++j) {
                // Note: capitalised variables are copies, not references
                Obj x = *DEFAULT_DATA[i];   const Obj X = x;
                Obj y = *DEFAULT_DATA[i];   const Obj Y = y;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
                ASSERT(noexcept(swap(x, y)));
#endif

                swap(x, y);

                ASSERTV(i, j, X, Y, x == Y);
                ASSERTV(i, j, X, Y, X == y);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value, and the
        //   original is unchanged.  As the constructors are both trivial,
        //   check that the move constructor has the same behavior.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //:
        //: 4 The constructor is not explicit.
        //:
        //: 5 The move constructor behaves identically to the copy constructor.
        //:
        //: 6 Both constructors have non-throwing exception specifications on
        //:   implementations that support 'noexcept'.
        //
        // Plan:
        //: 1 For each value in the global table of test values:
        //:   1 Create a non-constant object 'x', to be copied, and a reference
        //:     to 'const' X for use in testing expressions.
        //:
        //:   2 Create a constant object 'Y' from the same row of the test
        //:     table, to act as a reference value after assignment.
        //:
        //:   3 Create another 'const' object, 'Z', that is a copy of 'x',
        //:     using copy-initialization. (C-4)
        //:
        //:   4 Verify 'Z' has the same value as 'X'.  (C-1)
        //:
        //:   5 Verify 'X' still has the same value as 'Y'  (C-3)
        //:
        //:   6 Create a final test object, 'A', that is a copy of a 'const'
        //:     lvalue.
        //:
        //:   7 Verify 'A' still has the same value as 'Z'  (C-2)
        //:
        //: 2 Repeat the previous steps using the move constructor instead of
        //:   the copy constructor. (C-5)
        //:
        //: 3 Using the 'ASSERT_NOEXCEPT' macro, verify construction of a
        //:   temporary from both an lvalue and rvalue has a non-throwing
        //:   exception specification (C-6).
        //
        // Testing:
        //   type_index(const type_index& original) noexcept;
        //   type_index(type_index&& original) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        if (verbose) printf("\nTesting copy constructor\n");
        {
            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                Obj       x = *DEFAULT_DATA[i]; const Obj& X = x;
                const Obj Y = *DEFAULT_DATA[i];
                const Obj Z = x;

                ASSERTV(i, X, Z, Z == X);
                ASSERTV(i, X, Y, Y == X);

                const Obj A = Z;
                ASSERTV(i, X, Z, Z == A);
            }
        }

        if (verbose) printf("\nTesting copy constructor\n");
        {
            for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
                Obj       x = *DEFAULT_DATA[i]; const Obj& X = x;
                const Obj Y = *DEFAULT_DATA[i];
                const Obj Z = bslmf::MovableRefUtil::move(x);

                ASSERTV(i, X, Z, Z == X);
                ASSERTV(i, X, Y, Y == X);

                const Obj A = bslmf::MovableRefUtil::move(Z);
                ASSERTV(i, X, Z, Z == A);
            }
        }

        if (verbose) printf("\nTesting exception specifications\n");
        {
            Obj x(typeid(int));

            ASSERT_NOEXCEPT(Obj(x));
            ASSERT_NOEXCEPT(Obj(bslmf::MovableRefUtil::move(x)));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY-COMPARISON OPERATORS
        //  The equality and inequality comparison operators are tested as
        //  basic accessors in test case 4.
        //
        // Concerns:
        //: 1 No new concerns
        //
        // Plan:
        //: 1 No plan needed
        //
        // Testing:
        //   REDUNDANT: test case for equality comparison
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY-COMPARISON OPERATORS"
                            "\n=====================================\n");

        if (verbose) printf("\nNothing more to test\n");

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'debugprint'
        //   Ensure that the value of the object can be formatted appropriately
        //   for printing through the test library macros in a human-readable
        //   form.
        //
        // Concerns:
        //: 1 'debugprint' writes a human-readable representation of a
        //:   'type_index' object.  That format is expected to be the string
        //:   'type_index{"%s"}' with '%s' replaced by the string corresponding
        //:   to the 'name()' of the wrapped 'type_info' object.
        //:
        //: 2 Writing a 'debugprint' string does not change the value of the
        //:   supplied 'type_index' object.
        //
        // Plan:
        //: 1 Using a 'bsls::OutputRedirector' component to capture output,
        //:   loop through the global test table of standard values, and verify
        //:   that the 'debugprint' representation of each object has the
        //:   expected form.  (C-1)
        //:
        //: 2 After each call to 'debugprint', verify that the original value
        //:   has not changed, using the primary accessor ('operator=='). (C-2)
        //
        // Testing:
        //   void debugprint(const type_info& object);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'debugprint'"
                            "\n====================\n");

        bsls::OutputRedirector redirect(
                                      bsls::OutputRedirector::e_STDOUT_STREAM);

        for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
            const Obj X = *DEFAULT_DATA[i];
            Obj       y = *DEFAULT_DATA[i];

            if (veryVerbose) { T_ P(X); }

            redirect.enable();

            debugprint(y);

            redirect.load();
            redirect.disable();

            ASSERTV(i, X, y, X == y);

            const char *text   = redirect.getOutput();
            const char *cursor = text;

            const char *S_PREFIX = "type_index{\"";
            for (size_t j = 0; S_PREFIX[j] != 0; ++j, ++cursor) {
                ASSERTV(i, j, text, S_PREFIX[j],   *cursor,
                                    S_PREFIX[j] == *cursor);

                if (*cursor == 0) {
                    break;
                }
            }

            for(const char *nameCursor = DEFAULT_DATA[i]->name();
                *nameCursor != 0;
                ++nameCursor) {
                ASSERTV(i, cursor, nameCursor, cursor,    nameCursor,
                                              *cursor == *nameCursor);
                if (*cursor == 0) {
                    break;
                }
                ++cursor;
            }

            const char *S_SUFFIX = "\"}";
            for (size_t j = 0; S_SUFFIX[j] != 0; ++j, ++cursor) {
                ASSERTV(i, j, text, S_SUFFIX[j],   *cursor,
                                    S_SUFFIX[j] == *cursor);

                if (*cursor == 0) {
                    break;
                }
            }

            const size_t LENGTH = cursor - text;
            ASSERTV(LENGTH,   redirect.outputSize(),
                    LENGTH == redirect.outputSize());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   The comparison operators, '==' and '!=', are the most basic
        //   accessors for this component.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they refer
        //:   to the same type information..
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 8 Non-modifiable and modifiable objects produce the same result
        //:   when compared.
        //:
        //: 9 All comparisons have non-throwing exception specifications on
        //:   implementations that support 'noexcept'.
        //:
        // Plan:
        //: 1 Perform a 2-dimensional iteration over the test data table,
        //:   'DEFAULT_DATA', using nested 'for' loops.
        //:
        //: 2 In the outer loop, create a 'const' qualified object and verify
        //:   that it compares equal to itself, and does not compare not-equal
        //:   to itself (identity test) (C-2..3).
        //:
        //: 3 In the inner loop create a second 'const' object, and verify that
        //:   both (distinct) object compare equal only if they are constructed
        //:   from the same element of the test table.  Full coverage of the
        //:   test matrix establishes commutativity, by testing each possible
        //:   ordering of the two values. (C-1,4..7)
        //:
        //: 4 Using the 'sink' function, verify there are no additional member
        //:   overloads for each relational operator, ensuring that 'const' and
        //:   non-'const' lvalues call the same function. (C-8)
        //:
        //: 5 Using the 'ASSERT_NOEXCEPT' macro, comparison expressions using
        //:   either operator have non-throwing exception specifications (C-9).
        //
        // Testing:
        //   bool operator==(const type_index& other) const noexcept;
        //   bool operator!=(const type_index& other) const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
            const Obj X = *DEFAULT_DATA[i];

            ASSERTV(i, X,   X == X  );
            ASSERTV(i, X, !(X != X) );

            for (size_t j = i; j != DEFAULT_NUM_DATA; ++j) {
                const Obj Y = *DEFAULT_DATA[j];

                ASSERTV(i, j, X, Y, (X == Y) == (i == j));
                ASSERTV(i, j, X, Y, (X != Y) == (i != j));
                ASSERTV(i, j, X, Y, (Y == X) == (i == j));
                ASSERTV(i, j, X, Y, (Y != X) == (i != j));
            }
        }

        if (verbose) printf("\nVerify there are no non-'const' overloads\n");
        {
            sink(&bsl::type_index::operator==);
#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
            sink(&bsl::type_index::operator!=);
#endif
        }

        if (verbose) printf("\nTesting exception specifications\n");
        {
            const Obj X(typeid(int));

            ASSERT_NOEXCEPT(X == X);
            ASSERT_NOEXCEPT(X != X);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING THE TEST MACHINERY
        //
        // Concerns:
        //: 1 Each type-id in the array 'DEFAULT_DATA' is distinct from each
        //:   other type-id in that array.
        //
        // Plan:
        //: 1 Loop through the 'DEFAULT_DATA' array ensuring that no stored
        //:   'type_info' object has the same value as any other object in the
        //:   array.  Optimize the nested loop to test only later values in the
        //:   array, as earlier values are already tested.
        //
        // Testing:
        //   CONCERN: test machinery functions as expected
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING THE TEST MACHINERY"
                            "\n==========================\n");

        // Ensure all type-ids in the default array are unique

        for (size_t i = 0; i != DEFAULT_NUM_DATA; ++i) {
            ASSERTV(i, *DEFAULT_DATA[i] == *DEFAULT_DATA[i]); // compiler test

            for (size_t j = i + 1; j != DEFAULT_NUM_DATA; ++j) {
                ASSERTV(i, j, *DEFAULT_DATA[i] != *DEFAULT_DATA[j]);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTOR AND DESTRUCTOR
        //   Ensure that we can implicitly create a 'type_index' object from a
        //   'type_info', and so produce an object in any state needed for
        //   further testing.
        //
        // Concerns:
        //: 1 There is an implicit conversion constructor accepting a reference
        //:   to a 'std::type_info' object.
        //:
        //: 2 After construction, the object has the value associated with the
        //:   supplied 'std::type_info'.
        //:
        //: 3 An object can be safely destroyed.
        //:
        //: 4 Both constructor and destructor have non-throwing exception
        //:   specifications on implementations that support 'noexcept'.
        //
        // Plan:
        //: 1 Create a small sample of 'type_index' objects having distinct
        //:   values. (C-1)
        //:
        //: 2 Verify that objects constructed from equivalent 'typeid'
        //:   expressions have the same value, and object constructed from
        //:   differing 'typeid' expressions have different values (C-2).
        //:
        //: 3 Let the objects created in P-1 go out of scope (C-3).
        //:
        //: 4 Using the 'ASSERT_NOEXCEPT' macro, verify construction and
        //:   destruction of a temporary has a non-throwing exception
        //:   specification (C-4).
        //
        // Testing:
        //   type_index(const std::type_info& target) noexcept;
        //   ~type_index();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE CONSTRUCTOR AND DESTRUCTOR"
                            "\n========================================\n");


        if (verbose) printf("\nTesting typical values\n");
        {
            // Create a set of 3 test values, and verify they have distinct
            // values.

            const Obj X = typeid(int);
            const Obj Y = typeid(char);
            const Obj Z = typeid(void);

            if (veryVerbose) { T_ P(X) }
            if (veryVerbose) { T_ P(Y) }

            ASSERT(X == X);
            ASSERT(X != Y);
            ASSERT(Y == Y);
            ASSERT(Y != Z);
            ASSERT(Z == Z);
            ASSERT(Z != X);

            // Create a duplicate set of 3 test values, and verify they also
            // have distinct values.

            const Obj A = typeid(int);
            const Obj B = typeid(char);
            const Obj C = typeid(void);

            ASSERT(A == A);
            ASSERT(A != B);
            ASSERT(B == B);
            ASSERT(B != C);
            ASSERT(C == C);
            ASSERT(C != A);

            // Finally, confirm the duplicate values match the original values,
            // demonstrating that comparisons are not object-identity checks.

            ASSERT(A == X);
            ASSERT(B == Y);
            ASSERT(C == Z);
        }

        if (verbose) printf("\nTesting exception specifications\n");
        {
            ASSERT_NOEXCEPT(Obj(typeid(int)));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create a small sample of 'type_index' objects having distinct
        //:   values.
        //:
        //: 2 Test each accessor can be called for a 'const type_index' object,
        //:   including self-comparisons.
        //:
        //: 3 Assign to replace the value of the second 'type_index' object,
        //:   and confirm that the comparison operators reflect the change.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const Obj X = typeid(int);
        ASSERT(  X == X );
        ASSERT(!(X != X));
        ASSERT(!(X <  X));
        ASSERT(  X <= X );
        ASSERT(!(X >  X));
        ASSERT(  X >= X );

        if (verbose) printf("name: %s\n", X.name());
        if (verbose) printf("name: " ZU "n", X.hash_code());

        Obj Y = typeid(void);
        ASSERT(!(X == Y));
        ASSERT(  X != Y );
        ASSERT( (X <  Y) == typeid(int).before(typeid(void)));
        ASSERT( (X <= Y) == (X < Y) );
        ASSERT( (X >  Y) == typeid(void).before(typeid(int)));
        ASSERT( (X >= Y) == (X > Y) );

        Y = X;
        ASSERT(  X == Y );
        ASSERT(!(X != Y));
        ASSERT(!(X <  Y));
        ASSERT(  X <= Y );
        ASSERT(!(X >  Y));
        ASSERT(  X >= Y );

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
