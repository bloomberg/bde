// bdetu_unset.h                                                      -*-C++-*-
#ifndef INCLUDED_BDETU_UNSET
#define INCLUDED_BDETU_UNSET

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Define arbitrary "unset" values for fundamental and 'bdet' types.
//
//@CLASSES:
//               bdetu_Unset: namespace for operations on "unset" values
// bdetu_UnsetValueIsDefined: meta-fnc. indicating if 'TYPE' has an unset value
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component defines thoughtful (but arbitrary) special
// values for fourteen specific fundamental and 'bdet' types, and establishes
// a convention for these special values to be used as "unset" values for
// the corresponding types.  The templated 'bdetu_Unset' 'struct' provides a
// utility function, 'unsetValue', which returns the unset value for the
// parameter type.  The 'unsetValue' function is specialized for each of the
// types supported by this component.  Two additional 'bdetu_Unset' functions
// are provided for convenience: 'isUnset' tests for the unset value and
// 'makeUnset' sets an object to the unset value.  Note that all functions
// within the 'bdetu_Unset' 'struct' are 'static' methods.
//
// Also provided in this component is the 'bdetu_UnsetValueIsDefined'
// meta-function, whose contained enumerated 'VALUE' evaluates (at
// compile time) to 'true' for any of the supported types, and 'false' if the
// type does not have a 'bdetu_Unset::unsetValue' specialization.
//
///Unset Values for the Supported Fundamental and 'bdet' Types
///-----------------------------------------------------------
// Each of the fourteen supported fundamental and 'bdet' types is assigned
// a specific value that can be used to indicated that it is "unset":
//..
//    Type             "Unset" Value
//    ---------------  -------------
//    bool             false
//
//    char             0
//
//    short            -32768
//                     The smallest signed value that can be
//                     represented in 2's complement in 16 bits.
//
//    int              -2147483648
//                     The smallest signed value that can be
//                     represented in 2's complement in 32 bits.
//
//    Int64            -9223372036854775808
//                     The smallest signed value that can be
//                     represented in 2's complement in 64 bits.
//
//    float             -7568.0 / (1 << 30) / (1 << 30)
//                     An arbitrary small negative number.
//
//    double           -27953.0 / (1 << 30) / (1 << 30)
//                     An arbitrary small negative number.
//
//    bsl::string      The empty (zero-length) string.
//
//    bdet_Datetime    The default-constructed value.
//
//    bdet_DatetimeTz  The default-constructed value.
//
//    bdet_Date        The default-constructed value.
//
//    bdet_DateTz      The default-constructed value.
//
//    bdet_Time        The default-constructed value.
//
//    bdet_TimeTz      The default-constructed value.
//..
///Usage
///-----
///Usage Example 1
///- - - - - - - -
// The following snippets of code illustrate how to use the unset value for the
// type 'double'.  First, initialize 'unsetValue' with the unset value for
// 'double' and initialize a non-unset reference 'value' to 0.0:
//..
//    double unsetValue = bdetu_Unset<double>::unsetValue();
//    double value      = 0.0;
//..
// Test if 'value' is unset; it should *not* be:
//..
//    assert(0 == bdetu_Unset<double>::isUnset(value));
//..
// Set 'value' to the unset value:
//..
//    bdetu_Unset<double>::makeUnset(&value);
//..
// Assert that 'value' is now unset:
//..
//    assert(bdetu_Unset<double>::isUnset(value));
//    assert(value == unsetValue);
//..
///Usage Example 2
///- - - - - - - -
// The 'bdetu_Unset' 'struct' is particularly useful in implementing
// heterogenous container classes that have to store a default value.
// Consider, for example, that we want to implement a heterogenous list class,
// 'my_List', that allows storing arbitrary sequences of three primitive types,
// 'int', 'double', and 'bsl::string'.  The following is the class definition
// for 'my_List':
//..
//  // my_list.h
//
//  class my_List {
//..
// Define the list of types that this heterogenous container supports:
//..
//    public:
//      // TYPES
//      enum ELEMENT_TYPE { INT, DOUBLE, STRING };
//..
// Define a universal node that can store a value of either of the three
// supported types:
//..
//    private:
//      // PRIVATE TYPES
//      union Node {
//          int                                d_int;
//          double                             d_double;
//          bsls_ObjectBuffer<bsl::string>     d_string;
//          bsls_AlignmentUtil::MaxAlignedType d_align;
//      };
//..
// Define the vectors to store the types and corresponding values.  The element
// at index 'i' in 'd_types' specifies the type of the i'th element in the
// list; the corresponding i'th element in 'd_values' provides the value
// for that list element:
//..
//      // DATA
//      bsl::vector<ELEMENT_TYPE>  d_types;        // list element types
//      bsl::vector<Node>          d_values;       // list element values
//      bslma_Allocator           *d_allocator_p;  // holds (but doesn't own)
//                                                 // allocator
//..
// A minimal public interface (suitable for illustration only):
//..
//    public:
//      // CREATORS
//      my_List(bslma_Allocator *basicAllocator = 0);
//          // Create a list of length 0.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      my_List(const my_List& original, bslma_Allocator *basicAllocator = 0);
//          // Create a list having the value of the specified 'original'
//          // list.  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently installed
//          // default allocator is used.
//
//      ~my_List();
//          // Destroy this list.
//
//      // MANIPULATORS
//      const my_List& operator=(const my_List& rhs);
//          // Assign to this list the value of the specified 'rhs' list, and
//          // return a reference to this modifiable list.  Note that after the
//          // assignment, both lists will have identical sequences of elements
//          // (types and values).
//..
// Provide functions that append elements by value:
//..
//      void appendInt(int value);
//          // Append to this list an element of type 'INT' having the
//          // specified 'value'.
//
//      void appendDouble(double value);
//          // Append to this list an element of type 'DOUBLE' having the
//          // specified 'value'.
//
//      void appendString(const bsl::string& value);
//          // Append to this list an element of type 'STRING' having the
//          // specified 'value'.
//
//..
// Also provide functions that append unset elements that users can populate
// later:
//..
//      void appendUnsetInt();
//          // Append to this list an unset element of type 'INT'.
//
//      void appendUnsetDouble();
//          // Append to this list an unset element of type 'DOUBLE'.
//
//      void appendUnsetString();
//          // Append to this list an unset element of type 'STRING'.
//..
// And provide functions to set the values of existing elements:
//..
//      void setInt(int index, int value);
//          // Set the element of type 'INT' at the specified 'index' in this
//          // list to the specified 'value'.  The behavior is undefined unless
//          // '0 <= index < length()' and 'INT == elemType(index)'.
//
//      void setDouble(int index, double value);
//          // Set the element of type 'DOUBLE' at the specified 'index' in
//          // this list to the specified 'value'.  The behavior is undefined
//          // unless '0 <= index < length()' and 'DOUBLE == elemType(index)'.
//
//      void setString(int index, const bsl::string& value);
//          // Set the element of type 'STRING' at the specified 'index' in
//          // this list to the specified 'value'.  The behavior is undefined
//          // unless '0 <= index < length()' and 'STRING == elemType(index)'.
//..
// Finally, provide a standard set of accessors to determine the length of the
// list, and the value and the type of an element at a specified 'index':
//..
//      // ACCESSORS
//      int length() const;
//          // Return the number of elements in this list.
//
//      int theInt(int index) const;
//          // Return the element of type 'INT' at the specified 'index' in
//          // this list.  The behavior is undefined unless
//          // '0 <= index < length()' and 'INT == elemType(index)'.
//
//      double theDouble(int index) const;
//          // Return the element of type 'DOUBLE' at the specified 'index' in
//          // this list.  The behavior is undefined unless
//          // '0 <= index < length()' and 'DOUBLE == elemType(index)'.
//
//      const bsl::string& theString(int index) const;
//          // Return a reference to the non-modifiable element of type
//          // 'STRING' at the specified 'index' in this list.  The behavior is
//          // undefined unless '0 <= index < length()' and
//          // 'STRING == elemType(index)'.
//
//      ELEMENT_TYPE elemType(int index) const;
//          // Return the type of the element at the specified 'index' in this
//          // list.  The behavior is undefined unless '0 <= index < length()'.
//
//      bool isUnset(int index) const;
//          // Return 'true' if the element at the specified 'index' in this
//          // list is unset, and 'false' otherwise.  The behavior is undefined
//          // unless '0 <= index < length()'.
//  };
//..
// Below are the function definitions.  Note that, in the interest of brevity,
// exception-safety concerns are not addressed:
//..
//  // ========================================================================
//  //                        INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//  // CREATORS
//  inline
//  my_List::my_List(bslma_Allocator *basicAllocator)
//  : d_types(basicAllocator)
//  , d_values(basicAllocator)
//  , d_allocator_p(basicAllocator)
//  {
//  }
//
//  inline
//  my_List::my_List(const my_List& original, bslma_Allocator *basicAllocator)
//  : d_types(original.d_types, basicAllocator)
//  , d_values(original.d_values, basicAllocator)
//  , d_allocator_p(basicAllocator)
//  {
//  }
//
//  inline
//  my_List::~my_List()
//  {
//      BSLS_ASSERT_SAFE(d_types.size() == d_values.size());
//  }
//
//  // MANIPULATORS
//  inline
//  const my_List& my_List::operator=(const my_List& rhs)
//  {
//      if (this != &rhs) {
//          d_types  = rhs.d_types;
//          d_values = rhs.d_values;
//      }
//      return *this;
//  }
//
//  inline
//  void my_List::appendInt(int value)
//  {
//      d_types.push_back(INT);
//      Node node;
//      node.d_int = value;
//      d_values.push_back(node);
//  }
//
//  inline
//  void my_List::appendDouble(double value)
//  {
//      d_types.push_back(DOUBLE);
//      Node node;
//      node.d_double = value;
//      d_values.push_back(node);
//  }
//
//  inline
//  void my_List::appendString(const bsl::string& value)
//  {
//      d_types.push_back(STRING);
//      Node node;
//      new (node.d_string.buffer()) bsl::string(value, d_allocator_p);
//      d_values.push_back(node);
//  }
//..
// Note the use of the 'bdetu_Unset<TYPE>::unsetValue' function to initialize
// the elements in the following:
//..
//  inline
//  void my_List::appendUnsetInt()
//  {
//      d_types.push_back(INT);
//      Node node;
//      node.d_int = bdetu_Unset<int>::unsetValue();
//      d_values.push_back(node);
//  }
//
//  inline
//  void my_List::appendUnsetDouble()
//  {
//      d_types.push_back(DOUBLE);
//      Node node;
//      node.d_double = bdetu_Unset<double>::unsetValue();
//      d_values.push_back(node);
//  }
//
//  inline
//  void my_List::appendUnsetString()
//  {
//      d_types.push_back(STRING);
//      Node node;
//      new (node.d_string.buffer())
//                         bsl::string(bdetu_Unset<bsl::string>::unsetValue(),
//                                     d_allocator_p);
//      d_values.push_back(node);
//  }
//
//  inline
//  void my_List::setInt(int index, int value)
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//      BSLS_ASSERT_SAFE(INT == d_types[index]);
//
//      d_values[index].d_int = value;
//  }
//
//  inline
//  void my_List::setDouble(int index, double value)
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//      BSLS_ASSERT_SAFE(DOUBLE == d_types[index]);
//
//      d_values[index].d_double = value;
//  }
//
//  inline
//  void my_List::setString(int index, const bsl::string& value)
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//      BSLS_ASSERT_SAFE(STRING == d_types[index]);
//
//      d_values[index].d_string.object() = value;
//  }
//
//  // ACCESSORS
//  inline
//  int my_List::length() const
//  {
//      BSLS_ASSERT_SAFE(d_types.size() == d_values.size());
//
//      return d_types.size();
//  }
//
//  inline
//  int my_List::theInt(int index) const
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//      BSLS_ASSERT_SAFE(INT == d_types[index]);
//
//      return d_values[index].d_int;
//  }
//
//  inline
//  double my_List::theDouble(int index) const
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//      BSLS_ASSERT_SAFE(DOUBLE == d_types[index]);
//
//      return d_values[index].d_double;
//  }
//
//  inline
//  const bsl::string& my_List::theString(int index) const
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//      BSLS_ASSERT_SAFE(STRING == d_types[index]);
//
//      return d_values[index].d_string.object();
//  }
//
//  inline
//  my_List::ELEMENT_TYPE my_List::elemType(int index) const
//  {
//      BSLS_ASSERT_SAFE(0 <= index && index < length());
//
//      return d_types[index];
//  }
//..
// The 'isUnset' method is defined in the corresponding '.cpp' file:
//..
//  // my_list.cpp
//
//  bool my_List::isUnset(int index) const
//  {
//      BSLS_ASSERT(0 <= index && index < length());
//
//      bool isElementUnset = false;
//
//      switch (d_types[index]) {
//        case INT: {
//          isElementUnset = bdetu_Unset<int>::isUnset(d_values[index].d_int);
//        } break;
//        case DOUBLE: {
//          isElementUnset = bdetu_Unset<double>::isUnset(
//                                                   d_values[index].d_double);
//        } break;
//        case STRING: {
//          isElementUnset = bdetu_Unset<bsl::string>::isUnset(
//                                          d_values[index].d_string.object());
//        } break;
//      }
//
//      return isElementUnset;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ========================
                        // struct bdetu_Unset<TYPE>
                        // ========================

