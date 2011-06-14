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
//@AUTHOR: Ramya Iyer (riyer)
//
//@DESCRIPTION: This component provides a mechanism class, 'Printer', that, in
// many cases, simplifies the implementation of types providing a 'print'
// method with the signature:
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
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use 'Printer' to implement the
// standard 'print' function of a value-semantic class having multiple
// attributes.  Suppose we have a class, 'RecordAttributes', that provides a
// container for a fixed set of attributes.  A 'RecordAttributes' object has
// two attributes, "timestamp", of type 'my::Datetime', and
// "processID", of type 'int':
//..
//  class RecordAttributes {
//      // This class provides a container for a fixed set of attributes.
//
//      // DATA
//      my::Datetime d_timestamp;  // creation date and time
//      int          d_processID;  // process id of creator
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
// 'RecordAttributes::print' can be implemented using 'Printer':
//..
//  bsl::ostream& RecordAttributes::print(bsl::ostream& stream,
//                                        int           level,
//                                        int           spacesPerLevel) const
//  {
//      if (stream.bad()) {
//          return stream;                                            // RETURN
//      }
//
//      bslim::Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//      printer.printAttribute("timestamp", d_timestamp);
//      printer.printAttribute("process ID", d_processID);
//      printer.end();
//
//      return stream;
//  }
//..
// Sample output for 'RecordAttributes::print(bsl::cout, 0, -4)':
//..
//  [ timestamp = 01JAN0001_24:00:00.000 processID = 11 ]
//..
// Sample output for 'RecordAttributes::print(bsl::cout, 0, 4)':
//..
//  [
//      timestamp = 01JAN0001_24:00:00.000
//      processID = 11
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
// For example, consider a memory manager class, 'BlockList', that
// maintains a linked list of memory blocks:
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
//          bsls_AlignmentUtil::MaxAlignedType   d_memory;       // force
//                                                               // alignment
//      };
//
//      // DATA
//      Block           *d_head_p;      // address of first block of memory
//                                      // (or 0)
//
//      bslma_Allocator *d_allocator_p; // memory allocator; held, but not
//                                      // owned
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
//
//      Block *it = d_head_p;
//      while (it) {
//          printer.printHexAddr(it, 0);
//          it = it->d_next_p;
//      }
//
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
///Example 3: 'print' Utility for a Class Without the 'print' Method
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we use a 'Printer' object to help format the properties of
// a class supplied by a third-party that does not implement the standard
// 'print' method.  Consider a class, 'ThirdPartyClass', that has no standard
// 'print' method, but has functions to access each of its attributes:
//..
//  class ThirdPartyClass {
//      // ...
//    public:
//      // ...
//      // ACCESSORS
//      int getAttribute1() const;
//      int getAttribute2() const;
//  };
//..
// We create a component, 'ThirdPartyClassUtil', and define in it a 'print'
// utility method that formats objects of class 'ThirdPartyClass' as specified
// by 'level' and 'spacesPerLevel':
//..
//  struct ThirdPartyClassUtil {
//      static bsl::ostream& print(bsl::ostream&          stream,
//                                 const ThirdPartyClass& obj,
//                                 int                    level          = 0,
//                                 int                    spacesPerLevel = 4);
//  };
//..
// Note that, typically, other utility functions pertaining to
// 'ThirdPartyClass' can also reside in 'ThirdPartyClassUtil'.
//
// We then use the accessors of 'ThirdPartyClass' to implement the 'print'
// utility using a 'Printer' object:
//..
//  bsl::ostream&
//  ThirdPartyClassUtil::print(bsl::ostream&          stream,
//                             const ThirdPartyClass& obj,
//                             int                    level,
//                             int                    spacesPerLevel)
//  {
//      if (stream.bad()) {
//          return stream;                                            // RETURN
//      }
//
//      Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//      printer.printAttribute("Attribute2", obj.getAttribute1());
//      printer.printAttribute("Attribute2", obj.getAttribute2());
//      printer.end();
//
//      return stream;
//  }
//..
// Now suppose a class, 'MyClass', has an attribute of type 'ThirdPartyClass':
//..
//  class MyClass {
//      // ...
//      // DATA
//      ThirdPartyClass d_attributeA;
//      int             d_attributeB;
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
// Then the 'MyClass::print' method can be implemented using the 'Printer'
// mechanism:
//..
//  bsl::ostream& MyClass::print(bsl::ostream& stream,
//                               int           level,
//                               int           spacesPerLevel) const
//  {
//      if (stream.bad()) {
//          return stream;                                            // RETURN
//      }
//
//      Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//      printer.printForeign(d_attributeA,
//                           &ThirdPartyClassUtil::print,
//                           "AttributeA");
//      printer.printAttribute("AttributeB", d_attributeB);
//      printer.end();
//
//      return stream;
//  }
//..
// Sample output for 'MyClass::print(bsl::cout, 0, -4)':
//..
//  [ AttributeA = [ Attribute1 = 100 Attribute2 = 200] AttributeB = 1 ]
//..
// Sample output for 'MyClass::print(bsl::cout, 0, 4)':
//..
//  [
//      AttributeA = [
//          Attribute1 = 100
//          Attribute2 = 200
//      ]
//      AttributeB = 1
//  ]
//..
//
///Example 4: 'print' Method for a Low-Level Value-Semantic Class
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// For very simple classes, it may be desirable always to format the attributes
// on a single line.  In this example, we discuss the 'print' method formatting
// for such a low-level value semantic class.
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
//      // zone.  The offset of that time (in minutes) from GMT is also part of
//      // the value of this class.
//
//    private:
//      // DATA
//      int d_localDate;  // date in YYYYMMDD format, local to the timezone
//                        // indicated by 'd_offset'
//
//      int d_offset;     // offset from GMT (in minutes)
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

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
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
    // PRIVATE ACCESSORS
    void printEndIndentation() const;
        // Print to the output stream supplied at construction
        // 'absLevel() * spacesPerLevel()' blank spaces if
        // 'spacesPerLevel() >= 0', and print a single blank space otherwise.

    void printIndentation() const;
        // Print to the output stream supplied at construction
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces if
        // 'spacesPerLevel() >= 0', and print a single blank space otherwise.

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
        // If 'spacesPerLevel() >= 0', print a newline character.  If the
        // optionally specified 'suppressBracket' is false, print to the output
        // stream supplied at construction a closing square bracket, indented
        // by 'absLevel() * spacesPerLevel()' blank spaces.

    template <class TYPE>
    void print(const TYPE& data, const char *name) const;
        // [!DEPRECATED!] Format to the output stream supplied at construction
        // the specified 'data', prefixed by the specified 'name' if 'name' is
        // not 0.  Format 'data' based on the parameterized 'TYPE':
        //
        //: o If 'TYPE' is a fundamental type, output 'data' to the stream.
        //:
        //: o If 'TYPE' is a pointer type (other than 'char *' or 'void *'),
        //:   print the address value of 'data' in hexadecimal format, then
        //:   format the object at that address if 'data' is not 0, and print
        //:   the string "NULL" otherwise.  There will be a compile-time error
        //:   if 'data' is a pointer to a user-defined type that does not
        //:   provide a standard 'print' method.
        //:
        //: o If 'TYPE' is 'char *', print 'data' to the stream as a null-
        //:   terminated C-style string enclosed in quotes if 'data' is not 0,
        //:   and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is 'void *', print the address value of 'data' in
        //:   hexadecimal format if it is not 0, and print the string "NULL"
        //:   otherwise.
        //:
        //: o If 'TYPE' is any other type, call the standard 'print' method on
        //:   'data', specifying one additional level of indentation than the
        //:   current one.  There will be a compile-time error if 'TYPE' does
        //:   not provide a standard 'print' method.
        //
        // If 'spacesPerLevel() < 0', format 'data' on a single line.
        // If 'spacesPerLevel() >= 0', indent 'data' by
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces.  The behavior is
        // undefined if 'TYPE' is a 'char *', but not a null-terminated string.

    template <class TYPE>
    void printAttribute(const char *name, const TYPE& data) const;
        // Format to the output stream supplied at construction the specified
        // 'data', prefixed by the specified 'name'.  Format 'data' based on
        // the parameterized 'TYPE':
        //
        //: o If 'TYPE' is a fundamental type, output 'data' to the stream.
        //:
        //: o If 'TYPE' is a pointer type (other than 'char *' or 'void *'),
        //:   print the address value of 'data' in hexadecimal format, then
        //:   format the object at that address if 'data' is not 0, and print
        //:   the string "NULL" otherwise.  There will be a compile-time error
        //:   if 'data' is a pointer to a user-defined type that does not
        //:   provide a standard 'print' method.
        //:
        //: o If 'TYPE' is 'char *', print 'data' to the stream as a null-
        //:   terminated C-style string enclosed in quotes if 'data' is not 0,
        //:   and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is 'void *', print the address value of 'data' in
        //:   hexadecimal format if it is not 0, and print the string "NULL"
        //:   otherwise.
        //:
        //: o If 'TYPE' is any other type, call the standard 'print' method on
        //:   'data', specifying one additional level of indentation than the
        //:   current one.  There will be a compile-time error if 'TYPE' does
        //:   not provide a standard 'print' method.
        //
        // If 'spacesPerLevel() < 0', format 'data' on a single line.
        // If 'spacesPerLevel() >= 0', indent 'data' by
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces.  The behavior is
        // undefined if 'TYPE' is a 'char *', but not a null-terminated string.

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
        //: o If 'TYPE' is a pointer type (other than 'char *' or 'void *'),
        //:   print the address value of 'data' in hexadecimal format, then
        //:   format the object at that address if 'data' is not 0, and print
        //:   the string "NULL" otherwise.  There will be a compile-time error
        //:   if 'data' is a pointer to a user-defined type that does not
        //:   provide a standard 'print' method.
        //:
        //: o If 'TYPE' is 'char *', print 'data' to the stream as a null-
        //:   terminated C-style string enclosed in quotes if 'data' is not 0,
        //:   and print the string "NULL" otherwise.
        //:
        //: o If 'TYPE' is 'void *', print the address value of 'data' in
        //:   hexadecimal format if it is not 0, and print the string "NULL"
        //:   otherwise.
        //:
        //: o If 'TYPE' is any other type, call the standard 'print' method on
        //:   'data', specifying one additional level of indentation than the
        //:   current one.  There will be a compile-time error if 'TYPE' does
        //:   not provide a standard 'print' method.
        //
        // If 'spacesPerLevel() < 0', format 'data' on a single line.
        // If 'spacesPerLevel() >= 0', indent 'data' by
        // '(absLevel() + 1) * spacesPerLevel()' blank spaces.  The behavior is
        // undefined if 'TYPE' is a 'char *', but not a null-terminated string.

    int spacesPerLevel() const;
        // Return the number of whitespace characters to output for each
        // level of indentation.  The number of whitespace characters for
        // each level of indentation is configured using the 'spacesPerLevel'
        // supplied at construction.

    void start(bool suppressBracket = false) const;
        // Print to the output stream supplied at construction
        // 'absLevel() * spacesPerLevel()' blank spaces if the
        // 'suppressInitialIndentFlag' is 'false', and suppress the initial
        // indentation otherwise.  If the optionally specified
        // 'suppressBracket' is false, print an opening square bracket.

    bool suppressInitialIndentFlag() const;
        // Return 'true' if the initial output indentation will be
        // suppressed, and 'false' otherwise.  The initial indentation will be
        // suppressed if the 'level' supplied at construction is negative.
};

                        // =======================
                        // struct Printer_Selector
                        // =======================

