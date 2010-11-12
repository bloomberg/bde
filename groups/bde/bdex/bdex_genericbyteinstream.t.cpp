// bdex_genericbyteinstream.t.cpp              -*-C++-*-

#include <bdex_genericbyteinstream.h>
#include <bdex_genericbyteoutstream.h>          // for testing only

#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_instreamfunctions.h>             // for testing only

#include <bdesb_fixedmeminstreambuf.h>
#include <bdesb_fixedmeminput.h>
#include <bdesb_fixedmemoutstreambuf.h>
#include <bdesb_fixedmemoutput.h>
#include <bsls_platformutil.h>                  // for testing only
#include <bsls_stopwatch.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy(), memcmp(), strlen()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsls_assert.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// For all input methods in 'bdex_GenericByteInStream', the "unexternalization"
// from byte representation to the correct fundamental-type value is delegated
// to another component.  We assume that this process has been rigorously
// tested and verified.  Therefore, we are concerned only with the placement of
// the next-byte-position cursor and the alignment of bytes in the input
// stream.  For each input method, we verify these properties by first creating
// a 'bdex_GenericByteOutStream' object 'oX' containing some arbitrarily chosen
// values.  The values are interleaved with chosen "marker" bytes to check for
// alignment issues.  We then create a 'bdex_GenericByteInStream' object 'iX'
// initialized with the content of 'oX', and consecutively call the input
// method on 'iX' to verify that the extracted values and marker bytes equal to
// their respective chosen values.  After all values are extracted, we verify
// that the stream is valid, empty, and the cursor is properly placed.
//
// The class under test is a class template, so we choose to test it with a
// 'bdesb_FixedMemInput' parameter, but test case [24] verifies that it can be
// instantiated by a class that has the minimal requirements.
//
// Also since the main purpose of this component was for performance
// improvement, we have put in a performance test (test case [-1]).  A typical
// output is (with NUM_ITER == 1000000):
//
// With bdesb_FixedMemIn/Output:
//        Test performance on ctor+putInt8+dtor.
//        in 0.365789seconds
//        Test performance on ctor+putInt32+dtor.
//        in 0.466552seconds
//
// With bdesb_FixedMemIn/OutStreamBuf:
//        Test performance on ctor+putInt8+dtor.
//        in 1.05741seconds
//        Test performance on ctor+putInt32+dtor.
//        in 1.33651seconds
//-----------------------------------------------------------------------------
// [23] getLength(int& variable);
// [23] getVersion(int& variable);
// [22] getArrayFloat64(double *array, int numValues);
// [21] getArrayFloat32(float *array, int numValues);
// [20] getArrayInt64(bsls_PlatformUtil::Int64 *array, int numValues);
// [20] getArrayUint64(bsls_PlatformUtil::Uint64 *array, int numValues);
// [19] getArrayInt56(bsls_PlatformUtil::Int64 *array, int numValues);
// [19] getArrayUint56(bsls_PlatformUtil::Uint64 *array, int numValues);
// [18] getArrayInt48(bsls_PlatformUtil::Int64 *array, int numValues);
// [18] getArrayUint48(bsls_PlatformUtil::Uint64 *array, int numValues);
// [17] getArrayInt40(bsls_PlatformUtil::Int64 *array, int numValues);
// [17] getArrayUint40(bsls_PlatformUtil::Uint64 *array, int numValues);
// [16] getArrayInt32(int *array, int numValues);
// [16] getArrayUint32(unsigned int *array, int numValues);
// [15] getArrayInt24(int *array, int numValues);
// [15] getArrayUint24(unsigned int *array, int numValues);
// [14] getArrayInt16(short *array, int numValues);
// [14] getArrayUint16(unsigned short *array,int numValues);
// [13] getArrayInt8(char *array, int numValues);
// [13] getArrayInt8(signed char *array, int numValues);
// [13] getArrayUint8(char *array, int numValues);
// [13] getArrayUint8(unsigned char *array, int numValues);
// [12] getFloat64(double& variable);
// [11] getFloat32(float& variable);
// [10] getInt64(bsls_PlatformUtil::Int64& variable);
// [10] getUint64(bsls_PlatformUtil::Uint64& variable);
// [ 9] getInt56(bsls_PlatformUtil::Int64& variable);
// [ 9] getUint56(bsls_PlatformUtil::Uint64& variable);
// [ 8] getInt48(bsls_PlatformUtil::Int64& variable);
// [ 8] getUint48(bsls_PlatformUtil::Uint64& variable);
// [ 7] getInt40(bsls_PlatformUtil::Int64& variable);
// [ 7] getUint40(bsls_PlatformUtil::Uint64& variable);
// [ 6] getInt32(int& variable);
// [ 6] getUint32(unsigned int& variable);
// [ 5] getInt24(int& variable);
// [ 5] getUint24(unsigned int& variable);
// [ 4] getInt16(short& variable);
// [ 4] getUint16(unsigned short& variable);
// [ 3] getInt8(signed char& variable);
// [ 3] getUint8(char& variable);
// [ 3] getUint8(unsigned char& variable);
// [ 2] void invalidate();
// [ 2] getInt8(char& variable);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//=============================================================================

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
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                    STANDARD BDE OUTPUT TEST MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

const int SIZEOF_INT64   = 8;
const int SIZEOF_INT32   = 4;
const int SIZEOF_INT16   = 2;
const int SIZEOF_INT8    = 1;
const int SIZEOF_FLOAT64 = 8;
const int SIZEOF_FLOAT32 = 4;

//=============================================================================
//                      HELPER CLASSES AND FUNCTIONS FOR GENERICS
//-----------------------------------------------------------------------------
namespace genericTest {

class my_FixedMemInStreamBuf {
    // This class implements a very basic 'bdesb_FixedMemInput' suitable
    // for use in 'bdex_GenericByteInStream'.

  private:
    const char      *d_buffer_p; // input buffer
    bsl::streamsize  d_length;   // length of input buffer
    bsl::streamsize  d_pos;      // input cursor

  public:
    // CREATORS
    my_FixedMemInStreamBuf(const char *buffer, bsl::streamsize length);
        // Create an empty stream buffer that reads from the specified
        // character 'buffer' of the specified 'length'.  The behavior is
        // undefined unless 'buffer' is not zero and 0 < 'length'.  Note that
        // 'buffer' is held but not owned.

    ~my_FixedMemInStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    const char *data();
        // Return the address of the non-modifiable character buffer held by
        // this stream buffer.

