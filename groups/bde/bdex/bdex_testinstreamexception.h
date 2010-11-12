// bdex_testinstreamexception.h                                       -*-C++-*-
#ifndef INCLUDED_BDEX_TESTINSTREAMEXCEPTION
#define INCLUDED_BDEX_TESTINSTREAMEXCEPTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide exception class for (un)externalization operations.
//
//@CLASSES:
//   bdex_TestInStreamException: exception containing unexternalization info
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: 'bdex_testinstream'
//
//@DESCRIPTION: This component defines a simple exception object for testing
// exceptions during unexternalization operations.  The exception object
// 'bdex_TestInStreamException' contains information about the input request,
// which can be queried by the "catcher" of this exception.
//
///Usage
///-----
// In the following example, the 'bdex_TestInStreamException' object is thrown
// by one of the input methods of the 'my_TestInStream' object after the number
// of requests exceeds the stream's input limit.  This example demonstrates how
// to use a user-defined input data stream (e.g., 'my_TestInStream') and
// 'bdex_TestInStreamException' to verify that an object under test (e.g.,
// 'my_ShortArray') is exception neutral during the stream-in operation:
//..
//  // my_testinstream.h
//  #include <bdex_instream.h>
//
//  class my_TestInStream : public bdex_InStream {
//      int   d_cursor;      // index for the next byte to be extracted
//      char *d_buffer;      // stores byte values to be unexternalized
//      int   d_length;      // number of bytes in 'd_buffer'
//      int   d_validFlag;   // flag to indicate this stream's validity
//      int   d_inputLimit;  // number of input op's before exception
//
//      // CREATORS
//      my_TestInStream(const char *buffer, int numBytes);
//      ~my_TestInStream();
//      // ...
//
//      // MANIPULATORS
//      void invalidate()             { d_validFlag = 0;      }
//      void setInputLimit(int limit) { d_inputLimit = limit; }
//      void seek(int offset)         { d_cursor = offset;    }
//      void reset()                  { d_cursor = 0;         }
//
//      my_TestInStream& getInt64(bsls_PlatformUtil::Int64& variable);
//      my_TestInStream& getUint64(bsls_PlatformUtil::Uint64& variable);
//      // ...
//
//      // ACCESSORS
//      int inputLimit() const        { return d_inputLimit;           }
//      int cursor() const            { return d_cursor;               }
//      // ...
//  };
//  // ...
//..
// In the implementation of each 'my_TestInStream' input method, the input
// limit of the stream object is first examined before performing the input
// operation.  If the input limit is non-negative, it is decremented.  If after
// this decrement the input limit becomes negative, an exception object
// 'bdex_TestInStreamException' is created and initialized with the input data
// type code, and then the exception is thrown.
//..
//  // my_testinstream.cpp
//
//  // ...
//
//  my_TestInStream&
//  my_TestInStream::getUint64(bsls_PlatformUtil::Uint64& variable)
//  {
//      bdex_FieldCode::Type code = bdex_FieldCode::UINT64;
//  #ifdef BDE_BUILD_TARGET_EXC
//      if (0 <= d_inputLimit) {
//          --d_inputLimit;
//          if (0 > d_inputLimit) {
//              throw bdex_TestInStreamException(code);
//          }
//      }
//  #endif
//      if (d_validFlag) {
//          bdex_ByteStreamImpUtil::getUint64(&variable, &d_buffer[d_cursor]);
//          d_cursor += SIZEOF_INT64;
//      }
//      return *this;
//  }
//..
// Note that the symbol 'BDE_BUILD_TARGET_EXC' is defined at compile-time to
// indicate whether exceptions are enabled.  In the above code, if exceptions
// are not enabled, the code that throws 'bdex_TestInStreamException' is
// never executed.  The following is the test driver for 'my_ShortArray'.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment").
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//  #include <my_testinstream.h>
//  #include <bdex_testinstreamexception.h>
//
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
//  static
//  bool areEqual(const short *array1, const short *array2, int numElement)
//      // Return 'true' if the specified initial 'numElement' in the specified
//      // 'array1' and 'array2' have the same values, and 'false' otherwise.
//  {
//      for (int i = 0; i < numElement; ++i) {
//          if (array1[i] != array2[i]) return 0;
//      }
//      return 1;
//  }
//
//  int main(int argc, char *argv[]) {
//      int test = argc > 1 ? atoi(argv[1]) : 0;
//      int verbose = argc > 2;
//      int veryVerbose = argc > 3;
//      int veryVeryVerbose = argc > 4;
//
//      switch (test) { case 0:
//
//        // ...
//
//        case 6: {
//
//          const int MAX_BYTES = 1000;
//          struct {
//              int   d_line;
//              int   d_numBytes;         // number of bytes in input stream
//              char  d_bytes[MAX_BYTES]; // data in bytes
//              int   d_expNumElem;       // expected number of elements
//              short d_exp[NUM_VALUES];  // expected element values
//          } DATA[] = {
//     //Line  # bytes  bytes                   # elem  expected array
//     //----  -------  ----------------------  ------  --------------
//      { L_,     5,    "\x01\x00\x00\x00\x00",    0,   {}                   },
//      { L_,    11,    "\x01\x00\x00\x00\x01"
//                      "\x00\x01",                1,   { V0 }               },
//      { L_,    19,    "\x01\x00\x00\x00\x05"
//                      "\x00\x01\x00\x02"
//                      "\x00\x03\x00\x04"
//                      "\xff\xfb",                5,   { V0, V1, V2, V3, V4 }}
//          };
//
//          const int NUM_TEST = sizeof DATA / sizeof *DATA;
//
//          for (int ti = 0; ti < NUM_TEST; ++ti) {
//              const int    LINE      = DATA[ti].d_line;
//              const int    NUM_BYTES = DATA[ti].d_numBytes;
//              const char  *BYTES     = DATA[ti].d_bytes;
//              const int    NUM_ELEM  = DATA[ti].d_expNumElem;
//              const short *EXP       = DATA[ti].d_exp;
//
//              my_TestInStream testInStream(BYTES, NUM_BYTES);
//
//              BEGIN_BDEX_EXCEPTION_TEST { testInStream.reset();
//                  my_ShortArray mA;  const my_ShortArray& A = mA;
//                  mA.streamIn(testInStream);
//                  if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
//                  LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
//              } END_BDEX_EXCEPTION_TEST
//          }
//        } break;
//
//        // ...
//
//      }
//
//      // ...
//  }
//..
// In the above sample code, the macros 'BEGIN_BDEX_EXCEPTION_TEST' and
// 'END_BDEX_EXCEPTION_TEST' are defined so that they can be easily inserted to
// encompass a section of code to test exception neutrality during the
// stream-in operation.  The macros assume that a 'my_TestInStream' object
// named 'testInStream' exists and is initialized with data.  When placed
// around a section of code, the macros initially set the input limit of
// 'testInStream' to 0 and then executes the code.  If an exception is thrown
// during execution, the macros increment the stream's input limit by one and
// re-execute the code.  Note that the 'reset' method of 'testInStream' is
// invoked at the start of the code being tested to ensure that the stream is
// reset to the beginning of its data for every iteration over the code.  This
// process is repeated an indefinite number of times until no exception is
// thrown.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_FIELDCODE
#include <bdex_fieldcode.h>
#endif

namespace BloombergLP {

                        // ================================
                        // class bdex_TestInStreamException
                        // ================================

class bdex_TestInStreamException {
    // This class defines an exception object for unexternalization operations.
    // Instances of this class contain information about the unexternalization
    // request.

    bdex_FieldCode::Type d_dataType; // type of the input data requested

  public:
    // CREATORS
    bdex_TestInStreamException(bdex_FieldCode::Type type);
        // Create an exception object initialized with the specified 'type' of
        // the requested input data.

    ~bdex_TestInStreamException();
        // Destroy this object.

    // ACCESSORS
    bdex_FieldCode::Type dataType() const;
        // Return the type code (supplied at construction) of the requested
        // input data.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                                // --------
                                // CREATORS
                                // --------

inline
bdex_TestInStreamException::bdex_TestInStreamException(
                                                     bdex_FieldCode::Type type)
: d_dataType(type)
{
}

inline
bdex_TestInStreamException::~bdex_TestInStreamException()
{
}

                                // ---------
                                // ACCESSORS
                                // ---------

inline
bdex_FieldCode::Type bdex_TestInStreamException::dataType() const
{
    return d_dataType;
}

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
