// bslx_byteinstream.t.cpp                                            -*-C++-*-
#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>                 // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
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
// For all input methods in 'ByteInStream', the "unexternalization" from byte
// representation to the correct fundamental-type value is delegated to another
// component.  We assume that this process has been rigorously tested and
// verified.  Therefore, we are concerned only with the placement of the
// next-byte-position cursor and the alignment of bytes in the input stream.
// For each input method, we verify these properties by first creating a
// 'ByteOutStream' object 'oX' containing some arbitrarily chosen values.  The
// values are interleaved with chosen "marker" bytes to check for alignment
// issues.  We then create a 'ByteInStream' object 'iX' initialized with the
// content of 'oX', and consecutively call the input method on 'iX' to verify
// that the extracted values and marker bytes equal to their respective chosen
// values.  After all values are extracted, we verify that the stream is valid,
// empty, and the cursor is properly placed.
// ----------------------------------------------------------------------------
// [ 2] ByteInStream();
// [ 2] ByteInStream(const char *buffer, bsl::size_t numBytes);
// [ 2] ByteInStream(const bslstl::StringRef& srcData);
// [ 2] ~ByteInStream();
// [25] getLength(int& variable);
// [25] getVersion(int& variable);
// [12] getInt64(bsls::Types::Int64& variable);
// [12] getUint64(bsls::Types::Uint64& variable);
// [11] getInt56(bsls::Types::Int64& variable);
// [11] getUint56(bsls::Types::Uint64& variable);
// [10] getInt48(bsls::Types::Int64& variable);
// [10] getUint48(bsls::Types::Uint64& variable);
// [ 9] getInt40(bsls::Types::Int64& variable);
// [ 9] getUint40(bsls::Types::Uint64& variable);
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
// [26] getString(bsl::string& variable);
// [22] getArrayInt64(bsls::Types::Int64 *variables, int numVariables);
// [22] getArrayUint64(bsls::Types::Uint64 *variables, int numVariables);
// [21] getArrayInt56(bsls::Types::Int64 *variables, int numVariables);
// [21] getArrayUint56(bsls::Types::Uint64 *variables, int numVariables);
// [20] getArrayInt48(bsls::Types::Int64 *variables, int numVariables);
// [20] getArrayUint48(bsls::Types::Uint64 *variables, int numVariables);
// [19] getArrayInt40(bsls::Types::Int64 *variables, int numVariables);
// [19] getArrayUint40(bsls::Types::Uint64 *variables, int numVariables);
// [18] getArrayInt32(int *variables, int numVariables);
// [18] getArrayUint32(unsigned int *variables, int numVariables);
// [17] getArrayInt24(int *variables, int numVariables);
// [17] getArrayUint24(unsigned int *variables, int numVariables);
// [16] getArrayInt16(short *variables, int numVariables);
// [16] getArrayUint16(unsigned short *variables, int numVariables);
// [15] getArrayInt8(char *variables, int numVariables);
// [15] getArrayInt8(signed char *variables, int numVariables);
// [15] getArrayUint8(char *variables, int numVariables);
// [15] getArrayUint8(unsigned char *variables, int numVariables);
// [24] getArrayFloat64(double *variables, int numVariables);
// [23] getArrayFloat32(float *variables, int numVariables);
// [ 2] void invalidate();
// [27] void reset();
// [27] void reset(const char *buffer, bsl::size_t numBytes);
// [27] void reset(const bslstl::StringRef& srcData);
// [ 3] operator const void *() const;
// [ 3] bsl::size_t cursor() const;
// [ 3] const char *data() const;
// [ 3] bool isEmpty() const;
// [ 3] bool isValid() const;
// [ 3] bsl::size_t length() const;
//
// [ 4] ostream& operator<<(ostream& stream, const ByteInStream& obj);
// [28] ByteInStream& operator>>(ByteInStream&, TYPE& value);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [29] THIRD-PARTY EXTERNALIZATION
// [30] USAGE EXAMPLE
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

