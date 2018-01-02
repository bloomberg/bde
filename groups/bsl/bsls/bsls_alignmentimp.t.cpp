// bsls_alignmentimp.t.cpp                                            -*-C++-*-

#include <bsls_alignmentimp.h>

#include <bsls_platform.h>

#include <cstddef>     // offsetof() macro
#include <cstdlib>     // atoi()
#include <cstring>
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// Most of what this component implements are compile-time computations that
// differ among platforms.  The tests do assume that alignment of 'char' is 1,
// 'short' is 2, 'int' is 4, and 'double' is at least 4.  In addition, it is
// tested that all alignment-to-type calculations result are reversible, so
// that the alignment of the resulting type equals the original input.
//
// For the few run-time functions provided in this component, we establish
// post-conditions and test that the postconditions hold over a reasonable
// range of inputs.
//-----------------------------------------------------------------------------
// [ 1] bsls::AlignmentImpTag<SIZE>
// [ 1] bsls::AlignmentImpCalc<TYPE>
// [ 1] bsls::AlignmentImpPriorityToType<PRIORITY>
// [ 2] Usage Example 1
// [ 3] Usage Example 2
//-----------------------------------------------------------------------------
//=============================================================================

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

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

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define A(X) cout << #X " = " << ((void *) X) << endl;  // Print address
#define A_(X) cout << #X " = " << ((void *) X) << ", " << flush;
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                  GLOBAL DEFINITIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES AND FUNCTIONS USED IN TESTS
//-----------------------------------------------------------------------------

                                // -------
                                // Usage 2
                                // -------

///Example 2: Types Supporting 'AlignmentToType'
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we to be able to determine a fundamental or pointer type that has
// both its size and alignment requirement equal to the alignment requirement
// of a specified template parameter type.  We can utilize the
// 'AlignmentImpTag' 'struct' template, the overloads of
// 'AlignmentImpMatch::match' class method, the 'AlignmentImp_Priority'
// template class, and the 'AlignmentImpPrioriityToType' template class to do
// this calculation.

// First, we define a class template, 'ConvertAlignmentToType', that provides a
// 'Type' alias to a fundamental or pointer type that has both its alignment
// requirement and size equal to the 'ALIGNMENT' int parameter of the template.

template <int ALIGNMENT>
struct ConvertAlignmentToType {
    // This 'struct' provides a 'typedef', 'Type', that aliases a primitive
    // type having the specified 'ALIGNMENT' requirement and size.

  private:
    // PRIVATE TYPES
    typedef typename bsls::AlignmentImpMatch::MaxPriority MaxPriority;
        // 'MaxPriority' is a typedef to the 'AlignmentImp_Priority' template
        // class with the highest permissible value.

    typedef          bsls::AlignmentImpTag<ALIGNMENT>     Tag;
        // 'Tag' provides a typedef to the 'AlignmentImpTag' class configured
        // with this 'struct's 'ALIGNMENT' parameter.

    enum {
        // Compute the priority of the primitive type corresponding to the
        // specified 'ALIGNMENT'.  Many 'match' functions are declared, and
        // at least one whose alignment and size fields are identical and equal
        // to 'ALIGNMENT'.  Of those who match, the first match will be the one
        // with the highest priority 'AlignmentImp_Priority' arg.

        PRIORITY = sizeof(bsls::AlignmentImpMatch::match(Tag(),
                                                         Tag(),
                                                         MaxPriority()))
    };

  public:
    // TYPES
    typedef typename bsls::AlignmentImpPriorityToType<PRIORITY>::Type Type;
        // Convert the 'PRIORITY' value we calculated back to a type which has
        // the value 'ALIGNMENT' for both its alignment and it's size.
};

// Then, we define two user defined types that we will use
// 'ConvertAlignmentToType' on:

struct MyStructA {
    short  d_s;
    double d_d;
    int    d_i;
};

struct MyStructB {
    double d_d[20];
};

                                // -------
                                // Usage 1
                                // -------

///Example 1: AlignmentImpCalc Template
/// - - - - - - - - - - - - - - - - - -
// Suppose that we want to write a program that needs to calculate the
// alignment requirements of both user-defined types and built-in types.
// Further suppose that the program will run on a platform where the alignment
// requirement of 'int' is 4 bytes.

