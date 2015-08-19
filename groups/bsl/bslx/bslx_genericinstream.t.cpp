// bslx_genericinstream.t.cpp                                         -*-C++-*-

#include <bslx_genericinstream.h>
#include <bslx_genericoutstream.h>  // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// For all input methods in 'GenericInStream', the primary concern is the
// parsing of the byte representation to its correct output value.
//
// We have chosen the primary black-box manipulator for 'GenericInStream' to be
// 'getInt8'.
// ----------------------------------------------------------------------------
// [ 2] GenericInStream(STREAMBUF *streamBuf);
// [ 2] ~GenericInStream();
// [24] getLength(int& variable);
// [24] getVersion(int& variable);
// [11] getInt64(bsls::Types::Int64& variable);
// [11] getUint64(bsls::Types::Uint64& variable);
// [10] getInt56(bsls::Types::Int64& variable);
// [10] getUint56(bsls::Types::Uint64& variable);
// [ 9] getInt48(bsls::Types::Int64& variable);
// [ 9] getUint48(bsls::Types::Uint64& variable);
// [ 8] getInt40(bsls::Types::Int64& variable);
// [ 8] getUint40(bsls::Types::Uint64& variable);
// [ 7] getInt32(int& variable);
// [ 7] getUint32(unsigned int& variable);
// [ 6] getInt24(int& variable);
// [ 6] getUint24(unsigned int& variable);
// [ 5] getInt16(short& variable);
// [ 5] getUint16(unsigned short& variable);
// [ 2] getInt8(char& variable);
// [ 4] getInt8(signed char& variable);
// [ 4] getUint8(char& variable);
// [ 4] getUint8(unsigned char& variable);
// [13] getFloat64(double& variable);
// [12] getFloat32(float& variable);
// [25] getString(bsl::string& variable);
// [21] getArrayInt64(bsls::Types::Int64 *variables, int numVariables);
// [21] getArrayUint64(bsls::Types::Uint64 *variables, int numVariables);
// [20] getArrayInt56(bsls::Types::Int64 *variables, int numVariables);
// [20] getArrayUint56(bsls::Types::Uint64 *variables, int numVariables);
// [19] getArrayInt48(bsls::Types::Int64 *variables, int numVariables);
// [19] getArrayUint48(bsls::Types::Uint64 *variables, int numVariables);
// [18] getArrayInt40(bsls::Types::Int64 *variables, int numVariables);
// [18] getArrayUint40(bsls::Types::Uint64 *variables, int numVariables);
// [17] getArrayInt32(int *variables, int numVariables);
// [17] getArrayUint32(unsigned int *variables, int numVariables);
// [16] getArrayInt24(int *variables, int numVariables);
// [16] getArrayUint24(unsigned int *variables, int numVariables);
// [15] getArrayInt16(short *variables, int numVariables);
// [15] getArrayUint16(unsigned short *variables, int numVariables);
// [14] getArrayInt8(char *variables, int numVariables);
// [14] getArrayInt8(signed char *variables, int numVariables);
// [13] getArrayUint8(char *variables, int numVariables);
// [13] getArrayUint8(unsigned char *variables, int numVariables);
// [23] getArrayFloat64(double *variables, int numVariables);
// [22] getArrayFloat32(float *variables, int numVariables);
// [ 2] void invalidate();
// [ 3] operator const void *() const;
// [ 3] bool isValid() const;
//
// [26] GenericInStream& operator>>(GenericInStream&, TYPE&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [27] THIRD-PARTY EXTERNALIZATION
// [28] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                      HELPER CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

class TestStreamBuf {
    // This class implements a very basic stream buffer suitable for use in
    // 'bslx::GenericOutStream' and 'bslx::GenericInStream'.

    // DATA
    bsl::stringbuf d_buffer;  // buffer

    int            d_limit;   // number of bytes to read before failure; -1
                              // implies  will never fail

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestStreamBuf&);

  public:
    // TYPES
    struct traits_type {
        static int eof() {  return -1;  }
    };

    // CREATORS
    TestStreamBuf();
        // Create an empty stream buffer.

    ~TestStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    int pubsync();
        // Increments the flush count.

    int sbumpc();
        // Read the next character in this buffer.  Return the value of the
        // character on success, and 'traits_type::eof()' otherwise.

    void setLimit(int limit);
        // Set the input limit to the specified 'limit'.

    int sgetc();
        // Peek at the next character in this buffer.  Return the value of
        // the character on success, and 'traits_type::eof()' otherwise.

    bsl::streamsize sgetn(char *s, bsl::streamsize length);
        // Load the specified 'length' characters into the specified address
        // 's', and return the number of characters read.

    int sputc(char c);
        // Write the specified character 'c' to this buffer.  Return 'c' on
        // success, and 'traits_type::eof()' otherwise.

    bsl::streamsize sputn(const char *s, bsl::streamsize length);
        // Write the specified 'length' characters at the specified address 's'
        // to this buffer, and return the number of characters written.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestStreamBuf& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

// CREATORS
TestStreamBuf::TestStreamBuf()
: d_buffer()
, d_limit(-1)
{
}

TestStreamBuf::~TestStreamBuf()
{
}

// MANIPULATORS
int TestStreamBuf::pubsync()
{
    return d_buffer.pubsync();
}

int TestStreamBuf::sbumpc()
{
    if (-1 == d_limit) {
        return d_buffer.sbumpc();                                     // RETURN
    }
    if (0 < d_limit) {
        --d_limit;
        return d_buffer.sbumpc();                                     // RETURN
    }
    return traits_type::eof();
}

void TestStreamBuf::setLimit(int limit)
{
    d_limit = limit;
}

int TestStreamBuf::sgetc()
{
    if (0 != d_limit) {
        return d_buffer.sgetc();                                      // RETURN
    }
    return traits_type::eof();
}

bsl::streamsize TestStreamBuf::sgetn(char *s, bsl::streamsize  length)
{
    if (-1 == d_limit) {
        return d_buffer.sgetn(s, length);                             // RETURN
    }
    if (length <= d_limit) {
        d_limit -= static_cast<int>(length);
        return d_buffer.sgetn(s, length);                             // RETURN
    }
    d_limit = 0;
    return 0;
}

int TestStreamBuf::sputc(char c)
{
    return d_buffer.sputc(c);
}

bsl::streamsize TestStreamBuf::sputn(const char *s, bsl::streamsize length)
{
    return d_buffer.sputn(s, length);
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestStreamBuf& object)
{
    bsl::string         buffer = object.d_buffer.str();
    const int           len    = static_cast<int>(buffer.size());
    const char         *data   = buffer.data();
    bsl::ios::fmtflags  flags  = stream.flags();

    stream << bsl::hex;

    for (int i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) {
            stream << ' ';
        }
        if (0 == i % 8) { // output newline character and address every 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }

        stream << bsl::setw(2)
               << bsl::setfill('0')
               << static_cast<int>(static_cast<unsigned char>(data[i]));
    }

    stream.flags(flags);  // reset stream format flags

    return stream;
}

void debugprint(const TestStreamBuf& object)
{
    bsl::cout << object;
}

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef TestStreamBuf                   Buf;
typedef GenericInStream<TestStreamBuf>  Obj;
typedef GenericOutStream<TestStreamBuf> Out;

const int VERSION_SELECTOR = 20131127;
const int SIZEOF_INT64   = 8;
const int SIZEOF_INT56   = 7;
const int SIZEOF_INT48   = 6;
const int SIZEOF_INT40   = 5;
const int SIZEOF_INT32   = 4;
const int SIZEOF_INT24   = 3;
const int SIZEOF_INT16   = 2;
const int SIZEOF_INT8    = 1;
const int SIZEOF_FLOAT64 = 8;
const int SIZEOF_FLOAT32 = 4;

// ============================================================================
//                      GLOBAL TEST CLASSES
// ----------------------------------------------------------------------------

