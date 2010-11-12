// bael_attributecontainer.t.cpp          -*-C++-*-
#include <bael_attributecontainer.h>

#include <bael_attribute.h>             // for testing only

#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>                      // atoi()
#include <bsl_cstring.h>                      // strcmp(), memcmp(), memcpy()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The enumeration under test is non-standard in that the enumeration values
// are non-consecutive.  Therefore, the standard enumeration test plan is
// modified where necessary to accommodate the non-consecutive nature of this
// enumeration.  Note that the set of enumerator values under test are defined
// by { n << 5 : 1 <= n <= 7 }, so expressions similar to "n << 5" occur
// regularly in the test cases.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

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

typedef bael_AttributeContainer Obj;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: An Implementation of 'bael_AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we develop a 'bael_AttributeContainer'
// implementation specifically intended for a service offline that will perform
// rule-based logging based on a service client's bloomberg "uuid", "luw",
// and "firm number".  We define the class 'ServiceAttributes' that contains 3
// integer 'bael_Attribute' values with the names "uuid", "luw", and
// "firmNumber".
//
// Note that this implementation requires no memory allocation, so it will be
// more efficient than a more general set-based implementation if the
// container is frequently created, destroyed, or modified.  We will develop a
// 'bael_AttributeContainer' implementation that can hold any 'bael_Attribute'
// value in example 2 (and one is provided by the 'bael' package in the
// 'bael_defaultattributecontainer' component).
//..
      // serviceattributes.h

    class ServiceAttributes : public bael_AttributeContainer {
        // Provide a concrete implementation of the 'bael_AttributeContainer'
        // that holds the 'uuid', 'luw', and 'firmNumber' associated with a
        // request to the example service.  This container implementations
        // exposes those properties in attributes named "uuid", "luw", and
        // "firmNumber" respectively.

//..
// Note that we choose to have data members of type 'bael_Attribute' for
// simplicity.  It would be marginally more efficient to hold the values in
// 'int' data members, but the implementation of 'hasValue()' would be less
// readable.
//..
        // DATA
        bael_Attribute d_uuid;        // bloomberg user id
        bael_Attribute d_luw;         // bloomberg luw
        bael_Attribute d_firmNumber;  // bloomberg firm number

        // ...

      public:
        // PUBLIC CONSTANTS
        static const char * const UUID_ATTRIBUTE_NAME;
        static const char * const LUW_ATTRIBUTE_NAME;
        static const char * const FIRMNUMBER_ATTRIBUTE_NAME;
            // The names of the attributes exposed by this attribute container.

        // CREATORS
        ServiceAttributes(int uuid, int luw, int firmNumber);
            // Create a service attributes object with the specified 'uuid',
            // 'luw', and 'firmNumber'.

        virtual ~ServiceAttributes();
            // Destroy this service attributes object.

        // ACCESSORS
        virtual bool hasValue(const bael_Attribute& value) const;
            // Return 'true' if the attribute having specified 'value' exists
            // in this object, and 'false' otherwise.  This implementation will
            // return 'true' if 'value.name()' equals "uuid", "luw", or "firm"
            // and 'value.value()' is an 'int' equal to the corresponding
            // property value supplied at construction.

        virtual bsl::ostream& print(bsl::ostream& stream,
                                    int           level = 0,
                                    int           spacesPerLevel = 4) const;
            // Format this object to the specified output 'stream'.
    };


    // CREATORS
    inline
    ServiceAttributes::ServiceAttributes(int uuid, int luw, int firmNumber)
    : d_uuid(UUID_ATTRIBUTE_NAME, uuid)
    , d_luw(LUW_ATTRIBUTE_NAME, luw)
    , d_firmNumber(FIRMNUMBER_ATTRIBUTE_NAME, firmNumber)
    {
    }

      // serviceattributes.cpp

    // PUBLIC CONSTANTS
    const char * const ServiceAttributes::UUID_ATTRIBUTE_NAME       = "uuid";
    const char * const ServiceAttributes::LUW_ATTRIBUTE_NAME        = "luw";
    const char * const ServiceAttributes::FIRMNUMBER_ATTRIBUTE_NAME =
                                                                  "firmNumber";

    // CREATORS
    ServiceAttributes::~ServiceAttributes()
    {
    }

    // ACCESSORS
    bool ServiceAttributes::hasValue(const bael_Attribute& value) const
    {
        return d_uuid == value || d_luw == value || d_firmNumber == value;
    }

    bsl::ostream& ServiceAttributes::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
    {
        char EL = (spacesPerLevel < 0) ? ' ' : '\n';
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "[ "
               << d_uuid << " "
               << d_luw << " "
               << d_firmNumber << " "
               << "]" << EL;
        return stream;
    }
