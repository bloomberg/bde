// bdlat_nullablevaluefunctions.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlat_nullablevaluefunctions.h>

#include <bdlb_nullablevalue.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>   // for testing only
#include <bslmf_issame.h>

#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                        // ===========================
                        // class GetValue<LVALUE_TYPE>
                        // ===========================

template <class LVALUE_TYPE>
class GetValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE *d_lValue_p;  // held, not owned

  public:
    // CREATORS
    explicit GetValue(LVALUE_TYPE *lValue);

    // ACCESSORS
    int operator()(const LVALUE_TYPE& object) const;
        // Assign the specified 'object' to '*d_destination_p'.

    template <class RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Do nothing with the specified 'object'.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <class RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    RVALUE_TYPE d_value;

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    int operator()(RVALUE_TYPE *object) const;
        // Assign 'd_value' to the specified '*object'.

    template <class LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Do nothing with the specified 'object'.
};

                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <class LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <class LVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object) const
{
    *d_lValue_p = object;
    return 0;
}

template <class LVALUE_TYPE>
template <class RVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <class RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <class RVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object) const
{
    *object = d_value;
    return 0;
}

template <class RVALUE_TYPE>
template <class LVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *object) const
{
    return -1;
}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace Obj = bdlat_NullableValueFunctions;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

    // Definitions needed for Usage but not shown in the header file

    namespace BloombergLP {
    namespace your {

    class YourNullableValue {

        // DATA
        bool        d_isNull;
        bsl::string d_value;

      public:
        // CREATORS
        YourNullableValue()
        : d_isNull(true)
        , d_value()
        {
        }

        // MANIPULATORS
        void makeValue()
        {
            d_isNull = false;
            d_value.clear();
        }

        void makeNull()
        {
            d_isNull = true;
            d_value.clear();
        }

        bsl::string& value()
        {
            ASSERT(!d_isNull);

            return d_value;
        }

        // ACCESSORS
        const bsl::string& value() const
        {
            ASSERT(!d_isNull);

            return d_value;
        }
        bool isNull() const
        {
            return d_isNull;
        }
    };

    // MANIPULATORS
    void bdlat_nullableValueMakeValue(YourNullableValue *object)
    {
        object->makeValue();
    }

    template <class MANIPULATOR>
    int bdlat_nullableValueManipulateValue(YourNullableValue *object,
                                           MANIPULATOR&     manipulator)
    {
        ASSERT(!object->isNull());

        return manipulator(&object->value());
    }

    // ACCESSORS
    template <class ACCESSOR>
    int bdlat_nullableValueAccessValue(const YourNullableValue& object,
                                       ACCESSOR&              accessor)
    {
        ASSERT(!object.isNull());

        return accessor(object.value());
    }

    bool bdlat_nullableValueIsNull(const YourNullableValue& object)
    {
        return object.isNull();
    }

    }  // close namespace your

    namespace bdlat_NullableValueFunctions {

    // TRAITS
    template <>
    struct IsNullableValue<your::YourNullableValue> : public bsl::true_type {
    };

    template <>
    struct ValueType<your::YourNullableValue> {
        typedef bsl::string Type;
    };

    }  // close namespace bdlat_NullableValueFunctions
    }  // close enterprise namespace

///Usage
//------
// The following code illustrate the usage of this component.
//
///Example 1: Defining a "Nullable" Type
// - - - - - - - - - - - - - - - - - - -
// Suppose you had a type whose value could be in a "null" state.
//
//..
    namespace BloombergLP {
    namespace mine {

    struct MyNullableValue {

        // DATA
        bool d_isNull;
        int  d_value;

        // CREATORS
        MyNullableValue()
        {
            d_isNull = true;
        }
    };

    }  // close namespace mine
    }  // close enterprise namespace