namespace BloombergLP {
namespace bslx {

void debugprint(const ByteInStream& object)
{
    bsl::cout << object;
}

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef ByteInStream Obj;
typedef ByteOutStream Out;

const int VERSION_SELECTOR = 20131127;
const int SIZEOF_INT64   = 8;
const int SIZEOF_INT32   = 4;
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
      case 30: {
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
// 'janeSmith' and a 'bslx::ByteOutStream' 'outStream':
//..
    MyPerson            janeSmith("Jane", "Smith", 42);
    bslx::ByteOutStream outStream(20131127);
    const int           VERSION = 1;
    outStream.putVersion(VERSION);
    janeSmith.bdexStreamOut(outStream, VERSION);
    ASSERT(outStream.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith':
//..
    MyPerson janeCopy;
    ASSERT(janeCopy != janeSmith);
//..
// Then, create a 'bslx::ByteInStream' 'inStream' initialized with the
// buffer from the 'bslx::ByteOutStream' object 'outStream' and unexternalize
// this data into 'janeCopy':
//..
    bslx::ByteInStream inStream(outStream.data(), outStream.length());
    int                version;
    inStream.getVersion(version);
    janeCopy.bdexStreamIn(inStream, version);
    ASSERT(inStream.isValid());
//..
// Finally, 'assert' the obtained values are as expected and display the
// results to 'bsl::stdout':
//..
    ASSERT(version  == VERSION);
    ASSERT(janeCopy == janeSmith);

if (veryVerbose) {
    if (janeCopy == janeSmith) {
        bsl::cout << "Successfully serialized and de-serialized Jane Smith:"
                  << "\n\tFirstName: " << janeCopy.firstName()
                  << "\n\tLastName : " << janeCopy.lastName()
                  << "\n\tAge      : " << janeCopy.age() << bsl::endl;
    }
    else {
        bsl::cout << "Serialization unsuccessful.  'janeCopy' holds:"
                  << "\n\tFirstName: " << janeCopy.firstName()
                  << "\n\tLastName : " << janeCopy.lastName()
                  << "\n\tAge      : " << janeCopy.age() << bsl::endl;
    }
} // if (veryVerbose)
//..

      } break;
      case 29: {
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

            Out o(VERSION_SELECTOR);
            o << initial;
            ASSERT(3 == o.length());
            ASSERT(0 == bsl::memcmp(o.data(), "\x02\x00\x07", o.length()));

            Obj mX(o.data(), o.length());
            mX >> value;
            ASSERT(value == initial);
        }
        {
            ThirdParty::MyStruct::EnumValue initial =
                                                     ThirdParty::MyStruct::e_A;
            ThirdParty::MyStruct::EnumValue value   =
                                                     ThirdParty::MyStruct::e_B;

            Out o(0);
            o << initial;
            ASSERT(2 == o.length());
            ASSERT(0 == bsl::memcmp(o.data(), "\x01\x07", o.length()));

            Obj mX(o.data(), o.length());
            mX >> value;
            ASSERT(value == initial);
        }

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // UNEXTERNALIZATION FREE OPERATOR
        //   Verify 'operator>>' works correctly.
        //
        // Concerns:
        //: 1 Method inline-forwards to implementation correctly.
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
        //   ByteInStream& operator>>(ByteInStream&, TYPE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNEXTERNALIZATION FREE OPERATOR" << endl
                          << "===============================" << endl;

        {
            char initial = 'a';
            char value = 'b';

            Out o(VERSION_SELECTOR);
            o << initial;

            Obj mX(o.data(), o.length());
            mX >> value;
            ASSERT(value == initial);
        }
        {
            double initial = 7.0;
            double value = 1.0;

            Out o(VERSION_SELECTOR);
            o << initial;

            Obj mX(o.data(), o.length());
            mX >> value;
            ASSERT(value == initial);
        }
        {
            bsl::vector<int> initial;
            bsl::vector<int> value;
            value.push_back(3);
            for (int i = 0; i < 5; ++i) {
                if (veryVerbose) { P(i); }

                Out o(VERSION_SELECTOR);
                o << initial;

                Obj mX(o.data(), o.length());
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

            Out o(VERSION_SELECTOR);
            o << initial1 << initial2 << initial3;

            Obj mX(o.data(), o.length());
            mX >> value1 >> value2 >> value3;
            ASSERT(value1 == initial1);
            ASSERT(value2 == initial2);
            ASSERT(value3 == initial3);
        }

        {
            // Verify method has no effect if the stream is invalid.

            char initial = 'a';
            char value = 'b';

            Out o(VERSION_SELECTOR);
            o << initial;

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            mX.invalidate();
            mX >> value;
            ASSERT('b' == value);
            ASSERT(0 == X.cursor());
        }

        {
            // Verify the return value.

            char initial = 'a';
            char value = 'b';

            Out o(VERSION_SELECTOR);
            o << initial;

            Obj mX(o.data(), o.length());
            ASSERT(&mX == &(mX >> value));
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'reset'
        //   Verify the 'reset' methods work correctly.
        //
        // Concerns:
        //: 1 Every method sets the buffer and length correctly.
        //:
        //: 2 Every method sets the cursor to zero.
        //:
        //: 3 Every method marks the stream valid.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Initialize an input stream, read some data to ensure the cursor
        //:   is not zero, and invalidate the stream.
        //:
        //: 2 Call the 'reset' method and verify the buffer and length are
        //:   correctly assigned, the cursor is zero, and the stream is valid.
        //:   (C-1..3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void reset();
        //   void reset(const char *buffer, bsl::size_t numBytes);
        //   void reset(const bslstl::StringRef& srcData);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset'" << endl
                          << "===============" << endl;

        {
            Out o(VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));
            o.putString(bsl::string("beta"));
            o.putString(bsl::string("gamma"));

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            bsl::string val;
            mX.getString(val);
            mX.invalidate();
            ASSERT(0 != X.cursor());
            ASSERT(false == X.isValid());

            mX.reset();
            ASSERT(o.data() == X.data());
            ASSERT(0 == X.cursor());
            ASSERT(o.length() == X.length());
            ASSERT(X.isValid());
        }

        {
            Out o(VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));
            o.putString(bsl::string("beta"));
            o.putString(bsl::string("gamma"));

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            bsl::string val;
            mX.getString(val);
            mX.invalidate();
            ASSERT(0 != X.cursor());
            ASSERT(false == X.isValid());

            const char        *buffer = "abc";
            const bsl::size_t  LEN = 3;
            mX.reset(buffer, LEN);
            ASSERT(buffer == X.data());
            ASSERT(0 == X.cursor());
            ASSERT(LEN == X.length());
            ASSERT(X.isValid());
        }

        {
            Out o(VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));
            o.putString(bsl::string("beta"));
            o.putString(bsl::string("gamma"));

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }
            bsl::string val;
            mX.getString(val);
            mX.invalidate();
            ASSERT(0 != X.cursor());
            ASSERT(false == X.isValid());

