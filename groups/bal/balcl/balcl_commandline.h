// balcl_commandline.h                                                -*-C++-*-
#ifndef INCLUDED_BALCL_COMMANDLINE
#define INCLUDED_BALCL_COMMANDLINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide command line parsing, validation, and access.
//
//@CLASSES:
//               balcl::CommandLine: value-semantic command-line arguments
//  balcl::CommandLineOptionsHandle: references to parsed options
//
//@SEE_ALSO: balcl_optionvalue, balcl_optiontype
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'balcl::CommandLine', used to represent the command-line arguments passed to
// a process.  Also provided is 'balcl::CommandLineOptionsHandle', a class that
// provides access to the options (and associative values, if any) found in a
// 'balcl::CommandLine' object in a "parsed" state.
//
// The constructor of 'balcl::CommandLine' takes a specification describing the
// command-line arguments.  Once created, 'printUsage' can be invoked to print
// the usage syntax.  The 'parse' method takes command-line arguments and
// validates them against the specification supplied at construction, printing
// suitable messages on an optionally-specified stream in case of a parsing
// error.  Once parsed, options and values can be accessed using various access
// methods.  The class provides a set of *theType* access methods (for example,
// 'theString', 'theInt') that return the value of the specified option name.
// It is also possible to link a variable with an option in the specification;
// doing so will cause the variable to be loaded with the option value once
// 'parse' has been invoked and was successful.  The 'options' method returns a
// 'balcl::CommandLineOptionsHandle' containing option names and their values.
//
///Component Features Summary
///--------------------------
// This component offers the following features:
//
//: 1 Validation of command-line arguments against the provided specification.
//:
//: 2 The ability to specify sophisticated constraints easily.  Users can also
//:   build their own constraints and use them in the specification.
//:
//: 3 The ability to automatically generate usage syntax.
//:
//: 4 The ability to directly link a variable to an option.  After successful
//:   parsing, all linked variables are loaded with their corresponding option
//:   values.
//:
//: 5 The ability to access options and their corresponding values through
//:   various accessor methods.
//:
//: 6 The ability to parse Unix-style command lines (for example, grouping of
//:   flags, allowing any ordering of options, or a mix between options and
//:   non-option arguments, short tag and long tag forms, etc.).
//:
//: 7 The ability to have multiple non-option arguments of possibly different
//:   types.  Note that only the last non-option argument may be multi-valued,
//:   and that if a non-option has a default value, then all subsequent
//:   non-options must also have a default value.
//
// A lower bound can be placed on the number of multi-valued non-option
// arguments (e.g., two or more values) can be achieved by explicitly
// specifying the required number of single-valued non-option arguments of the
// same type before the unrestricted multi-value non-option of that same type.
//
///Background for Unix-Style Command-Line Arguments and Definitions
///----------------------------------------------------------------
// This section provides background on Unix-style command-line arguments, as
// well as definitions of terms used frequently in this documentation (such as
// "option", "flag", "non-option", "tag", "short tag", "long tag").  Readers
// familiar with Unix command lines can skim this section or omit entirely.
//
// Command-line arguments can be classified as:
//
//: o command name (there is only one, and it is always the first argument)
//: o options (tags and associated values)
//: o flags (boolean options)
//: o non-option arguments
//
// For example, in the following command line:
//..
//  $ mybuildcommand -e -c CC64 myproject
//..
// the command name is 'mybuildcommand'.  There is one option, described by
// '-c CC64': 'c' is the tag name, and 'CC64' is the option value.  There is
// also one boolean option (flag): '-e' is a flag, 'e' is the flag name.  The
// last parameter, 'myproject', is a non-option argument.
//
// Sometimes *option* is also used where "flag" or "non-option" would be more
// accurate.  What is actually intended should be clear from context.
//
// A user specifies an option on a command line by entering one of the tag
// values configured for that option.  Each option has a mandatory long tag and
// an optional short tag.  The short tag, if specified, must be a single
// alphabet symbol; the long tag generally must follow the same rules
// applicable to C/C++ identifiers, except that '-' is allowed (but not as the
// leading character).  When a short tag is used on a command line, it must be
// preceded by '-', and when a long tag is used it must be preceded by '--'.
// Flags have no corresponding values; they are either present or absent.
// Option tags *must* be followed by a corresponding option value.  An option
// can have multiple values (such options are called multi-valued options).
// When multiple values are provided for an option, the tag must appear with
// each value (see the section {Multi-Valued Options and How to Specify Them}).
// Arguments that are not the command name, options, or flags are called
// "non-option" arguments and can be either single-valued or multi-valued.
// They do not have any tag associated with them.
//
// Consider the syntax of a typical Unix-style command whose options are
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
//  'files...' describes the multi-valued non-option argument.
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
// specify a non-option argument beginning with '-', use a single '--'
// separator (not followed by a long tag).
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
//
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
// multi-valued option, and '+' denotes a multivalued option that must occur at
// least once.
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
//
///Order of Arguments
///------------------
// Command-line arguments can appear in any order.  For example, given the
// command-line specification described by the following usage string:
//..
//  usage: mysort [-r|reverse] [-o|outputfile <outfile>] [<file>]+
//..
// all the following command lines are valid (and equivalent):
//..
//  $ mysort -r -o myoutfile file1 file2 file3
//  $ mysort file1 file2 file3 -r -o myoutfile
//  $ mysort file1 -o myoutfile file2 -r file3
//  $ mysort file1 -o=myoutfile file2 -r file3
//..
// There are three exceptions to the above rule on argument order:
//
//: 1 An option tag must be followed by that option's value, if any (either in
//:   the next argument, or in the same argument using "=value").
//:
//: 2 When a non-option argument starts with a '-' then it must not appear
//:   before any option or flag *and* a '--' must be put on the command line to
//:   indicate the end of all options and flags.
//:
//: 3 Non-option arguments are parsed and assigned in the order they appear on
//:   the command line.
//
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
// Note that the order of values within the sequence of multi-valued non-option
// arguments differs in both examples, as per rule (3).  In the first example,
// the non-option arguments have for a value the (ordered) sequence:
//..
//  -weirdfilename file2 file3
//..
// while in the second example, the non-option argument value is the sequence:
//..
//  file2 file3 -weirdfilename
//..
// This order may or may not matter to the application.
//
///Specifying Command-Line Arguments
///---------------------------------
// A command line is described by an *option* *table* (supplied as an array of
// 'balcl::OptionInfo').  Each entry (row) of the table describes an option
// (i.e., an option, flag, or non-option argument).  Each entry has several
// fields, specified in the following order:
//..
//  Field name                     Main purpose (see below for more details)
//  ============================   ===========================================
//  tag field                      Specify tags (short and long) for options
//                                 and flags.  A non-option argument is
//                                 indicated by an empty string.
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
///'balcl::OptionInfo' versus 'balcl::Option'
/// - - - - - - - - - - - - - - - - - - - - -
// In some applications, command-line specifications *must* *be* defined using
// a statically-initialized array.  For that reason, there are two classes that
// serve the same purpose: 'balcl::OptionInfo' is a statically-initializable
// class but it does not conform to the 'bslma' allocator protocol, while
// 'balcl::Option' is convertible from 'balcl::OptionInfo', takes allocators,
// and is suitable for storing into containers.
//
///Tag Field
///- - - - -
// The tag field specifies the (optional) short tag and long tag for the
// corresponding option or flag, except that non-option arguments are indicated
// by an empty string for a tag field.  There can only be one multi-valued
// entry for non-option arguments, and it must be listed last among the
// non-options.
//
// The general format is either:
//: 1 "" (empty string) for non-option arguments;
//: 2 "<s>|<long>" for options and flags, where '<s>' is the short tag, and
//:   '<long>' is the long tag; or
//: 3 "<long>" for options and flags where a short tag is not specified.
//
// Note that for short tags ('<s>'), 's' must be a single character (different
// from '-' and '|'); for long tags ("<long>"), 'long' must have 2 or more
// characters (which may contain '-', except as the first character, but cannot
// contain '|').  Also note that either no tag (empty string), both short and
// long tags, or only a long tag, may be specified.
//
// The tag field cannot be omitted, but it can be the empty string.
//
///Name Field
/// - - - - -
// The name field specifies the name through which the option value can be
// accessed either through one of the *theType* methods or through the handle
// returned by the 'options' method.
//
// The general format is any non-empty string.  In most cases, the name will be
// used as-is.  Note that any suffix starting at the first occurrence of '=',
// if any, is removed from the name before storing in the 'balcl::OptionInfo'.
// Thus, if a name having such a suffix is specified in a 'balcl::OptionInfo'
// (e.g., "nameOption=someAttribute"), the correct name to use for querying
// this option by name (e.g., through the 'options' handle) does not include
// the suffix (e.g., 'cmdLine.numSpecified("nameOption=someAttribute")' will
// always return 0, but 'cmdLine.numSpecified("nameOption")' will return the
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
// and multi-valued non-option arguments) can be specified by using array
// types.  The list of the supported types is provided in the section
// {Supported Types} below.
//
// Other constraints can be placed on individual value(s).  When the type is an
// array type, then those constraints are placed on the individual value(s)
// held in the array and not on the entire array.  A list of useful constraints
// is provided in the section {Supported Constraint Values}.  Also see the
// section {Building New Constraints} to see how new constraints can be built
// so that they can be used in the same manner as the available constraints.
//
// Additionally, this field allows a specified variable to be linked to the
// option.  In that case, the variable is automatically loaded with the option
// value after parsing.
//
// The general format can be one of either:
//..
//  balcl::TypeInfo(&variable)
//  balcl::TypeInfo(&variable, constraint)
//      // Link the option with the specified 'variable'.  Note that the option
//      // type is inferred from the type of 'variable'.  Optionally place the
//      // user-specified 'constraint', of a type defined in
//      // 'balcl::Constraint', on the value.
//
//  balcl::TypeInfo(type)
//  balcl::TypeInfo(type, constraint)
//      // Specify the type of this option to be of the specified 'type'.
//      // Optionally place the user-specified 'constraint', of a type defined
//      // in 'balcl::Constraint', on the value.  Don't link this
//      // option with any variable.  'type' must be one of the static
//      // variables (null pointers) listed in
//      // {'balcl_optiontype'|Enumerators}.
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
//  balcl::OccurrenceInfo::e_HIDDEN
//  balcl::OccurrenceInfo::e_OPTIONAL
//  balcl::OccurrenceInfo::e_REQUIRED
//  a default value
//..
// If a default value is specified, the option is assumed to be optional; in
// addition, the default value must satisfy the type and constraint indicated
// by the specified type-and-constraint field.
//
// This field can be omitted, and is always omitted if the type-and-constraint
// field is not specified.  If omitted, the option is not required on the
// command line and has no default value; furthermore, if the option is not
// present on the command line, the linked variable, if any, is unaffected.
//
///Example Field Values
///--------------------
// The following tables give examples of field values.
//
///Example: Tag Field
/// - - - - - - - - -
// The tag field may be declared using the following forms:
//..
//     Usage                              Meaning
//  ==============    =======================================================
//  "o|outputfile"    The option being defined is either an option or a flag.
//                    The short tag is "o" and the long tag is "outputfile".
//
//                    Note that "o" alone is invalid since a long tag must be
//                    specified.
//
//  "outputfile"      The option being defined is either an option or a flag.
//                    There is no short tag and the long tag is "outputfile".
//
//      ""            Specifies a non-option argument.
//..
//
///Example: Name Field
///- - - - - - - - - -
// The name field may be declared using the following form:
//..
//      Usage                              Meaning
//  ===============   =======================================================
//     "xyz"          The option value can be accessed by "xyz".
//..
//
///Example: Type-and-Constraint Field
/// - - - - - - - - - - - - - - - - -
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
//  balcl::TypeInfo(&portNum)
//                    (1) Link the variable 'portNum' with this option value.
//                    That is, after successful parsing, this variable will
//                    contain the option value specified on the command line.
//                    (2) An integer value must be provided on the command
//                    line for this option (the type 'int' is inferred
//                    implicitly from the type of 'portNum').
//
//  balcl::TypeInfo(balcl::OptionType::k_INT)
//                    This option value must be an integer.
//
//  balcl::TypeInfo(&isVerbose)
//                    Load the variable 'isVerbose' with this option value.
//
//  balcl::TypeInfo(balcl::OptionType::k_BOOL)
//                    This option is a flag.
//
//  balcl::TypeInfo(&fileNames)
//                    Load the variable 'fileNames' with the values specified
//                    for this multi-valued option (or multi-valued
//                    non-option argument).  That is, after successful parsing,
//                    this variable will contain the sequence of values
//                    specified on the command line, in the same order.
//
//  balcl::TypeInfo(balcl::OptionType::k_STRING_ARRAY)
//                    This option value consists of a sequence of string
//                    values specified on the command line, in the same order.
//..
//
///Example: Occurrence Information Field
///- - - - - - - - - - - - - - - - - - -
// The following values may be used for this field:
//..
//     Usage                               Meaning
//  ===============   ========================================================
//  balcl::OccurrenceInfo::e_REQUIRED
//                    The value(s) for this option *must* be provided on the
//                    command line.  For options that are of an array type, at
//                    least one value must be provided.  Omission manifests
//                    as a parse error.
//
//  balcl::OccurrenceInfo::e_OPTIONAL
//                    Value(s) for this option may be omitted on the command
//                    line.
//
//  balcl::OccurrenceInfo::e_HIDDEN
//                    Same as 'e_OPTIONAL'; in addition, this option will not
//                    be displayed by 'printUsage'.
//
//  13                The default value for this option is 13 and the option
//                    is not required on the command line.  If no value is
//                    provided, then 13 is used.  If the type described by the
//                    type-and-constraint field is not integer, then it is an
//                    error.
//..
// *Note*: If an option is optional *and* no value is provided on the command
// line, then it will be in a null state (defined type but *no* defined value)
// in the handle returned by either 'options' or 'specifiedOptions'.  In
// addition, if a variable was linked to this option, it will be unmodified
// after parsing.
//
///Supported Types
///---------------
// The following types are supported.  The type is specified by an enumeration
// value (see {'balcl_optiontype'}) supplied as the first argument to:
//..
//  balcl::TypeInfo(type, constraint)
//..
// which is used to create the type-and-constraint field value in the
// command-line specification.  When the constraint need only specify the type
// of the option value (i.e., no linked variable or programmatic constraint),
// one can supply any of the public data members of 'balcl::OptionType' shown
// below:
//..
//  Type                            Specifier
//  -----------------------------   -------------------------
//  bool                            OptionType::k_BOOL
//  char                            OptionType::k_CHAR
//  int                             OptionType::k_INT
//  bsls::Types::Int64              OptionType::k_INT64
//  double                          OptionType::k_DOUBLE
//  bsl::string                     OptionType::k_STRING
//  bdlt::Datetime                  OptionType::k_DATETIME
//  bdlt::Date                      OptionType::k_DATE
//  bdlt::Time                      OptionType::k_TIME
//  bsl::vector<char>               OptionType::k_CHAR_ARRAY
//  bsl::vector<int>                OptionType::k_INT_ARRAY
//  bsl::vector<bsls::Types::Int64> OptionType::k_INT64_ARRAY
//  bsl::vector<double>             OptionType::k_DOUBLE_ARRAY
//  bsl::vector<bsl::string>        OptionType::k_STRING_ARRAY
//  bsl::vector<bdlt::Datetime>     OptionType::k_DATETIME_ARRAY
//  bsl::vector<bdlt::Date>         OptionType::k_DATE_ARRAY
//  bsl::vector<bdlt::Time>         OptionType::k_TIME_ARRAY
//..
// The ASCII representation of these values (i.e., the actual format of the
// values on command lines) depends on the type:
//: o Numeric Values: see {'bdlb_numericparseutil'}.
//: o Date/Time Values: see {'bdlt_iso8601util'}.
//
///Supported Constraint Values
///---------------------------
// This component supports constraint values for each type.  Specifically, the
// utility 'struct' 'balcl::Constraint' defines 'TYPEConstraint' types (for
// instance, 'StringConstraint', 'IntConstraint') that can be used to define a
// constraint suitable for the 'balcl::TypeInfo' class.
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
// defined in the utility 'struct' 'balcl::Constraint'.  Note that when passing
// a function as a constraint, the *address* of the function must be passed.
//
///Valid 'balcl::OptionInfo' Specifications
///----------------------------------------
// The 'balcl::CommandLine' class has a complex set of preconditions on the
// option specification table (array of 'balcl::OptionInfo' objects) passed to
// each of its constructors.  There are requirements on individual elements, on
// elements relative to each other, and on the entire set of elements.  If
// these preconditions are not met, the behavior of the constructor is
// undefined.
//
// The preconditions (some previously mentioned) are given in their entirety
// below.  Moreover, an overloaded class method,
// 'balcl::CommandLine::isValidOptionSpecification', is provided to allow
// programmatic checking without risk of incurring undefined behavior.
//
///Tag/Name/Description Fields
///- - - - - - - - - - - - - -
// The tag, name, and description fields must pass the 'isTagValid',
// 'isNameValid, and 'isDescriptionValid' methods of 'balcl::Option',
// respectively.
//
//: o The tag field:
//:   o If empty (a non-option argument), the option must not be a flag.
//:   o If non-empty, see {Tag Field} above for details.
//: o The name field must be non-empty.
//: o The description field must be non-empty.
//
// Collectively, each non-empty short tag, each long tag, and each name must be
// unique in the specification.
//
///Default Values
/// - - - - - - -
//: o Default values are disallowed for flags.
//: o The type of a default value must match the type of its option.
//: o The default value must satisfy the user-defined constraint on the option
//:   value, if any.
//
///Non-Option Arguments
/// - - - - - - - - - -
//: o Cannot be a flag (see {Tag/Name/Description Fields}).
//: o Cannot be a hidden option.
//: o Only the last non-option argument can be multi-valued (i.e., an array
//:   type).
//: o If a non-option argument has a default value, all subsequent non-option
//:   arguments must also have default values.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using Command Line Features In Concert
///- - - - - - - - - - - - - - - - - - - - - - - - -
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
// We choose the non-option argument to be an array of 'bsl::string' so as to
// accommodate multiple files.
//
// These options might be used incorrectly, as the following examples show:
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
//      // "insertionSort", and "shellSort"; otherwise, output to the specified
//      // 'stream' an appropriate error message and return 'false'.
//  {
//      if ("quickSort" == *algo || "insertionSort" == *algo
//       || "shellSort" == *algo) {
//          return true;                                              // RETURN
//      }
//      stream << "Error: sorting algorithm must be either "
//                "'quickSort', 'insertionSort', or 'shellSort'.\n";
//      return false;
//  }
//..
// Using this function, we can now use a 'balcl::CommandLine' object to parse
// command-line options.  The proper usage is shown below.  First we declare
// the variables to be linked to the options.  If they are needed at global
// scope, we could declare them as global variables, but we prefer to declare
// them as local variables inside 'main':
//..
//  int main(int argc, const char *argv[]) {
//..
// Note that it is important that variables that will be bound to optional
// command-line arguments be initialized to their default value, otherwise
// their value will unspecified if a value isn't provided on the command line
// (unless a default is specified via 'balcl::OccurrenceInfo'):
//..
//      bool isReverse         = false;
//      bool isCaseInsensitive = false;
//      bool isUniq            = false;
//
//      bsl::string outFile;
//      bsl::string sortAlgo;
//
//      bsl::vector<bsl::string> files;
//..
// Next, we build up an option specification table as follows:
//..
//      // build constraint for sortAlgo option
//      balcl::Constraint::StringConstraint validAlgoConstraint;
//      validAlgoConstraint = &isValidAlgorithm;
//
//      // option specification table
//      balcl::OptionInfo specTable[] = {
//        {
//          "r|reverse",                                     // tag
//          "isReverse",                                     // name
//          "sort in reverse order",                         // description
//          balcl::TypeInfo(&isReverse),                     // link
//          balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
//        },
//        {
//          "i|insensitivetocase",                           // tag
//          "isCaseInsensitive",                             // name
//          "be case insensitive while sorting",             // description
//          balcl::TypeInfo(&isCaseInsensitive),             // link
//          balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
//        },
//        {
//          "u|uniq",                                        // tag
//          "isUniq",                                        // name
//          "discard duplicate lines",                       // description
//          balcl::TypeInfo(&isUniq),                        // link
//          balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
//        },
//        {
//          "a|algorithm",                                   // tag
//          "sortAlgo",                                      // name
//          "sorting algorithm",                             // description
//          balcl::TypeInfo(&sortAlgo, validAlgoConstraint),
//                                                           // link and
//                                                           // constraint
//          balcl::OccurrenceInfo(bsl::string("quickSort"))
//                                                           // default
//                                                           // algorithm
//        },
//        {
//          "o|outputfile",                                  // tag
//          "outputFile",                                    // name
//          "output file",                                   // description
//          balcl::TypeInfo(&outFile),                       // link
//          balcl::OccurrenceInfo::e_REQUIRED                // occurrence info
//        },
//        {
//          "",                                              // non-option
//          "fileList",                                      // name
//          "files to be sorted",                            // description
//          balcl::TypeInfo(&files),                         // link
//          balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
//        }
//      };
//..
// We can now create a command-line specification and parse the command-line
// options:
//..
//      // Create command-line specification.
//      balcl::CommandLine cmdLine(specTable);
//
//      // Parse command-line options; if failure, print usage.
//      if (cmdLine.parse(argc, argv)) {
//          cmdLine.printUsage();
//          return -1;                                                // RETURN
//      }
//..
// Upon successful parsing, the 'cmdLine' object will acquire a value that
// conforms to the specified constraints.  We can examine these values as
// follows:
//..
//      // If successful, obtain command-line option values.
//      balcl::CommandLineOptionsHandle options = cmdLine.options();
//
//      // Access through linked variable.
//      bsl::cout << outFile << bsl::endl;
//
//      // Access through *theType* methods.
//      assert(cmdLine.theString("outputFile") == outFile);
//
//      // Access through 'options'.
//      assert(options.theString("outputFile") == outFile);
//
//      // Check that required option has been specified once.
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

