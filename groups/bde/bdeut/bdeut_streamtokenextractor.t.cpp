// bdeut_streamtokenextractor.t.cpp     -*- C++ -*- 

#define BDESTL_HIJACK_STD_NAMESPACE
    // Make it so that 'std' will always refer to 'BloombergLP::std'
    // This macro must be defined before including any standard headers.
    // This macro should not be defined for code that resides in the
    // 'BloombergLP' namespace.

#include <bdeut_streamtokenextractor.h>

#include <stdio.h>      // sprintf()
#include <stdlib.h>     // atoi()
#include <string.h>     // strcmp
#include <strstream>  // istrstream
#include <iostream>
#include <assert.h>
using namespace BloombergLP;
using namespace std;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ----------------------------------------------------------------------------
// bdeut_StreamTokenExtractor (members)
// ----------------------------------------------------------------------------
// [2] bdeut_StreamTokenExtractor(const char *soft, const char *hard= 0)
// [2] ~bdeut_StreamTokenExtractor()
// [2] int getLeader(istream *input)
// [2] int getToken(istream *input)
// [2] int getDelimiter(istream *input)
// [2] int get(istream *input)
// [3] void stripLeader(istream *input)
// [3] void stripToken(istream *input)
// [3] void stripDelimiter(istream *input)
// [3] void strip(istream *input)
// [2] const char *operator()() const
// [2] int isHard() const
// [2] int hasSoft() const
// ----------------------------------------------------------------------------
// [1] breathing test
// [4] stream state changes
// [4] buffer contents
// [4] 'get' return status
// [5] state transition table (pseudo white box) test
// [6] usage examples
// [7] stress test

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


// usage example functions
static void listTokens(istream *in, const char *softDelimChars, int verbose)
{
    bdeut_StreamTokenExtractor it(softDelimChars);
    for (;;) {
        if (it.get(in)) break;
        if (verbose) cout << it() << ":";       // print out the token only
    }
    if (verbose) cout << endl;
}

static int pipe(ostream &out, istream *in, 
                const char *soft, const char *hard, int verbose)
{
    bdeut_StreamTokenExtractor it(soft, hard);

    if (it.getLeader(in)) return 0;     // 0 bcs no hard delimiter seen
    if (verbose) out << it();           // stream out leader

    for (;;) {
       if (it.getToken(in)) break;
       if (verbose) out << it();        // stream out token

       if (it.getDelimiter(in)) break;
       if (verbose) out << it();        // stream out (trailing) delimiter
    }

    if (verbose) out << endl;
    return it.isHard();                 // Return 1 only if last token is
                                        // followed by a hard delimiter.
}
    
// 'getNamePhone1' is not presented here because it introduces a dependence
// on bdeut_strtokeniter component, a dependence which is otherwise 
// unneccessary.

static void getNamePhone2(istream *inStream, int verbose)
{
    
    const char *const SOFT_DELIM = " \t";
    const char *const HARD_DELIM = "|";

    bdeut_StreamTokenExtractor flush("\n");
    bdeut_StreamTokenExtractor extract(SOFT_DELIM, HARD_DELIM);

    while (inStream && !inStream->eof()) {     // Note: empty lines ignored
        for (;;) {                              
            flush.stripLeader(inStream);             // remove empty lines.
            if (extract.get(inStream)) return;       // Acquire <Last Name>. 
            if (verbose) cout.width(10);             // Preset column width.
            if (verbose) cout << extract() << " : "; // Append name & separatr.
            extract.strip(inStream);                 // Strip <First Name>. 
            if (extract.get(inStream)) return;       // Acquire <Phone #>. 
            if (verbose) cout << extract() << '\n';  // Append <phone #> & \n.
            flush.strip(inStream);                   // Flush thru newlines.
        }
    }
    if (verbose) cout << endl;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])

