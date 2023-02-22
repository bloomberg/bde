// bslmf_makeintegersequence.t.cpp                                    -*-C++-*-
#include <bslmf_makeintegersequence.h>
#include <bslmf_integersequence.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <limits>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace bslmf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-functions that represent an integer
// sequence make-function 'bslmf::MakeIntegerSequence'.  Thus, we need to
// ensure that an integer sequence returned by the factory function represents
// enumerated collection of increasing integer values of the specified length
// starting with 0 value.
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 1] bslmf::MakeIntegerSequence<std::size_t N>
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE

namespace {

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

                              //=========================
                              // struct LargeSequenceUtil
                              //=========================

template <class T, bool EXCEED_LIMIT = std::numeric_limits<T>::max() >= 1023>
struct LargeSequenceUtil
    // This class template provides a namespace for utility operations on
    // integer sequences having large index numbers to test performance
    // characteristics and ensure that instantiation of this class for types
    // with 'numeric_limits<T>::max() >= 1023' does not hit internal recursion
    // limits.
{
    static void test()
        // Ensure that 'MakeIntegerSequence<T, 1023>' factory-function
        // generates correct integer sequence type.
    {
        using Obj = MakeIntegerSequence<T, 1023>;
        using SEQ_TYPE = IntegerSequence<T,
                       0,    1,    2,    3,    4,    5,    6,    7,    8,    9,
                      10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
                      20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
                      30,   31,   32,   33,   34,   35,   36,   37,   38,   39,
                      40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
                      50,   51,   52,   53,   54,   55,   56,   57,   58,   59,
                      60,   61,   62,   63,   64,   65,   66,   67,   68,   69,
                      70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
                      80,   81,   82,   83,   84,   85,   86,   87,   88,   89,
                      90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
                     100,  101,  102,  103,  104,  105,  106,  107,  108,  109,
                     110,  111,  112,  113,  114,  115,  116,  117,  118,  119,
                     120,  121,  122,  123,  124,  125,  126,  127,  128,  129,
                     130,  131,  132,  133,  134,  135,  136,  137,  138,  139,
                     140,  141,  142,  143,  144,  145,  146,  147,  148,  149,
                     150,  151,  152,  153,  154,  155,  156,  157,  158,  159,
                     160,  161,  162,  163,  164,  165,  166,  167,  168,  169,
                     170,  171,  172,  173,  174,  175,  176,  177,  178,  179,
                     180,  181,  182,  183,  184,  185,  186,  187,  188,  189,
                     190,  191,  192,  193,  194,  195,  196,  197,  198,  199,
                     200,  201,  202,  203,  204,  205,  206,  207,  208,  209,
                     210,  211,  212,  213,  214,  215,  216,  217,  218,  219,
                     220,  221,  222,  223,  224,  225,  226,  227,  228,  229,
                     230,  231,  232,  233,  234,  235,  236,  237,  238,  239,
                     240,  241,  242,  243,  244,  245,  246,  247,  248,  249,
                     250,  251,  252,  253,  254,  255,  256,  257,  258,  259,
                     260,  261,  262,  263,  264,  265,  266,  267,  268,  269,
                     270,  271,  272,  273,  274,  275,  276,  277,  278,  279,
                     280,  281,  282,  283,  284,  285,  286,  287,  288,  289,
                     290,  291,  292,  293,  294,  295,  296,  297,  298,  299,
                     300,  301,  302,  303,  304,  305,  306,  307,  308,  309,
                     310,  311,  312,  313,  314,  315,  316,  317,  318,  319,
                     320,  321,  322,  323,  324,  325,  326,  327,  328,  329,
                     330,  331,  332,  333,  334,  335,  336,  337,  338,  339,
                     340,  341,  342,  343,  344,  345,  346,  347,  348,  349,
                     350,  351,  352,  353,  354,  355,  356,  357,  358,  359,
                     360,  361,  362,  363,  364,  365,  366,  367,  368,  369,
                     370,  371,  372,  373,  374,  375,  376,  377,  378,  379,
                     380,  381,  382,  383,  384,  385,  386,  387,  388,  389,
                     390,  391,  392,  393,  394,  395,  396,  397,  398,  399,
                     400,  401,  402,  403,  404,  405,  406,  407,  408,  409,
                     410,  411,  412,  413,  414,  415,  416,  417,  418,  419,
                     420,  421,  422,  423,  424,  425,  426,  427,  428,  429,
                     430,  431,  432,  433,  434,  435,  436,  437,  438,  439,
                     440,  441,  442,  443,  444,  445,  446,  447,  448,  449,
                     450,  451,  452,  453,  454,  455,  456,  457,  458,  459,
                     460,  461,  462,  463,  464,  465,  466,  467,  468,  469,
                     470,  471,  472,  473,  474,  475,  476,  477,  478,  479,
                     480,  481,  482,  483,  484,  485,  486,  487,  488,  489,
                     490,  491,  492,  493,  494,  495,  496,  497,  498,  499,
                     500,  501,  502,  503,  504,  505,  506,  507,  508,  509,
                     510,  511,  512,  513,  514,  515,  516,  517,  518,  519,
                     520,  521,  522,  523,  524,  525,  526,  527,  528,  529,
                     530,  531,  532,  533,  534,  535,  536,  537,  538,  539,
                     540,  541,  542,  543,  544,  545,  546,  547,  548,  549,
                     550,  551,  552,  553,  554,  555,  556,  557,  558,  559,
                     560,  561,  562,  563,  564,  565,  566,  567,  568,  569,
                     570,  571,  572,  573,  574,  575,  576,  577,  578,  579,
                     580,  581,  582,  583,  584,  585,  586,  587,  588,  589,
                     590,  591,  592,  593,  594,  595,  596,  597,  598,  599,
                     600,  601,  602,  603,  604,  605,  606,  607,  608,  609,
                     610,  611,  612,  613,  614,  615,  616,  617,  618,  619,
                     620,  621,  622,  623,  624,  625,  626,  627,  628,  629,
                     630,  631,  632,  633,  634,  635,  636,  637,  638,  639,
                     640,  641,  642,  643,  644,  645,  646,  647,  648,  649,
                     650,  651,  652,  653,  654,  655,  656,  657,  658,  659,
                     660,  661,  662,  663,  664,  665,  666,  667,  668,  669,
                     670,  671,  672,  673,  674,  675,  676,  677,  678,  679,
                     680,  681,  682,  683,  684,  685,  686,  687,  688,  689,
                     690,  691,  692,  693,  694,  695,  696,  697,  698,  699,
                     700,  701,  702,  703,  704,  705,  706,  707,  708,  709,
                     710,  711,  712,  713,  714,  715,  716,  717,  718,  719,
                     720,  721,  722,  723,  724,  725,  726,  727,  728,  729,
                     730,  731,  732,  733,  734,  735,  736,  737,  738,  739,
                     740,  741,  742,  743,  744,  745,  746,  747,  748,  749,
                     750,  751,  752,  753,  754,  755,  756,  757,  758,  759,
                     760,  761,  762,  763,  764,  765,  766,  767,  768,  769,
                     770,  771,  772,  773,  774,  775,  776,  777,  778,  779,
                     780,  781,  782,  783,  784,  785,  786,  787,  788,  789,
                     790,  791,  792,  793,  794,  795,  796,  797,  798,  799,
                     800,  801,  802,  803,  804,  805,  806,  807,  808,  809,
                     810,  811,  812,  813,  814,  815,  816,  817,  818,  819,
                     820,  821,  822,  823,  824,  825,  826,  827,  828,  829,
                     830,  831,  832,  833,  834,  835,  836,  837,  838,  839,
                     840,  841,  842,  843,  844,  845,  846,  847,  848,  849,
                     850,  851,  852,  853,  854,  855,  856,  857,  858,  859,
                     860,  861,  862,  863,  864,  865,  866,  867,  868,  869,
                     870,  871,  872,  873,  874,  875,  876,  877,  878,  879,
                     880,  881,  882,  883,  884,  885,  886,  887,  888,  889,
                     890,  891,  892,  893,  894,  895,  896,  897,  898,  899,
                     900,  901,  902,  903,  904,  905,  906,  907,  908,  909,
                     910,  911,  912,  913,  914,  915,  916,  917,  918,  919,
                     920,  921,  922,  923,  924,  925,  926,  927,  928,  929,
                     930,  931,  932,  933,  934,  935,  936,  937,  938,  939,
                     940,  941,  942,  943,  944,  945,  946,  947,  948,  949,
                     950,  951,  952,  953,  954,  955,  956,  957,  958,  959,
                     960,  961,  962,  963,  964,  965,  966,  967,  968,  969,
                     970,  971,  972,  973,  974,  975,  976,  977,  978,  979,
                     980,  981,  982,  983,  984,  985,  986,  987,  988,  989,
                     990,  991,  992,  993,  994,  995,  996,  997,  998,  999,
                    1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009,
                    1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019,
                    1020, 1021, 1022>;

        ASSERTV(L_, Obj::size(),
                    SEQ_TYPE::size(),
                    Obj::size() == SEQ_TYPE::size());
        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
};

template <class T,
          class MAX =
            typename bsl::integral_constant<T, std::numeric_limits<T>::max()> >
struct LargeSequenceUtil_Impl;
    // This class template declares an interface for a meta-function that tests
    // the 'bslmf::IntegerSequences' instantiated for the specified template
    // parameter 'T' which 'std::numeric_limits<T>::max()' value does not
    // exceed 1024.

template <class T>
struct LargeSequenceUtil<T, false> : LargeSequenceUtil_Impl<T>
    // This partial specialization of 'LargeSequenceUtil' tests performance
    // characteristics and recursion limits for types with
    // 'numeric_limits<T>::max() < 1023'.
{
};

template <class T>
struct LargeSequenceUtil_Impl<T, bsl::integral_constant<T, 127> >
    // This partial specialization of 'LargeSequenceUtil_Impl' tests
    // performance characteristics and internal recursion limits for types
    // that 'std::numeric_limits<T>::max() == 127'.
{
    static void test()
        // Ensure that 'MakeIntegerSequence<T, 1023>' factory-function
        // generates correct integer sequence type.
    {
        using Obj = MakeIntegerSequence<T, 127>;
        using SEQ_TYPE = IntegerSequence<T,
                               0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
                              10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
                              20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
                              30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
                              40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
                              50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
                              60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
                              70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
                              80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
                              90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
                             100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                             110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
                             120, 121, 122, 123, 124, 125, 126>;

        ASSERTV(L_, Obj::size(),
                    SEQ_TYPE::size(),
                    Obj::size() == SEQ_TYPE::size());
        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
};

template <class T>
struct LargeSequenceUtil_Impl<T, bsl::integral_constant<T, 255> >
    // This partial specialization of 'LargeSequenceUtil_Impl' tests
    // performance characteristics and internal recursion limits for types
    // that 'std::numeric_limits<T>::max() == 255'.
{
    static void test()
        // Ensure that 'MakeIntegerSequence<T, 1023>' factory-function
        // generates correct integer sequence type.
    {
        using Obj = MakeIntegerSequence<T, 255>;
        using SEQ_TYPE = IntegerSequence<T,
                               0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
                              10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
                              20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
                              30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
                              40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
                              50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
                              60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
                              70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
                              80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
                              90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
                             100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                             110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
                             120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
                             130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
                             140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
                             150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
                             160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
                             170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
                             180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
                             190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
                             200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
                             210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
                             220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
                             230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
                             240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
                             250, 251, 252, 253, 254>;

        ASSERTV(L_, Obj::size(),
                    SEQ_TYPE::size(),
                    Obj::size() == SEQ_TYPE::size());
        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
};

// ============================================================================
//                               TESTS
// ----------------------------------------------------------------------------

template <class T>
void testCase2()
    // ------------------------------------------------------------------------
    // 'bslmf::MakeIntegerSequence'
    //
    // Concerns:
    //: 1 That the 'bslmf::MakeIntegerSequence' resultant type represent
    //:   enumerated collection of increasing integer values of the specified
    //:   length 'N' starting with 0 value.
    //:
    //: 2 That 'T' can be any integer type.
    //:
    //: 3 That an instantiation of the 'bslmf::MakeIntegerSequence' template on
    //:   large index number does not hit internal recursion limit.
    //
    // Plan:
    //: 1 Define a number of index sequences having distinct lengths and
    //:   ensure that these sequences represent collections of integer
    //:   values having the specified length.
    //:
    //: 2 Define integer sequences having the length in a range [0..8] to test
    //:   the template function specializations.
    //:
    //: 3 Define integer sequences having the length 9, 10, 11 to catch edges
    //:   cases and rounding.
    //:
    //: 4 Define integer sequences having the length 15, 16, 17, 18, 19 that
    //:   will handle edge conditions recursing beyond the specializations,
    //:   and find rounding errors.
    //
    // Testing:
    //   bslmf::IntegerSequence<size_t N>
    // ------------------------------------------------------------------------
{
    {
        using Obj      = MakeIntegerSequence<T, 0>;
        using SEQ_TYPE = IntegerSequence<T>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 1>;
        using SEQ_TYPE = IntegerSequence<T, 0>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 2>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 3>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 4>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 5>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 6>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4, 5>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 7>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4, 5, 6>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 8>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4, 5, 6, 7>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 9>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4, 5, 6, 7, 8>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 10>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 11>;
        using SEQ_TYPE = IntegerSequence<T, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 15>;
        using SEQ_TYPE = IntegerSequence<T,
                                         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                        10, 11, 12, 13, 14>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 16>;
        using SEQ_TYPE = IntegerSequence<T,
                                         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                        10, 11, 12, 13, 14, 15>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 17>;
        using SEQ_TYPE = IntegerSequence<T,
                                         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                        10, 11, 12, 13, 14, 15, 16>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 18>;
        using SEQ_TYPE = IntegerSequence<T,
                                         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                        10, 11, 12, 13, 14, 15, 16, 17>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        using Obj      = MakeIntegerSequence<T, 19>;
        using SEQ_TYPE = IntegerSequence<T,
                                         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                        10, 11, 12, 13, 14, 15, 16, 17, 18>;

        ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
    }
    {
        LargeSequenceUtil<T>::test();
    }
}

// ============================================================================
//                             USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Pass C-array as a parameter to a function with variadic template
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Suppose we want to initialize a C-Array of known size 'N' with data read
// from a data source using a library class that provides a variadic template
// interface that loads a data of variable length into the supplied parameter
// pack.
//
// First, define a class template 'DataReader',
//..
template <std::size_t N>
class DataReader {
  public:
//..
// Then, implement a method that loads the specified parameter pack 'args' with
// data read from a data source.
//..
    template <class ...T>
    void read(T*... args) const
    {
        static_assert(sizeof...(args) == N, "");
        read_impl(args...);
    }
//..
// Next, for the test purpose provide simple implementation of the recursive
// variadic 'read_impl' function that streams the number of the C-Array's
// element to 'stdout'.
//..
private:
    template <class U, class ...T>
    void read_impl(U*, T*... args) const
    {
        printf("read element #%i\n",
               static_cast<int>(N - 1 - sizeof...(args)));
        read_impl(args...);
    }
//..
// Then, implement the recursion break condition:
//..
    void read_impl() const
    {
    }
};
//..
// Next, define a helper function template 'readData' that expands the
// parameter pack of indices 'I' and invokes the variadic template 'read'
// method of the specified 'reader' object.
//..
namespace {
template<class R, class T, std::size_t... I>
void readData(const R&  reader,
              T        *data,
              bslmf::IntegerSequence<std::size_t, I...>)
{
    reader.read(&data[I]...);
        // In pseudocode, this is equivalent to:
        // reader.read(&data[0],
        //             &data[1],
        //             &data[2],
        //             ...
        //             &data[N-1]);
}
}
//..
// Now, define function template 'readData' that invokes the helper function
// Note, that the 'bslmf::MakeIntegerSequence<std::size_t, N>' function
// generates an object of an integer sequence class instantiated with a
// template parameter pack of integers that will be expanded and used as an
// array's indices in the helper function when calling the
// 'Reader::read(T*...)' variadic template function.
//..
template<class T, std::size_t N>
void readData(const DataReader<N>& reader, T *data)
{
    readData(reader, data, bslmf::MakeIntegerSequence<std::size_t, N>());
}

}  // close unnamed namespace

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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                              "=============\n");

// Finally, define a 'data' C-Array and 'reader' variables and pass them to the
// 'readData' function as parameters.
//..
        constexpr int      k_SIZE = 5;
        DataReader<k_SIZE> reader;
        int                data[k_SIZE] = {0};

        readData(reader, data);
//..
// The streaming operator produces output in the following format on 'stdout':
//..
// read element #0
// read element #1
// read element #2
// read element #3
// read element #4
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bslmf::MakeIntegerSequence'
        //
        // Concerns:
        //: 1 That the 'bslmf::MakeIntegerSequence' resultant type represent
        //:   enumerated collection of increasing integer values of the
        //:   specified length 'N' starting with 0 value.
        //:
        //: 2 That 'T' can be any integer type.
        //
        // Plan:
        //: 1 Invoke 'testCase2' for each integer type.
        //
        // Testing:
        //   bslmf::IntegerSequence<size_t N>
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bslmf::MakeIntegerSequence'\n"
                              "===========================\n");