struct Printer_Selector {

    // TYPES
    enum {
        // Enumeration used to discriminate between fundamental, pointer, and
        // user-defined types.

        BSLIM_FUNDAMENTAL  = 0,
        BSLIM_POINTER      = 1,
        BSLIM_USER_DEFINED = 2
    };
};

                        // =========================
                        // struct Printer_DetectType
                        // =========================

template <class TYPE>
struct Printer_DetectType {
    // This struct provides a meta-function to classify a data type as either
    // a fundamental, pointer, or user-defined type.

    // TYPES
    enum {
        VALUE = bslmf_IsFundamental<TYPE>::VALUE
                ? Printer_Selector::BSLIM_FUNDAMENTAL
                : bslmf_IsPointer<TYPE>::VALUE
                  ? Printer_Selector::BSLIM_POINTER
                  : Printer_Selector::BSLIM_USER_DEFINED
    };
};

                        // =====================
                        // struct Printer_Helper
                        // =====================

struct Printer_Helper {
    // This struct is an aid to the implementation of the accessors of the
    // 'Printer' mechanism.  It provides a method template, 'print', that
    // adheres to the BDE 'print' method contract.

    // CLASS METHODS
    template <class TYPE>
    static void print(bsl::ostream& stream,
                      const TYPE&   data,
                      int           level,
                      int           spacesPerLevel);
        // Format the specified 'data' to the specified output `stream' at the
        // (absolute value of) the specified indentation `level', using the
        // specified 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  If `level' is
        // negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

