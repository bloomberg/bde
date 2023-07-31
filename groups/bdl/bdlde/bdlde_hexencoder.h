// bdlde_hexencoder.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLDE_HEXENCODER
#define INCLUDED_BDLDE_HEXENCODER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mechanism for encoding text into hexadecimal.
//
//@CLASSES:
//  bdlde::HexEncoder: mechanism for encoding text into hexadecimal
//
//@SEE_ALSO: bdlde_hexdecoder
//
//@DESCRIPTION: This component provides a class, 'bdlde::HexEncoder', for
// encoding plain text into its hexadecimal representation.
//
// 'bdlde::HexEncoder' and 'bdlde::HexDecoder' provide a pair of template
// functions (each parameterized separately on both input and output iterators)
// that can be used respectively to encode and to decode byte sequences of
// arbitrary length into and from the printable Hex representation.
//
// Each instance of either the encoder or decoder retains the state of the
// conversion from one supplied input to the next, enabling the processing of
// segmented input -- i.e., processing resumes where it left off with the next
// invocation on new input.  Instance methods are provided for both the
// encoder and decoder to (1) assert the end of input, (2) determine whether
// the input so far is currently acceptable, and (3) indicate whether a
// non-recoverable error has occurred.
//
///Hex Encoding
///------------
// The data stream is processed one byte at a time from left to right.  Each
// byte
//..
//      7 6 5 4 3 2 1 0
//     +-+-+-+-+-+-+-+-+
//     |               |
//     +-+-+-+-+-+-+-+-+
//      `------v------'
//            Byte
//..
// is segmented into two intermediate 4-bit quantities.
//..
//      3 2 1 0 3 2 1 0
//     +-+-+-+-+-+-+-+-+
//     |       |       |
//     +-+-+-+-+-+-+-+-+
//      `--v--' `--v--'
//       char0   char1
//..
// Each 4-bit quantity is in turn used as an index into the following character
// table to generate an 8-bit character.
//..
//     =================
//     *  Hex Alphabet *
//     -----------------
//     Val Enc  Val Enc
//     --- ---  --- ---
//       0 '0'    8 '8'
//       1 '1'    9 '9'
//       2 '2'   10 'A'
//       3 '3'   11 'B'
//       4 '4'   12 'C'
//       5 '5'   13 'D'
//       6 '6'   14 'E'
//       7 '7'   15 'F'
//     =================
//..
// Depending on the settings encoder represents values from 10 to 15 as
// uppercase ('A'-'F') or lowercase letters('a'-'f').
//
// Input values of increasing length along with their corresponding Hex
// encodings are illustrated below:
//..
//        Data: /* nothing */
//    Encoding: /* nothing */
//
//        Data: "0"     (0011 0000)
//    Encoding: 30
//
//        Data: "01"    (0011 0000 0011 0001)
//    Encoding: 3031
//
//        Data: "01A"   (0011 0000 0011 0001 1000 0001)
//    Encoding: 303141
//
//        Data: "01A?"  (0011 0000 0011 0001 1000 0001 0011 1111)
//    Encoding: 3031413F
//..
//
///Hex Decoding
///------------
// The data stream is processed two bytes at a time from left to right.  Each
// sequence of two 8-bit quantities
//..
//      7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |               |               |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      `------v------' `------v------'
//           Byte0           Byte1
//..
// is segmented into four intermediate 4-bit quantities.
//..
//      3 2 1 0 3 2 1 0 3 2 1 0 3 2 1 0
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |       |       |       |       |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      `--v--' `--v--' `--v--' `--v--'
//      chunk0   chunk1  chunk2  chunk3
//..
// The second and forth chunks are combined to get the resulting 8-bit
// character.
//
// Whitespace characters are ignored.  On any non-alphabet character the
// decoder reports an error.  In order for a Hex encoding to be valid the
// length of the input data (excluding any whitespace characters) must be a
// multiple of two.
//
// Input values of increasing length along with their corresponding Hex
// encodings are illustrated below (note that the encoded whitespace character
// is skipped and the resulting string does not contain it):
//..
//        Data: /* nothing */
//    Encoding: /* nothing */
//
//        Data: "4"       (0000 0100)
//    Encoding: /* nothing */
//
//        Data: "41"      (0000 0100 0000 0001)
//    Encoding: A
//
//        Data: "412"     (0000 0100 0000 0001 0000 0010)
//    Encoding: A
//
//        Data: "4120"    (0000 0100 0000 0001 0000 0010 0000 0000)
//    Encoding: A
//
//        Data: "41203"   (0000 0100 0000 0001 0000 0010 0000 0000
//                         0000 0011)
//    Encoding: A
//
//        Data: "41203F"  (0011 0000 0011 0001 1000 0001 0010 0011
//                         0000 0011 0000 1111)
//    Encoding: A?
//..
//
///Usage
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of 'bdlde::HexEncoder'
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example shows using a 'bdlde::HexEncoder' object to encode
// bytes into a hexidecimal format. For dependency reasons, a more complete
// example, showing both encoding and decoding can be found in
// 'bdlde_hexdecoder'.
//
// In the example below, we implement a function 'streamEncoder', that reads
// text from 'bsl::istream', encodes that text into hex representation, and
// writes the encoded text to a 'bsl::ostream'.  'streamEncoder' returns 0 on
// success and a negative value if the input data could not be successfully
// encoded or if there is an I/O  error.
//..
//  int streamEncoder(bsl::ostream& os, bsl::istream& is)
//      // Read the entire contents of the specified input stream 'is', convert
//      // the input plain text to hex representation, and write the encoded
//      // text to the specified output stream 'os'.  Return 0 on success, and
//      // a negative value otherwise.
//  {
//      enum {
//          SUCCESS      =  0,
//          ENCODE_ERROR = -1,
//          IO_ERROR     = -2
//      };
//..
// First we create an object, create buffers for storing data, and start loop
// that runs while the input stream contains some data:
//..
//      bdlde::HexEncoder converter;
//
//      const int INBUFFER_SIZE  = 1 << 10;
//      const int OUTBUFFER_SIZE = 1 << 10;
//
//      char inputBuffer[INBUFFER_SIZE];
//      char outputBuffer[OUTBUFFER_SIZE];
//
//      char *output    = outputBuffer;
//      char *outputEnd = outputBuffer + sizeof outputBuffer;
//
//      while (is.good()) {  // input stream not exhausted
//..
// On each iteration we read some data from the input stream:
//..
//          is.read(inputBuffer, sizeof inputBuffer);
//
//          const char *input    = inputBuffer;
//          const char *inputEnd = input + is.gcount();
//
//          while (input < inputEnd) { // input encoding not complete
//
//              int numOut;
//              int numIn;
//..
// Convert obtained text using 'bdlde::HexEncoder':
//..
//              int status = converter.convert(
//                                       output,
//                                       &numOut,
//                                       &numIn,
//                                       input,
//                                       inputEnd,
//                                       static_cast<int>(outputEnd - output));
//              if (status < 0) {
//                  return ENCODE_ERROR;                              // RETURN
//              }
//
//              output += numOut;
//              input  += numIn;
//..
// And write encoded text to the output stream:
//..
//              if (output == outputEnd) {  // output buffer full; write data
//                  os.write(outputBuffer, sizeof outputBuffer);
//                  if (os.fail()) {
//                      return IO_ERROR;                              // RETURN
//                  }
//                  output = outputBuffer;
//              }
//          }
//      }
//
//      while (1) {
//          int numOut = 0;
//..
// Then, we need to store the unhandled symbol (if there is one) to the output
// buffer and complete the work of our encoder:
//..
//          int more = converter.endConvert(
//                                      output,
//                                      &numOut,
//                                      static_cast<int>(outputEnd - output));
//          if (more < 0) {
//              return ENCODE_ERROR;                                  // RETURN
//          }
//
//          output += numOut;
//
//          if (!more) { // no more output
//              break;
//          }
//
//          assert(output == outputEnd);  // output buffer is full
//
//          os.write(outputBuffer, sizeof outputBuffer);  // write buffer
//          if (os.fail()) {
//              return IO_ERROR;                                      // RETURN
//          }
//          output = outputBuffer;
//      }
//
//      if (output > outputBuffer) {
//          os.write(outputBuffer, output - outputBuffer);
//      }
//
//      return is.eof() && os.good() ? SUCCESS : IO_ERROR;
//  }
//..
// Next, to demonstrate how our function works we need to create a stream with
// data to encode.  Assume that we have some character buffer,
// 'BLOOMBERG_NEWS', and a function, 'streamDecoder' mirroring the work of the
// 'streamEncoder'.  Below we should encode this string into a hexidecimal
// format:
//..
//  bsl::istringstream inStream(bsl::string(BLOOMBERG_NEWS,
//                                          strlen(BLOOMBERG_NEWS)));
//  bsl::stringstream  outStream;
//  bsl::stringstream  backInStream;
//..
// Then, we use our function to encode text:
//..
//  assert(0 == streamEncoder(outStream, inStream));
//..
// This example does *not* decode the resulting hexidecimal text, for a
// more complete example, see 'bdlde_hexdecoder'.

#include <bdlscm_version.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlde {

                       // ================
                       // class HexEncoder
                       // ================

class HexEncoder {
    // This class implements a mechanism capable of converting data of
    // arbitrary length to its corresponding Hex representation.

    // PRIVATE TYPES
    enum States {
        // Symbolic state values for the encoder

        e_ERROR_STATE   = -1,  // input is irreparably invalid
        e_INPUT_STATE   =  0,  // general input state
        e_DONE_STATE    =  1   // any additional input is error
    };

    // DATA
    int         d_state;          // current state of this object

    char        d_deferred;       // retained output character

    int         d_outputLength;   // total number of output characters

    bool        d_upperCaseFlag;  // flag to indicate if uppercase letters are
                                  // used

    const char *d_encodeTable_p;  // hexadecimal alphabet


    // NOT IMPLEMENTED
    HexEncoder(const HexEncoder&);
    HexEncoder& operator=(const HexEncoder&);

  public:
    // CREATORS
    explicit HexEncoder(bool upperCaseLetters = true);
        // Create a Hex encoder in the initial state.  Optionally specify the
        // 'upperCaseLetters' to indicate if values from 10 to 15 are encoded
        // as uppercase letters('A'-'F') or as lowercase letters('a'-'f').

    // ~HexEncoder() = default;
        // Destroy this object.

    // MANIPULATORS
    template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
    int convert(OUTPUT_ITERATOR out,
                INPUT_ITERATOR  begin,
                INPUT_ITERATOR  end);
    template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
    int convert(OUTPUT_ITERATOR  out,
                int             *numOut,
                int             *numIn,
                INPUT_ITERATOR   begin,
                INPUT_ITERATOR   end,
                int              maxNumOut = -1);
        // Append to the buffer addressed by the specified 'out' pending
        // character (if there is such) up to the optionally specified
        // 'maxNumOut' limit (default is negative, meaning no limit).  When
        // there is no pending output and 'maxNumOut' is still not reached,
        // begin to consume and encode a sequence of input characters starting
        // at the specified 'begin' position, up to but not including the
        // specified 'end' position.  Any resulting output is written to the
        // 'out' buffer up to the (cumulative) 'maxNumOut' limit.  If
        // 'maxNumOut' limit is reached, no further input will be consumed.
        // Load into the (optionally) specified 'numOut' and 'numIn' the number
        // of output bytes produced and input bytes consumed, respectively.
        // Return a non-negative value on success and a negative value
        // otherwise.  A successful return status indicates the number of
        // characters that would be output if 'endConvert' were called
        // subsequently with no output limit.  These bytes *may* be available
        // for output if this method is called with a sufficiently large
        // 'maxNumOut'.  Note that calling this method after 'endConvert' has
        // been invoked without an intervening 'reset' call will place this
        // instance in an error state, and return an error status.  Note also
        // that it is recommended that after all calls to 'convert' are
        // finished, the 'endConvert' method be called to complete the encoding
        // of any unprocessed input characters.

    template <class OUTPUT_ITERATOR>
    int endConvert(OUTPUT_ITERATOR out);
    template <class OUTPUT_ITERATOR>
    int endConvert(OUTPUT_ITERATOR out, int *numOut, int maxNumOut = -1);
        // Terminate encoding for this encoder; write any retained output
        // (e.g., from a previous call to 'convert' with a non-zero output
        // limit argument) to the specified 'out' buffer.  Optionally specify
        // the 'maxNumOut' limit on the number of bytes to output; if
        // 'maxNumOut' is negative, no limit is imposed.  Load into the
        // (optionally) specified 'numOut' the number of output bytes produced.
        // Return a non-negative value on success and a negative value
        // otherwise.  A successful return status indicates the number of
        // characters that would be output if 'endConvert' were called
        // subsequently with no output limit.  Any retained bytes are available
        // on a subsequent call to 'endConvert'.  Once this method is called,
        // no additional input may be supplied without an intervening call to
        // 'reset'; once this method returns a zero status, a subsequent call
        // will place this encoder in the error state, and return an error
        // status.

    void reset();
        // Reset this encoder to its initial state (i.e., as if no input had
        // been consumed).

    // ACCESSORS
    bool isAcceptable() const;
        // Return 'true' if the input read so far by this encoder is considered
        // syntactically complete, and 'false' otherwise.

    bool isDone() const;
        // Return 'true' if this encoder is in the done state (i.e.,
        // 'endConvert' has been called and any additional input will result in
        // an error), and if there is no pending output, and 'false' otherwise.

    bool isError() const;
        // Return 'true' if there is no possibility of achieving an
        // "acceptable" result, and 'false' otherwise.  Note that for an
        // encoder, no input can cause an error; the possible errors result
        // either from a call to the 'convert' method after the 'endConvert'
        // method is called the first time, or from a call to the 'endConvert'
        // method after the 'endConvert' method has returned successfully.

    bool isInitialState() const;
        // Return 'true' if this encoder is in the initial state (i.e., as if
        // no input had been consumed), and 'false' otherwise.

    bool isUpperCase() const;
        // Return 'true' if this encoder represents values from 10 to 15 as
        // uppercase letters('A'-'F'), and 'false' if these values are
        // represented as lowercase letters('a'-'f').

    int numOutputPending() const;
        // Return the number of characters that would be output if 'endConvert'
        // were called with no output limit.

    int outputLength() const;
        // Return the total length of the output emitted by this encoder
        // (possibly after one or more calls to the 'convert' or the 'input'
        // methods) since its initial construction or the latest 'reset'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// MANIPULATORS
template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int HexEncoder::convert(OUTPUT_ITERATOR out,
                        INPUT_ITERATOR  begin,
                        INPUT_ITERATOR  end)
{
    int dummyNumOut;
    int dummyNumIn;

    return convert(out, &dummyNumOut, &dummyNumIn, begin, end, -1);
}

template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int HexEncoder::convert(OUTPUT_ITERATOR  out,
                        int             *numOut,
                        int             *numIn,
                        INPUT_ITERATOR   begin,
                        INPUT_ITERATOR   end,
                        int              maxNumOut)
{
    BSLS_ASSERT(numOut);
    BSLS_ASSERT(numIn);

    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state) {
        int rv = e_DONE_STATE == d_state ? -2 : -1;
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn = 0;
        return rv;                                                    // RETURN
    }

    if (0 == maxNumOut) {
        *numOut = 0;
        *numIn = 0;
        return 0;                                                     // RETURN
    }

    int numConsumed = 0;
    int numEmitted = 0;

    // First we need to output pending symbol left over from the previous call.

    if (d_deferred) {
        *out = d_deferred;
        ++out;
        ++numEmitted;
        d_deferred = 0;
    }

    // Then we can handle new input.

    while (begin != end && numEmitted != maxNumOut) {
        if (d_deferred) {
            *out = d_deferred;
            ++out;
            ++numEmitted;
            d_deferred = 0;
            ++begin;
        }
        else {
            const char digit = static_cast<char>(*begin);
            ++numConsumed;

            *out = d_encodeTable_p[(digit >> 4) & 0x0f];
            ++out;
            ++numEmitted;
            d_deferred = d_encodeTable_p[digit & 0x0f];
        }
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;
    *numIn = numConsumed;
    return d_deferred ? 1 : 0;
}

template <class OUTPUT_ITERATOR>
int HexEncoder::endConvert(OUTPUT_ITERATOR out)
{
    int dummyNumOut;

    return endConvert(out, &dummyNumOut, -1);
}

template <class OUTPUT_ITERATOR>
int HexEncoder::endConvert(OUTPUT_ITERATOR out, int *numOut, int maxNumOut)
{
    BSLS_ASSERT(numOut);

    if (e_ERROR_STATE == d_state) {
        return -1;                                                    // RETURN
    }

    if (e_DONE_STATE == d_state && !d_deferred) {
        d_state = e_ERROR_STATE;
        return -1;                                                    // RETURN
    }

    d_state = e_DONE_STATE;

    if (d_deferred) {
        if (0 == maxNumOut) {
            return 1;                                                 // RETURN
        }
        else {
            *out = d_deferred;
            *numOut = 1;
            d_deferred = 0;
            d_outputLength++;
        }
    }

    return 0;
}

inline
void HexEncoder::reset()
{
    d_state = e_INPUT_STATE;
    d_deferred = 0;
    d_outputLength = 0;
}

// ACCESSORS
inline
bool HexEncoder::isAcceptable() const
{
    return e_INPUT_STATE == d_state && 0 == d_deferred;
}

inline
bool HexEncoder::isDone() const
{
    return e_DONE_STATE == d_state && 0 == d_deferred;
}

inline
bool HexEncoder::isError() const
{
    return e_ERROR_STATE == d_state;
}

inline
bool HexEncoder::isInitialState() const
{
    return e_INPUT_STATE == d_state && 0 == d_outputLength;
}

inline
bool HexEncoder::isUpperCase() const
{
    return d_upperCaseFlag;
}

inline
int HexEncoder::numOutputPending() const
{
    return d_deferred ? 1 : 0;
}

inline
int HexEncoder::outputLength() const
{
    return d_outputLength;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
