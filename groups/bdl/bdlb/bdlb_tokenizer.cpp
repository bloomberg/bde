// bdlb_tokenizer.cpp                                                 -*-C++-*-
#include <bdlb_tokenizer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_tokenizer_cpp,"$Id$ $CSID$")

namespace {

// The character inputs break down into the following types:
// Note that end-of-line is not an input type and handled separately
enum InputType {
    TOK = 0,  // token character
    SFT = 1,  // soft delimiter character
    HRD = 2,  // hard delimiter character
};
const int NUM_INPUTS = 3;

// The following defines the set of tokenizer state machine states
enum State {
    TOKEN = 0,  // Zero or more token characters seen in the input. No hard
                // or soft delimiter characters seen. Start state.
    DHEAD = 1,  // One or more soft delimiter characters seen after zero or
                // more token characters have been seen.
    DTAIL = 2,  // Exactly one hard delimiter character seen possibly
                // followed by zero or more soft delimiter characters.
};
const int NUM_STATES = 3;


// The following defines the set of tokenizer state machine actions:
enum Action {
    ACC_TOKEN = 0,  // accumulate token
    ACC_DELIM = 1,  // accumulate delimiter
    STOP      = 2,  // stop parsing state machine and return result
};

const State stateTable[NUM_INPUTS][NUM_STATES] = {
    // TOKEN   DHEAD   DTAIL
    // -----   -----   -----
    {  TOKEN,  TOKEN,  TOKEN },  // TOK
    {  DHEAD,  DHEAD,  DTAIL },  // SFT
    {  DTAIL,  DTAIL,  DTAIL },  // HRD
};

const Action actionTable[NUM_INPUTS][NUM_STATES] = {
    // TOKEN       DHEAD       DTAIL
    // ---------   ---------   ---------
    {  ACC_TOKEN,  STOP,       STOP      },  // TOK
    {  ACC_DELIM,  ACC_DELIM,  ACC_DELIM },  // SFT
    {  ACC_DELIM,  ACC_DELIM,  STOP      },  // HRD
};

}  // close unnamed namespace

namespace BloombergLP {

namespace bdlb {

                        // --------------------
                        // class Tokenizer_Data
                        // --------------------

Tokenizer_Data::Tokenizer_Data(const bslstl::StringRef& softDelimiters)
{
    bsl::memset(d_charTypes, TOK, k_MAX_CHARS);

    for(bslstl::StringRef::const_iterator it = softDelimiters.begin();
            it != softDelimiters.end();
            ++it) {
        // DUPLICATION TEST
        // ASSERT(d_charTypes[(unsigned char)*it] != TOK);
        // END OF DUPLICATION TEST
        d_charTypes[(unsigned char)*it] = SFT;
    }
}

Tokenizer_Data::Tokenizer_Data(const bslstl::StringRef& softDelimiters,
                               const bslstl::StringRef& hardDelimiters)
{
    bsl::memset(d_charTypes, TOK, k_MAX_CHARS);

    for(bslstl::StringRef::const_iterator it = softDelimiters.begin();
            it != softDelimiters.end();
            ++it) {
        // DUPLICATION TEST
        // ASSERT(d_charTypes[(unsigned char)*it] != TOK);
        // END OF DUPLICATION TEST
        d_charTypes[(unsigned char)*it] = SFT;
    }

    for(bslstl::StringRef::const_iterator it = hardDelimiters.begin();
            it != hardDelimiters.end();
            ++it) {
        // DUPLICATION TEST
        // ASSERT(d_charTypes[(unsigned char)*it] != TOK);
        // END OF DUPLICATION TEST
        d_charTypes[(unsigned char)*it] = HRD;
    }
}

