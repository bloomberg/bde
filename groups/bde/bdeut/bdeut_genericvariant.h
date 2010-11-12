// bdeut_genericvariant.h                -*-C++-*-
#ifndef INCLUDED_BDEUT_GENERICVARIANT
#define INCLUDED_BDEUT_GENERICVARIANT

//@PURPOSE: Provide a generic variant data type.
//
//@CLASSES:
//  bdeut_GenericVariant: generic variant data type
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@SEE_ALSO: bdeut_variant
//
//@DESCRIPTION: This component provides a generic value-semantic variant type
// 'bdeut_GenericVariant' that can hold almost any type of value.  It can be
// thought of as a 'void' pointer with value-semantics.  This means that it can
// be used, for example, to implement a vector of heterogeneous types.
//
// Since the generic variant object may hold almost *any* type of value, it has
// a templated 'is' method to query its type.  For example, calling 'is<int>()'
// will return true if the variant object holds an 'int' value, and false
// otherwise.
//
// A generic variant object may also hold *no* value.  This can be queried
// using the 'is<void>()' accessor.
//
// The value held by the generic variant object may be accessed or manipulated
// using the templated 'the' method.  For example, if the object holds an 'int'
// value, then calling 'the<int>()' will return a reference to that 'int'
// value.  Note that calling 'the<void>()' will result in undefined behavior,
// as will calling 'the<int>()' if 'is<int>()' returns false.
//
// A generic variant object can be assigned a new type using the templated
// 'assign' method.  For example, calling 'assign<int>()' will morph the value
// held by the variant object into an 'int'.  Note that there are certain types
// that cannot be assigned to a generic variant object (see "Restricted Types"
// below) and attempting to assign these types will result in undefined
// behavior.
//
// There are two forms of the 'assign' method.  The first takes no argument and
// assigns the default value of the assigned type to the generic variant
// object.  The second form takes one argument and assigns the type *and* value
// of the argument to the generic variant object.
//
///Value-Semantic Operations Provided
///----------------------------------
// The 'bdeut_GenericVariant' type is a value-semantic type and provides all
// the standard BDE value-semantic operations, i.e.:
//..
//  o default constructor with optional allocator
//  o copy constructor with optional allocator
//  o assignment operator
//  o print method                             (special cases apply; see below)
//  o 'bdex' streaming methods                 (special cases apply; see below)
//  o equality operator                        (special cases apply; see below)
//..
//
///Default Constructor
///- - - - - - - - - -
// When a generic variant object is constructed using the default constructor,
// it will contain *no* value.  This means the following code will not cause an
// assertion failure:
//..
//  bdeut_GenericVariant object;
//  assert(object.is<void>());
//..
//
///Copy Constructor
///- - - - - - - - -
// When a generic variant object is copy constructed from another generic
// variant object, it will contain the type of the original generic variant
// object.  This means the following code will not cause an assertion failure:
//..
//  bdeut_GenericVariant object1;
//  bdeut_GenericVariant object2(object1);
//  assert(object1.is<void>() && object2.is<void>());
//
//  object2.assign<int>();
//  assert(object1.is<void>() && object2.is<int>());
//
//  bdeut_GenericVariant object3(object2);
//  assert(object1.is<void>() && object2.is<int>() && object3.is<int>());
//..
// The constructed generic variant object will also contain the *value* of the
// original generic variant object (if there is a value).  This means the
// following code will not cause an assertion failure:
//..
//  bdeut_GenericVariant object1;
//
//  const int INT_VALUE = 123;
//
//  object1.assign(INT_VALUE);
//  assert(object1.is<int>() && INT_VALUE == object1.the<int>());
//
//  bdeut_GenericVariant object2(object1);
//  assert(object1.is<int>() && INT_VALUE == object1.the<int>()
//      && object2.is<int>() && INT_VALUE == object2.the<int>());
//..
//
///Assignment Operator
///- - - - - - - - - -
// When a generic variant object is assigned to another generic variant object,
// via 'operator=', the type of the value held by the variant on the left hand
// side will morph to the type of the value held by the variant on the right
// hand side.  This means the following code will not cause an assertion
// failure:
//..
//  bdeut_GenericVariant object1, object2;
//
//  object1.assign<int>();
//  assert(object1.is<int>() && object2.is<void>());
//
//  object2 = object1;
//  assert(object1.is<int>() && object2.is<int>());
//..
// The left hand side variant will also obtain the *value* held by the variant
// on the right hand side.  This means the following code will not cause an
// assertion failure:
//..
//  bdeut_GenericVariant object1, object2;
//
//  const int INT_VALUE = 123;
//
//  object1.assign(INT_VALUE);
//  assert(object1.is<int>() && INT_VALUE == object1.the<int>()
//      && object2.is<void>());
//
//  object2 = object1;
//  assert(object1.is<int>() && INT_VALUE == object1.the<int>()
//      && object2.is<int>() && INT_VALUE == object2.the<int>());
//..
//
///Print Method
///- - - - - - -
// When a generic variant object is printed, the print method for the type of
// value held in the object will be used.  See 'bdeu_printmethods' for more
// information about creating print methods for value-semantic types.
//
// The following special cases apply:
//..
//  o "(* NULL *)"        will be printed if 'is<void>()' is true.
//  o "(* UNPRINTABLE *)" will be printed if 'bdeu_printmethods' is unable to
//                        resolve the print operation (i.e., if the type does
//                        not have an 'operator<<' and does not declare any of
//                        the traits recognized by 'bdeu_printmethods').  See
//                        the 'bdeu_printmethods' component-level documentation
//                        for information about these traits.
//..
//
///'bdex' Streaming Methods
///- - - - - - - - - - - - -
// When a 'bdex' streaming method is called for a generic variant object, the
// corresponding 'bdex' streaming method for the type of value held by the
// variant object will be be used.
//
// The following special cases apply:
//..
//  o if 'is<void>()' is true, 'bdexStreamOut' and 'bdexStreamIn' will be
//    no-ops and 'maxSupportedBdexVersion' will return 0.  This allows
//    streaming empty variants without any special handling.
//  o if the type of value held by the variant object does not declare the
//    'bdex_TypeTraitStreamable' trait, 'bdexStreamOut' and 'bdexStreamIn' will
//    invalidate the stream and 'maxSupportedBdexVersion' will return 0.  This
//    allows any attempt to stream objects that do not declare this trait to be
//    caught during testing.  Note that this special case does not apply to
//    fundamental types or enumerations, since these types already have 'bdex'
//    streaming methods defined in the 'bdex_[in|out]streamfunctions'
//    component.
//..
//
// Important Note: When streaming *any* polymorphic object (including
// 'bdeut_GenericVariant'), it is important that the actual type of the objects
// at both the sender side and the receiver side are the same.  Failing to
// correctly set the type of the object at the receiver side will result in
// undefined behavior.  The 'typeInfo' accessor can be used, in conjunction
// with a "type-map", to help an implementation ensure that the types on both
// sides match.
//
///Equality Operator
///- - - - - - - - -
// When 'operator==' is called for two generic variant objects, it returns 1 if
// both the type *and* the value of the two variant objects match and 0
// otherwise.
//
// The following special cases apply:
//..
//  o if 'is<void>()' is true for *both* variant objects, 'operator==' will
//    return 1.
//  o if the type of value held in the variant objects does not define an
//    'operator==', the behavior is undefined.
//..
//
///Memory Supply
///-------------
// If a type assigned to a generic variant object declares the
// 'bdealg_TypeTraitUsesBdemaAllocator' trait, then the allocator passed to the
// constructor of the variant object will be used to supply memory for the
// object of the assigned type.
//
///Restricted Types
///----------------
// The following types are restricted and cannot be assigned to a generic
// variant object:
//..
//  o types that do not define a *public* default constructor
//  o types that do not define a *public* copy constructor
//  o types that do not define a *public* assignment operator
//..
// Attempting to assign any of these restricted types to a generic variant
// object will result in undefined behavior and will probably fail to compile.
//
///Usage 1
///-------
// The following snippets of code illustrate the syntax for assigning values of
// different types to a 'bdeut_GenericVariant' object.  First, we create two
// variant objects, 'variant1' and 'variant2'.  We check that neither of them
// contain a value and that both objects are equal:
//..
//  bdeut_GenericVariant variant1, variant2;
//
//  std::cout << variant1              << std::endl;  // prints "(* NULL *)"
//  std::cout << variant1.is<void>()   << std::endl;  // prints "true"
//  std::cout << variant1.is<int>()    << std::endl;  // prints "false"
//  std::cout << variant1.is<double>() << std::endl;  // prints "false"
//
//  std::cout << variant2              << std::endl;  // prints "(* NULL *)"
//  std::cout << variant2.is<void>()   << std::endl;  // prints "true"
//  std::cout << variant2.is<int>()    << std::endl;  // prints "false"
//  std::cout << variant2.is<double>() << std::endl;  // prints "false"
//
//  assert(variant1 == variant2);
//..
// Next, we assign an 'int' value to 'variant1' and print its output:
//..
//  const int INT_VALUE = 45;
//
//  variant1.assign(INT_VALUE);
//
//  std::cout << variant1              << std::endl;  // prints "45"
//  std::cout << variant1.is<void>()   << std::endl;  // prints "false"
//  std::cout << variant1.is<int>()    << std::endl;  // prints "true"
//  std::cout << variant1.is<double>() << std::endl;  // prints "false"
//  std::cout << variant1.the<int>()   << std::endl;  // prints "45"
//
//  assert(INT_VALUE == variant1.the<int>());
//..
// Next, we assign a 'double' value to 'variant2' and print its output:
//..
//  const double DOUBLE_VALUE = 9.284;
//
//  variant2.assign(DOUBLE_VALUE);
//
//  std::cout << variant2               << std::endl;  // prints "9.284"
//  std::cout << variant2.is<void>()    << std::endl;  // prints "false"
//  std::cout << variant2.is<int>()     << std::endl;  // prints "false"
//  std::cout << variant2.is<double>()  << std::endl;  // prints "true"
//  std::cout << variant2.the<double>() << std::endl;  // prints "9.284"
//
//  assert(DOUBLE_VALUE == variant2.the<double>());
//..
// Next, we assign to 'variant1' the type and value of 'variant2', then print
// its output and make sure that both objects are equal:
//..
//  variant1 = variant2;
//
//  std::cout << variant1               << std::endl;  // prints "9.284"
//  std::cout << variant1.is<void>()    << std::endl;  // prints "false"
//  std::cout << variant1.is<int>()     << std::endl;  // prints "false"
//  std::cout << variant1.is<double>()  << std::endl;  // prints "true"
//  std::cout << variant1.the<double>() << std::endl;  // prints "9.284"
//
//  assert(DOUBLE_VALUE == variant1.the<double>());
//
//  assert(variant1 == variant2);
//..
// Next, we reset 'variant1' and check that it does not contain a value and
// that it is no longer equal to 'variant2':
//..
//  variant1.assign<void>();  // equivalent to 'variant1.reset();'
//
//  std::cout << variant1              << std::endl;  // prints "(* NULL *)"
//  std::cout << variant1.is<void>()   << std::endl;  // prints "true"
//  std::cout << variant1.is<int>()    << std::endl;  // prints "false"
//  std::cout << variant1.is<double>() << std::endl;  // prints "false"
//
//  assert(variant1 != variant2);
//..
// Now we will assign a 'double' with the value of zero to 'variant1' and an
// 'int' with the "same" value of zero to 'variant2', and check that the two
// variant objects are not equal:
//..
//  variant1.assign<double>();     // assign default double value of 0
//
//  std::cout << variant1               << std::endl;  // prints "0"
//  std::cout << variant1.is<void>()    << std::endl;  // prints "false"
//  std::cout << variant1.is<int>()     << std::endl;  // prints "false"
//  std::cout << variant1.is<double>()  << std::endl;  // prints "true"
//  std::cout << variant1.the<double>() << std::endl;  // prints "0"
//
//  variant2.assign<int>();        // assign default integer value of 0
//
//  std::cout << variant2              << std::endl;  // prints "0"
//  std::cout << variant2.is<void>()   << std::endl;  // prints "false"
//  std::cout << variant2.is<int>()    << std::endl;  // prints "true"
//  std::cout << variant2.is<double>() << std::endl;  // prints "false"
//  std::cout << variant2.the<int>()   << std::endl;  // prints "0"
//
//  assert(0 == variant1.the<double>());
//  assert(0 == variant2.the<int>());
//
//  assert(variant1 != variant2);  // different types
//..
//
///Usage 2
///-------
// Suppose you wanted to create a generic cache that can hold objects of any
// arbitrary type.  Items in the cache can be looked up using a string
// identifier key:
//..
//  class MyGenericCache {
//      // A generic cache that holds heterogeneous objects.
//
//      // PRIVATE DATA MEMBERS
//      std::vector<bdeut_GenericVariant> d_objects;  // storage for objects
//      std::map<std::string, int>        d_keyMap;   // map from key to index
//                                                    // of element in the
//                                                    // 'd_objects' vector
//
//      // ... other cache "housekeeping" data ...
//
//    public:
//      // CREATORS
//      MyGenericCache(int cacheSize) : d_data(cacheSize) { }
//      ~MyGenericCache() { }
//
//      // MANIPULATORS
//      template <typename TYPE>
//      void addToCache(const std::string& key,
//                      const TYPE&        object);
//          // Add the specified 'object' identified by the specified 'key' to
//          // this cache.
//
//      // ACCESSORS
//      template <typename TYPE>
//      const TYPE* the(const std::string& key) const;
//          // Return a pointer to the object identified by the specified 'key'
//          // from this cache.  Return 0 if the cache does not hold an object
//          // with the specified 'key' or if the object is not of the
//          // specified 'TYPE'.
//  };
//..
// Since the 'addToCache' manipulator and the 'the' accessor are templatized,
// this means that any type of object can be added to or accessed from the
// cache.  However, the objects are stored in a 'std::vector', which requires
// that its template parameter is of a fixed type.  But we need a heterogeneous
// vector to implement the documented methods.  Therefore, we can use the
// 'bdeut_GenericVariant' type as the template parameter.  The 'addToCache'
// method can then be implemented as follows:
//..
//  template <typename TYPE>
//  void MyGenericCache::addToCache(const std::string& key,
//                                  const TYPE&        object)
//  {
//      int insertPosition;
//
//      // ...
//      // Some complicated cache logic goes here to determine which cache
//      // entry should be flushed out to make room for 'object', and set
//      // 'insertPosition' appropriately.
//      // ...
//
//      d_objects[insertPosition].assign(object);
//      d_keyMap[key] = insertPosition;
//  }
//..
// The 'the' accessor can be implemented as follows:
//..
//  template <typename TYPE>
//  const TYPE* MyGenericCache::the(const std::string& key) const
//  {
//      typedef std::map<std::string, int>::const_iterator MapIterator;
//
//      MapIterator it = d_keyMap.find(key);
//
//      if (d_keyMap.end() == it) {    // 'key' was not found
//          return 0;
//      }
//
//      const int itemPosition = it->second;
//
//      assert(0 <= itemPosition && itemPosition < d_objects.size());
//
//      const bdeut_GenericVariant& item = d_objects[itemPosition];
//
//      if (!item.is<TYPE>()) {        // 'item' is not a 'TYPE'
//          return 0;
//      }
//
//      return &item.the<TYPE>();
//  }
//..
// Such a cache could be used for anything.  For example, it can be used in a
// web browser to store different types of data and make them all share the
// same cache logic and cache "housekeeping" data.  The following classes
// define some common data types that might be used in the web browser:
//..
//  class HTMLData      { ... };
//  class JPEGData      { ... };
//  class FlashData     { ... };
//  class MPEGData      { ... };
//  class GIFData       { ... };
//  class QuickTimeData { ... };
//..
// A memory cache that temporarily stores all this data in memory can be
// declared as follows:
//..
//  MyGenericCache d_memoryCache(100);
//..
// Whenever the web browser needs to perform a particular operation on a
// particular data type (for example, rendering a JPEG image), it first tries
// to retrieve this data from the memory cache.  If the data is unavailable in
// the memory cache, it retrieves the data from either a disk cache or a web
// server.  If successful, it adds the data to the memory cache so that it does
// not need to download it again the next time:
//..
//  void renderJPEGImage(const std::string& url,
//                       int                xPosition,
//                       int                yPosition)
//      // Render the JPEG image having the specified 'url' at the specified
//      // 'xPosition' and the specified 'yPosition' of the browser window.
//  {
//      JPEGData *data = d_memoryCache.the<JPEGData>(url);
//
//      if (!data) {
//          JPEGData img;
//
//          // ... retrieve 'img' from disk cache or from a web server ...
//
//          d_memoryCache.addToCache(url, img);
//
//          data = d_memoryCache.the<JPEGData>(url);
//
//          assert(data);
//      }
//
//      // ... render 'data' at the specified position ...
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEALG_CONSTRUCTORPROXY
#include <bdealg_constructorproxy.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEMA_ALLOCATOR
#include <bdema_allocator.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BDEMF_ANYTYPE
#include <bdemf_anytype.h>  // bdemf_TypeRep
#endif

