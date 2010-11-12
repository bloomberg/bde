// bdex_byteinstreamraw.t.cpp                                         -*-C++-*-

#include <bdex_byteinstreamraw.h>
#include <bdex_byteoutstreamraw.h>              // for testing only
#include <bdex_byteoutstream.h>                 // for testing only

#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_instreamfunctions.h>             // for testing only

#include <bsls_platformutil.h>                  // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy(), memcmp(), strlen()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


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
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// For all input methods in 'bdex_ByteInStreamRaw', the "unexternalization"
// from byte representation to the correct fundamental-type value is delegated
// to another component.  We assume that this process has been rigorously
// tested and verified.  Therefore, we are concerned only with the placement of
// the next-byte-position cursor and the alignment of bytes in the input
// stream.  For each input method, we verify these properties by first creating
// a 'bdex_ByteOutStream' object 'oX' containing some arbitrarily chosen
// values.  The values are interleaved with chosen "marker" bytes to check for
// alignment issues.  We then create a 'bdex_ByteInStreamRaw' object 'iX'
// initialized with the content of 'oX', and consecutively call the input
// method on 'iX' to verify that the extracted values and marker bytes equal
// to their respective chosen values.  After all values are extracted, we
// verify that the stream is valid, empty, and the cursor is properly placed.
//-----------------------------------------------------------------------------
// [ 2] bdex_ByteInStreamRaw();
// [ 2] bdex_ByteInStreamRaw(const char *buffer, int numBytes);
// [ 2] ~bdex_ByteInStreamRaw();
// [25] getLength(int& variable);
// [25] getVersion(int& variable);
// [12] getInt64(bsls_PlatformUtil::Int64& variable);
// [12] getUint64(bsls_PlatformUtil::Uint64& variable);
// [11] getInt56(bsls_PlatformUtil::Int64& variable);
// [11] getUint56(bsls_PlatformUtil::Uint64& variable);
// [10] getInt48(bsls_PlatformUtil::Int64& variable);
// [10] getUint48(bsls_PlatformUtil::Uint64& variable);
// [ 9] getInt40(bsls_PlatformUtil::Int64& variable);
// [ 9] getUint40(bsls_PlatformUtil::Uint64& variable);
// [ 8] getInt32(int& variable);
// [ 8] getUint32(unsigned int& variable);
// [ 7] getInt24(int& variable);
// [ 7] getUint24(unsigned int& variable);
// [ 6] getInt16(short& variable);
// [ 6] getUint16(unsigned short& variable);
// [ 2] getInt8(char& variable);
// [ 5] getInt8(signed char& variable);
// [ 5] getUint8(char& variable);
// [ 5] getUint8(unsigned char& variable);
// [14] getFloat64(double& variable);
// [13] getFloat32(float& variable);
// [22] getArrayInt64(bsls_PlatformUtil::Int64 *array, int numValues);
// [22] getArrayUint64(bsls_PlatformUtil::Uint64 *array, int numValues);
// [21] getArrayInt56(bsls_PlatformUtil::Int64 *array, int numValues);
// [21] getArrayUint56(bsls_PlatformUtil::Uint64 *array, int numValues);
// [20] getArrayInt48(bsls_PlatformUtil::Int64 *array, int numValues);
// [20] getArrayUint48(bsls_PlatformUtil::Uint64 *array, int numValues);
// [19] getArrayInt40(bsls_PlatformUtil::Int64 *array, int numValues);
// [19] getArrayUint40(bsls_PlatformUtil::Uint64 *array, int numValues);
// [18] getArrayInt32(int *array, int numValues);
// [18] getArrayUint32(unsigned int *array, int numValues);
// [17] getArrayInt24(int *array, int numValues);
// [17] getArrayUint24(unsigned int *array, int numValues);
// [16] getArrayInt16(short *array, int numValues);
// [16] getArrayUint16(unsigned short *array,int numValues);
// [15] getArrayInt8(char *array, int numValues);
// [15] getArrayInt8(signed char *array, int numValues);
// [15] getArrayUint8(char *array, int numValues);
// [15] getArrayUint8(unsigned char *array, int numValues);
// [24] getArrayFloat64(double *array, int numValues);
// [23] getArrayFloat32(float *array, int numValues);
// [ 2] void invalidate();
// [ 3] operator const void *() const;
// [ 3] bool isEmpty() const;
// [ 3] int length() const;
// [ 3] int cursor() const;
//
// [ 4] ostream& operator<<(ostream& stream, const bdex_ByteInStreamRaw& obj);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [26] USAGE EXAMPLE
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdex_ByteInStreamRaw Obj;
typedef bdex_ByteOutStreamRaw Out;

