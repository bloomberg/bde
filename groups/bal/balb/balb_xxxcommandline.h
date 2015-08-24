// balb_xxxcommandline.h                                              -*-C++-*-
#ifndef INCLUDED_BALB_XXXCOMMANDLINE
#define INCLUDED_BALB_XXXCOMMANDLINE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide command line parsing, validation, and access.
//
//@CLASSES:
//               balb::CommandLine: value-semantic command-line arguments
//       balb::CommandLineTypeInfo: type/constraint associated with an option
//     balb::CommandLineOptionInfo: information describing an option
//     balb::CommandLineOptionType: namespace for option types
//     balb::CommandLineConstraint: namespace for available constraints
// balb::CommandLineOccurrenceInfo: whether an option is required/optional
//         balb::CommandLineOption: constructible from "OptionInfo" w/allocator
//
//@SEE_ALSO: baea_configuration
//
//@AUTHOR: Ujjwal Bhoota (ubhoota), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides the ability to parse, validate, and
// access the command-line arguments of a program using the class
// 'balb::CommandLine'.  Its constructor takes a specification describing the
// command-line arguments.  Once created, 'printUsage' can be invoked to print
// the usage syntax.  The 'parse' method takes command-line arguments and
// validates them against the specification supplied at construction, printing
// suitable messages on an optionally-specified stream in case of a parsing
// error.  Once parsed, options and values can be accessed using various access
// methods.  The class provides a set of *theType* access methods (for example,
// 'theString', 'theInt') that return the value of the specified option name.
// It is also possible to link a variable with an option in the specification;
// doing so will cause the variable to be loaded with the option value once
// 'parse' has been invoked and was successful.  The 'data' method returns a
// 'bdem_ConstRowBinding' containing option names and their values.  A similar
// method, 'specifiedData', is suitable for overwriting other configuration
// parameters (possibly obtained from a configuration file; see also the
// 'overwrite' method of 'balb::Configuration').
//
///Component Features Summary
///--------------------------
// This component offers the following features:
//..
// (1) Validation of command-line arguments against the provided specification.
//
// (2) The ability to specify sophisticated constraints easily.  Users can also
//     build their own constraints and use them in the specification.
//
// (3) The ability to automatically generate usage syntax.
//
// (4) The ability to directly link a variable to an option.  After successful
//     parsing, all linked variables are loaded with their corresponding option
//     values.
//
// (5) The ability to access options and their corresponding values through
//     various accessor methods.
//
// (6) The ability to parse UNIX-style command lines (for example, grouping of
//     flags, allowing any ordering of options, or a mix between options and
//     non-options, short tag and long tag forms, etc.).
//
// (7) The ability to have multiple non-options of possibly different types.
//     Note that only the last non-option may be multi-valued, and that if a
//     non-option has a default value, then all subsequent non-options must
//     also have a default value.
//..
// Note that this component now supports multiple non-options of possibly
// different types.  It therefore can handle non-option values of different
// types, and can be made to appear to have restrictions on the number of
// non-option values passed to a multi-valued non-option (e.g., to always
// specify two or more values for a multi-valued non-option, simply create two
// single-valued non-options of the same type before the unrestricted
// multi-valued non-option).
//
///Background for UNIX-Style Command-Line Arguments and Definitions
///----------------------------------------------------------------
// This section provides background on UNIX-style command-line arguments, as
// well as definitions of terms used frequently in this documentation (such as
// "option", "flag", "non-option", "tag", "short tag", "long tag").  Readers
// familiar with UNIX command lines can skim this section or omit entirely.
//
// Command-line arguments can be classified as:
//..
//  * command name (there is only one, and it is always the first argument)
//  * options (and corresponding values)
//  * boolean options (also called flags)
//  * non-options
//..
// For example, in the following command line:
//..
//  $ bde_build.pl -e -c CC64 bde
//..
// the command name is 'bde_build.pl'.  There is one option, described by
// '-c CC64': 'c' is the tag name, and 'CC64' is the option value.  There is
// also one boolean option (flag): '-e' is a flag,  'e' is the flag name.  The
// last parameter, 'bde', is a non-option.
//
// Sometimes *option* is also used where "flag" or "non-option" would be more
// accurate.  What is actually intended should be clear from context.
//
// Each option or flag has an optional short tag and a mandatory long tag.  The
// short tag, if specified, must be a single alphabet symbol; the long tag
// generally must follow the same rules applicable to C/C++ identifiers, except
// that '-' is allowed (but not as the leading character).  When a short tag is
// used on a command line, it must be preceded by '-', and when a long tag is
// used it must be preceded by '--'.  Flags have no corresponding values; they
// are either present or absent.  Option tags *must* be followed by a
// corresponding option value.  An option can have multiple values (such
// options are called multi-valued options).  When multiple values are provided
// for an option, the tag must appear with each value (see the section
// "Multi-Valued-Options and How to Specify Them").  Arguments that are not the
// command name, options, or flags are called non-options and can be either
// single-valued or multi-valued.  They do not have any tag associated with
// them.
//
// Consider the syntax of a typical UNIX-style command whose options are
// described by the usage string:
//..
//  usage: mysort [-r|reverse] [-o|outputfile <outfile>] files...
//..
// Here:
//..
//  '-r|reverse' is a flag: 'r' is the short tag, 'reverse' is the long tag.
//
//  '-o|outputfile' is an option: 'o' is the short tag, 'outputfile' is the
//                  long tag.  The value is parsed from the string 'outfile'.
//
//  'files...' describe the multi-valued non-option.
//..
// The command can be invoked as follows:
//..
//  $ mysort -r -o myoutfile file1 file2 file3
//..
// and an equivalent command line is:
//..
//  $ mysort --reverse --outputfile myoutfile file1 file2 file3
//..
// Note that short tags must be prepended with '-' and long tags with '--'.  To
// specify a non-option beginning with '-', use a single '--' separator (not
// followed by a long option name).
//
///Specifying Option Values
///------------------------
// This component supports a variety of forms for specifying option values.
// They are best described by example.  Consider the command-line specification
// described by the following usage string:
//..
//  usage: myserver [-p|port <portNumber>]
//..
// The following (valid) forms can be used with equivalent meaning:
//..
//  $ myserver -p 13
//  $ myserver -p=13  # option value is '13' and *not* '=13'
//  $ myserver -p13
//  $ myserver --port 13
//  $ myserver --port=13
//..
// Note that if '=13' is desired as an option value, then whitespace must be
// used as in:
//..
//  $ myserver -p =13  # option value *is* '=13'
//..
// All of the following are invalid:
//..
//  $ myserver -port13
//  $ myserver --p 13
//  $ myserver -port 13
//..
///Flag Grouping
///-------------
// Flags can be grouped (i.e., expressed more succinctly like '-ctv' instead of
// '-c -t -v').  While grouping flags, short tags must be used.  For example,
// given the command-line specification described by the following usage
// string:
//..
//  usage: mysort [-r|reverse] [-i|insensitiveToCase] [-u|uniq]
//..
// the following command lines are valid and equivalent:
//..
//  $ mysort -r -i -u
//  $ cmd -riu
//  $ cmd -uir
//  $ cmd -i -ru
//..
// Note that the last character in a group need not be a flag; it could be an
// option.  Any character that is the short tag of an option signals the end of
// the flag group, and it must be followed by the value of the option.  For
// example, given the command-line specification described by the following
// usage string:
//..
//  usage: mysort [-r|reverse] [-i|insensitiveToCase] [-u|uniq]
//                [-o|outfile <outputfile>]
//..
// the following command lines are valid and equivalent:
//..
//  $ mysort -riu -o myoutfile
//  $ mysort -riuo myoutfile
//  $ mysort -riuomyoutfile
//  $ mysort -riuo=myoutfile
//..
//
///Multi-Valued Options and How to Specify Them
///--------------------------------------------
// Options can have several values.  For example, in the command-line
// specification described by the following usage string, '*' denotes a
// multi-valued option, and '+' denotes a multivalued option that must occur
// at least once.
//..
//  usage: mycompiler [-l|library <libName>]* [-o|out outFile] [<object>]+
//..
// multiple values can be given as follows:
//..
//  $ mycompiler -l lib1 -l lib2 -l lib3 -o main a.o b.o
//..
// They need not be supplied contiguously.  For example, the following command
// line is valid and equivalent to the above:
//..
//  $ mycompiler -l lib1 -o main -l lib2 -l lib3 a.o b.o
//..
// Note that the tag needs to be repeated for every option value.  For example,
// the following command line is invalid (because '-l' must be repeated before
// both 'lib2' and 'lib3'):
//..
//  $ mycompiler -l lib1 lib2 lib3 -o main a.o b.o
//..
// Short and long forms can be used in mixed fashion, however:
//..
//  $ mycompiler -l lib1 -o main --library lib2 -l lib3 a.o b.o
//..
///Order of Arguments
///------------------
// Command-line arguments can appear in any order.  For example, given the
// command-line specification described by the following usage string:
//..
//  usage: mysort [-r|reverse] [-o|outputfile <outfile>] [<file>]+
//..
// all the following command lines are valid:
//..
//  $ mysort -r -o myoutfile file1 file2 file3
//  $ mysort file1 file2 file3 -r -o myoutfile
//  $ mysort file1 -o myoutfile file2 -r file3
//..
// There are three exceptions to the above rule on argument order:
//..
// (1) An option tag must be followed by that option's value, if any (either in
//     the next argument, or in the same argument using "=value").
//
// (2) When a non-option starts with a '-' then it must not appear before
//     any option or flag *and* a '--' must be put on the command line to
//     indicate the end of all options and flags.
//
// (3) Non-options are parsed and assigned in the order they appear on the
//     command line.
//..
// For example, the following is invalid because of rule (1) above (because
// '-o' should be followed by 'myoutfile'):
//..
//  $ mysort -o -r myoutfile file1 file2 file3
//..
// and the following is incorrect because of rule (2) (because
// '-weirdfilename', which starts with '-', must appear after '--'):
//..
//  $ mysort -weirdfilename file2 file3 -r -o myoutfile
//..
// The previous examples can be corrected in either of the following ways:
//..
//  $ mysort -r -o myoutfile -- -weirdfilename file2 file3
//  $ mysort file2 file3 -r -o myoutfile -- -weirdfilename
//..
// Note that the order of values within the sequence of multi-valued
// non-options differs in both examples, as per rule (3).  In the first
// example, the non-options have for a value the (ordered) sequence:
//..
//  -weirdfilename file2 file3
//..
// while in the second example, the non-option value is the sequence:
//..
//  file2 file3 -weirdfilename
//..
// This order may or may not matter to the application.
//
///Specifying Command-Line Arguments
///---------------------------------
// A command line is described by an *option* *table* (supplied as an array of
// 'balb::CommandLineOptionInfo').  Each entry (row) of the table describes an
// option (i.e., an option, flag, or non-option).  Each entry has several
// fields, specified in the following order:
//..
//  Field name                     Main purpose (see below for more details)
//  ============================   ===========================================
//  tag field                      Specify tags (short and long) for options
//                                 and flags.  A non-option is indicated by an
//                                 empty string.
//
//  name field                     Specify the name through which the option
//                                 value can be accessed.
//
//  description field              Used in printing usage.
//
//  type-and-constraint field      (1) Specify the type of the option value(s).
//                                 (2) Specify a variable to be linked to the
//                                     option.
//                                 (3) Specify other constraints on individual
//                                     value(s).
//
//  occurrence information field   (1) Specify a default value for an option.
//                                 (2) Specify whether an option is required on
//                                     the command line or is optional (by
//                                     default, an option is optional).
//..
// The first three fields *must* be specified.  The type-and-constraint field
// can be omitted (meaning no constraint), and the occurrence information field
// likewise can be omitted (meaning that the option is not required on the
// command line).
//
// The following sections provide a more detailed description of each field,
// including example values for each field.
//
///Tag Field
///- - - - -
// The tag field specifies the (optional) short tag and long tag for the
// corresponding option or flag, except that non-options are indicated by an
// empty string for a tag field.  There can only be one multi-valued entry for
// non-option arguments, and it must be listed last among the non-options.
//
// The general format is either: (1) "" (empty string) for non-options;
// (2) "<s>|<long>" for options and flags, where '<s>' is the short tag, and
// '<long>' is the long tag; or (3) "<long>" for options and flags where a
// short tag is not specified.  Note that for short tags ('<s>'), 's' must be
// a single character (different from '-' and '|'); for long tags ("<long>"),
// 'long' must have 2 or more characters (which may contain '-', except as the
// first character, but cannot contain '|').  Also note that either no tag
// (empty string), both short and long tags, or only a long tag, may be
// specified.
//
// The tag field cannot be omitted, but it can be the empty string.
//
///Name Field
/// - - - - -
// The name field specifies the name through which the option value can be
// accessed either through one of the *theType* methods or through the binding
// returned by the 'data' method.
//
// The general format is any non-empty string.  In most cases, the name will be
// used as-is.  Note that any suffix starting at the first occurrence of '=',
// if any, is removed from the name before storing in the
// 'balb::CommandLineOptionInfo'.  Thus, if a name having such a suffix is
// specified in a 'balb::CommandLineOptionInfo' (e.g.,
// "nameOption=someAttribute"), the correct name to use for querying this
// option by name (e.g., through the 'data' binding) does not include the
// suffix (e.g., 'cmdLine.numSpecified("nameOption=someAttribute")' will always
// return 0, but 'cmdLine.numSpecified("nameOption")' will return the
// appropriate value).
//
// This field cannot be omitted, and it cannot be an empty string.
//
///Description Field
///- - - - - - - - -
// The description field is used when printing the usage string.
//
// The general format is any non-empty string.
//
// This field cannot be omitted, and it cannot be an empty string.
//
///Type-and-Constraint Field
///- - - - - - - - - - - - -
// The type-and-constraint field specifies the type and constraints for the
// option values.  Flags are identified by having the boolean type; note that
// flags cannot have constraints.  Multiple values (for multi-valued options
// and multi-valued non-options) can be specified by using array types.  The
// list of the supported types is provided in the section "Supported Types"
// below.
//
// Other constraints can be placed on individual value(s).  When the type is an
// array type, then those constraints are placed on the individual value(s)
// held in the array and not on the entire array.  A list of useful constraints
// is provided in the section "Supported Constraint Values".  Also see the
// section "Building New Constraints" to see how new constraints can be built
// so that they can be used in the same manner as the available constraints.
//
// Additionally, this field allows a specified variable to be linked to the
// option.  In that case, the variable is automatically loaded with the option
// value after parsing.
//
// The general format can be one of either:
//..
//  balb::CommandLineTypeInfo(&variable)
//  balb::CommandLineTypeInfo(&variable, constraint)
//      // Link the option with the specified 'variable'.  Note that the option
//      // type is inferred from the type of 'variable'.  Optionally place the
//      // user-specified 'constraint', of a type defined in
//      // 'balb::CommandLineConstraint', on the value.
//
//  balb::CommandLineTypeInfo(type)
//  balb::CommandLineTypeInfo(type, constraint)
//      // Specify the type of this option to be of the specified 'type'.
//      // Optionally place the user-specified 'constraint', of a type defined
//      // in 'balb::CommandLineConstraint', on the value.  Don't link this
//      // option with any variable.  'type' must be one of the static
//      // variables listed in 'balb::CommandLineOptionType'.
//..
// This field can be omitted.  If so, the type is assumed to be of string type
// with no constraints and no variable is linked to the option.  No occurrence
// information field can then be specified; if such a field is desired, then
// the type-and-constraint field needs to be set explicitly.
//
///Occurrence Information Field
/// - - - - - - - - - - - - - -
// The occurrence information field is used to specify a default value for an
// option, and whether an option is required on the command line or is
// optional.  An option may also be "hidden" (i.e., not displayed by
// 'printUsage').
//
// The general format of this field is one of the following:
//..
//  balb::CommandLineOccurrenceInfo::BAEA_HIDDEN
//  balb::CommandLineOccurrenceInfo::BAEA_OPTIONAL
//  balb::CommandLineOccurrenceInfo::BAEA_REQUIRED
//  a default value
//..
// If a default value is specified, the option is assumed to be optional; in
// addition, the default value must satisfy the type and constraint indicated
// by the specified type-and-constraint field.
//
// This field can be omitted, and is always omitted if the type-and-constraint
// field is not specified.  If omitted, the option is not required on the
// command line and has no default value; furthermore, if the option is not
// present on the command line, the linked variable, if any, is uneffected.
//
///Example Field Values
///--------------------
// The following tables give examples of field values.
//
///Tag Field
///- - - - -
// The tag field may be declared using the following forms:
//..
//     Usage                              Meaning
//  ==============    =======================================================
//  "o|outputfile"    The option being defined is either an option or a flag.
//                    The short tag is 'o' and the long tag is 'outputfile'.
//
//  "outputfile"      The option being defined is either an option or a flag.
//                    There is no short tag and the long tag is 'outputfile'.
//
//      ""            The entry is the only non-option(s)
//..
// Note that "o" alone is not valid because a long tag *must* be specified.
//
///Name Field
/// - - - - -
// The name field may be declared using the following value:
//..
//      Usage                              Meaning
//  ===============   =======================================================
//     "xyz"          The option value can be accessed by "xyz".
//..
///Type-and-Constraint Field
///- - - - - - - - - - - - -
// Suppose, for example, that our application has the following parameters:
//..
//  int                      portNum;   // a variable to be linked to an
//                                      // option
//
//  bool                     isVerbose; // a variable to be linked to a flag
//
//  bsl::vector<bsl::string> fileNames; // a variable to be linked to a
//                                      // multi-valued option
//..
// The type and constraint fields may be declared using the following values:
//..
//     Usage                               Meaning
//  ===============   =======================================================
//  balb::CommandLineTypeInfo(&portNum)
//                    (1) Link the variable 'portNum' with this option value.
//                    That is, after successful parsing, this variable will
//                    contain the option value specified on the command line.
//                    (2) An integer value must be provided on the command
//                    line for this option (the type 'int' is inferred
//                    implicitly from the type of 'portNum').
//
//  balb::CommandLineTypeInfo(balb::CommandLineOptionType::INT)
//                    This option value must be an integer.
//
//  balb::CommandLineTypeInfo(&isVerbose)
//                    Load the variable 'isVerbose' with this option value.
//
//  balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
//                    This option is a flag.
//
//  balb::CommandLineTypeInfo(&fileNames)
//                    Load the variable 'fileNames' with the values specified
//                    for this multi-valued option (or multi-valued
//                    non-option).  That is, after successful parsing, this
//                    variable will contain the sequence of values specified
//                    on the command line, in the same order.
//
//  balb::CommandLineTypeInfo(balb::CommandLineOptionType::STRING_ARRAY)
//                    This option value consists of a sequence of string
//                    values specified on the command line, in the same order.
//..
///Occurrence Information Field
/// - - - - - - - - - - - - - -
// The following values may be used for this field:
//..
//     Usage                               Meaning
//  ===============   ========================================================
//  balb::CommandLineOccurrenceInfo::BAEA_REQUIRED
//                    The value(s) for this option must be provided on the
//                    command line.  For options that are of an array type, at
//                    least one value must be provided.
//
//  balb::CommandLineOccurrenceInfo::BAEA_OPTIONAL
//                    Value(s) for this option can be omitted on the command
//                    line.
//
//  balb::CommandLineOccurrenceInfo::BAEA_HIDDEN
//                    Same as 'BAEA_OPTIONAL'; in addition, this option will
//                    not be displayed by 'printUsage'.
//
//  13                The default value for this option is 13 and the option
//                    is not required on the command line.  If no value is
//                    provided, then 13 is used.  If the type described by the
//                    type-and-constraint field is not integer, then it is an
//                    error.
//..
// *Note*: If an option is optional AND no value is provided on the command
// line, then it will be unset in the bindings returned by either 'data' or
// 'specifiedData'.  In addition, if a variable was linked to this option, it
// will be unmodified after parsing.
//
///Supported Types
///---------------
// The following types are supported.  The standard BDE format for 'bdem' types
// is accepted (see the package-level documentation of 'bdepu' for formatting
// details).  The type is specified by an enumeration value supplied as the
// first argument to:
//..
//  balb::CommandLineTypeInfo(type, constraint)
//..
// which is used to create the type-and-constraint field value in the
// command-line specification:
//..
//  Type                        Specifier
//  -------------------------   ----------------------------------------
//  bool                        balb::CommandLineOptionType::BAEA_BOOL
//  char                        balb::CommandLineOptionType::BAEA_CHAR
//  short                       balb::CommandLineOptionType::BAEA_SHORT
//  int                         balb::CommandLineOptionType::BAEA_INT
//  bsls::Types::Int64          balb::CommandLineOptionType::BAEA_INT64
//  float                       balb::CommandLineOptionType::BAEA_FLOAT
//  double                      balb::CommandLineOptionType::BAEA_DOUBLE
//  bsl::string                 balb::CommandLineOptionType::BAEA_STRING
//  bdlt::Datetime               balb::CommandLineOptionType::BAEA_DATETIME
//  bdlt::Date                   balb::CommandLineOptionType::BAEA_DATE
//  bdlt::Time                   balb::CommandLineOptionType::BAEA_TIME
//  bsl::vector<char>           balb::CommandLineOptionType::BAEA_CHAR_ARRAY
//  bsl::vector<short>          balb::CommandLineOptionType::BAEA_SHORT_ARRAY
//  bsl::vector<int>            balb::CommandLineOptionType::BAEA_INT_ARRAY
//  bsl::vector<bsls::Types::Int64>
//                              balb::CommandLineOptionType::BAEA_INT64_ARRAY
//  bsl::vector<float>          balb::CommandLineOptionType::BAEA_FLOAT_ARRAY
//  bsl::vector<double>         balb::CommandLineOptionType::BAEA_DOUBLE_ARRAY
//  bsl::vector<bsl::string>    balb::CommandLineOptionType::BAEA_STRING_ARRAY
//  bsl::vector<bdlt::Datetime>  balb::CommandLineOptionType::BAEA_DATETIME_ARRAY
//  bsl::vector<bdlt::Date>      balb::CommandLineOptionType::BAEA_DATE_ARRAY
//  bsl::vector<bdlt::Time>      balb::CommandLineOptionType::BAEA_TIME_ARRAY
//..
///Supported Constraint Values
///---------------------------
// This component supports constraint values for each type.  Specifically, the
// utility 'struct' 'balb::CommandLineConstraint' defines 'TYPEConstraint' types
// (for instance, 'StringConstraint', 'IntConstraint') that can be used to
// define a constraint suitable for the 'balb::CommandLineTypeInfo' class.
//
///Building New Constraints
///------------------------
// A constraint is simply a function object that takes as its first argument
// the (address of the) data to be constrained and as its second argument the
// stream that should be written to with an appropriate error message when the
// data does not follow the constraint.  The functor should return a 'bool'
// value indicating whether or not the data abides by the constraint (with
// 'true' indicating success).  A constraint for a given option whose value has
// the given type must be convertible to one of the 'TYPEConstraint' types
// defined in the utility 'struct' 'balb::CommandLineConstraint'.  Note that
// when passing a function as a constraint, the *address* of the function must
// be passed.
//
///Usage
///-----
// Suppose we want to design a sorting utility named 'mysort' that has the
// following syntax:
//..
//  usage: mysort  [-r|reverse] [-i|insensitivetocase] [-u|uniq]
//                 [-a|algorithm sortAlgo] -o|outputfile <outputFile>
//                 [<file>]*
//                            // Sort the specified files (in 'fileList'),
//                            // using the specified sorting algorithm and
//                            // write the output to the specified output file.
//
//     option                               note
//  ============  ====================================================
//  -a|algorithm  (1) Value (provided on command line) of this option must
//                    be one among "quickSort", "insertionSort", "shellSort".
//                (2) If not provided, default value will be "quickSort".
//
//  -o|outfile    (1) This option must not be omitted on command line.
//..
// We choose the non-option to be an array of 'bsl::string' so as to
// accommodate multiple files.
//
// These options may be used incorrectly, as the following examples show:
//..
//             INCORRECT USE                        REASON FOR INACCURACY
//  ===========================================  ============================
//  $ mysort -riu -o myofile -aDUMBSORT f1 f2    Incorrect because 'DUMBSORT'
//                                               is not among valid values
//                                               for the -a option.
//
//  $ mysort -riu f1 f2                          Incorrect because no value
//                                               is provided for the -o option.
//..
// In order to enforce the constraint on the sorting algorithms that are
// supported, our application provides the following free function:
//..
//  bool isValidAlgorithm(const bsl::string *algo, bsl::ostream& stream)
//      // Return 'true' if the specified 'algo' is one among "quickSort",
//      // "insertionSort", and "shellSort"; otherwise, output to 'stream' an
//      // appropriate error message and return 'false'.
//  {
//      if ("quickSort" == *algo || "insertionSort" == *algo
//       || "shellSort" == *algo) {
//          return true;
//      }
//      stream << "Error: sorting algorithm must be either "
//                "'quickSort', 'insertionSort', or 'shellSort'.\n";
//      return false;
//  }
//..
// Using this function, we can now use a 'balb::CommandLine' object to parse
// command-line options.  The proper usage is shown below.  First we declare
// the variables to be linked to the options.  If they are needed at global
// scope, we could declare them as global variables, but we prefer to declare
// them as local variables inside 'main':
//..
//  int main(int argc, const char *argv[]) {
//..
// Note that it is important that variables that will be bound to optional
// command line arguments be initialized to their default value, otherwise
// their value will unspecified if a value isn't provided on the command line
// (unless a default is specified via 'balb::CommandLineOccurrenceInfo'):
//..
//      bool isReverse = false;
//      bool isCaseInsensitive = false;
//      bool isUniq = false;
//
//      bsl::string outFile;
//      bsl::string sortAlgo;
//
//      bsl::vector<bsl::string> files;
//..
// Next, we build up an option specification table as follows:
//..
//      // build constraint for sortAlgo option
//      balb::CommandLineConstraint::StringConstraint validAlgoConstraint;
//      validAlgoConstraint = &isValidAlgorithm;
//
//      // option specification table
//      balb::CommandLineOptionInfo specTable[] = {
//        {
//          "r|reverse",                                 // tag
//          "isReverse",                                 // name
//          "sort in reverse order",                     // description
//          balb::CommandLineTypeInfo(&isReverse)         // link
//        },
//        {
//          "i|insensitivetocase",                       // tag
//          "isCaseInsensitive",                         // name
//          "be case insensitive while sorting",         // description
//          balb::CommandLineTypeInfo(&isCaseInsensitive) // link
//        },
//        {
//          "u|uniq",                                    // tag
//          "isUniq",                                    // name
//          "discard duplicate lines",                   // description
//          balb::CommandLineTypeInfo(&isUniq)            // link
//        },
//        {
//          "a|algorithm",                               // tag
//          "sortAlgo",                                  // name
//          "sorting algorithm",                         // description
//          balb::CommandLineTypeInfo(&sortAlgo, validAlgoConstraint),
//                                                       // link and constraint
//          balb::CommandLineOccurrenceInfo(bsl::string("quickSort"))
//                                                       // default algorithm
//        },
//        {
//          "o|outputfile",                              // tag
//          "outputFile",                                // name
//          "output file",                               // description
//          balb::CommandLineTypeInfo(&outFile),          // link
//          balb::CommandLineOccurrenceInfo::BAEA_REQUIRED// occurrence info
//        },
//        {
//          "",                                          // non-option
//          "fileList",                                  // name
//          "files to be sorted",                        // description
//          balb::CommandLineTypeInfo(&files)             // link
//        }
//      };
//..
// We can now create a command-line specification and parse the command-line
// options:
//..
//      // create command-line specification
//      balb::CommandLine cmdLine(specTable);
//
//      // parse command-line options; if failure, print usage
//      if (cmdLine.parse(argc, argv)) {
//          cmdLine.printUsage();
//          return -1;
//      }
//..
// Upon successful parsing, the 'cmdLine' object will acquire a value which
// conforms to the specified constraints.  We can examine these values as
// follows:
//..
//      // if successful, obtain command-line option values
//      bdem_ConstRowBinding binding = cmdLine.data();
//
//      // access through linked variable
//      bsl::cout << outFile << bsl::endl;
//
//      // access through *theType* methods
//      assert(cmdLine.theString("outputFile") == outFile);
//
//      // access through binding
//      assert(binding.theString("outputFile") == outFile);
//
//      // check that required option has been specified once
//      assert(cmdLine.isSpecified("outputFile"));
//
//      int count = -1;
//      assert(cmdLine.isSpecified("outputFile", &count));
//      assert(1 == count);
//
//      return 0;
//  }
//..
// For instance, the following command lines:
//..
//  $ mysort -omyofile f1 f2 f3
//  $ mysort -ainsertionSort f1 f2 f3 -riu -o myofile outputFile
//  $ mysort --algorithm insertionSort --outputfile myofile f1 f2 f3 --uniq
//..
// will all produce the same output on 'stdout'.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_BINDING
#include <bdlmxxx_binding.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_LIST
#include <bdlmxxx_list.h>
#endif

