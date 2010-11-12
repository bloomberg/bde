// bdes_alignment.t.cpp       -*-C++-*-

#include <bdes_alignment.h>

#include <bsls_platform.h>  // for testing only

#include <bsl_cstddef.h>     // offsetof() macro
#include <bsl_cstdlib.h>     // atoi ()
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//==========================================================================
//                             TEST PLAN
// Most of what this component implements are compile-time computations that
// differ among platforms.  The tests do assume that alignment of 'char' is 1,
// 'short' is 2, 'int' is 4, and 'double' is at least 4.  In addition, certain
// invariants are tested, including:
//
// 1. That all alignment calculations result in a power of 2.
// 2. That all alignment-to-type calculations result are reversible, so that
//    the alignment of the resulting type equals the original input.
// 3. That the alignment of a 'struct' equals the alignment of its
//    most-strictly aligned member.
// 4. That 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT' really is the largest value
//    that will be produced by the alignment calculations and that
//    'bsls_AlignmentUtil::MaxAlignedType' is aligned at
//    'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.
//
// For the few run-time functions provided in this component, we establish
// post-conditions and test that the postconditions hold over a reasonable
// range of inputs.
//--------------------------------------------------------------------------
// [ 1] bdes_AlignmentOf<T>::VALUE
// [ 2] bdes_AlignmentToType<N>::Type
// [ 2] bdes_AlignmentOf<T>::Type
// [ 3] bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
// [ 4] bsls_AlignmentUtil::MaxAlignedType
// [ 4] bdes_Alignment::Align (deprecated)
//
// [ 5] int bsls_AlignmentUtil::calculateAlignmentFromSize(int size)
// [ 6] int bsls_AlignmentUtil::calculateAlignmentOffset(void *, int)
//--------------------------------------------------------------------------
// [ 3] PRINT ALIGNMENTS -- Display alignment values (very verbose mode only).
// [ 7] USAGE EXAMPLE -- Ensure the usage example compiles and works.
//==========================================================================

//--------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
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

//==========================================================================
//                  GLOBAL DEFINITIONS FOR TESTING
//--------------------------------------------------------------------------

// Placement new defined here since we can't include <new>.
// Can't use obvious definition because some compilers have placement new
// built-in.
struct Placement { void *d_ptr; Placement(void *p) : d_ptr(p) { } };
inline void *operator new(bsl::size_t, Placement p) throw() { return p.d_ptr; }
inline void operator delete(void *, Placement) throw() { }

typedef bsls_AlignmentUtil Class;
    // Abbreviation for 'bsls_AlignmentUtil' -- mostly to keep text under 80
    // columns.

#if defined(BSLS_PLATFORM__CMP_GNU) && defined(BSLS_PLATFORM__OS_AIX)
// case 6
const int BUF_SIZE = 3 * bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
static union {
    char                               d_buffer[BUF_SIZE];
    bsls_AlignmentUtil::MaxAlignedType d_align;
} alignedBuf;
#endif

//==========================================================================
//                             USAGE EXAMPLE
//--------------------------------------------------------------------------

///Usage Example 1:
///- - - - - - - -
// The following shows how 'bdes_AlignmentOf' can be used to create a static
// "database" of types and their attributes:
//..
    enum my_ElemType { MY_CHAR, MY_INT, MY_DOUBLE, MY_POINTER };

    struct my_ElemAttr {
        my_ElemType d_type;
        int         d_size;
        int         d_alignment;
    };

    static const my_ElemAttr MY_ATTRIBUTES[] = {
        { MY_CHAR,      sizeof(char),   bdes_AlignmentOf<char>::VALUE },
        { MY_INT,       sizeof(int),    bdes_AlignmentOf<int>::VALUE },
        { MY_DOUBLE,    sizeof(double), bdes_AlignmentOf<double>::VALUE },
        { MY_POINTER,   sizeof(void *), bdes_AlignmentOf<void *>::VALUE }
    };

    int usageExample1()
    {
        ASSERT(1 == MY_ATTRIBUTES[MY_CHAR].d_alignment);
        return 0;
    }
//..
///Usage Example 2:
///- - - - - - - -
// The following shows how 'bdes_AlignmentOf' automatically computes the
// alignment of a 'struct' as the alignment of its most restrictively aligned
// member.
//..
    class my_String {
        char d_buf[5];
        int  d_len;
      public:
        my_String(const char* s) : d_len(strlen(s)) { strcpy(d_buf, s); }
        ~my_String() { }

        int length() const { return d_len; }
        const char* c_str() const { return d_buf; }
    };

    int usageExample2()
    {
        ASSERT((int) bdes_AlignmentOf<my_String>::VALUE ==
               (int) bdes_AlignmentOf<int>::VALUE);
        return 0;
    }