                        // =======================
                        // struct Printer_PrintImp
                        // =======================

template <class TYPE, int SELECTOR>
struct Printer_PrintImp;
    // This struct template is *not* meant to be instantiated.  It is an aid to
    // the implementation of the 'Printer_Helper::print' method.  Each of its
    // specializations provides a 'print' method that adheres to the BDE
    // 'print' method contract.

template <class TYPE>
struct Printer_PrintImp<TYPE, Printer_Selector::BSLIM_FUNDAMENTAL> {
    // This struct template is a partial specialization of 'Printer_PrintImp'
    // for fundamental types.  It provides a 'print' method that streams out
    // the fundamental type passed as argument, in accordance with the BDE
    // 'print' method contract.

    // CLASS METHODS
    static void print(bsl::ostream& stream,
                      const TYPE&   data,
                      int           level,
                      int           spacesPerLevel);
        // Format the specified 'data' (of fundamental type) to the specified
        // output `stream' at (the absolute value of) the specified indentation
        // `level', using the specified 'spacesPerLevel', the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // `level' is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <>
struct Printer_PrintImp<bool, Printer_Selector::BSLIM_FUNDAMENTAL> {
    // This struct provides a specialization of 'Printer_PrintImp' for 'bool'.
    // It provides a 'print' method that writes the value of the 'bool'
    // argument passed to it as the string 'true' or 'false', in accordance
    // with the BDE 'print' method contract.