// First, we define a 'struct', 'MyStruct', for which want to determine the
// alignment requirement:

struct MyStruct {
    char  d_c;
    int   d_i;
    short d_s;
};

// Note that 'int' is the most alignment-demanding type within 'MyStruct'.

//-----------------------------------------------------------------------------

struct S1 { char d_buff[8]; S1(char); };                            // IMPLICIT
struct S2 { char d_buff[8]; int d_int; S2(); private: S2(const S2&); };
struct S3 { S1 d_s1; double d_double; short d_short; };
struct S4 { short d_shorts[5]; char d_c;  S4(int); private: S4(const S4&); };
#if (defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)  \
                                     || defined(BSLS_PLATFORM_OS_CYGWIN)  \
                                     || defined(BSLS_PLATFORM_OS_SOLARIS) \
    ) \
 && defined(BSLS_PLATFORM_CPU_X86)
struct S5 { long long d_longLong __attribute__((__aligned__(8))); };
#endif
union  U1 { char d_c; int *d_pointer; };

template <class T>
inline
bool samePtrType(T *, void *)
{
    return false;
}

template <class T>
inline
bool samePtrType(T *, T *)
{
    return true;
}

template <class T1, class T2>
inline
bool sameType(T1 t1, T2 t2)
{
    return samePtrType(&t1, &t2);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        // We will use the facilities in this component to evaluate a few
        // types: 'int', and 'MyStructA' & 'MyStructB' defined above.
        //
        // Next, we calculate alignments for our 3 types with
        // 'AlignmentImpCalc'.

        const int INT_ALIGNMENT = bsls::AlignmentImpCalc<int      >::VALUE;
        const int A_ALIGNMENT   = bsls::AlignmentImpCalc<MyStructA>::VALUE;
        const int B_ALIGNMENT   = bsls::AlignmentImpCalc<MyStructB>::VALUE;

        // Now, for each alignment requirement we just calculated, we utilize
        // 'ConvertAlignmentToType' to determine the fundamental or pointer
        // type having both size and alignment requirement equal to the
        // calculated alignment requirement.

        typedef ConvertAlignmentToType<INT_ALIGNMENT>::Type IntAlignType;
        typedef ConvertAlignmentToType<A_ALIGNMENT  >::Type ThisAlignType;
        typedef ConvertAlignmentToType<B_ALIGNMENT  >::Type ThatAlignType;

        // Finally, we observe that the alignments of the '*AlignType's are the
        // same as the alignments of the types they are derived from, and that
        // all the type determined by 'ConvertAlignmentToType' have sizes
        // equal to their alignment requirements:

        ASSERT(INT_ALIGNMENT == bsls::AlignmentImpCalc<IntAlignType >::VALUE);
        ASSERT(A_ALIGNMENT   == bsls::AlignmentImpCalc<ThisAlignType>::VALUE);
        ASSERT(B_ALIGNMENT   == bsls::AlignmentImpCalc<ThatAlignType>::VALUE);

        ASSERT(INT_ALIGNMENT == sizeof(IntAlignType));
        ASSERT(A_ALIGNMENT   == sizeof(ThisAlignType));
        ASSERT(B_ALIGNMENT   == sizeof(ThatAlignType));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << "Usage Example 1\n"
                             "===============\n";

        // Now, we use 'AlignmentImpCalc' to calculate the alignments of two
        // types, 'short' and the 'MyStruct' we just defined:

        enum {
            SHORT_ALIGNMENT     = bsls::AlignmentImpCalc<short   >::VALUE,
            MY_STRUCT_ALIGNMENT = bsls::AlignmentImpCalc<MyStruct>::VALUE };

        // Finally, we observe the values of our alignments, we observe that
        // the size of the 2 objects is a multiple of each object's alignment
        // (which is true for all C++ types), and we observe that the size of
        // 'MyStruct' is greater than its alignment.

        ASSERT(2 == SHORT_ALIGNMENT);
        ASSERT(4 == MY_STRUCT_ALIGNMENT);

        ASSERT(0 == sizeof(short   ) % SHORT_ALIGNMENT);
        ASSERT(0 == sizeof(MyStruct) % MY_STRUCT_ALIGNMENT);

        ASSERT(sizeof(MyStruct) > MY_STRUCT_ALIGNMENT);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING VARIOUS META-FUNCTIONS
        //
        // PLAN
        //   1) Verify that 'SIZE == sizeof(bsls::AlignmentImpTag<SIZE>)'.
        //   2) Verify that the 'bsls::AlignmentImpCalc<TYPE>::VALUE returns
        //      the correct alignment for various values of 'TYPE'.
        //   3) Verify that 'bsls::AlignmentImpPriorityToType<PRIORITY>::Type'
        //      returns the correct type for various values of PRIORITY.
        //
        // TACTICS
        //   Ad-hoc data selection
        //
        // TESTING
        //   bsls::AlignmentImpTag<SIZE>
        //   bsls::AlignmentImpCalc<TYPE>::Type
        //   bsls::AlignmentImpPriorityToType<PRIORITY>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTest bsls::AlignmentImpTag<SIZE>"
                          << "\n================================" << endl;
        {
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<1>),
                        1  == sizeof(bsls::AlignmentImpTag<1>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<2>),
                        2  == sizeof(bsls::AlignmentImpTag<2>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<3>),
                        3  == sizeof(bsls::AlignmentImpTag<3>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<4>),
                        4  == sizeof(bsls::AlignmentImpTag<4>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<5>),
                        5  == sizeof(bsls::AlignmentImpTag<5>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<6>),
                        6  == sizeof(bsls::AlignmentImpTag<6>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<7>),
                        7  == sizeof(bsls::AlignmentImpTag<7>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<8>),
                        8  == sizeof(bsls::AlignmentImpTag<8>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<9>),
                        9  == sizeof(bsls::AlignmentImpTag<9>));
            LOOP_ASSERT(sizeof(bsls::AlignmentImpTag<10>),
                        10 == sizeof(bsls::AlignmentImpTag<10>));
        }

        typedef void (*FuncPtr)();

        enum {
            CHAR_ALIGNMENT        = bsls::AlignmentImpCalc<char>::VALUE,
            SHORT_ALIGNMENT       = bsls::AlignmentImpCalc<short>::VALUE,
            INT_ALIGNMENT         = bsls::AlignmentImpCalc<int>::VALUE,
            LONG_ALIGNMENT        = bsls::AlignmentImpCalc<long>::VALUE,
            INT64_ALIGNMENT       = bsls::AlignmentImpCalc<long long>::VALUE,
            BOOL_ALIGNMENT        = bsls::AlignmentImpCalc<bool>::VALUE,
            WCHAR_T_ALIGNMENT     = bsls::AlignmentImpCalc<wchar_t>::VALUE,
            PTR_ALIGNMENT         = bsls::AlignmentImpCalc<void*>::VALUE,
            FUNC_PTR_ALIGNMENT    = bsls::AlignmentImpCalc<FuncPtr>::VALUE,
            FLOAT_ALIGNMENT       = bsls::AlignmentImpCalc<float>::VALUE,
            DOUBLE_ALIGNMENT      = bsls::AlignmentImpCalc<double>::VALUE,
            LONG_DOUBLE_ALIGNMENT = bsls::AlignmentImpCalc<long double>::VALUE,

            S1_ALIGNMENT          = bsls::AlignmentImpCalc<S1>::VALUE,
            S2_ALIGNMENT          = bsls::AlignmentImpCalc<S2>::VALUE,
            S3_ALIGNMENT          = bsls::AlignmentImpCalc<S3>::VALUE,
            S4_ALIGNMENT          = bsls::AlignmentImpCalc<S4>::VALUE,
#if (defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)  \
                                     || defined(BSLS_PLATFORM_OS_CYGWIN)  \
                                     || defined(BSLS_PLATFORM_OS_SOLARIS) \
    ) \
 && defined(BSLS_PLATFORM_CPU_X86)
            S5_ALIGNMENT          = bsls::AlignmentImpCalc<S5>::VALUE,
