// baejsn_encoder.t.cpp                                               -*-C++-*-
#include <baejsn_encoder.h>
#include <baejsn_decoder.h>

#include <bdem_schema.h>
#include <bdem_recorddef.h>

#include <bsl_string.h>
#include <bdeat_attributeinfo.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_enumeratorinfo.h>
#include <bdeat_selectioninfo.h>
#include <bdeat_sequencefunctions.h>
#include <bdeat_valuetypefunctions.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdex_instream.h>
#include <bdex_outstream.h>
#include <bsl_sstream.h>

#include <bdede_utf8util.h>
#include <bdem_row.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_table.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdeu_printmethods.h>  // for printing vector
#include <bdeu_chartype.h>

// These header are for testing only and the hierarchy level of baejsn was
// increase because of them.  They should be remove when possible.
#include <baea_testmessages.h>
#include <baea_serializableobjectproxy.h>
#include <baea_serializableobjectproxyutil.h>
#include <baexml_decoder.h>
#include <baexml_minireader.h>
#include <baexml_errorinfo.h>

#include <iostream>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP0_ASSERT ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baejsn_Encoder            Obj;
typedef baejsn_Encoder_EncodeImpl Impl;
typedef bsls::Types::Int64        Int64;
typedef bsls::Types::Uint64       Uint64;

