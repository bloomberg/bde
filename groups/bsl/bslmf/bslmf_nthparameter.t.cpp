// bslmf_nthparameter.t.cpp                                           -*-C++-*-

#include <bslmf_nthparameter.h>

#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <int V>
inline
int integerConstTypeToInt(bsl::integral_constant<int, V>) { return V; }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// We wish to implement a 'tuple'-like class that holds a heterogenous
// collection of elements, each of which might have a different type.  The
// metafunction, 'my_tuple_element<I, my_tuple<ELEMS...>>::Type' would be type
// of the 'I'th element in the tuple (where 'I' is zero-based).
//
// First, we define our 'my_tuple' class template.  The body of the class is
// unimportant for this usage examples:
//..
   template <class... ELEMS>
   class my_tuple {
       // ...
   };
//..
// Then, we use 'bslmf::NthParameter' to implement 'my_tuple_element':
//..
// #include <bslmf_nthparameter.h>

   template <std::size_t I, class TUPLE>
   struct my_tuple_element;  // Not defined

   template <std::size_t I, class... ELEMS>
   struct my_tuple_element<I, my_tuple<ELEMS...> > {
       typedef typename bslmf::NthParameter<I, ELEMS...>::Type Type;
   };
//..
// Finally, we test this implementation using 'bsl::is_same':
//..
// #include <bslmf_issame.h>

   int usageExample1()
   {
      typedef my_tuple<int, short, char*> ttype;

      ASSERT((bsl::is_same<int,    my_tuple_element<0, ttype>::Type>::value));
      ASSERT((bsl::is_same<short,  my_tuple_element<1, ttype>::Type>::value));
      ASSERT((bsl::is_same<char *, my_tuple_element<2, ttype>::Type>::value));

      ASSERT(! (bsl::is_same<short, my_tuple_element<0, ttype>::Type>::value));

      return 0;
   }
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslmf_nthparameter.t.cpp
template <class ELEMS_0 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_1 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_2 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_3 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_4 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_5 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_6 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_7 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_8 = BSLS_COMPILERFEATURES_NILT,
          class ELEMS_9 = BSLS_COMPILERFEATURES_NILT,
          class = BSLS_COMPILERFEATURES_NILT>
class my_tuple;

template <>
   class my_tuple<> {
   };

template <class ELEMS_01>
   class my_tuple<ELEMS_01> {
   };

template <class ELEMS_01,
          class ELEMS_02>
   class my_tuple<ELEMS_01,
                  ELEMS_02> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04,
          class ELEMS_05>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04,
                  ELEMS_05> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04,
          class ELEMS_05,
          class ELEMS_06>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04,
                  ELEMS_05,
                  ELEMS_06> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04,
          class ELEMS_05,
          class ELEMS_06,
          class ELEMS_07>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04,
                  ELEMS_05,
                  ELEMS_06,
                  ELEMS_07> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04,
          class ELEMS_05,
          class ELEMS_06,
          class ELEMS_07,
          class ELEMS_08>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04,
                  ELEMS_05,
                  ELEMS_06,
                  ELEMS_07,
                  ELEMS_08> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04,
          class ELEMS_05,
          class ELEMS_06,
          class ELEMS_07,
          class ELEMS_08,
          class ELEMS_09>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04,
                  ELEMS_05,
                  ELEMS_06,
                  ELEMS_07,
                  ELEMS_08,
                  ELEMS_09> {
   };

