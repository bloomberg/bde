// baea_serializableobjectproxyutil.t.cpp                             -*-C++-*-

#include <baea_serializableobjectproxyutil.h>
#include <baea_serializableobjectproxy.h>

#include <baea_testmessages.h>

#include <bael_defaultobserver.h>
#include <bael_log.h>
#include <bael_loggermanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <baexml_encoder.h>
#include <baexml_decoder.h>
#include <baexml_encoderoptions.h>
#include <baexml_decoderoptions.h>
#include <baexml_minireader.h>

#include <bdem_berencoder.h>
#include <bdem_berdecoder.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdesb_memoutstreambuf.h>

#include <bsls_stopwatch.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bslma_sequentialallocator.h>
#include <cstdlib>

using namespace BloombergLP;
using namespace BloombergLP::baea;

using bsl::cout;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// [  ] void makeEncodeProxy(SerializableObjectProxy *proxy, TYPE *obj);
// [  ] void makeDecodeProxy(SerializableObjectProxy *proxy, TYPE *obj);


//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef baea_SerializableObjectProxy     Proxy;
typedef baea_SerializableObjectProxyUtil Obj;
typedef bdeat_TypeCategory                Category;


const char LOG_CATEGORY[] = "BAEA_SERIALIZABLEOBJECTPROXYUTIL.TEST";

// Disable test of gcc Sun because it is too slow that the nightly build has to
// kill it, causing the build to fail.

#if !(defined(BSLS_PLATFORM__CMP_GNU) && defined(BSLS_PLATFORM__CPU_SPARC))

