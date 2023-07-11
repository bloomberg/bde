// bdlat_valuetypefunctions.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#define INCLUDED_BDLAT_VALUETYPEFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for "value type" functions.
//
//@CLASSES:
//  bdlat_ValueTypeFunctions: namespace for "value type" functions
//
//@DESCRIPTION: This component provides a 'namespace',
// 'bdlat_ValueTypeFunctions', defining functions that may be called on "value
// types".
//
// The functions in this namespace allow users to:
//
//: o 'assign' values to "value type" objects (ie., as if by using the
//:   assignment operator '*lhs = rhs'), and
//:
//: o 'reset' "value type" objects to their default state (i.e., as if each
//:   object was just constructed using its default constructor).
//
// Types in the 'bdlat' "value type" framework are required to have:
//: o a default constructor
//: o a copy assignment operator
//: o the free function template and the free function described below.
//
// Types in the 'bdlat' "value type" framework must define in the namespace
// where the type is defined, overloads of the following free function template
// and free frunction.  Note that the placeholder 'YOUR_TYPE' is not a template
// argument and should be replaced with the name of the type being plugged into
// the framework:
//..
//  // MANIPULATORS
//  template <class RHS_TYPE>
//  int bdlat_valueTypeAssign(YOUR_TYPE *lhs, const RHS_TYPE& rhs);
//      // Assign to the specified 'lhs' the value if the specified 'rhs'.
//      // Return 0 on success and a non-zero value otherwise.  If setting
//      // 'lhs' to 'rhs' would violate any preconditions, a non-zero value is
//      // returned.  If (template parameter) 'RHS_TYPE' cannot be used to set
//      // the value of 'lhs', a non-zero value is returned.
//
//  void bdlat_valueTypeReset(YOUR_TYPE *object);
//      // Reset the specified 'object' to that of its default state (i.e., to
//      // 'YOUR_TYPE()').
//..
// Notice that, unlike other 'bdlat' type infrastructures, the 'bdlat' "value"
// infrastructure does *not* require the setting of any traits, or definition
// of meta-functions, or creation of any 'typedef's.  For example, see
// {'bdlat_arrayfunctions'} and {'bdlat_nullablevaluefunctions'}.
//
// There are two significant implications of the contract for
// 'bdlat_valueTypeAssign':
//
//: o Interactions with incompatible types are handled at run-time, not
//:   compile-time.
//:
//: o Users of the 'bdlat_valueTypeAssign' function deal with a wide contract,
//:   even if the corresponding operation in 'YOUR_TYPE' has a narrow contract.
//
///Supported Types
///---------------
// This component provides "value type" support for the following types:
//
//: o All types with a default constructor and copy assignment operator.
//:
//: o Types having any of the following traits (see {'bdlat_typetraits'}):
//:
//:   o 'bdlat_TypeTraitBasicSequence'
//:   o 'bdlat_TypeTraitBasicChoice'
//:   o 'bdlat_TypeTraitBasicCustomizedType'
//:   o 'bsl::vector'
//:   o 'bsl::basic_string'
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implicit "Value Type"
/// - - - - - - - - - - - - - - - -
// Suppose you had a type that defines a "value".
//..
//  namespace BloombergLP {
//  namespace mine {
//
//  struct MyValueType {
//      int    d_int;
//      double d_double;
//  };
//
//  }  // close package namespace
//  }  // close enterprise namespace
//..
// Although our definition of 'MyValueType' was rather terse, several methods
// are implicitly defined by the compiler:
//..
//  void f()
//  {
//      using namespace BloombergLP;
//
//      mine::MyValueType a = { 1, 1.0 };  // aggregate braced initialization
//      mine::MyValueType b(a);            // implicit copy constructor
//
//      assert(b.d_int    == a.d_int);
//      assert(b.d_double == a.d_double);
//
//      a.d_int    = 2;
//      a.d_double = 3.14;
//
//      b = a;                             // implicit copy assignment operator
//
//      assert(b.d_int    == a.d_int);
//      assert(b.d_double == a.d_double);
//  }
//..
// Notice that the implicitly defined methods include a copy constructor and a
// copy assignment operator thereby implicitly making 'MyValueType' part of the
// 'bdlat' "value" framework.  As such, it can be manipulated using the methods
// of 'bdlat_ValueTypeFunctions':
//..
//  void myUsageScenario()
//  {
//      using namespace BloombergLP;
//
//      mine::MyValueType x = {  7, 10.0 };
//      mine::MyValueType y = { 99, -1.0 };
//
//      assert(x.d_int    != y.d_int);
//      assert(x.d_double != y.d_double);
//
//      int rc = bdlat_ValueTypeFunctions::assign(&x, y);
//      assert(0 == rc);
//
//      assert(x.d_int    == y.d_int);
//      assert(x.d_double == y.d_double);
//
//      bdlat_ValueTypeFunctions::reset(&y);
//
//      assert(x.d_int    != y.d_int);
//      assert(x.d_double != y.d_double);
//
//      assert(int()    == y.d_int);
//      assert(double() == y.d_double);
//  }
//..
//
///Example 2: Interacting with Other Types
///- - - - - - - - - - - - - - - - - - - -
// Suppose you want to enhance 'mine::MyValueType' to allow its value to be
// assigned from a 'bsl::pair<int, float>' object?  Do do so, create
// 'your::YourValueType' which has an implicit conversion from
// 'bsl::pair<int, float>':
//..
//  namespace BloombergLP {
//  namespace your {
//
//  struct YourValueType {
//
//      int    d_int;
//      double d_double;
//
//      YourValueType()
//      : d_int()
//      , d_double() { }
//
//      YourValueType(const YourValueType& original)
//      : d_int   (original.d_int)
//      , d_double(original.d_double) { }
//
//      YourValueType(int intValue, double doubleValue)
//      : d_int   (   intValue)
//      , d_double(doubleValue) { }
//
//      YourValueType(const bsl::pair<int, double>& value) // IMPLICIT
//      : d_int   (value.first)
//      , d_double(value.second) { }
//
//      YourValueType & operator=(const YourValueType& original) {
//          d_int    = original.d_int;
//          d_double = original.d_double;
//          return *this;
//      }
//  };
//
//  }  // close package namespace
//  }  // close enterprise namespace
//..
// Notice that, having defined a constructor, the compiler no longer generates
// the constructors that had been generated implicitly.  Accordingly, we have
// added a default constructor, copy constructor and assignment operator.
// Also, since aggregate initialization is no longer allowed, we have also
// added a value constructor and slightly modified the syntax of initialization
// in function 'g()' below:
//..
//  void g()
//  {
//      using namespace BloombergLP;
//
//      your::YourValueType a(1, 1.0);     // value initialization
//      your::YourValueType b(a);          // implicit copy constructor
//
//      assert(b.d_int    == a.d_int);
//      assert(b.d_double == a.d_double);
//
//      a.d_int    = 2;
//      a.d_double = 3.14;
//
//      b = a;                             // implicit copy assignment operator
//
//      assert(b.d_int    == a.d_int);
//      assert(b.d_double == a.d_double);
//
//      bsl::pair<int, double> value(4, 5.0);
//
//      a = value;
//
//      assert(4   == a.d_int);
//      assert(5.0 == a.d_double);
//  }
//..
// Since both copy construction and assignment are defined, 'YourValueType' can
// be handled by the 'bdlat' "value" infrastructure in much the same way as we
// did for 'MyValueType':
//..
//  void yourUsageScenario()
//  {
//      using namespace BloombergLP;
//      int rc;
//
//      your::YourValueType x( 7, 10.0);
//      your::YourValueType y(99, -1.0);
//
//      assert(x.d_int    != y.d_int);
//      assert(x.d_double != y.d_double);
//
//      rc = bdlat_ValueTypeFunctions::assign(&x, y);
//      assert(0 == rc);
//
//      assert(x.d_int    == y.d_int);
//      assert(x.d_double == y.d_double);
//
//      bdlat_ValueTypeFunctions::reset(&y);
//
//      assert(x.d_int    != y.d_int);
//      assert(x.d_double != y.d_double);
//
//      assert(int()   == y.d_int);
//      assert(float() == y.d_double);
//..
// However, since conversion from another type, 'bsl::pair<int, double>', is
// provided, the 'bdlat' "value" infrastructure can also use that type to set
// the value of objects.
//..
//      bsl::pair<int, double> value(4, 5.0);
//
//      rc = bdlat_ValueTypeFunctions::assign(&y, value);
//      assert(0 == rc);
//
//      assert(value.first  == y.d_int);
//      assert(value.second == y.d_double);
//..
// Unsurprisingly, such assignments do not work for arbitrary other types (for
// which conversion is not defined).  What is notable, is that this code does
// compile and fails at run-time.
//..
//      // Assign an incompatible type.
//      rc = bdlat_ValueTypeFunctions::assign(&y, bsl::string("4, 5.0"));
//      assert(0 != rc);
//  }
//..
//
///Installing an Atypical "Value" Type
///- - - - - - - - - - - - - - - - - -
// Suppose someone defines a pernicious "value" type, 'their::TheirValueType',
// having neither copy constructor nor copy assignment operator:
//..
//  namespace BloombergLP {
//  namespace their {
//
//  class TheirValueType {
//
//      // DATA
//      int    d_int;
//      double d_double;
//
//    private:
//      // NOT IMPLEMENTED
//      TheirValueType(const TheirValueType& original);   // = delete
//      TheirValueType& operator=(const TheirValueType&); // = delete
//
//    public:
//      // CREATORS
//      TheirValueType()
//      : d_int()
//      , d_double() { }
//
//      // MANIPULATORS
//      void setValue(const bsl::string& valueString);
//
//      // ACCESSORS
//      int       intValue() const { return d_int;    }
//      double doubleValue() const { return d_double; }
//  };
//
//  // MANIPULATORS
//  void TheirValueType::setValue(const bsl::string& valueString)
//  {
//       bsl::string::size_type pos = valueString.find(',');
//       BSLS_ASSERT(bsl::string::npos != pos);
//
//       d_int    = bsl::atoi(valueString.c_str());
//       d_double = bsl::atof(valueString.c_str() + pos + 1);
//  }
//
//  }  // close package namespace
//  }  // close enterprise namespace
//..
// Such a type can be used after a fashion (objects created, states changed,
// state changes observed), albeit using syntax that is significantly different
// than we used for 'MyValueType' and 'YourValueType':
//..
//  void h()
//  {
//      using namespace BloombergLP;
//
//      their::TheirValueType a;               // default constructor
//
//      assert(0   == a.   intValue());
//      assert(0.0 == a.doubleValue());
//
//  //  their::TheirValueType b(a);       // Error, no copy constructor
//
//      their::TheirValueType c;
//  //  c = a;                            // Error, no copy assignment operator
//
//      a.setValue("2, 3.14");
//
//      assert(2    == a.   intValue());
//      assert(3.14 == a.doubleValue());
//  }
//..
// Since 'TheirValueType' lacks both copy construction and assignment, that
// type is not implicitly supported by the 'bdlat' "value" infrastructure.
//
// However, the 'TheirValueType' can be made compatible with that
// infrastructure if "they" define the required overloads of
// 'bdlat_valueTypeAssign' and 'bdlat_valueTypeReset' in 'their' namespace:
//..
//  namespace BloombergLP {
//  namespace their {
//
//  int bdlat_valueTypeAssign(TheirValueType        *lhs,
//                            const TheirValueType&  rhs)
//  {
//      BSLS_ASSERT(lhs);
//
//      bsl::ostringstream oss;
//      oss << rhs.intValue() << ", " << rhs.doubleValue();
//
//      lhs->setValue(oss.str());
//      return 0;
//  }
//
//  int bdlat_valueTypeAssign(TheirValueType     *lhs,
//                            const bsl::string&  rhs)
//  {
//      BSLS_ASSERT(lhs);
//
//      lhs->setValue(rhs);
//      return 0;
//  }
//
//  // Overload for any other 'RHS_TYPE' to return an error.
//  template <class RHS_TYPE>
//  int bdlat_valueTypeAssign(TheirValueType  *lhs,
//                            const RHS_TYPE&  rhs)
//  {
//      BSLS_ASSERT(lhs);
//      (void)lhs;
//      (void)rhs;
//
//      return -999;  // Pick a distinctive non-negative value.
//  }
//  void bdlat_valueTypeReset(TheirValueType *object)
//  {
//      BSLS_ASSERT(object);
//
//      bsl::ostringstream oss;
//      oss << int() << ", " << double();
//
//      object->setValue(oss.str());
//  }
//
//  }  // close package namespace
//  }  // close enterprise namespace
//..
// Notice that three overloads of 'bdlat_valueTypeAssign' are defined above:
//
//: o The first, the overload that allows 'TheirValueType' to be "assigned" to
//:   itself is required by the 'bdlat' "value" infrastructure.
//:
//: o The second, the overload that allows "assignment" from a 'bsl::string' is
//:   not technically required by the infrastructure, but is a practical
//:   requirement because 'bsl::string' is the only way 'TheirValueType' can be
//:   changed from its default value.
//:
//: o Finally, we provide an overload templated on an arbitrary 'RHS_TYPE' so
//:   that, if any other types are passed, the code will compile (as required)
//:   but also unconditionally fail (as required).
//
// With these points of customization in place, 'TheirValueType' can now be
// manipulated by the 'bdlat' "value" infrastructure in much the same manner as
// was done for 'MyValueType' and 'YourValueType':
//..
//  void theirUsageScenario()
//  {
//      using namespace BloombergLP;
//
//      their::TheirValueType x;
//      their::TheirValueType y;
//
//      int rc;
//
//      rc = bdlat_ValueTypeFunctions::assign(&x, bsl::string(" 7, 10.0"));
//      assert(0 == rc);
//
//      rc = bdlat_ValueTypeFunctions::assign(&y, bsl::string("99, -1.0"));
//      assert(0 == rc);
//
//      assert(x.intValue()    != y.intValue());
//      assert(x.doubleValue() != y.doubleValue());
//
//      rc = bdlat_ValueTypeFunctions::assign(&x, y);
//      assert(0 == rc);
//
//      assert(x.intValue()    == y.intValue());
//      assert(x.doubleValue() == y.doubleValue());
//
//      bdlat_ValueTypeFunctions::reset(&y);
//
//      assert(int()   == y.intValue());
//      assert(float() == y.doubleValue());
//
//      // Assign an incompatible type.
//
//      bsl::pair<int, double> value(4, 5.0);
//      rc = bdlat_ValueTypeFunctions::assign(&y, value);
//      assert(   0 != rc);
//      assert(-999 == rc);
//  }
//..

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_typecategory.h>
#include <bdlat_typetraits.h>

