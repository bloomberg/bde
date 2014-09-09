// bslim_printer.t.cpp                                                -*-C++-*-

#include <bslim_printer.h>

#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_iostream.h>
#include <bsl_deque.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>
#include <bsl_sstream.h>
#include <bsl_cctype.h>
#include <bsl_cstring.h>

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

using namespace BloombergLP;
using namespace bsl;
using namespace bslim;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a mechanism class having accessor
// methods and method templates, each of which is tested over a number of test
// cases:
//: 1 Test initial indentation for different values of 'level' and
//:   'spacesPerLevel', and the output when a 'name' is supplied.
//: 2 Test the output of each method and method template (and its
//:   specializations, if any), for positive and negative values of
//:   'spacesPerLevel'.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] Printer(bsl::ostream *stream, int level, int spacesPerLevel);
//
// ACCESSORS
// [ 2] absLevel() const;
// [ 5] end() const;
// [ 6] template<class TYPE>
//         void print(const TYPE& data, const char *name) const;
// [ 7] template<class TYPE>
//         void printAttribute(const char *name, const TYPE& data) const;
// [12] template <class TYPE, class PRINT_FUNCTOR>
//         void printForeign(const TYPE&           data,
//                           const PRINT_FUNCTOR&  printFunctionObject,
//                           const char           *name) const;
// [11] void printHexAddr(const void *address, const char *name) const;
// [ 9] template <class TYPE>
//      void printOrNull(const TYPE& address, const char *name) const;
// [ 7] template<class TYPE>
//         void printValue(const TYPE& data) const;
// [ 2] spacesPerLevel() const;
// [ 4] start() const;
// [ 2] suppressInitialIndentFlag() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] 'printValue' ALL STL SEQUENCE AND ASSOCIATIVE CONTAINERS
// [17] USAGE EXAMPLE 1
// [18] USAGE EXAMPLE 2
// [19] USAGE EXAMPLE 3
// [20] USAGE EXAMPLE 4

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
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
#define T_ cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef Printer Obj;

struct TestEnumNoStreaming {
    // A test enum with no streaming operator.

    enum Enum {
        VALUE_A,
        VALUE_B
    };
};

struct TestEnumWithStreaming {
    // A test enum with a streaming operator.

    enum Enum {
        VALUE_A,
        VALUE_B
    };
};

bsl::ostream& operator<<(bsl::ostream&               stream,
                         TestEnumWithStreaming::Enum value)
{
    const char *ascii;
    switch (value) {
      case TestEnumWithStreaming::VALUE_A:
        ascii = "VALUE_A";
        break;
      case TestEnumWithStreaming::VALUE_B:
        ascii = "VALUE_B";
        break;
      default:
        BSLS_ASSERT(false);
    }
    stream << ascii;
    return stream;
}

//=============================================================================
//                        GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct HasPrint {
    // Class having a 'print' method

    // DATA
    int d_data;
    mutable int d_level;
    mutable int d_spacesPerLevel;

    // CREATORS
    explicit
    HasPrint(int data) : d_data(data) {}

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const
    {
        d_level = level;
        d_spacesPerLevel = spacesPerLevel;
        Obj printer(&stream, level, spacesPerLevel);
        printer.printAttribute("HasPrint:data", d_data);
        return stream;
    }
};

class NoPrint {
    // Class without a 'print' method

    // DATA
    int d_data;

  public:
    // CREATORS
    explicit
    NoPrint(int data) : d_data(data) {}

    // ACCESSORS
    int get() const { return d_data; }
};

struct NoPrintUtil {
    // Utility class for class 'NoPrint'

    static bsl::ostream& print(bsl::ostream&  stream,
                               const NoPrint& obj,
                               int            level          = 0,
                               int            spacesPerLevel = 4)
    {
        Obj printer(&stream, level, spacesPerLevel);
        printer.printValue(obj.get());
        return stream;
    }
};

void testFunctionAddress(int)
{
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// EXAMPLE 1

class StockTrade {
    // Record representing a stock trade.

    // DATA
    bsl::string d_ticker;
    double      d_price;
    double      d_quantity;

  public:
    // CREATORS
    StockTrade(const bsl::string& ticker,
               double             price,
               double             quantity)
    : d_ticker(ticker)
    , d_price(price)
    , d_quantity(quantity)
    {}

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const
    {
        if (stream.bad()) {
            return stream;                                            // RETURN
        }

        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        printer.printAttribute("ticker",   d_ticker);
        printer.printAttribute("price",    d_price);
        printer.printAttribute("quantity", d_quantity);
        printer.end();

        return stream;
    }
};


// EXAMPLE 2
//
class BlockList {
    // This class implements a low-level memory manager that allocates and
    // manages a sequence of memory blocks.

    // TYPES
    struct Block {
        // This 'struct' overlays the beginning of each managed block of
        // allocated memory, implementing a doubly-linked list of managed
        // blocks, and thereby enabling constant-time deletions from, as
        // well as additions to, the list of blocks.

        Block                                *d_next_p;       // next
                                                              // pointer

        Block                               **d_addrPrevNext; // enable
                                                              // delete

        bsls::AlignmentUtil::MaxAlignedType   d_memory;       // force
                                                              // alignment
    };

    // DATA
    Block            *d_head_p;      // address of first block of memory
                                     // (or 0)

    bslma::Allocator *d_allocator_p; // memory allocator; held, but not
                                     // owned

  public:
    // CREATORS
    explicit
    BlockList(bslma::Allocator *allocator);
        // Create an empty blocklist.

    ~BlockList();
        // Destroy this object and free its memory

    // MANIPULATORS
    void addBlock();
        // Add a block to the list

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Print out this object.
};

// CREATORS
BlockList::BlockList(bslma::Allocator *allocator)
: d_head_p(0)
, d_allocator_p(allocator)
{}

BlockList::~BlockList()
{
    for (Block *p = d_head_p; p; ) {
        Block *condemned = p;
        p = p->d_next_p;
        d_allocator_p->deallocate(condemned);
    }
}

// MANIPULATORS
void BlockList::addBlock()
{
    Block *pNew = (Block *) d_allocator_p->allocate(sizeof(Block) + 1000);
    pNew->d_addrPrevNext = &d_head_p;
    pNew->d_next_p = d_head_p;
    if (d_head_p) {
        d_head_p->d_addrPrevNext = &pNew->d_next_p;
    }
    d_head_p = pNew;
}

// ACCESSORS
bsl::ostream& BlockList::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (Block *it = d_head_p; it; it = it->d_next_p) {
        printer.printHexAddr(it, 0);
    }
    printer.end();

    return stream;
}

