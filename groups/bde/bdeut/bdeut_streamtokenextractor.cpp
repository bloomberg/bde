//bdeut_streamtokenextractor.cpp -*-C++-*-

#include <bdeut_streamtokenextractor.h>

#include <cstring>    // 'memset'
#include <ostream>

// Implementor's note: the buffer 'd_Buf' is never empty, and always
// null-terminated.  This invariant is created by the constructor (which does a
// push_back('\0')) and maintained by all the routines, and so it is safe to
// assume that &d_buf.front() is always a valid non-null pointer.  Hence the
// presence of BDE_ASSERT_CPP(!d_buf.empty()) in the 'isHard' and 'hasSoft'
// methods, as well as in the inline function.


                       // =======
                       // STATICS
                       // =======

namespace BloombergLP {

const int NUM_INPUTS = 4;

// The character inputs break down into the following types:
enum InputType {
        TOK =  0, // token
        SFT =  1, // soft delimiter
        HRD =  2, // hard delimiter
        EFF =  3  // end of file
};

// The following defines the set of similar actions to be performed:
enum Action {
        ACT_AC = 0, // Accumulate character in the internal buffer.
        ACT_SK = 1, // Skip over the character in the input stream.
        ACT_IL = 2, // Initialize internal buffer and goto NHDTK state.
        ACT_RT = 3, // Return from function.
        ACT_ER = 4  // Return from function.
};

enum State {
        INITL = 0, // Initialize the internal buffer to 0 and move to NHDTK
                   // state.
        NHDTK = 1, // No token or hard delimiter seen yet and possibly
                   // some soft delimiter seen.
        HARDD = 2, // Exactly one (1) hard delimiter seen and possibly
                   // some soft delimiter seen.
        SOFTD = 3, // One or more soft delimiter characters seen after one or
                   // more token characters have been seen.
        TOKEN = 4  // One or more tokens have been seen.
};

const int NUM_DELIM_STATES = 3;
const int NUM_TOKEN_STATES = 5;

static State nextDelimiterStateTable[NUM_DELIM_STATES][NUM_INPUTS] = {
  // TOK     SFT     HRD     EFF
  // -----   -----   -----   -----
  {  NHDTK,  NHDTK,  NHDTK,  NHDTK  },// INITL
  {  NHDTK,  NHDTK,  HARDD,  NHDTK  },// NHDTK
  {  NHDTK,  HARDD,  NHDTK,  NHDTK  },// HARDD
};

static State nextTokenStateTable[NUM_TOKEN_STATES][NUM_INPUTS] = {
  // TOK     SFT     HRD     EFF
  // -----   -----   -----   -----
  {  NHDTK,  NHDTK,  NHDTK,  NHDTK  },// INITL
  {  TOKEN,  NHDTK,  HARDD,  NHDTK  },// NHDTK
  {  NHDTK,  HARDD,  NHDTK,  NHDTK  },// HARDD
  {  NHDTK,  SOFTD,  HARDD,  NHDTK  },// SOFTD
  {  TOKEN,  SOFTD,  HARDD,  NHDTK  },// TOKEN
};

static void loadDelimTable(char *table, int size,
                           const char *softDelims, const char *hardDelims)
{
    std::memset(table, TOK, size);
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

static inline void ungetc(istream& stream)
// It would be nice if we could use ungetc function that should
// be in the std::istream class.  However, as of 12/20/2000, it is not
// in the SUN's implementation of istream.
{
   stream.seekg(-1, istream::cur);
}
                       // ================================
                       // class bdeut_StreamTokenExtractor
                       // ================================

bdeut_StreamTokenExtractor::bdeut_StreamTokenExtractor(
         const char *softDelims, const char *hardDelims)
{
    loadDelimTable(d_delimTable, TABLE_SIZE, softDelims, hardDelims);
    d_buf.push_back('\0');
}

bdeut_StreamTokenExtractor::~bdeut_StreamTokenExtractor()
{
}

int bdeut_StreamTokenExtractor::getLeader(std::istream *stream)
{
    char c;
    if (!stream || stream->get(c).eof()) {
        return 1;
    }

    d_buf.resize(0);

    while (SFT == d_delimTable[(unsigned char)c]) {
        d_buf.push_back(c);
        if (stream->get(c).eof()) {
            d_buf.push_back('\0');
            return 0;
        }
    }
    d_buf.push_back('\0');
    ungetc(*stream);
    return 0;
}

void bdeut_StreamTokenExtractor::stripLeader(std::istream *stream)
{
    char c;
    if (!stream || stream->get(c).eof()) {
        return;
    }

    while (SFT == d_delimTable[(unsigned char)c]) {
        if (stream->get(c).eof()) {
            return;
        }
    }
    ungetc(*stream);
    return;
}

int bdeut_StreamTokenExtractor::getToken(std::istream *stream)
{
    char c;
    if (!stream || stream->get(c).eof()) {
        return 1;
    }

    d_buf.resize(0);

    while (TOK == d_delimTable[(unsigned char)c]) {
        d_buf.push_back(c);
        if (stream->get(c).eof()) {
            d_buf.push_back('\0');
            return 0;
        }
    }
    d_buf.push_back('\0');
    ungetc(*stream);
    return 0;
}

void bdeut_StreamTokenExtractor::stripToken(std::istream *stream)
{
    char c;
    if (!stream || stream->get(c).eof()) {
        return;
    }

    while (TOK == d_delimTable[(unsigned char)c]) {
        if (stream->get(c).eof()) {
            return;
        }
    }
    ungetc(*stream);
    return;
}

int bdeut_StreamTokenExtractor::getDelimiter(std::istream *stream)
{
    if (!stream) return 1;

    static Action actionTable[NUM_DELIM_STATES][NUM_INPUTS] = {
      // TOK     SFT     HRD     EFF
      // ------  ------  ------  ------
      {  ACT_IL, ACT_IL, ACT_IL, ACT_IL },// INITL
      {  ACT_RT, ACT_AC, ACT_AC, ACT_RT },// NHDTK
      {  ACT_RT, ACT_AC, ACT_RT, ACT_RT },// HARDD
    };

    State currentState = INITL;
    InputType inputType = EFF;

    char c;
    while (!stream->get(c).eof()) {
        inputType = (InputType)d_delimTable[(unsigned char)c];
        switch (actionTable[currentState][inputType]) {
          case ACT_IL: {             // initialize internal buffer
              d_buf.resize(0);
              ungetc(*stream);
          } break;
          case ACT_AC: {             // accumulate character
              d_buf.push_back(c);
          } break;
          case ACT_RT: {             // return from function
              d_buf.push_back('\0');
              ungetc(*stream);
              return 0;              // return from function
          } break;
          case ACT_SK:
          case ACT_ER:
          default: {
            BDE_ASSERT_CPP(0);
          } break;
        };
        currentState = nextDelimiterStateTable[currentState][inputType];
    }
    d_buf.push_back('\0');
    return !currentState;            // return from function
}

void bdeut_StreamTokenExtractor::stripDelimiter(std::istream *stream)
{
    if (!stream) {
        return;
    }

    static Action actionTable[NUM_DELIM_STATES][NUM_INPUTS] = {
      // TOK     SFT     HRD     EFF
      // ------  ------  ------  ------
      {  ACT_ER, ACT_ER, ACT_ER, ACT_ER },// INITL  --> cannot be in this state
      {  ACT_RT, ACT_SK, ACT_SK, ACT_RT },// NHDTK
      {  ACT_RT, ACT_SK, ACT_RT, ACT_RT },// HARDD
    };

    State currentState = NHDTK;
    InputType inputType = EFF;

    char c;
    while (!stream->get(c).eof()) {
        inputType = (InputType)d_delimTable[(unsigned char)c];
        switch (actionTable[currentState][inputType]) {
          case ACT_SK: {             // accumulate character
          } break;
          case ACT_RT: {             // return from function
              ungetc(*stream);
              return;                // return from function
          } break;
          case ACT_IL:
          case ACT_AC:
          case ACT_ER:
          default: {
            BDE_ASSERT_CPP(0);
          } break;
        };
        currentState = nextDelimiterStateTable[currentState][inputType];
    }
}

int bdeut_StreamTokenExtractor::get(std::istream *stream)
{
    if (!stream) return 1;

    static Action actionTable[NUM_TOKEN_STATES][NUM_INPUTS] = {
      // TOK     SFT     HRD     EFF
      // ------  ------  ------  ------
      {  ACT_IL, ACT_IL, ACT_IL, ACT_IL },// INITL
      {  ACT_AC, ACT_SK, ACT_SK, ACT_RT },// NHDTK
      {  ACT_RT, ACT_SK, ACT_RT, ACT_RT },// HARDD
      {  ACT_RT, ACT_SK, ACT_SK, ACT_RT },// SOFTD
      {  ACT_AC, ACT_SK, ACT_SK, ACT_RT },// TOKEN
    };

    State currentState = INITL;
    InputType inputType = EFF;

    char c;
    while (!stream->get(c).eof()) {
        inputType = (InputType)d_delimTable[(unsigned char)c];
        switch (actionTable[currentState][inputType]) {
          case ACT_IL: {             // initialize internal buffer
              d_buf.resize(0);
              ungetc(*stream);
          } break;
          case ACT_AC: {             // accumulate character
              d_buf.push_back(c);
          } break;
          case ACT_SK: {             // skip character
          } break;
          case ACT_RT: {             // return from function
              d_buf.push_back('\0');
              ungetc(*stream);
              return 0;              // return from function
          } break;
          case ACT_ER:
          default: {
            BDE_ASSERT_CPP(0);
          } break;
        };
        currentState = nextTokenStateTable[currentState][inputType];
    }
    d_buf.push_back('\0');
    return !currentState;            // return from function
}

void bdeut_StreamTokenExtractor::strip(std::istream *stream)
{
    if (!stream) return;

    static Action actionTable[NUM_TOKEN_STATES][NUM_INPUTS] = {
      // TOK     SFT     HRD     EFF
      // ------  ------  ------  ------
      {  ACT_ER, ACT_ER, ACT_ER, ACT_ER },// INITL  --> cannot be in this state
      {  ACT_SK, ACT_SK, ACT_SK, ACT_RT },// NHDTK
      {  ACT_RT, ACT_SK, ACT_RT, ACT_RT },// HARDD
      {  ACT_RT, ACT_SK, ACT_SK, ACT_RT },// SOFTD
      {  ACT_SK, ACT_SK, ACT_SK, ACT_RT },// TOKEN
    };

    State currentState = NHDTK;
    InputType inputType = EFF;

    char c;
    while (!stream->get(c).eof()) {
        inputType = (InputType)d_delimTable[(unsigned char)c];
        switch (actionTable[currentState][inputType]) {
          case ACT_SK: {             // skip character
          } break;
          case ACT_RT: {             // return from function
              ungetc(*stream);
              return;                // return from function
          } break;
          case ACT_IL:
          case ACT_AC:
          case ACT_ER:
          default: {
            BDE_ASSERT_CPP(0);
          } break;
        };
        currentState = nextTokenStateTable[currentState][inputType];
    }
}

int bdeut_StreamTokenExtractor::isHard() const
{
    BDE_ASSERT_CPP(!d_buf.empty());
    const char *dBuf = &d_buf.front();
    while (*dBuf) {
        if (HRD == d_delimTable[(unsigned char)*dBuf++]) {
            return 1;
        }
    }
    return 0;
}

int bdeut_StreamTokenExtractor::hasSoft() const
{
    BDE_ASSERT_CPP(!d_buf.empty());
    const char *dBuf = &d_buf.front();
    unsigned char firstCharType = d_delimTable[(unsigned char)*dBuf];

    if (EFF == firstCharType) {
        return 0;
    }
    if (SFT == firstCharType) {
        return 1;
    }
    return SFT == d_delimTable[(unsigned char)*(dBuf+1)];
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
