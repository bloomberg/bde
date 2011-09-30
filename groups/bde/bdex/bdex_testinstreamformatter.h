// bdex_testinstreamformatter.h                                       -*-C++-*-
#ifndef INCLUDED_BDEX_TESTINSTREAMFORMATTER
#define INCLUDED_BDEX_TESTINSTREAMFORMATTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enable unexternalization of fundamental types with identification.
//
//@CLASSES:
//   bdex_TestInStreamFormatter: byte-array-based input stream class
//
//@AUTHOR: Lea Fester
//
//@SEE_ALSO: bdex_testoutstream
//
//@DESCRIPTION: This component implements the 'bdex_InStream' protocol as a
// byte-array-based data stream object that provides the input methods
// ("unexternalization") of a platform-independent externalization package.
// Each input method reads either a value or a homogeneous array of values of a
// fundamental type that has been formatted by a cooperating externalization
// component, 'bdex_TestOutStream'.  The supported types and required content
// are as follows (note that 'Int64' and 'Uint64' are 'typedef' names for the
// signed and unsigned 64-bit integer types, respectively, on the host
// platform):
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
//..
// In addition to basic streaming methods, this component provides an interface
// for efficient streaming of integer types.  In particular, 64-bit values
// can be streamed as 40, 48, 56, or 64-bit values, and 32-bit values can be
// streamed as 24 and 32-bit values.
//
// Note that (unlike output streams) input streams can be *invalidated*
// explicitly and queried for *validity* and *emptiness*.  Reading from an
// initially invalid stream has no effect.  Attempting to read beyond the end
// of a stream will automatically invalidate the stream.  Whenever an
// inconsistent value is detected, the stream should be invalidated explicitly.
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
// By default, 'bdex_TestInStreamFormatter' assumes that the first unit of
// data in the stream is of type 'char' and represents the version number of
// an object to be (un)externalized.  When the first input method is invoked
// on a 'bdex_TestInStreamFormatter', the stream checks for the existence of
// this version number and that it has a value greater than 0.  This automatic
// version checking helps to ensure that an object's externalization operation
// is correctly devised to include its version number as the first unit of
// data in the stream.  For data streams where automatic checking of version
// number is not desired (e.g., testing corrupted streams), this checking can
// be disabled by invoking the 'setSuppressVersionCheck' method with an
// argument of 1.
//
///Usage
///-----
// This 'bdex_TestInStreamFormatter' component (in conjunction with the
// companion 'bdex_testoutstream' component) is most useful in testing the
// 'bdex' externalization functionality of other components.  The following
// code exhibits the use of the 'bdex' test streams in performing a through
// test of the 'streamIn' and 'streamOut' functions of a hypothetical
// 'my_doublearray' component.
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
// In the following code, the macros 'BEGIN_BDEX_EXCEPTION_TEST' and
// 'END_BDEX_EXCEPTION_TEST' are defined so that they can be easily inserted to
// encompass a section of code to test exception neutrality during the
// stream-in operation.  The macros assume that a 'bdex_TestInStreamFormatter'
// object named 'testInStream' exists and is initialized with data.  When
// placed around a section of code, the macros initially set the input limit of
// 'testInStream' to 0 and then executes the code.  If an exception is thrown
// during execution, the macros increment the stream's input limit by one and
// re-execute the code.  Note that the 'reset' method of 'testInStream' is
// invoked at the start of the code being tested to ensure that the stream is
// reset to the beginning of its data for every iteration over the code.  This
// process is repeated indefinite times until no exception is thrown.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment").
//..
//  // my_DoubleArray.t.cpp
//  #include <my_doublearray.h>
//  #include <bdex_TestInStreamFormatter.h>
//  #include <bdex_TestInStreamException.h>
//  // ...
//
//  #ifdef BDE_BUILD_TARGET_EXC
//  #define BEGIN_BDEX_EXCEPTION_TEST                                        \$
//  {                                                                        \$
//      {                                                                    \$
//          static int firstTime = 1;                                        \$
//          if (veryVerbose && firstTime) bsl::cout <<                       \$
//              "### BDEX EXCEPTION TEST -- (ENABLED) --" << bsl::endl;      \$
//          firstTime = 0;                                                   \$
//      }                                                                    \$
//      if (veryVeryVerbose) bsl::cout <<                                    \$
//          "### Begin bdex exception test." << bsl::endl;                   \$
//      int bdexExceptionCounter = 0;                                        \$
//      static int bdexExceptionLimit = 100;                                 \$
//      testInStream.setInputLimit(bdexExceptionCounter);                    \$
//      do {                                                                 \$
//          try {
//
//  #define END_BDEX_EXCEPTION_TEST                                          \$
//          } catch (bdex_TestInStreamException& e) {                       \$
//              if (veryVerbose && bdexExceptionLimit || veryVeryVerbose)    \$
//              {                                                            \$
//                  --bdexExceptionLimit;                                    \$
//                  bsl::cout << "(" << bdexExceptionCounter << ')';         \$
//                  if (veryVeryVerbose) { bsl::cout << " BDEX_EXCEPTION: "  \$
//                      << "input limit = " << bdexExceptionCounter << ", "  \$
//                      << "last data type = " << e.dataType();              \$
//                  }                                                        \$
//                  else if (0 == bdexExceptionLimit) {                      \$
//                      bsl::cout <<                                         \$
//                               " [ Note: 'bdexExceptionLimit' reached. ]"; \$
//                  }                                                        \$
//                  bsl::cout << bsl::endl;                                  \$
//              }                                                            \$
//              testInStream.setInputLimit(++bdexExceptionCounter);          \$
//              continue;                                                    \$
//          }                                                                \$
//          testInStream.setInputLimit(-1);                                  \$
//          break;                                                           \$
//      } while (1);                                                         \$
//      if (veryVeryVerbose) bsl::cout <<                                    \$
//          "### End bdex exception test." << bsl::endl;                     \$
//  }
//  #else
//  #define BEGIN_BDEX_EXCEPTION_TEST                                        \$
//  {                                                                        \$
//      static int firstTime = 1;                                            \$
//      if (verbose && firstTime) { bsl::cout <<                             \$
//          "### BDEX EXCEPTION TEST -- (NOT ENABLED) --" << bsl::endl;      \$
//          firstTime = 0;                                                   \$
//      }                                                                    \$
//  }
//  #define END_BDEX_EXCEPTION_TEST
//  #endif
//
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
//      //     'bdex_TestInStreamFormatter' object to throw an exception for
//      //     every input method invoked within the 'streamIn' method.  Use
//      //     purify to verify that no memory is leaked.
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
//      const int VERSION = my_DoubleArray::maxSupportedBdexVersion();
//
//      if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
//                           " (valid) 'streamIn' functionality." << endl;
//      {
//          const my_DoubleArray X(h("ABC"));
//
//          bdesb_MemOutStreamBuf osb;
//          bdex_TestOutStreamFormatter out(&osb);
//          X.bdexStreamOut(out, VERSION);
//
//          const char *const OD  = osb.data();
//          const int         LOD = osb.length();
//
//          bdesb_FixedMemInStreamBuf sb(const_cast<char *>(OD), LOD);
//          bdex_TestInStreamFormatter in(&sb);  ASSERT(in);
//          ASSERT(sb.length() > 0);
//
//          my_DoubleArray t(h("DE"));
//
//          if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
//          ASSERT(X != t);
//          t.bdexStreamIn(in, VERSION); ASSERT(in);  ASSERT(0 == sb.length());
//
//          if (veryVerbose) { cout << "\t  Value after overwrite: "; P(t); }
//          ASSERT(X == t);
//      }
//      if (verbose) cout <<
//          "\nTesting bdex stream functions." << endl;
//
//      if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
//      {
//          static const char *SPECS[] = { // len: 0 - 5,
//              "",       "A",      "BC",     "CDE",    "DEAB",   "EABCD",
//          0}; // Null string required as last element.
//
//          static const int EXTEND[] = {
//              0, 1, 2, 3, 4, 5,
//          };
//          const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;
//
//          int uOldLen = -1;
//          for (int ui = 0; SPECS[ui]; ++ui) {
//              const char *const U_SPEC = SPECS[ui];
//              const int uLen = strlen(U_SPEC);
//
//              if (verbose) {
//                  cout << "\t\tFor source objects of length "
//                                                      << uLen << ":\t";
//                  P(U_SPEC);
//              }
//
//              LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
//              uOldLen = uLen;
//
//              const my_DoubleArray UU = h(U_SPEC);             // control
//              LOOP_ASSERT(ui, uLen == UU.length());   // same lengths
//
//              for (int uj = 0; uj < NUM_EXTEND; ++uj) {
//                  const int U_N = EXTEND[uj];
//
//                  my_DoubleArray mU;              stretchRemoveAll(&mU, U_N);
//                  const my_DoubleArray& U = mU;   hh(&mU, U_SPEC);
//
//                  bdesb_MemOutStreamBuf osb;
//                  bdex_TestOutStreamFormatter out(&osb);
//
//                  // testing stream-out operator here
//                  bdex_OutStreamFunctions::streamOut(out, U, VERSION);
//
//                  const char *const OD  = osb.data();
//                  const int         LOD = osb.length();
//
//                  // Must reset stream for each iteration of inner loop.
//                  bdesb_FixedMemInStreamBuf sb(const_cast<char *>(OD), LOD);
//                  bdex_TestInStreamFormatter testInStream(&sb);
//                  testInStream.setSuppressVersionCheck(1);
//                  LOOP2_ASSERT(U_SPEC, U_N, testInStream);
//                  LOOP2_ASSERT(U_SPEC, U_N, sb.length() > 0);
//
//                  for (int vi = 0; SPECS[vi]; ++vi) {
//                      const char *const V_SPEC = SPECS[vi];
//                      const int vLen = strlen(V_SPEC);
//
//                      const my_DoubleArray VV = h(V_SPEC);      // control
//
//                      if (0 == uj && veryVerbose || veryVeryVerbose) {
//                          cout << "\t\t\tFor destination objects of length "
//                                                      << vLen << ":\t";
//                          P(V_SPEC);
//                      }
//
//                      const int Z = ui == vi; // flag indicating same values
//
//                      for (int vj = 0; vj < NUM_EXTEND; ++vj) {
//                        BEGIN_BDEMA_EXCEPTION_TEST {
//                        BEGIN_BDEX_EXCEPTION_TEST {
//                          testInStream.reset();
//                          const int V_N = EXTEND[vj];
//          //--------------^
//          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
//          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, sb.length() > 0);
//          //--------------v
//                          my_DoubleArray mV;
//                          stretchRemoveAll(&mV, V_N);
//                          const my_DoubleArray& V = mV;  hh(&mV, V_SPEC);
//
//                          static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
//                          if (veryVeryVerbose||veryVerbose && firstFew > 0) {
//                              cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
//                              --firstFew;
//                          }
//
//                          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
//                          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
//                          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));
//
//                          // test stream-in operator here
//                          bdex_InStreamFunctions::streamIn(testInStream,
//                                                           mV,
//                                                           VERSION);
//
//                          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
//                          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
//                          LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);
//
//                        } END_BDEX_EXCEPTION_TEST
//                        } END_BDEMA_EXCEPTION_TEST
//                      }
//                  }
//              }
//          }
//      }
//      if (verbose) cout << "\tOn empty and invalid streams." << endl;
//      {
//          static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
//              "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
//              "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
//              "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
//          0}; // Null string required as last element.
//
//          static const int EXTEND[] = {
//              0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
//          };
//          const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;
//
//          bdesb_FixedMemInStreamBuf sb((char *)"", 0);
//          bdex_TestInStreamFormatter testInStream(&sb);
//          testInStream.setSuppressVersionCheck(1);
//
//          int oldLen = -1;
//          for (int ti = 0; SPECS[ti]; ++ti) {
//              const char *const SPEC = SPECS[ti];
//              const int curLen = strlen(SPEC);
//
//              if (verbose) cout << "\t\tFor objects of length "
//                                                  << curLen << '.' << endl;
//              LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
//              oldLen = curLen;
//
//              if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }
//
//              // Create control object X.
//
//              my_DoubleArray mX;    hh(&mX, SPEC);
//              const my_DoubleArray& X = mX;
//              LOOP_ASSERT(ti, curLen == X.length());  // same lengths
//
//              for (int ei = 0; ei < NUM_EXTEND; ++ei) {
//                BEGIN_BDEMA_EXCEPTION_TEST {
//                BEGIN_BDEX_EXCEPTION_TEST {
//                  testInStream.reset();
//
//                  const int N = EXTEND[ei];
//
//                  if (veryVerbose) { cout << "\t\t\t\t"; P(N); }
//
//                  my_DoubleArray t;           hh(&t, SPEC);
//                  stretchRemoveAll(&t, N);    hh(&t, SPEC);
//
//                // Ensure that reading from an empty or invalid input stream
//                // leaves the stream invalid and the target object unchanged.
//
//                                  LOOP2_ASSERT(ti, ei, testInStream);
//                                  LOOP2_ASSERT(ti, ei, X == t);
//
//                  bdex_InStreamFunctions::streamIn(testInStream, t, VERSION);
//                                  LOOP2_ASSERT(ti, ei, !testInStream);
//                                  LOOP2_ASSERT(ti, ei, X == t);
//
//                  bdex_InStreamFunctions::streamIn(testInStream, t, VERSION);
//                                  LOOP2_ASSERT(ti, ei, !testInStream);
//                                  LOOP2_ASSERT(ti, ei, X == t);
//
//                } END_BDEX_EXCEPTION_TEST
//                } END_BDEMA_EXCEPTION_TEST
//              }
//          }
//      }
//      if (verbose) cout <<
//          "\tOn incomplete (but otherwise valid) data." << endl;
//      {
//          const my_DoubleArray X1 = h("CDCD"), Y1 = h("BB"), Z1 = h("ABCDE");
//          const my_DoubleArray X2 = h("ADE"),  Y2 = h("CABDE"), Z2 = h("B");
//          const my_DoubleArray X3 = h("DEEDDE"), Y3 = h("C"), Z3 = h("DBED");
//
//          bdesb_MemOutStreamBuf osb;
//          bdex_TestOutStreamFormatter out(&osb);
//          bdex_OutStreamFunctions::streamOut(out, Y1, VERSION);
//          const int LOD1 = osb.length();
//          bdex_OutStreamFunctions::streamOut(out, Y2, VERSION);
//          const int LOD2 = osb.length();
//          bdex_OutStreamFunctions::streamOut(out, Y3, VERSION);
//          const int LOD  = osb.length();
//          const char *const OD = osb.data();
//
//          for (int i = 0; i < LOD; ++i) {
//              bdesb_FixedMemInStreamBuf sb(const_cast<char *>(OD), i);
//              bdex_TestInStreamFormatter testInStream(&sb);
//              bdex_TestInStreamFormatter& in = testInStream;
//              in.setSuppressVersionCheck(1);
//              LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == (sb.length() == 0));
//
//              if (veryVerbose) { cout << "\t\t"; P(i); }
//
//              BEGIN_BDEMA_EXCEPTION_TEST {
//              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
//
//              LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == (sb.length() == 0));
//
//              my_DoubleArray t1(X1), t2(X2), t3(X3);
//
//              if (i < LOD1) {
//                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i, !in);
//                                      if (0 == i) LOOP_ASSERT(i, X1 == t1);
//                  bdex_InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
//                  bdex_InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
//              }
//              else if (i < LOD2) {
//                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
//                  bdex_InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i, !in);
//                                   if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
//                  bdex_InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
//              }
//              else {
//                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
//                  bdex_InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
//                  bdex_InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in);
//                                   if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
//              }
//
//              my_DoubleArray w1(t1), w2(t2), w3(t3);
//                                            // make copies to be sure we can
//
//                              LOOP_ASSERT(i, Z1 != w1);
//              w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);
//
//                              LOOP_ASSERT(i, Z2 != w2);
//              w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);
//
//                              LOOP_ASSERT(i, Z3 != w3);
//              w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);
//
//              } END_BDEX_EXCEPTION_TEST
//              } END_BDEMA_EXCEPTION_TEST
//          }
//      }
//      if (verbose) cout << "\tOn corrupted data." << endl;
//
//      const my_DoubleArray W = h("");            // default value
//      const my_DoubleArray X = h("ABCDE");       // original value
//      const my_DoubleArray Y = h("DCB");         // new value
//
//      enum { A_LEN = 3 };
//      Element a[A_LEN];  a[0] = VVD;  a[1] = VVC;  a[2] = VVB;
//
//      if (verbose) cout << "\t\tGood stream (for control)." << endl;
//      {
//          const int length   = 3;
//
//          bdesb_MemOutStreamBuf osb;
//          bdex_TestOutStreamFormatter out(&osb);
//          out.putLength(length);
//          out.putArrayFloat64(a, A_LEN);
//          const char *const OD  = osb.data();
//          const int         LOD = osb.length();
//
//          my_DoubleArray t(X);   ASSERT(W != t);
//          ASSERT(X == t);    ASSERT(Y != t);
//          bdesb_FixedMemInStreamBuf sb(const_cast<char *>(OD), LOD);
//          bdex_TestInStreamFormatter in(&sb); ASSERT(in);
//          in.setSuppressVersionCheck(1);
//          bdex_InStreamFunctions::streamIn(in, t, VERSION);
//          ASSERT(in);
//          ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
//      }
//
//      if (verbose) cout << "\t\tBad version." << endl;
//      {
//          const char version = 0; // too small
//          const int length   = 3;
//
//          bdesb_MemOutStreamBuf osb;
//          bdex_TestOutStreamFormatter out(&osb);
//          out.putLength(length);
//          out.putArrayFloat64(a, A_LEN);
//          const char *const OD  = osb.data();
//          const int         LOD = osb.length();
//
//          my_DoubleArray t(X);   ASSERT(W != t);
//                                 ASSERT(X == t);    ASSERT(Y != t);
//          bdesb_FixedMemInStreamBuf sb(const_cast<char *>(OD), LOD);
//          bdex_TestInStreamFormatter in(&sb);
//          in.setSuppressVersionCheck(1); ASSERT(in);
//          bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                      ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
//      }
//      {
//          const char version = 2; // too large
//          const int length   = 3;
//
//          bdesb_MemOutStreamBuf osb;
//          bdex_TestOutStreamFormatter out(&osb);
//          out.putLength(length);
//          out.putArrayFloat64(a, A_LEN);
//          const char *const OD  = osb.data();
//          const int         LOD = osb.length();
//
//          my_DoubleArray t(X);   ASSERT(W != t);
//                                 ASSERT(X == t);    ASSERT(Y != t);
//          bdesb_FixedMemInStreamBuf sb(const_cast<char *>(OD), LOD);
//          bdex_TestInStreamFormatter in(&sb);
//          in.setSuppressVersionCheck(1); ASSERT(in);
//          bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                      ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
//      }
//    } break;
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_BYTEINSTREAMFORMATTER
#include <bdex_byteinstreamformatter.h>
#endif

