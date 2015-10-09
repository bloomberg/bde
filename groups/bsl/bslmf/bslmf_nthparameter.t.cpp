// bslmf_nthparameter.t.cpp                  -*-C++-*-

#include "bslmf_nthparameter.h"
#include <bslmf_integralconstant.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%hd", val); fflush(stdout); }
inline void dbg_print(unsigned short val) {printf("%hu", val); fflush(stdout);}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%lu", val); fflush(stdout); }
inline void dbg_print(unsigned long val) { printf("%lu", val); fflush(stdout);}
// inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
// inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

template <int V>
inline
int integerConstTypeToInt(bsl::integral_constant<int, V>) { return V; }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

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
   #include <bslmf_nthparameter.h>

   template <std::size_t I, class TUPLE>
   struct my_tuple_element;  // Not defined

   template <std::size_t I, class... ELEMS>
   struct my_tuple_element<I, my_tuple<ELEMS...> > {
       typedef typename bslmf::NthParameter<I, ELEMS...>::Type Type;
   };
//..
// Finally, we test this implementation using 'bsl::is_same':
//..
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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns: Usage example compiles and runs succesfully
        //
        // Plan: Copy usage example from component header literally into test
        //       driver.
	//
        // Testing: Usage example
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n==============\n");

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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
