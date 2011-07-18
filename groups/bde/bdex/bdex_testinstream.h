// bdex_testinstream.h                                                -*-C++-*-
#ifndef INCLUDED_BDEX_TESTINSTREAM
#define INCLUDED_BDEX_TESTINSTREAM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable unexternalization of fundamental types with identification.
//
//@CLASSES:
//   bdex_TestInStream: byte-array-based input stream class
//   BEGIN_BDEX_EXCEPTION_TEST: macro for testing streaming exceptions
//   END_BDEX_EXCEPTION_TEST: macro for testing streaming exceptions
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO: bdex_testoutstream
//
//@DESCRIPTION: This component conforms to the 'bdex_instream' (documentation
// only) "protocol", implementing a byte-array-based data stream object that
// provides platform-independent input methods ("unexternalization") on values,
// and arrays of values, of fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bdex_testoutstream' externalization component.  Each input method of
// 'bdex_TestInStream' reads either a value or a homogeneous array of values of
// a fundamental type, in a format that was written by the corresponding
// 'bdex_TestOutStream' method.  In general, the user of this component cannot
// rely on being able to read data that was written by any mechanism other than
// 'bdex_TestOutStream'.
//
// The supported types and required content are listed in the table below.  All
// of the fundamental types in the table may be input as scalar values or as
// homogeneous arrays.  'bsl::string' is input as a logical scalar string.
// Note that 'Int64' and 'Uint64' denote 'bsls_PlatformUtil::Int64' and
// 'bsls_PlatformUtil::Uint64', which in turn are 'typedef' names for the
// signed and unsigned 64-bit integer types, respectively, on the host
// platform.
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//
//      bsl::string       BDE STL implementation of the STL string class
//..
// This component also supports compact streaming in of integer types.  In
// particular, 64-bit values can be streamed in from 40, 48, 56, or 64-bit
// values, and 32-bit values can be streamed in from 24 and 32-bit values
// (consistent with what has been written to the stream, of course).  Note
// that, for signed types, the sign is preserved for all streamed-in values.
//
// Note that input streams can be *invalidated* explicitly and queried for
// *validity* and *emptiness*.  Reading from an initially invalid stream has no
// effect.  Attempting to read beyond the end of a stream will automatically
// invalidate the stream.  Whenever an inconsistent value is detected, the
// stream should be invalidated explicitly.
//
///Externalization and Value Semantics
///-----------------------------------
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length *L* and stream back in an array of length other than *L*.
//
///Input Limit
///-----------
// If exceptions are enabled at compile time, the test input stream can be
// configured to throw an exception after a specified number of input requests
// is exceeded.  If the input limit is less than zero (default), then the
// stream never throws an exception.  Note that a non-negative input limit is
// decremented after each input attempt, and throws only when the current input
// limit transitions from 0 to -1; no additional exceptions will be thrown
// until the input limit is again reset to a non-negative value.
//
///Version Checking
///----------------
// By default, 'bdex_TestInStream' assumes that the first unit of data in the
// stream is of type 'char' and represents the version number of an object to
// be (un)externalized.  When the first input method is invoked on a
// 'bdex_TestInStream', the stream checks for the existence of this version
// number and that it has a value greater than 0.  This automatic version
// checking helps to ensure that an object's externalization operation is
// correctly devised to include its version number as the first unit of data in
// the stream.  For data streams where automatic checking of version number is
// not desired (e.g., testing corrupted streams), this checking can be disabled
// by invoking the 'setSuppressVersionCheck' method with an argument of 1.
//
///Exception Test Macros
///---------------------
// This component also provides a pair of macros, 'BEGIN_BDEX_EXCEPTION_TEST'
// and 'END_BDEX_EXCEPTION_TEST'.  These macros can be used for testing
// exception safety of classes and their methods when 'bdex' streaming is
// involved.
//
// A 'bdex_TestInStream' named 'testInStream' *must* *be* *defined* *in*
// *scope* in order to use this pair of macros.  Note that if
// exception-handling is disabled (i.e., if 'BDE_BUILD_TARGET_EXC' is not
// defined), then the macros simply print the following:
//..
//          BDEX EXCEPTION TEST -- (NOT ENABLED) --
//..
// When exception-handling is enabled, the macros will set the 'testInStream's
// input stream size limit to 0, 'try' the code being tested, 'catch' any
// exceptions thrown due to input stream limit reached, and keep increasing the
// input limit until the code being tested completed successfully.
//
///Usage
///-----
// This 'bdex_testinstream' component (in conjunction with the companion
// 'bdex_testoutstream' component) is most useful in testing the 'bdex'
// externalization functionality of other components.  The following code
// exhibits the use of the 'bdex' test streams in conjunction with the
// 'BEGIN_BDEX_EXCEPTION_TEST' and 'END_BDEX_EXCEPTION_TEST' macros in
// performing a through test of the 'streamIn' and 'streamOut' functions of a
// hypothetical 'my_doublearray' component.
//
// The test program shown uses the standard 'bde' test methodology, and as such
// contains several useful constructs and conventions that are not fully
// described here.  Among them are:
//
// The 'ASSERT' and various 'LOOP_ASSERT' macros, which generate specific error
// reports if their last argument evaluates to zero.
//
// The 'g' function, which takes a 'char *' argument in specialized input
// language and generates an array with a value corresponding to the input
// specification.
//
// The 'verbose' flag is generated from a command-line argument, and indicates
// whether to generate non-error status messages.
//
// Various printing macros such as 'P()', 'P_()', and such, which format their
// arguments as appropriate status messages.
//
// 'Obj' is the standard 'typedef' for the object under test, in this case
// 'class my_DoubleArray'.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment").
//..
//  // my_DoubleArray.t.cpp
//  #include <my_doublearray.h>
//  #include <bdex_testinstream.h>
//  #include <bdex_testinstreamexception.h>
//  // ...
//
//  typedef my_DoubleArray Obj;
//  typedef double Element;
//
//  const Element VALUES[] = { 1.5, 2.5, 3.5, 4.75, -5.25 };
//
//  const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
//                &V1 = VALUES[1], &VB = V1,  // conjunction with VALUES array.
//                &V2 = VALUES[2], &VC = V2,
//                &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
//                &V4 = VALUES[4], &VE = V4;  // conjunction with 'g' and 'gg'.
//
//  const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
//
//  // ...
//
//  switch (test) { case 0:
//  // ...
//
//    case 10: {
//      // --------------------------------------------------------------------
//      // TESTING STREAMING FUNCTIONALITY:
//      //   First perform a trivial direct test of the 'streamOut' and
//      //   'streamIn' methods (the rest of the testing will use the stream
//      //   operators).
//      //
//      //   VALID STREAMS
//      //     Use 'g' to construct a set, S, of unique 'my_DoubleArray'
//      //     objects with substantial and varied differences in value.  Using
//      //     all combinations (u,v) in the cross product S X S, stream the
//      //     value of v (into a temporary copy of) u and assert u == v.
//      //
//      //   EXCEPTION NEUTRALITY
//      //     Perform the same test as described above for VALID STREAMS,
//      //     except that during the stream-in operation, configure the
//      //     'bdex_TestInStream' object to throw an exception for every
//      //     input method invoked within the 'streamIn' method.  Use purify
//      //     to verify that no memory is leaked.
//      //
//      //   EMPTY AND INVALID STREAMS
//      //     For each u in S, create a copy and attempt to stream into it
//      //     from an empty and then invalid stream.  Verify after each try
//      //     that the object is unchanged and that the stream is invalid.
//      //
//      //   INCOMPLETE (BUT OTHERWISE VALID) DATA
//      //     Write 3 distinct 'my_DoubleArray' objects to an output stream
//      //     buffer of total length N.  For each partial stream length from
//      //     0 to N - 1, construct an input stream and attempt to read into
//      //     objects initialized with distinct values.  Verify values of
//      //     objects that are either successfully modified or left entirely
//      //     unmodified, and that the stream became invalid immediately after
//      //     the first incomplete read.  Finally ensure that each object
//      //     streamed into is in some valid state by assigning it a distinct
//      //     new value and testing for equality.
//      //
//      //   CORRUPTED DATA
//      //     Use the underlying stream package to simulate an instance of a
//      //     typical valid (control) stream and verify that it can be
//      //     streamed in successfully.  Then for each data field in the
//      //     stream (beginning with the version number), provide one or more
//      //     similar tests with that data field corrupted.  After each test,
//      //     verify that the object is in some valid state after streaming,
//      //     and that the input stream has gone invalid.
//      //
//      // Testing:
//      //   bdex_InStream& streamIn(bdex_InStream& stream);
//      //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
//      //   Ensure streamIn is exception neutral
//      // --------------------------------------------------------------------
//
//      if (verbose) bsl::cout << bsl::endl
//                        << "Testing Streaming Functionality" << bsl::endl
//                        << "===============================" << bsl::endl;
//
//      // To corrupt the output stream of any BDE object for testing purposes,
//      // replace the first byte of the stream with the value M1 that follows:
//      char M1 = 0xff;
//
//      static const char *SPECS[] = {
//          "",     "A",     "B",      "CC",      "DE",       "ABC",
//          "BCDE", "CDEAB", "DEABCD", "EABCDEA", "ABCDEABC", "BCDEABCDE",
//      0}; // Null string required as last element.
//
//      if (verbose) bsl::cout <<
//          "\nTesting streamOut and (valid) streamIn functionality."
//                             << bsl::endl;
//
//      const int VERSION = Obj::maxSupportedBdexVersion();
//
//      if (verbose) bsl::cout <<
//          "\tTrivial direct test of streamIn and streamOut methods."
//                             << bsl::endl;
//      {
//          const Obj X(g("ABC"));
//          bdex_TestOutStream out;             X.bdexStreamOut(out, VERSION);
//
//          const char *const PD  = out.data();  ASSERT(*PD != M1);
//          const int         NPB = out.length();
//          bdex_TestInStream in(PD, NPB);
//          ASSERT(in);                          ASSERT(!in.isEmpty());
//          in.setSuppressVersionCheck(1);
//
//          Obj t(g("DE"));                      ASSERT(X != t);
//          t.bdexStreamIn(in, VERSION);         ASSERT(X == t);
//          ASSERT(in);                          ASSERT(in.isEmpty());
//      }
//
//      if (verbose) bsl::cout <<
//          "\tThorough test of bdex stream functions." << bsl::endl;
//      {
//          for (int i = 0; SPECS[i]; ++i) {
//              const Obj X(g(SPECS[i]));
//              bdex_TestOutStream out;
//              bdex_OutStreamFunctions::streamOut(out, X, VERSION);
//              const char *const PD  = out.data();
//                                              LOOP_ASSERT(i, *PD != M1);
//              const int         NPB = out.length();
//
//              // Verify that each new value overwrites every old value
//              // and that the input stream is emptied, but remains valid.
//
//              for (int j = 0; SPECS[j]; ++j) {
//                  bdex_TestInStream in(PD, NPB);
//                  in.setSuppressVersionCheck(1);
//                  LOOP2_ASSERT(i,j,in);   LOOP2_ASSERT(i,j,!in.isEmpty());
//
//                  Obj t(g(SPECS[j]));
//                  LOOP2_ASSERT(i, j, X == t == (i == j));
//                  bdex_InStreamFunctions::streamIn(in, t, VERSION);
//                  LOOP2_ASSERT(i, j, X == t);
//                  LOOP2_ASSERT(i, j, in); LOOP2_ASSERT(i,j,in.isEmpty());
//              }
//          }
//      }
//
//      if (verbose) bsl::cout <<
//          "\tTest streamIn method for exception neutrality." << bsl::endl;
//      {
//          for (int i = 0; SPECS[i]; ++i) {
//              const Obj X(g(SPECS[i]));
//              bdex_TestOutStream out;
//              bdex_OutStreamFunctions::streamOut(out, X, VERSION);
//              const char *const PD  = out.data();
//                                              LOOP_ASSERT(i, *PD != M1);
//              const int         NPB = out.length();
//
//              // Verify that each new value overwrites every old value
//              // and that the input stream is emptied, but remains valid.
//
//              for (int j = 0; SPECS[j]; ++j) {
//                  bdex_TestInStream testInStream(PD, NPB);
//                  bdex_TestInStream& in = testInStream;
//                  in.setSuppressVersionCheck(1);
//                  LOOP2_ASSERT(i,j,in);   LOOP2_ASSERT(i,j,!in.isEmpty());
//
//                  Obj t(g(SPECS[j]));
//                  LOOP2_ASSERT(i, j, X == t == (i == j));
//                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
//                      bdex_InStreamFunctions::streamIn(in, t, VERSION);
//                  } END_BDEX_EXCEPTION_TEST
//                  LOOP2_ASSERT(i, j, X == t);
//                  LOOP2_ASSERT(i, j, in); LOOP2_ASSERT(i,j,in.isEmpty());
//              }
//          }
//      }
//
//      if (verbose) bsl::cout << "\tOn empty and invalid streams."
//                             << bsl::endl;
//      {
//          bdex_TestOutStream out;
//          const char *const  PD = out.data();
//          const int         NPB = out.length();
//          ASSERT(0 == NPB);
//
//          for (int i = 0; SPECS[i]; ++i) {
//              bdex_TestInStream in(PD, NPB);
//              in.setSuppressVersionCheck(1);
//              LOOP_ASSERT(i, in);            LOOP_ASSERT(i, in.isEmpty());
//
//              // Ensure that reading from an empty or invalid input stream
//              // leaves the stream invalid and the target object unchanged.
//
//              const Obj X(g(SPECS[i])); Obj t(X);     LOOP_ASSERT(i, X == t);
//              bdex_InStreamFunctions::streamIn(in, t, VERSION);
//              LOOP_ASSERT(i, !in);         LOOP_ASSERT(i, X == t);
//              bdex_InStreamFunctions::streamIn(in, t, VERSION);
//              LOOP_ASSERT(i, !in);         LOOP_ASSERT(i, X == t);
//          }
//      }
//
//      if (verbose) bsl::cout <<
//          "\tOn incomplete (but otherwise valid) data." << bsl::endl;
//      {
//          const Obj W1 = g("A"),       X1 = g("CDCD"),   Y1 = g("BB");
//          const Obj W2 = g("BCBCBCB"), X2 = g("ADEAD"),  Y2 = g("CABDE");
//          const Obj W3 = g("DEEDDE"),  X3 = g("ABABAB"), Y3 = g("C");
//
//          bdex_TestOutStream out;
//          bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
//          const int NPB1 = out.length();
//          bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
//          const int NPB2 = out.length();
//          bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
//          const int NPB  = out.length();
//          const char *const PD = out.data(); ASSERT(*PD != M1);
//
//          for (int i = 0; i < NPB; ++i) {
//              bdex_TestInStream in(PD,i);  in.setQuiet(!veryVerbose);
//              in.setSuppressVersionCheck(1);
//              LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
//              Obj t1(W1), t2(W2), t3(W3);
//
//              if (i < NPB1) {
//                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i, !in);
//                                    if (0 == i) LOOP_ASSERT(i, W1 == t1);
//                  bdex_InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i, !in); LOOP_ASSERT(i, W2 == t2);
//                  bdex_InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in); LOOP_ASSERT(i, W3 == t3);
//              }
//              else if (i < NPB2) {
//                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i,  in); LOOP_ASSERT(i, X1 == t1);
//                  bdex_InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i, !in);
//                                 if (NPB1 == i) LOOP_ASSERT(i, W2 == t2);
//                  bdex_InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in); LOOP_ASSERT(i, W3 == t3);
//              }
//              else {
//                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i,  in); LOOP_ASSERT(i, X1 == t1);
//                  bdex_InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i,  in); LOOP_ASSERT(i, X2 == t2);
//                  bdex_InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in);
//                                 if (NPB2 == i) LOOP_ASSERT(i, W3 == t3);
//              }
//
//                              LOOP_ASSERT(i, Y1 != t1);
//              t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);
//
//                              LOOP_ASSERT(i, Y2 != t2);
//              t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);
//
//                              LOOP_ASSERT(i, Y3 != t3);
//              t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
//          }
//      }
//
//      if (verbose) bsl::cout << "\tOn corrupted data." << bsl::endl;
//
//      const Obj W = g("");            // default value
//      const Obj X = g("DCB");         // original value
//      const Obj Y = g("ABCDE");       // new value
//
//      if (verbose) bsl::cout << "\t\tGood stream (for control)."
//                             << bsl::endl;
//      {
//          const int length  = 5;
//
//          bdex_TestOutStream out;    // Stream out "new" value.
//          bdex_OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          Obj t(X);    ASSERT(W != t);   ASSERT(X == t);     ASSERT(Y != t);
//          bdex_TestInStream in(PD, NPB); ASSERT(in);
//          in.setSuppressVersionCheck(1);
//          bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(in);
//                       ASSERT(W != t);   ASSERT(X != t);     ASSERT(Y == t);
//      }
//
//      if (verbose) bsl::cout << "\t\tBad version." << bsl::endl;
//      {
//          const char version = -1; // too small
//          const int  length  = 5;
//
//          bdex_TestOutStream out;    // Stream out "new" value.
//          bdex_OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          Obj t(X);    ASSERT(W != t);    ASSERT(X == t);   ASSERT(Y != t);
//          bdex_TestInStream in(PD, NPB);  in.setQuiet(!veryVerbose);
//          in.setSuppressVersionCheck(1);
//          ASSERT(in);
//          bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                       ASSERT(W != t);    ASSERT(X == t);   ASSERT(Y != t);
//      }
//      {
//          const char version = 100; // too large
//          const int  length  = 5;
//
//          bdex_TestOutStream out;    // Stream out "new" value.
//          bdex_OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          Obj t(X);    ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
//          bdex_TestInStream in(PD, NPB);
//          in.setSuppressVersionCheck(1);
//          ASSERT(in);
//          bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                       ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
//      }
//
//      if (verbose) bsl::cout << "\t\tBad length." << bsl::endl;
//      {
//          const int length = -1; // too small
//
//          bdex_TestOutStream out;    // Stream out "new" value.
//          bdex_OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          Obj t(X);    ASSERT(W != t);   ASSERT(X == t);    ASSERT(Y != t);
//          bdex_TestInStream in(PD, NPB);
//          in.setSuppressVersionCheck(1);
//          ASSERT(in);
//          bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                       ASSERT(W != t);   ASSERT(X == t);    ASSERT(Y != t);
//      }
//    } break;
//    // ...
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_TESTINSTREAMEXCEPTION
#include <bdex_testinstreamexception.h>
#endif