//..
// We can now make 'mine::MyNullableValue' expose "nullable" behavior by
// implementing the necessary 'bdlta_NullableValueFunctions' for
// 'MyNullableValue' inside the 'mine' namespace and defining the required
// meta-functions withing the 'bdlat_NullableValueFunctions' namespace.
//
// First, we should forward declare all the functions that we will implement
// inside the 'mine' namespace:
//..
    namespace BloombergLP {
    namespace mine {

    // MANIPULATORS
    void bdlat_nullableValueMakeValue(MyNullableValue *object);
        // Assign to the specified "nullable" 'object' the default value for
        // the contained type (i.e., 'ValueType()').

    template <class MANIPULATOR>
    int bdlat_nullableValueManipulateValue(MyNullableValue *object,
                                           MANIPULATOR&     manipulator);
        // Invoke the specified 'manipulator' on the address of the value
        // stored in the specified "nullable" 'object'.  Return the value from
        // the invocation of 'manipulator'.  The behavior is undefined if
        // 'object' contains a null value.

    // ACCESSORS
    template <class ACCESSOR>
    int bdlat_nullableValueAccessValue(const MyNullableValue& object,
                                       ACCESSOR&              accessor);
        // Invoke the specified 'accessor' on a 'const'-reference to the value
        // stored in the specified "nullable" 'object'.  Return the value from
        // the invocation of 'accessor'.  The behavior is undefined if 'object'
        // contains a null value.

    bool bdlat_nullableValueIsNull(const MyNullableValue& object);
        // Return 'true' if the specified "nullable" 'object' contains a null
        // value, and 'false' otherwise.

    }  // close namespace mine
    }  // close enterprise namespace
