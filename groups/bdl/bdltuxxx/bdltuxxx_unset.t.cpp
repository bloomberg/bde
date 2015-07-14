// bdltuxxx_unset.t.cpp          -*-C++-*-

#include <bdltuxxx_unset.h>

#include <bsl_cstdlib.h>          // atoi()
#include <bsl_cstring.h>          // strcmp(), memcmp(), memcpy()
#include <bsl_iostream.h>

#include <bsls_alignmentutil.h>
#include <bsls_objectbuffer.h>
#include <bsl_vector.h>

#include <bdet_datetimeinterval.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component implements utility functions for the unset value for a set
// of 10 scalar types.  A test case is provided for each of these types in
// which the 3 utility functions are tested for that type.
//-----------------------------------------------------------------------------
// [ 1] isUnset<char>(const char&)
// [ 1] unsetValue<char>()
// [ 1] makeUnset<char>(char*)
// [ 2] isUnset<short>(const short&)
// [ 2] unsetValue<short>()
// [ 2] makeUnset<short>(short*)
// [ 3] isUnset<int>(const int&)
// [ 3] unsetValue<int>()
// [ 3] makeUnset<int>(int*)
// [ 4] isUnset<bsls::Types::Int64>(const bsls::Types::Int64&)
// [ 4] unsetValue<bsls::Types::Int64>()
// [ 4] makeUnset<bsls::Types::Int64>(bsls::Types::Int64*)
// [ 5] isUnset<float>(const float&)
// [ 5] unsetValue<float>()
// [ 5] makeUnset<float>(float*)
// [ 6] isUnset<double>(const double&)
// [ 6] unsetValue<double>()
// [ 6] makeUnset<double>(double*)
// [ 7] isUnset<bsl::string>(const bsl::string&)
// [ 7] unsetValue<bsl::string>()
// [ 7] makeUnset<bsl::string>(bsl::string*)
// [ 8] isUnset<bdlt::Date>(const bdlt::Date&)
// [ 8] unsetValue<bdlt::Date>()
// [ 8] makeUnset<bdlt::Date>(bdlt::Date*)
// [ 9] isUnset<bdlt::Time>(const bdlt::Time&)
// [ 9] unsetValue<bdlt::Time>()
// [ 9] makeUnset<bdlt::Time>(bdlt::Time*)
// [10] isUnset<bdlt::Datetime>(const bdlt::Datetime&)
// [10] unsetValue<bdlt::Datetime>()
// [10] makeUnset<bdlt::Datetime>(bdlt::Datetime*)
// [11] isUnset<bool>(const bool&)
// [11] unsetValue<bool>()
// [11] makeUnset<bool>(bool*)
// [12] isUnset<bdlt::DateTz>(const bdlt::DateTz&)
// [12] unsetValue<bdlt::DateTz>()
// [12] makeUnset<bdlt::DateTz>(bdlt::DateTz*)
// [13] isUnset<bdlt::TimeTz>(const bdlt::TimeTz&)
// [13] unsetValue<bdlt::TimeTz>()
// [13] makeUnset<bdlt::TimeTz>(bdlt::TimeTz*)
// [14] isUnset<bdlt::DatetimeTz>(const bdlt::DatetimeTz&)
// [14] unsetValue<bdlt::DatetimeTz>()
// [14] makeUnset<bdlt::DatetimeTz>(bdlt::DatetimeTz*)
// [15] bdltuxxx::UnsetValueIsDefined<TYPE>::VALUE
//-----------------------------------------------------------------------------
// [16] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int foo(double x)
{
    return bdltuxxx::Unset<double>::isUnset(x);
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Usage Example 2
// - - - - - - - -
// The 'bdltuxxx::Unset' 'struct' is particularly useful in implementing
// heterogenous container classes that have to store a default value.
// Consider, for example, that we want to implement a heterogenous list class,
// 'my_List', that allows storing arbitrary sequences of three primitive types,
// 'int', 'double', and 'bsl::string'.  The following is the class definition
// for 'my_List':
//..
    // my_list.h

    class my_List {
//..
// Define the list of types that this heterogenous container supports:
//..
      public:
        // TYPES
        enum ELEMENT_TYPE { INT, DOUBLE, STRING };
//..
// Define a universal node that can store a value of either of the three
// supported types:
//..
      private:
        // PRIVATE TYPES
        union Node {
            int                                 d_int;
            double                              d_double;
            bsls::ObjectBuffer<bsl::string>     d_string;
            bsls::AlignmentUtil::MaxAlignedType d_align;
       };
//..
// Define the vectors to store the types and corresponding values.  The element
// at index 'i' in 'd_types' specifies the type of the i'th element of the
// list; and in 'd_values' the value of the i'th element of the list:
//..
        // DATA
        bsl::vector<ELEMENT_TYPE>  d_types;        // list element types
        bsl::vector<Node>          d_values;       // list element values
        bslma::Allocator          *d_allocator_p;  // holds (but doesn't own)
                                                   // allocator
//..
// A minimal public interface (suitable for illustration only):
//..
      public:
        // CREATORS
        my_List(bslma::Allocator *basicAllocator = 0);
            // Create a list of length 0.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        my_List(const my_List& original, bslma::Allocator *basicAllocator = 0);
            // Create a list having the value of the specified 'original'
            // list.  Optionally specify a 'basicAllocator' used to supply
            // memory.  If 'basicAllocator' is 0, the currently installed
            // default allocator is used.

        ~my_List();
            // Destroy this list.

        // MANIPULATORS
        const my_List& operator=(const my_List& rhs);
            // Assign to this list the value of the specified 'rhs' list, and
            // return a reference to this modifiable list.  Note that after the
            // assignment, both lists will have identical sequences of elements
            // (types and values).
//..
// Provide functions that append elements by value:
//..
        void appendInt(int value);
            // Append to this list an element of type 'INT' having the
            // specified 'value'.

        void appendDouble(double value);
            // Append to this list an element of type 'DOUBLE' having the
            // specified 'value'.

        void appendString(const bsl::string& value);
            // Append to this list an element of type 'STRING' having the
            // specified 'value'.

//..
// Also provide functions that append unset elements that users can populate
// later:
//..
        void appendUnsetInt();
            // Append to this list an unset element of type 'INT'.

        void appendUnsetDouble();
            // Append to this list an unset element of type 'DOUBLE'.

        void appendUnsetString();
            // Append to this list an unset element of type 'STRING'.
//..
// And provide functions to set the values of existing elements:
//..
        void setInt(int index, int value);
            // Set the element of type 'INT' at the specified 'index' in this
            // list to the specified 'value'.  The behavior is undefined unless
            // '0 <= index < length()' and 'INT == elemType(index)'.

        void setDouble(int index, double value);
            // Set the element of type 'DOUBLE' at the specified 'index' in
            // this list to the specified 'value'.  The behavior is undefined
            // unless '0 <= index < length()' and 'DOUBLE == elemType(index)'.

        void setString(int index, const bsl::string& value);
            // Set the element of type 'STRING' at the specified 'index' in
            // this list to the specified 'value'.  The behavior is undefined
            // unless '0 <= index < length()' and 'STRING == elemType(index)'.
//..
// Finally, provide a standard set of accessors to determine the length of the
// list, and the value and the type of an element at a specified 'index':
//..
        // ACCESSORS
        int length() const;
            // Return the number of elements in this list.

        int theInt(int index) const;
            // Return the element of type 'INT' at the specified 'index' in
            // this list.  The behavior is undefined unless
            // '0 <= index < length()' and 'INT == elemType(index)'.

        double theDouble(int index) const;
            // Return the element of type 'DOUBLE' at the specified 'index' in
            // this list.  The behavior is undefined unless
            // '0 <= index < length()' and 'DOUBLE == elemType(index)'.

        const bsl::string& theString(int index) const;
            // Return a reference to the non-modifiable element of type
            // 'STRING' at the specified 'index' in this list.  The behavior is
            // undefined unless '0 <= index < length()' and
            // 'STRING == elemType(index)'.

        ELEMENT_TYPE elemType(int index) const;
            // Return the type of the element at the specified 'index' in this
            // list.  The behavior is undefined unless '0 <= index < length()'.

        bool isUnset(int index) const;
            // Return 'true' if the element at the specified 'index' in this
            // list is unset, and 'false' otherwise.  The behavior is undefined
            // unless '0 <= index < length()'.
  };
//..
// Below are the function definitions.  Note that, in the interest of brevity,
// exception-safety concerns are not addressed:
//..
    // ========================================================================
    //                        INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CREATORS
    inline
    my_List::my_List(bslma::Allocator *basicAllocator)
    : d_types(basicAllocator)
    , d_values(basicAllocator)
    , d_allocator_p(basicAllocator)
    {
    }

    inline
    my_List::my_List(const my_List& original, bslma::Allocator *basicAllocator)
    : d_types(original.d_types, basicAllocator)
    , d_values(original.d_values, basicAllocator)
    , d_allocator_p(basicAllocator)
    {
    }

    inline
    my_List::~my_List()
    {
        BSLS_ASSERT_SAFE(d_types.size() == d_values.size());
    }

    // MANIPULATORS
    inline
    const my_List& my_List::operator=(const my_List& rhs)
    {
        if (this != &rhs) {
            d_types  = rhs.d_types;
            d_values = rhs.d_values;
        }
        return *this;
    }

    inline
    void my_List::appendInt(int value)
    {
        d_types.push_back(INT);
        Node node;
        node.d_int = value;
        d_values.push_back(node);
    }

    inline
    void my_List::appendDouble(double value)
    {
        d_types.push_back(DOUBLE);
        Node node;
        node.d_double = value;
        d_values.push_back(node);
    }

    inline
    void my_List::appendString(const bsl::string& value)
    {
        d_types.push_back(STRING);
        Node node;
        new (node.d_string.buffer()) bsl::string(value, d_allocator_p);
        d_values.push_back(node);
    }
//..
// Note the use of the 'bdltuxxx::Unset<TYPE>::unsetValue' function to initialize
// the elements in the following:
//..
    inline
    void my_List::appendUnsetInt()
    {
        d_types.push_back(INT);
        Node node;
        node.d_int = bdltuxxx::Unset<int>::unsetValue();
        d_values.push_back(node);
    }

    inline
    void my_List::appendUnsetDouble()
    {
        d_types.push_back(DOUBLE);
        Node node;
        node.d_double = bdltuxxx::Unset<double>::unsetValue();
        d_values.push_back(node);
    }

    inline
    void my_List::appendUnsetString()
    {
        d_types.push_back(STRING);
        Node node;
        new (node.d_string.buffer())
                           bsl::string(bdltuxxx::Unset<bsl::string>::unsetValue(),
                                       d_allocator_p);
        d_values.push_back(node);
    }

    inline
    void my_List::setInt(int index, int value)
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());
        BSLS_ASSERT_SAFE(INT == d_types[index]);

        d_values[index].d_int = value;
    }

    inline
    void my_List::setDouble(int index, double value)
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());
        BSLS_ASSERT_SAFE(DOUBLE == d_types[index]);

        d_values[index].d_double = value;
    }

    inline
    void my_List::setString(int index, const bsl::string& value)
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());
        BSLS_ASSERT_SAFE(STRING == d_types[index]);

        d_values[index].d_string.object() = value;
    }

    // ACCESSORS
    inline
    int my_List::length() const
    {
        BSLS_ASSERT_SAFE(d_types.size() == d_values.size());

        return d_types.size();
    }

    inline
    int my_List::theInt(int index) const
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());
        BSLS_ASSERT_SAFE(INT == d_types[index]);

        return d_values[index].d_int;
    }

    inline
    double my_List::theDouble(int index) const
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());
        BSLS_ASSERT_SAFE(DOUBLE == d_types[index]);

        return d_values[index].d_double;
    }

    inline
    const bsl::string& my_List::theString(int index) const
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());
        BSLS_ASSERT_SAFE(STRING == d_types[index]);

        return d_values[index].d_string.object();
    }

    inline
    my_List::ELEMENT_TYPE my_List::elemType(int index) const
    {
        BSLS_ASSERT_SAFE(0 <= index && index < length());

        return d_types[index];
    }