#include <bdlb_nullablevalue.h>

#include <bslalg_hastrait.h>

#include <bslmf_conditional.h>
#include <bslmf_isconvertible.h>

#include <bsls_platform.h>

#include <bsl_string.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_if.h>
#endif  // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

                      // ==================================
                      // namespace bdlat_ValueTypeFunctions
                      // ==================================

namespace bdlat_ValueTypeFunctions {
    // The functions provided in this 'namespace' may be applied to value
    // types.  See the component-level documentation for what is meant by
    // "value type".

    // MANIPULATORS
    template <class LHS_TYPE, class RHS_TYPE>
    int assign(LHS_TYPE *lhs, const RHS_TYPE& rhs);
        // Assign the value of the specified 'rhs' to the object specified its
        // address 'lhs'.  Return 0 if successful, and a non-zero value
        // otherwise.

    template <class TYPE>
    void reset(TYPE *object);
        // Reset the value of the specified 'object' to its default value.

}  // close namespace bdlat_ValueTypeFunctions

                            // ====================
                            // default declarations
                            // ====================

namespace bdlat_ValueTypeFunctions {
    // This namespace declaration adds the default implementations of the
    // "value type" customization-point functions to
    // 'bdlat_ValueTypeFunctions'.  These default implementations are provided
    // for a variety of types.