namespace ThirdParty {

struct MyStruct {
  public:
    enum EnumValue {
        e_A = 7,
        e_B = 8,
        e_C = 9
    };
};

template <class STREAM>
STREAM& bdexStreamIn(STREAM&              stream,
                     MyStruct::EnumValue& variable,
                     int                  version)
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the specified 'version' format, and return a
    // reference to 'stream'.  If 'stream' is initially invalid, this operation
    // has no effect.  If 'version' is not supported, 'variable' is unaltered
    // and 'stream' is invalidated, but otherwise unmodified.  If 'version' is
    // supported but 'stream' becomes invalid during this operation, 'variable'
    // has an undefined, but valid, state.  Note that no version is read from
    // 'stream'.  See the 'bslx' package-level documentation for more
    // information on BDEX streaming of value-semantic types and containers.
{
    using bslx::InStreamFunctions::bdexStreamIn;

    if (stream) {
        switch (version) {
          case 2: {
            short newValue;
            stream.getInt16(newValue);
            if (stream) {
                variable = static_cast<MyStruct::EnumValue>(newValue);
            }
          } break;
          case 1: {
            char newValue;
            stream.getInt8(newValue);
            if (stream) {
                variable = static_cast<MyStruct::EnumValue>(newValue);
            }
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

template <class STREAM>
STREAM& bdexStreamOut(STREAM&                    stream,
                      const MyStruct::EnumValue& value,
                      int                        version)
    // Write the value of this object, using the specified 'version' format, to
    // the specified output 'stream', and return a reference to 'stream'.  If
    // 'stream' is initially invalid, this operation has no effect.  If
    // 'version' is not supported, 'stream' is invalidated, but otherwise
    // unmodified.  Note that 'version' is not written to 'stream'.  See the
    // 'bslx' package-level documentation for more information on BDEX
    // streaming of value-semantic types and containers.
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    if (stream) {
        switch (version) {
          case 2: {
            stream.putInt16(static_cast<short>(value));
          } break;
          case 1: {
            stream.putInt8(static_cast<char>(value));
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

inline
int maxSupportedBdexVersion(const MyStruct::EnumValue *,
                            int                        versionSelector)
    // Return the maximum valid BDEX format version, as indicated by the
    // specified 'versionSelector', to be passed to the 'bdexStreamOut' method.
    // Note that it is highly recommended that 'versionSelector' be formatted
    // as "YYYYMMDD", a date representation.  Also note that 'versionSelector'
    // should be a *compile*-time-chosen value that selects a format version
    // supported by both externalizer and unexternalizer.  See the 'bslx'
    // package-level documentation for more information on BDEX streaming of
    // value-semantic types and containers.
{
    using bslx::VersionFunctions::maxSupportedBdexVersion;

    return versionSelector >= VERSION_SELECTOR ? 2 : 1;
}

}  // close namespace ThirdParty


// ============================================================================
//                                 USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Unexternalization
///- - - - - - - - - - - - - - - - -
// Suppose we wish to implement a (deliberately simple) 'MyPerson' class as a
// value-semantic object that supports BDEX externalization and
// unexternalization.  In addition to whatever data and methods that we choose
// to put into our design, we must supply three methods having specific names
// and signatures in order to comply with the BDEX protocol: a class method
// 'maxSupportedBdexVersion', an accessor (i.e., a 'const' method)
// 'bdexStreamOut', and a manipulator (i.e., a non-'const' method)
// 'bdexStreamIn'.  This example shows how to implement those three methods.
//
// In this example we will not worry overly about "good design" of the
// 'MyPerson' component, and we will declare but not implement illustrative
// methods and free operators, except for the three required BDEX methods,
// which are implemented in full.  In particular, we will not make explicit use
// of 'bslma' allocators; a more complete design would do so:
//
// First, we implement 'MyPerson':
//..
    class MyPerson {
        bsl::string d_firstName;
        bsl::string d_lastName;
        int         d_age;

        friend bool operator==(const MyPerson&, const MyPerson&);

      public:
        // CLASS METHODS
        static int maxSupportedBdexVersion(int versionSelector);
            // Return the maximum valid BDEX format version, as indicated by
            // the specified 'versionSelector', to be passed to the
            // 'bdexStreamOut' method.  Note that it is highly recommended that
            // 'versionSelector' be formatted as "YYYYMMDD", a date
            // representation.  Also note that 'versionSelector' should be a
            // *compile*-time-chosen value that selects a format version
            // supported by both externalizer and unexternalizer.  See the
            // 'bslx' package-level documentation for more information on BDEX
            // streaming of value-semantic types and containers.

        // CREATORS
        MyPerson();
            // Create a default person.

        MyPerson(const char *firstName, const char *lastName, int age);
            // Create a person having the specified 'firstName', 'lastName',
            // and 'age'.

        MyPerson(const MyPerson& original);
            // Create a person having the value of the specified 'original'
            // person.

        ~MyPerson();
            // Destroy this object.

        // MANIPULATORS
        MyPerson& operator=(const MyPerson& rhs);
            // Assign to this person the value of the specified 'rhs' person,
            // and return a reference to this person.

        template <class STREAM>
        STREAM& bdexStreamIn(STREAM& stream, int version);
            // Assign to this object the value read from the specified input
            // 'stream' using the specified 'version' format, and return a
            // reference to 'stream'.  If 'stream' is initially invalid, this
            // operation has no effect.  If 'version' is not supported, this
            // object is unaltered and 'stream' is invalidated, but otherwise
            // unmodified.  If 'version' is supported but 'stream' becomes
            // invalid during this operation, this object has an undefined, but
            // valid, state.  Note that no version is read from 'stream'.  See
            // the 'bslx' package-level documentation for more information on
            // BDEX streaming of value-semantic types and containers.

        //...

        // ACCESSORS
        int age() const;
            // Return the age of this person.

        template <class STREAM>
        STREAM& bdexStreamOut(STREAM& stream, int version) const;
            // Write the value of this object, using the specified 'version'
            // format, to the specified output 'stream', and return a reference
            // to 'stream'.  If 'stream' is initially invalid, this operation
            // has no effect.  If 'version' is not supported, 'stream' is
            // invalidated, but otherwise unmodified.  Note that 'version' is
            // not written to 'stream'.  See the 'bslx' package-level
            // documentation for more information on BDEX streaming of
            // value-semantic types and containers.

        const bsl::string& firstName() const;
            // Return the first name of this person.

        const bsl::string& lastName() const;
            // Return the last name of this person.

        //...

    };

    // FREE OPERATORS
    bool operator==(const MyPerson& lhs, const MyPerson& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' person objects have
        // the same value, and 'false' otherwise.  Two person objects have the
        // same value if they have the same first name, last name, and age.

    bool operator!=(const MyPerson& lhs, const MyPerson& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
        // have the same value, and 'false' otherwise.  Two person objects
        // differ in value if they differ in first name, last name, or age.

    // ========================================================================
    //                  INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CLASS METHODS
    inline
    int MyPerson::maxSupportedBdexVersion(int /* versionSelector */) {
        return 1;
    }

    // CREATORS
    inline
    MyPerson::MyPerson()
    : d_firstName("")
    , d_lastName("")
    , d_age(0)
    {
    }

    inline
    MyPerson::MyPerson(const char *firstName, const char *lastName, int age)
    : d_firstName(firstName)
    , d_lastName(lastName)
    , d_age(age)
    {
    }

    inline
    MyPerson::~MyPerson()
    {
    }

    template <class STREAM>
    STREAM& MyPerson::bdexStreamIn(STREAM& stream, int version)
    {
        if (stream) {
            switch (version) {  // switch on the 'bslx' version
              case 1: {
                stream.getString(d_firstName);
                if (!stream) {
                    d_firstName = "stream error";  // *might* be corrupted;
                                                   //  value for testing
                    return stream;                                    // RETURN
                }
                stream.getString(d_lastName);
                if (!stream) {
                    d_lastName = "stream error";  // *might* be corrupted;
                                                  //  value for testing
                    return stream;                                    // RETURN
                }
                stream.getInt32(d_age);
                if (!stream) {
                    d_age = 999;     // *might* be corrupted; value for testing
                    return stream;                                    // RETURN
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
    inline
    int MyPerson::age() const
    {
        return d_age;
    }

    template <class STREAM>
    STREAM& MyPerson::bdexStreamOut(STREAM& stream, int version) const
    {
        switch (version) {
          case 1: {
            stream.putString(d_firstName);
            stream.putString(d_lastName);
            stream.putInt32(d_age);
          } break;
          default: {
            stream.invalidate();
          } break;
        }
        return stream;
    }

    inline
    const bsl::string& MyPerson::firstName() const
    {
        return d_firstName;
    }

    inline
    const bsl::string& MyPerson::lastName() const
    {
        return d_lastName;
    }

    // FREE OPERATORS
    inline
    bool operator==(const MyPerson& lhs, const MyPerson& rhs)
    {
        return lhs.d_firstName == rhs.d_firstName &&
               lhs.d_lastName  == rhs.d_lastName  &&
               lhs.d_age       == rhs.d_age;
    }

    inline
    bool operator!=(const MyPerson& lhs, const MyPerson& rhs)
    {
        return !(lhs == rhs);
    }

    class MyStreamBuf {
        // This class implements a very basic stream buffer suitable for use in
        // 'bslx::GenericOutStream' and 'bslx::GenericInStream'.

        // DATA
        bsl::deque<char> d_buffer;  // the input and output buffer

      private:
        // NOT IMPLEMENTED
        MyStreamBuf(const MyStreamBuf&);
        MyStreamBuf& operator=(const MyStreamBuf&);

      public:
        // TYPES
        struct traits_type {
            static int eof() { return -1; }
        };

        // CREATORS
        MyStreamBuf();
            // Create an empty stream buffer.

        ~MyStreamBuf();
            // Destroy this stream buffer.

        // MANIPULATORS
        int pubsync();
            // Return 0.

        int sbumpc();
            // Read the next character in this buffer.  Return the value of the
            // character on success, and 'traits_type::eof()' otherwise.

        int sgetc();
            // Peek at the next character in this buffer.  Return the value of
            // the character on success, and 'traits_type::eof()' otherwise.

        bsl::streamsize sgetn(char *s, bsl::streamsize length);
            // Load the specified 'length' characters into the specified
            // address 's', and return the number of characters read.

        int sputc(char c);
            // Write the specified character 'c' to this buffer.  Return 'c' on
            // success, and 'traits_type::eof()' otherwise.

        bsl::streamsize sputn(const char *s, bsl::streamsize length);
            // Write the specified 'length' characters at the specified address
            // 's' to this buffer, and return the number of characters written.
    };

    // ========================================================================
    //                  INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CREATORS
    MyStreamBuf::MyStreamBuf()
    : d_buffer()
    {
    }

    MyStreamBuf::~MyStreamBuf()
    {
    }

    // MANIPULATORS
    int MyStreamBuf::pubsync()
    {
        // In this implementation, there is nothing to be done except return
        // success.

        return 0;
    }

    int MyStreamBuf::sbumpc()
    {
        if (!d_buffer.empty()) {
            const int rv = static_cast<int>(d_buffer.front());
            d_buffer.pop_front();
            return rv;                                                // RETURN
        }
        return traits_type::eof();
    }

    int MyStreamBuf::sgetc()
    {
        if (!d_buffer.empty()) {
            return static_cast<int>(d_buffer.front());                // RETURN
        }
        return traits_type::eof();
    }

    bsl::streamsize MyStreamBuf::sgetn(char *s, bsl::streamsize length)
    {
        for (bsl::streamsize i = 0; i < length; ++i) {
            if (d_buffer.empty()) {
                return i;                                             // RETURN
            }
            s[i] = d_buffer.front();
            d_buffer.pop_front();
        }
        return length;
    }

    int MyStreamBuf::sputc(char c)
    {
        d_buffer.push_back(c);
        return static_cast<int>(c);
    }

    bsl::streamsize MyStreamBuf::sputn(const char      *s,
                                       bsl::streamsize  length)
    {
        for (bsl::streamsize i = 0; i < length; ++i) {
            d_buffer.push_back(s[i]);
        }
        return length;
    }

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 28: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

// Then, we can exercise the new 'MyPerson' value-semantic class by
// externalizing and reconstituting an object.  First, create a 'MyPerson'
// 'janeSmith1' and a 'bslx::GenericOutStream' 'outStream1':
//..
    MyPerson                               janeSmith1("Jane", "Smith", 42);
    bsl::stringbuf                         buffer1;
    bslx::GenericOutStream<bsl::stringbuf> outStream1(&buffer1, 20131127);
    const int                              VERSION1 = 1;
    outStream1.putVersion(VERSION1);
    janeSmith1.bdexStreamOut(outStream1, VERSION1);
    ASSERT(outStream1.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy1' initialized to the default value, and
// assert that 'janeCopy1' is different from 'janeSmith1':
//..
    MyPerson janeCopy1;
    ASSERT(janeCopy1 != janeSmith1);
//..
// Then, create a 'bslx::GenericInStream' 'inStream1' initialized with the
// buffer from the 'bslx::GenericOutStream' object 'outStream1' and
// unexternalize this data into 'janeCopy1':
//..
    bslx::GenericInStream<bsl::stringbuf> inStream1(&buffer1);
    int                                   version1;
    inStream1.getVersion(version1);
    janeCopy1.bdexStreamIn(inStream1, version1);
    ASSERT(inStream1.isValid());
//..
// Finally, 'assert' the obtained values are as expected and display the
// results to 'bsl::stdout':
//..
    ASSERT(version1  == VERSION1);
    ASSERT(janeCopy1 == janeSmith1);

if (veryVerbose) {
    if (janeCopy1 == janeSmith1) {
        bsl::cout << "Successfully serialized and de-serialized Jane Smith:"
                  << "\n\tFirstName: " << janeCopy1.firstName()
                  << "\n\tLastName : " << janeCopy1.lastName()
                  << "\n\tAge      : " << janeCopy1.age() << bsl::endl;
    }
    else {
        bsl::cout << "Serialization unsuccessful.  'janeCopy1' holds:"
                  << "\n\tFirstName: " << janeCopy1.firstName()
                  << "\n\tLastName : " << janeCopy1.lastName()
                  << "\n\tAge      : " << janeCopy1.age() << bsl::endl;
    }
} // if (veryVerbose)
//..
//
///Example 2: Sample 'STREAMBUF' Implementation
///- - - - - - - - - - - - - - - - - - - - - -
// For this example, we will implement 'MyStreamBuf', a minimal 'STREAMBUF' to
// to be used with 'bslx::GenericInStream' and 'bslx::GenericOutStream'.  The
// implementation will consist of only what is required of the type.  For
// comparison, we will reuse 'MyPerson' and repeat part of {Example 1} to
// demonstrate how to use 'bslx::GenericInStream'.
//
// First, we implement 'MyStreamBuf' (which, for brevity, simply uses the
// default allocator):
//..
//..
// Then, we create a 'MyPerson' 'janeSmith2' and a 'bslx::GenericOutStream'
// 'outStream2':
//..
    MyPerson                               janeSmith2("Jane", "Smith", 42);
    MyStreamBuf                            buffer2;
    bslx::GenericOutStream<MyStreamBuf>    outStream2(&buffer2, 20131127);
    const int                              VERSION2 = 1;
    outStream2.putVersion(VERSION2);
    janeSmith2.bdexStreamOut(outStream2, VERSION2);
    ASSERT(outStream2.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy2' initialized to the default value, and
// assert that 'janeCopy2' is different from 'janeSmith2':
//..
    MyPerson janeCopy2;
    ASSERT(janeCopy2 != janeSmith2);
//..
// Then, create a 'bslx::GenericInStream' 'inStream2' initialized with the
// buffer from the 'bslx::GenericOutStream' object 'outStream2' and
// unexternalize this data into 'janeCopy2':
//..
    bslx::GenericInStream<MyStreamBuf>    inStream2(&buffer2);
    int                                   version2;
    inStream2.getVersion(version2);
    janeCopy2.bdexStreamIn(inStream2, version2);
    ASSERT(inStream2.isValid());
//..
// Finally, 'assert' the obtained values are as expected:
//..
    ASSERT(version2  == VERSION2);
    ASSERT(janeCopy2 == janeSmith2);
//..

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // THIRD-PARTY EXTERNALIZATION
        //   Verify the technique described for overloading 'bdexStreamOut',
        //   'bdexStreamIn', and 'maxSupportedBdexVersion' functions is valid.
        //
        // Concerns:
        //: 1 Overloading the methods in the component's namespace will
        //:   allow access to these methods as expected (i.e., a 'using' clause
        //:   followed by unqualified access to the method).
        //:
        //: 2 'maxSupportedBdexVersion' provides the expected value.
        //:
        //: 3 'operator<<' externalizes the expected bytes.
        //:
        //: 4 'operator>>' unexternalizes the expected value.
        //
        // Plan:
        //: 1 Define an 'enum' wrapped in a 'struct' that externalizes
        //:   differently based upon version.
        //:
        //: 2 Directly verify the return value of 'maxSupportedBdexVersion' for
        //:   this 'enum'.  (C-2)
        //:
        //: 3 Verify the results of 'operator<<' and 'operator>>' against the
        //:   expected values.  (C-1, C-3..4)
        //
        // Testing:
        //   THIRD-PARTY EXTERNALIZATION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "THIRD-PARTY EXTERNALIZATION" << endl
                          << "===========================" << endl;

        {
            using bslx::VersionFunctions::maxSupportedBdexVersion;
            ThirdParty::MyStruct::EnumValue *v = 0;
            ASSERT(2 == maxSupportedBdexVersion(v, VERSION_SELECTOR));
            ASSERT(1 == maxSupportedBdexVersion(v, 0));
        }
        {
            ThirdParty::MyStruct::EnumValue initial =
                                                     ThirdParty::MyStruct::e_A;
            ThirdParty::MyStruct::EnumValue value   =
                                                     ThirdParty::MyStruct::e_B;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o << initial;

            Obj mX(&b);
            mX >> value;
            ASSERT(value == initial);
        }
        {
            ThirdParty::MyStruct::EnumValue initial =
                                                     ThirdParty::MyStruct::e_A;
            ThirdParty::MyStruct::EnumValue value   =
                                                     ThirdParty::MyStruct::e_B;

            Buf b;

            Out o(&b, 0);
            o << initial;

            Obj mX(&b);
            mX >> value;
            ASSERT(value == initial);
        }

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // UNEXTERNALIZATION FREE OPERATOR
        //   Verify 'operator>>' works correctly.
        //
        // Concerns:
        //: 1 The method inline-forwards to the implementation correctly.
        //:
        //: 2 Invocations of the method can be chained.
        //
        // Plan:
        //: 1 Externalize a set of values to an out stream.
        //:
        //: 2 Unexternalize the values and verify the values match the initial
        //:   values.  (C-1)
        //:
        //: 3 Unexternalize a set of values from the stream in one code line.
        //:   (C-2)
        //
        // Testing:
        //   GenericInStream& operator>>(GenericInStream&, TYPE&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNEXTERNALIZATION FREE OPERATOR" << endl
                          << "===============================" << endl;

        {
            char initial = 'a';
            char value = 'b';

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o << initial;

            Obj mX(&b);
            mX >> value;
            ASSERT(value == initial);
        }
        {
            double initial = 7.0;
            double value = 1.0;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o << initial;

            Obj mX(&b);
            mX >> value;
            ASSERT(value == initial);
        }
        {
            bsl::vector<int> initial;
            bsl::vector<int> value;
            value.push_back(3);
            for (int i = 0; i < 5; ++i) {
                if (veryVerbose) { P(i); }

                Buf b;

                Out o(&b, VERSION_SELECTOR);
                o << initial;

                Obj mX(&b);
                mX >> value;
                LOOP_ASSERT(i, value == initial);
                initial.push_back(i);
            }
        }
        {
            float       initial1 = 3.0;
            float       value1 = 1.0;
            bsl::string initial2 = "hello";
            bsl::string value2 = "bye";
            short       initial3 = 2;
            short       value3 = 1;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o << initial1 << initial2 << initial3;

            Obj mX(&b);
            mX >> value1 >> value2 >> value3;
            ASSERT(value1 == initial1);
            ASSERT(value2 == initial2);
            ASSERT(value3 == initial3);
        }

        {
            // Verify the method has no effect if the stream is invalid.

            char initial = 'a';
            char value = 'b';

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o << initial;

            Obj mX(&b);
            mX.invalidate();
            mX >> value;
            ASSERT('b' == value);
        }

        {
            // Verify the return value.

            char initial = 'a';
            char value = 'b';

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o << initial;

            Obj mX(&b);
            ASSERT(&mX == &(mX >> value));
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'getString'
        //   Verify this method unexternalizes the expected values.
        //
        // Concerns:
        //: 1 The method unexternalizes the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.
        //:   (C-1..2)
        //
        // Testing:
        //   getString(bsl::string& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'getString'" << endl
                 << "===================" << endl;
        }

        if (verbose) {
            cout << "\nTesting 'getString(bsl::string&)'." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));    o.putInt8(0xFF);
            o.putString(bsl::string("beta"));     o.putInt8(0xFE);
            o.putString(bsl::string("gamma"));    o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char        marker;
            bsl::string val;
            mX.getString(val);         mX.getInt8(marker);
            ASSERT(val == "alpha");    ASSERT('\xFF' == marker);
            mX.getString(val);         mX.getInt8(marker);
            ASSERT(val == "beta");     ASSERT('\xFE' == marker);
            mX.getString(val);         mX.getInt8(marker);
            ASSERT(val == "gamma");    ASSERT('\xFD' == marker);
            ASSERT(X);
        }

        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));

            Obj mX(&b);

            bsl::string val;
            mX.invalidate();
            mX.getString(val);
        }

        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));

            Obj mX(&b);

            bsl::string val;
            ASSERT(&mX == &mX.getString(val));
        }
        {
            // Verify error handling.

            const int SIZE = 6 * SIZEOF_INT8;
            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putString(bsl::string("alpha"));

                Obj mX(&b);  const Obj& X = mX;

                bsl::string val;
                mX.getString(val);
                ASSERT((i == SIZE) == X.isValid());
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // GET LENGTH AND VERSION
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getLength(int& variable);
        //   getVersion(int& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET LENGTH AND VERSION" << endl
                 << "======================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getLength(int&)." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putLength(1);             o.putInt8(0xFF);
            o.putLength(128);           o.putInt8(0xFE);
            o.putLength(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            int  val;
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(128 == val);        ASSERT('\xFE' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putLength(128);             o.putInt8(0xFD);
            o.putLength(127);             o.putInt8(0xFE);
            o.putLength(256);             o.putInt8(0xFF);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            int  val;
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(128 == val);        ASSERT('\xFD' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(127 == val);        ASSERT('\xFE' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(256 == val);        ASSERT('\xFF' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putLength(3);

            Obj mX(&b);

            int val = 0;
            mX.invalidate();
            mX.getLength(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putLength(3);

            Obj mX(&b);
            int val;
            ASSERT(&mX == &mX.getLength(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT8; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putLength(3);

                Obj mX(&b);  const Obj& X = mX;

                int val;
                mX.getLength(val);
                ASSERT((i == SIZEOF_INT8) == X.isValid());
            }
            for (int i = 0; i <= SIZEOF_INT32; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putLength(1000);

                Obj mX(&b);  const Obj& X = mX;

                int val;
                mX.getLength(val);
                ASSERT((i == SIZEOF_INT32) == X.isValid());
            }
        }

        if (verbose) {
            cout << "\nTesting getVersion(int&)." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putVersion(1);
            o.putVersion(2);
            o.putVersion(3);
            o.putVersion(4);

            Obj mX(&b);  const Obj& X = mX;
            if (veryVerbose) { P(b) }
            int val;
            mX.getVersion(val);            ASSERT(1 == val);
            mX.getVersion(val);            ASSERT(2 == val);
            mX.getVersion(val);            ASSERT(3 == val);
            mX.getVersion(val);            ASSERT(4 == val);
            ASSERT(X);
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putVersion(252);
            o.putVersion(253);
            o.putVersion(254);
            o.putVersion(255);

            Obj mX(&b);  const Obj& X = mX;
            if (veryVerbose) { P(b) }
            int val;
            mX.getVersion(val);            ASSERT(252 == val);
            mX.getVersion(val);            ASSERT(253 == val);
            mX.getVersion(val);            ASSERT(254 == val);
            mX.getVersion(val);            ASSERT(255 == val);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putVersion(3);

            Obj mX(&b);

            int val = 0;
            mX.invalidate();
            mX.getVersion(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putVersion(3);

            Obj mX(&b);
            int val;
            ASSERT(&mX == &mX.getVersion(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT8; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putVersion(3);

                Obj mX(&b);  const Obj& X = mX;

                int val;
                mX.getVersion(val);
                ASSERT((i == SIZEOF_INT8) == X.isValid());
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // GET 64-BIT FLOAT ARRAY TEST
        //   Verify this method unexternalizes the expected values.
        //
        // Concerns:
        //: 1 The method unexternalizes the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayFloat64(double *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                          << "GET 64-BIT FLOAT ARRAY TEST" << endl
                          << "===========================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getArrayFloat64." << endl;
        }
        {
            const double DATA[] = { 1, 2, 3 };
            const double V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat64(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char   marker;
            double ar[] = { V, V, V };
            mX.getArrayFloat64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayFloat64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayFloat64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayFloat64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const double DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 3);

            Obj mX(&b);

            double ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayFloat64(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const double DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 3);

            Obj mX(&b);

            double ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayFloat64(ar, 3));
        }
        {
            // Verify error handling.

            typedef double T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_FLOAT64;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayFloat64(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayFloat64(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            double DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayFloat64(static_cast<double *>(0),
                                                   0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayFloat64(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayFloat64(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayFloat64(DATA, 1));
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // GET 32-BIT FLOAT ARRAY TEST
        //   Verify this method unexternalizes the expected values.
        //
        // Concerns:
        //: 1 The method unexternalizes the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayFloat32(float *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 32-BIT FLOAT ARRAY TEST" << endl
                 << "===========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getArrayFloat32." << endl;
        }
        {
            const float DATA[] = { 1, 2, 3 };
            const float V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat32(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char  marker;
            float ar[] = { V, V, V };
            mX.getArrayFloat32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayFloat32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayFloat32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayFloat32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const float DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 3);

            Obj mX(&b);

            float ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayFloat32(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const float DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 3);

            Obj mX(&b);

            float ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayFloat32(ar, 3));
        }
        {
            // Verify error handling.

            typedef float T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_FLOAT32;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayFloat32(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayFloat32(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            float DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayFloat32(static_cast<float *>(0),
                                                   0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayFloat32(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayFloat32(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayFloat32(DATA, 1));
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // GET 64-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt64(bsls::Types::Int64 *variables, int numVariables);
        //   getArrayUint64(bsls::Types::Uint64 *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 64-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getArrayInt64." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Int64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt64(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 ar[] = { V, V, V };
            mX.getArrayInt64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt64(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt64(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt64(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt64(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Int64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT64;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt64(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt64(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint64." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint64(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint64(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint64(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint64(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 ar[] = { V, V, V };
            mX.getArrayUint64(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint64(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint64(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint64(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint64(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint64(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint64(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint64(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Uint64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT64;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint64(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint64(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt64(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt64(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt64(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt64(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt64(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint64(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint64(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint64(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint64(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint64(DATA, 1));
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // GET 56-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt56(bsls::Types::Int64 *variables, int numVariables);
        //   getArrayUint56(bsls::Types::Uint64 *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 56-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getArrayInt56." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Int64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt56(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt56(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt56(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt56(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 ar[] = { V, V, V };
            mX.getArrayInt56(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt56(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt56(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt56(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt56(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt56(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt56(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt56(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Int64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT56;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt56(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt56(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint56." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint56(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint56(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint56(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint56(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;
            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 ar[] = { V, V, V };
            mX.getArrayUint56(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint56(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint56(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint56(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint56(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint56(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint56(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint56(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Uint64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT56;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint56(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint56(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt56(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt56(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt56(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt56(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt56(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint56(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint56(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint56(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint56(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint56(DATA, 1));
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // GET 48-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt48(bsls::Types::Int64 *variables, int numVariables);
        //   getArrayUint48(bsls::Types::Uint64 *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 48-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getArrayInt48." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Int64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt48(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt48(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt48(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt48(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 ar[] = { V, V, V };
            mX.getArrayInt48(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt48(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt48(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt48(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt48(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt48(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt48(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt48(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Int64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT48;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt48(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt48(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint48." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint48(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint48(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint48(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint48(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 ar[] = { V, V, V };
            mX.getArrayUint48(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint48(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint48(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint48(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint48(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint48(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint48(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint48(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Uint64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT48;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint48(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint48(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt48(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt48(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt48(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt48(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt48(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint48(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint48(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint48(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint48(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint48(DATA, 1));
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // GET 40-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt40(bsls::Types::Int64 *variables, int numVariables);
        //   getArrayUint40(bsls::Types::Uint64 *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 40-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getArrayInt40." << endl;
        }
        {
            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Int64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt40(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt40(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt40(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt40(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 ar[] = { V, V, V };
            mX.getArrayInt40(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt40(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt40(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt40(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt40(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt40(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt40(DATA, 3);

            Obj mX(&b);

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt40(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Int64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT40;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt40(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt40(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint40." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint40(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint40(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint40(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint40(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 ar[] = { V, V, V };
            mX.getArrayUint40(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint40(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint40(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint40(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint40(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint40(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint40(DATA, 3);

            Obj mX(&b);

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint40(ar, 3));
        }
        {
            // Verify error handling.

            typedef bsls::Types::Uint64 T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT40;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint40(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint40(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt40(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt40(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt40(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt40(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt40(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint40(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint40(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint40(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint40(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint40(DATA, 1));
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // GET 32-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt32(int *variables, int numVariables);
        //   getArrayUint32(unsigned int *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 32-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getArrayInt32." << endl;
        }
        {
            const int DATA[] = { 1, 2, 3 };
            const int V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt32(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            int  ar[] = { V, V, V };
            mX.getArrayInt32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt32(DATA, 3);

            Obj mX(&b);

            int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt32(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt32(DATA, 3);

            Obj mX(&b);

            int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt32(ar, 3));
        }
        {
            // Verify error handling.

            typedef int T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT32;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt32(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt32(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint32." << endl;
        }
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint32(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint32(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint32(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint32(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char         marker;
            unsigned int ar[] = { V, V, V };
            mX.getArrayUint32(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint32(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint32(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint32(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint32(DATA, 3);

            Obj mX(&b);

            unsigned int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint32(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const unsigned int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint32(DATA, 3);

            Obj mX(&b);

            unsigned int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint32(ar, 3));
        }
        {
            // Verify error handling.

            typedef unsigned int T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT32;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint32(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint32(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            int DATA[5];
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt32(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt32(static_cast<int *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt32(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt32(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt32(DATA, 1));
            }
        }
        {
            unsigned int DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint32(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint32(
                                           static_cast<unsigned int *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint32(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint32(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint32(DATA, 1));
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // GET 24-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt24(int *variables, int numVariables);
        //   getArrayUint24(unsigned int *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 24-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getArrayInt24." << endl;
        }
        {
            const int DATA[] = { 1, 2, 3 };
            const int V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt24(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt24(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt24(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt24(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            int  ar[] = { V, V, V };
            mX.getArrayInt24(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt24(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt24(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt24(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt24(DATA, 3);

            Obj mX(&b);

            int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt24(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt24(DATA, 3);

            Obj mX(&b);

            int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt24(ar, 3));
        }
        {
            // Verify error handling.

            typedef int T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT24;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt24(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt24(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint24." << endl;
        }
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint24(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint24(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint24(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint24(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char         marker;
            unsigned int ar[] = { V, V, V };
            mX.getArrayUint24(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint24(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint24(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint24(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint24(DATA, 3);

            Obj mX(&b);

            unsigned int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint24(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const unsigned int DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint24(DATA, 3);

            Obj mX(&b);

            unsigned int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint24(ar, 3));
        }
        {
            // Verify error handling.

            typedef unsigned int T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT24;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint24(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint24(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            int DATA[5];
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt24(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt24(static_cast<int *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt24(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt24(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt24(DATA, 1));
            }
        }
        {
            unsigned int DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint24(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint24(
                                           static_cast<unsigned int *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint24(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint24(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint24(DATA, 1));
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // GET 16-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   getArrayInt16(short *variables, int numVariables);
        //   getArrayUint16(unsigned short *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 16-BIT INTEGER ARRAYS TEST" << endl
                 << "==============================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getArrayInt16." << endl;
        }
        {
            const short DATA[] = { 1, 2, 3 };
            const short V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt16(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt16(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt16(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt16(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char  marker;
            short ar[] = { V, V, V };
            mX.getArrayInt16(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt16(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt16(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt16(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const short DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt16(DATA, 3);

            Obj mX(&b);

            short ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt16(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const short DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt16(DATA, 3);

            Obj mX(&b);

            short ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt16(ar, 3));
        }
        {
            // Verify error handling.

            typedef short T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT16;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt16(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt16(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint16." << endl;
        }
        {
            const unsigned short DATA[] = { 1, 2, 3 };
            const unsigned short V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint16(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint16(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint16(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint16(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char           marker;
            unsigned short ar[] = { V, V, V };
            mX.getArrayUint16(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint16(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint16(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint16(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned short DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint16(DATA, 3);

            Obj mX(&b);

            unsigned short ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint16(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const unsigned short DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint16(DATA, 3);

            Obj mX(&b);

            unsigned short ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint16(ar, 3));
        }
        {
            // Verify error handling.

            typedef unsigned short T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT16;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint16(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint16(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            short DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt16(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt16(static_cast<short *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt16(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt16(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt16(DATA, 1));
            }
        }
        {
            unsigned short DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint16(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint16(
                                         static_cast<unsigned short *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayUint16(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint16(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayUint16(DATA, 1));
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // GET 8-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //    getArrayInt8(char *variables, int numVariables);
        //    getArrayInt8(signed char *variables, int numVariables);
        //    getArrayUint8(char *variables, int numVariables);
        //    getArrayUint8(unsigned char *variables, int numVariables);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 8-BIT INTEGER ARRAYS TEST" << endl
                 << "=============================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getArrayInt8(char*, int)." << endl;
        }
        {
            const char DATA[] = { 1, 2, 3 };
            const char V = static_cast<char>(0xFF);

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            char ar[] = { V, V, V };
            mX.getArrayInt8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(&b);

            char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(&b);

            char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt8(ar, 3));
        }
        {
            // Verify error handling.

            typedef char T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT8;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt8(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt8(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing getArrayInt8(signed char*, int)." << endl;
        }
        {
            const signed char DATA[] = { 1, 2, 3 };
            const signed char V = static_cast<signed char>(0xFF);

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;
            if (veryVerbose) { P(b) }

            char        marker;
            signed char ar[] = { V, V, V };
            mX.getArrayInt8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayInt8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayInt8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayInt8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const signed char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(&b);

            signed char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const signed char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(&b);

            signed char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt8(ar, 3));
        }
        {
            // Verify error handling.

            typedef signed char T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT8;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayInt8(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayInt8(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint8(char*, int)." << endl;
        }
        {
            const char DATA[] = { 1, 2, 3 };
            const char V = static_cast<char>(0xFF);

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            char ar[] = { V, V, V };
            mX.getArrayUint8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(&b);

            char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(&b);

            char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint8(ar, 3));
        }
        {
            // Verify error handling.

            typedef char T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT8;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint8(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint8(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint8(unsigned char*, int)." << endl;
        }
        {
            const unsigned char DATA[] = { 1, 2, 3 };
            const unsigned char V = 0xFF;

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj mX(&b);  const Obj& X = mX;
            if (veryVerbose) { P(b) }

            char          marker;
            unsigned char ar[] = { V, V, V };
            mX.getArrayUint8(ar, 0);
            ASSERT(V == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFF' == marker);

            mX.getArrayUint8(ar, 1);
            ASSERT(DATA[0] == ar[0] && V == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFE' == marker);

            mX.getArrayUint8(ar, 2);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && V == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFD' == marker);

            mX.getArrayUint8(ar, 3);
            ASSERT(DATA[0] == ar[0] && DATA[1] == ar[1] && DATA[2] == ar[2]);
            mX.getInt8(marker);            ASSERT('\xFC' == marker);

            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(&b);

            unsigned char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
        }
        {
            // Verify the return value.

            const unsigned char DATA[] = { 1, 2, 3 };

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(&b);

            unsigned char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint8(ar, 3));
        }
        {
            // Verify error handling.

            typedef unsigned char T;
            const T   DATA[] = { 1, 2, 3 };
            T         ar[]   = { 0, 0, 0 };
            const int NUM  = static_cast<int>(sizeof DATA / sizeof *DATA);
            const int SIZE = NUM * SIZEOF_INT8;

            for (int i = 0; i <= SIZE; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putArrayUint8(DATA, NUM);

                Obj mX(&b);  const Obj& X = mX;

                mX.getArrayUint8(ar, NUM);
                ASSERT((i == SIZE) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            char DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt8(static_cast<char *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_FAIL(mX.getArrayInt8(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt8(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_PASS(mX.getArrayInt8(DATA, 1));
            }
        }
        {
            signed char DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayInt8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_SAFE_FAIL(mX.getArrayInt8(
                                            static_cast<signed char *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_FAIL(mX.getArrayInt8(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_PASS(mX.getArrayInt8(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_PASS(mX.getArrayInt8(DATA, 1));
            }
        }
        {
            char DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_SAFE_FAIL(mX.getArrayUint8(static_cast<char *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_FAIL(mX.getArrayUint8(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 1));
            }
        }
        {
            unsigned char DATA[5];

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putArrayUint8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(&b);
                ASSERT_SAFE_FAIL(mX.getArrayUint8(
                                          static_cast<unsigned char *>(0), 0));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_FAIL(mX.getArrayUint8(DATA, -1));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 0));
            }
            {
                Obj mX(&b);
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 1));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // GET 64-BIT FLOAT TEST
        //   Verify this method unexternalizes the expected values.
        //
        // Concerns:
        //: 1 The method unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getFloat64(double& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 64-BIT FLOAT TEST" << endl
                 << "=====================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getFloat64." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putFloat64(1);           o.putInt8(0xFF);
            o.putFloat64(2);           o.putInt8(0xFE);
            o.putFloat64(3);           o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char   marker;
            double val;
            mX.getFloat64(val);        mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getFloat64(val);        mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getFloat64(val);        mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putFloat64(3);

            Obj mX(&b);

            double val = 0;
            mX.invalidate();
            mX.getFloat64(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putFloat64(3);

            Obj mX(&b);

            double val;
            ASSERT(&mX == &mX.getFloat64(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_FLOAT64; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putFloat64(3);

                Obj mX(&b);  const Obj& X = mX;

                double val;
                mX.getFloat64(val);
                ASSERT((i == SIZEOF_FLOAT64) == X.isValid());
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // GET 32-BIT FLOAT TEST
        //   Verify this method unexternalizes the expected values.
        //
        // Concerns:
        //: 1 The method unexternalizes the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getFloat32(float& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 32-BIT FLOAT TEST" << endl
                 << "=====================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getFloat32." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putFloat32(1);           o.putInt8(0xFF);
            o.putFloat32(2);           o.putInt8(0xFE);
            o.putFloat32(3);           o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char  marker;
            float val;
            mX.getFloat32(val);        mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getFloat32(val);        mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getFloat32(val);        mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putFloat32(3);

            Obj mX(&b);

            float val = 0;
            mX.invalidate();
            mX.getFloat32(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putFloat32(3);

            Obj mX(&b);

            float val;
            ASSERT(&mX == &mX.getFloat32(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_FLOAT32; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putFloat32(3);

                Obj mX(&b);  const Obj& X = mX;

                float val;
                mX.getFloat32(val);
                ASSERT((i == SIZEOF_FLOAT32) == X.isValid());
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GET 64-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt64(bsls::Types::Int64& variable);
        //   getUint64(bsls::Types::Uint64& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 64-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getInt64." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt64(1);             o.putInt8(0xFF);
            o.putInt64(2);             o.putInt8(0xFE);
            o.putInt64(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt64(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt64(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt64(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt64(3);

            Obj mX(&b);

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt64(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt64(3);

            Obj mX(&b);

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt64(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT64; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt64(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Int64 val;
                mX.getInt64(val);
                ASSERT((i == SIZEOF_INT64) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint64." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint64(1);             o.putInt8(0xFF);
            o.putUint64(2);             o.putInt8(0xFE);
            o.putUint64(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint64(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint64(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint64(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint64(3);

            Obj mX(&b);

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint64(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint64(3);

            Obj mX(&b);

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint64(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT64; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint64(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Uint64 val;
                mX.getUint64(val);
                ASSERT((i == SIZEOF_INT64) == X.isValid());
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // GET 56-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt56(bsls::Types::Int64& variable);
        //   getUint56(bsls::Types::Uint64& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 56-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }
        if (verbose) {
            cout << "\nTesting getInt56." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt56(1);             o.putInt8(0xFF);
            o.putInt56(2);             o.putInt8(0xFE);
            o.putInt56(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt56(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt56(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt56(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt56(3);

            Obj mX(&b);

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt56(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt56(3);

            Obj mX(&b);

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt56(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT56; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt56(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Int64 val;
                mX.getInt56(val);
                ASSERT((i == SIZEOF_INT56) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint56." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint56(1);             o.putInt8(0xFF);
            o.putUint56(2);             o.putInt8(0xFE);
            o.putUint56(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint56(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint56(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint56(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint56(3);

            Obj mX(&b);

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint56(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint56(3);

            Obj mX(&b);

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint56(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT56; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint56(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Uint64 val;
                mX.getUint56(val);
                ASSERT((i == SIZEOF_INT56) == X.isValid());
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // GET 48-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt48(bsls::Types::Int64& variable);
        //   getUint48(bsls::Types::Uint64& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 48-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt48." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt48(1);             o.putInt8(0xFF);
            o.putInt48(2);             o.putInt8(0xFE);
            o.putInt48(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt48(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt48(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt48(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt48(3);

            Obj mX(&b);

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt48(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt48(3);

            Obj mX(&b);

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt48(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT48; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt48(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Int64 val;
                mX.getInt48(val);
                ASSERT((i == SIZEOF_INT48) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint48." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint48(1);             o.putInt8(0xFF);
            o.putUint48(2);             o.putInt8(0xFE);
            o.putUint48(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint48(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint48(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint48(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint48(3);

            Obj mX(&b);

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint48(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint48(3);

            Obj mX(&b);

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint48(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT48; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint48(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Uint64 val;
                mX.getUint48(val);
                ASSERT((i == SIZEOF_INT48) == X.isValid());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GET 40-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt40(bsls::Types::Int64& variable);
        //   getUint40(bsls::Types::Uint64& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 40-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt40." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt40(1);             o.putInt8(0xFF);
            o.putInt40(2);             o.putInt8(0xFE);
            o.putInt40(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt40(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt40(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt40(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt40(3);

            Obj mX(&b);

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt40(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt40(3);

            Obj mX(&b);

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt40(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT40; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt40(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Int64 val;
                mX.getInt40(val);
                ASSERT((i == SIZEOF_INT40) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint40." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint40(1);             o.putInt8(0xFF);
            o.putUint40(2);             o.putInt8(0xFE);
            o.putUint40(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;
            if (veryVerbose) { P(b) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint40(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint40(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint40(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint40(3);

            Obj mX(&b);

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint40(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint40(3);

            Obj mX(&b);

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint40(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT40; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint40(3);

                Obj mX(&b);  const Obj& X = mX;

                bsls::Types::Uint64 val;
                mX.getUint40(val);
                ASSERT((i == SIZEOF_INT40) == X.isValid());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // GET 32-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt32(int& variable);
        //   getUint32(unsigned int& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 32-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt32." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt32(1);             o.putInt8(0xFF);
            o.putInt32(2);             o.putInt8(0xFE);
            o.putInt32(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            int  val;

            mX.getInt32(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt32(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt32(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt32(3);

            Obj mX(&b);

            int val = 0;
            mX.invalidate();
            mX.getInt32(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt32(3);

            Obj mX(&b);

            int val;
            ASSERT(&mX == &mX.getInt32(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT32; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt32(3);

                Obj mX(&b);  const Obj& X = mX;

                int val;
                mX.getInt32(val);
                ASSERT((i == SIZEOF_INT32) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint32." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint32(1);             o.putInt8(0xFF);
            o.putUint32(2);             o.putInt8(0xFE);
            o.putUint32(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char         marker;
            unsigned int val;
            mX.getUint32(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint32(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint32(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint32(3);

            Obj mX(&b);

            unsigned int val = 0;
            mX.invalidate();
            mX.getUint32(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint32(3);

            Obj mX(&b);

            unsigned int val;
            ASSERT(&mX == &mX.getUint32(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT32; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint32(3);

                Obj mX(&b);  const Obj& X = mX;

                unsigned int val;
                mX.getUint32(val);
                ASSERT((i == SIZEOF_INT32) == X.isValid());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // GET 24-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt24(int& variable);
        //   getUint24(unsigned int& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 24-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt24." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt24(1);             o.putInt8(0xFF);
            o.putInt24(2);             o.putInt8(0xFE);
            o.putInt24(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char marker;
            int  val;
            mX.getInt24(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt24(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt24(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt24(3);

            Obj mX(&b);

            int val = 0;
            mX.invalidate();
            mX.getInt24(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt24(3);

            Obj mX(&b);

            int val;
            ASSERT(&mX == &mX.getInt24(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT24; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt24(3);

                Obj mX(&b);  const Obj& X = mX;

                int val;
                mX.getInt24(val);
                ASSERT((i == SIZEOF_INT24) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint24." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint24(1);             o.putInt8(0xFF);
            o.putUint24(2);             o.putInt8(0xFE);
            o.putUint24(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char         marker;
            unsigned int val;
            mX.getUint24(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint24(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint24(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint24(3);

            Obj mX(&b);

            unsigned int val = 0;
            mX.invalidate();
            mX.getUint24(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint24(3);

            Obj mX(&b);

            unsigned int val;
            ASSERT(&mX == &mX.getUint24(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT24; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint24(3);

                Obj mX(&b);  const Obj& X = mX;

                unsigned int val;
                mX.getUint24(val);
                ASSERT((i == SIZEOF_INT24) == X.isValid());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // GET 16-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt16(short& variable);
        //   getUint16(unsigned short& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 16-BIT INTEGERS TEST" << endl
                 << "========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt16." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt16(1);             o.putInt8(0xFF);
            o.putInt16(2);             o.putInt8(0xFE);
            o.putInt16(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char  marker;
            short val;
            mX.getInt16(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt16(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt16(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt16(3);

            Obj mX(&b);

            short val = 0;
            mX.invalidate();
            mX.getInt16(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt16(3);

            Obj mX(&b);

            short val;
            ASSERT(&mX == &mX.getInt16(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT16; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt16(3);

                Obj mX(&b);  const Obj& X = mX;

                short val;
                mX.getInt16(val);
                ASSERT((i == SIZEOF_INT16) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint16." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint16(1);             o.putInt8(0xFF);
            o.putUint16(2);             o.putInt8(0xFE);
            o.putUint16(3);             o.putInt8(0xFD);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char           marker;
            unsigned short val;
            mX.getUint16(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint16(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint16(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint16(3);

            Obj mX(&b);

            unsigned short val = 0;
            mX.invalidate();
            mX.getUint16(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint16(3);

            Obj mX(&b);

            unsigned short val;
            ASSERT(&mX == &mX.getUint16(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT16; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint16(3);

                Obj mX(&b);  const Obj& X = mX;

                unsigned short val;
                mX.getUint16(val);
                ASSERT((i == SIZEOF_INT16) == X.isValid());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // GET 8-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.  Note that
        //   getInt8(char&) is tested in the PRIMARY MANIPULATORS test.
        //
        // Concerns:
        //: 1 The methods unexternalize the expected values.
        //:
        //: 2 The unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
        //
        // Testing:
        //   getInt8(signed char& variable);
        //   getUint8(char& variable);
        //   getUint8(unsigned char& variable);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "GET 8-BIT INTEGERS TEST" << endl
                 << "=======================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt8(signed char&)." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            signed char val;
            mX.getInt8(val);            ASSERT(1 == val);
            mX.getInt8(val);            ASSERT(2 == val);
            mX.getInt8(val);            ASSERT(3 == val);
            mX.getInt8(val);            ASSERT(4 == val);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(&b);

            signed char val = 0;
            mX.invalidate();
            mX.getInt8(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(&b);

            signed char val;
            ASSERT(&mX == &mX.getInt8(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT8; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt8(3);

                Obj mX(&b);  const Obj& X = mX;

                signed char val;
                mX.getInt8(val);
                ASSERT((i == SIZEOF_INT8) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint8(char&)." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            char val;
            mX.getUint8(val);            ASSERT(1 == val);
            mX.getUint8(val);            ASSERT(2 == val);
            mX.getUint8(val);            ASSERT(3 == val);
            mX.getUint8(val);            ASSERT(4 == val);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(&b);

            char val = 0;
            mX.invalidate();
            mX.getUint8(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(&b);

            char val;
            ASSERT(&mX == &mX.getUint8(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT8; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint8(3);

                Obj mX(&b);  const Obj& X = mX;

                char val;
                mX.getUint8(val);
                ASSERT((i == SIZEOF_INT8) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint8(unsigned char&)." << endl;
        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            Obj mX(&b);  const Obj& X = mX;

            if (veryVerbose) { P(b) }

            unsigned char val;
            mX.getUint8(val);            ASSERT(1 == val);
            mX.getUint8(val);            ASSERT(2 == val);
            mX.getUint8(val);            ASSERT(3 == val);
            mX.getUint8(val);            ASSERT(4 == val);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(&b);

            unsigned char val = 0;
            mX.invalidate();
            mX.getUint8(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(&b);

            unsigned char val;
            ASSERT(&mX == &mX.getUint8(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT8; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putUint8(3);

                Obj mX(&b);  const Obj& X = mX;

                unsigned char val;
                mX.getUint8(val);
                ASSERT((i == SIZEOF_INT8) == X.isValid());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST
        //   Verify functionality of the basic accessors.
        //
        // Concerns:
        //: 1 The methods return correct values.
        //
        // Plan:
        //: 1 Create an empty object, use 'invalidate' to modify state, and
        //:   verify the expected values for the methods.  (C-1)
        //
        // Testing:
        //   operator const void *() const;
        //   bool isValid() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "BASIC ACCESSORS TEST" << endl
                 << "====================" << endl;
        }
        if (verbose) {
            cout << "\nTesting operator const void *() and isValid()."
                 << endl;
        }
        int i, j;
        for (i = 0; i < 5; i++) {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            for (j = 0; j < i;  j++) o.putInt8(j);

            Obj mX(&b);   const Obj& X  = mX;
            Obj mX2(&b);  const Obj& X2 = mX2;

            if (veryVerbose) { P(b) }

            LOOP_ASSERT(i, X && X2);
            LOOP_ASSERT(i, X.isValid() && X2.isValid());

            mX.invalidate();
            LOOP_ASSERT(i, !X && X2);
            LOOP_ASSERT(i, !X.isValid() && X2.isValid());

            // invalidate stream x2 by making excessive 'get' calls
            char c;
            for (j = 0; j < i + 1; j++) {
                if (veryVerbose) { P(j); }
                mX2.getInt8(c);
            }
            LOOP_ASSERT(i, !X && !X2);
            LOOP_ASSERT(i, !X.isValid() && !X2.isValid());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   Verify functionality of primary manipulators.
        //
        // Concerns:
        //: 1 Constructor works appropriately.
        //:
        //: 2 'getInt8' produces the expected results.
        //:
        //: 3 'invalidate' produces the expected results.
        //:
        //: 4 The destructor functions properly.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create objects containing various data.
        //:
        //: 2 Modify state using 'getInt8' and 'invalidate'.
        //:
        //: 3 Verify state using basic accessors.  (C-1, C-2, C-3)
        //:
        //: 4 Since the destructor for this object is empty, the concern
        //:   regarding the destructor is trivially satisfied.  (C-4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   GenericInStream(STREAMBUF *streamBuf);
        //   ~GenericInStream();
        //   getInt8(char& variable);
        //   void invalidate();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PRIMARY MANIPULATORS TEST" << endl
                 << "=========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt8(char&) and ctor." << endl;

        }
        {
            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj mX(&b);  const Obj& X = mX;
            ASSERT(X);
            if (veryVerbose) { P(b) }
            char val;
            mX.getInt8(val);              ASSERT(1 == val);
            mX.getInt8(val);              ASSERT(2 == val);
            mX.getInt8(val);              ASSERT(3 == val);
            mX.getInt8(val);              ASSERT(4 == val);
            ASSERT(X);
        }
        {
            // Verify method has no effect if the stream is invalid.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(&b);

            char val = 0;
            mX.invalidate();
            mX.getInt8(val);
            ASSERT(0 == val);
        }
        {
            // Verify the return value.

            Buf b;

            Out o(&b, VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(&b);

            char val;
            ASSERT(&mX == &mX.getInt8(val));
        }
        {
            // Verify error handling.
            for (int i = 0; i <= SIZEOF_INT8; ++i) {
                Buf b;
                b.setLimit(i);

                Out o(&b, VERSION_SELECTOR);
                o.putInt8(3);

                Obj mX(&b);  const Obj& X = mX;

                char val;
                mX.getInt8(val);
                ASSERT((i == SIZEOF_INT8) == X.isValid());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting invalidate()." << endl;
        }
        for (int i = 0; i < 5; i++) {
            Buf b;

            // test default objects
            Obj mX(&b);  const Obj& X = mX;
            LOOP_ASSERT(i, X);
            mX.invalidate();
            LOOP_ASSERT(i, !X);

            // test objects of variable lengths
            Out o(&b, VERSION_SELECTOR);
            for (int j = 0; j < i;  j++) o.putInt8(j);

            Obj mX2(&b);  const Obj& X2 = mX2;
            if (veryVerbose) { P(b) }
            LOOP_ASSERT(i, X2);
            mX2.invalidate();
            LOOP_ASSERT(i, !X2);
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Buf mB;
            ASSERT_SAFE_PASS(Obj mX(&mB));
            ASSERT_SAFE_FAIL(Obj mX(0));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create 'GenericInStream' objects using default and buffer
        //:   constructors.
        //:
        //: 2 Exercise these objects using various methods.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "BREATHING TEST" << endl
                 << "==============" << endl;
        }
        if (verbose) {
            cout << "\nCreate object x." << endl;
        }

        Buf b;

        Obj x(&b);  const Obj& X = x;

        if (verbose) cout << "\nTry getInt32 with x." << endl;
        Out y(&b, VERSION_SELECTOR);
        int rv;
        y.putInt32(1);
        if (veryVerbose) { P(b); }
        x.getInt32(rv);
        ASSERT(1 == rv);

        if (verbose) {
            cout << "\nTry invalidate() with x." << endl;
        }
        x.invalidate();
        ASSERT(!X);
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

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