#endif
            U1_ALIGNMENT          = bsls::AlignmentImpCalc<U1>::VALUE
        };

        if (verbose) cout << "\nTest bsls::AlignmentImpCalc<TYPE>"
                          << "\n=================================" << endl;
        {
            int EXP_CHAR_ALIGNMENT        = 1;
            int EXP_BOOL_ALIGNMENT        = 1;
            int EXP_SHORT_ALIGNMENT       = 2;
            int EXP_WCHAR_T_ALIGNMENT     = 4;
            int EXP_INT_ALIGNMENT         = 4;
            int EXP_LONG_ALIGNMENT        = 4;
            int EXP_INT64_ALIGNMENT       = 8;
            int EXP_PTR_ALIGNMENT         = 4;
            int EXP_FUNC_PTR_ALIGNMENT    = 4;
            int EXP_FLOAT_ALIGNMENT       = 4;
            int EXP_DOUBLE_ALIGNMENT      = 8;
            int EXP_LONG_DOUBLE_ALIGNMENT = 8;

            int EXP_S1_ALIGNMENT          = 1;
            int EXP_S2_ALIGNMENT          = 4;
            int EXP_S3_ALIGNMENT          = 8;
            int EXP_S4_ALIGNMENT          = 2;
            int EXP_S5_ALIGNMENT          = 8;
            (void) EXP_S5_ALIGNMENT;
            int EXP_U1_ALIGNMENT          = 4;

// Specializations for different architectures
#if    (defined(BSLS_PLATFORM_OS_LINUX)    \
     || defined(BSLS_PLATFORM_OS_DARWIN)   \
     || defined(BSLS_PLATFORM_OS_SOLARIS)  \
        ) \
 && defined(BSLS_PLATFORM_CPU_X86)
            EXP_INT64_ALIGNMENT           = 4;
            EXP_DOUBLE_ALIGNMENT          = 4;
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
            EXP_LONG_DOUBLE_ALIGNMENT     = 4;
