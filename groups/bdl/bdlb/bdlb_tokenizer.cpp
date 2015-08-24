// bdlb_tokenizer.cpp                                                 -*-C++-*-
#include <bdlb_tokenizer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_tokenizer_cpp,"$Id$ $CSID$")

namespace {
// The character inputs break down into the following types:

enum InputType {
    TOK = 0,  // token
    SFT = 1,  // soft delimiter
    HRD = 2,  // hard delimiter
    EOS = 3   // end of string
};
const int NUM_INPUTS = 4;

// The following defines the set of similar actions to be performed:
enum Action {
    ACT_AT = 0,  // accumulate token
    ACT_AD = 1,  // accumulate delimiter
    ACT_RT = 2,  // return from function
    ACT_ER = 3   // an error has occurred or string is invalid
};

// The following defines the set of states:
enum State {
    START = 0,  // No token or hard delimiter character seen yet and
                // possibly some soft delimiter characters seen.
    SOFTD = 1,  // One or more soft delimiter characters seen after one or
                // more token characters have been seen.
    HARDD = 2,  // Exactly one hard delimiter character seen and
                // possibly some soft delimiter characters seen.
    TOKEN = 3   // One or more token characters have been seen.
};
const int NUM_STATES = 4;

const State nextStateTable[NUM_STATES][NUM_INPUTS] = {
    // TOK     SFT     HRD     EOS
    // -----   -----   -----   -----
    {  TOKEN,  START,  HARDD,  START  },  // START
    {  START,  SOFTD,  HARDD,  START  },  // SOFTD
    {  START,  HARDD,  START,  START  },  // HARDD
    {  TOKEN,  SOFTD,  HARDD,  START  },  // TOKEN
};

const Action actionTable[NUM_STATES][NUM_INPUTS] = {
    // TOK     SFT     HRD     EOS
    // ------  ------  ------  ------
    {  ACT_AT, ACT_ER, ACT_AD, ACT_ER },  // START
    {  ACT_RT, ACT_AD, ACT_AD, ACT_RT },  // SOFTD
    {  ACT_RT, ACT_AD, ACT_RT, ACT_RT },  // HARDD
    {  ACT_AT, ACT_AD, ACT_AD, ACT_RT },  // TOKEN
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

    if (!softDelimiters.isEmpty()) {
        for(bslstl::StringRef::const_iterator it = softDelimiters.begin();
            it != softDelimiters.end();
            ++it) {
            // DUPLICATION TEST
            // ASSERT(d_charTypes[(unsigned char)*it] != TOK);
            // END OF DUPLICATION TEST
            d_charTypes[(unsigned char)*it] = SFT;
        }
    }
}

Tokenizer_Data::Tokenizer_Data(const bslstl::StringRef& softDelimiters,
                               const bslstl::StringRef& hardDelimiters)
{
    bsl::memset(d_charTypes, TOK, k_MAX_CHARS);

    if (!softDelimiters.isEmpty()) {
        for(bslstl::StringRef::const_iterator it = softDelimiters.begin();
            it != softDelimiters.end();
            ++it) {
            // DUPLICATION TEST
            // ASSERT(d_charTypes[(unsigned char)*it] != TOK);
            // END OF DUPLICATION TEST
            d_charTypes[(unsigned char)*it] = SFT;
        }
    }

    if (!hardDelimiters.isEmpty()) {
        for(bslstl::StringRef::const_iterator it = hardDelimiters.begin();
            it != hardDelimiters.end();
            ++it) {
            // DUPLICATION TEST
            // ASSERT(d_charTypes[(unsigned char)*it] != TOK);
            // END OF DUPLICATION TEST
            d_charTypes[(unsigned char)*it] = HRD;
        }
    }
}

inline
bool Tokenizer_Data::isSoftDelimiter(const unsigned char ch) const
{
    return SFT == d_charTypes[ch];
}

inline
bool Tokenizer_Data::isHardDelimiter(const unsigned char ch) const
{
    return HRD == d_charTypes[ch];
}

inline
int Tokenizer_Data::toInputType(const unsigned char ch) const
{
    return static_cast<int>(d_charTypes[ch]);
}