                        // -----------------------
                        // class TokenizerIterator
                        // -----------------------

// CREATORS
TokenizerIterator::TokenizerIterator()
: d_sharedData_p(0)
, d_cursor_p(0)
, d_token_p(0)
, d_postDelim_p(0)
, d_end_p(0)
, d_endFlag(true)
{
    // Constructor for end iterator
}

TokenizerIterator::TokenizerIterator(const char           *input,
                                     const char           *end,
                                     const Tokenizer_Data *sharedData)
: d_sharedData_p(sharedData)
, d_cursor_p(input)
, d_token_p(input)
, d_postDelim_p(input)
, d_end_p(end)
, d_endFlag(false)
{
    // skip optional leader
    if (d_end_p) {
        while (d_end_p != d_cursor_p
               && SFT == d_sharedData_p->inputType(*d_cursor_p))
        {
            ++d_cursor_p;
        }
    } else {
        while (0 != *d_cursor_p
               && SFT == d_sharedData_p->inputType(*d_cursor_p))
        {
            ++d_cursor_p;
        }
    }

    ++*this;            // find first token
}

TokenizerIterator::TokenizerIterator(const TokenizerIterator&  other)
: d_sharedData_p(other.d_sharedData_p)
, d_cursor_p(other.d_cursor_p)
, d_token_p(other.d_token_p)
, d_postDelim_p(other.d_postDelim_p)
, d_end_p(other.d_end_p)
, d_endFlag(other.d_endFlag)
{
}

// MANIPULATORS
TokenizerIterator& TokenizerIterator::operator++()
{
    if (d_end_p) {
        d_token_p     = d_cursor_p;
        d_postDelim_p = d_cursor_p;

        if ( d_end_p == d_cursor_p ) {
            d_endFlag = true;
            return *this;                                             // RETURN
        }

        int currentState = TOKEN;
        int inputType;

        do {
            if (d_end_p == d_cursor_p) {
                return *this;                                         // RETURN
            }

            inputType = d_sharedData_p->inputType(*d_cursor_p);

            switch (actionTable[inputType][currentState]) {
              case ACC_TOKEN: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACC_DELIM: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case STOP: {       // stop parsing and return result
                return *this;                                         // RETURN
              } break;
            }

            currentState = stateTable[inputType][currentState];
            ++d_cursor_p;
        } while (true);
    } else {
        d_token_p     = d_cursor_p;
        d_postDelim_p = d_cursor_p;

        if ( 0 == *d_cursor_p ) {
            d_endFlag = true;
            return *this;                                             // RETURN
        }

        int currentState = TOKEN;
        int inputType;

        do {
            if (0 == *d_cursor_p) {
                return *this;                                         // RETURN
            }

            inputType = d_sharedData_p->inputType(*d_cursor_p);

            switch (actionTable[inputType][currentState]) {
              case ACC_TOKEN: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACC_DELIM: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case STOP: {       // stop parsing and return result
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = stateTable[inputType][currentState];
            ++d_cursor_p;
        } while (true);
    }

    BSLS_ASSERT(0);
}

                        // ---------------
                        // class Tokenizer
                        // ---------------
// CREATORS
Tokenizer::Tokenizer(const char               *input,
                     const bslstl::StringRef&  soft)
: d_sharedData(soft)
{
    BSLS_ASSERT(input);
    reset(input);
}

Tokenizer::Tokenizer(const char               *input,
                     const bslstl::StringRef&  soft,
                     const bslstl::StringRef&  hard)
: d_sharedData(soft, hard)
{
    BSLS_ASSERT(input);
    reset(input);
}

Tokenizer::Tokenizer(const bslstl::StringRef&  input,
                     const bslstl::StringRef&  soft)
: d_sharedData(soft)
{
    BSLS_ASSERT(input.begin());
    reset(input);
}

Tokenizer::Tokenizer(const bslstl::StringRef&  input,
                     const bslstl::StringRef&  soft,
                     const bslstl::StringRef&  hard)
: d_sharedData(soft, hard)
{
    BSLS_ASSERT(input.begin());
    reset(input);
}

Tokenizer::~Tokenizer()
{
}

Tokenizer& Tokenizer::operator++()
{
    // This function does not use slow isEos() for performance reasons
    if (d_end_p) {

        d_prevDelim_p = d_postDelim_p;  // current delimiter becomes previous
        d_token_p     = d_cursor_p;
        d_postDelim_p = d_cursor_p;

        if ( d_end_p == d_cursor_p ) {
            d_endFlag = true;
            return *this;                                             // RETURN
        }

        int currentState = TOKEN;
        int inputType;

        do {
            if (d_end_p == d_cursor_p) {
                return *this;                                         // RETURN
            }

            inputType = d_sharedData.inputType(*d_cursor_p);

            switch (actionTable[inputType][currentState]) {
              case ACC_TOKEN: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACC_DELIM: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case STOP: {       // stop parsing and return result
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = stateTable[inputType][currentState];
            ++d_cursor_p;
        } while (true);

    } else {

        d_prevDelim_p = d_postDelim_p;  // current delimiter becomes previous
        d_token_p     = d_cursor_p;
        d_postDelim_p = d_cursor_p;

        if ( 0 == *d_cursor_p ) {
            d_endFlag = true;
            return *this;                                             // RETURN
        }

        int currentState = TOKEN;
        int inputType;

        do {
            if (0 == *d_cursor_p) {
                return *this;                                         // RETURN
            }

            inputType = d_sharedData.inputType(*d_cursor_p);

            switch (actionTable[inputType][currentState]) {
              case ACC_TOKEN: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACC_DELIM: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case STOP: {       // stop parsing and return result
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = stateTable[inputType][currentState];
            ++d_cursor_p;
        } while (true);
    }

    BSLS_ASSERT(0);
}

void Tokenizer::resetImplementation(const char  *input,
                                    const char  *end)
{
    d_input_p     = input;
    d_cursor_p    = input;
    d_prevDelim_p = input;
    d_token_p     = input;
    d_postDelim_p = input;
    d_end_p       = end;
    d_endFlag     = false;

    if (d_end_p) {
        while (d_end_p != d_cursor_p
               && SFT == d_sharedData.inputType(*d_cursor_p))
        {
            ++d_cursor_p;
        }
    } else {
        while (0 != *d_cursor_p
               && SFT == d_sharedData.inputType(*d_cursor_p))
        {
            ++d_cursor_p;
        }
    }

    ++*this;                 // find first token
}

void Tokenizer::reset(const char *input)
{
    BSLS_ASSERT(input);
    return resetImplementation(input, 0);
}

void Tokenizer::reset(const bslstl::StringRef& input)
{
    BSLS_ASSERT(input.begin());
    return resetImplementation(input.begin(), input.end());
}

bool Tokenizer::hasPreviousSoft() const
{
    const char *p = d_prevDelim_p;

    while (p != d_token_p) {
        if (SFT == d_sharedData.inputType(*p)) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

bool Tokenizer::hasTrailingSoft() const
{
    if (d_endFlag) {
        return false;                                                 // RETURN
    }

    const char *p = d_postDelim_p;

    while (p != d_cursor_p) {
        if (SFT == d_sharedData.inputType(*p)) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

bool Tokenizer::isPreviousHard() const
{
    const char *p = d_prevDelim_p;

    while (p != d_token_p) {
        if (HRD == d_sharedData.inputType(*p)) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

bool Tokenizer::isTrailingHard() const
{
    if (d_endFlag) {
        return false;                                                 // RETURN
    }

    const char *p = d_postDelim_p;

    while (p != d_cursor_p) {
        if (HRD == d_sharedData.inputType(*p)) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

Tokenizer::iterator Tokenizer::begin() const
{
    return TokenizerIterator(d_input_p, d_end_p, &d_sharedData);
}

Tokenizer::iterator Tokenizer::end() const
{
    return TokenizerIterator();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