#ifndef INCLUDED_BDEX_FIELDCODE
#include <bdex_fieldcode.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_CSTDIO
#include <bsl_cstdio.h>     // for printing in macros
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ================================
                        // class bdex_TestInStreamFormatter
                        // ================================

class bdex_TestInStreamFormatter {
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
    int              d_validFlag;   // flag to indicate this stream's validity
    int              d_quietFlag;   // flag for "quiet" mode
    int              d_suppressVersionCheckFlag;
                                    // flag for "suppress-version-check" mode
    int              d_inputLimit;  // number of input op's before exception

    bdex_ByteInStreamFormatter  d_imp;
    bsl::streambuf             *d_streamBuf;

  private:
    // PRIVATE MANIPULATORS
    void checkArray(int                 *validFlag_p,
                    bdex_FieldCode::Type code,
                    int                  scalarSize,
                    int                  numElements,
                    int                  quietFlag);
    // Verify the validity of the field code and array length, and the
    // sufficiency of data at the specified '*cursor_p' position in the
    // specified 'buffer' of the specified 'bufferLength'.  Extract the field
    // code at the byte position '*cursor_p' from 'buffer'.  If the field code
    // does not correspond to the specified 'code', then set the specified
    // '*validFlag_p' to zero and, if the specified 'quietFlag' is zero, print
    // an error message.  Otherwise, advance '*cursor_p' by the size of the
    // field code and extract the array length.  If the length does not
    // correspond to the specified 'numElements', then set '*validFlag_p' to
    // zero and, if 'quietFlag' is zero, print an error message.  Otherwise,
    // advance '*cursor_p' by the size of the array length, and verify that
    // 'buffer' contains sufficient bytes for the specified 'scalarSize *
    // numElements'.  If there are too few bytes in 'buffer', then set
    // '*validFlag_p' to zero.  The behavior is undefined unless
    // '0 <= *cursor_p', '0 <= bufferLength', '0 < scalarSize' and
    // '0 <= numElements'.  Note that if '*validFlag_p' is initially zero, this
    // function has no effect.  Also note that error messages are not printed
    // for insufficient data in 'buffer'.

