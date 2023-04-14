// bslstl_typeindex.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_TYPEINDEX
#define INCLUDED_BSLSTL_TYPEINDEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an value-semantic type to represent 'type_info' objects.
//
//@CLASSES:
//  bsl::type_index: value-semantic type wrapping a 'std::type_info' object
//
//@CANONICAL_HEADER: bsl_typeindex.h
//
//@SEE_ALSO:
//  bsl_typeinfo
//
//@DESCRIPTION: This component defines an in-core value-semantic class,
// 'bsl::type_index', capable of representing a handle to a 'std::type_info'
// object that supports all the operations needed to serve as a key type in an
// associative or unordered container.
//
// This implementation of 'type_index' satisfies the contracts for the native
// 'std::type_index' specified in the ISO standard, including a specialization
// for the native standard library 'std::hash'.  It further provides an
// overload for 'hashAppend' to support the BDE hashing framework, and therefor
// idiomatic usage with 'bsl::hash'.  In general, the recommended best practice
// is that users of 'bsl' containers should use the 'bsl::type_index' class,
// while users of 'std' containers should use the 'std::type_index' class.
//
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
//  class Shape {
//    public:
//      virtual ~Shape() = 0;
//          // Destroy this object.
//
//      // Further details elided from example.
//  };
//..
// Then, we create a utility class containing a registry of factory functions
// indexed by their corresponding 'std::type_info', using 'bsl::type_index' to
// provide the value-semantic wrapper needed for the key used in the container.
// This registry will enable us to abstract away different constructors of the
// concrete object types.
//..
//  struct ShapeUtil {
//      // This 'struct' provides a namespace for utilities to manage the
//      // creation of objects implementing the 'Shape' protocol.
//
//      // PUBLIC TYPES
//
//      typedef bsl::function<bsl::shared_ptr<Shape>(int, int)> CreateFunction;
//      typedef bsl::unordered_map<bsl::type_index, CreateFunction>
//                                                             AbstractFactory;
//      // CLASS METHODS
//
//      template <class SHAPE_TYPE>
//      static bsl::shared_ptr<Shape> make(int x, int y);
//          // Return a 'shared_ptr' owning a newly created object of (template
//          // parameter) 'SHAPE_TYPE' at the specified position '(x, y)' if
//          // 'SHAPE_TYPE' has been registered with this utility, and an empty
//          // 'shared_ptr' otherwise.
//
//      template <class SHAPE_TYPE, class FACTORY>
//      static bool registerFactory(FACTORY factory);
//          // Register the specified 'factory' creating objects of (template
//          // parameter) 'SHAPE_TYPE'; return 'true' if this is the first
//          // successful attempt to register such a factory function, and
//          // 'false' otherwise.
//
//    private:
//      static AbstractFactory s_registry;      // registry for factories
//  };
//..
// Now, we can implement the register and make functions, using the standard
// 'typeid' operator to create the key values as needed.
//..
//  template <class SHAPE_TYPE>
//  bsl::shared_ptr<Shape> ShapeUtil::make(int x, int y) {
//      AbstractFactory::iterator it = s_registry.find(typeid(SHAPE_TYPE));
//      if (s_registry.end() == it) {
//          return 0;                                                 // RETURN
//      }
//
//      return it->second(x, y);
//  }
//
//  template <class SHAPE_TYPE, class FACTORY>
//  bool ShapeUtil::registerFactory(FACTORY factory) {
//      return s_registry.emplace(typeid(SHAPE_TYPE), factory).second;
//  }
//..
// Next, we provide several concrete implementations of our 'Shape' class, to
// demonstrate use of this hierarchy.
//..
//  class Circle : public Shape {
//      // This class represents a circle, described by a position and radius.
//
//    public:
//      // CREATORS
//
//      Circle(int x, int y, int radius);
//          // Create a 'Triangle' having the it center at the specified
//          // position '(x, y)', and having the specified 'radius'.
//
//      ~Circle();
//          // Destroy this object.
//
//      // Further details elided from example.
//  };
//
//  class Triangle : public Shape {
//      // This class represents a triangle.
//
//    public:
//      // CREATORS
//
//      Triangle(int x1, int y1, int x2, int y2, int x3, int y3);
//          // Create a 'Triangle' having the specified vertices, '(x1, y1)',
//          // '(x2, y2)', and '(x3, y3)'.
//
//      ~Triangle();
//          // Destroy this object.
//
//      // Further details elided from example.
//  };
//
//  class Polygon : public Shape {
//      // This class represents a polygon having an arbitrary number of
//      // vertices.
//
//    public:
//      // CREATORS
//
//      template <class ITERATOR>
//      Polygon(ITERATOR firstPoint, ITERATOR endPoint);
//          // Create a Polygon having vertices given by the specified range
//          // '[firstPoint, endPoint)'.
//
//      ~Polygon();
//          // Destroy this object.
//
//      // Further details elided from example.
//  };
//..
// Then, we provide some simple factory functions to create some shapes at the
// specified coordinates.
//..
// bsl::shared_ptr<Shape> makeCircle(int x, int y)
//     // Return a 'Circle' at the specified position '(x, y)'.
// {
//     return bsl::make_shared<Circle>(x, y, 5);
// }
//
// bsl::shared_ptr<Shape> makeTriangle(int x, int y)
//     // Return a 'Triangle' with its lower left vertex at the specified
//     // position '(x, y)'.
// {
//     return bsl::make_shared<Triangle>(x, y, x+3, y+4, x+6, y);
// }
//..
// Finally, we can exercise the whole system in a simple test driver.  Note
// that as we do not register a factory function for the 'Polygon' class, the
// attempt to create a 'Polygon' will fail.
//..
//  int main()
//      // Simulated test driver.
//  {
//      // Install a test allocator to confirm there are no memory leaks.
//      bslma::TestAllocator         ta("Usage example default allocator");
//      bslma::DefaultAllocatorGuard guard(&ta);
//
//      bool registeredCircle = ShapeUtil::registerFactory<Circle>(makeCircle);
//      assert(registeredCircle);
//
//      bool registeredTriangle =
//                          ShapeUtil::registerFactory<Triangle>(makeTriangle);
//      assert(registeredTriangle);
//
//      bsl::shared_ptr<Shape> example = ShapeUtil::make<Circle>(10, 10);
//      assert(0 != bsl::dynamic_pointer_cast<Circle>(example));
//
//      example = ShapeUtil::make<Triangle>(10, 10);
//      assert(0 != bsl::dynamic_pointer_cast<Triangle>(example));
//
//      example = ShapeUtil::make<Polygon>(10, 10);
//      assert(0 == example);
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_compare.h>

