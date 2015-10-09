// bslim_printer.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLIM_PRINTER
#define INCLUDED_BSLIM_PRINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to implement standard 'print' methods.
//
//@CLASSES:
//  bslim::Printer: mechanism to implement standard 'print' methods
//
//@DESCRIPTION: This component provides a mechanism class, 'bslim::Printer',
// that, in many cases, simplifies the implementation of types providing a
// 'print' method with the signature:
//..
//  bsl::ostream& print(bsl::ostream& stream,
//                      int           level          = 0,
//                      int           spacesPerLevel = 4) const;
//      // Format this object to the specified output 'stream' at the (absolute
//      // value of) the optionally specified indentation 'level' and return a
//      // reference to 'stream'.  If 'level' is specified, optionally specify
//      // 'spacesPerLevel', the number of spaces per indentation level for
//      // this and all of its nested objects.  If 'level' is negative,
//      // suppress indentation of the first line.  If 'spacesPerLevel' is
//      // negative format the entire output on one line, suppressing all but
//      // the initial indentation (as governed by 'level').  If 'stream' is
//      // not valid on entry, this operation has no effect.
//..
// Note that all value-semantic types are expected to provide this method.  Use
// of the 'Printer' mechanism provides a uniform style of output formatting:
//
//: o Enclose the object's salient attributes with square brackets.
//:
//: o Prefix each attribute with the attribute's name, separated by an "equal"
//:   sign surrounded by space characters (" = ").
//:
//: o If the attributes are to be printed on multiple lines, then print them
//:   with one more level of indentation than that of the enclosing brackets.
//:   If any of the attributes are compositions, then the composite values
//:   must be printed with an additional level of indentation.
//:
//: o If the attributes are to be printed on a single line, then separate each
//:   value with a single space character.
//:
//: o For small, common types, such as 'bdet_Date', the names of attributes,
//:   equal sign, and brackets may be omitted, with the entire value
//:   represented on a single line in a custom format.  For example, the
//:   'bdet_Date::print' method emits the date value in the format: 01JAN2001.
//
// For example, consider a class having two attributes, "ticker", represented
// by a 'bsl::string', and "price", represented by a 'double'.  The output for
// a 'print' method that produces standardized output for
// 'print(bsl::cout, 0, -4)' (single-line output) is shown below:
//..
//  [ ticker = "ABC" price = 65.89 ]
//..
// Output for 'print(bsl::cout, 0, 4)' (multi-line output) is shown below:
//..
//  [
//      ticker = "ABC"
//      price = 65.89
//  ]
//..
// The 'Printer' mechanism provides methods and method templates to format data
// as described above.  'Printer' objects are instantiated with the target
// stream to be written to, and the values of the indentation level of the
// data, 'level', and the spaces per level, 'spacesPerLevel'.  The methods
// provided by 'Printer', 'printAttribute', 'printValue', 'printOrNull',
// 'printHexAddr' and 'printForeign', use these values for formatting.  The
// 'start' and 'end' methods print the enclosing brackets of the output.  In
// order to generate the standard output format, 'start' should be called
// before any of the other methods, and 'end' should be called after all the
// other methods have been called.
//
///Usage
///-----
// In the following examples, we examine the implementation of the 'print'
// method of different types of classes using 'Printer'.
//
///Example 1: 'print' Method for a Value-Semantic Class
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use 'Printer' to implement the
// standard 'print' function of a value-semantic class having multiple
// attributes.  Suppose we have a class, 'RecordAttributes', that provides a
// container for a fixed set of attributes.  A 'RecordAttributes' object has
// two attributes, "timestamp", of type 'my::Datetime', and "processID", of
// type 'int':
//..
//  class StockTrade {
//      // This class represents the properties of a stock trace.
//
//      // DATA
//      bsl::string d_ticker;        // ticker symbol
//      double      d_price;         // stock price
//      double      d_quantity;      // quanity traded
//
//    public:
//      ...
//
//      // ACCESSORS
//      bsl::ostream& print(bsl::ostream& stream,
//                          int           level = 0,
//                          int           spacesPerLevel = 4) const
//      {
//          if (stream.bad()) {
//              return stream;                                        // RETURN
//          }
//
//          bslim::Printer printer(&stream, level, spacesPerLevel);
//          printer.start();
//          printer.printAttribute("ticker",   d_ticker);
//          printer.printAttribute("price",    d_price);
//          printer.printAttribute("quantity", d_quantity);
//          printer.end();
//
//          return stream;
//      }
//  };
//..
// Sample output for 'StockTrade::print(bsl::cout, 0, -4)':
//..
//  [ ticker = "IBM" price = 107.3 quantity = 200 ]
//..
// Sample output for 'StockTrade::print(bsl::cout, 0, 4)':
//..
//  [
//      ticker = "IBM"
//      price = 107.3
//      quantity = 200
//  ]
//..
//
///Example 2: 'print' Method for a Mechanism Class
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we discuss the implementation of 'print' for a mechanism
// class.  A mechanism class does not have any salient attributes that define
// its value (as a mechanism does not have a "value").  However, the 'print'
// method may be implemented to output the internal state of an object of such
// a type, e.g., for debugging purposes.
//
// For example, consider a memory manager class, 'BlockList', that maintains a
// linked list of memory blocks:
//..
//  class BlockList {
//      // This class implements a low-level memory manager that allocates and
//      // manages a sequence of memory blocks.
//
//      // TYPES
//      struct Block {
//          // This 'struct' overlays the beginning of each managed block of
//          // allocated memory, implementing a doubly-linked list of managed
//          // blocks, and thereby enabling constant-time deletions from, as
//          // well as additions to, the list of blocks.
//
//          Block                               *d_next_p;       // next
//                                                               // pointer
//
//          Block                              **d_addrPrevNext; // enable
//                                                               // delete
//
//          bsls::AlignmentUtil::MaxAlignedType   d_memory;      // force
//                                                               // alignment
//      };
//
//      // DATA
//      Block           *d_head_p;      // address of first block of memory
//                                      // (or 0)
//
//      bslma::Allocator *d_allocator_p; // memory allocator; held, but not
//                                       // owned
//
//    public:
//      // ...
//      // ACCESSORS
//      // ...
//      bsl::ostream& print(bsl::ostream& stream,
//                          int           level          = 0,
//                          int           spacesPerLevel = 4) const;
//  };
//..
// For the purposes of debugging, it may be useful to print the starting
// address of every memory block in a 'BlockList', which can be done using the
// 'printHexAddr' method of the 'Printer' class:
//..
//  bsl::ostream& BlockList::print(bsl::ostream& stream,
//                                 int           level,
//                                 int           spacesPerLevel) const
//  {
//      if (stream.bad()) {
//          return stream;                                            // RETURN
//      }
//
//      bslim::Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//      for (Block *it = d_head_p; it; it = it->d_next_p) {
//          printer.printHexAddr(it, 0);
//      }
//      printer.end();
//
//      return stream;
//  }
//..
// Sample output for 'BlockList::print(bsl::cout, 0, -4):
//..
//  [ 0x0012fab4 0x0012fab8 ]
//..
// Sample output for 'BlockList::print(bsl::cout, 0, 4):
//..
//  [
//      0x0012fab4
//      0x0012fab8
//  ]
//..
//
///Example 3: Foreign (Third-Party) Classes, and Printing STL Containers
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we use a 'Printer' object to help format the properties of
// a class supplied by a third-party that does not implement the standard
// 'print' method.  Consider a struct, 'ThirdPartyStruct', defined in
// '/usr/include/thirdparty.h' that has no standard 'print' method.  We will be
// using this struct within another class 'Customer', storing some 'Customer'
// objects in a map, and printing the map.
//..
//  struct ThirdPartyStruct {
//      // Suppose this struct is defined somewhere in
//      // '/usr/include/thirdparty.h', we have no control over it and hence
//      // cannot add a .print method to it.
//
//      enum { PRIVATE  = 1,
//             WRITABLE = 2 };
//
//      short pid;              // process id
//      short access_flags;     // options
//      char user_id[20];       // userid
//  };
//..
// We create a struct 'MyThirdPartyStructPrintUtil':
//..
//  struct MyThirdPartyStructPrintUtil {
//      static
//      bsl::ostream& print(bsl::ostream&           stream,
//                          const ThirdPartyStruct& data,
//                          int                     level = 0,
//                          int                      spacesPerLevel = 4);
//          // You write this function in your own code to accommodate
//          // 'ThirdPartyStruct'.
//  };
//
//  bsl::ostream& MyThirdPartyStructPrintUtil::print(
//                                      bsl::ostream&           stream,
//                                      const ThirdPartyStruct& data,
//                                      int                     level,
//                                      int                     spacesPerLevel)
//  {
//      bslim::Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//      printer.printAttribute("pid",          data.pid);
//      printer.printAttribute("access_flags", data.access_flags);
//      printer.printAttribute("user_id",      data.user_id);
//      printer.end();
//
//      return stream;
//  }
//..
//  We create a class 'Customer' that has a 'ThirdPartyStruct' in it:
//..
//  class Customer {
//      // DATA
//      bsl::string      d_companyName;
//      ThirdPartyStruct d_thirdPartyStruct;
//      bool             d_loyalCustomer;
//
//    public:
//      // CREATORS
//      Customer() {}
//
//      Customer(const bsl::string& companyName,
//               short              pid,
//               short              accessFlags,
//               const bsl::string& userId,
//               bool               loyalCustomer)
//      : d_companyName(companyName)
//      , d_loyalCustomer(loyalCustomer)
//      {
//          d_thirdPartyStruct.pid = pid;
//          d_thirdPartyStruct.access_flags = accessFlags;
//          bsl::strcpy(d_thirdPartyStruct.user_id, userId.c_str());
//      }
//
//      // ACCESSORS
//      void print(bsl::ostream& stream,
//                 int           level = 0,
//                 int           spacesPerLevel = 4) const
//      {
//          bslim::Printer printer(&stream, level, spacesPerLevel);
//          printer.start();
//          printer.printAttribute("CompanyName", d_companyName);
//          printer.printForeign(d_thirdPartyStruct,
//                               &MyThirdPartyStructPrintUtil::print,
//                               "ThirdPartyStruct");
//          printer.printAttribute("LoyalCustomer", d_loyalCustomer);
//          printer.end();
//      }
//  };
//..
//  We then create some 'Customer' objects and put them in a map:
//..
//  void myFunc()
//  {
//      bsl::map<int, Customer> myMap;
//      myMap[7] = Customer("Honeywell",
//                          27,
//                          ThirdPartyStruct::PRIVATE,
//                          "hw",
//                          true);
//      myMap[5] = Customer("IBM",
//                          32,
//                          ThirdPartyStruct::WRITABLE,
//                          "ibm",
//                          false);
//      myMap[8] = Customer("Burroughs",
//                          45,
//                          0,
//                          "burr",
//                          true);
//..
//  Now we print the map
//..
//      bslim::Printer printer(&cout, 0, 4);
//      printer.start();
//      printer.printValue(myMap);
//      printer.end();
//  }
//..
// The following is written to 'stdout':
//..
//  [
//      [
//          [
//              5
//              [
//                  CompanyName = "IBM"
//                  ThirdPartyStruct = [
//                      pid = 32
//                      access_flags = 2
//                      user_id = "ibm"
//                  ]
//                  LoyalCustomer = false
//              ]
//          ]
//          [
//              7
//              [
//                  CompanyName = "Honeywell"
//                  ThirdPartyStruct = [
//                      pid = 27
//                      access_flags = 1
//                      user_id = "hw"
//                  ]
//                  LoyalCustomer = true
//              ]
//          ]
//          [
//              8
//              [
//                  CompanyName = "Burroughs"
//                  ThirdPartyStruct = [
//                      pid = 45
//                      access_flags = 0
//                      user_id = "burr"
//                  ]
//                  LoyalCustomer = true
//              ]
//          ]
//      ]
//  ]
//..
//
///Example 4: Printing Ranges, and Typed Pointers
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this examples we demonstrate two capabilities of a 'bslim::Printer'
// object: printing a range of elements using iterators and printing a pointer
// type.
//
// The 'printValue' or 'printAttribute' methods of 'bslim::Printer' will print
// out all of the elements in the range specified by a pair of iterator
// arguments, which can be of any type that provides appropriately behaving
// operators '++', '*', and '==' (a non-void pointer would qualify).
//
// When 'bslim' encounters a single pointer of type 'TYPE *', where 'TYPE' is
// neither 'void' nor 'char', the pointer value is printed out in hex followed
// by printing out the value of 'TYPE'.  A compile error will occur if bslim is
// unable to print out 'TYPE'.
//
// As an example, we print out a range of pointers to sets.
//
// First we create 3 sets and populate them with different values.
//..
//  typedef bsl::set<int> Set;
//
//  Set s0, s1, s2;
//
//  s0.insert(0);
//  s0.insert(1);
//  s0.insert(2);
//
//  s1.insert(4);
//  s1.insert(5);
//
//  s2.insert(8);
//..
// Then, we store the addresses to those 3 sets into a fixed-length array:
//..
//  const Set *setArray[] = { &s0, &s1, &s2 };
//  const int NUM_SET_ARRAY = sizeof setArray / sizeof *setArray;
//..
// Next we use 'printValue' to print a range of values by supplying an iterator
// to the beginning and end of the range, in the address of 'setArray' and the
// address one past the end of 'setArray':
//..
//  bslim::Printer printer(&cout, 0, -1);
//  printer.printValue(setArray + 0, setArray + NUM_SET_ARRAY);
//..
//  The expected output is:
//..
//  [ 0xffbfd688 [ 0 1 2 ] 0xffbfd678 [ 4 5 ] 0xffbfd668 [ 8 ] ]
//..
//
///Example 5: 'print' Method for a Low-Level Value-Semantic Class
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// For very simple classes, it may be desirable always to format the attributes
// on a single line.  In this example, we discuss the 'print' method formatting
// for such a low-level value-semantic class.
//
// Usually, single-line or multi-line formatting options are specified by the
// value of the 'spacesPerLevel' argument, but for a simple class that always
// prints on a single line, the only difference between the single- and
// multi-line cases is that a newline character is printed at the end of the
// output for the multi-line case.  For such classes, the "name" of the
// attribute and the enclosing brackets may be omitted as well.
//
// For example, consider a class, 'DateTz', having as attributes a local date
// and a time offset:
//..
//  class DateTz {
//      // This 'class' represents a date value explicitly in a local time
//      // zone.  The offset of that time (in minutes) from UTC is also part of
//      // the value of this class.
//
//    private:
//      // DATA
//      int d_localDate;  // date in YYYYMMDD format, local to the timezone
//                        // indicated by 'd_offset'
//
//      int d_offset;     // offset from UTC (in minutes)
//
//    public:
//      // ...
//      // ACCESSORS
//      bsl::ostream& print(bsl::ostream& stream,
//                          int           level          = 0,
//                          int           spacesPerLevel = 4) const;
//      // ...
//  };
//..
// The 'Printer' class may be used in this case to print the start and end
// indentation by passing a 'suppressBracket' flag to the 'start' and 'end'
// methods.  The value itself can be written to the stream directly without
// using 'Printer'.  Note that to ensure correct formatting of the value in the
// presence of a call to 'setw' on the stream, the output must be written to a
// 'bsl::ostringstream' first; the string containing the output can then be
// written to the specified 'stream':
//..
//  bsl::ostream& DateTz::print(bsl::ostream& stream,
//                              int           level,
//                              int           spacesPerLevel) const
//  {
//     if (stream.bad()) {
//         return stream;                                             // RETURN
//     }
//
//     bsl::ostringstream tmp;
//     tmp << d_localDate;
//
//     const char sign    = d_offset < 0 ? '-' : '+';
//     const int  minutes = '-' == sign ? -d_offset : d_offset;
//     const int  hours   = minutes / 60;
//
//     // space usage: +-  hh  mm  nil
//     const int SIZE = 1 + 2 + 2 + 1;
//     char buf[SIZE];
//
//     // Use at most 2 digits for 'hours'
//     if (hours < 100) {
//         bsl::sprintf(buf, "%c%02d%02d", sign, hours, minutes % 60);
//     }
//     else {
//         bsl::sprintf(buf, "%cXX%02d", sign, minutes % 60);
//     }
//
//     tmp << buf;
//
//     bslim::Printer printer(&stream, level, spacesPerLevel);
//     printer.start(true);
//     stream << tmp.str();
//     printer.end(true);
//
//     return stream << bsl::flush;
//  }
//..
// Sample output for 'DateTz::print(bsl::cout, 0, -4):
//..
//  01JAN2011-0500
//..
// Sample output for 'DateTz::print(bsl::cout, 0, 4):
//..
//  01JAN2011-0500<\n>
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISARRAY
#include <bslmf_isarray.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_SELECTTRAIT
#include <bslmf_selecttrait.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