#ifndef INCLUDED_BDEMF_ASSERT
#include <bdemf_assert.h>
#endif

#ifndef INCLUDED_BDEMF_ISENUM
#include <bdemf_isenum.h>
#endif

#ifndef INCLUDED_BDEMF_ISFUNDAMENTAL
#include <bdemf_isfundamental.h>
#endif

#ifndef INCLUDED_BDEMF_ISPOINTER
#include <bdemf_ispointer.h>
#endif

#ifndef INCLUDED_BDEMF_METAVALUE
#include <bdemf_metavalue.h>  // bdemf_MetaInt
#endif

#ifndef INCLUDED_BDEMF_OR
#include <bdemf_or.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAM
#include <bdex_instream.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMADAPTER
#include <bdex_instreamadapter.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAM
#include <bdex_outstream.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMADAPTER
#include <bdex_outstreamadapter.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_TYPEINFO
#include <typeinfo>
#define INCLUDED_TYPEINFO
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

struct bdex_TypeTraitStreamable { };
    // TBD: this should go to one of the 'bdex' components

           // ======================================================
           // namespace bdeut_GenericVariant_OStreamOperatorDetector
           // ======================================================

namespace bdeut_GenericVariant_OStreamOperatorDetector {
    // Component-private namespace.  Do not use.  This namespace is used to
    // detect if a particular user-defined type has 'operator<<' declared for
    // it.