//..
// Then, we will implement these functions.  Recall that the two (non-template)
// functions should be defined in some '.cpp' file, unless you choose to make
// them 'inline' functions.
//..
    namespace BloombergLP {

    // MANIPULATORS
    void mine::bdlat_nullableValueMakeValue(MyNullableValue *object)
    {
        ASSERT(object);

        object->d_isNull = false;
        object->d_value  = 0;
    }

    template <class MANIPULATOR>
    int mine::bdlat_nullableValueManipulateValue(MyNullableValue *object,
                                                 MANIPULATOR&     manipulator)
    {
        ASSERT(object);
        ASSERT(!object->d_isNull);

        return manipulator(&object->d_value);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int mine::bdlat_nullableValueAccessValue(const MyNullableValue& object,
                                             ACCESSOR&              accessor)
    {
        ASSERT(!object.d_isNull);

        return accessor(object.d_value);
    }

    bool mine::bdlat_nullableValueIsNull(const MyNullableValue& object)
    {
        return object.d_isNull;
    }

    }  // close enterprise namespace
//..
// Finally, we specialize the 'IsNullableValue' and 'ValueType' meta-functions
// in the 'bdlat_NullableValueFunctions' namespace for the
// 'mine::MyNullableValue' type:
//..
    namespace BloombergLP {
    namespace bdlat_NullableValueFunctions {

    // TRAITS
    template <>
    struct IsNullableValue<mine::MyNullableValue> : public bsl::true_type {
    };

    template <>
    struct ValueType<mine::MyNullableValue> {
        typedef int Type;
    };

    }  // close namespace bdlat_NullableValueFunctions
    }  // close enterprise namespace
//..
// This completes the 'bdlat' infrastructure for 'mine::MyNullableValue' and
// allows the generic software to recognize the type as a nullable abstraction.
//
///Example 2: Using the Infrastructure Via General Methods
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlat' "nullable" framework provides a set of fundamental operations
// common to any "nullable" type.  We can build upon these operations to make
// our own utilities, or use them on our own types that are plugged into the
// framework, like 'mine::MyNullableValue', which we created in {Example 1}.
// For example, we can use the (fundamental) operations in the
// 'bdlat_NullableValueFunctions' namespace to operate on
// 'mine::NullableValue', even though they have no knowledge of that type in
// particular:
//
// Two of those operations are rather basic.  One simply informs whether or not
// an object is in the null state (the 'isNull' method).  Another sets an
// object to a default, non-null state (the 'makeValue' method).
//..
    void usageMakeObject()
    {
        BSLMF_ASSERT(bdlat_NullableValueFunctions::
                     IsNullableValue<mine::MyNullableValue>::value);

        mine::MyNullableValue object;
        ASSERT( bdlat_NullableValueFunctions::isNull(object));

        bdlat_NullableValueFunctions::makeValue(&object);
        ASSERT(!bdlat_NullableValueFunctions::isNull(object));
    }
//..
// The other two generic methods accomplish their actions via user-supplied
// functors.
//
// Let us define a generic functor that gives us access to the underlying value
// of the "nullable" type, if it's not null:
//..
    template <class VALUE_TYPE>
    class GetValueAccessor {

        // DATA
        VALUE_TYPE *d_value_p;

      public:
        // CREATORS
        explicit GetValueAccessor(VALUE_TYPE *value)
        : d_value_p(value)
        {
        }

        // MANIPULATORS
        int operator()(const VALUE_TYPE& containedValue)
            // Assign the value of the specified 'containedValue' to the object
            // addressed by 'd_value_p'.
        {
            *d_value_p = containedValue;
            return 0;
        }
    };
//..
// Notice that the above class makes no assumptions about the value being
// accessed other than it can be copied (in the constructor) and assigned (in
// the operator).
//
// This functor can be used to fetch the value of our nullable object:
//..
    void usageGetValue()
    {
        mine::MyNullableValue object;

        bdlat_NullableValueFunctions::makeValue(&object);
        ASSERT(!bdlat_NullableValueFunctions::isNull(object));

        int value;
        GetValueAccessor<int> accessor(&value);

        int rc = bdlat_NullableValueFunctions::accessValue(object, accessor);
        ASSERT(0 == rc);
        ASSERT(0 == value);
    }
//..
// Notice that we did not invoke 'accessValue' until 'object' had been set to a
// non-null state.  Doing otherwise would have led to undefined behavior.
//
// Finally, let's define a functor to set the state of a nullable object:
//..
    template <class VALUE_TYPE>
    class SetValueManipulator {

        // DATA
        VALUE_TYPE d_value;

      public:
        // CREATORS
        explicit SetValueManipulator(const VALUE_TYPE& value)
        : d_value(value)
        {
        }

        // ACCESSOR
        int operator()(VALUE_TYPE *value) const
        {
            *value = d_value;
            return 0;
        }
    };
//..
// As with the previous functor, this functor has no knowledge of the nullable
// type to which it will be applied.  The only assumption here is that the
// value (type) of our nullable type can be copy constructed and copy assigned.
//
// Let us use this functor to modify one of our nullable objects:
//..
    void usageSetValue()
    {
        mine::MyNullableValue object;

        bdlat_NullableValueFunctions::makeValue(&object);
        ASSERT(!bdlat_NullableValueFunctions::isNull(object));

        SetValueManipulator<int> manipulator(42);
        int rcm = bdlat_NullableValueFunctions::manipulateValue(&object,
                                                                manipulator);
        ASSERT(0 == rcm);

        // Confirm that the object was set to the expected state.

        int value;
        GetValueAccessor<int> accessor(&value);

        int rca = bdlat_NullableValueFunctions::accessValue(object, accessor);
        ASSERT( 0 == rca);
        ASSERT(42 == value);
    }
//..
//
///Example 3: Defining Utility Functions
///- - - - - - - - - - - - - - - - - - -
// Creating functor objects for each operation can be tedious and error prone;
// consequently, those types are often executed via utility functions.
//
// Suppose we want to create utilities for getting and setting the value
// associated with an arbitrary "nullable" type.
//
// These functors make minimal assumptions of 'VALUE_TYPE', merely that it is
// copy constructable and copy assignable.
//
//..
    struct NullableValueUtil {

        // CLASS METHODS
        template <class NULLABLE_VALUE_TYPE>
        static int getValue(
            typename bdlat_NullableValueFunctions
                                ::ValueType<NULLABLE_VALUE_TYPE>::Type *value,
            const NULLABLE_VALUE_TYPE&                                  object)
            // Load to the specified 'value' the value of the specified
            // nullable value 'object'.  This function template requires that
            // the specified 'NULLABLE_VALUE_TYPE' is a 'bdlat' "nullable"
            // type.  The behavior is undefined unless 'object' is in a
            // non-null state (i.e.,
            // 'false == bdlat_NullableValueFunctions::isNull(object))'.
        {
            BSLMF_ASSERT(bdlat_NullableValueFunctions
                                ::IsNullableValue<NULLABLE_VALUE_TYPE>::value);

            BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(object));

            typedef typename bdlat_NullableValueFunctions
                              ::ValueType<NULLABLE_VALUE_TYPE>::Type ValueType;

            GetValueAccessor<ValueType> valueAccessor(value);
            return bdlat_NullableValueFunctions::accessValue(object,
                                                             valueAccessor);
        }

        template <class NULLABLE_VALUE_TYPE>
        static int setValue(NULLABLE_VALUE_TYPE                        *object,
                            const typename bdlat_NullableValueFunctions
                               ::ValueType<NULLABLE_VALUE_TYPE>::Type&  value)

            // Set the value of the specified 'object' to the specified
            // 'value'.  This function template requires that the specified
            // 'NULLABLE_VALUE_TYPE' is a 'bdlat' "nullable" type.  The
            // behavior is undefined unless 'object' is in a non-null state
            // (i.e., 'false == bdlat_NullableValueFunctions::isNull(object))'.
            // Note that a "nullable" object can be put into a non-null state
            // by the 'bdlat_NullableValueFunctions::makeValue' function
            // overload for the 'NULLABLE_VALUE_TYPE'.
        {
            BSLMF_ASSERT(bdlat_NullableValueFunctions
                                ::IsNullableValue<NULLABLE_VALUE_TYPE>::value);

            BSLS_ASSERT(object);
            BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(*object));

            typedef typename bdlat_NullableValueFunctions
                              ::ValueType<NULLABLE_VALUE_TYPE>::Type ValueType;

            SetValueManipulator<ValueType> manipulator(value);
            return bdlat_NullableValueFunctions::manipulateValue(object,
                                                                 manipulator);
        }
    };