template <class ELEMS_01,
          class ELEMS_02,
          class ELEMS_03,
          class ELEMS_04,
          class ELEMS_05,
          class ELEMS_06,
          class ELEMS_07,
          class ELEMS_08,
          class ELEMS_09,
          class ELEMS_10>
   class my_tuple<ELEMS_01,
                  ELEMS_02,
                  ELEMS_03,
                  ELEMS_04,
                  ELEMS_05,
                  ELEMS_06,
                  ELEMS_07,
                  ELEMS_08,
                  ELEMS_09,
                  ELEMS_10> {
   };

   #include <bslmf_nthparameter.h>

   template <std::size_t I, class TUPLE>
   struct my_tuple_element;

   template <std::size_t I>
   struct my_tuple_element<I, my_tuple<> > {
       typedef typename bslmf::NthParameter<I>::Type Type;
   };

   template <std::size_t I, class ELEMS_01>
   struct my_tuple_element<I, my_tuple<ELEMS_01> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04,
                            class ELEMS_05>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04,
                                       ELEMS_05> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04,
                                               ELEMS_05>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04,
                            class ELEMS_05,
                            class ELEMS_06>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04,
                                       ELEMS_05,
                                       ELEMS_06> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04,
                                               ELEMS_05,
                                               ELEMS_06>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04,
                            class ELEMS_05,
                            class ELEMS_06,
                            class ELEMS_07>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04,
                                       ELEMS_05,
                                       ELEMS_06,
                                       ELEMS_07> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04,
                                               ELEMS_05,
                                               ELEMS_06,
                                               ELEMS_07>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04,
                            class ELEMS_05,
                            class ELEMS_06,
                            class ELEMS_07,
                            class ELEMS_08>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04,
                                       ELEMS_05,
                                       ELEMS_06,
                                       ELEMS_07,
                                       ELEMS_08> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04,
                                               ELEMS_05,
                                               ELEMS_06,
                                               ELEMS_07,
                                               ELEMS_08>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04,
                            class ELEMS_05,
                            class ELEMS_06,
                            class ELEMS_07,
                            class ELEMS_08,
                            class ELEMS_09>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04,
                                       ELEMS_05,
                                       ELEMS_06,
                                       ELEMS_07,
                                       ELEMS_08,
                                       ELEMS_09> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04,
                                               ELEMS_05,
                                               ELEMS_06,
                                               ELEMS_07,
                                               ELEMS_08,
                                               ELEMS_09>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03,
                            class ELEMS_04,
                            class ELEMS_05,
                            class ELEMS_06,
                            class ELEMS_07,
                            class ELEMS_08,
                            class ELEMS_09,
                            class ELEMS_10>
   struct my_tuple_element<I, my_tuple<ELEMS_01,
                                       ELEMS_02,
                                       ELEMS_03,
                                       ELEMS_04,
                                       ELEMS_05,
                                       ELEMS_06,
                                       ELEMS_07,
                                       ELEMS_08,
                                       ELEMS_09,
                                       ELEMS_10> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02,
                                               ELEMS_03,
                                               ELEMS_04,
                                               ELEMS_05,
                                               ELEMS_06,
                                               ELEMS_07,
                                               ELEMS_08,
                                               ELEMS_09,
                                               ELEMS_10>::Type Type;
   };

   #include <bslmf_issame.h>

   int usageExample1()
   {
      typedef my_tuple<int, short, char*> ttype;

      ASSERT((bsl::is_same<int,    my_tuple_element<0, ttype>::Type>::value));
      ASSERT((bsl::is_same<short,  my_tuple_element<1, ttype>::Type>::value));
      ASSERT((bsl::is_same<char *, my_tuple_element<2, ttype>::Type>::value));

      ASSERT(! (bsl::is_same<short, my_tuple_element<0, ttype>::Type>::value));

      return 0;
   }
#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
   template <class... ELEMS>
   class my_tuple {
   };
   #include <bslmf_nthparameter.h>

   template <std::size_t I, class TUPLE>
   struct my_tuple_element;

   template <std::size_t I, class... ELEMS>
   struct my_tuple_element<I, my_tuple<ELEMS...> > {
       typedef typename bslmf::NthParameter<I, ELEMS...>::Type Type;
   };
   #include <bslmf_issame.h>

   int usageExample1()
   {
      typedef my_tuple<int, short, char*> ttype;

      ASSERT((bsl::is_same<int,    my_tuple_element<0, ttype>::Type>::value));
      ASSERT((bsl::is_same<short,  my_tuple_element<1, ttype>::Type>::value));
      ASSERT((bsl::is_same<char *, my_tuple_element<2, ttype>::Type>::value));

      ASSERT(! (bsl::is_same<short, my_tuple_element<0, ttype>::Type>::value));

      return 0;
   }
