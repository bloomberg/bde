// bdlmxxx_selectbdemtype.t.cpp                                          -*-C++-*-

#include <bdlmxxx_selectbdemtype.h>

#include <bdlmxxx_elemtype.h>

#include <bdlt_date.h>            // for testing only
#include <bdlt_datetime.h>        // for testing only
#include <bdlt_time.h>            // for testing only

#include <bslmf_issame.h>         // for testing only

#include <bsls_types.h>

#include <bsl_cstdlib.h>
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
// This component contains a finite-set of template specializations, so we will
// test each specialization [1].  Finally, we will test the usage example [2]
// to ensure that it works as expected.
//-----------------------------------------------------------------------------
// [ 1] struct bdlmxxx::SelectBdemType
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class my_Class {};
enum my_Enum {};

// Note that 'DUMMY' template arguments are to workaround the "ambiguous
// partial specialization" bug in Sun5.2.

template <typename TYPE>
class MyArray { };

namespace BloombergLP {
    namespace bdeat_ArrayFunctions {
        template <typename TYPE>
        struct IsArray<MyArray<TYPE> > {
            enum { VALUE = 1 };
        };

        template <typename TYPE>
        struct ElementType<MyArray<TYPE> > {
            typedef TYPE Type;
        };
    }
}

template <typename TYPE, typename DUMMY1=void>
class MyCustomizedType { };

namespace BloombergLP {
    namespace bdeat_CustomizedTypeFunctions {
        template <typename TYPE, typename DUMMY1>
        struct IsCustomizedType<MyCustomizedType<TYPE, DUMMY1> > {
            enum { VALUE = 1 };
        };

        template <typename TYPE, typename DUMMY1>
        struct BaseType<MyCustomizedType<TYPE, DUMMY1> > {
            typedef TYPE Type;
        };
    }
}

template <typename TYPE, typename DUMMY1=void,
                         typename DUMMY2=void>
class MyNullableValue { };

namespace BloombergLP {
    namespace bdeat_NullableValueFunctions {
        template <typename TYPE, typename DUMMY1,
                                 typename DUMMY2>
        struct IsNullableValue<MyNullableValue<TYPE, DUMMY1,
                                                     DUMMY2> > {
            enum { VALUE = 1 };
        };

        template <typename TYPE, typename DUMMY1,
                                 typename DUMMY2>
        struct ValueType<MyNullableValue<TYPE, DUMMY1,
                                               DUMMY2> > {
            typedef TYPE Type;
        };
    }
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate basic usage of this component.
// Note that the 'bslmf::IsSame' meta-function is employed only for the
// purposes of illustration:
//..
    class Class {};
    enum Enum {};
//
// continued below in case 0 ...

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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

// ... continued from above
//  // 'bool' -> 'bool'
    ASSERT(bdlmxxx::ElemType::BDEM_BOOL == (int)bdlmxxx::SelectBdemType<bool>::VALUE);
    typedef bdlmxxx::SelectBdemType<bool>::Type boolProxyType;
    ASSERT(1 == (bslmf::IsSame<bool, boolProxyType>::VALUE));
//
//  // 'unsigned' -> 'int'
    ASSERT(bdlmxxx::ElemType::BDEM_INT ==
                                    (int)bdlmxxx::SelectBdemType<unsigned>::VALUE);
    typedef bdlmxxx::SelectBdemType<unsigned>::Type unsignedProxyType;
    ASSERT(1 == (bslmf::IsSame<int, unsignedProxyType>::VALUE));
//
//  // 'Enum' -> 'int'
    ASSERT(bdlmxxx::ElemType::BDEM_INT == (int)bdlmxxx::SelectBdemType<Enum>::VALUE);
    typedef bdlmxxx::SelectBdemType<Enum>::Type enumProxyType;
    ASSERT(1 == (bslmf::IsSame<int, enumProxyType>::VALUE));
//
//  // 'bsl::vector<Enum>' -> 'bsl::vector<int>'
    ASSERT(bdlmxxx::ElemType::BDEM_INT_ARRAY ==
                          (int)bdlmxxx::SelectBdemType<bsl::vector<Enum> >::VALUE);
    typedef bdlmxxx::SelectBdemType<bsl::vector<Enum> >::Type enumVectorProxyType;
    ASSERT(1 == (bslmf::IsSame<bsl::vector<int>, enumVectorProxyType>::VALUE));
//
//  // 'Class' -> 'bdlmxxx::List'
    ASSERT(bdlmxxx::ElemType::BDEM_LIST == (int)bdlmxxx::SelectBdemType<Class>::VALUE);
    typedef bdlmxxx::SelectBdemType<Class>::Type classProxyType;
    ASSERT(1 == (bslmf::IsSame<bdlmxxx::List, classProxyType>::VALUE));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING bdlmxxx::SelectBdemType
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct bdlmxxx::SelectBdemType
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'bdlmxxx::SelectBdemType'" << endl
                          << "=============================" << endl;