template <class TYPE>
struct bdetu_Unset {
    // This 'struct' provides a namespace for 'static' functions that retrieve
    // and assign the unset value for the fourteen fundamental and 'bdet'
    // scalars.  Note that there is never any need for a user of this
    // component to create an object of this type.

    // CLASS METHODS
    static TYPE unsetValue();
        // Return the unset value for the parameterized 'TYPE' if 'TYPE' is one
        // of the fourteen supported types, and generate a compile-time error
        // otherwise.

    static bool isUnset(const TYPE& value);
        // Return 'true' if the specified 'value' is the unset value for the
        // parameterized 'TYPE', and 'false' otherwise.  Specializations are
        // defined for this function for non-fundamental types.  Note that the
        // behavior of this function is the same as:
        //..
        //  bdetu_Unset<TYPE>::unsetValue() == value
        //..

    static void makeUnset(TYPE *object);
        // Set the specified 'object' to have the unset value for the
        // parameterized 'TYPE'.  Note that the behavior of this function is
        // the same as:
        //..
        //  *object = bdetu_Unset<TYPE>::unsetValue();
        //..

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static void setUnset(TYPE *object);
        // Set the specified 'object' to have the unset value for the
        // parameterized 'TYPE'.  Note that the behavior of this function is
        // the same as:
        //..
        //  *value = bdetu_Unset<TYPE>::unsetValue();
        //..
        // DEPRECATED: use 'makeUnset' instead.

#endif

};

                        // ======================================
                        // struct bdetu_UnsetValueIsDefined<TYPE>
                        // ======================================

