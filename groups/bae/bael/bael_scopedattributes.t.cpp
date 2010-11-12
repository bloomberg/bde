// bael_scopedattributes.t.cpp                                        -*-C++-*-
#include <bael_scopedattributes.h>

#include <bael_attributecontext.h>
#include <bael_attributecontainer.h>

#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_assert.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_set.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
int verbose;
int veryVerbose;
int veryVeryVerbose;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// 'AttributeSet', as defined in the 'bael_attributecontainer' component
// documentation.

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
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------

//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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
        if (verbose) cout << "\nTesting usage example"
                          << "\n====================="
                          << endl;
///USAGE
///-----
// In the following code fragment, we will use a 'bael_ScopedAttributes' to
// install a 'bael_AttributeContainer' in the current context.
//
// We first create the current attribute context and two attributes:
//..
    bael_AttributeContext *context = bael_AttributeContext::getContext();

    bael_Attribute a1("uuid", 4044457);
    bael_Attribute a2("name", "Gang Chen");
    ASSERT(false == context->hasAttribute(a1));
    ASSERT(false == context->hasAttribute(a2));
//..
// Now we create an 'AttributeSet' and add the two attributes to this set,
// then we use a 'bael_ScopedAttributes to install these attributes in the
// current thread's attribute context.
//
// Note that we use the 'AttributeSet' implementation of the
// 'bael_AttributeContainer' protocol defined in the component documentation
// for 'bael_attributecontainer' (the 'bael' package provides a similar class
// in the 'bael_defaultattributecontainer' component).
//..
    {
        AttributeSet attributes;
        attributes.insert(a1);
        attributes.insert(a2);
        bael_ScopedAttributes attributeGuard(&attributes);
        ASSERT(true == context->hasAttribute(a1));
        ASSERT(true == context->hasAttribute(a2));
//..
// When 'attributeGuard' goes out of scope and is destroyed, 'attributes'
// are removed from the current thread's attribute context, which prevents the
// attribute context from referring to an invalid memory address (on the
// stack).
//..
    }
    ASSERT(!context->hasAttribute(a1));
    ASSERT(!context->hasAttribute(a2));
//..

      } break;

      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    };

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