struct ThirdPartyStruct {
    // Suppose this struct is defined somewhere in /usr/include/foo.h, we have
    // no control over it and hence cannot add a .print method to it.

    enum { PRIVATE  = 1,
           WRITABLE = 2 };

    short pid;
    short access_flags;
    char user_id[20];
};

// in your own code

struct MyThirdPartyStructPrintUtil {
    static
    bsl::ostream& print(bsl::ostream&           stream,
                        const ThirdPartyStruct& data,
                        int                     level = 0,
                        int                      spacesPerLevel = 4);
        // You write this function in your own code to accommodate
        // 'ThirdPartyStruct'.
};

bsl::ostream& MyThirdPartyStructPrintUtil::print(
                                        bsl::ostream&           stream,
                                        const ThirdPartyStruct& data,
                                        int                     level,
                                        int                     spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("pid",          data.pid);
    printer.printAttribute("access_flags", data.access_flags);
    printer.printAttribute("user_id",      data.user_id);
    printer.end();

    return stream;
}

class Customer {
    // DATA
    bsl::string      d_companyName;
    ThirdPartyStruct d_thirdPartyStruct;
    bool             d_loyalCustomer;

  public:
    // CREATORS
    Customer() {}

    Customer(const bsl::string& companyName,
             short              pid,
             short              accessFlags,
             const bsl::string& userId,
             bool               loyalCustomer)
    : d_companyName(companyName)
    , d_loyalCustomer(loyalCustomer)
    {
        d_thirdPartyStruct.pid = pid;
        d_thirdPartyStruct.access_flags = accessFlags;
        bsl::strcpy(d_thirdPartyStruct.user_id, userId.c_str());
    }

    // ACCESSORS
    void print(bsl::ostream& stream,
               int           level = 0,
               int           spacesPerLevel = 4) const
    {
        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        printer.printAttribute("CompanyName", d_companyName);
        printer.printForeign(d_thirdPartyStruct,
                             &MyThirdPartyStructPrintUtil::print,
                             "ThirdPartyStruct");
        printer.printAttribute("LoyalCustomer", d_loyalCustomer);
        printer.end();
    }
};


//  EXAMPLE 3
//
class ThirdPartyClass {
    // ...

  public:
    // ACCESSORS
    int getAttribute1() const { return 0; }
    int getAttribute2() const { return 0; }
};

struct ThirdPartyClassUtil {
    static bsl::ostream& print(bsl::ostream&          stream,
                               const ThirdPartyClass& obj,
                               int                    level          = 0,
                               int                    spacesPerLevel = 4);
};

bsl::ostream&
ThirdPartyClassUtil::print(bsl::ostream&          stream,
                           const ThirdPartyClass& obj,
                           int                    level,
                           int                    spacesPerLevel)
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("Attribute2", obj.getAttribute1());
    printer.printAttribute("Attribute2", obj.getAttribute2());
    printer.end();

    return stream;
}

class MyClass {
    // ...
    // DATA
    ThirdPartyClass d_attributeA;
    int             d_attributeB;

  public:
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
    // ...
};

bsl::ostream& MyClass::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printForeign(d_attributeA,
                         &ThirdPartyClassUtil::print,
                         "AttributeA");
    printer.printAttribute("AttributeB", d_attributeB);
    printer.end();

    return stream;
}

// EXAMPLE 4
//
class DateTz {
    // This 'class' represents a date value explicitly in a local time
    // zone.  The offset of that time (in minutes) from UTC is also part of
    // the value of this class.

  private:
    // DATA
    int d_localDate;  // date in YYYYMMDD format, local to the timezone
                      // indicated by 'd_offset'

    int d_offset;     // offset from UTC (in minutes)

  public:
    // ...
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
    // ...
};