    // MANIPULATORS
    template <class LHS_TYPE, class RHS_TYPE>
    int bdlat_valueTypeAssign(LHS_TYPE *lhs, const RHS_TYPE& rhs);

    template <class TYPE>
    void bdlat_valueTypeReset(TYPE *object);

}  // close namespace bdlat_ValueTypeFunctions

                    // ===================================
                    // struct bdlat_ValueTypeFunctions_Imp
                    // ===================================

struct bdlat_ValueTypeFunctions_Imp {
    // This 'struct' contains functions used by the implementation of this
    // component.

    // TYPES
    struct IsConvertible    { };
    struct IsNotConvertible { };
    struct UseResetMethod   { };
    struct UseDefaultCtor   { };

    // CLASS METHODS
    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration  ,
                      const char&                      rhs,
                      bdlat_TypeCategory::Simple       );

    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration  ,
                      const short&                     rhs,
                      bdlat_TypeCategory::Simple       );

    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration  ,
                      const int&                       rhs,
                      bdlat_TypeCategory::Simple       );

    template <class LHS_TYPE>
    static int assign(LHS_TYPE                        *lhs,
                      bdlat_TypeCategory::Enumeration  ,
                      const bsl::string&               rhs,
                      bdlat_TypeCategory::Simple       );

    template <class RHS_TYPE>
    static int assign(char                            *lhs,
                      bdlat_TypeCategory::Simple       ,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration  );

    template <class RHS_TYPE>
    static int assign(short                           *lhs,
                      bdlat_TypeCategory::Simple       ,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration  );

    template <class RHS_TYPE>
    static int assign(int                             *lhs,
                      bdlat_TypeCategory::Simple       ,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration  );

    template <class RHS_TYPE>
    static int assign(bsl::string                     *lhs,
                      bdlat_TypeCategory::Simple       ,
                      const RHS_TYPE&                  rhs,
                      bdlat_TypeCategory::Enumeration  );

    template <class LHS_TYPE, class RHS_TYPE>
    static int assign(LHS_TYPE                   *lhs,
                      bdlat_TypeCategory::Simple  ,
                      const RHS_TYPE&             rhs,
                      bdlat_TypeCategory::Simple  );

    template <class LHS_TYPE,
              class LHS_CATEGORY,
              class RHS_TYPE,
              class RHS_CATEGORY>
    static int assign(LHS_TYPE        *lhs,
                      LHS_CATEGORY,
                      const RHS_TYPE&  rhs,
                      RHS_CATEGORY);

    template <class LHS_TYPE, class RHS_TYPE>
    static int assignSimpleTypes(LHS_TYPE        *lhs,
                                 const RHS_TYPE&  rhs,
                                 IsConvertible    );

    template <class LHS_TYPE, class RHS_TYPE>
    static int assignSimpleTypes(LHS_TYPE         *lhs,
                                 const RHS_TYPE&   rhs,
                                 IsNotConvertible  );

    template <class TYPE>
    static void reset(TYPE *object);

    template <class TYPE>
    static void reset(bdlb::NullableValue<TYPE> *object);

    template <class TYPE, class ALLOC>
    static void reset(bsl::vector<TYPE, ALLOC> *object);

    template <class CHAR_T, class CHAR_TRAITS, class ALLOC>
    static void reset(bsl::basic_string<CHAR_T, CHAR_TRAITS, ALLOC> *object);

    template <class TYPE>
    static void reset(TYPE *object, UseResetMethod);

    template <class TYPE>
    static void reset(TYPE *object, UseDefaultCtor);

};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ----------------------------------
                      // namespace bdlat_ValueTypeFunctions
                      // ----------------------------------

// MANIPULATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions::assign(LHS_TYPE *lhs, const RHS_TYPE& rhs)
{
    return bdlat_valueTypeAssign(lhs, rhs);
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions::reset(TYPE *object)
{
    return bdlat_valueTypeReset(object);
}

                            // -------------------
                            // default definitions
                            // -------------------

// MANIPULATORS
template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions::bdlat_valueTypeAssign(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs)
{
    typedef typename bdlat_TypeCategory::Select<LHS_TYPE>::Type LhsCategory;
    typedef typename bdlat_TypeCategory::Select<RHS_TYPE>::Type RhsCategory;

    return bdlat_ValueTypeFunctions_Imp::assign(lhs,
                                                LhsCategory(),
                                                rhs,
                                                RhsCategory());
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions::bdlat_valueTypeReset(TYPE *object)
{
    bdlat_ValueTypeFunctions_Imp::reset(object);
}

                    // -----------------------------------
                    // struct bdlat_ValueTypeFunctions_Imp
                    // -----------------------------------

// CLASS METHODS
template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration  ,
                                         const char&                      rhs,
                                         bdlat_TypeCategory::Simple       )
{
    return bdlat_EnumFunctions::fromInt(lhs, rhs);
}

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration  ,
                                         const short&                     rhs,
                                         bdlat_TypeCategory::Simple       )
{
    return bdlat_EnumFunctions::fromInt(lhs, rhs);
}

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration  ,
                                         const int&                       rhs,
                                         bdlat_TypeCategory::Simple       )
{
    return bdlat_EnumFunctions::fromInt(lhs, rhs);
}