namespace bslim {

                                // =============
                                // class Printer
                                // =============

class Printer {
    // This class implements a *mechanism* used to format data as required by
    // the standard BDE 'print' method contract.

    // DATA
    bsl::ostream *d_stream_p;                  // output stream (held, not
                                               // owned)

    int           d_level;                     // level used in formatting

    int           d_levelPlusOne;              // 'd_level + 1'; useful in
                                               // implementation

    bool          d_suppressInitialIndentFlag; // formatting flag

    int           d_spacesPerLevel;            // spaces per level used in
                                               // formatting

  private:
    // NOT IMPLEMENTED
    Printer& operator=(const Printer&);

  public:
    // CREATORS
    Printer(bsl::ostream *stream, int level, int spacesPerLevel);
        // Create a 'Printer' object that will print to the specified 'stream'
        // in a format dictated by the values of the specified 'level' and
        // 'spacesPerLevel', as per the contract of the standard BDE 'print'
        // method.  The behavior is undefined unless 'stream' is valid.

    ~Printer();
        // Destroy this 'Printer' object.

    // ACCESSORS
    int absLevel() const;
        // Return the absolute value of the formatting level supplied at
        // construction.

    void end(bool suppressBracket = false) const;
        // If 'spacesPerLevel() >= 0', print a newline character to the output
        // stream supplied at construction.  If the optionally specified
        // 'suppressBracket' is false, print a closing square bracket, indented
        // by 'absLevel() * spacesPerLevel()' blank spaces.


