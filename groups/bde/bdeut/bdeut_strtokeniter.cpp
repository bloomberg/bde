// bdeut_strtokeniter.cpp      -*-C++-*-
#include <bdeut_strtokeniter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeut_strtokeniter_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>        // for testing only
#include <bslma_testallocator.h>    // for testing only

#include <bsls_assert.h>

#include <bsl_cstring.h>    // 'memset'
#include <bsl_string.h>

///IMPLEMENTATION NOTES
///--------------------
// 'bdeut_StrTokenIter' was originally implemented using 'bdea_CharArray'.
// The internal capacity of instances of that container never shrinks.  Use of
// 'bdea_CharArray' was replaced with 'bsl::string' prior to releasing this
// component.  Note that the ISO C++ Standard apparently allows the internal
// capacity of 'bsl::string' instances to shrink (e.g., when calling 'clear'
// or 'erase').  An STL implementation that exhibited this behavior would
// adversely impact the efficiency of 'bdeut_StrTokenIter'.  However, STLport
// does *not* shrink the internal capacity of 'bsl::string' instances (except
// when 'resize' is called).

namespace BloombergLP {

// STATICS
// The character inputs break down into the following types:
enum InputType {
    TOK = 0,  // token
    SFT = 1,  // soft delimiter
    HRD = 2,  // hard delimiter
    EFF = 3   // end of file
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
    HARDD = 2,  // Exactly one (1) hard delimiter character seen and
                // possibly some soft delimiter characters seen.
    TOKEN = 3   // One or more token characters have been seen.
};
const int NUM_STATES = 4;

static State nextStateTable[NUM_STATES][NUM_INPUTS] = {
    // TOK     SFT     HRD     EFF
    // -----   -----   -----   -----
    {  TOKEN,  START,  HARDD,  START  },// START
    {  START,  SOFTD,  HARDD,  START  },// SOFTD
    {  START,  HARDD,  START,  START  },// HARDD
    {  TOKEN,  SOFTD,  HARDD,  START  },// TOKEN
};

static Action actionTable[NUM_STATES][NUM_INPUTS] = {
    // TOK     SFT     HRD     EFF
    // ------  ------  ------  ------
    {  ACT_AT, ACT_ER, ACT_AD, ACT_ER },// START
    {  ACT_RT, ACT_AD, ACT_AD, ACT_RT },// SOFTD
    {  ACT_RT, ACT_AD, ACT_RT, ACT_RT },// HARDD
    {  ACT_AT, ACT_AD, ACT_AD, ACT_RT },// TOKEN
};

static void loadDelimTable(char *table, int size,
                           const char *softDelims, const char *hardDelims)
{
    bsl::memset(table, TOK, size);
    table['\0'] = EFF;
    if (softDelims) {
        while (*softDelims) {
            table[(unsigned char)*softDelims++] = SFT;
        }
    }
    if (hardDelims) {
        while (*hardDelims) {
            table[(unsigned char)*hardDelims++] = HRD;
        }
    }
}