#ifndef INCLUDED_BDEX_TESTINSTREAMFORMATTER
#include <bdex_testinstreamformatter.h>
#endif

#ifndef INCLUDED_BDESB_FIXEDMEMINSTREAMBUF
#include <bdesb_fixedmeminstreambuf.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // =======================
                        // class bdex_TestInStream
                        // =======================

class bdex_TestInStream {
    // This class provides input methods to unexternalize values (and C-style
    // arrays of values) of fundamental types from their byte representations.
    // By default, each input method also verifies the stream version (but
    // verification may be disabled via the 'setSuppressVersionCheck' method)
    // and the input value type.  By default, if invalid data is detected,
    // error messages are displayed on 'stdout'; this error reporting may be
    // disables via the 'setQuiet' method.  Note that attempting to read beyond
    // the end of a stream will automatically invalidate the stream.  The class
    // supports validity testing by providing the method 'operator const
    // void *()'.

    // DATA
    bslma_Allocator            *d_allocator_p;  // holds memory allocator
    int                         d_length;       // length of 'd_buffer_p'
    char                       *d_buffer_p;     // bytes to be unexternalized
    bdesb_FixedMemInStreamBuf   d_streambuf;    // externalized values
    bdex_TestInStreamFormatter  d_formatter;    // formatter