    template <class TYPE>
    void printAttribute(const char *name, const TYPE& data) const;
        // Format to the output stream supplied at construction the specified
        // 'data', prefixed by the specified 'name'.  Format 'data' based on
        // the parameterized 'TYPE':
        //
        //: o If 'TYPE' is a fundamental type, output 'data' to the stream.
        //:
        //: o If 'TYPE' is a fixed length array ('Element[NUM]') and not a char
        //:   array, print out all the elements of the array.
        //:
        //: o If 'TYPE' is 'void * or 'const void *', or function pointer,
        //:   print the address value of 'data' in hexadecimal format if it is
        //:   not 0, and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is 'char *', 'const char *', 'char [*]', or 'const char
        //:   '[*]' or 'bsl::string' print 'data' to the stream as a
        //:   null-terminated C-style string enclosed in quotes if 'data' is
        //:   not 0, and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is a pointer type (other than the, potentially
        //:   const-qualified,  'char *' or 'void *'), print the address
        //:   value of 'data' in hexadecimal format, then format the object at
        //:   that address if 'data' is not 0, and print the string "NULL"
        //:   otherwise.  There will be a compile-time error if 'data' is a
        //:   pointer to a user-defined type that does not provide a standard
        //:   'print' method.
        //:
        //: o If 'TYPE' is a 'bsl::pair' object, print out the two elements of
        //:   the pair.
        //:
        //: o If 'TYPE' is a 'bslstl::StringRef' object, print the referenced
        //:   string enclosed in quotes (possibly including embedded 0s).
        //:
        //: o If 'TYPE' has STL iterators (this includes all STL sequence and
        //:   associative containers: vector, deque, list, set, map, multiset,
        //:   multimap, unordered_set, unordered_map, unordered_multiset, and
        //:   unordered_multimap), print all the objects in the container.
        //:
        //: o If 'TYPE' is any other type, call the standard 'print' method on
        //:   'data', specifying one additional level of indentation than the
        //:   current one.  There will be a compile-time error if 'TYPE' does
        //:   not provide a standard 'print' method.
        //
        // If 'spacesPerLevel() < 0', format 'data' on a single line.
        // Otherwise, indent 'data' by '(absLevel() + 1) * spacesPerLevel()'
        // blank spaces.  The behavior is undefined if 'TYPE' is a 'char *',
        // but not a null-terminated string.