                       // ------------------------
                       // class bdeut_StrTokenIter
                       // ------------------------

//CREATORS
bdeut_StrTokenIter::bdeut_StrTokenIter(const char      *input,
                                       const char      *softDelims,
                                       bslma_Allocator *basicAllocator)
: d_string_p(input)
, d_tokenBuf(basicAllocator)
, d_bufferA(basicAllocator)
, d_bufferB(basicAllocator)
, d_prevBuf_p(0)
, d_delimBuf_p(0)
{
    loadDelimTable(d_delimTable, BDEUT_TABLE_SIZE, softDelims, 0);
    reset(input);
}

bdeut_StrTokenIter::bdeut_StrTokenIter(const char      *input,
                                       const char      *softDelims,
                                       const char      *hardDelims,
                                       bslma_Allocator *basicAllocator)
: d_string_p(input)
, d_tokenBuf(basicAllocator)
, d_bufferA(basicAllocator)
, d_bufferB(basicAllocator)
, d_prevBuf_p(0)
, d_delimBuf_p(0)
{
    loadDelimTable(d_delimTable, BDEUT_TABLE_SIZE, softDelims, hardDelims);
    reset(input);
}

bdeut_StrTokenIter::~bdeut_StrTokenIter()
{
}

// MANIPULATORS
void bdeut_StrTokenIter::operator++()
{
    BSLS_ASSERT(d_string_p);
    if (EFF == d_delimTable[(unsigned char)*d_string_p]) {
        d_string_p = 0;  // invalidate iterator
        return;                                                       // RETURN
    }

    bsl::string *temp = d_delimBuf_p;
    d_delimBuf_p = d_prevBuf_p;
    d_prevBuf_p = temp;

    int currentState = START;
    d_tokenBuf.clear();  // plan to overwrite both buffers
    d_delimBuf_p->clear();

    for (int inputType = d_delimTable[(unsigned char)*d_string_p];;
        currentState = nextStateTable[currentState][inputType],
        ++d_string_p,
        inputType = d_delimTable[(unsigned char)*d_string_p]) {

        switch (actionTable[currentState][inputType]) {
          case ACT_AT: {  // accumulate token
              d_tokenBuf.push_back(*d_string_p);
          } break;
          case ACT_AD: {  // accumulate delimiter
              d_delimBuf_p->push_back(*d_string_p);
          } break;
          case ACT_RT: {  // return from operator++
              return;                                                 // RETURN
          } break;
          default: {
            BSLS_ASSERT(0);
          } break;
        };
    }
    BSLS_ASSERT(0);
}

void bdeut_StrTokenIter::reset(const char *input)
{
    d_string_p = input;
    d_tokenBuf.clear();  // plan to overwrite all buffers
    d_bufferA.clear();
    d_bufferB.clear();
    d_prevBuf_p = &d_bufferA;
    d_delimBuf_p = &d_bufferB;

    // accumulate leader
    if (input) {

        while (SFT == d_delimTable[(unsigned char)*d_string_p]) {
            d_bufferB.push_back(*d_string_p++);  // current
        }

        // "wake up" iterator
        ++*this;
    }
    if (!d_string_p || !input) {                 // no tokens in 'input'
        d_bufferA.clear();

        bsl::string *temp = d_delimBuf_p;        // swap pointers
        d_delimBuf_p = d_prevBuf_p;
        d_prevBuf_p  = temp;
    }
}

// ACCESSORS
bool bdeut_StrTokenIter::isHard() const
{
    const char *dBuf = d_delimBuf_p->data();
    while (*dBuf) {
        if (HRD == d_delimTable[(unsigned char)*dBuf++]) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool bdeut_StrTokenIter::isPreviousHard() const
{
    const char *dBuf = d_prevBuf_p->data();
    while (*dBuf) {
        if (HRD == d_delimTable[(unsigned char)*dBuf++]) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool bdeut_StrTokenIter::hasSoft() const
{
    const char *dBuf = d_delimBuf_p->data();
    unsigned char firstCharType = d_delimTable[(unsigned char)*dBuf];

    if (EFF == firstCharType) {
        return false;                                                 // RETURN
    }
    if (SFT == firstCharType) {
        return true;                                                  // RETURN
    }
    return SFT == d_delimTable[(unsigned char)*(dBuf+1)];
}

bool bdeut_StrTokenIter::hasPreviousSoft() const
{
    const char *dBuf = d_prevBuf_p->data();
    unsigned char firstCharType = d_delimTable[(unsigned char)*dBuf];

    if (EFF == firstCharType) {
        return false;                                                 // RETURN
    }
    if (SFT == firstCharType) {
        return true;                                                  // RETURN
    }
    return SFT == d_delimTable[(unsigned char)*(dBuf+1)];
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