    my_FixedMemInStreamBuf* pubsetbuf(const char      *buffer,
                                      bsl::streamsize  length);
        // Reset the internal buffer of this stream to the specified 'buffer'
        // of the specified 'length'.  Note that the next write operation will
        // start at the beginning of 'buffer'.

    int sbumpc();
        // Return the character at the current read position from this buffer
        // and advance the current read position by one, or
        // 'char_traits<char>::eof()' if the current read position is past the
        // input buffer end.

    int sgetc();
        // Return the character at the current read position from this buffer,
        // or 'char_traits<char>::eof()' if the current read position is past
        // the input buffer end.

    bsl::streamsize sgetn(char *s, bsl::streamsize length);
        // Write the specified 'length' characters at the current read
        // position from this buffer to the specified address 's' and advance
        // the current read position.  Return the number of characters
        // actually written.

    // ACCESSORS
    bsl::streamsize length() const;
        // Return the number of characters available in this stream buffer.
};

// CREATORS
my_FixedMemInStreamBuf::my_FixedMemInStreamBuf(const char      *buffer,
                                               bsl::streamsize  length)
: d_buffer_p(buffer)
, d_length(length)
, d_pos(0)
{
    BSLS_ASSERT(d_length > 0);
}

my_FixedMemInStreamBuf::~my_FixedMemInStreamBuf()
{
}

// MANIPULATORS
const char *my_FixedMemInStreamBuf::data()
{
    return d_buffer_p;
}

my_FixedMemInStreamBuf*
my_FixedMemInStreamBuf::pubsetbuf(const char *buffer, bsl::streamsize length)
{
    BSLS_ASSERT(d_length > 0);
    d_buffer_p = buffer;
    d_length = length;
    d_pos = 0;

    return this;
}

int my_FixedMemInStreamBuf::sbumpc()
{
    if (d_pos >= d_length) {
        return char_traits<char>::eof();
    }
    return d_buffer_p[d_pos++];
}

int my_FixedMemInStreamBuf::sgetc()
{
    if (d_pos >= d_length) {
        return char_traits<char>::eof();
    }
    return d_buffer_p[d_pos];
}

bsl::streamsize my_FixedMemInStreamBuf::sgetn(char            *s,
                                              bsl::streamsize  length)
{
    if (d_pos + length > d_length) {
        length = d_length - d_pos;
    }
    bsl::memcpy(s, d_buffer_p + d_pos, length);
    d_pos += length;
    return length;
}

// ACCESSORS
bsl::streamsize my_FixedMemInStreamBuf::length() const
{
    return d_pos;
}

typedef bdex_GenericByteInStream<my_FixedMemInStreamBuf> GObj;

} // closing namespace
//=============================================================================
//                  USAGE EXAMPLE
//-----------------------------------------------------------------------------
class my_Person {
    bsl::string d_firstName;
    bsl::string d_lastName;
    int         d_age;