static const char* XML_TEST_MESSAGES[] = {

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection1><element1>0</element1><element2>custom</element2><element3>99"
"9</element3><element4>0</element4><element5>999</element5><element5>999</elem"
"ent5><element6>custom</element6><element7>999</element7><element8>0</element8"
"><element8>0</element8><element9>custom</element9><element9>custom</element9>"
"<element10>0</element10><element10>0</element10><element11>999</element11><el"
"ement11>999</element11></selection1></element1><element2><selection2>1.5</sel"
"ection2></element2><element2><selection2>1.5</selection2></element2><element3"
"><selection2></selection2></element3><element4><selection1><element1>0</eleme"
"nt1><element2>custom</element2><element3>999</element3><element4>0</element4>"
"<element5>999</element5><element5>999</element5><element6>custom</element6><e"
"lement7>999</element7><element8>0</element8><element8>0</element8><element9>c"
"ustom</element9><element9>custom</element9><element10>0</element10><element10"
">0</element10><element11>999</element11><element11>999</element11></selection"
"1></element4><element4><selection1><element1>0</element1><element2>custom</el"
"ement2><element3>999</element3><element4>0</element4><element5>999</element5>"
"<element5>999</element5><element6>custom</element6><element7>999</element7><e"
"lement8>0</element8><element8>0</element8><element9>custom</element9><element"
"9>custom</element9><element10>0</element10><element10>0</element10><element11"
">999</element11><element11>999</element11></selection1></element4></selection"
"1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt3><selection1>true</selection1></element3></selection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection1><element1>255</element1><element4>255</element4><element6>cus"
"tom</element6><element7>999</element7><element8>255</element8><element8>255</"
"element8><element10>255</element10><element10>255</element10></selection1></e"
"lement1><element2><selection3><element8>true</element8><element9></element9><"
"element10>1.5</element10><element11>FF0001</element11><element12>2</element12"
"><element13>LONDON</element13></selection3></element2><element2><selection3><"
"element8>true</element8><element9></element9><element10>1.5</element10><eleme"
"nt11>FF0001</element11><element12>2</element12><element13>LONDON</element13><"
"/selection3></element2><element3><selection2>arbitrary string value</selectio"
"n2></element3><element4><selection1><element1>255</element1><element4>255</el"
"ement4><element6>custom</element6><element7>999</element7><element8>255</elem"
"ent8><element8>255</element8><element10>255</element10><element10>255</elemen"
"t10></selection1></element4><element4><selection1><element1>255</element1><el"
"ement4>255</element4><element6>custom</element6><element7>999</element7><elem"
"ent8>255</element8><element8>255</element8><element10>255</element10><element"
"10>255</element10></selection1></element4></selection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection2>255</selection2></element1><element2><selection3><element1><e"
"lement1>LONDON</element1><element1>LONDON</element1><element2>arbitrary strin"
"g value</element2><element2>arbitrary string value</element2><element3>true</"
"element3><element4>arbitrary string value</element4><element5><element1><elem"
"ent1>LONDON</element1><element1>LONDON</element1><element2>arbitrary string v"
"alue</element2><element2>arbitrary string value</element2><element3>true</ele"
"ment3><element4>arbitrary string value</element4><element5><element1><element"
"1>LONDON</element1><element1>LONDON</element1><element2>arbitrary string valu"
"e</element2><element2>arbitrary string value</element2><element3>true</elemen"
"t3><element4>arbitrary string value</element4><element6>LONDON</element6><ele"
"ment6>LONDON</element6></element1><element2>true</element2><element2>true</el"
"ement2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</elem"
"ent4><element4>FF0001</element4><element5>2</element5><element5>2</element5><"
"element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element"
"6></element5><element6>LONDON</element6><element6>LONDON</element6></element1"
"><element2>true</element2><element2>true</element2><element3>1.5</element3><e"
"lement3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><"
"element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</ele"
"ment6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</el"
"ement6><element6>LONDON</element6></element1><element1><element1>LONDON</elem"
"ent1><element1>LONDON</element1><element2>arbitrary string value</element2><e"
"lement2>arbitrary string value</element2><element3>true</element3><element4>a"
"rbitrary string value</element4><element5><element1><element1>LONDON</element"
"1><element1>LONDON</element1><element2>arbitrary string value</element2><elem"
"ent2>arbitrary string value</element2><element3>true</element3><element4>arbi"
"trary string value</element4><element5><element1><element1>LONDON</element1><"
"element1>LONDON</element1><element2>arbitrary string value</element2><element"
"2>arbitrary string value</element2><element3>true</element3><element4>arbitra"
"ry string value</element4><element6>LONDON</element6><element6>LONDON</elemen"
"t6></element1><element2>true</element2><element2>true</element2><element3>1.5"
"</element3><element3>1.5</element3><element4>FF0001</element4><element4>FF000"
"1</element4><element5>2</element5><element5>2</element5><element6>2012-08-18T"
"13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><elemen"
"t6>LONDON</element6><element6>LONDON</element6></element1><element2>true</ele"
"ment2><element2>true</element2><element3>1.5</element3><element3>1.5</element"
"3><element4>FF0001</element4><element4>FF0001</element4><element5>2</element5"
"><element5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012"
"-08-18T13:25:00</element6></element5><element6>LONDON</element6><element6>LON"
"DON</element6></element1><element2><selection2>1.5</selection2></element2><el"
"ement2><selection2>1.5</selection2></element2><element3>FF0001</element3><ele"
"ment4>2</element4><element5>2012-08-18T13:25:00</element5><element6>custom</e"
"lement6><element7>LONDON</element7><element8>true</element8><element9>arbitra"
"ry string value</element9><element10>1.5</element10><element11>FF0001</elemen"
"t11><element12>2</element12><element13>LONDON</element13><element14>true</ele"
"ment14><element14>true</element14><element15>1.5</element15><element15>1.5</e"
"lement15><element16>FF0001</element16><element16>FF0001</element16><element17"
">2</element17><element17>2</element17><element18>2012-08-18T13:25:00</element"
"18><element18>2012-08-18T13:25:00</element18><element19>custom</element19><el"
"ement19>custom</element19></selection3></element2><element2><selection3><elem"
"ent1><element1>LONDON</element1><element1>LONDON</element1><element2>arbitrar"
"y string value</element2><element2>arbitrary string value</element2><element3"
">true</element3><element4>arbitrary string value</element4><element5><element"
"1><element1>LONDON</element1><element1>LONDON</element1><element2>arbitrary s"
"tring value</element2><element2>arbitrary string value</element2><element3>tr"
"ue</element3><element4>arbitrary string value</element4><element5><element1><"
"element1>LONDON</element1><element1>LONDON</element1><element2>arbitrary stri"
"ng value</element2><element2>arbitrary string value</element2><element3>true<"
"/element3><element4>arbitrary string value</element4><element6>LONDON</elemen"
"t6><element6>LONDON</element6></element1><element2>true</element2><element2>t"
"rue</element2><element3>1.5</element3><element3>1.5</element3><element4>FF000"
"1</element4><element4>FF0001</element4><element5>2</element5><element5>2</ele"
"ment5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</"
"element6></element5><element6>LONDON</element6><element6>LONDON</element6></e"
"lement1><element2>true</element2><element2>true</element2><element3>1.5</elem"
"ent3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</ele"
"ment4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:"
"00</element6><element6>2012-08-18T13:25:00</element6></element5><element6>LON"
"DON</element6><element6>LONDON</element6></element1><element1><element1>LONDO"
"N</element1><element1>LONDON</element1><element2>arbitrary string value</elem"
"ent2><element2>arbitrary string value</element2><element3>true</element3><ele"
"ment4>arbitrary string value</element4><element5><element1><element1>LONDON</"
"element1><element1>LONDON</element1><element2>arbitrary string value</element"
"2><element2>arbitrary string value</element2><element3>true</element3><elemen"
"t4>arbitrary string value</element4><element5><element1><element1>LONDON</ele"
"ment1><element1>LONDON</element1><element2>arbitrary string value</element2><"
"element2>arbitrary string value</element2><element3>true</element3><element4>"
"arbitrary string value</element4><element6>LONDON</element6><element6>LONDON<"
"/element6></element1><element2>true</element2><element2>true</element2><eleme"
"nt3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><element"
"4>FF0001</element4><element5>2</element5><element5>2</element5><element6>2012"
"-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5>"
"<element6>LONDON</element6><element6>LONDON</element6></element1><element2>tr"
"ue</element2><element2>true</element2><element3>1.5</element3><element3>1.5</"
"element3><element4>FF0001</element4><element4>FF0001</element4><element5>2</e"
"lement5><element5>2</element5><element6>2012-08-18T13:25:00</element6><elemen"
"t6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6><eleme"
"nt6>LONDON</element6></element1><element2><selection2>1.5</selection2></eleme"
"nt2><element2><selection2>1.5</selection2></element2><element3>FF0001</elemen"
"t3><element4>2</element4><element5>2012-08-18T13:25:00</element5><element6>cu"
"stom</element6><element7>LONDON</element7><element8>true</element8><element9>"
"arbitrary string value</element9><element10>1.5</element10><element11>FF0001<"
"/element11><element12>2</element12><element13>LONDON</element13><element14>tr"
"ue</element14><element14>true</element14><element15>1.5</element15><element15"
">1.5</element15><element16>FF0001</element16><element16>FF0001</element16><el"
"ement17>2</element17><element17>2</element17><element18>2012-08-18T13:25:00</"
"element18><element18>2012-08-18T13:25:00</element18><element19>custom</elemen"
"t19><element19>custom</element19></selection3></element2><element3><selection"
"3><selection1>2</selection1></selection3></element3><element4><selection2>255"
"</selection2></element4><element4><selection2>255</selection2></element4></se"
"lection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection2>0</selection2></element1><element2><selection3><element1><ele"
"ment2></element2><element2></element2><element4></element4><element5><element"
"1><element2></element2><element2></element2><element4></element4><element5><e"
"lement1><element2></element2><element2></element2><element4></element4></elem"
"ent1></element5></element1></element5></element1><element1><element2></elemen"
"t2><element2></element2><element4></element4><element5><element1><element2></"
"element2><element2></element2><element4></element4><element5><element1><eleme"
"nt2></element2><element2></element2><element4></element4></element1></element"
"5></element1></element5></element1><element2><selection3><element8>true</elem"
"ent8><element9></element9><element10>1.5</element10><element11>FF0001</elemen"
"t11><element12>2</element12><element13>LONDON</element13></selection3></eleme"
"nt2><element2><selection3><element8>true</element8><element9></element9><elem"
"ent10>1.5</element10><element11>FF0001</element11><element12>2</element12><el"
"ement13>LONDON</element13></selection3></element2><element8>true</element8><e"
"lement9></element9><element10>1.5</element10><element11>FF0001</element11><el"
"ement12>2</element12><element13>LONDON</element13></selection3></element2><el"
"ement2><selection3><element1><element2></element2><element2></element2><eleme"
"nt4></element4><element5><element1><element2></element2><element2></element2>"
"<element4></element4><element5><element1><element2></element2><element2></ele"
"ment2><element4></element4></element1></element5></element1></element5></elem"
"ent1><element1><element2></element2><element2></element2><element4></element4"
"><element5><element1><element2></element2><element2></element2><element4></el"
"ement4><element5><element1><element2></element2><element2></element2><element"
"4></element4></element1></element5></element1></element5></element1><element2"
"><selection3><element8>true</element8><element9></element9><element10>1.5</el"
"ement10><element11>FF0001</element11><element12>2</element12><element13>LONDO"
"N</element13></selection3></element2><element2><selection3><element8>true</el"
"ement8><element9></element9><element10>1.5</element10><element11>FF0001</elem"
"ent11><element12>2</element12><element13>LONDON</element13></selection3></ele"
"ment2><element8>true</element8><element9></element9><element10>1.5</element10"
"><element11>FF0001</element11><element12>2</element12><element13>LONDON</elem"
"ent13></selection3></element2><element3><selection3><selection2>1.5</selectio"
"n2></selection3></element3><element4><selection2>0</selection2></element4><el"
"ement4><selection2>0</selection2></element4></selection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection3>custom</selection3></element1><element2><selection3><element1"
"><element1>LONDON</element1><element1>LONDON</element1><element3>true</elemen"
"t3><element5><element1><element1>LONDON</element1><element1>LONDON</element1>"
"<element3>true</element3><element5><element1><element1>LONDON</element1><elem"
"ent1>LONDON</element1><element3>true</element3><element6>LONDON</element6><el"
"ement6>LONDON</element6></element1><element2>true</element2><element2>true</e"
"lement2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</ele"
"ment4><element4>FF0001</element4><element5>2</element5><element5>2</element5>"
"<element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</elemen"
"t6></element5><element6>LONDON</element6><element6>LONDON</element6></element"
"1><element2>true</element2><element2>true</element2><element3>1.5</element3><"
"element3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4>"
"<element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</el"
"ement6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</e"
"lement6><element6>LONDON</element6></element1><element1><element1>LONDON</ele"
"ment1><element1>LONDON</element1><element3>true</element3><element5><element1"
"><element1>LONDON</element1><element1>LONDON</element1><element3>true</elemen"
"t3><element5><element1><element1>LONDON</element1><element1>LONDON</element1>"
"<element3>true</element3><element6>LONDON</element6><element6>LONDON</element"
"6></element1><element2>true</element2><element2>true</element2><element3>1.5<"
"/element3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001"
"</element4><element5>2</element5><element5>2</element5><element6>2012-08-18T1"
"3:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><element"
"6>LONDON</element6><element6>LONDON</element6></element1><element2>true</elem"
"ent2><element2>true</element2><element3>1.5</element3><element3>1.5</element3"
"><element4>FF0001</element4><element4>FF0001</element4><element5>2</element5>"
"<element5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-"
"08-18T13:25:00</element6></element5><element6>LONDON</element6><element6>LOND"
"ON</element6></element1><element2><selection3><element1><element1>LONDON</ele"
"ment1><element1>LONDON</element1><element2>arbitrary string value</element2><"
"element2>arbitrary string value</element2><element3>true</element3><element4>"
"arbitrary string value</element4><element5><element1><element1>LONDON</elemen"
"t1><element1>LONDON</element1><element2>arbitrary string value</element2><ele"
"ment2>arbitrary string value</element2><element3>true</element3><element4>arb"
"itrary string value</element4><element5><element1><element1>LONDON</element1>"
"<element1>LONDON</element1><element2>arbitrary string value</element2><elemen"
"t2>arbitrary string value</element2><element3>true</element3><element4>arbitr"
"ary string value</element4><element6>LONDON</element6><element6>LONDON</eleme"
"nt6></element1><element2>true</element2><element2>true</element2><element3>1."
"5</element3><element3>1.5</element3><element4>FF0001</element4><element4>FF00"
"01</element4><element5>2</element5><element5>2</element5><element6>2012-08-18"
"T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><eleme"
"nt6>LONDON</element6><element6>LONDON</element6></element1><element2>true</el"
"ement2><element2>true</element2><element3>1.5</element3><element3>1.5</elemen"
"t3><element4>FF0001</element4><element4>FF0001</element4><element5>2</element"
"5><element5>2</element5><element6>2012-08-18T13:25:00</element6><element6>201"
"2-08-18T13:25:00</element6></element5><element6>LONDON</element6><element6>LO"
"NDON</element6></element1><element1><element1>LONDON</element1><element1>LOND"
"ON</element1><element2>arbitrary string value</element2><element2>arbitrary s"
"tring value</element2><element3>true</element3><element4>arbitrary string val"
"ue</element4><element5><element1><element1>LONDON</element1><element1>LONDON<"
"/element1><element2>arbitrary string value</element2><element2>arbitrary stri"
"ng value</element2><element3>true</element3><element4>arbitrary string value<"
"/element4><element5><element1><element1>LONDON</element1><element1>LONDON</el"
"ement1><element2>arbitrary string value</element2><element2>arbitrary string "
"value</element2><element3>true</element3><element4>arbitrary string value</el"
"ement4><element6>LONDON</element6><element6>LONDON</element6></element1><elem"
"ent2>true</element2><element2>true</element2><element3>1.5</element3><element"
"3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><elemen"
"t5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</element6>"
"<element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6"
"><element6>LONDON</element6></element1><element2>true</element2><element2>tru"
"e</element2><element3>1.5</element3><element3>1.5</element3><element4>FF0001<"
"/element4><element4>FF0001</element4><element5>2</element5><element5>2</eleme"
"nt5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</el"
"ement6></element5><element6>LONDON</element6><element6>LONDON</element6></ele"
"ment1><element2><selection2>1.5</selection2></element2><element2><selection2>"
"1.5</selection2></element2><element3>FF0001</element3><element4>2</element4><"
"element5>2012-08-18T13:25:00</element5><element6>custom</element6><element7>L"
"ONDON</element7><element8>true</element8><element9>arbitrary string value</el"
"ement9><element10>1.5</element10><element11>FF0001</element11><element12>2</e"
"lement12><element13>LONDON</element13><element14>true</element14><element14>t"
"rue</element14><element15>1.5</element15><element15>1.5</element15><element16"
">FF0001</element16><element16>FF0001</element16><element17>2</element17><elem"
"ent17>2</element17><element18>2012-08-18T13:25:00</element18><element18>2012-"
"08-18T13:25:00</element18><element19>custom</element19><element19>custom</ele"
"ment19></selection3></element2><element2><selection3><element1><element1>LOND"
"ON</element1><element1>LONDON</element1><element2>arbitrary string value</ele"
"ment2><element2>arbitrary string value</element2><element3>true</element3><el"
"ement4>arbitrary string value</element4><element5><element1><element1>LONDON<"
"/element1><element1>LONDON</element1><element2>arbitrary string value</elemen"
"t2><element2>arbitrary string value</element2><element3>true</element3><eleme"
"nt4>arbitrary string value</element4><element5><element1><element1>LONDON</el"
"ement1><element1>LONDON</element1><element2>arbitrary string value</element2>"
"<element2>arbitrary string value</element2><element3>true</element3><element4"
">arbitrary string value</element4><element6>LONDON</element6><element6>LONDON"
"</element6></element1><element2>true</element2><element2>true</element2><elem"
"ent3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><elemen"
"t4>FF0001</element4><element5>2</element5><element5>2</element5><element6>201"
"2-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5"
"><element6>LONDON</element6><element6>LONDON</element6></element1><element2>t"
"rue</element2><element2>true</element2><element3>1.5</element3><element3>1.5<"
"/element3><element4>FF0001</element4><element4>FF0001</element4><element5>2</"
"element5><element5>2</element5><element6>2012-08-18T13:25:00</element6><eleme"
"nt6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6><elem"
"ent6>LONDON</element6></element1><element1><element1>LONDON</element1><elemen"
"t1>LONDON</element1><element2>arbitrary string value</element2><element2>arbi"
"trary string value</element2><element3>true</element3><element4>arbitrary str"
"ing value</element4><element5><element1><element1>LONDON</element1><element1>"
"LONDON</element1><element2>arbitrary string value</element2><element2>arbitra"
"ry string value</element2><element3>true</element3><element4>arbitrary string"
" value</element4><element5><element1><element1>LONDON</element1><element1>LON"
"DON</element1><element2>arbitrary string value</element2><element2>arbitrary "
"string value</element2><element3>true</element3><element4>arbitrary string va"
"lue</element4><element6>LONDON</element6><element6>LONDON</element6></element"
"1><element2>true</element2><element2>true</element2><element3>1.5</element3><"
"element3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4>"
"<element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</el"
"ement6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</e"
"lement6><element6>LONDON</element6></element1><element2>true</element2><eleme"
"nt2>true</element2><element3>1.5</element3><element3>1.5</element3><element4>"
"FF0001</element4><element4>FF0001</element4><element5>2</element5><element5>2"
"</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25"
":00</element6></element5><element6>LONDON</element6><element6>LONDON</element"
"6></element1><element2><selection2>1.5</selection2></element2><element2><sele"
"ction2>1.5</selection2></element2><element3>FF0001</element3><element4>2</ele"
"ment4><element5>2012-08-18T13:25:00</element5><element6>custom</element6><ele"
"ment7>LONDON</element7><element8>true</element8><element9>arbitrary string va"
"lue</element9><element10>1.5</element10><element11>FF0001</element11><element"
"12>2</element12><element13>LONDON</element13><element14>true</element14><elem"
"ent14>true</element14><element15>1.5</element15><element15>1.5</element15><el"
"ement16>FF0001</element16><element16>FF0001</element16><element17>2</element1"
"7><element17>2</element17><element18>2012-08-18T13:25:00</element18><element1"
"8>2012-08-18T13:25:00</element18><element19>custom</element19><element19>cust"
"om</element19></selection3></element2><element3>FF0001</element3><element4>2<"
"/element4><element5>2012-08-18T13:25:00</element5><element6>custom</element6>"
"<element7>LONDON</element7><element8>true</element8><element9>arbitrary strin"
"g value</element9><element10>1.5</element10><element11>FF0001</element11><ele"
"ment12>2</element12><element13>LONDON</element13><element14>true</element14><"
"element14>true</element14><element15>1.5</element15><element15>1.5</element15"
"><element16>FF0001</element16><element16>FF0001</element16><element17>2</elem"
"ent17><element17>2</element17><element18>2012-08-18T13:25:00</element18><elem"
"ent18>2012-08-18T13:25:00</element18><element19>custom</element19><element19>"
"custom</element19></selection3></element2><element2><selection3><element1><el"
"ement1>LONDON</element1><element1>LONDON</element1><element3>true</element3><"
"element5><element1><element1>LONDON</element1><element1>LONDON</element1><ele"
"ment3>true</element3><element5><element1><element1>LONDON</element1><element1"
">LONDON</element1><element3>true</element3><element6>LONDON</element6><elemen"
"t6>LONDON</element6></element1><element2>true</element2><element2>true</eleme"
"nt2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</element"
"4><element4>FF0001</element4><element5>2</element5><element5>2</element5><ele"
"ment6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6><"
"/element5><element6>LONDON</element6><element6>LONDON</element6></element1><e"
"lement2>true</element2><element2>true</element2><element3>1.5</element3><elem"
"ent3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><ele"
"ment5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</elemen"
"t6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</eleme"
"nt6><element6>LONDON</element6></element1><element1><element1>LONDON</element"
"1><element1>LONDON</element1><element3>true</element3><element5><element1><el"
"ement1>LONDON</element1><element1>LONDON</element1><element3>true</element3><"
"element5><element1><element1>LONDON</element1><element1>LONDON</element1><ele"
"ment3>true</element3><element6>LONDON</element6><element6>LONDON</element6></"
"element1><element2>true</element2><element2>true</element2><element3>1.5</ele"
"ment3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</el"
"ement4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:25"
":00</element6><element6>2012-08-18T13:25:00</element6></element5><element6>LO"
"NDON</element6><element6>LONDON</element6></element1><element2>true</element2"
"><element2>true</element2><element3>1.5</element3><element3>1.5</element3><el"
"ement4>FF0001</element4><element4>FF0001</element4><element5>2</element5><ele"
"ment5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-1"
"8T13:25:00</element6></element5><element6>LONDON</element6><element6>LONDON</"
"element6></element1><element2><selection3><element1><element1>LONDON</element"
"1><element1>LONDON</element1><element2>arbitrary string value</element2><elem"
"ent2>arbitrary string value</element2><element3>true</element3><element4>arbi"
"trary string value</element4><element5><element1><element1>LONDON</element1><"
"element1>LONDON</element1><element2>arbitrary string value</element2><element"
"2>arbitrary string value</element2><element3>true</element3><element4>arbitra"
"ry string value</element4><element5><element1><element1>LONDON</element1><ele"
"ment1>LONDON</element1><element2>arbitrary string value</element2><element2>a"
"rbitrary string value</element2><element3>true</element3><element4>arbitrary "
"string value</element4><element6>LONDON</element6><element6>LONDON</element6>"
"</element1><element2>true</element2><element2>true</element2><element3>1.5</e"
"lement3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</"
"element4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:"
"25:00</element6><element6>2012-08-18T13:25:00</element6></element5><element6>"
"LONDON</element6><element6>LONDON</element6></element1><element2>true</elemen"
"t2><element2>true</element2><element3>1.5</element3><element3>1.5</element3><"
"element4>FF0001</element4><element4>FF0001</element4><element5>2</element5><e"
"lement5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08"
"-18T13:25:00</element6></element5><element6>LONDON</element6><element6>LONDON"
"</element6></element1><element1><element1>LONDON</element1><element1>LONDON</"
"element1><element2>arbitrary string value</element2><element2>arbitrary strin"
"g value</element2><element3>true</element3><element4>arbitrary string value</"
"element4><element5><element1><element1>LONDON</element1><element1>LONDON</ele"
"ment1><element2>arbitrary string value</element2><element2>arbitrary string v"
"alue</element2><element3>true</element3><element4>arbitrary string value</ele"
"ment4><element5><element1><element1>LONDON</element1><element1>LONDON</elemen"
"t1><element2>arbitrary string value</element2><element2>arbitrary string valu"
"e</element2><element3>true</element3><element4>arbitrary string value</elemen"
"t4><element6>LONDON</element6><element6>LONDON</element6></element1><element2"
">true</element2><element2>true</element2><element3>1.5</element3><element3>1."
"5</element3><element4>FF0001</element4><element4>FF0001</element4><element5>2"
"</element5><element5>2</element5><element6>2012-08-18T13:25:00</element6><ele"
"ment6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6><el"
"ement6>LONDON</element6></element1><element2>true</element2><element2>true</e"
"lement2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</ele"
"ment4><element4>FF0001</element4><element5>2</element5><element5>2</element5>"
"<element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</elemen"
"t6></element5><element6>LONDON</element6><element6>LONDON</element6></element"
"1><element2><selection2>1.5</selection2></element2><element2><selection2>1.5<"
"/selection2></element2><element3>FF0001</element3><element4>2</element4><elem"
"ent5>2012-08-18T13:25:00</element5><element6>custom</element6><element7>LONDO"
"N</element7><element8>true</element8><element9>arbitrary string value</elemen"
"t9><element10>1.5</element10><element11>FF0001</element11><element12>2</eleme"
"nt12><element13>LONDON</element13><element14>true</element14><element14>true<"
"/element14><element15>1.5</element15><element15>1.5</element15><element16>FF0"
"001</element16><element16>FF0001</element16><element17>2</element17><element1"
"7>2</element17><element18>2012-08-18T13:25:00</element18><element18>2012-08-1"
"8T13:25:00</element18><element19>custom</element19><element19>custom</element"
"19></selection3></element2><element2><selection3><element1><element1>LONDON</"
"element1><element1>LONDON</element1><element2>arbitrary string value</element"
"2><element2>arbitrary string value</element2><element3>true</element3><elemen"
"t4>arbitrary string value</element4><element5><element1><element1>LONDON</ele"
"ment1><element1>LONDON</element1><element2>arbitrary string value</element2><"
"element2>arbitrary string value</element2><element3>true</element3><element4>"
"arbitrary string value</element4><element5><element1><element1>LONDON</elemen"
"t1><element1>LONDON</element1><element2>arbitrary string value</element2><ele"
"ment2>arbitrary string value</element2><element3>true</element3><element4>arb"
"itrary string value</element4><element6>LONDON</element6><element6>LONDON</el"
"ement6></element1><element2>true</element2><element2>true</element2><element3"
">1.5</element3><element3>1.5</element3><element4>FF0001</element4><element4>F"
"F0001</element4><element5>2</element5><element5>2</element5><element6>2012-08"
"-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><el"
"ement6>LONDON</element6><element6>LONDON</element6></element1><element2>true<"
"/element2><element2>true</element2><element3>1.5</element3><element3>1.5</ele"
"ment3><element4>FF0001</element4><element4>FF0001</element4><element5>2</elem"
"ent5><element5>2</element5><element6>2012-08-18T13:25:00</element6><element6>"
"2012-08-18T13:25:00</element6></element5><element6>LONDON</element6><element6"
">LONDON</element6></element1><element1><element1>LONDON</element1><element1>L"
"ONDON</element1><element2>arbitrary string value</element2><element2>arbitrar"
"y string value</element2><element3>true</element3><element4>arbitrary string "
"value</element4><element5><element1><element1>LONDON</element1><element1>LOND"
"ON</element1><element2>arbitrary string value</element2><element2>arbitrary s"
"tring value</element2><element3>true</element3><element4>arbitrary string val"
"ue</element4><element5><element1><element1>LONDON</element1><element1>LONDON<"
"/element1><element2>arbitrary string value</element2><element2>arbitrary stri"
"ng value</element2><element3>true</element3><element4>arbitrary string value<"
"/element4><element6>LONDON</element6><element6>LONDON</element6></element1><e"
"lement2>true</element2><element2>true</element2><element3>1.5</element3><elem"
"ent3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><ele"
"ment5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</elemen"
"t6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</eleme"
"nt6><element6>LONDON</element6></element1><element2>true</element2><element2>"
"true</element2><element3>1.5</element3><element3>1.5</element3><element4>FF00"
"01</element4><element4>FF0001</element4><element5>2</element5><element5>2</el"
"ement5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00<"
"/element6></element5><element6>LONDON</element6><element6>LONDON</element6></"
"element1><element2><selection2>1.5</selection2></element2><element2><selectio"
"n2>1.5</selection2></element2><element3>FF0001</element3><element4>2</element"
"4><element5>2012-08-18T13:25:00</element5><element6>custom</element6><element"
"7>LONDON</element7><element8>true</element8><element9>arbitrary string value<"
"/element9><element10>1.5</element10><element11>FF0001</element11><element12>2"
"</element12><element13>LONDON</element13><element14>true</element14><element1"
"4>true</element14><element15>1.5</element15><element15>1.5</element15><elemen"
"t16>FF0001</element16><element16>FF0001</element16><element17>2</element17><e"
"lement17>2</element17><element18>2012-08-18T13:25:00</element18><element18>20"
"12-08-18T13:25:00</element18><element19>custom</element19><element19>custom</"
"element19></selection3></element2><element3>FF0001</element3><element4>2</ele"
"ment4><element5>2012-08-18T13:25:00</element5><element6>custom</element6><ele"
"ment7>LONDON</element7><element8>true</element8><element9>arbitrary string va"
"lue</element9><element10>1.5</element10><element11>FF0001</element11><element"
"12>2</element12><element13>LONDON</element13><element14>true</element14><elem"
"ent14>true</element14><element15>1.5</element15><element15>1.5</element15><el"
"ement16>FF0001</element16><element16>FF0001</element16><element17>2</element1"
"7><element17>2</element17><element18>2012-08-18T13:25:00</element18><element1"
"8>2012-08-18T13:25:00</element18><element19>custom</element19><element19>cust"
"om</element19></selection3></element2><element3><selection3><selection3><elem"
"ent8>true</element8><element9></element9><element10>1.5</element10><element11"
">FF0001</element11><element12>2</element12><element13>LONDON</element13></sel"
"ection3></selection3></element3><element4><selection3>custom</selection3></el"
"ement4><element4><selection3>custom</selection3></element4></selection1></Obj"
">",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection4>999</selection4></element1><element2><selection4><selection1>"
"true</selection1></selection4></element2><element2><selection4><selection1>tr"
"ue</selection1></selection4></element2><element3><selection3><selection3><ele"
"ment1><element1>LONDON</element1><element1>LONDON</element1><element2>arbitra"
"ry string value</element2><element2>arbitrary string value</element2><element"
"3>true</element3><element4>arbitrary string value</element4><element5><elemen"
"t1><element1>LONDON</element1><element1>LONDON</element1><element2>arbitrary "
"string value</element2><element2>arbitrary string value</element2><element3>t"
"rue</element3><element4>arbitrary string value</element4><element5><element1>"
"<element1>LONDON</element1><element1>LONDON</element1><element2>arbitrary str"
"ing value</element2><element2>arbitrary string value</element2><element3>true"
"</element3><element4>arbitrary string value</element4><element6>LONDON</eleme"
"nt6><element6>LONDON</element6></element1><element2>true</element2><element2>"
"true</element2><element3>1.5</element3><element3>1.5</element3><element4>FF00"
"01</element4><element4>FF0001</element4><element5>2</element5><element5>2</el"
"ement5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00<"
"/element6></element5><element6>LONDON</element6><element6>LONDON</element6></"
"element1><element2>true</element2><element2>true</element2><element3>1.5</ele"
"ment3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</el"
"ement4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:25"
":00</element6><element6>2012-08-18T13:25:00</element6></element5><element6>LO"
"NDON</element6><element6>LONDON</element6></element1><element1><element1>LOND"
"ON</element1><element1>LONDON</element1><element2>arbitrary string value</ele"
"ment2><element2>arbitrary string value</element2><element3>true</element3><el"
"ement4>arbitrary string value</element4><element5><element1><element1>LONDON<"
"/element1><element1>LONDON</element1><element2>arbitrary string value</elemen"
"t2><element2>arbitrary string value</element2><element3>true</element3><eleme"
"nt4>arbitrary string value</element4><element5><element1><element1>LONDON</el"
"ement1><element1>LONDON</element1><element2>arbitrary string value</element2>"
"<element2>arbitrary string value</element2><element3>true</element3><element4"
">arbitrary string value</element4><element6>LONDON</element6><element6>LONDON"
"</element6></element1><element2>true</element2><element2>true</element2><elem"
"ent3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><elemen"
"t4>FF0001</element4><element5>2</element5><element5>2</element5><element6>201"
"2-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5"
"><element6>LONDON</element6><element6>LONDON</element6></element1><element2>t"
"rue</element2><element2>true</element2><element3>1.5</element3><element3>1.5<"
"/element3><element4>FF0001</element4><element4>FF0001</element4><element5>2</"
"element5><element5>2</element5><element6>2012-08-18T13:25:00</element6><eleme"
"nt6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6><elem"
"ent6>LONDON</element6></element1><element2><selection2>1.5</selection2></elem"
"ent2><element2><selection2>1.5</selection2></element2><element3>FF0001</eleme"
"nt3><element4>2</element4><element5>2012-08-18T13:25:00</element5><element6>c"
"ustom</element6><element7>LONDON</element7><element8>true</element8><element9"
">arbitrary string value</element9><element10>1.5</element10><element11>FF0001"
"</element11><element12>2</element12><element13>LONDON</element13><element14>t"
"rue</element14><element14>true</element14><element15>1.5</element15><element1"
"5>1.5</element15><element16>FF0001</element16><element16>FF0001</element16><e"
"lement17>2</element17><element17>2</element17><element18>2012-08-18T13:25:00<"
"/element18><element18>2012-08-18T13:25:00</element18><element19>custom</eleme"
"nt19><element19>custom</element19></selection3></selection3></element3><eleme"
"nt4><selection4>999</selection4></element4><element4><selection4>999</selecti"
"on4></element4></selection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection1><element4>255</element4><element6>custom</element6><element7>"
"999</element7></selection1></element1><element2><selection4><selection2></sel"
"ection2></selection4></element2><element2><selection4><selection2></selection"
"2></selection4></element2><element3><selection3><selection3><element1><elemen"
"t2></element2><element2></element2><element4></element4><element5><element1><"
"element2></element2><element2></element2><element4></element4><element5><elem"
"ent1><element2></element2><element2></element2><element4></element4></element"
"1></element5></element1></element5></element1><element1><element2></element2>"
"<element2></element2><element4></element4><element5><element1><element2></ele"
"ment2><element2></element2><element4></element4><element5><element1><element2"
"></element2><element2></element2><element4></element4></element1></element5><"
"/element1></element5></element1><element2><selection3><element8>true</element"
"8><element9></element9><element10>1.5</element10><element11>FF0001</element11"
"><element12>2</element12><element13>LONDON</element13></selection3></element2"
"><element2><selection3><element8>true</element8><element9></element9><element"
"10>1.5</element10><element11>FF0001</element11><element12>2</element12><eleme"
"nt13>LONDON</element13></selection3></element2><element8>true</element8><elem"
"ent9></element9><element10>1.5</element10><element11>FF0001</element11><eleme"
"nt12>2</element12><element13>LONDON</element13></selection3></selection3></el"
"ement3><element4><selection1><element4>255</element4><element6>custom</elemen"
"t6><element7>999</element7></selection1></element4><element4><selection1><ele"
"ment4>255</element4><element6>custom</element6><element7>999</element7></sele"
"ction1></element4></selection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt2><selection4><selection2>arbitrary string value</selection2></selection4><"
"/element2><element2><selection4><selection2>arbitrary string value</selection"
"2></selection4></element2><element3><selection3><selection3><element1><elemen"
"t1>LONDON</element1><element1>LONDON</element1><element3>true</element3><elem"
"ent5><element1><element1>LONDON</element1><element1>LONDON</element1><element"
"3>true</element3><element5><element1><element1>LONDON</element1><element1>LON"
"DON</element1><element3>true</element3><element6>LONDON</element6><element6>L"
"ONDON</element6></element1><element2>true</element2><element2>true</element2>"
"<element3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><e"
"lement4>FF0001</element4><element5>2</element5><element5>2</element5><element"
"6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></ele"
"ment5><element6>LONDON</element6><element6>LONDON</element6></element1><eleme"
"nt2>true</element2><element2>true</element2><element3>1.5</element3><element3"
">1.5</element3><element4>FF0001</element4><element4>FF0001</element4><element"
"5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</element6><"
"element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6>"
"<element6>LONDON</element6></element1><element1><element1>LONDON</element1><e"
"lement1>LONDON</element1><element3>true</element3><element5><element1><elemen"
"t1>LONDON</element1><element1>LONDON</element1><element3>true</element3><elem"
"ent5><element1><element1>LONDON</element1><element1>LONDON</element1><element"
"3>true</element3><element6>LONDON</element6><element6>LONDON</element6></elem"
"ent1><element2>true</element2><element2>true</element2><element3>1.5</element"
"3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</elemen"
"t4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00<"
"/element6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON"
"</element6><element6>LONDON</element6></element1><element2>true</element2><el"
"ement2>true</element2><element3>1.5</element3><element3>1.5</element3><elemen"
"t4>FF0001</element4><element4>FF0001</element4><element5>2</element5><element"
"5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13"
":25:00</element6></element5><element6>LONDON</element6><element6>LONDON</elem"
"ent6></element1><element2><selection3><element1><element1>LONDON</element1><e"
"lement1>LONDON</element1><element2>arbitrary string value</element2><element2"
">arbitrary string value</element2><element3>true</element3><element4>arbitrar"
"y string value</element4><element5><element1><element1>LONDON</element1><elem"
"ent1>LONDON</element1><element2>arbitrary string value</element2><element2>ar"
"bitrary string value</element2><element3>true</element3><element4>arbitrary s"
"tring value</element4><element5><element1><element1>LONDON</element1><element"
"1>LONDON</element1><element2>arbitrary string value</element2><element2>arbit"
"rary string value</element2><element3>true</element3><element4>arbitrary stri"
"ng value</element4><element6>LONDON</element6><element6>LONDON</element6></el"
"ement1><element2>true</element2><element2>true</element2><element3>1.5</eleme"
"nt3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</elem"
"ent4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:0"
"0</element6><element6>2012-08-18T13:25:00</element6></element5><element6>LOND"
"ON</element6><element6>LONDON</element6></element1><element2>true</element2><"
"element2>true</element2><element3>1.5</element3><element3>1.5</element3><elem"
"ent4>FF0001</element4><element4>FF0001</element4><element5>2</element5><eleme"
"nt5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T"
"13:25:00</element6></element5><element6>LONDON</element6><element6>LONDON</el"
"ement6></element1><element1><element1>LONDON</element1><element1>LONDON</elem"
"ent1><element2>arbitrary string value</element2><element2>arbitrary string va"
"lue</element2><element3>true</element3><element4>arbitrary string value</elem"
"ent4><element5><element1><element1>LONDON</element1><element1>LONDON</element"
"1><element2>arbitrary string value</element2><element2>arbitrary string value"
"</element2><element3>true</element3><element4>arbitrary string value</element"
"4><element5><element1><element1>LONDON</element1><element1>LONDON</element1><"
"element2>arbitrary string value</element2><element2>arbitrary string value</e"
"lement2><element3>true</element3><element4>arbitrary string value</element4><"
"element6>LONDON</element6><element6>LONDON</element6></element1><element2>tru"
"e</element2><element2>true</element2><element3>1.5</element3><element3>1.5</e"
"lement3><element4>FF0001</element4><element4>FF0001</element4><element5>2</el"
"ement5><element5>2</element5><element6>2012-08-18T13:25:00</element6><element"
"6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6><elemen"
"t6>LONDON</element6></element1><element2>true</element2><element2>true</eleme"
"nt2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</element"
"4><element4>FF0001</element4><element5>2</element5><element5>2</element5><ele"
"ment6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6><"
"/element5><element6>LONDON</element6><element6>LONDON</element6></element1><e"
"lement2><selection2>1.5</selection2></element2><element2><selection2>1.5</sel"
"ection2></element2><element3>FF0001</element3><element4>2</element4><element5"
">2012-08-18T13:25:00</element5><element6>custom</element6><element7>LONDON</e"
"lement7><element8>true</element8><element9>arbitrary string value</element9><"
"element10>1.5</element10><element11>FF0001</element11><element12>2</element12"
"><element13>LONDON</element13><element14>true</element14><element14>true</ele"
"ment14><element15>1.5</element15><element15>1.5</element15><element16>FF0001<"
"/element16><element16>FF0001</element16><element17>2</element17><element17>2<"
"/element17><element18>2012-08-18T13:25:00</element18><element18>2012-08-18T13"
":25:00</element18><element19>custom</element19><element19>custom</element19><"
"/selection3></element2><element2><selection3><element1><element1>LONDON</elem"
"ent1><element1>LONDON</element1><element2>arbitrary string value</element2><e"
"lement2>arbitrary string value</element2><element3>true</element3><element4>a"
"rbitrary string value</element4><element5><element1><element1>LONDON</element"
"1><element1>LONDON</element1><element2>arbitrary string value</element2><elem"
"ent2>arbitrary string value</element2><element3>true</element3><element4>arbi"
"trary string value</element4><element5><element1><element1>LONDON</element1><"
"element1>LONDON</element1><element2>arbitrary string value</element2><element"
"2>arbitrary string value</element2><element3>true</element3><element4>arbitra"
"ry string value</element4><element6>LONDON</element6><element6>LONDON</elemen"
"t6></element1><element2>true</element2><element2>true</element2><element3>1.5"
"</element3><element3>1.5</element3><element4>FF0001</element4><element4>FF000"
"1</element4><element5>2</element5><element5>2</element5><element6>2012-08-18T"
"13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><elemen"
"t6>LONDON</element6><element6>LONDON</element6></element1><element2>true</ele"
"ment2><element2>true</element2><element3>1.5</element3><element3>1.5</element"
"3><element4>FF0001</element4><element4>FF0001</element4><element5>2</element5"
"><element5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012"
"-08-18T13:25:00</element6></element5><element6>LONDON</element6><element6>LON"
"DON</element6></element1><element1><element1>LONDON</element1><element1>LONDO"
"N</element1><element2>arbitrary string value</element2><element2>arbitrary st"
"ring value</element2><element3>true</element3><element4>arbitrary string valu"
"e</element4><element5><element1><element1>LONDON</element1><element1>LONDON</"
"element1><element2>arbitrary string value</element2><element2>arbitrary strin"
"g value</element2><element3>true</element3><element4>arbitrary string value</"
"element4><element5><element1><element1>LONDON</element1><element1>LONDON</ele"
"ment1><element2>arbitrary string value</element2><element2>arbitrary string v"
"alue</element2><element3>true</element3><element4>arbitrary string value</ele"
"ment4><element6>LONDON</element6><element6>LONDON</element6></element1><eleme"
"nt2>true</element2><element2>true</element2><element3>1.5</element3><element3"
">1.5</element3><element4>FF0001</element4><element4>FF0001</element4><element"
"5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</element6><"
"element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6>"
"<element6>LONDON</element6></element1><element2>true</element2><element2>true"
"</element2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</"
"element4><element4>FF0001</element4><element5>2</element5><element5>2</elemen"
"t5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</ele"
"ment6></element5><element6>LONDON</element6><element6>LONDON</element6></elem"
"ent1><element2><selection2>1.5</selection2></element2><element2><selection2>1"
".5</selection2></element2><element3>FF0001</element3><element4>2</element4><e"
"lement5>2012-08-18T13:25:00</element5><element6>custom</element6><element7>LO"
"NDON</element7><element8>true</element8><element9>arbitrary string value</ele"
"ment9><element10>1.5</element10><element11>FF0001</element11><element12>2</el"
"ement12><element13>LONDON</element13><element14>true</element14><element14>tr"
"ue</element14><element15>1.5</element15><element15>1.5</element15><element16>"
"FF0001</element16><element16>FF0001</element16><element17>2</element17><eleme"
"nt17>2</element17><element18>2012-08-18T13:25:00</element18><element18>2012-0"
"8-18T13:25:00</element18><element19>custom</element19><element19>custom</elem"
"ent19></selection3></element2><element3>FF0001</element3><element4>2</element"
"4><element5>2012-08-18T13:25:00</element5><element6>custom</element6><element"
"7>LONDON</element7><element8>true</element8><element9>arbitrary string value<"
"/element9><element10>1.5</element10><element11>FF0001</element11><element12>2"
"</element12><element13>LONDON</element13><element14>true</element14><element1"
"4>true</element14><element15>1.5</element15><element15>1.5</element15><elemen"
"t16>FF0001</element16><element16>FF0001</element16><element17>2</element17><e"
"lement17>2</element17><element18>2012-08-18T13:25:00</element18><element18>20"
"12-08-18T13:25:00</element18><element19>custom</element19><element19>custom</"
"element19></selection3></selection3></element3></selection1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection1><eleme"
"nt1><selection1><element1>0</element1><element2>custom</element2><element3>99"
"9</element3><element4>0</element4><element5>999</element5><element5>999</elem"
"ent5><element6>custom</element6><element7>999</element7><element8>0</element8"
"><element8>0</element8><element9>custom</element9><element9>custom</element9>"
"<element10>0</element10><element10>0</element10><element11>999</element11><el"
"ement11>999</element11></selection1></element1><element2><selection1>2</selec"
"tion1></element2><element2><selection1>2</selection1></element2><element3><se"
"lection1>true</selection1></element3><element4><selection1><element1>0</eleme"
"nt1><element2>custom</element2><element3>999</element3><element4>0</element4>"
"<element5>999</element5><element5>999</element5><element6>custom</element6><e"
"lement7>999</element7><element8>0</element8><element8>0</element8><element9>c"
"ustom</element9><element9>custom</element9><element10>0</element10><element10"
">0</element10><element11>999</element11><element11>999</element11></selection"
"1></element4><element4><selection1><element1>0</element1><element2>custom</el"
"ement2><element3>999</element3><element4>0</element4><element5>999</element5>"
"<element5>999</element5><element6>custom</element6><element7>999</element7><e"
"lement8>0</element8><element8>0</element8><element9>custom</element9><element"
"9>custom</element9><element10>0</element10><element10>0</element10><element11"
">999</element11><element11>999</element11></selection1></element4></selection"
"1></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection2>FF0001"
"</selection2></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>255</element2><element3>2012-08-18T13:25:00</e"
"lement3></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>0</element2><element3>2012-08-18T13:25:00</ele"
"ment3><element4><selection2>1.5</selection2></element4><element5>1.5</element"
"5></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>255</element2><element3>2012-08-18T13:25:00</e"
"lement3><element4><selection3><element8>true</element8><element9></element9><"
"element10>1.5</element10><element11>FF0001</element11><element12>2</element12"
"><element13>LONDON</element13></selection3></element4></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>0</element2><element3>2012-08-18T13:25:00</ele"
"ment3><element4><selection3><element1><element1>LONDON</element1><element1>LO"
"NDON</element1><element2>arbitrary string value</element2><element2>arbitrary"
" string value</element2><element3>true</element3><element4>arbitrary string v"
"alue</element4><element5><element1><element1>LONDON</element1><element1>LONDO"
"N</element1><element2>arbitrary string value</element2><element2>arbitrary st"
"ring value</element2><element3>true</element3><element4>arbitrary string valu"
"e</element4><element5><element1><element1>LONDON</element1><element1>LONDON</"
"element1><element2>arbitrary string value</element2><element2>arbitrary strin"
"g value</element2><element3>true</element3><element4>arbitrary string value</"
"element4><element6>LONDON</element6><element6>LONDON</element6></element1><el"
"ement2>true</element2><element2>true</element2><element3>1.5</element3><eleme"
"nt3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><elem"
"ent5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</element"
"6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</elemen"
"t6><element6>LONDON</element6></element1><element2>true</element2><element2>t"
"rue</element2><element3>1.5</element3><element3>1.5</element3><element4>FF000"
"1</element4><element4>FF0001</element4><element5>2</element5><element5>2</ele"
"ment5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</"
"element6></element5><element6>LONDON</element6><element6>LONDON</element6></e"
"lement1><element1><element1>LONDON</element1><element1>LONDON</element1><elem"
"ent2>arbitrary string value</element2><element2>arbitrary string value</eleme"
"nt2><element3>true</element3><element4>arbitrary string value</element4><elem"
"ent5><element1><element1>LONDON</element1><element1>LONDON</element1><element"
"2>arbitrary string value</element2><element2>arbitrary string value</element2"
"><element3>true</element3><element4>arbitrary string value</element4><element"
"5><element1><element1>LONDON</element1><element1>LONDON</element1><element2>a"
"rbitrary string value</element2><element2>arbitrary string value</element2><e"
"lement3>true</element3><element4>arbitrary string value</element4><element6>L"
"ONDON</element6><element6>LONDON</element6></element1><element2>true</element"
"2><element2>true</element2><element3>1.5</element3><element3>1.5</element3><e"
"lement4>FF0001</element4><element4>FF0001</element4><element5>2</element5><el"
"ement5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-"
"18T13:25:00</element6></element5><element6>LONDON</element6><element6>LONDON<"
"/element6></element1><element2>true</element2><element2>true</element2><eleme"
"nt3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><element"
"4>FF0001</element4><element5>2</element5><element5>2</element5><element6>2012"
"-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5>"
"<element6>LONDON</element6><element6>LONDON</element6></element1><element2><s"
"election2>1.5</selection2></element2><element2><selection2>1.5</selection2></"
"element2><element3>FF0001</element3><element4>2</element4><element5>2012-08-1"
"8T13:25:00</element5><element6>custom</element6><element7>LONDON</element7><e"
"lement8>true</element8><element9>arbitrary string value</element9><element10>"
"1.5</element10><element11>FF0001</element11><element12>2</element12><element1"
"3>LONDON</element13><element14>true</element14><element14>true</element14><el"
"ement15>1.5</element15><element15>1.5</element15><element16>FF0001</element16"
"><element16>FF0001</element16><element17>2</element17><element17>2</element17"
"><element18>2012-08-18T13:25:00</element18><element18>2012-08-18T13:25:00</el"
"ement18><element19>custom</element19><element19>custom</element19></selection"
"3></element4><element5>1.5</element5></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>255</element2><element3>2012-08-18T13:25:00</e"
"lement3><element4><selection3><element1><element2></element2><element2></elem"
"ent2><element4></element4><element5><element1><element2></element2><element2>"
"</element2><element4></element4><element5><element1><element2></element2><ele"
"ment2></element2><element4></element4></element1></element5></element1></elem"
"ent5></element1><element1><element2></element2><element2></element2><element4"
"></element4><element5><element1><element2></element2><element2></element2><el"
"ement4></element4><element5><element1><element2></element2><element2></elemen"
"t2><element4></element4></element1></element5></element1></element5></element"
"1><element2><selection3><element8>true</element8><element9></element9><elemen"
"t10>1.5</element10><element11>FF0001</element11><element12>2</element12><elem"
"ent13>LONDON</element13></selection3></element2><element2><selection3><elemen"
"t8>true</element8><element9></element9><element10>1.5</element10><element11>F"
"F0001</element11><element12>2</element12><element13>LONDON</element13></selec"
"tion3></element2><element8>true</element8><element9></element9><element10>1.5"
"</element10><element11>FF0001</element11><element12>2</element12><element13>L"
"ONDON</element13></selection3></element4></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>0</element2><element3>2012-08-18T13:25:00</ele"
"ment3><element4><selection3><element1><element1>LONDON</element1><element1>LO"
"NDON</element1><element3>true</element3><element5><element1><element1>LONDON<"
"/element1><element1>LONDON</element1><element3>true</element3><element5><elem"
"ent1><element1>LONDON</element1><element1>LONDON</element1><element3>true</el"
"ement3><element6>LONDON</element6><element6>LONDON</element6></element1><elem"
"ent2>true</element2><element2>true</element2><element3>1.5</element3><element"
"3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><elemen"
"t5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</element6>"
"<element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</element6"
"><element6>LONDON</element6></element1><element2>true</element2><element2>tru"
"e</element2><element3>1.5</element3><element3>1.5</element3><element4>FF0001<"
"/element4><element4>FF0001</element4><element5>2</element5><element5>2</eleme"
"nt5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</el"
"ement6></element5><element6>LONDON</element6><element6>LONDON</element6></ele"
"ment1><element1><element1>LONDON</element1><element1>LONDON</element1><elemen"
"t3>true</element3><element5><element1><element1>LONDON</element1><element1>LO"
"NDON</element1><element3>true</element3><element5><element1><element1>LONDON<"
"/element1><element1>LONDON</element1><element3>true</element3><element6>LONDO"
"N</element6><element6>LONDON</element6></element1><element2>true</element2><e"
"lement2>true</element2><element3>1.5</element3><element3>1.5</element3><eleme"
"nt4>FF0001</element4><element4>FF0001</element4><element5>2</element5><elemen"
"t5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T1"
"3:25:00</element6></element5><element6>LONDON</element6><element6>LONDON</ele"
"ment6></element1><element2>true</element2><element2>true</element2><element3>"
"1.5</element3><element3>1.5</element3><element4>FF0001</element4><element4>FF"
"0001</element4><element5>2</element5><element5>2</element5><element6>2012-08-"
"18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><ele"
"ment6>LONDON</element6><element6>LONDON</element6></element1><element2><selec"
"tion3><element1><element1>LONDON</element1><element1>LONDON</element1><elemen"
"t2>arbitrary string value</element2><element2>arbitrary string value</element"
"2><element3>true</element3><element4>arbitrary string value</element4><elemen"
"t5><element1><element1>LONDON</element1><element1>LONDON</element1><element2>"
"arbitrary string value</element2><element2>arbitrary string value</element2><"
"element3>true</element3><element4>arbitrary string value</element4><element5>"
"<element1><element1>LONDON</element1><element1>LONDON</element1><element2>arb"
"itrary string value</element2><element2>arbitrary string value</element2><ele"
"ment3>true</element3><element4>arbitrary string value</element4><element6>LON"
"DON</element6><element6>LONDON</element6></element1><element2>true</element2>"
"<element2>true</element2><element3>1.5</element3><element3>1.5</element3><ele"
"ment4>FF0001</element4><element4>FF0001</element4><element5>2</element5><elem"
"ent5>2</element5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18"
"T13:25:00</element6></element5><element6>LONDON</element6><element6>LONDON</e"
"lement6></element1><element2>true</element2><element2>true</element2><element"
"3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><element4>"
"FF0001</element4><element5>2</element5><element5>2</element5><element6>2012-0"
"8-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></element5><e"
"lement6>LONDON</element6><element6>LONDON</element6></element1><element1><ele"
"ment1>LONDON</element1><element1>LONDON</element1><element2>arbitrary string "
"value</element2><element2>arbitrary string value</element2><element3>true</el"
"ement3><element4>arbitrary string value</element4><element5><element1><elemen"
"t1>LONDON</element1><element1>LONDON</element1><element2>arbitrary string val"
"ue</element2><element2>arbitrary string value</element2><element3>true</eleme"
"nt3><element4>arbitrary string value</element4><element5><element1><element1>"
"LONDON</element1><element1>LONDON</element1><element2>arbitrary string value<"
"/element2><element2>arbitrary string value</element2><element3>true</element3"
"><element4>arbitrary string value</element4><element6>LONDON</element6><eleme"
"nt6>LONDON</element6></element1><element2>true</element2><element2>true</elem"
"ent2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</elemen"
"t4><element4>FF0001</element4><element5>2</element5><element5>2</element5><el"
"ement6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6>"
"</element5><element6>LONDON</element6><element6>LONDON</element6></element1><"
"element2>true</element2><element2>true</element2><element3>1.5</element3><ele"
"ment3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><el"
"ement5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</eleme"
"nt6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</elem"
"ent6><element6>LONDON</element6></element1><element2><selection2>1.5</selecti"
"on2></element2><element2><selection2>1.5</selection2></element2><element3>FF0"
"001</element3><element4>2</element4><element5>2012-08-18T13:25:00</element5><"
"element6>custom</element6><element7>LONDON</element7><element8>true</element8"
"><element9>arbitrary string value</element9><element10>1.5</element10><elemen"
"t11>FF0001</element11><element12>2</element12><element13>LONDON</element13><e"
"lement14>true</element14><element14>true</element14><element15>1.5</element15"
"><element15>1.5</element15><element16>FF0001</element16><element16>FF0001</el"
"ement16><element17>2</element17><element17>2</element17><element18>2012-08-18"
"T13:25:00</element18><element18>2012-08-18T13:25:00</element18><element19>cus"
"tom</element19><element19>custom</element19></selection3></element2><element2"
"><selection3><element1><element1>LONDON</element1><element1>LONDON</element1>"
"<element2>arbitrary string value</element2><element2>arbitrary string value</"
"element2><element3>true</element3><element4>arbitrary string value</element4>"
"<element5><element1><element1>LONDON</element1><element1>LONDON</element1><el"
"ement2>arbitrary string value</element2><element2>arbitrary string value</ele"
"ment2><element3>true</element3><element4>arbitrary string value</element4><el"
"ement5><element1><element1>LONDON</element1><element1>LONDON</element1><eleme"
"nt2>arbitrary string value</element2><element2>arbitrary string value</elemen"
"t2><element3>true</element3><element4>arbitrary string value</element4><eleme"
"nt6>LONDON</element6><element6>LONDON</element6></element1><element2>true</el"
"ement2><element2>true</element2><element3>1.5</element3><element3>1.5</elemen"
"t3><element4>FF0001</element4><element4>FF0001</element4><element5>2</element"
"5><element5>2</element5><element6>2012-08-18T13:25:00</element6><element6>201"
"2-08-18T13:25:00</element6></element5><element6>LONDON</element6><element6>LO"
"NDON</element6></element1><element2>true</element2><element2>true</element2><"
"element3>1.5</element3><element3>1.5</element3><element4>FF0001</element4><el"
"ement4>FF0001</element4><element5>2</element5><element5>2</element5><element6"
">2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></elem"
"ent5><element6>LONDON</element6><element6>LONDON</element6></element1><elemen"
"t1><element1>LONDON</element1><element1>LONDON</element1><element2>arbitrary "
"string value</element2><element2>arbitrary string value</element2><element3>t"
"rue</element3><element4>arbitrary string value</element4><element5><element1>"
"<element1>LONDON</element1><element1>LONDON</element1><element2>arbitrary str"
"ing value</element2><element2>arbitrary string value</element2><element3>true"
"</element3><element4>arbitrary string value</element4><element5><element1><el"
"ement1>LONDON</element1><element1>LONDON</element1><element2>arbitrary string"
" value</element2><element2>arbitrary string value</element2><element3>true</e"
"lement3><element4>arbitrary string value</element4><element6>LONDON</element6"
"><element6>LONDON</element6></element1><element2>true</element2><element2>tru"
"e</element2><element3>1.5</element3><element3>1.5</element3><element4>FF0001<"
"/element4><element4>FF0001</element4><element5>2</element5><element5>2</eleme"
"nt5><element6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</el"
"ement6></element5><element6>LONDON</element6><element6>LONDON</element6></ele"
"ment1><element2>true</element2><element2>true</element2><element3>1.5</elemen"
"t3><element3>1.5</element3><element4>FF0001</element4><element4>FF0001</eleme"
"nt4><element5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00"
"</element6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDO"
"N</element6><element6>LONDON</element6></element1><element2><selection2>1.5</"
"selection2></element2><element2><selection2>1.5</selection2></element2><eleme"
"nt3>FF0001</element3><element4>2</element4><element5>2012-08-18T13:25:00</ele"
"ment5><element6>custom</element6><element7>LONDON</element7><element8>true</e"
"lement8><element9>arbitrary string value</element9><element10>1.5</element10>"
"<element11>FF0001</element11><element12>2</element12><element13>LONDON</eleme"
"nt13><element14>true</element14><element14>true</element14><element15>1.5</el"
"ement15><element15>1.5</element15><element16>FF0001</element16><element16>FF0"
"001</element16><element17>2</element17><element17>2</element17><element18>201"
"2-08-18T13:25:00</element18><element18>2012-08-18T13:25:00</element18><elemen"
"t19>custom</element19><element19>custom</element19></selection3></element2><e"
"lement3>FF0001</element3><element4>2</element4><element5>2012-08-18T13:25:00<"
"/element5><element6>custom</element6><element7>LONDON</element7><element8>tru"
"e</element8><element9>arbitrary string value</element9><element10>1.5</elemen"
"t10><element11>FF0001</element11><element12>2</element12><element13>LONDON</e"
"lement13><element14>true</element14><element14>true</element14><element15>1.5"
"</element15><element15>1.5</element15><element16>FF0001</element16><element16"
">FF0001</element16><element17>2</element17><element17>2</element17><element18"
">2012-08-18T13:25:00</element18><element18>2012-08-18T13:25:00</element18><el"
"ement19>custom</element19><element19>custom</element19></selection3></element"
"4><element5>1.5</element5></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>255</element2><element3>2012-08-18T13:25:00</e"
"lement3><element4><selection4><selection1>true</selection1></selection4></ele"
"ment4></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>0</element2><element3>2012-08-18T13:25:00</ele"
"ment3><element4><selection4><selection2></selection2></selection4></element4>"
"<element5>1.5</element5></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>255</element2><element3>2012-08-18T13:25:00</e"
"lement3><element4><selection4><selection2>arbitrary string value</selection2>"
"</selection4></element4></selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection3><eleme"
"nt1>custom</element1><element2>0</element2><element3>2012-08-18T13:25:00</ele"
"ment3><element4><selection1>2</selection1></element4><element5>1.5</element5>"
"</selection3></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection4></sele"
"ction4></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection4><eleme"
"nt1>LONDON</element1><element1>LONDON</element1><element2>arbitrary string va"
"lue</element2><element2>arbitrary string value</element2><element3>true</elem"
"ent3><element4>arbitrary string value</element4><element5><element1><element1"
">LONDON</element1><element1>LONDON</element1><element2>arbitrary string value"
"</element2><element2>arbitrary string value</element2><element3>true</element"
"3><element4>arbitrary string value</element4><element5><element1><element1>LO"
"NDON</element1><element1>LONDON</element1><element2>arbitrary string value</e"
"lement2><element2>arbitrary string value</element2><element3>true</element3><"
"element4>arbitrary string value</element4><element6>LONDON</element6><element"
"6>LONDON</element6></element1><element2>true</element2><element2>true</elemen"
"t2><element3>1.5</element3><element3>1.5</element3><element4>FF0001</element4"
"><element4>FF0001</element4><element5>2</element5><element5>2</element5><elem"
"ent6>2012-08-18T13:25:00</element6><element6>2012-08-18T13:25:00</element6></"
"element5><element6>LONDON</element6><element6>LONDON</element6></element1><el"
"ement2>true</element2><element2>true</element2><element3>1.5</element3><eleme"
"nt3>1.5</element3><element4>FF0001</element4><element4>FF0001</element4><elem"
"ent5>2</element5><element5>2</element5><element6>2012-08-18T13:25:00</element"
"6><element6>2012-08-18T13:25:00</element6></element5><element6>LONDON</elemen"
"t6><element6>LONDON</element6></selection4></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection4><eleme"
"nt2></element2><element2></element2><element4></element4><element5><element1>"
"<element2></element2><element2></element2><element4></element4><element5><ele"
"ment1><element2></element2><element2></element2><element4></element4></elemen"
"t1></element5></element1></element5></selection4></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection5>2012-0"
"8-18T13:25:00</selection5></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection6>custom"
"</selection6></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-inst"
"ance'><selection7>LONDON</selection7></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion1><element4>255</element4><element6>custom</element6><element7>999</eleme"
"nt7></selection1></selection8></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion1><element1>0</element1><element2>custom</element2><element3>999</element"
"3><element4>0</element4><element5>999</element5><element5>999</element5><elem"
"ent6>custom</element6><element7>999</element7><element8>0</element8><element8"
">0</element8><element9>custom</element9><element9>custom</element9><element10"
">0</element10><element10>0</element10><element11>999</element11><element11>99"
"9</element11></selection1></selection8></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion1><element1>255</element1><element4>255</element4><element6>custom</eleme"
"nt6><element7>999</element7><element8>255</element8><element8>255</element8><"
"element10>255</element10><element10>255</element10></selection1></selection8>"
"</Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion2>255</selection2></selection8></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion2>0</selection2></selection8></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion3>custom</selection3></selection8></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection8><selec"
"tion4>999</selection4></selection8></Obj>",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection9/></Obj"
">",

"<Obj xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'><selection10><elem"
"ent1>3000000000</element1><element2>32794</element2><element3>922337203685478"
"5808</element3></selection10></Obj>"
};

