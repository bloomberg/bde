// bslmf_nthparameter.t.cpp                  -*-C++-*-

#include "bslmf_nthparameter.h"

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
   class my_tuple_element;  // Not defined
  
   template <std::size_t I, class... ELEMS>
   class my_tuple_element<I, my_tuple<ELEMS...> > {
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
   class my_tuple_element;
  
   template <std::size_t I>
   class my_tuple_element<I, my_tuple<> > {
       typedef typename bslmf::NthParameter<I>::Type Type;
   };

   template <std::size_t I, class ELEMS_01>
   class my_tuple_element<I, my_tuple<ELEMS_01> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02>
   class my_tuple_element<I, my_tuple<ELEMS_01,
                                      ELEMS_02> > {
       typedef typename bslmf::NthParameter<I, ELEMS_01,
                                               ELEMS_02>::Type Type;
   };

   template <std::size_t I, class ELEMS_01,
                            class ELEMS_02,
                            class ELEMS_03>
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   class my_tuple_element<I, my_tuple<ELEMS_01,
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
   }
#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
   template <class... ELEMS>
   class my_tuple {
   };
   #include <bslmf_nthparameter.h>
  
   template <std::size_t I, class TUPLE>
   class my_tuple_element;
  
   template <std::size_t I, class... ELEMS>
   class my_tuple_element<I, my_tuple<ELEMS...> > {
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
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
	//
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n==============\n");

        usageExample1();

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