//..
// The 'isUnset' method is defined in the corresponding '.cpp' file:
//..
    // my_list.cpp

    bool my_List::isUnset(int index) const
    {
        BSLS_ASSERT(0 <= index && index < length());

        bool isElementUnset = false;

        switch (d_types[index]) {
          case INT: {
            isElementUnset = bdltuxxx::Unset<int>::isUnset(d_values[index].d_int);
          } break;
          case DOUBLE: {
            isElementUnset = bdltuxxx::Unset<double>::isUnset(
                                                     d_values[index].d_double);
          } break;
          case STRING: {
            isElementUnset = bdltuxxx::Unset<bsl::string>::isUnset(
                                            d_values[index].d_string.object());
          } break;
        }

        return isElementUnset;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// Usage Example 1
// - - - - - - - -
// The following snippets of code illustrate how to use the unset value for the
// type 'double'.  First, initialize 'unsetValue' with the unset value for
// 'double' and initialize a non-unset reference 'value' to 0.0:
//..
      double unsetValue = bdltuxxx::Unset<double>::unsetValue();
      double value      = 0.0;
//..
// Test if 'value' is unset; it should *not* be:
//..
      ASSERT(0 == bdltuxxx::Unset<double>::isUnset(value));
//..
// Set 'value' to the unset value:
//..
      bdltuxxx::Unset<double>::makeUnset(&value);
//..
// Assert that 'value' is now unset:
//..
      ASSERT(bdltuxxx::Unset<double>::isUnset(value));
      ASSERT(value == unsetValue);
//..

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'bdltuxxx::UnsetValueIsDefined'
        //
        // Plan:
        //  1. Brute force test the 'VALUE' for each defined type and confirm
        //     that it is a non-zero value.
        //  2. For types not having a defined unset value confirm that 'VALUE'
        //     is '0'.
        //
        // Testing:
        //   bdltuxxx::UnsetValueIsDefined<TYPE>::VALUE
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing functions for bdet::UnsetValueIsDefined<TYPE>"
                 << endl
                 << "===================================================="
                 << endl;
        ASSERT(bdltuxxx::UnsetValueIsDefined<bool>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<char>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<short>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<int>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bsls::Types::Int64>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<float>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<double>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bsl::string>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bdlt::Datetime>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bdlt::DatetimeTz>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bdlt::Date>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bdlt::DateTz>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bdlt::Time>::VALUE);
        ASSERT(bdltuxxx::UnsetValueIsDefined<bdlt::TimeTz>::VALUE);

        ASSERT(!bdltuxxx::UnsetValueIsDefined<unsigned char>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<signed char>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<unsigned short>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<unsigned int>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<unsigned long>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<signed long>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<long>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<bsls::Types::Uint64>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<long double>::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<bsl::vector<char> >::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<bsl::vector<int> >::VALUE);
        ASSERT(!bdltuxxx::UnsetValueIsDefined<bdlt::DatetimeInterval>::VALUE);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATETIMETZ
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bdlt::DatetimeTz> function.
        //   2. Assert the value for unsetValue<bdlt::DatetimeTz> is as
        //      expected.
        //   3. For a variable of type bdlt::DatetimeTz with a non unset value,
        //      use makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bdlt::DatetimeTz>(const bdlt::DatetimeTz&)
        //   unsetValue<bdlt::DatetimeTz>()
        //   makeUnset<bdlt::DatetimeTz>(bdlt::DatetimeTz*)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing functions for bdlt::DatetimeTz" << endl
                         << "=====================================" << endl;

        typedef bdlt::DatetimeTz T;

        const T UNSETVALUE;
        const bdlt::Datetime NONUNSETDATETIME(10, 10, 10, 10, 10, 10, 10);
        const T NONUNSETVALUE(NONUNSETDATETIME, -5);

        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(UNSETVALUE));

        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(NONUNSETVALUE));

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_TIMETZ
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bdlt::TimeTz> function.
        //   2. Assert the value for unsetValue<bdlt::TimeTz> is as expected.
        //   3. For a variable of type bdlt::TimeTz with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bdlt::TimeTz>(const bdlt::TimeTz&)
        //   unsetValue<bdlt::TimeTz>()
        //   makeUnset<bdlt::TimeTz>(bdlt::TimeTz*)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing functions for bdlt::TimeTz" << endl
                          << "=================================" << endl;

        typedef bdlt::TimeTz T;

        const T UNSETVALUE;
        const bdlt::Time NONUNSETTIME(10, 10, 10, 10);
        const T NONUNSETVALUE(NONUNSETTIME, -5);

        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(UNSETVALUE));

        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(NONUNSETVALUE));

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATETZ
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bdlt::DateTz> function.
        //   2. Assert the value for unsetValue<bdlt::DateTz> is as expected.
        //   3. For a variable of type bdlt::DateTz with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bdlt::DateTz>(const bdlt::DateTz&)
        //   unsetValue<bdlt::DateTz>()
        //   makeUnset<bdlt::DateTz>(bdlt::DateTz*)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing functions for bdlt::DateTz" << endl
                          << "=================================" << endl;

        typedef bdlt::DateTz T;

        const T UNSETVALUE;
        const bdlt::Date NONUNSETDATE(10, 10, 10);
        const T NONUNSETVALUE(NONUNSETDATE, -5);

        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(UNSETVALUE));

        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(NONUNSETVALUE));

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BOOL
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //   from the isUnset<bool> function.
        //   2. Assert the value for unsetValue<bool> is as expected.
        //   3. For a variable of type bool with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bool>(const bool&)
        //   unsetValue<bool>()
        //   makeUnset<bool>(bool*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bool" << endl
                                  << "==========================" << endl;

        typedef bool T;

        const T DATA[]  = { true, false };
        const T UNSETVALUE = false;
        const T NONUNSETVALUE = true;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_((int) DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P((int) bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATETIME
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bdlt::Datetime> function.
        //   2. Assert the value for unsetValue<bdlt::Datetime> is as expected.
        //   3. For a variable of type bdlt::Datetime with a non unset value,
        //      use makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bdlt::Datetime>(const bdlt::Datetime&)
        //   unsetValue<bdlt::Datetime>()
        //   makeUnset<bdlt::Datetime>(bdlt::Datetime*)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing functions for bdlt::Datetime" << endl
                         << "===================================" << endl;

        typedef bdlt::Datetime T;

        const T UNSETVALUE;
        const bdlt::Date NONUNSETDATE(10, 10, 10);
        const T NONUNSETVALUE(NONUNSETDATE);

        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(UNSETVALUE));

        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(NONUNSETVALUE));

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_TIME
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bdlt::Time> function.
        //   2. Assert the value for unsetValue<bdlt::Time> is as expected.
        //   3. For a variable of type bdlt::Time with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bdlt::Time>(const bdlt::Time&)
        //   unsetValue<bdlt::Time>()
        //   makeUnset<bdlt::Time>(bdlt::Time*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bdlt::Time" << endl
                                  << "===============================" << endl;

        typedef bdlt::Time T;

        const T UNSETVALUE;
        const T NONUNSETVALUE(10, 10, 10);

        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(UNSETVALUE));

        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(NONUNSETVALUE));

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATE
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bdlt::Date> function.
        //   2. Assert the value for unsetValue<bdlt::Date> is as expected.
        //   3. For a variable of type bdlt::Date with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bdlt::Date>(const bdlt::Date&)
        //   unsetValue<bdlt::Date>()
        //   makeUnset<bdlt::Date>(bdlt::Date*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bdlt::Date" << endl
                                  << "===============================" << endl;

        typedef bdlt::Date T;

        const T UNSETVALUE;
        const T NONUNSETVALUE(10, 10, 10);

        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(UNSETVALUE));

        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(NONUNSETVALUE));

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE bsl::string
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bsl::string> function.
        //   2. Assert the value for unsetValue<bsl::string> is as expected.
        //   3. For a variable of type bsl::string with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bsl::string>(const bsl::string&)
        //   unsetValue<bsl::string>()
        //   makeUnset<bsl::string>(bsl::string*)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing functions for bsl::string" << endl
                         << "=================================" << endl;

        typedef bsl::string T;

        const T UNSETVALUE;
        const T DATA[]  = { UNSETVALUE, " ", "abc" };
        const T NONUNSETVALUE = "x";

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE DOUBLE
        // Concern:
        //   The value chosen for the unset value must never round and must
        //   never equal zero.
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<double> function.
        //   2. Assert the value for unsetValue<double> is as expected.
        //   3. For a variable of type double with a non unset value, use
        //      'makeUnset' and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<double>(const double&)
        //   unsetValue<double>()
        //   makeUnset<double>(double*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for double" << endl
                                  << "============================" << endl;

        typedef double T;

        const T UNSETVALUE = -27953.0 / (1 << 30) / (1 << 30);
        const T DATA[]  = { UNSETVALUE, -1, 1, 0, 127, -127 };
        const T NONUNSETVALUE = 10;

        ASSERT((UNSETVALUE * (1 << 30) * (1 << 30)) == -27953.0);

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        if(veryVerbose) P(UNSETVALUE);

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE FLOAT
        // Concern:
        //   The value chosen for the unset value must never round and must
        //   never equal zero.
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<float> function.
        //   2. Assert the value for unsetValue<float> is as expected.
        //   3. For a variable of type float with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<float>(const float&)
        //   unsetValue<float>()
        //   makeUnset<float>(float*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for float" << endl
                                  << "===========================" << endl;

        typedef float T;

        const T UNSETVALUE = -7568.0/(1<<30)/(1<<30);
        const T DATA[]  = { UNSETVALUE, -1, 1, 0, 127, -127 };
        const T NONUNSETVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        ASSERT((UNSETVALUE * (1<<30) * (1<<30)) == -7568.0);

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE INT64
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<bsls::Types::Int64> function.
        //   2. Assert the value for unsetValue<bsls::Types::Int64> is as
        //      expected.
        //   3. For a variable of type bsls::Types::Int64 with a non
        //      unset value, use makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<bsls::Types::Int64>(const bsls::Types::Int64&)
        //   unsetValue<bsls::Types::Int64>()
        //   makeUnset<bsls::Types::Int64>(bsls::Types::Int64*)
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl << "Testing functions for bsls::Types::Int64" << endl
            << "==============================================" << endl;

        typedef bsls::Types::Int64 T;

        const T UNSETVALUE = -(bsls::Types::Int64)
                              (((bsls::Types::Uint64) 1 << 63) - 1) - 1;
        const T DATA[]  = { UNSETVALUE, -1, 1, 0, 127, -127 };
        const T NONUNSETVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE INT
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //      from the isUnset<int> function.
        //   2. Assert the value for unsetValue<int> is as expected.
        //   3. For a variable of type int with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<int>(const int&)
        //   unsetValue<int>()
        //   makeUnset<int>(int*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for int" << endl
                                  << "==========================" << endl;

        typedef int T;

        const T DATA[]  = { -2147483647 - 1, -1, 1, 0, 127, -127 };
        const T UNSETVALUE = -2147483647 - 1;
        const T NONUNSETVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE SHORT
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //   from the isUnset<short> function.
        //   2. Assert the value for unsetValue<short> is as expected.
        //   3. For a variable of type short with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<short>(const short&)
        //   unsetValue<short>()
        //   makeUnset<short>(short*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for short" << endl
                                  << "==========================" << endl;

        typedef short T;

        const T DATA[]  = { -32768, -1, 1, 0, 127, -127 };
        const T UNSETVALUE = -32768;
        const T NONUNSETVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P(bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE CHAR
        //
        // Plan:
        //   1. For a set of values (unset and not unset) test the return value
        //   from the isUnset<char> function.
        //   2. Assert the value for unsetValue<char> is as expected.
        //   3. For a variable of type char with a non unset value, use
        //      makeUnset and verify that it becomes unset.
        //
        // Testing:
        //   isUnset<char>(const char&)
        //   unsetValue<char>()
        //   makeUnset<char>(char*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for char" << endl
                                  << "==========================" << endl;

        typedef char T;

        const T DATA[]  = { 0, -1, 1, 127, -127 };
        const T UNSETVALUE = 0;
        const T NONUNSETVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            bool hasUnset = DATA[i] == UNSETVALUE;
            if (veryVerbose) { P_((int) DATA[i]); P(hasUnset); }
            LOOP_ASSERT(i, bdltuxxx::Unset<T>::isUnset(DATA[i]) == hasUnset);
        }

        if (veryVerbose) P((int) bdltuxxx::Unset<T>::unsetValue());

        ASSERT(UNSETVALUE == bdltuxxx::Unset<T>::unsetValue());

        T U1 = NONUNSETVALUE, U2 = NONUNSETVALUE;
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U1));
        ASSERT(0 == bdltuxxx::Unset<T>::isUnset(U2));

        bdltuxxx::Unset<T>::makeUnset(&U2);
        ASSERT(1 == bdltuxxx::Unset<T>::isUnset(U2));
      } break;
      case -1: {
        bsl::cout << "Test timing of bdltuxxx::Unset<double>::isUnset()"
                  << bsl::endl;

        double x = 0.0;
        for (int i = 0; i < 100000; ++i) {
            foo(x);
            x += 1.0;
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