const int SIZEOF_INT64   = 8;
const int SIZEOF_INT32   = 4;
const int SIZEOF_INT16   = 2;
const int SIZEOF_INT8    = 1;
const int SIZEOF_FLOAT64 = 8;
const int SIZEOF_FLOAT32 = 4;

//=============================================================================
//              Classes, functions, etc., needed for Usage Example
//-----------------------------------------------------------------------------

// Suppose we wish to implement a (deliberately simple) 'my_Person' class (in
// an appropriate 'my_person' component) as a value-semantic object that
// supports 'bdex' externalization.  In addition to whatever data and methods
// that we choose to put into our design, we must supply three methods of
// specific name and signature in order to comply with the 'bdex' "protocol":
// a class method 'maxSupportedBdexVersion' and two object methods, an accessor
// (i.e., a 'const' method) 'bdexStreamOut', and a manipulator (i.e., a
// non-'const' method) 'bdex_StreamIn'.  This example shows how to implement
// those three methods for the simple "person" component.  Note that, so long
// as the above three methods are provided, there is no need to declare or
// implement overloaded 'operator<<' and 'operator>>'; these operators are
// provided by each 'bdex' stream as appropriate, templatized by an assumed
// 'bdex' compliant value-semantic 'OBJECT' type.
//
// In this example we will not worry overly about "good design" of the person
// component, and we will declare but not implement the suite of value-semantic
// methods and free operators.  In particular, we will not make explicit use of
// 'bdema' allocators; a more complete design would do so.
//..
   // my_person.h

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
           // Create a person object having the specified 'firstName',
           // 'lastName', and 'age'.

       my_Person(const my_Person& original);
           // Create a person object having value of the specified 'original'
           // person.

       ~my_Person();
           // Destroy this object.

       // MANIPULATORS
       my_Person& operator=(const my_Person& rhs);
           // Assign to this person object the value of the specified 'rhs'
           // person.

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
       // Return 'true' if the specified 'lhs' and 'rhs' person objects have
       // the same value and 'false' otherwise.  Two person objects have the
       // same value if they have the same first name, last name, and age.

   inline
   bool operator!=(const my_Person& lhs, const my_Person& rhs);
       // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
       // have the same value and 'false' otherwise.  Two person objects differ
       // in value if they differ in first name, last name, or age.

   bsl::ostream& operator<<(bsl::ostream& stream, const my_Person& person);
       // Write the specified 'date' value to the specified output 'stream' in
       // some reasonable format.

   // INLINE FUNCTION DEFINITIONS
   inline
   my_Person::my_Person()
   : d_firstName("")
   , d_lastName("")
   , d_age(0)
   {
   }

   inline
   my_Person::my_Person(const char *firstName, const char *lastName, int age)
   : d_firstName(firstName)
   , d_lastName(lastName)
   , d_age(age)
   {
   }

   inline
   my_Person::~my_Person()
   {
   }

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
   const bsl::string& my_Person::firstName() const
   {
       return d_firstName;
   }

   const bsl::string& my_Person::lastName() const
   {
       return d_lastName;
   }

   int my_Person::age() const
   {
       return d_age;
   }

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

   // FREE OPERATORS
   inline
   bool operator==(const my_Person& lhs, const my_Person& rhs)
   {
       return lhs.d_firstName == rhs.d_firstName &&
              lhs.d_lastName  == rhs.d_lastName  &&
              lhs.d_age       == rhs.d_age;
   }

   inline
   bool operator!=(const my_Person& lhs, const my_Person& rhs)
   {
       return !(lhs == rhs);
   }

//=============================================================================
//                                   MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 26: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