    template <class ITERATOR>
    void printAttribute(const char      *name,
                        const ITERATOR&  begin,
                        const ITERATOR&  end) const;
        // Format to the output stream supplied at construction, the specified
        // 'name' followed by the range of values starting at the specified
        // 'begin' position and ending immediately before the specified 'end'
        // position.  The parameterized 'ITERATOR' type must support
        // 'operator++', 'operator*', and 'operator=='.  This function will
        // call 'printValue' on each element in the range '[begin, end)'.

    void printEndIndentation() const;
        // Print to the output stream supplied at construction
        // 'absLevel() * spacesPerLevel()' blank spaces if
        // 'spacesPerLevel() >= 0', and print a single blank space otherwise.

    template <class TYPE, class PRINT_FUNCTOR>
    void printForeign(const TYPE&           data,
                      const PRINT_FUNCTOR&  printFunctionObject,
                      const char           *name) const;
        // Print to the output stream supplied at construction the specified
        // 'name', if name is not 0, and then call the specified
        // 'printFunctionObject' with the specified 'data', the 'stream'
        // supplied at construction, 'absLevel() + 1', and 'spacesPerLevel()'.
        // The parameterized 'PRINT_FUNCTOR' must be an invocable type whose
        // arguments match the following function signature:
        //..
        //  bsl::ostream& (*)(bsl::ostream& stream,
        //                    const TYPE&   data,
        //                    int           level,
        //                    int           spacesPerLevel)
        //..