#else
            EXP_LONG_DOUBLE_ALIGNMENT     = 16;
#endif

            EXP_S3_ALIGNMENT              = 4;
            LOOP2_ASSERT(S5_ALIGNMENT, EXP_S5_ALIGNMENT,
                         EXP_S5_ALIGNMENT == S5_ALIGNMENT);
#endif

#if defined(BSLS_PLATFORM_CPU_64_BIT)
            EXP_LONG_ALIGNMENT            = 8;
            EXP_PTR_ALIGNMENT             = 8;
            EXP_FUNC_PTR_ALIGNMENT        = 8;
            EXP_U1_ALIGNMENT              = 8;
#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_OS_LINUX)
            EXP_LONG_DOUBLE_ALIGNMENT     = 8;
#else
            EXP_LONG_DOUBLE_ALIGNMENT     = 16;
#endif
#endif

#if defined(BSLS_PLATFORM_OS_AIX)
    #if !defined(BSLS_PLATFORM_CPU_64_BIT)
            EXP_WCHAR_T_ALIGNMENT         = 2;
    #endif
            EXP_DOUBLE_ALIGNMENT          = 4;
            EXP_LONG_DOUBLE_ALIGNMENT     = 4;
            EXP_S3_ALIGNMENT              = 4;
#endif

#if defined(BSLS_PLATFORM_OS_CYGWIN)
            EXP_WCHAR_T_ALIGNMENT         = 2;
            EXP_LONG_DOUBLE_ALIGNMENT     = 4;
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
            EXP_WCHAR_T_ALIGNMENT         = 2;
    #if defined(BSLS_PLATFORM_CPU_64_BIT)
            EXP_LONG_ALIGNMENT            = 4;
            EXP_LONG_DOUBLE_ALIGNMENT     = 8;
    #endif