{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

#if defined(BDES_PLATFORM__CMP_EPC)
    // The EPC C++ compiler on DGUX has a bug in the constructor for
    // ostrstream that take a buffer and a length.  The constructor will
    // destroy the last byte of the variable immediately preceeding the
    // buffer used to hold the stream.  If the parameter specifying the
    // buffer passed to the constructor is 1 less than the size of the buffer
    // stack integrity is maintained.
    const int epcBug = 1;
#else
    const int epcBug = 0;
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    //bdema_TestAllocator  testAllocator(veryVeryVerbose);
    //bdema_Allocator     *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // Stress Test:  The second argument specifies how big.
        //               (Making it negative suppresses output.)
        //
        // Plan:
        //    o Create various extractors and run them through
        //      an exhaustive loop test.
        //
        // Testing:
        //    stress test
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing..." << endl << "\tStress test" << endl;

        const int DEFAULT = -1000;
        int SIZE = argc > 2 ? atoi(argv[2]) : DEFAULT; // keep default standard
        if (!SIZE) SIZE = -DEFAULT;
        if (SIZE < 0) {
            SIZE = -SIZE;
            verbose = 0;
        }
 
        if (verbose) cout << "SIZE = " << SIZE << endl;
 
        //        vvvv  Adjust to make default test run a few (2-5) seconds.
        const int size = SIZE * 1;

        const int NUM_FEEDBACKS = 50;
        int feedback = size / NUM_FEEDBACKS;
        if (feedback <= 0) feedback = 1;
 

        const char *softDelim = " ,.\n";
        const char *hardDelim = "?";
  
        if (verbose) cout << 
                "\tLoop through all extractor methods" << endl;
        {
            if (verbose) cerr << "     " 
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";
                
            for (int i = size - 1; i >= 0; --i)  {
                if (verbose && 0 == i % feedback) cerr << '.';
 
              char buf[500];
              sprintf(buf, "%s", ", .Now ,, is .? the ??time ..for all good,men \n");
              istrstream s(buf);
              bdeut_StreamTokenExtractor it(softDelim, hardDelim); 
   
              ASSERT(0 == it.getLeader(&s))  
              ASSERT(0 == strcmp(", .",it()))
              ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("Now",it()))
              ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" ,, ",it()))
              ASSERT(0 == it.isHard())         
              ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("is",it()))
              ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" .? ",it()))
              ASSERT(1 == it.isHard())         
              ASSERT(0 == it.get(&s))          ASSERT(0 == strcmp("the",it()))
              ASSERT(0 == it.isHard())         
              ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp("?",it()))
              ASSERT(1 == it.isHard())         
              it.stripLeader(&s);              ASSERT(0 == strcmp("?",it()))
              it.stripToken(&s);               ASSERT(0 == strcmp("?",it()))
              it.stripDelimiter(&s);           ASSERT(0 == strcmp("?",it()))
              ASSERT(1 == it.isHard())         
              ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("for",it()))
              ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" ",it()))
              ASSERT(0 == it.isHard())         
              ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("all",it()))
              ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" ",it()))
              ASSERT(0 == it.isHard())         
              it.strip(&s);                    ASSERT(0 == strcmp(" ",it()))
              ASSERT(0 == it.isHard())         
              ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("men",it()))
              ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" \n",it()))
              ASSERT(0 == it.isHard())         
              ASSERT(1 == it.getLeader(&s))
  
              sprintf(buf, "%s", "??The,.\nlazy ? ?   ? ");
              istrstream s1(buf);
              ASSERT(0 == it.getLeader(&s1))   ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s1))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getDelimiter(&s1)) ASSERT(0 == strcmp("?",it()))
              ASSERT(1 == it.isHard())          
              ASSERT(0 == it.get(&s1))         ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.isHard())          
              it.stripLeader(&s1);             ASSERT(0 == strcmp("",it()))
              it.stripToken(&s1);              ASSERT(0 == strcmp("",it()))
              it.stripDelimiter(&s1);          ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.isHard())          
              ASSERT(0 == it.getLeader(&s1))   ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s1))    ASSERT(0 == strcmp("lazy",it()))
              ASSERT(0 == it.getDelimiter(&s1)) ASSERT(0 == strcmp(" ? ",it()))
              ASSERT(1 == it.isHard())          
              it.strip(&s1);                   ASSERT(0 == strcmp(" ? ",it()))
              ASSERT(1 == it.isHard())          
              ASSERT(0 == it.getLeader(&s1))   ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getToken(&s1))    ASSERT(0 == strcmp("",it()))
              ASSERT(0 == it.getDelimiter(&s1)) ASSERT(0 == strcmp("? ",it()))
              ASSERT(1 == it.isHard())          
              ASSERT(1 == it.getLeader(&s))
            }
            if (verbose) cerr << "END" << endl;
        }

        if (verbose) cout <<"Done. (stress...)" << endl;

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\tTesting \'listTokens\' example" << endl;
        {
          char buf[50];
          sprintf(buf, "%s", "\n Now\t\tis the    time... ");
          istrstream in(buf);
          listTokens(&in, " \t\n", verbose);
        }
        
        if (verbose) cout << "\tTesting \'pipe\' example" << endl;
        {
          char buf[80];
          sprintf(buf, "%s", "\t These/are /  /\nthe   good-old  / \tdays:: \n");
          istrstream s(buf);
          istrstream in("\t These/are /  /\nthe   good-old  / \tdays:: \n");
          ASSERT(1 == pipe(cout, &in, " \t\n", "/:", verbose))
        }
        
        const char *database = 
          "\n\n\n\n\n\n\n\n"
          "Costello | Lou     | 456-789-0123 | ...... | Comedian    | .....\n"
          "\n\n\n\n\n\n\n\n\n\n\n\n\n"
          "Einstein | Albert  | 345-678-9012 | ...... | Scientist   | .....\n"
          "Harding  | Tonya   | 234-567-8901 | ...... | Ice Skater  | .....\n"
          "\n\n"
          "Jordan   | Michael | 123-456-7890 | ...... | NBA Star    | .....\n"
          "Rambo    | John    | 567-890-1234 | ...... | US Marines  | .....\n"
          "\n\n\n\n\n\n";

        char buf[1000];
        sprintf(buf, "%s", database);
        istrstream s(buf);

        if (verbose) cout << "\tTesting \'getNamePhone2\' example" << endl;
        {
          istrstream in(buf);
          getNamePhone2(&in, verbose);
        }
        
        if (verbose) cout << "Done. (usage...)" << endl;
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // State machine: pseudo white box test
        //
        // Plan:
        //    o For each parsing function "pair" (state/input)
        //        o Create input that exercises every possible transition.
        //          (Identify coordinates of each transition, for each test.)
        //        o i.e.,
        //             input0   input1   input2   ...   inputN-1
        //           +--------+--------+--------+ ... +--------+
        //           |        |        |        |     |        | // state 0
        //           +--------+--------+--------+ ... +--------+
        //           |        |        |        |     |        | // state 1
        //           +--------+--------+--------+ ... +--------+
        //           :        :        :        :     :        :
        //           :        :        :        :     :        :
        //           +--------+--------+--------+ ... +--------+
        //           |        |        |        |     |        | // state M-1
        //           +--------+--------+--------+ ... +--------+
        //
        // Testing:
        //    state transition table
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing..." << "State Machine" << endl;

        const char *softDelims = " ";
        const char *hardDelims = "@";
        bdeut_StreamTokenExtractor it(softDelims, hardDelims);

        if (verbose) cout << "\tTesting delimiter state table" << endl;
        //    // TOK     SFT     HRD     EFF
        //    // ------  ------- ------- ------- 
        //    {  NHDTK,  NHDTK,  NHDTK,  NHDTK  },// INITL
        //    {  NHDTK,  NHDTK,  HARDD,  NHDTK  },// NHDTK
        //    {  NHDTK,  HARDD,  NHDTK,  NHDTK  },// HARDD

        {
          if (verbose) cout << "\t\tTesting NHDTK state" << endl;

          char buf1[1]; sprintf(buf1, "%s", "");
          istrstream eofInputFromNHDTK(buf1);    // homing: null, defining: null
          char buf2[2]; sprintf(buf2, "%s", "x");
          istrstream tokInputFromNHDTK(buf2);    // homing: null, defining: null
          char buf3[3]; sprintf(buf3, "%s", " @");
          istrstream sftInputFromNHDTK(buf3);    // homing: null, defining: hard
          char buf4[3]; sprintf(buf4, "%s", "@@");
          istrstream hrdInputFromNHDTK(buf4);    // homing: null, defining: hard
          ASSERT(1 == it.getDelimiter(&eofInputFromNHDTK))    
          ASSERT(0 == strcmp("", it()))      
          ASSERT(0 == it.getDelimiter(&tokInputFromNHDTK))    
          ASSERT(0 == strcmp("", it()))      
          ASSERT(0 == it.getDelimiter(&sftInputFromNHDTK))    
          ASSERT(0 == strcmp(" @", it()))      
          ASSERT(0 == it.getDelimiter(&hrdInputFromNHDTK))    
          ASSERT(0 == strcmp("@", it()))      
        }

        {
          if (verbose) cout << "\t\tTesting HARDD state" << endl;

          char buf1[2]; sprintf(buf1, "%s", "@");
          istrstream eofInputFromHARDD(buf1);   // homing: hard, defining: null
          char buf2[3]; sprintf(buf2, "%s", "@x");
          istrstream tokInputFromHARDD(buf2);   // homing: hard, defining: null
          char buf3[4]; sprintf(buf3, "%s", "@ @");
          istrstream sftInputFromHARDD(buf3);   // homing: hard, defining: hard
          char buf4[4]; sprintf(buf4, "%s", "@@@");
          istrstream hrdInputFromHARDD(buf4);   // homing: hard, defining: hard
          ASSERT(0 == it.getDelimiter(&eofInputFromHARDD))    
          ASSERT(0 == strcmp("@", it()))      
          ASSERT(0 == it.getDelimiter(&tokInputFromHARDD))    
          ASSERT(0 == strcmp("@", it()))      
          ASSERT(0 == it.getDelimiter(&sftInputFromHARDD))    
          ASSERT(0 == strcmp("@ ", it()))      
          ASSERT(0 == it.getDelimiter(&hrdInputFromHARDD))    
          ASSERT(0 == strcmp("@", it()))      
        }

        if (verbose) cout << "\tTesting token state table" << endl;
        //    // TOK     SFT     HRD     EFF
        //    // ------  ------- ------- ------- 
        //    {  NHDTK,  NHDTK,  NHDTK,  NHDTK  },// INITL
        //    {  TOKEN,  NHDTK,  HARDD,  NHDTK  },// NHDTK
        //    {  NHDTK,  SOFTD,  HARDD,  NHDTK  },// SOFTD
        //    {  NHDTK,  HARDD,  NHDTK,  NHDTK  },// HARDD
        //    {  TOKEN,  SOFTD,  HARDD,  NHDTK  },// TOKEN

        {
          if (verbose) cout << "\t\tTesting NHDTK state" << endl;

          char buf1[1]; sprintf(buf1, "%s", "");
          istrstream eofInputFromNHDTK(buf1);    // homing: null, defining: null
          char buf2[3]; sprintf(buf2, "%s", "x@");
          istrstream tokInputFromNHDTK(buf2);   // homing: null, defining: hard
          char buf3[3]; sprintf(buf3, "%s", " @");
          istrstream sftInputFromNHDTK(buf3);   // homing: null, defining: hard
          char buf4[3]; sprintf(buf4, "%s", "@@");
          istrstream hrdInputFromNHDTK(buf4);   // homing: null, defining: hard
          ASSERT(1 == it.get(&eofInputFromNHDTK))    
          ASSERT(0 == strcmp("@", it()))      
          ASSERT(0 == it.get(&tokInputFromNHDTK))    
          ASSERT(0 == strcmp("x", it()))      
          ASSERT(0 == it.get(&sftInputFromNHDTK))    
          ASSERT(0 == strcmp("", it()))      
          ASSERT(0 == it.get(&hrdInputFromNHDTK))    
          ASSERT(0 == strcmp("", it()))      
        }

        {
          if (verbose) cout << "\t\tTesting HARDD state" << endl;

          char buf1[2]; sprintf(buf1, "%s", "@");
          istrstream eofInputFromHARDD(buf1);   // homing: hard, defining: null
          char buf2[4]; sprintf(buf2, "%s", "@x@");
          istrstream tokInputFromHARDD(buf2);   // homing: hard, defining: hard
          char buf3[4]; sprintf(buf3, "%s", "@ @");
          istrstream sftInputFromHARDD(buf3);   // homing: hard, defining: hard
          char buf4[4]; sprintf(buf4, "%s", "@@@");
          istrstream hrdInputFromHARDD(buf4);   // homing: hard, defining: hard
          ASSERT(0 == it.get(&eofInputFromHARDD))    
          ASSERT(0 == strcmp("", it()))      
          ASSERT(0 == it.get(&tokInputFromHARDD))    
          ASSERT(0 == strcmp("", it()))      
          ASSERT(0 == it.get(&sftInputFromHARDD))    
          ASSERT(0 == strcmp("", it()))      
          ASSERT(0 == it.get(&hrdInputFromHARDD))    
          ASSERT(0 == strcmp("", it()))      
        }

        {
          if (verbose) cout << "\t\tTesting SOFTD state" << endl;

          char buf1[3]; sprintf(buf1, "%s", "x ");
          istrstream eofInputFromHARDD(buf1);   // h: tok-sft, defining: null
          char buf2[5]; sprintf(buf2, "%s", "x x@");
          istrstream tokInputFromHARDD(buf2);   // h: tok-sft, defining: hard
          char buf3[5]; sprintf(buf3, "%s", "x  @");
          istrstream sftInputFromHARDD(buf3);   // h: tok-sft, defining: hard
          char buf4[5]; sprintf(buf4, "%s", "x @@");
          istrstream hrdInputFromHARDD(buf4);   // h: tok-sft, defining: hard
          ASSERT(0 == it.get(&eofInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
          ASSERT(0 == it.get(&tokInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
          ASSERT(0 == it.get(&sftInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
          ASSERT(0 == it.get(&hrdInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
        }

        {
          if (verbose) cout << "\t\tTesting TOKEN state" << endl;

          char buf1[2]; sprintf(buf1, "%s", "x");
          istrstream eofInputFromHARDD(buf1);   // homing: tok, defining: null
          char buf2[4]; sprintf(buf2, "%s", "xx@");
          istrstream tokInputFromHARDD(buf2);   // homing: tok, defining: hard
          char buf3[4]; sprintf(buf3, "%s", "x @");
          istrstream sftInputFromHARDD(buf3);   // homing: tok, defining: hard
          char buf4[4]; sprintf(buf4, "%s", "x@@");
          istrstream hrdInputFromHARDD(buf4);   // homing: tok, defining: hard
          ASSERT(0 == it.get(&eofInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
          ASSERT(0 == it.get(&tokInputFromHARDD))    
          ASSERT(0 == strcmp("xx", it()))      
          ASSERT(0 == it.get(&sftInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
          ASSERT(0 == it.get(&hrdInputFromHARDD))    
          ASSERT(0 == strcmp("x", it()))      
        }

        if (verbose) cout << "Done. (State Machine...)" << endl;
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // Buffer contents and stream status: Boundry conditions
        //
        // Plan:
        //    o Ensure buffer is not loaded on eos.
        //    o Ensure 'get' on empty stream causes stream to change state.
        //    o Ensure 'get' on non-empty stream doesn't effect the state.
        //    o Ensure 'strip' never affects the state of the stream.
        //
        // Testing:
        //    stream state changes
        //    buffer contents
        //    'get' return status
        // --------------------------------------------------------------------

        if (verbose) cout << "Done. (stream and buffer status...)" << endl;
        if (verbose) cout << "Testing..." 
                          << "Buffer contents and stream status"  << endl;

        if (verbose) cout <<  "\t\t\t| softDelims\t| hardDelims" << endl;
        if (verbose) cout <<  "\t\t\t| ----------\t ----------" << endl;

        const char *softDelims = " ";
        const char *hardDelims = "@";
        const char * const str = "  xxx @ @  xxxx@@xxx @  ";

        {
          if (verbose) cout << "\tstream: \"\"\t|     0\t\t|     0" << endl;
          bdeut_StreamTokenExtractor it(0, 0);
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) cout <<"\tstream: \"\"\t|     0\t\t|     \"\"" <<endl;
          istrstream in("");
          bdeut_StreamTokenExtractor it(0, "");
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: \"\"\t|     0\t\t| hardDelims" << endl;
          bdeut_StreamTokenExtractor it(0, hardDelims);
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) cout << "\tstream: \"\"\t|     \"\"\t|     0" << endl;
          bdeut_StreamTokenExtractor it("", 0);
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: \"\"\t|     \"\"\t|     \"\"" << endl;
          bdeut_StreamTokenExtractor it("", "");
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: \"\"\t|     \"\"\t| hardDelims" << endl;
          bdeut_StreamTokenExtractor it("", hardDelims);
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: \"\"\t| softDelims\t|     0" << endl;
          bdeut_StreamTokenExtractor it(softDelims, 0);
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: \"\"\t| softDelims\t|     \"\"" << endl;
          bdeut_StreamTokenExtractor it(softDelims, "");
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: \"\"\t| softDelims\t| hardDelims" << endl;
          bdeut_StreamTokenExtractor it(softDelims, hardDelims);
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(1 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))      ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == *it())
          it.stripLeader(&in5);               ASSERT(0 == *it())
          it.stripToken(&in6);                ASSERT(0 == *it())
          it.stripDelimiter(&in7);            ASSERT(0 == *it())
          it.strip(&in8);                     ASSERT(0 == *it())
        }

        {
          if (verbose) 
              cout << "\tstream: stream\t|     0\t\t|     0" << endl;
          bdeut_StreamTokenExtractor it(0, 0);
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp(str, it()))
          ASSERT(1 == it.getDelimiter(&in1))  ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp(str, it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp(str, it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp(str, it()))
          it.strip(&in8);                     ASSERT(0 == strcmp(str, it()))
        }

        {
          if (verbose)
              cout << "\tstream: stream\t|     0\t\t|     \"\"" << endl;
          bdeut_StreamTokenExtractor it(0, "");
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp(str, it()))
          ASSERT(1 == it.getDelimiter(&in1))  ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp(str, it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp(str, it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp(str, it()))
          it.strip(&in8);                     ASSERT(0 == strcmp(str, it()))
        }

        {
          if (verbose)
              cout << "\tstream: stream\t|     0\t\t| hardDelims" << endl;
          bdeut_StreamTokenExtractor it("", hardDelims);
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))   ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in1))    ASSERT(0 == strcmp("  xxx ", it()))
          it.stripDelimiter(&in1);          ASSERT(0 == strcmp("  xxx ", it()))
          ASSERT(0 == it.getDelimiter(&in1))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getToken(&in2))    ASSERT(0 == strcmp("  xxx ", it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))         ASSERT(0 == strcmp("  xxx ", it()))
          it.stripLeader(&in5);             ASSERT(0 == strcmp("  xxx ", it()))
          it.stripToken(&in6);              ASSERT(0 == strcmp("  xxx ", it()))
          it.stripDelimiter(&in7);          ASSERT(0 == strcmp("  xxx ", it()))
          it.strip(&in8);                   ASSERT(0 == strcmp("  xxx ", it()))
        }

        {
          if (verbose)
              cout <<"\tstream: stream\t|     \"\"\t|     0" << endl;
          bdeut_StreamTokenExtractor it("", 0);
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp(str, it()))
          ASSERT(1 == it.getDelimiter(&in1))  ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp(str, it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp(str, it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp(str, it()))
          it.strip(&in8);                     ASSERT(0 == strcmp(str, it()))
        }

       {
          if (verbose)
              cout << "\tstream: stream\t|     \"\"\t|     \"\"" << endl;
          bdeut_StreamTokenExtractor it("", "");
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp(str, it()))
          ASSERT(1 == it.getDelimiter(&in1))  ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp(str, it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp(str, it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp(str, it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp(str, it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp(str, it()))
          it.strip(&in8);                     ASSERT(0 == strcmp(str, it()))
        }

        {
          if (verbose)
              cout << "\tstream: stream\t|     \"\"\t| hardDelims" << endl;
          bdeut_StreamTokenExtractor it("", hardDelims);
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))   ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in1))    ASSERT(0 == strcmp("  xxx ", it()))
          it.stripDelimiter(&in1);          ASSERT(0 == strcmp("  xxx ", it()))
          ASSERT(0 == it.getDelimiter(&in1))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getToken(&in2))    ASSERT(0 == strcmp("  xxx ", it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))         ASSERT(0 == strcmp("  xxx ", it()))
          it.stripLeader(&in5);             ASSERT(0 == strcmp("  xxx ", it()))
          it.stripToken(&in6);              ASSERT(0 == strcmp("  xxx ", it()))
          it.stripDelimiter(&in7);          ASSERT(0 == strcmp("  xxx ", it()))
          it.strip(&in8);                   ASSERT(0 == strcmp("  xxx ", it()))
        }

        {
          if (verbose)
              cout << "\tstream: stream\t| softDelims\t|     0" << endl;
          bdeut_StreamTokenExtractor it(softDelims, 0);
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("  ", it()))
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp("xxx", it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.getDelimiter(&in1))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("  ", it()))
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp("xxx", it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp("xxx", it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp("xxx", it()))
          it.strip(&in8);                     ASSERT(0 == strcmp("xxx", it()))
        }

        {
          if (verbose)
              cout << "\tstream: stream\t| softDelims\t|     \"\"" << endl;
          bdeut_StreamTokenExtractor it(softDelims, "");
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("  ", it()))
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp("xxx", it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.getDelimiter(&in1))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("  ", it()))
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp("xxx", it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp("xxx", it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp("xxx", it()))
          it.strip(&in8);                     ASSERT(0 == strcmp("xxx", it()))
        }

        {
          if (verbose)
              cout << "\tstream: stream\t| softDelims\t| hardDelims" << endl;
          bdeut_StreamTokenExtractor it(softDelims, hardDelims);
          char buf[100]; sprintf(buf, "%s", str);
          istrstream in1(buf); istrstream in2(buf); istrstream in3(buf); 
          istrstream in4(buf); istrstream in5(buf); istrstream in6(buf);
          istrstream in7(buf); istrstream in8(buf);
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("  ", it()))
          ASSERT(0 == it.getToken(&in1))      ASSERT(0 == strcmp("xxx", it()))
          it.stripDelimiter(&in1);            ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.getDelimiter(&in1))  ASSERT(0 == strcmp("@  ", it()))
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("  ", it()))
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          it.stripLeader(&in5);               ASSERT(0 == strcmp("xxx", it()))
          it.stripToken(&in6);                ASSERT(0 == strcmp("xxx", it()))
          it.stripDelimiter(&in7);            ASSERT(0 == strcmp("xxx", it()))
          it.strip(&in8);                     ASSERT(0 == strcmp("xxx", it()))
        }

        if (verbose) cout << "Done. (stream and buffer status...)" << endl;
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // 'strip' functions: Black Box
        //
        // Plan:
        //    o Create various kinds of extractors.
        //    o Apply each strip function to a variety of input streams.
        //    o Use 'get' to do initial load and verify contents of the 
        //      buffer do not change.
        //
        // Testing:
        //    int stripLeader(istream *input)
        //    int stripToken(istream *input)
        //    int stripDelimiter(istream *input)
        //    int strip(istream *input)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing..." << "\'strip\' functions" << endl;

        const char *softDelims = " ";
        const char *hardDelims = "@";
        bdeut_StreamTokenExtractor it(softDelims, hardDelims); 

        {
          if (verbose) cout << 
            "\tTesting case with empty istream" << endl;
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(1 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with all soft delimiter characters only" << endl;
          char buf[17]; sprintf(buf, "%s", "                ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(1 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with all hard delimiter characters only" << endl;
          char buf[11]; sprintf(buf, "%s", "@@@@@@@@@@");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("@", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("@", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("@", it()))
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with all token characters only" << endl;
          char buf[11]; sprintf(buf, "%s", "xxxxxxxxxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(1 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-token" << endl;
          char buf[6]; sprintf(buf, "%s", "  xxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-hard-soft-eof" << endl;
          char buf[5]; sprintf(buf, "%s", "  @ ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-hard-soft-hard" << endl;
          char buf[6]; sprintf(buf, "%s", "  @ @");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("@", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("@", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("@", it()))
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-hard-soft-token" << endl;
          char buf[6]; sprintf(buf, "%s", "  @ x");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("x", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("x", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-eof" << endl;
          char buf[7]; sprintf(buf, "%s", "xxx   ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == strcmp("x", it()))
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-hard-soft-eof" << endl;
          char buf[9]; sprintf(buf, "%s", "xxx  @  ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-hard-soft-hard" << endl;
          char buf[10]; sprintf(buf, "%s", "xxx  @  @");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-hard-soft-token" << endl;
          char buf[14]; sprintf(buf, "%s", "xxx   @   xxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("xxx", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("xxx", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with hard-soft-eof" << endl;
          char buf[4]; sprintf(buf, "%s", "@  ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(1 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(1 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with hard-soft-hard" << endl;
          char buf[5]; sprintf(buf, "%s", "@  @");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == *it())
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("@", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("@", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("@", it()))
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("", it()))
        }

        {
          if (verbose) cout << 
            "\tTesting case with hard-soft-token" << endl;
          char buf[7]; sprintf(buf, "%s", "@  xxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          it.stripLeader(&in1);              ASSERT(0 == *it())
          ASSERT(0 == it.getLeader(&in1))    ASSERT(0 == strcmp("", it()))
          it.stripLeader(&in1);              ASSERT(0 == strcmp("", it()))
          it.stripToken(&in2);               ASSERT(0 == *it())
          ASSERT(0 == it.getToken(&in2))     ASSERT(0 == strcmp("", it()))
          it.stripToken(&in1);               ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.getDelimiter(&in3)) ASSERT(0 == strcmp("", it()))
          it.stripDelimiter(&in3);           ASSERT(0 == strcmp("", it()))
          it.strip(&in4);                    ASSERT(0 == *it())
          ASSERT(0 == it.get(&in4))          ASSERT(0 == strcmp("xxx", it()))
          it.strip(&in4);                    ASSERT(0 == strcmp("xxx", it()))
        }

        if (verbose) cout << "Done. (\'strip\' functions...)" << endl;

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // 'get' functions: Black Box test
        //
        // Plan:
        //    o Create various kinds of extractors.
        //    o Apply each get function to a variety of input streams.
        //    o Verify contents of the buffer.
        //    o Verify buffer is/is not hard.
        //    o Verify buffer has/has not soft.
        //    o Verify return status.
        //
        // Testing:
        //    bdeut_StreamTokenExtractor(const char *soft, const char *hard= 0)
        //    ~bdeut_StreamTokenExtractor() - via purify
        //    int getLeader(istream *input)
        //    int getToken(istream *input)
        //    int getDelimiter(istream *input)
        //    int get(istream *input)
        //    const char *operator()() const
        //    int isHard() const
        //    int hasSoft() const
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing..." << "\'get\' functions()" << endl;

        int i;
        char all[256];
        for (i = 0; i < 256; ++i) {
            all[i] = i+1;
        }
            
        {
          if (verbose) cout << 
            "\tTesting case where all chars are soft delimiters" << endl;
            istrstream in(all);
            bdeut_StreamTokenExtractor it(all); 
            ASSERT(0 == it.getLeader(&in))  
            for (i = 0; i < 256; ++i) {
                LOOP_ASSERT(i, all[i] == *(it() + i))
            }
            ASSERT(1 == it.getToken(&in)) 
            for (i = 0; i < 256; ++i) {
                LOOP_ASSERT(i, all[i] == *(it() + i))
            }
        }

        {
          if (verbose) cout << 
            "\tTesting case where all chars are hard delimiters" << endl;
            istrstream in(all);
            bdeut_StreamTokenExtractor it(0, all);
            ASSERT(0 == it.getLeader(&in))    ASSERT(0 == strcmp("", it()))
            ASSERT(0 == it.getToken(&in))     ASSERT(0 == strcmp("", it()))
            ASSERT(0 == it.getDelimiter(&in)) ASSERT(all[0] == *it())
            ASSERT(0 == it.hasSoft()); ASSERT(1 == it.isHard());
        }

        {
            istrstream in(all);
            bdeut_StreamTokenExtractor it(all, all);
            ASSERT(0 == it.getLeader(&in))    ASSERT(0 == strcmp("", it()))
            ASSERT(0 == it.getToken(&in))     ASSERT(0 == strcmp("", it()))
            ASSERT(0 == it.getDelimiter(&in)) ASSERT(all[0] == *it())
            ASSERT(0 == it.hasSoft()); ASSERT(1 == it.isHard());
        }

        {
          if (verbose) cout << 
            "\tTesting case where no chars are delimiters" << endl;
            istrstream in(all);
            bdeut_StreamTokenExtractor it(0, 0); 
            ASSERT(0 == it.getLeader(&in))     ASSERT(0 == strcmp("", it()))
            ASSERT(0 == it.getToken(&in))      
            for (i = 0; i < 256; ++i) {
                LOOP_ASSERT(i, all[i] == *(it() + i))
            }
            ASSERT(1 == it.getDelimiter(&in)) 
            for (i = 0; i < 256; ++i) {
                LOOP_ASSERT(i, all[i] == *(it() + i))
            }
            ASSERT(0 == it.hasSoft()); ASSERT(0 == it.isHard());
        }

        const char *softDelims = " ";
        const char *hardDelims = "@";
        bdeut_StreamTokenExtractor it(softDelims, hardDelims); 

        {
          if (verbose) cout << 
            "\tTesting case with empty istream" << endl;
          char buf[1]; sprintf(buf, "%s", "");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(1 == it.getLeader(&in1))       ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(1 == it.getToken(&in2))        ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(1 == it.getDelimiter(&in3))    ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))             ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with all soft delimiter characters only" << endl;
          char buf[17]; sprintf(buf, "%s", "                ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))       ASSERT(0 == strcmp(buf, it()))
          ASSERT(1 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))        ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))    ASSERT(0 == strcmp(buf, it()))
          ASSERT(1 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))             ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))             ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with all hard delimiter characters only" << endl;
          char buf[11]; sprintf(buf, "%s", "@@@@@@@@@@");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))       ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))        ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))    ASSERT(0 == strcmp("@", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))             ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))             ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with all token characters only" << endl;
          char buf[11]; sprintf(buf, "%s", "xxxxxxxxxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))       ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))        ASSERT(0 == strcmp(buf, it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))    ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))             ASSERT(0 == strcmp(buf, it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))             ASSERT(0 == strcmp(buf, it()))
          ASSERT(0 == it.hasSoft())             ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-token" << endl;
          char buf[6]; sprintf(buf, "%s", "  xxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))      ASSERT(0 == strcmp("  ", it()))
          ASSERT(1 == it.hasSoft())            ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))       ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())            ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))   ASSERT(0 == strcmp("  ", it()))
          ASSERT(1 == it.hasSoft())            ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))            ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())            ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))            ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())            ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-hard-soft-eof" << endl;
          char buf[5]; sprintf(buf, "%s", "  @ ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("  ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("  @ ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-hard-soft-hard" << endl;
          char buf[6]; sprintf(buf, "%s", "  @ @");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("  ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("  @ ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with soft-hard-soft-token" << endl;
          char buf[6]; sprintf(buf, "%s", "  @ x");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("  ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("  @ ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("x", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-eof" << endl;
          char buf[7]; sprintf(buf, "%s", "xxx   ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-hard-soft-eof" << endl;
          char buf[9]; sprintf(buf, "%s", "xxx  @  ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-hard-soft-hard" << endl;
          char buf[10]; sprintf(buf, "%s", "xxx  @  @");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with token-soft-hard-soft-token" << endl;
          char buf[14]; sprintf(buf, "%s", "xxx   @   xxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with hard-soft-eof" << endl;
          char buf[4]; sprintf(buf, "%s", "@  ");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("@  ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(1 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with hard-soft-hard" << endl;
          char buf[5]; sprintf(buf, "%s", "@  @");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("@  ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        {
          if (verbose) cout << 
            "\tTesting case with hard-soft-token" << endl;
          char buf[7]; sprintf(buf, "%s", "@  xxx");
          istrstream in1(buf); istrstream in2(buf); 
          istrstream in3(buf); istrstream in4(buf); 
          ASSERT(0 == it.getLeader(&in1))     ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getToken(&in2))      ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.getDelimiter(&in3))  ASSERT(0 == strcmp("@  ", it()))
          ASSERT(1 == it.hasSoft())           ASSERT(1 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
          ASSERT(0 == it.get(&in4))           ASSERT(0 == strcmp("xxx", it()))
          ASSERT(0 == it.hasSoft())           ASSERT(0 == it.isHard())
        }

        if (verbose) cout << "Done. (\'get\' functions...)" << endl;

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // Breathing Test: 
        //
        // Plan:
        //    o create a StreamTokenExtractor(const char*, const char*)
        //    o verify basic correctness.
        //
        // Testing:
        //    bdeut_StreamTokenExtractor(const char *soft, const char *hard= 0)
        //    ~bdeut_StreamTokenExtractor()
        //    int getLeader(istream *input)
        //    int getToken(istream *input)
        //    int getDelimiter(istream *input)
        //    int get(istream *input)
        //    void stripLeader(istream *input)
        //    void stripToken(istream *input)
        //    void stripDelimiter(istream *input)
        //    void strip(istream *input)
        //    const char *operator()() const
        //    int isHard() const
        // --------------------------------------------------------------------

        if (verbose) cout << 
            endl << "Breathing test" << endl;

        char buf[100]; 
        sprintf(buf, "%s", ", .Now ,, is .? the ??time ..for all good,men \n");
        istrstream s(buf);

        const char *softDelim = " ,.\n";
        const char *hardDelim = "?";
  
        {
            bdeut_StreamTokenExtractor it(softDelim, hardDelim); 
   
            ASSERT(0 == it.getLeader(&s))  
            ASSERT(0 == strcmp(", .",it()))
            ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("Now",it()))
            ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" ,, ",it()))
            ASSERT(0 == it.isHard())         
            ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("is",it()))
            ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" .? ",it()))
            ASSERT(1 == it.isHard())         
            ASSERT(0 == it.get(&s))          ASSERT(0 == strcmp("the",it()))
            ASSERT(0 == it.isHard())         
            ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp("?",it()))
            ASSERT(1 == it.isHard())         
            it.stripLeader(&s);              ASSERT(0 == strcmp("?",it()))
            it.stripToken(&s);               ASSERT(0 == strcmp("?",it()))
            it.stripDelimiter(&s);           ASSERT(0 == strcmp("?",it()))
            ASSERT(1 == it.isHard())         
            ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("for",it()))
            ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" ",it()))
            ASSERT(0 == it.isHard())         
            ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("all",it()))
            ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" ",it()))
            ASSERT(0 == it.isHard())         
            it.strip(&s);                    ASSERT(0 == strcmp(" ",it()))
            ASSERT(0 == it.isHard())         
            ASSERT(0 == it.getLeader(&s))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s))     ASSERT(0 == strcmp("men",it()))
            ASSERT(0 == it.getDelimiter(&s)) ASSERT(0 == strcmp(" \n",it()))
            ASSERT(0 == it.isHard())         
            ASSERT(1 == it.getLeader(&s))

            sprintf(buf, "??The,.\nlazy ? ?   ? ");
            istrstream s1(buf);
            ASSERT(0 == it.getLeader(&s1))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s1))     ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getDelimiter(&s1)) ASSERT(0 == strcmp("?",it()))
            ASSERT(1 == it.isHard())          
            ASSERT(0 == it.get(&s1))          ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.isHard())          
            it.stripLeader(&s1);              ASSERT(0 == strcmp("",it()))
            it.stripToken(&s1);               ASSERT(0 == strcmp("",it()))
            it.stripDelimiter(&s1);           ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.isHard())          
            ASSERT(0 == it.getLeader(&s1))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s1))     ASSERT(0 == strcmp("lazy",it()))
            ASSERT(0 == it.getDelimiter(&s1)) ASSERT(0 == strcmp(" ? ",it()))
            ASSERT(1 == it.isHard())          
            it.strip(&s1);                    ASSERT(0 == strcmp(" ? ",it()))
            ASSERT(1 == it.isHard())          
            ASSERT(0 == it.getLeader(&s1))    ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getToken(&s1))     ASSERT(0 == strcmp("",it()))
            ASSERT(0 == it.getDelimiter(&s1)) ASSERT(0 == strcmp("? ",it()))
            ASSERT(1 == it.isHard())          
            ASSERT(1 == it.getLeader(&s))
        }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