#ifndef INCLUDED_BDLMXXX_SCHEMA
#include <bdlmxxx_schema.h>
#endif

#ifndef INCLUDED_BDLF_FUNCTION
#include <bdlf_function.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#ifdef BSLS_PLATFORM_OS_WINDOWS
#ifdef OPTIONAL
#undef OPTIONAL
#endif
#endif
#endif // BDE_OMIT_INTERNAL_DEPRECATED



namespace BloombergLP {



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close namespace bdet


namespace balb {class CommandLine_Constraint;

                      // ===============================
                      // class CommandLineOccurrenceInfo
                      // ===============================

class CommandLineOccurrenceInfo {
    // This 'class' is a simple attribute class that describes a (potential)
    // default value for an option, and whether the option is required,
    // optional, or hidden.
    //
    // More generally, this class supports a complete set of *in*-*core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, and 'ostream' printing.  (A precise operational
    // definition of when two instances have the same value can be found in the
    // description of 'operator==' for the class).  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bool      d_isRequired;    // 'true' if option is required

    bool      d_isHidden;      // 'true' if option is hidden

    bdlmxxx::List d_defaultValue;  // single-element list storing default value
                               // (if any), empty otherwise

  public:
    // TYPES
    enum OccurrenceType {
        BAEA_REQUIRED = 0,  // option is required
        BAEA_OPTIONAL = 1,  // option is optional
        BAEA_HIDDEN   = 2   // option is optional and hidden

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , REQUIRED = BAEA_REQUIRED
      , OPTIONAL = BAEA_OPTIONAL
      , HIDDEN   = BAEA_HIDDEN
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(CommandLineOccurrenceInfo,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    CommandLineOccurrenceInfo(bslma::Allocator *basicAllocator = 0);
        // Construct an object such that the associated option is optional and
        // has no default value.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    CommandLineOccurrenceInfo(OccurrenceType    type,
                                   bslma::Allocator *basicAllocator = 0);
        // Construct an object such that the associated option has the
        // specified 'type' as its occurrence type and has no default value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit
    CommandLineOccurrenceInfo(char              defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(short             defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(int               defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                                 bsls::Types::Int64  defaultValue,
                                 bslma::Allocator   *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(float             defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(double            defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(const bsl::string&  defaultValue,
                                   bslma::Allocator   *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(const bdlt::Datetime&  defaultValue,
                                   bslma::Allocator     *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(const bdlt::Date&  defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(const bdlt::Time&  defaultValue,
                                   bslma::Allocator *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                                 const bsl::vector<char>&  defaultValue,
                                 bslma::Allocator         *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                                const bsl::vector<short>&  defaultValue,
                                bslma::Allocator          *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                                  const bsl::vector<int>&  defaultValue,
                                  bslma::Allocator        *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                   const bsl::vector<bsls::Types::Int64>&  defaultValue,
                   bslma::Allocator                       *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                                const bsl::vector<float>&  defaultValue,
                                bslma::Allocator          *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                               const bsl::vector<double>&  defaultValue,
                               bslma::Allocator           *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                          const bsl::vector<bsl::string>&  defaultValue,
                          bslma::Allocator                *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                        const bsl::vector<bdlt::Datetime>&  defaultValue,
                        bslma::Allocator                  *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                            const bsl::vector<bdlt::Date>&  defaultValue,
                            bslma::Allocator              *basicAllocator = 0);
    explicit
    CommandLineOccurrenceInfo(
                            const bsl::vector<bdlt::Time>&  defaultValue,
                            bslma::Allocator              *basicAllocator = 0);
        // Construct an object such that the associated option is optional and
        // has the specified 'defaultValue'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    CommandLineOccurrenceInfo(
                    const CommandLineOccurrenceInfo&  original,
                    bslma::Allocator                      *basicAllocator = 0);
        // Construct a 'CommandLineOccurrenceInfo' object having the value
        // of the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~CommandLineOccurrenceInfo();
        // Destroy this object.

    // MANIPULATORS
    CommandLineOccurrenceInfo& operator=(
                                    const CommandLineOccurrenceInfo& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    void setDefaultValue(const bdlmxxx::ConstElemRef& defaultValue);
        // Set the type and default value of the associated option to the
        // specified 'defaultValue'.  The behavior is undefined unless the
        // associated option is optional and 'defaultValue' is bound to
        // a value whose type matches that of one of the
        // 'CommandLineOptionType' members (except 'BAEA_BOOL').

    void setHidden();
        // Set the associated option to be hidden.  The behavior is undefined
        // unless the option is optional.

    // ACCESSORS
    bdlmxxx::ConstElemRef defaultValue() const;
        // Return a 'bdem' reference to the non-modifiable default value of
        // this object.  The behavior is undefined unless this object has a
        // default value.

    bool hasDefaultValue() const;
        // Return 'true' if this object has a default value, and 'false'
        // otherwise.

    bool isHidden() const;
        // Return 'true' if this option is hidden (i.e., not printed in the
        // usage string), and 'false' otherwise.

    bool isRequired() const;
        // Return 'true' if this option is required, and 'false' otherwise.

    OccurrenceType occurrenceType() const;
        // Return the occurrence type of this option (i.e., required, optional,
        // or hidden).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this object.  If 'level' is negative, suppress indentation of the
        // first line.  If 'stream' is not valid on entry, this operation has
        // no effect.  The behavior is undefined if 'spacesPerLevel' is
        // negative.
};

// FREE OPERATORS
bool operator==(const CommandLineOccurrenceInfo& lhs,
                const CommandLineOccurrenceInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two objects of type 'CommandLineOccurrenceInfo'
    // have the same value if and only if they have the same occurrence type
    // and either both do not have a default value, or their respective
    // default values have the same type and value.

bool operator!=(const CommandLineOccurrenceInfo& lhs,
                const CommandLineOccurrenceInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two objects of type
    // 'CommandLineOccurrenceInfo' do not have the same value if and only
    // if they have different occurrence types, or exactly one has a default
    // value, or else both have a default value but their respective default
    // values have either different types or different values.

bsl::ostream&
operator<<(bsl::ostream& stream, const CommandLineOccurrenceInfo& rhs);
    // Write the value of the specified 'rhs' object to the specified 'stream'
    // in a (multi-line) human readable format and return a reference to the
    // modifiable 'stream'.  Note that the last line is *not* terminated by a
    // newline character.

// BACKWARD COMPATIBILITY
typedef CommandLineOccurrenceInfo CommandLineOccuranceInfo;
    // This class describes a (potential) default value for an option, and
    // whether the option is required or optional.
    //
    // DEPRECATED: Use 'CommandLineOccurrenceInfo' instead.

                        // ============================
                        // struct CommandLineOptionType
                        // ============================

struct CommandLineOptionType {
    // This 'struct' provides a namespace for the various option types.  The
    // following static variables are used to describe the type of an option.
    // The only way they should be used is as the first argument to the
    // 'CommandLineTypeInfo' constructor.  For example, when 'STRING'
    // is supplied as the first argument to the 'CommandLineTypeInfo'
    // constructor, the type of the option is inferred to be 'bsl::string'.

    // CLASS DATA
    static bool                       * const BAEA_BOOL;
    static char                       * const BAEA_CHAR;
    static short                      * const BAEA_SHORT;
    static int                        * const BAEA_INT;
    static bsls::Types::Int64         * const BAEA_INT64;
    static float                      * const BAEA_FLOAT;
    static double                     * const BAEA_DOUBLE;
    static bsl::string                * const BAEA_STRING;
    static bdlt::Datetime              * const BAEA_DATETIME;
    static bdlt::Date                  * const BAEA_DATE;
    static bdlt::Time                  * const BAEA_TIME;
    static bsl::vector<char>          * const BAEA_CHAR_ARRAY;
    static bsl::vector<short>         * const BAEA_SHORT_ARRAY;
    static bsl::vector<int>           * const BAEA_INT_ARRAY;
    static bsl::vector<bsls::Types::Int64>
                                      * const BAEA_INT64_ARRAY;
    static bsl::vector<float>         * const BAEA_FLOAT_ARRAY;
    static bsl::vector<double>        * const BAEA_DOUBLE_ARRAY;
    static bsl::vector<bsl::string>   * const BAEA_STRING_ARRAY;
    static bsl::vector<bdlt::Datetime> * const BAEA_DATETIME_ARRAY;
    static bsl::vector<bdlt::Date>     * const BAEA_DATE_ARRAY;
    static bsl::vector<bdlt::Time>     * const BAEA_TIME_ARRAY;
};

                        // ============================
                        // struct CommandLineConstraint
                        // ============================

struct CommandLineConstraint {
    // This 'struct' provides a namespace for a set of useful constraints for
    // each of the scalar 'bdem' element types used by this component (except
    // for 'BDEM_BOOL').

    // TYPES
    typedef bdlf::Function<bool (*)(const char *,
                                   bsl::ostream&)>    CharConstraint;
        // Functor type constraining a value of type 'char'.

    typedef bdlf::Function<bool (*)(const short *,
                                   bsl::ostream&)>    ShortConstraint;
        // Functor type constraining a value of type 'short'.

    typedef bdlf::Function<bool (*)(const int *,
                                   bsl::ostream&)>    IntConstraint;
        // Functor type constraining an integer.

    typedef bdlf::Function<bool (*)(const bsls::Types::Int64 *,
                                   bsl::ostream&)>    Int64Constraint;
        // Functor type constraining a 64-bit integer.

    typedef bdlf::Function<bool (*)(const float *,
                                   bsl::ostream&)>    FloatConstraint;
        // Functor type constraining a value of type 'float'.

    typedef bdlf::Function<bool (*)(const double *,
                                   bsl::ostream&)>    DoubleConstraint;
        // Functor type constraining a value of type 'double'.

    typedef bdlf::Function<bool (*)(const bsl::string *,
                                   bsl::ostream&)>    StringConstraint;
        // Functor type constraining a string.

    typedef bdlf::Function<bool (*)(const bdlt::Datetime *,
                                   bsl::ostream&)>    DatetimeConstraint;
        // Functor type constraining a value of type 'bdlt::Datetime'.

    typedef bdlf::Function<bool (*)(const bdlt::Date *,
                                   bsl::ostream&)>    DateConstraint;
        // Functor type constraining a value of type 'bdlt::Date'.

    typedef bdlf::Function<bool (*)(const bdlt::Time *,
                                   bsl::ostream&)>    TimeConstraint;
        // Functor type constraining a value of type 'bdlt::Time'.
};

                         // =========================
                         // class CommandLineTypeInfo
                         // =========================

class CommandLineTypeInfo {
    // This 'class' is a simple attribute class that describes the type, the
    // variable to be linked, and the constraint on an option.  Note that the
    // constraint type is opaque, but it is possible to apply the constraint to
    // an element of the same type as the option and see whether it is valid
    // (using the 'checkConstraint' method).
    //
    // More generally, this class supports a complete set of *in*-*core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, and 'ostream' printing.  (A precise operational
    // definition of when two instances have the same value can be found in the
    // description of 'operator==' for the class).  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bdlmxxx::ElemType::Type  d_elemType;          // type of the option

    void                *d_linkedVariable_p;  // variable to be linked (held)

    bsl::shared_ptr<CommandLine_Constraint>
                         d_constraint_p;      // constraint on the option value

    bslma::Allocator    *d_allocator_p;       // memory allocator (held)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(CommandLineTypeInfo,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    CommandLineTypeInfo(bslma::Allocator *basicAllocator = 0);
        // Construct an object having 'string' type for the associated option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  No variable is linked and no constraint is put on the value.

    explicit
    CommandLineTypeInfo(bool             *variable,
                             bslma::Allocator *basicAllocator = 0);
        // Construct a flag accepting 'bool' as the type for the associated
        // option (i.e., for the linked variable).  If the specified 'variable'
        // is not 0, then link it with the option.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  No constraint
        // can be put on the option.

    explicit
    CommandLineTypeInfo(char                         *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(char                         *variable,
                             const CommandLineConstraint::CharConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'char' type for the associated option.
        // If the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option initially has no
        // constraint.

    explicit
    CommandLineTypeInfo(short                        *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(short                        *variable,
                             const CommandLineConstraint::ShortConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'short' type for the associated option.
        // If the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option initially has no
        // constraint.

    explicit
    CommandLineTypeInfo(int                          *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(int                          *variable,
                             const CommandLineConstraint::IntConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'int' type for the associated option.  If
        // the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option initially has no
        // constraint.

    explicit
    CommandLineTypeInfo(bsls::Types::Int64    *variable,
                             bslma::Allocator      *basicAllocator = 0);
    CommandLineTypeInfo(bsls::Types::Int64    *variable,
                             const CommandLineConstraint::
                                         Int64Constraint&  constraint,
                             bslma::Allocator      *basicAllocator = 0);
        // Construct an object having 'bsls::Types::Int64' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(float                        *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(float                        *variable,
                             const CommandLineConstraint::
                                         FloatConstraint&  constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'float' type for the associated option.
        // If the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option initially has no
        // constraint.

    explicit
    CommandLineTypeInfo(double                       *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(double                       *variable,
                             const CommandLineConstraint::
                                        DoubleConstraint&  constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'double' type for the associated option.
        // If the specified 'variable' is not 0, then link it with the option.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Optionally specify a 'constraint' to put on the option.  If
        // 'constraint' is not specified, the option initially has no
        // constraint.

    explicit
    CommandLineTypeInfo(bsl::string                  *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::string                  *variable,
                             const CommandLineConstraint::
                                        StringConstraint&  constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::string' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with
        // the option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option initially has
        // no constraint.

    explicit
    CommandLineTypeInfo(bdlt::Datetime                *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bdlt::Datetime                *variable,
                             const CommandLineConstraint::
                                      DatetimeConstraint&  constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bdlt::Datetime' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with
        // the option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option initially has
        // no constraint.

    explicit
    CommandLineTypeInfo(bdlt::Date                    *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bdlt::Date                    *variable,
                             const CommandLineConstraint::DateConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bdlt::Date' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with
        // the option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option initially has
        // no constraint.

    explicit
    CommandLineTypeInfo(bdlt::Time                    *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bdlt::Time                    *variable,
                             const CommandLineConstraint::TimeConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bdlt::Time' type for the associated
        // option.  If the specified 'variable' is not 0, then link it with
        // the option.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Optionally specify a 'constraint' to put on the
        // option.  If 'constraint' is not specified, the option initially has
        // no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<char>            *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<char>            *variable,
                             const CommandLineConstraint::CharConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<char>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<short>           *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<short>           *variable,
                             const CommandLineConstraint::ShortConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<short>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<int>             *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<int>             *variable,
                             const CommandLineConstraint::IntConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<int>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(
                   bsl::vector<bsls::Types::Int64> *variable,
                   bslma::Allocator                *basicAllocator = 0);
    CommandLineTypeInfo(
                   bsl::vector<bsls::Types::Int64> *variable,
                   const CommandLineConstraint::Int64Constraint&
                                                    constraint,
                   bslma::Allocator                *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bsls::Types::Int64>'
        // type for the associated option.  If the specified 'variable' is not
        // 0, then link it with the option.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Optionally
        // specify a 'constraint' to put on the option.  If 'constraint' is not
        // specified, the option initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<float>           *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<float>           *variable,
                             const CommandLineConstraint::FloatConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<float>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<double>       *variable,
                             bslma::Allocator          *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<double>       *variable,
                             const CommandLineConstraint::
                                     DoubleConstraint&  constraint,
                             bslma::Allocator          *basicAllocator = 0);
        // Construct an object having 'bsl::vector<double>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<bsl::string>     *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<bsl::string>     *variable,
                             const CommandLineConstraint::
                                        StringConstraint&  constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bsl::string>' type for
        // the associated option.  If the specified 'variable' is not 0, then
        // link it with the option.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Optionally specify a
        // 'constraint' to put on the option.  If 'constraint' is not
        // specified, the option initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<bdlt::Datetime>   *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<bdlt::Datetime>   *variable,
                             const CommandLineConstraint::
                                      DatetimeConstraint&  constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bdlt::Datetime>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<bdlt::Date>       *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(bsl::vector<bdlt::Date>       *variable,
                             const CommandLineConstraint::DateConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bdlt::Date>' type for the
        // associated option.  If the specified 'variable' is not 0, then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    explicit
    CommandLineTypeInfo(bsl::vector<bdlt::Time>       *variable,
                             bslma::Allocator             *basicAllocator = 0);
    CommandLineTypeInfo(
                             bsl::vector<bdlt::Time>       *variable,
                             const CommandLineConstraint::TimeConstraint&
                                                           constraint,
                             bslma::Allocator             *basicAllocator = 0);
        // Construct an object having 'bsl::vector<bdlt::Time>' type for the
        // associated option.  If the specified 'variable' is not 0,  then link
        // it with the option.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify a 'constraint' to put
        // on the option.  If 'constraint' is not specified, the option
        // initially has no constraint.

    CommandLineTypeInfo(
                          const CommandLineTypeInfo&  original,
                          bslma::Allocator                *basicAllocator = 0);
        // Create an object having the value of the specified 'original'
        // object.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Note that this object shares the same constraint
        // as the 'original' object.

    ~CommandLineTypeInfo();
        // Destroy this object.

    // MANIPULATORS
    CommandLineTypeInfo& operator=(const CommandLineTypeInfo& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference to this modifiable object.

    void resetConstraint();
        // Reset this object so that it no longer has a constraint associated
        // with it.  This method has no effect if there was no constraint
        // associated this object before the call.

    void resetLinkedVariable();
        // Reset this object so that it no longer has a linked variable
        // associated with it.  This method has no effect if there was no
        // linked variable associated this object before the call.

    void setConstraint(
             const CommandLineConstraint::CharConstraint&     constraint);
    void setConstraint(
             const CommandLineConstraint::ShortConstraint&    constraint);
    void setConstraint(
             const CommandLineConstraint::IntConstraint&      constraint);
    void setConstraint(
             const CommandLineConstraint::Int64Constraint&    constraint);
    void setConstraint(
             const CommandLineConstraint::FloatConstraint&    constraint);
    void setConstraint(
             const CommandLineConstraint::DoubleConstraint&   constraint);
    void setConstraint(
             const CommandLineConstraint::StringConstraint&   constraint);
    void setConstraint(
             const CommandLineConstraint::DatetimeConstraint& constraint);
    void setConstraint(
             const CommandLineConstraint::DateConstraint&     constraint);
    void setConstraint(
             const CommandLineConstraint::TimeConstraint&     constraint);
        // Put the specified 'constraint' on the value of this option,
        // replacing any constraint that had been in effect (if any).  The
        // behavior is undefined unless this option is not a flag and the
        // 'bdem' element type of this option corresponds to the type of
        // 'constraint'.  Note that two distinct objects that have the same
        // 'constraint' put on them will compare *unequal* unless the
        // constraint is shared among them, which can be done by:
        //..
        //  aTypeInfo.setConstraint(anotherTypeInfo.constraint());
        //..

    void
    setConstraint(
               const bsl::shared_ptr<CommandLine_Constraint>& constraint);
        // Set the constraint of this option to the specified 'constraint'.
        // The behavior is undefined unless the option associated with
        // 'constraint' has the same type as the option associated with this
        // object.  Note that the linked variable, if any, is unchanged by this
        // method.

    void setLinkedVariable(bool                                  *variable);
    void setLinkedVariable(char                                  *variable);
    void setLinkedVariable(short                                 *variable);
    void setLinkedVariable(int                                   *variable);
    void setLinkedVariable(bsls::Types::Int64                    *variable);
    void setLinkedVariable(float                                 *variable);
    void setLinkedVariable(double                                *variable);
    void setLinkedVariable(bsl::string                           *variable);
    void setLinkedVariable(bdlt::Datetime                         *variable);
    void setLinkedVariable(bdlt::Date                             *variable);
    void setLinkedVariable(bdlt::Time                             *variable);
    void setLinkedVariable(bsl::vector<char>                     *variable);
    void setLinkedVariable(bsl::vector<short>                    *variable);
    void setLinkedVariable(bsl::vector<int>                      *variable);
    void setLinkedVariable(bsl::vector<bsls::Types::Int64>       *variable);
    void setLinkedVariable(bsl::vector<float>                    *variable);
    void setLinkedVariable(bsl::vector<double>                   *variable);
    void setLinkedVariable(bsl::vector<bsl::string>              *variable);
    void setLinkedVariable(bsl::vector<bdlt::Datetime>            *variable);
    void setLinkedVariable(bsl::vector<bdlt::Date>                *variable);
    void setLinkedVariable(bsl::vector<bdlt::Time>                *variable);
        // Set this object to have the type indicated by the specified
        // 'variable', and reset this object so that it no longer has a
        // constraint associated with it.  If 'variable' is not 0, then link it
        // with this option.

    // ACCESSORS
    bool checkConstraint(const bdlmxxx::ConstElemRef& element) const;
    bool checkConstraint(const bdlmxxx::ConstElemRef& element,
                         bsl::ostream&            stream) const;
        // Return 'true' if the specified 'element' has the 'bdem' element
        // type of this option and satisfies the constraint for this option,
        // and 'false' otherwise.  Optionally specify a 'stream'; if 'stream'
        // is specified and validation fails, a descriptive error message
        // indicating the reason for the failure is written to 'stream'.

    bool checkConstraint(const void       *variable) const;
    bool checkConstraint(const void       *variable,
                         bsl::ostream&     stream) const;
        // Return 'true' if the contents of the specified 'variable', cast to
        // the 'bdem' element type of this option, satisfies the constraint for
        // this option, and 'false' otherwise.  Optionally specify a 'stream';
        // if 'stream' is specified and validation fails, a descriptive error
        // message indicating the reason for the failure is written to
        // 'stream'.  The behavior is undefined unless 'variable' addresses a
        // valid object of the 'bdem' element type of this option.

    bsl::shared_ptr<CommandLine_Constraint> constraint() const;
        // Return the address of the (opaque) object storing the constraint
        // associated with this option.

    void *linkedVariable() const;
        // Return the address of the modifiable variable linked to this option,
        // or 0 no variable is linked.

    bdlmxxx::ElemType::Type type() const;
        // Return the 'bdem' element type of this option.  Note that an option
        // is a flag if it is of type 'BDEM_BOOL'.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this object.  If 'level' is negative, suppress indentation of the
        // first line.  If 'stream' is not valid on entry, this operation has
        // no effect.  The behavior is undefined if 'spacesPerLevel' is
        // negative.
};

// FREE OPERATORS
bool operator==(const CommandLineTypeInfo& lhs,
                const CommandLineTypeInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'CommandLineTypeInfo' objects have the same
    // value if their associated options have the same type, both objects do
    // not have a linked variable or both refer to the same variable, and both
    // do not have a constraint or both refer to the same constraint.  Note
    // that two objects constructed from copies of the same constraint will
    // *not* be identical (use 'shareConstraint' to ensure that both
    // constraints will be the same).

bool operator!=(const CommandLineTypeInfo& lhs,
                const CommandLineTypeInfo& rhs);
    // Return 'true' if the specified 'lhs' command-line option info has a
    // different value from the specified 'rhs' command-line option info, and
    // 'false' otherwise.  Two 'CommandLineTypeInfo' objects do not have
    // the same value if the associated options have different types, or one
    // object has a linked variable and the other either does not or refers to
    // a different variable, and one has a constraint and the other either does
    // not or refers to a different constraint.  Note that two objects
    // constructed from copies of the same constraint will *not* be identical
    // (use 'shareConstraint' to ensure that both constraints will be the
    // same).

bsl::ostream&
operator<<(bsl::ostream& stream, const CommandLineTypeInfo& rhs);
    // Write the value of the specified 'rhs' object to the specified 'stream'
    // stream in a (multi-line) human readable format.  Note that the last line
    // is *not* terminated by a newline character.

                        // ============================
                        // struct CommandLineOptionInfo
                        // ============================

struct CommandLineOptionInfo {
    // This 'struct' is a simple attribute class that describes the information
    // associated with an option, namely the associated tag (as a string, from
    // which the short and long tags are extracted), the option name, the
    // description used in printing usage, and optional associated
    // 'CommandLineTypeInfo' and 'CommandLineOccurrenceInfo' objects.
    //
    // By design, this 'struct' does not have any user-defined constructors, so
    // there is no provision for passing an allocator to its data members (all
    // of which take an allocator).  Consequently, all instances of this class
    // use the default allocator.  If proper allocator propagation is desired
    // (e.g., for storage within an allocator-aware container for which the use
    // of the default allocator is counter-indicated), one may use
    // 'CommandLineOption', which is both allocator-aware and
    // constructible from 'CommandLineOptionInfo'.
    //
    // The main purpose of this 'struct' is to provide a type whose values can
    // be statically-initialized.  For example:
    //..
    //  const CommandLineOptionInfo OPTIONS[] = {
    //     {
    //       "s(hortTag)|longTag",
    //       "optionName",
    //       "option description",
    //       CommandLineTypeInfo(/* . . . */),       // optional
    //       CommandLineOccurrenceInfo(/* . . . */)  // optional
    //     },
    //     // ...
    //  };
    //..
    // Note that each of the first three fields can be default-constructed, and
    // thus omitted in such a declaration; however, such an object will be of
    // limited use because it will be rejected by 'CommandLine' unless
    // each of the first three fields has a non-default string value.  See the
    // "Usage" section for an example of such initialization.

    // PUBLIC TYPES
    enum ArgType {
        BAEA_FLAG       = 0,
        BAEA_OPTION     = 1,
        BAEA_NON_OPTION = 2
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , FLAG       = BAEA_FLAG
      , OPTION     = BAEA_OPTION
      , NON_OPTION = BAEA_NON_OPTION
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // PUBLIC DATA
    bsl::string              d_tag;          // tags (or "" for non-option)

    bsl::string              d_name;         // accessing name and (optional)
                                             // default value

    bsl::string              d_description;  // description used in printing
                                             // usage

    CommandLineTypeInfo d_typeInfo;     // (optional) type/variable to be
                                             // linked, (optional) constraint

    CommandLineOccurrenceInfo
                             d_defaultInfo;  // indicates if the option is
                                             // required, and a potential
                                             // default value
};

// FREE OPERATORS
bool operator==(const CommandLineOptionInfo& lhs,
                const CommandLineOptionInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'CommandLineOptionInfo' objects have the
    // same value if they have the same tag string, the same name, the same
    // description, the same type info, and the same occurrence info values.

bool operator!=(const CommandLineOptionInfo& lhs,
                const CommandLineOptionInfo& rhs);
    // Return 'true' if the specified 'lhs' command-line option info has a
    // different value from the specified 'rhs' command-line option info, and
    // 'false' otherwise.  Two 'CommandLineOptionInfo' objects do not have
    // the same value if they have different tag strings, or different names,
    // or different descriptions, or different type infos, or different
    // occurrence info values, or any combination thereof.

bsl::ostream&
operator<<(bsl::ostream& stream, const CommandLineOptionInfo& rhs);
    // Write the value of the specified 'rhs' object to the specified 'stream'
    // stream in a (multi-line) human readable format.  Note that the last line
    // is *not* terminated by a newline character.

                          // =======================
                          // class CommandLineOption
                          // =======================

class CommandLineOption {
    // This 'class', constructible from and implicitly convertible to
    // 'CommandLineOptionInfo', provides the same attributes, but also
    // uses a 'bslma::Allocator' and thus can be stored in a container.  A
    // minor subtlety arises in the 'name' attribute, whereby any suffix
    // starting with an '=' sign in the 'name' attribute of a
    // 'CommandLineOptionInfo' is removed in order to derive the 'name' of
    // the 'CommandLineOption'.
    //
    // More generally, this class supports a complete set of *in*-*core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, and 'ostream' printing.  (A precise operational
    // definition of when two instances have the same value can be found in the
    // description of 'operator==' for the class).  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bsls::ObjectBuffer<CommandLineOptionInfo>
                          d_optionInfo;   // underlying option info

    bslma::Allocator     *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // PRIVATE MANIPULATORS
    void init(const CommandLineOptionInfo& info);
        // Initialize the underlying option info from the value of the
        // specified 'info' object.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(CommandLineOption,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    CommandLineOption(bslma::Allocator *basicAllocator = 0);
        // Create a default (empty) command-line option.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit
    CommandLineOption(
                        const CommandLineOptionInfo&  optionInfo,
                        bslma::Allocator                  *basicAllocator = 0);
        // Create a command-line option containing the value of the specified
        // 'optionInfo'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    CommandLineOption(
                        const CommandLineOption&      original,
                        bslma::Allocator                  *basicAllocator = 0);
        // Create a 'CommandLineOption' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~CommandLineOption();
        // Destroy this command-line option object.

    // MANIPULATORS
    CommandLineOption& operator=(const CommandLineOption&     rhs);
    CommandLineOption& operator=(const CommandLineOptionInfo& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference to this modifiable object.

    // ACCESSORS
    operator const CommandLineOptionInfo&() const;
        // Return a reference to a non-modifiable 'CommandLineOptionInfo'
        // object having the value of this option.

    CommandLineOptionInfo::ArgType argType() const;
        // Return the 'ArgType' enumerator identifying whether this option is a
        // flag (i.e., a tag not followed by a value), or whether it is an
        // option (i.e., a tag followed by a value), or a non-option (i.e., a
        // value not preceded by a short or long tag).

    const bsl::string& description() const;
        // Return the description of this option.

    bool isArray() const;
        // Return 'true' if the value of this option is of array type, and
        // 'false' otherwise.

    bool isLongTagValid(const char *longTag, bsl::ostream& stream) const;
        // Return 'true' if the specified 'longTag' is valid, leaving the
        // specified 'stream' uneffected; otherwise, write a diagnostic message
        // to 'stream' and return 'false'.

    bool isTagValid(bsl::ostream& stream) const;
        // Return 'true' if the tag is valid for this option, leaving the
        // specified 'stream' uneffected; otherwise, write a diagnostic message
        // to 'stream' and return 'false'.

    bool isDescriptionValid(bsl::ostream& stream) const;
        // Return 'true' if the description is valid for this option, leaving
        // the specified 'stream' uneffected; otherwise, write a diagnostic
        // message to 'stream' and return 'false'.

    bool isNameValid(bsl::ostream& stream) const;
        // Return 'true' if the name is valid for this option, leaving the
        // specified 'stream' uneffected; otherwise, write a diagnostic message
        // to 'stream' and return 'false'.

    const char *longTag() const;
        // Return the string used to identify this option on a command line
        // preceded with '--', or 0 if there is no long tag associated with
        // this option.  The behavior is undefined if this option is a
        // non-option (i.e., has no tag).

    const bsl::string& name() const;
        // Return the name of this option.

    const CommandLineOccurrenceInfo& occurrenceInfo() const;
        // Return a reference to the non-modifiable occurrence info for this
        // option (i.e., whether the option is required, optional, or hidden,
        // and its default value, if any).

    char shortTag() const;
        // Return the single character used to identify this option on a
        // command line preceded with a single '-', or 0 if this option only
        // has a long tag.  The behavior is undefined if this option is a
        // non-option (i.e., has no tag).

    const bsl::string& tagString() const;
        // Return the tag string provided to this object at construction.  Note
        // that this string is empty if this is a non-option.

    const CommandLineTypeInfo& typeInfo() const;
        // Return a reference to the non-modifiable type info for this option
        // (i.e., the type of the option, whether the option is linked to a
        // variable, and whether it has a constraint).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this object.  If 'level' is negative, suppress indentation of the
        // first line.  If 'stream' is not valid on entry, this operation has
        // no effect.  The behavior is undefined if 'spacesPerLevel' is
        // negative.
};

// FREE OPERATORS
bool operator==(const CommandLineOption& lhs,
                const CommandLineOption& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'CommandLineOption' objects have the same
    // value if their underlying 'CommandLineOptionInfo' objects have the
    // same value.

bool operator!=(const CommandLineOption& lhs,
                const CommandLineOption& rhs);
    // Return 'true' if the specified 'lhs' command-line option has a different
    // value from the specified 'rhs' command-line option, and 'false'
    // otherwise.  Two 'CommandLineOption' objects do not have the same
    // value if their underlying 'CommandLineOptionInfo' objects do not
    // have the same value.

bsl::ostream&
operator<<(bsl::ostream& stream, const CommandLineOption& rhs);
    // Write the value of the specified 'rhs' object to the specified 'stream'
    // stream in a (multi-line) human readable format.  Note that the last line
    // is *not* terminated by a newline character.

                             // =================
                             // class CommandLine
                             // =================

class CommandLine {
    // This 'class' parses, validates, and provides access to command-line
    // arguments.  The constructor takes the specification describing the
    // command-line arguments.  Once created, 'printUsage' can be invoked to
    // print the usage syntax.  The 'parse' method takes command-line arguments
    // and validates them against the specification provided at creation,
    // writing a suitable message to an optionally-specified stream in case of
    // a parsing error.  Once parsed, options and values can be accessed using
    // various accessors.  The class provides a set of 'theType' access methods
    // (for example 'theString', 'theInt') that return the value of the
    // specified option name.  It is also possible to link a variable with an
    // option in the specification; doing so will cause the variable to be
    // loaded with the option value once 'parse' has been invoked.  The 'data'
    // method returns a 'bdem_ConstRowBinding' containing option names and
    // their values.  A similar but different method, 'specifiedData', is
    // suitable for overwriting other configuration parameters (possibly
    // obtained from a configuration file).
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing.  (A precise operational definition
    // of when two instances have the same value can be found in the
    // description of 'operator==' for the class).  This class is *exception*
    // *neutral* with no guarantee of rollback: if an exception is thrown
    // during the invocation of a method on a pre-existing instance, the
    // container is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // PRIVATE TYPES
    enum State {
        BAEA_INVALID    = 0,
        BAEA_PARSED     = 1,
        BAEA_NOT_PARSED = 2
    };

    // DATA
    bsl::vector<CommandLineOption> d_options;    // command-line options
                                                      // specified at
                                                      // construction

    bsl::vector<bsl::vector<int> >      d_positions;  // set of positions of
                                                      // each command-line
                                                      // option in the
                                                      // arguments specified to
                                                      // 'parse', indexed by
                                                      // position of option in
                                                      // 'd_options'

    bsl::vector<int>                    d_nonOptionIndices;
                                                      // set of positions of
                                                      // each command-line
                                                      // non-option in the
                                                      // arguments specified to
                                                      // 'parse', indexed by
                                                      // non-option rank in
                                                      // 'd_options'

    State                               d_state;      // indicates whether
                                                      // 'parse' was invoked,
                                                      // and with what result

    bsl::vector<bsl::string>            d_arguments;  // command-line arguments
                                                      // specified to 'parse'

    bdlmxxx::Schema                         d_schema;     // schema describing the
                                                      // structure of the
                                                      // option values in the
                                                      // returned binding

    bdlmxxx::List                           d_data;       // primary option values,
                                                      // recorded one by one as
                                                      // they are parsed
                                                      // by the 'parse'
                                                      // manipulator

    bdlmxxx::List                           d_data1;      // final option values,
                                                      // copied from 'd_data'
                                                      // and from default
                                                      // values by 'postParse'

    mutable bdlmxxx::List                   d_data2;      // specified data, i.e.,
                                                      // 'd_data1' with
                                                      // non-specified
                                                      // arguments reset,
                                                      // created by the
                                                      // 'specifiedData'
                                                      // manipulator

    mutable bool                        d_isBindin2Valid;
                                                      // records whether
                                                      // 'd_data2' was
                                                      // initialized by
                                                      // 'specifiedData'

  private:
    // PRIVATE MANIPULATORS
    void clear();
        // Reset this command line object to an uninitialized state.  Note that
        // the state is set to 'INVALID' and not 'NOT_PARSED'.

    void initArgs(int argc, const char *const argv[]);
        // Store the specified 'argc' entries from the specified 'argv' array
        // of command-line arguments in this object.

    int parse(bsl::ostream& stream);
        // Parse the command-line arguments one-by-one, matching them against
        // the appropriate options, setting the primary option values along the
        // way.  Return 0 if parsing succeeds, and a non-zero value otherwise.
        // Upon encountering an error, output to the specified 'stream' a
        // descriptive error message and abort parsing subsequent arguments.

    int postParse(bsl::ostream& stream);
        // Verify that all required arguments have been given a value, and in
        // that case load all the primary option values (if set) or default
        // option values (if the primary option value is optional and set by no
        // command-line argument) into the final option values and set the
        // linked variables, if any.  Return 0 on success, and a non-zero value
        // if not all required arguments are provided with a value (and in this
        // case, leave the final option values and linked variables unchanged).

    void validateAndInitialize();
    void validateAndInitialize(bsl::ostream& stream);
        // Validate the command-line options passed at construction with
        // respect to the command-line documented invariants (e.g., valid tags,
        // names, and descriptions, default values satisfying constraints,
        // uniqueness of tags and names, and miscellaneous constraints), and
        // initialize all internal state for parsing.  Optionally specify an
        // output 'stream'.  If 'stream' is not specified, 'bsl::cerr' is used.
        // Upon encountering errors, output descriptive error messages and
        // abort the execution of this program by invoking the currently
        // installed assertion handler (unconditionally).  Note that this
        // method will attempt to report as many errors as possible before
        // aborting, but that correcting all these errors will not guarantee
        // that subsequent execution would result in successful validation.

    // PRIVATE ACCESSORS
    int findTag(const char *longTag, int tagLength) const;
        // Return the index (in the options table passed at construction) of a
        // command-line option whose long tag matches the specified 'longTag'
        // string of the specified 'tagLength', or -1 if no such 'longTag'
        // exists.

    int findTag(char shortTag) const;
        // Return the index (in the options table passed at construction) of an
        // option whose short tag matches the specified 'shortTag' character,
        // or -1 if no such 'shortTag' exists.

    int findName(const bsl::string& name) const;
        // Return the index (in the options table passed at construction) of an
        // option whose name matches the specified 'name' string, or -1 if no
        // such 'name' exists.

    void location(bsl::ostream& stream,
                  int           argc,
                  int           start = -1,
                  int           end = -1) const;
        // Output to the specified 'stream' an error message detailing the
        // location for the specified 'argc' position.  Optionally specify a
        // 'start' character position in that argument; if 'start' is
        // specified, optionally specified an 'end' character position as well.
        // The error message will reflect as complete an information as given.

    int longestTagSize() const;
        // Return the length of the longest long tag in all the possible
        // options of this command line.  Note that short tags are ignored.

    int longestNameSize() const;
        // Return the length of the longest name in all the possible options of
        // this command line.

    int missing(bool checkAlsoNonOptions = true) const;
        // Check that all required options have already been parsed.
        // Optionally specify 'checkAlsoNonOptions'.  If 'checkAlsoNonOptions'
        // is 'true' or not specified, required non-options that have not been
        // parsed will also be returned; if 'false', they will be ignored.
        // Return the index of the first missing option, or -1 if none.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(CommandLine,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    template <int LENGTH>
    CommandLine(const CommandLineOptionInfo (&specTable)[LENGTH],
                     bsl::ostream&                      stream,
                     bslma::Allocator                  *basicAllocator = 0);
    template <int LENGTH>
    CommandLine(CommandLineOptionInfo (&specTable)[LENGTH],
                     bsl::ostream&                stream,
                     bslma::Allocator            *basicAllocator = 0);
        // Create an object with the command-line options described by the
        // specified 'specTable'.  The 'specTable' *must* *be* a statically-
        // created array.  If the tag, description, or name specified for an
        // option in the 'specTable' is invalid, then the behavior is
        // undefined; nevertheless, an appropriate error message is written to
        // the specified 'stream'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently-installed
        // default allocator is used.

    template <int LENGTH>
    CommandLine(const CommandLineOptionInfo (&specTable)[LENGTH],
                     bslma::Allocator                  *basicAllocator = 0);
    template <int LENGTH>
    CommandLine(CommandLineOptionInfo (&specTable)[LENGTH],
                     bslma::Allocator            *basicAllocator = 0);
        // Create an object with the command-line options described by the
        // specified 'specTable'.  The 'specTable' *must* *be* a statically-
        // created array.  If the tag, description, or name specified for an
        // option in the 'specTable' is invalid, then the behavior is
        // undefined; nevertheless, an appropriate error message is written to
        // 'bsl::cerr'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently-installed default
        // allocator is used.

    CommandLine(const CommandLineOptionInfo *specTable,
                     int                               length,
                     bslma::Allocator                 *basicAllocator = 0);
        // Create an object with the command-line options described by the
        // specified 'specTable' of the specified 'length'.  The 'specTable'
        // need not be a statically-created array.  If the tag, description, or
        // name specified for an option in the 'specTable' is invalid, then the
        // behavior is undefined; nevertheless, an appropriate error message is
        // written to 'bsl::cerr'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    CommandLine(const CommandLineOptionInfo *specTable,
                     int                               length,
                     bsl::ostream&                     stream,
                     bslma::Allocator                 *basicAllocator = 0);
        // Create an object with the command-line options described by the
        // specified 'specTable' of the specified 'length'.  The 'specTable'
        // need not be a statically-created array.  If the tag, description, or
        // name specified for an option in the 'specTable' is invalid, then the
        // behavior is undefined; nevertheless, an appropriate error message is
        // written to the specified 'stream'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    CommandLine(const CommandLine&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create a command-line object having the value of the specified
        // 'original' command line, if the 'original' is parsed, and otherwise
        // having a state such that parsing command-line arguments results in
        // the same value as parsing the same command-line arguments with the
        // 'original'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless 'original'
        // is valid (i.e., 'original.isInvalid()' returns 'false').

    ~CommandLine();
        // Destroy this command-line object.

    // MANIPULATORS
    CommandLine& operator=(const CommandLine& rhs);
        // Assign to this command-line object the value of the specified 'rhs'
        // command-line object and return a reference to this modifiable
        // object.  The behavior is undefined unless both 'rhs' and
        // this object are valid (i.e., 'isInvalid()' and 'rhs.isInvalid()'
        // both return 'false').

    int parse(int argc, const char *const argv[]);
    int parse(int argc, const char *const argv[], bsl::ostream& stream);
        // Parse the command-line arguments contained in the array starting at
        // the specified 'argv' having the specified 'argc' length.  Optionally
        // specify a 'stream' to which an appropriate error message is written
        // if parsing fails.  If 'stream' is not specified, 'stderr' is used.
        // Return 0 on success, and a non-zero value otherwise.  The behavior
        // is undefined if 'parse' has already been invoked on this object.

    // ACCESSORS
    bdem_ConstRowBinding data() const;
        // Return the command-line options and their values.  If an option was
        // not entered on the command line *and* a default value was provided
        // for that option, then that default value will be used (note the
        // difference with the 'specifiedData' method).  If an option was not
        // entered on the command line *and* no default value was provided for
        // that option, then the corresponding value will be unset.  All the
        // flags (that is, 'boolean' options) will have integer type (rather
        // than 'bool') and the 'true' and 'false' values will be described by
        // integer values 1 and 0, respectively.  The behavior is undefined
        // unless 'isParsed' returns 'true'.

    bool isInvalid() const;
        // Return 'true' if this object was parsed unsuccessfully, and 'false'
        // otherwise.  Note that if 'parse' was not invoked, this method
        // returns 'false'.  Also note that if this object was assigned the
        // value of another 'CommandLine' object after if was parsed
        // successfully, it is the unsuccessful parsed status of that object
        // that will be returned.

    bool isParsed() const;
        // Return 'true' if this object was parsed successfully, and 'false'
        // otherwise.  Note that if 'parse' was invoked but failed, this method
        // returns 'false'.  Also note that if this object was assigned the
        // value of another 'CommandLine' object after if was parsed
        // successfully, it is the successful parsed status of that object that
        // will be returned.

    bool isSpecified(const bsl::string& name) const;
    bool isSpecified(const bsl::string& name, int *count) const;
        // Return 'true' if the option with the specified 'name' has been
        // entered on the command line and, if the optionally-specified 'count'
        // is not 0, load into 'count' the number of times the option 'name'
        // has been entered on the command line; otherwise, return 'false' and
        // leave 'count' uneffected.

    int numSpecified(const bsl::string& name) const;
        // Return the number of times the option with the specified 'name' has
        // been entered on the command line, or 0 if 'name' is not the name of
        // a field in the command-line specification passed at construction to
        // this object.

    const bsl::vector<int>& positions(const bsl::string& name) const;
        // Return the positions where the option with the specified 'name' has
        // been entered on the command line.  If the option was not specified,
        // return an empty vector.

    int position(const bsl::string& name) const;
        // Return the position where the option with the specified 'name' has
        // been entered on the command line.  If the option was not specified,
        // return -1.  The behavior is undefined unless the option is of scalar
        // type.

    bdem_ConstRowBinding specifiedData() const;
        // Return the command-line options and their values.  If an option
        // was not entered on the command line, then the corresponding value
        // will be unset (note the difference with the 'data' method).  This
        // method is especially useful for overwriting some other configuration
        // (potentially obtained from a configuration file).  All the flags
        // will have integer type (rather than 'bool') and the 'true' and the
        // 'false' values will be described by integer values 1 and 0,
        // respectively.  The behavior is undefined unless 'isParsed' returns
        // 'true'.

    bool theBool(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type 'bool' and
        // 'isParsed' returns 'true'.

    char theChar(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type 'char' and
        // 'isParsed' returns 'true'.

    short theShort(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type 'short' and
        // 'isParsed' returns 'true'.

    int theInt(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type 'int' and
        // 'isParsed' returns 'true'.

    bsls::Types::Int64 theInt64(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type
        // 'bsls::Types::Int64' and 'isParsed' returns 'true'.

    float theFloat(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type 'float' and
        // 'isParsed' returns 'true'.

    double theDouble(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless the option is of type 'double' and
        // 'isParsed' returns 'true'.

    const bsl::string& theString(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::string' and 'isParsed' returns 'true'.

    const bdlt::Datetime& theDatetime(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bdlt::Datetime' and 'isParsed' returns 'true'.

    const bdlt::Date& theDate(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bdlt::Date' and 'isParsed' returns 'true'.

    const bdlt::Time& theTime(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bdlt::Time' and 'isParsed' returns 'true'.

    const bsl::vector<char>& theCharArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<char>' and 'isParsed' returns 'true'.

    const bsl::vector<short>& theShortArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<short>' and 'isParsed' returns 'true'.

    const bsl::vector<int>& theIntArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<int>' and 'isParsed' returns 'true'.

    const bsl::vector<bsls::Types::Int64>& theInt64Array(
                                                const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<bsls::Types::Int64>' and 'isParsed'
        // returns 'true'.

    const bsl::vector<float>& theFloatArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<float>' and 'isParsed' returns 'true'.

    const bsl::vector<double>& theDoubleArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<double>' and 'isParsed' returns 'true'.

    const bsl::vector<bsl::string>& theStringArray(
                                                const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<bsl::string>' and 'isParsed' returns 'true'.

    const bsl::vector<bdlt::Datetime>& theDatetimeArray(
                                                const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<bdlt::Datetime>' and 'isParsed' returns
        // 'true'.

    const bsl::vector<bdlt::Date>& theDateArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<bdlt::Date>' and 'isParsed' returns 'true'.

    const bsl::vector<bdlt::Time>& theTimeArray(const bsl::string& name) const;
        // Return a reference to the non-modifiable value of the option having
        // the specified 'name'.  The behavior is undefined unless the option
        // is of type 'bsl::vector<bdlt::Time>' and 'isParsed' returns 'true'.

    void printUsage() const;
    void printUsage(bsl::ostream& stream) const;
        // Print usage to the specified output 'stream', describing what the
        // command line should look like.  If 'stream' is not specified, print
        // usage to 'stderr'.  This method can be invoked at any time, even
        // before 'parse' has been invoked on this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this command-line object to the specified output 'stream'
        // at the (absolute value of) the optionally specified indentation
        // 'level' and return a reference to 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of spaces
        // per indentation level for this object.  If 'level' is negative,
        // suppress indentation of the first line.  The behavior is undefined
        // unless '0 <= spacesPerLevel'.  If 'stream' is not valid on entry,
        // this operation has no effect.
};

// FREE OPERATORS
bool operator==(const CommandLine& lhs, const CommandLine& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two command-line arguments have the same value if
    // and only if they are both parsed successfully and have the same
    // command-line specifications and the same values for flags, options, and
    // non-options.  Note that two identical copies built with the same option
    // table, but unparsed, are *not* equal.

bool operator!=(const CommandLine& lhs, const CommandLine& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' have the same value, and
    // 'true' otherwise.  Two command-line arguments do not have the same value
    // if and only if they have different command-line specifications, or one
    // is parsed successfully but the other is not, or neither is, or else both
    // have the same specification and both are parsed successfully but they
    // have different values for at least one flag, option or non-option.  Note
    // that two identical copies built with the same option table, but
    // unparsed, are *not* equal.

bsl::ostream& operator<<(bsl::ostream& stream, const CommandLine& rhs);
    // Write the options and their values in the specified 'rhs' to the
    // specified output 'stream' in a (multi-line) human readable format.  Note
    // that the last line is *not* terminated by a newline character.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class CommandLine
                             // -----------------

// CREATORS
template <int LENGTH>
CommandLine::CommandLine(
                         const CommandLineOptionInfo (&specTable)[LENGTH],
                         bsl::ostream&                      stream,
                         bslma::Allocator                  *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(CommandLineOption(specTable[i]));
    }
    validateAndInitialize(stream);
    d_state = BAEA_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(
                               CommandLineOptionInfo (&specTable)[LENGTH],
                               bsl::ostream&                stream,
                               bslma::Allocator            *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(CommandLineOption(specTable[i]));
    }
    validateAndInitialize(stream);
    d_state = BAEA_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(
                         const CommandLineOptionInfo (&specTable)[LENGTH],
                         bslma::Allocator                  *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(CommandLineOption(specTable[i]));
    }
    validateAndInitialize();
    d_state = BAEA_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(
                               CommandLineOptionInfo (&specTable)[LENGTH],
                               bslma::Allocator            *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(BAEA_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(CommandLineOption(specTable[i]));
    }
    validateAndInitialize();
    d_state = BAEA_NOT_PARSED;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