    typedef char TestReturnType;  // Return type used to test for the existence
                                  // of 'operator<<'.

    BDEMF_ASSERT(sizeof(std::ostream&) != sizeof(TestReturnType));
        // If this assertion fails on a particular platform, we must change the
        // size of 'TestReturnType'.

    template <typename CHAR_T, typename TRAITS, typename TYPE>
    TestReturnType operator<<(std::basic_ostream<CHAR_T, TRAITS>&,
                              const TYPE&);
        // This operator is declared but never defined.  It is never used at
        // run-time.

    template <typename TYPE>
    struct Detect_Imp {
        // 'VALUE' will be 1 if 'TYPE' has 'operator<<' declared for it, and 0
        // otherwise.

        enum {
            VALUE = sizeof(TestReturnType)
                    != sizeof(bdemf_TypeRep<std::ostream>::rep()
                                                 << bdemf_TypeRep<TYPE>::rep())
        };
    };

    template <typename TYPE>
    struct Detect : bdemf_MetaInt<Detect_Imp<TYPE>::VALUE> {
        // This metafunction evaluates to 'bdemf_MetaInt<1>' if the specified
        // user-defined 'TYPE' has 'operator<<' declared for it, and
        // 'bdemf_MetaInt<0>' otherwise.
    };
}

            // ====================================================
            // struct bdeut_GenericVariant_HasOStreamOperator<TYPE>
            // ====================================================