    // NOT DEFINED
    bdex_TestInStream(const bdex_TestInStream&);
    bdex_TestInStream& operator=(const bdex_TestInStream&);

    bool operator==(const bdex_TestInStream&) const;
    bool operator!=(const bdex_TestInStream&) const;

  public:
    // CREATORS
    bdex_TestInStream(bslma_Allocator *basicAllocator = 0);
        // Create an empty input byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The constructed
        // object is useless until a buffer is set with the 'reload' method.

    bdex_TestInStream(const char *buffer, int numBytes,
                      bslma_Allocator *basicAllocator = 0);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from (a copy of) the specified 'buffer'.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numBytes' and
        // 'numBytes' is less than or equal to the length of 'buffer'.

    ~bdex_TestInStream();
        // Destroy this input byte stream.

    // MANIPULATORS
    void setQuiet(int flag);
        // Set this stream's quiet mode to the specified 'flag'.  If 'flag' is
        // non-zero, then quiet mode is turned ON, and no error message will be
        // written to the standard output.  If 'flag' is zero, then quiet mode
        // is turned OFF.  Note that quiet mode is turned OFF by default.

    void setSuppressVersionCheck(int flag);
        // Set this stream's suppress-version-check mode to the specified
        // 'flag'.  If 'flag' is non-zero, then suppress-version-check mode is
        // turned ON, and this stream will not check for the existence of
        // version information when reading.  If 'flag' is zero, then
        // suppress-version-check mode is turned OFF, and the version
        // information will be checked.  Note that suppress-version-check mode
        // is turned OFF by default.