    // CLASS METHODS
    static void print(bsl::ostream& stream,
                      bool          data,
                      int           level,
                      int           spacesPerLevel);
        // Format the specified 'data' (of fundamental type) to the specified
        // output `stream' at (the absolute value of) the specified indentation
        // `level', using the specified 'spacesPerLevel', the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // `level' is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <>
struct Printer_PrintImp<char, Printer_Selector::BSLIM_FUNDAMENTAL> {
    // This struct provides a specialization of 'Printer_PrintImp' for 'char'.
    // It provides a 'print' method that streams the 'char' argument passed to
    // it enclosed within quotes if it is an alpha-numeric character, or any of
    // the special characters
    // argument passed to it as the string 'true' or 'false', in accordance
    // with the BDE 'print' method contract.

    // CLASS METHODS
    static bool isPrintable(char character);
        // Return true if the specified 'character' lies in the range [32,127),
        // and return false otherwise.

    static void print(bsl::ostream& stream,
                      char          data,
                      int           level,
                      int           spacesPerLevel);
        // Format the specified 'data' (of fundamental type) to the specified
        // output `stream' at (the absolute value of) the specified indentation
        // `level', using the specified 'spacesPerLevel', the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // `level' is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <class TYPE>
struct Printer_PrintImp<TYPE, Printer_Selector::BSLIM_POINTER> {
    // This struct template is a partial specialization of 'Printer_PrintImp'
    // for pointer types.  It provides a 'print' method that formats the
    // address held by the pointer type passed as argument, and then formats
    // the held object, in accordance with the BDE 'print' method contract.