    void checkScalar(int                 *validFlag_p,
                     bdex_FieldCode::Type code,
                     int                  scalarSize,
                     int                  quietFlag);
    // Verify the validity of the field code and the sufficiency of data at the
    // specified '*cursor_p' position in the specified 'buffer' of the
    // specified 'bufferLength'.  Extract the field code at the byte position
    // '*cursor_p' from 'buffer'.  If the field code does not correspond to the
    // specified 'code', then set the specified '*validFlag_p' to zero and, if
    // the specified 'quietFlag' is zero, print an error message.  Otherwise,
    // advance '*cursor_p' by the size of the field code, and verify that
    // 'buffer' contains sufficient bytes for the specified 'scalarSize'.  If
    // there are too few bytes, then set '*validFlag_p' to zero.  The behavior
    // is undefined unless '0 <= *cursor_p', '0 <= bufferLength' and
    // '0 < scalarSize'.  Note that if '*validFlag_p' is initially zero, this
    // function has no effect.  Also note that error messages are not printed
    // for insufficient data in 'buffer'.

    void checkVersion(int        *validFlag_p,
                      int         suppressVersionCheckFlag,
                      int         quietFlag);
    // Verify that the leading 'SIZEOF_CODE + SIZEOF_VERSION' bytes in the
    // specified 'buffer' of the specified 'bufferLength' constitute a valid
    // version code (field code and value).  Extract the field code for the
    // version at byte position 0.  If the field code is not equal to
    // 'bdex_FieldCode::INT8', then set the specified '*validFlag_p' to zero
    // and, if the specified 'quietFlag' is zero, print an error message.
    // Otherwise, extract the value of the version that follows the field code.
    // If version is less than or equal to zero, then set '*validFlag_p' to
    // zero and, if 'quietFlag' is zero, print an error message.  The behavior
    // is undefined unless '0 <= bufferLength'.  Note that if '*validFlag_p' is
    // initially zero or 'suppressVersionCheckFlag' is non-zero, this function
    // has no effect.  Also note that error messages are not printed for
    // insufficient data in 'buffer'.

