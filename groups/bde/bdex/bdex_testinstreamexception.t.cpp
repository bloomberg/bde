// bdex_testinstreamexception.t.cpp -*-C++-*-

#include <bdex_testinstreamexception.h>
#include <bdex_bytestreamimputil.h>     // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a simple exception object that contains an attribute
// initialized by its constructor.  We exercise both the constructor and the
// accessor for the attribute by creating objects initialized with varying
// values, and ensure that the accessor returns the expected values.
//-----------------------------------------------------------------------------
// [1] bdex_TestInStreamException(typeCode);
// [1] ~bdex_TestInStreamException();
// [1] bdex_FieldCode::dataType() const;
//-----------------------------------------------------------------------------
// [2] USAGE TEST - Make sure main usage example compiles and works.
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define Q_(X) cout << "<| " #X " |>" << flush;  // Q(X) without '\n'
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_shortarray.h

class my_ShortArray {
    short *d_array_p;    // dynamically-allocated array of short integers
    int    d_size;       // physical size of the 'd_array_p' array (elements)
    int    d_length;     // logical length of the 'd_array_p' array (elements)

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_ShortArray();
    ~my_ShortArray();

    template <typename STREAM>
    STREAM& streamIn(STREAM& stream);

    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
    operator const short *() const { return d_array_p; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// my_shortarray.cpp

my_ShortArray::my_ShortArray()
: d_size(INITIAL_SIZE)
, d_length(0)
{
    int sz = d_size * sizeof *d_array_p;
    d_array_p = (short *) operator new(sz);
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);

    operator delete(d_array_p);
}

static void reallocate(short **array, int *size,
                       int newSize, int length)
    // Reallocate memory in the specified 'array' and update the specified size
    // to the specified 'newSize.  The specified 'length' number of leading
    // elements are preserved.  The behavior is undefined unless 1 <= newSize,
    // 0 <= length, and newSize <= length.
{
    ASSERT(array);
    ASSERT(*array);             // this is not 'allocate'
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant

    short *tmp = *array;
    *array = (short *) operator new(newSize * sizeof **array);
    // COMMIT
    memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;
    operator delete(tmp);
}

void my_ShortArray::increaseSize()
{
    reallocate(&d_array_p, &d_size, d_size * GROW_FACTOR, d_length);
}

template <typename STREAM>
STREAM& my_ShortArray::streamIn(STREAM& stream)
{
    if (stream) {
        char version;
        stream >> version;
        if (!stream) {
            return stream;                                      // RETURN
        }

        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            int newLength;
            stream >> newLength;
            if (!stream) {
                return stream;                                  // RETURN
            }
            if (newLength < 0) {
                stream.invalidate();
                return stream;                                  // RETURN
            }
            if (newLength > d_size) {
                int newSize = newLength;
                reallocate(&d_array_p, &d_size, newSize, d_length);
            }
            ASSERT(newLength <= d_size);

            // No need to initialize any new elements (newLength > d_length).
            d_length = newLength;
            stream.getArrayInt16(d_array_p, d_length);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;                                              // RETURN
}

ostream& operator<<(ostream& stream, const my_ShortArray& array)
{
    stream << '[';
    const int len = array.length();
    for (int i = 0; i < len; ++i) {
        stream << ' ' << array[i];
    }
    return stream << " ]" << flush;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_testinstream.h

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;
/* TBD
class my_TestInStream {
    int   d_cursor;      // index for the next byte to be extracted
    char *d_buffer;      // stores byte values to be unexternalized
    int   d_length;      // number of bytes in 'd_buffer'
    int   d_validFlag;   // flag to indicate this stream's validity
    int   d_inputLimit;  // number of input op's before exception

  public:
    // CREATORS
    my_TestInStream(const char *buffer, int numBytes);
    ~my_TestInStream();

    // MANIPULATORS
    void invalidate()             { d_validFlag = 0;      }
    void setInputLimit(int limit) { d_inputLimit = limit; }
    void seek(int offset)         { d_cursor = offset;    }
    void reset()                  { d_cursor = 0;         }

    my_TestInStream& getLength(int& variable)                 { return *this; }
    my_TestInStream& getVersion(int& variable)                { return *this; }

    my_TestInStream& getInt64(Int64& variable)                { return *this; }
    my_TestInStream& getUint64(Uint64& variable)              { return *this; }
    my_TestInStream& getInt56(Int64& variable)                { return *this; }
    my_TestInStream& getUint56(Uint64& variable)              { return *this; }
    my_TestInStream& getInt48(Int64& variable)                { return *this; }
    my_TestInStream& getUint48(Uint64& variable)              { return *this; }
    my_TestInStream& getInt40(Int64& variable)                { return *this; }
    my_TestInStream& getUint40(Uint64& variable)              { return *this; }
    my_TestInStream& getInt32(int& variable);
    my_TestInStream& getUint32(unsigned int& variable)        { return *this; }
    my_TestInStream& getInt24(int& variable)                  { return *this; }
    my_TestInStream& getUint24(unsigned int& variable)        { return *this; }
    my_TestInStream& getInt16(short& variable)                { return *this; }
    my_TestInStream& getUint16(unsigned short& variable)      { return *this; }
    my_TestInStream& getInt8(char& variable);
    my_TestInStream& getInt8(signed char& variable)           { return *this; }
    my_TestInStream& getUint8(char& variable)                 { return *this; }
    my_TestInStream& getUint8(unsigned char& variable)        { return *this; }
    my_TestInStream& getFloat64(double& variable)             { return *this; }
    my_TestInStream& getFloat32(float& variable)              { return *this; }
    my_TestInStream& getArrayInt64(Int64 *array, int length)  { return *this; }
    my_TestInStream& getArrayUint64(Uint64 *array, int length){ return *this; }
    my_TestInStream& getArrayInt56(Int64 *array, int length)  { return *this; }
    my_TestInStream& getArrayUint56(Uint64 *array, int length){ return *this; }
    my_TestInStream& getArrayInt48(Int64 *array, int length)  { return *this; }
    my_TestInStream& getArrayUint48(Uint64 *array, int length){ return *this; }
    my_TestInStream& getArrayInt40(Int64 *array, int length)  { return *this; }
    my_TestInStream& getArrayUint40(Uint64 *array, int length){ return *this; }
    my_TestInStream& getArrayInt32(int *array, int length)    { return *this; }
    my_TestInStream& getArrayUint32(unsigned int *array, int length)
                                                            { return *this; }
    my_TestInStream& getArrayInt24(int *array, int length)    { return *this; }
    my_TestInStream& getArrayUint24(unsigned int *array, int length)
                                                            { return *this; }
    my_TestInStream& getArrayInt16(short *array, int length);
    my_TestInStream& getArrayUint16(unsigned short *array, int length)
                                                            { return *this; }
    my_TestInStream& getArrayInt8(char *array, int length)    { return *this; }
    my_TestInStream& getArrayInt8(signed char *array, int length)
                                                            { return *this; }
    my_TestInStream& getArrayUint8(char *array, int length)   { return *this; }
    my_TestInStream& getArrayUint8(unsigned char *array, int length)
                                                            { return *this; }
    my_TestInStream& getArrayFloat64(double *array, int count){return *this; }
    my_TestInStream& getArrayFloat32(float *array, int length){return *this; }

    // ACCESSORS
    operator const void *() const { return d_validFlag ? this : 0; }
    bool isEmpty() const          { return d_length <= d_cursor;   }
    int inputLimit() const        { return d_inputLimit;           }
    int cursor() const            { return d_cursor;               }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_testinstream.cpp

enum {
    SIZEOF_INT64   = 8,
    SIZEOF_INT56   = 7,
    SIZEOF_INT48   = 6,
    SIZEOF_INT40   = 5,
    SIZEOF_INT32   = 4,
    SIZEOF_INT24   = 3,
    SIZEOF_INT16   = 2,
    SIZEOF_INT8    = 1,
    SIZEOF_FLOAT64 = 8,
    SIZEOF_FLOAT32 = 4,
    SIZEOF_CODE    = SIZEOF_INT8,
    SIZEOF_VERSION = SIZEOF_INT8,
    SIZEOF_ARRLEN  = SIZEOF_INT32
};

my_TestInStream::my_TestInStream(const char *buffer, int numBytes)
: d_cursor(0)
, d_length(numBytes)
, d_validFlag(1)
, d_inputLimit(-1)
{
    d_buffer = (char *) operator new(numBytes);
    memcpy(d_buffer, buffer, numBytes);
}

my_TestInStream::~my_TestInStream()
{
    ASSERT(d_cursor <= d_length);
    operator delete(d_buffer);
}

my_TestInStream& my_TestInStream::getInt32(int& variable)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(bdex_FieldCode::BDEX_INT32);
        }
    }
#endif
    if (d_validFlag &&
        (d_validFlag = d_cursor + SIZEOF_INT32 <= d_length)) {
        bdex_ByteStreamImpUtil::getInt32(&variable, &d_buffer[d_cursor]);
        d_cursor += SIZEOF_INT32;
    }
    return *this;
}

my_TestInStream& my_TestInStream::getInt8(char& variable)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(bdex_FieldCode::BDEX_INT8);
        }
    }
#endif
    if (d_validFlag &&
        (d_validFlag = d_cursor + SIZEOF_INT8 <= d_length)) {
        bdex_ByteStreamImpUtil::getInt8(&variable, &d_buffer[d_cursor]);
        d_cursor += SIZEOF_INT8;
    }
    return *this;
}

my_TestInStream& my_TestInStream::getArrayInt16(short *array, int length)
{
    ASSERT(array);
    ASSERT(0 <= length);
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw bdex_TestInStreamException(bdex_FieldCode::BDEX_INT16);
        }
    }
#endif
    if (d_validFlag &&
        (d_validFlag = d_cursor + SIZEOF_INT16 * length <= d_length)) {
        bdex_ByteStreamImpUtil::getArrayInt16(array, &d_buffer[d_cursor],
                                              length);
        d_cursor += SIZEOF_INT16 * length;
    }
    return *this;
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.t.cpp

typedef short Element;
const Element VALUES[] = { 1, 2, 3, 4, -5 };
const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

const Element &V0 = VALUES[0], &VA = V0,
              &V1 = VALUES[1], &VB = V1,
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,
              &V4 = VALUES[4], &VE = V4;

#if 0  // not used
static bool areEqual(const short *array1, const short *array2, int numElement)
    // Return 'true' if the specified initial 'numElement' in the specified
    // 'array1' and 'array2' have the same values, and 'false' otherwise.
{
    for (int i = 0; i < numElement; ++i) {
        if (array1[i] != array2[i]) return 0;
    }
    return 1;
}
#endif

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Verify that the usage example is free of syntax errors and works
        //   as advertised.
        //
        // Testing:
        //   Make sure main usage example compiles and works.
        // --------------------------------------------------------------------
      /* TBD
        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        const int MAX_BYTES = 1000;
        struct {
            int   d_line;
            int   d_numBytes;         // number of bytes in input stream
            char  d_bytes[MAX_BYTES]; // data in bytes
            int   d_expNumElem;       // expected number of elements
            short d_exp[NUM_VALUES];  // expected element values
        } DATA[] = {
   //Line  # bytes  bytes                   # elem  expected array
   //----  -------  ----------------------  ------  --------------
    { L_,     5,    "\x01\x00\x00\x00\x00",    0,   { 0 }                },
    { L_,    11,    "\x01\x00\x00\x00\x01"
                    "\x00\x01",                1,   { V0 }               },
    { L_,    19,    "\x01\x00\x00\x00\x05"
                    "\x00\x01\x00\x02"
                    "\x00\x03\x00\x04"
                    "\xff\xfb",                5,   { V0, V1, V2, V3, V4 }}
        };

        const int NUM_TEST = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            const int    LINE      = DATA[ti].d_line;
            const int    NUM_BYTES = DATA[ti].d_numBytes;
            const char  *BYTES     = DATA[ti].d_bytes;
            const int    NUM_ELEM  = DATA[ti].d_expNumElem;
            const short *EXP       = DATA[ti].d_exp;

            my_TestInStream testInStream(BYTES, NUM_BYTES);

            BEGIN_BDEX_EXCEPTION_TEST { testInStream.reset();
                my_ShortArray mA;  const my_ShortArray& A = mA;
                mA.streamIn(testInStream);
                if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
                LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
            } END_BDEX_EXCEPTION_TEST
        }
        */
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Create 'bdex_TestInStreamException' objects with varying initial
        //   values.  Verify that each object contains the expected value using
        //   basic accessor 'dataType'.
        //
        // Testing:
        //   bdex_TestInStreamException(typeCode);
        //   ~bdex_TestInStreamException();
        //   bdex_FieldCode::Type dataType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing ctor and accessor." << endl;
        {
            typedef bdex_FieldCode FC;
            const FC::Type DATA[] = {
                FC::BDEX_INT8,     FC::BDEX_UINT8,
                FC::BDEX_INT16,    FC::BDEX_UINT16,
                FC::BDEX_INT24,    FC::BDEX_UINT24,
                FC::BDEX_INT32,    FC::BDEX_UINT32,
                FC::BDEX_INT40,    FC::BDEX_UINT40,
                FC::BDEX_INT48,    FC::BDEX_UINT48,
                FC::BDEX_INT56,    FC::BDEX_UINT56,
                FC::BDEX_INT64,    FC::BDEX_UINT64,
                FC::BDEX_FLOAT32,  FC::BDEX_FLOAT64,
                FC::BDEX_INVALID,  (FC::Type) 0, (FC::Type) 100
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_DATA; ++i) {
                const bdex_TestInStreamException X(DATA[i]);
                LOOP_ASSERT(i, DATA[i] == X.dataType());
            }
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