template <class LHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                        *lhs,
                                         bdlat_TypeCategory::Enumeration  ,
                                         const bsl::string&               rhs,
                                         bdlat_TypeCategory::Simple       )
{
    return bdlat_EnumFunctions::fromString(lhs, rhs.data(), (int)rhs.length());
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(char                            *lhs,
                                         bdlat_TypeCategory::Simple       ,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration  )
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const int MIN_CHAR = -128;
    const int MAX_CHAR =  127;

    int proxy;

    bdlat_EnumFunctions::toInt(&proxy, rhs);

    if (proxy < MIN_CHAR || proxy > MAX_CHAR) {
        return k_FAILURE;                                             // RETURN
    }

    *lhs = static_cast<char>(proxy);

    return k_SUCCESS;
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(short                           *lhs,
                                         bdlat_TypeCategory::Simple       ,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration  )
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const int MIN_SHORT = -32768;
    const int MAX_SHORT =  32767;

    int proxy;

    bdlat_EnumFunctions::toInt(&proxy, rhs);

    if (proxy < MIN_SHORT || proxy > MAX_SHORT) {
        return k_FAILURE;                                             // RETURN
    }

    *lhs = static_cast<short>(proxy);

    return k_SUCCESS;
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(int                             *lhs,
                                         bdlat_TypeCategory::Simple       ,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration  )
{
    enum { k_SUCCESS = 0 };

    bdlat_EnumFunctions::toInt(lhs, rhs);

    return k_SUCCESS;
}

template <class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(bsl::string                     *lhs,
                                         bdlat_TypeCategory::Simple       ,
                                         const RHS_TYPE&                  rhs,
                                         bdlat_TypeCategory::Enumeration  )
{
    enum { k_SUCCESS = 0 };

    bdlat_EnumFunctions::toString(lhs, rhs);

    return k_SUCCESS;
}

template <class LHS_TYPE, class RHS_TYPE>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE                   *lhs,
                                         bdlat_TypeCategory::Simple  ,
                                         const RHS_TYPE&             rhs,
                                         bdlat_TypeCategory::Simple  )
{
    enum {
        IS_CONVERTIBLE = bslmf::IsConvertible<RHS_TYPE, LHS_TYPE>::value
    };

    typedef typename bsl::conditional<IS_CONVERTIBLE,
                                      IsConvertible,
                                      IsNotConvertible>::type Selector;

    return assignSimpleTypes(lhs, rhs, Selector());
}