    friend bool operator==(const my_Person&, const my_Person&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion(void) { return 1; }
        // Return the most current 'bdex' streaming version number supported
        // by this class.  (See the 'bdex' package-level documentation for
        // more information on 'bdex' streaming of container types.)

    // CREATORS
    my_Person();
        // Create a default person.

    my_Person(const char *firstName, const char *lastName, int age);
        // Create a person having the specified 'firstName', 'lastName', and
        // 'age'.

    my_Person(const my_Person& original);
        // Create a person having value of the specified 'original' person.

    ~my_Person();
        // Destroy this object.

    // MANIPULATORS
    my_Person& operator=(const my_Person& rhs);
        // Assign to this person the value of the specified 'rhs' person.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes
        // invalid during this operation, this object is valid, but its
        // value is undefined.  If the specified 'version' is not supported,
        // 'stream' is marked invalid, but this object is unaltered.  Note
        // that no version is read from 'stream'.  (See the 'bdex'
        // package-level documentation for more information on 'bdex'
        // streaming of container types.)

    // Other manipulators omitted.

    // ACCESSORS
    const bsl::string& firstName() const;
        // Return the first name of this person.

    const bsl::string& lastName() const;
        // Return the last name of this person.

    int age() const;
        // Return the age of this person.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  Optionally specify an
        // explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If
        // 'version' is specified, that format is used but *not* written to
        // 'stream'.  If 'version' is not supported, 'stream' is left
        // unmodified.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of container types).

   // Other accessors omitted.

};

// FREE OPERATORS
inline
bool operator==(const my_Person& lhs, const my_Person& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' person objects have the
    // same value and 'false' otherwise.  Two person objects have the same
    // value if they have the same first name, last name, and age.

inline
bool operator!=(const my_Person& lhs, const my_Person& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
    // have the same value and 'false' otherwise.  Two person objects differ in
    // value if they differ in first name, last name, or age.

bsl::ostream& operator<<(bsl::ostream& stream, const my_Person& person);
    // Write the specified 'date' value to the specified output 'stream' in
    // some reasonable format.

                         // INLINE FUNCTION DEFINITIONS
my_Person::my_Person(const char *first, const char *last, int age)
  : d_firstName(first), d_lastName(last), d_age(age)
{}

my_Person::my_Person(void)
{}

my_Person::~my_Person(void)
{}

const bsl::string& my_Person::firstName() const
{ return (const bsl::string &)d_firstName; }

const bsl::string& my_Person::lastName() const
{ return (const bsl::string &)d_lastName; }

int my_Person::age() const { return d_age; }

template <class STREAM>
inline
STREAM& my_Person::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {    // switch on the 'bdex' version
          case 1: {
              stream.getString(d_firstName);
              if (!stream) {
                  d_firstName = "stream error";  // *might* be corrupted;
                                                 //  value for testing
                  return stream;
              }
              stream.getString(d_lastName);
              if (!stream) {
                  d_lastName = "stream error";  // *might* be corrupted;
                                                //  value for testing
                  return stream;
              }
              stream.getInt32(d_age);
              if (!stream) {
                 d_age = 1;      // *might* be corrupted; value for testing
                  return stream;
              }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& my_Person::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
          stream.putString(d_firstName);
          stream.putString(d_lastName);
          stream.putInt32(d_age);
      } break;
    }
    return stream;
}

inline
bool operator==(const my_Person& lhs, const my_Person& rhs)
{
    if(&lhs == &rhs) {
       return true;
    }

    return( (lhs.d_firstName == rhs.d_firstName) &&
            (lhs.d_lastName == rhs.d_lastName) &&
            (lhs.d_age == rhs.d_age) );
}

inline
bool operator!=(const my_Person& lhs, const my_Person& rhs)
{
     return( !(lhs == rhs));
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 25: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        // --------------------------------------------------------------------

        my_Person JaneSmith("Jane", "Smith", 42);
        char buffer[200];
        bdesb_FixedMemOutStreamBuf osb(buffer, 200);
        bdex_GenericByteOutStream<bdesb_FixedMemOutStreamBuf> outStream(&osb);
        const int VERSION = 1;
        outStream.putVersion(VERSION);
        bdex_OutStreamFunctions::streamOut(outStream, JaneSmith, VERSION);

        my_Person janeCopy;
        ASSERT(janeCopy != JaneSmith);
        bdesb_FixedMemInput sb(buffer, osb.length());
        bdex_GenericByteInStream<bdesb_FixedMemInput> inStream(&sb);
        int version;
        inStream.getVersion(version);
        bdex_InStreamFunctions::streamIn(inStream, janeCopy, version);
        ASSERT(janeCopy == JaneSmith);

        // Verify the results on 'stdout'.
        if (janeCopy == JaneSmith) {
            if (verbose)
            cout << "Successfully serialized and de-serialized Jane Smith:"
                 << "\n\tFirstName: " << janeCopy.firstName()
                 << "\n\tLastName : " << janeCopy.lastName()
                 << "\n\tAge      : " << janeCopy.age() << endl;
        }
        else {
            if (verbose)
            cout << "Serialization unsuccessful.  janeCopy holds:"
                 << "\n\tFirstName: " << janeCopy.firstName()
                 << "\n\tLastName : " << janeCopy.lastName()
                 << "\n\tAge      : " << janeCopy.age() << endl;
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // CLASS 'bdex_GenericByteInStream'
        //
        // Concerns:
        //   That 'bdex_GenericBdexByteInStream' can be parameterized
        //   by a bare-bones 'bsl::streambuf' implementation that only provides
        //   'sbumpc', 'sgetc' and 'sgetn'.
        //
        // Plan:
        //   Pass a bare-bone buffered stream (fixed-length).
        //
        // Testing:
        //   CLASS 'bdex_GenericBdexByteInStream'
        // --------------------------------------------------------------------

        if (verbose)
         cout << "\nBREATHING TEST FOR 'bdex_GenericBdexByteInStream'"
              << "\n================================================="
              << endl;

        using namespace genericTest;

        if (verbose)
            cout << "\nCreate bdex_GenericByteInStream x1 using default ctor."
                 << endl;
        int i;
        char workSpace[1];
        my_FixedMemInStreamBuf sb(workSpace, 1);
        bdex_GenericByteInStream<my_FixedMemInStreamBuf> x1(&sb);

        if (verbose)
            cout << "\nCreate bdex_GenericByteInStream x2 w/ an initial value."
                 << endl;
        my_FixedMemInStreamBuf sb2(const_cast<char *>("\x00\x01\x02\x03\x04"),
                                   5);
        bdex_GenericByteInStream<my_FixedMemInStreamBuf> x2(&sb2);

        if (verbose) cout << "\nTry getInt8() with x2." << endl;
        for (i = 0; i < 5; i++) {
            char c;
            x2.getInt8(c);
            LOOP_ASSERT(i, i == c);
        }

        if (verbose) cout << "\nTry invalidate() with x2." << endl;
        x2.invalidate();
        ASSERT(!x2);

        if (verbose) cout << "\nTry invalid operation with x1." << endl;
        x1.getInt32(i);
        ASSERT(!x1);

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // GET LENGTH AND VERSION:
        //
        // Testing:
        //   getLength(int& variable);
        //   getVersion(int& variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET LENGTH AND VERSION TEST" << endl
                          << "===========================" << endl;


        if (verbose) cout << "\nTesting getLength(int&)." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putLength(1);             o.putInt8(0xFF);
            o.putLength(128);           o.putInt8(0xFE);
            o.putLength(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 9);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int val;
            x.getLength(val);          x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getLength(val);          x.getInt8(marker);
            ASSERT(128 == val);        ASSERT('\xFE' == marker);
            x.getLength(val);          x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getLength(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putLength(128);             o.putInt8(0xFD);
            o.putLength(127);             o.putInt8(0xFE);
            o.putLength(256);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 12);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int val;
            x.getLength(val);          x.getInt8(marker);
            ASSERT(128 == val);        ASSERT('\xFD' == marker);
            x.getLength(val);          x.getInt8(marker);
            ASSERT(127 == val);        ASSERT('\xFE' == marker);
            x.getLength(val);          x.getInt8(marker);
            ASSERT(256 == val);        ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getLength(val);
            ASSERT(!x);
        }

        if (verbose) cout << "\nTesting getVersion(int&)." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putVersion(1);
            o.putVersion(2);
            o.putVersion(3);
            o.putVersion(4);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            int val;
            x.getVersion(val);            ASSERT(1 == val);
            x.getVersion(val);            ASSERT(2 == val);
            x.getVersion(val);            ASSERT(3 == val);
            x.getVersion(val);            ASSERT(4 == val);
            ASSERT(x);

            x.getVersion(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putVersion(252);
            o.putVersion(253);
            o.putVersion(254);
            o.putVersion(255);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            int val;
            x.getVersion(val);            ASSERT(252 == val);
            x.getVersion(val);            ASSERT(253 == val);
            x.getVersion(val);            ASSERT(254 == val);
            x.getVersion(val);            ASSERT(255 == val);
            ASSERT(x);

            x.getVersion(val);
            ASSERT(!x);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // GET 64-BIT FLOAT ARRAYS TEST:
        //
        // Testing:
        //   getArrayFloat64(double *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 64-BIT FLOAT ARRAYS TEST" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting getArrayFloat64." << endl;
        {
            const double DATA[] = { 1, 2, 3 };
            const double V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayFloat64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat64(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 52);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            double ar[] = { V, V, V };
            x.getArrayFloat64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayFloat64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayFloat64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayFloat64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayFloat64(ar, 0);
            ASSERT(x);

            x.getArrayFloat64(ar, 1);
            ASSERT(!x);
        }
        {
            const double DATA[] = { 4, 5, 6 };
            const double V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayFloat64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat64(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 52);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            double ar[] = { V, V, V };
            x.getArrayFloat64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayFloat64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayFloat64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayFloat64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayFloat64(ar, 0);
            ASSERT(x);

            x.getArrayFloat64(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // GET 32-BIT FLOAT ARRAYS TEST:
        //
        // Testing:
        //   getArrayFloat32(float *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 32-BIT FLOAT ARRAYS TEST" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting getArrayFloat32." << endl;
        {
            const float DATA[] = { 1, 2, 3 };
            const float V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayFloat32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat32(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 28);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            float ar[] = { V, V, V };
            x.getArrayFloat32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayFloat32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayFloat32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayFloat32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayFloat32(ar, 0);
            ASSERT(x);

            x.getArrayFloat32(ar, 1);
            ASSERT(!x);
        }
        {
            const float DATA[] = { 4, 5, 6 };
            const float V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayFloat32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat32(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 28);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            float ar[] = { V, V, V };
            x.getArrayFloat32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayFloat32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayFloat32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayFloat32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayFloat32(ar, 0);
            ASSERT(x);

            x.getArrayFloat32(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // GET 64-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt64(bsls_PlatformUtil::Int64 *array, int numValues);
        //   getArrayUint64(bsls_PlatformUtil::Uint64 *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 64-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt64." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt64(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 54);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt64(ar, 0);
            ASSERT(x);

            x.getArrayInt64(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt64(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 54);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt64(ar, 0);
            ASSERT(x);

            x.getArrayInt64(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint64." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint64(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint64(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint64(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint64(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 54);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint64(ar, 0);
            ASSERT(x);

            x.getArrayUint64(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint64(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint64(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint64(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint64(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 54);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint64(ar, 0);
            ASSERT(x);

            x.getArrayUint64(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // GET 56-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt56(bsls_PlatformUtil::Int64 *array, int numValues);
        //   getArrayUint56(bsls_PlatformUtil::Uint64 *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 56-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt56." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt56(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt56(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt56(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt56(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 46);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt56(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt56(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt56(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt56(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt56(ar, 0);
            ASSERT(x);

            x.getArrayInt56(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt56(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt56(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt56(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt56(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 46);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt56(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt56(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt56(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt56(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt56(ar, 0);
            ASSERT(x);

            x.getArrayInt56(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint56." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint56(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint56(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint56(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint56(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 46);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint56(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint56(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint56(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint56(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint56(ar, 0);
            ASSERT(x);

            x.getArrayUint56(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint56(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint56(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint56(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint56(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 46);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint56(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint56(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint56(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint56(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint56(ar, 0);
            ASSERT(x);

            x.getArrayUint56(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // GET 48-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt48(bsls_PlatformUtil::Int64 *array, int numValues);
        //   getArrayUint48(bsls_PlatformUtil::Uint64 *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 48-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt48." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt48(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt48(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt48(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt48(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 40);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt48(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt48(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt48(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt48(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt48(ar, 0);
            ASSERT(x);

            x.getArrayInt48(ar, 1);
            ASSERT(!x);

        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt48(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt48(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt48(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt48(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 40);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt48(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt48(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt48(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt48(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt48(ar, 0);
            ASSERT(x);

            x.getArrayInt48(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint48." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint48(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint48(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint48(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint48(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 40);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint48(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint48(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint48(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint48(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint48(ar, 0);
            ASSERT(x);

            x.getArrayUint48(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint48(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint48(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint48(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint48(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 40);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint48(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint48(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint48(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint48(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint48(ar, 0);
            ASSERT(x);

            x.getArrayUint48(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // GET 40-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt40(bsls_PlatformUtil::Int64 *array, int numValues);
        //   getArrayUint40(bsls_PlatformUtil::Uint64 *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 40-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt40." << endl;
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt40(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt40(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt40(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt40(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 34);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt40(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt40(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt40(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt40(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt40(ar, 0);
            ASSERT(x);

            x.getArrayInt40(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt40(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt40(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt40(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt40(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 34);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 ar[] = { V, V, V };
            x.getArrayInt40(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt40(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt40(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt40(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt40(ar, 0);
            ASSERT(x);

            x.getArrayInt40(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint40." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint40(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint40(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint40(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint40(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 34);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint40(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint40(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint40(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint40(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint40(ar, 0);
            ASSERT(x);

            x.getArrayUint40(ar, 1);
            ASSERT(!x);
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint40(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint40(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint40(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint40(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 34);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 ar[] = { V, V, V };
            x.getArrayUint40(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint40(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint40(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint40(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint40(ar, 0);
            ASSERT(x);

            x.getArrayUint40(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // GET 32-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt32(int *array, int numValues);
        //   getArrayUint32(unsigned int *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 32-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt32." << endl;
        {
            const int DATA[] = { 1, 2, 3 };
            const int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt32(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 28);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int ar[] = { V, V, V };
            x.getArrayInt32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt32(ar, 0);
            ASSERT(x);

            x.getArrayInt32(ar, 1);
            ASSERT(!x);
        }
        {
            const int DATA[] = { 4, 5, 6 };
            const int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt32(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 28);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int ar[] = { V, V, V };
            x.getArrayInt32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt32(ar, 0);
            ASSERT(x);

            x.getArrayInt32(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint32." << endl;
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint32(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint32(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint32(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint32(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 28);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int ar[] = { V, V, V };
            x.getArrayUint32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint32(ar, 0);
            ASSERT(x);

            x.getArrayUint32(ar, 1);
            ASSERT(!x);
        }
        {
            const unsigned int DATA[] = { 4, 5, 6 };
            const unsigned int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint32(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint32(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint32(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint32(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 28);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int ar[] = { V, V, V };
            x.getArrayUint32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint32(ar, 0);
            ASSERT(x);

            x.getArrayUint32(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // GET 24-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt24(int *array, int numValues);
        //   getArrayUint24(unsigned int *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 24-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt24." << endl;
        {
            const int DATA[] = { 1, 2, 3 };
            const int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt24(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt24(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt24(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt24(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 22);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int ar[] = { V, V, V };
            x.getArrayInt24(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt24(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt24(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt24(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt24(ar, 0);
            ASSERT(x);

            x.getArrayInt24(ar, 1);
            ASSERT(!x);
        }
        {
            const int DATA[] = { 4, 5, 6 };
            const int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt24(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt24(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt24(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt24(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 22);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int ar[] = { V, V, V };
            x.getArrayInt24(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt24(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt24(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt24(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt24(ar, 0);
            ASSERT(x);

            x.getArrayInt24(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint24." << endl;
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint24(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint24(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint24(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint24(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 22);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int ar[] = { V, V, V };
            x.getArrayUint24(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint24(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint24(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint24(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint24(ar, 0);
            ASSERT(x);

            x.getArrayUint24(ar, 1);
            ASSERT(!x);
        }
        {
            const unsigned int DATA[] = { 4, 5, 6 };
            const unsigned int V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint24(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint24(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint24(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint24(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 22);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int ar[] = { V, V, V };
            x.getArrayUint24(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint24(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint24(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint24(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint24(ar, 0);
            ASSERT(x);

            x.getArrayUint24(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // GET 16-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //   getArrayInt16(short *array, int numValues);
        //   getArrayUint16(unsigned short *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 16-BIT INTEGER ARRAYS TEST" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt16." << endl;
        {
            const short DATA[] = { 1, 2, 3 };
            const short V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt16(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt16(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt16(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt16(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 16);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            short ar[] = { V, V, V };
            x.getArrayInt16(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt16(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt16(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt16(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt16(ar, 0);
            ASSERT(x);

            x.getArrayInt16(ar, 1);
            ASSERT(!x);
        }
        {
            const short DATA[] = { 4, 5, 6 };
            const short V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt16(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt16(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt16(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt16(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 16);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            short ar[] = { V, V, V };
            x.getArrayInt16(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt16(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt16(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt16(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt16(ar, 0);
            ASSERT(x);

            x.getArrayInt16(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint16." << endl;
        {
            const unsigned short DATA[] = { 1, 2, 3 };
            const unsigned short V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint16(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint16(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint16(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint16(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 16);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned short ar[] = { V, V, V };
            x.getArrayUint16(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint16(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint16(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint16(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint16(ar, 0);
            ASSERT(x);

            x.getArrayUint16(ar, 1);
            ASSERT(!x);
        }
        {
            const unsigned short DATA[] = { 4, 5, 6 };
            const unsigned short V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint16(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint16(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint16(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint16(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 16);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned short ar[] = { V, V, V };
            x.getArrayUint16(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint16(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint16(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint16(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint16(ar, 0);
            ASSERT(x);

            x.getArrayUint16(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // GET 8-BIT INTEGER ARRAYS TEST:
        //
        // Testing:
        //    getArrayInt8(char *array, int numValues);
        //    getArrayInt8(signed char *array, int numValues);
        //    getArrayUint8(char *array, int numValues);
        //    getArrayUint8(unsigned char *array, int numValues);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 8-BIT INTEGER ARRAYS TEST" << endl
                          << "=============================" << endl;

        if (verbose) cout << "\nTesting getArrayInt8(char*, int)." << endl;
        {
            const char DATA[] = { 1, 2, 3 };
            const char V = (char) 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);

            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            char ar[] = { V, V, V };
            x.getArrayInt8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt8(ar, 0);
            ASSERT(x);

            x.getArrayInt8(ar, 1);
            ASSERT(!x);
        }
        {
            const char DATA[] = { 4, 5, 6 };
            const char V = (char) 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            char ar[] = { V, V, V };
            x.getArrayInt8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt8(ar, 0);
            ASSERT(x);

            x.getArrayInt8(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting getArrayInt8(signed char*, int)." << endl;
        {
            const signed char DATA[] = { 1, 2, 3 };
            const signed char V = (char) 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);

            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            signed char ar[] = { V, V, V };
            x.getArrayInt8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt8(ar, 0);
            ASSERT(x);

            x.getArrayInt8(ar, 1);
            ASSERT(!x);
        }
        {
            const signed char DATA[] = { 4, 5, 6 };
            const signed char V = (char) 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            signed char ar[] = { V, V, V };
            x.getArrayInt8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayInt8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayInt8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayInt8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayInt8(ar, 0);
            ASSERT(x);

            x.getArrayInt8(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint8(char*, int)." << endl;
        {
            const char DATA[] = { 1, 2, 3 };
            const char V = (char) 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            char ar[] = { V, V, V };
            x.getArrayUint8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint8(ar, 0);
            ASSERT(x);

            x.getArrayUint8(ar, 1);
            ASSERT(!x);
        }
        {
            const char DATA[] = { 4, 5, 6 };
            const char V = (char) 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            char ar[] = { V, V, V };
            x.getArrayUint8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint8(ar, 0);
            ASSERT(x);

            x.getArrayUint8(ar, 1);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting getArrayUint8(unsigned char*, int)." << endl;
        {
            const unsigned char DATA[] = { 1, 2, 3 };
            const unsigned char V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned char ar[] = { V, V, V };
            x.getArrayUint8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint8(ar, 0);
            ASSERT(x);

            x.getArrayUint8(ar, 1);
            ASSERT(!x);
        }
        {
            const unsigned char DATA[] = { 4, 5, 6 };
            const unsigned char V = 0xFF;

            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            bdesb_FixedMemInput isb(workSpace, 10);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned char ar[] = { V, V, V };
            x.getArrayUint8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFF' == marker);

            x.getArrayUint8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFE' == marker);

            x.getArrayUint8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            x.getInt8(marker);            ASSERT('\xFD' == marker);

            x.getArrayUint8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            x.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(x);

            x.getArrayUint8(ar, 0);
            ASSERT(x);

            x.getArrayUint8(ar, 1);
            ASSERT(!x);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // GET 64-BIT FLOATS TEST:
        //
        // Testing:
        //   getFloat64(double &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 64-BIT FLOATS TEST" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting getFloat64." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putFloat64(1);           o.putInt8(0xFF);
            o.putFloat64(2);           o.putInt8(0xFE);
            o.putFloat64(3);           o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 27);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            double val;
            x.getFloat64(val);         x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getFloat64(val);         x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getFloat64(val);         x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getFloat64(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putFloat64(4);           o.putInt8(0xFD);
            o.putFloat64(5);           o.putInt8(0xFE);
            o.putFloat64(6);           o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 27);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            double val;
            x.getFloat64(val);         x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getFloat64(val);         x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getFloat64(val);         x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getFloat64(val);
            ASSERT(!x);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GET 32-BIT FLOATS TEST:
        //
        // Testing:
        //   getFloat32(float &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 32-BIT FLOATS TEST" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting getFloat32." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putFloat32(1);           o.putInt8(0xFF);
            o.putFloat32(2);           o.putInt8(0xFE);
            o.putFloat32(3);           o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 15);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            float val;
            x.getFloat32(val);         x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getFloat32(val);         x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getFloat32(val);         x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getFloat32(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putFloat32(4);           o.putInt8(0xFD);
            o.putFloat32(5);           o.putInt8(0xFE);
            o.putFloat32(6);           o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 15);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            float val;
            x.getFloat32(val);         x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getFloat32(val);         x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getFloat32(val);         x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getFloat32(val);
            ASSERT(!x);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // GET 64-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt64(bsls_PlatformUtil::Int64 val &variable);
        //   getUint64(bsls_PlatformUtil::Uint64 val &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 64-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt64." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt64(1);             o.putInt8(0xFF);
            o.putInt64(2);             o.putInt8(0xFE);
            o.putInt64(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 27);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt64(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt64(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt64(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt64(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt64(4);             o.putInt8(0xFD);
            o.putInt64(5);             o.putInt8(0xFE);
            o.putInt64(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 27);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt64(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt64(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt64(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt64(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint64." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint64(1);             o.putInt8(0xFF);
            o.putUint64(2);             o.putInt8(0xFE);
            o.putUint64(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 27);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint64(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint64(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint64(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint64(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint64(4);             o.putInt8(0xFD);
            o.putUint64(5);             o.putInt8(0xFE);
            o.putUint64(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 27);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint64(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint64(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint64(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint64(val);
            ASSERT(!x);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // GET 56-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt56(bsls_PlatformUtil::Int64 val &variable);
        //   getUint56(bsls_PlatformUtil::Uint64 val &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 56-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt56." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt56(1);             o.putInt8(0xFF);
            o.putInt56(2);             o.putInt8(0xFE);
            o.putInt56(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 24);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt56(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt56(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt56(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt56(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt56(4);             o.putInt8(0xFD);
            o.putInt56(5);             o.putInt8(0xFE);
            o.putInt56(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 24);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt56(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt56(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt56(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt56(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint56." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint56(1);             o.putInt8(0xFF);
            o.putUint56(2);             o.putInt8(0xFE);
            o.putUint56(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 24);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint56(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint56(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint56(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint56(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint56(4);             o.putInt8(0xFD);
            o.putUint56(5);             o.putInt8(0xFE);
            o.putUint56(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 24);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint56(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint56(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint56(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint56(val);
            ASSERT(!x);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GET 48-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt48(bsls_PlatformUtil::Int64 val &variable);
        //   getUint48(bsls_PlatformUtil::Uint64 val &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 48-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt48." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt48(1);             o.putInt8(0xFF);
            o.putInt48(2);             o.putInt8(0xFE);
            o.putInt48(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 21);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt48(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt48(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt48(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt48(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt48(4);             o.putInt8(0xFD);
            o.putInt48(5);             o.putInt8(0xFE);
            o.putInt48(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 21);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt48(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt48(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt48(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt48(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint48." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint48(1);             o.putInt8(0xFF);
            o.putUint48(2);             o.putInt8(0xFE);
            o.putUint48(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 21);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint48(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint48(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint48(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint48(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint48(4);             o.putInt8(0xFD);
            o.putUint48(5);             o.putInt8(0xFE);
            o.putUint48(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 21);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint48(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint48(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint48(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint48(val);
            ASSERT(!x);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // GET 40-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt40(bsls_PlatformUtil::Int64 val &variable);
        //   getUint40(bsls_PlatformUtil::Uint64 val &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 40-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt40." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt40(1);             o.putInt8(0xFF);
            o.putInt40(2);             o.putInt8(0xFE);
            o.putInt40(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 18);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt40(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt40(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt40(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt40(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt40(4);             o.putInt8(0xFD);
            o.putInt40(5);             o.putInt8(0xFE);
            o.putInt40(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 18);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Int64 val;
            x.getInt40(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt40(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt40(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt40(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint40." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint40(1);             o.putInt8(0xFF);
            o.putUint40(2);             o.putInt8(0xFE);
            o.putUint40(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 18);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint40(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint40(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint40(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint40(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint40(4);             o.putInt8(0xFD);
            o.putUint40(5);             o.putInt8(0xFE);
            o.putUint40(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 18);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            bsls_PlatformUtil::Uint64 val;
            x.getUint40(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint40(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint40(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint40(val);
            ASSERT(!x);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // GET 32-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt32(int &variable);
        //   getUint32(unsigned int &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 32-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt32." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt32(1);             o.putInt8(0xFF);
            o.putInt32(2);             o.putInt8(0xFE);
            o.putInt32(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 15);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int val;
            x.getInt32(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt32(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt32(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt32(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt32(4);             o.putInt8(0xFD);
            o.putInt32(5);             o.putInt8(0xFE);
            o.putInt32(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 15);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int val;
            x.getInt32(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt32(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt32(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt32(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint32." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint32(1);             o.putInt8(0xFF);
            o.putUint32(2);             o.putInt8(0xFE);
            o.putUint32(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 15);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int val;
            x.getUint32(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint32(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint32(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint32(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint32(4);             o.putInt8(0xFD);
            o.putUint32(5);             o.putInt8(0xFE);
            o.putUint32(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 15);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int val;
            x.getUint32(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint32(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint32(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint32(val);
            ASSERT(!x);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // GET 24-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt24(int &variable);
        //   getUint24(unsigned int &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 24-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt24." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt24(1);             o.putInt8(0xFF);
            o.putInt24(2);             o.putInt8(0xFE);
            o.putInt24(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 12);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int val;
            x.getInt24(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt24(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt24(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt24(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt24(4);             o.putInt8(0xFD);
            o.putInt24(5);             o.putInt8(0xFE);
            o.putInt24(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 12);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            int val;
            x.getInt24(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt24(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt24(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt24(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint24." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint24(1);             o.putInt8(0xFF);
            o.putUint24(2);             o.putInt8(0xFE);
            o.putUint24(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 12);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int val;
            x.getUint24(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint24(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint24(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint24(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint24(4);             o.putInt8(0xFD);
            o.putUint24(5);             o.putInt8(0xFE);
            o.putUint24(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 12);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned int val;
            x.getUint24(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint24(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint24(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint24(val);
            ASSERT(!x);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // GET 16-BIT INTEGERS TEST:
        //
        // Testing:
        //   getInt16(short &variable);
        //   getUint16(unsigned short &variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 16-BIT INTEGERS TEST" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting getInt16." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt16(1);             o.putInt8(0xFF);
            o.putInt16(2);             o.putInt8(0xFE);
            o.putInt16(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 9);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            short val;
            x.getInt16(val);           x.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            x.getInt16(val);           x.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            x.getInt16(val);           x.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getInt16(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt16(4);             o.putInt8(0xFD);
            o.putInt16(5);             o.putInt8(0xFE);
            o.putInt16(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 9);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            short val;
            x.getInt16(val);           x.getInt8(marker);
            ASSERT(4 == val);          ASSERT('\xFD' == marker);
            x.getInt16(val);           x.getInt8(marker);
            ASSERT(5 == val);          ASSERT('\xFE' == marker);
            x.getInt16(val);           x.getInt8(marker);
            ASSERT(6 == val);          ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getInt16(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint16." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint16(1);             o.putInt8(0xFF);
            o.putUint16(2);             o.putInt8(0xFE);
            o.putUint16(3);             o.putInt8(0xFD);

            bdesb_FixedMemInput isb(workSpace, 9);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned short val;
            x.getUint16(val);           x.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            x.getUint16(val);           x.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            x.getUint16(val);           x.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(x);

            x.getUint16(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint16(4);             o.putInt8(0xFD);
            o.putUint16(5);             o.putInt8(0xFE);
            o.putUint16(6);             o.putInt8(0xFF);

            bdesb_FixedMemInput isb(workSpace, 9);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            char marker;
            unsigned short val;
            x.getUint16(val);           x.getInt8(marker);
            ASSERT(4 == val);           ASSERT('\xFD' == marker);
            x.getUint16(val);           x.getInt8(marker);
            ASSERT(5 == val);           ASSERT('\xFE' == marker);
            x.getUint16(val);           x.getInt8(marker);
            ASSERT(6 == val);           ASSERT('\xFF' == marker);
            ASSERT(x);

            x.getUint16(val);
            ASSERT(!x);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GET 8-BIT INTEGERS TEST:
        //   Note that getInt8(char&) is tested in the PRIMARY MANIPULATORS
        //   test.
        //
        // Testing:
        //   getInt8(signed char& variable);
        //   getUint8(char& variable);
        //   getUint8(unsigned char& variable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GET 8-BIT INTEGERS TEST" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting getInt8(signed char&)." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            signed char val;
            x.getInt8(val);            ASSERT(1 == val);
            x.getInt8(val);            ASSERT(2 == val);
            x.getInt8(val);            ASSERT(3 == val);
            x.getInt8(val);            ASSERT(4 == val);
            ASSERT(x);

            x.getInt8(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt8(5);
            o.putInt8(6);
            o.putInt8(7);
            o.putInt8(8);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            signed char val;
            x.getInt8(val);            ASSERT(5 == val);
            x.getInt8(val);            ASSERT(6 == val);
            x.getInt8(val);            ASSERT(7 == val);
            x.getInt8(val);            ASSERT(8 == val);
            ASSERT(x);

            x.getInt8(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint8(char&)." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            char val;
            x.getUint8(val);            ASSERT(1 == val);
            x.getUint8(val);            ASSERT(2 == val);
            x.getUint8(val);            ASSERT(3 == val);
            x.getUint8(val);            ASSERT(4 == val);
            ASSERT(x);

            x.getUint8(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint8(5);
            o.putUint8(6);
            o.putUint8(7);
            o.putUint8(8);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            char val;
            x.getUint8(val);            ASSERT(5 == val);
            x.getUint8(val);            ASSERT(6 == val);
            x.getUint8(val);            ASSERT(7 == val);
            x.getUint8(val);            ASSERT(8 == val);
            ASSERT(x);

            x.getUint8(val);
            ASSERT(!x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint8(unsigned char&)." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            unsigned char val;
            x.getUint8(val);            ASSERT(1 == val);
            x.getUint8(val);            ASSERT(2 == val);
            x.getUint8(val);            ASSERT(3 == val);
            x.getUint8(val);            ASSERT(4 == val);
            ASSERT(x);

            x.getUint8(val);
            ASSERT(!x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putUint8(5);
            o.putUint8(6);
            o.putUint8(7);
            o.putUint8(8);

            bdesb_FixedMemInput isb(workSpace, 4);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            if (veryVerbose) { P(x) }
            unsigned char val;
            x.getUint8(val);            ASSERT(5 == val);
            x.getUint8(val);            ASSERT(6 == val);
            x.getUint8(val);            ASSERT(7 == val);
            x.getUint8(val);            ASSERT(8 == val);
            ASSERT(x);

            x.getUint8(val);
            ASSERT(!x);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST:
        //   For each independent test, create bdex_GenericByteInStream objects
        //   containing various data, then use primary manipulators 'getInt8'
        //   and 'invalidate' to modify the state of the
        //   bdex_GenericByteInStream.  Use basic accessors to verify the final
        //   state of the bdex_GenericByteInStream objects.
        //
        // Testing
        //   bdex_ByteInStream();
        //   bdex_ByteInStream(const char *buffer, int numBytes);
        //   ~bdex_ByteInStream();   // by purify
        //   getInt8(char& variable);
        //   void invalidate();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting getInt8(char&) and ctors." << endl;
        {
            char workSpace[100];
            bdesb_FixedMemInput isb(workSpace, 100);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x0(&isb);

            ASSERT(x0);

            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
                                             // test ctor initialized w/ char *
            ASSERT(x);

            char val;
            x.getInt8(val);              ASSERT(1 == val);
            x.getInt8(val);              ASSERT(2 == val);
            x.getInt8(val);              ASSERT(3 == val);
            x.getInt8(val);              ASSERT(4 == val);
            ASSERT(x);
        }
        {
            char workSpace[100];
            bdesb_FixedMemInput isb(workSpace, 100);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x0(&isb);
            ASSERT(x0);

            bdesb_FixedMemOutput osb(workSpace, 100);
            bdex_GenericByteOutStream<bdesb_FixedMemOutput> o(&osb);
            o.putInt8(5);
            o.putInt8(6);
            o.putInt8(7);
            o.putInt8(8);

            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);
            ASSERT(x);

            char val;
            x.getInt8(val);              ASSERT(5 == val);
            x.getInt8(val);              ASSERT(6 == val);
            x.getInt8(val);              ASSERT(7 == val);
            x.getInt8(val);              ASSERT(8 == val);
            ASSERT(x);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting invalidate()." << endl;
        for (int i = 0; i < 5; i++) {
            // test default bdex_GenericByteInStream<bdesb_FixedMemInput>
            char workSpace[100];
            bdesb_FixedMemInput isb(workSpace, 100);
            bdex_GenericByteInStream<bdesb_FixedMemInput> x(&isb);

            LOOP_ASSERT(i, x);
            x.invalidate();
            LOOP_ASSERT(i, !x);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdex_GenericByteInStream' containing various data.
        //   Exercise these bdex_GenericByteInStream using appropriate input
        //   methods and primary manipulators, then verify the state of the
        //   resulting bdex_GenericByteInStream using basic accessors.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose)
            cout << "\nCreate bdex_GenericByteInStream x1 using default ctor."
                 << endl;
        int i;
        char workSpace[1];
        bdesb_FixedMemInput sb(workSpace, 1);
        bdex_GenericByteInStream<bdesb_FixedMemInput> x1(&sb);

        if (verbose)
            cout << "\nCreate bdex_GenericByteInStream x2 w/ an initial value."
                 << endl;

        bdesb_FixedMemInput sb2(const_cast<char *>("\x00\x01\x02\x03\x04"), 5);
        bdex_GenericByteInStream<bdesb_FixedMemInput> x2(&sb2);

        if (verbose) cout << "\nTry getInt8() with x2." << endl;
        for (i = 0; i < 5; i++) {
            char c;
            x2.getInt8(c);
            LOOP_ASSERT(i, i == c );
        }

        if (verbose) cout << "\nTry invalidate() with x2." << endl;
        x2.invalidate();
        ASSERT(!x2);

        if (verbose) cout << "\nTry invalid operation with x1." << endl;
        x1.getInt32(i);
        ASSERT(!x1);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPERFORMANCE TEST"
                          << "\n================" << endl;

        const int NUM_ITERS = (argc > 2) ? atoi(argv[2]) : 1000;
        const int NUM_OUT_ITERS = (argc > 3) ? atoi(argv[3]) : 10000000;

        if (NUM_ITERS < 1 || NUM_OUT_ITERS < 1) {
            cout << "Usage: test -1 numInIters numOutIters\n"
                 << "\twhere numInIters = number of putInt8 in inner loop\n"
                 << "\tand numOutIters  = number of ctor+putInt8+dtor in"
                                                            " outer loop\n"
                 << "\t(default values to 1000 and 1000000, resp.)\n";
            return -1;
        }

        if (verbose) { P_(NUM_OUT_ITERS);  P(NUM_ITERS); }
        char *workSpace = new char[(NUM_ITERS+1) * sizeof(int)];
        const int SIZE = NUM_ITERS+1;

        if (verbose) cout << "\nWith bdesb_FixedMemIn/Output:" << endl;

        if (verbose) cout << "\tTest performance on ctor+putInt8+dtor.\n";
        {
            for (int i = 0; i < SIZE; ++i) {
                workSpace[i] = (char)(i * (i + 1) / 2);
            }

            bsls_Stopwatch timer; timer.start();
            for (int i = 0; i <= NUM_OUT_ITERS; ++i) {
                char marker = 0;
                bdesb_FixedMemOutput  sbo(workSpace, SIZE);
                bdex_GenericByteOutStream<bdesb_FixedMemOutput> xo(&sbo);
                bdesb_FixedMemInput  sbi(workSpace, SIZE);
                bdex_GenericByteInStream<bdesb_FixedMemInput> xi(&sbi);
                for (int j = 0; j < NUM_ITERS; ++j) {
                    xo.putInt8(0x7f); // value does not matter
                    xi.getInt8(marker);
                }
                LOOP_ASSERT(i, 0x7f == marker);
            }
            double elapsed = timer.elapsedTime();
            if (verbose) cout << "\tin " << elapsed << "seconds\n";
        }

        if (verbose) cout << "\tTest performance on ctor+putInt32+dtor.\n";
        {
            for (int i = 0; i < SIZE; ++i) {
                workSpace[i] = (char)(i * (i + 1) / 2);
            }

            bsls_Stopwatch timer; timer.start();
            for (int i = 0; i <= NUM_OUT_ITERS; ++i) {
                int marker = 0;
                bdesb_FixedMemOutput  sbo(workSpace, SIZE);
                bdex_GenericByteOutStream<bdesb_FixedMemOutput> xo(&sbo);
                bdesb_FixedMemInput  sbi(workSpace, SIZE);
                bdex_GenericByteInStream<bdesb_FixedMemInput> xi(&sbi);
                for (int j = 0; j < NUM_ITERS; ++j) {
                    xo.putInt32(0x7fffffff); // value does not matter
                    xi.getInt32(marker);
                }
                LOOP_ASSERT(i, 0x7fffffff == marker);
            }
            double elapsed = timer.elapsedTime();
            if (verbose) cout << "\tin " << elapsed << "seconds\n";
        }

        if (verbose) cout << "\nWith bdesb_FixedMemIn/OutStreamBuf:" << endl;

        if (verbose) cout << "\tTest performance on ctor+putInt8+dtor.\n";
        {
            for (int i = 0; i < SIZE; ++i) {
                workSpace[i] = (char)(i * (i + 1) / 2);
            }

            bsls_Stopwatch timer; timer.start();
            for (int i = 0; i <= NUM_OUT_ITERS; ++i) {
                char marker = 0;
                bdesb_FixedMemOutStreamBuf sbo(workSpace, SIZE);
                bdex_GenericByteOutStream<bdesb_FixedMemOutStreamBuf> xo(&sbo);
                bdesb_FixedMemInStreamBuf sbi(workSpace, SIZE);
                bdex_GenericByteInStream<bdesb_FixedMemInStreamBuf> xi(&sbi);
                for (int j = 0; j < NUM_ITERS; ++j) {
                    xo.putInt8(0x7f); // value does not matter
                    xi.getInt8(marker);
                }
                LOOP_ASSERT(i, 0x7f == marker);
            }
            double elapsed = timer.elapsedTime();
            if (verbose) cout << "\tin " << elapsed << "seconds\n";
        }

        if (verbose) cout << "\tTest performance on ctor+putInt32+dtor.\n";
        {
            for (int i = 0; i < SIZE; ++i) {
                workSpace[i] = (char)(i * (i + 1) / 2);
            }

            bsls_Stopwatch timer; timer.start();
            for (int i = 0; i <= NUM_OUT_ITERS; ++i) {
                int marker = 0;
                bdesb_FixedMemOutStreamBuf sbo(workSpace, SIZE);
                bdex_GenericByteOutStream<bdesb_FixedMemOutStreamBuf> xo(&sbo);
                bdesb_FixedMemInStreamBuf sbi(workSpace, SIZE);
                bdex_GenericByteInStream<bdesb_FixedMemInStreamBuf> xi(&sbi);
                for (int j = 0; j < NUM_ITERS; ++j) {
                    xo.putInt32(0x7fffffff); // value does not matter
                    xi.getInt32(marker);
                }
                LOOP_ASSERT(i, 0x7fffffff == marker);
            }
            double elapsed = timer.elapsedTime();
            if (verbose) cout << "\tin " << elapsed << "seconds\n";
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