            const char        *buffer = "abc";
            const bsl::size_t  LEN = 3;

            bslstl::StringRef stringRef(buffer, LEN);
            mX.reset(stringRef);
            ASSERT(buffer == X.data());
            ASSERT(0 == X.cursor());
            ASSERT(LEN == X.length());
            ASSERT(X.isValid());
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;
            ASSERT_SAFE_PASS(mX.reset(0, 0));
            ASSERT_SAFE_FAIL(mX.reset(0, 1));
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'getString'
        //   Verify this method unexternalizes the expected values.
        //
        // Concerns:
        //: 1 Method unexternalizes the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
        //
        // Plan:
        //: 1 Unexternalize at different offsets and verify the values.  (C-1,
        //:   C-2)
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
            Out o(VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));    o.putInt8(0xFF);
            o.putString(bsl::string("beta"));     o.putInt8(0xFE);
            o.putString(bsl::string("gamma"));    o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char        marker;
            bsl::string val;
            mX.getString(val);         mX.getInt8(marker);
            ASSERT(val == "alpha");    ASSERT('\xFF' == marker);
            mX.getString(val);         mX.getInt8(marker);
            ASSERT(val == "beta");     ASSERT('\xFE' == marker);
            mX.getString(val);         mX.getInt8(marker);
            ASSERT(val == "gamma");    ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }

        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsl::string val;
            mX.invalidate();
            mX.getString(val);
            ASSERT(val.empty());
            ASSERT(0 == X.cursor());
        }

        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putString(bsl::string("alpha"));

            Obj mX(o.data(), o.length());

            bsl::string val;
            ASSERT(&mX == &mX.getString(val));
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // GET LENGTH AND VERSION
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putLength(1);             o.putInt8(0xFF);
            o.putLength(128);           o.putInt8(0xFE);
            o.putLength(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char marker;
            int  val;
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(128 == val);        ASSERT('\xFE' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            Out o(VERSION_SELECTOR);
            o.putLength(128);             o.putInt8(0xFD);
            o.putLength(127);             o.putInt8(0xFE);
            o.putLength(256);             o.putInt8(0xFF);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char marker;
            int  val;
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(128 == val);        ASSERT('\xFD' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(127 == val);        ASSERT('\xFE' == marker);
            mX.getLength(val);         mX.getInt8(marker);
            ASSERT(256 == val);        ASSERT('\xFF' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putLength(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            int val = 0;
            mX.invalidate();
            mX.getLength(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putLength(3);

            Obj mX(o.data(), o.length());
            int val;
            ASSERT(&mX == &mX.getLength(val));
        }

        if (verbose) {
            cout << "\nTesting getVersion(int&)." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putVersion(1);
            o.putVersion(2);
            o.putVersion(3);
            o.putVersion(4);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }
            int val;
            mX.getVersion(val);            ASSERT(1 == val);
            mX.getVersion(val);            ASSERT(2 == val);
            mX.getVersion(val);            ASSERT(3 == val);
            mX.getVersion(val);            ASSERT(4 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            Out o(VERSION_SELECTOR);
            o.putVersion(252);
            o.putVersion(253);
            o.putVersion(254);
            o.putVersion(255);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }
            int val;
            mX.getVersion(val);            ASSERT(252 == val);
            mX.getVersion(val);            ASSERT(253 == val);
            mX.getVersion(val);            ASSERT(254 == val);
            mX.getVersion(val);            ASSERT(255 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putVersion(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            int val = 0;
            mX.invalidate();
            mX.getVersion(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putVersion(3);

            Obj mX(o.data(), o.length());
            int val;
            ASSERT(&mX == &mX.getVersion(val));
        }
      } break;
      case 24: {
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

            Out o(VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat64(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const double DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            double ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayFloat64(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const double DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 3);

            Obj mX(o.data(), o.length());

            double ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayFloat64(ar, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            double DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayFloat64(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayFloat64(static_cast<double *>(0),
                                                   0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayFloat64(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayFloat64(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayFloat64(DATA, 1));
            }
        }
      } break;
      case 23: {
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

            Out o(VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayFloat32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayFloat32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayFloat32(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const float DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            float ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayFloat32(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const float DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 3);

            Obj mX(o.data(), o.length());

            float ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayFloat32(ar, 3));
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            float DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayFloat32(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayFloat32(static_cast<float *>(0),
                                                   0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayFloat32(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayFloat32(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayFloat32(DATA, 1));
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // GET 64-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt64(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt64(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt64(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt64(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt64(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt64(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt64(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt64(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint64." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint64(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint64(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint64(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint64(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint64(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint64(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint64(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint64(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt64(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt64(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt64(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt64(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt64(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint64(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint64(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint64(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint64(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint64(DATA, 1));
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // GET 56-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt56(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt56(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt56(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt56(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt56(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt56(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt56(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt56(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint56." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint56(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint56(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint56(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint56(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint56(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint56(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint56(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint56(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt56(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt56(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt56(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt56(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt56(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint56(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint56(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint56(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint56(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint56(DATA, 1));
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // GET 48-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt48(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt48(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt48(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt48(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt48(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt48(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt48(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt48(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint48." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint48(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint48(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint48(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint48(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint48(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint48(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint48(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint48(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt48(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt48(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt48(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt48(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt48(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint48(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint48(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint48(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint48(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint48(DATA, 1));
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // GET 40-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt40(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt40(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt40(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt40(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt40(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt40(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Int64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt40(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt40(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint40." << endl;
        }
        {
            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };
            const bsls::Types::Uint64 V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint40(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint40(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint40(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint40(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint40(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint40(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const bsls::Types::Uint64 DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint40(DATA, 3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint40(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::Types::Int64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt40(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt40(
                                     static_cast<bsls::Types::Int64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt40(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt40(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt40(DATA, 1));
            }
        }
        {
            bsls::Types::Uint64 DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint40(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint40(
                                    static_cast<bsls::Types::Uint64 *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint40(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint40(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint40(DATA, 1));
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // GET 32-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt32(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt32(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt32(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt32(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt32(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt32(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt32(DATA, 3);

            Obj mX(o.data(), o.length());

            int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt32(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint32." << endl;
        }
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint32(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint32(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint32(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint32(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint32(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint32(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const unsigned int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint32(DATA, 3);

            Obj mX(o.data(), o.length());

            unsigned int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint32(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            int DATA[5];
            Out o(VERSION_SELECTOR);
            o.putArrayInt32(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt32(static_cast<int *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt32(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt32(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt32(DATA, 1));
            }
        }
        {
            unsigned int DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint32(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint32(
                                           static_cast<unsigned int *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint32(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint32(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint32(DATA, 1));
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // GET 24-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt24(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt24(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt24(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt24(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt24(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt24(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt24(DATA, 3);

            Obj mX(o.data(), o.length());

            int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt24(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint24." << endl;
        }
        {
            const unsigned int DATA[] = { 1, 2, 3 };
            const unsigned int V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint24(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint24(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint24(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint24(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint24(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned int ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint24(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const unsigned int DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint24(DATA, 3);

            Obj mX(o.data(), o.length());

            unsigned int ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint24(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            int DATA[5];
            Out o(VERSION_SELECTOR);
            o.putArrayInt24(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt24(static_cast<int *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt24(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt24(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt24(DATA, 1));
            }
        }
        {
            unsigned int DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint24(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint24(
                                           static_cast<unsigned int *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint24(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint24(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint24(DATA, 1));
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // GET 16-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt16(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt16(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt16(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt16(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const short DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt16(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            short ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt16(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const short DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt16(DATA, 3);

            Obj mX(o.data(), o.length());

            short ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt16(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint16." << endl;
        }
        {
            const unsigned short DATA[] = { 1, 2, 3 };
            const unsigned short V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint16(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint16(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint16(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint16(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned short DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint16(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned short ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint16(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const unsigned short DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint16(DATA, 3);

            Obj mX(o.data(), o.length());

            unsigned short ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint16(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            short DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt16(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt16(static_cast<short *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt16(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt16(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt16(DATA, 1));
            }
        }
        {
            unsigned short DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint16(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint16(
                                         static_cast<unsigned short *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint16(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint16(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint16(DATA, 1));
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // GET 8-BIT INTEGER ARRAYS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(o.data(), o.length());

            char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt8(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing getArrayInt8(signed char*, int)." << endl;
        }
        {
            const signed char DATA[] = { 1, 2, 3 };
            const signed char V = static_cast<signed char>(0xFF);

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 0);             o.putInt8(0xFF);
            o.putArrayInt8(DATA, 1);             o.putInt8(0xFE);
            o.putArrayInt8(DATA, 2);             o.putInt8(0xFD);
            o.putArrayInt8(DATA, 3);             o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const signed char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            signed char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayInt8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const signed char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 3);

            Obj mX(o.data(), o.length());

            signed char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayInt8(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint8(char*, int)." << endl;
        }
        {
            const char DATA[] = { 1, 2, 3 };
            const char V = static_cast<char>(0xFF);

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(o.data(), o.length());

            char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint8(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getArrayUint8(unsigned char*, int)." << endl;
        }
        {
            const unsigned char DATA[] = { 1, 2, 3 };
            const unsigned char V = 0xFF;

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 0);            o.putInt8(0xFF);
            o.putArrayUint8(DATA, 1);            o.putInt8(0xFE);
            o.putArrayUint8(DATA, 2);            o.putInt8(0xFD);
            o.putArrayUint8(DATA, 3);            o.putInt8(0xFC);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }

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
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            const unsigned char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned char ar[] = { 0, 0, 0 };
            mX.invalidate();
            mX.getArrayUint8(ar, 3);
            ASSERT(0 == ar[0] && 0 == ar[1] && 0 == ar[2]);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            const unsigned char DATA[] = { 1, 2, 3 };

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 3);

            Obj mX(o.data(), o.length());

            unsigned char ar[] = { 0, 0, 0 };
            ASSERT(&mX == &mX.getArrayUint8(ar, 3));
        }

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            char DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt8(static_cast<char *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt8(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt8(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt8(DATA, 1));
            }
        }
        {
            signed char DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayInt8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt8(
                                            static_cast<signed char *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayInt8(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt8(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayInt8(DATA, 1));
            }
        }
        {
            char DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint8(static_cast<char *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint8(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 1));
            }
        }
        {
            unsigned char DATA[5];

            Out o(VERSION_SELECTOR);
            o.putArrayUint8(DATA, 5);

            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint8(
                                          static_cast<unsigned char *>(0), 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_FAIL(mX.getArrayUint8(DATA, -1));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 0));
            }
            {
                Obj mX(o.data(), o.length());
                ASSERT_SAFE_PASS(mX.getArrayUint8(DATA, 1));
            }
        }
      } break;
      case 14: {
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
            Out o(VERSION_SELECTOR);
            o.putFloat64(1);           o.putInt8(0xFF);
            o.putFloat64(2);           o.putInt8(0xFE);
            o.putFloat64(3);           o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char   marker;
            double val;
            mX.getFloat64(val);        mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getFloat64(val);        mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getFloat64(val);        mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putFloat64(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            double val = 0;
            mX.invalidate();
            mX.getFloat64(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putFloat64(3);

            Obj mX(o.data(), o.length());

            double val;
            ASSERT(&mX == &mX.getFloat64(val));
        }
      } break;
      case 13: {
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
            Out o(VERSION_SELECTOR);
            o.putFloat32(1);           o.putInt8(0xFF);
            o.putFloat32(2);           o.putInt8(0xFE);
            o.putFloat32(3);           o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char  marker;
            float val;
            mX.getFloat32(val);        mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getFloat32(val);        mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getFloat32(val);        mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putFloat32(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            float val = 0;
            mX.invalidate();
            mX.getFloat32(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putFloat32(3);

            Obj mX(o.data(), o.length());

            float val;
            ASSERT(&mX == &mX.getFloat32(val));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // GET 64-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt64(1);             o.putInt8(0xFF);
            o.putInt64(2);             o.putInt8(0xFE);
            o.putInt64(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt64(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt64(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt64(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt64(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt64(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt64(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt64(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint64." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint64(1);             o.putInt8(0xFF);
            o.putUint64(2);             o.putInt8(0xFE);
            o.putUint64(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint64(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint64(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint64(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint64(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint64(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint64(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint64(val));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GET 56-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt56(1);             o.putInt8(0xFF);
            o.putInt56(2);             o.putInt8(0xFE);
            o.putInt56(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt56(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt56(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt56(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt56(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt56(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt56(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt56(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint56." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint56(1);             o.putInt8(0xFF);
            o.putUint56(2);             o.putInt8(0xFE);
            o.putUint56(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint56(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint56(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint56(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint56(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint56(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint56(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint56(val));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // GET 48-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt48(1);             o.putInt8(0xFF);
            o.putInt48(2);             o.putInt8(0xFE);
            o.putInt48(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt48(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt48(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt48(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt48(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt48(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt48(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt48(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint48." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint48(1);             o.putInt8(0xFF);
            o.putUint48(2);             o.putInt8(0xFE);
            o.putUint48(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint48(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint48(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint48(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint48(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint48(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint48(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint48(val));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // GET 40-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt40(1);             o.putInt8(0xFF);
            o.putInt40(2);             o.putInt8(0xFE);
            o.putInt40(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char               marker;
            bsls::Types::Int64 val;
            mX.getInt40(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt40(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt40(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt40(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Int64 val = 0;
            mX.invalidate();
            mX.getInt40(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt40(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Int64 val;
            ASSERT(&mX == &mX.getInt40(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint40." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint40(1);             o.putInt8(0xFF);
            o.putUint40(2);             o.putInt8(0xFE);
            o.putUint40(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            if (veryVerbose) { P(X) }

            char                marker;
            bsls::Types::Uint64 val;
            mX.getUint40(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint40(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint40(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint40(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            bsls::Types::Uint64 val = 0;
            mX.invalidate();
            mX.getUint40(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint40(3);

            Obj mX(o.data(), o.length());

            bsls::Types::Uint64 val;
            ASSERT(&mX == &mX.getUint40(val));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GET 32-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt32(1);             o.putInt8(0xFF);
            o.putInt32(2);             o.putInt8(0xFE);
            o.putInt32(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char marker;
            int  val;

            mX.getInt32(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt32(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt32(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt32(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            int val = 0;
            mX.invalidate();
            mX.getInt32(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt32(3);

            Obj mX(o.data(), o.length());

            int val;
            ASSERT(&mX == &mX.getInt32(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint32." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint32(1);             o.putInt8(0xFF);
            o.putUint32(2);             o.putInt8(0xFE);
            o.putUint32(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char         marker;
            unsigned int val;
            mX.getUint32(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint32(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint32(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint32(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned int val = 0;
            mX.invalidate();
            mX.getUint32(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint32(3);

            Obj mX(o.data(), o.length());

            unsigned int val;
            ASSERT(&mX == &mX.getUint32(val));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // GET 24-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt24(1);             o.putInt8(0xFF);
            o.putInt24(2);             o.putInt8(0xFE);
            o.putInt24(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char marker;
            int  val;
            mX.getInt24(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt24(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt24(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt24(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            int val = 0;
            mX.invalidate();
            mX.getInt24(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt24(3);

            Obj mX(o.data(), o.length());

            int val;
            ASSERT(&mX == &mX.getInt24(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint24." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint24(1);             o.putInt8(0xFF);
            o.putUint24(2);             o.putInt8(0xFE);
            o.putUint24(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char         marker;
            unsigned int val;
            mX.getUint24(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint24(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint24(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint24(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned int val = 0;
            mX.invalidate();
            mX.getUint24(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint24(3);

            Obj mX(o.data(), o.length());

            unsigned int val;
            ASSERT(&mX == &mX.getUint24(val));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // GET 16-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt16(1);             o.putInt8(0xFF);
            o.putInt16(2);             o.putInt8(0xFE);
            o.putInt16(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char  marker;
            short val;
            mX.getInt16(val);          mX.getInt8(marker);
            ASSERT(1 == val);          ASSERT('\xFF' == marker);
            mX.getInt16(val);          mX.getInt8(marker);
            ASSERT(2 == val);          ASSERT('\xFE' == marker);
            mX.getInt16(val);          mX.getInt8(marker);
            ASSERT(3 == val);          ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt16(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            short val = 0;
            mX.invalidate();
            mX.getInt16(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt16(3);

            Obj mX(o.data(), o.length());

            short val;
            ASSERT(&mX == &mX.getInt16(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint16." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint16(1);             o.putInt8(0xFF);
            o.putUint16(2);             o.putInt8(0xFE);
            o.putUint16(3);             o.putInt8(0xFD);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char           marker;
            unsigned short val;
            mX.getUint16(val);          mX.getInt8(marker);
            ASSERT(1 == val);           ASSERT('\xFF' == marker);
            mX.getUint16(val);          mX.getInt8(marker);
            ASSERT(2 == val);           ASSERT('\xFE' == marker);
            mX.getUint16(val);          mX.getInt8(marker);
            ASSERT(3 == val);           ASSERT('\xFD' == marker);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint16(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned short val = 0;
            mX.invalidate();
            mX.getUint16(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint16(3);

            Obj mX(o.data(), o.length());

            unsigned short val;
            ASSERT(&mX == &mX.getUint16(val));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // GET 8-BIT INTEGERS TEST
        //   Verify these methods unexternalize the expected values.  Note that
        //   getInt8(char&) is tested in the PRIMARY MANIPULATORS test.
        //
        // Concerns:
        //: 1 Methods unexternalize the expected values.
        //:
        //: 2 Unexternalization position does not effect output.
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
            Out o(VERSION_SELECTOR);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            signed char val;
            mX.getInt8(val);            ASSERT(1 == val);
            mX.getInt8(val);            ASSERT(2 == val);
            mX.getInt8(val);            ASSERT(3 == val);
            mX.getInt8(val);            ASSERT(4 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            signed char val = 0;
            mX.invalidate();
            mX.getInt8(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(o.data(), o.length());

            signed char val;
            ASSERT(&mX == &mX.getInt8(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint8(char&)." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            char val;
            mX.getUint8(val);            ASSERT(1 == val);
            mX.getUint8(val);            ASSERT(2 == val);
            mX.getUint8(val);            ASSERT(3 == val);
            mX.getUint8(val);            ASSERT(4 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            char val = 0;
            mX.invalidate();
            mX.getUint8(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(o.data(), o.length());

            char val;
            ASSERT(&mX == &mX.getUint8(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting getUint8(unsigned char&)." << endl;
        }
        {
            Out o(VERSION_SELECTOR);
            o.putUint8(1);
            o.putUint8(2);
            o.putUint8(3);
            o.putUint8(4);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            if (veryVerbose) { P(X) }

            unsigned char val;
            mX.getUint8(val);            ASSERT(1 == val);
            mX.getUint8(val);            ASSERT(2 == val);
            mX.getUint8(val);            ASSERT(3 == val);
            mX.getUint8(val);            ASSERT(4 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            unsigned char val = 0;
            mX.invalidate();
            mX.getUint8(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putUint8(3);

            Obj mX(o.data(), o.length());

            unsigned char val;
            ASSERT(&mX == &mX.getUint8(val));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT OPERATOR TEST
        //   Verify the method produces the expected output format.
        //
        // Concerns:
        //: 1 Method produces expected output format.
        //
        // Plan:
        //: 1 For a small set of objects, use 'ostringstream' to write the
        //:   object's value to a string buffer and then compare to expected
        //:   output format.  (C-1)
        //
        // Testing:
        //   ostream& operator<<(ostream& stream, const ByteInStream& obj);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PRINT OPERATOR TEST" << endl
                 << "===================" << endl;
        }

        if (verbose) {
            cout << "\nTesting print operator." << endl;
        }

        const int   SIZE = 1000;   // Must be big enough to hold output string.
        const char  XX = static_cast<char>(0xFF);  // Value that represents an
                                                   // unset char.
        char        ctrl[SIZE];    memset(ctrl, XX, SIZE);
        const char *CTRL = ctrl;

        {
            Obj mX;  const Obj& X = mX;

            const char *EXPECTED = "";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Out o(VERSION_SELECTOR);
            o.putInt8(0);  o.putInt8(1);  o.putInt8(2);  o.putInt8(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            const char *EXPECTED =
                "\n0000\t00 01 02 03";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Out o(VERSION_SELECTOR);
            o.putInt8(0);  o.putInt8(1);  o.putInt8(2);  o.putInt8(3);
            o.putInt8(4);  o.putInt8(5);  o.putInt8(6);  o.putInt8(7);
            o.putInt8(8);  o.putInt8(9);  o.putInt8(10); o.putInt8(11);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            const char *EXPECTED =
                "\n0000\t00 01 02 03 04 05 06 07"
                "\n0008\t08 09 0a 0b";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
        }
        {
            Out o(VERSION_SELECTOR);
            o.putInt8(  0);  o.putInt8( 1);  o.putInt8( 2);  o.putInt8( 3);
            o.putInt8(  4);  o.putInt8( 5);  o.putInt8( 6);  o.putInt8( 7);
            o.putInt8(  8);  o.putInt8( 9);  o.putInt8(10);  o.putInt8(11);
            o.putInt8(127);  o.putInt8(-1);  o.putInt8(-2);  o.putInt8(-3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            const char *EXPECTED =
                "\n0000\t00 01 02 03 04 05 06 07"
                "\n0008\t08 09 0a 0b 7f ff fe fd";

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << X << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            const int LEN = static_cast<int>(strlen(EXPECTED)) + 1;
            if (veryVerbose) cout << "\tEXPECTED : " << EXPECTED << endl
                                  << "\tACTUAL : "   << RESULT   << endl;
            ASSERT(XX == RESULT[SIZE-1]); // check for overrun
            ASSERT(0 == memcmp(RESULT, EXPECTED, LEN));
            ASSERT(0 == memcmp(RESULT + LEN, CTRL + LEN, SIZE - LEN));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS TEST
        //   Verify functionality of the basic accessors.
        //
        // Concerns:
        //: 1 Methods return correct values.
        //
        // Plan:
        //: 1 Create an empty object, use 'getInt8' and 'invalidate' to modify
        //:   state, and verify the expected values for the methods.  (C-1)
        //
        // Testing:
        //   operator const void *() const;
        //   bsl::size_t cursor() const;
        //   const char *data() const;
        //   bool isEmpty() const;
        //   bool isValid() const;
        //   bsl::size_t length() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "BASIC ACCESSORS TEST" << endl
                 << "====================" << endl;
        }
        if (verbose) {
            cout << "\nTesting operator const void *(), isValid() and data()."
                 << endl;
        }
        int i, j;
        for (i = 0; i < 5; i++) {
            Obj mX;  const Obj& X = mX;
            LOOP_ASSERT(i, X);
            LOOP_ASSERT(i, 0 == X.data());

            Out o(VERSION_SELECTOR);
            for (j = 0; j < i;  j++) o.putInt8(j);

            Obj mX2(o.data(), o.length());  const Obj& X2 = mX2;
            if (veryVerbose) { P(X2) }
            LOOP_ASSERT(i, X2 && X2.isValid());
            LOOP_ASSERT(i, 0 == bsl::memcmp(X2.data(), o.data(), o.length()));

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

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting isEmpty(), length() and cursor()." << endl;

        for (i = 0; i < 5; i++) {
            // test default empty objects
            Obj mX;  const Obj& X = mX;
            LOOP_ASSERT(i, X.isEmpty());
            LOOP_ASSERT(i, X.length() == 0);
            LOOP_ASSERT(i, X.cursor() == 0);

            // test objects of variable lengths
            Out o(VERSION_SELECTOR);
            for (j = 0; j < i; j++) {
                o.putInt8(j);
            }

            Obj mX2(o.data(), o.length());  const Obj& X2 = mX2;
            if (veryVerbose) { P(X2) }
            LOOP_ASSERT(i, (0 == i && X2.isEmpty()) || !X2.isEmpty());
            LOOP_ASSERT(i, X2.length() == static_cast<bsl::size_t>(i));
            LOOP_ASSERT(i, X2.cursor() == 0);

            char c;
            for (j = 0; j < i; j++) {
                if (veryVerbose) { P_(i) P(j); }
                LOOP2_ASSERT(i,
                             j,
                             X2.cursor() ==
                                    static_cast<bsl::size_t>(SIZEOF_INT8 * j));
                mX2.getInt8(c);
            }
            LOOP_ASSERT(i, X2.isEmpty());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   Verify functionality of primary manipulators.
        //
        // Concerns:
        //: 1 Constructors work appropriately.
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
        //   ByteInStream();
        //   ByteInStream(const char *buffer, bsl::size_t numBytes);
        //   ByteInStream(const bslstl::StringRef& srcData);
        //   ~ByteInStream();
        //   getInt8(char& variable);
        //   void invalidate();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "PRIMARY MANIPULATORS TEST" << endl
                 << "=========================" << endl;
        }

        if (verbose) {
            cout << "\nTesting getInt8(char&) and ctors." << endl;

        }
        {
            Obj mX0;  const Obj& X0 = mX0;  // test default ctor
            if (veryVerbose) { P(X0) }
            ASSERT(X0);
            ASSERT(0 == X0.length());
        }
        {
            // Test constructor initialized with a 'char *'.

            Out o(VERSION_SELECTOR);
            o.putInt8(1);
            o.putInt8(2);
            o.putInt8(3);
            o.putInt8(4);

            Obj mX(o.data(), o.length());  const Obj& X = mX;
            ASSERT(X);
            ASSERT(X.length() == o.length());
            if (veryVerbose) { P(X) }
            char val;
            mX.getInt8(val);              ASSERT(1 == val);
            mX.getInt8(val);              ASSERT(2 == val);
            mX.getInt8(val);              ASSERT(3 == val);
            mX.getInt8(val);              ASSERT(4 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Test constructor initialized with a 'bslstl::StringRef'.

            Out o(VERSION_SELECTOR);
            o.putInt8(5);
            o.putInt8(6);
            o.putInt8(7);
            o.putInt8(8);

            bslstl::StringRef srcData(o.data(), static_cast<int>(o.length()));

            Obj mX(srcData);  const Obj& X = mX;
            ASSERT(X);
            ASSERT(X.length() == o.length());
            if (veryVerbose) { P(X) }
            char val;
            mX.getInt8(val);              ASSERT(5 == val);
            mX.getInt8(val);              ASSERT(6 == val);
            mX.getInt8(val);              ASSERT(7 == val);
            mX.getInt8(val);              ASSERT(8 == val);
            ASSERT(X);
            ASSERT(X.isEmpty());
            ASSERT(X.cursor() == X.length());
        }
        {
            // Verify method has no effect if the stream is invalid.

            Out o(VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(o.data(), o.length());  const Obj& X = mX;

            char val = 0;
            mX.invalidate();
            mX.getInt8(val);
            ASSERT(0 == val);
            ASSERT(0 == X.cursor());
        }
        {
            // Verify the return value.

            Out o(VERSION_SELECTOR);
            o.putInt8(3);

            Obj mX(o.data(), o.length());

            char val;
            ASSERT(&mX == &mX.getInt8(val));
        }

        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting invalidate()." << endl;
        }
        for (int i = 0; i < 5; i++) {
            // test default objects
            Obj mX;  const Obj& X = mX;
            LOOP_ASSERT(i, X);
            mX.invalidate();
            LOOP_ASSERT(i, !X);

            // test objects of variable lengths
            Out o(VERSION_SELECTOR);
            for (int j = 0; j < i;  j++) o.putInt8(j);

            Obj mX2(o.data(), o.length());  const Obj& X2 = mX2;
            if (veryVerbose) { P(X2) }
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

            ASSERT_SAFE_PASS(Obj mX(0, 0));
            ASSERT_SAFE_FAIL(Obj mX(0, 1));
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
        //: 1 Create 'ByteInStream' objects using default and buffer
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
            cout << "\nCreate object x1 using default ctor." << endl;
        }

        int i;
        Obj mX1;  const Obj& X1 = mX1;
        ASSERT(0 == X1.length());

        if (verbose) {
            cout << "\nCreate object x2 w/ an initial value." << endl;
        }
        Obj mX2("\x00\x01\x02\x03\x04", 5);  const Obj& X2 = mX2;
        if (veryVerbose) { P(X2); }
        ASSERT(5 == X2.length());

        const char *data = X2.data();  (void)data;

        if (verbose) {
            cout << "\nTry getInt8() with x2." << endl;
        }
        for (i = 0; i < 5; i++) {
            if (veryVerbose) { P(i); }
            char c;
            mX2.getInt8(c);
            LOOP_ASSERT(i, i == c);
        }

        if (verbose) {
            cout << "\nTry isEmpty() with x2." << endl;
        }
        ASSERT(1 == X2.isEmpty());

        if (verbose) {
            cout << "\nTry invalidate() with x2." << endl;
        }
        mX2.invalidate();
        ASSERT(!X2);

        if (verbose) {
            cout << "\nTry invalid operation with x1." << endl;
        }
        mX1.getInt32(i);
        ASSERT(!X1);
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