template <class LHS_TYPE,
          class LHS_CATEGORY,
          class RHS_TYPE,
          class RHS_CATEGORY>
int bdlat_ValueTypeFunctions_Imp::assign(LHS_TYPE        *,
                                         LHS_CATEGORY     ,
                                         const RHS_TYPE&  ,
                                         RHS_CATEGORY     )
{
    enum { k_FAILURE = -1 };

    return k_FAILURE;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions_Imp::assignSimpleTypes(LHS_TYPE        *lhs,
                                                    const RHS_TYPE&  rhs,
                                                    IsConvertible    )
{
    enum { k_SUCCESS = 0 };

    *lhs = static_cast<LHS_TYPE>(rhs);

    return k_SUCCESS;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
int bdlat_ValueTypeFunctions_Imp::assignSimpleTypes(LHS_TYPE         *,
                                                    const RHS_TYPE&   ,
                                                    IsNotConvertible  )
{
    enum { k_FAILURE = -1 };

    return k_FAILURE;
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(TYPE *object)
{
    enum {
        HAS_TRAIT = bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::VALUE
                 || bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE
                 || bslalg::HasTrait<TYPE,
                                    bdlat_TypeTraitBasicCustomizedType>::VALUE
    };

    typedef typename bsl::conditional<
                  HAS_TRAIT,
                  bdlat_ValueTypeFunctions_Imp::UseResetMethod,
                  bdlat_ValueTypeFunctions_Imp::UseDefaultCtor>::type Selector;

    bdlat_ValueTypeFunctions_Imp::reset(object, Selector());
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(bdlb::NullableValue<TYPE> *object)
{
    object->reset();
}

template <class TYPE, class ALLOC>
inline
void bdlat_ValueTypeFunctions_Imp::reset(bsl::vector<TYPE, ALLOC> *object)
{
    object->clear();
}

template <class CHAR_T, class CHAR_TRAITS, class ALLOC>
inline
void bdlat_ValueTypeFunctions_Imp::reset(
                         bsl::basic_string<CHAR_T, CHAR_TRAITS, ALLOC> *object)
{
    object->erase();
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(TYPE *object, UseResetMethod)
{
    object->reset();
}

template <class TYPE>
inline
void bdlat_ValueTypeFunctions_Imp::reset(TYPE *object, UseDefaultCtor)
{
    *object = TYPE();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