// // my_testapp.m.cpp
//
// using namespace bsl;
//
// int main(int argc, char *argv[])
   {
       my_Person JaneSmith("Jane", "Smith", 42);
       const int             BUFFER_SIZE = 10000;
       char                  buffer[BUFFER_SIZE];
       bdex_ByteOutStreamRaw outStream(buffer);
       const int             VERSION = 1;
       outStream.putVersion(VERSION);
       bdex_OutStreamFunctions::streamOut(outStream, JaneSmith, VERSION);
//..
// Next create a 'my_Person' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith'.
//..
       my_Person janeCopy;                       ASSERT(janeCopy != JaneSmith);
//..
// Now create a 'bdex_ByteInStreamRaw' 'inStream' initialized with the
// user-supplied 'buffer' that was provided to the 'bdex_ByteOutStreamRaw'
// object 'outStream'.  Note that 'bdex_ByteOutStreamRaw' has 'data' and
// 'length' accessors, and so may be used exactly as 'bdex_ByteOutStream', but
// here we choose to illustrate the "raw" nature of the component by supplying
// the buffer and its length explicitly from our own knowledge.  In a more
// realistic example, we'd know the actual length of the buffer by some means
// (e.g., from the transport mechanism that brought the buffer into our
// process).  Our use of 'BUFFER_SIZE' here is to call attention to the fact
// that the stream is "raw", and not to suggest elegant usage.
//..
       bdex_ByteInStreamRaw inStream(buffer, BUFFER_SIZE);
       int version;
       inStream.getVersion(version);
       bdex_InStreamFunctions::streamIn(inStream, janeCopy, version);
       ASSERT(janeCopy == JaneSmith);
//
//     // Verify the results on 'stdout'.
       if (verbose) {
           if (janeCopy == JaneSmith) {
               cout << "Successfully serialized and de-serialized Jane Smith:"
                    << "\n\tFirstName: " << janeCopy.firstName()
                    << "\n\tLastName : " << janeCopy.lastName()
                    << "\n\tAge      : " << janeCopy.age() << endl;
           }
           else {
               cout << "Serialization unsuccessful.  janeCopy holds:"
                    << "\n\tFirstName: " << janeCopy.firstName()
                    << "\n\tLastName : " << janeCopy.lastName()
                    << "\n\tAge      : " << janeCopy.age() << endl;
           }
       }
//     return 0;
   }

      } break;
      case 25: {
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
            char buffer[6 + 3];
            Out o(buffer, sizeof buffer);
            o.putLength(1);             o.putInt8(0xFF);
            o.putLength(128);           o.putInt8(0xFE);
            o.putLength(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[9 + 3];
            Out o(buffer, sizeof buffer);
            o.putLength(128);             o.putInt8(0xFD);
            o.putLength(127);             o.putInt8(0xFE);
            o.putLength(256);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        if (verbose) cout << "\nTesting getVersion(int&)." << endl;
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putVersion(1);
            o.putVersion(2);
            o.putVersion(3);
            o.putVersion(4);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            int val;
            x.getVersion(val);            ASSERT(1 == val);
            x.getVersion(val);            ASSERT(2 == val);
            x.getVersion(val);            ASSERT(3 == val);
            x.getVersion(val);            ASSERT(4 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putVersion(252);
            o.putVersion(253);
            o.putVersion(254);
            o.putVersion(255);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            int val;
            x.getVersion(val);            ASSERT(252 == val);
            x.getVersion(val);            ASSERT(253 == val);
            x.getVersion(val);            ASSERT(254 == val);
            x.getVersion(val);            ASSERT(255 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 24: {
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

            char buffer[48 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayFloat64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat64(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const double DATA[] = { 4, 5, 6 };
            const double V = 0xFF;

            char buffer[48 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayFloat64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat64(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 23: {
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

            char buffer[24 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayFloat32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat32(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const float DATA[] = { 4, 5, 6 };
            const float V = 0xFF;

            char buffer[24 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayFloat32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat32(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 22: {
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

            char buffer[48 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt64(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char buffer[48 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt64(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint64." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[48 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint64(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint64(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint64(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint64(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[48 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint64(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint64(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint64(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint64(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 21: {
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

            char buffer[42 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt56(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt56(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt56(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt56(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char buffer[42 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt56(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt56(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt56(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt56(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint56." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[42 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint56(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint56(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint56(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint56(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[42 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint56(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint56(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint56(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint56(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 20: {
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

            char buffer[36 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt48(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt48(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt48(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt48(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char buffer[36 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt48(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt48(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt48(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt48(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint48." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[36 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint48(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint48(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint48(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint48(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[36 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint48(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint48(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint48(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint48(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 19: {
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

            char buffer[30 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt40(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt40(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt40(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt40(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Int64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Int64 V = 0xFF;

            char buffer[30 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt40(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt40(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt40(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt40(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint40." << endl;
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 1, 2, 3 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[30 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint40(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint40(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint40(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint40(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const bsls_PlatformUtil::Uint64 DATA[] = { 4, 5, 6 };
            const bsls_PlatformUtil::Uint64 V = 0xFF;

            char buffer[30 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint40(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint40(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint40(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint40(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 18: {
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

            char buffer[24 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt32(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const int DATA[] = { 4, 5, 6 };
            const int V = 0xFF;

            char buffer[24 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt32(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint32." << endl;
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            char buffer[24 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint32(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint32(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint32(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint32(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const unsigned int DATA[] = { 4, 5, 6 };
            const unsigned int V = 0xFF;

            char buffer[24 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint32(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint32(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint32(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint32(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 17: {
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

            char buffer[18 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt24(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt24(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt24(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt24(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const int DATA[] = { 4, 5, 6 };
            const int V = 0xFF;

            char buffer[18 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt24(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt24(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt24(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt24(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint24." << endl;
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            char buffer[18 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint24(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint24(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint24(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint24(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const unsigned int DATA[] = { 4, 5, 6 };
            const unsigned int V = 0xFF;

            char buffer[18 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint24(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint24(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint24(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint24(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 16: {
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

            char buffer[12 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt16(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt16(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt16(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt16(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const short DATA[] = { 4, 5, 6 };
            const short V = 0xFF;

            char buffer[12 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt16(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt16(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt16(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt16(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint16." << endl;
        {
            const unsigned short DATA[] = { 1, 2, 3 };
            const unsigned short V = 0xFF;

            char buffer[12 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint16(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint16(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint16(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint16(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const unsigned short DATA[] = { 4, 5, 6 };
            const unsigned short V = 0xFF;

            char buffer[12 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint16(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint16(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint16(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint16(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 15: {
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

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const char DATA[] = { 4, 5, 6 };
            const char V = (char) 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting getArrayInt8(signed char*, int)." << endl;
        {
            const signed char DATA[] = { 1, 2, 3 };
            const signed char V = (char) 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const signed char DATA[] = { 4, 5, 6 };
            const signed char V = (char) 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getArrayUint8(char*, int)." << endl;
        {
            const char DATA[] = { 1, 2, 3 };
            const char V = (char) 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const char DATA[] = { 4, 5, 6 };
            const char V = (char) 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting getArrayUint8(unsigned char*, int)." << endl;
        {
            const unsigned char DATA[] = { 1, 2, 3 };
            const unsigned char V = 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            const unsigned char DATA[] = { 4, 5, 6 };
            const unsigned char V = 0xFF;

            char buffer[6 + 4];
            Out o(buffer, sizeof buffer);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 14: {
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
            char buffer[24 + 3];
            Out o(buffer, sizeof buffer);
            o.putFloat64(1);           o.putInt8(0xFF);
            o.putFloat64(2);           o.putInt8(0xFE);
            o.putFloat64(3);           o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[24 + 3];
            Out o(buffer, sizeof buffer);
            o.putFloat64(4);           o.putInt8(0xFD);
            o.putFloat64(5);           o.putInt8(0xFE);
            o.putFloat64(6);           o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 13: {
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
            char buffer[12 + 3];
            Out o(buffer, sizeof buffer);
            o.putFloat32(1);           o.putInt8(0xFF);
            o.putFloat32(2);           o.putInt8(0xFE);
            o.putFloat32(3);           o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[12 + 3];
            Out o(buffer, sizeof buffer);
            o.putFloat32(4);           o.putInt8(0xFD);
            o.putFloat32(5);           o.putInt8(0xFE);
            o.putFloat32(6);           o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 12: {
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
            char buffer[24 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt64(1);             o.putInt8(0xFF);
            o.putInt64(2);             o.putInt8(0xFE);
            o.putInt64(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[24 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt64(4);             o.putInt8(0xFD);
            o.putInt64(5);             o.putInt8(0xFE);
            o.putInt64(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint64." << endl;
        {
            char buffer[24 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint64(1);             o.putInt8(0xFF);
            o.putUint64(2);             o.putInt8(0xFE);
            o.putUint64(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[24 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint64(4);             o.putInt8(0xFD);
            o.putUint64(5);             o.putInt8(0xFE);
            o.putUint64(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 11: {
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
            char buffer[21 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt56(1);             o.putInt8(0xFF);
            o.putInt56(2);             o.putInt8(0xFE);
            o.putInt56(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[21 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt56(4);             o.putInt8(0xFD);
            o.putInt56(5);             o.putInt8(0xFE);
            o.putInt56(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint56." << endl;
        {
            char buffer[21 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint56(1);             o.putInt8(0xFF);
            o.putUint56(2);             o.putInt8(0xFE);
            o.putUint56(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[21 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint56(4);             o.putInt8(0xFD);
            o.putUint56(5);             o.putInt8(0xFE);
            o.putUint56(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 10: {
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
            char buffer[18 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt48(1);             o.putInt8(0xFF);
            o.putInt48(2);             o.putInt8(0xFE);
            o.putInt48(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[18 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt48(4);             o.putInt8(0xFD);
            o.putInt48(5);             o.putInt8(0xFE);
            o.putInt48(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint48." << endl;
        {
            char buffer[18 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint48(1);             o.putInt8(0xFF);
            o.putUint48(2);             o.putInt8(0xFE);
            o.putUint48(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[18 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint48(4);             o.putInt8(0xFD);
            o.putUint48(5);             o.putInt8(0xFE);
            o.putUint48(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 9: {
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
            char buffer[15 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt40(1);             o.putInt8(0xFF);
            o.putInt40(2);             o.putInt8(0xFE);
            o.putInt40(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[15 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt40(4);             o.putInt8(0xFD);
            o.putInt40(5);             o.putInt8(0xFE);
            o.putInt40(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint40." << endl;
        {
            char buffer[15 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint40(1);             o.putInt8(0xFF);
            o.putUint40(2);             o.putInt8(0xFE);
            o.putUint40(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[15 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint40(4);             o.putInt8(0xFD);
            o.putUint40(5);             o.putInt8(0xFE);
            o.putUint40(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 8: {
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
            char buffer[12 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt32(1);             o.putInt8(0xFF);
            o.putInt32(2);             o.putInt8(0xFE);
            o.putInt32(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[12 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt32(4);             o.putInt8(0xFD);
            o.putInt32(5);             o.putInt8(0xFE);
            o.putInt32(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint32." << endl;
        {
            char buffer[12 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint32(1);             o.putInt8(0xFF);
            o.putUint32(2);             o.putInt8(0xFE);
            o.putUint32(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[12 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint32(4);             o.putInt8(0xFD);
            o.putUint32(5);             o.putInt8(0xFE);
            o.putUint32(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 7: {
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
            char buffer[9 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt24(1);             o.putInt8(0xFF);
            o.putInt24(2);             o.putInt8(0xFE);
            o.putInt24(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[9 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt24(4);             o.putInt8(0xFD);
            o.putInt24(5);             o.putInt8(0xFE);
            o.putInt24(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint24." << endl;
        {
            char buffer[9 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint24(1);             o.putInt8(0xFF);
            o.putUint24(2);             o.putInt8(0xFE);
            o.putUint24(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[9 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint24(4);             o.putInt8(0xFD);
            o.putUint24(5);             o.putInt8(0xFE);
            o.putUint24(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 6: {
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
            char buffer[6 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt16(1);             o.putInt8(0xFF);
            o.putInt16(2);             o.putInt8(0xFE);
            o.putInt16(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[6 + 3];
            Out o(buffer, sizeof buffer);
            o.putInt16(4);             o.putInt8(0xFD);
            o.putInt16(5);             o.putInt8(0xFE);
            o.putInt16(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint16." << endl;
        {
            char buffer[6 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint16(1);             o.putInt8(0xFF);
            o.putUint16(2);             o.putInt8(0xFE);
            o.putUint16(3);             o.putInt8(0xFD);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[6 + 3];
            Out o(buffer, sizeof buffer);
            o.putUint16(4);             o.putInt8(0xFD);
            o.putUint16(5);             o.putInt8(0xFE);
            o.putUint16(6);             o.putInt8(0xFF);

            Obj x(buffer, sizeof buffer);
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
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 5: {
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
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            signed char val;
            x.getInt8(val);            ASSERT(1 == val);
            x.getInt8(val);            ASSERT(2 == val);
            x.getInt8(val);            ASSERT(3 == val);
            x.getInt8(val);            ASSERT(4 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putInt8(5);
            o.putInt8(6);
            o.putInt8(7);
            o.putInt8(8);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            signed char val;
            x.getInt8(val);            ASSERT(5 == val);
            x.getInt8(val);            ASSERT(6 == val);
            x.getInt8(val);            ASSERT(7 == val);
            x.getInt8(val);            ASSERT(8 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint8(char&)." << endl;
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            char val;
            x.getUint8(val);            ASSERT(1 == val);
            x.getUint8(val);            ASSERT(2 == val);
            x.getUint8(val);            ASSERT(3 == val);
            x.getUint8(val);            ASSERT(4 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putUint8(5);
            o.putUint8(6);
            o.putUint8(7);
            o.putUint8(8);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            char val;
            x.getUint8(val);            ASSERT(5 == val);
            x.getUint8(val);            ASSERT(6 == val);
            x.getUint8(val);            ASSERT(7 == val);
            x.getUint8(val);            ASSERT(8 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting getUint8(unsigned char&)." << endl;
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            unsigned char val;
            x.getUint8(val);            ASSERT(1 == val);
            x.getUint8(val);            ASSERT(2 == val);
            x.getUint8(val);            ASSERT(3 == val);
            x.getUint8(val);            ASSERT(4 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putUint8(5);
            o.putUint8(6);
            o.putUint8(7);
            o.putUint8(8);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            unsigned char val;
            x.getUint8(val);            ASSERT(5 == val);
            x.getUint8(val);            ASSERT(6 == val);
            x.getUint8(val);            ASSERT(7 == val);
            x.getUint8(val);            ASSERT(8 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT OPERATOR TEST:
        //   For each of a small representative set of objects, use
        //   'ostrstream' to write that object's value to a string buffer and
        //   then compare this buffer with the expected output format.
        //
        // Testing:
        //   ostream& operator<<(ostream&, const bdex_ByteInStreamRaw&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT OPERATOR TEST" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting print operator." << endl;

        const int SIZE = 172;        // Must be big enough to hold output
                                     // string plus overrun space detection.

        const int BUFFER_SIZE = 12;  // Must be big enough to contain all the
                                     // data put in the byteoutstreamraw

        const char XX = (char) 0xFF; // Value that represents an unset char.

        char ctrl[SIZE];
        memset(ctrl, XX, SIZE);
        const char *CTRL = ctrl;

        {
            char buffer[BUFFER_SIZE];  memset(buffer, 0, 12);

            Obj x(buffer, sizeof buffer);
            const char *EXPECTED =
                                 "\n0000\t00000000 00000000 00000000 00000000"
                                        " 00000000 00000000 00000000 00000000"
                                 "\n0008\t00000000 00000000 00000000 00000000";

            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);

            ostrstream out(buf, SIZE);
            out << x << ends;

            const int LEN = strlen(EXPECTED)+ 1;

            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            Out o(buffer, sizeof buffer);
            o.putInt8(0);  o.putInt8(1);  o.putInt8(2);  o.putInt8(3);

            Obj x(buffer, sizeof buffer);

            const char *EXPECTED =
                                 "\n0000\t00000000 00000001 00000010 00000011"
                                        " 00000000 00000000 00000000 00000000"
                                 "\n0008\t00000000 00000000 00000000 00000000";

            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);

            ostrstream out(buf, SIZE);
            out << x << ends;

            const int LEN = strlen(EXPECTED) + 1;

            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            Out o(buffer, sizeof buffer);
            o.putInt8(0);  o.putInt8(1);  o.putInt8(2);  o.putInt8(3);
            o.putInt8(4);  o.putInt8(5);  o.putInt8(6);  o.putInt8(7);
            o.putInt8(8);  o.putInt8(9);  o.putInt8(10); o.putInt8(11);

            Obj x(buffer, sizeof buffer);
            const char *EXPECTED =
                                 "\n0000\t00000000 00000001 00000010 00000011"
                                        " 00000100 00000101 00000110 00000111"
                                 "\n0008\t00001000 00001001 00001010 00001011";

            char buf[SIZE];
            memcpy(buf, CTRL, SIZE);

            ostrstream out(buf, SIZE);
            out << x << ends;

            const int LEN = strlen(EXPECTED) + 1;

            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << buf << endl;

            ASSERT(XX == buf[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(buf, EXPECTED, LEN));
            ASSERT(0 == memcmp(buf + LEN, CTRL + LEN, SIZE - LEN));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST:
        //   For each independent test, create objects containing various data.
        //   Use basic accessors to verify the initial state of the objects.
        //   Then use primary manipulators 'getInt8' and 'invalidate' to modify
        //   the state of the objects, and use basic accessors to verify the
        //   resulting state.
        //
        // Testing
        //   operator const void *() const;
        //   bool isEmpty() const;
        //   int length() const;
        //   int cursor() const;
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "BASIC ACCESSORS TEST" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting operator const void *()." << endl;

        {
            char buffer[1];
            Obj x(buffer, sizeof buffer);
            ASSERT(x);
            x.invalidate();
            ASSERT(!x);
        }
        {
            int i;
            char buffer[1];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);

            Obj x(buffer, sizeof buffer);
            if(veryVerbose) { P(x) }
            ASSERT(x);

            // Make sure the stream is still valid after an acceptable number
            // of calls to 'get' methods.
            char c;
            for (i = 0; i < sizeof buffer; i++) {
                x.getInt8(c);
            }
            ASSERT(x);

            // Invalidate stream 'x' by making excessive 'get' calls
            x.getInt8(c);
            ASSERT(!x);
        }
        {
            int i;
            char buffer[2];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);

            Obj x(buffer, sizeof buffer);
            if(veryVerbose) { P(x) }
            ASSERT(x);

            // Make sure the stream is still valid after an acceptable number
            // of calls to 'get' methods.
            char c;
            for (i = 0; i < sizeof buffer; i++) {
                x.getInt8(c);
            }
            ASSERT(x);

            // Invalidate stream 'x' by making excessive 'get' calls
            x.getInt8(c);
            ASSERT(!x);
        }
        {
            int i;
            char buffer[3];
            Out o(buffer,  sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);

            Obj x(buffer, sizeof buffer);
            if(veryVerbose) { P(x) }
            ASSERT(x);

            // Make sure the stream is still valid after an acceptable number
            // of calls to 'get' methods.
            char c;
            for (i = 0; i < sizeof buffer; i++) {
                x.getInt8(c);
            }
            ASSERT(x);

            // Invalidate stream 'x' by making excessive 'get' calls
            x.getInt8(c);
            ASSERT(!x);
        }
        {
            int i;
            char buffer[4];
            Out o(buffer,  sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj x(buffer, sizeof buffer);
            if(veryVerbose) { P(x) }
            ASSERT(x);

            // Make sure the stream is still valid after an acceptable number
            // of calls to 'get' methods.
            char c;
            for (i = 0; i < sizeof buffer; i++) {
                x.getInt8(c);
            }
            ASSERT(x);

            // Invalidate stream 'x' by making excessive 'get' calls
            x.getInt8(c);
            ASSERT(!x);
        }
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting isEmpty(), length() and cursor()." << endl;

        {
            char buffer[1];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }

            ASSERT(!x.isEmpty());
            ASSERT(x.length() == 1);
            ASSERT(x.cursor() == 0);

            int j;
            char c;
            for (j = 0; j < sizeof buffer; j++) {
                LOOP_ASSERT(j,
                            x.cursor() == SIZEOF_INT8 * j);
                x.getInt8(c);
            }
            ASSERT(x.isEmpty());
        }
        {
            char buffer[2];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }

            ASSERT(!x.isEmpty());
            ASSERT(x.length() == 2);
            ASSERT(x.cursor() == 0);

            int j;
            char c;
            for (j = 0; j < sizeof buffer; j++) {
                LOOP_ASSERT(j,
                            x.cursor() == SIZEOF_INT8 * j);
                x.getInt8(c);
            }
            ASSERT(x.isEmpty());
        }
        {
            char buffer[3];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }

            ASSERT(!x.isEmpty());
            ASSERT(x.length() == 3);
            ASSERT(x.cursor() == 0);

            int j;
            char c;
            for (j = 0; j < sizeof buffer; j++) {
                LOOP_ASSERT(j,
                            x.cursor() == SIZEOF_INT8 * j);
                x.getInt8(c);
            }
            ASSERT(x.isEmpty());
        }
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }

            ASSERT(!x.isEmpty());
            ASSERT(x.length() == 4);
            ASSERT(x.cursor() == 0);

            int j;
            char c;
            for (j = 0; j < sizeof buffer; j++) {
                LOOP_ASSERT(j,
                            x.cursor() == SIZEOF_INT8 * j);
                x.getInt8(c);
            }
            ASSERT(x.isEmpty());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST:
        //   For each independent test, create objects containing various data,
        //   then use primary manipulators 'getInt8' and 'invalidate' to modify
        //   the state of the objects.  Use basic accessors to verify the final
        //   state of the objects.
        //
        // Testing
        //   bdex_ByteInStreamRaw();
        //   bdex_ByteInStreamRaw(const char *buffer, int numBytes);
        //   ~bdex_ByteInStreamRaw();   // by purify
        //   getInt8(char& variable);
        //   void invalidate();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting getInt8(char&) and ctors." << endl;
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj x(buffer, sizeof buffer);
                                             // test ctor initialized w/ char *
            ASSERT(x);
            ASSERT(x.length() == o.length());
            if (veryVerbose) { P(x) }
            char val;
            x.getInt8(val);              ASSERT(1 == val);
            x.getInt8(val);              ASSERT(2 == val);
            x.getInt8(val);              ASSERT(3 == val);
            x.getInt8(val);              ASSERT(4 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }
        {
            char buffer[4];

            Out o(buffer, sizeof buffer);
            o.putInt8(5);
            o.putInt8(6);
            o.putInt8(7);
            o.putInt8(8);

            Obj x(buffer, sizeof buffer);
            ASSERT(x);
            ASSERT(x.length() == o.length());
            if (veryVerbose) { P(x) }
            char val;
            x.getInt8(val);              ASSERT(5 == val);
            x.getInt8(val);              ASSERT(6 == val);
            x.getInt8(val);              ASSERT(7 == val);
            x.getInt8(val);              ASSERT(8 == val);
            ASSERT(x);
            ASSERT(x.isEmpty());
            ASSERT(x.cursor() == x.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting invalidate()." << endl;
        // test objects of variable lengths
        {
            char buffer[1];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            ASSERT(x);
            x.invalidate();
            ASSERT(!x);
        }
        {
            char buffer[2];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            ASSERT(x);
            x.invalidate();
            ASSERT(!x);
        }
        {
            char buffer[3];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            ASSERT(x);
            x.invalidate();
            ASSERT(!x);
        }
        {
            char buffer[4];
            Out o(buffer, sizeof buffer);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj x(buffer, sizeof buffer);
            if (veryVerbose) { P(x) }
            ASSERT(x);
            x.invalidate();
            ASSERT(!x);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdex_ByteInStreamRaw' objects containing various data.
        //   Exercise these objects using appropriate input methods and primary
        //   manipulators, then verify the state of the resulting objects using
        //   basic accessors.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        if (verbose) cout << "\nCreate object x1 and x2 w/ an initial value."
                          << endl;

        Obj x1("\x00", 1);
        Obj x2("\x00\x01\x02\x03\x04", 5);

        if (veryVerbose) { P(x1); }
        if (veryVerbose) { P(x2); }

        ASSERT(1 == x1.length());
        ASSERT(5 == x2.length());

        if (verbose) cout << "\nTry getInt8() with x2." << endl;
        int i;
        for (i = 0; i < 5; i++) {
            char c;
            x2.getInt8(c);
            LOOP_ASSERT(i, i == c);
        }

        if (verbose) cout << "\nTry isEmpty() with x2." << endl;
        ASSERT(1 == x2.isEmpty());

        if (verbose) cout << "\nTry invalidate() with x2." << endl;
        x2.invalidate();
        ASSERT(!x2);

        if (verbose) cout << "\nTry invalid operation with x1." << endl;
        x1.getInt32(i);
        ASSERT(!x1);
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