    void printHexAddr(const void *address, const char *name) const;
        // Write to the output stream supplied at construction the specified
        // 'address' in a hexadecimal format, if 'address' is not 0, and print
        // the string "NULL" otherwise, prefixed by the specified 'name' if
        // 'name' is not 0.  If 'spacesPerLevel() < 0', print on a single line.
        // If 'spacesPerLevel() >= 0', indent by
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces.

    void printIndentation() const;
        // Print to the output stream supplied at construction
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces if
        // 'spacesPerLevel() >= 0', and print a single blank space otherwise.

    template <class TYPE>
    void printOrNull(const TYPE& address, const char *name) const;
        // Format to the output stream supplied at construction the object at
        // the specified 'address', if 'address' is not 0, and print the string
        // "NULL" otherwise, prefixed by the specified 'name' if 'name' is not
        // 0.  If 'spacesPerLevel() < 0', print on a single line.  If
        // 'spacesPerLevel() >= 0', indent by
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces.  The behavior is
        // undefined unless 'TYPE' is a pointer type.

    template <class TYPE>
    void printValue(const TYPE& data) const;
        // Format to the output stream supplied at construction the specified
        // 'data'.  Format 'data' based on the parameterized 'TYPE':
        //
        //: o If 'TYPE' is a fundamental type, output 'data' to the stream.
        //:
        //: o If 'TYPE' is a fixed length array ('Element[NUM]') and not a char
        //:   array, print out all the elements of the array.
        //:
        //: o If 'TYPE' is 'void * or 'const void *', or function pointer,
        //:   print the address value of 'data' in hexadecimal format if it is
        //:   not 0, and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is 'char *', 'const char *', 'char [*]', or 'const char
        //:   '[*]' or 'bsl::string' print 'data' to the stream as a
        //:   null-terminated C-style string enclosed in quotes if 'data' is
        //:   not 0, and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is a pointer type (other than the, potentially
        //:   const-qualified,  'char *' or 'void *'), print the address
        //:   value of 'data' in hexadecimal format, then format the object at
        //:   that address if 'data' is not 0, and print the string "NULL"
        //:   otherwise.  There will be a compile-time error if 'data' is a
        //:   pointer to a user-defined type that does not provide a standard
        //:   'print' method.
        //:
        //: o If 'TYPE' is a 'bsl::pair' object, print out the two elements of
        //:   the pair.
        //:
        //: o If 'TYPE' is a 'bslstl::StringRef' object, print the referenced
        //:   string enclosed in quotes (possibly including embedded 0s).
        //:
        //: o If 'TYPE' has STL iterators (this includes all STL sequence and
        //:   associative containers: vector, deque, list, set, map, multiset,
        //:   multimap, unordered_set, unordered_map, unordered_multiset, and
        //:   unordered_multimap), print all the objects in the container.
        //:
        //: o If 'TYPE' is any other type, call the standard 'print' method on
        //:   'data', specifying one additional level of indentation than the
        //:   current one.  There will be a compile-time error if 'TYPE' does
        //:   not provide a standard 'print' method.
        //
        // If 'spacesPerLevel() < 0', format 'data' on a single line.
        // Otherwise, indent 'data' by '(absLevel() + 1) * spacesPerLevel()'
        // blank spaces.  The behavior is undefined if 'TYPE' is a 'char *',
        // but not a null-terminated string.