#endif

            LOOP2_ASSERT(CHAR_ALIGNMENT, EXP_CHAR_ALIGNMENT,
                         EXP_CHAR_ALIGNMENT == CHAR_ALIGNMENT);
            LOOP2_ASSERT(BOOL_ALIGNMENT, EXP_BOOL_ALIGNMENT,
                         EXP_BOOL_ALIGNMENT == BOOL_ALIGNMENT);
            LOOP2_ASSERT(SHORT_ALIGNMENT, EXP_SHORT_ALIGNMENT,
                         EXP_SHORT_ALIGNMENT == SHORT_ALIGNMENT);
            LOOP2_ASSERT(WCHAR_T_ALIGNMENT, EXP_WCHAR_T_ALIGNMENT,
                         EXP_WCHAR_T_ALIGNMENT == WCHAR_T_ALIGNMENT);
            LOOP2_ASSERT(INT_ALIGNMENT, EXP_INT_ALIGNMENT,
                         EXP_INT_ALIGNMENT == INT_ALIGNMENT);
            LOOP2_ASSERT(LONG_ALIGNMENT, EXP_LONG_ALIGNMENT,
                         EXP_LONG_ALIGNMENT == LONG_ALIGNMENT);
            LOOP2_ASSERT(INT64_ALIGNMENT, EXP_INT64_ALIGNMENT,
                         EXP_INT64_ALIGNMENT == INT64_ALIGNMENT);
            LOOP2_ASSERT(PTR_ALIGNMENT, EXP_PTR_ALIGNMENT,
                         EXP_PTR_ALIGNMENT == PTR_ALIGNMENT);
            LOOP2_ASSERT(FUNC_PTR_ALIGNMENT, EXP_FUNC_PTR_ALIGNMENT,
                         EXP_FUNC_PTR_ALIGNMENT == FUNC_PTR_ALIGNMENT);
            LOOP2_ASSERT(FLOAT_ALIGNMENT, EXP_FLOAT_ALIGNMENT,
                         EXP_FLOAT_ALIGNMENT == FLOAT_ALIGNMENT);
            LOOP2_ASSERT(DOUBLE_ALIGNMENT, EXP_DOUBLE_ALIGNMENT,
                         EXP_DOUBLE_ALIGNMENT == DOUBLE_ALIGNMENT);
            LOOP2_ASSERT(LONG_DOUBLE_ALIGNMENT, EXP_LONG_DOUBLE_ALIGNMENT,
                         EXP_LONG_DOUBLE_ALIGNMENT == LONG_DOUBLE_ALIGNMENT);

            LOOP2_ASSERT(S1_ALIGNMENT, EXP_S1_ALIGNMENT,
                         EXP_S1_ALIGNMENT == S1_ALIGNMENT);
            LOOP2_ASSERT(S2_ALIGNMENT, EXP_S2_ALIGNMENT,
                         EXP_S2_ALIGNMENT == S2_ALIGNMENT);
            LOOP2_ASSERT(S3_ALIGNMENT, EXP_S3_ALIGNMENT,
                         EXP_S3_ALIGNMENT == S3_ALIGNMENT);
            LOOP2_ASSERT(S4_ALIGNMENT, EXP_S4_ALIGNMENT,
                         EXP_S4_ALIGNMENT == S4_ALIGNMENT);
            LOOP2_ASSERT(U1_ALIGNMENT, EXP_U1_ALIGNMENT,
                         EXP_U1_ALIGNMENT == U1_ALIGNMENT);
        }

        // Test sameType function.
        ASSERT(sameType(int(), int()));
        ASSERT(!sameType(int(), short()));

        if (verbose)
            cout << "\nTest bsls::AlignmentImpPriorityToType<PRIORITY>"
                 << "\n==============================================="
                 << endl;
        {

            long double  LD = 0.0;
            void        *V  = 0;
            long long    LL = 0;

            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<1>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<1>::Type(),
                                 LD));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<2>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<2>::Type(),
                                 double()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<3>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<3>::Type(),
                                 float()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<4>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<4>::Type(),
                                 FuncPtr()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<5>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<5>::Type(),
                                 V));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<6>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<6>::Type(),
                                 wchar_t()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<7>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<7>::Type(),
                                 bool()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<8>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<8>::Type(),
                                 LL));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<9>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<9>::Type(),
                                 long()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<10>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<10>::Type(),
                                 int()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<11>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<11>::Type(),
                                 short()));
            LOOP_ASSERT(bsls::AlignmentImpPriorityToType<12>::Type(),
                        sameType(bsls::AlignmentImpPriorityToType<12>::Type(),
                                 char()));
#if (defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)) \
  && defined(BSLS_PLATFORM_CPU_X86)
            ASSERT(sameType(bsls::AlignmentImpPriorityToType<13>::Type(),
                            bsls::AlignmentImp8ByteAlignedType()));
#endif

        }
      } break;
      default: {
        cerr << "WARNING: CASE `"<< test << "' NOT FOUND." <<endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