                        // -----------------------
                        // class TokenizerIterator
                        // -----------------------

// CREATORS
TokenizerIterator::TokenizerIterator()
: d_cursor_p(0)
, d_token_p(0)
, d_postDelim_p(0)
, d_end_p(0)
, d_tokenizerData_p(0)
, d_isEnd(true)
{
    // Constructor for end iterator
}

TokenizerIterator::TokenizerIterator(const char           *input,
                                     const char           *end,
                                     const Tokenizer_Data *tokenizerData)
: d_cursor_p(input)
, d_token_p(input)
, d_postDelim_p(input)
, d_end_p(end)
, d_tokenizerData_p(tokenizerData)
, d_isEnd(false)
{
    BSLS_ASSERT(input);
    BSLS_ASSERT(tokenizerData);

    if (d_cursor_p) {   // skip optional leader
        while (!isEos()
               && d_tokenizerData_p->isSoftDelimiter(
                                                   (unsigned char)*d_cursor_p))
        {
            ++d_cursor_p;
        }

        if (isEos()) {
            d_isEnd = true;     // no token found after the leader
        } else {
            ++*this;            // find first token
        }
    } else {
        d_isEnd = true;
    }
}

TokenizerIterator::TokenizerIterator(const TokenizerIterator&  other)
: d_cursor_p(other.d_cursor_p)
, d_token_p(other.d_token_p)
, d_postDelim_p(other.d_postDelim_p)
, d_end_p(other.d_end_p)
, d_tokenizerData_p(other.d_tokenizerData_p)
, d_isEnd(other.d_isEnd)
{
}

// MANIPULATORS
TokenizerIterator& TokenizerIterator::operator++()
{
    BSLS_ASSERT(d_cursor_p);
    BSLS_ASSERT(d_tokenizerData_p);

    if ( d_isEnd ) {
        return *this;                                                 // RETURN
    }

    // This function does not use slow isEos() for performance reasons
    if (d_end_p) {
        d_token_p     = d_cursor_p;
        d_postDelim_p = d_cursor_p;

        if ( d_end_p == d_cursor_p ) {
            d_isEnd = true;            // invalidate tokenizer iterator
            return *this;                                             // RETURN
        }

        int currentState = START;
        do {
            int inputType = (d_end_p == d_cursor_p)
                            ? EOS
                            : d_tokenizerData_p->toInputType(
                                                   (unsigned char)*d_cursor_p);

            switch (actionTable[currentState][inputType]) {
              case ACT_AT: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACT_AD: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case ACT_RT: {  // return from operator++
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = nextStateTable[currentState][inputType];
            ++d_cursor_p;
        } while (true);
    } else {
        d_token_p     = d_cursor_p;
        d_postDelim_p = d_cursor_p;

        if ( 0 == *d_cursor_p ) {
            d_isEnd = true;       // invalidate tokenizer iterator
            return *this;                                             // RETURN
        }

        int currentState = START;
        do {
            int inputType = (0 == *d_cursor_p)
                            ? EOS
                            : d_tokenizerData_p->toInputType(
                                                   (unsigned char)*d_cursor_p);

            switch (actionTable[currentState][inputType]) {
              case ACT_AT: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACT_AD: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case ACT_RT: {  // return from operator++
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = nextStateTable[currentState][inputType];
            ++d_cursor_p;
        } while (true);
    }
    BSLS_ASSERT(0);
}

inline
bool TokenizerIterator::isEqual(const TokenizerIterator& other) const
{
    // Must be both ether at the end or not to continue; fast path decision
    if (d_isEnd != other.d_isEnd) {
        return false;                                                 // RETURN
    }

    // Comparing with end iterators
    if ( d_isEnd && other.d_isEnd ) {
           return true;                                               // RETURN
    }


    return (    (d_cursor_p        == other.d_cursor_p)
             && (d_token_p         == other.d_token_p)
             && (d_postDelim_p     == other.d_postDelim_p)
             && (d_tokenizerData_p == other.d_tokenizerData_p)
             && (d_end_p           == other.d_end_p) );
}

                        // ---------------
                        // class Tokenizer
                        // ---------------
// CREATORS
Tokenizer::Tokenizer(const char               *input,
                     const bslstl::StringRef&  soft)
: d_tokenizerData(soft)
{
    BSLS_ASSERT(input);
    reset(input);
}

Tokenizer::Tokenizer(const char               *input,
                     const bslstl::StringRef&  soft,
                     const bslstl::StringRef&  hard)
: d_tokenizerData(soft, hard)
{
    BSLS_ASSERT(input);
    reset(input);
}

Tokenizer::Tokenizer(const bslstl::StringRef&  input,
                     const bslstl::StringRef&  soft)
: d_tokenizerData(soft)
{
    reset(input);
}

Tokenizer::Tokenizer(const bslstl::StringRef&  input,
                     const bslstl::StringRef&  soft,
                     const bslstl::StringRef&  hard)
: d_tokenizerData(soft, hard)
{
    reset(input);
}

Tokenizer::~Tokenizer()
{
}

Tokenizer& Tokenizer::operator++()
{
    BSLS_ASSERT(d_cursor_p);

    if (d_isEnd) {
        return *this;
    }

    // This function does not use slow isEos() for performance reasons
    if (d_end_p) {
        d_prevDelim_p = d_postDelim_p;  // current delimiter becomes previous
        d_postDelim_p = d_cursor_p;
        d_token_p     = d_cursor_p;

        if ( d_end_p == d_cursor_p ) {
            d_isEnd = true;            // invalidate tokenizer iterator
            return *this;                                             // RETURN
        }

        int currentState = START;
        do {
            int inputType = (d_end_p == d_cursor_p)
                            ? EOS
                            : d_tokenizerData.toInputType(
                                                   (unsigned char)*d_cursor_p);

            switch (actionTable[currentState][inputType]) {
              case ACT_AT: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACT_AD: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case ACT_RT: {  // return from operator++
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = nextStateTable[currentState][inputType];
            ++d_cursor_p;
        } while (true);
    } else {
        d_prevDelim_p = d_postDelim_p;  // current delimiter becomes previous
        d_postDelim_p = d_cursor_p;
        d_token_p     = d_cursor_p;

        if ( 0 == *d_cursor_p ) {
            d_isEnd = true;       // invalidate tokenizer iterator
            return *this;                                             // RETURN
        }

        int currentState = START;
        do {
            int inputType = (0 == *d_cursor_p)
                            ? EOS
                            : d_tokenizerData.toInputType(
                                                   (unsigned char)*d_cursor_p);

            switch (actionTable[currentState][inputType]) {
              case ACT_AT: {  // accumulate token
                ++d_postDelim_p;
              } break;
              case ACT_AD: {  // accumulate delimiter
                // Nothing to do.
              } break;
              case ACT_RT: {  // return from operator++
                return *this;                                         // RETURN
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }

            currentState = nextStateTable[currentState][inputType];
            ++d_cursor_p;
        } while (true);
    }
    BSLS_ASSERT(0);
}

void Tokenizer::resetImpl(const char  *input,
                          const char  *end)
{
    d_input_p     = input;
    d_cursor_p    = input;
    d_prevDelim_p = input;
    d_token_p     = input;
    d_postDelim_p = input;
    d_end_p       = end;
    d_isEnd       = false;

    if (d_cursor_p) {                // accumulate leader in previous delimiter
        while (!isEos()
                && d_tokenizerData.isSoftDelimiter((unsigned char)*d_cursor_p))
        {
            ++d_cursor_p;
        }

        if (isEos()) {
            d_isEnd = true;
        } else {
            ++*this;                 // find first token
        }
    } else {
        d_isEnd = true;
    }
}

void Tokenizer::reset(const char *input)
{
    BSLS_ASSERT(input);
    return resetImpl(input);
}

void Tokenizer::reset(const bslstl::StringRef& input)
{
    return resetImpl(input.begin(), input.end());
}

bool Tokenizer::hasSoft() const
{
    BSLS_ASSERT(d_postDelim_p);
    BSLS_ASSERT(d_cursor_p);

    if (d_isEnd) {
        return false;                                                 // RETURN
    }

    const char *p = d_postDelim_p;

    while (p != d_cursor_p) {
        if (d_tokenizerData.isSoftDelimiter(static_cast<unsigned char>(*p))) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

bool Tokenizer::hasPreviousSoft() const
{
    BSLS_ASSERT(d_prevDelim_p);
    BSLS_ASSERT(d_token_p);

    const char *p = d_prevDelim_p;

    while (p != d_token_p) {
        if (d_tokenizerData.isSoftDelimiter(static_cast<unsigned char>(*p))) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

bool Tokenizer::isHard() const
{
    BSLS_ASSERT(d_postDelim_p);
    BSLS_ASSERT(d_cursor_p);

    if (d_isEnd) {
        return false;                                                 // RETURN
    }

    const char *p = d_postDelim_p;

    while (p != d_cursor_p) {
        if (d_tokenizerData.isHardDelimiter(static_cast<unsigned char>(*p))) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

bool Tokenizer::isPreviousHard() const
{
    BSLS_ASSERT(d_prevDelim_p);
    BSLS_ASSERT(d_token_p);

    const char *p = d_prevDelim_p;

    while (p != d_token_p) {
        if (d_tokenizerData.isHardDelimiter(static_cast<unsigned char>(*p))) {
            return true;                                              // RETURN
        }
        ++p;
    }
    return false;
}

Tokenizer::iterator Tokenizer::begin() const
{
    return TokenizerIterator(d_input_p, d_end_p, &d_tokenizerData);
}

Tokenizer::iterator Tokenizer::end() const
{
    return TokenizerIterator();
}
// MANIPULATORS

}  // close package namespace



// FREE OPERATORS
bool bdlb::operator==(const TokenizerIterator& lhs,
                      const TokenizerIterator& rhs)
{
    return lhs.isEqual(rhs);
}

bool bdlb::operator!=(const TokenizerIterator& lhs,
                      const TokenizerIterator& rhs)
{
    return !lhs.isEqual(rhs);
}

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