//..
///Usage Example 3:
///- - - - - - - -
// 'bdes_AlignmentOf<TYPE>::Type' can be used to create properly-aligned
// buffers for constructing objects of 'TYPE'.  If 'TYPE' is
// 'bsls_AlignmentUtil::MaxAlignedType' the resulting buffer can hold any type
// at all.
//..
    template <typename TYPE, int SIZE = sizeof(TYPE)>
    union my_AlignedBuffer
    {
        typename bdes_AlignmentOf<TYPE>::Type d_align;      // force alignment
        char                                  d_buff[SIZE]; // force size
    };
//..
// This small test program creates a couple of aligned buffers and constructs
// objects into the aligned memory.
//..
    int usageExample3()
    {
        // Aligned
        my_AlignedBuffer<my_String> buf;
        my_String* const strptr = reinterpret_cast<my_String*>(&buf);
        new (strptr) my_String("yes"); // Construct my_String obj into buffer
        ASSERT(0 == strcmp("yes", strptr->c_str()));
        strptr->~my_String();          // Destroy my_String object

        // 1000-byte buffer aligned for any type.
        my_AlignedBuffer<bsls_AlignmentUtil::MaxAlignedType, 1000> buf2;
        double* doublePtr = reinterpret_cast<double*>(&buf2);
        doublePtr[9] = 10.5; // No bus errors: doublePtr is properly aligned.

        return 0;
    }
//..
///Usage Example 4:
///- - - - - - - -
// The following function illustrates how to use the
// 'calculateAlignmentOffset' function to align a memory address properly and
// 'calculateAlignmentFromSize' to compute a safe alignment when none is
// supplied.  This function allocates memory with specified size and alignment
// requirements from a global buffer.  (See 'bdema_bufferallocator' for a more
// robust implementation.)
//..
    enum { BUFFER_SIZE = 1000 };

    static union {
        bsls_AlignmentUtil::MaxAlignedType d_dummy;  // maximally align buffer
        char                               d_buffer[BUFFER_SIZE];
    } memory;

    static int cursor = 0;

    static void *allocateFromBuffer(int size, int alignment = 0)
        // Allocate memory of the specified 'size' and 'alignment' from
        // a fixed-size global buffer.  Return the address of the allocated
        // memory, or 0 if insufficient space remains in the buffer.
        // If 'alignment' is zero (i.e., not specified), then compute the
        // alignment based on size.
        // The behavior is undefined unless 0 < size and 0 <= alignment.
    {
        ASSERT(0 < size); ASSERT(0 <= alignment);

        if (0 == alignment) {
            alignment = bsls_AlignmentUtil::calculateAlignmentFromSize(size);
        }

        int offset = bsls_AlignmentUtil::calculateAlignmentOffset(
                     memory.d_buffer + cursor, alignment);

        if (cursor + offset + size > BUFFER_SIZE) {
            return 0;   // Insufficient space remains.
        }

        cursor += offset;
        void *result = memory.d_buffer + cursor;
        cursor += size;

        return result;
    }

    class Object {     // Assume we cannot know the specific data
        char  d_c;     // members of 'Object'.
        int   d_i;
        void *d_ptr;
    };
//..
// This test program uses the 'allocateFromBuffer' function above to allocate
// memory for various different types of objects.
//..
    int usageExample4()
    {
        enum {
            CHAR_ALIGNMENT  = bsls_AlignmentFromType<char>::VALUE,
            SHORT_ALIGNMENT = bsls_AlignmentFromType<short>::VALUE
        };

        char *charPtr   = (char *)   allocateFromBuffer(3 * sizeof(char),
                                                        CHAR_ALIGNMENT);

        short *shortPtr = (short *)  allocateFromBuffer(3 * sizeof(short),
                                                        SHORT_ALIGNMENT);

        ASSERT(0 == (((char*) shortPtr - memory.d_buffer) % SHORT_ALIGNMENT));

        // Let 'allocateFromBuffer' calculate a safe alignment for 'Object'
        // based on its size.
        Object *objPtr = (Object *)  allocateFromBuffer(3 * sizeof(Object));

        ASSERT(0 == (((char*) objPtr - memory.d_buffer) %
                     bdes_AlignmentOf<Object>::VALUE));

        // Assuming 1, 2, and 4 are the alignments for 'char', 'short', and
        // 'Object' respectively, the following diagram illustrates the memory
        // layout within 'memory.d_buffer':
        //
        //   charPtr         shortPtr                        objPtr
        //      |               |                               |
        //      V               V                               V
        //    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--
        //    | x | x | x |   | x | x | x | x | x | x |   |   | x | x | x |...
        //    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--
        //      0   1   2   3   4   5   6   7   8   9  10  11  12  13  14

        if (!charPtr || !shortPtr || !objPtr) {
            bsl::cerr << "Global buffer is not large enough." << bsl::endl;
            return -1;
        }

        // ...

        return 0;
    }