template <typename TYPE>
struct bdeut_GenericVariant_HasOStreamOperator
        : bdemf_Or<bdemf_IsFundamental<TYPE>,
                   bdemf_IsPointer<TYPE>,
                   bdemf_IsEnum<TYPE>,
                   bdeut_GenericVariant_OStreamOperatorDetector::Detect<TYPE>
                  > {
    // Component-private struct.  Do not use.  This metafunction evaluates to
    // 'bdemf_MetaInt<1>' if the specified 'TYPE' has 'operator<<' declared,
    // and 'bdemf_MetaInt<0>' otherwise.
};

               // =============================================
               // struct bdeut_GenericVariant_IsPrintable<TYPE>
               // =============================================

template <typename TYPE>
struct bdeut_GenericVariant_IsPrintable
        : bdemf_Or<bdeu_PrintMethods_DetectTraitIndex<TYPE>,
                   bdeut_GenericVariant_HasOStreamOperator<TYPE>
                  > {
    // This metafunction evaluates to 'bdemf_MetaInt<1>' if the specified
    // 'TYPE' can be used with 'bdeu_PrintMethods', and 'bdemf_MetaInt<0>'
    // otherwise.
};

          // =======================================================
          // namespace bdeut_GenericVariant_EqualityOperatorDetector
          // =======================================================

namespace bdeut_GenericVariant_EqualityOperatorDetector {
    // Component-private namespace.  Do not use.  This namespace is used to
    // detect if a particular user-defined type has 'operator==' declared for
    // it.

    struct TestReturnType {
        // Return type used to test for the existence of 'operator=='.

        char d_dummy[256];
    };

    BDEMF_ASSERT(sizeof(int)  != sizeof(TestReturnType));
    BDEMF_ASSERT(sizeof(bool) != sizeof(TestReturnType));
        // If these assertions fail on a particular platform, we must change
        // the size of 'TestReturnType'.

    template <typename TYPE>
    TestReturnType operator==(const TYPE&, const TYPE&);
        // This operator is declared but never defined.  It is never used at
        // run-time.

    template <typename TYPE>
    struct Detect_Imp {
        // 'VALUE' will be 1 if 'TYPE' has 'operator==' declared for it, and 0
        // otherwise.

        enum {
            VALUE = sizeof(TestReturnType)
                    != sizeof(bdemf_TypeRep<TYPE>::rep()
                                                 == bdemf_TypeRep<TYPE>::rep())
        };
    };

    template <typename TYPE>
    struct Detect : bdemf_MetaInt<Detect_Imp<TYPE>::VALUE> {
        // This metafunction evaluates to 'bdemf_MetaInt<1>' if the specified
        // user-defined 'TYPE' has 'operator==' declared, and
        // 'bdemf_MetaInt<0>' otherwise.
    };
}

           // =====================================================
           // struct bdeut_GenericVariant_HasEqualityOperator<TYPE>
           // =====================================================

template <typename TYPE>
struct bdeut_GenericVariant_HasEqualityOperator
        : bdemf_Or<bdemf_IsFundamental<TYPE>,
                   bdemf_IsPointer<TYPE>,
                   bdemf_IsEnum<TYPE>,
                   bdeut_GenericVariant_EqualityOperatorDetector::Detect<TYPE>
                  > {
    // This metafunction evaluates to 'bdemf_MetaInt<1>' if the specified
    // 'TYPE' has 'operator==' declared, and 'bdemf_MetaInt<0>' otherwise.
};

 // ==========================================================================
 // struct bdeut_GenericVariant_HasEqualityOperator<std::vector<TYPE, ALLOC> >
 // ==========================================================================

template <typename TYPE, typename ALLOC>
struct bdeut_GenericVariant_HasEqualityOperator<std::vector<TYPE, ALLOC> >
        : bdemf_MetaInt<1> {
    // This partial specialization is required for 'std::vector' because
    // 'std::vector' does not have its own equality operator - it instead uses
    // the equality operator defined in 'bdestl_Vector'.  The free operator
    // detection mechanism used above does not recognize free operators defined
    // in base classes.
};

             // =================================================
             // struct bdeut_GenericVariant_HasBdexFunction<TYPE>
             // =================================================