bsl::ostream& DateTz::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
   if (stream.bad()) {
       return stream;                                                 // RETURN
   }

   bsl::ostringstream tmp;
   tmp << d_localDate;

   const char sign    = d_offset < 0 ? '-' : '+';
   const int  minutes = '-' == sign ? -d_offset : d_offset;
   const int  hours   = minutes / 60;

   // space usage: +-  hh  mm  nil
   const int SIZE = 1 + 2 + 2 + 1;
   char buf[SIZE];

   // Use at most 2 digits for 'hours'
   if (hours < 100) {
       bsl::sprintf(buf, "%c%02d%02d", sign, hours, minutes % 60);
   }
   else {
       bsl::sprintf(buf, "%cXX%02d", sign, minutes % 60);
   }

   tmp << buf;

   bslim::Printer printer(&stream, level, spacesPerLevel);
   printer.start(true);
   stream << tmp.str();
   printer.end(true);

   return stream << bsl::flush;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;
    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 4
        //
        // Concerns:
        //   Demonstrate to the user 3 things
        //:   o Printing a fixed-length array of objects
        //:   o Printing a pointer to a type
        //:   o Printing a range of objects
        //:   o Repeat printing an STL object
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUsage Example 4\n"
                               "===============\n";

        typedef bsl::set<int> Set;

        Set s0, s1, s2;

        s0.insert(0);
        s0.insert(1);
        s0.insert(2);

        s1.insert(4);
        s1.insert(5);

        s2.insert(8);
        const Set *setArray[] = { &s0, &s1, &s2 };
        const int NUM_SET_ARRAY = sizeof setArray / sizeof *setArray;

        if (verbose) Q(Array as object);
        {
            bsl::ostringstream oss;
            bslim::Printer printer(&oss, 0, -1);
            printer.printValue(setArray + 0, setArray + NUM_SET_ARRAY);
            if (verbose) cout << oss.str() << endl;
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 3
        //
        // Concerns:
        //   Demonstrate printing an STL container, and printing a
        //   foreign object that we can't tool with a .print function.
        //
        // Plan:
        //   Create a 'Customer' type that contains a type 'ThirdPartyStruct'
        //   that does not cooperate with being printed.  Tool 'Customer'
        //   to print 'ThirdPartyStruct', and then put some 'Customer' objects
        //   into a map, and print the map.
        //
        // Testing:
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUsage Example 3\n"
                               "===============\n";

        bsl::map<int, Customer> myMap;
        myMap[7] = Customer("Honeywell",
                            27,
                            ThirdPartyStruct::PRIVATE,
                            "hw",
                            true);
        myMap[5] = Customer("IBM",
                            32,
                            ThirdPartyStruct::WRITABLE,
                            "ibm",
                            false);
        myMap[8] = Customer("Burroughs",
                            45,
                            0,
                            "burr",
                            true);

        bsl::ostringstream oss;
        bslim::Printer printer(&oss, 0, 4);
        printer.start();
        printer.printValue(myMap);
        printer.end();

        const char *EXP = "[\n"
                          "    [\n"
                          "        [\n"
                          "            5\n"
                          "            [\n"
                          "                CompanyName = \"IBM\"\n"
                          "                ThirdPartyStruct = [\n"
                          "                    pid = 32\n"
                          "                    access_flags = 2\n"
                          "                    user_id = \"ibm\"\n"
                          "                ]\n"
                          "                LoyalCustomer = false\n"
                          "            ]\n"
                          "        ]\n"
                          "        [\n"
                          "            7\n"
                          "            [\n"
                          "                CompanyName = \"Honeywell\"\n"
                          "                ThirdPartyStruct = [\n"
                          "                    pid = 27\n"
                          "                    access_flags = 1\n"
                          "                    user_id = \"hw\"\n"
                          "                ]\n"
                          "                LoyalCustomer = true\n"
                          "            ]\n"
                          "        ]\n"
                          "        [\n"
                          "            8\n"
                          "            [\n"
                          "                CompanyName = \"Burroughs\"\n"
                          "                ThirdPartyStruct = [\n"
                          "                    pid = 45\n"
                          "                    access_flags = 0\n"
                          "                    user_id = \"burr\"\n"
                          "                ]\n"
                          "                LoyalCustomer = true\n"
                          "            ]\n"
                          "        ]\n"
                          "    ]\n"
                          "]\n";

        LOOP2_ASSERT(EXP, oss.str(), EXP == oss.str());
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns: That it compiles and runs
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUsage Example 2\n"
                               "===============\n";

        bslma::TestAllocator ta;
        BlockList bl(&ta);
        bl.addBlock();
        bl.addBlock();

        bsl::ostringstream out;

        // It's kind of hard to compare output, since we can't predict ptr
        // values, or even how many hex digits will be in the printed value.

        {
            bl.print(out, 0, -1);
            const bsl::string& outStr = out.str();
            const char *result = outStr.c_str();

            if (verbose) P(result);

            char buf[1000];
            char *to = buf;
            const char *from = result;
            bool exclude = false;
            for (; *from; ++from) {
                if (bsl::isspace(*from)) exclude = false;
                if (!exclude) *to++ = *from;
                if ('x' == *from) exclude = true;
            }
            *to = 0;

            const char *EXP = "[ 0x 0x ]";

            LOOP2_ASSERT(out.str(), buf, !bsl::strcmp(EXP, buf));
        }

        out.str("");

        {
            bl.print(out, 0, 4);
            const bsl::string& outStr = out.str();
            const char *result = outStr.c_str();

            if (verbose) P(result);

            char buf[1000];
            char *to = buf;
            const char *from = result;
            bool exclude = false;
            for (; *from; ++from) {
                if (bsl::isspace(*from)) exclude = false;
                if (!exclude) *to++ = *from;
                if ('x' == *from) exclude = true;
            }
            *to = 0;

            const char *EXP = "[\n"
                              "    0x\n"
                              "    0x\n"
                              "]\n";

            LOOP2_ASSERT(out.str(), buf, !bsl::strcmp(EXP, buf));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns: That it compiles and runs
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Usage Example 1" << endl
                                  << "===============" << endl;

        bsl::ostringstream out;

        StockTrade st("IBM", 107.3, 200);

        {
            st.print(out, 0, -1);
            const char *EXP =
                           "[ ticker = \"IBM\" price = 107.3 quantity = 200 ]";
            LOOP_ASSERT(out.str(), EXP == out.str());
        }

        out.str("");

        {
            st.print(out, 0, 4);
            const char *EXP = "[\n"
                              "    ticker = \"IBM\"\n"
                              "    price = 107.3\n"
                              "    quantity = 200\n"
                              "]\n";
            LOOP_ASSERT(out.str(), EXP == out.str());
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'printValue' ALL STL SEQUENCE AND ASSOCIATIVE CONTAINERS
        //
        // Concern:
        //   Though 'bslim' has no awareness of STL types, it knows about pairs
        //   and can print ranges, which should enable it to print these types.
        //   Verify that this is the case.
        //
        // Plan:
        //   Create and populate various 'bsl' objects, print using range
        //   'printValue', and verify that the string printed out is what is
        //   expected.
        // --------------------------------------------------------------------

        if (verbose) cout << endl <<
                  "'printValue' ALL STL SEQUENCE AND ASSOCIATIVE CONTAINERS\n"
                  "========================================================\n";

        struct S {
            int d_key;
            int d_value;
        };

        S uniqData[] = { { -3, 10 }, {  2,  1 }, {  7,  5 }, {  5,  5 },
                         {  9,  5 }, {  3,  7 }, { 22, 17 }, {  1,  1 } };
        S redundantData[] =
                       { { -3, 10 }, {  2,  1 }, {  7,  5 }, {  5,  5 },
                         { -3,  5 }, {  2,  7 }, {  2, 17 }, {  5,  1 } };
        enum { NUM_DATA = sizeof(uniqData) / sizeof(*uniqData) };

        int uniqKeys[     NUM_DATA];
        int redundantKeys[NUM_DATA];
        for (int i = 0; i < NUM_DATA; ++i) {
            uniqKeys[i]      = uniqData[i].d_key;
            redundantKeys[i] = redundantData[i].d_key;
        }

        // STL sequence & associative containers, in the order in which they
        // are listed in http://www.sgi.com/tech/stl/table_of_contents.html.

        {
            bsl::vector<int> v(&uniqKeys[0], uniqKeys + NUM_DATA);
            const bsl::vector<int>& V = v;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("vector", V);

            const char *EXP = "      vector = [\n"
                              "        -3\n"
                              "        2\n"
                              "        7\n"
                              "        5\n"
                              "        9\n"
                              "        3\n"
                              "        22\n"
                              "        1\n"
                              "      ]\n";
            LOOP2_ASSERT(EXP, out.str(), EXP == out.str());
        }

        {
            bsl::deque<int> d(&uniqKeys[0], uniqKeys + NUM_DATA);
            const bsl::deque<int>& D = d;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("deque", D);

            const char *EXP = "      deque = [\n"
                              "        -3\n"
                              "        2\n"
                              "        7\n"
                              "        5\n"
                              "        9\n"
                              "        3\n"
                              "        22\n"
                              "        1\n"
                              "      ]\n";
            LOOP2_ASSERT(EXP, out.str(), EXP == out.str());
        }

        {
            bsl::list<int> l(&uniqKeys[0], uniqKeys + NUM_DATA);
            const bsl::list<int>& L = l;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("list", L);

            const char *EXP = "      list = [\n"
                              "        -3\n"
                              "        2\n"
                              "        7\n"
                              "        5\n"
                              "        9\n"
                              "        3\n"
                              "        22\n"
                              "        1\n"
                              "      ]\n";
            LOOP2_ASSERT(EXP, out.str(), EXP == out.str());
        }


        // We don't seem to have bit_vector

        {
            bsl::set<int> s(&uniqKeys[0], uniqKeys + NUM_DATA);
            const bsl::set<int>& S = s;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("set", S);

            const char *EXP = "      set = [\n"
                              "        -3\n"
                              "        1\n"
                              "        2\n"
                              "        3\n"
                              "        5\n"
                              "        7\n"
                              "        9\n"
                              "        22\n"
                              "      ]\n";
            LOOP2_ASSERT(EXP, out.str(), EXP == out.str());
        }

        {
            bsl::map<int, int> m;    const bsl::map<int, int>& M = m;
            for (int i = 0; i < NUM_DATA; ++i) {
                const S& s = uniqData[i];
                m[s.d_key] = s.d_value;
            }
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("map", M);

            const char *EXP = "      map = [\n"
                              "        [\n"
                              "          -3\n"
                              "          10\n"
                              "        ]\n"
                              "        [\n"
                              "          1\n"
                              "          1\n"
                              "        ]\n"
                              "        [\n"
                              "          2\n"
                              "          1\n"
                              "        ]\n"
                              "        [\n"
                              "          3\n"
                              "          7\n"
                              "        ]\n"
                              "        [\n"
                              "          5\n"
                              "          5\n"
                              "        ]\n"
                              "        [\n"
                              "          7\n"
                              "          5\n"
                              "        ]\n"
                              "        [\n"
                              "          9\n"
                              "          5\n"
                              "        ]\n"
                              "        [\n"
                              "          22\n"
                              "          17\n"
                              "        ]\n"
                              "      ]\n";
            LOOP2_ASSERT(EXP, out.str(), EXP == out.str());
        }

        {
            bsl::multiset<int> ms(&redundantKeys[0], redundantKeys + NUM_DATA);
            const bsl::multiset<int>& MS = ms;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("multiset", MS);

            const char *EXP = "      multiset = [\n"
                              "        -3\n"
                              "        -3\n"
                              "        2\n"
                              "        2\n"
                              "        2\n"
                              "        5\n"
                              "        5\n"
                              "        7\n"
                              "      ]\n";
            LOOP2_ASSERT(EXP, out.str(), EXP == out.str());
        }

        {
            bsl::multimap<int, int> mm;
            const bsl::multimap<int, int>& MM = mm;
            for (int i = 0; i < NUM_DATA; ++i) {
                const S& s = redundantData[i];
                mm.insert(std::pair<int, int>(s.d_key, s.d_value));
            }
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("multimap", MM);

            // Verifying the result in a platform-independent way is difficult
            // because the order it which elements with the same key occur
            // might not be defined.

            const bsl::string& result = out.str();

            {
                const char *match = "      multimap = [\n";

                // find returning 0 means 'found at the beginning'

                ASSERT(0 == result.find(match));
            }
            for (int i = 0; i < NUM_DATA; ++i) {
                const S& s = redundantData[i];
                bsl::ostringstream matchss;
                matchss << "\n        [\n"
                             "          " << s.d_key << "\n"
                             "          " << s.d_value << "\n"
                             "        ]\n";
                ASSERT(bsl::string::npos != result.find(matchss.str()));
            }

            {
                int newlines = 0;
                for (const char *pc = result.c_str(); *pc; ++pc) {
                    if ('\n' == *pc) {
                        ++newlines;
                    }
                }
                ASSERT(4 * NUM_DATA + 2 == newlines);
            }
        }

        {
            bsl::unordered_set<int> s(&uniqKeys[0], uniqKeys + NUM_DATA);
            const bsl::unordered_set<int>& S = s;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("unordered_set", S);

            bsl::ostringstream EXP;
            {
                EXP << "      unordered_set = [\n";
                bsl::unordered_set<int>::const_iterator iter = S.begin();
                while (iter != S.end()) {
                    EXP << "        " << *iter << '\n';
                    ++iter;
                }
                EXP << "      ]\n";
            }

            LOOP2_ASSERT(EXP.str(), out.str(), EXP.str() == out.str());
        }

        {
            bsl::unordered_map<int, int> m;
            const bsl::unordered_map<int, int>& M = m;
            for (int i = 0; i < NUM_DATA; ++i) {
                const S& s = uniqData[i];
                m[s.d_key] = s.d_value;
            }
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("unordered_map", M);

            bsl::ostringstream EXP;
            {
                EXP << "      unordered_map = [\n";
                bsl::unordered_map<int, int>::const_iterator iter = M.begin();
                while (iter != M.end()) {
                    EXP << "        [\n";
                    EXP << "          " << iter->first << '\n';
                    EXP << "          " << iter->second << '\n';
                    EXP << "        ]\n";
                    ++iter;
                }
                EXP << "      ]\n";
            }

            LOOP2_ASSERT(EXP.str(), out.str(), EXP.str() == out.str());
        }

        {
            bsl::unordered_multiset<int> ms(&redundantKeys[0],
                                            redundantKeys + NUM_DATA);
            const bsl::unordered_multiset<int>& MS = ms;
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("unordered_multiset", MS);

            bsl::ostringstream EXP;
            {
                EXP << "      unordered_multiset = [\n";
                bsl::unordered_multiset<int>::const_iterator iter = MS.begin();
                while (iter != MS.end()) {
                    EXP << "        " << *iter << '\n';
                    ++iter;
                }
                EXP << "      ]\n";
            }

            LOOP2_ASSERT(EXP.str(), out.str(), EXP.str() == out.str());
        }

        {
            bsl::unordered_multimap<int, int> mm;
            const bsl::unordered_multimap<int, int>& MM = mm;
            for (int i = 0; i < NUM_DATA; ++i) {
                const S& s = redundantData[i];
                mm.insert(std::pair<int, int>(s.d_key, s.d_value));
            }
            bsl::ostringstream out;
            bslim::Printer p(&out, 2, 2);
            p.printAttribute("unordered_multimap", MM);

            bsl::ostringstream EXP;
            {
                EXP << "      unordered_multimap = [\n";
                bsl::unordered_multimap<int, int>::const_iterator iter =
                                                                    MM.begin();
                while (iter != MM.end()) {
                    EXP << "        [\n";
                    EXP << "          " << iter->first << '\n';
                    EXP << "          " << iter->second << '\n';
                    EXP << "        ]\n";
                    ++iter;
                }
                EXP << "      ]\n";
            }

            LOOP2_ASSERT(EXP.str(), out.str(), EXP.str() == out.str());
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // 'printAttribute' WITH RANGE
        //
        // Concerns:
        //   Need to exercise 'printAttribute' with a range.
        //
        // Plan:
        //   Create an array and a set, populate both, print them out with
        //   range 'printAttribute', and verify that the output is as
        //   expected.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'printAttribute' with range\n"
                                     "===================================\n";

        int array[] = { 2, 8, 10, 7, 3, 9 };
        enum { NUM_IN_ARRAY = sizeof(array) / sizeof(*array) };

        {
            int array[] = { 2, 8, 10, 7, 3, 9 };
            enum { NUM_IN_ARRAY = sizeof(array) / sizeof(*array) };

            bsl::ostringstream out;
            Obj p(&out, 2, 2);
            p.start();
            p.printAttribute("Array", array + 0, array + NUM_IN_ARRAY);
            p.end();

            const char *EXPECTED = "    [\n"
                                   "      Array = [\n"
                                   "        2\n"
                                   "        8\n"
                                   "        10\n"
                                   "        7\n"
                                   "        3\n"
                                   "        9\n"
                                   "      ]\n"
                                   "    ]\n";

            LOOP2_ASSERT(EXPECTED, out.str(), EXPECTED == out.str());

            if (verbose) {
                bsl::cout << out.str();
            }
        }

        {
            bsl::set<int> mySet;
            for (int i = 0; i < NUM_IN_ARRAY; ++i) {
                mySet.insert(array[i]);
            }

            bsl::ostringstream out;
            Obj p(&out, 2, 2);
            p.start();
            p.printAttribute("mySet", mySet);
            p.end();

            const char *EXPECTED = "    [\n"
                                   "      mySet = [\n"
                                   "        2\n"
                                   "        3\n"
                                   "        7\n"
                                   "        8\n"
                                   "        9\n"
                                   "        10\n"
                                   "      ]\n"
                                   "    ]\n";

            LOOP2_ASSERT(EXPECTED, out.str(), EXPECTED == out.str());

            if (verbose) {
                bsl::cout << out.str();
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTION NEUTRALITY
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING EXCEPTION NEUTRALITY" << endl
                                  << "============================" << endl;

        void *data = reinterpret_cast<void *>(0xdeadbeef);
        (void) data;  // used
        ostringstream out;

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'printForeign'
        //
        // Concerns: That 'printForeign'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //: 3 prints the expected output when 'name' is not supplied.
        //: 4 calls the foreign function with the values of '-(absLevel + 1)'
        //    and 'spacesPerLevel'.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printForeign' when called with a
        //:   'NoPrint' object, and a 'name'.  For each set of values in the
        //:   table, ensure that the actual output of 'printOrNull' is the same
        //:   as the expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printForeign' when called with a
        //:   'NoPrint' object, but 'name' is not supplied.  For each set of
        //:   values in the table, ensure that the actual output of
        //:   'printOrNull' is the same as the expected output.
        //
        // Testing:
        //   template <class TYPE, class PRINT_FUNCTOR>
        //   void printForeign(const TYPE&           data,
        //                     const PRINT_FUNCTOR&  printFunctionObject,
        //                     const char           *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'printForeign'" << endl
                                  << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    0,    0, "data = %d\n"               },
                { L_,    0,    2, "  data =     %d\n"         },
                { L_,    2,    0, "data = %d\n"               },
                { L_,    2,    2, "      data =         %d\n" },
                { L_,    2,   -2, " data =  %d"               },
                { L_,   -2,    2, "      data =         %d\n" },
                { L_,   -2,   -2, " data =  %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 433001; NoPrint data(tData);
                Obj p(&out, LEVEL, SPL);
                p.printForeign(data, &NoPrintUtil::print, "data");

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "              %d\n" },
                { L_,    2,   -2, "  %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 433001; NoPrint data(tData);
                Obj p(&out, LEVEL, SPL);
                p.printForeign(data, &NoPrintUtil::print, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'printHexAddr'
        //
        // Concerns: That 'printHexAddr'
        //: 1 prints the input address in hexadecimal format when 'name' is not
        //:   supplied.
        //: 2 prints the string "NULL" when a null pointer is passed to it.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printHexAddr' when 'name' is not
        //:   supplied.  For each set of values in the table, ensure that the
        //:   actual output of 'printHexAddr' is the same as the expected
        //:   output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printHexAddr' when a null pointer
        //:   is passed to it.  For each set of values in the table, ensure
        //:   that the actual output of 'printHexAddr' is the same as the
        //:   expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printHexAddr(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'printHexAddr'" << endl
                                  << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    3, "         %s\n" },
                { L_,    2,   -3, " %s"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int *data = reinterpret_cast<int *>(0xdeadbeef);
                Obj p(&out, LEVEL, SPL); p.printHexAddr(data, 0);

                ostringstream ptr;
                ptr << hex << showbase
                    << reinterpret_cast<bsls::Types::UintPtr>(data);
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                            ptr.str().c_str());
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      NULL\n" },
                { L_,    2,   -2, " NULL"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                void *data = 0;
                Obj p(&out, LEVEL, SPL); p.printHexAddr(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'printHexAddr' (indentation and name)
        //
        // Concerns: That 'printHexAddr'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printHexAddr' when a 'name' is
        //:   supplied.  For each set of values in the table, ensure that the
        //:   actual output of 'printHexAddr' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printHexAddr(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'printHexAddr' (indentation and name)" << endl
                 << "=============================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    0,    0, "data = %s\n"          },
            { L_,    0,    2, "  data = %s\n"        },
            { L_,    2,    0, "data = %s\n"          },
            { L_,    2,    3, "         data = %s\n" },
            { L_,    2,   -3, " data = %s"           },
            { L_,   -2,    3, "         data = %s\n" },
            { L_,   -2,   -3, " data = %s"           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int *data = reinterpret_cast<int *>(0xdeadbeef);
            Obj p(&out, LEVEL, SPL); p.printHexAddr(data, "data");

            ostringstream ptr;
            ptr << hex << showbase
                << reinterpret_cast<bsls::Types::UintPtr>(data);
            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), ptr.str().c_str());
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'printOrNull' (null pointers)
        //
        // Concerns:
        //: 1 That 'printOrNull' prints the string "NULL" when a null pointer
        //:   is passed to it.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   null pointer.  For each set of values in the table, ensure that
        //:   the actual output of 'printOrNull' when called with a null
        //:   pointer of type 'void *', 'const void *', 'const char *',
        //:  'int *' and 'HasPrint *' is the same as the expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printOrNull(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'printOrNull' (null pointers)" << endl
                          << "=====================================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      NULL\n" },
                { L_,    2,   -2, " NULL"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream vOut, cvOut, vvOut, vcvOut, cOut, iOut, uOut;

                void *data = 0;
                Obj pV(&vOut, LEVEL, SPL); pV.printOrNull(data, 0);
                Obj pCV(&cvOut, LEVEL, SPL);
                pCV.printOrNull((const void *)data, 0);
                Obj pC(&cOut, LEVEL, SPL);
                pC.printOrNull((const char *)data, 0);
                Obj pI(&iOut, LEVEL, SPL); pI.printOrNull((int *)data, 0);
                Obj pU(&uOut, LEVEL, SPL); pU.printOrNull((HasPrint *)data, 0);

                const bsl::string& EXPECTED = DATA[i].d_expected;

                // void *
                {
                    const bsl::string& ACTUAL = vOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    const bsl::string& ACTUAL = cvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const char *
                {
                    const bsl::string& ACTUAL = cOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // int *
                {
                    const bsl::string& ACTUAL = iOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // HasPrint *
                {
                    const bsl::string& ACTUAL = uOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: 'printOrNull' (non-null pointer types)
        //
        // Concerns: That 'printOrNull'
        //: 1 prints the address held by the pointer in hexadecimal format, but
        //:   does not attempt to dereference the pointer if the pointer type
        //:   is 'void *', or  'const void *'.
        //: 2 dereferences the pointer and prints a character if the
        //:   pointer type is 'const char *'.
        //: 3 dereferences the pointer and prints the held value if the
        //:   pointer points to a fundamental type.
        //: 4 calls the 'print' method of the held object with the values of
        //:   '-(absLevel + 1)' and 'spacesPerLevel' if the pointer points to a
        //:   user-defined type.
        //: 5 prints a newline character after printing the data if
        //:   'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'void *', or 'const void *'.  For each set of values in the
        //:   table, ensure that the actual output of 'printOrNull' is the
        //:   same as the expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'const char *'.  For each set of values in the table, ensure that
        //:   the actual output of 'printOrNull' is the same as the expected
        //:   output.
        //: 3 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'int *'.  For each set of values in the table, ensure that the
        //:   actual output of 'printOrNull' is the same as the expected
        //:   output.
        //: 4 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'HasPrint *'.  For each set of values in the table, ensure that
        //:   the actual output of 'printOrNull' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printOrNull(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'printOrNull' (non-null pointer types)" << endl
                 << "==============================================" << endl;

        {
            if (verbose) cout << "void *" << endl
                              << "------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %s\n" },
                { L_,    2,   -2, " %s"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out, cOut, vOut, vcOut;
                void *data = reinterpret_cast<void *> (0xdeadbeef);
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);
                Obj pC(&cOut, LEVEL, SPL);
                pC.printOrNull((const void *)data, 0);

                ostringstream ptr;
                ptr << hex << showbase
                    << reinterpret_cast<bsls::Types::UintPtr>(data);
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                            ptr.str().c_str());
                const bsl::string EXPECTED(buf);

                // void *
                {
                    const bsl::string& ACTUAL = out.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    const bsl::string& ACTUAL = cOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
            }
        }
        {
            if (verbose) cout << "const char *" << endl
                              << "------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      '%c'\n" },
                { L_,    2,   -2, " '%c'"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                const char *data = "testing char *";
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), *data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "int *" << endl
                              << "-----" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %d\n" },
                { L_,    2,   -2, " %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int testData = 88932; int *data = &testData;
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), *data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "HasPrint *" << endl
                              << "----------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "              HasPrint:data = %d\n" },
                { L_,    2,   -2, "  HasPrint:data = %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 73326; HasPrint tClass(tData);
                HasPrint *data = &tClass;
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);

                int LEVEL_EXP = -(p.absLevel() + 1);
                int LEVEL_ACT = data->d_level;
                int SPL_EXP = SPL; int SPL_ACT = data->d_spacesPerLevel;

                LOOP2_ASSERT(LEVEL_EXP, LEVEL_ACT, LEVEL_EXP == LEVEL_ACT);
                LOOP2_ASSERT(SPL_EXP, SPL_ACT, SPL_EXP == SPL_ACT);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'printOrNull' (indentation and name)
        //
        // Concerns: That 'printOrNull'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when a 'name' is
        //:   supplied.  For each set of values in the table, ensure that the
        //:   actual output of 'printOrNull' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printOrNull(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'printOrNull' (indentation and name)" << endl
                 << "============================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    0,    0, "data = %d\n"          },
            { L_,    0,    2, "  data = %d\n"        },
            { L_,    2,    0, "data = %d\n"          },
            { L_,    2,    3, "         data = %d\n" },
            { L_,    2,   -3, " data = %d"           },
            { L_,   -2,    3, "         data = %d\n" },
            { L_,   -2,   -3, " data = %d"           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int tData = 448992; int *data = &tData;
            Obj p(&out, LEVEL, SPL); p.printOrNull(data, "data");

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), *data);
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'printAttribute' and 'printValue'
        //
        // Concerns:
        //: 1 That 'printAttribute' and 'printValue' call 'print' correctly.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output when 'printAttribute' and
        //:   'printValue' are called.  For each set of values in the table,
        //:   ensure that the actual outputs are the same as the expected
        //:   outputs.
        //
        // Testing:
        //   template<class TYPE>
        //   void printAttribute(const char *name, const TYPE& data) const;
        //   template<class TYPE>
        //   void printValue(const TYPE& data) const;
        // --------------------------------------------------------------------
        if (verbose)
        cout << endl
             << "TESTING 'printAttribute' and 'printValue'" << endl
             << "=========================================" << endl;
        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- ---------------
            { L_,    2,    3, "         data = %d\n         %d\n" },
            { L_,    2,   -3, " data = %d %d"                     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int data = 448992;
            Obj p(&out, LEVEL, SPL);
            p.printAttribute("data", data); p.printValue(data);

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), data, data);
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 6: {
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'end'
        //
        // Concerns: 'end' prints the correct indentation for different values
        // of 'level' and 'spacesPerLevel' and ']' if 'suppressBracket' is
        // 'false', and does not print the indentation and ']' otherwise.
        // 'end' should also print a newline if 'spacesPerLevel' >= 0.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, suppress bracket flag, and expected output of 'end'.  For
        //:   each set of values in the table, ensure that the actual output of
        //:   'end' is the same as the expected output.
        //
        // Testing:
        //   void end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'end'" << endl
                                  << "=============" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_level;           // indentation level
            int         d_spacesPerLevel;  // spaces per indentation level
            bool        d_suppressBracket; // suppress bracket flag
            bsl::string d_expected;        // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL SUPPRESS BRACKET EXPECTED OUTPUT
            //----  ----- --- ---------------- ---------------
            { L_,    0,    0, false,           "]\n"          },
            { L_,    0,    2, false,           "]\n"          },
            { L_,    2,    0, false,           "]\n"          },
            { L_,    2,    3, false,           "      ]\n"    },
            { L_,    2,   -3, false,           " ]"           },
            { L_,   -2,    3, false,           "      ]\n"    },
            { L_,   -2,   -3, false,           " ]"           },
            { L_,    0,    0, true ,           "\n"           },
            { L_,    0,    2, true ,           "\n"           },
            { L_,    2,    0, true ,           "\n"           },
            { L_,    2,    3, true ,           "\n"           },
            { L_,    2,   -3, true ,           ""             },
            { L_,   -2,    3, true ,           "\n"           },
            { L_,   -2,   -3, true ,           ""             },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;
            bool suppressBracket = DATA[i].d_suppressBracket;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            Obj p(&out, LEVEL, SPL); p.end(suppressBracket);

            const bsl::string& EXPECTED = DATA[i].d_expected;
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'start'
        //
        // Concerns: 'start' prints the correct indentation for different
        // values of 'level' and 'spacesPerLevel', and then prints '[' if
        // 'suppressBracket' is 'false', and does not print '[' otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, suppress bracket flag, and expected output of 'start'.
        //:   For each set of values in the table, ensure that the actual
        //:   output of 'start' is the same as the expected output.
        //
        // Testing:
        //   void start() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'start'" << endl
                                  << "===============" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_level;           // indentation level
            int         d_spacesPerLevel;  // spaces per indentation level
            bool        d_suppressBracket; // suppress bracket flag
            bsl::string d_expected;        // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL SUPPRESS BRACKET EXPECTED OUTPUT
            //----  ----- --- ---------------- ---------------
            { L_,    0,    0, false,           "[\n"          },
            { L_,    0,    2, false,           "[\n"          },
            { L_,    2,    0, false,           "[\n"          },
            { L_,    2,    3, false,           "      [\n"    },
            { L_,    2,   -3, false,           "      ["      },
            { L_,   -2,    3, false,           "[\n"          },
            { L_,   -2,   -3, false,           "["            },
            { L_,    0,    0, true ,           ""             },
            { L_,    0,    2, true ,           ""             },
            { L_,    2,    0, true ,           ""             },
            { L_,    2,    3, true ,           "      "       },
            { L_,    2,   -3, true ,           "      "       },
            { L_,   -2,    3, true ,           ""             },
            { L_,   -2,   -3, true ,           ""             }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;
            bool suppressBracket = DATA[i].d_suppressBracket;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            Obj p(&out, LEVEL, SPL); p.start(suppressBracket);

            const bsl::string& EXPECTED = DATA[i].d_expected;
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 'HasPrint::print' prints the expected output for different values
        //:    of level and spaces per level.
        //: 2 'NoPrintUtil::print' prints the expected output for different
        //:   values of level and spaces per level.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'HasPrint::print'.  For each set of
        //:   values in the table, ensure that the actual output of
        //:   'HasPrint::print' is the same as the expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'NoPrintUtil::print'.  For each set
        //:   of values in the table,  ensure that the actual output of
        //:   'NoPrintUtil::print' is the same as the expected output.
        //
        // Testing:
        //   bsl::ostream& HasPrint::print(bsl::ostream&, int, int) const;
        //   bsl::ostream& NoPrintUtil::print
        //                           (bsl::ostream&, const NoPrint&, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING TEST APPARATUS" << endl
                                  << "======================" << endl;

        {
            if (verbose) cout << "HasPrint::print" << endl
                              << "---------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      HasPrint:data = %d\n" },
                { L_,    2,   -2, " HasPrint:data = %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out; int tData = 73326; HasPrint data(tData);
                data.print(out, LEVEL, SPL);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "NoPrintUtil::print" << endl
                              << "------------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %d\n" },
                { L_,    2,   -2, " %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out; int tData = 73326; NoPrint data(tData);
                NoPrintUtil::print(out, data, LEVEL, SPL);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR AND BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The constructor can create an object having any state required
        //:   for thorough testing.
        //: 2 Accessors return the corresponding state variable of the object.
        //: 3 Accessors are declared 'const'.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, and spaces
        //:   per level.  For each set of values in the table, create a 'const
        //:   Printer' object, and then use the basic accessors to compare the
        //:   values set in the object with the input values.
        //
        // Testing:
        //   Printer(bsl::ostream *stream, int level, int spacesPerLevel);
        //   int absLevel() const;
        //   bool suppressInitialIndentFlag() const;
        //   int spacesPerLevel() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CONSTRUCTOR AND BASIC ACCESSORS" << endl
                          << "=======================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
        } DATA[] = {
            //LINE  LEVEL SPL
            //----  ----- ---
            { L_,    0,    0 },
            { L_,    2,    3 },
            { L_,    2,   -3 },
            { L_,   -2,    3 },
            { L_,   -2,   -3 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE   = DATA[i].d_lineNum;
            int LEVEL        = DATA[i].d_level;
            int SPL_EXP      = DATA[i].d_spacesPerLevel;
            int ABSLVL_EXP   = LEVEL > 0 ? LEVEL:-LEVEL;
            bool SIIFLAG_EXP = LEVEL < 0 ? true:false;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL_EXP) }

            ostringstream out; const Obj p(&out, LEVEL, SPL_EXP);
            int ABSLVL_ACT = p.absLevel();
            int SPL_ACT = p.spacesPerLevel();
            int SIIFLAG_ACT = p.suppressInitialIndentFlag();

            LOOP2_ASSERT(ABSLVL_EXP, ABSLVL_ACT, ABSLVL_EXP == ABSLVL_ACT);
            LOOP2_ASSERT(SPL_EXP, SPL_ACT, SPL_EXP == SPL_ACT);
            LOOP2_ASSERT(SIIFLAG_EXP, SIIFLAG_ACT, SIIFLAG_EXP == SIIFLAG_ACT);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to support comprehensive
        //:   testing.
        //
        // Plan: Create a 'Printer' object, and call its 'print' method with an
        // integer.  Ensure that the actual output is the same as the expected
        // output.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        ostringstream out;
        int data = 4892;
        Obj p(&out, 0, 4); p.printValue(data);

        const bsl::string EXPECTED("    4892\n");
        const bsl::string& ACTUAL = out.str();

        if (veryVerbose) {
            cout << "\tEXPECTED:\n" << "\t" << EXPECTED << endl
                 << "\tACTUAL:\n" << "\t" << ACTUAL << endl;
        }
        LOOP2_ASSERT(EXPECTED, ACTUAL, EXPECTED == ACTUAL);

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
