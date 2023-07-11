// bdlde_hexdecoder.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLDE_HEXDECODER
#define INCLUDED_BDLDE_HEXDECODER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide automata converting from hex encodings.
//
//@CLASSES:
//  bdlde::HexDecoder: automata for hex decoding
//
//@SEE_ALSO: bdlde_hexencoder
//
//@DESCRIPTION: This component provides a class, 'bdlde::HexDecoder', for
// decoding hexadecimal representation into plain text.
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
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of 'bdlde::HexDecoder'
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example shows how to use a 'bdlde::HexDecoder' object to
// implement a function, 'streamDecoder', that reads hex representation from
// 'bsl::istream', decodes that text, and writes the decoded text to a
// 'bsl::ostream'.  'streamDecoder' returns 0 on success and a negative value
// if the input data could not be successfully decoded or if there is an I/O
// error.
//..
//  int streamDecoder(bsl::ostream& os, bsl::istream& is)
//      // Read the entire contents of the specified input stream 'is', convert
//      // the input hex encoding into plain text, and write the decoded text
//      // to the specified output stream 'os'.  Return 0 on success, and a
//      // negative value otherwise.
//  {
//      enum {
//          SUCCESS      =  0,
//          DECODE_ERROR = -1,
//          IO_ERROR     = -2
//      };
//..
// First we create an object, create buffers for storing data, and start loop
// that runs while the input stream contains some data:
//..
//      bdlde::HexDecoder converter;
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
//              int numOut = 0;
//              int numIn  = 0;
//..
// Convert obtained text using 'bdlde::HexDecoder':
//..
//              int status = converter.convert(
//                                       output,
//                                       &numOut,
//                                       &numIn,
//                                       input,
//                                       inputEnd,
//                                       static_cast<int>(outputEnd - output));
//              if (status < 0) {
//                  return DECODE_ERROR;                              // RETURN
//              }
//
//              output += numOut;
//              input  += numIn;
//..
// And write decoded text to the output stream:
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
//      if (output > outputBuffer) {
//          os.write (outputBuffer, output - outputBuffer);
//      }
//..
// Then we need to complete the work of our decoder:
//..
//      int more = converter.endConvert();
//      if (more < 0) {
//          return DECODE_ERROR;                                      // RETURN
//      }
//
//      return is.eof() && os.good() ? SUCCESS : IO_ERROR;
//  }
//..
// Next, to demonstrate how our function works we need to create a stream with
// encoded data.  Assume that we have some character string, 'BLOOMBERG_NEWS',
// and a function, 'streamEncoder' mirroring the work of the 'streamDecoder':
//..
//  bsl::istringstream inStream(bsl::string(BLOOMBERG_NEWS,
//                                          strlen(BLOOMBERG_NEWS)));
//  bsl::stringstream  outStream;
//  bsl::stringstream  backInStream;
//
//  assert(0 == streamEncoder(outStream,    inStream));
//..
// Now, we use our function to decode text:
//..
//  assert(0 == streamDecoder(backInStream, outStream));
//..
// Finally, we observe that the output fully matches the original text:
//..
//  assert(0 == strcmp(BLOOMBERG_NEWS, backInStream.str().c_str()));
//..

#include <bdlscm_version.h>

#include <bsls_assert.h>

#include <bsl_iterator.h>

namespace BloombergLP {
namespace bdlde {

class HexDecoder {
    // This class implements a mechanism capable of converting data of
    // arbitrary length from its corresponding Hex representation.

    // PRIVATE TYPES
    enum States {
        // Symbolic state values for the decoder.

        e_ERROR_STATE        = -1, // input is irreparably invalid
        e_INPUT_STATE        =  0, // general input state
        e_DONE_STATE         =  1  // any additional input is an error
    };

    // DATA
    int         d_state;         // current state of this object
    char        d_firstDigit;    // first (left) hex digit to decode
    int         d_outputLength;  // total number of output characters
    const char *d_decodeTable_p; // character code table

    // PRIVATE CLASS METHODS
    static bool isSpace(char character);
        // Return 'true' if the specified 'character' is whitespace (i.e.,
        // space, tab, CR, NL, VT, or FF), and 'false' otherwise.

    static bool isXdigit(char character);
        // Return 'true' if the specified 'character' is a hex digit, and
        // 'false' otherwise.

    // NOT IMPLEMENTED
    HexDecoder(const HexDecoder&);
    HexDecoder& operator=(const HexDecoder&);

  public:
    // CREATORS
    HexDecoder();
        // Create a Hex decoder in the initial state.

    // ~HexDecoder() = default;
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
        // Append to the buffer addressed by the specified 'out' all pending
        // output (if there is any) up to the optionally specified 'maxNumOut'
        // limit (default is negative, meaning no limit).  When there is no
        // pending output and the 'maxNumOut' is still not reached, begin to
        // consume and decode a sequence of input characters starting at the
        // specified 'begin' position, up to but not including the specified
        // 'end' position.  Any resulting output is written to the 'out' buffer
        // up to the (cumulative) 'maxNumOut' limit.  If 'maxNumOut' limit is
        // reached, no further input will be consumed.  Load into the
        // (optionally) specified 'numOut' and 'numIn' the number of output
        // bytes produced and input bytes consumed, respectively.  Return 0 on
        // success and a negative value otherwise.  Note that calling this
        // method after 'endConvert' has been invoked without an intervening
        // 'reset' call will place this instance in an error state, and return
        // an error status.