template <typename TYPE>
struct bdeut_GenericVariant_HasBdexFunction
        : bdemf_Or<bdemf_IsFundamental<TYPE>,
                   bdemf_IsEnum<TYPE>,
                   bdealg_HasTrait<TYPE, bdex_TypeTraitStreamable>
                  > {
    // This metafunction evaluates to 'bdemf_MetaInt<1>' if the specified
    // 'TYPE' is a fundamental type, an enumeration type, or has declared the
    // 'bdex_TypeTraitStreamable' trait, and 'bdemf_MetaInt<0>' otherwise.
};

                 // =========================================
                 // namespace bdeut_GenericVariant_PrintProxy
                 // =========================================

namespace bdeut_GenericVariant_PrintProxy {
    // Component-private namespace.  Do not use.  This namespace is used as a
    // proxy for printing values stored in generic variant objects.  It
    // contains two print functions that are overloaded on a meta integer that
    // indicates whether the value is printable or not.

    template <typename TYPE>
    inline
    std::ostream& print(std::ostream&    stream,
                        const TYPE&      value,
                        int              level,
                        int              spacesPerLevel,
                        bdemf_MetaInt<1> printable)
        // This function is called for values that are printable.  The
        // corresponding 'bdeu_PrintMethods' print method is called.
    {
        return bdeu_PrintMethods::print(stream,
                                        value,
                                        level,
                                        spacesPerLevel);
    }

    template <typename TYPE>
    inline
    std::ostream& print(std::ostream&    stream,
                        const TYPE&      value,
                        int              level,
                        int              spacesPerLevel,
                        bdemf_MetaInt<0> notPrintable)
        // This function is called for values that cannot be printed, so
        // "(* UNPRINTABLE *)" is printed.
    {
        return bdeu_PrintMethods::print(stream,
                                        "(* UNPRINTABLE *)",
                                        level,
                                        spacesPerLevel);
    }
}

                // ============================================
                // namespace bdeut_GenericVariant_EqualityProxy
                // ============================================

namespace bdeut_GenericVariant_EqualityProxy {
    // Component-private namespace.  Do not use.  This namespace is used as a
    // proxy for comparing values stored in generic variant objects.  It
    // contains two 'equal' functions that are overloaded on a meta integer
    // value that indicates whether the values are comparable or not.

    template <typename TYPE>
    inline
    int equal(const TYPE& lhs, const TYPE& rhs, bdemf_MetaInt<1> comparable)
        // This function is called for values that are comparable.  The
        // corresponding 'operator==' is called.
    {
        return lhs == rhs;
    }

    template <typename TYPE>
    inline
    int equal(const TYPE& lhs, const TYPE& rhs, bdemf_MetaInt<0> notComparable)
        // This function is called for values that are not comparable.  The
        // behavior is undefined.
    {
        // TBD: assert(0);

        return 1;
    }
}

                  // ========================================
                  // namespace bdeut_GenericVariant_BdexProxy
                  // ========================================

namespace bdeut_GenericVariant_BdexProxy {
    // Component-private namespace.  Do not use.  This namespace is used as a
    // proxy for streaming values stored in generic variant objects.  It
    // contains the standard 'bdex' functions that are overloaded on a meta
    // integer that indicates whether the value can be used with the
    // 'bdex_[in|out]streamfunctions' components.

    template <typename TYPE>
    inline
    int maxSupportedVersion(const TYPE&      value,
                            bdemf_MetaInt<1> streamable)
        // This function is called for values that are streamable.  The
        // corresponding 'maxSupportedVersion' function is called.
    {
// TBD CC 5.2 emits an error during template instantiation
#if defined(BDES_PLATFORM__CMP_SUN) && BDES_PLATFORM__CMP_VERSION < 0x550
        return 0;
#else
        return bdex_VersionFunctions::maxSupportedVersion(value);
#endif
    }

    template <typename TYPE>
    inline
    int maxSupportedVersion(const TYPE&      value,
                            bdemf_MetaInt<0> notStreamable)
        // This function is called for values that are not streamable, so just
        // return 0.
    {
        return 0;
    }

    template <typename TYPE>
    inline
    bdex_InStream& streamIn(bdex_InStream&   stream,
                            TYPE&            value,
                            int              version,
                            bdemf_MetaInt<1> streamable)
        // This function is called for values that are streamable.  The
        // corresponding 'streamIn' function is called.
    {
// TBD CC 5.2 emits an error during template instantiation
#if defined(BDES_PLATFORM__CMP_SUN) && BDES_PLATFORM__CMP_VERSION < 0x550
        return stream;
#else
        return bdex_InStreamFunctions::streamIn(stream, value, version);
#endif
    }

    template <typename TYPE>
    inline
    bdex_InStream& streamIn(bdex_InStream&   stream,
                            TYPE&            value,
                            int              version,
                            bdemf_MetaInt<0> notStreamable)
        // This function is called for values that are not streamable, so just
        // invalidate the stream.
    {
        stream.invalidate();
        return stream;
    }

    template <typename TYPE>
    inline
    bdex_OutStream& streamOut(bdex_OutStream&  stream,
                              const TYPE&      value,
                              int              version,
                              bdemf_MetaInt<1> streamable)
        // This function is called for values that are streamable.  The
        // corresponding 'streamOut' function is called.
    {
// TBD CC 5.2 emits an error during template instantiation
#if defined(BDES_PLATFORM__CMP_SUN) && BDES_PLATFORM__CMP_VERSION < 0x550
        return stream;
#else
        return bdex_OutStreamFunctions::streamOut(stream, value, version);
#endif
    }

    template <typename TYPE>
    inline
    bdex_OutStream& streamOut(bdex_OutStream&  stream,
                              const TYPE&      value,
                              int              version,
                              bdemf_MetaInt<0> notStreamable)
        // This function is called for values that are not streamable, so just
        // invalidate the stream.
    {
        stream.invalidate();
        return stream;
    }
}

                         // ==========================
                         // class bdeut_GenericVariant
                         // ==========================

class bdeut_GenericVariant {
    // This is a generic variant data type with value-semantic operations.
    // Objects of this type can be morphed at runtime.  It can be thought of as
    // a 'void' pointer with value-semantics.