//
///Example 2: A Generic Implementation of 'bael_AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this second example we define a 'bael_AttributeContainer' that can
// contain any valid 'bael_Attribute' value (a "generic"
// 'bael_AttributeContainer').  In practice, an implementation that can
// contain any attribute values may be less efficient than an implementation
// specifically created for a particular group of attributes needed by an
// application (as seen in example 1).
//
// Note that the 'bael' package provides a similar 'bael_AttributeContainer'
// implementation in the 'bael_defaultattributecontainer' component.
//..
     // attributeset.h

    class AttributeSet : public bael_AttributeContainer {
        // A simple set based implementation of the 'bael_AttributeContainer'
        // protocol used for testing.

//..
// To defined an stl-set (or hash set) for 'bael_Attribute' values, we must
// define a comparison (or hash) operation for attribute values.  Here we
// define a comparison functor that compares attributes by name, then by
// value-type, and finally by value.
//..
        struct AttributeComparator {
            bool operator()(const bael_Attribute& lhs,
                            const bael_Attribute& rhs) const
                // Return 'true' if the specified 'lhs' is ordered before the
                // specified 'rhs'.
            {
                int cmp = bsl::strcmp(lhs.name(), rhs.name());
                if (0 != cmp) {
                    return cmp < 0;
                }
                if (lhs.value().typeIndex() != rhs.value().typeIndex()) {
                    return lhs.value().typeIndex() < rhs.value().typeIndex();
                }
                switch (lhs.value().typeIndex()) {
                  case 0: // unset
                    return true;
                  case 1: // int
                    return lhs.value().the<int>() < rhs.value().the<int>();
                  case 2: // int64
                    return lhs.value().the<bsls_PlatformUtil::Int64>()
                        <  rhs.value().the<bsls_PlatformUtil::Int64>();
                 case 3: // string
                   return lhs.value().the<bsl::string>()
                       <  rhs.value().the<bsl::string>();
                }
                BSLS_ASSERT(false);
                return false;
            }
        };

        // DATA
        bsl::set<bael_Attribute, AttributeComparator> d_set;

        // ...

      public:
        // CREATORS
        AttributeSet(bslma_Allocator *basicAllocator = 0);
            // Create this attribute set.

        virtual ~AttributeSet();
            // Destroy this attribute set.

        // MANIPULATORS
        void insert(const bael_Attribute& value);
            // Add the specified value to this attribute set.

        bool remove(const bael_Attribute& value);
            // Remove the specified value from this attribute set, return
            // 'true' if the attribute was found, and 'false' if 'value' was
            // not a member of this set.

        // ACCESSORS
        virtual bool hasValue(const bael_Attribute& value) const;
            // Return 'true' if the attribute having specified 'value' exists
            // in this object, and 'false' otherwise.

        virtual bsl::ostream& print(bsl::ostream& stream,
                                    int           level = 0,
                                    int           spacesPerLevel = 4) const;
            // Format this object to the specified output 'stream' at the
            // (absolute value of) the optionally specified indentation 'level'
            // and return a reference to 'stream'.
    };