    // CLASS METHODS
    static void print(bsl::ostream& stream,
                      const TYPE&   data,
                      int           level,
                      int           spacesPerLevel);
        // Format the specified (pointer type) 'data' to the specified output
        // `stream' at the (absolute value of) the specified indentation
        // `level', using the specified 'spacesPerLevel', the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // 'data' is not 0, print the address held by data in hexadecimal
        // format and then format the held object, and print the string "NULL"
        // otherwise.  If `level' is negative, suppress indentation of the
        // first line.  If `spacesPerLevel' is negative format the entire
        // output on one line, suppressing all but the initial indentation (as
        // governed by `level').  If `stream' is not valid on entry, this
        // operation has no effect.
};

template <>
struct Printer_PrintImp<const void *, Printer_Selector::BSLIM_POINTER> {
    // This struct provides a specialization of 'Printer_PrintImp' for pointers
    // of type 'const void *'.  It provides a 'print' method that formats the
    // address held by the pointer passed as argument in accordance with the
    // BDE 'print' method contract, but does not dereference the pointer.

    // CLASS METHODS
    static void print(bsl::ostream&  stream,
                      const void    *data,
                      int            level,
                      int            spacesPerLevel);
        // Format the specified 'data' to the specified output `stream' at the
        // (absolute value of) the specified indentation `level', using the
        // specified 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  If 'data' is not 0,
        // print the address held by data in hexadecimal format and then format
        // the held object, and print the string "NULL" otherwise.  If `level'
        // is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <>
struct Printer_PrintImp<void *, Printer_Selector::BSLIM_POINTER> {
    // This struct provides a specialization of 'Printer_PrintImp' for pointers
    // of type 'void *'.  It provides a 'print' method that formats the address
    // held by the pointer passed as argument in accordance with the BDE
    // 'print' method contract, but does not dereference the pointer.