//..

//==========================================================================
//                  CLASSES AND FUNCTIONS USED IN TESTS
//--------------------------------------------------------------------------

#define IS_POW2(X) (((X) | ((X)-1)) + 1 == 2 * (X))

struct S1 { char d_buff[8]; S1(char); };
struct S2 { char d_buff[8]; int d_int; S2(); private: S2(const S2&); };
struct S3 { S1 d_s1; double d_double; short d_short; };
struct S4 { short d_shorts[5]; char d_c;  S4(int); private: S4(const S4&); };
#if defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_X86)
struct S5 { long long d_longLong __attribute__((__aligned__(8))); };
#endif
union  U1 { char d_c; int *d_pointer; };

template <typename T>
inline bool samePtrType(T *p1, void *p2)
{
    return false;
}

template <typename T>
inline bool samePtrType(T *p1, T *p2)
{
    return true;
}

template <typename T1, typename T2>
inline bool sameType(T1 t1, T2 t2)
{
    return samePtrType(&t1, &t2);
}

//==========================================================================
//                              MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Make sure main usage examples compile and work as advertized.
        //   Thoroughly test the example function 'allocateFromBuffer'.
        //
        // Test plan:
        //   Copy usage example verbatim into test driver then change 'assert'
        //   to 'ASSERT'.  Since usage example code declares template classes
        //   and functions, it must be placed outside of main().  Within this
        //   test case, therefore, we call the externally-declared functions
        //   defined above.
        //
        //   For completeness, test allocateFromBuffer by allocating
        //   various-sized and aligned objects.  Note that both scenarios of
        //   when 'memory.d_buffer' is 4-byte aligned and 8-byte aligned are
        //   tested.  If 'memory.d_buffer' is 8-byte aligned, 'cursor' is
        //   offset by 4 to simulate a 4-byte aligned buffer.  If
        //   'memory.d_buffer' is 4-byte aligned, 'cursor' is offset by 4 to
        //   simulate an 8-byte aligned buffer.
        //
        // Testing:
        //   USAGE EXAMPLE -- Ensure the usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE" << endl
                          << "\n=====" << endl;

        usageExample1();
        usageExample2();
        usageExample3();
        usageExample4();

        // -------------------------------------------------------------------
        // The following is a pretty thorough test case for
        // allocateFromBuffer.  It is overkill, since allocateFromBuffer is
        // just a usage example, but here it is.

        struct {
            int d_line;      // line number
            int d_size;      // allocation request size
            int d_alignment; // alignment
            int d_exp4;      // expected memory offset (buffer 4-byte aligned)
            int d_exp8;      // expected memory offset (buffer 8-byte aligned)
        } DATA[] = {
            //line    allocate    align-    offset      offset
            //no.     size        ment      (4-byte)    (8-byte)
            //----    --------    ------    --------    -------
            { L_,        1,         1,         0,          0   },
            { L_,        2,         1,         1,          1   },
            { L_,        2,         1,         3,          3   },
            { L_,        2,         1,         5,          5   },
            { L_,        2,         2,         8,          8   },
            { L_,        2,         2,        10,         10   },
            { L_,       16,         8,        12,         16   },
            { L_,        1,         1,        28,         32   },
            { L_,       20,         4,        32,         36   },
            { L_,       20,         4,        52,         56   },
            { L_,      911,         8,        76,         80   },
            { L_,        4,         4,       988,        992   },
            { L_,        2,         2,       992,        996   },
            { L_,        1,         1,       994,        998   },
            { L_,        1,         1,       995,        999   }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // Loop through initial buffer alignment of 4 and 8 bytes
        for (int ti = 4;
             ti <= 8 && ti <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
             ti += 4) {

            // Cursor should be set to 0 or 4, depending on whether
            // buffer alignment is as desired or needs to be offset.
            if (size_t(memory.d_buffer) % 8 == size_t(ti) % 8)
                cursor = 0;
            else
                cursor = 4;

            // adjusted pointer to buffer
            const char * const B = memory.d_buffer + cursor;
            if (veryVerbose) { TAB; A(B); }

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE = DATA[di].d_line;
                const int SIZE = DATA[di].d_size;
                const int ALIGN = DATA[di].d_alignment;
                const int EXP = (ti == 4 ? DATA[di].d_exp4 : DATA[di].d_exp8);

                char *p = (char *) allocateFromBuffer(SIZE, ALIGN);
                int diff = p - B;
                if (veryVerbose) { TAB; P_(diff); TAB; P(EXP); }
                LOOP3_ASSERT(LINE, ti, di, EXP == diff);
            }
        }
      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION 'bsls_AlignmentUtil::calculateAlignmentOffset'
        //   Ensure correctness for each pair of (ma, a), where 'ma' is a
        //   memory address from 'baseAddr' to
        //   'baseAddr + 2 * BSLS_MAX_ALIGNMENT + 1' and 'a' is an alignment in
        //   [1 2 4 .. BSLS_MAX_ALIGNMENT] such that:
        //     0 <= result
        //     result < a
        //     0 == (ma + result - baseAddr) % a;
        //     for each k in [ 0 .. result - 1 ]
        //         0 != (ma + k - baseAddr) % a;
        // Plan:
        //   Declare baseAddr as a pointer to a maximally-aligned memory
        //     buffer of size 2 * BSLS_MAX_ALIGNMENT + 1.
        //   Iterate 'ma' over each address in buffer and iterate 'a' each
        //     power of two up to BSLS_MAX_ALIGNMENT.
        //   Compute bsls_AlignmentUtil::calculateAlignmentOffset(ma, a) and
        //     verify all concerns listed above.
        //
        // Tactics:
        //   Area test over meaningful range of inputs.
        //
        // Testing:
        //   int bsls_AlignmentUtil::calculateAlignmentOffset(void *, int)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test calculateAlignmentOffset" << endl
                                  << "=============================" << endl;

        // MSC compiler lays out structs with 8-byte alignment, but then
        // allows those structs to appear on the stack with 4-byte alignment.
        // This is not necessarily a bug, but it is counter-intuitive.  To be
        // sure that this buffer has max alignment, we put it in static
        // storage.  This is for testing only -- in practice, it is rarely
        // necessary to force the alignment of a stack variable beyond what
        // the compiler requires.  Moreover, this trick does not even work for
        // gcc builds on AIX.