static const int NUM_XML_TEST_MESSAGES =
                      (sizeof(XML_TEST_MESSAGES) / sizeof(*XML_TEST_MESSAGES));


static const char* TEST_MESSAGES[] = {

"{"
"    \"selection1\" : {"
"        \"element3\" : {"
"            \"selection1\" : true"
"        }"
"    }"
"}",


"{"
"    \"selection1\" : {"
"        \"element1\" : {"
"            \"selection1\" : {"
"                \"element1\"  : 0,"
"                \"element2\"  : \"custom\","
"                \"element3\"  : 999,"
"                \"element4\"  : 0,"
"                \"element5\"  : [ 999 ] ,"
"                \"element6\"  : \"custom\","
"                \"element7\"  : 999,"
"                \"element8\"  : [ 0 ],"
"                \"element9\"  : [ \"custom\" ],"
"                \"element10\" : [ 0 ],"
"                \"element11\" : [ 999 ]"
"            }"
"        },"
"        \"element2\" : ["
"            { \"selection2\" : 1.5 },"
"            { \"selection2\" : 1.5 }"
"        ],"
"        \"element3\" : {"
"            \"selection2\" : \"\""
"        },"
"       \"element4\" : ["
"            { \"selection1\" : {"
"                \"element1\"  : 0,"
"                \"element2\"  : \"custom\","
"                \"element3\"  : 999,"
"                \"element4\"  : 0,"
"                \"element5\"  : [ 999 ] ,"
"                \"element6\"  : \"custom\","
"                \"element7\"  : 999,"
"                \"element8\"  : [ 0 ],"
"                \"element9\"  : [ \"custom\" ],"
"                \"element10\" : [ 0 ],"
"                \"element11\" : [ 999 ]"
"            }},"
"            { \"selection1\" : {"
"                \"element1\"  : 0,"
"                \"element2\"  : \"custom\","
"                \"element3\"  : 999,"
"                \"element4\"  : 0,"
"                \"element5\"  : [ 999 ] ,"
"                \"element6\"  : \"custom\","
"                \"element7\"  : 999,"
"                \"element8\"  : [ 0 ],"
"                \"element9\"  : [ \"custom\" ],"
"                \"element10\" : [ 0 ],"
"                \"element11\" : [ 999 ]"
"            }}"
"       ]"
"    }"
"}",


"{"
"    \"selection3\" : {"
"        \"element1\" : \"custom\","
"        \"element2\" : 255,"
"        \"element3\" : \"2012-08-18T13:25:00\","
"        \"element4\" : {"
"            \"selection3\" : {"
"                \"element8\" : true,"
"                \"element9\" : \"\","
"                \"element10\" : 1.5,"
"                \"element11\" : \"VGhlIHF1aWNrIGJyb3duIGZveA==\","
"                \"element12\" : 2,"
"                \"element13\" : \"LONDON\""
"            }"
"        }"
"    }"
"}"

};