// }}} END GENERATED CODE
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns: Usage example compiles and runs successfully
        //
        // Plan: Copy usage example from component header literally into test
        //       driver.
        //
        // Testing: Usage example
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns
        //  o 'bslmf::NthParameter' can handle from 1 to 10 type arguments.
        //  o 'bslmf::NthParameter' produces the correct 'Type' for 'N' in
        //    range 0 to 9.
        //
        // Plan:
        //  o Use 'integral_constant' to create 10 different types 'T0' to 'T9'
        //    as aliases for 'integral_constant<int, 0>' to
        //    'integral_constant<int, 9>', respectively.
        //  o Instantiate 'bslmf::NthParameter<0, T0>' and verify that the
        //    resulting 'Type' is 'T0'.
        //  o Repeat the test with an ever longer list of type arguments and
        //    with every valid value of 'N' until we've tested
        //    bslmf::NthParameter<9, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>'.
        //
        // Testing:
        //   bslmf::NthParameter<N, FIRST_PARAM, ...>::Type
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        typedef bsl::integral_constant<int, 0> T0;
        typedef bsl::integral_constant<int, 1> T1;
        typedef bsl::integral_constant<int, 2> T2;
        typedef bsl::integral_constant<int, 3> T3;
        typedef bsl::integral_constant<int, 4> T4;
        typedef bsl::integral_constant<int, 5> T5;
        typedef bsl::integral_constant<int, 6> T6;
        typedef bsl::integral_constant<int, 7> T7;
        typedef bsl::integral_constant<int, 8> T8;
        typedef bsl::integral_constant<int, 9> T9;

#define DO_TEST(N, ...) {                                               \
              typedef bslmf::NthParameter<N, __VA_ARGS__>::Type Result; \
              ASSERT(N == integerConstTypeToInt(Result()));             \
      }

          DO_TEST(0, T0);
          DO_TEST(0, T0, T1);
          DO_TEST(1, T0, T1);
          DO_TEST(0, T0, T1, T2);
          DO_TEST(1, T0, T1, T2);
          DO_TEST(2, T0, T1, T2);
          DO_TEST(0, T0, T1, T2, T3);
          DO_TEST(1, T0, T1, T2, T3);
          DO_TEST(2, T0, T1, T2, T3);
          DO_TEST(3, T0, T1, T2, T3);
          DO_TEST(0, T0, T1, T2, T3, T4);
          DO_TEST(1, T0, T1, T2, T3, T4);
          DO_TEST(2, T0, T1, T2, T3, T4);
          DO_TEST(3, T0, T1, T2, T3, T4);
          DO_TEST(4, T0, T1, T2, T3, T4);
          DO_TEST(0, T0, T1, T2, T3, T4, T5);
          DO_TEST(1, T0, T1, T2, T3, T4, T5);
          DO_TEST(2, T0, T1, T2, T3, T4, T5);
          DO_TEST(3, T0, T1, T2, T3, T4, T5);
          DO_TEST(4, T0, T1, T2, T3, T4, T5);
          DO_TEST(5, T0, T1, T2, T3, T4, T5);
          DO_TEST(0, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(1, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(2, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(3, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(4, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(5, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(6, T0, T1, T2, T3, T4, T5, T6);
          DO_TEST(0, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(1, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(2, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(3, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(4, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(5, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(6, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(7, T0, T1, T2, T3, T4, T5, T6, T7);
          DO_TEST(0, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(1, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(2, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(3, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(4, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(5, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(6, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(7, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(8, T0, T1, T2, T3, T4, T5, T6, T7, T8);
          DO_TEST(0, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(1, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(2, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(3, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(4, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(5, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(6, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(7, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(8, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);
          DO_TEST(9, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);

#undef DO_TEST
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: Exercise basic functionality of component
        //
        // Plan: Invoke 'NthParameter' metafunction with a few different
        //   combinations of parameters and confirm that it yields the
        //   expected 'Type' data member.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#define TEST_NTH_PARAMETER(N, T, ...) \
        ASSERT((bsl::is_same<T,       \
                            bslmf::NthParameter<N, __VA_ARGS__>::Type>::value))

        TEST_NTH_PARAMETER(0, int         , int, char*, void, const short&);
        TEST_NTH_PARAMETER(1, char*       , int, char*, void, const short&);
        TEST_NTH_PARAMETER(2, void        , int, char*, void, const short&);
        TEST_NTH_PARAMETER(3, const short&, int, char*, void, const short&);

        TEST_NTH_PARAMETER(0, double      , double);

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