    template <class ITERATOR>
    void printValue(const ITERATOR& begin,
                    const ITERATOR& end) const;
        // Format to the output stream supplied at construction, the range of
        // values starting at the specified 'begin' position and ending
        // immediately before the specified 'end' position.  The parameterized
        // 'ITERATOR' type must support 'operator++', 'operator*', and
        // 'operator=='.  This function will call 'printValue' on each element
        // in the range '[begin, end)'.

    int spacesPerLevel() const;
        // Return the number of whitespace characters to output for each level
        // of indentation.  The number of whitespace characters for each level
        // of indentation is configured using the 'spacesPerLevel' supplied at
        // construction.

    void start(bool suppressBracket = false) const;
        // Print to the output stream supplied at construction
        // 'absLevel() * spacesPerLevel()' blank spaces if the
        // 'suppressInitialIndentFlag' is 'false', and suppress the initial
        // indentation otherwise.  If the optionally specified
        // 'suppressBracket' is 'false', print an opening square bracket.

    bool suppressInitialIndentFlag() const;
        // Return 'true' if the initial output indentation will be suppressed,
        // and 'false' otherwise.  The initial indentation will be suppressed
        // if the 'level' supplied at construction is negative.
};

                        // =====================
                        // struct Printer_Helper
                        // =====================

struct Printer_Helper {
    // This struct is an aid to the implementation of the accessors of the
    // 'Printer' mechanism.  It provides a method template, 'print', that
    // adheres to the BDE 'print' method contract.  It is not to be accessed
    // directly by clients of 'bslim'.

    // CLASS METHODS
    template <class TYPE>
    static void print(bsl::ostream& stream,
                      const TYPE&   data,
                      int           level,
                      int           spacesPerLevel);
        // Format the specified 'data' to the specified output 'stream' at the
        // (absolute value of) the specified indentation 'level', using the
        // specified 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  Note that this
        // function dispatches to 'printRaw' based on the type traits of the
        // deduced (template parameter) 'TYPE'.

    template <class ITERATOR>
    static void print(bsl::ostream&   stream,
                      const ITERATOR& begin,
                      const ITERATOR& end,
                      int             level,
                      int             spacesPerLevel);
        // Format the range of objects specified by '[ begin, end )' to the
        // specified output 'stream' at the (absolute value of) the specified
        // indentation 'level', using the specified 'spacesPerLevel', the
        // number of spaces per indentation level for the objects and their
        // nested objects, where 'ITERATOR' supports the operators '++' and '*'
        // to access the objects.  Individual objects are printed with
        // 'printValue'.