  private:
    // TYPES
    class VariantImplBase {
        // Base class for the implementation of the variant type.  This class
        // defines a protocol that will be used in the implementation of
        // 'bdeut_GenericVariant' to perform operations on various types (e.g.,
        // clone an object, print an object etc).  An implementation of this
        // protocol will be generated for each type that is assigned to a
        // 'bdeut_GenericVariant' object (see the 'VariantImpl' template
        // below).

      public:
        // CREATORS
        virtual ~VariantImplBase()
            // Destroy this variant implementation object.
        {
        }

        // MANIPULATORS
        virtual void assign(const VariantImplBase *rhs) = 0;
            // Assign to this object the value of the specified 'rhs'.  The
            // behavior is undefined unless 'rhs' and 'this' are objects of the
            // same implementation of this protocol.

        virtual bdex_InStream& streamIn(bdex_InStream& stream,
                                        int            version) = 0;
            // Assign to this object the value read from the specified 'stream'
            // using the specified 'version' schema and return a reference to
            // the modifiable 'stream'.  Note that a version is not read from
            // the stream.

        // ACCESSORS
        virtual VariantImplBase* clone(bdema_Allocator *allocator) const = 0;
            // Return a clone of this object using the specified 'allocator'.

        virtual bool eq(const VariantImplBase *rhs) const = 0;
            // Return true if this object has the same value as the specified
            // 'rhs'.  The behavior is undefined unless 'rhs' and 'this' are
            // objects of the same implementation of this protocol.

        virtual int maxSupportedBdexVersion() const = 0;
            // Return the maximum supported 'bdex' version for the object
            // stored in the derived class.

        virtual std::ostream& print(std::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const = 0;
            // Format this object to the specified output 'stream' at the
            // (absolute value of) the optionally specified indentation 'level'
            // and return a reference to 'stream'.  If 'level' is specified,
            // optionally specify 'spacesPerLevel', the number of spaces per
            // indentation level for this and all of its nested objects.  If
            // 'level' is negative, suppress indentation of the first line.  If
            // 'spacesPerLevel' is negative, format the entire output on one
            // line, suppressing all but the initial indentation (as governed
            // by 'level').  If 'stream' is not valid on entry, this operation
            // has no effect.  If the object stored in the derived class is not
            // printable, then print "(* UNPRINTABLE *)".

        virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
                                          int             version) const = 0;
            // Write this object to the specified 'stream' using the specified
            // 'version' schema and return a reference to the modifiable
            // 'stream'.  Note that the version is not written to the stream.

        virtual const std::type_info& typeInfo() const = 0;
            // Return the type information about the object stored in the
            // derived class.
    };

    template <typename TYPE>
    class VariantImpl : public VariantImplBase {
        // Implementation of the 'VariantImplBase' protocol for the specified
        // 'TYPE'.  An instantiation of this class will be generated for each
        // type that is assigned to a 'bdeut_GenericVariant' object.

        // PRIVATE TYPES
        typedef VariantImpl<TYPE>             TypeOfSelf; // Type of this class
                                                          // (for notational
                                                          // convenience).
        typedef bdealg_ConstructorProxy<TYPE> TypeProxy;  // Constructor proxy
                                                          // for 'TYPE'.

        // PRIVATE DATA MEMBERS
        TypeProxy d_objectProxy;  // proxy for embedded 'TYPE' object

      public:
        // CREATORS
        explicit VariantImpl(bdema_Allocator *basicAllocator)
            // Construct a variant implementation object having the default
            // value using the specified 'basicAllocator' to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.
        : d_objectProxy(TYPE(), basicAllocator)
        {
        }

        VariantImpl(const TYPE&      value,
                    bdema_Allocator *basicAllocator)
            // Construct a variant implementation object having the specified
            // 'value' using the specified 'basicAllocator' to supply memory.
            // If 'basicAllocator' is 0, the currently installed default
            // allocator is used.
        : d_objectProxy(value, basicAllocator)
        {
        }

        virtual ~VariantImpl()
            // Destroy this object.
        {
        }

        // MANIPULATORS
        TYPE& object()
            // Return a reference to the modifiable 'TYPE' object that is
            // embedded in this object.
        {
            return d_objectProxy.object();
        }

        virtual void assign(const VariantImplBase *rhs)
            // Assign to this object the value of the specified 'rhs'.  The
            // behavior is undefined unless 'rhs' is of type 'TypeOfSelf'.
        {
            const TypeOfSelf *derived = static_cast<const TypeOfSelf*>(rhs);
            // TBD: assert(derived);

            object() = derived->object();
        }

        virtual bdex_InStream& streamIn(bdex_InStream& stream,
                                        int            version)
            // Assign to this object the value read from the specified 'stream'
            // using the specified 'version' schema and return a reference to
            // the modifiable 'stream'.  Note that a version is not read from
            // the stream.
        {
            return bdeut_GenericVariant_BdexProxy::streamIn(
                                 stream,
                                 object(),
                                 version,
                                 bdeut_GenericVariant_HasBdexFunction<TYPE>());
        }

        // ACCESSORS
        const TYPE& object() const
            // Return a reference to the non-modifiable 'TYPE' object that is
            // embedded in this object.
        {
            return d_objectProxy.object();
        }

        virtual VariantImplBase* clone(bdema_Allocator *allocator) const
            // Return a clone of this object using the specified 'allocator'.
        {
            // TBD: assert(allocator);

            return new (*allocator) TypeOfSelf(object(), allocator);
        }

        virtual bool eq(const VariantImplBase *rhs) const
            // Return true if this object has the same value as the specified
            // 'rhs'.  The behavior is undefined unless 'rhs' is of type
            // 'TypeOfSelf'.
        {
            const TypeOfSelf *derived = static_cast<const TypeOfSelf*>(rhs);
            // TBD: assert(derived);

            return bdeut_GenericVariant_EqualityProxy::equal(
                             object(),
                             derived->object(),
                             bdeut_GenericVariant_HasEqualityOperator<TYPE>());
        }

