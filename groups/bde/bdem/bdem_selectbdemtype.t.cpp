// bdem_selectbdemtype.t.cpp                                          -*-C++-*-

#include <bdem_selectbdemtype.h>

#include <bdem_elemtype.h>

#include <bdet_date.h>            // for testing only
#include <bdet_datetime.h>        // for testing only
#include <bdet_time.h>            // for testing only

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
// [ 1] struct bdem_SelectBdemType
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
// Note that the 'bslmf_IsSame' meta-function is employed only for the
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
    ASSERT(bdem_ElemType::BDEM_BOOL == (int)bdem_SelectBdemType<bool>::VALUE);
    typedef bdem_SelectBdemType<bool>::Type boolProxyType;
    ASSERT(1 == (bslmf_IsSame<bool, boolProxyType>::VALUE));
//
//  // 'unsigned' -> 'int'
    ASSERT(bdem_ElemType::BDEM_INT ==
                                    (int)bdem_SelectBdemType<unsigned>::VALUE);
    typedef bdem_SelectBdemType<unsigned>::Type unsignedProxyType;
    ASSERT(1 == (bslmf_IsSame<int, unsignedProxyType>::VALUE));
//
//  // 'Enum' -> 'int'
    ASSERT(bdem_ElemType::BDEM_INT == (int)bdem_SelectBdemType<Enum>::VALUE);
    typedef bdem_SelectBdemType<Enum>::Type enumProxyType;
    ASSERT(1 == (bslmf_IsSame<int, enumProxyType>::VALUE));
//
//  // 'bsl::vector<Enum>' -> 'bsl::vector<int>'
    ASSERT(bdem_ElemType::BDEM_INT_ARRAY ==
                          (int)bdem_SelectBdemType<bsl::vector<Enum> >::VALUE);
    typedef bdem_SelectBdemType<bsl::vector<Enum> >::Type enumVectorProxyType;
    ASSERT(1 == (bslmf_IsSame<bsl::vector<int>, enumVectorProxyType>::VALUE));