static const char* TEST_MESSAGES[] = {

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

void enlargeTestObjects(std::vector<baea::FeatureTestMessage>* objects,
                        int                                    arraySize)
{
    std::string longString =
"\"My name is Ozymandias, king of kings:"
"Look on my works, ye Mighty, and despair!\""
"Nothing beside remains.";

    for (std::vector<baea::FeatureTestMessage>::iterator it =
            objects->begin();
            it != objects->end(); ++it) {

        if (it->isSelection1Value() &&
            it->selection1().element4().size() &&
            (int)it->selection1().element4().size() < arraySize) {
            baea::Choice3 value = it->selection1().element4()[0];

            int elementsToAdd = arraySize - it->selection1().element4().size();
            for (int i = 0; i < elementsToAdd; ++i)
            {
                it->selection1().element4().push_back(value);
            }
        }

        if (it->isSelection1Value() &&
            it->selection1().element2().size() &&
            (int)it->selection1().element2().size() < arraySize) {
            baea::Choice1 value = it->selection1().element2()[0];
            int elementsToAdd = arraySize - it->selection1().element2().size();
            for (int i = 0; i < elementsToAdd; ++i)
            {
                it->selection1().element2().push_back(value);
            }
        }

        if (it->isSelection3Value() &&
            !it->selection3().element4().isNull() &&
            it->selection3().element4().value().isSelection3Value()) {
            baea::Sequence4& obj =
                it->selection3().element4().value().selection3();

            int elementsToAdd = arraySize - obj.element17().size();
            for (int i = 0; i < elementsToAdd; ++i) {
                obj.element17().push_back(i);
            }
        }

        if (it->isSelection4Value()) {
            baea::Sequence3& seq = it->selection4();
            for (int i = 0; i < arraySize; ++i) {
                seq.element2().push_back(longString);
            }
        }
    }
}

void constructTestObjects(std::vector<baea::FeatureTestMessage>* objects)
{
    baexml_MiniReader reader;
    baexml_DecoderOptions options;
    baexml_ErrorInfo e;
    baexml_Decoder decoder(&options, &reader, &e);

    for (int i = 0;
             i < (int)(sizeof(TEST_MESSAGES) / sizeof(*TEST_MESSAGES));
             ++i)
    {
        baea::FeatureTestMessage object;
        std::istringstream ss(TEST_MESSAGES[i]);
        int rc = decoder.decode(ss.rdbuf(), &object);
        if (0 != rc) {
            bsl::cout << "Failed to decode from initialization data (i="
                      << i << "): "
                      << decoder.loggedMessages() << bsl::endl;
        }
        if (baea::FeatureTestMessage::SELECTION_ID_UNDEFINED ==
            object.selectionId()) {
            bsl::cout << "Decoded unselected choice from initialization data"
                      << " (i=" << i << "):" << TEST_MESSAGES[i]
                      << bsl::endl;
            rc = 9;
        }
        BSLS_ASSERT(0 == rc); // test invariant
        objects->push_back(object);
    }
}

#endif

//=============================================================================
//                      TEST ACCESSORS AND MANIPULATORS
//-----------------------------------------------------------------------------

struct ExtractAddressAccessor {
    const void *d_address;

    ExtractAddressAccessor() : d_address(0) {};

    int operator()(const baea_SerializableObjectProxy& object)
    {
        d_address = object.object();
        return 0;
    }

    template <class TYPE>
    int operator() (const TYPE&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ExtractAddressManipulator {
    const void *d_address;

    ExtractAddressManipulator() : d_address(0) {};

    int operator()(baea_SerializableObjectProxy *object)
    {
        d_address = object->object();
        return 0;
    }

    template <class TYPE>
    int operator() (TYPE *)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};


template <class TYPE>
struct SimpleAccessor {
    const void *d_address;
    int         d_rc;

    // CREATORS
    SimpleAccessor() : d_address(0), d_rc(0) {};

    // MANIPULATORS
    void reset()
    {
        d_address = 0;
        d_rc = 0;
    }

    int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
    {
        d_address = &object;
        return d_rc;
    }

    template<typename OTHER_TYPE, typename OTHER_CATEGORY>
    int operator()(const OTHER_TYPE&, const OTHER_CATEGORY&)
    {
        // This is needed to compile because there are many Simple types, but
        // should not be called.

        ASSERTV(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct SimpleManipulator {
    void *d_address;

    SimpleManipulator() : d_address(0) {};

    int operator()(TYPE *value, bdeat_TypeCategory::Simple)
    {
        d_address = value;
        return 0;
    }

    template <class OTHER_TYPE, class OTHER_CATEGORY>
    int operator()(OTHER_TYPE *, const OTHER_CATEGORY&)
    {
        // This is needed to compile because there are many Simple types, but
        // should not be called.

        BSLS_ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ByteArrayAccessor {
    const bsl::vector<char> *d_address;

    ByteArrayAccessor() : d_address(0) {}

    int operator() (const bsl::vector<char>& object,
                    const bdeat_TypeCategory::Array&)
    {
        d_address = &object;
        return 0;
    }

    int operator() (const baea_SerializableObjectProxy&,
                    const bdeat_TypeCategory::Array&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ByteArrayManipulator {
    const bsl::vector<char> *d_address;

    ByteArrayManipulator() : d_address(0) {}

    int operator() (bsl::vector<char> *object,
                    const bdeat_TypeCategory::Array&)
    {
        d_address = object;
        return 0;
    }

    int operator() (baea_SerializableObjectProxy *,
                    const bdeat_TypeCategory::Array&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceAccessor {
    const baea_SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_SelectionInfo            d_info;

    ChoiceAccessor() : d_proxy(0), d_address(0) {}

    int operator() (const baea_SerializableObjectProxy& object,
                    const bdeat_SelectionInfo&     info)
    {
        d_proxy = &object;
        d_address = object.object();
        d_info = info;
        return 0;
    }

    template <typename TYPE>
    int operator() (const TYPE&, const bdeat_SelectionInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceManipulator
{
    baea_SerializableObjectProxy *d_proxy;
    const void              *d_address;
    bdeat_SelectionInfo      d_info;

    ChoiceManipulator() : d_proxy(0), d_address(0) {}

    int operator() (baea_SerializableObjectProxy*   object,
                    const bdeat_SelectionInfo& info)
    {
        d_proxy = object;
        d_address = object->object();
        d_info = info;
        return 0;
    }

    template <typename TYPE>
    int operator() (TYPE*, const bdeat_SelectionInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct SequenceAccessor {
    const baea_SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_AttributeInfo            d_info;

    SequenceAccessor() : d_proxy(0), d_address(0) {}

    int operator() (const baea_SerializableObjectProxy& object,
                    const bdeat_AttributeInfo&     info)
    {
        d_proxy = &object;
        d_address = object.object();
        d_info = info;
        return 0;
    }

    template <typename TYPE>
    int operator() (const TYPE& object, const bdeat_AttributeInfo& info)
        // For nullable adaptor.
    {
        ExtractAddressAccessor accessor;
        bdeat_nullableValueAccessValue(object, accessor);
        d_address = accessor.d_address;
        d_info = info;
        return 0;
    }
};

struct SequenceManipulator {
    const baea_SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_AttributeInfo            d_info;

    // CREATORS
    SequenceManipulator() : d_proxy(0), d_address(0) {}

    // MANIPULATORS
    void reset()
    {
        d_proxy = 0;
        d_address = 0;
    }

    int operator() (baea_SerializableObjectProxy    *object,
                    const bdeat_AttributeInfo&  info)
    {
        d_proxy = object;
        d_address = object->object();

        d_info = info;
        return 0;
    }

    template <typename TYPE>
    int operator() (TYPE *object, const bdeat_AttributeInfo& info)
        // For nullable adaptor.
    {
        ExtractAddressAccessor accessor;
        bdeat_nullableValueAccessValue(*object, accessor);
        d_address = accessor.d_address;
        d_info = info;
        return 0;
    }
};

// ============================================================================
//                      TEST DRIVER
// ----------------------------------------------------------------------------

template <class TYPE>
void executeSimpleCategoryTest(const char *typeName)
{
    if (verbose) cout << "Simple encode proxy" << endl;
    {
        TYPE object;
        Proxy mX;  const Proxy& X = mX;

        Obj::makeEncodeProxy(&mX, &object);
        ASSERTV(typeName, Category::BDEAT_SIMPLE_CATEGORY == X.category());

        SimpleAccessor<TYPE> accessor;

        ASSERTV(typeName, 0 == bdeat_typeCategoryAccessSimple(X, accessor));
        ASSERTV(typeName, &object == accessor.d_address);
    }

    if (verbose) cout << "Simple decode proxy" << endl;
    {
        TYPE object;
        Proxy mX;  const Proxy& X = mX;

        Obj::makeDecodeProxy(&mX, &object);
        ASSERTV(typeName, Category::BDEAT_SIMPLE_CATEGORY == X.category());

        SimpleManipulator<TYPE> manipulator;

        ASSERTV(typeName,
                0 == bdeat_typeCategoryManipulateSimple(&mX, manipulator));
        ASSERTV(typeName, &object == manipulator.d_address);
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;

    // if negative test case, interpret the first two arguments after
    // the test number as parameters rather than verboseness
    int verbosityStartPoint = test < 0 ? 5 : 2;

    verbose = (argc > verbosityStartPoint);
    veryVerbose = (argc > verbosityStartPoint+1);
    veryVeryVerbose = (argc > verbosityStartPoint+2);
    veryVeryVeryVerbose = (argc > verbosityStartPoint+3);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bael_DefaultObserver            observer(&bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    bael_Severity::Level            passthrough = bael_Severity::BAEL_OFF;
    bael_LoggerManager::initSingleton(&observer, configuration);

    if (verbose)             passthrough = bael_Severity::BAEL_WARN;
    if (veryVerbose)         passthrough = bael_Severity::BAEL_INFO;
    if (veryVeryVerbose)     passthrough = bael_Severity::BAEL_DEBUG;
    if (veryVeryVeryVerbose) passthrough = bael_Severity::BAEL_TRACE;

    bael_LoggerManager::singleton().setDefaultThresholdLevels(
                                                      bael_Severity::BAEL_OFF,
                                                      passthrough,
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_OFF);

    switch (test) { case 0: // Zero is always the leading case.
#if !(defined(BSLS_PLATFORM__CMP_GNU) && defined(BSLS_PLATFORM__CPU_SPARC))
      case 15: {
#else
      case 11: {
#endif
        // --------------------------------------------------------------------
        // Usage Example
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

///Usage Example
///=============
// In this section we show the intended usage of this component.
//
///Example 1: Serializing a BAS Request
///------------------------------------
// In this example, we demonstrate how to encode a BAS message object using
// 'baea_SerializableObjectProxy' to reduce the complexity of the resulting
// object code as compared with encoding and decoding the message object
// directly.
//
// Suppose we are given a BAS message component with a request type named
// 'SimpleRequest' which has the field 'data' of type 'bsl::string' and
// 'responseLength' of type 'int'.  First, we create a 'Request' object named
// 'encodeMessage' that we would like to encode:
//..
    baea::Request encodeMessage;
    encodeMessage.makeSimpleRequest();
    encodeMessage.simpleRequest().data() = "Test message";
    encodeMessage.simpleRequest().responseLength() = 12;
//..
// Then, we create a proxy object to be used when encoding:
//..
    baea_SerializableObjectProxy encodeProxy;
//..
// Next, we use the 'makeEncodeProxy' method to populate 'encodeProxy' to
// represent the object that we would like to encode.
//..
    baea_SerializableObjectProxyUtil::makeEncodeProxy(&encodeProxy,
                                                       &encodeMessage);
//..
// Now, we setup the encoder and encode the request into a memory stream
// buffer:
//..
    baexml_EncoderOptions eOptions;
    baexml_Encoder encoder(&eOptions);
    bdesb_MemOutStreamBuf osb;
    encoder.encode(&osb, encodeProxy);
//..
// Finally, we can print out the XML string that is stored in the memory
// buffer:
//..
    if (verbose)
    std::cout << osb.data() << std::endl;
//..
//
///Example 2: Decoding an XML String
///---------------------------------
// In this example, we demonstrate how to decode a XML string using the
// 'baea_SerializableObjectProxy'.
//
// First, we create the XML string that we would like to decode into a
// 'SimpleRequest' that was described in the previous example:
//..
    const char *xmlString =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
            "<Request xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
            "<simpleRequest>"
                "<data>Hello, world!</data>"
                "<responseLength>13</responseLength>"
            "</simpleRequest>"
        "</Request>";
//..
// Then, we create the request object we would like the XML string to be
// decoded into.
//..
    baea::Request decodeMessage;
//..
// Next, we create the proxy object to be used when decoding:
//..
    baea_SerializableObjectProxy decodeProxy;
//..
// Then, we use the 'makeDecodeProxy' method to populate 'decodeProxy' to
// represent the object that we would like to decode to.
//..
    baea_SerializableObjectProxyUtil::makeDecodeProxy(&decodeProxy,
                                                       &decodeMessage);
//..
// Next, we setup the decoder and decode the message:
//..
    baexml_DecoderOptions dOptions;
    baexml_MiniReader reader;
    baexml_Decoder decoder(&dOptions, &reader);
//
    bdesb_FixedMemInStreamBuf isb(xmlString, strlen(xmlString));
    decoder.decode(&isb, &decodeProxy);
//..
// Finally, we verify the 'decodeMessage' contains the expected values.
//..
    ASSERT(true == decodeMessage.isSimpleRequestValue());
    ASSERT("Hello, world!" == decodeMessage.simpleRequest().data());
    ASSERT(13 == decodeMessage.simpleRequest().responseLength());
//..
      } break;
#if !(defined(BSLS_PLATFORM__CMP_GNU) && defined(BSLS_PLATFORM__CPU_SPARC))
      case 14: {
        // --------------------------------------------------------------------
        // XML decoder feature test
        //
        // Plan: for each of several objects which collectively represent the
        // functionality of the encoder, encode the object in XML using the
        // standard method, then decode it using the
        // 'baea_SerializableObjectProxy'.  Assert that the two objects are
        // identical.
        // --------------------------------------------------------------------
        std::vector<baea::FeatureTestMessage> testObjects;
        constructTestObjects(&testObjects);

        for (int i = 0; i < (int)testObjects.size(); ++i) {
            bdesb_MemOutStreamBuf osb;

            baexml_EncoderOptions eOptions;
            baexml_Encoder encoder(&eOptions);

            int rc = encoder.encode(&osb, testObjects[i]);
            BSLS_ASSERT(0 == rc); //test invariant

            baea::FeatureTestMessage decoded;

            baea_SerializableObjectProxy decorator;
            baea_SerializableObjectProxyUtil::makeDecodeProxy(&decorator,
                                                         &decoded);

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());

            baexml_MiniReader reader;
            baexml_DecoderOptions options;
            baexml_ErrorInfo e;
            baexml_Decoder decoder(&options, &reader, &e);

            rc = decoder.decode(&isb, &decorator);
            LOOP_ASSERT(i, 0 == rc);
            if (0 == rc) {
                LOOP2_ASSERT(testObjects[i], decoded,
                             testObjects[i] == decoded);
            }
            else {
                break;
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // BER decoder feature test
        //
        // Plan: for each of several objects which collectively represent the
        // functionality of the encoder, encode the object in BER using the
        // standard method, then decode it using the
        // 'baea_SerializableObjectProxy'.  Assert that the two objects are
        // identical.
        // --------------------------------------------------------------------
        std::vector<baea::FeatureTestMessage> testObjects;
        constructTestObjects(&testObjects);

        for (int i = 0; i < (int)testObjects.size(); ++i) {
            bdesb_MemOutStreamBuf osb;

            bdem_BerEncoder encoder;
            bdem_BerDecoder decoder;

            int rc = encoder.encode(&osb, testObjects[i]);
            BSLS_ASSERT(0 == rc); //test invariant

            baea::FeatureTestMessage decoded;

            baea_SerializableObjectProxy decorator;
            baea_SerializableObjectProxyUtil::makeDecodeProxy(&decorator,
                                                         &decoded);

            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
            rc = decoder.decode(&isb, &decorator);
            LOOP_ASSERT(i, 0 == rc);
            if (0 == rc) {
                LOOP2_ASSERT(testObjects[i], decoded,
                             testObjects[i] == decoded);
            }
            else {
                break;
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // XML encoder feature test
        //
        // Plan: for each of several objects which collectively represent the
        // functionality of the encoder, encode the object in XML using
        // 'baea_SerializableObjectProxy', then decode it using the standard
        // method.  Assert that the two objects are identical.
        // --------------------------------------------------------------------
        std::vector<baea::FeatureTestMessage> testObjects;
        constructTestObjects(&testObjects);

        baexml_MiniReader reader;
        baexml_DecoderOptions options;
        baexml_ErrorInfo e;
        baexml_Decoder decoder(&options, &reader, &e);

        baexml_EncoderOptions eOptions;
        baexml_Encoder encoder(&eOptions);

        int i = 0;
        for (std::vector<baea::FeatureTestMessage>::iterator it =
                 testObjects.begin(); it != testObjects.end(); ++it) {
            bdesb_MemOutStreamBuf osb;
            baea_SerializableObjectProxy decorator;
            baea_SerializableObjectProxyUtil::makeEncodeProxy(&decorator,
                                                              &(*it));

            ASSERT(0 == encoder.encode(&osb, decorator));

            baea::FeatureTestMessage decoded;
            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &decoded));

            LOOP2_ASSERT(*it, decoded, *it == decoded);
            if (verbose && *it != decoded) {
                std::string debug(osb.data(), osb.length());
                cout << "i=" << i
                     << ": XML=\"" << debug << "\"" << endl;
            }

            ++i;
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // BER encoder feature test
        //
        // Plan: for each of several objects which collectively represent the
        // functionality of the encoder, encode the object in BER using
        // baea_SerializableObjectProxy, then decode it using the standard
        // method.  Assert that the two objects are identical.
        // --------------------------------------------------------------------
        std::vector<baea::FeatureTestMessage> testObjects;
        constructTestObjects(&testObjects);

        for (int i = 0; i < (int)testObjects.size(); ++i) {
            bdesb_MemOutStreamBuf osb;
            baea_SerializableObjectProxy decorator;
            baea_SerializableObjectProxyUtil::makeEncodeProxy(&decorator,
                                                         &testObjects[i]);

            bdem_BerEncoder encoder;
            bdem_BerDecoder decoder;

            int rc = encoder.encode(&osb, decorator);
            LOOP_ASSERT(i, 0 == rc);
            if (0 != rc) {
                cout << encoder.loggedMessages() << endl;
                break;
            }

            baea::FeatureTestMessage decoded;
            bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
            ASSERT(0 == decoder.decode(&isb, &decoded));
            LOOP2_ASSERT(testObjects[i], decoded, testObjects[i] == decoded);
        }
      } break;
#endif
      case 10: {
        // --------------------------------------------------------------------
        // XML breathing test
        //
        // Concern: that a baea_SerializableObjectProxy can be populated and
        // then the XML encoder/decoder can be invoked on it.
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "XML breathing" << endl
                 << "=============" << endl;
        }
        baea::Request request;
        request.makeSimpleRequest().data() = "The quick brown fox";

        baexml_EncoderOptions eOptions;
        baexml_Encoder encoder(&eOptions);

        baea_SerializableObjectProxy decorator;
        baea_SerializableObjectProxyUtil::makeEncodeProxy(&decorator,
                                                     &request);

        bdesb_MemOutStreamBuf osb;
        ASSERT(0 == encoder.encode(&osb, decorator));

        request.reset();
        baea_SerializableObjectProxyUtil::makeDecodeProxy(&decorator,
                                                     &request);

        bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());

        baexml_MiniReader reader;
        baexml_DecoderOptions options;
        baexml_ErrorInfo e;
        baexml_Decoder decoder(&options, &reader, &e);

        ASSERT(0 == decoder.decode(&isb, &decorator));
        ASSERT(request.isSimpleRequestValue());
        ASSERT(request.simpleRequest().data() == "The quick brown fox");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // BER breathing test
        //
        // Concern: that a baea_SerializableObjectProxy can be populated and
        // then the BER encoder/decoder can be invoked on it.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BER breathing" << endl
                 << "=============" << endl;
        }
        baea::Request request;
        request.makeSimpleRequest().data() = "The quick brown fox";

        bdem_BerEncoder encoder;

        baea_SerializableObjectProxy decorator;
        baea_SerializableObjectProxyUtil::makeEncodeProxy(&decorator,
                                                     &request);

        bdesb_MemOutStreamBuf osb;
        ASSERT(0 == encoder.encode(&osb, decorator));

        request.reset();
        baea_SerializableObjectProxyUtil::makeDecodeProxy(&decorator,
                                                     &request);

        bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());

        bdem_BerDecoder decoder;

        ASSERT(0 == decoder.decode(&isb, &decorator));
        ASSERT(request.isSimpleRequestValue());
        ASSERT(request.simpleRequest().data() == "The quick brown fox");

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CUSTOMIZED
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches another
        //:   'makeEncode/DecodeProxy' method for the the underlying base type
        //:   of the Customized type.
        //:
        //: 2 The 'loader', 'valueMaker' and 'objectFetcher' methods supplied
        //:   to 'loadNullable' behaves correctly.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Nullable type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Nullable type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify manipulate methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Customized);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Customized);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CUSTOMIZED" << endl
                          << "==================" << endl;

        if (verbose) cout << "Testing Customized type for encoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;

            baea::CustomInt object;
            Obj::makeEncodeProxy(&mX, &object);

            ASSERTV(Category::BDEAT_SIMPLE_CATEGORY == X.category());

            SimpleAccessor<int> accessor;

            ASSERTV(0 == bdeat_typeCategoryAccessSimple(X, accessor));
            ASSERTV(&object.toInt() == accessor.d_address);
        }

        if (verbose) cout << "Testing Customized type for decoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;

            baea::CustomInt object;
            Obj::makeDecodeProxy(&mX, &object);

            ASSERTV(Category::BDEAT_SIMPLE_CATEGORY == X.category());

            SimpleManipulator<int> manipulator;

            ASSERTV(0 == bdeat_typeCategoryManipulateSimple(&mX, manipulator));
            ASSERTV(&object.toInt() == manipulator.d_address);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING NULLABLE
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches the correct
        //:   "load" method when supplied with a Nullable type.
        //:
        //: 2 The 'loader', 'valueMaker' and 'objectFetcher' methods supplied
        //:   to 'loadNullable' behaves correctly.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Nullable type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Nullable type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify manipulate methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Nullable);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Nullable);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING NULLABLE" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing Nullable type for encoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;
            bdeut_NullableValue<int> object;

            Obj::makeEncodeProxy(&mX, &object);

            SimpleAccessor<int> accessor;
            ASSERTV(-1 == bdeat_typeCategoryAccessNullableValue(X, accessor));
            ASSERTV(true == X.isNull());

            object.makeValue(1);

            Obj::makeEncodeProxy(&mX, &object);

            ASSERTV(0 == bdeat_typeCategoryAccessNullableValue(X, accessor));
            ASSERTV(&object.value() == accessor.d_address);
        }

        if (verbose) cout << "Testing Nullable type for decoding" << endl;
        {
            Proxy mX;
            bdeut_NullableValue<int> object;

            Obj::makeDecodeProxy(&mX, &object);

            ASSERTV(true == object.isNull());

            mX.makeValue();
            ASSERTV(false == object.isNull());

            SimpleManipulator<int> manipulator;
            ASSERTV(0 == bdeat_typeCategoryManipulateNullableValue(
                                                                 &mX,
                                                                 manipulator));
            ASSERTV(&object.value() == manipulator.d_address);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING SEQUENCE
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches the correct
        //:   "load" method when supplied with a Sequence type.
        //:
        //: 2 The 'loader' method supplied to 'loadSequence' behaves correctly.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Sequence type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Sequence type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify manipulate methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Sequence);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Sequence);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SEQUENCE" << endl
                          << "================" << endl;

        const bdeat_AttributeInfo *INFO = Sequence1::ATTRIBUTE_INFO_ARRAY;
        const int NUM_INFO = Sequence1::NUM_ATTRIBUTES;

        if (verbose) cout << "Testing Sequence type for encoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;
            Sequence1 object;
            object.element1().makeValue();
            object.element3().makeSelection(0);
            Obj::makeEncodeProxy(&mX, &object);

            ASSERTV(0 == strcmp(baea::Sequence1::CLASS_NAME, X.className()));

            ASSERTV(Category::BDEAT_SEQUENCE_CATEGORY == X.category());

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int ID = INFO[ti].d_id;

                SequenceAccessor accessor;
                ASSERTV(0 == bdeat_sequenceAccessAttribute(X, accessor, ID));

                ASSERTV(accessor.d_info == INFO[ti]);
                switch (ti) {
                  case 0: {
                      ASSERTV(accessor.d_address ==
                                                   &object.element1().value());
                  } break;
                  case 1: {
                      ASSERTV(accessor.d_address == object.element2().data());
                  } break;
                  case 2: {
                      ASSERTV(accessor.d_address == &object.element3());
                  } break;
                  case 3: {
                      ASSERTV(accessor.d_address == object.element4().data());
                  } break;
                }
            }
        }

        if (verbose) cout << "Testing Sequence type for encoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;

            Sequence1 object;
            object.element1().makeValue();
            object.element3().makeSelection(0);
            Obj::makeDecodeProxy(&mX, &object);

            ASSERTV(Category::BDEAT_SEQUENCE_CATEGORY == X.category());

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int ID = INFO[ti].d_id;

                SequenceManipulator manipulator;
                ASSERTV(0 == bdeat_sequenceManipulateAttribute(&mX,
                                                               manipulator,
                                                               ID));

                ASSERTV(manipulator.d_info == INFO[ti]);
                switch (ti) {
                  case 0: {
                      ASSERTV(manipulator.d_address ==
                                                   &object.element1().value());
                  } break;
                  case 1: {
                      ASSERTV(manipulator.d_address == &object.element2());
                  } break;
                  case 2: {
                      ASSERTV(manipulator.d_address == &object.element3());
                  } break;
                  case 3: {
                      ASSERTV(manipulator.d_address == &object.element4());
                  } break;
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CHOICE
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches the correct
        //:   "load" method when supplied with a Choice type.
        //:
        //: 2 The 'chooser' and 'loader' method supplied to
        //:   'loadChoiceForEncoding' or 'loadChoiceForDecoding' behaves
        //:   correctly.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Choice type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Choice type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify manipulate methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Choice);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Choice);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CHOICE" << endl
                          << "==============" << endl;

        const bdeat_SelectionInfo *INFO = Choice1::SELECTION_INFO_ARRAY;
        const int NUM_INFO = Choice1::NUM_SELECTIONS;

        if (verbose) cout << "Testing Choice type for encoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;
            baea::Choice1 object;

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int ID = INFO[ti].d_id;

                object.makeSelection(ID);
                if (3 == ti) {
                    object.selection4().makeSelection(1);
                }

                Obj::makeEncodeProxy(&mX, &object);
                ASSERTV(0 == strcmp(baea::Choice1::CLASS_NAME, X.className()));

                ASSERTV(Category::BDEAT_CHOICE_CATEGORY == X.category());

                ChoiceAccessor accessor;
                ASSERTV(0 == bdeat_choiceAccessSelection(mX, accessor));

                switch (ti) {
                  case 0:
                  case 1: {
                    ASSERTV(&object == accessor.d_address);
                    ASSERTV(INFO[ti] == accessor.d_info);
                  } break;
                  case 2: {
                    ASSERTV(&object.selection3() == accessor.d_address);
                    ASSERTV(INFO[ti] == accessor.d_info);
                  } break;
                  case 3: {
                    ASSERTV(&object.selection4() == accessor.d_address);
                    ASSERTV(INFO[ti] == accessor.d_info);
                  } break;
                }
            }
        }

        if (verbose) cout << "Testing Choice type for decoding" << endl;
        {
            Proxy mX;  const Proxy& X = mX;
            baea::Choice1 object;
            Obj::makeDecodeProxy(&mX, &object);

            ASSERTV(Category::BDEAT_CHOICE_CATEGORY == X.category());

            for (int ti = 0; ti < NUM_INFO; ++ti) {
                const int ID = INFO[ti].d_id;

                ASSERTV(0  == bdeat_choiceMakeSelection(&mX, ID));
                ASSERTV(ID == object.selectionId());

                ChoiceManipulator manipulator;
                ASSERTV(0 == bdeat_choiceManipulateSelection(&mX,
                                                             manipulator));

                ASSERTV(manipulator.d_info == INFO[ti]);
                switch (ti) {
                  case 0: {
                      ASSERTV(manipulator.d_address == &object.selection1());
                  } break;
                  case 1: {
                      ASSERTV(manipulator.d_address == &object.selection2());
                  } break;
                  case 2: {
                      ASSERTV(manipulator.d_address == &object.selection3());
                  } break;
                  case 3: {
                      ASSERTV(manipulator.d_address == &object.selection4());
                  } break;
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ARRAY
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches the correct
        //:   "load" method when supplied with a Array type.
        //:
        //: 2 Supplying a 'vector<char>' to the 'makeEncodeProxy' or
        //:   'makeDecodeProxy' creates a byte array.
        //:
        //: 3 The 'resizer' and 'loader' method supplied to 'loadArray' behaves
        //:   correctly.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Array type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Array type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify manipulate methods behave as expected.
        //:
        //: 7 Invoke 'bdeat_arrayResize' and verify the 'vector' being
        //:   represented is resized.
        //:
        //: 8 Create a proxy for a 'vector<char>'.
        //:
        //: 9 Verify that the proxy has the expected category and
        //:   'isByteArrayValue' is 'true'.
        //:
        //:10 Verify access methods behave as expected.
        //:
        //:11 Verify manipulate methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Array);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Array);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ARRAY" << endl
                          << "=============" << endl;

        if (verbose) cout << "Testing Array type for encoding" << endl;
        {
            Proxy mX; const Proxy& X = mX;

            bsl::vector<int> object;
            const int SIZE = 5;
            object.resize(SIZE);
            for (int i = 0; i < SIZE; ++i) {
                object[i] = i;
            }

            Obj::makeEncodeProxy(&mX, &object);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());

            ASSERTV(false == X.isByteArrayValue());

            for (int i = 0; i < (int)object.size(); ++i) {
                ExtractAddressAccessor accessor;
                ASSERTV(0 == bdeat_arrayAccessElement(X, accessor, i));
                ASSERTV(&object[i] == accessor.d_address);
            }

        }

        if (verbose) cout << "Testing Array type for decoding" << endl;
        {
            Proxy mX; const Proxy& X = mX;

            bsl::vector<int> object;
            Obj::makeDecodeProxy(&mX, &object);

            ASSERTV(Category::BDEAT_ARRAY_CATEGORY == X.category());
            ASSERTV(false == X.isByteArrayValue());

            bdeat_arrayResize(&mX, 1);
            ASSERTV(1 == object.size());

            bdeat_arrayResize(&mX, 5);
            ASSERTV(5 == object.size());

            ASSERTV(false == X.isByteArrayValue());

            for (int i = 0; i < (int)object.size(); ++i) {
                ExtractAddressManipulator manipulator;
                ASSERTV(0 ==
                            bdeat_arrayManipulateElement(&mX, manipulator, i));
                ASSERTV(&object[i] == manipulator.d_address);
            }
        }

        if (verbose) cout << "Testing byte array" << endl;
        {
            Proxy mX; const Proxy& X = mX;

            bsl::vector<char> object;
            object.push_back(0);
            object.push_back(1);
            object.push_back(2);
            object.push_back(3);
            object.push_back(4);

            Obj::makeEncodeProxy(&mX, &object);

            ASSERTV(true == X.isByteArrayValue());

            ByteArrayAccessor accessor;
            ASSERTV(0 == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(&object == accessor.d_address);

            Obj::makeDecodeProxy(&mX, &object);

            ASSERTV(true == X.isByteArrayValue());

            ByteArrayManipulator manipulator;
            ASSERTV(0 == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(&object == manipulator.d_address);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ENUMERATION
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches the correct
        //:   "load" method when supplied with a Enumeration type.
        //:
        //: 2 The 'intSetter' and 'stringSetter' supplied to 'loadEnum' behaves
        //:   correctly.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Enumeration type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Enumeration type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify setter methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Enumeration);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Enumeration);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ENUMERATION" << endl
                          << "===================" << endl;

        const bdeat_EnumeratorInfo *INFO = Enumerated::ENUMERATOR_INFO_ARRAY;
        const int INFO_SIZE = Enumerated::NUM_ENUMERATORS;

        if (verbose) cout << "Enumeration encode proxy" << endl;
        {
            for (int ti = 0; ti < (int)INFO_SIZE; ++ti) {
                const int   VALUE = INFO[ti].d_value;

                Proxy mX;  const Proxy& X = mX;

                Enumerated::Value object;
                Enumerated::fromInt(&object, VALUE);

                Obj::makeEncodeProxy(&mX, &object);

                ASSERTV(Category::BDEAT_ENUMERATION_CATEGORY == X.category());

                int intResult;
                bdeat_enumToInt(&intResult, X);
                ASSERTV(VALUE == intResult);
            }
        }

        if (verbose) cout << "Enumeration decode proxy" << endl;
        {
            Proxy mX;
            Enumerated::Value object;

            Obj::makeDecodeProxy(&mX, &object);

            for (int ti = 0; ti < INFO_SIZE; ++ti) {
                const int   VALUE  = INFO[ti].d_value;
                const char *NAME   = INFO[ti].d_name_p;
                const int   LENGTH = INFO[ti].d_nameLength;

                object = Enumerated::NEW_YORK;  // reset object
                ASSERTV(0 == bdeat_enumFromInt(&mX, ti));
                ASSERTV(VALUE == object);

                object = Enumerated::NEW_YORK;  // reset object
                ASSERTV(0 == bdeat_enumFromString(&mX, NAME, LENGTH));
                ASSERTV(VALUE == object);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING SIMPLE TYPE
        //
        // Concerns:
        //: 1 'makeEncodeProxy' and 'makeDecodeProxy' dispatches the correct
        //:   "load" method when supplied with a Simple type.
        //
        // Plan:
        //: 1 Create a proxy for encoding with a Simple type.
        //:
        //: 2 Verify that the proxy has the expected category.
        //:
        //: 3 Verify access methods behave as expected.
        //:
        //: 4 Create a proxy for encoding with a Simple type.
        //:
        //: 5 Verify that the proxy has the expected category.
        //:
        //: 6 Verify manipulate methods behave as expected.
        //
        // Testing:
        //  void makeEncodeProxy(ObjectProxy *proxy, TYPE *obj, Simple);
        //  void makeDecodeProxy(ObjectProxy *proxy, TYPE *obj, Simple);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SIMPLE TYPE" << endl
                          << "===================" << endl;

        executeSimpleCategoryTest<char>("char");
        executeSimpleCategoryTest<unsigned char>("uchar");
        executeSimpleCategoryTest<short>("short");
        executeSimpleCategoryTest<int>("int");
        executeSimpleCategoryTest<bsls_Types::Uint64>("uint64");
        executeSimpleCategoryTest<float>("float");
        executeSimpleCategoryTest<double>("double");
        executeSimpleCategoryTest<bsl::string>("string");
        executeSimpleCategoryTest<bdet_Datetime>("datetime");
        executeSimpleCategoryTest<bdet_Date>("date");
        executeSimpleCategoryTest<bdet_Time>("time");
        executeSimpleCategoryTest<bool>("bool");
        executeSimpleCategoryTest<bdet_DatetimeTz>("datetimetz");
        executeSimpleCategoryTest<bdet_DateTz>("datetz");
        executeSimpleCategoryTest<bdet_TimeTz>("timetz");
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
        //: 1 Create a Simple proxy object and verify its category.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SIMPLE TYPE" << endl
                          << "===================" << endl;

        if (verbose) cout << "Simple encode proxy" << endl;
        {
            char object = 1;
            Proxy mX;  const Proxy& X = mX;

            Obj::makeEncodeProxy(&mX, &object);
            ASSERTV(Category::BDEAT_SIMPLE_CATEGORY == X.category());
        }

        if (verbose) cout << "Simple decode proxy" << endl;
        {
            char object = 1;
            Proxy mX;  const Proxy& X = mX;

            Obj::makeDecodeProxy(&mX, &object);
            ASSERTV(Category::BDEAT_SIMPLE_CATEGORY == X.category());
        }
      } break;
#if !(defined(BSLS_PLATFORM__CMP_GNU) && defined(BSLS_PLATFORM__CPU_SPARC))
      case -2: {
        // --------------------------------------------------------------------
        // Codec performance test: SERIALIZABLEOBJECTPROXY
        //
        // Encode all the test messages -- but modified with template magic to
        // make all their arrays much larger -- into memory many times, for
        // performance testing.  Do this with baea_SerializableObjectProxy.
        // Case -1 is an identical test but uses the generated types.
        // --------------------------------------------------------------------

        int arraySize = 100;
        if (argc > 2) {
            arraySize = atoi(argv[2]);
        }
        int numIter = 200;
        if (argc > 3) {
             numIter = atoi(argv[3]);
        }
        bool isXml = false;
        if (argc > 4) {
            isXml = argv[4][0] == 'X' || argv[4][0] == 'x';
        }

        cout << "BENCHMARK: SERIALIZABLEOBJECTPROXY ENCODING" << endl
             << "USAGE: -1 <arraySize> <numIter> <encodingType>" << endl
             << "(arraySize=" << arraySize
             << " numIter=" << numIter
             << " encoding=" << (isXml ? 'X' : 'B')
             << ")" << endl
             << "==========================" << endl;

        std::vector<baea::FeatureTestMessage> objects;
        constructTestObjects(&objects);
        enlargeTestObjects(&objects, arraySize);

        if (verbose) {
            cout << "Test objects constructed." << endl;
        }

        // BER
        bdem_BerEncoder encoder;
        baexml_EncoderOptions options;
        baexml_Encoder xEncoder(&options);
        bdem_BerDecoder decoder;
        baexml_MiniReader reader;
        baexml_DecoderOptions dOptions;
        baexml_ErrorInfo e;
        baexml_Decoder xDecoder(&dOptions, &reader, &e);

        enum {
            BUFFER_SIZE = 256 * 1024
        };
        char BUFFER[BUFFER_SIZE];
        bslma_SequentialAllocator alloc(BUFFER, BUFFER_SIZE);

        unsigned numBytesEncoded = 0;

        baea::FeatureTestMessage request;
        baea_SerializableObjectProxy requestDec;
        baea_SerializableObjectProxyUtil::makeDecodeProxy(&requestDec,
                                                          &request);

        bsls_Stopwatch timer;
        timer.start(true);
        for (int i = 0; i < numIter; ++i) {
            for (std::vector<baea::FeatureTestMessage>::iterator jt =
                     objects.begin();
                 jt != objects.end(); ++jt) {
                alloc.release();
                bdesb_MemOutStreamBuf osb(&alloc);

                baea_SerializableObjectProxy decorator;
                baea_SerializableObjectProxyUtil::makeEncodeProxy(&decorator,
                                                             &(*jt));

                if (isXml) {
                    ASSERT(0 == xEncoder.encode(&osb, decorator));
                }
                else {
                    ASSERT(0 == encoder.encode(&osb, decorator));
                }

                if (isXml) {
                    bdesb_FixedMemInStreamBuf isb2(osb.data(), osb.length());
                    request.reset();
                    ASSERT(0 == xDecoder.decode(&isb2, &requestDec));
                }
                else {
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                    request.reset();
                    ASSERT(0 == decoder.decode(&isb, &requestDec));
                }
                numBytesEncoded += osb.length();
            }
        }
        timer.stop();
        double systemTime, userTime, wallTime;
        timer.accumulatedTimes(&systemTime, &userTime, &wallTime);
        printf("Bytes Encoded: %u\n"
               "Objects Encoded: %d\n"
               "Encoding time: WALL: %3.2lf USER %3.2lf SYSTEM %3.2lf\n",
               numBytesEncoded, (int)(numIter * objects.size()),
               wallTime, userTime, systemTime);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // Codec performance test: GENERATED
        //
        // Encode all the test messages -- but modified with template magic
        // to make all their arrays much larger -- into memory many times, for
        // performance testing.  Do this without baea_SerializableObjectProxy.
        // Case -2 is an identical test but uses baea_SerializableObjectProxy.
        // --------------------------------------------------------------------

        int arraySize = 100;
        if (argc > 2) {
            arraySize = atoi(argv[2]);
        }
        int numIter = 200;
        if (argc > 3) {
             numIter = atoi(argv[3]);
        }
        bool isXml = false;
        if (argc > 4) {
            isXml = argv[4][0] == 'X' || argv[4][0] == 'x';
        }

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        bsl::cerr << "!ABORTING BENCHMARK!  Safe mode build!" << bsl::endl;
        return 1;
#endif

        cout << "BENCHMARK: GENERATED ENCODING" << endl
             << "USAGE: -1 <arraySize> <numIter>" << endl
             << "(arraySize=" << arraySize
             << " numIter=" << numIter
             << " encoding=" << (isXml ? 'X' : 'B')
             << ")" << endl
             << "==========================" << endl;

        std::vector<baea::FeatureTestMessage> objects;
        constructTestObjects(&objects);
        enlargeTestObjects(&objects, arraySize);

        if (verbose) {
            cout << "Test objects constructed." << endl;
        }

        bdem_BerEncoder encoder;
        baexml_EncoderOptions options;
        baexml_Encoder xEncoder(&options);
        bdem_BerDecoder decoder;
        baexml_MiniReader reader;
        baexml_DecoderOptions dOptions;
        baexml_ErrorInfo e;
        baexml_Decoder xDecoder(&dOptions, &reader, &e);

        enum {
            BUFFER_SIZE = 256 * 1024
        };
        char BUFFER[BUFFER_SIZE];
        bslma_SequentialAllocator alloc(BUFFER, BUFFER_SIZE);

        unsigned numBytesEncoded = 0;

        baea::FeatureTestMessage request;

        bsls_Stopwatch timer;
        timer.start(true);
        for (int i = 0; i < numIter; ++i) {
            for (std::vector<baea::FeatureTestMessage>::iterator jt =
                     objects.begin();
                 jt != objects.end(); ++jt) {
                alloc.release();
                bdesb_MemOutStreamBuf osb(&alloc);

                if (isXml) {
                    ASSERT(0 == xEncoder.encode(&osb, *jt));
                }
                else {
                    ASSERT(0 == encoder.encode(&osb, *jt));
                }

                if (isXml) {
                    bdesb_FixedMemInStreamBuf isb2(osb.data(), osb.length());
                    request.reset();
                    ASSERT(0 == xDecoder.decode(&isb2, &request));
                }
                else {
                    bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
                    request.reset();
                    ASSERT(0 == decoder.decode(&isb, &request));
                }

                numBytesEncoded += osb.length();
            }
        }
        timer.stop();
        double systemTime, userTime, wallTime;
        timer.accumulatedTimes(&systemTime, &userTime, &wallTime);
        printf("Bytes Encoded: %u\n"
               "Objects Encoded: %d\n"
               "Encoding time: WALL: %3.2lf USER %3.2lf SYSTEM %3.2lf\n",
               numBytesEncoded, (int)(numIter * objects.size()),
               wallTime, userTime, systemTime);

      } break;
#endif
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