        virtual int maxSupportedBdexVersion() const
            // Return the maximum supported 'bdex' version for 'TYPE'.
        {
            return bdeut_GenericVariant_BdexProxy::maxSupportedVersion(
                                 object(),
                                 bdeut_GenericVariant_HasBdexFunction<TYPE>());
        }

        virtual std::ostream& print(std::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
            // Format this object to the specified output 'stream' at the
            // (absolute value of) the optionally specified indentation 'level'
            // and return a reference to 'stream'.  If 'level' is specified,
            // optionally specify 'spacesPerLevel', the number of spaces per
            // indentation level for this and all of its nested objects.  If
            // 'level' is negative, suppress indentation of the first line.  If
            // 'spacesPerLevel' is negative, format the entire output on one
            // line, suppressing all but the initial indentation (as governed
            // by 'level').  If 'stream' is not valid on entry, this operation
            // has no effect.  If 'TYPE' is not printable, then print
            // "(* UNPRINTABLE *)".
        {
            return bdeut_GenericVariant_PrintProxy::print(
                                     stream,
                                     object(),
                                     level,
                                     spacesPerLevel,
                                     bdeut_GenericVariant_IsPrintable<TYPE>());
        }

        virtual bdex_OutStream& streamOut(bdex_OutStream& stream,
                                          int             version) const
            // Write this object to the specified 'stream' using the specified
            // 'version' schema and return a reference to the modifiable
            // 'stream'.  Note that the version is not written to the stream.
        {
            return bdeut_GenericVariant_BdexProxy::streamOut(
                                 stream,
                                 object(),
                                 version,
                                 bdeut_GenericVariant_HasBdexFunction<TYPE>());
        }

        virtual const std::type_info& typeInfo() const
            // Return the type information about 'TYPE'.
        {
            return typeid(TYPE);
        }
    };

    // PRIVATE DATA MEMBERS
    VariantImplBase *d_object_p;     // variant object implementation
    bdema_Allocator *d_allocator_p;  // allocator for supplying memory

    // FRIENDS
    friend int operator==(const bdeut_GenericVariant& lhs,
                          const bdeut_GenericVariant& rhs);
    friend std::ostream& operator<<(std::ostream&               stream,
                                    const bdeut_GenericVariant& rhs);

  public:
    // CREATORS
    explicit bdeut_GenericVariant(bdema_Allocator *basicAllocator = 0);
        // Create a generic variant object with the default unset value using
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdeut_GenericVariant(const bdeut_GenericVariant&  original,
                         bdema_Allocator             *basicAllocator = 0);
        // Create a copy of the specified 'original' object using the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, an
        // appropriate default allocator is used.

    ~bdeut_GenericVariant();
        // Destroy this generic variant object.

    // MANIPULATORS
    bdeut_GenericVariant& operator=(const bdeut_GenericVariant& rhs);
        // Assign to this object the type and value of the specified 'rhs'
        // object.  This object will morph to the type of the 'rhs' object.
        // Return a reference to this modifiable object.

    template <typename TYPE>
    bdeut_GenericVariant& assign();
        // Assign to this object the default value of the specified 'TYPE'.
        // This object will morph to the specified 'TYPE'.  Return a reference
        // to this modifiable object.  Note that the 'assign<void>()'
        // specialization will reset this object to the default unset value.