        testCase2<         char     >();
        testCase2<  signed char     >();
        testCase2<unsigned char     >();
#if defined BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
        testCase2<         char8_t  >();
#endif
        testCase2<         wchar_t  >();
#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        testCase2<         char16_t >();
        testCase2<         char32_t >();
#endif
        testCase2<         short    >();
        testCase2<unsigned short    >();
        testCase2<         int      >();
        testCase2<unsigned int      >();
        testCase2<         long     >();
        testCase2<unsigned long     >();
        testCase2<         long long>();
        testCase2<unsigned long long>();

        {
            using Obj      = MakeIntegerSequence<bool, 0>;
            using SEQ_TYPE = IntegerSequence<bool>;

            ASSERTV(L_, SEQ_TYPE::size(),   Obj::size(),
                        SEQ_TYPE::size() == Obj::size());
            ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
        }
        {
            using Obj      = MakeIntegerSequence<bool, 1>;
            using SEQ_TYPE = IntegerSequence<bool, 0>;

            ASSERTV(L_, SEQ_TYPE::size(),   Obj::size(),
                        SEQ_TYPE::size() == Obj::size());
            ASSERTV(L_, (bsl::is_same<Obj, SEQ_TYPE>::value));
        }
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

#else

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    ASSERT(true); // remove unused warning for 'aSsErT'

    printf("Cannot test 'bslmf::IntegerSequence' in pre-C++11 mode.\n");
    return -1;
}

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