    // NOT DEFINED
    bdex_TestInStreamFormatter(const bdex_TestInStreamFormatter&);
    bdex_TestInStreamFormatter& operator=(const bdex_TestInStreamFormatter&);

    bool operator==(const bdex_TestInStreamFormatter&) const;
    bool operator!=(const bdex_TestInStreamFormatter&) const;

  public:
    // CREATORS
    bdex_TestInStreamFormatter(bsl::streambuf *streamBuffer);
        // Create an input byte stream that reads bytes obtained from the
        // specified 'streamBuffer' and formatted by a corresponding
        // 'bdex_ByteOutStreamFormatter' object.

    ~bdex_TestInStreamFormatter();
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
        // Replace this stream's content with the specified initial 'numBytes'
        // from the specified 'buffer' and set the index of the next byte to be
        // extracted from this stream to 0 (i.e., the beginning of the stream).
        // Also validate this stream if it is currently invalid.  The behavior
        // is undefined unless '0 <= numBytes'.

    bdex_TestInStreamFormatter& getLength(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 8-bit unsigned integer or 32-bit signed integer value representing
        // a length (see the package group level documentation) into the
        // specified 'variable' if its type is appropriate, and return a
        // reference to this modifiable stream dependent on the most
        // significant bit of the next byte in the stream; if it is 0 consume
        // an 8-bit unsigned integer, otherwise a 32-bit signed integer after
        // setting this bit to zero.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variable' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getString(bsl::string& str);
        // Consume a string from this input stream, place that value in the
        // specified 'str', and return a reference to this modifiable stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // The string must be prefaced by a non-negative integer indicating
        // the number of characters composing the string.  Behavior is
        // undefined if the length indicator is not greater than 0.

    bdex_TestInStreamFormatter& getVersion(int& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer value representing a version (see the package group
        // level documentation) into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

                        // *** scalar integer values ***

    bdex_TestInStreamFormatter& getInt64(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint64(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 64-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt56(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint56(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 56-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt48(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint48(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 48-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt40(bsls_PlatformUtil::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint40(bsls_PlatformUtil::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 40-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt32(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 32-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint32(unsigned int& variable);
        // Verify the type of the next value in this stream, consume that
        // 32-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt24(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 24-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint24(unsigned int& variable);
        // Verify the type of the next value in this stream, consume that
        // 24-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt16(short& variable);
        // Verify the type of the next value in this stream, consume that
        // 16-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint16(unsigned short& variable);
        // Verify the type of the next value in this stream, consume that
        // 16-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getInt8(char& variable);
    bdex_TestInStreamFormatter& getInt8(signed char& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // signed integer value into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    bdex_TestInStreamFormatter& getUint8(char& variable);
    bdex_TestInStreamFormatter& getUint8(unsigned char& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer value into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

                        // *** scalar floating-point values ***

    bdex_TestInStreamFormatter& getFloat64(double& variable);
        // Verify the type of the next value in this stream, consume that IEEE
        // double-precision (8-byte) floating-point value into the specified
        // 'variable' if its type is appropriate, and return a reference to
        // this modifiable stream.  If the type is incorrect, then this stream
        // is marked invalid and the value of 'variable' is unchanged.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.  Note that
        // for non-conforming platforms, this operation may be lossy.

    bdex_TestInStreamFormatter& getFloat32(float& variable);
        // Verify the type of the next value in this stream, consume that IEEE
        // single-precision (4-byte) floating-point value into the specified
        // 'variable' if its type is appropriate, and return a reference to
        // this modifiable stream.  If the type is incorrect, then this stream
        // is marked invalid and the value of 'variable' is unchanged.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.  Note that
        // for non-conforming platforms, this operation may be lossy.

                        // *** arrayed integer values ***

    bdex_TestInStreamFormatter&
    getArrayInt64(bsls_PlatformUtil::Int64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayUint64(bsls_PlatformUtil::Uint64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayInt56(bsls_PlatformUtil::Int64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayUint56(bsls_PlatformUtil::Uint64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayInt48(bsls_PlatformUtil::Int64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayUint48(bsls_PlatformUtil::Uint64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayInt40(bsls_PlatformUtil::Int64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayUint40(bsls_PlatformUtil::Uint64 *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayInt32(int *array, int  length);
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

    bdex_TestInStreamFormatter&
    getArrayUint32(unsigned int *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayInt24(int *array, int  length);
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

    bdex_TestInStreamFormatter&
    getArrayUint24(unsigned int *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayInt16(short *array, int length);
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

    bdex_TestInStreamFormatter&
    getArrayUint16(unsigned short *array, int length);
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

    bdex_TestInStreamFormatter& getArrayInt8(char *array, int   length);
    bdex_TestInStreamFormatter& getArrayInt8(signed char *array, int length);
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

    bdex_TestInStreamFormatter& getArrayUint8(char *array, int length);
    bdex_TestInStreamFormatter&
    getArrayUint8(unsigned char *array, int length);
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

                        // *** arrayed floating-point values ***

    bdex_TestInStreamFormatter& getArrayFloat64(double *array, int length);
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

    bdex_TestInStreamFormatter& getArrayFloat32(float *array, int length);
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

    int length() const;
        // Return the total number of bytes stored in this stream.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.

    int isQuiet() const;
        // Return 1 if this stream's quiet mode is ON, and 0 otherwise.

    int isSuppressVersionCheck() const;
        // Return 1 if this stream's version-checking mode is OFF, and 0
        // otherwise.

    int inputLimit() const;
        // Return the current number of input requests left before an exception
        // is thrown.  A negative value indicated that no exception is
        // scheduled.
};

// FREE FUNCTIONS
// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename VALUE>
inline
bdex_TestInStreamFormatter& operator>>(bdex_TestInStreamFormatter& stream,
                                       VALUE&                      value);
    // Read from the specified 'stream' a 'bdex' version and assign to the
    // specified 'bdex'-compliant 'value' the value read from 'stream' using
    // 'VALUE's 'bdex' version format as read from 'stream'.  Return a
    // reference to the modifiable 'stream'.  If 'stream' is initially invalid,
    // this operation has no effect.  If 'stream' becomes invalid during this
    // operation, 'value' is valid, but its value is undefined.  If the read
    // in version is not supported by 'value', 'stream' is marked invalid,
    // but 'value' is unaltered.  The behavior is undefined unless the data in
    // 'stream' was written by a 'bdex_ByteOutStream' object.  (See the 'bdex'
    // package-level documentation for more information on 'bdex' streaming of
    // container types.)  Note that specializations for the supported
    // fundamental (scalar) types and for 'bsl::string' are also provided, in
    // which case this operator calls the appropriate 'get' method from this
    // component.

// TBD #endif

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------------
                        // class bdex_TestInStreamFormatter
                        // --------------------------------

// MANIPULATORS
inline
void bdex_TestInStreamFormatter::setQuiet(int flag)
{
    d_quietFlag = !!flag;
}

inline
void bdex_TestInStreamFormatter::setSuppressVersionCheck(int flag)
{
    d_suppressVersionCheckFlag = !!flag;
}

inline
void bdex_TestInStreamFormatter::setInputLimit(int limit)
{
    d_inputLimit = limit;
}

inline
void bdex_TestInStreamFormatter::invalidate()
{
    d_validFlag = 0;
}

inline
void bdex_TestInStreamFormatter::seek(int offset)
{
    BSLS_ASSERT_SAFE(0 <= offset);

    d_streamBuf->pubseekpos(offset);
    d_validFlag = 1;
}

inline
void bdex_TestInStreamFormatter::reset()
{
    d_validFlag = 1;
    d_streamBuf->pubseekpos(0);
}

inline
void bdex_TestInStreamFormatter::reload(const char *buffer, int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_validFlag = 1;
    d_streamBuf->pubsetbuf(const_cast<char *>(buffer), numBytes);
}

// ACCESSORS
inline
bdex_TestInStreamFormatter::operator const void *() const
{
    return (d_validFlag) ? this : 0;
}

inline
int bdex_TestInStreamFormatter::isQuiet() const
{
    return d_quietFlag;
}

inline
int bdex_TestInStreamFormatter::isSuppressVersionCheck() const
{
    return d_suppressVersionCheckFlag;
}

inline
int bdex_TestInStreamFormatter::inputLimit() const
{
    return d_inputLimit;
}

// FREE OPERATORS
// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

template <typename VALUE>
inline
bdex_TestInStreamFormatter& operator>>(bdex_TestInStreamFormatter& stream,
                                       VALUE&                      value)
{
    return bdex_InStreamFunctions::streamInVersionAndObject(stream, value);
}

// TBD #endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