//..
// The 'bael_AttributeContainer' methods are simple wrappers around
// operations on an 'bsl::set'.
//..
    inline
    AttributeSet::AttributeSet(bslma_Allocator *basicAllocator)
    : d_set(AttributeComparator(), basicAllocator)
    {
    }

    // MANIPULATORS
    inline
    void AttributeSet::insert(const bael_Attribute& value)
    {
        d_set.insert(value);
    }

    inline
    bool AttributeSet::remove(const bael_Attribute& value)
    {
        return d_set.erase(value) > 0;
    }

    // attributeset.cpp

    // CREATORS
    AttributeSet::~AttributeSet()
    {
    }

    // ACCESSORS
    bool AttributeSet::hasValue(const bael_Attribute& value) const
    {
        return d_set.find(value) != d_set.end();
    }

    bsl::ostream& AttributeSet::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
    {
        char EL = (spacesPerLevel < 0) ? ' ' : '\n';
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "[" << EL;

        bsl::set<bael_Attribute>::const_iterator it = d_set.begin();
        for (; it != d_set.end(); ++it) {
            it->print(stream, level+1, spacesPerLevel);
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]" << EL;
        return stream;
    }

//=============================================================================
//                         GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class TestAttributeContainer : public bael_AttributeContainer {

  public:
    // TYPES
    enum Method {
        NONE = 0,
        DESTRUCTOR,
        HAS_VALUE,
        PRINT
    };

  private:
    // DATA
    Method *d_lastMethod;

  public:

    // CREATORS
    TestAttributeContainer(Method *lastMethod)
    : d_lastMethod(lastMethod)
    {
        *lastMethod = NONE;
    }

    // CREATORS
    virtual ~TestAttributeContainer();
        // Destroy this object and set the 'lastMethod' supplied at
        // construction to 'DESTRUCTOR'.

    // ACCESSORS
    virtual bool hasValue(const bael_Attribute& value) const;
        // Return 'true' and set the 'lastMethod' supplied at construction  to
        // 'HAS_VALUE'.

    virtual bsl::ostream& print(bsl::ostream& stream,
                                int           level = 0,
                                int           spacesPerLevel = 4) const;
        // Return a reference to the specified 'stream' and set the
        // 'lastMethod' supplied at construction  to 'PRINT'
};

// CREATORS
TestAttributeContainer::~TestAttributeContainer()
{
    *d_lastMethod = DESTRUCTOR;
}

// ACCESSORS
bool TestAttributeContainer::hasValue(const bael_Attribute& value) const
{
    *d_lastMethod = HAS_VALUE;
    return true;
}

bsl::ostream& TestAttributeContainer::print(
                                        bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    *d_lastMethod = PRINT;
    return stream;
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

//..
//
///Example 3. Using a 'bael_AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - -
// In this final (trivial) example, we demonstrate calling the methods of the
// 'bael_AttributeContainer' protocol.
//
// First we create an object of a concrete type that implements the
// 'bael_AttributeContainer' protocol (e.g., 'ServiceAttributes'
// defined in example 1).  Then we obtain a reference to this object.
//..
    ServiceAttributes serviceAttributes(3938908, 1, 9001);
    const bael_AttributeContainer& attributes = serviceAttributes;
//..
// We use 'hasValue()' to examine the values in the container:
//..
    ASSERT(attributes.hasValue(bael_Attribute("uuid", 3938908)));
    ASSERT(attributes.hasValue(bael_Attribute("luw",  1)));
    ASSERT(attributes.hasValue(bael_Attribute("firmNumber", 9001)));

    ASSERT(!attributes.hasValue(bael_Attribute("uuid", 1391015)));
    ASSERT(!attributes.hasValue(bael_Attribute("luw",  2)));
    ASSERT(!attributes.hasValue(bael_Attribute("wrong name", 3938908)));
//..
// Finally we can print the attribute values in the container:
//..
    bsl::cout << attributes << bsl::endl;
//..
// The resulting output should look like:
//..
// [ [ uuid = 3938908 ]   [ luw = 1 ]   [ firmNumber = 9001 ] ]
//..
      } break;
      case 1: {


          TestAttributeContainer::Method method;
          {
              TestAttributeContainer mX(&method);
              const TestAttributeContainer& MX = mX;

              ASSERT(TestAttributeContainer::NONE == method);

              MX.hasValue(bael_Attribute("A", 1));

              ASSERT(TestAttributeContainer::HAS_VALUE == method);

              MX.print(bsl::cout, 1, 3);

              ASSERT(TestAttributeContainer::PRINT == method);
          }
          ASSERT(TestAttributeContainer::DESTRUCTOR == method);
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
