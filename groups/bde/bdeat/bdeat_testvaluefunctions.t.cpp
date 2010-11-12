// bdeat_testvaluefunctions.t.cpp                  -*-C++-*-

#include <bdeat_testvaluefunctions.h>

#include <bdeat_enumfunctions.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_sequencefunctions.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_time.h>

#include <bdes_platformutil.h>

#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//-----------------------------------------------------------------------------
// [ 1] Simple Types
// [ 2] Vectors of Simple Types
// [ 3] Enumerations
// [ 4] Sequences
// [ 5] Choices
// [ 6] Vectors of Choices, Enumerations, Sequences
//-----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE

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

namespace Obj = bdeat_TestValueFunctions;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <typename TYPE>
bool areElementsUnique(const std::vector<TYPE>& V)
{
    const std::size_t len = V.size();
    for (std::size_t i = 0; i < len; ++i) {
        for (std::size_t j = i + 1; j < len; ++j) {
            if (V[i] == V[j]) {
                return false;
            }
        }
    }
    return true;
}

//=============================================================================
//                      CLASSES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

// ****** THE FOLLOWING IS GENERATED CODE ******

// geom_point.h   -*-C++-*-
#ifndef INCLUDED_GEOM_POINT
#define INCLUDED_GEOM_POINT

//@PURPOSE:
//  TODO: provide purpose
//
//@CLASSES: Point
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TODO: provide annotation

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

namespace BloombergLP {

namespace geom {

class Point {

  private:
    double d_x; // todo: provide annotation
    double d_y; // todo: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_X = 0,
            // index for "X" attribute
        ATTRIBUTE_INDEX_Y = 1
            // index for "Y" attribute
    };

    enum {
        ATTRIBUTE_ID_X = 1,
            // id for "X" attribute
        ATTRIBUTE_ID_Y = 2
            // id for "Y" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Point")

    static const double DEFAULT_X;
        // default value of "X" attribute

    static const double DEFAULT_Y;
        // default value of "Y" attribute

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(Point, bdeat_TypeTraitBasicSequence);

  public:
    // CREATORS
    Point();
        // Create an object of type 'Point' having the default value.

    Point(const Point& original);
        // Create an object of type 'Point' having the value
        // of the specified 'original' object.

    ~Point();
        // Destroy this object.

    // MANIPULATORS
    Point& operator=(const Point& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    double& x();
        // Return a reference to the modifiable "X" attribute of this object.

    double& y();
        // Return a reference to the modifiable "Y" attribute of this object.


    // ACCESSORS
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    const double& x() const;
        // Return a reference to the non-modifiable "X"
        // attribute of this object.

    const double& y() const;
        // Return a reference to the non-modifiable "Y"
        // attribute of this object.

};

// FREE OPERATORS
inline
int operator==(const Point& lhs, const Point& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects have the
    // same value, and 0 otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
int operator!=(const Point& lhs, const Point& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects do not have
    // the same value, and 0 otherwise.  Two attribute objects do not have the
    // same value if one or more respective attributes differ in values.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

// ---------------------------------------------------------------------------

// CREATORS
inline
Point::Point()
: d_x(DEFAULT_X)
, d_y(DEFAULT_Y)
{
}

inline
Point::Point(const Point& original)
: d_x(original.d_x)
, d_y(original.d_y)
{
}

inline
Point::~Point()
{
}

// MANIPULATORS
inline
Point&
Point::operator=(const Point& rhs)
{
    if (this != &rhs) {
        d_x = rhs.d_x;
        d_y = rhs.d_y;
    }
    return *this;
}

inline
void Point::reset()
{
    d_x = DEFAULT_X;
    d_y = DEFAULT_Y;
}

template <class MANIPULATOR>
inline
int Point::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_x, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_X]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_y, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_Y]);
    if (ret) {
        return ret;                                                 // RETURN
    }