#include <balscm_version.h>

#include <balcl_option.h>
#include <balcl_optioninfo.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>

#include <bdlb_printmethods.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>    // 'bsl::size_t'
#include <bsl_cstring.h>    // 'bsl::strcmp'
#include <bsl_iosfwd.h>
#include <bsl_sstream.h>    // 'bsl::ostringstream'
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace bdlt { class Date; }
namespace bdlt { class Datetime; }
namespace bdlt { class Time; }

namespace balcl {

                        // =============================
                        // struct CommandLine_SchemaData
                        // =============================

struct CommandLine_SchemaData{
    // This 'struct', a pure value-semantic type, is used to manage option
    // value type and name information in support of public interfaces to
    // parsed options.  See 'CommandLineOptionsHandle'.

    // PUBLIC DATA
    OptionType::Enum  d_type;    // option data type
    const char       *d_name_p;  // option name
};

// FREE OPERATORS
bool operator==(const CommandLine_SchemaData& lhs,
                const CommandLine_SchemaData& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'CommandLine_SchemaData' objects have the same
    // value if their 'type' attributes are the same and if their 'name'
    // attributes compare equal.

bool operator!=(const CommandLine_SchemaData& lhs,
                const CommandLine_SchemaData& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do note have the same
    // value, and 'false' otherwise.  Two 'CommandLine_SchemaData' objects do
    // not have the same value if their 'type' attributes are not the same or
    // if their 'name' attributes do not compare equal.

                        // =================
                        // class CommandLine
                        // =================

class CommandLineOptionsHandle;

class CommandLine {
    // This value-semantic 'class' parses, validates, and provides access to
    // command-line arguments.  The constructor takes the specification
    // describing the command-line arguments.  Once created, 'printUsage' can
    // be invoked to print the usage syntax.  The 'parse' method takes
    // command-line arguments and validates them against the specification
    // provided at creation, writing a suitable message to an
    // optionally-specified stream in case of a parsing error.  Once parsed,
    // options and values can be accessed using various accessors.  The class
    // has a set of 'theType' methods (e.g., 'theString', 'theInt') that
    // provide access, by name, to the value of the indicated option.  It is
    // also possible to link a variable with an option in the specification;
    // doing so will cause the variable to be loaded with the option value once
    // 'parse' has been invoked.  The 'options' method returns a
    // 'balcl::CommandLineOptionsHandle' object referring to the option names
    // and their values.  A similar but different method, 'specifiedOptions',
    // is suitable for overwriting other configuration parameters (possibly
    // obtained from a configuration file).

    // PRIVATE TYPES
    typedef bsl::vector<CommandLine_SchemaData> CommandLine_Schema;
    typedef bsl::vector<OptionValue> OptionValueList;

    enum State {
        e_INVALID    = 0,
        e_PARSED     = 1,
        e_NOT_PARSED = 2
    };

    // DATA
    bsl::vector<Option>                 d_options;    // command-line options
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

    CommandLine_Schema                  d_schema;     // schema describing the
                                                      // structure of the
                                                      // option values in the
                                                      // returned handle

    OptionValueList                     d_data;       // primary option values,
                                                      // recorded one by one as
                                                      // they are parsed by the
                                                      // 'parse' manipulator

    OptionValueList                     d_data1;      // final option values,
                                                      // copied from 'd_data'
                                                      // and from default
                                                      // values by 'postParse'

    mutable OptionValueList             d_data2;      // specified data, i.e.,
                                                      // 'd_data1' with
                                                      // non-specified
                                                      // arguments reset,
                                                      // created by the
                                                      // 'specifiedOptions'
                                                      // manipulator

    mutable bool                        d_isBindin2Valid;
                                                      // records whether
                                                      // 'd_data2' was
                                                      // initialized by
                                                      // 'specifiedOptions'

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
        // Upon encountering an error, output to the specified 'stream' a
        // descriptive error message and abort parsing subsequent arguments.

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
    int findName(const bsl::string& name) const;
        // Return the index (in the options table passed at construction) of an
        // option whose name matches the specified 'name' string, or -1 if no
        // such 'name' exists.

    int findTag(const char *longTag, bsl::size_t tagLength) const;
        // Return the index (in the options table passed at construction) of a
        // command-line option whose long tag matches the specified 'longTag'
        // string of the specified 'tagLength', or -1 if no such 'longTag'
        // exists.

    int findTag(char shortTag) const;
        // Return the index (in the options table passed at construction) of an
        // option whose short tag matches the specified 'shortTag' character,
        // or -1 if no such 'shortTag' exists.

    void location(bsl::ostream& stream,
                  int           index,
                  int           start = -1,
                  int           end   = -1) const;
        // Output to the specified 'stream' a message describing the location
        // in the argument at the specified 'index' (in the list of
        // command-line arguments) where an error was found.  Optionally
        // specify a 'start' character position in that argument; if 'start' is
        // specified, optionally specify an 'end' character position as well.

    int longestNonFlagNameSize() const;
        // Return the length of the longest name in all the possible non-flag
        // options of this command line.

    int longestTagSize() const;
        // Return the length of the longest long tag in all the possible
        // options of this command line.  Note that short tags are ignored.

    int missing(bool checkAlsoNonOptions = true) const;
        // Check that all required options have already been parsed.
        // Optionally specify 'checkAlsoNonOptions'.  If 'checkAlsoNonOptions'
        // is 'true' or not specified, required non-option arguments that have
        // not been parsed will also be returned; if 'false', they will be
        // ignored.  Return the index of the first missing option, or -1 if
        // none.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CommandLine, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(CommandLine, bdlb::HasPrintMethod);

    // CLASS METHODS
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                        const OptionInfo (&specTable)[LENGTH]);
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                              OptionInfo (&specTable)[LENGTH]);
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                         const OptionInfo (&specTable)[LENGTH],
                                         bsl::ostream&      stream);
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                            OptionInfo    (&specTable)[LENGTH],
                                            bsl::ostream&   stream);
        // Return 'true' if the specified (statically-initialized) 'specTable'
        // of the specified 'LENGTH' has a valid set of command-line option
        // specifications, and 'false' otherwise.  Optionally specify 'stream'
        // to which error messages are written.  If no 'stream' is specified,
        // this method produces no output.  See {Valid 'balcl::OptionInfo'
        // Specifications} for a description of the validity requirements.

    static bool isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                int               length);
    static bool isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                int               length,
                                                bsl::ostream&     stream);
        // Return 'true' if the specified 'specTable' of the specified 'length'
        // has a valid set of command-line option specifications, and 'false'
        // otherwise.  Optionally specify 'stream' to which error messages are
        // written.  If no 'stream' is specified, this method produces not
        // output.  See {Valid 'balcl::OptionInfo' Specifications} for a
        // description of the validity requirements.  The behavior is undefined
        // unless '0 <= length'.  Note that 'specTable' need not be statically
        // initialized.

    // CREATORS
    template <int LENGTH>
    CommandLine(const OptionInfo (&specTable)[LENGTH],
                bsl::ostream&      stream,
                bslma::Allocator  *basicAllocator = 0);
    template <int LENGTH>
    CommandLine(OptionInfo      (&specTable)[LENGTH],
                bsl::ostream&     stream,
                bslma::Allocator *basicAllocator = 0);
        // Create an object accepting the command-line options described by the
        // specified (statically-initialized) 'specTable'.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'specTable' satisfies the
        // 'isValidOptionSpecificationTable' function.  Note that an
        // appropriate error message is written to the specified 'stream'.

    template <int LENGTH>
    explicit
    CommandLine(const OptionInfo (&specTable)[LENGTH],
                bslma::Allocator  *basicAllocator = 0);
    template <int LENGTH>
    explicit
    CommandLine(OptionInfo       (&specTable)[LENGTH],
                bslma::Allocator  *basicAllocator = 0);
        // Create an object accepting the command-line options described by the
        // specified (statically-initialized) 'specTable'.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'specTable' satisfies the
        // 'isValidOptionSpecificationTable' function.  Note that an
        // appropriate error message is written to 'bsl::cerr'.

    CommandLine(const OptionInfo *specTable,
                int               length,
                bslma::Allocator *basicAllocator = 0);
        // Create an object accepting the command-line options described by the
        // specified 'specTable' of the specified 'length'.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'specTable' satisfies the
        // 'isValidOptionSpecificationTable' function.  Note that an
        // appropriate error message is written to 'bsl::cerr'.  Also note that
        // 'specTable' need not be statically initialized.

    CommandLine(const OptionInfo *specTable,
                int               length,
                bsl::ostream&     stream,
                bslma::Allocator *basicAllocator = 0);
        // Create an object accepting the command-line options described by the
        // specified 'specTable' of the specified 'length'.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'specTable' satisfies the
        // 'isValidOptionSpecificationTable' function.  Note that an
        // appropriate error message is written to the specified 'stream'.
        // Also note that 'specTable' need not be statically initialized.

    CommandLine(const CommandLine&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create a command-line object having the value of the specified
        // 'original' command line, if the 'original' is parsed, and otherwise
        // having a state such that parsing command-line arguments results in
        // the same value as parsing the same command-line arguments with the
        // 'original'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless 'original' is
        // valid (i.e., 'original.isValid()' returns 'true').

    ~CommandLine();
        // Destroy this command-line object.

    // MANIPULATORS
    CommandLine& operator=(const CommandLine& rhs);
        // Assign to this command-line object the value of the specified 'rhs'
        // command-line object and return a reference providing modifiable
        // access to this object.  The behavior is undefined unless both 'rhs'
        // and this object are valid (i.e., 'isValid()' and 'rhs.isValid()'
        // both return 'true').

    int parse(int argc, const char *const argv[]);
    int parse(int argc, const char *const argv[], bsl::ostream& stream);
        // Parse the command-line arguments contained in the array starting at
        // the specified 'argv' having the specified 'argc' length.  Optionally
        // specify a 'stream' to which an appropriate error message is written
        // if parsing fails.  If 'stream' is not specified, 'bsl::cerr' is
        // used.  Return 0 on success, and a non-zero value otherwise.  After a
        // successful call 'true == isParsed()', 'true == isValid()', and the
        // information provided by 'argv' can be viewed via the accessors.
        // After an unsuccessful call 'false == isParsed()' and
        // 'false == isValid()'.  The behavior is undefined unless
        // 'false == isParsed()' and 'true == isValid()'.  Note that the
        // behavior is undefined if 'parse' is invoked more than once on an
        // object (successful or not).

    // ACCESSORS
    bool isParsed() const;
        // Return 'true' if this object was parsed successfully, and 'false'
        // otherwise.  Note that if 'parse' was invoked but failed, this method
        // returns 'false'.

    bool isSpecified(const bsl::string& name) const;
    bool isSpecified(const bsl::string& name, int *count) const;
        // Return 'true' if the option with the specified 'name' has been
        // entered on the command line and, if the optionally specified 'count'
        // is not 0, load into 'count' the number of times the option 'name'
        // has been entered on the command line; otherwise, return 'false' and
        // leave 'count' unaffected.  Note that, in order to receive the valid
        // value, the command line must be successfully parsed.

    bool isValid() const;
        // Return 'true' if this object is in a valid state, and 'false'
        // otherwise.  Objects are in a valid state after construction from a
        // valid set of option specifications (see the function-level
        // documentation of the 'isValidOptionSpecificationTable' method) and
        // after a successful invocation of the 'parse' method.  Conversely,
        // construction from invalid option specifications or an unsuccessful
        // invocation of the 'parse' method leaves the object in an invalid
        // state.  Note that additional object state is available from the
        // 'isParsed' accessor method.

    int numSpecified(const bsl::string& name) const;
        // Return the number of times the option with the specified 'name' has
        // been entered on the command line, or 0 if 'name' is not the name of
        // a field in the command-line specification passed at construction to
        // this object.  Note that, in order to receive the valid number of
        // occurrences, the command line must be successfully parsed.

    CommandLineOptionsHandle options() const;
        // Return the command-line options and their values.  If an option was
        // not entered on the command line *and* a default value was provided
        // for that option, then that default value will be used (note the
        // difference with the 'specifiedOptions' method).  If an option was
        // not entered on the command line *and* no default value was provided
        // for that option, then the corresponding option will be in a null
        // state.  The behavior is undefined unless 'isParsed' returns 'true'.

    int position(const bsl::string& name) const;
        // Return the position where the option with the specified 'name' has
        // been entered on the command line.  If the option was not specified,
        // return -1.  The behavior is undefined unless the option is of scalar
        // type.

    const bsl::vector<int>& positions(const bsl::string& name) const;
        // Return the positions where the option with the specified 'name' has
        // been entered on the command line.  If the option was not specified,
        // return an empty vector.  Note that, in order to receive the valid
        // positions, the command line must be successfully parsed.

    void printUsage() const;
    void printUsage(bsl::ostream& stream) const;
        // Print usage to the specified output 'stream', describing what the
        // command line should look like.  If 'stream' is not specified, print
        // usage to 'stderr'.  This method can be invoked at any time, even
        // before 'parse' has been invoked on this object.

    CommandLineOptionsHandle specifiedOptions() const;
        // Return the command-line options and their values.  If an option was
        // not entered on the command line, then the option will be in a null
        // state (note the difference with the 'options' method).  This method
        // is especially useful for overwriting some other configuration
        // (potentially obtained from a configuration file).  The behavior is
        // undefined unless 'isParsed' returns 'true'.

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order
    bool theBool(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless 'isParsed' returns 'true', the object
        // has a 'name' option, and that option has type 'bool'.

    char theChar(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless 'isParsed' returns 'true', the object
        // has a 'name' option, and that option has type 'char'.

    int theInt(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless 'isParsed' returns 'true', the object
        // has a 'name' option, and that option has type 'int'.

    bsls::Types::Int64 theInt64(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless 'isParsed' returns 'true', the object
        // has a 'name' option, and that option has type 'bsls::Types::Int64'.

    double theDouble(const bsl::string& name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless 'isParsed' returns 'true', the object
        // has a 'name' option, and that option has type 'double'.

    const bsl::string& theString(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::string'.

    const bdlt::Datetime& theDatetime(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bdlt::Datetime'.

    const bdlt::Date& theDate(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bdlt::Date'.

    const bdlt::Time& theTime(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bdlt::Time'.

    const bsl::vector<char>& theCharArray(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<char>'.

    const bsl::vector<int>& theIntArray(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<int>'.

    const bsl::vector<bsls::Types::Int64>& theInt64Array(
                                                const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<bsls::Types::Int64>'.

    const bsl::vector<double>& theDoubleArray(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<double>'.

    const bsl::vector<bsl::string>& theStringArray(
                                                const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<bsl::string>'.

    const bsl::vector<bdlt::Datetime>& theDatetimeArray(
                                                const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<bdlt::Datetime>'.

    const bsl::vector<bdlt::Date>& theDateArray(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<bdlt::Date>'.

    const bsl::vector<bdlt::Time>& theTimeArray(const bsl::string& name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless 'isParsed'
        // returns 'true', the object has a 'name' option, and that option has
        // type 'bsl::vector<bdlt::Time>'.
// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator at construction is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this command-line object to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this object.  If 'level' is negative, suppress
        // indentation of the first line.  The behavior is undefined unless
        // '0 <= spacesPerLevel'.  If 'stream' is not valid on entry, this
        // operation has no effect.
};

// FREE OPERATORS
bool operator==(const CommandLine& lhs, const CommandLine& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two command-line arguments have the same value if
    // and only if they are both parsed successfully and have the same
    // command-line specifications and the same values for flags, options, and
    // non-option arguments.  Note that two identical copies built with the
    // same option table, but unparsed, are *not* equal.

bool operator!=(const CommandLine& lhs, const CommandLine& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' have the same value, and
    // 'true' otherwise.  Two command-line arguments do not have the same value
    // if and only if they have different command-line specifications, or one
    // is parsed successfully but the other is not, or neither is, or else both
    // have the same specification and both are parsed successfully but they
    // have different values for at least one flag, option or non-option
    // argument.  Note that two identical copies built with the same option
    // table, but unparsed, are *not* equal.

bsl::ostream& operator<<(bsl::ostream& stream, const CommandLine& rhs);
    // Write the options and their values in the specified 'rhs' to the
    // specified output 'stream' in a (multi-line) human readable format and
    // return a reference to 'stream'.  Note that the last line is *not*
    // terminated by a newline character.

                        // ==============================
                        // class CommandLineOptionsHandle
                        // ==============================

class CommandLineOptionsHandle {
    // This class provides access to the parsed options (if any) of its
    // creating 'CommandLine' object.  The behavior is undefined if any of the
    // methods of this class (accessors all) are invoked after a subsequent
    // invocation of the 'parse' method of the creating object or after the
    // destruction of the creating object.

    // PRIVATE TYPES
    typedef bsl::vector<CommandLine_SchemaData> CommandLine_Schema;
    typedef bsl::vector<OptionValue>            OptionValueList;

    // DATA
    const OptionValueList    *d_data_p;
    const CommandLine_Schema *d_schema_p;

    // FRIENDS
    friend class CommandLine;
    friend bool operator==(const CommandLineOptionsHandle&,
                           const CommandLineOptionsHandle&);
    friend bool operator!=(const CommandLineOptionsHandle&,
                           const CommandLineOptionsHandle&);

    // PRIVATE CREATORS
    CommandLineOptionsHandle(const OptionValueList    *dataPtr,
                             const CommandLine_Schema *schemaPtr);
        // Create a 'CommandLineOptionsHandle' object referring to the
        // specified 'dataPtr' and 'schemaPtr'.  The behavior is undefined
        // unless 'dataPtr->size() == schemaPtr->size()'.

  public:
    // ACCESSORS
    const char *name(bsl::size_t index) const;
        // Return the name of the option at the specified 'index'.  The
        // behavior is undefined unless '0 <= index < numOptions()' and this
        // handle was obtained from a 'CommandLine' object where
        // 'true == isParsed()'.

    bsl::size_t numOptions() const;
        // Return the number of parsed options.

    OptionType::Enum type(bsl::size_t index) const;
        // Return the type of the option at the specified 'index'.  The
        // behavior is undefined unless '0 <= index < numOptions()' and this
        // handle was obtained from a 'CommandLine' object where
        // 'true == isParsed()'.

    const OptionValue& value(bsl::size_t index) const;
        // Return a 'const' reference to the value (possibly in a null state)
        // of the option at the specified 'index'.  The behavior is undefined
        // unless '0 <= index < numOptions()' and this handle was obtained from
        // a 'CommandLine' object where 'true == isParsed()'.

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order
    template <class TYPE>
    const TYPE& the(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  Template parameter 'TYPE' must be one of the
        // supported types (see {Supported Types}).  The behavior is undefined
        // unless this handle was obtained from a 'CommandLine' object that
        // 'isParsed()' and has a 'name' option of type 'TYPE'.

    bool theBool(const char *name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless this handle was obtained from a
        // 'CommandLine' object that 'isParsed()' and has a 'name' option of
        // type 'bool'.

    char theChar(const char *name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless this handle was obtained from a
        // 'CommandLine' object that 'isParsed()' and has a 'name' option of
        // type 'char'.

    int theInt(const char *name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless this handle was obtained from a
        // 'CommandLine' object that 'isParsed()' and has a 'name' option of
        // type 'int'.

    bsls::Types::Int64 theInt64(const char *name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless this handle was obtained from a
        // 'CommandLine' object that 'isParsed()' and has a 'name' option of
        // type 'bsls::Types::Int64'.

    double theDouble(const char *name) const;
        // Return the value of the option having the specified 'name'.  The
        // behavior is undefined unless this handle was obtained from a
        // 'CommandLine' object that 'isParsed()' and has a 'name' option of
        // type 'double'.

    const bsl::string& theString(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::string'.

    const bdlt::Datetime& theDatetime(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless the option this
        // handle was obtained from a 'CommandLine' object that 'isParsed()'
        // and has a 'name' option of type 'bdlt::Datetime'.

    const bdlt::Date& theDate(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bdlt::Date'.

    const bdlt::Time& theTime(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bdlt::Time'.

    const bsl::vector<char>& theCharArray(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<char>'.

    const bsl::vector<int>& theIntArray(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<int>'.

    const bsl::vector<bsls::Types::Int64>& theInt64Array(const char *name)
                                                                         const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<bsls::Types::Int64>'.

    const bsl::vector<double>& theDoubleArray(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<double>'.

    const bsl::vector<bsl::string>& theStringArray(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<bsl::string>'.

    const bsl::vector<bdlt::Datetime>& theDatetimeArray(const char *name)
                                                                         const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<bdlt::Datetime>'.

    const bsl::vector<bdlt::Date>& theDateArray(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<bdlt::Date>'.

    const bsl::vector<bdlt::Time>& theTimeArray(const char *name) const;
        // Return a 'const' reference to the value of the option having the
        // specified 'name'.  The behavior is undefined unless this handle was
        // obtained from a 'CommandLine' object that 'isParsed()' and has a
        // 'name' option of type 'bsl::vector<bdlt::Time>'.

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order
};

// FREE OPERATORS
bool operator==(const CommandLineOptionsHandle& lhs,
                const CommandLineOptionsHandle& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'CommandLineOptionsHandle' objects have the same
    // value if they have the same 'numOptions' and each of those options have
    // the same 'name', 'type', and 'value' or are in the null state.

bool operator!=(const CommandLineOptionsHandle& lhs,
                const CommandLineOptionsHandle& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'CommandLineOptionsHandle' objects do
    // not have the same value if they do not have the same 'numOptions' or if
    // any of their options differ in 'name', 'type', or 'value (or null
    // state).

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // struct CommandLine_SchemaData
                        // -----------------------------

}  // close package namespace

// FREE OPERATORS
inline
bool balcl::operator==(const CommandLine_SchemaData& lhs,
                       const CommandLine_SchemaData& rhs)
{
    return lhs.d_type == rhs.d_type
       &&  0 == bsl::strcmp(lhs.d_name_p, rhs.d_name_p);
}

inline
bool balcl::operator!=(const CommandLine_SchemaData& lhs,
                       const CommandLine_SchemaData& rhs)
{
    return lhs.d_type != rhs.d_type
       ||  0 != bsl::strcmp(lhs.d_name_p, rhs.d_name_p);
}

namespace balcl {

                        // -----------------
                        // class CommandLine
                        // -----------------

// CLASS METHODS
template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                         const OptionInfo (&specTable)[LENGTH])
{
    return isValidOptionSpecificationTable(specTable, LENGTH);
}

template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                               OptionInfo (&specTable)[LENGTH])
{
    return isValidOptionSpecificationTable(specTable, LENGTH);
}

template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                         const OptionInfo (&specTable)[LENGTH],
                                         bsl::ostream&      stream)
{
    return isValidOptionSpecificationTable(specTable, LENGTH, stream);
}

template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                            OptionInfo    (&specTable)[LENGTH],
                                            bsl::ostream&   stream)
{
    return isValidOptionSpecificationTable(specTable, LENGTH, stream);
}

inline
bool CommandLine::isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                  int               length)
{
    BSLS_ASSERT(specTable);
    BSLS_ASSERT(0 <= length);

    bsl::ostringstream oss;
    return isValidOptionSpecificationTable(specTable, length, oss);
}

// CREATORS
template <int LENGTH>
CommandLine::CommandLine(const OptionInfo (&specTable)[LENGTH],
                         bsl::ostream&      stream,
                         bslma::Allocator  *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(Option(specTable[i]));
    }
    validateAndInitialize(stream);
    d_state = e_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(OptionInfo      (&specTable)[LENGTH],
                         bsl::ostream&     stream,
                         bslma::Allocator *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(Option(specTable[i]));
    }
    validateAndInitialize(stream);
    d_state = e_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(const OptionInfo (&specTable)[LENGTH],
                         bslma::Allocator  *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(Option(specTable[i]));
    }
    validateAndInitialize();
    d_state = e_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(OptionInfo      (&specTable)[LENGTH],
                         bslma::Allocator *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_data1(basicAllocator)
, d_data2(basicAllocator)
, d_isBindin2Valid(false)
{
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(Option(specTable[i]));
    }
    validateAndInitialize();
    d_state = e_NOT_PARSED;
}

                        // ------------------------------
                        // class CommandLineOptionsHandle
                        // ------------------------------

// PRIVATE CREATORS
inline
CommandLineOptionsHandle::CommandLineOptionsHandle(
                                           const OptionValueList    *dataPtr,
                                           const CommandLine_Schema *schemaPtr)
: d_data_p(dataPtr)
, d_schema_p(schemaPtr)
{
    BSLS_ASSERT(dataPtr);
    BSLS_ASSERT(schemaPtr);
    BSLS_ASSERT(dataPtr->size() == schemaPtr->size());
}

// ACCESSORS
inline
const char *CommandLineOptionsHandle::name(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_schema_p->size());

    return (*d_schema_p)[index].d_name_p;
}

inline
bsl::size_t CommandLineOptionsHandle::numOptions() const
{
    return d_schema_p->size();
}

inline
OptionType::Enum CommandLineOptionsHandle::type(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_schema_p->size());

    return (*d_schema_p)[index].d_type;
}

inline
const OptionValue& CommandLineOptionsHandle::value(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_schema_p->size());

    return (*d_data_p)[index];
}

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order
template <class TYPE>
const TYPE& CommandLineOptionsHandle::the(const char *name) const
{
    BSLS_ASSERT(name);

    bsl::size_t index = -1;
    for (CommandLine_Schema::const_iterator itr  = d_schema_p->cbegin(),
                                            end  = d_schema_p->cend();
                                            end != itr; ++itr) {
        if (0 == bsl::strcmp(itr->d_name_p, name)) {
            BSLS_ASSERT(itr->d_type == OptionType::TypeToEnum<TYPE>::value);
            index = bsl::distance(d_schema_p->begin(), itr);
            break;
        }
    }

    BSLS_ASSERT(0     <= index);
    BSLS_ASSERT(index <  d_data_p->size());

    return  (*d_data_p)[index].the<TYPE>();
}

inline
bool CommandLineOptionsHandle::theBool(const char *name) const
{
    return the<OptionType::Bool>(name);
}

inline
char CommandLineOptionsHandle::theChar(const char *name) const
{
    return the<OptionType::Char>(name);
}

inline
int CommandLineOptionsHandle::theInt(const char *name) const
{
    return the<OptionType::Int>(name);
}

inline
bsls::Types::Int64 CommandLineOptionsHandle::theInt64(const char *name) const
{
    return the<OptionType::Int64>(name);
}

inline
double CommandLineOptionsHandle::theDouble(const char *name) const
{
    return the<OptionType::Double>(name);
}

inline
const bsl::string&
CommandLineOptionsHandle::theString(const char *name) const
{
    return the<OptionType::String>(name);
}

inline
const bdlt::Datetime&
CommandLineOptionsHandle::theDatetime(const char *name) const
{
    return the<OptionType::Datetime>(name);
}

inline
const bdlt::Date&
CommandLineOptionsHandle::theDate(const char *name) const
{
    return the<OptionType::Date>(name);
}

inline
const bdlt::Time& CommandLineOptionsHandle::theTime(const char *name) const
{
    return the<OptionType::Time>(name);
}

inline
const bsl::vector<char>&
CommandLineOptionsHandle::theCharArray(const char *name) const
{
    return the<OptionType::CharArray>(name);
}

inline
const bsl::vector<int>&
CommandLineOptionsHandle::theIntArray(const char *name) const
{
    return the<OptionType::IntArray>(name);
}

inline
const bsl::vector<bsls::Types::Int64>&
CommandLineOptionsHandle::theInt64Array(const char *name) const
{
    return the<OptionType::Int64Array>(name);
}

inline
const bsl::vector<double>&
CommandLineOptionsHandle::theDoubleArray(const char *name) const
{
    return the<OptionType::DoubleArray>(name);
}

inline
const bsl::vector<bsl::string>&
CommandLineOptionsHandle::theStringArray(const char *name) const
{
    return the<OptionType::StringArray>(name);
}

inline
const bsl::vector<bdlt::Datetime>&
CommandLineOptionsHandle::theDatetimeArray(const char *name) const
{
    return the<OptionType::DatetimeArray>(name);
}

inline
const bsl::vector<bdlt::Date>&
CommandLineOptionsHandle::theDateArray(const char *name) const
{
    return the<OptionType::DateArray>(name);
}

inline
const bsl::vector<bdlt::Time>&
CommandLineOptionsHandle::theTimeArray(const char *name) const
{
    return the<OptionType::TimeArray>(name);
}

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

}  // close package namespace

// FREE OPERATORS
inline
bool balcl::operator==(const balcl::CommandLineOptionsHandle& lhs,
                       const balcl::CommandLineOptionsHandle& rhs)
{
    BSLS_ASSERT(lhs.d_data_p); BSLS_ASSERT(lhs.d_schema_p);
    BSLS_ASSERT(rhs.d_data_p); BSLS_ASSERT(rhs.d_schema_p);

    return *lhs.d_data_p   == *rhs.d_data_p
        && *lhs.d_schema_p == *rhs.d_schema_p;
}

inline
bool balcl::operator!=(const balcl::CommandLineOptionsHandle& lhs,
                       const balcl::CommandLineOptionsHandle& rhs)
{
    BSLS_ASSERT(lhs.d_data_p); BSLS_ASSERT(lhs.d_schema_p);
    BSLS_ASSERT(rhs.d_data_p); BSLS_ASSERT(rhs.d_schema_p);

    return *lhs.d_data_p   != *rhs.d_data_p
        || *lhs.d_schema_p != *rhs.d_schema_p;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