//..
// Now, we can use these functors to write generic utility functions for
// getting and setting the value types of arbitrary "nullable" classes.
//..
    void myUsageScenario()
    {
        mine::MyNullableValue object;
        ASSERT(bdlat_NullableValueFunctions::isNull(object));

        bdlat_NullableValueFunctions::makeValue(&object);
        ASSERT(!bdlat_NullableValueFunctions::isNull(object));

        typedef
        bdlat_NullableValueFunctions::ValueType<mine::MyNullableValue>::Type
                                                                 MyValueType;

        int rcs = NullableValueUtil::setValue(&object, MyValueType(42));
        ASSERT(0 == rcs);

        MyValueType value;
        int rcg = NullableValueUtil::getValue(&value, object);
        ASSERT( 0 == rcg);
        ASSERT(42 == value);
    }
//..
//
///Example 4: Achieving Type Independence
/// - - - - - - - - - - - - - - - - - - -
// Finally, suppose we have another type such as 'your::YourNullableType',
// shown below:
//..
//  namespace BloombergLP {
//  namespace your {
//
//  class YourNullableValue {
//
//      // DATA
//      bool        d_isNull;
//      bsl::string d_value;
//
//    public:
//      // CREATORS
//      YourNullableValue()
//      : d_isNull(true)
//      , d_value()
//      {
//      }
//
//      // MANIPULATORS
//      void makeValue()
//      {
//          d_isNull = false;
//          d_value.clear();
//      }
//
//      void makeNull()
//      {
//          d_isNull = true;
//          d_value.clear();
//      }
//
//      bsl::string& value()
//      {
//          ASSERT(!d_isNull);
//
//          return d_value;
//      }
//
//      // ACCESSORS
//      const bsl::string& value() const
//      {
//          ASSERT(!d_isNull);
//
//          return d_value;
//      }
//      bool isNull() const
//      {
//          return d_isNull;
//      }
//  };
//
//  }  // close namespace your
//  }  // close enterprise namespace
//..
// Notice that while there are many similarities to 'mine::MyNullableValue'
// there are clearly differences:
//: o The value type is 'bsl::string', not 'int'.
//: o Attributes are accessed via accessor methods, not public data members.
//
// Nevertheless, since 'your::YourNullableValue' also provides the functions
// and types expected by the 'bdlat' infrastructure (not shown) we can
// successfully use 'your::YourNullableValue' value instead of
// 'mine::MyNullableValue' in the previous usage scenario, with no other
// changes:
//..
    void yourUsageScenario()
    {
        your::YourNullableValue object;  // YOUR NULLABLE TYPE
        ASSERT(bdlat_NullableValueFunctions::isNull(object));

        bdlat_NullableValueFunctions::makeValue(&object);
        ASSERT(!bdlat_NullableValueFunctions::isNull(object));

        typedef
        bdlat_NullableValueFunctions::ValueType<your::YourNullableValue>::Type
                                                                 YourValueType;

        int rcs = NullableValueUtil::setValue(&object, YourValueType("NB"));
        ASSERT(0 == rcs);

        YourValueType value;
        int rcg = NullableValueUtil::getValue(&value, object);
        ASSERT(  0  == rcg);
        ASSERT("NB" == value);
    }