                      // Fundamental types

    static void printRaw(bsl::ostream&                  stream,
                         char                           data,
                         int                            level,
                         int                            spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_fundamental>);
    static void printRaw(bsl::ostream&                  stream,
                         bool                           data,
                         int                            level,
                         int                            spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_fundamental>);
    template <class TYPE>
    static void printRaw(bsl::ostream&                  stream,
                         TYPE                           data,
                         int                            level,
                         int                            spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_fundamental>);
    template <class TYPE>
    static void printRaw(bsl::ostream&                  stream,
                         TYPE                           data,
                         int                            level,
                         int                            spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_enum>);

                      // Function pointer types

    template <class TYPE>
    static void printRaw(bsl::ostream&                      stream,
                         const TYPE&                        data,
                         int                                level,
                         int                                spacesPerLevel,
                         bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

                      // Pointer types

    static void printRaw(bsl::ostream&              stream,
                         const char                *data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_pointer>);
    static void printRaw(bsl::ostream&              stream,
                         const void                *data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_pointer>);
    template <class TYPE>
    static void printRaw(bsl::ostream&              stream,
                         const TYPE                *data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_pointer>);
    template <class TYPE>
    static void printRaw(bsl::ostream&              stream,
                         const TYPE                *data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<bsl::is_array>);

                      // Types with STL iterators

    static void printRaw(bsl::ostream&                      stream,
                         const bsl::string&                 data,
                         int                                level,
                         int                                spacesPerLevel,
                         bslmf::SelectTraitCase<bslalg::HasStlIterators>);
    template <class TYPE>
    static void printRaw(bsl::ostream&                      stream,
                         const TYPE&                        data,
                         int                                level,
                         int                                spacesPerLevel,
                         bslmf::SelectTraitCase<bslalg::HasStlIterators>);

                      // Default types

    template <class T1, class T2>
    static void printRaw(bsl::ostream&              stream,
                         const bsl::pair<T1, T2>&   data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<>);

    static void printRaw(bsl::ostream&              stream,
                         const bslstl::StringRef&   data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<>);

    template <class TYPE>
    static void printRaw(bsl::ostream&              stream,
                         const TYPE&                data,
                         int                        level,
                         int                        spacesPerLevel,
                         bslmf::SelectTraitCase<>);
        // The 'print' method of this class dispatches based on 'TYPE' and
        // traits to a 'printRaw' method to do the actual printing of the
        // specified 'data' to the specified 'stream' with indentation based on
        // the specified 'level' and 'spacesPerLevel'.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


                                // -------------
                                // class Printer
                                // -------------

// ACCESSORS

template <class TYPE>
void Printer::printAttribute(const char *name, const TYPE& data) const
{
    BSLS_ASSERT_SAFE(0 != name);

    printIndentation();

    *d_stream_p << name << " = ";

    Printer_Helper::print(*d_stream_p,
                          data,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

template <class ITERATOR>
void Printer::printAttribute(const char      *name,
                             const ITERATOR&  begin,
                             const ITERATOR&  end) const
{
    BSLS_ASSERT_SAFE(0 != name);

    printIndentation();

    *d_stream_p << name << " = ";

    Printer_Helper::print(*d_stream_p,
                          begin,
                          end,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

template <class TYPE, class PRINT_FUNCTOR>
void Printer::printForeign(const TYPE&           data,
                           const PRINT_FUNCTOR&  printFunctionObject,
                           const char           *name) const
{
    printIndentation();

    if (name) {
        *d_stream_p << name << " = ";
    }

    printFunctionObject(*d_stream_p,
                        data,
                        -d_levelPlusOne,
                        d_spacesPerLevel);
}

template <class TYPE>
void Printer::printOrNull(const TYPE& address, const char *name) const
{
    printIndentation();

    if (name) {
        *d_stream_p << name << " = ";
    }

    if (0 == address) {
        *d_stream_p << "NULL";
        if (d_spacesPerLevel >= 0) {
            *d_stream_p << '\n';
        }
    }
    else {
        Printer_Helper::print(*d_stream_p,
                              *address,
                              -d_levelPlusOne,
                              d_spacesPerLevel);
    }
}

template <>
inline
void Printer::printOrNull<const void *>(const void *const&  address,
                                        const char         *name) const
{
    printIndentation();

    if (name) {
        *d_stream_p << name << " = ";
    }
    const void *temp = address;

    Printer_Helper::print(*d_stream_p,
                          temp,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

template <>
inline
void Printer::printOrNull<void *>(void *const& address, const char *name) const
{
    const void *const& temp = address;
    printOrNull(temp, name);
}

template <class TYPE>
inline
void Printer::printValue(const TYPE& data) const
{
    printIndentation();

    Printer_Helper::print(*d_stream_p,
                          data,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

template <class ITERATOR>
void Printer::printValue(const ITERATOR& begin,
                         const ITERATOR& end) const
{
    printIndentation();

    Printer_Helper::print(*d_stream_p,
                          begin,
                          end,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

                            // ---------------------
                            // struct Printer_Helper
                            // ---------------------

// CLASS METHODS

// 'Printer_Helper::print(stream, data, level, spacesPerLevel)', though defined
// first in the struct, is implemented last within this class so it can inline
// the calls to 'printRaw' that it makes.

template <class ITERATOR>
inline
void Printer_Helper::print(bsl::ostream&   stream,
                           const ITERATOR& begin,
                           const ITERATOR& end,
                           const int       level,
                           const int       spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (ITERATOR it = begin; end != it; ++it) {
        printer.printValue(*it);
    }
    printer.end();
}

                      // Fundamental types

template <class TYPE>
inline
void Printer_Helper::printRaw(bsl::ostream&                  stream,
                              TYPE                           data,
                              int                            ,
                              int                            spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_fundamental>)
{
    stream << data;
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

template <class TYPE>
inline
void Printer_Helper::printRaw(bsl::ostream&                  stream,
                              TYPE                           data,
                              int                            ,
                              int                            spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_enum>)
{
    printRaw(stream, data, 0, spacesPerLevel,
             bslmf::SelectTraitCase<bsl::is_fundamental>());
}

                      // Function pointer types

template <class TYPE>
inline
void Printer_Helper::printRaw(
                             bsl::ostream&                      stream,
                             const TYPE&                        data,
                             int                                level,
                             int                                spacesPerLevel,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    // GCC 3.4.6 does not allow a reinterpret-cast a function pointer directly
    // to 'void *', so first cast it to an integer data type.

    Printer_Helper::print(stream,
                          reinterpret_cast<const void *>(
                                 reinterpret_cast<bsls::Types::UintPtr>(data)),
                          level,
                          spacesPerLevel);
}

                      // Pointer types

template <class TYPE>
inline
void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const TYPE                *data,
                              int                        level,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_pointer>)
{
    printRaw(stream,
             static_cast<const void *>(data),
             level,
             -1,
             bslmf::SelectTraitCase<bsl::is_pointer>());
    if (0 == data) {
        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
    }
    else {
        stream << ' ';
        Printer_Helper::print(stream, *data, level, spacesPerLevel);
    }
}

template <class TYPE>
inline
void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const TYPE                *data,
                              int                        level,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_array>)
{
    printRaw(stream, data, level, spacesPerLevel,
             bslmf::SelectTraitCase<bsl::is_pointer>());
}


                      // Types with STL iterators

inline
void Printer_Helper::printRaw(
                             bsl::ostream&                      stream,
                             const bsl::string&                 data,
                             int                                level,
                             int                                spacesPerLevel,
                             bslmf::SelectTraitCase<bslalg::HasStlIterators>)
{
    printRaw(stream,
             data.c_str(),
             level,
             spacesPerLevel,
             bslmf::SelectTraitCase<bsl::is_pointer>());
}

template <class TYPE>
inline
void Printer_Helper::printRaw(
                             bsl::ostream&                      stream,
                             const TYPE&                        data,
                             int                                level,
                             int                                spacesPerLevel,
                             bslmf::SelectTraitCase<bslalg::HasStlIterators>)
{
    print(stream, data.begin(), data.end(), level, spacesPerLevel);
}

                      // Default types

template <class T1, class T2>
inline
void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const bsl::pair<T1, T2>&   data,
                              int                        level,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<>)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printValue(data.first);
    printer.printValue(data.second);
    printer.end();
}

template <class TYPE>
inline
void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const TYPE&                data,
                              int                        level,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<>)
{
    data.print(stream, level, spacesPerLevel);
}

// This method, though declared first in the struct, is placed last among the
// methods in 'Printer_Helper' so that it can inline the 'printRaw' methods it
// calls.

template <class TYPE>
inline
void Printer_Helper::print(bsl::ostream& stream,
                           const TYPE&   data,
                           int           level,
                           int           spacesPerLevel)
{
    typedef bslmf::SelectTrait<TYPE,
                               bsl::is_fundamental,
                               bsl::is_enum,
                               bslmf::IsFunctionPointer,
                               bsl::is_pointer,
                               bsl::is_array,
                               bslalg::HasStlIterators> Selection;

    Printer_Helper::printRaw(stream, data, level, spacesPerLevel, Selection());
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