static const int NUM_TEST_MESSAGES =
                              (sizeof(TEST_MESSAGES) / sizeof(*TEST_MESSAGES));


namespace BloombergLP {

namespace test {

                               // =============
                               // class Address
                               // =============

class Address {
    // Address: TBD: Provide annotation

  private:
    bsl::string d_street;
        // street: TBD: Provide annotation
    bsl::string d_city;
        // city: TBD: Provide annotation
    bsl::string d_state;
        // state: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_STREET = 0,
            // index for "Street" attribute
        ATTRIBUTE_INDEX_CITY = 1,
            // index for "City" attribute
        ATTRIBUTE_INDEX_STATE = 2
            // index for "State" attribute
    };

    enum {
        ATTRIBUTE_ID_STREET = 0,
            // id for "Street" attribute
        ATTRIBUTE_ID_CITY = 1,
            // id for "City" attribute
        ATTRIBUTE_ID_STATE = 2
            // id for "State" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Address")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Address(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Address(const Address& original,
            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Address();
        // Destroy this object.

    // MANIPULATORS
    Address& operator=(const Address& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& street();
        // Return a reference to the modifiable "Street" attribute of this
        // object.

    bsl::string& city();
        // Return a reference to the modifiable "City" attribute of this
        // object.

    bsl::string& state();
        // Return a reference to the modifiable "State" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& street() const;
        // Return a reference to the non-modifiable "Street" attribute of this
        // object.

    const bsl::string& city() const;
        // Return a reference to the non-modifiable "City" attribute of this
        // object.

    const bsl::string& state() const;
        // Return a reference to the non-modifiable "State" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Address& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

namespace test {

                               // -------------
                               // class Address
                               // -------------

// CONSTANTS

const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdeat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",                 // name
        sizeof("street") - 1,     // name length
        "street: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",                 // name
        sizeof("city") - 1,     // name length
        "city: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",                 // name
        sizeof("state") - 1,     // name length
        "state: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Address::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='I'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='T'
             && bdeu_CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
            }
        } break;
        case 6: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='T')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Address::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
      case ATTRIBUTE_ID_CITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
      case ATTRIBUTE_ID_STATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Address::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

// CLASS METHODS
inline
int Address::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Address::Address(bslma_Allocator *basicAllocator)
: d_street(basicAllocator)
, d_city(basicAllocator)
, d_state(basicAllocator)
{
}

inline
Address::Address(
        const Address& original,
        bslma_Allocator *basicAllocator)
: d_street(original.d_street, basicAllocator)
, d_city(original.d_city, basicAllocator)
, d_state(original.d_state, basicAllocator)
{
}

inline
Address::~Address()
{
}

// MANIPULATORS
inline
Address&
Address::operator=(const Address& rhs)
{
    if (this != &rhs) {
        d_street = rhs.d_street;
        d_city = rhs.d_city;
        d_state = rhs.d_state;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Address::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_street, 1);
            bdex_InStreamFunctions::streamIn(stream, d_city, 1);
            bdex_InStreamFunctions::streamIn(stream, d_state, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Address::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_street);
    bdeat_ValueTypeFunctions::reset(&d_city);
    bdeat_ValueTypeFunctions::reset(&d_state);
}

template <class MANIPULATOR>
inline
int Address::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return manipulator(&d_street,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Address::street()
{
    return d_street;
}

inline
bsl::string& Address::city()
{
    return d_city;
}

inline
bsl::string& Address::state()
{
    return d_state;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_street, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_city, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_state, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Address::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int Address::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Address::street() const
{
    return d_street;
}

inline
const bsl::string& Address::city() const
{
    return d_city;
}

inline
const bsl::string& Address::state() const
{
    return d_state;
}

}  // close namespace test

namespace test {

                               // ==============
                               // class Employee
                               // ==============

class Employee {
    // Employee: TBD: Provide annotation

  private:
    bsl::string d_name;
        // name: TBD: Provide annotation
    Address d_homeAddress;
        // homeAddress: TBD: Provide annotation
    int d_age;
        // age: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_HOME_ADDRESS = 1,
            // index for "HomeAddress" attribute
        ATTRIBUTE_INDEX_AGE = 2
            // index for "Age" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        ATTRIBUTE_ID_AGE = 2
            // id for "Age" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Employee")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Employee(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Employee(const Employee& original,
             bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Employee();
        // Destroy this object.

    // MANIPULATORS
    Employee& operator=(const Employee& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    Address& homeAddress();
        // Return a reference to the modifiable "HomeAddress" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const Address& homeAddress() const;
        // Return a reference to the non-modifiable "HomeAddress" attribute of
        // this object.

    const int& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Employee& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)


namespace test {

                               // --------------
                               // class Employee
                               // --------------

// CONSTANTS

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdeat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                 // name
        sizeof("name") - 1,     // name length
        "name: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                 // name
        sizeof("homeAddress") - 1,     // name length
        "homeAddress: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",                 // name
        sizeof("age") - 1,     // name length
        "age: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='G'
             && bdeu_CharType::toUpper(name[2])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='N'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
            }
        } break;
        case 11: {
            if (bdeu_CharType::toUpper(name[0])=='H'
             && bdeu_CharType::toUpper(name[1])=='O'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='A'
             && bdeu_CharType::toUpper(name[5])=='D'
             && bdeu_CharType::toUpper(name[6])=='D'
             && bdeu_CharType::toUpper(name[7])=='R'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='S'
             && bdeu_CharType::toUpper(name[10])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Employee::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}
// CLASS METHODS
inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Employee::Employee(bslma_Allocator *basicAllocator)
: d_name(basicAllocator)
, d_homeAddress(basicAllocator)
, d_age()
{
}

inline
Employee::Employee(
        const Employee& original,
        bslma_Allocator *basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_homeAddress(original.d_homeAddress, basicAllocator)
, d_age(original.d_age)
{
}

inline
Employee::~Employee()
{
}

// MANIPULATORS
inline
Employee&
Employee::operator=(const Employee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_age = rhs.d_age;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Employee::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(stream, d_homeAddress, 1);
            bdex_InStreamFunctions::streamIn(stream, d_age, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Employee::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_name);
    bdeat_ValueTypeFunctions::reset(&d_homeAddress);
    bdeat_ValueTypeFunctions::reset(&d_age);
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(&d_homeAddress,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Employee::name()
{
    return d_name;
}

inline
Address& Employee::homeAddress()
{
    return d_homeAddress;
}

inline
int& Employee::age()
{
    return d_age;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Employee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_homeAddress, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_age, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Employee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_homeAddress,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return accessor(d_homeAddress,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Employee::name() const
{
    return d_name;
}

inline
const Address& Employee::homeAddress() const
{
    return d_homeAddress;
}

inline
const int& Employee::age() const
{
    return d_age;
}

}  // close namespace test

} // close namespace BloombergLP

namespace {

#if 0
void constructTestAggregate(std::vector<bcem_Aggregate>* objects)
{
    baexml_MiniReader reader;
    baexml_DecoderOptions options;
    baexml_ErrorInfo e;
    baexml_Decoder decoder(&options, &reader, &e);

    for (int i = 0; i < NUM_XML_TEST_MESSAGES; ++i) {
        bcem_Aggregate object;
        std::istringstream ss(XML_TEST_MESSAGES[i]);

        baexml_MiniReader reader;
        baexml_DecoderOptions options;
        baexml_ErrorInfo e;
        baexml_Decoder decoder(&options, &reader, &e);

        int rc = decoder.decode(ss.rdbuf(), &object);

        if (0 != rc) {
            bsl::cout << "Failed to decode from initialization data (i="
                      << i << "): "
                      << decoder.loggedMessages() << bsl::endl;
        }
        BSLS_ASSERT(0 == rc); // test invariant
        objects->push_back(object);
    }
}
#endif

void constructFeatureTestMessage(std::vector<baea::FeatureTestMessage>* objects)
{
    baexml_MiniReader reader;
    baexml_DecoderOptions options;
    baexml_ErrorInfo e;
    baexml_Decoder decoder(&options, &reader, &e);

    for (int i = 0; i < NUM_XML_TEST_MESSAGES; ++i) {
        baea::FeatureTestMessage object;
        std::istringstream ss(XML_TEST_MESSAGES[i]);

        // Use 'baea_SerializableObjectProxy' to speed up compile time.
        baea_SerializableObjectProxy sop;
        baea_SerializableObjectProxyUtil::makeDecodeProxy(&sop, &object);

        baexml_MiniReader reader;
        baexml_DecoderOptions options;
        baexml_ErrorInfo e;
        baexml_Decoder decoder(&options, &reader, &e);

        int rc = decoder.decode(ss.rdbuf(), &sop);

        if (0 != rc) {
            bsl::cout << "Failed to decode from initialization data (i="
                      << i << "): "
                      << decoder.loggedMessages() << bsl::endl;
        }
        if (baea::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
                                                        object.selectionId()) {
            bsl::cout << "Decoded unselected choice from initialization data"
                      << " (i=" << i << "):" << XML_TEST_MESSAGES[i]
                      << bsl::endl;
            rc = 9;
        }
        BSLS_ASSERT(0 == rc); // test invariant
        objects->push_back(object);
    }
}

}  // close anonymous namespace

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    //veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    //veryVeryVeryVerbose = argc > 5;
    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // Encode test
        //
        // Concerns:
        //: 1 'encode' only works for Choice or Sequence type.
        //
        // Testing:
        //  int encode(const TYPE & value);
        // --------------------------------------------------------------------
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // Encode Array
        // --------------------------------------------------------------------
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // Encode Customized
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // Encode Choice
        // --------------------------------------------------------------------
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // Encode Sequence
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Encode Enumeration
        // --------------------------------------------------------------------
        const int NUM_ENUMERATORS = baea::Enumerated::NUM_ENUMERATORS;
        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            baea::Enumerated::Value mX = (baea::Enumerated::Value) ti;
            const baea::Enumerated::Value& X = mX;

            bsl::string exp("\"");
            exp += baea::Enumerated::toString(X);
            exp += '\"';

            Obj  encoder;
            bsl::ostringstream oss;
            Impl impl(&encoder, oss.rdbuf());
            ASSERTV(ti, 0 == impl.encode(X));

            bsl::string result = oss.str();
            ASSERTV(ti, result, exp, result == exp);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Encode Date/Time
        // --------------------------------------------------------------------
        const struct {
            int         d_line;
            int         d_year;
            int         d_month;
            int         d_day;
            int         d_hour;
            int         d_minute;
            int         d_second;
            int         d_millisecond;
            int         d_offset;
        } DATA[] = {
            //Line Year   Mon  Day  Hour  Min  Sec     ms   offset
            //---- ----   ---  ---  ----  ---  ---     --   ------

            // Valid dates and times
            { L_,     1,   1,   1,    0,   0,   0,     0,        0 },
            { L_,  2005,   1,   1,    0,   0,   0,     0,      -90 },
            { L_,   123,   6,  15,   13,  40,  59,     0,     -240 },
            { L_,  1999,  10,  12,   23,   0,   1,     0,     -720 },

            // Vary milliseconds
            { L_,  1999,  10,  12,   23,   0,   1,     0,       90 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,      240 },
            { L_,  1999,  10,  12,   23,   0,   1,   999,      720 },
            { L_,  1999,  12,  31,   23,  59,  59,   999,      720 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const char *datetimeTzResults[] = {
            "\"0001-01-01T00:00:00.000+00:00\"",
            "\"2005-01-01T00:00:00.000-01:30\"",
            "\"0123-06-15T13:40:59.000-04:00\"",
            "\"1999-10-12T23:00:01.000-12:00\"",
            "\"1999-10-12T23:00:01.000+01:30\"",
            "\"1999-10-12T23:00:01.456+04:00\"",
            "\"1999-10-12T23:00:01.999+12:00\"",
            "\"1999-12-31T23:59:59.999+12:00\""
        };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const int YEAR        = DATA[ti].d_year;
            const int MONTH       = DATA[ti].d_month;
            const int DAY         = DATA[ti].d_day;
            const int HOUR        = DATA[ti].d_hour;
            const int MINUTE      = DATA[ti].d_minute;
            const int SECOND      = DATA[ti].d_second;
            const int MILLISECOND = DATA[ti].d_millisecond;
            const int OFFSET      = DATA[ti].d_offset;;

            bdet_Date theDate(YEAR, MONTH, DAY);
            bdet_Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdet_Datetime theDatetime(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MILLISECOND);

            bdet_DateTz     theDateTz(theDate, OFFSET);
            bdet_TimeTz     theTimeTz(theTime, OFFSET);
            bdet_DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            {
                const char *EXP = datetimeTzResults[ti];
                Obj  encoder;

                bsl::ostringstream oss;
                Impl impl(&encoder, oss.rdbuf());
                ASSERTV(LINE, 0 == impl.encode(theDatetimeTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Encode Numbers
        // --------------------------------------------------------------------
        if (verbose) cout << "Encode double" << endl;
        {
            const struct {
                int         d_line;
                double      d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------

                { L_,     0.0,      "0.000000000000000e+00" },
                { L_,     0.125,    "1.250000000000000e-01" },
                { L_,     1.0,      "1.000000000000000e+00" },
                { L_,    10.0,      "1.000000000000000e+01" },
                { L_,    -1.5,      "-1.500000000000000e+00" },
                { L_,    -1.5e1,    "-1.500000000000000e+01" },
                { L_,    -9.9e100,  "-9.900000000000000e+100" },
                { L_,    -3.14e300, "-3.140000000000000e+300" },
                { L_,    3.14e300,  "3.140000000000000e+300" },
                { L_,    1.0e-1,    "1.000000000000000e-01" },
                { L_,    2.23e-308, "2.230000000000000e-308" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const double      VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;
                Obj  encoder;

                bsl::ostringstream oss;
                Impl impl(&encoder, oss.rdbuf());
                ASSERTV(LINE, 0 == impl.encode(VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode int" << endl;
        {
            const struct {
                int         d_line;
                Int64       d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------
                { L_,      -1, "-1" },
                { L_,       0, "0" },
                { L_,       1, "1" },
                { L_,      42, "42" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const Int64       VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;
                Obj  encoder;

                bsl::ostringstream oss;
                Impl impl(&encoder, oss.rdbuf());
                ASSERTV(LINE, 0 == impl.encode(VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Encode Strings
        //
        // Concerns:
        //: 1 Character are encoded as a single character string.
        //:
        //: 2 All escape charaters are encoded corrected.
        //:
        //: 3 Control characters are encoded as hex.
        //
        // Testing:
        //  int encodeSimple(char value);
        //  int encodeSimple(const bsl::string & value);
        //  int encodeSimple(const char *value);
        // --------------------------------------------------------------------

        if (verbose) cout << "Encode char" << endl;
        {
            const struct {
                int         d_line;
                char        d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------
                { L_,     ' ', "\" \"" },
                { L_,     '!', "\"!\"" },
                { L_,     '0', "\"0\"" },
                { L_,     '9', "\"9\"" },
                { L_,     'A', "\"A\"" },
                { L_,     'Z', "\"Z\"" },
                { L_,     'a', "\"a\"" },
                { L_,     'z', "\"z\"" },
                { L_,     '~', "\"~\"" },
                { L_,     '"', "\"\\\"\"" },
                { L_,    '\\', "\"\\\\\"" },
                { L_,     '/', "\"\\/\"" },
                { L_,    '\b', "\"\\b\"" },
                { L_,    '\f', "\"\\f\"" },
                { L_,    '\n', "\"\\n\"" },
                { L_,    '\r', "\"\\r\"" },
                { L_,    '\t', "\"\\t\"" },
                { L_,    '\0', "\"\\u0000\"" },
                { L_,  '\x01', "\"\\u0001\"" },
                { L_,  '\x1f', "\"\\u001f\"" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char        VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;
                Obj  encoder;

                bsl::ostringstream oss;
                Impl impl(&encoder, oss.rdbuf());
                ASSERTV(LINE, 0 == impl.encode(VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------
                { L_,  "",     "\"\"" },
                { L_,  " ",    "\" \"" },
                { L_,  "~",    "\"~\"" },
                { L_,  "A quick brown fox jump over a lazy dog!",
                               "\"A quick brown fox jump over a lazy dog!\"" },
                { L_,  "\"",   "\"\\\"\"" },
                { L_,  "\\",   "\"\\\\\"" },
                { L_,  "/",    "\"\\/\"" },
                { L_,  "\b",   "\"\\b\"" },
                { L_,  "\f",   "\"\\f\"" },
                { L_,  "\n",   "\"\\n\"" },
                { L_,  "\r",   "\"\\r\"" },
                { L_,  "\t",   "\"\\t\"" },
                { L_,  "\x01", "\"\\u0001\"" },
                { L_,  "\x1f", "\"\\u001f\"" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;
                Obj  encoder;

                if (veryVeryVerbose) cout << "Test 'char *'" << endl;
                {
                    bsl::ostringstream oss;
                    Impl impl(&encoder, oss.rdbuf());
                    ASSERTV(LINE, 0 == impl.encode(VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (veryVeryVerbose) cout << "Test 'string'" << endl;
                {
                    bsl::ostringstream oss;
                    Impl impl(&encoder, oss.rdbuf());
                    ASSERTV(LINE, 0 == impl.encode(bsl::string(VALUE)));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST BCEM_AGGREGATE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);

        bdem_RecordDef *address = schema->createRecord("Address");
        address->appendField(bdem_ElemType::BDEM_STRING,       "street");
        address->appendField(bdem_ElemType::BDEM_STRING,       "city");
        address->appendField(bdem_ElemType::BDEM_STRING,       "state");

        bdem_RecordDef *employee = schema->createRecord("Employee");
        employee->appendField(bdem_ElemType::BDEM_STRING, "name");
        employee->appendField(bdem_ElemType::BDEM_LIST,   address, "homeAddress");
        employee->appendField(bdem_ElemType::BDEM_INT, "age");

        bcem_Aggregate bob(schema, "Employee");

        char jsonText[] =
            "{"
                "\"name\":\"Bob\","
                "\"homeAddress\":{"
                    "\"street\":\"Some Street\","
                    "\"city\":\"Some City\","
                    "\"state\":\"Some State\""
                "},"
                "\"age\":21"
            "}";

        baejsn_Decoder decoder;
        bsl::istringstream iss(jsonText);

        ASSERTV(0 == decoder.decode(iss, &bob));

        //P(bob);

        baejsn_Encoder encoder;
        bsl::ostringstream oss;
        ASSERTV(0 == encoder.encode(oss, bob));
        ASSERTV(oss.str() == jsonText);
        if (verbose) {
            P(oss.str());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // COMPLEX TEST MESSAGES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        std::vector<baea::FeatureTestMessage> testObjects;
        constructFeatureTestMessage(&testObjects);

        for (int ti = 0; ti < (int)testObjects.size(); ++ti) {
            //const char *jsonText = TEST_MESSAGES[ti];

            //P(message);
            baejsn_Encoder encoder;
            bsl::ostringstream oss;
            ASSERTV(0 == encoder.encode(oss, testObjects[ti]));

            //ASSERTV(oss.str() == jsonText);
            if (verbose) {
                P(oss.str());
            }

            baejsn_Decoder decoder;
            bsl::istringstream iss(oss.str());

            baea::FeatureTestMessage object;
            ASSERTV(ti, 0 == decoder.decode(iss, &object));

            ASSERTV(ti, testObjects[ti], object, testObjects[ti] == object);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        test::Employee bob;

        char jsonText[] =
            "{"
                "\"name\":\"Bob\","
                "\"homeAddress\":{"
                    "\"street\":\"Some Street\","
                    "\"city\":\"Some City\","
                    "\"state\":\"Some State\""
                "},"
                "\"age\":21"
            "}";

        bsl::istringstream iss(jsonText);

        baejsn_Decoder decoder;
        ASSERTV(0 == decoder.decode(iss, &bob));

        ASSERTV(bob.name(), "Bob"         == bob.name());
        ASSERT("Some Street" == bob.homeAddress().street());
        ASSERT("Some City"   == bob.homeAddress().city());
        ASSERT("Some State"  == bob.homeAddress().state());
        ASSERT(21            == bob.age());


        baejsn_Encoder encoder;
        bsl::ostringstream oss;
        ASSERTV(0 == encoder.encode(oss, bob));
        ASSERTV(oss.str() == jsonText);
        if (verbose) {
            P(oss.str());
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