#if !defined(BSLS_PLATFORM__CMP_GNU) || !defined(BSLS_PLATFORM__OS_AIX)
        const int BUF_SIZE = 3 * bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

        static union {
            char                               d_buffer[BUF_SIZE];
            bsls_AlignmentUtil::MaxAlignedType d_align;
        } alignedBuf;
#endif

        char *baseAddr = alignedBuf.d_buffer;

        for (char *ma = baseAddr;
             ma < baseAddr + 2 * bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT + 1;
             ++ma) {

            for (int a = 1;
                 a <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT; a <<= 1) {

                int offset = bsls_AlignmentUtil::calculateAlignmentOffset(ma,
                                                                          a);

                if (veryVerbose) {
                    cout << "\tMemory address = " << (void*) ma
                         << "\tAlignment = " << a
                         << "\tOffset = " << offset << endl;
                }

                LOOP2_ASSERT(a, ma, 0 <= offset);
                LOOP2_ASSERT(a, ma, offset < a);
                LOOP2_ASSERT(a, ma, 0 == (ma - baseAddr + offset) % a);
                for (int k = 0; k < offset; ++k) {
                    LOOP3_ASSERT(a, ma, k, 0 != (ma + k - baseAddr) % a);
                }
            }
        }
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION 'bsls_AlignmentUtil::calculateAlignmentFromSize'
        //   Ensure correctness for all object sizes:
        //     Result must be greater than 0.
        //     Result must be less than or equal to BSLS_MAX_ALIGNMENT
        //     Size must be evenly divisible by result
        //     Size must not be evenly divisible by 2*result unless
        //         result == BSLS_MAX_ALIGNMENT
        //
        // Plan:
        //   Assume BSLS_MAX_ALIGNMENT <= 4.
        //   Compare results against expected values for inputs from 1 to 4.
        //   Try all sizes up to 2 * BSLS_MAX_ALIGNMENT + 1.
        //   Test all concerns listed above.
        //
        // Tactics:
        //   Area test over meaningful range of inputs.
        //
        // Testing:
        //   int bsls_AlignmentUtil::calculateAlignmentFromSize(int)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Test calculateAlignmentFromSize" << endl
            << "===============================" << endl;

        static struct {
            int d_line;
            int d_input;
            int d_expected;
        } const DATA[] = {
            // line input expected
            // ---- ----- --------
            {  L_,    1,    1 },
            {  L_,    2,    2 },
            {  L_,    3,    1 },
            {  L_,    4,    4 }
        };

        const int DATA_SIZE = sizeof(DATA) / sizeof(DATA[0]);

        for (int i = 0; i < DATA_SIZE; ++i) {
            const int LINE     = DATA[i].d_line;
            const int INPUT    = DATA[i].d_input;
            const int EXPECTED = DATA[i].d_expected;

            int a = bsls_AlignmentUtil::calculateAlignmentFromSize(INPUT);
            LOOP_ASSERT(LINE, EXPECTED == a);
        }

        for (int size = 1;
             size <= 2 * bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT + 1; ++size) {
            int a = bsls_AlignmentUtil::calculateAlignmentFromSize(size);

            if (veryVerbose) {
                cout << "    Object size = " << size
                     << "\tGuessed alignment = " << a << endl;
            }

            LOOP_ASSERT(size,  a > 0);
            LOOP_ASSERT(size,  IS_POW2(a));
            LOOP_ASSERT(size,  a <= bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
            LOOP_ASSERT(size,  0 == size % a);
            LOOP_ASSERT(size, (0 != size % (2 * a) ||
                               a == bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));

        }

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING bsls_AlignmentUtil::MaxAlignedType
        //   Ensure that alignment of MaxAlignedType is BSLS_MAX_ALIGNMENT
        //     and sizeof(MaxAlignedType) == BSLS_MAX_ALIGNMENT
        //   Ensure that alignment of Align (deprecated) is BSLS_MAX_ALIGNMENT
        //     and sizeof(align) == BSLS_MAX_ALIGNMENT
        //
        // Plan:
        //   Use bdes_AlignmentOf<T>::VALUE (previously tested), to verify
        //     alignment of MaxAlignedType and Aligned.
        //
        // Tactics:
        //   Exhaustive test over entire input range.
        //
        // Testing:
        //   bsls_AlignmentUtil::MaxAlignedType
        //   bdes_Alignment::Align (deprecated)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "MaxAlignedType TEST" << endl
                                  << "===================" << endl;

        ASSERT((int) bdes_AlignmentOf<Class::MaxAlignedType>::VALUE ==
               (int) Class::BSLS_MAX_ALIGNMENT);

        ASSERT(sizeof(Class::MaxAlignedType) == Class::BSLS_MAX_ALIGNMENT);

        ASSERT((int) bdes_AlignmentOf<Class::MaxAlignedType>::VALUE ==
               (int) Class::BSLS_MAX_ALIGNMENT);

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING COMPILE-TIME CONSTANTS
        //   Ensure that for each basic type:
        //     Its corresponding alignment constant divides its size,
        //     BSLS_MAX_ALIGNMENT is divisible by the alignment constant,
        //     The alignment is a non-negative integral power of 2.
        //   Ensure that BSLS_MAX_ALIGNMENT is the same as one or more of the
        //     primitive alignments.
        //   Ensure that BSLS_MAX_ALIGNMENT is the largest primitive alignment.
        //
        // Plan:
        //   Verify each concern for each primitive data type.
        //   Note: Verifying that BSLS_MAX_ALIGNMENT is divisible by each
        //     alignment constant guarantees that BSLS_MAX_ALIGNMENT is at
        //     least as big as each alignment constant.
        //   Display alignments in very verbose mode (only).
        //   Since all alignment values are powers of two, using a bit-wise
        //     OR of all of them creates a bit-map of used alignments.
        //   Verify that BSLS_MAX_ALIGNMENT is represented in the bit-map.
        //   Verify that 2 * BSLS_MAX_ALIGNMENT is NOT in the bit-map.
        //
        //   In veryVerbose mode, print the alignment of each type being
        //     tested.
        //
        // Tactics:
        //   Exhaustive test over entire input range.
        //
        // Testing:
        //   bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
        //
        //   PRINT ALIGNMENTS IN VERY VERBOSE MODE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test compile-time constants" << endl
                                  << "===========================" << endl;
        if (veryVerbose) {
            cout << endl << "ALIGNMENTS" << endl
                         << "----------" << endl;
            P(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
        }

        if (veryVerbose) {
            cout << endl << "SIZES" << endl
                         << "-----" << endl;
            P(sizeof(char));
            P(sizeof(short));
            P(sizeof(int));
            P(sizeof(long));
            P(sizeof(long long));
            P(sizeof(bool));
            P(sizeof(wchar_t));
            P(sizeof(void*));
            P(sizeof(void (*)));
            P(sizeof(float));
            P(sizeof(double));
            P(sizeof(long double));

            P(sizeof(bsls_AlignmentUtil::MaxAlignedType));
        }

        if (verbose) cout << "\nVerify 0 == size % alignment" << endl;
        ASSERT(0 == sizeof(Class::MaxAlignedType) % Class::BSLS_MAX_ALIGNMENT);

        if (verbose) cout <<
            "\nVerify alignment is non-negative integral power of 2." << endl;

        ASSERT(1 == IS_POW2(0));        ASSERT(1 == IS_POW2(1));
        ASSERT(1 == IS_POW2(2));        ASSERT(0 == IS_POW2(3));
        ASSERT(1 == IS_POW2(4));        ASSERT(0 == IS_POW2(5));
        ASSERT(0 == IS_POW2(6));        ASSERT(0 == IS_POW2(7));
        ASSERT(1 == IS_POW2(8));        ASSERT(0 == IS_POW2(9));

        ASSERT(IS_POW2(Class::BSLS_MAX_ALIGNMENT));

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS  bdes_AlignmentToType<N>::Type AND
        //     bdes_AlignmentOf<T>::Type
        //
        //   Ensure that the right type is selected for each possible
        //      alignment.
        //   Given a type T with alignment N, ensure that
        //      bdes_AlignmentToType<N> is the same as
        //      bdes_AlignmentOf<T>::Type.
        //   Ensure reversibility: alignment(alignmentToType(N)) == N
        //
        // PLAN
        //   1. Compute type for alignments 1, 2, and 4 and confirm that they
        //      correspond to 'char', 'short', and 'int', respectively.
        //   2. Compute alignment, N for each primitive type T and for four
        //      'struct' and 'union' types using bdes_AlignmentOf<T>::VALUE.
        //   3. For each alignment computed in step 2, compute alignment type
        //      using bdes_AlignmentToType<N>::Type.
        //   4. For each alignment N from step 2 and type T from step 3,
        //      verify that bdes_AlignmentToType<N>::Type is the same as
        //      bdes_AlignmentOf<T>::Type using a helper function,
        //      isSameType().
        //   5. For each alignment N from step 2 and type T from 3, verify that
        //      N == bdes_AlignmentOf<T>::VALUE.
        //
        // TACTICS
        //   Ad-hoc data selection
        //   Area test using primitive types.
        //
        // TESTING
        //   bdes_AlignmentToType<N>::Type
        //   bdes_AlignmentOf<T>::Type
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTest bdes_AlignmentToType<T>::Type"
                          << "\n==================================" << endl;

        // Test sameType function.
        ASSERT(sameType(int(), int()));
        ASSERT(! sameType(int(), short()));

        if (veryVerbose) cout << "  Testing char, short & int" << endl;
        ASSERT(sameType(bdes_AlignmentToType<1>::Type(), char()));
        ASSERT(sameType(bdes_AlignmentToType<2>::Type(), short()));
        ASSERT(sameType(bdes_AlignmentToType<4>::Type(), int()));

        typedef void (*FuncPtr)();
        enum {
            CHAR_ALIGNMENT        = bdes_AlignmentOf<char>::VALUE,
            SHORT_ALIGNMENT       = bdes_AlignmentOf<short>::VALUE,
            INT_ALIGNMENT         = bdes_AlignmentOf<int>::VALUE,
            LONG_ALIGNMENT        = bdes_AlignmentOf<long>::VALUE,
            INT64_ALIGNMENT       = bdes_AlignmentOf<long long>::VALUE,
            BOOL_ALIGNMENT        = bdes_AlignmentOf<bool>::VALUE,
            WCHAR_T_ALIGNMENT     = bdes_AlignmentOf<wchar_t>::VALUE,
            PTR_ALIGNMENT         = bdes_AlignmentOf<void*>::VALUE,
            FUNC_PTR_ALIGNMENT    = bdes_AlignmentOf<FuncPtr>::VALUE,
            FLOAT_ALIGNMENT       = bdes_AlignmentOf<float>::VALUE,
            DOUBLE_ALIGNMENT      = bdes_AlignmentOf<double>::VALUE,
            LONG_DOUBLE_ALIGNMENT = bdes_AlignmentOf<long double>::VALUE,

            S1_ALIGNMENT          = bdes_AlignmentOf<S1>::VALUE,
            S2_ALIGNMENT          = bdes_AlignmentOf<S2>::VALUE,
            S3_ALIGNMENT          = bdes_AlignmentOf<S3>::VALUE,
            S4_ALIGNMENT          = bdes_AlignmentOf<S4>::VALUE,
#if defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_X86)
            S5_ALIGNMENT          = bdes_AlignmentOf<S5>::VALUE,
#endif
            U1_ALIGNMENT          = bdes_AlignmentOf<U1>::VALUE
        };

        typedef bdes_AlignmentToType<CHAR_ALIGNMENT>::Type     CharAlign;
        typedef bdes_AlignmentToType<SHORT_ALIGNMENT>::Type    ShortAlign;
        typedef bdes_AlignmentToType<INT_ALIGNMENT>::Type      IntAlign;
        typedef bdes_AlignmentToType<LONG_ALIGNMENT>::Type     LongAlign;
        typedef bdes_AlignmentToType<INT64_ALIGNMENT>::Type    LongLongAlign;
        typedef bdes_AlignmentToType<BOOL_ALIGNMENT>::Type     BoolAlign;
        typedef bdes_AlignmentToType<WCHAR_T_ALIGNMENT>::Type  Wchar_tAlign;
        typedef bdes_AlignmentToType<PTR_ALIGNMENT>::Type      PtrAlign;
        typedef bdes_AlignmentToType<FUNC_PTR_ALIGNMENT>::Type FuncPtrAlign;
        typedef bdes_AlignmentToType<FLOAT_ALIGNMENT>::Type    FloatAlign;
        typedef bdes_AlignmentToType<DOUBLE_ALIGNMENT>::Type   DoubleAlign;
        typedef bdes_AlignmentToType<LONG_DOUBLE_ALIGNMENT>::Type
            LongDoubleAlign;

        typedef bdes_AlignmentToType<S1_ALIGNMENT>::Type       S1Align;
        typedef bdes_AlignmentToType<S2_ALIGNMENT>::Type       S2Align;
        typedef bdes_AlignmentToType<S3_ALIGNMENT>::Type       S3Align;
        typedef bdes_AlignmentToType<S4_ALIGNMENT>::Type       S4Align;
#if defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_X86)
        typedef bsls_AlignmentToType<S5_ALIGNMENT>::Type       S5Align;
#endif
        typedef bdes_AlignmentToType<U1_ALIGNMENT>::Type       U1Align;

        if (veryVerbose) cout << "  Testing that bdes_AlignmentToType<N>::Type"
                              << "\n  is the same as bdes_AlignmentOf<T>::Type"
                              << endl;
        ASSERT(sameType(CharAlign(),     bdes_AlignmentOf<char>::Type()));
        ASSERT(sameType(ShortAlign(),    bdes_AlignmentOf<short>::Type()));
        ASSERT(sameType(IntAlign(),      bdes_AlignmentOf<int>::Type()));
        ASSERT(sameType(LongAlign(),     bdes_AlignmentOf<long>::Type()));
        ASSERT(sameType(LongLongAlign(), bdes_AlignmentOf<long long>::Type()));
        ASSERT(sameType(BoolAlign(),     bdes_AlignmentOf<bool>::Type()));
        ASSERT(sameType(Wchar_tAlign(),  bdes_AlignmentOf<wchar_t>::Type()));
        ASSERT(sameType(PtrAlign(),      bdes_AlignmentOf<void*>::Type()));
        ASSERT(sameType(FuncPtrAlign(),  bdes_AlignmentOf<FuncPtr>::Type()));
        ASSERT(sameType(FloatAlign(),    bdes_AlignmentOf<float>::Type()));
        ASSERT(sameType(DoubleAlign(),   bdes_AlignmentOf<double>::Type()));
        ASSERT(sameType(LongDoubleAlign(),
                        bdes_AlignmentOf<long double>::Type()));

        ASSERT(sameType(S1Align(),       bdes_AlignmentOf<S1>::Type()));
        ASSERT(sameType(S2Align(),       bdes_AlignmentOf<S2>::Type()));
        ASSERT(sameType(S3Align(),       bdes_AlignmentOf<S3>::Type()));
        ASSERT(sameType(S4Align(),       bdes_AlignmentOf<S4>::Type()));
#if defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_X86)
        ASSERT(sameType(S5Align(),       bdes_AlignmentOf<S5>::Type()));
#endif
        ASSERT(sameType(U1Align(),       bdes_AlignmentOf<U1>::Type()));

        if (veryVerbose) cout << "  Testing that bdes_AlignmentToType "
                              << "is reversible" << endl;
        ASSERT(CHAR_ALIGNMENT   == (int) bdes_AlignmentOf<CharAlign>::VALUE);
        ASSERT(SHORT_ALIGNMENT  == (int) bdes_AlignmentOf<ShortAlign>::VALUE);
        ASSERT(INT_ALIGNMENT    == (int) bdes_AlignmentOf<IntAlign>::VALUE);
        ASSERT(LONG_ALIGNMENT   == (int) bdes_AlignmentOf<LongAlign>::VALUE);
        ASSERT(INT64_ALIGNMENT  ==
               (int) bdes_AlignmentOf<LongLongAlign>::VALUE);
        ASSERT(BOOL_ALIGNMENT   == (int) bdes_AlignmentOf<BoolAlign>::VALUE);
        ASSERT(WCHAR_T_ALIGNMENT ==
               (int) bdes_AlignmentOf<Wchar_tAlign>::VALUE);
        ASSERT(PTR_ALIGNMENT    == (int) bdes_AlignmentOf<PtrAlign>::VALUE);
        ASSERT(FUNC_PTR_ALIGNMENT ==
               (int) bdes_AlignmentOf<FuncPtrAlign>::VALUE);
        ASSERT(FLOAT_ALIGNMENT  == (int) bdes_AlignmentOf<FloatAlign>::VALUE);
        ASSERT(DOUBLE_ALIGNMENT == (int) bdes_AlignmentOf<DoubleAlign>::VALUE);
        ASSERT(LONG_DOUBLE_ALIGNMENT ==
               (int) bdes_AlignmentOf<LongDoubleAlign>::VALUE);

        ASSERT(S1_ALIGNMENT     == (int) bdes_AlignmentOf<S1Align>::VALUE);
        ASSERT(S2_ALIGNMENT     == (int) bdes_AlignmentOf<S2Align>::VALUE);
        ASSERT(S3_ALIGNMENT     == (int) bdes_AlignmentOf<S3Align>::VALUE);
        ASSERT(S4_ALIGNMENT     == (int) bdes_AlignmentOf<S4Align>::VALUE);
#if defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_X86)
        ASSERT(S5_ALIGNMENT     == (int) bdes_AlignmentOf<S5Align>::VALUE);
#endif
        ASSERT(U1_ALIGNMENT     == (int) bdes_AlignmentOf<U1Align>::VALUE);
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTION bdes_AlignmentOf<T>::VALUE
        //   Ensure that alignment values are correctly computed.
        //   Ensure that alignment of structs is computed as alignment of
        //      most restrictive member.
        //   Ensure that bdes_AlignmentOf can be instantiated on a type that
        //      has no default constructor and has a private copy constructor.
        //
        // PLAN
        //   To test 'bdes_AlignmentOf<T>::VALUE', we must instantiate
        //   'bdes_AlignmentOf' for several types and test that the VALUE
        //   member has the expected value.  Since the alignment requirements
        //   for char, short and int are 1, 2 and 4 on all currently-supported
        //   platforms, we use these as our test cases.  We also verify that
        //   double has an alignment at least as restrictive as int and that
        //   'unsigned int' has exactly the same alignment as 'int'.
        //
        //   To test the alignment of structs, we choose three structures with
        //   different compositions as well as one union and test that
        //   bdes_AlignmentOf computes an alignment value for each of them
        //   equal to the alignment of the most restrictive element.  At least
        //   one of these structured type has no default constructor and/or
        //   has a private copy constructor.
        //
        //   In veryVerbose mode, print the alignment of each type being
        //   tested.
        //
        // TACTICS
        //   Ad-hoc test data.
        //
        // TESTING
        //   bdes_AlignmentOf<T>::VALUE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTest bdes_AlignmentOf<T>::VALUE"
                          << "\n===============================" << endl;

        if (veryVerbose) {
            cout << "\nAlignment Values:" << endl;
            P(bdes_AlignmentOf<char>::VALUE);
            P(bdes_AlignmentOf<short>::VALUE);
            P(bdes_AlignmentOf<int>::VALUE);
            P(bdes_AlignmentOf<unsigned>::VALUE);
            P(bdes_AlignmentOf<double>::VALUE);

            P(bdes_AlignmentOf<S1>::VALUE);
            P(bdes_AlignmentOf<S2>::VALUE);
            P(bdes_AlignmentOf<S3>::VALUE);
            P(bdes_AlignmentOf<U1>::VALUE);
        }

        ASSERT(1 == bdes_AlignmentOf<char>::VALUE);
        ASSERT(2 == bdes_AlignmentOf<short>::VALUE);
        ASSERT(4 == bdes_AlignmentOf<int>::VALUE);
        ASSERT(4 == bdes_AlignmentOf<unsigned>::VALUE);
        ASSERT(4 <= bdes_AlignmentOf<double>::VALUE);

        ASSERT(bdes_AlignmentOf<char>::VALUE   ==
               (int) bdes_AlignmentOf<S1>::VALUE);
        ASSERT(bdes_AlignmentOf<int>::VALUE    ==
               (int) bdes_AlignmentOf<S2>::VALUE);
        ASSERT(bdes_AlignmentOf<double>::VALUE ==
               (int) bdes_AlignmentOf<S3>::VALUE);
        ASSERT(bdes_AlignmentOf<short>::VALUE  ==
               (int) bdes_AlignmentOf<S4>::VALUE);
        ASSERT(bdes_AlignmentOf<void*>::VALUE  ==
               (int) bdes_AlignmentOf<U1>::VALUE);
#if defined(BSLS_PLATFORM__OS_LINUX) && defined(BSLS_PLATFORM__CPU_X86)
        ASSERT(bdes_AlignmentOf<bsls_AlignmentImp8ByteAlignedType>::VALUE ==
               (int) bdes_AlignmentOf<S5>::VALUE);
#endif

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
