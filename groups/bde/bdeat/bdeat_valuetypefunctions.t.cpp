// bdeat_valuetypefunctions.t.cpp                  -*-C++-*-

#include <bdeat_valuetypefunctions.h>

#include <bdeat_typetraits.h>

#include <bslalg_typetraits.h>

#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//-----------------------------------------------------------------------------
// [ 2] void bdeat_ValueTypeFunctions::reset(TYPE *object);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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

//=============================================================================
//                      CLASSES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

enum Enum { E1 = 1, E2 = 2, E3 = 3 };

namespace geom {

class Sequence_Point {

  public:
    BSLALG_DECLARE_NESTED_TRAITS(Sequence_Point, bdeat_TypeTraitBasicSequence);

  private:
    int d_x;  // X coordinate
    int d_y;  // Y coordinate

  public:
    // CREATORS
    Sequence_Point()
    : d_x(0)
    , d_y(0)
    {
    }

    Sequence_Point(const Sequence_Point& original)
    : d_x(original.d_x)
    , d_y(original.d_y)
    {
    }

    ~Sequence_Point()
    {
    }

    // MANIPULATORS
    Sequence_Point& operator=(const Sequence_Point& rhs)
    {
        d_x = rhs.d_x;
        d_y = rhs.d_y;
        return *this;
    }

    void reset()
    {
        d_x = 0;
        d_y = 0;
    }

    int& x()
    {
        return d_x;
    }

    int& y()
    {
        return d_y;
    }

    // ACCESSORS
    int x() const
    {
        return d_x;
    }

    int y() const
    {
        return d_y;
    }
};

class Choice_Point {

  public:
    BSLALG_DECLARE_NESTED_TRAITS(Choice_Point, bdeat_TypeTraitBasicChoice);

  private:
    int d_x;  // X coordinate
    int d_y;  // Y coordinate

  public:
    // CREATORS
    Choice_Point()
    : d_x(0)
    , d_y(0)
    {
    }

    Choice_Point(const Choice_Point& original)
    : d_x(original.d_x)
    , d_y(original.d_y)
    {
    }

    ~Choice_Point()
    {
    }

    // MANIPULATORS
    Choice_Point& operator=(const Choice_Point& rhs)
    {
        d_x = rhs.d_x;
        d_y = rhs.d_y;
        return *this;
    }

    void reset()
    {
        d_x = 0;
        d_y = 0;
    }

    int& x()
    {
        return d_x;
    }

    int& y()
    {
        return d_y;
    }

    // ACCESSORS
    int x() const
    {
        return d_x;
    }

    int y() const
    {
        return d_y;
    }
};

}  // end namespace geom

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

void usageExample()
{
    using namespace BloombergLP;

    int               intVal    = 123;
    float             floatVal  = 34.56;
    bsl::string       stringVal = "Hello";
    bsl::vector<char> vecVal, vecVal2, vecVal3;

    vecVal.push_back('T');
    vecVal.push_back('e');
    vecVal.push_back('s');
    vecVal.push_back('t');

    vecVal2 = vecVal;

    // ASSERT(0 == bdeat_ValueTypeFunctions::assign(&vecVal3, vecVal2));

    bdeat_ValueTypeFunctions::reset(&intVal);
    bdeat_ValueTypeFunctions::reset(&floatVal);
    bdeat_ValueTypeFunctions::reset(&stringVal);
    bdeat_ValueTypeFunctions::reset(&vecVal);

    ASSERT(0 == intVal);
    ASSERT(0 == floatVal);
    ASSERT(stringVal.empty());
    ASSERT(vecVal.empty());

    // ASSERT(vecVal2 == vecVal3);
}

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
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageExample();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST RESET
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void bdeat_ValueTypeFunctions::reset(TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'reset'"
                          << "\n===============" << endl;

        {
            Enum mX = E2;
            ASSERT(E1 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            // TBD what to ASSERT, if anything
            // ASSERT(E1 == mX);
        }

        {
            bool mX = true;
            ASSERT(false != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(false == mX);
        }

        {
            char mX = 'a';;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            signed char mX = 'a';
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            unsigned char mX = 'a';
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            short mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            unsigned short mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            int mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            unsigned int mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            bsls_PlatformUtil::Int64  mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            bsls_PlatformUtil::Uint64  mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            float mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            double mX = 77;
            ASSERT(0 != mX);
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            bsl::string mX("foo");  const bsl::string& X = mX;
            ASSERT(0 != X.size());
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.size());
        }

        {
            bsl::vector<int> mX;  const bsl::vector<int>& X = mX;
            mX.push_back(77);
            ASSERT(0 != X.size());
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.size());
        }

        {
            geom::Sequence_Point mX;  const geom::Sequence_Point& X = mX;
            mX.x() = 77;
            mX.y() = 77;
            ASSERT(0 != X.x() && 0 != X.y());
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.x() && 0 == X.y());
        }

        {
            geom::Choice_Point mX;  const geom::Choice_Point& X = mX;
            mX.x() = 77;
            mX.y() = 77;
            ASSERT(0 != X.x() && 0 != X.y());
            bdeat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.x() && 0 == X.y());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHODS TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHODS TEST" << endl
                          << "============" << endl;

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