    return ret;
}

template <class MANIPULATOR>
inline
int Point::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_X: {
        return manipulator(&d_x, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_X]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_Y: {
        return manipulator(&d_y, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_Y]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

inline
double& Point::x()
{
    return d_x;
}

inline
double& Point::y()
{
    return d_y;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Point::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_x, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_X]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_y, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_Y]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Point::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_X: {
        return accessor(d_x, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_X]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_Y: {
        return accessor(d_y, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_Y]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

inline
const double& Point::x() const
{
    return d_x;
}

inline
const double& Point::y() const
{
    return d_y;
}

}  // close namespace geom;

// FREE OPERATORS
inline
int geom::operator==(const geom::Point& lhs, const geom::Point& rhs)
{
    return  lhs.x() == rhs.x()
         && lhs.y() == rhs.y();
}

inline
int geom::operator!=(const geom::Point& lhs, const geom::Point& rhs)
{
    return  lhs.x() != rhs.x()
         || lhs.y() != rhs.y();
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_point.cpp  -*-C++-*-
// #include <geom_point.h>

#include <iostream>

namespace BloombergLP {
namespace geom {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Point::CLASS_NAME[] = "Point";
    // the name of this class

const double Point::DEFAULT_X = 0;
    // default value of 'X' attribute
const double Point::DEFAULT_Y = 0;
    // default value of 'Y' attribute

const bdeat_AttributeInfo Point::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_INDEX_X,
        ATTRIBUTE_ID_X,
        "X",                  // name
        sizeof("X") - 1,      // name length
        "todo: provide annotation"           // annotation
    },
    {
        ATTRIBUTE_INDEX_Y,
        ATTRIBUTE_ID_Y,
        "Y",                  // name
        sizeof("Y") - 1,      // name length
        "todo: provide annotation"           // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

                                // --------
                                // CREATORS
                                // --------


                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

}  // close namespace geom;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_circle.h   -*-C++-*-
#ifndef INCLUDED_GEOM_CIRCLE
#define INCLUDED_GEOM_CIRCLE

//@PURPOSE:
//  TODO: provide purpose
//
//@CLASSES: Circle
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TODO: provide annotation

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_GEOM_POINT
// #include <geom_point.h>
#endif

namespace BloombergLP {

namespace geom {

class Circle {

  private:
    Point d_center; // todo: provide annotation
    double d_radius; // todo: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_CENTER = 0,
            // index for "Center" attribute
        ATTRIBUTE_INDEX_RADIUS = 1
            // index for "Radius" attribute
    };

    enum {
        ATTRIBUTE_ID_CENTER = 1,
            // id for "Center" attribute
        ATTRIBUTE_ID_RADIUS = 2
            // id for "Radius" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Circle")

    static const double DEFAULT_RADIUS;
        // default value of "Radius" attribute

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(Circle, bdeat_TypeTraitBasicSequence);

  public:
    // CREATORS
    Circle();
        // Create an object of type 'Circle' having the default value.

    Circle(const Circle& original);
        // Create an object of type 'Circle' having the value
        // of the specified 'original' object.

    ~Circle();
        // Destroy this object.

    // MANIPULATORS
    Circle& operator=(const Circle& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    Point& center();
        // Return a reference to the modifiable "Center" attribute of this object.

    double& radius();
        // Return a reference to the modifiable "Radius" attribute of this object.


    // ACCESSORS
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    const Point& center() const;
        // Return a reference to the non-modifiable "Center"
        // attribute of this object.

    const double& radius() const;
        // Return a reference to the non-modifiable "Radius"
        // attribute of this object.

};

// FREE OPERATORS
inline
int operator==(const Circle& lhs, const Circle& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects have the
    // same value, and 0 otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
int operator!=(const Circle& lhs, const Circle& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects do not have
    // the same value, and 0 otherwise.  Two attribute objects do not have the
    // same value if one or more respective attributes differ in values.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

// ---------------------------------------------------------------------------

// CREATORS
inline
Circle::Circle()
: d_center()
, d_radius(DEFAULT_RADIUS)
{
}

inline
Circle::Circle(const Circle& original)
: d_center(original.d_center)
, d_radius(original.d_radius)
{
}

inline
Circle::~Circle()
{
}

// MANIPULATORS
inline
Circle&
Circle::operator=(const Circle& rhs)
{
    if (this != &rhs) {
        d_center = rhs.d_center;
        d_radius = rhs.d_radius;
    }
    return *this;
}

inline
void Circle::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_center);
    d_radius = DEFAULT_RADIUS;
}

template <class MANIPULATOR>
inline
int Circle::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_center, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CENTER]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_radius, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RADIUS]);
    if (ret) {
        return ret;                                                 // RETURN
    }


    return ret;
}

template <class MANIPULATOR>
inline
int Circle::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CENTER: {
        return manipulator(&d_center, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CENTER]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_RADIUS: {
        return manipulator(&d_radius, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RADIUS]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

inline
Point& Circle::center()
{
    return d_center;
}

inline
double& Circle::radius()
{
    return d_radius;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Circle::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_center, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CENTER]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_radius, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RADIUS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Circle::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CENTER: {
        return accessor(d_center, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CENTER]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_RADIUS: {
        return accessor(d_radius, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RADIUS]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

inline
const Point& Circle::center() const
{
    return d_center;
}

inline
const double& Circle::radius() const
{
    return d_radius;
}

}  // close namespace geom;

// FREE OPERATORS
inline
int geom::operator==(const geom::Circle& lhs, const geom::Circle& rhs)
{
    return  lhs.center() == rhs.center()
         && lhs.radius() == rhs.radius();
}

inline
int geom::operator!=(const geom::Circle& lhs, const geom::Circle& rhs)
{
    return  lhs.center() != rhs.center()
         || lhs.radius() != rhs.radius();
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_circle.cpp  -*-C++-*-
// #include <geom_circle.h>

#include <iostream>

// #include <geom_point.h>

namespace BloombergLP {
namespace geom {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Circle::CLASS_NAME[] = "Circle";
    // the name of this class

const double Circle::DEFAULT_RADIUS = 0;
    // default value of 'Radius' attribute

const bdeat_AttributeInfo Circle::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_INDEX_CENTER,
        ATTRIBUTE_ID_CENTER,
        "Center",                  // name
        sizeof("Center") - 1,      // name length
        "todo: provide annotation"           // annotation
    },
    {
        ATTRIBUTE_INDEX_RADIUS,
        ATTRIBUTE_ID_RADIUS,
        "Radius",                  // name
        sizeof("Radius") - 1,      // name length
        "todo: provide annotation"           // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

                                // --------
                                // CREATORS
                                // --------


                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

}  // close namespace geom;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_polygontype.h   -*-C++-*-
#ifndef INCLUDED_GEOM_POLYGONTYPE
#define INCLUDED_GEOM_POLYGONTYPE

//@PURPOSE:
//  TODO: provide purpose
//
//@CLASSES: PolygonType
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TODO: provide annotation

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

namespace BloombergLP {

namespace geom {

struct PolygonType {

    // TYPES
    enum Value {
        TRIANGLE = 0,
            // todo: provide annotation
        RECTANGLE = 1,
            // todo: provide annotation
        RHOMBUS = 2
            // todo: provide annotation
    };

    enum {
        NUM_ENUMERATORS = 3 // the number of enumerators in the 'Value' enumeration
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "PolygonType")

    static const bdeat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

    // CLASS METHODS
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).
};

// FREE OPERATORS

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// The following inlined functions are invoked from other inline functions.

inline
int PolygonType::fromInt(PolygonType::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case TRIANGLE:
      case RECTANGLE:
      case RHOMBUS:
        *result = (PolygonType::Value)number;
        return SUCCESS;                                         // RETURN
      default:
        return NOT_FOUND;
    }
}

// ---------------------------------------------------------------------------

// CLASS METHODS
inline
const char *PolygonType::toString(PolygonType::Value value)
{
    switch (value) {
      case TRIANGLE: {
        return "triangle";
      } break;
      case RECTANGLE: {
        return "rectangle";
      } break;
      case RHOMBUS: {
        return "rhombus";
      } break;
      default:
        BDE_ASSERT_H(!"encountered out-of-bound enumerated value");
    }

    return 0;
}

}  // close namespace geom;

// TRAITS
template <>
struct bdealg_TypeTraits<geom::PolygonType::Value>
: bdeat_TypeTraitBasicEnumeration
{
    typedef geom::PolygonType Wrapper;
};

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_polygontype.cpp  -*-C++-*-
// #include <geom_polygontype.h>

#include <iostream>

#include <bdes_assert.h>

namespace BloombergLP {
namespace geom {

                               // ---------
                               // CONSTANTS
                               // ---------

const char PolygonType::CLASS_NAME[] = "PolygonType";
    // the name of this class

const bdeat_EnumeratorInfo PolygonType::ENUMERATOR_INFO_ARRAY[] = {
    {
        PolygonType::TRIANGLE,
        "Triangle",                  // name
        sizeof("Triangle") - 1,      // name length
        "todo: provide annotation"            // annotation
    },
    {
        PolygonType::RECTANGLE,
        "Rectangle",                  // name
        sizeof("Rectangle") - 1,      // name length
        "todo: provide annotation"            // annotation
    },
    {
        PolygonType::RHOMBUS,
        "Rhombus",                  // name
        sizeof("Rhombus") - 1,      // name length
        "todo: provide annotation"            // annotation
    }
};
                               // -------------
                               // CLASS METHODS
                               // -------------

                                // --------
                                // CREATORS
                                // --------


                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

}  // close namespace geom;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_polygon.h   -*-C++-*-
#ifndef INCLUDED_GEOM_POLYGON
#define INCLUDED_GEOM_POLYGON

//@PURPOSE:
//  TODO: provide purpose
//
//@CLASSES: Polygon
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TODO: provide annotation

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef INCLUDED_GEOM_POINT
// #include <geom_point.h>
#endif

#ifndef INCLUDED_GEOM_POLYGONTYPE
// #include <geom_polygontype.h>
#endif

namespace BloombergLP {

class bdema_Allocator;

namespace geom {

class Polygon {

  private:
    std::vector<Point> d_vertices; // todo: provide annotation
    PolygonType::Value d_type; // todo: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_VERTICES = 0,
            // index for "Vertices" attribute
        ATTRIBUTE_INDEX_TYPE = 1
            // index for "Type" attribute
    };

    enum {
        ATTRIBUTE_ID_VERTICES = 1,
            // id for "Vertices" attribute
        ATTRIBUTE_ID_TYPE = 2
            // id for "Type" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Polygon")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS2(Polygon,
                                  bdeat_TypeTraitBasicSequence,
                                  bdealg_TypeTraitUsesBdemaAllocator);

  public:
    // CREATORS
    explicit Polygon(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Polygon' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    Polygon(const Polygon& original, bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Polygon' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~Polygon();
        // Destroy this object.

    // MANIPULATORS
    Polygon& operator=(const Polygon& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    std::vector<Point>& vertices();
        // Return a reference to the modifiable "Vertices" attribute of this object.

    PolygonType::Value& type();
        // Return a reference to the modifiable "Type" attribute of this object.


    // ACCESSORS
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    const std::vector<Point>& vertices() const;
        // Return a reference to the non-modifiable "Vertices"
        // attribute of this object.

    const PolygonType::Value& type() const;
        // Return a reference to the non-modifiable "Type"
        // attribute of this object.

};

// FREE OPERATORS
inline
int operator==(const Polygon& lhs, const Polygon& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects have the
    // same value, and 0 otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
int operator!=(const Polygon& lhs, const Polygon& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects do not have
    // the same value, and 0 otherwise.  Two attribute objects do not have the
    // same value if one or more respective attributes differ in values.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

// ---------------------------------------------------------------------------

// CREATORS
inline
Polygon::Polygon(bdema_Allocator *basicAllocator)
: d_vertices(bdema_Default::allocator(basicAllocator))
#ifndef BDES_PLATFORM__CMP_MSVC
, d_type()
#else
, d_type(PolygonType::TRIANGLE)
#endif
{
}

inline
Polygon::Polygon(
    const Polygon& original,
    bdema_Allocator *basicAllocator)
: d_vertices(original.d_vertices, bdema_Default::allocator(basicAllocator))
, d_type(original.d_type)
{
}

inline
Polygon::~Polygon()
{
}

// MANIPULATORS
inline
Polygon&
Polygon::operator=(const Polygon& rhs)
{
    if (this != &rhs) {
        d_vertices = rhs.d_vertices;
        d_type = rhs.d_type;
    }
    return *this;
}

inline
void Polygon::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_vertices);
    bdeat_ValueTypeFunctions::reset(&d_type);
}

template <class MANIPULATOR>
inline
int Polygon::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_vertices, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VERTICES]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
    if (ret) {
        return ret;                                                 // RETURN
    }


    return ret;
}

template <class MANIPULATOR>
inline
int Polygon::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VERTICES: {
        return manipulator(&d_vertices, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VERTICES]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_TYPE: {
        return manipulator(&d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

inline
std::vector<Point>& Polygon::vertices()
{
    return d_vertices;
}

inline
PolygonType::Value& Polygon::type()
{
    return d_type;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Polygon::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_vertices, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VERTICES]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Polygon::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VERTICES: {
        return accessor(d_vertices, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VERTICES]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_TYPE: {
        return accessor(d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

inline
const std::vector<Point>& Polygon::vertices() const
{
    return d_vertices;
}

inline
const PolygonType::Value& Polygon::type() const
{
    return d_type;
}

}  // close namespace geom;

// FREE OPERATORS
inline
int geom::operator==(const geom::Polygon& lhs, const geom::Polygon& rhs)
{
    return  lhs.vertices() == rhs.vertices()
         && lhs.type() == rhs.type();
}

inline
int geom::operator!=(const geom::Polygon& lhs, const geom::Polygon& rhs)
{
    return  lhs.vertices() != rhs.vertices()
         || lhs.type() != rhs.type();
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_polygon.cpp  -*-C++-*-
// #include <geom_polygon.h>

#include <iostream>

#include <vector>
// #include <geom_point.h>
// #include <geom_polygontype.h>

namespace BloombergLP {
namespace geom {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Polygon::CLASS_NAME[] = "Polygon";
    // the name of this class


const bdeat_AttributeInfo Polygon::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_INDEX_VERTICES,
        ATTRIBUTE_ID_VERTICES,
        "Vertices",                  // name
        sizeof("Vertices") - 1,      // name length
        "todo: provide annotation"           // annotation
    },
    {
        ATTRIBUTE_INDEX_TYPE,
        ATTRIBUTE_ID_TYPE,
        "Type",                  // name
        sizeof("Type") - 1,      // name length
        "todo: provide annotation"           // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

                                // --------
                                // CREATORS
                                // --------


                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------


}  // close namespace geom;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_figure.h   -*-C++-*-
#ifndef INCLUDED_GEOM_FIGURE
#define INCLUDED_GEOM_FIGURE

//@PURPOSE:
//  TODO: provide purpose
//
//@CLASSES: Figure
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  TODO: provide annotation

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_BDES_OBJECTBUFFER
#include <bdes_objectbuffer.h>
#endif

#ifndef INCLUDED_GEOM_CIRCLE
// #include <geom_circle.h>
#endif

#ifndef INCLUDED_GEOM_POLYGON
// #include <geom_polygon.h>
#endif

namespace BloombergLP {

class bdema_Allocator;

namespace geom {

class Figure {

    union {
        bdes_ObjectBuffer< Circle > d_circle;
                // todo: provide annotation
        bdes_ObjectBuffer< Polygon > d_polygon;
                // todo: provide annotation
    };

    int                 d_selectionId;

    bdema_Allocator    *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_CIRCLE = 0,
            // index for "Circle" selection
        SELECTION_INDEX_POLYGON = 1
            // index for "Polygon" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = 0,

        SELECTION_ID_CIRCLE = 1,
            // id for "Circle" selection
        SELECTION_ID_POLYGON = 2
            // id for "Polygon" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Figure")


    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS2(Figure,
                                  bdeat_TypeTraitBasicChoice,
                                  bdealg_TypeTraitUsesBdemaAllocator);

  public:
    // CREATORS
    explicit Figure(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Figure' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    Figure(const Figure& original, bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Figure' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~Figure();
        // Destroy this object.

    // MANIPULATORS
    Figure& operator=(const Figure& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    void makeCircle();
    void makeCircle(const Circle& value);
        // Set the value of this object to be a "Circle" value.
        // Optionally specify the 'value' of the "Circle".  If
        // 'value' is not specified, the default "Circle" value is
        // used.

    void makePolygon();
    void makePolygon(const Polygon& value);
        // Set the value of this object to be a "Polygon" value.
        // Optionally specify the 'value' of the "Polygon".  If
        // 'value' is not specified, the default "Polygon" value is
        // used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Circle& circle();
        // Return a reference to the modifiable "Circle" selection
        // of this object if "Circle" is the current selection.
        // The behavior is undefined unless "Circle" is the
        // selection of this object.

    Polygon& polygon();
        // Return a reference to the modifiable "Polygon" selection
        // of this object if "Polygon" is the current selection.
        // The behavior is undefined unless "Polygon" is the
        // selection of this object.

    // ACCESSORS
    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and 'SELECTION_ID_UNDEFINED' otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Circle& circle() const;
        // Return a reference to the non-modifiable "Circle"
        // selection of this object if "Circle" is the current
        // selection.  The behavior is undefined unless "Circle"
        // is the selection of this object.

    const Polygon& polygon() const;
        // Return a reference to the non-modifiable "Polygon"
        // selection of this object if "Polygon" is the current
        // selection.  The behavior is undefined unless "Polygon"
        // is the selection of this object.

};

// FREE OPERATORS
inline
int operator==(const Figure& lhs, const Figure& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' objects have the same value,
    // and 0 otherwise.  Two 'Figure' objects have the same value
    // if either the selections in both objects have the same ids and the same
    // values, or both selections are undefined.

inline
int operator!=(const Figure& lhs, const Figure& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' objects do not have the same
    // values, as determined by 'operator==', and 0 otherwise.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
void Figure::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_CIRCLE: {
        d_circle.object().~Circle();
      } break;
      case SELECTION_ID_POLYGON: {
        d_polygon.object().~Polygon();
      } break;
      default:
        BDE_ASSERT_H(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
void Figure::makeCircle()
{
    if (SELECTION_ID_CIRCLE == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_circle.object());
    }
    else {
        reset();
        new (d_circle.buffer()) Circle;
        d_selectionId = SELECTION_ID_CIRCLE;
    }
}

inline
void Figure::makeCircle(const Circle& value)
{
    if (SELECTION_ID_CIRCLE == d_selectionId) {
        d_circle.object() = value;
    }
    else {
        reset();
        new (d_circle.buffer()) Circle(value);
        d_selectionId = SELECTION_ID_CIRCLE;
    }
}

inline
void Figure::makePolygon()
{
    if (SELECTION_ID_POLYGON == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_polygon.object());
    }
    else {
        reset();
        new (d_polygon.buffer()) Polygon(d_allocator_p);
        d_selectionId = SELECTION_ID_POLYGON;
    }
}

inline
void Figure::makePolygon(const Polygon& value)
{
    if (SELECTION_ID_POLYGON == d_selectionId) {
        d_polygon.object() = value;
    }
    else {
        reset();
        new (d_polygon.buffer()) Polygon(value, d_allocator_p);
        d_selectionId = SELECTION_ID_POLYGON;
    }
}

// ---------------------------------------------------------------------------

// CREATORS
inline
Figure::Figure(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
Figure::Figure(
    const Figure& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_CIRCLE: {
        new (d_circle.buffer())
            Circle(original.d_circle.object());
      } break;
      case SELECTION_ID_POLYGON: {
        new (d_polygon.buffer())
            Polygon(original.d_polygon.object(), d_allocator_p);
      } break;
      default:
        BDE_ASSERT_H(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
Figure::~Figure()
{
    reset();
}

// MANIPULATORS
inline
Figure&
Figure::operator=(const Figure& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CIRCLE: {
            makeCircle(rhs.d_circle.object());
          } break;
          case SELECTION_ID_POLYGON: {
            makePolygon(rhs.d_polygon.object());
          } break;
          default:
            BDE_ASSERT_H(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

inline
int Figure::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_CIRCLE: {
        makeCircle();
      } break;
      case SELECTION_ID_POLYGON: {
        makePolygon();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                           // RETURN
    }
    return SUCCESS;
}

template <class MANIPULATOR>
inline
int Figure::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case Figure::SELECTION_ID_CIRCLE:
        return manipulator(&d_circle.object(), SELECTION_INFO_ARRAY[SELECTION_INDEX_CIRCLE]);        // RETURN
      case Figure::SELECTION_ID_POLYGON:
        return manipulator(&d_polygon.object(), SELECTION_INFO_ARRAY[SELECTION_INDEX_POLYGON]);        // RETURN
      default:
        BDE_ASSERT_H(Figure::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
    }
}

inline
Circle& Figure::circle()
{
    BDE_ASSERT_H(SELECTION_ID_CIRCLE == d_selectionId);
    return d_circle.object();
}

inline
Polygon& Figure::polygon()
{
    BDE_ASSERT_H(SELECTION_ID_POLYGON == d_selectionId);
    return d_polygon.object();
}

// ACCESSORS

inline
int Figure::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int Figure::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_CIRCLE:
        return accessor(d_circle.object(), SELECTION_INFO_ARRAY[SELECTION_INDEX_CIRCLE]);        // RETURN
      case SELECTION_ID_POLYGON:
        return accessor(d_polygon.object(), SELECTION_INFO_ARRAY[SELECTION_INDEX_POLYGON]);        // RETURN
      default:
        BDE_ASSERT_H(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
    }
}

inline
const Circle& Figure::circle() const
{
    BDE_ASSERT_H(SELECTION_ID_CIRCLE == d_selectionId);
    return d_circle.object();
}

inline
const Polygon& Figure::polygon() const
{
    BDE_ASSERT_H(SELECTION_ID_POLYGON == d_selectionId);
    return d_polygon.object();
}

}  // close namespace geom;

// FREE OPERATORS
inline
int geom::operator==(const geom::Figure& lhs, const geom::Figure& rhs)
{
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case geom::Figure::SELECTION_ID_CIRCLE:
            return lhs.circle() == rhs.circle();
                                                                    // RETURN
          case geom::Figure::SELECTION_ID_POLYGON:
            return lhs.polygon() == rhs.polygon();
                                                                    // RETURN
          default:
            BDE_ASSERT_H(geom::Figure::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return 1;                                               // RETURN
        }
    }
    else {
        return 0;
   }
}

inline
int geom::operator!=(const geom::Figure& lhs, const geom::Figure& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// geom_figure.cpp  -*-C++-*-
// #include <geom_figure.h>

#include <iostream>

// #include <geom_circle.h>
// #include <geom_polygon.h>

namespace BloombergLP {
namespace geom {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Figure::CLASS_NAME[] = "Figure";
    // the name of this class


const bdeat_SelectionInfo Figure::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_INDEX_CIRCLE,
        SELECTION_ID_CIRCLE,
        "Circle",                  // name
        sizeof("Circle") - 1,      // name length
        "todo: provide annotation",           // annotation
    },
    {
        SELECTION_INDEX_POLYGON,
        SELECTION_ID_POLYGON,
        "Polygon",                  // name
        sizeof("Polygon") - 1,      // name length
        "todo: provide annotation",           // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

                                // --------
                                // CREATORS
                                // --------


                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

}  // close namespace geom;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

// ****** END OF GENERATED CODE ******

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF CHOICES, ENUMERATIONS, SEQUENCES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Vectors of Choices, Enumerations, Sequences"
                 << endl
                 << "==================================================="
                 << endl;

        const int NUM_VALUES = 256;

        {
            typedef std::vector<geom::PolygonType::Value> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<geom::Point> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<geom::Circle> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<geom::Polygon> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<geom::Figure> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CHOICES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Choices" << endl
                                  << "===============" << endl;

        const int NUM_VALUES = 256;

        {
            typedef geom::Figure Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Sequences" << endl
                                  << "=================" << endl;

        const int NUM_VALUES = 256;

        {
            typedef geom::Point Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef geom::Circle Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef geom::Polygon Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Enumerations" << endl
                                  << "====================" << endl;

        {
            typedef geom::PolygonType::Value Type;

            const int NUM_VALUES = geom::PolygonType::NUM_ENUMERATORS;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV(X[0]);  const Type& V = mV;
            Type mW(X[1]);  const Type& W = mW;

            ASSERT(V == X[0]);
            ASSERT(0 != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT(V == X[0]);

            ASSERT(W == X[1]);
            ASSERT(0 != Obj::loadTestValue(&mW, NUM_VALUES));
            ASSERT(W == X[1]);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF SIMPLE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Vectors of Simple Types" << endl
                                  << "===============================" << endl;

        // Vectors have no prescribed size limit, so test something reasonable.

        const int NUM_VALUES = 256;

        {
            typedef std::vector<bool> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<char> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<signed char> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<unsigned char> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<short> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<unsigned short> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<int> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<unsigned int> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<bdes_PlatformUtil::Int64> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<bdes_PlatformUtil::Uint64> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<float> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<double> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<std::string> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<bdet_Date> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<bdet_Datetime> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

        {
            typedef std::vector<bdet_Time> Type;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING SIMPLE TYPES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Simple Types" << endl
                                  << "====================" << endl;

        {
            typedef bool Type;

            const int NUM_VALUES = 2;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;  const Type& V = mV;

            mV = false;
            ASSERT(false == V);
            ASSERT(0     != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT(false == V);

            mV = true;
            ASSERT(true  == V);
            ASSERT(0     != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT(true  == V);
        }

        {
            typedef char Type;

            const int NUM_VALUES = UCHAR_MAX + 1;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;  const Type& V = mV;

            mV = 'A';
            ASSERT('A' == V);
            ASSERT( 0  != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT('A' == V);

            mV = 'Z';
            ASSERT('Z' == V);
            ASSERT( 0  != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT('Z' == V);
        }

        {
            typedef signed char Type;

            const int NUM_VALUES = UCHAR_MAX + 1;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;  const Type& V = mV;

            mV = 'A';
            ASSERT('A' == V);
            ASSERT( 0  != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT('A' == V);

            mV = 'Z';
            ASSERT('Z' == V);
            ASSERT( 0  != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT('Z' == V);
        }

        {
            typedef unsigned char Type;

            const int NUM_VALUES = UCHAR_MAX + 1;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;  const Type& V = mV;

            mV = 'A';
            ASSERT('A' == V);
            ASSERT( 0  != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT('A' == V);

            mV = 'Z';
            ASSERT('Z' == V);
            ASSERT( 0  != Obj::loadTestValue(&mV, NUM_VALUES));
            ASSERT('Z' == V);
        }

        {
            typedef short Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;  const Type& V = mV;

            mV = 66;
            ASSERT(66 == V);
            ASSERT( 0 != Obj::loadTestValue(&mV, USHRT_MAX + 1));
            ASSERT(66 == V);

            mV = 77;
            ASSERT(77 == V);
            ASSERT( 0 != Obj::loadTestValue(&mV, USHRT_MAX + 1));
            ASSERT(77 == V);
        }

        {
            typedef unsigned short Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;  const Type& V = mV;

            mV = 66;
            ASSERT(66 == V);
            ASSERT( 0 != Obj::loadTestValue(&mV, USHRT_MAX + 1));
            ASSERT(66 == V);

            mV = 77;
            ASSERT(77 == V);
            ASSERT( 0 != Obj::loadTestValue(&mV, USHRT_MAX + 1));
            ASSERT(77 == V);
        }

        {
            typedef int Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef unsigned int Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef bdes_PlatformUtil::Int64 Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef bdes_PlatformUtil::Uint64 Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef float Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef double Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef std::string Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            Type mV;
            ASSERT(0 == Obj::loadTestValue(&mV, INT_MAX));
        }

        {
            typedef bdet_Date Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            const int MAX_DAYS = ((9999 - 1753 + 1) * 12) * 28 - 1;

            Type mY;
            ASSERT(0 == Obj::loadTestValue(&mY, MAX_DAYS));

            Type mV(X[0]);  const Type& V = mV;
            Type mW(X[1]);  const Type& W = mW;

            ASSERT(V == X[0]);
            ASSERT(0 != Obj::loadTestValue(&mV, MAX_DAYS + 1));
            ASSERT(V == X[0]);

            ASSERT(W == X[1]);
            ASSERT(0 != Obj::loadTestValue(&mW, MAX_DAYS + 1));
            ASSERT(W == X[1]);
        }

        {
            typedef bdet_Datetime Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            const int MAX_DAYS = ((9999 - 1753 + 1) * 12) * 28 - 1;

            Type mY;
            ASSERT(0 == Obj::loadTestValue(&mY, MAX_DAYS));

            Type mV(X[0]);  const Type& V = mV;
            Type mW(X[1]);  const Type& W = mW;

            ASSERT(V == X[0]);
            ASSERT(0 != Obj::loadTestValue(&mV, MAX_DAYS + 1));
            ASSERT(V == X[0]);

            ASSERT(W == X[1]);
            ASSERT(0 != Obj::loadTestValue(&mW, MAX_DAYS + 1));
            ASSERT(W == X[1]);
        }

        {
            typedef bdet_Time Type;

            const int NUM_VALUES = 256;

            std::vector<Type> mX;  const std::vector<Type>& X = mX;

            for (int ti = 0; ti < NUM_VALUES; ++ti) {
                Type mY;  const Type& Y = mY;
                LOOP_ASSERT(ti, 0 == Obj::loadTestValue(&mY, ti));
                mX.push_back(Y);
            }
            ASSERT(true == areElementsUnique(X));

            const int MAX_MILLISECS = 86400000 - 1;

            Type mY;
            ASSERT(0 == Obj::loadTestValue(&mY, MAX_MILLISECS));

            Type mV(X[0]);  const Type& V = mV;
            Type mW(X[1]);  const Type& W = mW;

            ASSERT(V == X[0]);
            ASSERT(0 != Obj::loadTestValue(&mV, MAX_MILLISECS + 1));
            ASSERT(V == X[0]);

            ASSERT(W == X[1]);
            ASSERT(0 != Obj::loadTestValue(&mW, MAX_MILLISECS + 1));
            ASSERT(W == X[1]);
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