//
//  // 'Class' -> 'bdem_List'
    ASSERT(bdem_ElemType::BDEM_LIST == (int)bdem_SelectBdemType<Class>::VALUE);
    typedef bdem_SelectBdemType<Class>::Type classProxyType;
    ASSERT(1 == (bslmf_IsSame<bdem_List, classProxyType>::VALUE));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING bdem_SelectBdemType
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct bdem_SelectBdemType
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'bdem_SelectBdemType'" << endl
                          << "=============================" << endl;

        enum {
            IS_ARRAY =
                   bdeat_ArrayFunctions::IsArray<MyCustomizedType<int> >::VALUE
        };

        char c[IS_ARRAY ? -1 : 1];

        {
            typedef bool TT;  // test type
            typedef bool PT;  // proxy type

            const int ET = bdem_ElemType::BDEM_BOOL;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef char TT;
            typedef char PT;

            const int ET = bdem_ElemType::BDEM_CHAR;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef signed char TT;
            typedef char        PT;

            const int ET = bdem_ElemType::BDEM_CHAR;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef unsigned char TT;
            typedef char          PT;

            const int ET = bdem_ElemType::BDEM_CHAR;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef short TT;
            typedef short PT;

            const int ET = bdem_ElemType::BDEM_SHORT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef unsigned short TT;
            typedef short          PT;

            const int ET = bdem_ElemType::BDEM_SHORT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef wchar_t TT;
            typedef short   PT;

            const int ET = bdem_ElemType::BDEM_SHORT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef int TT;
            typedef int PT;

            const int ET = bdem_ElemType::BDEM_INT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef unsigned int TT;
            typedef int          PT;

            const int ET = bdem_ElemType::BDEM_INT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsls_Types::Int64 TT;
            typedef bsls_Types::Int64 PT;

            const int ET = bdem_ElemType::BDEM_INT64;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsls_Types::Uint64 TT;
            typedef bsls_Types::Int64  PT;

            const int ET = bdem_ElemType::BDEM_INT64;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef float TT;
            typedef float PT;

            const int ET = bdem_ElemType::BDEM_FLOAT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef double TT;
            typedef double PT;

            const int ET = bdem_ElemType::BDEM_DOUBLE;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::string TT;
            typedef bsl::string PT;

            const int ET = bdem_ElemType::BDEM_STRING;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdet_Date TT;
            typedef bdet_Date PT;

            const int ET = bdem_ElemType::BDEM_DATE;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdet_Datetime TT;
            typedef bdet_Datetime PT;

            const int ET = bdem_ElemType::BDEM_DATETIME;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdet_Time TT;
            typedef bdet_Time PT;

            const int ET = bdem_ElemType::BDEM_TIME;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<char> TT;
            typedef bsl::vector<char> PT;

            const int ET = bdem_ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<signed char> TT;
            typedef bsl::vector<char>        PT;

            const int ET = bdem_ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<unsigned char> TT;
            typedef bsl::vector<char>          PT;

            const int ET = bdem_ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<short> TT;
            typedef bsl::vector<short> PT;

            const int ET = bdem_ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<unsigned short> TT;
            typedef bsl::vector<short>          PT;

            const int ET = bdem_ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<wchar_t> TT;
            typedef bsl::vector<short>   PT;

            const int ET = bdem_ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<int> TT;
            typedef bsl::vector<int> PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<unsigned int> TT;
            typedef bsl::vector<int>          PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bsls_Types::Int64> TT;
            typedef bsl::vector<bsls_Types::Int64> PT;

            const int ET = bdem_ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bsls_Types::Uint64> TT;
            typedef bsl::vector<bsls_Types::Int64>  PT;

            const int ET = bdem_ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<float> TT;
            typedef bsl::vector<float> PT;

            const int ET = bdem_ElemType::BDEM_FLOAT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<double> TT;
            typedef bsl::vector<double> PT;

            const int ET = bdem_ElemType::BDEM_DOUBLE_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bsl::string> TT;
            typedef bsl::vector<bsl::string> PT;

            const int ET = bdem_ElemType::BDEM_STRING_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdet_Date> TT;
            typedef bsl::vector<bdet_Date> PT;

            const int ET = bdem_ElemType::BDEM_DATE_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdet_Datetime> TT;
            typedef bsl::vector<bdet_Datetime> PT;

            const int ET = bdem_ElemType::BDEM_DATETIME_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdet_Time> TT;
            typedef bsl::vector<bdet_Time> PT;

            const int ET = bdem_ElemType::BDEM_TIME_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef my_Class  TT;
            typedef bdem_List PT;

            const int ET = bdem_ElemType::BDEM_LIST;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<my_Class> TT;
            typedef bdem_Table            PT;

            const int ET = bdem_ElemType::BDEM_TABLE;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef my_Enum TT;
            typedef int     PT;

            const int ET = bdem_ElemType::BDEM_INT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<my_Enum> TT;
            typedef bsl::vector<int>     PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<char>     TT;
            typedef bsl::vector<char> PT;

            const int ET = bdem_ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<signed char> TT;
            typedef bsl::vector<char>    PT;

            const int ET = bdem_ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<unsigned char> TT;
            typedef bsl::vector<char>      PT;

            const int ET = bdem_ElemType::BDEM_CHAR_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<short>     TT;
            typedef bsl::vector<short> PT;

            const int ET = bdem_ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<unsigned short> TT;
            typedef bsl::vector<short>      PT;

            const int ET = bdem_ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<wchar_t>   TT;
            typedef bsl::vector<short> PT;

            const int ET = bdem_ElemType::BDEM_SHORT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<int>     TT;
            typedef bsl::vector<int> PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<unsigned int> TT;
            typedef bsl::vector<int>      PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bsls_Types::Int64>     TT;
            typedef bsl::vector<bsls_Types::Int64> PT;

            const int ET = bdem_ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bsls_Types::Uint64>    TT;
            typedef bsl::vector<bsls_Types::Int64> PT;

            const int ET = bdem_ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<float>     TT;
            typedef bsl::vector<float> PT;

            const int ET = bdem_ElemType::BDEM_FLOAT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<double>     TT;
            typedef bsl::vector<double> PT;

            const int ET = bdem_ElemType::BDEM_DOUBLE_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bsl::string>     TT;
            typedef bsl::vector<bsl::string> PT;

            const int ET = bdem_ElemType::BDEM_STRING_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdet_Date>     TT;
            typedef bsl::vector<bdet_Date> PT;

            const int ET = bdem_ElemType::BDEM_DATE_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdet_Datetime>     TT;
            typedef bsl::vector<bdet_Datetime> PT;

            const int ET = bdem_ElemType::BDEM_DATETIME_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdet_Time>     TT;
            typedef bsl::vector<bdet_Time> PT;

            const int ET = bdem_ElemType::BDEM_TIME_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<my_Class> TT;
            typedef bdem_Table        PT;

            const int ET = bdem_ElemType::BDEM_TABLE;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<my_Enum> TT;
            typedef bsl::vector<int> PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<const unsigned int> TT;
            typedef bsl::vector<int>            PT;

            const int ET = bdem_ElemType::BDEM_INT_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<const bsls_Types::Int64> TT;
            typedef bsl::vector<bsls_Types::Int64>   PT;

            const int ET = bdem_ElemType::BDEM_INT64_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdet_DatetimeTz TT;
            typedef bdet_DatetimeTz PT;

            const int ET = bdem_ElemType::BDEM_DATETIMETZ;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdet_DateTz TT;
            typedef bdet_DateTz PT;

            const int ET = bdem_ElemType::BDEM_DATETZ;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdet_TimeTz TT;
            typedef bdet_TimeTz PT;

            const int ET = bdem_ElemType::BDEM_TIMETZ;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bool> TT;
            typedef bsl::vector<bool> PT;

            const int ET = bdem_ElemType::BDEM_BOOL_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bool>     TT;
            typedef bsl::vector<bool> PT;

            const int ET = bdem_ElemType::BDEM_BOOL_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdet_DatetimeTz> TT;
            typedef bsl::vector<bdet_DatetimeTz> PT;

            const int ET = bdem_ElemType::BDEM_DATETIMETZ_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdet_DatetimeTz> TT;
            typedef bsl::vector<bdet_DatetimeTz> PT;

            const int ET = bdem_ElemType::BDEM_DATETIMETZ_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdet_DateTz> TT;
            typedef bsl::vector<bdet_DateTz> PT;

            const int ET = bdem_ElemType::BDEM_DATETZ_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdet_DateTz> TT;
            typedef bsl::vector<bdet_DateTz> PT;

            const int ET = bdem_ElemType::BDEM_DATETZ_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bsl::vector<bdet_TimeTz> TT;
            typedef bsl::vector<bdet_TimeTz> PT;

            const int ET = bdem_ElemType::BDEM_TIMETZ_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdet_TimeTz> TT;
            typedef bsl::vector<bdet_TimeTz> PT;

            const int ET = bdem_ElemType::BDEM_TIMETZ_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdem_Choice TT;
            typedef bdem_Choice PT;

            const int ET = bdem_ElemType::BDEM_CHOICE;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdem_Choice>   TT;
            typedef bdem_ChoiceArray       PT;

            const int ET = bdem_ElemType::BDEM_CHOICE_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdem_Row TT;
            typedef bdem_Row PT;

            const int ET = bdem_ElemType::BDEM_ROW;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdem_Row> TT;
            typedef bdem_Table        PT;

            const int ET = bdem_ElemType::BDEM_TABLE;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef bdem_ChoiceArrayItem TT;
            typedef bdem_ChoiceArrayItem PT;

            const int ET = bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyArray<bdem_ChoiceArrayItem> TT;
            typedef bdem_ChoiceArray              PT;

            const int ET = bdem_ElemType::BDEM_CHOICE_ARRAY;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<unsigned int> TT;
            typedef int                            PT;

            const int ET = bdem_ElemType::BDEM_INT;

            LOOP_ASSERT(bdem_SelectBdemType<TT>::VALUE,
                        ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<bsls_Types::Int64> TT;
            typedef bsls_Types::Int64                   PT;

            const int ET = bdem_ElemType::BDEM_INT64;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<const unsigned int> TT;
            typedef int                                  PT;

            const int ET = bdem_ElemType::BDEM_INT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType<const bsls_Types::Int64> TT;
            typedef bsls_Types::Int64                         PT;

            const int ET = bdem_ElemType::BDEM_INT64;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<unsigned int> TT;
            typedef int                           PT;

            const int ET = bdem_ElemType::BDEM_INT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<bsls_Types::Int64> TT;
            typedef bsls_Types::Int64                  PT;

            const int ET = bdem_ElemType::BDEM_INT64;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<const unsigned int> TT;
            typedef int                                 PT;

            const int ET = bdem_ElemType::BDEM_INT;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
        }

        {
            typedef MyNullableValue<const bsls_Types::Int64> TT;
            typedef bsls_Types::Int64                        PT;

            const int ET = bdem_ElemType::BDEM_INT64;

            ASSERT(ET == bdem_SelectBdemType<TT>::VALUE);
            ASSERT( 1 == (bslmf_IsSame<PT,
                                       bdem_SelectBdemType<TT>::Type>::VALUE));
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