    void setInputLimit(int limit);
        // Set the number of input operations allowed on this stream to the
        // specified 'limit' before an exception is thrown.  If 'limit' is less
        // than 0, no exception is to be thrown.  By default, no exception is
        // scheduled.

    void seek(int offset);
        // Set the index of the next byte to be extracted from this stream to
        // the specified 'offset' and validate this stream if it is currently
        // invalid.  The behavior is undefined unless offset is less than or
        // equal to the number of bytes in this stream.

    void reset();
        // Set the index of the next byte to be extracted from this stream to 0
        // (i.e., the beginning of the stream) and validate this stream if it
        // is currently invalid.

    void reload(const char *buffer, int numBytes);
        // Replace this stream's content with (a copy of) the specified initial
        // 'numBytes' from the specified 'buffer' and set the index of the next
        // byte to be extracted from this stream to 0 (i.e., the beginning of
        // the stream).  Also validate this stream if it is currently invalid.
        // The behavior is undefined unless '0 <= numBytes'.

    bdex_TestInStream& getLength(int& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer or 32-bit signed integer value representing a
        // length (see the package group level documentation) into the
        // specified 'variable' if its type is appropriate, and return a
        // reference to this modifiable stream dependent on the most
        // significant bit of the next byte in the stream; if it is 0 consume
        // an 8-bit unsigned integer, otherwise a 32-bit signed integer after
        // setting this bit to zero.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variable' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getString(bsl::string& str);
        // Consume a string from this input stream, place that value in the
        // specified 'str', and return a reference to this modifiable stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // The string must be prefaced by a non-negative integer indicating the
        // number of characters composing the string.  Behavior is undefined if
        // the length indicator is not greater than 0.

    bdex_TestInStream& getVersion(int& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer value representing a version (see the package group
        // level documentation) into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename TYPE>
    bdex_TestInStream& get(TYPE& variable);
        // TBD

// TBD #endif

                        // *** scalar integer values ***

    bdex_TestInStream& getInt64(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint64(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 64-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt56(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint56(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 56-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt48(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint48(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 48-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt40(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint40(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 40-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt32(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 32-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint32(unsigned int& variable);
        // Verify the type of the next value in this stream, consume that
        // 32-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt24(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 24-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint24(unsigned int& variable);
        // Verify the type of the next value in this stream, consume that
        // 24-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt16(short& variable);
        // Verify the type of the next value in this stream, consume that
        // 16-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getUint16(unsigned short& variable);
        // Verify the type of the next value in this stream, consume that
        // 16-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStream& getInt8(char& variable);
    bdex_TestInStream& getInt8(signed char& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // signed integer value into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    bdex_TestInStream& getUint8(char& variable);
    bdex_TestInStream& getUint8(unsigned char& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer value into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

                        // *** scalar floating-point values ***

    bdex_TestInStream& getFloat64(double& variable);
        // Verify the type of the next value in this stream, consume that IEEE
        // double-precision (8-byte) floating-point value into the specified
        // 'variable' if its type is appropriate, and return a reference to
        // this modifiable stream.  If the type is incorrect, then this stream
        // is marked invalid and the value of 'variable' is unchanged.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.  Note that
        // for non-conforming platforms, this operation may be lossy.

    bdex_TestInStream& getFloat32(float& variable);
        // Verify the type of the next value in this stream, consume that IEEE
        // single-precision (4-byte) floating-point value into the specified
        // 'variable' if its type is appropriate, and return a reference to
        // this modifiable stream.  If the type is incorrect, then this stream
        // is marked invalid and the value of 'variable' is unchanged.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.  Note that
        // for non-conforming platforms, this operation may be lossy.

                        // *** arrays of integer values ***

    bdex_TestInStream& getArrayInt64(bsls_PlatformUtil::Int64 *array,
                                     int                       length);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint64(bsls_PlatformUtil::Uint64 *array,
                                      int                        length);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt56(bsls_PlatformUtil::Int64 *array,
                                     int                       length);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint56(bsls_PlatformUtil::Uint64 *array,
                                      int                        length);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt48(bsls_PlatformUtil::Int64 *array,
                                     int                       length);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint48(bsls_PlatformUtil::Uint64 *array,
                                      int                        length);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt40(bsls_PlatformUtil::Int64 *array,
                                     int                       length);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint40(bsls_PlatformUtil::Uint64 *array,
                                      int                        length);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt32(int *array, int length);
        // Verify the type of the next value in this stream, consume that
        // 32-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint32(unsigned int *array, int length);
        // Verify the type of the next value in this stream, consume that
        // 32-bit unsigned integer array value into the specified 'array' of
        // the specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt24(int *array, int length);
        // Verify the type of the next value in this stream, consume that
        // 24-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint24(unsigned int *array, int length);
        // Verify the type of the next value in this stream, consume that
        // 24-bit unsigned integer array value into the specified 'array' of
        // the specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt16(short *array, int length);
        // Verify the type of the next value in this stream, consume that
        // 16-bit signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint16(unsigned short *array, int length);
        // Verify the type of the next value in this stream, consume that
        // 16-bit unsigned integer array value into the specified 'array' of
        // the specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayInt8(char *array, int length);
    bdex_TestInStream& getArrayInt8(signed char *array, int length);
        // Verify the type of the next value in this stream, consume that 8-bit
        // signed integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

    bdex_TestInStream& getArrayUint8(char *array, int length);
    bdex_TestInStream& getArrayUint8(unsigned char *array, int length);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer array value into the specified 'array' of the
        // specified 'length' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'array' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.

                        // *** arrays of floating-point values ***

    bdex_TestInStream& getArrayFloat64(double *array, int length);
        // Verify the type of the next value in this stream, consume that IEEE
        // double-precision (8-byte) floating-point array value into the
        // specified 'array' of the specified 'length' if its type and length
        // are appropriate, and return a reference to this modifiable stream.
        // If the type is incorrect, then this stream is marked invalid and the
        // value of 'array' is unchanged.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    bdex_TestInStream& getArrayFloat32(float *array, int length);
        // Verify the type of the next value in this stream, consume that IEEE
        // single-precision (4-byte) floating-point array value into the
        // specified 'array' of the specified 'length' if its type and length
        // are appropriate, and return a reference to this modifiable stream.
        // If the type is incorrect, then this stream is marked invalid and the
        // value of 'array' is unchanged.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'array' is undefined.  The behavior is undefined unless
        // '0 <= length'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if this stream is valid, and 0 otherwise.  An
        // invalid stream denotes a stream in which insufficient or invalid
        // data was detected during an extraction operation.  Note that an
        // empty stream will be valid unless an extraction attempt or explicit
        // invalidation causes it to be otherwise.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.  The
        // behavior is undefined unless this stream is valid (i.e., would
        // convert to a non-zero value via the 'operator const void *()'
        // member).  Note that this function enables higher-level components
        // to verify that, after successfully reading all expected data, no
        // data remains.

    int length() const;
        // Return the total number of bytes stored in this stream.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.

    bool isQuiet() const;
        // Return 'true' if this stream's quiet mode is ON, and 'false'
        // otherwise.

    bool isSuppressVersionCheck() const;
        // Return 'true' if this stream's version-checking mode is OFF, and
        // 'false' otherwise.

    int inputLimit() const;
        // Return the current number of input requests left before an exception
        // is thrown.  A negative value indicated that no exception is
        // scheduled.

    friend bsl::ostream& operator<<(bsl::ostream&, const bdex_TestInStream&);
};

// FREE OPERATORS
// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename VALUE>
inline
bdex_TestInStream& operator>>(bdex_TestInStream& stream, VALUE& value);
    // Read from the specified 'stream' a 'bdex' version and assign to the
    // specified 'bdex'-compliant 'object' the value read from 'stream' using
    // 'OBJECT's 'bdex' version format as read from 'stream'.  Return a
    // reference to the modifiable 'stream'.  If 'stream' is initially invalid,
    // this operation has no effect.  If 'stream' becomes invalid during this
    // operation, 'object' is valid, but its value is undefined.  If the read
    // in version is not supported by 'object', 'stream' is marked invalid,
    // but 'object' is unaltered.  The behavior is undefined unless the data in
    // 'stream' was written by a 'bdex_ByteOutStream' object.  (See the 'bdex'
    // package-level documentation for more information on 'bdex' streaming of
    // container types.)  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'get' method from this
    // component.

// TBD #endif

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const bdex_TestInStream& obj);
    // Write the specified 'obj' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'

//=============================================================================
// STANDARD BDEX EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC

#ifndef BEGIN_BDEX_EXCEPTION_TEST
#define BEGIN_BDEX_EXCEPTION_TEST                                             \
{                                                                             \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime) bsl::cout <<                            \
            "### BDEX EXCEPTION TEST -- (ENABLED) --" << '\n';                \
        firstTime = 0;                                                        \
    }                                                                         \
    if (veryVeryVerbose) bsl::cout <<                                         \
        "### Begin bdex exception test." << '\n';                             \
    int bdexExceptionCounter = 0;                                             \
    static int bdexExceptionLimit = 100;                                      \
    testInStream.setInputLimit(bdexExceptionCounter);                         \
    do {                                                                      \
        try {
#endif  // BEGIN_BDEX_EXCEPTION_TEST

#ifndef END_BDEX_EXCEPTION_TEST
#define END_BDEX_EXCEPTION_TEST                                               \
        } catch (bdex_TestInStreamException& e) {                             \
            if ((veryVerbose && bdexExceptionLimit) || veryVeryVerbose)       \
            {                                                                 \
                --bdexExceptionLimit;                                         \
                bsl::cout << "(" << bdexExceptionCounter << ')';              \
                if (veryVeryVerbose) { bsl::cout << " BDEX_EXCEPTION: "       \
                    << "input limit = " << bdexExceptionCounter << ", "       \
                    << "last data type = " << e.dataType();                   \
                }                                                             \
                else if (0 == bdexExceptionLimit) {                           \
                    bsl::cout << " [ Note: 'bdexExceptionLimit' reached. ]";  \
                }                                                             \
                bsl::cout << '\n';                                            \
            }                                                                 \
            testInStream.setInputLimit(++bdexExceptionCounter);               \
            continue;                                                         \
        }                                                                     \
        testInStream.setInputLimit(-1);                                       \
        break;                                                                \
    } while (1);                                                              \
    if (veryVeryVerbose) bsl::cout <<                                         \
        "### End bdex exception test." << '\n';                               \
}
#endif  // END_BDEX_EXCEPTION_TEST

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BEGIN_BDEX_EXCEPTION_TEST
#define BEGIN_BDEX_EXCEPTION_TEST                                             \
{                                                                             \
    static int firstTime = 1;                                                 \
    if (verbose && firstTime) { bsl::cout <<                                  \
        "### BDEX EXCEPTION TEST -- (NOT ENABLED) --" << '\n';                \
        firstTime = 0;                                                        \
    }                                                                         \
}
#endif  // BEGIN_BDEX_EXCEPTION_TEST

#ifndef END_BDEX_EXCEPTION_TEST
#define END_BDEX_EXCEPTION_TEST
#endif

#endif  // BDE_BUILD_TARGET_EXC

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// MANIPULATORS
inline
void bdex_TestInStream::setQuiet(int flag)
{
    d_formatter.setQuiet(flag);
}

inline
void bdex_TestInStream::setSuppressVersionCheck(int flag)
{
    d_formatter.setSuppressVersionCheck(flag);
}

inline
void bdex_TestInStream::setInputLimit(int limit)
{
    d_formatter.setInputLimit(limit);
}

inline
void bdex_TestInStream::invalidate()
{
    d_formatter.invalidate();
}

inline
void bdex_TestInStream::seek(int offset)
{
    d_formatter.seek(offset);
}

inline
void bdex_TestInStream::reset()
{
    d_formatter.reset();
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename TYPE>
inline
bdex_TestInStream& bdex_TestInStream::get(TYPE& variable)
{
    return bdex_InStreamMethodsUtil::streamInVersionAndObject(*this, variable);
}

// TBD #endif

// ACCESSORS
inline
bdex_TestInStream::operator const void *() const
{
    return d_formatter ? this : 0;
}

inline
bool bdex_TestInStream::isEmpty() const
{
    return 0 == d_streambuf.length();
}

inline
int bdex_TestInStream::length() const
{
    return d_length;
}

inline
int bdex_TestInStream::cursor() const
{
    return d_length - static_cast<int>(d_streambuf.length());
}

inline
bool bdex_TestInStream::isQuiet() const
{
    return d_formatter.isQuiet();
}

inline
bool bdex_TestInStream::isSuppressVersionCheck() const
{
    return d_formatter.isSuppressVersionCheck();
}

inline
int bdex_TestInStream::inputLimit() const
{
    return d_formatter.inputLimit();
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename VALUE>
inline
bdex_TestInStream& operator>>(bdex_TestInStream& stream, VALUE& value)
{
    return stream.get(value);
}

// TBD #endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