//..
// Notice that syntax and order of 'bdlat_NullableValueFunction' functions
// calls have not been changed.  The only difference is that the contained
// type has changed from 'int' to 'bsl::string'.
//
// Finally, instead of defining a new "nullable" type, we could substitute the
// existing type template 'bdlb::NullableValue'.  Note that this component
// provides specializations of the 'bdlat_nullableValueFunctions' for that
// type.  Since the accessor and manipulator functions we created earlier are
// type neutral, we can simply drop 'bdlb::NullableValue<float>' into our
// familiar scenario:
//..
    void anotherUsageScenario()
    {
        bdlb::NullableValue<float> object;  // BDE NULLABLE TYPE
        ASSERT(bdlat_NullableValueFunctions::isNull(object));

        bdlat_NullableValueFunctions::makeValue(&object);
        ASSERT(!bdlat_NullableValueFunctions::isNull(object));

        typedef
        bdlat_NullableValueFunctions::ValueType<bdlb::NullableValue<float> >
                                                    ::Type AnotherValueType;

        int rcs = NullableValueUtil::setValue(&object, AnotherValueType(2.0));
        ASSERT(0 == rcs);

        AnotherValueType value;
        int rcg = NullableValueUtil::getValue(&value, object);
        ASSERT(0   == rcg);
        ASSERT(2.0 == value);
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
        case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   That the usage example compiles and runs as expected.
        //
        // Plan:
        //   Copy-paste the example, change the assert into ASSERT,
        //   and pass a string stream instead of 'bsl::cout' in order to check
        //   the print format.
        //
        // Testing:
        //   Usage Example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;
        usageMakeObject();
        usageGetValue();
        usageSetValue();

             myUsageScenario();
           yourUsageScenario();
        anotherUsageScenario();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsNullableValue
        //   struct ValueType
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == Obj::IsNullableValue<int>::value);
        ASSERT(1 == Obj::IsNullableValue<bdlb::NullableValue<int> >::value);

        typedef Obj::ValueType<bdlb::NullableValue<int> >::Type ValueType;
        ASSERT(1 == (bslmf::IsSame<ValueType, int>::value));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHOD FORWARDING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD FORWARDING TEST" << endl
                          << "======================" << endl;

        bdlb::NullableValue<int> mV;  const bdlb::NullableValue<int>& V = mV;

        Obj::makeValue(&mV); ASSERT(!Obj::isNull(V));

        int              value;
        GetValue<int>    getter(&value);
        AssignValue<int> setter1(33);
        AssignValue<int> setter2(44);

        Obj::manipulateValue(&mV, setter1);
        Obj::accessValue(V, getter); ASSERT(33 == value);

        Obj::manipulateValue(&mV, setter2);
        Obj::accessValue(V, getter); ASSERT(44 == value);

        mV.reset();
        ASSERT(Obj::isNull(V));
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