template <typename TYPE>
struct bdetu_UnsetValueIsDefined {
    // This meta-function provides (at compile time) a nested enumerated
    // 'VALUE' that is 'true' if 'TYPE' has an unset value defined in this
    // component, and 'false' otherwise.

    enum {
        VALUE = bslmf_IsSame<TYPE, bool>::VALUE
             || bslmf_IsSame<TYPE, char>::VALUE
             || bslmf_IsSame<TYPE, short>::VALUE
             || bslmf_IsSame<TYPE, int>::VALUE
             || bslmf_IsSame<TYPE, bsls_Types::Int64>::VALUE
             || bslmf_IsSame<TYPE, float>::VALUE
             || bslmf_IsSame<TYPE, double>::VALUE
             || bslmf_IsSame<TYPE, bsl::string>::VALUE
             || bslmf_IsSame<TYPE, bdet_Datetime>::VALUE
             || bslmf_IsSame<TYPE, bdet_DatetimeTz>::VALUE
             || bslmf_IsSame<TYPE, bdet_Date>::VALUE
             || bslmf_IsSame<TYPE, bdet_DateTz>::VALUE
             || bslmf_IsSame<TYPE, bdet_Time>::VALUE
             || bslmf_IsSame<TYPE, bdet_TimeTz>::VALUE
    };
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------------
                        // struct bdetu_Unset<TYPE>
                        // ------------------------

// CLASS METHODS
template <>
inline
bool bdetu_Unset<bool>::unsetValue()
{
    return false;
}

template <>
inline
char bdetu_Unset<char>::unsetValue()
{
    return 0;
}

template <>
inline
short bdetu_Unset<short>::unsetValue()
{
    const short unsetShortValue = -0x7fff - 1;  // 16 bits

    return unsetShortValue;
}

template <>
inline
int bdetu_Unset<int>::unsetValue()
{
    const int unsetIntValue = -0x7fffffff - 1;  // 32 bits

    return unsetIntValue;
}

template <>
inline
bsls_Types::Int64 bdetu_Unset<bsls_Types::Int64>::unsetValue()
{
    const bsls_Types::Int64 unsetInt64Value =
                                         -0x7fffffffffffffffLL - 1;  // 64 bits

    return unsetInt64Value;
}

template <>
inline
float bdetu_Unset<float>::unsetValue()
{
    const float unsetFloatValue = -7568.0 / (1 << 30) / (1 << 30);

    return unsetFloatValue;
}

template <>
inline
double bdetu_Unset<double>::unsetValue()
{
    const double unsetDoubleValue = -27953.0 / (1 << 30) / (1 << 30);

    return unsetDoubleValue;
}

template <>
inline
bsl::string bdetu_Unset<bsl::string>::unsetValue()
{
    return bsl::string();
}

template <>
inline
bdet_Datetime bdetu_Unset<bdet_Datetime>::unsetValue()
{
    return bdet_Datetime();
}

template <>
inline
bdet_DatetimeTz bdetu_Unset<bdet_DatetimeTz>::unsetValue()
{
    return bdet_DatetimeTz();
}

template <>
inline
bdet_Date bdetu_Unset<bdet_Date>::unsetValue()
{
    return bdet_Date();
}

template <>
inline
bdet_DateTz bdetu_Unset<bdet_DateTz>::unsetValue()
{
    return bdet_DateTz();
}

template <>
inline
bdet_Time bdetu_Unset<bdet_Time>::unsetValue()
{
    return bdet_Time();
}

template <>
inline
bdet_TimeTz bdetu_Unset<bdet_TimeTz>::unsetValue()
{
    return bdet_TimeTz();
}

template <class TYPE>
inline
bool bdetu_Unset<TYPE>::isUnset(const TYPE& value)
{
    return value == unsetValue();
}

template <>
inline
bool bdetu_Unset<bool>::isUnset(const bool& value)
{
    return false == value;
}

template <>
inline
bool bdetu_Unset<char>::isUnset(const char& value)
{
    return 0 == value;
}

template <>
inline
bool bdetu_Unset<short>::isUnset(const short& value)
{
    const short unsetShortValue = -0x7fff - 1;  // 16 bits

    return unsetShortValue == value;
}

template <>
inline
bool bdetu_Unset<int>::isUnset(const int& value)
{
    const int unsetIntValue = -0x7fffffff - 1;  // 32 bits

    return unsetIntValue == value;
}

template <>
inline
bool bdetu_Unset<bsls_Types::Int64>::isUnset(const bsls_Types::Int64& value)
{
    const bsls_Types::Int64 unsetInt64Value =
                                         -0x7fffffffffffffffLL - 1;  // 64 bits

    return unsetInt64Value == value;
}

template <>
inline
bool bdetu_Unset<float>::isUnset(const float& value)
{
    const float unsetFloatValue = -7568.0 / (1 << 30) / (1 << 30);

    return unsetFloatValue == value;
}

template <>
inline
bool bdetu_Unset<double>::isUnset(const double& value)
{
    const double unsetDoubleValue = -27953.0 / (1 << 30) / (1 << 30);

    return unsetDoubleValue == value;
}

template <>
inline
bool bdetu_Unset<bsl::string>::isUnset(const bsl::string& value)
{
    return 0 == value.length();
}

template <class TYPE>
inline
void bdetu_Unset<TYPE>::makeUnset(TYPE *object)
{
    BSLS_ASSERT_SAFE(object);

    *object = unsetValue();
}

template <>
inline
void bdetu_Unset<bsl::string>::makeUnset(bsl::string *object)
    // Set the specified string 'object' to have the unset value for strings.
    // Note that this specialization provides a more optimal implementation of
    // 'makeUnset' for 'bsl::string'.
{
    BSLS_ASSERT_SAFE(object);

    object->erase();
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

template <class TYPE>
inline
void bdetu_Unset<TYPE>::setUnset(TYPE *value)
{
    makeUnset(value);
}

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