    int endConvert();
        // Terminate encoding for this decoder.  Return 0 on success, and a
        // negative value otherwise.

    void reset();
        // Reset this decoder to its initial state (i.e., as if no input had
        // been consumed).

    // ACCESSORS
    bool isAcceptable() const;
        // Return 'true' if the input read so far by this decoder is considered
        // syntactically complete and all resulting output has been emitted;
        // return 'false' otherwise.  Note that there must not be any
        // unprocessed characters accumulated in the input buffer of this
        // decoder.

    bool isDone() const;
        // Return 'true' if this decoder is in the done state (i.e.,
        // 'endConvert' has been called and any additional input will result in
        // an error), and if there is no pending output; return 'false'
        // otherwise.

    bool isError() const;
        // Return 'true' if this decoder has encountered an irrecoverable error
        // and 'false' otherwise.  An irrecoverable error is one for which
        // there is no subsequent possibility of achieving an "acceptable"
        // result (as defined by the 'isAcceptable' method).

    bool isInitialState() const;
        // Return 'true' if this decoder is in the initial state (i.e., as if
        // no input had been consumed) and 'false' otherwise.

    bool isMaximal() const;
        // Return 'true' if the input to this decoder is maximal (i.e., the
        // input contains an end-of-input sentinel, signaling that no further
        // input should be expected).  *Always* returns 'false' for Hex
        // decoders since the encoding scheme does not specify an end-of-input
        // sentinel.

    int outputLength() const;
        // Return the total length of the output emitted by this decoder
        // (possibly after several calls to the 'convert' or the 'input'
        // methods) since its initial construction or the latest 'reset'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// PRIVATE CLASS METHODS
inline
bool HexDecoder::isSpace(char character)
{
    static const bool k_SPACE_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
    };

    return k_SPACE_TABLE[static_cast<unsigned char>(character)];
}

inline
bool HexDecoder::isXdigit(char character)
{
    static const bool k_XDIGIT_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
    };

    return k_XDIGIT_TABLE[static_cast<unsigned char>(character)];
}

// MANIPULATORS
template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int HexDecoder::convert(OUTPUT_ITERATOR out,
                        INPUT_ITERATOR  begin,
                        INPUT_ITERATOR  end)
{
    int dummyNumOut;
    int dummyNumIn;

    return convert(out, &dummyNumOut, &dummyNumIn, begin, end, -1);
}

template <class OUTPUT_ITERATOR, class INPUT_ITERATOR>
int HexDecoder::convert(OUTPUT_ITERATOR  out,
                        int             *numOut,
                        int             *numIn,
                        INPUT_ITERATOR   begin,
                        INPUT_ITERATOR   end,
                        int              maxNumOut)
{
    BSLS_ASSERT(numOut);
    BSLS_ASSERT(numIn);

    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state) {
        int rv  = e_DONE_STATE == d_state ? -2 : -1;
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn  = 0;
        return rv;                                                    // RETURN
    }

    if (0 == maxNumOut) {
        *numOut = 0;
        *numIn = 0;
        return 0;                                                     // RETURN
    }

    INPUT_ITERATOR originalBegin = begin;
    int            numEmitted = 0;

    while (begin != end && numEmitted != maxNumOut) {
        const char digit = static_cast<char>(*begin);
        ++begin;

        if (!isSpace(digit)) {
            if (!isXdigit(digit)) {
                *numOut = numEmitted;
                d_outputLength += numEmitted;
                *numIn = static_cast<int>(bsl::distance(originalBegin, begin));
                d_state = e_ERROR_STATE;
                return -1;                                            // RETURN
            }

            if (0 == d_firstDigit) {
                d_firstDigit = digit;
            }
            else {
                char value = static_cast<char>(
                       (d_decodeTable_p[static_cast<int>(d_firstDigit)] << 4) |
                       (d_decodeTable_p[static_cast<int>(digit       )]));
                *out = value;

                ++out;
                ++numEmitted;
                d_firstDigit = 0;
            }
        }
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;
    *numIn = static_cast<int>(bsl::distance(originalBegin, begin));
    return 0;
}

inline
void HexDecoder::reset()
{
    d_state = e_INPUT_STATE;
    d_firstDigit = 0;
    d_outputLength = 0;
}

// ACCESSORS
inline
bool HexDecoder::isAcceptable() const
{
    return e_INPUT_STATE == d_state && !d_firstDigit;
}

inline
bool HexDecoder::isDone() const
{
    return e_DONE_STATE == d_state;
}

inline
bool HexDecoder::isError() const
{
    return e_ERROR_STATE == d_state;
}

inline
bool HexDecoder::isInitialState() const
{
    return e_INPUT_STATE == d_state && 0 == d_outputLength && !d_firstDigit;
}

inline
bool HexDecoder::isMaximal() const
{
    return false;
}

inline
int HexDecoder::outputLength() const
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