    template <typename TYPE>
    bdeut_GenericVariant& assign(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE'.
        // This object will morph to the specified 'TYPE'.  Return a reference
        // to this modifiable object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    bdeut_GenericVariant& reset();
        // Assign to this object the default unset value.  Return a reference
        // to this modifiable object.  Note that this method is functionally
        // equivalent to 'assign<void>()'.

    template <typename TYPE>
    TYPE& the();
        // Return a reference to the modifiable value held by this variant
        // object.  The behavior is undefined unless 'is<TYPE>()' returns true
        // and 'TYPE' is not 'void'.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.

    template <typename TYPE>
    bool is() const;
        // Return true if the value held by this variant object is of the
        // specified 'TYPE', and false otherwise.  Note that the 'is<void>()'
        // specialization will return true if this object does not hold a
        // value (i.e., if it is unset), and false otherwise.

    int maxSupportedBdexVersion() const;
        // Return the maximim supported 'bdex' version of the object held by
        // this variant object, or 0 if 'is<void>()' returns true.

    std::ostream& print(std::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that the
        // string "(* NULL *)" will be printed if 'is<void>()' and the string
        // "(* UNPRINTABLE *)" will be printed if the value held by this
        // variant object is not printable.

    template <typename TYPE>
    const TYPE& the() const;
        // Return a reference to the non-modifiable value held by this variant
        // object.  The behavior is undefined unless 'is<TYPE>()' returns true
        // and 'TYPE' is not 'void'.

    const std::type_info& typeInfo() const;
        // Return the type information for the value held by this this variant
        // object.  Note that 'typeid(void)' is returned if 'is<void>()'
        // returns true.
};

// FREE OPERATORS
inline
int operator==(const bdeut_GenericVariant& lhs,
               const bdeut_GenericVariant& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' variant objects have the same
    // type and value, and 0 otherwise.

inline
int operator!=(const bdeut_GenericVariant& lhs,
               const bdeut_GenericVariant& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' variant objects do not have
    // the same type or value, and 0 otherwise.

inline
std::ostream& operator<<(std::ostream&               stream,
                         const bdeut_GenericVariant& rhs);
    // Write the specified 'rhs' to the specified output 'stream' in some
    // reasonable (single-line) format and return a reference to 'stream'.

// TRAITS
template <>
struct bdealg_TypeTraits<bdeut_GenericVariant>
                                          : bdealg_TypeTraitUsesBdemaAllocator,
                                            bdeu_TypeTraitHasPrintMethod,
                                            bdex_TypeTraitStreamable
{
    // This struct aggregates the traits for the 'bdeut_GenericVariant' class,
    // which uses 'bdema' allocator, has a print method, and is streamable
    // using the 'bdex' protocol.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

inline
bdeut_GenericVariant::bdeut_GenericVariant(bdema_Allocator *basicAllocator)
: d_object_p(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    // TBD: assert(d_allocator_p);
}

inline
bdeut_GenericVariant::bdeut_GenericVariant(
                                   const bdeut_GenericVariant&  original,
                                   bdema_Allocator             *basicAllocator)
: d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    // TBD: assert(d_allocator_p);

    if (original.d_object_p) {
        d_object_p = original.d_object_p->clone(d_allocator_p);
    }
    else {
        d_object_p = 0;
    }
}

inline
bdeut_GenericVariant::~bdeut_GenericVariant()
{
    if (d_object_p) {
        d_allocator_p->deleteObject(d_object_p);
    }
}

// MANIPULATORS

inline
bdeut_GenericVariant& bdeut_GenericVariant::operator=(
                                               const bdeut_GenericVariant& rhs)
{
    if (&rhs != this) {
        if (rhs.d_object_p) {
            if (d_object_p) {
                if (typeid(*d_object_p) == typeid(*rhs.d_object_p)) {
                    d_object_p->assign(rhs.d_object_p);
                }
                else {
                    VariantImplBase *original = d_object_p;
                    d_object_p = rhs.d_object_p->clone(d_allocator_p);
                    d_allocator_p->deleteObject(original);
                }
            }
            else {
                d_object_p = rhs.d_object_p->clone(d_allocator_p);
            }
        }
        else {
            reset();
        }
    }

    return *this;
}

template <typename TYPE>
inline
bdeut_GenericVariant& bdeut_GenericVariant::assign()
{
    typedef VariantImpl<TYPE> TheImplClass;

    if (d_object_p) {
        if (typeid(TheImplClass) == typeid(*d_object_p)) {
            TheImplClass *derived = static_cast<TheImplClass*>(d_object_p);
            derived->object() = TYPE();
        }
        else {
            VariantImplBase *original = d_object_p;
            d_object_p = new (*d_allocator_p) TheImplClass(d_allocator_p);
            d_allocator_p->deleteObject(original);
        }
    }
    else {
        d_object_p = new (*d_allocator_p) TheImplClass(d_allocator_p);
    }

    return *this;
}

template <>
inline
bdeut_GenericVariant& bdeut_GenericVariant::assign<void>()
{
    if (d_object_p) {
        d_allocator_p->deleteObject(d_object_p);
        d_object_p = 0;
    }

    return *this;
}

template <typename TYPE>
inline
bdeut_GenericVariant& bdeut_GenericVariant::assign(const TYPE& value)
{
    typedef VariantImpl<TYPE> TheImplClass;

    if (d_object_p) {
        if (typeid(TheImplClass) == typeid(*d_object_p)) {
            TheImplClass *derived = static_cast<TheImplClass*>(d_object_p);
            derived->object() = value;
        }
        else {
            VariantImplBase *original = d_object_p;
            d_object_p = new (*d_allocator_p) TheImplClass(value,
                                                           d_allocator_p);
            d_allocator_p->deleteObject(original);
        }
    }
    else {
        d_object_p = new (*d_allocator_p) TheImplClass(value, d_allocator_p);
    }

    return *this;
}

template <class STREAM>
inline
STREAM& bdeut_GenericVariant::bdexStreamIn(STREAM& stream, int version)
{
    if (d_object_p) {
        bdex_InStreamAdapter<STREAM> inAdapter(&stream);
        d_object_p->streamIn(inAdapter, version);
    }

    return stream;
}

inline
bdeut_GenericVariant& bdeut_GenericVariant::reset()
{
    return assign<void>();
}

template <typename TYPE>
inline
TYPE& bdeut_GenericVariant::the()
{
    // TBD: assert(!is<void> && is<TYPE>());

    typedef VariantImpl<TYPE> TheImplClass;

    TheImplClass *derived = static_cast<TheImplClass*>(d_object_p);

    return derived->object();
}

// ACCESSORS

template <class STREAM>
inline
STREAM& bdeut_GenericVariant::bdexStreamOut(STREAM& stream, int version) const
{
    if (d_object_p) {
        bdex_OutStreamAdapter<STREAM> outAdapter(&stream);
        d_object_p->streamOut(outAdapter, version);
    }

    return stream;
}

template <typename TYPE>
inline
bool bdeut_GenericVariant::is() const
{
    if (!d_object_p) {
        return false;
    }

    typedef VariantImpl<TYPE> TheImplClass;

    return typeid(TheImplClass) == typeid(*d_object_p);
}

template <>
inline
bool bdeut_GenericVariant::is<void>() const
{
    return !d_object_p;
}

inline
int bdeut_GenericVariant::maxSupportedBdexVersion() const
{
    if (d_object_p) {
        return d_object_p->maxSupportedBdexVersion();
    }

    return 0;
}

inline
std::ostream& bdeut_GenericVariant::print(std::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (d_object_p) {
        return d_object_p->print(stream, level, spacesPerLevel);
    }

    return bdeu_PrintMethods::print(stream,
                                    "(* NULL *)",
                                    level,
                                    spacesPerLevel);
}

template <typename TYPE>
inline
const TYPE& bdeut_GenericVariant::the() const
{
    // TBD: assert(!is<void> && is<TYPE>());

    typedef VariantImpl<TYPE> TheImplClass;

    const TheImplClass *derived = static_cast<
                                          const TheImplClass*>(d_object_p);

    return derived->object();
}

inline
const std::type_info& bdeut_GenericVariant::typeInfo() const
{
    if (d_object_p) {
        return d_object_p->typeInfo();
    }

    return typeid(void);
}

// FREE OPERATORS

inline
int operator==(const bdeut_GenericVariant& lhs,
               const bdeut_GenericVariant& rhs)
{
    if (lhs.d_object_p && rhs.d_object_p) {
        if (typeid(*lhs.d_object_p) == typeid(*rhs.d_object_p)) {
            return lhs.d_object_p->eq(rhs.d_object_p);
        }
        else {
            return 0;
        }
    }
    else {
        // Check that both of them are 0.

        return lhs.d_object_p == rhs.d_object_p;
    }
}

inline
int operator!=(const bdeut_GenericVariant& lhs,
               const bdeut_GenericVariant& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& operator<<(std::ostream&               stream,
                         const bdeut_GenericVariant& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