        enum {
            IS_ARRAY =
                   bdeat_ArrayFunctions::IsArray<MyCustomizedType<int> >::VALUE
        };

        char c[IS_ARRAY ? -1 : 1];

        {
            typedef bool TT;  // test type
            typedef bool PT;  // proxy type

            const int ET = bdlmxxx::ElemType::BDEM_BOOL;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef char TT;
            typedef char PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef signed char TT;
            typedef char        PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef unsigned char TT;
            typedef char          PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef short TT;
            typedef short PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef unsigned short TT;
            typedef short          PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef wchar_t TT;
            typedef short   PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef int TT;
            typedef int PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef unsigned int TT;
            typedef int          PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsls::Types::Int64 TT;
            typedef bsls::Types::Int64 PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsls::Types::Uint64 TT;
            typedef bsls::Types::Int64  PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef float TT;
            typedef float PT;

            const int ET = bdlmxxx::ElemType::BDEM_FLOAT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef double TT;
            typedef double PT;

            const int ET = bdlmxxx::ElemType::BDEM_DOUBLE;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::string TT;
            typedef bsl::string PT;

            const int ET = bdlmxxx::ElemType::BDEM_STRING;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlt::Date TT;
            typedef bdlt::Date PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATE;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlt::Datetime TT;
            typedef bdlt::Datetime PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETIME;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlt::Time TT;
            typedef bdlt::Time PT;

            const int ET = bdlmxxx::ElemType::BDEM_TIME;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<char> TT;
            typedef bsl::vector<char> PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<signed char> TT;
            typedef bsl::vector<char>        PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<unsigned char> TT;
            typedef bsl::vector<char>          PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<short> TT;
            typedef bsl::vector<short> PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<unsigned short> TT;
            typedef bsl::vector<short>          PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<wchar_t> TT;
            typedef bsl::vector<short>   PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<int> TT;
            typedef bsl::vector<int> PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<unsigned int> TT;
            typedef bsl::vector<int>          PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bsls::Types::Int64> TT;
            typedef bsl::vector<bsls::Types::Int64> PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bsls::Types::Uint64> TT;
            typedef bsl::vector<bsls::Types::Int64>  PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<float> TT;
            typedef bsl::vector<float> PT;

            const int ET = bdlmxxx::ElemType::BDEM_FLOAT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<double> TT;
            typedef bsl::vector<double> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bsl::string> TT;
            typedef bsl::vector<bsl::string> PT;

            const int ET = bdlmxxx::ElemType::BDEM_STRING_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdlt::Date> TT;
            typedef bsl::vector<bdlt::Date> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATE_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdlt::Datetime> TT;
            typedef bsl::vector<bdlt::Datetime> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETIME_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdlt::Time> TT;
            typedef bsl::vector<bdlt::Time> PT;

            const int ET = bdlmxxx::ElemType::BDEM_TIME_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef my_Class  TT;
            typedef bdlmxxx::List PT;

            const int ET = bdlmxxx::ElemType::BDEM_LIST;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<my_Class> TT;
            typedef bdlmxxx::Table            PT;

            const int ET = bdlmxxx::ElemType::BDEM_TABLE;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef my_Enum TT;
            typedef int     PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<my_Enum> TT;
            typedef bsl::vector<int>     PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<char>     TT;
            typedef bsl::vector<char> PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<signed char> TT;
            typedef bsl::vector<char>    PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<unsigned char> TT;
            typedef bsl::vector<char>      PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<short>     TT;
            typedef bsl::vector<short> PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<unsigned short> TT;
            typedef bsl::vector<short>      PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<wchar_t>   TT;
            typedef bsl::vector<short> PT;

            const int ET = bdlmxxx::ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<int>     TT;
            typedef bsl::vector<int> PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<unsigned int> TT;
            typedef bsl::vector<int>      PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bsls::Types::Int64>     TT;
            typedef bsl::vector<bsls::Types::Int64> PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bsls::Types::Uint64>    TT;
            typedef bsl::vector<bsls::Types::Int64> PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<float>     TT;
            typedef bsl::vector<float> PT;

            const int ET = bdlmxxx::ElemType::BDEM_FLOAT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<double>     TT;
            typedef bsl::vector<double> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bsl::string>     TT;
            typedef bsl::vector<bsl::string> PT;

            const int ET = bdlmxxx::ElemType::BDEM_STRING_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlt::Date>     TT;
            typedef bsl::vector<bdlt::Date> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATE_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlt::Datetime>     TT;
            typedef bsl::vector<bdlt::Datetime> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETIME_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlt::Time>     TT;
            typedef bsl::vector<bdlt::Time> PT;

            const int ET = bdlmxxx::ElemType::BDEM_TIME_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<my_Class> TT;
            typedef bdlmxxx::Table        PT;

            const int ET = bdlmxxx::ElemType::BDEM_TABLE;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<my_Enum> TT;
            typedef bsl::vector<int> PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<const unsigned int> TT;
            typedef bsl::vector<int>            PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<const bsls::Types::Int64> TT;
            typedef bsl::vector<bsls::Types::Int64>   PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlt::DatetimeTz TT;
            typedef bdlt::DatetimeTz PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETIMETZ;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlt::DateTz TT;
            typedef bdlt::DateTz PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETZ;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlt::TimeTz TT;
            typedef bdlt::TimeTz PT;

            const int ET = bdlmxxx::ElemType::BDEM_TIMETZ;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bool> TT;
            typedef bsl::vector<bool> PT;

            const int ET = bdlmxxx::ElemType::BDEM_BOOL_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bool>     TT;
            typedef bsl::vector<bool> PT;

            const int ET = bdlmxxx::ElemType::BDEM_BOOL_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdlt::DatetimeTz> TT;
            typedef bsl::vector<bdlt::DatetimeTz> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlt::DatetimeTz> TT;
            typedef bsl::vector<bdlt::DatetimeTz> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdlt::DateTz> TT;
            typedef bsl::vector<bdlt::DateTz> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETZ_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlt::DateTz> TT;
            typedef bsl::vector<bdlt::DateTz> PT;

            const int ET = bdlmxxx::ElemType::BDEM_DATETZ_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdlt::TimeTz> TT;
            typedef bsl::vector<bdlt::TimeTz> PT;

            const int ET = bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlt::TimeTz> TT;
            typedef bsl::vector<bdlt::TimeTz> PT;

            const int ET = bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlmxxx::Choice TT;
            typedef bdlmxxx::Choice PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHOICE;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlmxxx::Choice>   TT;
            typedef bdlmxxx::ChoiceArray       PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHOICE_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlmxxx::Row TT;
            typedef bdlmxxx::Row PT;

            const int ET = bdlmxxx::ElemType::BDEM_ROW;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlmxxx::Row> TT;
            typedef bdlmxxx::Table        PT;

            const int ET = bdlmxxx::ElemType::BDEM_TABLE;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdlmxxx::ChoiceArrayItem TT;
            typedef bdlmxxx::ChoiceArrayItem PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdlmxxx::ChoiceArrayItem> TT;
            typedef bdlmxxx::ChoiceArray              PT;

            const int ET = bdlmxxx::ElemType::BDEM_CHOICE_ARRAY;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<unsigned int> TT;
            typedef int                            PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            LOOP_ASSERT(bdlmxxx::SelectBdemType<TT>::VALUE,
                        ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<bsls::Types::Int64> TT;
            typedef bsls::Types::Int64                   PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<const unsigned int> TT;
            typedef int                                  PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<const bsls::Types::Int64> TT;
            typedef bsls::Types::Int64                         PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<unsigned int> TT;
            typedef int                           PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<bsls::Types::Int64> TT;
            typedef bsls::Types::Int64                  PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<const unsigned int> TT;
            typedef int                                 PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<const bsls::Types::Int64> TT;
            typedef bsls::Types::Int64                        PT;

            const int ET = bdlmxxx::ElemType::BDEM_INT64;

            ASSERT(ET == bdlmxxx::SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf::IsSame<PT,
                                       bdlmxxx::SelectBdemType<TT>::Type>::VALUE));
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