#include <functional>
#include <typeinfo>

#include <stddef.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                                // ================
                                // class type_index
                                // ================

class type_index {
    // This class implements an in-core value-semantic type wrapping a standard
    // 'type_info' object.

    // DATA
    const std::type_info *d_targetType_p;  // pointer to 'type_info' object

  public:
    // CREATORS
    type_index(const std::type_info& target) BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Create a 'type_index' object having a reference to the type
        // designated by the specified 'target'.

    //! type_index(const type_index& original) = default;
        // Create a 'type_index' object having the value of the specified
        // 'original' date.  Note that this trivial function is implicitly
        // supplied by the compiler.

    //! ~type_index() = default;
        // Destroy this object.  Note that this trivial function is implicitly
        // supplied by the compiler.

    // MANIPULATORS
    //! type_index& operator=(const type_index& rhs) = default;
        // Assign to this object the value of the specified 'rhs' type-index,
        // and return a reference providing modifiable access to this object.
        // Note that this trivial function is implicitly supplied by the
        // compiler.

    // ACCESSORS
    bool operator==(const type_index& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this and the specified 'other' objects have the
        // same value, and 'false' otherwise.  Two 'type_index' objects have
        // the same value if they refer to the same type.

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    bool operator!=(const type_index& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this and the specified 'other' objects do not have
        // the same value, and 'false' otherwise.  Two 'type_index' objects do
        // not have the same value if they refer to different types.
#else
    strong_ordering operator<=>(const type_index& other) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Perform a three-way comparison with the specified 'other' object;
        // return the result of that comparison.
#endif

    bool operator< (const type_index& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the type referenced by this object is ordered
        // before the type referenced by the specified 'other' object in the
        // implementation defined ordering of types defined by the compiler,
        // and 'false' otherwise.

    bool operator<=(const type_index& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the type referenced by this object is the same as
        // the type referenced by the specified 'other' object, or ordered
        // before the type referenced by the 'other' in the implementation
        // defined ordering of types defined by the compiler, and 'false'
        // otherwise.

    bool operator> (const type_index& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the type referenced by this object is ordered after
        // the type referenced by the specified 'other' object in the
        // implementation defined ordering of types defined by the compiler,
        // and 'false' otherwise.

    bool operator>=(const type_index& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the type referenced by this object is the same as
        // the type referenced by the specified 'other' object, or ordered
        // after the type referenced by the 'other' in the implementation
        // defined ordering of types defined by the compiler, and 'false'
        // otherwise.

    size_t hash_code() const BSLS_KEYWORD_NOEXCEPT;
        // Return the 'hash_code' of the referenced 'type_info' object.

    const char* name() const BSLS_KEYWORD_NOEXCEPT;
        // Return the 'name' of the referenced 'type_info' object.
};

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const type_index& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'type_index'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                                // ----------------
                                // class type_index
                                // ----------------

// CREATORS
inline
type_index::type_index(const std::type_info& target) BSLS_KEYWORD_NOEXCEPT
: d_targetType_p(&target)
{
}

// ACCESSORS
inline
bool type_index::operator==(const type_index& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return *d_targetType_p == *other.d_targetType_p;
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
inline
bool type_index::operator!=(const type_index& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return *d_targetType_p != *other.d_targetType_p;
}
#else
inline
strong_ordering type_index::operator<=>(const type_index& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return *d_targetType_p == *other.d_targetType_p ? strong_ordering::equal :
      d_targetType_p->before(*other.d_targetType_p) ? strong_ordering::less :
                                                      strong_ordering::greater;
}
#endif

inline
bool type_index::operator<(const type_index& other) const BSLS_KEYWORD_NOEXCEPT
{
    return d_targetType_p->before(*other.d_targetType_p);
}

inline
bool type_index::operator<=(const type_index& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !other.d_targetType_p->before(*d_targetType_p);
}

inline
bool type_index::operator>(const type_index& other) const BSLS_KEYWORD_NOEXCEPT
{
    return other.d_targetType_p->before(*d_targetType_p);
}

inline
bool type_index::operator>=(const type_index& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !d_targetType_p->before(*other.d_targetType_p);
}

inline
const char* type_index::name() const BSLS_KEYWORD_NOEXCEPT
{
    return d_targetType_p->name();
}

template <>
struct is_trivially_copyable<type_index> : true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'type_index' is a trivially copyable type.  Note that this explicit
    // specialization is needed only for C++98/03 compilers, as the C++11 trait
    // will implicitly deduce triviality.
};

}  // close namespace bsl

// FREE FUNCTIONS
template <class HASHALG>
inline
void bsl::hashAppend(HASHALG& hashAlg, const type_index& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.hash_code());
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)

namespace std {
                        // ---------------------------
                        // class hash<bsl::type_index>
                        // ---------------------------

template <>
struct hash<bsl::type_index> {
    // This 'struct' provides an explicit specialization of the standard
    // 'hash' template, enabling 'bsl::type_index' to be used as the key type
    // for standard unordered associative containers.

    // ACCESSORS

    size_t operator()(const bsl::type_index& target) const
                                                          BSLS_KEYWORD_NOEXCEPT
        // Return the 'hash_code' of the 'type_info' object associated with the
        // specified 'target'.
    {
        return target.hash_code();
    }
};

}  // close namespace std

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#endif

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