    // CLASS METHODS
    static void print(bsl::ostream&  stream,
                      const void    *data,
                      int            level,
                      int            spacesPerLevel);
        // Format the specified 'data' to the specified output `stream' at the
        // (absolute value of) the specified indentation `level', using the
        // specified 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  If 'data' is not 0,
        // print the address held by data in hexadecimal format and then format
        // the held object, and print the string "NULL" otherwise.  If `level'
        // is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <>
struct Printer_PrintImp<const volatile void *,
                        Printer_Selector::BSLIM_POINTER> {
    // This struct provides a specialization of 'Printer_PrintImp' for pointers
    // of type 'const volatile void *'.  It provides a 'print' method that
    // formats the address held by the pointer passed as argument in accordance
    // with the BDE 'print' method contract, but does not dereference the
    // pointer.

    // CLASS METHODS
    static void print(bsl::ostream&        stream,
                      const volatile void *data,
                      int                  level,
                      int                  spacesPerLevel);
        // Format the specified 'data' to the specified output `stream' at the
        // (absolute value of) the specified indentation `level', using the
        // specified 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  If 'data' is not 0,
        // print the address held by data in hexadecimal format and then format
        // the held object, and print the string "NULL" otherwise.  If `level'
        // is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <>
struct Printer_PrintImp<volatile void *, Printer_Selector::BSLIM_POINTER> {
    // This struct provides a specialization of 'Printer_PrintImp' for pointers
    // of type 'volatile void *'.  It provides a 'print' method that formats
    // the address held by the pointer passed as argument in accordance with
    // the BDE 'print' method contract, but does not dereference the pointer.

    // CLASS METHODS
    static void print(bsl::ostream&        stream,
                      const volatile void *data,
                      int                  level,
                      int                  spacesPerLevel);
        // Format the specified 'data' to the specified output `stream' at the
        // (absolute value of) the specified indentation `level', using the
        // specified 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  If 'data' is not 0,
        // print the address held by data in hexadecimal format and then format
        // the held object, and print the string "NULL" otherwise.  If `level'
        // is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <>
struct Printer_PrintImp<const char *, Printer_Selector::BSLIM_POINTER> {
    // This struct provides a specialization of 'Printer_PrintImp' for pointers
    // of type 'const char *'.  It provides a 'print' method that formats the
    // string held by the pointer passed as argument in accordance with the BDE
    // 'print' method contract.

    // CLASS METHODS
    static void print(bsl::ostream&  stream,
                      const char    *data,
                      int            level,
                      int            spacesPerLevel);
        // Format the specified string, 'data', to the specified output
        // `stream' at the (absolute value of) the specified indentation
        // `level', using the specified 'spacesPerLevel', the number of spaces
        // per indentation level for this and all of its nested objects.  If
        // `level' is negative, suppress indentation of the first line.  If
        // `spacesPerLevel' is negative format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // `level').  If `stream' is not valid on entry, this operation has no
        // effect.
};

template <class TYPE>
struct Printer_PrintImp<TYPE, Printer_Selector::BSLIM_USER_DEFINED> {
    // This struct provides a specialization of 'Printer_PrintImp' for
    // user-defined data types.  It provides a 'print' method that calls the
    // standard 'print' method of the parameterized user-defined data 'TYPE'.
    // Note that if the user-defined data type does not provide a standard
    // 'print' method, then a compile time error will be generated.

    // CLASS METHODS
    static void print(bsl::ostream& stream,
                      const TYPE&   data,
                      int           level,
                      int           spacesPerLevel);
        // Call the 'print' method of the specified 'data' (of user-defined
        // type), passing to it the values of the specified 'stream', the
        // specified 'level' and the specified 'spacesPerLevel'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------
                        // class Printer
                        // -------------

// ACCESSORS
template <class TYPE>
void Printer::print(const TYPE& data, const char *name) const
{
    printIndentation();

    if (name) {
        *d_stream_p << name << " = ";
    }

    Printer_Helper::print(*d_stream_p,
                          data,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

template <class TYPE>
void Printer::printAttribute(const char *name, const TYPE& data) const
{
    BSLS_ASSERT_SAFE(name != 0);
    print(data, name);
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

template <>
inline
void Printer::printOrNull<const volatile void *>(
                                       const volatile void *const&  address,
                                       const char                  *name) const
{
    printIndentation();

    if (name) {
        *d_stream_p << name << " = ";
    }
    const volatile void *temp = address;

    Printer_Helper::print(*d_stream_p,
                          temp,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

template <>
inline
void Printer::printOrNull<volatile void *>(volatile void *const&  address,
                                           const char            *name) const
{
    const volatile void *const& temp = address;
    printOrNull(temp, name);
}

template <class TYPE>
void Printer::printValue(const TYPE& data) const
{
    print(data, 0);
}

                        // ---------------------
                        // struct Printer_Helper
                        // ---------------------

// CLASS METHODS
template <class TYPE>
inline
void Printer_Helper::print(bsl::ostream& stream,
                           const TYPE&   data,
                           int           level,
                           int           spacesPerLevel)
{
    Printer_PrintImp<TYPE, Printer_DetectType<TYPE>::VALUE>::print(
                                                               stream,
                                                               data,
                                                               level,
                                                               spacesPerLevel);
}

                        // -----------------------
                        // struct Printer_PrintImp
                        // -----------------------

template <class TYPE>
void Printer_PrintImp<TYPE, Printer_Selector::BSLIM_FUNDAMENTAL>::print(
                                                  bsl::ostream& stream,
                                                  const TYPE&   data,
                                                  int           ,
                                                  int           spacesPerLevel)
{
    stream << data;
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

inline
void Printer_PrintImp<bool, Printer_Selector::BSLIM_FUNDAMENTAL>::print(
                                                  bsl::ostream& stream,
                                                  bool          data,
                                                  int           ,
                                                  int           spacesPerLevel)
{
    bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha
           << data;
    stream.flags(fmtFlags);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

inline
bool Printer_PrintImp<char, Printer_Selector::BSLIM_FUNDAMENTAL>::isPrintable(
                                                                char character)
{
    return (32 <= character) && (character < 127);
}

inline
void Printer_PrintImp<char, Printer_Selector::BSLIM_FUNDAMENTAL>::print(
                                                  bsl::ostream& stream,
                                                  char          data,
                                                  int           ,
                                                  int           spacesPerLevel)
{
#define HANDLE_CONTROL_CHAR(value) case value: stream << #value; break;
    if (isPrintable(data)) {
        // print within quotes
        stream << "'" << data <<"'";
    }
    else {
        switch(data) {
            HANDLE_CONTROL_CHAR('\n');
            HANDLE_CONTROL_CHAR('\t');
            HANDLE_CONTROL_CHAR('\0');

            default:
                // print as hex
                bsl::ios_base::fmtflags fmtFlags = stream.flags();
                stream << bsl::hex
                       << bsl::showbase
                       << static_cast<bsls_Types::UintPtr>(data);
                stream.flags(fmtFlags);

        }
    }
#undef HANDLE_CONTROL_CHAR

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

template <class TYPE>
void Printer_PrintImp<TYPE, Printer_Selector::BSLIM_POINTER>::print(
                                                  bsl::ostream& stream,
                                                  const TYPE&   data,
                                                  int           level,
                                                  int           spacesPerLevel)
{
    if (0 == data) {
        stream << "NULL";
        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
    }
    else {
        bsl::ios_base::fmtflags fmtFlags = stream.flags();
        stream << bsl::hex
               << bsl::showbase
               << data
               << ' ';
        stream.flags(fmtFlags);
        Printer_Helper::print(stream, *data, level, spacesPerLevel);
    }
}

inline
void Printer_PrintImp<const void *, Printer_Selector::BSLIM_POINTER>::print(
                                                 bsl::ostream&  stream,
                                                 const void    *data,
                                                 int            ,
                                                 int            spacesPerLevel)
{
    if (0 == data) {
        stream << "NULL";
    }
    else {
        bsl::ios_base::fmtflags fmtFlags = stream.flags();
        stream << bsl::hex
               << bsl::showbase
               << reinterpret_cast<bsls_Types::UintPtr>(data);
        stream.flags(fmtFlags);
    }
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

inline
void Printer_PrintImp<void *, Printer_Selector::BSLIM_POINTER>::print(
                                                 bsl::ostream&  stream,
                                                 const void    *data,
                                                 int            level,
                                                 int            spacesPerLevel)
{
    Printer_PrintImp<const void *, Printer_Selector::BSLIM_POINTER>::print(
                                                               stream,
                                                               data,
                                                               level,
                                                               spacesPerLevel);
}

inline
void Printer_PrintImp<const volatile void *,
                      Printer_Selector::BSLIM_POINTER>::print(
                                           bsl::ostream&        stream,
                                           const volatile void *data,
                                           int                  ,
                                           int                  spacesPerLevel)
{
    if (0 == data) {
        stream << "NULL";
    }
    else {
        bsl::ios_base::fmtflags fmtFlags = stream.flags();
        stream << bsl::hex
               << bsl::showbase
               << reinterpret_cast<bsls_Types::UintPtr>(data);
        stream.flags(fmtFlags);
    }
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

inline
void Printer_PrintImp<volatile void *, Printer_Selector::BSLIM_POINTER>::print(
                                           bsl::ostream&        stream,
                                           const volatile void *data,
                                           int                  level,
                                           int                  spacesPerLevel)
{
    Printer_PrintImp<const volatile void *,
                     Printer_Selector::BSLIM_POINTER>::print(stream,
                                                             data,
                                                             level,
                                                             spacesPerLevel);
}

inline
void Printer_PrintImp<const char *, Printer_Selector::BSLIM_POINTER>::print(
                                                 bsl::ostream&  stream,
                                                 const char    *data,
                                                 int            ,
                                                 int            spacesPerLevel)
{
    if (0 == data) {
        stream << "NULL";
    }
    else {
        stream << '"' << data << '"';
    }
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

template <class TYPE>
inline
void Printer_PrintImp<TYPE, Printer_Selector::BSLIM_USER_DEFINED>::print(
                                                  bsl::ostream& stream,
                                                  const TYPE&   data,
                                                  int           level,
                                                  int           spacesPerLevel)
{
    data.print(stream, level, spacesPerLevel);
}

}  // close namespace bslim
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
